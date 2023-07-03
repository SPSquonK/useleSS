#pragma once

#include "WndComponentSlots.h"

class CPiercingMessageBox;

class CWndPiercing final : public CWndNeuz {
public:
	CSfx * m_pSfx = nullptr;

	CWndComponentSlots<3> m_slots;
	
	CWndPiercing(); 
	~CWndPiercing(); 
	
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void	OnDestroy( void );
	virtual void	OnRButtonUp( UINT nFlags, CPoint point );
}; 

class CPiercingMessageBox final : public CWndCustomMessageBox {
public:
	std::array<OBJID, 3> m_Objid;

	explicit CPiercingMessageBox(const std::array<CWndComponentSlot, 3> & slots);
	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
};
