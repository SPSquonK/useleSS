#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndGuildCombat1to1.h"

#include "DPClient.h"

//////////////////////////////////////////////////////////////////////////
// 1:1 길드 컴뱃 참가자 구성
//////////////////////////////////////////////////////////////////////////


void CWndGuildCombat1to1Selection::SetSelection(std::span<const u_long> players) {
	PlayerLineup::DoubleListManager(
		GetDlgItem<CWndListGuild >(WIDC_LISTBOX1),
		GetDlgItem<CWndListSelect>(WIDC_LISTBOX2)
	).Reset(players);
}

void CWndGuildCombat1to1Selection::ResetSelection() {
	PlayerLineup::DoubleListManager(
		GetDlgItem<CWndListGuild >(WIDC_LISTBOX1),
		GetDlgItem<CWndListSelect>(WIDC_LISTBOX2)
	).Reset();
}

void CWndGuildCombat1to1Selection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 시간 지났는지를 판단
	if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_OPEN )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_CANNOT_MAKEUP) ); //지금은 명단작성을 할 수 없습니다.
		Destroy();
		return;
	}

	ReplaceListBox<PlayerLineup, PlayerLineup::SimpleDisplayer  >(WIDC_LISTBOX1);
	ReplaceListBox<PlayerLineup, PlayerLineup::NumberedDisplayer>(WIDC_LISTBOX2);
	
	MoveParentCenter();

	ResetSelection();
} 

BOOL CWndGuildCombat1to1Selection::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_1TO1_SELECTION, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombat1to1Selection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	const auto Manager = [&]() {
		return PlayerLineup::DoubleListManager(
			GetDlgItem<CWndListGuild >(WIDC_LISTBOX1),
			GetDlgItem<CWndListSelect>(WIDC_LISTBOX2)
		);
	};

	switch (nID) {
		case WIDC_BUTTON1:
		{
			const PlayerLineup::RuleSet ruleSet {
				.maxSelect = static_cast<size_t>(g_GuildCombat1to1Mng.m_nMaxJoinPlayer),
				.minimumLevel = static_cast<unsigned int>(g_GuildCombat1to1Mng.m_nMinJoinPlayerLevel)
			};

			const PlayerLineup::SelectReturn answer = Manager().ToSelect(ruleSet);

			switch (answer) {
				using enum PlayerLineup::SelectReturn;
				case NoSelection:
					break;
				case FullLineup: {
					CString str;
					str.Format(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_SELECTION_MAX), g_GuildCombat1to1Mng.m_nMaxJoinPlayer);
					g_WndMng.OpenMessageBox(str);
					break;
				}
				case NoGuild:
					break;
				case NotAMember:
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_NOT_GUILD_MEMBER));
					break;
				case TooLowLevel:
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_LIMIT_LEVEL_NOTICE));
					break;
				case AlreadyInLineup:
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_ALREADY_ENTRY)); //이미 등록되어 있습니다. 다시 등록해주세요.
					break;
				case Ok:
					break;
			}
			
			return TRUE;
		}
		case WIDC_BUTTON2: Manager().ToGuild();  return TRUE;
		case WIDC_BUTTON3: Manager().MoveUp();   return TRUE;
		case WIDC_BUTTON4: Manager().MoveDown(); return TRUE;
		case WIDC_FINISH : OnClickFinish();      return TRUE;
		case WIDC_RESET: {
			CGuildCombat1to1SelectionResetConfirm * pBox = new CGuildCombat1to1SelectionResetConfirm;
			g_WndMng.OpenCustomBox("", pBox);
			return TRUE;
		}
		case WIDC_CLOSE:
			Destroy();
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndGuildCombat1to1Selection::OnClickFinish() {
	// 시간 지났는지를 판단
	if (g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_OPEN) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT1TO1_CANNOT_MAKEUP));
		Destroy();
		return;
	}
	
	CWndListSelect * pWndSelect = GetDlgItem<CWndListSelect>(WIDC_LISTBOX2);

	if (pWndSelect->GetSize() < static_cast<size_t>(g_GuildCombat1to1Mng.m_nMinJoinPlayer)) {
		CString str;
		str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_MIN_PLAYER), g_GuildCombat1to1Mng.m_nMinJoinPlayer);
		g_WndMng.OpenMessageBox(str);
		return;
	}

	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return;

	std::vector<u_long> lineup;
	for (int i = 0; i != pWndSelect->GetSize(); ++i) {
		lineup.emplace_back((*pWndSelect)[i].playerId);
	}

	g_DPlay.SendGC1to1MemberLienUp(lineup);
	Destroy();
}

//////////////////////////////////////////////////////////////////////////
// 1:1 길드 컴뱃 신청하기
//////////////////////////////////////////////////////////////////////////

void CWndGuildCombat1to1Offer::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// 여기는 타이틀 바의 텍스트를 출력하는 곳 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, 4, m_strTitle, m_dwColor );
		p2DRender->SetFont(pOldFont);

		LPCTSTR szNameLevel;
		if (m_nCombatType == CombatType::GCGuild) {
			szNameLevel = prj.GetText(TID_GAME_GUILDCOMBAT_OFFER);
		} else if (m_nCombatType == CombatType::GC1to1) {
			szNameLevel = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFER);
		} else {
			szNameLevel = "???";
		}

		SetTitle( szNameLevel );
	}
}

void CWndGuildCombat1to1Offer::EnableAccept(BOOL bFlag) {
	if (CWndBase * pWndButton = GetDlgItem(WIDC_ACCEPT)) {
		pWndButton->EnableWindow(bFlag);
	}
}

void CWndGuildCombat1to1Offer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndText::SetupDescription(
		GetDlgItem<CWndText>(WIDC_TEXT1),
		_T("GuildCombatRedChipGuild.inc")
	);
	
	MoveParentCenter();
} 

BOOL CWndGuildCombat1to1Offer::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_GUILDCOMBAT_1TO1_OFFER, pWndParent, 0, CPoint(0, 0));
}

void CWndGuildCombat1to1Offer::SetGold(DWORD nCost) {
	if (CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1)) {
		CString str;
		str.Format("%d", nCost);
		pWndEdit->SetString(str);
	}
}

BOOL CWndGuildCombat1to1Offer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ACCEPT )
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			
		LPCTSTR str2 = pWndEdit->GetString();
		DWORD nCost = atoi( str2 );

		if( m_dwReqGold != 0 )
		{
			if( nCost <= m_dwBackupGold )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_CURRENT_REQUEST) ); //기존 페냐보다 더 많은 금액으로 신청을 하시기 바랍니다.
				return FALSE;
			}
		}
		else
		{
			if( nCost < m_dwMinGold )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_LIMIT_MIN) ); //최소금액보다 더 많은 페냐로 신청하시기 바랍니다.
				return FALSE;
			}
		}

		CWndGuildCombat1to1OfferMessageBox* pMsg = new CWndGuildCombat1to1OfferMessageBox;

		g_WndMng.OpenCustomBox( "", pMsg );
		CString str;

		if( m_dwReqGold == 0 )
		{
			str.Format( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_REQUEST), 0, nCost ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
		}
		else
		{
			str.Format( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_REQUEST), m_dwBackupGold, nCost-m_dwBackupGold ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
		}

		pMsg->SetValue( str, nCost );
			
		
	}
	else if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// 1:1 길드 대전 참가자 구성 확인 창
//////////////////////////////////////////////////////////////////////////

BOOL CGuildCombat1to1SelectionResetConfirm::Initialize( CWndBase* pWndParent )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_REMAKE_MAKEUP), //명단작성을 다시 하시겠습니까?
		pWndParent, 
		MB_OKCANCEL );
}

BOOL CGuildCombat1to1SelectionResetConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			{
				CWndGuildCombat1to1Selection *pGuildCombat1to1Selection = (CWndGuildCombat1to1Selection*)g_WndMng.GetWndBase( APP_GUILDCOMBAT_1TO1_SELECTION );

				if( pGuildCombat1to1Selection )
					pGuildCombat1to1Selection->ResetSelection();

				Destroy();
			}
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// 1:1 길드 대전 입찰 확인 창
//////////////////////////////////////////////////////////////////////////

BOOL CWndGuildCombat1to1OfferMessageBox::Initialize( CWndBase* pWndParent )
{
	return CWndMessageBox::Initialize( "", 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndGuildCombat1to1OfferMessageBox::SetValue( CString str, DWORD nCost )
{
	m_wndText.SetString( str );
	m_nCost = nCost;	
}

BOOL CWndGuildCombat1to1OfferMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			{
				CWndGuildCombat1to1Offer* pWndGuildCombat1to1Offer = (CWndGuildCombat1to1Offer*)g_WndMng.GetWndBase(APP_GUILDCOMBAT_1TO1_OFFER);
				if(pWndGuildCombat1to1Offer)
				{
					using CombatType = CWndGuildCombat1to1Offer::CombatType;
					if(pWndGuildCombat1to1Offer->m_nCombatType == CombatType::GCGuild)
					{
						g_DPlay.SendGuildCombatApp( m_nCost );
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatOffer);
					}
					else if(pWndGuildCombat1to1Offer->m_nCombatType == CombatType::GC1to1)
					{
						g_DPlay.SendGC1to1Tender( m_nCost );
						SAFE_DELETE(g_WndMng.m_pWndGuildCombat1to1Offer);
					}
				}
				Destroy();
			}
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

