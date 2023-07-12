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
// Constructors
public:
	CRect         m_rect;
	DWORD         m_dwListCtrlStyle;
	CMover*		  m_pMover;

	void InitItem( CMover* pMover );

	CWndTradeCtrl();

	void Create( DWORD m_dwListCtrlStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );

	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

};



#endif // !defined(AFX_WNDTRADECTRL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

