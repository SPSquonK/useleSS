#include "StdAfx.h"
#include "User.h"


constexpr static float GetSoloExpDebuff(const int myLevel, const DWORD enemyLevel) {
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

	const auto expLimit = static_cast<EXPFLOAT>(prj.m_aExpCharacter[m_nLevel].nLimitExp);
	if (fExpValue > expLimit) fExpValue = expLimit;

	// Flight Exp
	if (fFxpValue) {
		if (AddFxp(static_cast<int>(fFxpValue))) {
			g_UserMng.AddSetFlightLevel(this, GetFlightLv());
		}

		AddSetFxp(m_nFxp, GetFlightLv());
	}

	if (AddExperience(static_cast<EXPINTEGER>(fExpValue), TRUE, TRUE, TRUE)) {
		// Level up
		LevelUpSetting();
	} else {
		ExpUpSetting();
	}

	AddSetExperience(GetExp1(), (WORD)m_nLevel, m_nSkillPoint, m_nSkillLevel);
}
