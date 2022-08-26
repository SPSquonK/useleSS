#pragma once

#include <algorithm>
#include <type_traits>

namespace sqktd {
  template<typename MapType>
  [[nodiscard]] constexpr typename MapType::mapped_type find_in_map(
    const MapType & map,
    const typename MapType::key_type & key,
    const typename MapType::mapped_type defaultValue
  ) {
    const auto it = map.find(key);
    if (it != map.end()) return it->second;
    return defaultValue;
  }

	template<typename Collection>
	[[nodiscard]] constexpr bool all_are(const Collection & collection, const typename Collection::value_type & value) {
		return std::ranges::all_of(collection, [&](const auto & value_) { return value_ == value; });
	}

}
