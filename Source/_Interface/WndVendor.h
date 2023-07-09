#ifndef __WNDVENDOR__H
#define __WNDVENDOR__H

#include "wndvendorctrl.h"
#include "wndregvend.h"
#include "wndvendorbuy.h"

class CWndVendorMessage;

class CWndVendor final : public CWndNeuz 
{
public:
	CWndVendorCtrl	m_wndctrlVendor;
	CWndVendorBuy*	m_pWndVendorBuy;
	CMover*	m_pVendor;
	CWndRegVend*	m_pWndRegVend;

	CWndVendorMessage* m_pwndVenderMessage;

public: 
	CWndVendor(); 
	~CWndVendor(); 

	void	SetVendor( CMover* pVendor );

	BOOL Initialize( CWndBase * pWndParent = nullptr );
	void			ReloadItemList();
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual BOOL	Process();
	virtual	void	OnInitialUpdate(); 
	virtual BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void	OnSize( UINT nType, int cx, int cy ); 
	virtual void	OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void	OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void	OnDestroy( void );
}; 

class CWndVendorMessage final : public CWndNeuz 
{ 
public: 
	void RemoveChattingMemver(LPCTSTR lpszName);
	void AddChattingMemver( LPCTSTR lpszName );
	CWndVendorMessage(); 
	~CWndVendorMessage(); 
	
	CString m_strPlayer;

	CWndText		m_wndChat;
	CWndText		m_wndInfo;
	BOOL			m_nIsOwner;

	void WriteBuyInfo(char *pBuyerName, CString strItem, int nItemNum, int	nTotalCost);
	
	void InitSize( void );
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage );
	void OnInputString();
	
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndVendorConfirm : public CWndCustomMessageBox
{ 
public: 
	CString m_strVendorName;
	void	SetVendorName( CString str );
	void	SetValue( CString str );
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

#endif
