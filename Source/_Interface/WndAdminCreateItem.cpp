#include "stdafx.h"
#include "AppDefine.h"
#include "WndAdminCreateItem.h"
#include "FuncTextCmd.h"
#include <numeric>


CWndAdminCreateItem::Item::Item(
	const ItemProp * itemProp,
	const ExtraItemProp * extraItemProp
) {
	this->name.Format("%s (Lv%d)", itemProp->szName, itemProp->dwLimitLevel1);
	this->itemProp = itemProp;
	this->extraItemProp = extraItemProp;
	this->texture = std::nullopt; ;
}

void CWndAdminCreateItem::Displayer::Render(
	C2DRender * const p2DRender, const CRect rect,
	Item & item, const DWORD color, const WndTListBox::DisplayArgs & args
) const {
	if (!hasIcons) {
		p2DRender->TextOut(rect.left, rect.top, item.name.GetString(), color);
	} else {
		if (!item.texture) item.texture = ItemProps::GetItemIconTexture(item.itemProp->dwID);

		if (item.texture && item.texture.has_value()) {
			(*item.texture)->Render(p2DRender, rect.TopLeft() + CPoint(2, 0));
		}
		
		p2DRender->TextOut(rect.left + 38, rect.top, item.name.GetString(), color);

		if (item.extraItemProp && !item.extraItemProp->otherName.empty()) {
			p2DRender->TextOut(
				rect.left + 38, 
				rect.top + p2DRender->m_pFont->GetMaxHeight() + 1,
				item.extraItemProp->otherName.c_str(), color - 0xA0000000
			);
		}

		CRect iconRect(rect.TopLeft() + CPoint(2, 0), CSize(32, 32));

		if (iconRect.PtInRect(args.mousePosition)) {
			CPoint point2 = args.mousePosition + p2DRender->m_ptOrigin;
			iconRect.OffsetRect(p2DRender->m_ptOrigin);

			CItemElem itemElem;
			itemElem.m_dwItemId = item.itemProp->dwID;
			g_WndMng.PutToolTip_Item(&itemElem, point2, &iconRect);
		}
	}
}

void CWndAdminCreateItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();

	if (CWndButton * withIconsCheckBox = GetDlgItem<CWndButton>(WIDC_CHECK)) {
		withIconsCheckBox->SetCheck(1);
	}

	LoadExtraItemProps();

	ReplaceListBox<Item, Displayer>(WIDC_CONTROL1);
	UpdateRendering();

	UpdateItems(NULL_ID, SEX_SEXLESS, -1, 0, "");

	CWndComboBox * pWndItemKind = GetDlgItem<CWndComboBox>(WIDC_ITEM_KIND);
	{
		CWndListBox::LISTITEM & allKinds = pWndItemKind->AddString("ALL");
		allKinds.m_dwData = NULL_ID;

		for (const auto & [intId, textId] : CScript::m_defines.GetOrBuild("IK2_")) {
			CWndListBox::LISTITEM & kind = pWndItemKind->AddString(textId.GetString());
			kind.m_dwData = static_cast<DWORD>(intId);
		}

		pWndItemKind->m_wndListBox.SortListBox();

		const int nIndex = pWndItemKind->m_wndListBox.FindString(0, "ALL");
		pWndItemKind->SetCurSel(nIndex);

		BoundListBoxOfComboBoxSize(*pWndItemKind, std::nullopt, 20);
	}

	CWndComboBox * pWndItemSex = GetDlgItem<CWndComboBox>(WIDC_ITEM_SEX);
	{
		for (const auto & [intId, textId] : CScript::m_defines.GetOrBuild("SEX_")) {
			CWndListBox::LISTITEM & sex = pWndItemSex->AddString(textId.GetString());
			sex.m_dwData = static_cast<DWORD>(intId);
		}

		pWndItemSex->SetCurSel(2);
	}

	CWndComboBox * pWndItemJob = (CWndComboBox *)GetDlgItem(WIDC_ITEM_JOB);
	{
		CWndListBox::LISTITEM & allJobs = pWndItemJob->AddString("ALL");
		allJobs.m_dwData = -1;

		for (const auto & [intId, textId] : CScript::m_defines.GetOrBuild("JOB_")) {
			CWndListBox::LISTITEM & itemJob = pWndItemJob->AddString(textId.GetString());
			itemJob.m_dwData = static_cast<DWORD>(intId);
		}

		pWndItemJob->m_wndListBox.SortListBox();

		const int nIndex = pWndItemJob->m_wndListBox.FindString(0, "ALL");
		pWndItemJob->SetCurSel(nIndex);

		BoundListBoxOfComboBoxSize(*pWndItemJob, std::nullopt, 20);
	}

	MoveParentCenter();
} 

void CWndAdminCreateItem::UpdateRendering() {
	CWndButton * button = GetDlgItem<CWndButton>(WIDC_CHECK);
	const bool withIcons = button && button->GetCheck();

	ItemPropListBox * pItemPropList = GetDlgItem<ItemPropListBox>(WIDC_CONTROL1);
	if (pItemPropList) {
		pItemPropList->displayer.hasIcons = withIcons;

		if (withIcons) {
			pItemPropList->SetLineHeight(36);
		} else {
			pItemPropList->SetLineHeight(m_pFont->GetMaxHeight() + 1 + 1);
		}
	}
}

void CWndAdminCreateItem::LoadExtraItemProps() {
	// Read extra names file
	std::map<CString, CString> otherLanguageBindings;

	CScanner scanner;
	if (scanner.Load("propItem.txt-french.txt")) {
		otherLanguageBindings = LoadStrings(scanner);
	}

	// Build an inverse mapping from names to IDS
	std::map<CString, std::set<CString>> inverseOriginalNames;
	for (const auto & [ids, text] : CScript::m_mapString) {
		if (ids.Find("IDS_") == 0 && text != "") {
			inverseOriginalNames[text].emplace(ids);
		}
	}

	// Conflate the map
	std::multimap<std::string, std::string> conflatedRenaming;
	for (const auto & [text, idss] : inverseOriginalNames) {
		// List the text corresponding to these idss
		std::set<CString> names;

		for (const CString & ids : idss) {
			auto itOtherLanguage = otherLanguageBindings.find(ids);
			if (itOtherLanguage != otherLanguageBindings.end()
				&& itOtherLanguage->second.GetLength() > 0
				) {
				names.emplace(itOtherLanguage->second);
			}
		}

		// Build english name - other language name mapping
		for (const CString & name : names) {
			conflatedRenaming.emplace(text.GetString(), name.GetString());
		}

		// We can not directly insert in the first loop because we want
		// duplicated names to only appear once
	}


	for (const ItemProp & pItemProp : prj.m_aPropItem) {
		ExtraItemProp extra;

		extra.names.emplace_back(pItemProp.szName);

		auto [itOtherName, itOtherNameEnd] = conflatedRenaming.equal_range(extra.names[0]);
		while (itOtherName != itOtherNameEnd) {
			extra.names.emplace_back(itOtherName->second);
			if (extra.otherName.empty()) {
				extra.otherName = itOtherName->second;
			}

			++itOtherName;
		}

		for (std::string & name : extra.names) {
			STDStringToLower(name);
		}

		extraItemProps.SetAtGrow(pItemProp.dwID, extra);
	}

}

std::map<CString, CString> CWndAdminCreateItem::LoadStrings(CScanner & scanner) {
	std::map<CString, CString> result;

	while (true) {
		scanner.GetToken();

		if (scanner.tok == FINISHED) break;

		CString str = scanner.Token;
		if (str.Find("IDS", 0) != 0) {
			scanner.GetToken();
			continue;
		}

		scanner.GetLastFull();

		result.emplace(str.GetString(), scanner.Token.GetString());
	}

	return result;
}

void CWndAdminCreateItem::STDStringToLower(std::string & string) {
	std::transform(string.begin(), string.end(), string.begin(),
		[](char c) { return std::tolower(c); }
	);
}

// TODO: this should be a member function of CWndComboBox
void CWndAdminCreateItem::BoundListBoxOfComboBoxSize(
	CWndComboBox & comboBox, std::optional<int> minSize, std::optional<int> maxSize
) {
	const int size = comboBox.m_wndListBox.GetCount();
	const int newSize = std::clamp(size, minSize.value_or(size), maxSize.value_or(size));

	if (size == newSize) return;

	CRect rect = comboBox.m_wndListBox.GetWindowRect(TRUE);
	rect.bottom = rect.top + (newSize * (comboBox.m_pFont->GetMaxHeight() + 3)) + 8;
	comboBox.m_wndListBox.SetWndRect(rect);
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAdminCreateItem::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_ADMIN_CREATEITEM, pWndParent, 0, CPoint(0, 0));
}

std::optional<DWORD> CWndAdminCreateItem::GetSelectedInComboBox(UINT comboBoxId) /* const */ {
	CWndComboBox * const pWnd = GetDlgItem<CWndComboBox>(comboBoxId);
	if (!pWnd) return std::nullopt;

	const int curSel = pWnd->GetCurSel();
	if (curSel == -1) return std::nullopt;

	return pWnd->GetItemData(curSel);
}

BOOL CWndAdminCreateItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ITEM_KIND || nID == WIDC_ITEM_SEX || nID == WIDC_ITEM_JOB || nID == WIDC_LEVEL )
	{
		if( message == WNM_SELCHANGE || message == EN_CHANGE )
		{
			UpdateItems();
		}
	} else if (nID == WIDC_ITEM_NAME) {
		if (message == EN_CHANGE) {
			UpdateItems();
		}
	} else if (nID == WIDC_CHECK) {
		UpdateRendering();
	}
	else
	if( nID == WIDC_OK || ( nID == WIDC_CONTROL1 && message == WNM_DBLCLK ) )
	{
		const ItemPropListBox * pListBox = GetDlgItem<ItemPropListBox>(WIDC_CONTROL1);
		const Item * item = pListBox->GetCurSelItem();

		if (item && item->itemProp) {
			const short quantity = GetQuantityToCreateOf(item->itemProp);

			if (quantity != 0) {
				CString string;
				string.Format("/ci \"%s\" %d", item->itemProp->szName, static_cast<int>(quantity));
				g_textCmdFuncs.ParseCommand(string.GetString(), g_pPlayer);
			}
		}

	} else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE) {
		nID = WTBID_CLOSE;
		Destroy(TRUE);
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndAdminCreateItem::UpdateItems() {
	CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_LEVEL);
	LPCTSTR string = pWndEdit->GetString();
	const DWORD dwLevel = atoi(string);

	const DWORD dwKind = GetSelectedInComboBox(WIDC_ITEM_KIND).value_or(-1);
	const DWORD dwSex = GetSelectedInComboBox(WIDC_ITEM_SEX).value_or(SEX_SEXLESS);
	const DWORD dwJob = GetSelectedInComboBox(WIDC_ITEM_JOB).value_or(-1);


	CWndEdit * pWndEditName = GetDlgItem<CWndEdit>(WIDC_ITEM_NAME);
	LPCTSTR stringName = pWndEditName->GetString();

	UpdateItems(dwKind, dwSex, dwJob, dwLevel, stringName);
}

short CWndAdminCreateItem::GetQuantityToCreateOf(const ItemProp * itemProp) {
	return itemProp->dwPackMax;
}



void CWndAdminCreateItem::UpdateItems(
	DWORD kind, DWORD sex, DWORD job, DWORD level, std::string textFilter
) {
	ItemPropListBox * pListBox = GetDlgItem<ItemPropListBox>(WIDC_CONTROL1);

	STDStringToLower(textFilter);

	pListBox->ResetContent();
	for (const ItemProp & pItemProp : prj.m_aPropItem) {

		if (kind != NULL_ID && pItemProp.dwItemKind2 != kind) continue;

		if (sex != SEX_SEXLESS && pItemProp.dwItemSex != sex) continue;

		if (pItemProp.dwLimitLevel1 < level) continue;

		if (job != -1 && pItemProp.dwItemJob != job) continue;

		const ExtraItemProp * extra = extraItemProps.GetAt(pItemProp.dwID);

		if (!textFilter.empty()) {
			if (extra) {
				const bool filterNotInName = std::none_of(
					extra->names.begin(), extra->names.end(),
					[&](const std::string & name) {
						return name.contains(textFilter);
					}
				);

				if (filterNotInName) continue;
			}
		}

		pListBox->Add(Item(&pItemProp, extra));
	}
}