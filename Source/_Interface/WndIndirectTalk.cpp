#include "stdafx.h"
#include "AppDefine.h"
#include "WndIndirectTalk.h"
#include "FuncTextCmd.h"

/****************************************************
  WndId : APP_ADMIN_INDIRECT_TALK
  CtrlId : WIDC_EDIT1 - Edit
  CtrlId : WIDC_STATIC1 - Mover ID
  CtrlId : WIDC_EDIT2 - Edit
  CtrlId : WIDC_BUTTON1 - Send
****************************************************/

void CWndIndirectTalk::OnDraw(C2DRender * p2DRender) {
	CWorld * pWorld = g_WorldMng();

	CObj * pFocus = pWorld->GetObjFocus();
	TCHAR szNum[32];
	if (CMover * pMover = pFocus ? pFocus->ToMover() : nullptr) {
		itoa(pMover->GetId(), szNum, 10);
	} else {
		itoa(0, szNum, 10);
	}

	CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);
	pWndEdit->SetString(szNum);
}

void CWndIndirectTalk::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	GetDlgItem<CWndEdit>(WIDC_EDIT2)->AddWndStyle(EBS_AUTOVSCROLL);
	MoveParentCenter();
}

BOOL CWndIndirectTalk::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_ADMIN_INDIRECT_TALK, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndIndirectTalk::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWorld* pWorld = g_WorldMng();
	CObj* pObj = pWorld->GetObjFocus();
	if (pObj && pObj->GetType() == OT_MOVER) {
		if (nID == WIDC_BUTTON1 || (nID == WIDC_EDIT2 && message == EN_RETURN)) {
			CWndEdit * pWndEdit1 = GetDlgItem<CWndEdit>(WIDC_EDIT1);
			CWndEdit * pWndEdit2 = GetDlgItem<CWndEdit>(WIDC_EDIT2);
			LPCTSTR lpId = pWndEdit1->m_string;
			LPCTSTR lpText = pWndEdit2->m_string;
			CString string;
			string.Format("/id %s %s", lpId, lpText);
			g_textCmdFuncs.ParseCommand(string.GetString(), g_pPlayer);
			pWndEdit2->Empty();
		}
	}

	if (nID == WTBID_CLOSE) {
		Destroy(TRUE);
		return TRUE;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

