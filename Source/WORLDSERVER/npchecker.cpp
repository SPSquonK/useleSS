#include "stdafx.h"
#include "npchecker.h"

CNpcChecker * CNpcChecker::GetInstance(void) {
	static CNpcChecker	sNpcChecker;
	return &sNpcChecker;
}

void CNpcChecker::RemoveFrom(std::set<OBJID> & set, const OBJID id) {
	auto it = set.find(id);
	if (it != set.end()) set.erase(it);
}

void CNpcChecker::AddNpc(CObj * const pObj) {
	CMover * pNpc = pObj->ToMover();
	if (!pNpc) return;

	const CHARACTER * pCharacter = pNpc->GetCharacter();
	if (!pCharacter) return;

	const OBJID objid = pNpc->GetId();
	bool atLeastOne = false;
	for (int i = 0; i < MAX_MOVER_MENU; ++i) {
		if (pCharacter->m_abMoverMenu[i]) {
			m_perMenu[i].insert(objid);
			atLeastOne = true;
		}
	}

	if (atLeastOne) {
		m_all.insert(objid);
	}
}

void CNpcChecker::RemoveNpc(CObj * const pObj) {
	CMover * pNpc = pObj->ToMover();
	if (!pNpc) return;

	const CHARACTER * pCharacter = pNpc->GetCharacter();
	if (!pCharacter) return;

	const OBJID objid = pNpc->GetId();
	for (int j = 0; j < MAX_MOVER_MENU; ++j) {
		RemoveFrom(m_perMenu[j], objid);
	}

	RemoveFrom(m_all, objid);
}

std::optional<bool> CNpcChecker::IsCloseNpc(int nMenu, const CObj * const pObj) const {
	if (nMenu < 0 || nMenu >= MAX_MOVER_MENU) return std::nullopt;

	return HasNear(m_perMenu[nMenu], pObj, MAX_LEN_MOVER_MENU);
}

bool CNpcChecker::IsCloseFromAnyNpc(const CObj * const pObj) const {
	return HasNear(m_all, pObj, MAX_NPC_RADIUS);
}

bool CNpcChecker::HasNear(const std::set<OBJID> & objIds, const CObj * const pObj, const double maxDistance) {
	const CWorld * const pWorld = pObj->GetWorld();
	const D3DXVECTOR3 v = pObj->GetPos();

	for (const auto objId : objIds) {
		const CMover * const pOther = prj.GetMover(objId);
		if (pWorld != pOther->GetWorld()) continue;

		D3DXVECTOR3 vOut = v - pOther->GetPos();
		vOut.y = 0.0f;

		if (fabs((D3DXVec3LengthSq(&vOut))) < maxDistance) return true;
	}

	return false;
}
