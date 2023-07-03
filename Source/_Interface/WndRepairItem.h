#pragma once

class C2DRender;
class CItemElem;

class CWndRepairItemCtrl : public CWndBase {
	CItemElem * m_pFocusItem;
	int		m_nCurSel;

public:
	//	Constructions
	CWndRepairItemCtrl();
	~CWndRepairItemCtrl() override = default;

	DWORD * m_pdwIdRepair;

	void	InitItem(DWORD * pdwIdRepair);
	void	Create(DWORD dwListCtrlStyle, const RECT & rect, CWndBase * pParentWnd, UINT nID);

	int		HitTest(CPoint point);

	void OnLButtonDown(UINT nFlags, CPoint point) override;

	virtual	void	SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void	OnInitialUpdate(void);
	virtual	void	OnDraw(C2DRender * p2DRender);
	virtual	BOOL	OnEraseBkgnd(C2DRender * p2DRender);
	virtual	void	PaintFrame(C2DRender * p2DRender);
	virtual	BOOL	OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	//	Implementation
};


class CWndRepairItem : public CWndNeuz 
{ 
public:
	CWndStatic* pWndStaticCost;
	CWndRepairItemCtrl	m_wndItemCtrl;
	DWORD	m_adwIdRepair[MAX_REPAIRINGITEM];
	DWORD	m_dwCost;


	void OnInit( void );

public: 
	CWndRepairItem(); 
	~CWndRepairItem(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual	void OnDestroy( void );

private:
	void OnItemElemDrop(CItemElem * pItemBase, DWORD targetSlot);
}; 
