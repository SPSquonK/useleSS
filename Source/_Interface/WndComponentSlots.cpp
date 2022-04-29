#include "StdAfx.h"
#include "WndComponentSlots.h"


void CWndComponentSlot::Clear() {
	if (m_item) {
		m_item->SetExtra(0);
		m_item = nullptr;
	}
}

void CWndComponentSlot::Set(CItemElem * item) {
	if (m_item) m_item->SetExtra(0);
	m_item = item;
	m_item->SetExtra(1);
}

void CWndComponentSlot::Draw(C2DRender * p2DRender, CWndBase * window) {
	if (!m_item) return;

	CTexture * texture = m_item->GetTexture();
	if (!texture) return;

	m_item->GetTexture()->Render(p2DRender, m_rect.TopLeft(), 255);

	if (m_item->m_nItemNum > 1) {
		TCHAR szTemp[32];
		_stprintf(szTemp, "%d", m_item->GetExtra());
		p2DRender->TextOut(m_rect.right - 11, m_rect.bottom - 11, szTemp, 0xff1010ff);
	}

	CRect hitrect = m_rect;
	CPoint point = window->GetMousePoint();
	if (m_rect.PtInRect(point)) {
		CPoint point2 = point;
		window->ClientToScreen(&point2);
		window->ClientToScreen(&hitrect);

		g_WndMng.PutToolTip_Item(m_item, point2, &hitrect);
	}
}

