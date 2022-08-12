
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

void CWndGuildTabPower::UpdateData()
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		SetData( pGuild->m_adwPower );

		if( pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
			EnableButton( TRUE );
		else EnableButton( FALSE );
	}
	else
	{
		DWORD adwPower [MAX_GM_LEVEL] = { 0 };
		SetData( adwPower );

		EnableButton( FALSE );
	}
}

void CWndGuildTabPower::EnableButton(const BOOL bEnable) {
	ForEachPower([bEnable](CWndButton & button, int, DWORD) {
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
	std::invocable<UINT, int, DWORD> auto func
) {
	func(WIDC_CHECK1 , GUD_KINGPIN  , PF_GUILDHOUSE_FURNITURE);
	func(WIDC_CHECK9 , GUD_KINGPIN  , PF_GUILDHOUSE_UPKEEP   );
	func(WIDC_CHECK2 , GUD_CAPTAIN  , PF_GUILDHOUSE_FURNITURE);
	func(WIDC_CHECK10, GUD_CAPTAIN  , PF_GUILDHOUSE_UPKEEP   );
	func(WIDC_CHECK3 , GUD_SUPPORTER, PF_GUILDHOUSE_FURNITURE);
	func(WIDC_CHECK11, GUD_SUPPORTER, PF_GUILDHOUSE_UPKEEP   );
	func(WIDC_CHECK4 , GUD_ROOKIE   , PF_GUILDHOUSE_FURNITURE);
	func(WIDC_CHECK12, GUD_ROOKIE   , PF_GUILDHOUSE_UPKEEP   );
}

void CWndGuildTabPower::ForEachPower(
	std::invocable<CWndButton &, int, DWORD> auto func
) {
	ForEachPower([&](const UINT widgetId, const int gud, const DWORD power) {
		CWndButton * button = GetDlgItem<CWndButton>(widgetId);
		func(*button, gud, power);
		});
}

void CWndGuildTabPower::SetData(DWORD dwPower[]) {
	memcpy(m_adwPower, dwPower, sizeof(DWORD) * MAX_GM_LEVEL);

	ForEachPower([&](CWndButton & button, const int gud, const DWORD power) {
		button.SetCheck((m_adwPower[gud] & power) ? TRUE : FALSE);
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

	ForEachPower([&](const UINT buttonID, const int gud, const DWORD power) {
		if (nID == buttonID) {
			CWndButton * pWndCheck = GetDlgItem<CWndButton>(buttonID);
			if (pWndCheck->GetCheck()) {
				m_adwPower[gud] |= power;
			} else {
				m_adwPower[gud] &= ~power;
			}
			m_hasBeenChanged = true;
		}
		});

	 if( WIDC_BUTTON1 == nID && m_hasBeenChanged)	// 보내기
	{
		g_DPlay.SendGuildAuthority( pGuild->GetGuildId(), m_adwPower );
		m_hasBeenChanged = false;
	}

	CWndButton* pWndBtnOK = GetDlgItem<CWndButton>(WIDC_BUTTON1);
	if (pWndBtnOK) {
		pWndBtnOK->EnableWindow(m_hasBeenChanged ? TRUE : FALSE);
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

