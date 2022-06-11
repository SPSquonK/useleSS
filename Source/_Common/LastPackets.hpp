#pragma once

#include "ar.h"
#include <mutex>

/// @SPSquonK, 2022-06, Last received packets memorizer
///
/// Stores in memory the last processed packets, without any allocation. 
/// Intended to be used in conjunction with a debugger
/// 
/// Released under the SquonK Hidden Boss License


template<size_t NbOfPackets, size_t LengthOfPackets>
class CLastReceivedPackets {
private:
	struct PacketStorage {
		const char * context = "";
		DWORD packetId = 0;
		size_t length = 0;
		char hexaContent[LengthOfPackets * 2 + 1] = { '\0' };
		bool inEdition = false;

		void Change(const char * context, DWORD packetId, const BYTE * lpByte, DWORD dwMsgSize);
	};


	std::mutex m_mutex;
	std::array<PacketStorage, NbOfPackets> m_packets;
	size_t m_nextReplaced = 0;

private:
	bool isEnabled = true;

public:
	void Push(const char * context, DWORD packetId, const BYTE * lpByte, DWORD dwMsgSize);

private:
	PacketStorage & FindNextPacket();
};

// static assert NbOfPackets > 0

template<size_t NbOfPackets, size_t LengthOfPackets>
void CLastReceivedPackets<NbOfPackets, LengthOfPackets>::Push(const char * context, DWORD packetId, const BYTE * lpByte, DWORD dwMsgSize) {
	if (!isEnabled) return;

	PacketStorage & placeholder = FindNextPacket();
	placeholder.Change(context, packetId, lpByte, dwMsgSize);
	placeholder.inEdition = false;
}

template<size_t NbOfPackets, size_t LengthOfPackets>
void CLastReceivedPackets<NbOfPackets, LengthOfPackets>::PacketStorage::Change(const char * context, DWORD packetId, const BYTE * lpByte, DWORD dwMsgSize) {
	this->context = context;
	this->packetId = packetId;
	this->hexaContent[0] = '\0';
	this->length = NULL_ID;

	size_t until = dwMsgSize;
	if (until >= LengthOfPackets) dwMsgSize = LengthOfPackets;
	
	static constexpr const char * const valueToHex = "0123456789ABCDEF";

	for (size_t i = 0; i != until; ++i) {
		this->hexaContent[i * 2 + 0] = (lpByte[i] >> 4) & 0xF;
		this->hexaContent[i * 2 + 1] =  lpByte[i]       & 0xF;
	}
	this->hexaContent[until * 2] = '\0';
	this->length = dwMsgSize;
}


template<size_t NbOfPackets, size_t LengthOfPackets>
CLastReceivedPackets<NbOfPackets, LengthOfPackets>::PacketStorage & CLastReceivedPackets<NbOfPackets, LengthOfPackets>::FindNextPacket() {
	std::lock_guard the_guardian(m_mutex);

	while (m_packets[m_nextReplaced].inEdition) {
		m_nextReplaced = (m_nextReplaced + 1) % NbOfPackets;
	}
	
	const size_t thisOne = m_nextReplaced;
	m_nextReplaced = (m_nextReplaced + 1) % NbOfPackets;

	m_packets[thisOne].inEdition = true;
	return m_packets[thisOne];
}




