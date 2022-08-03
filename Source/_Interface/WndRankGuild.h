#ifndef __WNDRANKGUILD__H
#define __WNDRANKGUILD__H

#include "WndRankCmn.h"

class CWndRankTabBest : public CWndRankTab {
public:
	CWndRankTabBest();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankTabUnited : public CWndRankTab {
public:
	CWndRankTabUnited();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankTabPenya : public CWndRankTab {
public:
	CWndRankTabPenya();
	CString ToString(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankGuild : public CWndNeuz 
{ 
public: 
	CWndRankGuild(); 
	~CWndRankGuild(); 
	
	CWndRankTabBest		m_WndRankTabBest;
	CWndRankTabUnited	m_WndRankTabUnited;
	CWndRankTabPenya	m_WndRankTabPenya;

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