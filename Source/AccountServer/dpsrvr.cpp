#include "stdafx.h"
#include "dpdbsrvr.h"

#ifdef __GPAUTH_01
#include "lang.h"
#endif	// __GPAUTH_01

#include "BillingMgr.h"
#include "dpsrvr.h"
#include "account.h"
#include "mytrace.h"

#include "dbmanager.h"

const int MAX_CONN = 50000;

CDPSrvr_AccToCert::CDPSrvr_AccToCert()
{
	m_bCheckAddr	= true;
	m_nMaxConn		=	MAX_CONN;
	m_bReloadPro	= TRUE;
	memset( m_sAddrPmttd, 0, sizeof(m_sAddrPmttd) );
	m_nSizeofAddrPmttd	= 0;

	BEGIN_MSG;
	ON_MSG( PACKETTYPE_ADD_ACCOUNT, &CDPSrvr_AccToCert::OnAddAccount );
	ON_MSG( PACKETTYPE_REMOVE_ACCOUNT, &CDPSrvr_AccToCert::OnRemoveAccount );
	ON_MSG( PACKETTYPE_PING, &CDPSrvr_AccToCert::OnPing );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPSrvr_AccToCert::OnCloseExistingConnection );
}

void CDPSrvr_AccToCert::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPSrvr_AccToCert::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);
	
	LPBYTE lpBuffer		= (LPBYTE)lpMsg + nSize;
	u_long uBufSize		= dwMsgSize - nSize;

	CAr ar( lpBuffer, uBufSize );
	GETTYPE( ar )
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	ASSERT( pfn );
	( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg );
}

void CDPSrvr_AccToCert::OnAddConnection( DPID dpid )
{
	g_AccountMng.AddConnection( dpid );
	SendServersetList( dpid );
}

void CDPSrvr_AccToCert::OnRemoveConnection( DPID dpid )
{
	g_AccountMng.RemoveConnection( dpid );
}

void CDPSrvr_AccToCert::DestroyPlayer( DPID dpid1, DPID dpid2 )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DESTROY_PLAYER, dpid2 );
	SEND( ar, this, dpid1 );
}

void CDPSrvr_AccToCert::OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 )
{
	const auto [lpAddr, lpszAccount, cbAccountFlag] = ar.Extract<char[16], SAccountName, BYTE>();
	DWORD dwAuthKey = 0;
	int		fCheck = 0;

#ifdef __GPAUTH_02
	char szCheck[255]	= { 0,};
#ifdef __EUROPE_0514
	char szBak[MAX_ACCOUNT]	= { 0,};
#endif	// __EUROPE_0514
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
		ar.ReadString( szCheck, 255 );
#ifdef __EUROPE_0514
	ar.ReadString( szBak, MAX_ACCOUNT );
#endif	// __EUROPE_0514
	DWORD dwPCBangClass;
	ar >> dwPCBangClass;

#endif	// __GPAUTH_02
	BYTE cbResult = ACCOUNT_CHECK_OK;		 

	// 1. 외부 아이피 검사 
	if( m_bCheckAddr )							
	{
		CMclAutoLock	Lock( m_csAddrPmttd );
		int i = NULL;
		for( i = 0; i < m_nSizeofAddrPmttd; i++ )
		{
			if( strstr( lpAddr, m_sAddrPmttd[i] ) )
				break;		
		}
		if( i == m_nSizeofAddrPmttd )
			cbResult = ACCOUNT_EXTERNAL_ADDR;	 
	}

	CMclAutoLock	Lock( m_csIPCut );
	if( lpAddr[0] == '\0' || IsABClass(lpAddr) == TRUE )
		cbResult = ACCOUNT_IPCUT_ADDR;		

	if( lpAddr[0] == '\0' )
		Error( "Not Addr : %s", lpAddr );
	
	// 2. MAX, 중복검사  
	if( cbResult == ACCOUNT_CHECK_OK )			
	{
		if( g_AccountMng.m_nCount < m_nMaxConn )	
		{
			cbResult = g_AccountMng.AddAccount( lpszAccount, dpid1, dpid2, &dwAuthKey, cbAccountFlag, fCheck );
			if( dwPCBangClass != 0 )
				g_AccountMng.PushPCBangPlayer( dwAuthKey, dwPCBangClass );
		}
		else
			cbResult = ACCOUNT_OVERFLOW;
	}

	OnAfterChecking(dpid1, dpid2, {
		.f = cbResult,
		.lpszAccount = lpszAccount,
		.dwAuthKey = dwAuthKey,
		.cbAccountFlag = cbAccountFlag,
		.lTimeSpan = 0,
#ifdef __GPAUTH_02
		.szCheck = szCheck,
#ifdef __EUROPE_0514
		.szBak = szBak
#endif
#endif
		});
}


void CDPSrvr_AccToCert::OnAfterChecking(DPID dpid1, DPID dpid2, const OnAfterCheckingParams & params) {
	if( params.f == ACCOUNT_CHECK_OK )
	{
		g_DbManager.UpdateTracking( TRUE, params.lpszAccount );  // 유저가 login 했음을 디비에 쓴다.
	}

	BEFORESENDSOLE( ar, PACKETTYPE_ADD_ACCOUNT, dpid2 );
	ar << params.f;
	ar << params.dwAuthKey;
	ar << params.cbAccountFlag;

#ifdef __GPAUTH_01
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
	{
		ar.WriteString(params.lpszAccount );
#ifdef __GPAUTH_02
		ar.WriteString(params.szCheck );
#endif	// __GPAUTH_02
	}
#ifdef __EUROPE_0514
	ar.WriteString(params.szBak );
#endif	// __EUROPE_0514
#endif	// __GPAUTH_01

	SEND( ar, this, dpid1 );
}

void CDPSrvr_AccToCert::OnRemoveAccount( CAr & ar, DPID dpid1, DPID dpid2 )
{
	g_AccountMng.RemoveAccount( dpid1, dpid2 );
}


void CDPSrvr_AccToCert::OnPing( CAr & ar, DPID dpid1, DPID dpid2 )
{
	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );

	CAccount* pAccount	= g_AccountMng.GetAccount( dpid1, dpid2 );
	if( NULL != pAccount )
		pAccount->m_dwPing	= timeGetTime();
}

bool CDPSrvr_AccToCert::LoadAddrPmttd( LPCTSTR lpszFileName )
{
	CMclAutoLock	Lock( m_csAddrPmttd );

	m_nSizeofAddrPmttd	= 0;

	CScanner s;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			strcpy( m_sAddrPmttd[m_nSizeofAddrPmttd++], (LPCSTR)s.Token );
			s.GetToken();
		}
		return true;
	}
	return false;
}


BOOL CDPSrvr_AccToCert::LoadIPCut( LPCSTR lpszFileName )
{
	CMclAutoLock	Lock( m_csIPCut );

	InitIPCut();

	char strABClass[MAX_PATH];

	CScanner s;
	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			ZeroMemory( strABClass, sizeof( strABClass ) );
			
			GetABCClasstoString( s.Token, strABClass, m_nIPCut[m_nSizeofIPCut][0] );
			s.GetToken();
			s.GetToken();
			GetABCClasstoString( s.Token, strABClass, m_nIPCut[m_nSizeofIPCut][1] );

			int nFindLast = -1;
			STRING2INT::iterator iFind = m_sIPCut.find( strABClass );
			if( iFind != m_sIPCut.end() )
			{	
				int nFind = iFind->second;
				int nBufFind = m_nIPCut[nFind][2];
				nFindLast = nFind;
				while( nBufFind != -1 )
				{
					nFindLast = nBufFind;
					nBufFind = m_nIPCut[nBufFind][2];
				}
			}
			m_sIPCut.emplace(strABClass, m_nSizeofIPCut);
			++m_nSizeofIPCut;

			if( nFindLast != -1 )
			{
				m_nIPCut[nFindLast][2] = m_nSizeofIPCut - 1;
			}
			s.GetToken();
		}
		return TRUE;
	}
	
	return FALSE;
}

BOOL CDPSrvr_AccToCert::IsABClass( LPCSTR lpszIP )
{
	char strABClass[MAX_PATH] = {0,};
	int nCClass = 0;
	GetABCClasstoString( lpszIP, strABClass, nCClass );
	STRING2INT::iterator iFind = m_sIPCut.find( strABClass );
	if( iFind != m_sIPCut.end() )
	{
		int nFind = iFind->second;
		if( m_nIPCut[nFind][0] <= nCClass && nCClass <= m_nIPCut[nFind][1] )
		{
			return TRUE;
		}

		while( m_nIPCut[nFind][2] != -1 )
		{
			nFind = m_nIPCut[nFind][2];
			if( m_nIPCut[nFind][0] <= nCClass && nCClass <= m_nIPCut[nFind][1] )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CDPSrvr_AccToCert::GetABCClasstoString( LPCSTR lpszIP, char * lpABClass, int &nCClass )
{
	int nFindAB, nFindC;
	CString strBuf1;
	char strCClass[MAX_PATH];
	ZeroMemory( strCClass, sizeof( strCClass ) );
	strBuf1 = lpszIP;
	nFindAB = strBuf1.Find( "." );
	if( nFindAB == -1 )
		AfxMessageBox( strBuf1 );
	nFindAB = strBuf1.Find( ".", nFindAB + 1 );
	if( nFindAB == -1 )
		AfxMessageBox( strBuf1 );
	
	memcpy( lpABClass, lpszIP, nFindAB );
	nFindC = strBuf1.Find( ".", nFindAB + 1 );
	if( nFindC == -1 )
		AfxMessageBox( strBuf1 );
	for( int i = nFindAB + 1, j = 0 ; i < nFindC ; ++i, ++j )
	{
		strCClass[j] = lpszIP[i];
	}
	nCClass = atoi( strCClass );
}


void CDPSrvr_AccToCert::InitIPCut( void )
{
	m_nSizeofIPCut = 0;
	for( size_t i = 0 ; i < MAX_IP ; ++i )
	{
		m_nIPCut[i][0] = 0;
		m_nIPCut[i][1] = 0;
		m_nIPCut[i][2] = -1;
	}
	m_sIPCut.clear();
}

void CDPSrvr_AccToCert::SendServersetList( DPID dpid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_SRVR_LIST, DPID_UNKNOWN );
	ar.WriteString( m_szVer );
#ifdef __SECURITY_0628
	ar.WriteString( m_szResVer );
#endif	// __SECURITY_0628

	m_servers.read([&](const CListedServers & servers) { ar << servers; });
	SEND( ar, this, dpid );
}

void CDPSrvr_AccToCert::SendPlayerCount( u_long uId, long lCount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PLAYER_COUNT, DPID_UNKNOWN );
	ar << uId << lCount;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPSrvr_AccToCert::SendEnableServer( u_long uId, long lEnable )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ENABLE_SERVER, DPID_UNKNOWN );
	ar << uId << lEnable;
	SEND( ar, this, DPID_ALLPLAYERS );
}

BOOL CDPSrvr_AccToCert::EnableServer( DWORD dwParent, DWORD dwID, long lEnable ) {
	BOOL res = FALSE;
	m_servers.write([&](CListedServers & servers) {
		const u_long uId = dwParent * 100 + dwID;
		if (CListedServers::Channel * channel = servers.GetChannel(dwParent, dwID)) {
			channel->lEnable = lEnable;
			SendEnableServer(uId, lEnable);
			g_MyTrace.Add(uId, FALSE, "%d/%d - %s", dwParent, dwID, lEnable ? "o" : "x");
			
			res = TRUE;
		}
		});

	return res;
}

void CDPSrvr_AccToCert::OnCloseExistingConnection( CAr & ar, DPID dpid1, DPID dpid2 )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	CloseExistingConnection( lpszAccount, 0 );	// 0 - 에러가 아니다.
}

// 현재 접속한 어카운트를 끊는다.
// 접속은 2가지 경우가 있다. - 접속 후 플레이를 하는 경우 
//                           - 접속 과정 
void CDPSrvr_AccToCert::CloseExistingConnection( LPCTSTR lpszAccount, LONG lError )
{
	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( lpszAccount );
	if( pAccount )
	{
		if( pAccount->m_fRoute )	// 접속 후 플레이를 하는 경우 
		{
			g_dpDbSrvr.SendCloseExistingConnection( lpszAccount, lError );
		}
		else						// 접속 과정 
		{
			DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
			g_AccountMng.RemoveAccount( pAccount->m_dpid1, pAccount->m_dpid2 );
		}
	}
}

CDPSrvr_AccToCert		g_dpSrvr;
