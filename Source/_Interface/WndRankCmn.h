#pragma once

#include "guild.h"
#include <boost/container/static_vector.hpp>

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
};

