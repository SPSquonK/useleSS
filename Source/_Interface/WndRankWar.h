#ifndef __WNDRANKWAR__H
#define __WNDRANKWAR__H

#include "WndRankCmn.h"

class CWndRankWarTabGiveUp : public CWndRankTab {
public:
	CWndRankWarTabGiveUp();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankWarTabLose : public CWndRankTab {
public:
	CWndRankWarTabLose();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankWarTabWin : public CWndRankTab {
public:
	CWndRankWarTabWin();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankWar : public CWndNeuz 
{ 
public: 
	CWndRankWar(); 
	~CWndRankWar(); 

	CWndRankWarTabGiveUp	m_WndRankWarTabGiveUp;
	CWndRankWarTabLose		m_WndRankWarTabLose;
	CWndRankWarTabWin		m_WndRankWarTabWin;	

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif
