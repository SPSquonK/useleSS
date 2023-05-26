#pragma once

#include <array>
#include <algorithm>
#include <ranges>
#include <stack>
#include <exception>

template<typename T, size_t MaxSize>
class ExistingObjects {
public:
	std::array<T *, MaxSize> objects;
	size_t realSize = 0; // never decreases
	std::stack<size_t> emptyPlaces;

	ExistingObjects() {
		objects.fill(nullptr);
	}

	size_t Add(T * object);
	bool Remove(T * object, size_t index);


	template<typename Predicate>
	struct ValidIterator {
		T * const * _begin;
		T * const * _end;

		ValidIterator(T * const * start, size_t from, size_t end)
			: _begin(start + from), _end(start + end) {
			if (from != end && !Predicate()(*_begin)) {
				_next();
			}
		}


		ValidIterator & operator++() { _next(); return *this; }
		[[nodiscard]] T * operator*() const { return *_begin; }

		bool operator==(const ValidIterator & other) const noexcept {
			return _begin == other._begin && _end == other._end;
		}

		bool operator!=(const ValidIterator & other) const noexcept {
			return !(operator==(other));
		}

	private:
		void _next() {
			if (_begin == _end) return;
			
			do {
				++_begin;
			} while (_begin != _end && !Predicate()(*_begin));
		}
	};

	template<typename Predicate>
	struct _Range {
		ValidIterator<Predicate> _begin;
		ValidIterator<Predicate> _end;

		_Range(std::span<T * const> objects)
			: _begin(objects.data(), 0, objects.size())
			, _end(objects.data(), objects.size(), objects.size()) {
		}

		auto begin() { return _begin; }
		auto end() { return _end; }
	};

	[[nodiscard]] auto ValidObjs() const {
		struct _ValidObjPredicate {
			bool operator()(T * pObj) { return IsValidObj(pObj); }
		};

		return _Range<_ValidObjPredicate>(objects);
	}

	[[nodiscard]] auto Range() const {
		struct _NotNullPredicate {
			bool operator()(T * pObj) { return pObj; }
		};

		return _Range<_NotNullPredicate>(objects);
	}
};

template<typename T, size_t MaxSize>
size_t ExistingObjects<T, MaxSize>::Add(T * object) {
	size_t where;
	if (!emptyPlaces.empty()) {
		where = emptyPlaces.top();
		emptyPlaces.pop();
	} else {
		if (realSize == MaxSize) throw std::exception("Can not add a new object");
		where = realSize++;
	}

	objects[where] = object;
	return where;
}

template<typename T, size_t MaxSize>
bool ExistingObjects<T, MaxSize>::Remove(T * object, size_t index) {
	if (objects[index] != object) return false;
	emptyPlaces.emplace(index);
	objects[index] = nullptr;
	return true;
}

