#pragma once

#include <algorithm>
#include <type_traits>

namespace sqktd {
  /// Returns the value of *std::ranges::find_if(container, p). If the
  /// iterator is invalid, return defaultValue instead.
  template<typename Container, typename UnaryPredicate>
  [[nodiscard]] constexpr typename Container::value_type find_if_by_value(
    const Container & container,
    UnaryPredicate p,
    typename Container::value_type defaultValue = 0
  ) {
    const auto it = std::ranges::find_if(container, p);
    if (it != container.end()) return *it;
    return defaultValue;
  }

  template<typename MapType>
  [[nodiscard]] constexpr typename MapType::mapped_type find_in_map(
    const MapType & map,
    const typename MapType::key_type & key,
    const typename MapType::mapped_type defaultValue = nullptr
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
