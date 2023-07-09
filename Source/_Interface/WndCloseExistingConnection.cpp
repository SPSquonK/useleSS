#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndCloseExistingConnection.h"
#include "WndManager.h"
#include "dpcertified.h"


/****************************************************
  WndId : APP_CLOSE_EXISTING_CONNECTION - !
  CtrlId : WIDC_STATIC1 - Static
  CtrlId : WIDC_YES - Button
  CtrlId : WIDC_NO - Button
****************************************************/
 
void CWndCloseExistingConnection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0065) ), 0xff000000 );
	m_wndText.ResetString();

	MoveParentCenter();
} 

BOOL CWndCloseExistingConnection::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog( APP_CLOSE_EXISTING_CONNECTION, pWndParent, 0, CPoint(0, 0) );
}

BOOL CWndCloseExistingConnection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) { 
	if (nID == WIDC_YES) {
		g_dpCertified.SendCloseExistingConnection(g_Neuz.m_szAccount, g_Neuz.m_szPassword);
		Sleep(1000);
		EnableLoginAndDestroy();
	} else if (nID == WIDC_NO) {
		EnableLoginAndDestroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCloseExistingConnection::EnableLoginAndDestroy() {
	if (CWndLogin * pWndLogin = g_WndMng.GetWndBase<CWndLogin>(APP_LOGIN)) {
		pWndLogin->GetDlgItem(WIDC_OK)->EnableWindow(TRUE);
	}

	Destroy(TRUE);
}
