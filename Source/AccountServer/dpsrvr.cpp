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
	
	ON_MSG(PACKETTYPE_ADD_ACCOUNT, &CDPSrvr_AccToCert::OnAddAccount);
	ON_MSG(PACKETTYPE_REMOVE_ACCOUNT, &CDPSrvr_AccToCert::OnRemoveAccount);
	ON_MSG(PACKETTYPE_PING, &CDPSrvr_AccToCert::OnPing);
	ON_MSG(PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPSrvr_AccToCert::OnCloseExistingConnection);
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
	Handle(ar, dw, idFrom, *(UNALIGNED LPDPID)lpMsg);

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

bool CDPSrvr_AccToCert::LoadIpManager(LPCTSTR addrPmttd, LPCSTR ipCut) {
	bool res = false;
	m_IPManager.access([&](IPManager & manager) {
		m_IPManagerEmpty = false;
		res = manager.Load(addrPmttd, ipCut);
		m_IPManagerEmpty = manager.IsEmpty();
	});
	return res;
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

	// 1. 외부 아이피 검사 
	BYTE cbResult;
	
	if (lpAddr[0] != '\0') {
		if (m_IPManagerEmpty) {
			cbResult = m_bCheckAddr ? ACCOUNT_EXTERNAL_ADDR : ACCOUNT_CHECK_OK;
		} else {
			m_IPManager.access([&](const IPManager & manager) {
				cbResult = manager.GetStatus(lpAddr, !m_bCheckAddr);
			});
		}
	} else {
		cbResult = ACCOUNT_IPCUT_ADDR;
		Error("Not Addr : %s / %s", lpAddr, lpszAccount);
	}
	
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

void CDPSrvr_AccToCert::SendPlayerCount(u_long uId, long lCount) {
	BroadcastPacket<PACKETTYPE_PLAYER_COUNT, u_long, long>(uId, lCount);
}

void CDPSrvr_AccToCert::SendEnableServer(u_long uId, long lEnable) {
	BroadcastPacket<PACKETTYPE_ENABLE_SERVER, u_long, long>(uId, lEnable);
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

///////////////////////////////////////////////////////////////////////////////


BYTE CDPSrvr_AccToCert::IPManager::GetStatus(LPCTSTR ipAddress, bool onlyBan) const {
	// Banned
	const std::optional<std::uint32_t> myIp = IPStringToUInt32(ipAddress);
	if (!myIp) {
		Error("AccountServer IPCut: Invalid IP received for checking %s", ipAddress);
		return ACCOUNT_IPCUT_ADDR;
	}

	const bool isBanned = std::any_of(banned.begin(), banned.end(),
		[ipAddress = *myIp](const IPRange & range) {
			return ipAddress >= range.from && ipAddress <= range.to;
		}
	);

	if (isBanned) return ACCOUNT_IPCUT_ADDR;

	// Permitted
	if (onlyBan) return ACCOUNT_CHECK_OK;
	
	const bool notPrefixedWithPermitted = std::ranges::none_of(permitted,
		[&](const CString & str) {
			return std::strstr(ipAddress, str.GetString()) == 0;
		}
	);

	return notPrefixedWithPermitted ? ACCOUNT_EXTERNAL_ADDR : ACCOUNT_CHECK_OK;
}

bool CDPSrvr_AccToCert::IPManager::IsEmpty() const noexcept {
	return permitted.empty() && banned.empty();
}


bool CDPSrvr_AccToCert::IPManager::Load(LPCTSTR szAddrPmttdFilename, LPCSTR szIpCutFilename) {
	// Always load both files
	const bool pmttd = LoadAddrPmttd(szAddrPmttdFilename);
	const bool ipCut = LoadIPCut(szIpCutFilename);
	return pmttd || ipCut;
}

bool CDPSrvr_AccToCert::IPManager::LoadAddrPmttd(LPCTSTR szAddrPmttdFilename) {
	permitted.clear();

	CScanner s;
	if (!s.Load(szAddrPmttdFilename)) return false;

	while (true) {
		s.GetToken();
		if (s.tok == FINISHED) break;
		permitted.emplace_back(s.Token);
	}

	return true;
}

std::optional<std::uint32_t> CDPSrvr_AccToCert::IPManager::IPStringToUInt32(const char * string) {
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

bool CDPSrvr_AccToCert::IPManager::LoadIPCut(LPCSTR szIpCutFilename) {
	banned.clear();

	CScanner s;
	if (!s.Load(szIpCutFilename)) {
		return false;
	}

	bool result = false;

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
			banned.emplace_back(IPRange{ from.value(), to.value() });
		}

		s.GetToken();
	}

	return result;
}

