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
	size_t realSize; // never decreases
	std::stack<size_t> emptyPlaces;


	auto Range() const {
		return std::ranges::subrange(objects.begin(), objects.begin() + realSize)
			| std::views::filter([](T * const ptr) { return ptr; });
	}

	size_t Add(T * object);
	bool Remove(T * object, size_t index);
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

