#pragma once

#include "WndSqKComponents.h"

class CWndLvReqDown : public CWndNeuz {
public: 
	class LeveledDownItemReceiver : public CWndItemReceiver {
		bool CanReceiveItem(const CItemElem & itemElem, bool) override;
	};

	static constexpr UINT WIDC_Receiver = 500;

	LeveledDownItemReceiver m_receiver;

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
	BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point) override;
}; 
