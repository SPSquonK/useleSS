#ifndef __WNDUPGRADEBASE__H
#define __WNDUPGRADEBASE__H

#include "WndComponentSlots.h"

#define MAX_UPGRADE      6

class CWndUpgradeBase final : public CWndNeuz 
{
public: 
	DWORD      m_dwReqItem[2];
	int        m_nCost;

	CWndComponentSlots<MAX_UPGRADE> m_slots;

	int        m_nCount[2];
	int        m_nMaxCount;
	
	CWndUpgradeBase(); 
	~CWndUpgradeBase(); 
	
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void	OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void	OnDestroy( void );
	virtual void	OnRButtonUp( UINT nFlags, CPoint point );

private:
	bool DropMainItem(CItemElem * pItemElem);

}; 


#endif
