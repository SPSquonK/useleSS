#ifndef __WNDCOMMITEM__H
#define __WNDCOMMITEM__H

class C2DRender;
class CItemElem;

#include "WndComponentSlots.h"

class CWndCommItemCtrl final : public CWndBase {
	int           m_nFontHeight;
	CWndScrollBar m_wndScrollBar;

	CWndWorld * pWndWorld;
	int			m_dwDraw[SM_MAX + MAX_SKILLINFLUENCE];
	int			m_nMaxDraw;

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

public:
	CRect         m_rect;
	DWORD         m_dwListCtrlStyle;

	int		GetMaxBuff();			// 출력해야할 MAX값얻어오기
	void	DrawSM(C2DRender * p2DRender, CPoint * pPoint, int x, int & nScroll);
	void	DrawSkill(C2DRender * p2DRender, CPoint * pPoint, int x, int & nScroll);

	CWndCommItemCtrl();
	~CWndCommItemCtrl() override;

	void Create(DWORD m_dwListCtrlStyle, RECT & rect, CWndBase * pParentWnd, UINT nID);


// Overridables
	void OnMouseWndSurface(CPoint point) override;
	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE) override;
	void OnInitialUpdate() override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnSize(UINT nType, int cx, int cy) override;
	BOOL OnEraseBkgnd(C2DRender * p2DRender) override;
	void PaintFrame(C2DRender * p2DRender) override;

	HRESULT RestoreDeviceObjects() override;
	HRESULT InvalidateDeviceObjects() override;
	HRESULT DeleteDeviceObjects() override;
};

class CWndCommItem final : public CWndNeuz {
public:
	CWndCommItemCtrl m_wndCommItemCtrl;
public: 
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	void OnInitialUpdate() override;
}; 

class CWndCommercialElem : public CWndNeuz 
{ 
public: 
	CWndComponentSlots<2> m_slots;

	
	
	
	CWndCommercialElem(); 
	~CWndCommercialElem(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

private:
	bool IsRestrictionItem(CItemElem * pItemElem);
	bool IsUpgradeItem(CItemElem * pItemElem);
	bool IsSMItem(CItemElem * pItemElem);
}; 

class CWndRemoveElem : public CWndNeuz 
{ 
public: 
	BYTE		m_nType;
	int			m_nParts;
	DWORD		m_dwItemId;
	OBJID		m_objid;
	CItemElem*	m_pItemElem;
	BOOL		m_bSetting;

	CWndRemoveElem(); 
	~CWndRemoveElem(); 

	void OnSetItem( BYTE nType, DWORD dwItemId, OBJID objid, int nParts, CItemElem* pItemElem );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#endif
