// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDTRADECTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDTRADECTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C2DRender;
class CItemElem;

class CWndTradeCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 

	CItemElem *    m_pFocusItem;
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	DWORD         m_nWndColor   ;
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;
	CPtrArray     m_aItems;
	CPtrArray     m_aColumns;

// Constructors
public:
static CTextureMng m_textureMng;
	CRect         m_rect;
	BOOL          m_bDrag;
	DWORD         m_dwListCtrlStyle;
	CMover*		  m_pMover;

	void InitItem( CMover* pMover );

	CWndTradeCtrl();
	virtual ~CWndTradeCtrl();

	void Create( DWORD m_dwListCtrlStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 

// Attributes
	int GetItemCount() const;
	BOOL SetItem(const LVITEM* pItem);

// Operations
	int InsertItem(const LVITEM* pItem);

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

};



#endif // !defined(AFX_WNDTRADECTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

