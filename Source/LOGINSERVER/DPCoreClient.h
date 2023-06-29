#pragma once

#include "DPMng.h"

class CDPCoreClient : public CDPMng,
	public DPMngFeatures::SendPacketNone<CDPCoreClient>
{
private:
	DPMngFeatures::PacketHandler<CDPCoreClient> m_handlers;

public:
	CDPCoreClient();

	// Operations
	void SysMessageHandler ( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom ) override;
	void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom ) override;

	void	SendToServer( LPVOID lpMsg, DWORD dwMsgSize )
		{	Send( lpMsg, dwMsgSize, DPID_SERVERPLAYER );	}

	void	SendPreJoin( DWORD dwAuthKey, const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer );
	void	SendLeave( u_long idPlayer );
	void	QueryTickCount( void );
	void	OnQueryTickCount( CAr & ar );

private:
	// Handlers
	void	OnPreJoin( CAr & ar );	// result
	void	OnQueryRemovePlayer( CAr & ar );
};

extern CDPCoreClient g_dpCoreClient;
