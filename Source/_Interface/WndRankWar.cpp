#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndRankCmn.h"

/****************************************************
  WndId : APP_RANK_WARTABGIVEUP - 항복패
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 항복패
****************************************************/

CWndRankWarTabGiveUp::CWndRankWarTabGiveUp()
	: CWndRankTab(CGuildRank::RANKING::R4, APP_RANK_WARTABGIVEUP) {
}

CWndRankTab::ValuesToPrint CWndRankWarTabGiveUp::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return { ranking.m_nSurrender };
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/****************************************************
  WndId : APP_RANK_WARTABLOSE - Applet
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 승
  CtrlId : WIDC_STATIC5 - /
  CtrlId : WIDC_STATIC6 - 패
  CtrlId : WIDC_STATIC1 - 순위
****************************************************/

CWndRankWarTabLose::CWndRankWarTabLose()
	: CWndRankTab(CGuildRank::RANKING::R3, APP_RANK_WARTABLOSE) {
}

CWndRankTab::ValuesToPrint CWndRankWarTabLose::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return { ranking.m_nWin, ranking.m_nLose };
}

/****************************************************
  WndId : APP_RANK_WARTABWIN - 길드전승리
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 승
  CtrlId : WIDC_STATIC5 - /
  CtrlId : WIDC_STATIC6 - 패
****************************************************/

CWndRankWarTabWin::CWndRankWarTabWin()
	: CWndRankTab(CGuildRank::RANKING::R2, APP_RANK_WARTABWIN) {
}

CWndRankTab::ValuesToPrint CWndRankWarTabWin::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return { ranking.m_nWin, ranking.m_nLose };
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************
  WndId : APP_RANK_WAR - 길드전승패 랭킹창
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

void CWndRankWar::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_WndRankWarTabWin.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_WARTABWIN );
	m_WndRankWarTabLose.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_WARTABLOSE );
	m_WndRankWarTabGiveUp.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_WARTABGIVEUP );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKWIN );
	tabTabItem.pWndBase = &m_WndRankWarTabWin;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKLOSE );
	tabTabItem.pWndBase = &m_WndRankWarTabLose;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKGIVEUP );
	tabTabItem.pWndBase = &m_WndRankWarTabGiveUp;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankWar::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_RANK_WAR, pWndParent, 0, CPoint( 0, 0 ) );
} 

