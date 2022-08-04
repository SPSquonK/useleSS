#pragma once

#include "guild.h"
#include <boost/container/static_vector.hpp>
#include <memory>

class CWndRankTab : public CWndNeuz {
private:
	CGuildRank::RANKING m_rank;
	UINT m_appId;

public:
	using ValuesToPrint = boost::container::static_vector<int, 2>;

	CWndRankTab(CGuildRank::RANKING rank, UINT appId) : m_rank(rank), m_appId(appId) {}

	virtual ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) = 0;

	int		m_nCurrentList = 0;			// 출력될 멤버리스트의 시작 인덱스.
	int		m_nMxOld = 0;
	int   m_nMyOld = 0;			// 과거 좌표.

	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();

private:
	void MoveCurrentList(int delta);
};


class CWndRankTabBest : public CWndRankTab {
public:
	CWndRankTabBest();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankTabUnited : public CWndRankTab {
public:
	CWndRankTabUnited();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankTabPenya : public CWndRankTab {
public:
	CWndRankTabPenya();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankGuild : public CWndNeuz {
public:
	CWndRankTabBest		m_WndRankTabBest;
	CWndRankTabUnited	m_WndRankTabUnited;
	CWndRankTabPenya	m_WndRankTabPenya;

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate();
};


class CWndRankInfoTabLevel : public CWndRankTab {
public:
	CWndRankInfoTabLevel();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankInfoTabPlayTime : public CWndRankTab {
public:
	CWndRankInfoTabPlayTime();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankInfo : public CWndNeuz {
public:
	CWndRankInfoTabLevel	m_WndRankInfoTabLevel;
	CWndRankInfoTabPlayTime m_WndRankInfoTabPlayTime;

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate();
};


class CWndRankWarTabGiveUp : public CWndRankTab {
public:
	CWndRankWarTabGiveUp();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankWarTabLose : public CWndRankTab {
public:
	CWndRankWarTabLose();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankWarTabWin : public CWndRankTab {
public:
	CWndRankWarTabWin();
	ValuesToPrint GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) override;
};

class CWndRankWar : public CWndNeuz {
public:
	CWndRankWarTabGiveUp	m_WndRankWarTabGiveUp;
	CWndRankWarTabLose		m_WndRankWarTabLose;
	CWndRankWarTabWin		m_WndRankWarTabWin;

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual	void OnInitialUpdate();
};

