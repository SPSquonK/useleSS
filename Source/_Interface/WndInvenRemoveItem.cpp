#include "stdafx.h"
#include "resData.h"
#include "WndInvenRemoveItem.h"
#include "defineText.h"
#include "DPClient.h"

void CWndInvenRemoveItem::InitItem(CItemElem * pItemElem) {
	m_pItemElem = pItemElem;
	const int nItemNum = pItemElem->m_nItemNum;

	SetEditNumToMax();

	if (nItemNum <= 1)
		m_pWndEditNum->EnableWindow(FALSE);
	else
		m_pWndEditNum->SetFocus();
}

void CWndInvenRemoveItem::OnDraw(C2DRender * p2DRender)
{
	if (m_pItemElem) {
		m_pItemElem->GetTexture()->Render(p2DRender, m_pWndItemCtrl->rect.TopLeft(), 255);
	}
}

void CWndInvenRemoveItem::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	m_pWndItemCtrl = GetWndCtrl(WIDC_CUSTOM1);
	m_pWndEditNum = GetDlgItem<CWndEdit>(WIDC_EDIT1);

	MoveParentCenter();
}

BOOL CWndInvenRemoveItem::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_INVEN_REMOVE_ITEM, pWndParent, WBS_MODAL, CPoint(0, 0));
}

void CWndInvenRemoveItem::OnMouseWndSurface(CPoint point) {
	if (m_pItemElem && m_pWndItemCtrl->rect.PtInRect(point)) {
		CPoint point2 = point;
		CRect  DrawRect = m_pWndItemCtrl->rect;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);
		g_WndMng.PutToolTip_Item(m_pItemElem, point2, &DrawRect, APP_INVEN_REMOVE_ITEM);
	}
}

BOOL CWndInvenRemoveItem::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult)
{
	switch (nID) {
		case WIDC_CANCEL:
			Destroy();
			break;
		case WIDC_OK: {
			const auto quantity = OnButtonOK();

			if (quantity) {
				const auto it = std::ranges::find_if(g_Neuz.m_savedInven,
					[&](const SavedSoldItem & sold) {
						return sold.objid == m_pItemElem->m_dwObjId
							&& sold.itemId == m_pItemElem->m_dwItemId;
					});

				if (it != g_Neuz.m_savedInven.end()) {
					it->Clear();
				}

				g_DPlay.SendRemoveItem(m_pItemElem, quantity.value());
				Destroy();
			} else {
				SetEditNumToMax();
				g_WndMng.PutString(TID_ERROR_NOT_ITEM_NUM);
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

std::optional<int> CWndInvenRemoveItem::OnButtonOK() {
	LPCTSTR szNumber = m_pWndEditNum->GetString();
	const auto optIntNumber = StringToInt(szNumber);

	if (!optIntNumber) {
		return std::nullopt;
	}

	if (*optIntNumber <= 0 || *optIntNumber > m_pItemElem->m_nItemNum) {
		return std::nullopt;
	}

	return optIntNumber;
}

void CWndInvenRemoveItem::SetEditNumToMax() {
	char szItemNum[10] = { 0, };
	_itoa(m_pItemElem->m_nItemNum, szItemNum, 10);
	m_pWndEditNum->SetString(szItemNum);
}

std::optional<int> CWndInvenRemoveItem::StringToInt(LPCTSTR string) {
	if (!*string) return std::nullopt; // Empty string

	int retval = 0;

	// Only numbers
	while (*string) {
		if (!isdigit2(*string)) {
			return false;
		}

		retval = retval * 10 + (*string - '0');

		++string;
	}

	return retval;
}