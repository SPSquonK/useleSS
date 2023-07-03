#pragma once

#include "resdata.h"

class CWndTextQuest : public CWndMessageBox
{ 
public: 
	CItemElem * m_pItemBase;

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndTextFromItem final : public CWndNeuz { 
public: 
	CItemElem * m_pItemBase = nullptr;

	void SetItemBase(CItemElem * pItemBase) { m_pItemBase = pItemBase; }

	BOOL Initialize(CWndBase * pWndParent, CItemElem * pItemBase, DWORD baseApp) {
		m_pItemBase = pItemBase;
		return CWndNeuz::InitDialog( baseApp, pWndParent, 0, CPoint(0, 0) );
	}

	void OnInitialUpdate() override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
}; 
