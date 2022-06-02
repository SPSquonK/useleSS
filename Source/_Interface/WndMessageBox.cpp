#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "dploginclient.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 메시지 박스 기본 클레스 
//
//
// IDABORT    The Abort CHILD was selected.
// IDCANCEL   The Cancel CHILD was selected.
// IDIGNORE   The Ignore CHILD was selected.
// IDNO       The No CHILD was selected.
// IDOK       The OK CHILD was selected.
// IDRETRY    The Retry CHILD was selected.
// IDYES      The Yes CHILD was selected. 
//
// MB_ABORTRETRYIGNORE   The message box contains three pushCHILDs: Abort, Retry, and Ignore.
// MB_OK   The message box contains one pushCHILD: OK.
// MB_OKCANCEL   The message box contains two pushCHILDs: OK and Cancel.
// MB_RETRYCANCEL   The message box contains two pushCHILDs: Retry and Cancel.
// MB_YESNO   The message box contains two pushCHILDs: Yes and No.
// MB_YESNOCANCEL   The message box contains three pushCHILDs: Yes, No, and Cancel. 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static void SetupWindowsStyleDialog(
	CWndBase * self, const UINT type,
	std::array<CWndButton, 3> & buttons, CWndText & wndText
) {
	const int x = self->m_rectClient.Width() / 2;
	const int y = self->m_rectClient.Height() - 30;
	const CSize size = CSize(60, 25);

	const CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	const CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	const CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	const CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	const CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	const CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	CRect rect = self->GetClientRect();
	rect.DeflateRect(10, 10, 10, 35);
	wndText.AddWndStyle(WBS_VSCROLL);
	wndText.Create(WBS_NODRAWFRAME, rect, self, 0);

	switch (type) {
		case MB_ABORTRETRYIGNORE:
			buttons[0].Create("ABORT", 0, rect3_1, self, IDABORT);
			buttons[1].Create("RETRY", 0, rect3_1, self, IDRETRY);
			buttons[2].Create("IGNORE", 0, rect3_1, self, IDIGNORE);
			break;
		case MB_OK:
			buttons[0].Create("OK", 0, rect1_1, self, IDOK);
			buttons[0].SetTexture(self->m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtOk.tga"));
			buttons[0].FitTextureSize();
			break;
		case MB_CANCEL:
			buttons[0].Create("CANCEL", 0, rect1_1, self, IDCANCEL);
			buttons[0].SetTexture(self->m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtCancel.tga"));
			buttons[0].FitTextureSize();
			break;
		case MB_OKCANCEL:
			buttons[0].Create("OK", 0, rect2_1, self, IDOK);
			buttons[0].SetTexture(self->m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtOk.tga"));
			buttons[0].FitTextureSize();
			buttons[1].Create("CANCEL", 0, rect2_2, self, IDCANCEL);
			buttons[1].SetTexture(self->m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtCancel.tga"));
			buttons[1].FitTextureSize();
			break;
		case MB_RETRYCANCEL:
			buttons[0].Create("RETRY", 0, rect2_1, self, IDRETRY);
			buttons[1].Create("CANCEL", 0, rect2_2, self, IDCANCEL);
			break;
		case MB_YESNO:
			buttons[0].Create("YES", 0, rect2_1, self, IDYES);
			buttons[1].Create("NO", 0, rect2_2, self, IDNO);
			break;
		case MB_YESNOCANCEL:
			buttons[0].Create("YES", 0, rect3_1, self, IDYES);
			buttons[1].Create("NO", 0, rect3_2, self, IDNO);
			buttons[2].Create("CANCEL", 0, rect3_3, self, IDCANCEL);
			break;
	}
	buttons[0].SetDefault(TRUE);
}


void CWndMessageBox::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	SetupWindowsStyleDialog(this, m_nType, m_wndButtons, m_wndText);

	MoveParentCenter();
}
void CWndMessageBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_RETURN )
	{
		CWndLogin *pWndLogin = (CWndLogin *)g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			CWndEdit* pPassword = (CWndEdit*) pWndLogin->GetDlgItem( WIDC_PASSWORD );
			if( pPassword )
			{
				pPassword->SetString("");//>m_string = "";
				pPassword->PostMessage( WM_SETFOCUS );//SetFocus();
			}
			Destroy(); 
		}
	}
}

BOOL CWndMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case IDABORT: 
		case IDCANCEL:   
		case IDIGNORE:   
		case IDNO:       
		case IDOK:       
		case IDRETRY:    
		case IDYES:
			if( _tcscmp( m_wndText.m_string, _T( prj.GetText(TID_DIAG_0035) ) ) == 0 )
				::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0 );
			Destroy(); 
			break;
	}
	if( GetParentWnd() != m_pWndRoot )
		GetParentWnd()->OnCommand( nID, message, this );
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return TRUE;
}
BOOL CWndMessageBox::Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType )
{
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	return Create( lpszMessage, nType, CRect( 0, 0, 0, 0 ), APP_MESSAGEBOX, pWndParent );//RUE;
}
BOOL CWndMessageBox::Create( LPCTSTR lpszMessage, UINT nType, const RECT& rect, UINT nID, CWndBase* pWndParent )
{
	m_nType = nType;
	SetTitle( lpszMessage );
	CWndNeuz::InitDialog( nID, pWndParent, WBS_MODAL, CPoint( -1, -1 ) );
	m_wndText.SetString( lpszMessage, 0xff000000 );
	m_wndText.ResetString();
	return 1;
}

void CWndMessageBox::OnEnter(const UINT nChar) {
	constexpr auto IsDisable = [](CWndMessageBox & self) {
		const CWndButton * const pWndButton = self.GetDlgItem<CWndButton>(IDOK);
		return pWndButton && pWndButton->IsWindowEnabled();
	};

	if (IsDisable(*this)) {
		Destroy();
	}
}



void CWndMessageBoxUpper::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	SetupWindowsStyleDialog(this, m_nType, m_wndButtons, m_wndText);

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
}

BOOL CWndMessageBoxUpper::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case IDABORT: 
		case IDCANCEL:   
		case IDIGNORE:   
		case IDNO:       
		case IDOK:       
		case IDRETRY:    
		case IDYES:
#ifndef _DEBUG			
			if( m_bPostLogoutMsg )
				::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, TRUE, 0 );
#endif
			Destroy(); 
			break;
	}
	
	if( nID == 10000 )
		return TRUE;

	if( GetParentWnd() != m_pWndRoot )
		GetParentWnd()->OnCommand( nID, message, this );

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndMessageBoxUpper::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return TRUE;
}
BOOL CWndMessageBoxUpper::Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType, BOOL bPostLogoutMsg )
{
	m_bPostLogoutMsg = bPostLogoutMsg;
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
	//Create( _T( "매시지 박스" ), nType, rect, APP_MESSAGEBOX );
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	return Create( lpszMessage, nType, rect, APP_MESSAGEBOX, pWndParent );//RUE;
}
BOOL CWndMessageBoxUpper::Create( LPCTSTR lpszMessage, UINT nType, const RECT& rect, UINT nID, CWndBase* pWndParent )
{
	m_nType = nType;
	SetTitle( lpszMessage );
	CWndNeuz::InitDialog( nID, pWndParent, WBS_MODAL, CPoint( -1, -1 ) );
	m_wndText.SetString( lpszMessage, 0xff000000 );
	m_wndText.ResetString();
	return 1;
}
