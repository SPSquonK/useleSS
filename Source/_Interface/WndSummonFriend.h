#pragma once

class CWndSummonFriendMsg : public CWndNeuz 
{ 
	OBJID m_objid = 0;
	DWORD m_dwData = 0;
	TCHAR m_szName[ MAX_NAME ] = "";
public: 
	
	void	SetData( OBJID objid, DWORD dwData, char* szName, char* szWorldName );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndSummonFriend : public CWndNeuz 
{ 
private:
	DWORD	m_dwData;
public: 
	void	SetData( WORD wId, WORD wReset );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndSummonPartyMsg : public CWndNeuz 
{ 
	CTexture			m_Texture;
public:
	void	SetData( OBJID objid, DWORD dwData, const char* szWorldName );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual HRESULT DeleteDeviceObjects();
}; 

class CWndSummonParty : public CWndNeuz 
{ 
private:
	DWORD	m_dwData;
public: 

	void	SetData( WORD wId, WORD wReset );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
};

class CWndSummonPartyUse : public CWndNeuz 
{ 
public: 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 
