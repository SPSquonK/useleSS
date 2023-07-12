#ifndef __WNDVENDORCTRL_H__
#define	__WNDVENDORCTRL_H__

#pragma	once

class C2DRender;
class CItemElem;

class CWndVendorCtrl : public CWndBase
{
	CTexture*		   m_pTex;
	CItemElem *	m_pFocusItem;
	int		m_nCurSel;
	int		m_nFontHeight;
	DWORD	m_nWndColor;
	DWORD	m_nFontColor;
	DWORD	m_nSelectColor;

public:
//	Constructions
	static	CTextureMng		m_textureMng;
	CWndVendorCtrl();
	~CWndVendorCtrl()	{}

	CRect	m_rect;
	BOOL	m_bDrag;
	DWORD	m_dwListCtrlStyle;
	CMover*		m_pMover;
	
	void	InitVendor( CMover* pMover );
	void	Create( DWORD dwListCtrlStyle, const RECT & rect, CWndBase* pParentWnd, UINT nID );

	int		HitTest( CPoint point );
	CItemElem * GetItem( BYTE i );
	void	OnDrawItemInfo( C2DRender* p2DRender, CItemElem* pItemElem, int nX, int nY );
	CString GetNumberFormatSelling( LPCTSTR szNumber );

//	Overridables
	virtual	void	SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void	OnInitialUpdate( void );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	virtual	void	OnRButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnRButtonDown( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void	OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual	BOOL	OnEraseBkgnd( C2DRender* p2DRender );
	virtual	void	PaintFrame( C2DRender* p2DRender );
	virtual	void	OnMouseMove( UINT nFlags, CPoint point );
	virtual	BOOL	OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
//	Implementation

};

#endif	// __WNDVENDORCTRL_H__