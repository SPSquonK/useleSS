#ifndef __WNDVENDORBUY__H
#define __WNDVENDORBUY__H

#include "WndRegVend.h"

class CWndVendorBuy final : public CWndNeuz
{
	CItemElem *	m_pItemBase;
	int	m_iIndex;
public:
	CWndVendorBuy(CItemElem * pItemBase, int iIndex)
		: m_pItemBase(pItemBase), m_iIndex(iIndex) {}

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();

	BOOL	        m_bIsFirst = FALSE;
	CALC_DATA       m_Calc{ 0, '\0' };
	virtual	void	OnChar(UINT nChar);
	int				ProcessCalc( CALC_DATA calc, int num );
};

#endif
