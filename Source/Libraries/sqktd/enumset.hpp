#pragma once

#include <bitset>
#include <concepts>
#include "ar.h"

namespace sqktd {
	// TODO: add assertion that unsigned long is 32 bits

	template <typename E>
	concept EnumSettable = std::is_scoped_enum_v<E> && 
		requires () { E::_MAX; };

	template <EnumSettable E>
	class EnumSet {
		std::bitset<std::to_underlying(E::_MAX)> bitset;

	public:
		EnumSet() = default;
		explicit EnumSet(unsigned long baseValue) : bitset(baseValue) {}

		constexpr void SetAll() { bitset.set(); }
		constexpr void UnsetAll() { bitset.reset(); }
		constexpr void Set(const E e) { bitset.set(std::to_underlying(e)); }
		constexpr void Unset(const E e) { bitset.reset(std::to_underlying(e)); }
		[[nodiscard]] bool Test(const E e) const { return bitset.test(std::to_underlying(e)); }
		[[nodiscard]] bool operator[](const E e) const { return bitset[std::to_underlying(e)]; }

		constexpr unsigned long ToULong() const noexcept { return bitset.to_ulong(); }

		friend ::CAr & operator<<(::CAr & ar, const sqktd::EnumSet<E> & self)
		requires (std::to_underlying(E::_MAX) <= 32) {
			return ar << self.bitset.to_ulong();
		}

		friend ::CAr & operator>>(::CAr & ar, sqktd::EnumSet<E> & self)
			requires (std::to_underlying(E::_MAX) <= 32) {
			unsigned long value; ar >> value;
			self.bitset = std::bitset<std::to_underlying(E::_MAX)>(value);
			return ar;
		}
	};
}
