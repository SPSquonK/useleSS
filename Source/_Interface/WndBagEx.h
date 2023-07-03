#ifndef __WNDBAGEX__H
#define __WNDBAGEX__H

class CWndBagEx : public CWndNeuz 
{ 
public: 

	CWndItemCtrl	m_wndItemCtrl[3];
	CRect			m_RectItemCtrl[3];
	BOOL			m_bUse[3];
	CTexture*		m_pTexNouse;
	CTexture*		m_pTexIco;
	CTexture*		m_pTexIco_empty;

	CWndBagEx(); 
	~CWndBagEx(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );

	void	InitItem( void );
}; 
#endif
