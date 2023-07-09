#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabApp.h"
#include "WndManager.h"

#include "DPClient.h"
#include "guild.h"




/****************************************************
  WndId : APP_GUILD_TABAPP 
****************************************************/
CWndGuildTabApp::CWndGuildTabApp() 
{ 
	for( int i=0; i<MAX_GM_LEVEL; i++ )
		m_aPowers[i].SetAll();

	m_pWndGuildPayConfirm = NULL;
}

void CWndGuildTabApp::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		
	TEXTUREVERTEX2 pVertex[ 6 * MAX_GM_LEVEL ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	LPWNDCTRL pCustom = NULL;
	
	pCustom = GetWndCtrl( WIDC_CUSTOM1 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 0 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM2 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 1 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM3 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 2 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM4 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 3 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM5 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 4 + ( 6 * 1 ), &pVertices, 0xffffffff );
	
	pWndWorld->m_texMsgIcon.Render( pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );

} 

void CWndGuildTabApp::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	UpdateData();
	MoveParentCenter();
}

void CWndGuildTabApp::ForEachPower(
	std::invocable<UINT, int, GuildPower> auto func
) {
	func(WIDC_CHECK1 , GUD_MASTER   , GuildPower::MemberLevel);
	func(WIDC_CHECK6 , GUD_MASTER   , GuildPower::Level      );
	func(WIDC_CHECK11, GUD_MASTER   , GuildPower::Invitation );
	func(WIDC_CHECK16, GUD_MASTER   , GuildPower::Penya      );
	func(WIDC_CHECK21, GUD_MASTER   , GuildPower::Item       );
	func(WIDC_CHECK2 , GUD_KINGPIN  , GuildPower::MemberLevel);
	func(WIDC_CHECK7 , GUD_KINGPIN  , GuildPower::Level      );
	func(WIDC_CHECK12, GUD_KINGPIN  , GuildPower::Invitation );
	func(WIDC_CHECK17, GUD_KINGPIN  , GuildPower::Penya      );
	func(WIDC_CHECK22, GUD_KINGPIN  , GuildPower::Item       );
	func(WIDC_CHECK3 , GUD_CAPTAIN  , GuildPower::MemberLevel);
	func(WIDC_CHECK8 , GUD_CAPTAIN  , GuildPower::Level      );
	func(WIDC_CHECK13, GUD_CAPTAIN  , GuildPower::Invitation );
	func(WIDC_CHECK18, GUD_CAPTAIN  , GuildPower::Penya      );
	func(WIDC_CHECK23, GUD_CAPTAIN  , GuildPower::Item       );
	func(WIDC_CHECK4 , GUD_SUPPORTER, GuildPower::MemberLevel);
	func(WIDC_CHECK9 , GUD_SUPPORTER, GuildPower::Level      );
	func(WIDC_CHECK14, GUD_SUPPORTER, GuildPower::Invitation );
	func(WIDC_CHECK19, GUD_SUPPORTER, GuildPower::Penya      );
	func(WIDC_CHECK24, GUD_SUPPORTER, GuildPower::Item       );
	func(WIDC_CHECK5 , GUD_ROOKIE   , GuildPower::MemberLevel);
	func(WIDC_CHECK10, GUD_ROOKIE   , GuildPower::Level      );
	func(WIDC_CHECK15, GUD_ROOKIE   , GuildPower::Invitation );
	func(WIDC_CHECK20, GUD_ROOKIE   , GuildPower::Penya      );
	func(WIDC_CHECK25, GUD_ROOKIE   , GuildPower::Item       );
}

void CWndGuildTabApp::ForEachPower(
	std::invocable<CWndButton &, int, GuildPower> auto func
) {
	ForEachPower([&](const UINT widgetId, const int gud, const GuildPower power) {
		CWndButton * button = GetDlgItem<CWndButton>(widgetId);
		func(*button, gud, power);
		});
}

BOOL CWndGuildTabApp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	if( pGuild == NULL )
		return FALSE;
	
	CGuildMember* pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
	
	if( pGuildMember == NULL )
		return FALSE;
	
	if(	pGuildMember->m_nMemberLv != GUD_MASTER )
		return FALSE;
	

	ForEachPower([&](const UINT buttonID, const int gud, const GuildPower power) {
		if (gud == GUD_MASTER) return;

		if (nID == buttonID) {
			CWndButton * pWndCheck = GetDlgItem<CWndButton>(buttonID);
			if (pWndCheck->GetCheck()) {
				m_aPowers[gud].Set(power);
			} else {
				m_aPowers[gud].Unset(power);
			}
		}
		});

	// 설절버튼 눌렀을때 실행 클라에서 먼저 가지고 있는권한이 다르면 서버로 전송
	if( nID == WIDC_BUTTON1 )	// 마스트
	{
		m_pWndGuildPayConfirm = std::make_unique<CWndGuildPayConfirm>();
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_MASTER;
	}
	else if( nID == WIDC_BUTTON2 )  // 킹 핀
	{
		m_pWndGuildPayConfirm = std::make_unique<CWndGuildPayConfirm>();
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_KINGPIN;
	}
	else if( nID == WIDC_BUTTON3 )  // 캡 틴
	{
		m_pWndGuildPayConfirm = std::make_unique<CWndGuildPayConfirm>();
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_CAPTAIN;
	}
	else if( nID == WIDC_BUTTON4 )  // 서포터
	{
		m_pWndGuildPayConfirm = std::make_unique<CWndGuildPayConfirm>();
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_SUPPORTER;
	}
	else if( nID == WIDC_BUTTON5 )  // 루 키
	{
		m_pWndGuildPayConfirm = std::make_unique<CWndGuildPayConfirm>();
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_ROOKIE;
	}
	else if( nID == WIDC_OK )	// 보내기
	{
		g_DPlay.SendGuildAuthority(m_aPowers);
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


void CWndGuildTabApp::SetPenya() {
	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return;

	CString strMessege;
	for (int i = 0; i < MAX_GM_LEVEL; ++i) {
		strMessege.Format("%d", pGuild->m_aPenya[i]);
		m_pWndPenya[i]->SetTitle(strMessege);
	}
}


void CWndGuildTabApp::SetData(const GuildPowerss & dwPower) {
	m_aPowers = dwPower;

	ForEachPower([&](CWndButton & button, const int gud, const GuildPower power) {
		button.SetCheck(m_aPowers[gud][power] ? TRUE : FALSE);

		if (gud == GUD_MASTER) button.EnableWindow(FALSE);
		});
}

/****************************************************
  WndId : APP_GUILD_PAYCONFIRM
****************************************************/

void CWndGuildPayConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildPayConfirm::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_GUILD_PAYCONFIRM );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_PAYCONFIRM ); 
} 

BOOL CWndGuildPayConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

		CString strcost = pWndEdit->GetString();

		BOOL bisdigit = TRUE;
		for( int i = 0 ; i < strcost.GetLength() ; i++ )
		{
			if( isdigit2( strcost.GetAt(i) ) == FALSE )
			{
				bisdigit = FALSE;	
				break;
			}
		}

		
		if( bisdigit )
		{ 
			DWORD dwSendPenya = atoi( pWndEdit->GetString() );

			if( 0 <= dwSendPenya && dwSendPenya <= 999999 )
			{	
				CGuild * pGuild = g_pPlayer->GetGuild();

				if( pGuild )
				{
					// Send보냄
					g_DPlay.SendGuilPenya( pGuild->GetGuildId(), m_dwAppellation, dwSendPenya );
				}
				Destroy();
			}
			else
			{
				pWndEdit->SetString( "" );
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDONLYNUMBER), MB_OK );
			}
		}
		else
		{
			pWndEdit->SetString( "" );
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDONLYNUMBER), MB_OK );
		}
	}
	else
	if( nID == WIDC_NO )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildTabApp::EnableButton(BOOL bEnable)
{
	ForEachPower([bEnable](CWndButton & button, int gud, GuildPower) {
		if (gud == GUD_MASTER) button.EnableWindow(FALSE);
		else button.EnableWindow(bEnable);
		});

	CWndButton* pWndCheck = NULL;
	
	// 버튼 부분
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndCheck->EnableWindow(bEnable);
	
}

void CWndGuildTabApp::UpdateData()
{
	m_pWndPenya[GUD_MASTER] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_MASTER );
	m_pWndPenya[GUD_KINGPIN] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_KINGPIN );
	m_pWndPenya[GUD_CAPTAIN] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_CAPTAIN );
	m_pWndPenya[GUD_SUPPORTER] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_SUPPORTER );
	m_pWndPenya[GUD_ROOKIE] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_ROOKIE );
	
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	if( pGuild  )
	{
		CString str;
		CWndStatic* pStatic;
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_MASTER );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_MASTER), pGuild->GetMaxMemberLvSize( GUD_MASTER ) );
		pStatic->SetTitle( str );
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_KINGPIN );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_KINGPIN), pGuild->GetMaxMemberLvSize( GUD_KINGPIN ) );
		pStatic->SetTitle( str );
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_CAPTAIN  );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_CAPTAIN), pGuild->GetMaxMemberLvSize( GUD_CAPTAIN ) );
		pStatic->SetTitle( str );
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_SUPPORTER );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_SUPPORTER), pGuild->GetMaxMemberLvSize( GUD_SUPPORTER ) );
		pStatic->SetTitle( str );

		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_ROOKIE );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_ROOKIE), pGuild->GetMaxMemberSize() );
		pStatic->SetTitle( str );
		
		SetData( pGuild->m_aPower );
		SetPenya();
		
		if(	pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
			EnableButton( TRUE );
		else
			EnableButton( FALSE );
	}
	else
	{
		SetData( GuildPowerss() );

		EnableButton( FALSE );
		
		for( int i = 0 ; i < MAX_GM_LEVEL ; ++i )
		{
			m_pWndPenya[i]->SetTitle( "0" );
		}
	}
	;
}
