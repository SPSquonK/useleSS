#pragma once 

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>
#include <span>
#include "ListedServer.h"
#include "sqktd/mutexed_object.h"

class CDPCertifier : public CDPMng,
	public DPMngFeatures::PacketHandler<CDPCertifier, DPID>
{
public:
	sqktd::mutexed_on_write_object<CListedServers> m_servers;

	char	m_szVer[32]     = "";
#ifdef __SECURITY_0628
	char	m_szResVer[100] = "";
#endif	// __SECURITY_0628
public:
//	Constructions
	CDPCertifier();
	~CDPCertifier() override = default;
// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );

	struct SendServerListArgs {
		DWORD dwAuthKey;
		BYTE cbAccountFlag;
		long lTimeSpan;
#ifdef __GPAUTH_01
		const char * szGPotatoNo;
#ifdef __GPAUTH_02
		const char * szCheck;
#ifdef __EUROPE_0514
		const char * szBak;
#endif
#endif
#endif
	};

	void	SendServerList(DPID dpid, SendServerListArgs args);

	void	SendError( LONG lError, DPID dpid );
#ifdef __GPAUTH
	void	SendErrorString( const char* szError, DPID dpid );
#endif	// __GPAUTH


private:
	// Handlers
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnCertify( CAr & ar, DPID dpid );
	void	OnPing( CAr & ar, DPID dpid );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid );
	void	OnKeepAlive( CAr & ar, DPID dpid );
	void	OnError( CAr & ar, DPID dpid );
};

extern CDPCertifier g_dpCertifier;
