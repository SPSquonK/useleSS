#pragma once

#include "boost/container/small_vector.hpp"
#include <ranges>

struct SINGLE_DST {
	int	nDst;
	int	nAdj;

#ifdef __CLIENT
	[[nodiscard]] CString ToString() const;
#endif
};

template<typename T>
concept MultipleDsts = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, SINGLE_DST>;

using SmallDstList = boost::container::small_vector<SINGLE_DST, 16>;
