#include "StdAfx.h"
#include "ResData.h"
#include "defineText.h"
#include "WndSqKComponents.h"

///////////////////////////////////////////////////////////////////////////////


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


///////////////////////////////////////////////////////////////////////////////

CWndItemReceiver::~CWndItemReceiver() {
	ResetItem();
}

void CWndItemReceiver::OnInitialUpdate() {
	CWndBase::OnInitialUpdate();
	m_item = nullptr;
}

void CWndItemReceiver::OnDraw(C2DRender * p2DRender) {
	if (m_features.colorWhenHoverWithItem
		&& CWndBase::m_GlobalShortcut.m_dwShortcut == ShortcutType::Item) {

		const CPoint point = GetMousePoint();
		const CRect window = GetWindowRect();
		if (window.PtInRect(point)) {
			p2DRender->RenderFillRect(window, *m_features.colorWhenHoverWithItem);
		}
	}

	if (!m_item) {
		if (m_shadow) {
			m_shadow->first->Render(p2DRender, GetWindowRect().TopLeft(), m_shadow->second);
		}

		return;
	}

	CTexture * const texture = m_item->GetTexture();
	if (!texture) return;

	texture->Render(p2DRender, GetWindowRect().TopLeft());
}

void CWndItemReceiver::ChangeShadowTexture(const ItemProp * itemProp, std::optional<DWORD> opacity) {
	if (!itemProp) {
		m_shadow = std::nullopt;
		return;
	}

	if (!m_shadow && !opacity) {
		opacity = 50;
	}
	
	CTexture * texture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_ITEM, itemProp->szIcon), 0xffff00ff);
	if (!texture) return;

	if (!opacity) { // By construction, m_shadow is not nullopt
		m_shadow->first = texture;
	} else {
		m_shadow = std::make_pair(texture, opacity.value());
	}
}

void CWndItemReceiver::OnMouseWndSurface(CPoint point) {
	CRect rect = GetWindowRect();
	ClientToScreen(&point);
	ClientToScreen(&rect);
	if (m_item) {
		g_WndMng.PutToolTip_Item(m_item, point, &rect);
	} else if (m_defaultTooltip != 0) {
		g_toolTip.PutToolTip(100, prj.GetText(m_defaultTooltip), rect, point, 0);
	}
}

void CWndItemReceiver::OnRButtonUp(UINT, CPoint) { ResetItemWithNotify(); }
void CWndItemReceiver::OnLButtonDblClk(UINT, CPoint) { ResetItemWithNotify(); }

void CWndItemReceiver::ResetItemWithNotify() {
	if (m_features.removable && m_item) {
		ResetItem();
		NotifyChange();
	}
}

BOOL CWndItemReceiver::OnDropIcon(SHORTCUT * pShortcut, CPoint) {
	if (pShortcut->m_dwShortcut != ShortcutType::Item) {
		return FALSE;
	}
	
	CWndBase * pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (pWndFrame->GetWndId() != APP_INVENTORY) {
		return FALSE;
	}
	
	if (!g_pPlayer) {
		return FALSE;
	}

	if (g_pPlayer->m_Inventory.IsEquip(pShortcut->m_dwId)) {
		g_WndMng.PutString(TID_GAME_EQUIPPUT);
		SetForbid(TRUE);
		return FALSE;
	}

	CItemElem * pItemElem = g_pPlayer->GetItemId(pShortcut->m_dwId);
	if (!pItemElem) {
		SetForbid(TRUE);
		return FALSE;
	}

	const bool b = SetAnItem(pItemElem, SetMode::Verbose);
	if (!b) {
		SetForbid(TRUE);
	}
	return b ? TRUE : FALSE;
}

bool CWndItemReceiver::SetAnItem(CItemElem * itemElem, SetMode setMode) {
	if (itemElem == nullptr) {
		if (m_item) {
			ResetItem();
			NotifyChange();
		}
		return true;
	}

	if (setMode != SetMode::NeverFail) {
		if (!CanReceiveItem(*itemElem, setMode == SetMode::Verbose)) {
			return false;
		}
	}

	ResetItem();
	m_item = itemElem;
	itemElem->SetExtra(itemElem->GetExtra() + 1);
	NotifyChange();
	return true;
}

void CWndItemReceiver::ResetItem() {
	if (!g_pPlayer) {
		m_item = nullptr;
		return;
	}

	if (!m_item) return;

	const auto currentExtra = m_item->GetExtra();
	if (currentExtra != 0) {
		m_item->SetExtra(currentExtra - 1);
	}

	m_item = nullptr;
}

void CWndItemReceiver::NotifyChange() {
	m_pParentWnd->OnChildNotify(
		WNM_ItemReceiver_Changed, m_nIdWnd,
		reinterpret_cast<LRESULT *>(m_item)
	);
}
