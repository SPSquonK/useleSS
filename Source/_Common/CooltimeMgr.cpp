#include "stdafx.h"
#include "misc.h"
#include "CooltimeMgr.h"
#include "ProjectCmn.h"

// Get the cool-time group number of the cool-time item.
// Items in the same group receive the same cooldown when used once.
// Note: If you want to modify it, adjust MAX_COOLTIME_TYPE carefully in the header file.
DWORD CCooltimeMgr::GetGroup(const ItemProp & pItemProp) {
	if (pItemProp.dwSkillReady == 0) return 0;

	switch (pItemProp.dwItemKind2) {
		case IK2_FOOD:
			if (pItemProp.dwItemKind3 == IK3_PILL)
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
	return g_tmCurrent > m_cds[dwGroup - 1].time;
}

void CCooltimeMgr::StartCooldown(const ItemProp & itemProp) {
	if (itemProp.GetCoolTime() == 0) return;

	const auto group = GetGroup(itemProp);
	if (group != 0) {
		m_cds[group - 1] = { g_tmCurrent + itemProp.GetCoolTime(), g_tmCurrent };
	}
}

std::optional<CCooltimeMgr::RemainingCooldown>
CCooltimeMgr::GetElapsedTime(const ItemProp & itemProp) const {
	const auto group = GetGroup(itemProp);
	if (group == 0) return std::nullopt;
	const Cooldown & cds = m_cds[group - 1];
	if (cds.time == 0 || cds.time <= g_tmCurrent) return std::nullopt;

	return RemainingCooldown{ g_tmCurrent - cds.base, cds.time - cds.base };
}

[[nodiscard]] DWORD CCooltimeMgr::GetRemainingTime(const ItemProp & itemProp) const {
	const auto group = GetGroup(itemProp);
	if (group == 0) return 0;
	const Cooldown & cds = m_cds[group - 1];
	if (cds.time == 0 || cds.time <= g_tmCurrent) return 0;

	return cds.time - g_tmCurrent;
}

