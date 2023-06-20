#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndGuildCombat1to1.h"

#include "DPClient.h"
#include "playerdata.h"

//////////////////////////////////////////////////////////////////////////

void PlayerLineup::SimpleDisplayer::Render(
	C2DRender * const p2DRender, CRect rect,
	PlayerLineup & displayed,
	DWORD color, const WndTListBox::DisplayArgs & misc
) const {
	if (displayed.cachedText.IsEmpty()) {
		PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(displayed.playerId);
		displayed.cachedText.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.jobs.info[pPlayerData->data.nJob].szName);
	}

	p2DRender->TextOut(rect.left, rect.top, displayed.cachedText.GetString(), color);
}

void PlayerLineup::NumberedDisplayer::Render(
	C2DRender * const p2DRender, CRect rect,
	PlayerLineup & displayed,
	DWORD color, const WndTListBox::DisplayArgs & misc
) const {
	if (displayed.cachedText.IsEmpty()) {
		PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(displayed.playerId);
		displayed.cachedText.Format("No.%d  Lv%.2d	%.16s %.10s",
			misc.i + 1,
			pPlayerData->data.nLevel, pPlayerData->szPlayer,
			prj.jobs.info[pPlayerData->data.nJob].szName
		);
	}

	p2DRender->TextOut(rect.left, rect.top, displayed.cachedText.GetString(), color);
}

void PlayerLineup::LineupManager::Reset(std::span<const u_long> lineup) {
	pWndPool->ResetContent();
	pWndLineup->ResetContent();

	const CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return;

	for (const CGuildMember * pMember : pGuild->m_mapPMember | std::views::values) {
		PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
		if (pPlayerData->data.uLogin == 0) continue;

		if (!std::ranges::contains(lineup, pMember->m_idPlayer)) {
			pWndPool->Add(PlayerLineup{ pMember->m_idPlayer });
		}
	}

	for (const u_long playerId : lineup) {
		pWndLineup->Add(PlayerLineup{ playerId });
	}
}

PlayerLineup::SelectReturn PlayerLineup::LineupManager::ToSelect(const LineupRuleSet & ruleSet) {
	const auto [idSel, selPlayer] = pWndPool->GetSelection();
	if (idSel == -1) return SelectReturn::NoSelection;

	if (pWndLineup->GetSize() >= ruleSet.maxSelect.value_or(0)) {
		return SelectReturn::FullLineup;
	}

	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return SelectReturn::NoGuild;

	CGuildMember * pGuildMember = pGuild->GetMember(selPlayer->playerId);
	if (!pGuildMember) {
		return SelectReturn::NotAMember;
	}

	const PlayerData * pd = CPlayerDataCenter::GetInstance()->GetPlayerData(pGuildMember->m_idPlayer);
	if (pd->data.nLevel < ruleSet.minimumLevel.value_or(0)) {
		return SelectReturn::TooLowLevel;
	}

	const u_long toAdd = selPlayer->playerId;
	pWndPool->Erase(idSel);

	int inSelection = pWndLineup->Find([toAdd](const PlayerLineup & p) { return p.playerId == toAdd; });

	if (inSelection != -1) {
		return SelectReturn::AlreadyInLineup;
	}

	pWndLineup->Add(PlayerLineup{ toAdd });
	return SelectReturn::Ok;
}

void PlayerLineup::LineupManager::ToGuild() {
	const auto [idSel, selPlayer] = pWndLineup->GetSelection();
	if (idSel == -1) return;

	const u_long playerId = selPlayer->playerId;

	pWndLineup->Erase(idSel);

	int inGuild = pWndPool->Find([playerId](const PlayerLineup & p) { return p.playerId == playerId; });
	if (inGuild == -1) {
		pWndPool->Add(PlayerLineup{ playerId });
	}
}

void PlayerLineup::LineupManager::MoveUp() {
	const int sel = pWndLineup->GetCurSel();
	if (sel == -1 || sel == 0) return;

	const auto & [a, b] = pWndLineup->Swap(sel - 1, sel);
	a->cachedText = "";
	b->cachedText = "";
}

void PlayerLineup::LineupManager::MoveDown() {
	const int sel = pWndLineup->GetCurSel();
	if (sel == -1 || sel + 1 == static_cast<int>(pWndLineup->GetSize())) return;

	const auto & [a, b] = pWndLineup->Swap(sel, sel + 1);
	a->cachedText = "";
	b->cachedText = "";
}

//////////////////////////////////////////////////////////////////////////
// 1:1 길드 컴뱃 참가자 구성
//////////////////////////////////////////////////////////////////////////


void CWndGuildCombat1to1Selection::SetSelection(std::span<const u_long> players) {
	PlayerLineup::LineupManager(
		GetDlgItem<CWndListGuild >(WIDC_LISTBOX1),
		GetDlgItem<CWndListSelect>(WIDC_LISTBOX2)
	).Reset(players);
}

void CWndGuildCombat1to1Selection::ResetSelection() {
	PlayerLineup::LineupManager(
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

BOOL CWndGuildCombat1to1Selection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_1TO1_SELECTION, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombat1to1Selection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	const auto Manager = [&]() {
		return PlayerLineup::LineupManager(
			GetDlgItem<CWndListGuild >(WIDC_LISTBOX1),
			GetDlgItem<CWndListSelect>(WIDC_LISTBOX2)
		);
	};

	switch (nID) {
		case WIDC_BUTTON1:
		{
			const PlayerLineup::LineupRuleSet ruleSet {
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

CWndGuildCombat1to1Offer::CWndGuildCombat1to1Offer(int nCombatType) 
{
	m_dwMinGold    = 0;
	m_dwBackupGold = 0;
	m_dwReqGold    = 0;
	m_nCombatType = nCombatType;
}

CWndGuildCombat1to1Offer::~CWndGuildCombat1to1Offer() 
{
}

void CWndGuildCombat1to1Offer::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// 여기는 타이틀 바의 텍스트를 출력하는 곳 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFER) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFER) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}
		
void CWndGuildCombat1to1Offer::OnDraw( C2DRender* p2DRender ) 
{	
}

void CWndGuildCombat1to1Offer::EnableAccept( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_ACCEPT );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );	
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

BOOL CWndGuildCombat1to1Offer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_1TO1_OFFER, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombat1to1Offer::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombat1to1Offer::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombat1to1Offer::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombat1to1Offer::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombat1to1Offer::SetGold( DWORD nCost )
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	if( pWndEdit )
	{
		CString str;
		str.Format( "%d", nCost );
		pWndEdit->SetString(str);
	}				
}
/*
void CWndGuildCombat1to1Offer::SetTotalGold( __int64 nCost )
{
	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	pStatic->AddWndStyle(WSS_MONEY);
	
	if( pStatic )
	{
		CString str;
		str.Format( "%I64d", nCost );
		pStatic->SetTitle(str);
	}
}
*/
BOOL CWndGuildCombat1to1Offer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ACCEPT )
	{
		if( g_pPlayer )
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			
			DWORD nCost;
			CString str = pWndEdit->GetString();
			nCost = atoi( str );

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
			if( pMsg )
			{
				g_WndMng.OpenCustomBox( "", pMsg, this );
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
		}
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

BOOL CGuildCombat1to1SelectionResetConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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

BOOL CWndGuildCombat1to1OfferMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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
					if(pWndGuildCombat1to1Offer->m_nCombatType == 0)
					{
						g_DPlay.SendGuildCombatApp( m_nCost );
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatOffer);
					}
					else if(pWndGuildCombat1to1Offer->m_nCombatType == 1)
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

