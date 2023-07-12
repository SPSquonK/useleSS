#pragma once

#include <map>
#include <mutex>
#include <dplay.h>

class CPacketLog final
{
public:
	struct PacketRecvd {
		std::uint64_t dwPackets = 0;
		std::uint64_t dwBytes = 0;

		void AddPacket(std::uint64_t bytes);
		PacketRecvd & operator+=(const PacketRecvd & other);
	};

	struct PlayerRecvd {
		std::uint64_t dwTotalBytes = 0;
		std::uint64_t dwTotalPackets = 0;
		std::map<DWORD, PacketRecvd> mpar;

		void AddPacket(std::uint64_t packetId, std::uint64_t bytes) {
			dwTotalPackets += 1;
			dwTotalBytes += bytes;
			mpar[packetId].AddPacket(bytes);
		}
	};

private:
	CPacketLog() = default;
public:
	static	CPacketLog*	Instance();
	void	Add( DPID dpid, DWORD dwHdr, std::uint64_t dwBytes );
	void	Print();
	void	Reset();

private:
	std::map<DPID, PlayerRecvd>	m_players;
	std::mutex m_AccessLock;
};
