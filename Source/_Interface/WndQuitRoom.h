#pragma once

class CWndQuitRoom final : public CWndNeuz {
public: 
	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnDraw(C2DRender * p2DRender) override {};
	void OnInitialUpdate() override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;
};
