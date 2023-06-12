#include "stdafx.h"
#include "packetlog.h"
#include <vector>
#include <algorithm>
#include <tuple>


///////////////////////////////////////////////////////////////////////////////

void CPacketLog::PacketRecvd::AddPacket(std::uint64_t bytes) {
	dwPackets += 1;
	dwBytes += bytes;
}

CPacketLog::PacketRecvd & CPacketLog::PacketRecvd::operator+=(const CPacketLog::PacketRecvd & other) {
	dwPackets += other.dwPackets;
	dwBytes += other.dwBytes;
	return *this;
}

std::map<DWORD, CPacketLog::PacketRecvd> & operator+=(
	std::map<DWORD, CPacketLog::PacketRecvd> & dest,
	const std::map<DWORD, CPacketLog::PacketRecvd> & src
	) {

	for (const auto & [packetId, packetRecvd] : src) {
		dest[packetId] += packetRecvd;
	}

	return dest;
}

static bool pllpr2(
	const std::pair<DWORD, CPacketLog::PacketRecvd> & l,
	const std::pair<DWORD, CPacketLog::PacketRecvd> & r
) {
	return std::tuple(l.second.dwBytes, l.second.dwPackets)
		> std::tuple(r.second.dwBytes, r.second.dwPackets);
}


///////////////////////////////////////////////////////////////////////////////


CPacketLog * CPacketLog::Instance() {
	static CPacketLog	sPacketLog;
	return &sPacketLog;
}

void CPacketLog::Add(DPID dpid, DWORD dwHdr, std::uint64_t dwBytes) {
	std::lock_guard lg(m_AccessLock);
	m_players[dpid].AddPacket(dwHdr, dwBytes);
}

void CPacketLog::Reset() {
	std::lock_guard lg(m_AccessLock);
	m_players.clear();
	Error("[CPacketLog.Reset]");
}

static bool pllpr1(
	const std::pair<DPID, CPacketLog::PlayerRecvd> & l,
	const std::pair<DPID, CPacketLog::PlayerRecvd> & r
) {
	return std::tuple(l.second.dwTotalBytes, l.second.dwTotalPackets, l.first)
		> std::tuple(r.second.dwTotalBytes, r.second.dwTotalPackets, r.first);
}

static float ComputeRatio(std::uint64_t value, std::uint64_t over) {
	if (over == 0) return 0.0f;
	return static_cast<float>(value * 100) / static_cast<float>(over);
}

void CPacketLog::Print() {
	static constexpr size_t MAX_DETAIL_PACKETLOG = 100;

	std::lock_guard lg(m_AccessLock);

	std::vector<std::pair<DPID, PlayerRecvd>> vPlayers;
	DWORD dwTotalBytes	= 0, dwTotalPackets	= 0;
	for (const auto & player : m_players) {
		dwTotalBytes   += player.second.dwTotalBytes;
		dwTotalPackets += player.second.dwTotalPackets;
		vPlayers.push_back(player);
	}
	std::sort(vPlayers.begin(), vPlayers.end(), pllpr1);

	Error( "\r\n\r\nTotalBytesReceived: %llu(bytes)\t%llu(packets)", dwTotalBytes, dwTotalPackets );

	FILE * f = fopen("error.txt", "a");
	if (!f) return;

	size_t c = 0;

	for (const auto & [playerDPID, playerPackets] : vPlayers) {
		std::vector<std::pair<DWORD, PacketRecvd>> vPackets;
		for (const auto & packet : playerPackets.mpar) {
			vPackets.emplace_back(packet);
		}
		std::sort(vPackets.begin(), vPackets.end(), pllpr2);

		fprintf( f, "\t(%3.2f%%) %d(socket) %llu(bytes) %llu(packets)\r\n",
			ComputeRatio(playerPackets.dwTotalBytes, dwTotalBytes),
			playerDPID,
			playerPackets.dwTotalBytes, playerPackets.dwTotalPackets
		);
		
		if( c++ < MAX_DETAIL_PACKETLOG ) {
			for (const auto & [packetId, packetStats] : vPackets) {
				fprintf(f, "\t\t(%3.2f%%)\t%08X: %llu(bytes) %llu(packets)\r\n",
					ComputeRatio(packetStats.dwBytes, playerPackets.dwTotalBytes),
					packetId,
					packetStats.dwBytes, packetStats.dwPackets
				);
			}
		}
	}

	fclose(f);
}