#pragma once

class CWndChatFilter : public CWndNeuz 
{ 
public: 
	virtual void SetButtonStatus();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 
