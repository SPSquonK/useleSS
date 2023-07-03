#pragma once

#include "WndSqKComponents.h"

class CWndBlessingCancel : public CWndNeuz {
	class CFashionReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	static constexpr UINT WIDC_Receiver = 900;
	CFashionReceiver m_receiver;

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnInitialUpdate() override; 
}; 
