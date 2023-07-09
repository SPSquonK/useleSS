#pragma once

class CWndPostSend final : public CWndNeuz
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
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};

class CWndPostItemWarning final : public CWndNeuz 
{ 
public: 
	int	   m_nMailIndex = -1;
	
	void	SetIndex( int nIndex ) { m_nMailIndex = nIndex; }
	void	SetString(const char * string);
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndPostDeleteConfirm;
class CWndPostRead final : public CWndNeuz
{
	int					m_nMailIndex = -1;
	CWndGold			m_wndGold;
	BOOL				m_bDrag = FALSE;
	CWndPostDeleteConfirm* m_pDeleteConfirm = nullptr;
	
public:	
	CWndPostItemWarning*		m_pWndPostItemWarning = nullptr;
	void MailReceiveItem();
	void MailReceiveGold();
	void ClearData();
	
	void SetValue( int nMailIndex );
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );	
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};


class CWndPostReceive final : public CWndNeuz
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
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnMouseWndSurface( CPoint point );
};
class CWndPost final : public CWndNeuz
{
public:
	CWndPostSend		m_PostTabSend;
	CWndPostReceive		m_PostTabReceive;
	
	virtual ~CWndPost(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

public:
	void CloseMailRequestingBox( void );

private:
	CWndMailRequestingBox* m_pWndMailRequestingBox = nullptr;
};

class CWndPostDeleteConfirm final : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int		m_nIndex = 0;
public: 
	
	void			SetValue( int nIndex ) { m_nIndex = nIndex; }
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
}; 
