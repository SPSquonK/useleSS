#pragma once

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>
#include "ListedServer.h"
#include "sqktd/mutexed_object.h"

class CDPSrvr_AccToCert : public CDPMng, 
	public DPMngFeatures::BroadcastPacketSole<CDPSrvr_AccToCert>
{
private:
	DPMngFeatures::PacketHandler<CDPSrvr_AccToCert, DPID, DPID> m_handlers;
public:
	static constexpr size_t MAX_IP = 10240;

	bool	m_bCheckAddr;		// 접속하는 account의 address를 검사해야 하는가?
	int		m_nMaxConn;
	BOOL	m_bReloadPro;
	CMclCritSec		m_csAddrPmttd;
	char	m_sAddrPmttd[128][16];
	int		m_nSizeofAddrPmttd;
	

	struct IPRange { std::uint32_t from; std::uint32_t to; };
	sqktd::mutexed_object<std::vector<IPRange>> m_IPCut;
	bool m_IPCutIsEmpty = false; // If true, m_ipCut is guaranteed to be empty

	sqktd::mutexed_on_write_object<CListedServers> m_servers;

	char	m_szVer[32]     = "20040706";
#ifdef __SECURITY_0628
	char	m_szResVer[100] = "";
#endif	// __SECURITY_0628

public:
//	Constructions
	CDPSrvr_AccToCert();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
// Operations
	void	CloseExistingConnection( LPCTSTR lpszAccount, LONG lError );
	bool	LoadAddrPmttd( LPCTSTR lpszFileName );
	bool	LoadIPCut( LPCSTR lpszFileName );
	void	DestroyPlayer( DPID dpid1, DPID dpid2 );

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	SendServersetList( DPID dpid );
	void	SendPlayerCount( u_long uId, long lCount );

	BOOL	EnableServer( DWORD dwParent, DWORD dwID, long lEnable );
	void	SendEnableServer( u_long uId, long lEnable );
	
	struct OnAfterCheckingParams {
		BYTE f;
		LPCTSTR lpszAccount;
		DWORD dwAuthKey;
		BYTE cbAccountFlag;
		long lTimeSpan;
#ifdef __GPAUTH_02
		const char * szCheck;
#ifdef __EUROPE_0514
		const char * szBak;
#endif
#endif
	};

	void OnAfterChecking(DPID dpid1, DPID dpid2, const OnAfterCheckingParams & params);

	static std::optional<std::uint32_t> IPStringToUInt32(const char * string);
//	Handlers
private:
	bool IsBanned(LPCSTR lpszIP);

private:
	void	OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnRemoveAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnPing( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid1, DPID dpid2 );
};

extern CDPSrvr_AccToCert g_dpSrvr;

