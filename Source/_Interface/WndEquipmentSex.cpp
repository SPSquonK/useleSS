#include "StdAfx.h"
#include "AppDefine.h"
#include "WndEquipmentSex.h"
#include "ItemMorph.h"
#include <array>

BOOL CWndEquipmentSex::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_EQUIPMENT_SEX, pWndParent, 0, CPoint(0, 0));
}

void CWndEquipmentSex::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndTListBox<Displayed, DisplayedDisplayer> & box = ReplaceListBox<Displayed, DisplayedDisplayer>(WIDC_LISTBOX);
	box.SetLineHeight(36);


	ChangeMode(Mode::Vanilla);

	CWndButton * radio = GetDlgItem<CWndButton>(WIDC_RADIO1);
	radio->SetGroup(TRUE);
	radio->SetCheck(TRUE);

	AddQuantity(WIDC_RADIO1, m_lists.vanilla.size());
	AddQuantity(WIDC_RADIO2, m_lists.detected.size());
	AddQuantity(WIDC_RADIO3, m_lists.unattributed.size());

	MoveParentCenter();
}


void CWndEquipmentSex::AddQuantity(UINT widgetId, size_t size) {
	CWndBase * widget = GetDlgItem(widgetId);
	CString title = widget->GetTitle();
	title.AppendFormat(" (%lu)", size);
	widget->SetTitle(title.GetString());
}

static void CopyToClipboard(const CString & str) {
	HWND hwnd = GetDesktopWindow();
	if (OpenClipboard(hwnd)) {
		const size_t length = str.GetLength();
		EmptyClipboard();
		HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, length + 1);
		char * const buffer = static_cast<char *>(GlobalLock(clipbuffer));
		std::strcpy(buffer, str.GetString());
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}
}

BOOL CWndEquipmentSex::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_RADIO1) {
		ChangeMode(Mode::Vanilla);
	} else if (nID == WIDC_RADIO2) {
		ChangeMode(Mode::Detected);
	} else if (nID == WIDC_RADIO3) {
		ChangeMode(Mode::Unattributed);
	} else if (nID == WIDC_LISTBOX) {
		CWndTListBox<Displayed, DisplayedDisplayer> * box = GetDlgItem<CWndTListBox<Displayed, DisplayedDisplayer>>(WIDC_LISTBOX);
		const Displayed * selection = box->GetCurSelItem();
		if (selection && selection->item1) {
			const auto & map = CScript::m_defines.GetOrBuild("II_");

			CString str;

			if (const auto nameIt = map.find(selection->item1->dwID); nameIt != map.end()) {
				str += nameIt->second;
			} else {
				str.AppendFormat("Item #%lu", selection->item1->dwID);
			}

			if (selection->item2) {
				str += " / ";

				if (const auto nameIt = map.find(selection->item2->dwID); nameIt != map.end()) {
					str += nameIt->second;
				} else {
					str.AppendFormat("Item #%lu", selection->item2->dwID);
				}
			}

			CopyToClipboard(str);
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


void CWndEquipmentSex::ChangeMode(const Mode mode) {
	if (mode == m_currentMode) return;
	m_currentMode = mode;

	m_lists.EnsureBuilt();

	CWndTListBox<Displayed, DisplayedDisplayer> * box = GetDlgItem<CWndTListBox<Displayed, DisplayedDisplayer>>(WIDC_LISTBOX);
	box->ResetContent();

	CScript::m_defines.GetOrBuild("II_");

	const std::vector<Displayed> * toList =
		mode == Mode::Vanilla ? &m_lists.vanilla :
		mode == Mode::Detected ? &m_lists.detected :
		&m_lists.unattributed;

	for (const Displayed & displayed : *toList) {
		box->Add(displayed);
	}
}

static constexpr int CenterBlockOnAxis(int axisLength, int itemLength) {
	const int axisMid = axisLength / 2;
	const int itemMid = itemLength / 2;
	return axisMid - itemMid;
}

void CWndEquipmentSex::DisplayedDisplayer::Render(
	C2DRender * const p2DRender, const CRect rect,
	const Displayed & displayed,
	const DWORD color, const WndTListBox::DisplayArgs & misc
) const {
	boost::container::small_vector<const ItemProp *, 2> toDisplay;
	if (displayed.item1) toDisplay.emplace_back(displayed.item1);
	if (displayed.item2) toDisplay.emplace_back(displayed.item2);

	if (toDisplay.empty()) {
		return;
	}

	const int blockXLength = static_cast<int>(toDisplay.size()) * 32;
	const int blockYLength = static_cast<int>(toDisplay.size()) * 18;
	const int startXOffset = CenterBlockOnAxis(64, blockXLength);
	const int startYOffset = CenterBlockOnAxis(rect.Height(), blockYLength);

	for (int i = 0; std::cmp_less(i, toDisplay.size()); ++i) {
		CTexture * texture = ItemProps::GetItemIconTexture(toDisplay[i]->dwID);
		if (texture) {
			p2DRender->RenderTexture(
				rect.TopLeft()
				+ CPoint(startXOffset + i * 32, 0)
				+ CPoint(0, CenterBlockOnAxis(rect.Height(), 32)),
				texture
			);
		}

		CPoint tidPoint = rect.TopLeft()
			+ CPoint(64 + 3, 0)
			+ CPoint(0, startYOffset + i * 18);

		const auto it = CScript::m_defines.Lookup(toDisplay[i]->dwID, "II_");
		if (it) {
			p2DRender->TextOut(tidPoint.x, tidPoint.y, it->GetString(), color);
		}

//		// Name display
//		CPoint textPoint = rect.TopLeft()
//			+ CPoint(64 + 3 + 224, 0)
//			+ CPoint(0, startYOffset + i * 18);
//
//		p2DRender->TextOut(
//			textPoint.x, textPoint.y, toDisplay[i]->szName, color
//		);
	}

	p2DRender->RenderFillRect(
		CRect(
			CPoint(rect.left + 4, rect.bottom - 1),
			CPoint(rect.right - 10, rect.bottom)
		),
		0xFF808080
	);
}

void CWndEquipmentSex::Lists::EnsureBuilt() {
	if (!empty) {
		return;
	}

	empty = false;

	class Builder {
	private:
		std::vector<CWndEquipmentSex::Displayed> res;
		std::set<const ItemProp *> alreadyAdded;

	public:
		void Push(const ItemProp * p1, const ItemProp * p2 = nullptr) {
			if (p1 && alreadyAdded.contains(p1)) return;
			if (p2 && alreadyAdded.contains(p2)) return;

			res.emplace_back(Displayed{ p1, p2 });

			if (p1) alreadyAdded.emplace(p1);
			if (p2) alreadyAdded.emplace(p2);
		}

		[[nodiscard]] auto Build() const { return res; }
		[[nodiscard]] auto Size() const { return res.size(); }
	};

	Builder forVanilla;
	Builder forDetected;
	Builder forUnattributed;

	for (const ItemProp & itemProp : prj.m_aPropItem) {
		if (itemProp.dwItemSex != SEX_MALE && itemProp.dwItemSex != SEX_FEMALE) continue;

		const ItemProp * vanilla = ItemMorph::VanillaMorph::GetTransyItem(itemProp);
		if (vanilla) forVanilla.Push(&itemProp, vanilla);

		const ItemProp * reflexive = ItemMorph::ReflexiveMorph::GetTransyItem(itemProp);
		if (reflexive) forDetected.Push(&itemProp, reflexive);

		if (vanilla && vanilla != reflexive) {
			const std::string s = std::format(
				"Error: Item {} maps to {} in vanilla and to {} in reflexive",
				itemProp.szName,
				vanilla->szName,
				reflexive ? reflexive->szName : "nothing"
			);

			g_WndMng.PutString(s.c_str());
		}

		if (!vanilla && !reflexive) {
			forUnattributed.Push(&itemProp, vanilla);
		}
	}

	vanilla = forVanilla.Build();
	detected = forDetected.Build();
	unattributed = forUnattributed.Build();
}

std::string CWndEquipmentSex::Displayed::ToString(const std::map<int, CString> & idsToDefines) const {
	std::string res;

	for (const ItemProp * prop : { item1, item2 }) {
		if (!prop) continue;
		if (!res.empty()) res += " / ";

		res += item1->szName;
		const auto iiId = idsToDefines.find(prop->dwID);
		if (iiId != idsToDefines.end()) {
			res += " (";
			res += iiId->second.GetString();
			res += ")";
		}
	}

	return res;
}
