#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabInfo.h"
#include "WndGuildTabApp.h"
#include "WndGuildTabWar.h"
#include "WndGuild.h"
#include "DPClient.h"
#include "MsgHdr.h"

#include "guild.h"

/****************************************************
  WndId : APP_GUILD
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/

void CWndGuild::UpdateDataAll() {
	m_WndGuildTabInfo.UpdateData();
	m_WndGuildTabApp.UpdateData();
	m_WndGuildTabMember.UpdateData();
	m_WndGuildTabWar.UpdateData();
	m_WndGuildTabPower.UpdateData();
}

void CWndGuild::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndTabCtrl * pWndTabCtrl = GetDlgItem<CWndTabCtrl>(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_WndGuildTabInfo.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABINFO);
	m_WndGuildTabApp.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABAPPELLATION);
	m_WndGuildTabMember.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABMEMBER_EX);
	m_WndGuildTabWar.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABGUILDWAR);
	m_WndGuildTabPower.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TAPPOWER);

	WTCITEM tabTabItem;

	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDINFO);
	tabTabItem.pWndBase = &m_WndGuildTabInfo;
	pWndTabCtrl->InsertItem(0, &tabTabItem);


	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDMEMBER);
	tabTabItem.pWndBase = &m_WndGuildTabMember;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_APPELLATION);
	tabTabItem.pWndBase = &m_WndGuildTabApp;
	pWndTabCtrl->InsertItem(2, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDWAR);
	tabTabItem.pWndBase = &m_WndGuildTabWar;
	pWndTabCtrl->InsertItem(3, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_AGIT);
	tabTabItem.pWndBase = &m_WndGuildTabPower;
	pWndTabCtrl->InsertItem(4, &tabTabItem);


#ifdef __S_SERVER_UNIFY
	if (!g_WndMng.m_bAllAction) {
		Destroy();
		return;
	}
#endif // __S_SERVER_UNIFY

	MoveParentCenter();

	m_WndGuildTabMember.m_rectParent = GetWindowRect(TRUE);
}

BOOL CWndGuild::Initialize(CWndBase * pWndParent, DWORD) {
	if (g_eLocal.GetState(EVE_SCHOOL))
		return FALSE;

	return CWndNeuz::InitDialog(APP_GUILD, pWndParent, 0, CPoint(0, 0));
}

void CWndGuild::SetCurTab(int index) {
	CWndTabCtrl * pWndTabCtrl = GetDlgItem<CWndTabCtrl>(WIDC_TABCTRL1);
	assert(pWndTabCtrl);

	pWndTabCtrl->SetCurSel(index);
}


//---------------------------------------------------------------------------------------------------------------------------
// Guild invitation confirm window
//---------------------------------------------------------------------------------------------------------------------------

void CWndGuildConfirm::OpenWindow(const u_long idMaster, const CGuild & guild) {
	CWndGuildConfirm *& window = g_WndMng.m_pWndGuildConfirm;
	
	SAFE_DELETE(window);
	window = new CWndGuildConfirm(idMaster);
	window->Initialize();
	window->SetGuildName(guild.m_szGuild);
}

void CWndGuildConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	Move70();
}

BOOL CWndGuildConfirm::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_GUILD_INVATE, pWndParent, 0, CPoint(0, 0));
}

void CWndGuildConfirm::SetGuildName(const char * pGuildName) {
	GetDlgItem(WIDC_STATIC3)->SetTitle(pGuildName);
}

BOOL CWndGuildConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_OK || message == EN_RETURN) {
		g_DPlay.SendPacket<PACKETTYPE_ADD_GUILD_MEMBER, u_long>(m_idMaster);
		Destroy();
	} else if (nID == WIDC_NO) {
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
