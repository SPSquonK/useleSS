#include "StdAfx.h"
#include "WndItemDisplayer.h"


void CWndItemDisplayer::SetItem(CItemElem & item) {
	// Ensure that we trigger operator=
	m_item = CItemElem();
	*m_item = item;

	texture.LoadTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, item.GetProp()->szIcon), 0xffff00ff);
}

void CWndItemDisplayer::OnInitialUpdate() {
	CWndBase::OnInitialUpdate();
	m_item = std::nullopt;
}

void CWndItemDisplayer::OnDraw(C2DRender * p2DRender) {
	if (!m_item) return;

	texture.Render(p2DRender, GetWindowRect().TopLeft(), 255);
	// Texture1.Render(p2DRender, m_Rect[1].TopLeft(), 255);


	CPoint point = GetMousePoint();
	CRect rect = GetWindowRect();
	if (rect.PtInRect(point)) {
		ClientToScreen(&point);
		ClientToScreen(&rect);
		if (m_item) {
			g_WndMng.PutToolTip_Item(&*m_item, point, &rect);
		} else if (m_defaultTooltip != NULL) {
			g_toolTip.PutToolTip(100, prj.GetText(m_defaultTooltip), rect, point, 0);
		}
	}
}

DWORD CWndItemDisplayer::GetItemId() const {
	return m_item ? m_item->m_dwItemId : NULL;
}
