#include "stdafx.h"
#include "dpaccountclient.h"
#include "dpcertifier.h"
#include "dbmanager.h"
#include "..\Resource\Lang.h"
#include <algorithm>

#include "user.h"

CDPCertifier::CDPCertifier()
{
	ON_MSG( PACKETTYPE_CERTIFY, &CDPCertifier::OnCertify );
	ON_MSG( PACKETTYPE_PING, &CDPCertifier::OnPing );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPCertifier::OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_KEEP_ALIVE, &CDPCertifier::OnKeepAlive );
	ON_MSG( PACKETTYPE_ERROR, &CDPCertifier::OnError );
}

void CDPCertifier::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
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

void CDPCertifier::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	
	if( dwMsgSize < 4 )
	{
		WriteError( "PACKET//0" );
		return;
	}

	DWORD dw; ar >> dw;

	if (Handle(ar, dw, dpid)) {
		if (ar.IsOverflow()) Error("Certifier-Neuz: Packet %08x overflowed", dw);
	}
}

void CDPCertifier::OnAddConnection( DPID dpid )
{
	if (!g_CertUserMng.AddUser(dpid)) {
#ifdef __US_LOGIN_0223
		DestroyPlayer(dpid);
#endif
	}
}

// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 초단위로 보낸다.
static long GetPlayLeftTime( BYTE cbAccountFlag )
{
	if( cbAccountFlag & ACCOUNT_FLAG_18 )	// 성인은 남은 22:00시에 끊기지 않기에, 시간을 알리지 않게함 
		return 0;		

	CTime cur = CTime::GetCurrentTime();
	
	CTimeSpan span; 
	if( cur.GetHour() < 22 )
	{
		CTime target( cur.GetYear(), cur.GetMonth(), cur.GetDay(), 22, 0, 0 );
		span = target - cur;
	}
	else
	{
		CTime target( cur.GetYear(), cur.GetMonth(), cur.GetDay(), 24, 0, 0 );
		span = target - cur;

		CTimeSpan ts( 0, 22, 0, 0 ); 
		span += ts;
	}

	return std::max<long>( (long)( span.GetTotalSeconds() ), 1 );	// 적어도 1초 이상 ( 0 과 구분하기 위해서 )
}

void CDPCertifier::SendServerList(DPID dpid, SendServerListArgs args)
{
	BEFORESEND( ar, PACKETTYPE_SRVR_LIST );
	ar << args.dwAuthKey;
	ar << args.cbAccountFlag;

#ifdef __GPAUTH_01
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
	{
		ar.WriteString( args.szGPotatoNo );
#ifdef __GPAUTH_02
		ar.WriteString( args.szCheck );
#endif	// __GPAUTH_02
	}
#ifdef __EUROPE_0514
	ar.WriteString( args.szBak );
#endif	// __EUROPE_0514

#endif	// __GPAUTH_01

	if( ::GetLanguage() == LANG_THA )
	{
		long lTimeLeft = GetPlayLeftTime( args.cbAccountFlag );	// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 보낸다.
		ar << lTimeLeft;
	}

	m_servers.read([&ar](const CListedServers & servers) { ar << servers; });

	SEND( ar, this, dpid );
}

void CDPCertifier::SendError( LONG lError, DPID dpId )
{
	BEFORESEND( ar, PACKETTYPE_ERROR );
	ar << lError;
	SEND( ar, this, dpId );
//	DestroyPlayer( dpId ); shutdown을 호출하지 못하는 이 아픔 
}

#ifdef __GPAUTH
void CDPCertifier::SendErrorString( const char* szError, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_ERROR_STRING );
	ar.WriteString( szError );
	SEND( ar, this, dpid );
}
#endif	// __GPAUTH

// Handlers
/*________________________________________________________________________________*/
void CDPCertifier::OnRemoveConnection(const DPID dpid) {
	g_CertUserMng.RemoveUser(dpid);
	g_dpAccountClient.SendRemoveAccount(dpid);
}

void CDPCertifier::OnError( CAr & ar, DPID dpid )
{
	DestroyPlayer( dpid );
}

void CDPCertifier::OnCertify( CAr & ar, DPID dpid )
{
	char pszVer[32]	= { 0, };
	ar.ReadString( pszVer, 32 );

	if( strcmp( m_szVer, pszVer ) )
	{
		SendError( ERROR_ILLEGAL_VER, dpid );
		return;
	}
#ifdef __SECURITY_0628
	char pszResVer[100]	= { 0,};
	ar.ReadString( pszResVer, 100 );
	if( lstrlen( m_szResVer ) > 0 && lstrcmp( m_szResVer, pszResVer ) )
	{
		SendError( ERROR_FLYFF_RESOURCE_MODIFIED, dpid );
		return;
	}
#endif	// __SECURITY_0628

	char pszAccount[MAX_ACCOUNT] = {0, }, pszPwd[MAX_PASSWORD] = {0, };
	ar.ReadString( pszAccount, MAX_ACCOUNT );

	//////////////////////////////////////////////////////////////////////////
	//	mulcom	BEGIN100218	패스워드 암호화
#ifdef __ENCRYPT_PASSWORD
	char		szEnc[ 16 * MAX_PASSWORD ] = {0, };
	char		szDec[ 16 * MAX_PASSWORD ] = {0, };
	
	ar.Read( szEnc, 16 * MAX_PASSWORD );
	
	g_xRijndael->ResetChain();
	g_xRijndael->Decrypt( szEnc, szDec, 16 * MAX_PASSWORD, CRijndael::CBC );
	
	::memcpy( pszPwd, szDec, MAX_PASSWORD );
#else
	ar.ReadString( pszPwd, MAX_PASSWORD );
#endif
	//	mulcom	END100218	패스워드 암호화
	//////////////////////////////////////////////////////////////////////////

	if (pszAccount[0] == '\0' || std::string_view(pszAccount).contains('\'')
		|| std::string_view(pszPwd).contains('\'')
		) {
		DestroyPlayer(dpid);
		return;
	}

#ifdef __EUROPE_0514
	if (!g_CertUserMng.SetAccount(dpid, pszAccount)) {
		return;
	}
#endif	// __EUROPE_0514

	DB_OVERLAPPED_PLUS * pData = g_DbManager.Alloc();
	memset( &pData->AccountInfo, 0, sizeof(ACCOUNT_INFO) );
	strcpy( pData->AccountInfo.szAccount, pszAccount );
	strcpy( pData->AccountInfo.szPassword, pszPwd );
	_tcslwr( pData->AccountInfo.szAccount );
#ifdef __TWN_LOGIN0816
	char pszSessionPwd[MAX_SESSION_PWD]	= { 0,};
	if( GetLanguage() == LANG_TWN )
		ar.ReadString( pszSessionPwd, MAX_SESSION_PWD );
	lstrcpy( pData->AccountInfo.szSessionPwd, pszSessionPwd );
#endif	// __TWN_LOGIN0816
	pData->dwIP         = GetPlayerAddr( dpid );
	pData->dpId			= dpid;
	pData->nQueryMode	= CERTIFY;
	g_DbManager.PostQ( pData );
}

void CDPCertifier::OnPing( CAr & ar, DPID dpid )
{
	g_dpAccountClient.SendPing( dpid );
}

void CDPCertifier::OnCloseExistingConnection( CAr & ar, DPID dpid )
{
	char pszAccount[MAX_ACCOUNT], pszPwd[MAX_PASSWORD];
	ar.ReadString( pszAccount, MAX_ACCOUNT );
	ar.ReadString( pszPwd, MAX_PASSWORD );

	if( pszAccount[0] == '\0' )
		return;

	DB_OVERLAPPED_PLUS * pData = g_DbManager.Alloc();
	strcpy( pData->AccountInfo.szAccount, pszAccount );
	strcpy( pData->AccountInfo.szPassword, pszPwd );
	_tcslwr( pData->AccountInfo.szAccount );
	pData->dwIP			= 0;		// don't use
	pData->dpId			= dpid;
	pData->nQueryMode	= CLOSE_EXISTING_CONNECTION;
	g_DbManager.PostQ( pData );
}

void CDPCertifier::OnKeepAlive(CAr & ar, DPID dpid) {
	g_CertUserMng.KeepAlive(dpid);
}
/*________________________________________________________________________________*/
CDPCertifier	g_dpCertifier;
