#include "StdAfx.h"
#include "WndGuildTabPower.h"
#include "AppDefine.h"
#include "DPClient.h"

void CWndGuildTabPower::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	UpdateData();
	MoveParentCenter();
}

BOOL CWndGuildTabPower::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return FALSE;

	CGuildMember * pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
	if (!pGuildMember) return FALSE;
	if (pGuildMember->m_nMemberLv != GUD_MASTER) return FALSE;

	ForEachPower([&](const UINT buttonID, const int gud, const GuildPower power) {
		if (nID == buttonID) {
			CWndButton * pWndCheck = GetDlgItem<CWndButton>(buttonID);
			if (pWndCheck->GetCheck()) {
				m_aPowers[gud].Set(power);
			} else {
				m_aPowers[gud].Unset(power);
			}
			m_hasBeenChanged = true;
		}
		});

	if (WIDC_BUTTON1 == nID && m_hasBeenChanged) {
		g_DPlay.SendGuildAuthority(m_aPowers);
		m_hasBeenChanged = false;
	} else if (WIDC_BUTTON11 == nID) {
		UpdateData();
	}

	GetDlgItem<CWndButton>(WIDC_BUTTON1)->EnableWindow(m_hasBeenChanged ? TRUE : FALSE);	GetDlgItem<CWndButton>(WIDC_BUTTON1)->EnableWindow(m_hasBeenChanged ? TRUE : FALSE);
	GetDlgItem<CWndButton>(WIDC_BUTTON11)->EnableWindow(m_hasBeenChanged ? TRUE : FALSE);	GetDlgItem<CWndButton>(WIDC_BUTTON1)->EnableWindow(m_hasBeenChanged ? TRUE : FALSE);

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndGuildTabPower::UpdateData() {
	if (CGuild * pGuild = g_pPlayer->GetGuild()) {
		SetData(pGuild->m_aPower);
		EnableButton(pGuild->IsMaster(g_pPlayer->m_idPlayer) ? TRUE : FALSE);
	} else {
		SetData(GuildPowerss());
		EnableButton(FALSE);
	}
}

void CWndGuildTabPower::SetData(const GuildPowerss & dwPower) {
	m_aPowers = dwPower;

	ForEachPower([&](CWndButton & button, const int gud, const GuildPower power) {
		button.SetCheck((m_aPowers[gud][power]) ? TRUE : FALSE);
		});

	m_hasBeenChanged = false;
}

void CWndGuildTabPower::EnableButton(const BOOL bEnable) {
	ForEachPower([bEnable](CWndButton & button, int, GuildPower) {
		button.EnableWindow(bEnable);
		});

	GetDlgItem(WIDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(WIDC_BUTTON11)->EnableWindow(FALSE);
}

void CWndGuildTabPower::ForEachPower(
	std::invocable<UINT, int, GuildPower> auto func
) {
	func(WIDC_CHECK1 , GUD_KINGPIN  , GuildPower::GuildHouseFurniture);
	func(WIDC_CHECK9 , GUD_KINGPIN  , GuildPower::GuildHouseUpKeep   );
	func(WIDC_CHECK2 , GUD_CAPTAIN  , GuildPower::GuildHouseFurniture);
	func(WIDC_CHECK10, GUD_CAPTAIN  , GuildPower::GuildHouseUpKeep   );
	func(WIDC_CHECK3 , GUD_SUPPORTER, GuildPower::GuildHouseFurniture);
	func(WIDC_CHECK11, GUD_SUPPORTER, GuildPower::GuildHouseUpKeep   );
	func(WIDC_CHECK4 , GUD_ROOKIE   , GuildPower::GuildHouseFurniture);
	func(WIDC_CHECK12, GUD_ROOKIE   , GuildPower::GuildHouseUpKeep   );
}

void CWndGuildTabPower::ForEachPower(
	std::invocable<CWndButton &, int, GuildPower> auto func
) {
	ForEachPower([&](const UINT widgetId, const int gud, const GuildPower power) {
		func(*GetDlgItem<CWndButton>(widgetId), gud, power);
		});
}
