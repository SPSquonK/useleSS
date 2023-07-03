#pragma once

#include "WndTListBox.hpp"
#include <unordered_map>

class CWndAdminCreateItem : public CWndNeuz { 
public:
	struct ExtraItemProp {
		std::vector<std::string> names;
		std::string otherName;
	};

	struct Item {
		CString name;
		const ItemProp * itemProp;
		const ExtraItemProp * extraItemProp;
		std::optional<CTexture *> texture;

		Item(const ItemProp * itemProp, const ExtraItemProp * extraItemProp);
	};

	struct Displayer {
		bool hasIcons = true;

		void Render(
			C2DRender * const p2DRender, CRect rect,
			Item & item, DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	using ItemPropListBox = CWndTListBox<Item, Displayer>;
		
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();

	static void BoundListBoxOfComboBoxSize(CWndComboBox & comboBox, std::optional<int> minSize, std::optional<int> maxSize);

	static short GetQuantityToCreateOf(const ItemProp * itemProp);
	static void STDStringToLower(std::string & string);
	static std::map<CString, CString> LoadStrings(CScanner & scanner);

private:
	void LoadExtraItemProps();

private:
	[[nodiscard]] std::optional<DWORD> GetSelectedInComboBox(UINT comboBoxId) /* const */;
	void UpdateItems();
	void UpdateItems(DWORD kind, DWORD sex, DWORD job, DWORD level, std::string textFilter);

	void UpdateRendering();


	CFixedArray<ExtraItemProp> extraItemProps;

}; 
