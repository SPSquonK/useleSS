#include "StdAfx.h"
#include "AppDefine.h"
#include "WndEquipmentSex.h"

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
	
	CWndListBox * box = GetDlgItem<CWndListBox>(WIDC_LISTBOX);
	box->ResetContent();

	m_displayed = GetItemsToDisplay(m_currentMode);

	for (const Displayed & displayed : m_displayed) {
		const std::string str = displayed.ToString();
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
	std::vector<CWndEquipementSex::Displayed> res;

	for (size_t i = 0; i != prj.m_aPropItem.GetSize(); ++i) {
		ItemProp * prop = prj.m_aPropItem.GetAt(i);
		if (!prop) continue;
		if (prop->dwID != i) continue;

		res.emplace_back(Displayed{ prop, nullptr });

		if (res.size() >= 15) break;
	}

	return res;
}

std::string CWndEquipementSex::Displayed::ToString() const {
	std::string res;

	for (const ItemProp * prop : { item1, item2 }) {
		if (!prop) continue;
		if (!res.empty()) res += " / ";

		res += item1->szName;
		std::optional<CString> iiId = CWndEquipementSex::FindStringIdOf(CScript::m_defines, prop->dwID, "II_");
		if (iiId) {
			res += " (";
			res += iiId->GetString();
			res += ")";
		}
	}

	return res;
}

std::optional<CString> CWndEquipementSex::FindStringIdOf(
	const std::map<CString, int> & defines, int defineId, std::string_view prefix
) {
	const auto it = std::ranges::find_if(defines,
		[&](const std::pair<const CString, int> & pair) {
			if (pair.second != defineId) return false;
			return pair.first.Left(prefix.size()) == prefix.data();
		}
	);

	if (it == defines.end()) return std::nullopt;
	return it->first;
}

