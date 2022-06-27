#include "StdAfx.h"
#include "AppDefine.h"
#include "WndEquipmentSex.h"
#include "ItemMorph.h"

BOOL CWndEquipementSex::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_EQUIPMENTSEX, pWndParent, 0, CPoint(0, 0));
}

void CWndEquipementSex::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndListBox * box = GetDlgItem<CWndListBox>(WIDC_LISTBOX);
	// box->m_nFontHeight = 48;

	ChangeMode(Mode::Vanilla);

	CWndButton * radio = GetDlgItem<CWndButton>(WIDC_RADIO);
	radio->SetGroup(TRUE);
	radio->SetCheck(TRUE);

	MoveParentCenter();
}

BOOL CWndEquipementSex::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_RADIO) {
		ChangeMode(Mode::Vanilla);
	} else if (nID == WIDC_RADIO1) {
		ChangeMode(Mode::Detected);
	} else if (nID == WIDC_RADIO2) {
		ChangeMode(Mode::Unattributed);
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


void CWndEquipementSex::ChangeMode(const Mode mode) {
	if (mode == m_currentMode) return;
	m_currentMode = mode;
	
	CWndListBox * box = GetDlgItem<CWndListBox>(WIDC_LISTBOX);
	box->ResetContent();

	m_displayed = GetItemsToDisplay(m_currentMode);


	const auto idsToDefines = BuildReverseIndex(CScript::m_defines, "II_");

	for (const Displayed & displayed : m_displayed) {
		const std::string str = displayed.ToString(idsToDefines);
		box->AddString(str.c_str());
	}
}

// TODO: get SFlyFF ListBox implementation
/*
void CWndEquipementSex::OnDraw(C2DRender * p2DRender) {
	CWndListBox * box = GetDlgItem<CWndListBox>(WIDC_LISTBOX);

	const CRect bounds = box->GetClientRect();

	const int from = box->GetScrollPos();

	CPoint topLeft = bounds.TopLeft();

	int current = from;
	while (topLeft.y + 48 < bounds.bottom) {
		if (current >= static_cast<int>(m_displayed.size())) {
			break;
		}

		const Displayed & disp = m_displayed[current];

		CRect uglyRect = CRect(topLeft, CSize(20, 30));
		p2DRender->RenderRect(&uglyRect, 0x0000FFFF);
		p2DRender->TextOut(topLeft.x, topLeft.y, disp.item1->szName);

		topLeft.y += 48;
		++current;
	}
}
*/

std::vector<CWndEquipementSex::Displayed> CWndEquipementSex::GetItemsToDisplay(const Mode mode) {
	class Builder {
	private:
		std::vector<CWndEquipementSex::Displayed> res;
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

	Builder builder;

	for (size_t i = 0; i != prj.m_aPropItem.GetSize(); ++i) {
		ItemProp * prop = prj.m_aPropItem.GetAt(i);
		if (!prop) continue;
		if (prop->dwID != i) continue;
		if (prop->dwItemSex != SEX_MALE && prop->dwItemSex != SEX_FEMALE) continue;

		switch (mode) {
			case Mode::Vanilla: {
				const ItemProp * vanilla = ItemMorph::GetTransyItem(*prop);
				if (vanilla) builder.Push(prop, vanilla);
				break;
			}
			case Mode::Detected: {


				break;
			}
			case Mode::Unattributed: {
				const ItemProp * vanilla = ItemMorph::GetTransyItem(*prop);
				if (!vanilla) {
					builder.Push(prop, vanilla);
				}
				break;
			}
		}
	}

	return builder.Build();
}

std::string CWndEquipementSex::Displayed::ToString(const std::map<int, CString> & idsToDefines) const {
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

std::map<int, CString> CWndEquipementSex::BuildReverseIndex(
	const std::map<CString, int> & defines, std::string_view prefix
) {
	std::map<int, CString> reverseIndex;

	for (const auto & [textId, numberId] : defines) {
		if (textId.Left(prefix.size()) == prefix.data()) {
			const bool dupe = reverseIndex.insert_or_assign(numberId, textId).second;
			if (dupe) {
				Error("BuildReverseIndex(%s): Duplicated entry %d - %s and %s",
					prefix.data(), numberId, reverseIndex[numberId], textId.GetString()
					);
			}
		}
	}
	
	return reverseIndex;
}

