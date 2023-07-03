#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndSealChar.h"
#include "MsgHdr.h"
#include "DPClient.h"


///////////////////////////////////////////////////////////////////////////////

void CWndSealChar::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndText * const pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	LPCTSTR strTemp = prj.GetText(TID_DIAG_SEAL_CHAR_TEXT01);
	pWndText->SetString(strTemp);

	Move70();
}

BOOL CWndSealChar::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_SEAL_CHAR, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSealChar::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_OK) {
		g_DPlay.SendPacket<PACKETTYPE_SEALCHAR_REQ>();
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


///////////////////////////////////////////////////////////////////////////////

void CWndSealCharSelect::OpenOrResetWindow(std::vector<Target> targets) {
	if (!g_WndMng.m_pWndSealCharSelect) {
		g_WndMng.m_pWndSealCharSelect = new CWndSealCharSelect(std::move(targets));
		g_WndMng.m_pWndSealCharSelect->Initialize();
	} else {
		g_WndMng.m_pWndSealCharSelect->m_targets = std::move(targets);
		g_WndMng.m_pWndSealCharSelect->UpdateRadioButtons();
	}
}

void CWndSealCharSelect::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	LPCTSTR strTemp = _T(prj.GetText(TID_DIAG_SEAL_CHAR_SELECT_TEXT01));
	pWndText->SetString(strTemp);

	UpdateRadioButtons();

	Move70();
}

BOOL CWndSealCharSelect::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_SEAL_CHAR_SELECT, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSealCharSelect::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_OK) {
		UINT nRadios[2] = { WIDC_RADIO1, WIDC_RADIO2 };
		size_t i;
		for (i = 0; i < 2; ++i) {
			CWndButton * pButton = GetDlgItem<CWndButton>(nRadios[i]);
			if (pButton->GetCheck())
				break;
		}

		if (i < m_targets.size()) {
			CWndSealCharSend::OpenOrResetWindow(m_targets[i]);
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSealCharSelect::UpdateRadioButtons() {
	std::array<CWndButton *, 2> buttons = {
		GetDlgItem<CWndButton>(WIDC_RADIO1), GetDlgItem<CWndButton>(WIDC_RADIO2)
	};
	
	buttons[0]->SetGroup(TRUE);
	buttons[0]->SetCheck(TRUE);
	buttons[1]->SetCheck(FALSE);

	size_t max = std::min(buttons.size(), m_targets.size());

	for (size_t i = 0; i != max; ++i) {
		buttons[i]->EnableWindow(TRUE);
		buttons[i]->SetTitle(m_targets[i].name);
	}

	for (size_t i = max; i != buttons.size(); ++i) {
		buttons[i]->EnableWindow(FALSE);
	}
}


///////////////////////////////////////////////////////////////////////////////

void CWndSealCharSend::OpenOrResetWindow(const CWndSealCharSelect::Target & target) {
	CWndSealCharSend *& pWndCWndSealCharSend = g_WndMng.m_pWndSealCharSend;
	if (!pWndCWndSealCharSend) {
		pWndCWndSealCharSend = new CWndSealCharSend;
		pWndCWndSealCharSend->Initialize();
	}

	pWndCWndSealCharSend->UpdateTarget(target);
}

void CWndSealCharSend::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	Move70();
}

BOOL CWndSealCharSend::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_SEAL_CHAR_SEND, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSealCharSend::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_YES) {
		g_DPlay.SendPacket<PACKETTYPE_SEALCHARCONM_REQ, OBJID>(m_target.id);
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSealCharSend::UpdateTarget(const CWndSealCharSelect::Target & target) {
	m_target = target;

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);

	CString strTemp;
	strTemp.Format(_T(prj.GetText(TID_DIAG_SEAL_CHAR_SEND_TEXT01)), m_target.name);
	pWndText->SetString(strTemp);
}


///////////////////////////////////////////////////////////////////////////////

void CWndSealCharSet::OpenOrResetWindow(DWORD scrollPosition) {
	if (!g_WndMng.m_pWndSealCharSet) {
		g_WndMng.m_pWndSealCharSet = new CWndSealCharSet;
		g_WndMng.m_pWndSealCharSet->Initialize();
	}

	g_WndMng.m_pWndSealCharSet->m_scrollPos = scrollPosition;
}

void CWndSealCharSet::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	LPCTSTR strTemp = _T(prj.GetText(TID_DIAG_SEAL_CHAR_SET_TEXT01));
	pWndText->SetString(strTemp);

	Move70();
}

BOOL CWndSealCharSet::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_SEAL_CHAR_SET, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSealCharSet::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_YES) {
		g_DPlay.SendPacket<PACKETTYPE_SEALCHARGET_REQ, DWORD>(m_scrollPos);
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
