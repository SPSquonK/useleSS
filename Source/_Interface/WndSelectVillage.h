#pragma once

class CReturnScrollMsgBox : public CWndMessageBox {
public: 
	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
}; 

class CWndSelectVillage : public CWndNeuz {
public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
}; 
