#include "stdafx.h"

#include "rangda.h"
#include "worldmng.h"
#include "lord.h"

////////////////////////////////////////////////////////////////////////////////

bool CRangda::IsDamaged(CMover * pMonster) {
	return (pMonster->GetHitPoint() < pMonster->GetMaxHitPoint());
}


////////////////////////////////////////////////////////////////////////////////

CRangda::CRangda(Prop prop)
	: prop(prop), state() {}

void CRangda::OnTimer() {
	CMover * pMonster = GetMonster();
	if (IsValidObj(pMonster)) {
		ProcessReplace(pMonster);
	} else {
		ProcessGenerate();
	}
}

////////

void CRangda::SetObj(OBJID objid) {
	state.m_objid = objid;
	state.m_nGenerateCountdown = prop.m_nInterval;
	state.m_nReplaceCountdown = prop.m_nReplace;
	state.m_bReplaceable = true;
}

void CRangda::CreateMonster() {
	const RANGDA_POS pos = prop.GetRandomPos(state.m_nOldPos);
	CWorld * const pWorld = g_WorldMng.GetWorld(pos.dwWorldId);
	if (!pWorld) return;

	CMover * const pMonster = static_cast<CMover *>(::CreateObj(OT_MOVER, prop.m_dwMonster));
	if (!pMonster) return;

	pMonster->m_bActiveAttack = prop.m_bActiveAttack;
	pMonster->SetPos(pos.vPos);
	pMonster->InitMotion(MTI_STAND);
	pMonster->UpdateLocalMatrix();
	pMonster->AddItToGlobalId();
	pWorld->ADDOBJ(pMonster, FALSE, nDefaultLayer);
	SetObj(pMonster->GetId());
}

CMover * CRangda::GetMonster() {
	if (state.m_objid == NULL_ID) {
		return nullptr;
	}

	CMover * pMonster = prj.GetMover(state.m_objid);
	if (!pMonster) {
		state.m_objid = NULL_ID;
		return nullptr;
	}

	return pMonster;
}

////////

void CRangda::ProcessReplace(CMover * pMonster) {
	if (!state.m_bReplaceable) return;

	if (IsDamaged(pMonster)) {
		state.m_bReplaceable = false;
		return;
	}

	if (DecrementCounter(state.m_nReplaceCountdown)) {
		pMonster->Delete();
		CreateMonster();
	}
}

void CRangda::ProcessGenerate() {
	if (DecrementCounter(state.m_nGenerateCountdown)) {
		CreateMonster();
	}
}

bool CRangda::DecrementCounter(int & tickCounter) {
	if (tickCounter > 0)
		tickCounter--;
	return tickCounter == 0;
}

////////

CRangda::RANGDA_POS CRangda::Prop::GetRandomPos(std::optional<size_t> & oldPos) const {
	ASSERT(!m_vvPos.empty());

	if (m_vvPos.size() == 1) {
		oldPos = 0;
		return m_vvPos[0];
	}

	size_t nPos;
	do {
		nPos = xRandom(0, m_vvPos.size());
	} while (oldPos && nPos == *oldPos);

	oldPos = nPos;
	return m_vvPos[nPos];
}


////////////////////////////////////////////////////////////////////////////////
// CRangdaController

CRangdaController * CRangdaController::Instance() {
	static CRangdaController sRangdaController;
	return &sRangdaController;
}

void CRangdaController::OnTimer() {
	for (CRangda & pRangda : m_vRangda) {
		pRangda.OnTimer();
	}
}

bool CRangdaController::LoadScript(const char * szFile) {
	CScript s;
	if (!s.Load(szFile)) return false;

	while (true) {
		const DWORD dwMonster = s.GetNumber();		// subject or FINISHED
		if (s.tok == FINISHED) break;

		CRangda::Prop prop{ dwMonster };

		s.GetToken();	// {
		while (true) {
			s.GetToken();	// subject or '}'
			if (*s.token == '}') break;

			if (s.Token == _T("nInterval"))
				prop.m_nInterval = s.GetNumber();
			else if (s.Token == _T("nReplace"))
				prop.m_nReplace = s.GetNumber();
			else if (s.Token == _T("bActiveAttack"))
				prop.m_bActiveAttack = static_cast<BOOL>(s.GetNumber());
			else if (s.Token == _T("vRangda")) {
				s.GetToken();	// {

				while (true) {
					CRangda::RANGDA_POS pos;
					pos.dwWorldId = s.GetNumber();

					if (*s.token == '}') break;

					pos.vPos.x = s.GetFloat();
					pos.vPos.y = s.GetFloat();
					pos.vPos.z = s.GetFloat();

					prop.m_vvPos.emplace_back(pos);
				}
			}
		}

		m_vRangda.emplace_back(prop);
	}

	return true;
}

