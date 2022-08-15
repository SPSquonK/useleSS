#pragma once

template <typename T> class CFixedArrayIterator;

template <class T> class CFixedArray final {
private:
	std::vector<T> m_data;
	std::vector<size_t> m_offsets;

public:
	CFixedArray();

	int  GetSize() { return static_cast<int>(m_data.size()); }
	void SetAtGrow(size_t nIndex, const T * pData);
	T * GetAt(DWORD dwIndex);
	void Optimize();
	void RemoveAll() { *this = CFixedArray(); }

	CFixedArrayIterator<T> begin();
	CFixedArrayIterator<T> end();

	friend class CFixedArrayIterator<T>;
};

template <class T> inline CFixedArray<T>::CFixedArray() {
	m_data.reserve(100);
	m_offsets.reserve(100);
}

template <class T> inline T * CFixedArray<T>::GetAt(DWORD dwIndex) {
	if (dwIndex >= m_offsets.size()) return nullptr;

	const auto offset = m_offsets[dwIndex];
	if (offset == NULL_ID) return nullptr;

	return &m_data[offset];
}

template <class T> void CFixedArray<T>::Optimize() {
	m_data.shrink_to_fit();
	m_offsets.shrink_to_fit();
}

template <class T> void CFixedArray<T>::SetAtGrow(const size_t nIndex, const T * const pData) {
	if (nIndex >= m_offsets.size()) {
		m_offsets.resize(nIndex, NULL_ID);
	}

	T * target;
	if (m_offsets[nIndex] == NULL_ID) {
		const size_t offset = m_data.size();
		m_offsets[nIndex] = offset;
		target = &m_data.emplace_back();
	} else {
		target = &m_data[m_offsets[nIndex]];
	}

	std::memcpy(target, pData, sizeof(T));
}


template <typename T> class CFixedArrayIterator {
	size_t m_i;
	CFixedArray<T> * m_container;

public:
	CFixedArrayIterator(CFixedArray<T> * container, size_t index);
	[[nodiscard]] operator bool() const { return m_i != m_container->m_offsets.size(); }
	[[nodiscard]] T & operator*() const { return *m_container->GetAt(m_i); }

	[[nodiscard]] bool operator==(const CFixedArrayIterator & other) const {
		return m_container == other.m_container && m_i == other.m_i;
	}
	[[nodiscard]] bool operator!=(const CFixedArrayIterator & other) const { return !operator==(other); }
	CFixedArrayIterator & operator++();

private:
	void NextUntilValidOrEnd();
};

template <typename T>
CFixedArrayIterator<T> CFixedArray<T>::begin() {
	return CFixedArrayIterator<T>(this, 0);
}

template <typename T>
CFixedArrayIterator<T> CFixedArray<T>::end() {
	return CFixedArrayIterator<T>(this, m_data.size());
}

template <typename T>
CFixedArrayIterator<T>::CFixedArrayIterator(CFixedArray<T> * container, size_t index)
: m_i(index), m_container(container) {
	NextUntilValidOrEnd();
}

template <typename T>
CFixedArrayIterator<T> & CFixedArrayIterator<T>::operator++() {
	if (!*this) return *this;
	++m_i;
	NextUntilValidOrEnd();
	return *this;
}

template <typename T>
void CFixedArrayIterator<T>::NextUntilValidOrEnd() {
	while (true) {
		if (!*this) return;
		if (m_container->GetAt(m_i)) return;
		++m_i;
	}
}
