#pragma once

class CWndIndirectTalk final : public CWndNeuz {
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
};
