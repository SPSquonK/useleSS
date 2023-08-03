#pragma once

#include <memory>

namespace sqktd {
	/* Like std::unique_ptr<T[]>, but the size is stored */
	template<typename T>
	class dynamic_array {
	private:
		std::unique_ptr<T[]> m_data;
		size_t m_size;

	public:
		dynamic_array() : m_data(nullptr), m_size(0) {}
		dynamic_array(size_t size) : m_data(std::make_unique<T[]>(size)), m_size(size) {}

		[[nodiscard]] T * operator[](size_t size) const { return m_data[size]; }

		[[nodiscard]] size_t size() const noexcept { return m_size; }

		[[nodiscard]] auto begin() const noexcept { return m_data.get(); }
		[[nodiscard]] auto end() const noexcept { return m_data.get() + m_size; }
	};
}
