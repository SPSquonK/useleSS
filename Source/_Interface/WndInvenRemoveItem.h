#pragma once

class CWndInvenRemoveItem final : public CWndNeuz {
private:
	CItemElem*	m_pItemElem    = nullptr;
	LPWNDCTRL		m_pWndItemCtrl = nullptr;
	CWndEdit*		m_pWndEditNum  = nullptr;

public:
	void InitItem(CItemElem * pItemElem);
	
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	void OnMouseWndSurface(CPoint point) override;

private:
	std::optional<int> OnButtonOK();
	void SetEditNumToMax();
	static std::optional<int> StringToInt(LPCTSTR string);
};
