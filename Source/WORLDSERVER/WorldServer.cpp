#include "StdAfx.h"
#include "Resource.h"
#include "GlobalTime.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "User.h"
#include "WorldMng.h"
#include "ThreadMng.h"
#include "AIBear.h"
#include "AIBigMuscle.h"
#include "AIClockWorks.h"
#include "AIGuard.h"
#include "AIKrrr.h"
#include "AIPet.h"
#include "guild.h"
#include "guildwar.h"
#include "party.h"
#include "Chatting.h"
#include "MiniDumper.h"
#include "spevent.h"
#include "ItemScript.h"
#include "WorldDialog.h"
#include "WScript.h"
#include "DisplayedInfo.h"

#ifdef __VM_0819
#include "vmlog.h"
#endif	// __VM_0819

#include "couple.h"
#include "housing.h"


#include "slord.h"

#ifdef __NEW_PROFILE
#include "ProfileInfo.h"
#endif // __NEW_PROFILE

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL				Script( LPCSTR lpszFileName );
void				ExitInstance( void );
void				SetTitle();
extern BOOL			LoadAIScript();

const int			MAX_LOADSTRING = 100;
static TCHAR		g_szTitle[MAX_LOADSTRING];			
static TCHAR		g_szWindowClass[MAX_LOADSTRING];	
HWND g_hMainWnd;
char				g_szCoreAddr[16];

static char  		g_szINI[] = "WorldServer.ini";


extern	char				g_szDBAddr[16];
extern	char				g_sHeartbeat[32];


#if defined(__LUASCRIPT060908)
	#ifdef _DEBUG
		#pragma comment(lib, "../lua/luaD.lib" )	
	#else
		#pragma comment(lib, "../lua/lua.lib" )
	#endif
#endif

void InitLanguageFromResource( HINSTANCE hInstance )
{
	char szLang[16], szSubLang[16];
	LoadString(hInstance, IDS_LANG, szLang, 16);	
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);	
	
	char szCodePage[16];
	LoadString(hInstance, IDS_CODEPAGE, szCodePage, 16);	
	g_codePage = atoi( szCodePage );

	::SetLanguageInfo( atoi( szLang ), atoi( szSubLang ) );
}

extern void md5( char* out, const char* in );


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	LoadString(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WORLDSERVER, g_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	InitLanguageFromResource( hInstance );
	if( ::GetLanguage() != LANG_KOR )
		g_eLocal.SetState( EVE_18, 1 );

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		ExitInstance();
		return FALSE;
	}

	MSG msg;
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ExitInstance();

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WORLDSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_WORLDSERVER;
	wcex.lpszClassName	= g_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_TIMER:
			{
				WORD wTimerID	= wParam;
				switch( wTimerID )
				{
					case 0:
						g_dpDBClient.SendPing();
						g_DPCoreClient.SendPing();
						break;
					case 1:
						// g_UserMng.OutputStatistics();
						break;
				}
				break;
			}
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps );
			g_DisplayedInfo.Paint(hdc);
			EndPaint( hWnd, &ps );
			break;
		case WM_LBUTTONDOWN:
			CRunObject::GetInstance()->ToggleProfiling();
#ifdef __MEM_TRACE
#ifndef _DEBUG
			CMemTrace::Instance()->Print();
#endif	// _DEBUG
#endif	// __MEM_TRACE
#ifdef __VM_0819
			CVMLog::Instance()->Log();
#endif	// __VM_0819
			break;
		case WM_RBUTTONDOWN:
#ifdef __NEW_PROFILE
			CProfileInfo::GetInstance()->SetToggleProfiling();
#endif // __NEW_PROFILE
			break;
		case WM_DESTROY:
			KillTimer( g_hMainWnd, 0 );
			KillTimer( g_hMainWnd, 1 );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// float point ?????? exception?? ???? ????.
void EnableFloatException()
{
	int cw = _controlfp( 0, 0 );
	cw &= ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
	_controlfp( cw, MCW_EM );
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // Set up the highest resolution timer we can manage 
//	TIMECAPS tc; 
//	UINT nPeriod;
//	if( TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(tc)) ) 
//		nPeriod = tc.wPeriodMin;
//	else
//		nPeriod = 1;  
//	timeBeginPeriod( nPeriod ); 

	HWND hWnd;
	g_hMainWnd	= hWnd	= CreateWindow(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	int x = 480, y = 0;
	SetWindowPos( hWnd, NULL, x, y, 800, 416, SWP_SHOWWINDOW );

	if (!hWnd)
		return FALSE;

	SetTimer( g_hMainWnd, 0, SEC( 60 ), NULL );	// 60
	SetTimer( g_hMainWnd, 1, MIN( 60 ), NULL );	// 3600

	CTime time = CTime::GetCurrentTime();
	g_DisplayedInfo.SetLogInfo(LOGTYPE_TIME, "%s", time.Format("%Y/%m/%d %H:%M:%S"));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifndef _DEBUG
// 	#if defined(__MINIDUMP)
// 		InitDumper( DUMP_LEVEL_0 );
// 	#else
// 		InitEH();
// 	#endif // !__MINIDUMP
#endif	// _DEBUG

	EnableFloatException();

	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( InitializeScriptLib() == FALSE )
		return FALSE;

	xSRand( timeGetTime()  );	// ?????? ?????? ?????? xRandom()?? ??????...

	if( Script( g_szINI ) == TRUE )
	{
		try {
			if (!prj.OpenProject("Masquerade.prj")) {
				Error("OpenProject ERROR check Masquerade.prj");
				return FALSE;
			}
		} catch (ProjectLoadError & e) {
			Error("OpenProject ERROR: %s", e.what());
			throw e;
		}

		g_DisplayedInfo.Redraw();
		SetTitle();

		if( CompileItemScript( "ItemScript.lua" ) == FALSE )
			return FALSE;

	#if defined(__REMOVE_SCIRPT_060712)
		if( CWorldDialog::GetInstance().Init() == FALSE )
		{
			OutputDebugString( "WorldDialog.DLL load failed\n" );
			return FALSE;
		}
	#endif
		return CRunObject::GetInstance()->Init();
	}
	else
	{
		char szBuffer[300] = {0,};
		char szDir[255] = {0,};
		GetCurrentDirectory( 255, szDir );
		sprintf( szBuffer, "%s\\WorldServer.ini File not found", szDir );
		AfxMessageBox( szBuffer );
		return FALSE;
	}
}

void ExitInstance( void )
{
	CRunObject::GetInstance()->Close();

	g_DPSrvr.DeleteDPObject();
	g_DPCoreClient.DeleteDPObject();
	g_dpDBClient.DeleteDPObject();

	g_WorldMng.Free();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE( CUser::m_pPool );	
	SAFE_DELETE( CMover::m_pPool );		
	SAFE_DELETE( CActionMover::m_pPool );
	SAFE_DELETE( CAIMonster::m_pPool );	
	SAFE_DELETE( CItem::m_pPool );	
#endif	// __MEM_TRACE
#endif	// __VM_0820

	g_PartyMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE( CParty::m_pPool );
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_ChattingMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE( CChatting::m_pPool );
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_GuildWarMng.Clear();

	g_GuildMng.Clear();

	CHousingMng::GetInstance()->Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE( CGuildMember::sm_pPool );
	SAFE_DELETE( CGuild::sm_pPool );
	SAFE_DELETE( CAIPet::m_pPool );
	SAFE_DELETE( CAIKrrr::m_pPool );
	SAFE_DELETE( CAIGuard::m_pPool );
	SAFE_DELETE( CAIClockWorks::m_pPool );
	SAFE_DELETE( CAIBigMuscle::m_pPool );
	SAFE_DELETE( CAIBear::m_pPool );
#endif	// __MEM_TRACE
#endif	// __VM_0820
	UninitializeNetLib();
	UninitializeScriptLib();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE( CAIPet::m_pPool );
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_Object3DMng.Destroy();
	prj.m_modelMng.DeleteDeviceObjects();

	CSLord::Instance()->DestroyColleagues();
}


void SetTitle()
{
	char szLang[MAX_PATH]	= { 0, };
#ifdef __LANG_1013
	sprintf( szLang, CLangMan::GetInstance()->GetLangData( ::GetLanguage() )->szTitle );
#else	// __LANG_1013
	switch( ::GetLanguage() )
	{
		case 0:	sprintf( szLang, "Korean" );	break;
		case 1:	sprintf( szLang, "English" );	break;
		case 2:	sprintf( szLang, "Japanese" );	break;
		case 3:	sprintf( szLang, "Chinese" );	break;
		case 4:	sprintf( szLang, "Thailand" );	break;
		case 5:	sprintf( szLang, "Taiwan" );	break;
		default:	break;
	}
#endif	// __LANG_1013

	char szTitle[256];
	sprintf( szTitle, "%s(%d)-%s", g_szTitle, g_uKey, szLang );
	SetWindowText( g_hMainWnd, szTitle );
}

BOOL Script( LPCSTR lpszFileName )
{
	g_eLocal.SetState( EVE_RESPAWN, 1 );

	CScanner s;
	if( s.Load( lpszFileName ) == FALSE)
	{
		Error("Fail to open file - %s", lpszFileName );
		return FALSE;
	}

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == "ResourceFolder" )
		{
			s.GetToken();
			if( !SetCurrentDirectory( s.Token ) )
			{
				Error( "Folder not found, %s", s.Token );
				return FALSE;
			}
		}
		else if( s.Token == "Key" )
		{
			g_uKey		 = (DWORD)s.GetNumber();
#ifdef __S8_SERVER_PORT
			g_uIdofMulti = g_uKey % 100;
#else // __S8_SERVER_PORT
			g_uIdofMulti = g_uKey / 100;
#endif // __S8_SERVER_PORT
		}
		else if( s.Token == "Core" )
		{
			s.GetToken();
			strcpy( g_szCoreAddr, s.Token );
		}
		else if( s.Token == "DB" )
		{
			s.GetToken();
			strcpy( g_szDBAddr, s.Token );
		}
		else if( s.Token == "Account" )
		{
			s.GetToken();
		}
		else if( s.Token == "Heartbeat" )
		{
			s.GetToken();
			strcpy( g_sHeartbeat, s.Token );
		}
		else if( s.Token == "18" )
		{
			g_eLocal.SetState( EVE_18, 1 );		// 18?? ???? 
		}
		else if( s.Token == "PK" )
		{
			g_eLocal.SetState( EVE_PK, 1 );
		}
		else if( s.Token == "PKCOST" )
		{
			g_eLocal.SetState( EVE_PKCOST, 1 );
		}
		else if( s.Token == "STEAL" )
		{
			g_eLocal.SetState( EVE_STEAL, 1 );				
		}
		else if (s.Token == "GUILDBANK")
		{
			g_eLocal.SetState( ENABLE_GUILD_INVENTORY, 1 );
		}
		else if( s.Token == "GUILDWAR" )
		{
			g_eLocal.SetState( EVE_GUILDWAR, 1 );
		}
		else if( s.Token == "DROPITEM_REMOVE" )
		{
			g_eLocal.SetState( EVE_DROPITEMREMOVE, 1 );
		}			
		else if( s.Token == "SCHOOL" )
		{
			//g_eLocal.SetState( EVE_SCHOOL, 1 );
		}
		else if( s.Token == "GUILDCOMBAT")
		{
			g_eLocal.SetState( EVE_GUILDCOMBAT, 1 );
		}
		else if( s.Token == "WORMON" )
		{
			g_eLocal.SetState( EVE_WORMON, 1 );
		}
		else if( s.Token == "respawn" )
		{
			g_eLocal.SetState( EVE_RESPAWN, (BYTE)s.GetNumber() );
		}
#ifdef __S_RECOMMEND_EVE
		else if( s.Token == "RECOMMEND" )
		{
			g_eLocal.SetState( EVE_RECOMMEND, 1 );
		}
#endif // __S_RECOMMEND_EVE

		else if( s.Token == "GUILDCOMBAT1TO1" )
		{
			g_eLocal.SetState( EVE_GUILDCOMBAT1TO1, 1 );
		}

#ifdef __JEFF_11_4
		else if( s.Token == "ARENA" )
		{
			g_eLocal.SetState( EVE_ARENA, 1 );
		}
#endif // __JEFF_11_4

		else if( s.Token == "SECRETROOM" )
		{
			g_eLocal.SetState( EVE_SECRETROOM, 1 );
		}
		else if( s.Token == "RAINBOWRACE" )
		{
			g_eLocal.SetState( EVE_RAINBOWRACE, 1 );
		}

		s.GetToken();
	}
	return TRUE;
}
