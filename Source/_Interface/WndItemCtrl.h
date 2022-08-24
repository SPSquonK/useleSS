// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDITEMLIST_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDITEMLIST_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C2DRender;
class CItemElem;

class CWndItemCtrl : public CWndBase
{
public:
	CWndItemCtrl();
	virtual ~CWndItemCtrl();

public:
	CItemContainer* m_pItemContainer;

	static CTextureMng	m_textureMng;

	void	Create( DWORD m_dwListCtrlStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	void	InitItem( CItemContainer* pItemContainer, DWORD SendToId );
	CItemElem* GetItem( DWORD dwIndex );
	int		GetItemCount() const;
	BOOL	SetItem(const LVITEM* pItem);

	int		GetSelectedItem( int i );
	UINT	GetSelectedCount();
	BOOL	IsSelectedItem( int iItem );

// Operations

	void	RanderIcon( C2DRender* p2DRender, CItemElem* pItemElem, int x, int y, int nParent, int nalpha = 255 );

	CItemElem* GetItemFromArr( DWORD dwIndex );
	void	SetScrollBarVisible( BOOL bVisible = FALSE ) { m_wndScrollBar.SetVisible( bVisible ); };
	void	SetDieFlag(BOOL flag);
// Overridables
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual BOOL OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	
protected:
	DWORD			m_dwFrame;
	CDWordArray		m_dwSelectAry;
	BOOL			m_bDrag;
	BOOL			m_bVisibleCount;		// °¹¼ö¸¦ Ç¥½ÃFLAG
	CItemElem*		m_pFocusItem;
	int				m_nCurSelect;
	int				m_nFontHeight;
	DWORD			m_nWndColor;
	DWORD			m_nFontColor; 
	DWORD			m_nSelectColor;
	CWndScrollBar	m_wndScrollBar;
	CPtrArray		m_aItems;
	CPtrArray		m_aColumns;
	int				m_nOnSelect;
	CItemElem*		m_pArrayItemElem[100];
	int				m_nArrayCount;

	int		HitTest( CPoint point );
	
public:
	void	UpdateTooltip( void );

	////{{AFX_MSG(CListCtrl)
	//afx_msg void OnNcDestroy();
	////}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
public:
	BOOL m_useDieFlag;
};

#endif // !defined(AFX_WNDITEMLIST_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

