#pragma once

#include "DPMng.h"
#include "MsgHdr.h"

#undef	theClass
#define theClass	CDPLoginSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID

class CDPLoginSrvr : public CDPMng,
	public DPMngFeatures::BroadcastPacketNone<CDPLoginSrvr>
{
private:
	DPMngFeatures::PacketHandler<CDPLoginSrvr, DPID> m_handlers;

public:
	void SendCacheAddr(DPID dpid);

public:
//	Constructions
	CDPLoginSrvr();
	
//	Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendError( LONG lError, DPID dpid );
	void	SendHdr( DWORD dwHdr, DPID dpid );

private:
//	Handlers
	void	OnAddConnection( CAr & ar, DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnPreJoin( CAr & ar, DPID dpid );
	void	OnPing( CAr & ar, DPID dpid );
	void	OnQueryTickCount( CAr & ar, DPID dpid );

	void	SendNumPadId( DPID dpid );
public:
	void	SendLoginProtect( BOOL bLogin, DPID dpid );
};

extern CDPLoginSrvr g_dpLoginSrvr;
