#pragma once

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>
#include "ListedServer.h"
#include "sqktd/mutexed_object.h"

class CDPSrvr_AccToCert : public CDPMng, 
	public DPMngFeatures::BroadcastPacketSole<CDPSrvr_AccToCert>,
	public DPMngFeatures::PacketHandler<CDPSrvr_AccToCert, DPID, DPID>
{
public:
	static constexpr size_t MAX_IP = 10240;

	int		m_nMaxConn;
	BOOL	m_bReloadPro;

	class IPManager {
	public:
		struct IPRange { std::uint32_t from; std::uint32_t to; };
		static std::optional<std::uint32_t> IPStringToUInt32(const char * string);

	private:
		std::vector<CString> permitted;
		std::vector<IPRange> banned;
	public:
		[[nodiscard]] BYTE GetStatus(LPCSTR ipAddress, bool onlyBan) const;
		
		[[nodiscard]] bool IsEmpty() const noexcept;
		bool Load(LPCTSTR addrPmttd, LPCSTR ipCut);
	private:
		bool LoadAddrPmttd(LPCTSTR szAddrPmttdFilename);
		bool LoadIPCut(LPCTSTR szIpCutFilename);
	};

	bool	m_bCheckAddr;		// If true, only IPManager::permitted IP addresses are allowed
	sqktd::mutexed_object<IPManager> m_IPManager;
	bool m_IPManagerEmpty = false; // If true, m_IPManager is guaranteed to be empty

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
	
	bool	LoadIpManager(LPCTSTR addrPmttd, LPCSTR ipCut);
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

//	Handlers
private:
	void	OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnRemoveAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnPing( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid1, DPID dpid2 );
};

extern CDPSrvr_AccToCert g_dpSrvr;

