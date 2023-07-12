#pragma once

#include <expected>

class CWndGuildName final : public CWndNeuz 
{ 
public: 
	CWndGuildName(); 

	BYTE	m_nId;
	void	SetData( BYTE nId = 0xff )	{	m_nId	= nId;	}

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
}; 


class CWndGuildNickName final : public CWndNeuz 
{ 
public: 
	u_long m_idPlayer;

public:
	CWndGuildNickName(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 
