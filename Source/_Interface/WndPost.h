#pragma once

class CWndPostSend : public CWndNeuz
{
	int		m_nCost = 0;
	int		m_nCount = 0;
	BYTE  m_nItem = 0xFF;

public:	
	void ClearData();
	virtual ~CWndPostSend(); 
	
	
	BYTE GetItemId() { return m_nItem; }
	void SetItemId( BYTE nId );
	void SetCost( int nCost ) { m_nCost = nCost; }
	void SetCount( int nCount ) { m_nCount = nCount; }
	void SetReceive( const char* pchar );
	void SetTitle( const char* pchar );	
	void SetText( const char* pchar );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
};

class CWndPostItemWarning : public CWndNeuz 
{ 
public: 
	int	   m_nMailIndex = -1;
	
	void	SetIndex( int nIndex ) { m_nMailIndex = nIndex; }
	void	SetString(const char * string);
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndPostDeleteConfirm;
class CWndPostRead : public CWndNeuz
{
	int					m_nMailIndex;
	CWndGold			m_wndGold;
	BOOL				m_bDrag;
	CWndPostDeleteConfirm* m_pDeleteConfirm;
	
public:	
	CWndPostItemWarning*		m_pWndPostItemWarning;
	void MailReceiveItem();
	void MailReceiveGold();
	void ClearData();
	CWndPostRead(); 
	virtual ~CWndPostRead(); 
	
	void SetValue( int nMailIndex );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );	
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};


class CWndPostReceive : public CWndNeuz
{
	CTexture			m_Texture[3];
	CWndScrollBar		m_wndScrollBar;
	int					m_nSelect = -1;
	int					m_nMax = 0;
	CWndPostRead*		m_wndPostRead = nullptr;
public:	
	static constexpr int MAX_MAIL_LIST_PER_PAGE = 6;

	void UpdateScroll();
	
	virtual ~CWndPostReceive(); 

	int          GetSelectIndex( const CPoint& point );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnMouseWndSurface( CPoint point );
};
class CWndPost : public CWndNeuz
{
public:
	CWndPostSend		m_PostTabSend;
	CWndPostReceive		m_PostTabReceive;
	
	virtual ~CWndPost(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

public:
	void CloseMailRequestingBox( void );

private:
	CWndMailRequestingBox* m_pWndMailRequestingBox = nullptr;
};

class CWndPostDeleteConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int		m_nIndex = 0;
public: 
	
	void			SetValue( int nIndex ) { m_nIndex = nIndex; }
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
}; 
