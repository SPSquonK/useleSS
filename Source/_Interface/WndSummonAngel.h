#pragma once

#include "WndSqKComponents.h"

class CWndOrichalcumReceiver : public CWndItemReceiver {
public:
	CWndOrichalcumReceiver()
		: CWndItemReceiver(Features{ .colorWhenHoverWithItem = 0x60FFFF00 }) {
	}

	bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override {
		return ItemProps::IsOrichalcum(itemElem);
	}
};

class CWndMoonstoneReceiver : public CWndItemReceiver {
public:
	CWndMoonstoneReceiver()
		: CWndItemReceiver(Features{ .colorWhenHoverWithItem = 0x60FFFF00 }) {
	}

	bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override {
		return ItemProps::IsMoonstone(itemElem);
	}
};

class CWndSummonAngel final : public CWndNeuz {
private:
	static constexpr unsigned int MaxSlotPerItem = 10;
	static constexpr UINT StartOffsetWidcSlots = 1500;

	CWndText * m_pText = nullptr;
	CWndStatic * m_pStatic[3] = { nullptr, nullptr, nullptr };
	
	std::array<CWndOrichalcumReceiver, MaxSlotPerItem> m_oriReceivers;
	std::array<CWndMoonstoneReceiver, MaxSlotPerItem> m_moonReceivers;

	bool m_nowStarting = false;

	int m_nOrichalcum = 0;
	int m_nMoonstone  = 0;

	float m_RedAngelRate   = 0.f;
	float m_BlueAngelRate  = 0.f;
	float m_GreenAngelRate = 0.f;

	
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	void OnDestroy() override;

	void SetQuestText(const CHAR * szChar);
	void SetDieFromInventory(CItemElem & pItemElem);

private:
	
	void OnChangedItems();
	void ReFreshAll();
	void SummonRateRefresh();

	template <typename F> void ForEachReceiver(F f) {
		for (auto & receiver : m_oriReceivers)  { f(receiver); }
		for (auto & receiver : m_moonReceivers) { f(receiver); }
	}
}; 


