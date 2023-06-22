#pragma once

#include <expected>

class CWndGuildName : public CWndNeuz 
{ 
public: 
	CWndGuildName(); 

	BYTE	m_nId;
	void	SetData( BYTE nId = 0xff )	{	m_nId	= nId;	}

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
}; 


class CWndGuildNickName : public CWndNeuz 
{ 
public: 
	u_long m_idPlayer;

public:
	CWndGuildNickName(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 
