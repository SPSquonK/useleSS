#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndRankCmn.h"

/****************************************************
  WndId : APP_RANK_INFOTABLEVEL - 최대레벨합
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 평균레벨
****************************************************/

CWndRankInfoTabLevel::CWndRankInfoTabLevel()
	: CWndRankTab(CGuildRank::RANKING::R7, APP_RANK_INFOTABLEVEL) {
}

CWndRankTab::ValuesToPrint CWndRankInfoTabLevel::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return { static_cast<int>(ranking.m_AvgLevel) };
}

/****************************************************
  WndId : APP_RANK_INFOTABPLAYTIME - 누적플레이최대
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
****************************************************/

CWndRankInfoTabPlayTime::CWndRankInfoTabPlayTime()
	: CWndRankTab(CGuildRank::RANKING::R8, APP_RANK_INFOTABPLAYTIME) {
}

CWndRankTab::ValuesToPrint CWndRankInfoTabPlayTime::GetValuesToPrint(const CGuildRank::GUILD_RANKING & ranking) {
	return {};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/****************************************************
  WndId : APP_RANK_INFO - 길드기타 랭킹창
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

void CWndRankInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	
	m_WndRankInfoTabLevel.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_INFOTABLEVEL );
	m_WndRankInfoTabPlayTime.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_INFOTABPLAYTIME );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKLEVEL );
	tabTabItem.pWndBase = &m_WndRankInfoTabLevel;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKPLAYTIME );
	tabTabItem.pWndBase = &m_WndRankInfoTabPlayTime;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_RANK_INFO, pWndParent, 0, CPoint( 0, 0 ) );
} 

