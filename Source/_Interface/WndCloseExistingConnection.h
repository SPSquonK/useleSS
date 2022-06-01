#pragma once

class CWndCloseExistingConnection : public CWndNeuz {
private:
	CWndText	m_wndText;

	void EnableLoginAndDestroy();
public:
	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override;
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnInitialUpdate() override;
}; 
