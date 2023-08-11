#pragma once

#include "WndComponentSlots.h"

class CWndUpgradeBase final : public CWndNeuz 
{
public: 
	CWndComponentSlot m_slot;
	
	CWndUpgradeBase(); 
	~CWndUpgradeBase(); 
	
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void	OnDestroy( void );
	virtual void	OnRButtonUp( UINT nFlags, CPoint point );

private:
	bool DropMainItem(CItemElem * pItemElem);

}; 
