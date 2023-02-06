#pragma once

#include <memory>

namespace sqktd {
	/// A class that maybe owns a pointer, maybe not.
	/// 
	/// Default behavior is that it does not own pointers that are passed to it
	/// and it mainly behaves like a T *.
	/// But if you pass to this class a std::unique_ptr<T>, this class will
	/// remember to release the data.
	/// 
	/// @SPSquonK, 2022-05
	/// License: https://squonk.fr/SquonK-Hidden-Boss-License.txt
	template<typename T>
	class maybe_owned_ptr final {
		T * m_rawPtr = nullptr; // The raw pointer
		bool m_isOwned = false; // Whetever it is this class responsability to free the pointer

		void ensure_owns_nothing() {
			if (m_rawPtr && m_isOwned) {
				delete m_rawPtr;
				m_rawPtr = nullptr;
			}
		}

	public:
		// Rule of 5

		// Builds an empty pointer
		maybe_owned_ptr() = default;
		maybe_owned_ptr(const maybe_owned_ptr &) = delete;
		maybe_owned_ptr & operator=(const maybe_owned_ptr &) = delete;
		maybe_owned_ptr(maybe_owned_ptr && other) noexcept { operator=(std::move(other)); }
		maybe_owned_ptr & operator=(maybe_owned_ptr && other) noexcept {
			if (this == &other) return *this;

			std::swap(m_rawPtr, other.m_rawPtr);
			std::swap(m_isOwned, other.m_isOwned);
			return *this;
		}
		~maybe_owned_ptr() { if (m_rawPtr && m_isOwned) { delete m_rawPtr; } }

		// From T* and std::unique_ptr<T>

		// Builds an observing pointer
		explicit(false) maybe_owned_ptr(T * t) { operator=(t); }
		// Builds an owning pointer
		explicit(false) maybe_owned_ptr(std::unique_ptr<T> t) { operator=(std::move(t)); }

		// Affect an observed pointer
		maybe_owned_ptr & operator=(T * t) {
			ensure_owns_nothing();

			m_rawPtr = t;
			m_isOwned = false;
			return *this;
		}

		// Affect an owning pointer
		maybe_owned_ptr & operator=(std::unique_ptr<T> t) {
			ensure_owns_nothing();

			m_rawPtr = t.release();
			m_isOwned = true;
			return *this;
		}

		// As easy to use as a T*

		[[nodiscard]] operator bool() const noexcept { return m_rawPtr != nullptr; }
		[[nodiscard]] operator T * () { return m_rawPtr; }
		[[nodiscard]] operator const T * () const noexcept { return m_rawPtr; }

		T & operator*() { return *m_rawPtr; }
		const T & operator*() const { return *m_rawPtr; }

		T * operator->() { return m_rawPtr; }
		const T * operator->() const { return m_rawPtr; }
	};
}
