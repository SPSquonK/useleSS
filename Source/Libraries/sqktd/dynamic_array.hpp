#pragma once

#include <memory>
#include <ranges>

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

		[[nodiscard]] T & operator[](size_t size) const { return m_data[size]; }
		[[nodiscard]] size_t size() const noexcept { return m_size; }

		[[nodiscard]] T * get() const noexcept { return m_data.get(); }

		[[nodiscard]] auto begin() const noexcept { return m_data.get(); }
		[[nodiscard]] auto end() const noexcept { return m_data.get() + m_size; }
		void reset() { m_data.reset(); m_size = 0; }

		// Return the range of the keys of the array
		//
		// The advantage of this method over doing a for (i = 0; i < size; ++i) loop
		// is you can pass the indice type you want and not bother with signed-ness
		template<typename IndiceType = size_t>
		[[nodiscard]] auto keys() const noexcept {
			return std::views::iota(IndiceType(0), static_cast<IndiceType>(m_size));
		}

		// Return the range of the key-values of the array
		//
		// `for (const auto [i, ptr] : array.entries()) {`
		// is equivalent to
		// for (size_t i = 0; i != array.size(); ++i) {
		//   T * ptr = &array[i];
		template<typename IndiceType = size_t>
		[[nodiscard]] auto entries() const noexcept {
			// Not using zip because I do not believe in its performances
			struct Sentinel {};
			struct Iterator {
				IndiceType i;
				T * ptr;
				IndiceType end;

				[[nodiscard]] std::pair<IndiceType, T *> operator*() const {
					return std::make_pair(i, ptr);
				}

				Iterator & operator++() { ++i; ++ptr; return *this; }

				[[nodiscard]] bool operator==(Sentinel) const noexcept {
					return i == end;
				}
			};

			struct Range {
				const dynamic_array * self;
				Iterator begin() {
					return Iterator{
						static_cast<IndiceType>(0),
						self->m_data.get(),
						static_cast<IndiceType>(self->m_size)
					};
				}
				Sentinel end() { return Sentinel{}; }
			};

			return Range{ this };
		}
	};
}
