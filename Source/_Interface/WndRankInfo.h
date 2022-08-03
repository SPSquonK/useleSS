#ifndef __WNDRANKINFO__H
#define __WNDRANKINFO__H

#include "WndRankCmn.h"

class CWndRankInfoTabLevel : public CWndRankTab {
public:
	CWndRankInfoTabLevel();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankInfoTabPlayTime : public CWndRankTab {
public:
	CWndRankInfoTabPlayTime();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankInfo : public CWndNeuz 
{ 
public: 
	CWndRankInfo(); 
	~CWndRankInfo(); 
	
	CWndRankInfoTabLevel	m_WndRankInfoTabLevel;
	CWndRankInfoTabPlayTime m_WndRankInfoTabPlayTime;

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

