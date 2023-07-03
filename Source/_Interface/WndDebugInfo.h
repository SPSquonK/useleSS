#pragma once

/// DebugInfo
class CWndDebugInfo final : public CWndNeuz {
public:
	CWndDebugInfo(); 
	
	void OnDraw(C2DRender* p2DRender) override;
	void OnInitialUpdate() override;
	BOOL Initialize(CWndBase* pWndParent = NULL);

	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnDestroyChildWnd( CWndBase* pWndChild ) override;
};
