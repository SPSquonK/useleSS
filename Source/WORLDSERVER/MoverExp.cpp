#include "StdAfx.h"
#include <numeric>
#include "User.h"
#include "party.h"
#include "GroupUtils.h"

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
			std::vector<DamageDealer> m_players;

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
			std::vector<DamageDealer> m_players;
			unsigned int m_totalDamage = 0;

			Accumulator(const CMover & pDead, const SET_OBJID & enemies) {
				for (const auto & [objid, nHitInfo] : enemies) {
					CMover * const pEnemy = prj.GetMover(objid);
					m_totalDamage += nHitInfo.nHit;

					if (!IsValidObj(pEnemy)) continue;
					if (!pEnemy->IsPlayer()) continue;
					if (!pDead.IsValidArea(pEnemy, EXP_RANGE)) continue;

					m_players.emplace_back(objid, static_cast<CUser *>(pEnemy), nHitInfo.nHit);
				}
			}

			[[nodiscard]] bool IsEmpty() const noexcept {
				return m_totalDamage == 0 || m_players.size() == 0;
			}

			std::vector<ParterParty> IsolateParties(const CMover & pDead) {
				std::vector<ParterParty> result;

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
			* CPCBang::GetInstance()->GetExpFactor(static_cast<CUser *>(this));

		member.m_user->AddExperienceSolo(fExpValuePerson, pMoverProp, fFxpValue, false);
	}
}


///////////////////////////////////////////////////////////////////////////////
// Solo experience

static constexpr float GetSoloExpDebuff(const int myLevel, const DWORD enemyLevel) {
	if (myLevel >= enemyLevel) return 1.f;

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

enum class PartyDistributionType { Unknown, Level, Contribution };

// Returns the kind of exp distribution of the party. Even though FlyFF
// developers only develloped two, the original code base reflected that more
// were planned.
static PartyDistributionType GetPartyDistributionType(const CParty & party) {
	if (!party.m_nKindTroup) { // Not an advanced party
		return PartyDistributionType::Level;
	} else {
		switch (party.m_nTroupsShareExp) {
			case 0: // Level
				return PartyDistributionType::Level;
			case 1:	// Contribution
				return PartyDistributionType::Contribution;
			case 2:	// Equal distribution
				// Not implemented
				break;
		}
	}

	return PartyDistributionType::Unknown;
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
		if (maxLevel < user->GetLevel()) maxLevel = user->GetLevel();
	}

	// Decreased experience to be the highest level party member among nearby party members
	const float fFactor = GetExperienceReduceFactor((int)pMoverProp.dwLevel, maxLevel);
	fExpValue *= static_cast<EXPFLOAT>(fFactor);

	// If the monster level and the average level delta is 5 or higher, the
	// party experience and points do not increase.
	m_party->GetPoint(totalLevel, m_players.size(), pDead.GetLevel());

	const auto type = GetPartyDistributionType(*m_party);

	// Compute min level exclusive to earn exp
	if (maxLevel < 20) maxLevel = 0;
	else maxLevel = maxLevel - 20;

	switch (type) {
		case PartyDistributionType::Contribution:
			AddExperiencePartyContribution(fExpValue, maxLevel);
			break;
		case PartyDistributionType::Level:
		default:
			AddExperiencePartyLevel(fExpValue, maxLevel);
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

	if (AddExperience(nExp, TRUE, TRUE, TRUE))
		LevelUpSetting();
	else
		ExpUpSetting();

	AddSetExperience(GetExp1(), (WORD)m_nLevel, m_nSkillPoint, m_nSkillLevel);
}

