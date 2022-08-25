#pragma once

#include <map>
#include <optional>
#include <string_view>

class CWndEquipmentSex : public CWndNeuz {
public:
	enum class Mode {
		None, Vanilla, Detected, Unattributed
	};

	struct Displayed {
		const ItemProp * item1;
		const ItemProp * item2;

		[[nodiscard]] std::string ToString(const std::map<int, CString> & idsToDefines) const;
	};

	struct DisplayedDisplayer {
		std::map<int, CString> reverseIndex;

		void Render(
			C2DRender * const p2DRender, CRect rect,
			const Displayed & displayed,
			DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	struct Lists {
		bool empty = true;
		std::vector<Displayed> vanilla;
		std::vector<Displayed> detected;
		std::vector<Displayed> unattributed;

		void EnsureBuilt();
	};

private:
	Mode m_currentMode = Mode::None;
	Lists m_lists;

	void ChangeMode(Mode newMode);
	void AddQuantity(UINT widgetId, size_t size);

public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

	static std::map<int, CString> BuildReverseIndex(
		const std::map<CString, int> & defines, std::string_view prefix
	);
};
