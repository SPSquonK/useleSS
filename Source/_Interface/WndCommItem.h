#pragma once

class C2DRender;
class CItemElem;

#include "WndComponentSlots.h"
#include <boost/container/small_vector.hpp>

class CWndCommItemCtrl final : public CWndBase {
	int           m_nFontHeight;
	CWndScrollBar m_wndScrollBar;

	CWndWorld * pWndWorld;
	boost::container::small_vector<DWORD, SM_MAX * 2> m_dwDraw;

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

public:
	CRect         m_rect;
	DWORD         m_dwListCtrlStyle;

	[[nodiscard]] int GetMaxBuff() const;			// 출력해야할 MAX값얻어오기
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
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	void OnInitialUpdate() override;
}; 

class CWndCommercialElem final : public CWndNeuz 
{ 
public: 
	CWndComponentSlots<2> m_slots;

	CWndCommercialElem(); 
	~CWndCommercialElem(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

private:
	bool IsRestrictionItem(CItemElem * pItemElem);
	bool IsUpgradeItem(CItemElem * pItemElem);
	bool IsSMItem(CItemElem * pItemElem);
}; 

class CWndRemoveElem final : public CWndNeuz 
{ 
public: 
	BYTE		m_nType = 0;
	int			m_nParts = 0;
	DWORD		m_dwItemId = 0;
	OBJID		m_objid = 0;
	CItemElem*	m_pItemElem = nullptr;
	BOOL		m_bSetting = FALSE;

	void OnSetItem( BYTE nType, DWORD dwItemId, OBJID objid, int nParts, CItemElem* pItemElem );
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
}; 

