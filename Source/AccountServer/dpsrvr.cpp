#include "stdafx.h"
#include "dpdbsrvr.h"

#ifdef __GPAUTH_01
#include "lang.h"
#endif	// __GPAUTH_01

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

	m_handlers.Add(PACKETTYPE_ADD_ACCOUNT, &CDPSrvr_AccToCert::OnAddAccount);
	m_handlers.Add(PACKETTYPE_REMOVE_ACCOUNT, &CDPSrvr_AccToCert::OnRemoveAccount);
	m_handlers.Add(PACKETTYPE_PING, &CDPSrvr_AccToCert::OnPing);
	m_handlers.Add( PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPSrvr_AccToCert::OnCloseExistingConnection );
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
	static constexpr size_t	nSize = sizeof(DPID);
	
	LPBYTE lpBuffer		= (LPBYTE)lpMsg + nSize;
	u_long uBufSize		= dwMsgSize - nSize;

	CAr ar( lpBuffer, uBufSize );
	DWORD dw; ar >> dw;
	m_handlers.Handle(this, ar, dw, idFrom, *(UNALIGNED LPDPID)lpMsg);

	if (ar.IsOverflow()) Error("Account-Certifier: Packet %08x overflowed", dw);
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

	if( lpAddr[0] == '\0' || IsBanned(lpAddr) )
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

std::optional<std::uint32_t> CDPSrvr_AccToCert::IPStringToUInt32(const char * string) {
	std::uint32_t result = 0;


	size_t numberOfDots = 0;
	size_t i = 0;

	std::uint32_t accumulator = 0;

	while (string[i] != '\0') {
		if (string[i] >= '0' && string[i] <= '9') {
			accumulator = accumulator * 10 + (string[i] - '0');
		} else if (string[i] == '.') {
			if (accumulator > 0xFF) return std::nullopt;
			result = result * 0x100 + accumulator;
			
			accumulator = 0;
			++numberOfDots;
		} else {
			return std::nullopt;
		}

		++i;
	}

	if (numberOfDots != 3) return std::nullopt;
	if (accumulator > 0xFF) return std::nullopt;

	return result + accumulator;
}

bool CDPSrvr_AccToCert::LoadIPCut( LPCSTR lpszFileName ) {
	bool result = false;

	m_IPCut.access([&](std::vector<IPRange> & ipRange) {
		m_IPCutIsEmpty = false;
		ipRange.clear();

		CScanner s;
		if (!s.Load(lpszFileName)) {
			result = false;
			m_IPCutIsEmpty = true;
			return;
		}
		
		result = true;

		s.GetToken();
		while (s.tok != FINISHED) {

			std::optional<std::uint32_t> from = IPStringToUInt32(s.Token.GetString());
			if (!from) {
				result = false;
				Error("AccountServer LoadIPCut : Invalid value read %s", s.Token.GetString());
			}
			s.GetToken();
			s.GetToken();
			std::optional<std::uint32_t> to = IPStringToUInt32(s.Token.GetString());
			if (!to) {
				result = false;
				Error("AccountServer LoadIPCut: Invalid value read %s", s.Token.GetString());
			}

			if (from && to) {
				ipRange.emplace_back(IPRange{ from.value(), to.value() });
			}

			s.GetToken();
		}

		m_IPCutIsEmpty = ipRange.empty();
		});

	return result;
}

bool CDPSrvr_AccToCert::IsBanned(LPCSTR lpszIP) {
	const std::optional<std::uint32_t> myIp = IPStringToUInt32(lpszIP);
	if (!myIp) {
		Error("AccountServer IPCut: Invalid IP received for checking %s", lpszIP);
		return false;
	}

	if (m_IPCutIsEmpty) return false;
	// Note: It may be possible that without the if, m_IPCutIsEmpty field would
	// become false when reaching m_IPCut. But the only scenario that it could
	// appear is if a new file is loaded between the previous line and the
	// resource access. This is very unlikely and anyway, people connecting
	// within the milliseconds their IP is banned is bound to happend anyway.

	bool result = false;

	m_IPCut.access([&](const std::vector<IPRange> & ipRange) {
		result = std::any_of(ipRange.begin(), ipRange.end(),
			[ipAddress = *myIp](const IPRange & range) {
				return ipAddress >= range.from && ipAddress <= range.to;
			});	
		});

	return result;
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
