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

void CWndRankInfo::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndTabCtrl * pWndTabCtrl = GetDlgItem<CWndTabCtrl>(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_tabManager.In(*pWndTabCtrl, rect)
		.Add<CWndRankInfoTabLevel>(APP_RANK_INFOTABLEVEL, TID_GAME_TOOLTIP_RANKLEVEL)
		.Add<CWndRankInfoTabPlayTime>(APP_RANK_INFOTABPLAYTIME, TID_GAME_TOOLTIP_RANKPLAYTIME);

	MoveParentCenter();
}

BOOL CWndRankInfo::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_RANK_INFO, pWndParent, 0, CPoint(0, 0));
}

