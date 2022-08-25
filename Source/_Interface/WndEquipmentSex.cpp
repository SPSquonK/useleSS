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
			CString str = box->displayer.reverseIndex[selection->item1->dwID];

			if (selection->item2) {
				str += " / ";
				str += box->displayer.reverseIndex[selection->item2->dwID];
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

	const auto idsToDefines = BuildReverseIndex(CScript::m_defines, "II_");
	box->displayer.reverseIndex = idsToDefines;

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

		const auto it = reverseIndex.find(toDisplay[i]->dwID);
		if (it != reverseIndex.end()) {
			p2DRender->TextOut(
				tidPoint.x, tidPoint.y, it->second.GetString(), color
			);
		}

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

struct CStringDetectedMorphs {
	std::map<const ItemProp *, const ItemProp *> pairs;
	std::set<const ItemProp *> contained;

	CStringDetectedMorphs();

	bool Contains(const ItemProp * prop) const {
		return contained.contains(prop);
	}
};

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

	CStringDetectedMorphs stringDetector;

	for (const ItemProp & itemProp : prj.m_aPropItem) {
		if (itemProp.dwItemSex != SEX_MALE && itemProp.dwItemSex != SEX_FEMALE) continue;

		const ItemProp * vanilla = ItemMorph::GetTransyItem(itemProp);
		if (vanilla) forVanilla.Push(&itemProp, vanilla);

		if (itemProp.dwItemSex == SEX_MALE) {
			const auto propFem = stringDetector.pairs.find(&itemProp);
			if (propFem != stringDetector.pairs.end()) {
				forDetected.Push(&itemProp, propFem->second);
			}
		}

		if (!vanilla && !stringDetector.Contains(&itemProp)) {
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

std::map<int, CString> CWndEquipmentSex::BuildReverseIndex(
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

CStringDetectedMorphs::CStringDetectedMorphs() {
	const auto reverseIndex = CWndEquipmentSex::BuildReverseIndex(CScript::m_defines, "II_");
	if (reverseIndex.size() == 0) return;

	const auto trySomething = [&](const ItemProp & maleItem, const CString & name) -> bool {
		if (name == "") return false;

		const auto idIt = CScript::m_defines.find(name);
		if (idIt == CScript::m_defines.end()) return false;

		const ItemProp * femaleItem = prj.m_aPropItem.GetAt(idIt->second);
		if (!femaleItem) return false;
		if (femaleItem->dwItemSex != SEX_FEMALE) return false;

		pairs.emplace(&maleItem, femaleItem);
		contained.emplace(&maleItem);
		contained.emplace(femaleItem);

		return true;
	};

	constexpr auto Replace = [](const char * name, const char * from, const char * to) -> CString {
		CString res = name;
		const auto index = res.Find(from);

		if (index == -1) return "";

		return res.Left(index) + to + res.Right(res.GetLength() - index - strlen(from));
	};


	using DCCS = std::pair<const char *, const char *>;

	const std::initializer_list<std::pair<const char *, const char *>> pairs = {
		DCCS("_M_", "_F_"),
		DCCS("M_CHR_TUXEDO01", "F_CHR_DRESS01"),
		DCCS("M_CHR_TUXEDO02", "F_CHR_DRESS03"),
		DCCS("M_CHR_TUXEDO03", "F_CHR_DRESS04"),
		DCCS("M_CHR_BULL01", "F_CHR_COW01"),
		DCCS("M_CHR_CHINESE01", "F_CHR_MARTIALART01"),
		DCCS("M_CHR_HATTER01", "F_CHR_ALICE01"),
	};

	for (const ItemProp & prop : prj.m_aPropItem) {
		if (prop.dwItemSex != SEX_MALE) continue;


		for (const auto [male, female] : pairs) {
			bool ok = trySomething(prop, Replace(reverseIndex.at(prop.dwID), male, female));
			if (ok) break;
		}

	}

	// II_ARM_F_CLO_MAS_WIG07BL_1 II_ARM_M_CLO_MAS_WIG09BR_1
	// II_ARM_F_CLO_MAS_WIG07B_1 II_ARM_M_CLO_MAS_WIG09B_1
	// II_ARM_F_CLO_MAS_WIG07BR_1 II_ARM_M_CLO_MAS_WIG09S_1
	// II_ARM_M_CLO_MAS_WIG04BL II_ARM_F_CLO_MAS_WIG04GO
	// II_ARM_M_CLO_MAS_WIG04SB II_ARM_F_CLO_MAS_WIG04G
	// II_ARM_M_CLO_MAS_WIG06R II_ARM_F_CLO_MAS_WIG06B
}

