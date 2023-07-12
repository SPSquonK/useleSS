#pragma once

class CReturnScrollMsgBox : public CWndCustomMessageBox {
public: 
	BOOL Initialize(CWndBase * pWndParent = nullptr) final;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
}; 

class CWndSelectVillage final : public CWndNeuz {
public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
}; 
