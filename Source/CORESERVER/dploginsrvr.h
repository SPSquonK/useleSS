#pragma once

#include "DPMng.h"

class CDPLoginSrvr : public CDPMng,
	public DPMngFeatures::BroadcastPacketNone<CDPLoginSrvr>,
	public DPMngFeatures::PacketHandler<CDPLoginSrvr>
{
public:
//	Constructions
	CDPLoginSrvr();
	virtual	~CDPLoginSrvr();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations

	void	SendQueryRemovePlayer( const CHAR* lpszAccount );
//	Handlers
private:
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	OnQueryTickCount( CAr & ar );
	void	OnPreJoin( CAr & ar );
	void	OnLeave( CAr & ar );
};

extern	CDPLoginSrvr	g_dpLoginSrvr;
