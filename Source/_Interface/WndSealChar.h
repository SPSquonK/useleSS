#pragma once

class CWndSealChar : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndSealCharSelect : public CWndNeuz 
{ 
public: 
	char	m_szSrc1[ MAX_NAME ] = "";
	OBJID	m_idSrc1;
	char	m_szSrc2[ MAX_NAME ] = "";
	OBJID	m_idSrc2;
	LONG	m_lPlayerSlot1;
	LONG	m_lPlayerSlot2;
	short	m_sCount;
	
	CWndSealCharSelect(); 

	void	SetData( short sCount,LONG lPlayerSolt1,LONG lPlayerSolt2,u_long uPlayerID1,u_long uPlayerID2, char* szName1, char* szName2 );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndSealCharSend : public CWndNeuz 
{ 
public: 
	char	m_szSrc1[ MAX_NAME ];	// 이름
	OBJID	m_idSrc1;
	
	CWndSealCharSend(); 

	void	SetData( u_long uPlayerID1, char* szName1 );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndSealCharSet : public CWndNeuz 
{ 
public: 
	char	m_szSrc1[ MAX_NAME ];	// 이름
	OBJID	m_idSrc1;
	DWORD	m_dwData;

	CWndSealCharSet(); 

	void SetData( DWORD dwId, WORD wReset );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 
