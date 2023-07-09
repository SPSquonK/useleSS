#pragma once

class CWndCloseExistingConnection final : public CWndNeuz {
private:
	CWndText	m_wndText;

	void EnableLoginAndDestroy();
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnInitialUpdate() override;
}; 
