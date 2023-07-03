#pragma once

class CWndCommand final : public CWndListBox {
public: 
	CWndBase * m_pWndEdit = nullptr;

	~CWndCommand() override = default;

	virtual BOOL Initialize(CWndBase * pWndParent = NULL);
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;
	void OnLButtonDblClk(UINT nFlags, CPoint point) override;
	void OnSetFocus(CWndBase * pOldWnd) override;
	void OnKillFocus(CWndBase * pNewWnd) override;

private:
	void DrawKoreanDescription(C2DRender * p2DRender);
};
