#ifndef __WNDREPAIRITEMCTRL_H__
#define	__WNDREPAIRITEMCTRL_H__

#pragma	once

class C2DRender;
class CItemElem;

class CWndRepairItemCtrl : public CWndBase
{
	void	InterpretScript( CScanner & s, CPtrArray & ptrArray );
	CItemElem*	m_pFocusItem;
	int		m_nCurSel;
	int		m_nFontHeight;
	DWORD	m_nWndColor;
	DWORD	m_nFontColor;
	DWORD	m_nSelectColor;

public:
//	Constructions
	static	CTextureMng		m_textureMng;
	CWndRepairItemCtrl();
	~CWndRepairItemCtrl()	{}

	CRect	m_rect;
	BOOL	m_bDrag;
	DWORD	m_dwListCtrlStyle;
//	DWORD*		m_pItemContainer;
	DWORD*		m_pdwIdRepair;
	
//	void	InitItem( DWORD* pItemContainer );
	void	InitItem( DWORD* pdwIdRepair );
	void	Create( DWORD dwListCtrlStyle, const RECT & rect, CWndBase* pParentWnd, UINT nID );
//	void	LoadListBoxScript( LPCTSTR lpFileName );

	int		HitTest( CPoint point );

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
public:
	int		InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
								UINT nStateMask, int nImage, LPARAM lParam );
protected:
protected:
};

#endif	// __WNDREPAIRITEMCTRL_H__