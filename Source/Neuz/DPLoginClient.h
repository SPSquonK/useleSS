#ifndef __DPLOGINCLIENT_H__
#define __DPLOGINCLIENT_H__

#include "DPMng.h"
#include "MsgHdr.h"

namespace MoverSub {
	struct SkinMeshs;
}

class CDPLoginClient : public CDPMng,
	public DPMngFeatures::SendPacketSole,
	public DPMngFeatures::PacketHandler<CDPLoginClient>
{
private:
	BOOL	m_fDestroy; 
	BOOL	m_fConn;
	CTimer	m_timer;
	BOOL	m_bShowDisconnectMsg;
	LONG	m_lError;
	u_long	m_idNumberPad;
public:
	int		m_nSlot;

//	Constructions
	CDPLoginClient();
	virtual	~CDPLoginClient();
//	Override
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

//	Operations
	LONG	GetErrorCode() { return m_lError; }
	LONG	GetNetError();
	void	Destroy( BOOL fDestroy );
	BOOL	IsDestroyed( void );
	BOOL	IsConnected( void );
	void	Ping( void );
	void	QueryTickCount( void );
	void	SendGetPlayerList( DWORD dwID, LPCSTR lpszAccount, LPCSTR lpszpw );
	void	SendCreatePlayer( BYTE nSlot, LPCSTR lpszPlayer, const MoverSub::SkinMeshs & skin, BYTE nCostume, DWORD dwHairColor, BYTE nSex, BYTE nJob, int nBankPW = 0 );
	void	SendDeletePlayer( BYTE nSlot, LPCTSTR szNo );
	void	SendPreJoin( const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer, int nSlot, int nSecretNum = 0 );
	void	OnPreJoin( CAr & ar );

private:
//	Handlers
	void	OnPlayerList( CAr & ar );
	void	OnError( CAr & ar );
	void	OnCacheAddr( CAr & ar );
	void	OnQueryTickCount( CAr & ar );
	void	OnOneHourNotify( CAr & ar );

	void	OnPing( CAr & ar );

	void	OnLoginNumPad( CAr & ar );
	void	OnLoginProtect( CAr & ar );
public:
	u_long	GetNumberPad( void ) const;
};

inline void CDPLoginClient::Destroy( BOOL fDestroy )
	{	m_fDestroy	= fDestroy;	}
inline BOOL CDPLoginClient::IsDestroyed( void )
	{	return( m_fDestroy );	}
inline BOOL CDPLoginClient::IsConnected( void )
	{	return( m_fConn );	}

inline void CDPLoginClient::Ping( void )
{
	if( m_timer.IsTimeOut() ) {
		m_timer.Reset();
		BEFORESENDSOLE( ar, PACKETTYPE_PING, DPID_UNKNOWN );
		DWORD dwPingTime = GetCurrentTime();
		ar << dwPingTime;
		++g_Neuz.m_nPingCount;
		if( g_Neuz.m_nPingCount > 1)
		{
			g_Neuz.m_dwPingTime = 0;
		}
		SEND( ar, this, DPID_SERVERPLAYER );
	}

	if( m_fDestroy ) {
		DeleteDPObject();
		m_fDestroy	= FALSE;
	}
}

extern CDPLoginClient g_dpLoginClient;

#endif	// __DPLOGINCLIENT_H__