#include "stdafx.h"
#include "misc.h"
#include "CooltimeMgr.h"
#include "ProjectCmn.h"

// Get the cool-time group number of the cool-time item.
// Items in the same group receive the same cooldown when used once.
// Note: If you want to modify it, adjust MAX_COOLTIME_TYPE carefully in the header file.
DWORD CCooltimeMgr::GetGroup(const ItemProp * const pItemProp) {
	if (pItemProp->dwSkillReady == 0) return 0;

	switch ( pItemProp->dwItemKind2 ) {
	case IK2_FOOD:
		if (pItemProp->dwItemKind3 == IK3_PILL)
			return 2;
		return 1;
	case IK2_SKILL: // case IK2_POTION:
		return 3;
	default:
		return 0;
	}
}

bool CCooltimeMgr::CanUse(const DWORD dwGroup) const {
	ASSERT(dwGroup > 0);
	return g_tmCurrent > GetTime(dwGroup);
}

// Record the time used and use it as a basis for judgment in CanUse.
void CCooltimeMgr::SetTime(const DWORD dwGroup, const DWORD dwCoolTime) {
	ASSERT(dwGroup > 0 && dwCoolTime > 0);
	const DWORD dwTick = g_tmCurrent;
	m_cds[dwGroup - 1] = { dwTick + dwCoolTime, dwTick };
}	
