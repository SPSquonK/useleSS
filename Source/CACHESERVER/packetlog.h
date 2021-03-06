#ifndef __PACKET_LOG_H
#define __PACKET_LOG_H

#include <map>
#include <dplay.h>

typedef struct	_PacketRecvd
{
	DWORD	dwHdr;
	DWORD	dwPackets;
	DWORD	dwBytes;
	_PacketRecvd( DWORD h, DWORD dwRecvd ) : dwHdr( h ), dwPackets( 1 ), dwBytes( dwRecvd )		{}
}	PacketRecvd;

typedef	std::map<DWORD, PacketRecvd>	MPAR;

typedef struct	_PlayerRecvd
{
	DPID	dpid;
	DWORD dwTotalBytes;
	DWORD dwTotalPackets;
	MPAR mpar;
	_PlayerRecvd( DPID d, DWORD dwBytes, MPAR & m ) : dpid( d ), dwTotalBytes( dwBytes ), dwTotalPackets( 1 ), mpar( m )	{}
} PlayerRecvd;

typedef std::map<DPID, PlayerRecvd>	MPLR;
typedef std::vector<PacketRecvd>	VPAR;
typedef std::vector<PlayerRecvd>	VPLR;

class CPacketLog
{
private:
	CPacketLog();
public:
	virtual	~CPacketLog();
	static	CPacketLog*	Instance();
	void	Add( DPID dpid, DWORD dwHdr, DWORD dwBytes );
	void	Print();
	void	Reset();

private:
	MPLR	m_players;
	CMclCritSec		m_AccessLock;
};

#endif	// __PACKET_LOG_H