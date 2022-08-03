#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndRankCmn.h"

/****************************************************
  WndId : APP_RANK_GUILDTABBEST - 최강길드
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC3 - 길드명칭
  CtrlId : WIDC_STATIC4 - 길드장명칭
  CtrlId : WIDC_STATIC5 - 승
  CtrlId : WIDC_STATIC6 - /
  CtrlId : WIDC_STATIC7 - 패
****************************************************/

CWndRankTabBest::CWndRankTabBest()
	: CWndRankTab(CGuildRank::RANKING::R1, APP_RANK_GUILDTABBEST) {
}

CWndRankTab::ValuesToPrint CWndRankTabBest::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return { ranking.m_nWin, ranking.m_nLose };
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************
  WndId : APP_RANK_GUILDTABUNITED - 최대결속
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 승
  CtrlId : WIDC_STATIC5 - /
  CtrlId : WIDC_STATIC6 - 패
****************************************************/

CWndRankTabUnited::CWndRankTabUnited()
	: CWndRankTab(CGuildRank::RANKING::R5, APP_RANK_GUILDTABUNITED) {
}

CWndRankTab::ValuesToPrint CWndRankTabUnited::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return { ranking.m_nWin, ranking.m_nLose };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************
  WndId : APP_RANK_GUILDTABPENYA - 최대재력
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
****************************************************/

CWndRankTabPenya::CWndRankTabPenya()
	: CWndRankTab(CGuildRank::RANKING::R6, APP_RANK_GUILDTABPENYA) {
}

CWndRankTab::ValuesToPrint CWndRankTabPenya::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************
  WndId : APP_RANK_GUILD - 길드 랭킹창
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

void CWndRankGuild::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	
	m_WndRankTabBest.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_GUILDTABBEST );
	m_WndRankTabUnited.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_GUILDTABUNITED );
	m_WndRankTabPenya.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_GUILDTABPENYA );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKBEST );
	tabTabItem.pWndBase = &m_WndRankTabBest;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKUNITED );
	tabTabItem.pWndBase = &m_WndRankTabUnited;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKPENYA );
	tabTabItem.pWndBase = &m_WndRankTabPenya;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankGuild::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_RANK_GUILD, pWndParent, 0, CPoint( 0, 0 ) );
} 
