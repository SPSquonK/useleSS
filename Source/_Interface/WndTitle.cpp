#include "stdafx.h"
#include <format>

#include "defineObj.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPLoginClient.h"
#include "DPClient.h"
#include "dpCertified.h"
#include "wndCredit.h"
#include "..\_Common\Debug.h"
#include "webbox.h"

#include "WndManager.h"
#include "Network.h"
#include "Vector3Helper.h"

extern BYTE  nMaleHairColor[10][3];
extern BYTE  nFeMaleHairColor[10][3];

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#include "DPCollectClient.h"
#endif // __CERTIFIER_COLLECTING_SYSTEM

BOOL GetIePath( LPSTR lpPath )
{	
	LONG result;
	HKEY hKey;
	DWORD dwType; 
	char data[MAX_PATH];
	DWORD dataSize = MAX_PATH+1;

	result	= ::RegOpenKeyEx ( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", 0, KEY_QUERY_VALUE, &hKey );
	if (result == ERROR_SUCCESS) 
	{
		result = ::RegQueryValueEx ( hKey, "Path", NULL, &dwType, (unsigned char *)data, &dataSize );
		strcpy( lpPath, data );
		lpPath[lstrlen( lpPath )-1]	= '\0';
	}
	else
		return FALSE;

	RegCloseKey( hKey );
	return TRUE;
}

BOOL CWndConnectingBox::Initialize( CWndBase* pWndParent, DWORD nType  ) 
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );

	Create( _T( prj.GetText(TID_DIAG_0068) ), /*MB_CANCEL*/0xFFFFFFFF, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0064) ) );
	
	m_wndText.ResetString();
	return CWndMessageBox::Initialize( pWndParent, 0 );
}
BOOL CWndConnectingBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return TRUE;
}

BOOL CWndCharBlockBox::Initialize( CWndBase* pWndParent, DWORD nType  ) 
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );

	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_CANCEL, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0073) ) );

	m_wndText.ResetString();
	return CWndMessageBox::Initialize( pWndParent, 0 );
}
BOOL CWndCharBlockBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return CWndMessageBox::OnChildNotify( message, nID, pLResult );
}
BOOL CWndAllCharBlockBox::Initialize( CWndBase* pWndParent, DWORD nType  ) 
{
    CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );

	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_CANCEL, rect, APP_MESSAGEBOX );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0074) ) );

	m_wndText.ResetString();
	return CWndMessageBox::Initialize( pWndParent, 0 );
}
BOOL CWndAllCharBlockBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
		case IDCANCEL:   
			{
				g_dpLoginClient.DeleteDPObject();
				Windows::DestroyIfOpened(APP_SELECT_CHAR);
				g_dpCertified.SendCertify();
				g_WndMng.OpenApplet(APP_LOGIN);
			}
			break;
		}
	}
	return CWndMessageBox::OnChildNotify( message, nID, pLResult );
}

//////////////////////////////////////////////////////////////////////////////////
// Login
//////////////////////////////////////////////////////////////////////////////////

CWndLogin::CWndLogin()
{
	SetPutRegInfo( FALSE );
#ifdef __NPKCRYPT
	m_hKCrypt	= NULL;
	TCHAR	szFileName[MAX_PATH]	= {0};
	TCHAR	szGameFileName[MAX_PATH]	= {0};
	BOOL	bStatus	= FALSE;
	char szPath[MAX_PATH];
	if( GetCurrentDirectory( MAX_PATH, szPath ) == 0 ) 
		return;
	
	sprintf( (char*)szGameFileName, "%s\\neuz.exe", szPath );
	if( NPKGetAppCompatFlag( szGameFileName ) != apcfNone )
    {
		OutputDebugString("--> ReStart Program\n");
    }
    else
	{
		OutputDebugString("--> Load KeyCrypt\n");
		bStatus		= NPKGetLoadStartup();

		GetModuleFileName( NULL, szFileName, sizeof(szFileName) );
		char *ptr = strrchr( szFileName, '\\' );
		if( ptr != NULL )
			*ptr	= 0;

		NPKSetDrvPath( szFileName );
		m_hKCrypt	= NPKOpenDriver();

		if(m_hKCrypt <= 0)
		{
			HLOCAL	hlocal = NULL;
			DWORD	dwError = GetLastError();
			CString	strErrMsg = "";
			
			BOOL fOk = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
				NULL, 
				dwError, 
				MAKELANGID(LANG_KOREAN, SUBLANG_ENGLISH_US), 
				(PTSTR) &hlocal, 
				0, 
				NULL);
			
			if(fOk)
			{
				strErrMsg = (PCTSTR) LocalLock(hlocal);
				::LocalFree(hlocal);
			}
			else
			{
				switch(dwError)
				{
				case NPK_ERROR_NOTADMIN:
					break;

				case NPK_ERROR_DRIVERVERSION:
//					strErrMsg.LoadString(IDS_ERR_DRIVERVERSION);				
					break;

				case NPK_ERROR_VERIFYVERSION:
//					strErrMsg.LoadString(IDS_ERR_VERIFYVERSION);				
					break;
				}
			}

//			::MessageBox(GetSafeHwnd(), strErrMsg, "nProtect KeyCrypt", MB_OK | MB_ICONERROR);
//			PostQuitMessage(0);
		} else {

			NPKLoadAtStartup(TRUE);
			NPKRegCryptMsg( m_hKCrypt, g_Neuz.GetSafeHwnd(), WM_USER + 1094 );
		}
	}
	
#endif	// __NPKCRYPT
}
CWndLogin::~CWndLogin()
{
#ifdef __NPKCRYPT
	if( m_hKCrypt > 0 )
		NPKCloseDriver( m_hKCrypt );
#endif	// __NPKCRYPT
}

BOOL CWndLogin::Process()
{
	if( g_Neuz.m_dwTimeOutDis < GetTickCount() )
	{
		g_Neuz.m_dwTimeOutDis = 0xffffffff;
		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		g_WndMng.OpenMessageBoxUpper(  _T( prj.GetText(TID_DIAG_0043) ) );
		
		m_bDisconnect = TRUE;
	}

	if( g_WndMng.m_pWndMessageBoxUpper == NULL && m_bDisconnect )
	{
		m_bDisconnect = FALSE;
		g_WndMng.CloseMessageBox();
		g_dpCertified.DeleteDPObject();
		CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
		pButton->EnableWindow( TRUE );
	}
	
	return 1;
}

void CWndLogin::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rect = GetClientRect();

#ifdef __REG
	m_wndRegist. Create( "Registration", 0, CRect( 0, 0, 100, 20 ), this, 1001 ); rect.OffsetRect( 120,0 );
#endif

	CWndEdit* pAccount = (CWndEdit*) GetDlgItem( WIDC_ACCOUNT );
	CWndEdit* pPassword = (CWndEdit*) GetDlgItem( WIDC_PASSWORD );
	CWndButton* pSaveAccount = (CWndButton*) GetDlgItem( WIDC_CHECK1 );

	if( ::GetLanguage() == LANG_FRE )
	{
		CWndStatic	* pWndStatic2	= (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		CRect rc	= pWndStatic2->GetWndRect();
		rc.right	+= 24;
		pWndStatic2->SetWndRect( rc, TRUE );

		CRect rc1	= pAccount->GetWndRect();
		CRect rc2	= pPassword->GetWndRect();
		rc1.left	+= 12;
		rc1.right	= rc2.right;
		pAccount->SetWndRect( rc1, TRUE );
		rc2.left	+= 12;
		pPassword->SetWndRect( rc2, TRUE );
	}
	
	pAccount->EnableModeChange( FALSE );
	pAccount->SetTabStop( TRUE );

	pPassword->AddWndStyle( EBS_PASSWORD );
	pPassword->SetTabStop( TRUE );
	pPassword->EnableModeChange( FALSE );

	CWndButton* pOk   = (CWndButton*)GetDlgItem( WIDC_OK );
	CWndButton* pQuit = (CWndButton*)GetDlgItem( WIDC_QUIT );

	pOk->SetDefault( TRUE );

	pAccount->SetString( g_Option.m_szAccount );
	pSaveAccount->SetCheck( g_Option.m_bSaveAccount );

	if( g_Option.m_szAccount[ 0 ] )
		pPassword->SetFocus();
	else
		pAccount->SetFocus();

	MoveParentCenter();

/*	if( ::GetLanguage() != LANG_KOR )
	{
#ifdef __FOR_PROLOGUE_UPDATE
		CWndButton* pCredit   = (CWndButton*)GetDlgItem( WIDC_CREDIT );
		pCredit->EnableWindow(FALSE);
		pCredit->SetVisible(FALSE);	
#else //__FOR_PROLOGUE_UPDATE*/
		CWndButton* pAbout   = (CWndButton*)GetDlgItem( WIDC_ABOUT );
		pAbout->EnableWindow(FALSE);
		pAbout->SetVisible(FALSE);	
		CWndButton* pCredit   = (CWndButton*)GetDlgItem( WIDC_CREDIT );
		pCredit->EnableWindow(FALSE);
		pCredit->SetVisible(FALSE);	
		CWndButton* pPrologue   = (CWndButton*)GetDlgItem( WIDC_PROLOGUE );
		pPrologue->EnableWindow(FALSE);
		pPrologue->SetVisible(FALSE);
	
		pOk->Move( 72, 105 );
		pQuit->Move( 72, 135 );
/*#endif //__FOR_PROLOGUE_UPDATE
	}*/

#ifdef __THROUGHPORTAL0810
	if( g_Neuz.m_bThroughPortal )
	{
		CRect HanrectWindow = GetWindowRect( TRUE );
		SetWndRect( CRect( HanrectWindow.left, HanrectWindow.top, HanrectWindow.right - 120, HanrectWindow.bottom - 115 ) );
		CRect rectLayout = m_pWndRoot->GetLayoutRect();
		Move( (int)( rectLayout.Width() / 2 - m_rectWindow.Width() / 2 ), (int)( rectLayout.Height() * 0.65 ) );
	}
#endif	// __THROUGHPORTAL0810

	if( ::GetLanguage() == LANG_KOR )
	{
		CWndButton* pPrologue   = (CWndButton*)GetDlgItem( WIDC_PROLOGUE );
		pPrologue->SetVisible(FALSE);
	}

	switch( ::GetLanguage() )
	{
		case LANG_KOR:
		case LANG_FRE:
		case LANG_GER:
			{
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rectWindow = GetWindowRect( TRUE );
				rectWindow.top = 400 * rectRoot.Height() / 768;
				Move( rectWindow.TopLeft() );
				break;
			}
	}


#ifdef __THROUGHPORTAL0810
	if( g_Neuz.m_bThroughPortal )
#else	// __THROUGHPORTAL0810
	if( g_Neuz.m_bHanGame )
#endif	// __THROUGHPORTAL0810
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		pStatic->SetVisible( FALSE );
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		pStatic->SetVisible( FALSE );
		CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_ACCOUNT );
		pEdit->SetVisible( FALSE );
		pEdit->Move( 800, 800 );
		pEdit = (CWndEdit*)GetDlgItem( WIDC_PASSWORD );
		pEdit->SetVisible( FALSE );
		pEdit->Move( 800, 800 );
		CWndButton* pButton = (CWndButton*) GetDlgItem( WIDC_CHECK1 );
		pButton->SetVisible( FALSE );
		pButton = (CWndButton*) GetDlgItem( WIDC_OK );
		CRect rectButton = pButton->GetWndRect();
		pButton->Move( 13, 10 );
		pButton = (CWndButton*) GetDlgItem( WIDC_ABOUT );
		pButton->Move( 13, 35 );
		pButton = (CWndButton*) GetDlgItem( WIDC_CREDIT );
		pButton->Move( 13, 60 );
		pButton = (CWndButton*) GetDlgItem( WIDC_PROLOGUE );
		pButton->Move( 13, 60 );
		pButton = (CWndButton*) GetDlgItem( WIDC_QUIT );
		pButton->Move( 13, 85 );
	}

	g_Neuz.m_dwTimeOutDis = 0xffffffff;
	m_bDisconnect = FALSE;
}
BOOL CWndLogin::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	return CWndNeuz::InitDialog( APP_LOGIN, pWndParent, WBS_KEY, CPoint( 0, 0 ) );
}

void CWndLogin::Connected( long lTimeSpan )
{
	g_WndMng.CloseMessageBox();
	g_WndMng.OpenApplet( APP_SELECT_SERVER );
	Destroy();
}

BOOL CWndLogin::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	switch(nID)
	{
		case WIDC_CHECK1:
			{
				g_Option.m_bSaveAccount = !g_Option.m_bSaveAccount;
				CWndEdit* pAccount = (CWndEdit*) GetDlgItem( WIDC_ACCOUNT );
				strcpy( g_Option.m_szAccount, pAccount->GetString() );
			}
			break;
		case WIDC_ACCOUNT:
		case WIDC_PASSWORD:
			//if( message != EN_CHANGE )
				break;
		case WIDC_ABOUT:
			{
				if( GetLanguage() == LANG_FRE )
				{
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory( &si, sizeof(si) );
					si.cb	= sizeof(si);
					// Start the child process. 
					char lpPath[MAX_PATH]	= { 0, };
					char lpCommandLine[MAX_PATH]	= { 0,};
					if( !GetIePath( lpPath ) )
						break;
					sprintf( lpCommandLine, "%s\\IEXPLORE.EXE http://flyff.gpotato.eu", lpPath );
					
					if( !CreateProcess( NULL,
					lpCommandLine,
					NULL,
					NULL,
					FALSE,
					0,
					NULL,
					NULL,
					&si,
					&pi )
					) 
					{
					}
					// Close process and thread handles.
					CloseHandle( pi.hProcess );
					CloseHandle( pi.hThread );
				}
				else
				{
					CWndAbout* pWndAbout = new CWndAbout;
					pWndAbout->Initialize();
				}
			}
			break;
		case WIDC_PROLOGUE:
		case WIDC_CREDIT:
			{
				CWndCredit* pWndCredit = new CWndCredit;
				pWndCredit->Initialize();
			}
			break;
		case WIDC_OK: // ???? 
			{
#			ifdef __CRC
				if( !g_dpCertified.ConnectToServer( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE, CSock::crcWrite ) )
#			else	// __CRC
				if( !g_dpCertified.ConnectToServer( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE ) )
#			endif	// __CRC
				{
					// Can't connect to server
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0043) ) );
//					g_WndMng.OpenMessageBox( _T( "?????? ?? ????????. ???????? ?????? ????????????." ) );
					CNetwork::GetInstance().OnEvent( CERT_CONNECT_FAIL );
					break;
				}
				CNetwork::GetInstance().OnEvent( CERT_CONNECTED );

				CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
				pButton->EnableWindow( FALSE );

				CWndEdit* pAccount = (CWndEdit*) GetDlgItem( WIDC_ACCOUNT );
				CWndEdit* pPassword = (CWndEdit*) GetDlgItem( WIDC_PASSWORD );

				CString strAccount, strPassword;
				strAccount	= pAccount->GetString();
				strPassword	= pPassword->GetString();
				
				if( IsAcValid( pAccount->GetString() ) == FALSE )
				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0005) ) );
//					g_WndMng.OpenMessageBox( _T( "?????? 3~16?? ????, ?????? ?????? ?? ????, ?????? ?????? ?? ????????." ) );
					pButton->EnableWindow( TRUE );
					return TRUE;
				}
				if( IsPwdValid( pPassword->GetString() ) == FALSE )
				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0030) ) );
//					g_WndMng.OpenMessageBox( _T( "?????? 3~16?? ????, ?????? ?????? ?? ????????." ) );
					pButton->EnableWindow( TRUE );
					return TRUE;
				}

				strcpy( g_Option.m_szAccount, pAccount->GetString() );
#ifdef __THROUGHPORTAL0810
				if( g_Neuz.m_bThroughPortal == FALSE )
#else	// __THROUGHPORTAL0810
				if( g_Neuz.m_bHanGame == FALSE )	// ?????? ?????? ???? ???????? ????, ???? ???? ???????? 
#endif	// __THROUGHPORTAL0810
					g_Neuz.SetAccountInfo( pAccount->GetString(), pPassword->GetString() );
				g_dpCertified.SendCertify();

				g_WndMng.OpenCustomBox( NULL, new CWndConnectingBox );
				break;
			}

		case WIDC_QUIT: // ???? 
		case WTBID_CLOSE:
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

#ifdef __CON_AUTO_LOGIN
void CWndLogin::SetAccountAndPassword( const CString& account, const CString& pass )
{
	CWndEdit* pAccount = (CWndEdit*) GetDlgItem( WIDC_ACCOUNT );
	CWndEdit* pPassword = (CWndEdit*) GetDlgItem( WIDC_PASSWORD );

	if( pAccount && pPassword )
	{
		pAccount->SetString( account );
		pPassword->SetString( pass );
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////
// Select Server
/////////////////////////////////////////////////////////////////////////////////////

CWndSelectServer::CWndSelectServer()
{
	m_atexPannel = NULL;
	m_dwChangeBannerTime = g_tmCurrent+SEC(10);
	m_vecStrBanner.clear();
	SetPutRegInfo( FALSE );
}
CWndSelectServer::~CWndSelectServer()
{
	if( m_atexPannel )
	{
		SAFE_DELETE( m_atexPannel );
	}
}

void CWndSelectServer::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();

	CWndButton* pNext = (CWndButton*)GetDlgItem( WIDC_NEXT );
	pNext->SetDefault( TRUE );

	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
	CRect ReRect = pWndList->GetWindowRect(TRUE);
	ReRect.bottom -= 5;
	pWndList->SetWndRect(ReRect);

	CWndListBox* pWndListMulti	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
	ReRect = pWndListMulti->GetWindowRect(TRUE);
	ReRect.bottom -= 5;
	pWndListMulti->SetWndRect(ReRect);

	pWndListMulti->m_nFontColor = 0xff000000;
	pWndListMulti->m_nSelectColor = 0xff0000ff;

	//pWndList->AddWndStyle( WBS_NODRAWFRAME );
	pWndList->m_nFontColor = 0xff000000;
	pWndList->m_nSelectColor = 0xff0000ff;

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;

	TCHAR szTitle[3][10] = {_T("Server"), _T("Ping"), _T("Max") };
	
	for (CListedServers::Server & server : g_dpCertified.m_servers.GetServers()) {
		if (server.lEnable != 0) {
			CWndListBox::LISTITEM & item = pWndList->AddString(server.lpName);
			item.m_dwData = reinterpret_cast<DWORD>(&server);
		}
	}

	pWndListMulti->ResetContent();

	if (g_Option.m_nSer >= pWndList->GetCount()) {
		g_Option.m_nSer = 0;
		g_Option.m_nMSer = 0;
	}

	if (pWndList->GetCount() > 0) {
		pWndList->SetCurSel(g_Option.m_nSer);
	}

	if (!g_dpCertified.m_servers.GetServers().empty()) {
		CListedServers::Server & server = g_dpCertified.m_servers.GetServers()[g_Option.m_nSer];
		std::span<CListedServers::Channel> channels = server.channels;

		DisplayChannels(*pWndListMulti, channels);

		if (g_Option.m_nMSer >= pWndListMulti->GetCount()) {
			g_Option.m_nMSer = 0;
		}

		if (pWndListMulti->GetCount() != 0) {
			pWndListMulti->SetCurSel(g_Option.m_nMSer);
		}
	}


//	if( ::GetLanguage() == LANG_TWN )
//	{
//		CWndButton* pWndBack = (CWndButton*)GetDlgItem( WIDC_BACK );
//		pWndBack->EnableWindow( FALSE );
//	}
	
	MoveParentCenter();

	if( ::GetLanguage() == LANG_JAP )
	{
		CRect rect2 = m_pWndRoot->GetLayoutRect();
		
		int width = (rect2.right-rect2.left) / 2;
		
		Move( width, m_rectWindow.top );
	}	

	pNext->SetFocus();

/////////////////////////////////////////////////////////////////////////////////////////
	int nCount = 0;

	CScript script;
	if( script.Load(MakePath(DIR_THEME, "TexBannerList.inc" )) )
	{
		int nLang;
		nLang = script.GetNumber();
		do 
		{
			if( nLang == ::GetLanguage() )
			{
				script.GetToken();
				
				nCount = atoi( script.token );
				
				script.GetToken();				
				
				for( int i=0; i<nCount; i++ )
				{
					CString addStr = script.token;
					m_vecStrBanner.push_back( addStr );
					script.GetToken();
				}
				
				if( nCount <= 0 )
				{
					Error( "TexBannerList.inc?? ?????? 0????" );
					return;
				}
				
				break;
			}
			else
				script.GetLastFull();
			
			nLang = script.GetNumber();
		} while( script.tok != FINISHED );		
	}
	
	if( nCount > 0 )
	{
		SAFE_DELETE( m_atexPannel );
		m_atexPannel = new IMAGE;
		LoadImage( MakePath( DIR_THEME, m_vecStrBanner[xRandom(nCount)] ), m_atexPannel );
		AdjustWndBase();	
	}
/////////////////////////////////////////////////////////////////////////////////////////
}
BOOL CWndSelectServer::Process()
{
	if( g_tmCurrent > m_dwChangeBannerTime )
	{
		m_dwChangeBannerTime = g_tmCurrent+SEC(10);

		SAFE_DELETE( m_atexPannel );
		m_atexPannel = new IMAGE;
		LoadImage( MakePath( DIR_THEME, m_vecStrBanner[xRandom(m_vecStrBanner.size())] ), m_atexPannel );
		AdjustWndBase();	
	}
	
	return TRUE;
}
BOOL CWndSelectServer::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	return CWndNeuz::InitDialog( APP_SELECT_SERVER, pWndParent, WBS_KEY, CPoint( 0, 0 ) );
}

void CWndSelectServer::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl;
	
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_COMMER_BANNER2 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	
	if( m_atexPannel ) 
		PaintTexture( pDest, m_atexPannel, pt, size );	
}

void CWndSelectServer::Connected()
{
#if defined(_DEBUG)
	g_Neuz.WaitLoading();
#endif

	g_WndMng.CloseMessageBox();
	g_WndMng.OpenApplet(APP_SELECT_CHAR);
	Destroy();
}

void CWndSelectServer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWndListBox* pWndListServer		= (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
	CWndListBox* pWndListMulti	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );

	if( nChar == VK_UP )	
	{
		DWORD dwIndex = pWndListMulti->GetCurSel();

		if( dwIndex > 0 )
			pWndListMulti->SetCurSel(--dwIndex);
	}
	else
	if( nChar == VK_DOWN )	
	{
		DWORD dwIndex = pWndListMulti->GetCurSel();
		
		if( (int)( dwIndex ) < pWndListMulti->GetCount()-1 )
			pWndListMulti->SetCurSel(++dwIndex);
	}
}

void CWndSelectServer::DisplayChannels(CWndListBox & listBox, const std::span<CListedServers::Channel> & channels) {
	char lpString[256];

	for (CListedServers::Channel & channel : channels) {
		//if (channel.lEnable) {

		long lCount = channel.lCount;
		long lMax = channel.lMax;

		long lBusy = (long)(lMax * 0.8);

		const char * lpStrtmp;
		if (lCount < lBusy)
			lpStrtmp = prj.GetText(TID_GAME_NORMAL);	//"????"
		else if (lCount < lMax)
			lpStrtmp = prj.GetText(TID_GAME_BUSY);		//"????"
		else
			lpStrtmp = prj.GetText(TID_GAME_FULL);

		const auto r = std::format_to_n(lpString, std::size(lpString) - 1, "{}({})", channel.lpName, lpStrtmp);
		*r.out = '\0';
		
		CWndListBox::LISTITEM & item = listBox.AddString(lpString);
		item.m_dwData = reinterpret_cast<DWORD>(&channel);
	}
}

BOOL CWndSelectServer::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_SELCHANGE )
	{
		switch( nID )
		{
			case 11:
				{
					break;
				}
			case 188:
				{
					char lpString[MAX_PATH]	= { 0, };
					char lpStrtmp[32]	= { 0, };
					long lCount	= 0;
					long lMax	= 0;
					CWndListBox* pWndListServer		= (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
					CWndListBox* pWndListMulti	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );

					pWndListMulti->ResetContent();
					CListedServers::Server * pServerDesc	= (CListedServers::Server *) pWndListServer->GetItemData( pWndListServer->GetCurSel() );
					DisplayChannels(*pWndListMulti, pServerDesc->channels);
					
					if( pWndListMulti->GetCount() )
						pWndListMulti->SetCurSel( 0 );
					break;
				}
		}
	}
	else
	switch(nID)
	{
		case 10000: // close msg
		case WIDC_BACK: // Back
			g_WndMng.OpenApplet(APP_LOGIN);
			Destroy();
			g_dpCertified.DeleteDPObject();
			break;
		case WIDC_NEXT: // accept
			{
				CWndListBox* pWnd	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
				if( pWnd->GetCount() <= 0 )
					break;

				CListedServers::Channel * pDesc = reinterpret_cast<CListedServers::Channel *>(pWnd->GetItemData( pWnd->GetCurSel() ));
				if( pDesc )
				{
					if( !( g_Neuz.m_cbAccountFlag & ACCOUNT_FLAG_SCHOOLEVENT ) && pDesc->lCount > pDesc->lMax )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0041) ) );
						break;
					}

					if (::GetLanguage() != LANG_THA) {
						if (pDesc->b18 && !(g_Neuz.m_cbAccountFlag & ACCOUNT_FLAG_18)) {
							g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0058)));   // 18?????? ???????? ?????? ?? ????????.
							break;
						}
					}
				}
			}

			if( FALSE == g_dpCertified.IsConnected() )
			{
				CNetwork::GetInstance().OnEvent( LOGIN_CONNECT_STEP_ERROR );

				g_WndMng.OpenApplet(APP_LOGIN);
				Destroy();
				g_dpCertified.DeleteDPObject();
				g_dpLoginClient.DeleteDPObject();	// 2004^04^19
				break;
			}
			g_WndMng.OpenCustomBox( NULL, new CWndConnectingBox );

			CWndListBox* pWndList	= (CWndListBox*)GetDlgItem( WIDC_CONTROL0 );
			CListedServers::Server * pTServerDesc	= (CListedServers::Server *)pWndList->GetItemData( pWndList->GetCurSel() );

			g_Option.m_nSer = pWndList->GetCurSel();
			g_Neuz.m_dwSys	= pTServerDesc->dwID;
			LPCSTR lpAddr	= pTServerDesc->lpAddr;
			pWndList	= (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
			CListedServers::Channel * pServerDesc = (CListedServers::Channel *)pWndList->GetItemData(pWndList->GetCurSel());
			g_Option.m_nMSer = pWndList->GetCurSel();
			g_Neuz.m_uIdofMulti		= pServerDesc->dwID;
			g_Neuz.m_b18Server		= pServerDesc->b18;

			std::span<CListedServers::Server> servers = g_dpCertified.m_servers.GetServers();

			const auto itServer = std::ranges::find_if(servers,
				[&](const CListedServers::Server & server) {
					return std::ranges::any_of(server.channels,
						[&](const CListedServers::Channel & channel) {return &channel == pServerDesc; });
				}
			);
			
			if (itServer != servers.end()) {
				if (itServer->dwID != g_Neuz.m_dwSys) {

					CWndListBox * pWndListBox = (CWndListBox *)GetDlgItem(WIDC_CONTROL0);
					for (int i = 0; i < pWndListBox->GetCount(); i++) {
						CListedServers::Server * ptr = (CListedServers::Server *)pWndListBox->GetItemData(i);
						if (ptr == &*itServer) {
							pWndListBox->SetCurSel(i);
							g_Option.m_nSer = i;
							g_Neuz.m_dwSys = ptr->dwID;
							lpAddr = ptr->lpAddr;
							break;
						}
					}
				}
			}


#		ifdef __CRC
			if( !g_dpLoginClient.ConnectToServer( lpAddr, PN_LOGINSRVR, TRUE, CSock::crcWrite ) )
#		else	__CRC
			if( !g_dpLoginClient.ConnectToServer( lpAddr, PN_LOGINSRVR, TRUE ) )
#		endif	// __CRC
			{
				// Can't connect to server
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0043) ) );
//				g_WndMng.OpenMessageBox( _T( "?????? ?? ????????. ???????? ?????? ????????????." ) );
				CNetwork::GetInstance().OnEvent( LOGIN_CONNECT_FAIL );
				break;
			}
			CNetwork::GetInstance().OnEvent( LOGIN_CONNECTED );

			g_dpLoginClient.QueryTickCount();
	#if __NPROTECT_VER >= 4
			g_Neuz.m_loginSI.dwID = pServerDesc->dwID;
		#ifdef __GPAUTH_01
			g_Neuz.m_loginSI.pszAccount		= g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount;
		#else	// __GPAUTH_01
			g_Neuz.m_loginSI.pszAccount  = g_Neuz.m_szAccount;
		#endif	// __GPAUTH_01
			g_Neuz.m_loginSI.pszPassword = g_Neuz.m_szPassword;
			g_Neuz.m_loginSI.nCount = 1;
	#else
		#ifdef __GPAUTH_01
			g_dpLoginClient.SendGetPlayerList( pServerDesc->dwID, g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_szPassword );
		#else	// __GPAUTH_01
			g_dpLoginClient.SendGetPlayerList( pServerDesc->dwID, g_Neuz.m_szAccount, g_Neuz.m_szPassword );
		#endif	// __GPAUTH_01
	#endif // __NPROTECT_VER 
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

/////////////////////////////////////////////////////////////////////////////////////
// Delete Character
/////////////////////////////////////////////////////////////////////////////////////

void CWndDeleteChar::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
#ifdef __THROUGHPORTAL0810
	if( ( g_Neuz.m_bThroughPortal && GetLanguage() != LANG_TWN && ::GetLanguage() != LANG_KOR )
		|| ( GetLanguage() == LANG_ENG && GetSubLanguage() == LANG_SUB_USA )
		|| ( GetLanguage() == LANG_ENG && GetSubLanguage() == LANG_SUB_IND )
		|| ( GetLanguage() == LANG_VTN )
		)
#else	// __THROUGHPORTAL0810
	if( g_Neuz.m_bHanGame )
#endif	// __THROUGHPORTAL0810
	{
		CWndEdit *WndEdit   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		WndEdit->SetVisible( FALSE );
		WndEdit->EnableWindow( FALSE );
	}
	
	

	CWndNeuz::AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
}

BOOL CWndDeleteChar::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( APP_DELETE_CHAR, nullptr, WBS_MODAL );
	CWndEdit *WndEdit   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( WndEdit )
	{
		WndEdit->AddWndStyle( EBS_PASSWORD|EBS_AUTOHSCROLL );
		WndEdit->SetFocus();
	}

	MoveParentCenter();

#ifdef __THROUGHPORTAL0810
	if( ( g_Neuz.m_bThroughPortal && GetLanguage() != LANG_TWN && ::GetLanguage() != LANG_KOR )
		|| ( GetLanguage() == LANG_ENG && GetSubLanguage() == LANG_SUB_USA )
		|| ( GetLanguage() == LANG_ENG && GetSubLanguage() == LANG_SUB_IND )
		|| ( ::GetLanguage() == LANG_VTN )
		)
	{
		CWndStatic *pWnd1	= (CWndStatic*)GetDlgItem( WIDC_CONTROL1 );
		CWndStatic *pWnd2	= (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		CWndSelectChar* pWnd	= (CWndSelectChar *)g_WndMng.GetWndBase( APP_SELECT_CHAR );
		pWnd1->SetTitle( g_Neuz.m_apPlayer[pWnd->m_nSelectCharacter]->GetName() );
		pWnd2->SetTitle( GETTEXT(TID_HANGAME_COMFORMDELETE) );
	}
	/*
	else if( GetLanguage() == LANG_JAP )
	{
		CWndStatic *WndEdit   = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		CRect rect = WndEdit->GetWindowRect();
		WndEdit->Move(rect.left+24, rect.top+30 );
	}
	*/
#else	// __THROUGHPORTAL0810
	if( ::GetLanguage() == LANG_JAP )
	{
		if( g_Neuz.m_bHanGame )
		{
			CWndStatic *pWnd1	= (CWndStatic*)GetDlgItem( WIDC_CONTROL1 );
			CWndStatic *pWnd2	= (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			CWndSelectChar* pWnd	= (CWndSelectChar *)g_WndMng.GetWndBase( APP_SELECT_CHAR );
			pWnd1->SetTitle( g_Neuz.m_apPlayer[pWnd->m_nSelectCharacter]->GetName() );
			pWnd2->SetTitle( GETTEXT(TID_HANGAME_COMFORMDELETE) );
		}
		else
		{
			CWndStatic *WndEdit   = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			CRect rect = WndEdit->GetWindowRect();
			WndEdit->Move(rect.left+24, rect.top+30 );
		}
	}
#endif	// __THROUGHPORTAL0810

	return TRUE;
}

void CWndDeleteChar::DeletePlayer( int nSelect, LPCTSTR szNo )
{
	g_dpLoginClient.SendDeletePlayer( nSelect, szNo );
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_OK );
	pWndButton->EnableWindow( FALSE );
	pWndButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
	pWndButton->EnableWindow( FALSE );								
}

BOOL CWndDeleteChar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CWndSelectChar* pWnd = (CWndSelectChar *)g_WndMng.GetWndBase( APP_SELECT_CHAR );
		if( pWnd == NULL )
			return CWndNeuz::OnChildNotify( message, nID, pLResult ); 

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		if( pWndEdit == NULL )
			return CWndNeuz::OnChildNotify( message, nID, pLResult ); 

		BOOL bOK = FALSE;
		LPCTSTR szNo = pWndEdit->GetString();
#ifdef __THROUGHPORTAL0810
		if( ( g_Neuz.m_bThroughPortal && GetLanguage() != LANG_TWN && ::GetLanguage() != LANG_KOR )
			|| ( GetLanguage() == LANG_ENG && GetSubLanguage() == LANG_SUB_USA )
			|| ( GetLanguage() == LANG_ENG && GetSubLanguage() == LANG_SUB_IND )
			|| ( ::GetLanguage() == LANG_VTN )
			)
			szNo = g_Neuz.m_szPassword;
#endif	// __THROUGHPORTAL0810
		switch( ::GetLanguage() )
		{
		case LANG_KOR:
#ifdef __DELETE_CHAR_CHANGE_KEY_VALUE	// 100304_mirchang ???????????? 2?????????? ????(????, ???????? i-PIN)
			if( strlen(szNo) == 4 )
#else // __DELETE_CHAR_CHANGE_KEY_VALUE
			if( strlen(szNo) == 7 )
#endif // __DELETE_CHAR_CHANGE_KEY_VALUE
			{
				BOOL bisdigit = TRUE;
#ifdef __DELETE_CHAR_CHANGE_KEY_VALUE
				for( int i = 0 ; i < 4 ; i++ )
#else // __DELETE_CHAR_CHANGE_KEY_VALUE
				for( int i = 0 ; i < 7 ; i++ )
#endif // __DELETE_CHAR_CHANGE_KEY_VALUE
				{
					if( (isdigit2( szNo[i] ) == FALSE) )
					{
						bisdigit = FALSE;
						break;											
					}
				}

				if( bisdigit )
				{
					DeletePlayer( pWnd->m_nSelectCharacter, szNo );
					bOK = TRUE;
				}
			}
			break;
		default:
#ifndef __THROUGHPORTAL0810
			if( g_Neuz.m_bHanGame && GetLanguage() != LANG_TWN )
				szNo = g_Neuz.m_szPassword;
#endif	// __THROUGHPORTAL0810
			if( 0 < strlen( szNo ) && strlen( szNo ) < 64 )
			{
				DeletePlayer( pWnd->m_nSelectCharacter, szNo );
				bOK = TRUE;
			}
			break;
		}

		if( bOK == FALSE )
		{
			pWndEdit->SetString( "" );
#ifdef __THROUGHPORTAL0810
			if( g_Neuz.m_bThroughPortal == FALSE || GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_KOR )
#else	// __THROUGHPORTAL0810
			if( g_Neuz.m_bHanGame == FALSE )
#endif	// __THROUGHPORTAL0810
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0044) ) ); // "???????? ???? 7?????? ?????? ??????. ???? ????????????"
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////
// Select Character
/////////////////////////////////////////////////////////////////////////////////////

int CWndSelectChar::m_nSelectCharacter = 0;

CWndSelectChar::CWndSelectChar()
{
	m_pWndDeleteChar = NULL;
	m_pWnd2ndPassword = NULL;
	ZeroMemory( m_pBipedMesh, sizeof( m_pBipedMesh ) );	
	m_dwMotion[ 0 ] = MTI_SITSTAND;
	m_dwMotion[ 1 ] = MTI_SITSTAND;
	m_dwMotion[ 2 ] = MTI_SITSTAND;
	SetPutRegInfo( FALSE );
	m_CreateApply = TRUE; //???????? ???? ???? ???? ?????? ???? ????.
}
CWndSelectChar::~CWndSelectChar()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		SAFE_DELETE( m_pBipedMesh[ i ] );
	}
	SAFE_DELETE( m_pWndDeleteChar );
	SAFE_DELETE( m_pWnd2ndPassword );
}

void CWndSelectChar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( !g_Neuz.m_timerConnect.Over() )
		return;

	int nSelectCharBuf = m_nSelectCharacter;
	if( nChar == VK_LEFT )
	{
		--nSelectCharBuf;
		if( 0 > nSelectCharBuf )
		{
			nSelectCharBuf = MAX_CHARACTER_LIST - 1;
		}
		SelectCharacter( nSelectCharBuf );
	}
	else if( nChar == VK_RIGHT )
	{
		++nSelectCharBuf;
		if( nSelectCharBuf >= MAX_CHARACTER_LIST )
		{
			nSelectCharBuf = 0;
		}
		SelectCharacter( nSelectCharBuf );
	}
}

void CWndSelectChar::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndDeleteChar )
		SAFE_DELETE( m_pWndDeleteChar );
	if( pWndChild == m_pWnd2ndPassword )
		SAFE_DELETE( m_pWnd2ndPassword );
}
HRESULT CWndSelectChar::InitDeviceObjects()
{
	CWndBase::InitDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->InitDeviceObjects( m_pApp->m_pd3dDevice );
	}

	return S_OK;
}
HRESULT CWndSelectChar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->RestoreDeviceObjects();
	}
	return S_OK;
}
HRESULT CWndSelectChar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->InvalidateDeviceObjects();
	}
	return S_OK;
}
HRESULT CWndSelectChar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
			m_pBipedMesh[ i ]->DeleteDeviceObjects();
	}
	return S_OK;
}
BOOL CWndSelectChar::Process()
{
	/*
	 *	

	ANILOOP_1PLAY			(0x00000001)			// ???? ???????? ??.
	ANILOOP_CONT			(0x00000002)			// ???? ???????? ?????? ????????
	ANILOOP_LOOP			(0x00000004)			// ????
	ANILOOP_RETURN			(0x00000008)			// ???? - ???????? ????.
	ANILOOP_BACK			(0x00000010)			// ?????? ????. - ???????? ????

	 */
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		CRect rect = m_aRect[ i ];
		CModelObject* pModel = (CModelObject*)m_pBipedMesh[ i ];
		CMover* pMover = g_Neuz.m_apPlayer[ i ];
	
		if( g_Neuz.m_apPlayer[i] != NULL && pModel )
		{
			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			if( m_nSelectCharacter == i )
			{
				// ?????? ????????? ???????? MTI_STAND?? ???? 
				if( m_dwMotion[ i ] == MTI_GETUP )
				{
					if( pModel->IsEndFrame() && pModel->m_nLoop == ANILOOP_1PLAY )
					{
						SetMotion( pModel, nMover, MTI_STAND, ANILOOP_LOOP, 0 );
						m_dwMotion[ i ] = MTI_STAND;
					}
				}
			}
			else
			{
				// ???? ?????? ??????? ???????? ?????? ??????. MTI_SIT???? ????
				if( m_dwMotion[ i ] != MTI_SITSTAND )
				{
					if( pModel->IsEndFrame() && pModel->m_nLoop == ANILOOP_1PLAY )
					{
						SetMotion( pModel, nMover, MTI_SITSTAND, ANILOOP_LOOP, 0 );
						m_dwMotion[ i ] = MTI_SITSTAND;
					}
					else
					if( m_dwMotion[ i ] != MTI_SIT )
					{
						SetMotion( pModel, nMover, MTI_SIT, ANILOOP_1PLAY, 0 );
						m_dwMotion[ i ] = MTI_SIT;
					}
				}
				// ?????? ??????? ???????? MTI_SITSTAND?? ???? 
			}
			pModel->FrameMove();
		}
	}

#ifndef _DEBUG			
	if( g_Neuz.m_dwTimeOutDis < GetTickCount() )
	{
		g_Neuz.m_dwTimeOutDis = 0xffffffff;
		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		g_WndMng.OpenMessageBoxUpper(  _T( prj.GetText(TID_DIAG_0043) ) );

		m_bDisconnect = TRUE;
	}

	if( g_WndMng.m_pWndMessageBoxUpper == NULL && m_bDisconnect )
	{
		Destroy();
		g_WndMng.CloseMessageBox();
		g_dpCertified.SendCertify();
		g_WndMng.OpenApplet(APP_LOGIN);
		CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_LOGIN );
		return 0;	
	}
#endif //_DEBUG			
	
	return CWndNeuz::Process();
}

void CWndSelectChar::OnDraw( C2DRender* p2DRender )
{
	CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
	CWndButton* pWndCreate = (CWndButton*)GetDlgItem( WIDC_CREATE );
	CWndButton* pWndDelete = (CWndButton*)GetDlgItem( WIDC_DELETE );

	CRect rect;
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		rect = m_aRect[ i ];

		if( g_Neuz.m_apPlayer[i] != NULL )
		{
			if( g_WndMng.GetWndBase( APP_2ND_PASSWORD_NUMBERPAD ) == NULL )
			{
				POINT point = GetMousePoint();
				if( m_aRect[ i ].PtInRect( point ) )
				{
					CRect rectHittest = m_aRect[ i ];
					CPoint point2 = point;
					ClientToScreen( &point2 );
					ClientToScreen( rectHittest );
					WndMgr::CTooltipBuilder::PutToolTip_Character(i, point2, rectHittest);
				}
			}
			if( m_nSelectCharacter == i )
			{
				CRect rectTemp = rect;

				rectTemp.top += 5;
				rectTemp.bottom -= 5;

				rectTemp.left += i;
				rectTemp.right += i;
				
				p2DRender->RenderFillRect(rectTemp, D3DCOLOR_ARGB( 20, 80, 250, 80 ) );
					
				p2DRender->TextOut( rect.left, rect.bottom + 10, g_Neuz.m_apPlayer[i]->GetName(), 0xff6060ff );
				p2DRender->TextOut( rect.left + 1, rect.bottom + 10, g_Neuz.m_apPlayer[i]->GetName(), 0xff6060ff );
			}
			else
				p2DRender->TextOut( rect.left, rect.bottom + 10, g_Neuz.m_apPlayer[i]->GetName(), 0xff505050 );

			CModelObject* pModel = (CModelObject*)m_pBipedMesh[ i ];
			LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

			pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

			//rect = GetClientRect();

			// ?????? ???? 
			D3DVIEWPORT9 viewport;

			viewport.X      = p2DRender->m_ptOrigin.x + rect.left;
			viewport.Y      = p2DRender->m_ptOrigin.y + rect.top;
			viewport.Width  = rect.Width();
			viewport.Height = rect.Height();
			viewport.MinZ   = 0.0f;
			viewport.MaxZ   = 1.0f;
			pd3dDevice->SetViewport(&viewport);
			pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

//			POINT point = GetMousePoint();

			//point.x -= 280;
			//point.y -= 15;

			CRect rectViewport( 0, 0, viewport.Width, viewport.Height );

			// ???????? 
			D3DXMATRIX matProj;
			D3DXMatrixIdentity( &matProj );
			FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
			D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
			pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

			// ?????? 
			D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vecPos(  0.0f, 0.5f, -3.5f );

			D3DXMATRIX matView = D3DXR::LookAtLH010(vecPos, vecLookAt);

			pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

			// ???? 
			D3DXMATRIXA16 matWorld;
			D3DXMATRIXA16 matScale;
			D3DXMATRIXA16 matRot;
			D3DXMATRIXA16 matTrans;

			// ?????? 
			D3DXMatrixIdentity(&matScale);
			D3DXMatrixIdentity(&matRot);
			D3DXMatrixIdentity(&matTrans);
			D3DXMatrixIdentity(&matWorld);
			
			D3DXMatrixScaling(&matScale,1.6f,1.6f,1.6f);
			D3DXMatrixTranslation(&matTrans,0.0f,-1.15f,0.0f);

			matWorld = matWorld * matScale * matRot * matTrans;
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

			// ?????? 
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
			::SetLight( FALSE );
			::SetFog( FALSE );
			SetDiffuse( 1.0f, 1.0f, 1.0f );
			SetAmbient( 1.0f, 1.0f, 1.0f );
			
			O3D_ELEMENT *pElem = pModel->GetParts( PARTS_RIDE );
			if( pElem && pElem->m_pObject3D )
				pModel->TakeOffParts( PARTS_RIDE );
			
			pElem = pModel->GetParts( PARTS_HAIR );

			if( pElem && pElem->m_pObject3D )
			{
				pElem->m_pObject3D->m_fAmbient[0] = g_Neuz.m_apPlayer[i]->m_fHairColorR;
				pElem->m_pObject3D->m_fAmbient[1] = g_Neuz.m_apPlayer[i]->m_fHairColorG;
				pElem->m_pObject3D->m_fAmbient[2] = g_Neuz.m_apPlayer[i]->m_fHairColorB;
			}
			
			//pModel->FrameMove();
			D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
			g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
			pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
			::SetTransformView( matView );
			::SetTransformProj( matProj );

			g_Neuz.m_apPlayer[i]->OverCoatItemRenderCheck(pModel);
			
			// ?????? ???????? ?????????????????  // ?????? ????????
			DWORD dwId	= g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_CAP].dwId;
			ItemProp* pItemProp = NULL;

			if( dwId != NULL_ID )
			{
				O3D_ELEMENT* pElement = NULL;
				pItemProp	= prj.GetItemProp( dwId );

				if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
				{
					pElement = pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
				}

				// ?????????? ?????????? ???????????????? ?????? ???? ?????? ???????? ??????
				dwId	= g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwId;
				if( dwId != NULL_ID )
				{
					if( !(g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								pModel->SetEffect(PARTS_HAIR, XE_HIDE );
							
							pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
						else
						{
							if( pElement )
								pElement->m_nEffect &= ~XE_HIDE;
						}
					}
				}
			}
			else
			{
				// ?????????? ?????????? ???????????????? ?????? ???? ?????? ???????? ??????
				dwId	= g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].dwId;
				if( dwId != NULL_ID )
				{
					if( !(g_Neuz.m_apPlayer[i]->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{						
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								pModel->SetEffect(PARTS_HAIR, XE_HIDE );

							pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
					}
				}
			}

#ifdef __YENV
			SetLightVec( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );
#endif //__YENV
			
			pModel->Render( p2DRender->m_pd3dDevice, &matWorld );

			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		
			
			viewport.X      = p2DRender->m_ptOrigin.x;// + 5;
			viewport.Y      = p2DRender->m_ptOrigin.y;// + 5;
			viewport.Width  = p2DRender->m_clipRect.Width();
			viewport.Height = p2DRender->m_clipRect.Height();
			viewport.MinZ   = 0.0f;
			viewport.MaxZ   = 1.0f;
			pd3dDevice->SetViewport(&viewport);

		}
		else
		{
			if( m_nSelectCharacter == i )
				p2DRender->TextOut( rect.left, rect.bottom + 10, prj.GetText( TID_GAME_WND_SELECT_CHARACTER_EMPTY ), 0xff6060ff );
			else
				p2DRender->TextOut( rect.left, rect.bottom + 10, prj.GetText( TID_GAME_WND_SELECT_CHARACTER_EMPTY ), 0xff505050 );
		}			

		if( m_nSelectCharacter == i )
		{
			if( g_Neuz.m_apPlayer[i]  )
			{
				pWndCreate->EnableWindow( FALSE );
				pWndAccept->EnableWindow( TRUE );
				pWndDelete->EnableWindow( TRUE );
			}
			else
			{
				pWndCreate->EnableWindow( TRUE );
				pWndAccept->EnableWindow( FALSE );
				pWndDelete->EnableWindow( FALSE );
			}
		}
	}
}
void CWndSelectChar::DeleteCharacter()
{
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_pBipedMesh[ i ] )
		{
			m_pBipedMesh[ i ]->InvalidateDeviceObjects();
			m_pBipedMesh[ i ]->DeleteDeviceObjects();
			SAFE_DELETE( m_pBipedMesh[ i ] );
		}
	}
}
void CWndSelectChar::UpdateCharacter()
{
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		CMover* pMover = g_Neuz.m_apPlayer[i];
		if( pMover )
		{
			// ????, ???????? ???? ?????? ???????? ???? 
			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			m_pBipedMesh[ i ] = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
			if( i == m_nSelectCharacter )
			{
				prj.m_modelMng.LoadMotion( m_pBipedMesh[ i ],  OT_MOVER, nMover, MTI_STAND );
				m_dwMotion[ i ] = MTI_STAND;
			}
			else
			{
				prj.m_modelMng.LoadMotion( m_pBipedMesh[ i ],  OT_MOVER, nMover, MTI_SITSTAND );
				m_dwMotion[ i ] = MTI_SITSTAND;
			}
			CMover::UpdateParts( pMover->GetSex(), pMover->m_dwSkinSet, pMover->m_dwFace, pMover->m_dwHairMesh, pMover->m_dwHeadMesh, pMover->m_aEquipInfo, m_pBipedMesh[ i ], NULL/*&pMover->m_Inventory*/ );
		}
	}
}
void CWndSelectChar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();

	LPWNDCTRL lpText1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpText2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpText3 = GetWndCtrl( WIDC_CUSTOM3 );
	
//	m_wndText1.Create( 0, lpText1->rect, this, WIDC_CUSTOM1 );
//	m_wndText2.Create( 0, lpText2->rect, this, WIDC_CUSTOM2 );
//	m_wndText3.Create( 0, lpText3->rect, this, WIDC_CUSTOM3 );
//	m_wndText1.HideCaret();
//	m_wndText2.HideCaret();
///	m_wndText3.HideCaret();
	/*
	CWndText* pWndText1 = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CWndText* pWndText2 = (CWndText*)GetDlgItem( WIDC_TEXT2 );
	CWndText* pWndText3 = (CWndText*)GetDlgItem( WIDC_TEXT3 );
	
	pWndText1->DelWndStyle( WBS_VSCROLL );
	pWndText2->DelWndStyle( WBS_VSCROLL );
	pWndText3->DelWndStyle( WBS_VSCROLL );
	*/
	
	//rect.SetRect( 108, 349, 180, 370 );
	//m_wndBack  .Create( _T( "Back"   ), 0, rect, this, 100 ); rect.OffsetRect( 83, 0 );
	//m_wndCreate.Create( _T( "Create" ), 0, rect, this, 101 ); rect.OffsetRect( 83, 0 );
	//m_wndDelete.Create( _T( "Delete" ), 0, rect, this, 102 ); rect.OffsetRect( 83, 0 );
	//m_wndAccept.Create( _T( "Accept" ), 0, rect, this, 103 );
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_DELETE );
//WndText* pWndText = (CWndButton*)GetDlgItem( WIDC_EDIT1 );

//	pWndButton->EnableWindow( FALSE );

	CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
	pWndAccept->SetDefault( TRUE );
	CWndButton* pWndBack = (CWndButton*)GetDlgItem( WIDC_BACK );
	pWndBack->SetFocus( );
	
//	if( ::GetLanguage() == LANG_TWN )
//	{
//		pWndBack->EnableWindow( FALSE );
//	}

	m_bDisconnect = FALSE;

	rect = CRect( 16, 16, 174, 254 );
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		m_aRect[ i ] = rect;
		rect.OffsetRect( 170, 0 );
	}
	
	//???????? ???? ???? ???? ?????? ???? ????. 2007/01/02 ~ 2007/01/11 ???? ????.
#if defined( __MAINSERVER )
/*	if(g_Option.m_nSer != 1)
	{
		CTime time = CTime::GetCurrentTime();
		int year, month, day;
		year = time.GetYear();
		month = time.GetMonth();
		day = time.GetDay();
		if(year == 2007 && month == 1)
		{
			if(day > 1 && day < 12)
				m_CreateApply = FALSE;
		}
	}
*/
#endif //( __MAINSERVER )
	
	MoveParentCenter();
}

BOOL CWndSelectChar::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	CRect rect = m_pWndRoot->MakeCenterRect( 590, 400 );
	SetTitle( _T( "Select Character" ) );
	return CWndNeuz::InitDialog( APP_SELECT_CHAR, pWndParent, WBS_KEY, CPoint( 0, 0 ) );
}

void CWndSelectChar::Connected()
{
	if( m_nSelectCharacter < 0 || m_nSelectCharacter >= 5 )
	{
		LPCTSTR szErr = Error( "CWndSelectChar::Connected : ???????? %d", m_nSelectCharacter );
		//ADDERRORMSG( szErr );
		int *p = NULL;
		*p = 1;
	}
#ifdef __USE_IDPLAYER0519
	#ifdef __GPAUTH_01
	g_dpLoginClient.SendPreJoin( g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
	#else	// __GPAUTH_01
	g_dpLoginClient.SendPreJoin( g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
	#endif	// __GPAUTH_01
#else	// __USE_IDPLAYER0519
	#ifdef __GPAUTH_01
	g_dpLoginClient.SendPreJoin( g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, g_Neuz.m_apPlayer[m_nSelectCharacter]->GetName(), m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
	#else	// __GPAUTH_01
	g_dpLoginClient.SendPreJoin( g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, g_Neuz.m_apPlayer[m_nSelectCharacter]->GetName(), m_nSelectCharacter, g_Neuz.m_n2ndPasswordNumber );
	#endif	// __GPAUTH_01
#endif	// __USE_IDPLAYER0519

	CNetwork::GetInstance().OnEvent( LOGIN_REQ_PREJOIN );

	// ata2k - (1)???? ????
	if( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 30 );
	else
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 15 );
}

BOOL CWndSelectChar::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( !g_Neuz.m_timerConnect.Over() )
		return CWndNeuz::OnChildNotify( message, nID, pLResult );

	switch(nID)
	{
		case 10000: // close msg
		case WIDC_BACK: // Back
			{
#ifdef __CERTIFIER_COLLECTING_SYSTEM
				DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM
				g_dpLoginClient.DeleteDPObject();
				Sleep( 1000 );	// ????.
#			ifdef __CRC
				if( !g_dpCertified.ConnectToServer( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE, CSock::crcWrite ) )
#			else	// __CRC
				if( !g_dpCertified.ConnectToServer( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE ) )
#			endif	// __CRC
				{
					// Can't connect to server
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0043) ) );
//					g_WndMng.OpenMessageBox( _T( "?????? ?? ????????. ???????? ?????? ????????????." ) );
					CNetwork::GetInstance().OnEvent( CERT_CONNECT_FAIL );
					break;
				}
				CNetwork::GetInstance().OnEvent( CERT_CONNECTED );
				Destroy();
				g_dpCertified.SendCertify();

				g_WndMng.OpenApplet(APP_LOGIN);
				CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_LOGIN );
//				pWndBase->SetVisible( FALSE );
				break;
			}
		case WIDC_CREATE: // Create
			if( m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[ m_nSelectCharacter ] == NULL )
			{
				if(m_CreateApply) //???????? ???? ???? ???? ?????? ???? ????.
				{	
					u_short uSlot	= (u_short)m_nSelectCharacter;
					Destroy();
					g_WndMng.OpenApplet(APP_CREATE_CHAR);
					CWndCreateChar* pWndCreateChar	= (CWndCreateChar*)g_WndMng.GetWndBase( APP_CREATE_CHAR );
					if( pWndCreateChar ) {
						pWndCreateChar->m_Player.m_uSlot	= uSlot;
					}
				}
				else
				{
//					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_CREATECHAR_WARNNING) ) ); //Message : ???????? ???? ???? ???? ?????? ???? ????.
				}
			}
			break;
		case WIDC_DELETE: // Delete
			if( m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[ m_nSelectCharacter ] )
			{
				SAFE_DELETE( m_pWndDeleteChar );
				m_pWndDeleteChar = new CWndDeleteChar;
				m_pWndDeleteChar->Initialize( this, APP_DELETE_CHAR );
			}
			break;
		case WIDC_ACCEPT: // Accept
			{
				switch( m_nSelectCharacter )
				{
					case 0:
						g_Option.m_pGuide = &(g_Option.m_nGuide1);
						break;
					case 1:
						g_Option.m_pGuide = &(g_Option.m_nGuide2);
						break;
					case 2:
						g_Option.m_pGuide = &(g_Option.m_nGuide3);
						break;
					default:
						Error( "?????? ?????? ?????? ??????!! : %d", m_nSelectCharacter );
						return FALSE;
				}
				CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
				pWndAccept->EnableWindow( FALSE );
			}
#ifdef __CERTIFIER_COLLECTING_SYSTEM
			DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM
			
			if( g_Neuz.m_nCharacterBlock[m_nSelectCharacter] == 0 )
			{
				g_WndMng.OpenCustomBox( NULL, new CWndCharBlockBox );
			}
			else
			{
				if( FALSE == g_dpLoginClient.IsConnected() )
				{
					CNetwork::GetInstance().OnEvent( CACHE_CONNECT_STEP_ERROR );

					g_WndMng.OpenApplet(APP_LOGIN);
					Destroy();
					g_dpLoginClient.DeleteDPObject();
					break;
				}

				if( ::IsUse2ndPassWord() == TRUE )
				{
					if( m_pWnd2ndPassword )
						SAFE_DELETE( m_pWnd2ndPassword );
					m_pWnd2ndPassword = new CWnd2ndPassword();
					m_pWnd2ndPassword->Initialize( this, APP_2ND_PASSWORD_NUMBERPAD );
					m_pWnd2ndPassword->SetInformation( g_dpLoginClient.GetNumberPad(), m_nSelectCharacter );
#ifdef __CON_AUTO_LOGIN
					for( int i = 0; i < 4; ++i )
						m_pWnd2ndPassword->InsertPassword( g_Console._nPasswordFigure[ i ] );
					m_pWnd2ndPassword->OnChildNotify( 0, WIDC_BUTTON_OK, 0 );
#endif // __CON_AUTO_LOGIN
				}
				else
				{
					g_WndMng.OpenCustomBox( _T( prj.GetText(TID_DIAG_0064) ), new CWndConnectingBox );
					//g_WndMng.OpenCustomBox( _T( "????????????. ?????? ?????? ????????." ), new CWndConnectingBox );

					if( g_DPlay.Connect( g_Neuz.m_lpCacheAddr, g_Neuz.m_uCachePort ) )
					{						
						CNetwork::GetInstance().OnEvent( CACHE_CONNECTED );
						if( m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[m_nSelectCharacter] )
						{
							g_Neuz.m_dwTempMessage = 1;
							g_Neuz.m_timerConnect.Set( SEC( 1 ) );
						}
					}
					else
					{
						CNetwork::GetInstance().OnEvent( CACHE_CONNECT_FAIL );
						TRACE( _T( "Can't connect to server. : %s \n" ), g_Neuz.m_lpCacheAddr );
					}
				}
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndSelectChar::SetMotion( CModelObject* pModel, DWORD dwIndex, DWORD dwMotion, int nLoop, DWORD dwOption )
{
	//CModelObject* pModel = (CModelObject*)pModel;
	DWORD	dwOrigMotion = dwMotion;
	/*
static DWORD m_dwOrigMotion = MTI_STAND;
	if( dwMotion == m_dwOrigMotion )	// ???? ?????? ?????? ??????...
	{
		if( nLoop == ANILOOP_LOOP )		return FALSE;	// ???????? ???? ?? ????
		if( pModel->m_bEndFrame == FALSE )		// ???? ?????????????????? 
			return FALSE;						// ????.
		if( pModel->m_bEndFrame && nLoop == ANILOOP_CONT )	// ???????????? ?????????? ?????????? ?????? ?????????? ????
			return FALSE;
	}
	*/
	prj.m_modelMng.LoadMotion( pModel, OT_MOVER, dwIndex, dwMotion );
	//m_dwOrigMotion = dwMotion;

	pModel->m_bEndFrame = FALSE;
	pModel->SetLoop( nLoop );
	if( dwOption & MOP_NO_TRANS ) pModel->SetMotionBlending( FALSE );
	else pModel->SetMotionBlending( TRUE );
	
	return TRUE;
}
void CWndSelectChar::SelectCharacter( int i )
{
	if( m_nSelectCharacter != i )
	{
		m_nSelectCharacter = i;
		CMover* pMover = g_Neuz.m_apPlayer[ i ];
		if( pMover )
		{
			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			CModelObject* pModel = (CModelObject*)m_pBipedMesh[ m_nSelectCharacter ];
			if( pModel )
				SetMotion( pModel, nMover, MTI_GETUP, ANILOOP_1PLAY, 0 );
			m_dwMotion[ i ] = MTI_GETUP;
			//pModel->SetMotion( MTI_SIT, ANILOOP_1PLAY ); // idle2 ???? 

			if( ::GetLanguage() == LANG_JAP && g_Option.m_bVoice )
			{
				float fVolume = 0;

				fVolume = g_Option.m_fEffectVolume;
				g_SoundMng.m_nSoundVolume = 0;
				
				if( pMover->GetSex() == SEX_MALE )
					PLAYSND( "VocM-CharaChoice.wav" );
				else
					PLAYSND( "VocF-CharaChoice.wav" );
				g_SoundMng.m_nSoundVolume = (int)( -(1.0f-fVolume)*5000 );
			}
		}
	}
}
void CWndSelectChar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( !g_Neuz.m_timerConnect.Over() )
		return;
		
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( /*m_pBipedMesh[ i ] &&*/ m_aRect[ i ].PtInRect( point ) )
		{
			if( g_Neuz.m_nCharacterBlock[i] == 0 )
			{
				g_WndMng.OpenCustomBox( NULL, new CWndCharBlockBox );
					
			}
			else
			{
				SelectCharacter( i );
			}
		}
	}
}
//pMover->SetMotion( MTI_STAND );



/////////////////////////////////////////////////////////////////////////////////////
// Select Character
/////////////////////////////////////////////////////////////////////////////////////
CWndCreateChar::CWndCreateChar()
{
	m_pModel = NULL;
	m_Player.m_bySkinSet = SKINSET_01;
	m_Player.m_byHairMesh = HAIRMESH_01;
	m_Player.m_byHairColor = 0;
	m_Player.m_byHeadMesh = 0;
	m_Player.m_bySex = SEX_FEMALE;
	m_Player.m_byCostume = 0;
	SetPutRegInfo( FALSE );
}
CWndCreateChar::~CWndCreateChar()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
	SAFE_DELETE( m_pModel );
}
HRESULT CWndCreateChar::InitDeviceObjects()
{
	CWndBase::InitDeviceObjects();
	if( m_pModel )
		m_pModel->InitDeviceObjects( m_pApp->m_pd3dDevice );
	return S_OK;
}
HRESULT CWndCreateChar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pModel )
		m_pModel->RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndCreateChar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	if( m_pModel )
		m_pModel->InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndCreateChar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	//m_pModel->DeleteDeviceObjects();
	return S_OK;
}
void CWndCreateChar::OnDraw( C2DRender* p2DRender )
{
	CRect rect = GetClientRect();

	CPoint pt( 20, 15 );
/*
	for( int i = 0; i < 5; i++)
	{
		p2DRender->RenderLine( pt, CPoint( pt.x + 200, pt. y ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );
		p2DRender->RenderLine( CPoint( pt.x - 10, pt.y + 20 ), CPoint( pt.x + 200 - 10, pt. y + 20 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );

		p2DRender->RenderLine( pt, CPoint( pt.x - 10, pt.y + 20 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );
		p2DRender->RenderLine( CPoint( pt.x + 200, pt. y ), CPoint( pt.x + 200 - 10, pt. y + 20 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );
		p2DRender->RenderLine( CPoint( pt.x + 100, pt. y ), CPoint( pt.x + 100 - 10, pt. y + 20 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );


		pt.y += 30;
	}
	*/
	pt = CPoint( 260, 15 );

//	p2DRender->RenderLine( pt, CPoint( pt.x + 300, pt. y ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );
//	p2DRender->RenderLine( CPoint( pt.x - 10, pt.y + 220 ), CPoint( pt.x + 300 - 10, pt. y + 220 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );

//	p2DRender->RenderLine( pt, CPoint( pt.x - 10, pt.y + 20 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );
//	p2DRender->RenderLine( CPoint( pt.x + 300, pt. y ), CPoint( pt.x + 300 - 10, pt. y + 20 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );
/*
	rect = CRect( 30, 10, 115, 230 );
	int y = 50;
	p2DRender->TextOut( rect.left, rect.top + y, _T( "Name" ) ); y += 50;
	p2DRender->TextOut( rect.left, rect.top + y, _T( "Job" ) ); y += 30;
	p2DRender->TextOut( rect.left, rect.top + y, _T( "Gender" ) ); y += 30;
	p2DRender->TextOut( rect.left, rect.top + y, _T( "Hair Style" ) ); y += 30; // ???????? ???? 
	p2DRender->TextOut( rect.left, rect.top + y, _T( "Hair Color" ) ); y += 30; // ?????? ?? ???? 
	p2DRender->TextOut( rect.left, rect.top + y, _T( "Face" ) ); y += 30; // ?????? ?? ???? 
	*/
	//p2DRender->TextOut( rect.left, rect.top + y, _T( "Costume" ) ); y += 30; // ?????? ?? ???? 
	//p2DRender->TextOut( rect.left, rect.top +130, _T( "Skin Color" ) );
	//DRender->TextOut( rect.left, rect.top +220, _T( "Underwear" ) );
	//p2DRender->TextOut( rect.left, rect.top +130, _T( "Underwear" ) );
	
	//p2DRender->RenderRoundRect(CRect(4,     4,128*2+6,       96+6),D3DCOLOR_TEMP(255,150,150,250));
/*
	CRect rect = CRect( 4, 96 + 6 + 4, 128 * 2 + 6, 96 + 6 + 4 + 96 + 6 );
	p2DRender->RenderRoundRect( rect, D3DCOLOR_TEMP( 255, 150, 150, 250 ) );
	rect.DeflateRect( 1, 1 );
	p2DRender->RenderFillRect( rect, D3DCOLOR_TEMP( 255, 200, 200, 240 ) );

	CRect rect = CRect( 4, 96 + 6 + 4, 128 * 2 + 6, 96 + 6 + 4 + 96 + 6 );
	p2DRender->RenderRoundRect( rect, D3DCOLOR_TEMP( 255, 150, 150, 250 ) );
	rect.DeflateRect( 1, 1 );
	p2DRender->RenderFillRect( rect, D3DCOLOR_TEMP( 255, 200, 200, 240 ) );
*/
	//p2DRender->TextOut(10,60,"aaaa",D3DCOLOR_TEMP(255,100,100,200));


	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	rect = GetClientRect();

//	p2DRender->RenderRect( CRect( 280, 15, 550, 320 ), D3DCOLOR_ARGB( 200, 50, 150, 250 ) );
/*
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE, bEnable );
	if(1)
	{
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  0xffffffff ) ;//CWorld::m_dwBgColor );

		//pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(m_fFogStartValue) );
		//pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(m_fFogEndValue) );
		//pd3dDevice->SetRenderState( D3DRS_FOGDENSITY, FtoDW(m_fFogDensity) );
		if( 1) //m_bUsingTableFog )
		{
			pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
//			pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
//			pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
			pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR );
		}
	}
	pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
	*/


	// ?????? ???? 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 280;
	viewport.Y      = p2DRender->m_ptOrigin.y + 0;
	viewport.Width  = 550 - 280;
	viewport.Height = 320 - 15;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	POINT point = GetMousePoint();

	point.x -= 280;
	point.y -= 15;

	CRect rectViewport( 0, 0, viewport.Width, viewport.Height );
/*
	// ???????? 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ( (FLOAT) viewport.Width ) / (FLOAT) viewport.Height ;
	FLOAT fov = D3DX_PI / 4.0f;//796.0f;
	FLOAT h = cos( fov / 2 ) / sin( fov / 2 );
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, 1.0f, 10.0f );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// ?????? 
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.0f, -5.0f );
/*
	if( rectViewport.PtInRect( point ) )
	{
		// height : 100 = point.y = ?
		int x = 100 * point.x / rectViewport.Width();
		int y = 100 * point.y / rectViewport.Height(); 

		x -= 50;
		y -= 50;

		//vecPos.x = x; vecLookAt.x = x;
		vecPos.y = (FLOAT)-y / 60.0f; vecLookAt.y = (FLOAT)-y / 60.0f;
		vecPos.z += (FLOAT)-y / 20.0f; vecLookAt.z += (FLOAT)-y / 20.0f;

	}
	*/
	const D3DXMATRIX matView = D3DXR::LookAtLH010(vecPos, vecLookAt);

	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// ???? 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// ?????? 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
	
	D3DXMatrixScaling(&matScale,1.4f,1.4f,1.4f);
	D3DXMatrixTranslation(&matTrans,0.0f,-1.1f,0.0f);

	matWorld = matWorld * matScale * matRot * matTrans;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// ?????? 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
	::SetLight( FALSE );
	::SetFog( FALSE );
	SetDiffuse( 1.0f, 1.0f, 1.0f );
	SetAmbient( 1.0f, 1.0f, 1.0f );
	
	m_pModel->FrameMove();
	D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
	pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
	::SetTransformView( matView );
	::SetTransformProj( matProj );

	
	O3D_ELEMENT* pElem = m_pModel->GetParts( PARTS_HAIR );
	
	if( pElem && pElem->m_pObject3D )
	{
		if( m_Player.m_bySex == SEX_MALE )
		{
			pElem->m_pObject3D->m_fAmbient[0] = (nMaleHairColor[m_Player.m_byHairMesh][0])/255.f;
			pElem->m_pObject3D->m_fAmbient[1] = (nMaleHairColor[m_Player.m_byHairMesh][1])/255.f;
			pElem->m_pObject3D->m_fAmbient[2] = (nMaleHairColor[m_Player.m_byHairMesh][2])/255.f;
		}
		else
		{
			pElem->m_pObject3D->m_fAmbient[0] = (nFeMaleHairColor[m_Player.m_byHairMesh][0])/255.f;
			pElem->m_pObject3D->m_fAmbient[1] = (nFeMaleHairColor[m_Player.m_byHairMesh][1])/255.f;
			pElem->m_pObject3D->m_fAmbient[2] = (nFeMaleHairColor[m_Player.m_byHairMesh][2])/255.f;
		}
	}
	
	m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );

	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	viewport.X      = p2DRender->m_ptOrigin.x;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

}
void CWndCreateChar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_Player.m_byJob = JOB_VAGRANT;

	CWndButton* pWndHairColorLeft  = (CWndButton*) GetDlgItem( WIDC_HAIRCOLOR_LEFT );
	CWndButton* pWndHairColorRight = (CWndButton*) GetDlgItem( WIDC_HAIRCOLOR_RIGHT );
	CWndButton* pWndHairFaceLeft   = (CWndButton*) GetDlgItem( WIDC_FACE_LEFT );
	CWndButton* pWndHairFaceRight  = (CWndButton*) GetDlgItem( WIDC_FACE_RIGHT );

	if( GetLanguage() != LANG_THA )
	{
		CWndStatic	* pStatic	= (CWndStatic*)GetDlgItem( WIDC_STATIC7 );
		if( pStatic )
			pStatic->EnableWindow( FALSE );
		pStatic	= (CWndStatic*)GetDlgItem( WIDC_STATIC8 );
		if( pStatic )
			pStatic->EnableWindow( FALSE );
	}

	pWndHairColorLeft->EnableWindow( FALSE );
	pWndHairColorRight->EnableWindow( FALSE );

	CWndButton* pWndOk = (CWndButton*)GetDlgItem( WIDC_OK );
	pWndOk->SetDefault( TRUE );

	
	SetSex( m_Player.m_bySex );
	MoveParentCenter();
	
	CWndEdit* pWndName = (CWndEdit*) GetDlgItem( WIDC_NAME );
	pWndName->SetFocus();

	m_Player.m_byHairMesh = (char)( xRandom( 0, MAX_BASE_HAIR ) );
	m_Player.m_byHeadMesh = (char)( xRandom( 0, MAX_DEFAULT_HEAD ) );
	CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );

	CWndStatic* pWnd2ndPasswordText =  ( CWndStatic* )GetDlgItem( WIDC_STATIC_2ND_PASSWORD_TEXT );
	assert( pWnd2ndPasswordText );
	pWnd2ndPasswordText->m_dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );

	CWndEdit* pWnd2ndPassword = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD );
	assert( pWnd2ndPassword );
	pWnd2ndPassword->AddWndStyle( EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER );
	pWnd2ndPassword->SetMaxStringNumber( MAX_2ND_PASSWORD_NUMBER );
	CWndEdit* pWnd2ndPasswordConfirm = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD_CONFIRM );
	assert( pWnd2ndPasswordConfirm );
	pWnd2ndPasswordConfirm->AddWndStyle( EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER );
	pWnd2ndPasswordConfirm->SetMaxStringNumber( MAX_2ND_PASSWORD_NUMBER );
}

void CWndCreateChar::SetSex( int nSex )
{
	m_Player.m_bySex = nSex;

	int nMover = m_Player.m_bySex == SEX_MALE ? MI_MALE : MI_FEMALE;

	SAFE_DELETE( m_pModel );
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );

	if( nSex == SEX_MALE )
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	else
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND2 ); // ???????? ?????? 

	memset( m_Player.m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
	{
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
			m_Player.m_aEquipInfo[i].dwId	= NULL_ID;
	}

	for (const DWORD dwEquip : prj.jobs.items[m_Player.m_byJob].adwMale[m_Player.m_bySex]) {
		const ItemProp * const pItemProp = prj.GetItemProp(dwEquip);
		m_Player.m_aEquipInfo[pItemProp->dwParts].dwId = dwEquip;
	}

	CMover::UpdateParts( m_Player.m_bySex, 0, m_Player.m_bySkinSet, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
}

BOOL CWndCreateChar::Initialize( CWndBase* pWndParent, DWORD dwStyle )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 590, 400 );
	return CWndNeuz::InitDialog( APP_CREATE_CHAR, pWndParent, WBS_KEY, CPoint( 0, 0 ) );
}


DWORD IsValidPlayerNameTWN( CString& string )
{
	const char* begin	= string;
	const char* end		= begin + string.GetLength();
	const char* iter	= begin;
	char bytes[16];
	
	while( *iter && iter < end ) 
	{ 
		const char* next = CharNext(iter);

		memcpy( bytes, iter, next-iter );
		bytes[next-iter] = 0;

		if( IsMultiByte( iter ) ) 
		{
			wchar_t ch = MAKEWORD( bytes[1], bytes[0] );
			
			if( ch >= 0xA259 && ch <= 0xA261 || ch == 0xA2CD || ch >= 0xA440 && ch <= 0xC67E || ch >= 0xC940 && ch <= 0xF9D5 )
				;
			else
				return TID_DIAG_0014;
		}
		else if( isalnum( bytes[0] ) == FALSE || iscntrl( bytes[0] ) )
			return TID_DIAG_0013;

		iter = next;
	}
	return 0;
}

// return 0   : OK
//        0 > : error
DWORD IsValidPlayerName( CString& strName )
{
	strName.TrimLeft();
	strName.TrimRight();

	LPCTSTR lpszString = strName;
	if( strName.IsEmpty() )
		return TID_DIAG_0031;			// "?????? ????????????."

#ifdef __RULE_0615
	// "?????? ???? 2?? ????, 8?? ?????? ????????????."
	// "?????? ???? 4?? ????, 16?? ?????? ????????????."
	if( strName.GetLength() < 4 || strName.GetLength() > 16 )
		return TID_DIAG_RULE_0;
#else	// __RULE_0615
	if( strName.GetLength() < 3 || strName.GetLength() > 16 )
		return TID_DIAG_0011;			// "?????? 3???? ????, 16???? ?????? ???? ????????????."
#endif	// __RULE_0615

	char c = strName[ 0 ];

	if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszString ) )
		return TID_DIAG_0012;			// "?????? ???????? ?????? ?????? ?? ????????."

	int j;
	switch( ::GetLanguage() )
	{
	case LANG_THA:
		for( j = 0; j < strName.GetLength(); ++j )
		{
			c = strName[ j ];
			if( IsNative( &lpszString[ j ] ) == FALSE && ( isalnum( c ) == FALSE || iscntrl( c ) ) )		
				return TID_DIAG_0013;	// ?????? ?????????? ????????, ???? ?????? ?????? ?? ????????.	
		}
		break;
	case LANG_TWN:
	case LANG_HK:
		return IsValidPlayerNameTWN( strName );

	default:
		for( j = 0; j < strName.GetLength(); ++j )
		{
			c = strName[ j ];
			if( IsDBCSLeadByte(c) ) 
			{
				++j;
				if( ::GetLanguage() == LANG_KOR )
				{
					char c2 = strName[ j ];
					WORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( IsHangul( word ) == FALSE ) 
						return TID_DIAG_0014;
				}					
			}
			else if( isalnum( c ) == FALSE || iscntrl( c ) )
			{
				char szLetter[2]	= { c, '\0' };
				if( (		::GetLanguage() == LANG_GER 
						||	::GetLanguage() == LANG_RUS
					) && prj.nameValider.IsAllowedLetter( szLetter ) )
					continue;
				return TID_DIAG_0013;
			}
		}
		break;
	}

	return 0;
}

BOOL CWndCreateChar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
		case 10000:
			return FALSE;
		case WIDC_MALE: // male
			SetSex( SEX_MALE );
			break;
		case WIDC_FEMALE: // female
			SetSex( SEX_FEMALE );
			break;
		case WIDC_HAIRSTYLE_LEFT: // hair
			m_Player.m_byHairMesh--;
			if( m_Player.m_byHairMesh < 0 )
				m_Player.m_byHairMesh = MAX_BASE_HAIR - 1;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;
		case WIDC_HAIRSTYLE_RIGHT: // hair
			m_Player.m_byHairMesh++;
			if( m_Player.m_byHairMesh >= MAX_BASE_HAIR )
				m_Player.m_byHairMesh = 0;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;
		case WIDC_FACE_LEFT: // head
			m_Player.m_byHeadMesh--;
			if( m_Player.m_byHeadMesh < 0 )
				m_Player.m_byHeadMesh = MAX_DEFAULT_HEAD - 1;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;
		case WIDC_FACE_RIGHT: // head
			m_Player.m_byHeadMesh++;

			if( m_Player.m_byHeadMesh >= MAX_DEFAULT_HEAD )
				m_Player.m_byHeadMesh = 0;
			CMover::UpdateParts( m_Player.m_bySex, m_Player.m_bySkinSet, m_Player.m_byFace, m_Player.m_byHairMesh, m_Player.m_byHeadMesh, m_Player.m_aEquipInfo, m_pModel, NULL );
			break;

		case WIDC_CANCEL: // Cancel 
			{
				Destroy();
				g_WndMng.OpenApplet(APP_SELECT_CHAR);
				CWndSelectChar* pWndSelectChar 
					= (CWndSelectChar*)g_WndMng.GetWndBase( APP_SELECT_CHAR );
				if( pWndSelectChar )
					pWndSelectChar->UpdateCharacter();
				break;
			}
		case WIDC_OK: // Create
			{
				CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_NAME );
				CString string = pEdit->m_string;
				
				DWORD dwError = IsValidPlayerName( string );
				if( dwError > 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText(dwError) );
					pEdit->SetFocus();
					return TRUE;
				}

				if (prj.nameValider.IsNotAllowedName(string)) {
					g_WndMng.OpenMessageBox(prj.GetText(TID_DIAG_0020));	// "???????? ???? ??????????"
					return TRUE;
				}

				CWndEdit* pWnd2ndPassword = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD );
				assert( pWnd2ndPassword );
				CWndEdit* pWnd2ndPasswordConfirm = ( CWndEdit* )GetDlgItem( WIDC_EDIT_2ND_PASSWORD_CONFIRM );
				assert( pWnd2ndPasswordConfirm );
				if( strcmp( pWnd2ndPassword->GetString(), _T( "" ) ) == 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR01 ) );	// 2?? ?????????? ???????? ????????.
					return TRUE;
				}
				if( static_cast< int >( strlen( pWnd2ndPassword->GetString() ) ) < MAX_2ND_PASSWORD_NUMBER )
				{
					CString strError = _T( "" );
					strError.Format( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR02 ), MAX_2ND_PASSWORD_NUMBER );
					g_WndMng.OpenMessageBox( strError );	// 2?? ?????????? ???? %d???????? ???????? ??????.
					return TRUE;
				}
				if( strcmp( pWnd2ndPassword->GetString(), _T( "0000" ) ) == 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR03 ) );	// ???????? ?????????? 2?? ?????????? ?????? ?? ????????.
					return TRUE;
				}
				if( strcmp( pWnd2ndPasswordConfirm->GetString(), _T( "" ) ) == 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR04 ) );	// 2?? ???????? ?????? ???????? ????????.
					return TRUE;
				}
				if( strcmp( pWnd2ndPassword->GetString(), pWnd2ndPasswordConfirm->GetString() ) != 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_INPUT_ERROR05 ) );	// 2?? ???????? ?????? 2?? ?????????? ???????? ????????.
					return TRUE;
				}

//				_tcscpy( m_Player.m_szName, string );
				
				CWndButton*	pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
				pButton->EnableWindow( FALSE );
				pButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
				pButton->EnableWindow( FALSE );
				DWORD dwHairColor = 0xffffffff;
				
				if( m_Player.m_bySex == SEX_MALE )
				{
					dwHairColor = D3DCOLOR_ARGB( 255, nMaleHairColor[m_Player.m_byHairMesh][0],
													nMaleHairColor[m_Player.m_byHairMesh][1],
													nMaleHairColor[m_Player.m_byHairMesh][2] );
				}
				else
				{
					dwHairColor = D3DCOLOR_ARGB( 255, nFeMaleHairColor[m_Player.m_byHairMesh][0],
													nFeMaleHairColor[m_Player.m_byHairMesh][1],
													nFeMaleHairColor[m_Player.m_byHairMesh][2] );
				}

				g_dpLoginClient.SendCreatePlayer( (BYTE)( m_Player.m_uSlot ), string, m_Player.m_byFace, m_Player.m_byCostume, m_Player.m_bySkinSet, m_Player.m_byHairMesh, dwHairColor, m_Player.m_bySex, m_Player.m_byJob, m_Player.m_byHeadMesh, atoi( pWnd2ndPassword->GetString() ) );
			}
			break;
		case 10002: // Accept
			{
			g_WndMng.OpenCustomBox( NULL, new CWndConnectingBox );
			g_Neuz.m_dwTempMessage = 1;
			g_Neuz.m_timerConnect.Set( 1 );
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}



