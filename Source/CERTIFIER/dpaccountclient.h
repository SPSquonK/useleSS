#pragma once

#include "dpmng.h"
#include "msghdr.h"

class CDPAccountClient : public CDPMng
{
private:
	DPMngFeatures::PacketHandler<CDPAccountClient, DPID> m_handlers;

public:
//	Constructions
	CDPAccountClient();
//	Overrides
	void SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) override {}
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations


	struct CDPAccountClient_SendAddAccount_Params {
		char * lpAddr;
		LPCTSTR lpszAccount;
		BYTE cbAccountFlag;
		DPID idFrom;
		int fCheck;

#ifdef __GPAUTH_02
		const char * szCheck;
#ifdef __EUROPE_0514
		const char * szBak;
		DWORD dwPCBangClass;
#endif	// __EUROPE_0514
#endif	// __GPAUTH_02
	};

	void SendAddAccount(DPID idFrom, const CDPAccountClient_SendAddAccount_Params & params);
	void	SendRemoveAccount( DPID idFrom );
	void	SendRoute( DWORD dwIdofServer, u_long uIdofMulti, DPID idFrom );
	void	SendPing( DPID idFrom );
	void	SendCloseExistingConnection( const char* lpszAccount );

private:
//	Handlers
	void	OnAddAccount( CAr & ar, DPID dpid );
	void	OnDestroyPlayer( CAr & ar, DPID dpid );
	void	OnServersetList( CAr & ar, DPID dpid );
	void	OnPlayerCount( CAr & ar, DPID dpid );

	void	OnEnableServer( CAr & ar, DPID dpid );
};

extern CDPAccountClient g_dpAccountClient;
