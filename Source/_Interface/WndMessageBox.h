// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDMESSAGEBOX_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDMESSAGEBOX_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndMessageBox : public CWndNeuz
{
	std::array<CWndButton, 3> m_wndButtons;

protected:
	CWndText   m_wndText   ;
	UINT       m_nType     ;

public:
	BOOL Create(LPCTSTR lpszMessage,UINT nType,const RECT& rect,UINT nID, CWndBase* pWndParent = NULL );
	virtual void OnEnter( UINT nChar );

	
	virtual	void OnInitialUpdate();
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType = MB_OK );
	//BOOL Initialize(CString strMessage,UINT nType = MB_OK);

	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
//	virtual BOOL OnCommand(UINT nID,DWORD dwMessage);
};
class CWndMessageBoxUpper : public CWndNeuz
{
protected:
	std::array<CWndButton, 3> m_wndButtons;
	CWndText   m_wndText   ;
	UINT       m_nType     ;
	BOOL	   m_bPostLogoutMsg	= FALSE;

public:
	
	BOOL Create(LPCTSTR lpszMessage,UINT nType,const RECT& rect,UINT nID, CWndBase* pWndParent = NULL );
	
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType = MB_OK, BOOL bPostLogoutMsg = FALSE );
	//BOOL Initialize(CString strMessage,UINT nType = MB_OK);
	
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	//	virtual BOOL OnCommand(UINT nID,DWORD dwMessage);
};
#define DECLARE_WNDMESSAGEBOX( class_name ) \
	class class_name : public CWndMessageBox \
	{ \
		virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); \
		BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult); \
	}; 

#endif // !defined(AFX_WNDMESSAGEBOX_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
