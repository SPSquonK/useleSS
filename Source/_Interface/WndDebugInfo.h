// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDDEBUGINFO_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDDEBUGINFO_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////////////////
// DebugInfo
//
class CWndDebugInfo : public CWndNeuz
{

public:
	CWndDebugInfo(); 
	virtual ~CWndDebugInfo();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );

};
// Control 테스트 윈도 

class CWndControlTest : public CWndNeuz
{
	CWndText m_wndText;
	CWndButton m_wndMusic;
	CWndButton m_wndSound;
	CWndComboBox m_wndComboBox;
	CWndListCtrl m_wndlistCtrl;
	CWndButton   m_wndButtonIcon;
	CWndButton   m_wndButtonReport;
	CWndButton   m_wndButtonList;
public:
	CWndControlTest(); 
	virtual ~CWndControlTest();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


#endif // !defined(AFX_WNDDEBUGINFO_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
