#pragma once

#include "WndTListBox.hpp"

class CWndAdminCreateItem : public CWndNeuz { 
public:
	struct Item {
		CString name;
		const ItemProp * itemProp;

		explicit Item(const ItemProp * itemProp);
	};

	struct Displayer {
		void Render(
			C2DRender * const p2DRender, const CRect rect,
			const Item & item, const DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	using ItemPropListBox = CWndTListBox<Item, Displayer>;
		
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();

private:
	void UpdateItems(DWORD kind, DWORD sex, DWORD job, DWORD level);
}; 
