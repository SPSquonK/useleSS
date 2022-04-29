#pragma once

struct CWndComponentSlot {
	CItemElem * m_item = nullptr;
	CRect m_rect = CRect();

	[[nodiscard]] constexpr operator bool() const noexcept { return m_item; }
	[[nodiscard]] bool IsIn(const CPoint point) const { return PtInRect(&m_rect, point); }
	void Clear();
	void Set(CItemElem * item);
	void Draw(C2DRender * p2DRender, CWndBase * window);

	const CItemElem * operator->() const { return m_item; }
};

template<size_t N>
struct CWndComponentSlots {
	std::array<CWndComponentSlot, N> values;

	CWndComponentSlot & operator[](const size_t n) { return values[n]; }
	[[nodiscard]] const CWndComponentSlot & operator[](const size_t n) const { return values[n]; }

	CWndComponentSlots() { values.fill(CWndComponentSlot{ }); }

	void Clear() { for (auto & slot : values) slot.Clear(); }

	void SetRects(const std::array<CRect, N> & rects) {
		for (size_t i = 0; i != N; ++i) {
			values[i].m_rect = rects[i];
		}
	}

	void Draw(C2DRender * p2DRender, CWndBase * window) {
		for (auto & slot : values) {
			slot.Draw(p2DRender, window);
		}
	}

};
