#include "StdAfx.h"
#include "WndItemDisplayer.h"

void CWndItemDisplayer::SetItem(/* const */ CItemElem & item) {
	// Ensure that we trigger operator=
	m_item = CItemElem();
	*m_item = item;
	m_item->SetTexture();
}

void CWndItemDisplayer::OnInitialUpdate() {
	CWndBase::OnInitialUpdate();
	m_item = std::nullopt;
}

void CWndItemDisplayer::OnDraw(C2DRender * const p2DRender) {
	if (!m_item && m_defaultTooltip == NULL) return;

	if (m_item && m_item->m_pTexture) {
		m_item->m_pTexture->Render(p2DRender, GetWindowRect().TopLeft(), 255);
	}

	CPoint point = GetMousePoint();
	CRect rect = GetWindowRect();
	if (!rect.PtInRect(point)) return;

	ClientToScreen(&point);
	ClientToScreen(&rect);
	if (m_item) {
		g_WndMng.PutToolTip_Item(&*m_item, point, &rect);
	} else if (m_defaultTooltip != NULL) {
		g_toolTip.PutToolTip(100, prj.GetText(m_defaultTooltip), rect, point, 0);
	}
}

DWORD CWndItemDisplayer::GetItemId() const {
	return m_item ? m_item->m_dwItemId : NULL;
}
