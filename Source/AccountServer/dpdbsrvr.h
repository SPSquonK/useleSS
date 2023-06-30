#pragma once

#include "dpmng.h"
#include "msghdr.h"

class CAccount;

class CDPDBSrvr : public CDPMng,
	public DPMngFeatures::BroadcastPacketNone<CDPDBSrvr>,
	public DPMngFeatures::PacketHandler<CDPDBSrvr, DPID, BYTE *, u_long>
{
public:
//	Constructions
	CDPDBSrvr();

//	Overrides
	void	SysMessageHandler ( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId ) override;
	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId ) override;

// Operations
	void	SendOneHourNotify( CAccount* pAccount );
	void	SendPlayerCount( void );
	void	OnRemoveConnection( DPID dpid );
	void	SendCloseExistingConnection( const char*lpszAccount, LONG lError );
	void	SendFail( long lError, const char* lpszAccount, DPID dpid );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2 );
	void	SendLogSMItem( );


private:
//	Handlers
	void	OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveAccount( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnJoin( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveAllAccounts( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnBuyingInfo( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnServerEnable( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );

#ifdef __REMOVE_PLAYER_0221
	void	OnRemovePlayer(CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize);
#endif	// __REMOVE_PLAYER_0221

};

extern CDPDBSrvr g_dpDbSrvr;
