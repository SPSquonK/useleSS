#pragma once

class CReturnScrollMsgBox : public CWndMessageBox {
public: 
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
}; 

class CWndSelectVillage : public CWndNeuz {
public: 
	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
}; 
