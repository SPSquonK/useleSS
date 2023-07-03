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

	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();

private:
	void MoveCurrentList(int delta);
};

class CWndTabCtrlManager {
private:
	std::vector<std::unique_ptr<CWndBase>> m_tabs;

public:
	// API 1: Add a tab into a given CWndTablCtrl

	template<typename WindowType>
	void Add(CWndTabCtrl & tabCtrl, CRect rect, DWORD widgetId, DWORD textId) {
		CWndBase * const window = m_tabs.emplace_back(std::make_unique<WindowType>()).get();
		Add(tabCtrl, rect, *window, widgetId, textId);
	}


	// API 2: For a given CWndTabCtrl, add many tabs

	class AddInTabCtrl {
		CWndTabCtrlManager & m_manager;
		CWndTabCtrl & m_tabCtrl;
		CRect m_rect;

	public:
		AddInTabCtrl(CWndTabCtrlManager & manager, CWndTabCtrl & tabCtrl, CRect rect)
			: m_manager(manager), m_tabCtrl(tabCtrl), m_rect(rect) {
		}

		template<typename WindowType>
		AddInTabCtrl Add(DWORD widgetId, DWORD textId) {
			m_manager.Add<WindowType>(m_tabCtrl, m_rect, widgetId, textId);
			return *this;
		}
	};

	AddInTabCtrl In(CWndTabCtrl & tabCtrl, CRect rect) {
		return AddInTabCtrl(*this, tabCtrl, rect);
	}

private:
	void Add(CWndTabCtrl & tabCtrl, CRect rect, CWndBase & tab, DWORD widgetId, DWORD textId);
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
	CWndTabCtrlManager m_tabManager;

	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
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
	CWndTabCtrlManager m_tabManager;

	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
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
	CWndTabCtrlManager m_tabManager;

	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
	virtual	void OnInitialUpdate();
};

