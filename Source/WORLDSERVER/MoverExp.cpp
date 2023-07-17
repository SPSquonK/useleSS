#include "StdAfx.h"
#include <numeric>
#include "User.h"
#include "party.h"
#include "GroupUtils.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"

#include "boost/container/small_vector.hpp"

#undef min
#undef max

static constexpr float EXP_RANGE = 64.f;

namespace {
	namespace Rewarder {
		// Somebody that did damage to a monster
		struct DamageDealer {
			OBJID m_objid;
			CUser * m_user;
			unsigned int m_contribution;

			explicit DamageDealer(CUser * const user)
				: m_objid(user->GetId()), m_user(user), m_contribution(0) {
			}

			DamageDealer(const OBJID objid, CUser * const user, const unsigned int contribution)
				: m_objid(objid), m_user(user), m_contribution(contribution) {
			}

			static CUser * GetUser(const DamageDealer & parter) {
				return parter.m_user;
			}
		};

		// A party that did damage to a monster
		struct ParterParty {
			unsigned long m_id;
			CParty * m_party;
			boost::container::small_vector<DamageDealer, 8> m_players;

			ParterParty(CParty & party, const CMover & pDead)
				: m_id(party.m_uPartyId), m_party(&party) {

				for (CUser * const member : AllMembers(party)) {
					if (pDead.IsValidArea(member, EXP_RANGE)) {
						m_players.emplace_back(member);
					}
				}
			}

			void AddPlayer(const DamageDealer & toAdd) {
				auto memberIt = std::find_if(
					m_players.begin(),
					m_players.end(),
					[&](const DamageDealer & existing) { return existing.m_user == toAdd.m_user; }
				);

				if (memberIt == m_players.end()) {
					m_players.emplace_back(toAdd);
				} else {
					memberIt->m_contribution += toAdd.m_contribution;
				}
			}

			[[nodiscard]] unsigned int GetTotalContribution() const {
				const auto xs = m_players | std::views::transform([](const auto & dealer) { return dealer.m_contribution; });
				return std::accumulate(xs.begin(), xs.end(), 0u);
			}

			void ExpReward(float baseExp, CMover & pDead, MoverProp * pMoverProp, float fFxpValue) const;

			[[nodiscard]] auto GetPlayers() const {
				return m_players | std::views::transform(DamageDealer::GetUser);
			}

		private:
			void AddExperienceParty(EXPFLOAT fExpValue, CMover & pDead, const MoverProp & pMoverProp) const;
			void AddExperiencePartyContribution(EXPFLOAT fExpValue, int nMaxLevel10) const;
			void AddExperiencePartyLevel(EXPFLOAT fExpValue, int nMaxLevel10) const;

			[[nodiscard]] float ComputeSumOfSquaredLevel() const;
		};

		struct Accumulator {
			boost::container::small_vector<DamageDealer, 16> m_players;
			unsigned int m_totalDamage = 0;

			Accumulator(const CMover & pDead, const MoverEnnemies & enemies) {
				for (const auto & [objid, nHit] : enemies) {
					CMover * const pEnemy = prj.GetMover(objid);
					m_totalDamage += nHit;

					if (!IsValidObj(pEnemy)) continue;
					if (!pEnemy->IsPlayer()) continue;
					if (!pDead.IsValidArea(pEnemy, EXP_RANGE)) continue;

					m_players.emplace_back(objid, static_cast<CUser *>(pEnemy), nHit);
				}
			}

			[[nodiscard]] bool IsEmpty() const noexcept {
				return m_totalDamage == 0 || m_players.size() == 0;
			}

			[[nodiscard]] auto IsolateParties(const CMover & pDead) {
				boost::container::small_vector<ParterParty, 4> result;

				for (auto it = m_players.begin(); it != m_players.end();) {
					const auto & player = *it;

					CParty * pParty = g_PartyMng.GetParty(player.m_user->m_idparty);
					if (!pParty) {
						++it;
					} else if (!pParty->IsMember(player.m_user->m_idPlayer)) {
						++it;
					} else {
						auto partyIt = std::find_if(result.begin(), result.end(),
							[&](const ParterParty & pparty) { return pparty.m_party == pParty; }
						);

						if (partyIt == result.end()) {
							result.emplace_back(*pParty, pDead);
							partyIt = result.end() - 1;
						}

						partyIt->AddPlayer(*it);

						it = m_players.erase(it);
					}
				}

				return result;
			}
		};
	}
}

///////////////////////////////////////////////////////////////////////////////
// Entry point

static float GetBaseExp(const EXPFLOAT fExpValue, unsigned int contribution, DWORD maxHP) {
	return fExpValue * static_cast<float>(contribution) / static_cast<float>(maxHP);
}

void CMover::AddExperienceKillMember(CMover * pDead, EXPFLOAT fExpValue, MoverProp * pMoverProp, float fFxpValue) {
	Rewarder::Accumulator accumulator = Rewarder::Accumulator(*pDead, pDead->m_idEnemies);
	if (accumulator.IsEmpty()) return;

	if (accumulator.m_players.size() > 1024) {
		Error("CMover::AddExperienceKillMember - enemy size is too big (%lu / 1024)", accumulator.m_players.size());
	}

	const auto parties = accumulator.IsolateParties(*pDead);

	for (const auto & party : parties) {
		const float baseExp = GetBaseExp(fExpValue, party.GetTotalContribution(), pDead->GetMaxHitPoint());
		party.ExpReward(baseExp, *pDead, pMoverProp, fFxpValue);
	}

	for (const auto & member : accumulator.m_players) {
		const float fExpValuePerson = GetBaseExp(fExpValue, member.m_contribution, pDead->GetMaxHitPoint())
			* CPCBang::GetInstance()->GetExpFactor(member.m_user);

		member.m_user->AddExperienceSolo(fExpValuePerson, pMoverProp, fFxpValue, false);
	}
}


///////////////////////////////////////////////////////////////////////////////
// Solo experience

static constexpr float GetSoloExpDebuff(const int myLevel, const DWORD enemyLevel) {
	if (myLevel <= enemyLevel) return 1.f;

	const auto delta = myLevel - enemyLevel;
	if (delta == 1 || delta == 2) return 0.7f;
	if (delta == 3 || delta == 4) return 0.4f;
	return 0.1f;
}

void CUser::AddExperienceSolo(EXPFLOAT fExpValue, MoverProp * pMoverProp, float fFxpValue, bool bParty) {
	// 1.5x EXP when playing solo during Valentine's Event
	if (g_eLocal.GetState(EVE_EVENT0214) && !bParty) {
		fExpValue *= 1.5;
	}

	const auto debuff = GetSoloExpDebuff(GetLevel(), pMoverProp->dwLevel);
	if (debuff != 1.f) {
		fExpValue *= debuff;
		fFxpValue *= debuff;
	}

	AddPartyMemberExperience(fExpValue, fFxpValue);
}


///////////////////////////////////////////////////////////////////////////////
// Party experience

// Returns the experience reduction value: Based on the difference between the
// target and the highest level party member among the surrounding party
// members...
static float GetExperienceReduceFactor(const int nLevel, const int nMaxLevel) {
	static constexpr std::array<float, 9> korean = { 0.7f, 0.7f, 0.4f, 0.4f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f };
	static constexpr std::array<float, 9> other = { 0.8f, 0.6f, 0.35f, 0.2f, 0.12f, 0.08f, 0.04f, 0.02f, 0.01f };

	const int nDelta = std::min(nMaxLevel - nLevel, 9);
	if (nDelta <= 0) return 1.0f;

	if (::GetLanguage() == LANG_KOR) {
		return korean[nDelta - 1];
	} else {
		return other[nDelta - 1];
	}
}

// Returns the actual kind of exp distribution of the party. This function
// should be useless as m_nTroupsShareExp should be sanitized now
static CParty::ShareExpMode GetPartyDistributionType(const CParty & party) {
	if (!party.m_nKindTroup) { // Not an advanced party
		return CParty::ShareExpMode::Level;
	} else if (CParty::IsValidMode(party.m_nTroupsShareExp)) {
		return party.m_nTroupsShareExp;
	} else {
		return CParty::ShareExpMode::Level;
	}
}

void Rewarder::ParterParty::ExpReward(float baseExp, CMover & pDead, MoverProp * pMoverProp, float fFxpValue) const {
	const float fExpValuePerson = baseExp * CPCBang::GetInstance()->GetPartyExpFactor(GetPlayers());

	if (m_players.size() <= 1) {
		for (const auto & member : m_players) {
			member.m_user->AddExperienceSolo(fExpValuePerson, pMoverProp, fFxpValue, true);
		}
		return;
	}

	AddExperienceParty(fExpValuePerson, pDead, *pMoverProp);
}

void Rewarder::ParterParty::AddExperienceParty(EXPFLOAT fExpValue, CMover & pDead, const MoverProp & pMoverProp) const {
	int maxLevel = 0;
	int totalLevel = 0;
	for (const CUser * const user : GetPlayers()) {
		totalLevel += user->GetLevel();
		maxLevel = std::max(maxLevel, user->GetLevel());
	}

	// Decreased experience to be the highest level party member among nearby party members
	const float fFactor = GetExperienceReduceFactor((int)pMoverProp.dwLevel, maxLevel);
	fExpValue *= static_cast<EXPFLOAT>(fFactor);

	// If the monster level and the average level delta is 5 or higher, the
	// party experience and points do not increase.
	m_party->GetPoint(totalLevel, m_players.size(), pDead.GetLevel());

	const CParty::ShareExpMode type = GetPartyDistributionType(*m_party);

	// Compute min level exclusive to earn exp
	const int minRequiredLevelExclusive = maxLevel >= 20 ? maxLevel - 20 : 0;

	switch (type) {
		case CParty::ShareExpMode::Level:
		default:
			AddExperiencePartyLevel(fExpValue, minRequiredLevelExclusive);
			break;
		case CParty::ShareExpMode::Contribution:
			AddExperiencePartyContribution(fExpValue, minRequiredLevelExclusive);
			break;
	}
}

// Reward by contribution to damage
void Rewarder::ParterParty::AddExperiencePartyContribution(EXPFLOAT fExpValue, int nMaxLevel10) const {
	const float fMaxMemberLevel = ComputeSumOfSquaredLevel();

	int nAttackMember = 0;
	for (const auto & member : m_players) {
		if (member.m_contribution > 0) {
			++nAttackMember;
		}
	}

	float fAddExp = (float)((fExpValue * 0.2f) * (m_party->m_nSizeofMember - 1));

	float fFullParty = 0.0f;
	if (m_players.size() == MAX_PTMEMBER_SIZE_SPECIAL) {
		fFullParty = (float)((fExpValue * 0.1f));
	}
	float fOptionExp = 0.0f;
	if (nAttackMember > 1) {
		fOptionExp = (float)((fExpValue * (float)nAttackMember / 100.0f));
	}

	for (const auto & member : m_players) {
		if (member.m_user->GetLevel() <= nMaxLevel10) continue;

		const float fContribution = static_cast<float>(member.m_contribution) / static_cast<float>(GetTotalContribution());

		const EXPINTEGER damageExp = static_cast<EXPINTEGER>(fExpValue * fContribution);

		const float level = static_cast<float>(member.m_user->GetLevel());
		const float squaredLevel = level * level;
		const EXPINTEGER watchExp = (fAddExp * (squaredLevel / fMaxMemberLevel)) + fOptionExp + fFullParty;

		const EXPINTEGER nMemberExp = damageExp + watchExp;

		member.m_user->AddPartyMemberExperience(nMemberExp, 0);
	}
}

void Rewarder::ParterParty::AddExperiencePartyLevel(EXPFLOAT fExpValue, int nMaxLevel10) const {
	const float fMaxMemberLevel = ComputeSumOfSquaredLevel();

	const float fAddExp = (float)((fExpValue * 0.2f) * (m_players.size() - 1));

	for (CUser * const user : GetPlayers()) {
		if (user->GetLevel() <= nMaxLevel10) continue;

		const float level = static_cast<float>(user->GetLevel());
		const float squaredLevel = level * level;
		const EXPINTEGER nMemberExp = static_cast<EXPINTEGER>((fExpValue + fAddExp) * (squaredLevel / fMaxMemberLevel));
		user->AddPartyMemberExperience(nMemberExp, 0);
	}
}

float Rewarder::ParterParty::ComputeSumOfSquaredLevel() const {
	float fMaxMemberLevel = 0.0f;
	for (const CUser * const user : GetPlayers()) {
		const float level = static_cast<float>(user->GetLevel());
		fMaxMemberLevel += level * level;
	}
	return fMaxMemberLevel;
}

void CUser::AddPartyMemberExperience(EXPINTEGER nExp, int nFxp) {
	const auto expLimit = prj.m_aExpCharacter[m_nLevel].nLimitExp;
	if (nExp > expLimit) nExp = expLimit;

	if (nFxp) {
		if (AddFxp(nFxp)) { // Flight level up
			g_UserMng.AddSetFlightLevel(this, GetFlightLv());
		}

		AddSetFxp(m_nFxp, GetFlightLv());
	}

	EarnExperience(nExp, true, true);
}


void CUser::EarnExperience(EXPINTEGER nExp, bool applyMultipliers, bool reducePropency) {
	if (AddExperience(nExp, applyMultipliers, reducePropency)) {
		g_UserMng.AddSetLevel(this, (short)GetLevel());
		AddSetGrowthLearningPoint(m_nRemainGP);
		g_dpDBClient.SendLogLevelUp(this, 1);
		g_dpDBClient.SendUpdatePlayerData(this);
	} else {
		// 레벨 5이상 로그_레벨업 테이블에 로그를 남긴다
		// 20% 단위로 로그를 남김
		if (GetLevel() > 5) // 레벨 5이상
		{
			int nNextExpLog = (int)(m_nExpLog / 20 + 1) * 20;
			int nExpPercent = (int)(GetExp1() * 100 / GetMaxExp1());
			if (nExpPercent >= nNextExpLog) {
				m_nExpLog = nExpPercent;
				g_dpDBClient.SendLogLevelUp(this, 5);
			}
		}
	}
	
	AddSetExperience(GetExp1(), (WORD)m_nLevel, m_nSkillPoint, m_nSkillLevel);
}

bool CUser::AddExperience(EXPINTEGER nExp, bool applyMultipliers, bool reducePropency) {
#ifdef __VTN_TIMELIMIT
	if (::GetLanguage() == LANG_VTN) {
		if (IsPlayer() && m_nAccountPlayTime != -1) {
			if (m_nAccountPlayTime < 0 || m_nAccountPlayTime > MIN(300)) {
				nExp = 0;
			} else if (m_nAccountPlayTime >= MIN(180) && m_nAccountPlayTime <= MIN(300)) {
				nExp = (EXPINTEGER)(nExp * 0.5f);
			}
		}
	}
#endif // __VTN_TIMELIMIT

	if (IsAuthHigher(AUTH_ADMINISTRATOR) && IsMode(MODE_EXPUP_STOP)) {
		return false;
	}

	if (nExp <= 0) return false;

	if (m_nHitPoint <= 0) return false;

	if (IsJobTypeOrBetter(JTYPE_MASTER)) nExp /= 2;

	if (applyMultipliers) {
		EXPINTEGER nAddExp = static_cast<CUser *>(this)->GetAddExpAfterApplyRestPoint(nExp);
		nExp = (EXPINTEGER)(nExp * GetExpFactor());
		nExp += nAddExp;
	}

	if (applyMultipliers && HasBuffByIk3(IK3_ANGEL_BUFF)) {
		int nAngel = 100;

		IBuff * pBuff = m_buffs.GetBuffByIk3(IK3_ANGEL_BUFF);
		WORD wId = (pBuff ? pBuff->GetId() : 0);

		if (wId > 0) {
			ItemProp * pItemProp = prj.GetItemProp(wId);
			if (pItemProp)
				nAngel = (int)((float)pItemProp->nAdjParamVal[0]);
		}
		if (nAngel <= 0 || 100 < nAngel)
			nAngel = 100;

		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[m_nAngelLevel].nExp1 / 100 * nAngel;
		if (m_nAngelExp < nMaxAngelExp) {
			nExp /= 2;
			m_nAngelExp += nExp;

#ifdef __ANGEL_LOG
#ifdef __EXP_ANGELEXP_LOG
			int nAngelExpPercent = (int)(m_nAngelExp * 100 / nMaxAngelExp);
			int nNextAngelExpLog = (int)(((CUser *)this)->m_nAngelExpLog / 20 + 1) * 20;

			if (nAngelExpPercent >= nNextAngelExpLog) {
				((CUser *)this)->m_nAngelExpLog = nAngelExpPercent;
				ItemProp * pItemProp = prj.GetItemProp(wId);

				if (pItemProp) {
					LogItemInfo aLogItem;
					aLogItem.Action = "&";
					aLogItem.SendName = ((CUser *)this)->GetName();
					aLogItem.RecvName = "ANGEL_EXP_LOG";
					aLogItem.WorldId = ((CUser *)this)->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = ((CUser *)this)->GetGold();
					//aLogItem.ItemName	= pItemProp->szName;
					_stprintf(aLogItem.szItemName, "%d", pItemProp->dwID);
					aLogItem.Gold_1 = (DWORD)(m_nAngelExp);
					g_DPSrvr.OnLogItem(aLogItem);
				}
			}
#endif //  __EXP_ANGELEXP_LOG


			BOOL bAngelComplete = FALSE;
			if (m_nAngelExp > nMaxAngelExp) {
				m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}

			AddAngelInfo(bAngelComplete);
#endif // __WORLDSERVER
		}
	}

	if (reducePropency && IsChaotic()) {
		m_dwPKExp = (DWORD)(m_dwPKExp + nExp);
		DWORD dwPropensity = GetPKPropensity();
		int nLevelPKExp = prj.GetLevelExp(GetLevel());
		if (nLevelPKExp != 0) {
			int nSubExp = m_dwPKExp / nLevelPKExp;
			if (nSubExp) {
				SetPKPropensity(GetPKPropensity() - (m_dwPKExp / nLevelPKExp));
				if (dwPropensity <= GetPKPropensity())
					SetPKPropensity(0);
				m_dwPKExp %= nLevelPKExp;

				g_UserMng.AddPKPropensity(this);
				g_dpDBClient.SendLogPkPvp(this, NULL, 0, 'P');
			}
		}
	}

	// TODO: test and reread this function because it is very messy right now

	// nExp += prj.m_aExpCharacter[m_nLevel + 1].nExp1 * (50 + xRandom(75)) / 100;

	return AddRawExperience(nExp);
}

bool CUser::AddRawExperience(EXPINTEGER nExp) {
	// Can earn exp?
	constexpr auto GetMaxLevel = [](const DWORD jobType) -> std::pair<bool, int> {
		switch (jobType) {
			case JTYPE_BASE:
				return { false, MAX_JOB_LEVEL };
			case JTYPE_EXPERT:
				return { false, MAX_JOB_LEVEL + MAX_EXP_LEVEL };
			default:
			case JTYPE_PRO:
			case JTYPE_MASTER:     
				return { true, MAX_LEVEL };
			case JTYPE_HERO:
				return { false, MAX_LEGEND_LEVEL };
			case JTYPE_LEGEND_HERO:
				return { true, MAX_3RD_LEGEND_LEVEL };
		}
	};

	const auto [lastLevelIsExpable, maxLevel] = GetMaxLevel(GetJobType());

	if (m_nLevel > maxLevel) return false;
	if (m_nLevel == maxLevel && !lastLevelIsExpable) return false;

	// Add exp
	m_nExp1 += nExp;

	// Can level up?
	const int nNextLevel = m_nLevel + 1;
	if (m_nExp1 < prj.m_aExpCharacter[nNextLevel].nExp1) return false;

	if (m_nLevel == maxLevel) {
		m_nExp1 = prj.m_aExpCharacter[nNextLevel].nExp1 - 1;
		return false;
	}

	// Apply level up
	nExp = m_nExp1 - prj.m_aExpCharacter[nNextLevel].nExp1;
	m_nExp1 = 0;
	m_nLevel = nNextLevel;

	// Lewel reward
	if (m_nDeathLevel < m_nLevel) {
		m_nRemainGP += prj.m_aExpCharacter[nNextLevel].dwLPPoint;

		if (IsJobTypeOrBetter(JTYPE_MASTER)) {
			m_nRemainGP++;
			SetMasterSkillPointUp();
		} else {
			const int nGetPoint = ((m_nLevel - 1) / 20) + 2;
			AddSkillPoint(nGetPoint);
			g_dpDBClient.SendLogSkillPoint(LOG_SKILLPOINT_GET_HUNT, nGetPoint, this, NULL);
		}

		GiveRecommendEveItems();

		m_nExpLog = 0;

		prj.m_EventLua.SetLevelUpGift(this, m_nLevel);

		CCampusHelper::GetInstance()->SetLevelUpReward(this);
	}

	// Heal
	m_nHitPoint = GetMaxHitPoint();
	m_nManaPoint = GetMaxManaPoint();
	m_nFatiguePoint = GetMaxFatiguePoint();

	// Things
	if (m_nLevel == 20) SetFlightLv(1);

	if (nExp != 0) AddRawExperience(nExp);
	return true;
}
