
#include "StdAfx.h"

//Author : gmpbigsun
//Date : 2009_11_16

#include "WndGuildTabPower.h"
#include "AppDefine.h"
#include "DPClient.h"

void CWndGuildTabPower::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	UpdateData();

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	//최초엔 버튼 disable 
	CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndBtnOK->EnableWindow(m_bChanedCheckBox);
} 

BOOL CWndGuildTabPower::Initialize( CWndBase* pWndParent, DWORD ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILD_TAPPOWER, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndGuildTabPower::UpdateData() {
	CGuild * pGuild = g_pPlayer->GetGuild();
	if (pGuild) {
		SetData(pGuild->m_aPower);

		EnableButton(pGuild->IsMaster(g_pPlayer->m_idPlayer) ? TRUE : FALSE);
	} else {
		SetData(GuildPowerss());

		EnableButton(FALSE);
	}
}

void CWndGuildTabPower::EnableButton(const BOOL bEnable) {
	ForEachPower([bEnable](CWndButton & button, int, GuildPower) {
		button.EnableWindow(bEnable);
		});

	CWndButton * pWndCheck = NULL;
	// 버튼 부분
	pWndCheck = (CWndButton *)GetDlgItem(WIDC_BUTTON1);
	pWndCheck->EnableWindow(bEnable);

	//취소버튼 일단 잠금 
	pWndCheck = (CWndButton *)GetDlgItem(WIDC_BUTTON11);
	pWndCheck->EnableWindow(FALSE);
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
		CWndButton * button = GetDlgItem<CWndButton>(widgetId);
		func(*button, gud, power);
		});
}

void CWndGuildTabPower::SetData(const GuildPowerss & dwPower) {
	m_aPowers = dwPower;

	ForEachPower([&](CWndButton & button, const int gud, const GuildPower power) {
		button.SetCheck((m_aPowers[gud][power]) ? TRUE : FALSE);
		});

	m_hasBeenChanged = false;
}

BOOL CWndGuildTabPower::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	if( !pGuild )
		return FALSE;
	
	CGuildMember* pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
	
	if( !pGuildMember )
		return FALSE;
	
	if(	pGuildMember->m_nMemberLv != GUD_MASTER )
		return FALSE;

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

	if (WIDC_BUTTON1 == nID && m_hasBeenChanged)	// 보내기
	{
		g_DPlay.SendGuildAuthority(m_aPowers);
		m_hasBeenChanged = false;
	}

	CWndButton* pWndBtnOK = GetDlgItem<CWndButton>(WIDC_BUTTON1);
	if (pWndBtnOK) {
		pWndBtnOK->EnableWindow(m_hasBeenChanged ? TRUE : FALSE);
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

