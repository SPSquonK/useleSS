#pragma once

#include "DPMng.h"
#include "MsgHdr.h"

class CDPCacheSrvr : public CDPMng
{
public:
	// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

private:
	// Handlers
	void	OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveConnection( DPID dpid );
	void	OnKeepAlive( CAr & ar, DPID dpid, LPBYTE, u_long );
};

extern CDPCacheSrvr g_DPCacheSrvr;
