#include "stdafx.h"
#include <ranges>
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuild.h"
#include "WndGuildTabMember.h"
#include "WndManager.h"
#include "DPClient.h"
#include "MsgHdr.h"
#include "defineQuest.h"
#include "playerdata.h"

#include "guild.h"

#include "eveschool.h"


/****************************************************
  WndId : APP_GUILD_TABMEMBER 
****************************************************/
#define		MAX_MEMBER_LIST_DRAW	14		// 한화면에 보이는 리스트 수.
#define		MEMBER_LIST_HEIGHT		18		// 리스트의 한라인 폭 y += MEMBER_LIST_HEIGHT 로 쓰라.

//////////////////////////////////////////////////////////////////////////
// Common Local Func.
//////////////////////////////////////////////////////////////////////////
bool prMemberLevelAsce(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;

	if(player1.nMemberLv > player2.nMemberLv)
		rtn_val = true;
	else if(player1.nMemberLv == player2.nMemberLv)
	{
		if(player1.nClass < player2.nClass)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prMemberLevelDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;

	if(player1.nMemberLv < player2.nMemberLv)
		rtn_val = true;
	else if(player1.nMemberLv == player2.nMemberLv)
	{
		if(player1.nClass > player2.nClass)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prJobAsce(MEMBERLIST player1, MEMBERLIST player2) {
	const auto nPlayer1JobType = prj.jobs.info[player1.nJob].dwJobType;
	const auto nPlayer2JobType = prj.jobs.info[player2.nJob].dwJobType;

	if (nPlayer1JobType < nPlayer2JobType) return false;
	if (nPlayer1JobType > nPlayer2JobType) return true;
	return player1.nJob > player2.nJob;
}

bool prJobDesc(MEMBERLIST player1, MEMBERLIST player2) {
	const auto nPlayer1JobType = prj.jobs.info[player1.nJob].dwJobType;
	const auto nPlayer2JobType = prj.jobs.info[player2.nJob].dwJobType;

	if (nPlayer1JobType < nPlayer2JobType) return true;
	if (nPlayer1JobType > nPlayer2JobType) return false;
	return player1.nJob < player2.nJob;
}

bool prLevelAsce(MEMBERLIST player1, MEMBERLIST player2) {
	const auto nPlayer1JobType = prj.jobs.info[player1.nJob].dwJobType;
	const auto nPlayer2JobType = prj.jobs.info[player2.nJob].dwJobType;

	if (nPlayer1JobType > nPlayer2JobType) return true;
	if (nPlayer1JobType < nPlayer2JobType) return false;
	if (player1.nLevel > player2.nLevel) return true;
	return false;
}

bool prLevelDesc(MEMBERLIST player1, MEMBERLIST player2) {
	const auto nPlayer1JobType = prj.jobs.info[player1.nJob].dwJobType;
	const auto nPlayer2JobType = prj.jobs.info[player2.nJob].dwJobType;

	if (nPlayer1JobType < nPlayer2JobType) return true;
	if (nPlayer1JobType > nPlayer2JobType) return false;
	if (player1.nLevel < player2.nLevel) return true;
	return false;
}

bool prNameAsce(MEMBERLIST player1, MEMBERLIST player2) {
	CString strplayer1Name = player1.szName;
	CString strplayer2Name = player2.szName;

	return strplayer1Name > strplayer2Name;
}

bool prNameDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	CString strplayer1Name = player1.szName;
	CString strplayer2Name = player2.szName;

	return strplayer1Name < strplayer2Name;
}

bool prAliasAsce(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.szAlias);
	strplayer2Name.Format("%s", player2.szAlias);

	if(strplayer1Name > strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

bool prAliasDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.szAlias);
	strplayer2Name.Format("%s", player2.szAlias);

	if(strplayer1Name < strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

CWndGuildTabMember::CWndGuildTabMember() 
{ 
	m_nCurrentList = 0;
	m_nSelect = 0;
	m_nMxOld = m_nMyOld = 0;
	m_bSortbyMemberLevel = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_bSortbyAlias = TRUE;
} 

int CWndGuildTabMember::GetMemberLevelIcon(const int nbClass) {
	switch (nbClass) {
		case 0: return 44;
		case 1: return 45;
		case 2: return 46;
		case 3: return 47;
		case 4: return 48;
		default: return -1;
	}
}

void CWndGuildTabMember::OnDraw( C2DRender* p2DRender ) 
{ 
	const int nPage = MAX_MEMBER_LIST_DRAW;
	const int nRange = static_cast<int>(m_list.size());
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	if(nRange - m_wndScrollBar.GetScrollPos() + 1 > m_wndScrollBar.GetScrollPage() )
		m_nCurrentList = m_wndScrollBar.GetScrollPos(); 
	if(nRange < m_wndScrollBar.GetScrollPos() )
		m_nCurrentList = 0;

	static constexpr int sx = 8;
	int sy = 32;	
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	// 영웅, 마스터 아이콘 추가됬으니 vertex 하나 늘임
	TEXTUREVERTEX2 pVertex[MAX_MEMBER_LIST_DRAW * 6 * 5];

	TEXTUREVERTEX2* pVertices = pVertex;

	const int until = std::min<int>(m_nCurrentList + MAX_MEMBER_LIST_DRAW, static_cast<int>(m_list.size()));

	for( int i = m_nCurrentList; i < until; i ++ ) {
		MEMBERLIST * pMember = &m_list[i];
		const int nData = pMember->nMemberLv;			

		const int nClass = pMember->nClass;

		const int iconId = GetMemberLevelIcon(pMember->nMemberLv);
		if (iconId != -1) {
			const int nposx = sx + 14 - nClass * 5;

			for (int j = 0; j <= nClass; ++j) {
				pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(nposx + j * 10, sy - 4), iconId, &pVertices, 0xffffffff);
			}
		}

		DWORD Color;

		if( pMember->bIsOnLine )
			Color = 0xffffffff;
		else
			Color = 0xffff6464;
		// Draw Job Icon
		const auto jobIcons = Project::Jobs::PlayerDataIcon(pMember->nJob, pMember->nLevel);
		if (jobIcons.master != 0) {
			pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(sx + 64, sy - 3), jobIcons.master, &pVertices, Color);
		}
		pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(sx + 84, sy - 3), jobIcons.job, &pVertices, Color);


		DWORD dwColor;
		if (i == m_nSelect) {
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		} else {
			if (pMember->bIsOnLine)
				dwColor = 0xff000000;
			else
				dwColor = 0xFF909090;
		}
		p2DRender->TextOut( sx + 126, sy, pMember->nLevel, dwColor );
		
		const CString strFormat = strings::CStringMaxSize(pMember->szName, 10);
		p2DRender->TextOut( sx + 168, sy, strFormat, dwColor );
		p2DRender->TextOut( sx + 264, sy, pMember->szAlias , dwColor );

		sy += MEMBER_LIST_HEIGHT;
	}

	pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	
} 
void CWndGuildTabMember::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_wndMenu.CreateMenu( this );	
	
//	CString strMember;
	CRect rect = GetWindowRect();
	rect.top += 40;
	rect.bottom	-= 10 ;
	rect.left -= 30;
	rect.right -= 5;
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);

	int nPage, nRange;
	nPage = MAX_MEMBER_LIST_DRAW;
	nRange = static_cast<int>(m_list.size());
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	// 버튼 이미지 세팅
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_MLEVEL);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabMLevel.bmp" ), 0xffff00ff );
		else
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabMLevel.bmp" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_JOB);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabJob.bmp" ), 0xffff00ff );
		else
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabJob.bmp" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_LEVEL);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabLevel.bmp" ), 0xffff00ff );
		else
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabLevel.bmp" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_NAME);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabChar.bmp" ), 0xffff00ff );
		else
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabChar.bmp" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_ALIAS);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabAlias.bmp" ), 0xffff00ff );
		else
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtGuildMemberTabAlias.bmp" ), 0xffff00ff );
	}

	UpdateData();

	SortbyMemberLevel();	// Default Sort

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildTabMember::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_GUILD_TABMEMBER_EX, pWndParent, 0, CPoint( 0, 0 ) );
} 
BOOL CWndGuildTabMember::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	if( nID == 1000 && dwMessage == 514 )	// Scroll Bar
	{
		return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
	}

	CMover* pMover	= g_pPlayer;
	if( !pMover )
		return FALSE;
	CGuild* pGuild	= pMover->GetGuild();
	if( !pGuild )
		return FALSE;
	if( m_nSelect < 0 || std::cmp_greater_equal(m_nSelect, m_list.size()))
		return FALSE;

	if( g_pPlayer->m_idWar != WarIdNone)
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDWARERRORMEMBER ) );
		return FALSE;
	}

	if( pWndBase == &m_wndMenu )
	{
		switch( nID )
		{
		case MGI_APPELL_UP:		// 호칭 업.
			g_DPlay.SendGuildMemberLv( pMover->m_idPlayer,
				m_list[m_nSelect].idPlayer, m_list[m_nSelect].nMemberLv - 1 );
			break;
		case MGI_APPELL_DOWN:
			g_DPlay.SendGuildMemberLv( pMover->m_idPlayer,
				m_list[m_nSelect].idPlayer, m_list[m_nSelect].nMemberLv + 1 );
			break;

		case MGI_CLASS_UP:		// 등급 업
			g_DPlay.SendGuildClass( pMover->m_idPlayer,
				m_list[m_nSelect].idPlayer, 1 );
			break;
		case MGI_CLASS_DOWN:	
			g_DPlay.SendGuildClass( pMover->m_idPlayer,
				m_list[m_nSelect].idPlayer, 0 );
			break;
		case MGI_CHG_MASTER:
			{
				if(m_list[m_nSelect].idPlayer != g_pPlayer->m_idPlayer )
				{
					if(m_list[m_nSelect].bIsOnLine )
						g_DPlay.SendChgMaster( pMover->m_idPlayer, m_list[m_nSelect].idPlayer );
					else
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_NOTLOGIN ), MB_OK, this );
				}
			}
			break;
		case MGI_NICKNAME:		// 별칭 부여
			{
				CGuild* pGuild = g_pPlayer->GetGuild();
				if( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
				{
					if( 10 > pGuild->m_nLevel )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_GUILDNOTLEVEL) ) );	
					}
					else
					{
						SAFE_DELETE( g_WndMng.m_pWndGuildNickName );
						g_WndMng.m_pWndGuildNickName = new CWndGuildNickName;
						g_WndMng.m_pWndGuildNickName->m_idPlayer = m_list[m_nSelect].idPlayer;
						g_WndMng.m_pWndGuildNickName->Initialize( &g_WndMng, APP_GUILD_NICKNAME );
					}
				}
				else
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDONLYMASTER ), MB_OK, this );
				}
				
			}
			break;

		case MGI_GUILD_LEAVE:	// 탈퇴
			{
				if(  pGuild->GetStateOfQuest( QUEST_WARMON_LV1 ) == QS_BEGIN )	// 클락워크 퀘스트면 길드 해체 안됨
				{
					QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( QUEST_WARMON_LV1 );
					if( pQuestProp )
					{
						g_WndMng.OpenMessageBox( prj.GetText( TID_GUILD_QUEST_LEAVEERROR ), MB_OK, this );
					}
					return FALSE;
				}
				else
				{
					if( g_GuildCombatMng.m_bRequest && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE )
					//if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE )
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDCOMBAT_NOT_LEAVE_GUILD ) );		//"수정해야함 : 길드대전에 신청한길드는 대전중에는 길드 탈퇴를 할수 없습니다" );
					else if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_CLOSE )
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_NOTLEAVEGUILD ) );
					else
					{
						g_DPlay.SendRemoveGuildMember( pMover->m_idPlayer, m_list[m_nSelect].idPlayer );
					}
				}
			}
			break;
		} // switch nID
	}
		
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildTabMember::OnSize( UINT nType, int cx, int cy ) \
{ 
	CRect rect = GetWindowRect();
	rect.top = rect.top + 40;
	rect.bottom	= rect.bottom - 10 ;
	rect.left = rect.right - 40;
	rect.right = rect.right - 25;
	m_wndScrollBar.SetWndRect( rect );
	
	const int nPage = MAX_MEMBER_LIST_DRAW;
	const int nRange = static_cast<int>(m_list.size());
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildTabMember::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	int mx = point.x - 8;
	int my = point.y - 32;
	m_nSelect = m_nCurrentList + my / MEMBER_LIST_HEIGHT;
	if( m_nSelect < 0 )
		m_nSelect = 0;
	if( std::cmp_greater_equal(m_nSelect, m_list.size()) )
		m_nSelect = static_cast<int>(m_list.size()) - 1;
} 

void CWndGuildTabMember::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndGuildTabMember::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_wndMenu.DeleteAllMenu();
	m_wndMenu.AddButton(MGI_APPELL_UP  , prj.GetText(TID_GAME_GUILD_APPELLATIONUP));
	m_wndMenu.AddButton(MGI_APPELL_DOWN, prj.GetText(TID_GAME_GUILD_APPELLATIONDOWN));
	m_wndMenu.AddButton(MGI_CLASS_UP   , prj.GetText(TID_GAME_GUILD_CLASSUP));
	m_wndMenu.AddButton(MGI_CLASS_DOWN , prj.GetText(TID_GAME_GUILD_CLASSDOWN));
	m_wndMenu.AddButton(MGI_NICKNAME   , prj.GetText(TID_GAME_GUILD_NICKNAME));
	m_wndMenu.AddButton(MGI_GUILD_LEAVE, prj.GetText(TID_GAME_GUILD_LEAVE));
	
	if (g_pPlayer) {
		CGuild * pGuild = g_pPlayer->GetGuild();
		if (pGuild && pGuild->IsMaster(g_pPlayer->m_idPlayer))
			m_wndMenu.AddButton(MGI_CHG_MASTER, prj.GetText(TID_GAME_CHG_MASTER));
	}

	m_wndMenu.Move( CPoint( m_rectCurrentWindow.left, m_rectCurrentWindow.top ) + point );
	m_wndMenu.SetVisible( TRUE );//!m_wndMenuMover.IsVisible() );
	m_wndMenu.SetFocus();
	
} 

BOOL CWndGuildTabMember::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	m_wndMenu.SetVisible( FALSE );

	switch(nID)
	{
		case WIDC_MLEVEL:
			SortbyMemberLevel();
			break;
		case WIDC_JOB:
			SortbyJob();
			break;
		case WIDC_LEVEL:
			SortbyLevel();
			break;
		case WIDC_NAME:
			SortbyName();
			break;
		case WIDC_ALIAS:
			SortbyAlias();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildTabMember::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.

		// TODO: a clamp function for the list / scrollbar
		m_nCurrentList += nDistY;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
		const int m_nMax = static_cast<int>(m_list.size());
		if( (m_nCurrentList + MAX_MEMBER_LIST_DRAW - 1) >= m_nMax )
		{
			m_nCurrentList = m_nMax - MAX_MEMBER_LIST_DRAW;
			if( m_nCurrentList < 0 )
				m_nCurrentList = 0;
		}
	
	}

	m_nMxOld = point.x;
	m_nMyOld = point.y;
}
void CWndGuildTabMember::OnMouseWndSurface( CPoint point )
{
	int mx = point.x - 8;
	int my = point.y - 32;
	int nSelect = m_nCurrentList + my / MEMBER_LIST_HEIGHT;

	if( nSelect < 0 )
		nSelect = 0;
	const int m_nMax = static_cast<int>(m_list.size());
	if( nSelect >= m_nMax )
		nSelect = m_nMax - 1;
	
	int nMax = (m_nMax > MAX_MEMBER_LIST_DRAW) ? MAX_MEMBER_LIST_DRAW : m_nMax;

	int offset_y = 30;

	for( int i=0; i<nMax; i++ )
	{
		CRect rect = CRect( 4,offset_y,m_rectLayout.right - m_rectLayout.left, (offset_y+15) );
		offset_y+=MEMBER_LIST_HEIGHT;

		if( PtInRect( &rect, point ) )
		{
			CRect rect2 = CRect( point.x, point.y, point.x+200, point.y+20 );
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rect2 );

			CString str;

			str.Format( "%s\n%s %u\n%s %d\n%s %d", m_list[ nSelect ].szName,
											   prj.GetText(TID_GAME_TOOLTIP_EXPMERIT ), m_list[ nSelect ].dwGivePxpCount,
				                               prj.GetText(TID_GAME_TOOLTIP_PENYAMERIT ), m_list[ nSelect ].nGiveGold,
											   prj.GetText(TID_GAME_TOOLTIP_GIVEUPNUM ), m_list[ nSelect ].nLose );

			g_toolTip.PutToolTip( m_nIdWnd, str, rect2, point2 );
			break;
		}
	}
}	

BOOL CWndGuildTabMember::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	
//    if( nFlags & MK_SHIFT )
//		nZoom /= 8;
	if( zDelta > 0 )
		m_nCurrentList -= 3;
	else
		m_nCurrentList += 3;

	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	const int m_nMax = static_cast<int>(m_list.size());
	if( (m_nCurrentList + MAX_MEMBER_LIST_DRAW - 1) >= m_nMax )
	{
		m_nCurrentList = m_nMax - MAX_MEMBER_LIST_DRAW;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}

	m_wndScrollBar.SetScrollPos( m_nCurrentList );
	return TRUE;
}


void CWndGuildTabMember::UpdateData()
{
	m_list.clear();

	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		m_list.reserve(pGuild->m_mapPMember.size());
		for(const CGuildMember * const pMember : pGuild->m_mapPMember | std::views::values) {
			MEMBERLIST & element = m_list.emplace_back();


			element.idPlayer       = pMember->m_idPlayer;
			element.nMemberLv      = pMember->m_nMemberLv;			// 호칭
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
			element.nJob = pPlayerData->data.nJob;
			element.nSex = pPlayerData->data.nSex;
			element.nLevel = pPlayerData->data.nLevel;	// 레벨
			element.bIsOnLine      = ( pPlayerData->data.uLogin > 0 );

			element.nGiveGold = pMember->m_nGiveGold;					// 길드에 기부한 페냐
			element.dwGivePxpCount = pMember->m_dwGivePxpCount;				// 길드에 기부한 PXP횟수( 스킬 경험치 )
			element.nWin = pMember->m_nWin;							// 무엇을 이겼지?
			element.nLose          = pMember->m_nLose;						// 무엇을 졌을까나?
		
			LPCSTR pszPlayer	= pPlayerData->szPlayer;
			lstrcpy(element.szName, pszPlayer );
			strcpy(element.szAlias, pMember->m_szAlias );
			element.nClass		= pMember->m_nClass;

		}
	}
	else
	{
		m_nCurrentList = 0;
		m_nSelect = 0;
		m_nMxOld = m_nMyOld = 0;
	}		
}


void CWndGuildTabMember::SortbyMemberLevel()
{
	if (!m_list.empty())
	{
		if(m_bSortbyMemberLevel)
		{
			std::ranges::sort(m_list, prMemberLevelAsce);
			m_bSortbyMemberLevel = FALSE;
		}
		else
		{
			std::ranges::sort(m_list, prMemberLevelDesc );
			m_bSortbyMemberLevel = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyJob()
{
	if(!m_list.empty())
	{
		if(m_bSortbyJob)
		{
			std::ranges::sort(m_list, prJobAsce );
			m_bSortbyJob = FALSE;
		}
		else
		{
			std::ranges::sort(m_list, prJobDesc );
			m_bSortbyJob = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyLevel()
{
	if (!m_list.empty())
	{
		if(m_bSortbyLevel)
		{
			std::ranges::sort(m_list, prLevelAsce );
			m_bSortbyLevel = FALSE;
		}
		else
		{
			std::ranges::sort(m_list, prLevelDesc );
			m_bSortbyLevel = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyName()
{
	if (!m_list.empty())
	{
		if(m_bSortbyName)
		{
			std::ranges::sort(m_list, prNameAsce );
			m_bSortbyName = FALSE;
		}
		else
		{
			std::ranges::sort(m_list, prNameDesc );
			m_bSortbyName = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyAlias()
{
	if (!m_list.empty())
	{
		if(m_bSortbyAlias)
		{
			std::ranges::sort(m_list, prAliasAsce );
			m_bSortbyAlias = FALSE;
		}
		else
		{
			std::ranges::sort(m_list, prAliasDesc );
			m_bSortbyAlias = TRUE;
		}
	}
}

