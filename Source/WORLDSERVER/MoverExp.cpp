#include "StdAfx.h"
#include "User.h"
#include "party.h"
#include "GroupUtils.h"

#undef min
#undef max


///////////////////////////////////////////////////////////////////////////////
// Entry point

static BOOL GetPartyMemberFind(const CMover & pDead, CParty * pParty, CUser * apMember[], int * nTotalLevel, int * nMaxLevel10, int * nMaxLevel, int * nMemberSize);

void CMover::AddExperienceKillMember(CMover * pDead, EXPFLOAT fExpValue, MoverProp * pMoverProp, float fFxpValue) {
	std::vector<OBJID>	adwEnemy;
	std::vector<int>		anHitPoint;
	DWORD	dwMaxEnemyHit = 0;
	for (SET_OBJID::iterator it = pDead->m_idEnemies.begin(); it != pDead->m_idEnemies.end(); ++it) {
		adwEnemy.push_back((*it).first);
		anHitPoint.push_back((*it).second.nHit);
		dwMaxEnemyHit += (*it).second.nHit;
	}

	if (adwEnemy.size() > 1024) {
		Error("CMover::AddExperienceKillMember - enemy size is too big");
	}

	if (dwMaxEnemyHit == 0)
		return;

	for (DWORD j = 0; j < adwEnemy.size(); j++) {
		if (adwEnemy[j] == 0) continue;

		CMover * pEnemy_ = prj.GetMover(adwEnemy[j]);
		if (IsValidObj(pEnemy_) && pDead->IsValidArea(pEnemy_, 64.0f) && pEnemy_->IsPlayer())
		{
			CUser * pEnemy = static_cast<CUser *>(pEnemy_);
			DWORD dwHitPointParty = 0;
			CParty * pParty = g_PartyMng.GetParty(pEnemy->m_idparty);
			if (pParty && pParty->IsMember(pEnemy->m_idPlayer)) {
				dwHitPointParty = anHitPoint[j];
				for (DWORD k = j + 1; k < adwEnemy.size(); k++) {
					if (adwEnemy[k] == 0)
						continue;	// Skip duplicates
					CMover * pEnemy2 = prj.GetMover(adwEnemy[k]);
					if (IsValidObj(pEnemy2) && pDead->IsValidArea(pEnemy2, 64.0f) && pEnemy2->IsPlayer())
					{
						if (pEnemy->m_idparty == pEnemy2->m_idparty && pParty->IsMember(pEnemy2->m_idPlayer))
						{
							dwHitPointParty += anHitPoint[k];
							adwEnemy[k] = 0;
						}
					} else {
						adwEnemy[k] = 0;
					}
				}
			}
			if (dwHitPointParty > 0)
				anHitPoint[j] = dwHitPointParty;
			float fExpValuePerson = (float)(fExpValue * (float(anHitPoint[j]) / float(dwMaxEnemyHit)));
			if (dwHitPointParty)
			{
				int nTotalLevel = 0;
				int nMaxLevel10 = 0;
				int nMaxLevel = 0;
				int nMemberSize = 0;
				CUser * apMember[MAX_PTMEMBER_SIZE];
				memset(apMember, 0, sizeof(apMember));

				if (!GetPartyMemberFind(*pDead, pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize))
					break;
				fExpValuePerson *= CPCBang::GetInstance()->GetPartyExpFactor(apMember, nMemberSize);
				if (1 < nMemberSize)
					pEnemy->AddExperienceParty(pDead, fExpValuePerson, pMoverProp, fFxpValue, pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize);
				else
					pEnemy->AddExperienceSolo(fExpValuePerson, pMoverProp, fFxpValue, true);
			} else {
				if (IsPlayer())
					fExpValuePerson *= CPCBang::GetInstance()->GetExpFactor(static_cast<CUser *>(this));
				pEnemy->AddExperienceSolo(fExpValuePerson, pMoverProp, fFxpValue, false);
			}
		}
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

BOOL GetPartyMemberFind(const CMover & pDead, CParty * pParty, CUser * apMember[], int * nTotalLevel, int * nMaxLevel10, int * nMaxLevel, int * nMemberSize) {
	for (CUser * pUsertmp : AllMembers(*pParty)) {
		if (pDead.IsValidArea(pUsertmp, 64.0f)) {
			apMember[(*nMemberSize)++] = pUsertmp;
			(*nTotalLevel) += pUsertmp->GetLevel();
			if ((*nMaxLevel10) < pUsertmp->GetLevel()) {
				(*nMaxLevel) = (*nMaxLevel10) = pUsertmp->GetLevel();
			}
		}
	}

	if (0 < (*nMaxLevel10) - 20) {
		(*nMaxLevel10) -= 20;
	} else {
		(*nMaxLevel10) = 0;
	}

	if ((*nMemberSize) == 0 || (*nTotalLevel) == 0) {
		return FALSE;
	}

	return TRUE;
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

void CUser::AddExperienceParty(CMover * pDead, EXPFLOAT fExpValue, MoverProp * pMoverProp, float fFxpValue, CParty * pParty, CUser * apMember[], int * nTotalLevel, int * nMaxLevel10, int * nMaxLevel, int * nMemberSize) {
	// Decreased experience to be the highest level party member among nearby party members
	float fFactor = GetExperienceReduceFactor((int)pMoverProp->dwLevel, *nMaxLevel);
	fExpValue *= static_cast<EXPFLOAT>(fFactor);
	fFxpValue *= fFactor;

	// If the monster level and the average level delta is 5 or higher, the
	// party experience and points do not increase.
	pParty->GetPoint((*nTotalLevel), (*nMemberSize), pDead->GetLevel());

	const auto type = GetPartyDistributionType(*pParty);

	switch (type) {
		case PartyDistributionType::Contribution:
			AddExperiencePartyContribution(pDead, apMember, pParty, fExpValue, (*nMemberSize), (*nMaxLevel10));
			break;
		case PartyDistributionType::Level:
		default:
			AddExperiencePartyLevel(apMember, pParty, fExpValue, (*nMemberSize), (*nMaxLevel10));
	}
}

// Reward by contribution to damage
void CUser::AddExperiencePartyContribution(CMover * pDead, CUser * apMember[], CParty * pParty, EXPFLOAT fExpValue, int nMemberSize, int nMaxLevel10) {
	int nAttackMember = 0;
	float fMaxMemberLevel = 0.0f;
	for (int i = 0; i < nMemberSize; i++) {
		if (pDead->GetEnemyHit(apMember[i]->GetId()) != 0) {
			++nAttackMember;
		}
		fMaxMemberLevel += ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
	}

	float fAddExp = (float)((fExpValue * 0.2f) * (pParty->m_nSizeofMember - 1));
	
	float fFullParty = 0.0f;
	if (nMemberSize == MAX_PTMEMBER_SIZE_SPECIAL) {
		fFullParty = (float)((fExpValue * 0.1f));
	}
	float fOptionExp = 0.0f;
	if (nAttackMember > 1) {
		fOptionExp = (float)((fExpValue * (float)nAttackMember / 100.0f));
	}

	for (int i = 0; i < nMemberSize; i++) {
		if (apMember[i]->GetLevel() <= nMaxLevel10) continue;

		const int nHit = pDead->GetEnemyHit(apMember[i]->GetId());
		const float fContribution = std::min((float)nHit * 100 / (float)pDead->GetMaxHitPoint(), 100.f);

		const EXPINTEGER damageExp = static_cast<EXPINTEGER>(fExpValue * (fContribution / 100.0f));

		const float squaredLevel = ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
		const EXPINTEGER watchExp = (fAddExp * (squaredLevel / fMaxMemberLevel)) + fOptionExp + fFullParty;

		const EXPINTEGER nMemberExp = damageExp + watchExp;

		apMember[i]->AddPartyMemberExperience(nMemberExp, 0);
	}
}

void CUser::AddExperiencePartyLevel(CUser * apMember[], CParty * pParty, EXPFLOAT fExpValue, int nMemberSize, int nMaxLevel10) {
	float fMaxMemberLevel = 0.0f;
	for (int i = 0; i < nMemberSize; i++) {
		fMaxMemberLevel += ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
	}

	const float fAddExp = (float)((fExpValue * 0.2f) * (nMemberSize - 1));

	for (int i = 0; i < nMemberSize; i++) {
		if (apMember[i]->GetLevel() <= nMaxLevel10) continue;
		
		const float squaredLevel = ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
		const EXPINTEGER nMemberExp = static_cast<EXPINTEGER>((fExpValue + fAddExp) * (squaredLevel / fMaxMemberLevel));
		apMember[i]->AddPartyMemberExperience(nMemberExp, 0);
	}
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

