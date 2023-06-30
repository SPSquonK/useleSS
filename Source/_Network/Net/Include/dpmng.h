#ifndef __DPMNG_H__
#define __DPMNG_H__

#pragma once

#include "dpsock.h"
#include "ar.h"
#include "mymap.h"
#include <sqktd/flatter_map.hpp>

extern void UninitializeNetLib();
extern BOOL InitializeNetLib();
extern	void	TestNetLib( const char* lpAddr, u_short uPort );

#define LOAD_WS2_32_DLL	\
	WSADATA wsaData;	\
	int err;	\
	err	= WSAStartup( 0x0202, &wsaData );	\
	if( err == SOCKET_ERROR )	\
	{	\
		TRACE( "WSAStartup() failed with error %ld\n", WSAGetLastError() );	\
		return FALSE;	\
	}

#define UNLOAD_WS2_32_DLL	\
	WSACleanup();

#define BEFORESEND( ar, dw )	\
	CAr ar;	\
	int nBufSize;	\
	ar << dw;

#define	BEFORESENDSOLE( ar, dw, dpid_ )	\
	CAr ar;		\
	int nBufSize;	\
	ar << dpid_ << dw;

#define BEFORESENDDUAL( ar, dw, dpid_, dpid__ )	\
	CAr ar;		\
	int nBufSize;	\
	ar << dpid_ << dpid__ << dw;

#define	SEND( ar, pDPMng, idTo ) \
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );	\
	(pDPMng)->Send( (LPVOID)lpBuf, nBufSize, idTo );


class CDPMng
{
private:
	CDPSock*	m_pDPSock;
	HANDLE	m_hRecvThread;
	HANDLE	m_hClose;
public:
//	Constructions
	CDPMng();
	virtual	~CDPMng();
//	Operations
	HANDLE	GetRecvHandle( void );
	HANDLE	GetCloseHandle( void );
	BOOL	CreateDPObject( BOOL bSingleThreaded = FALSE );
	BOOL	DeleteDPObject( void );
	BOOL	InitConnection( LPVOID lpConnection, u_short uPort = 0 );
	BOOL	ReceiveMessage( void );

#ifdef __CRC
	BOOL	CreateSession( LPCSTR lpSession, DWORD dwcrc = 0 );
	BOOL	JoinSession( LPCSTR lpSession, DWORD dwcrc = 0, u_long uWaitingTime = 10000 );
	BOOL	StartServer( u_short uPort, BOOL bSingleThreaded = FALSE, DWORD dwcrc = 0 );
	BOOL	ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded = FALSE, DWORD dwcrc = 0, u_long uWaitingTime = 10000 );
#else	// __CRC
	BOOL	CreateSession( LPCSTR lpSession, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
	BOOL	JoinSession( LPCSTR lpSession, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
	BOOL	StartServer( u_short uPort, BOOL bSingleThreaded = FALSE, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
	BOOL	ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded = FALSE, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
#endif	// __CRC
	BOOL	Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo );
	BOOL	DestroyPlayer( DPID dpid );

#ifdef __INFO_SOCKLIB0516
	DWORD	GetDebugInfo( DPID dpid );
#endif	// __INFO_SOCKLIB0516

	void	GetHostAddr( LPSTR lpAddr );
	void	GetPlayerAddr( DPID dpid, LPSTR lpAddr );
	DWORD	GetPlayerAddr( DPID dpid );


//	Override
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )		= 0;
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )	= 0;
};

inline HANDLE CDPMng::GetRecvHandle( void )
{	
	return	m_pDPSock->GetRecvHandle();	
}

inline HANDLE CDPMng::GetCloseHandle( void )
{	
	return m_hClose;	
}

#ifdef __CRC
	inline BOOL CDPMng::StartServer( u_short uPort, BOOL bSingleThreaded, DWORD dwcrc )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)NULL, uPort ) && 
				CreateSession( NULL, dwcrc ) );	
	}
	inline BOOL CDPMng::ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded, DWORD dwcrc, u_long uWaitingTime )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)lpConnection, uPort ) && 
				JoinSession( NULL, dwcrc, uWaitingTime ) );	
	}
#else	
	inline BOOL CDPMng::StartServer( u_short uPort, BOOL bSingleThreaded, BUFFER_TYPE nBufferType )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)NULL, uPort ) && 
				CreateSession( NULL, nBufferType ) );	
	}
	inline BOOL CDPMng::ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded, BUFFER_TYPE nBufferType )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)lpConnection, uPort ) && 
				JoinSession( NULL, nBufferType ) );	
	}
#endif	// __CRC

inline BOOL CDPMng::Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo )
{	
	if( !m_pDPSock )
		return FALSE;
	return m_pDPSock->Send( (char*)lpData, dwDataSize, dpidTo );
}

namespace DPMngFeatures {

template<typename Derived>
class SendPacketNone {
public:
	template<DWORD PacketId, typename ... Ts>
	void SendPacket(const Ts & ... ts) {
		BEFORESEND(ar, PacketId);
		ar.Accumulate(ts...);
		SEND(ar, static_cast<Derived *>(this), DPID_SERVERPLAYER);
	}
};

template<typename Derived>
class SendPacketSole {
public:
	template<DWORD PacketId, typename ... Ts>
	void SendPacket(const Ts & ... ts) {
		BEFORESENDSOLE(ar, PacketId, DPID_UNKNOWN);
		ar.Accumulate(ts...);
		SEND(ar, static_cast<Derived *>(this), DPID_SERVERPLAYER);
	}
};

template<typename Derived>
class SendPacketDual {
public:
	template<DWORD PacketId, typename ... Ts>
	void SendPacket(const Ts & ... ts) {
		BEFORESENDDUAL(ar, PacketId, DPID_UNKNOWN, DPID_UNKNOWN);
		ar.Accumulate(ts...);
		SEND(ar, static_cast<Derived *>(this), DPID_SERVERPLAYER);
	}
};


template<typename Derived>
class BroadcastPacketNone {
public:
	template<DWORD PacketId, typename ... Ts>
	void BroadcastPacket(const Ts & ... ts) {
		BEFORESEND(ar, PacketId);
		ar.Accumulate(ts...);
		SEND(ar, static_cast<Derived *>(this), DPID_ALLPLAYERS);
	}
};

template<typename Derived>
class BroadcastPacketSole {
public:
	template<DWORD PacketId, typename ... Ts>
	void BroadcastPacket(const Ts & ... ts) {
		BEFORESENDSOLE(ar, PacketId, DPID_ALLPLAYERS);
		ar.Accumulate(ts...);
		SEND(ar, static_cast<Derived *>(this), DPID_ALLPLAYERS);
	}
};

template<typename Derived>
class BroadcastPacketDual {
public:
	template<DWORD PacketId, typename ... Ts>
	void BroadcastPacket(const Ts & ... ts) {
		BEFORESENDDUAL(ar, PacketId, DPID_ALLPLAYERS, DPID_ALLPLAYERS);
		ar.Accumulate(ts...);
		SEND(ar, static_cast<Derived *>(this), DPID_ALLPLAYERS);
	}
};

// Packet Handler Component: give it a list of handlers, and call them from the id.
template<typename Self, typename ... ExtraTypes>
class PacketHandlerComponent {
public:
	using Handler = void (Self :: *)(CAr &, ExtraTypes...);

private:
	sqktd::flatter_map<DWORD, Handler> m_handlers;

public:
	void Add(DWORD packetId, Handler handler) {
		m_handlers.emplace(packetId, handler);
	}

	bool Handle(Self * self, CAr & ar, DWORD packetId, ExtraTypes ... extra) {
		const auto handler = m_handlers.get_at(packetId);

		if (handler) {
			((*self).*(*handler))(ar, extra...);
			return true;
		} else {
			return false;
		}
	}
};


// CRTP PacketHandler with an ON_MSG method (for backward compatibility)
template<typename Self, typename ... ExtraTypes>
class PacketHandler {
private:
	PacketHandlerComponent<Self, ExtraTypes...> m_handlers;
public:
	void ON_MSG(DWORD packetId, typename PacketHandlerComponent<Self, ExtraTypes...>::Handler handler) {
		m_handlers.Add(packetId, handler);
	}

	bool Handle(CAr & ar, DWORD packetId, ExtraTypes ... extra) {
		return m_handlers.Handle(static_cast<Self *>(this), ar, packetId, extra...);
	}
};

}

#endif //__DPMNG_H__