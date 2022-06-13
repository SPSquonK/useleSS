#pragma once

#include "ar.h"
#include <mutex>
#include <limits>
#include <span>

/// @SPSquonK, 2022-06, Last received packets memorizer
///
/// Stores in memory the last processed packets, without any allocation. 
/// Intended to be used in conjunction with a debugger
/// 
/// Released under the SquonK Hidden Boss License

namespace sqk {
	namespace debugutils {
		namespace _ {
			static consteval bool LastReceivedPacketsOk(size_t NbOfPackets, size_t LengthOfPackets) {
				constexpr auto max_ = std::numeric_limits<size_t>::max;
				return NbOfPackets > 0 && (LengthOfPackets < (max_() - 1) / 2);
			}
		}

		template<size_t NbOfPackets, size_t LengthOfPackets>
			requires (_::LastReceivedPacketsOk(NbOfPackets, LengthOfPackets))
			class CLastReceivedPackets {
			private:
				struct PacketStorage {
					const char * context = "";
					DWORD packetId = 0;
					size_t length = 0;
					char hexaContent[LengthOfPackets * 2 + 1] = { '\0' };
					bool inEdition = false;

					void Change(const char * context, DWORD packetId, std::span<const BYTE> bytes);
				};

				bool m_isEnabled = true;
				std::mutex m_mutex;
				std::array<PacketStorage, NbOfPackets> m_packets;
				size_t m_nextReplaced = 0;

				PacketStorage & FindNextPacket();

			public:
				void Push(const char * context, DWORD packetId, const BYTE * lpByte, DWORD dwMsgSize);
		};

		template<size_t NbOfPackets, size_t LengthOfPackets>
			requires (_::LastReceivedPacketsOk(NbOfPackets, LengthOfPackets))
			void CLastReceivedPackets<NbOfPackets, LengthOfPackets>::Push(const char * context, DWORD packetId, const BYTE * lpByte, DWORD dwMsgSize) {
			if (!m_isEnabled) return;

			PacketStorage & placeholder = FindNextPacket();
			std::span span(lpByte, dwMsgSize);
			placeholder.Change(context, packetId, span);
			placeholder.inEdition = false;
		}

		template<size_t NbOfPackets, size_t LengthOfPackets>
			requires (_::LastReceivedPacketsOk(NbOfPackets, LengthOfPackets))
			void CLastReceivedPackets<NbOfPackets, LengthOfPackets>::PacketStorage::Change(
				const char * const context,
				const DWORD packetId,
				std::span<const BYTE> bytes
			) {
			this->context = context;
			this->packetId = packetId;
			this->hexaContent[0] = '\0';
			this->length = static_cast<size_t>(-1);

			const size_t originalSize = bytes.size();
			if (bytes.size() > LengthOfPackets) {
				bytes = std::span<const BYTE>(bytes.begin(), bytes.begin() + LengthOfPackets);
			}

			static constexpr const char * const valueToHex = "0123456789ABCDEF";

			const size_t until = bytes.size();
			for (size_t i = 0; i != until; ++i) {
				this->hexaContent[i * 2 + 0] = valueToHex[(bytes[i] >> 4) & 0xF];
				this->hexaContent[i * 2 + 1] = valueToHex[ bytes[i]       & 0xF];
			}
			this->hexaContent[until * 2] = '\0';
			this->length = originalSize;
		}

		template<size_t NbOfPackets, size_t LengthOfPackets>
			requires (_::LastReceivedPacketsOk(NbOfPackets, LengthOfPackets))
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
	}
}
