#include "stdafx.h"
#include "dpcertifier.h"
#include "dpaccountclient.h"
#ifdef __GPAUTH_01
#include "lang.h"
#endif	// __GPAUTH_01

#include "user.h"

CDPAccountClient::CDPAccountClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_ADD_ACCOUNT, &CDPAccountClient::OnAddAccount );
	ON_MSG( PACKETTYPE_DESTROY_PLAYER, &CDPAccountClient::OnDestroyPlayer );
	ON_MSG( PACKETTYPE_SRVR_LIST, &CDPAccountClient::OnServersetList );
	ON_MSG( PACKETTYPE_PLAYER_COUNT, &CDPAccountClient::OnPlayerCount );
	ON_MSG( PACKETTYPE_ENABLE_SERVER, &CDPAccountClient::OnEnableServer );
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpid2	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );


	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
//	ASSERT( pfn );
	if( pfn )
		( this->*( pfn ) )( ar, dpid2 );
}



void CDPAccountClient::SendAddAccount(DPID idFrom, const CDPAccountClient_SendAddAccount_Params & params) {
	BEFORESENDSOLE( ar, PACKETTYPE_ADD_ACCOUNT, idFrom );
	
	ar.WriteString( params.lpAddr );
	ar.WriteString( params.lpszAccount );
	ar << params.cbAccountFlag;

#ifdef __BILLING0712
	ar << params.fCheck;
#endif//__BILLING0712

#ifdef __GPAUTH_02
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
		ar.WriteString( params.szCheck );
#endif	// __GPAUTH_02
#ifdef __EUROPE_0514
	ar.WriteString( params.szBak );
#endif	// __EUROPE_0514
	ar << params.dwPCBangClass;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAccount( DPID idFrom )
{
	BEFORESENDSOLE( ar, PACKETTYPE_REMOVE_ACCOUNT, idFrom );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRoute( DWORD dwIdofServer, u_long uIdofMulti, DPID idFrom )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ROUTE, idFrom );
	ar << dwIdofServer;
	ar << uIdofMulti;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendPing( DPID idFrom )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PING, idFrom );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendCloseExistingConnection( const char* lpszAccount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::OnAddAccount( CAr & ar, DPID dpid )
{
	BYTE	cbResult;
	DWORD	dwAuthKey = 0;
	BYTE	cbAccountFlag = 0;
	long	lTimeSpan = 0;

	ar >> cbResult;
	ar >> dwAuthKey;
	ar >> cbAccountFlag;

#ifdef __BILLING0712
	ar >> lTimeSpan;
#endif

#ifdef __GPAUTH_01
	char szGPotatoNo[10]	= { 0,};
#ifdef __GPAUTH_02
	char szCheck[255]	= { 0,};
#endif	// __GPAUTH_02
#ifdef __EUROPE_0514
	char szBak[MAX_ACCOUNT]	= { 0,};
#endif	// __EUROPE_0514
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
	{
		ar.ReadString( szGPotatoNo, 10 );
#ifdef __GPAUTH_02
		ar.ReadString( szCheck, 255 );
#endif	// __GPAUTH_02
	}
#ifdef __EUROPE_0514
	ar.ReadString( szBak, MAX_ACCOUNT );
#endif	// __EUROPE_0514

#endif	// __GPAUTH_01

	switch( cbResult )
	{
		case ACCOUNT_DUPLIACTE:					// 중복 에러
			g_dpCertifier.SendError( ERROR_DUPLICATE_ACCOUNT, dpid );			
			break;
		case ACCOUNT_CHECK_OK:					// 성공
			{
#ifndef __EUROPE_0514
			const char * szBak = nullptr;
#endif

			if (!g_CertUserMng.AccountCheckOk(dpid, szBak)) {
				return;
			}

			#ifdef __GPAUTH_01
				#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
				g_dpCertifier.SendServerList( dpid, dwAuthKey, cbAccountFlag, lTimeSpan, szGPotatoNo, szCheck, szBak );
#else	// __EUROPE_0514
				g_dpCertifier.SendServerList( dpid, dwAuthKey, cbAccountFlag, lTimeSpan, szGPotatoNo, szCheck );
#endif	// __EUROPE_0514
				#else	// __GPAUTH_02
				g_dpCertifier.SendServerList( dpid, dwAuthKey, cbAccountFlag, lTimeSpan, szGPotatoNo );
				#endif	// __GPAUTH_02
			#else	// __GPAUTH_01
				g_dpCertifier.SendServerList( dpid, dwAuthKey, cbAccountFlag, lTimeSpan );
			#endif	// __GPAUTH_01
				break;
			}
		case ACCOUNT_EXTERNAL_ADDR:				// 허용되지 않는 주소 
			g_dpCertifier.SendError( ERROR_EXTERNAL_ADDR, dpid );
			break;
		case ACCOUNT_OVERFLOW:					// 인원초과
			g_dpCertifier.SendError( ERROR_OVERFLOW, dpid );
			break;
		case ACCOUNT_BILLING_INFO_FAILED:		// 빌링 정보 없음
			g_dpCertifier.SendError( ERROR_BILLING_INFO_FAILED, dpid );
			break;
		case ACCOUNT_BILLING_DATABASE_ERROR:	// 빌링 DB 에러 
			g_dpCertifier.SendError( ERROR_BILLING_DATABASE_ERROR, dpid );
			break;
		case ACCOUNT_BILLING_TIME_OVER:			// 빌링 사용시간 만료 
			g_dpCertifier.SendError( ERROR_BILLING_TIME_OVER, dpid );
			break;
		case ACCOUNT_BILLING_OTHER_ERROR:		// 빌링 기타 다른 에러 
			g_dpCertifier.SendError( ERROR_BILLING_OTHER_ERROR, dpid );
			break;
		case ACCOUNT_BILLING_DISCONNECTED:
			g_dpCertifier.SendError( ERROR_BILLING_DISCONNECTED, dpid );
			break;
		case ACCOUNT_IPCUT_ADDR:
			g_dpCertifier.SendError( ERROR_IP_CUT, dpid );
			break;
		default:
			{
				Error("CDPAccountClient::OnAddAccount result:%d ", cbResult );
				g_CertUserMng.PrintUserAccount(dpid);
				break;
			}
	}
}

void CDPAccountClient::OnDestroyPlayer( CAr & ar, DPID dpid )
{
	g_dpCertifier.DestroyPlayer( dpid );
}

#include <span>

void CDPAccountClient::OnServersetList( CAr & ar, DPID dpid )
{
	ar.ReadString( g_dpCertifier.m_szVer );
#ifdef __SECURITY_0628
	ar.ReadString( g_dpCertifier.m_szResVer );
#endif	// __SECURITY_0628

	ar >> g_dpCertifier.m_servers;
	g_dpCertifier.m_2ServersetPtr = GetUpdatedServerSet(g_dpCertifier.m_servers);
}

void CDPAccountClient::OnPlayerCount( CAr & ar, DPID dpid )
{
	u_long uId;
	long lCount;
	ar >> uId >> lCount;

	const auto i2	= g_dpCertifier.m_2ServersetPtr.find( uId );
	if( i2 != g_dpCertifier.m_2ServersetPtr.end() )
		InterlockedExchange( &i2->second->lCount, lCount );
}

void CDPAccountClient::OnEnableServer( CAr & ar, DPID dpid )
{
	u_long uId;
	long lEnable;
	ar >> uId >> lEnable;

	const auto i2	= g_dpCertifier.m_2ServersetPtr.find( uId );
	if( i2 != g_dpCertifier.m_2ServersetPtr.end() )
		InterlockedExchange( &i2->second->lEnable, lEnable );
}

CDPAccountClient	g_dpAccountClient;