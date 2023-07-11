#include "stdafx.h"
#include "dpcertifier.h"
#include "dpaccountclient.h"
#ifdef __GPAUTH_01
#include "lang.h"
#endif	// __GPAUTH_01

#include "user.h"

CDPAccountClient::CDPAccountClient()
{
	ON_MSG( PACKETTYPE_ADD_ACCOUNT, &CDPAccountClient::OnAddAccount );
	ON_MSG( PACKETTYPE_DESTROY_PLAYER, &CDPAccountClient::OnDestroyPlayer );
	ON_MSG( PACKETTYPE_SRVR_LIST, &CDPAccountClient::OnServersetList );
	ON_MSG( PACKETTYPE_PLAYER_COUNT, &CDPAccountClient::OnPlayerCount );
	ON_MSG( PACKETTYPE_ENABLE_SERVER, &CDPAccountClient::OnEnableServer );
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static constexpr size_t	nSize	= sizeof(DPID);

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpid2	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	DWORD dw; ar >> dw;

	if (Handle(ar, dw, dpid2)) {
		if (ar.IsOverflow()) Error("Certifier-Account: Packet %08x overflowed", dw);
	}
}



void CDPAccountClient::SendAddAccount(DPID idFrom, const CDPAccountClient_SendAddAccount_Params & params) {
	BEFORESENDSOLE( ar, PACKETTYPE_ADD_ACCOUNT, idFrom );
	
	ar.WriteString( params.lpAddr );
	ar.WriteString( params.lpszAccount );
	ar << params.cbAccountFlag;

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
#ifdef __EUROPE_0514
			if (!g_CertUserMng.AccountCheckOk(dpid, szBak)) {
				return;
			}
#endif

			CDPCertifier::SendServerListArgs args;
			args.dwAuthKey = dwAuthKey;
			args.cbAccountFlag = cbAccountFlag;
			args.lTimeSpan = lTimeSpan;

#ifdef __GPAUTH_01
			args.szGPotatoNo = szGPotatoNo;
#ifdef __GPAUTH_02
			args.szCheck = szCheck;
#ifdef __EUROPE_0514
			args.szBak = szBak;
#endif
#endif
#endif

			g_dpCertifier.SendServerList(dpid, args);
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

void CDPAccountClient::OnServersetList(CAr & ar, DPID) {
	ar.ReadString(g_dpCertifier.m_szVer);
#ifdef __SECURITY_0628
	ar.ReadString(g_dpCertifier.m_szResVer);
#endif	// __SECURITY_0628

	g_dpCertifier.m_servers.write([&](auto & servers) { ar >> servers; });
}

void CDPAccountClient::OnPlayerCount(CAr & ar, DPID ) {
	const auto [uId, lCount] = ar.Extract<u_long, long>();

	g_dpCertifier.m_servers.write([&](CListedServers & servers) {
		if (CListedServers::Channel * channel = servers.GetFromUId(uId)) {
			channel->lCount = lCount;
		}
		});
}

void CDPAccountClient::OnEnableServer(CAr & ar, DPID) {
	const auto [uId, lEnable] = ar.Extract<u_long, long>();

	g_dpCertifier.m_servers.write([&](CListedServers & servers) {
		if (CListedServers::Channel * channel = servers.GetFromUId(uId)) {
			channel->lEnable = lEnable;
		}
		});
}

CDPAccountClient	g_dpAccountClient;
