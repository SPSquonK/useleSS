#pragma once

namespace sqktd {
	/// A class that acts like a std::unique_ptr but that is copyable.
	///
	/// @SPSquonK, 2022-06
	/// License: https://squonk.fr/SquonK-Hidden-Boss-License.txt
	template<typename T>
	class copyable_unique_ptr final {
	public:
		using pointer = T *;
		using element_type = T;

	private:
		T * m_raw_ptr = nullptr;

		void ensure_owns_nothing() {
			if (m_raw_ptr) {
				delete m_raw_ptr;
				m_raw_ptr = nullptr;
			}
		}

	public:
		explicit copyable_unique_ptr(T * const raw_ptr = nullptr)
			: m_raw_ptr(raw_ptr) {
		}

		copyable_unique_ptr(const copyable_unique_ptr & other) {
			if (other.m_raw_ptr) {
				m_raw_ptr = new T(*other.m_raw_ptr);
			}
		}

		copyable_unique_ptr(copyable_unique_ptr && other) {
			std::swap(m_raw_ptr, other.m_raw_ptr);
		}

		copyable_unique_ptr & operator=(const copyable_unique_ptr & other) {
			if (this == &other) return *this;

			ensure_owns_nothing();
			if (other.m_raw_ptr) m_raw_ptr = new T(*other.m_raw_ptr);

			return *this;
		}

		copyable_unique_ptr & operator=(copyable_unique_ptr && other) noexcept {
			std::swap(m_raw_ptr, other.m_raw_ptr);
			return *this;
		}

		~copyable_unique_ptr() { if (m_raw_ptr) delete m_raw_ptr; }

		[[nodiscard]] operator bool() const noexcept { return m_raw_ptr; }

		[[nodiscard]] T & operator*() const { return *m_raw_ptr; }
		[[nodiscard]] T * operator->() const { return m_raw_ptr; }
		[[nodiscard]] T * get() const { return m_raw_ptr; }
		
		void reset() { ensure_owns_nothing(); }
	};

	template<typename T, typename ... Args>
	[[nodiscard]] copyable_unique_ptr<T> make_copyable_unique_ptr(Args && ... args) {
		T * ptr = new T(::std::forward<Args>(args)...);
		return copyable_unique_ptr<T>(ptr);
	}
}
