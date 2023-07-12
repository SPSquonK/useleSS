#pragma once

namespace sqktd {
	template<typename T>
	bool is_among(T searched, std::convertible_to<T> auto ... among) {
		return ((searched == among) || ...);
	};
}