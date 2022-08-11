#include "stdafx.h"
#include "ResData.h"
#include "WndGuildTabWar.h"
#include "guildwar.h"
#include "defineText.h"
#include "MsgHdr.h"

#include "DPClient.h"

/****************************************************
  WndId : APP_GUILD_TABGUILDWAR - 길드전
  CtrlId : WIDC_STATIC1 - 전쟁길드명칭 :
  CtrlId : WIDC_STATIC2 - 전쟁길드로고 :
  CtrlId : WIDC_STATIC3 - 길드전승리횟수 :
  CtrlId : WIDC_STATIC4 - 길드전패배횟수 :
  CtrlId : WIDC_STATIC5 - 길드전항복횟수 :
  CtrlId : WIDC_CUSTOM1 - Custom
  CtrlId : WIDC_GUILDWARGUILDNAME - 
  CtrlId : WIDC_GUILDWARWIN - 
  CtrlId : WIDC_GUILDWARLOSE - 
  CtrlId : WIDC_GUILDWARGIVEUP - 
  CtrlId : WIDC_BUTTON1 - 길드전선언
  CtrlId : WIDC_BUTTON2 - 휴전요청
  CtrlId : WIDC_BUTTON3 - 길드전항복
****************************************************/

CWndGuildTabWar::CWndGuildTabWar() 
{ 
	m_pWndGuildWarDecl = NULL;
	m_pWndGuildWarGiveUp = NULL;
	m_pWndGuildWarPeace = NULL;
} 
CWndGuildTabWar::~CWndGuildTabWar() 
{ 
	SAFE_DELETE( m_pWndGuildWarDecl );
	SAFE_DELETE( m_pWndGuildWarGiveUp );
	SAFE_DELETE( m_pWndGuildWarPeace );
} 
void CWndGuildTabWar::OnDraw( C2DRender* p2DRender ) 
{ 
	if( !g_pPlayer )
		return;

	CGuildWar* pWar	= g_pPlayer->GetWar();
	if( !pWar )
		return;
	CWndBase* pWndBase	= GetDlgItem( WIDC_GUILDWARLEFTTIME );
	if( pWndBase )
	{
		CTimeSpan tsLeft	= pWar->GetEndTime() - CTime::GetCurrentTime();
		pWndBase->SetTitle( tsLeft.Format( "%H:%M:%S" ) );
	}
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	CGuild* pGuild = g_pPlayer->GetGuild();
	DWORD dwEnemyLogo = 0;
	u_long	idEnemyGuild = NULL_ID;

	if( pWar->m_Acpt.idGuild != pGuild->m_idGuild )		// Acpt와 Decl측 둘중하나는 적길드다.
		idEnemyGuild = pWar->m_Acpt.idGuild;
	else
	if( pWar->m_Decl.idGuild != pGuild->m_idGuild )
		idEnemyGuild = pWar->m_Decl.idGuild;

	if( idEnemyGuild != NULL_ID )
	{
		CGuild *pEnemyGuild = g_GuildMng.GetGuild( idEnemyGuild );
		if( pEnemyGuild )
			dwEnemyLogo = pEnemyGuild->m_dwLogo;		// 적길드의 로고 읽어냄.
		else
		{
			Error( "CWndGuildTabWar : 적길드를 찾을수 없음." );
			return;
		}
	} else
	{
		Error( "CWndGuildTabWar : 적길드를 찾을수 없음." );
		return;
	}
	

	if( pWndWorld && dwEnemyLogo )
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		
		CPoint point = lpWndCtrl->rect.TopLeft();
		
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		
		pWndWorld->m_pTextureLogo[dwEnemyLogo-1].Render( &g_Neuz.m_2DRender, point, 255 );
		
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}
} 

void CWndGuildTabWar::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	UpdateData();

	// 미국 버튼 이미지 변경
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtGuildDuel.bmp" ), 0xffff00ff );
	}

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildTabWar::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_GUILD_TABGUILDWAR, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildTabWar::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildTabWar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildTabWar::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildTabWar::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildTabWar::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildTabWar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild == NULL )
		return FALSE;

	switch( nID )
	{
	case WIDC_BUTTON1:		// 길드전 선언
		if( !pGuild->IsMaster(g_pPlayer->m_idPlayer) )
			return FALSE;
		SAFE_DELETE(m_pWndGuildWarDecl);
		m_pWndGuildWarDecl = new CWndGuildWarDecl;
		m_pWndGuildWarDecl->Initialize( this );
		break;
	case WIDC_BUTTON2:		// 길드전 휴전
		if( !pGuild->IsMaster(g_pPlayer->m_idPlayer) )
			return FALSE;
		SAFE_DELETE(m_pWndGuildWarPeace);
		m_pWndGuildWarPeace = new CWndGuildWarPeace;
		m_pWndGuildWarPeace->Initialize( this );
		break;
	case WIDC_BUTTON3:		// 길드전 항복
		SAFE_DELETE(m_pWndGuildWarGiveUp);
		m_pWndGuildWarGiveUp = new CWndGuildWarGiveUp;
		m_pWndGuildWarGiveUp->Initialize( this );
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildTabWar::UpdateData( void )
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		CString strText;
		CWndBase *pWndText;

		CGuild *pEnemyGuild = g_GuildMng.GetGuild( pGuild->m_idEnemyGuild );
		
		pWndText = GetDlgItem( WIDC_GUILDWARGUILDNAME );
		if( pEnemyGuild )
			pWndText->SetTitle( pEnemyGuild->m_szGuild );		// 길드명
		else
			pWndText->SetTitle( "" );

		pWndText	= GetDlgItem( WIDC_GUILDWARLEFTTIME );
		pWndText->SetTitle( "" );

		pWndText = GetDlgItem( WIDC_GUILDWARWIN );
		strText.Format( prj.GetText( TID_GAME_GUILD_WAR_DATA_WIN ), pGuild->m_nWin );
		pWndText->SetTitle( strText );		// x 승
		pWndText = GetDlgItem( WIDC_GUILDWARLOSE );
		strText.Format( prj.GetText( TID_GAME_GUILD_WAR_DATA_LOSE ), pGuild->m_nLose );
		pWndText->SetTitle( strText );		// x 패
		pWndText = GetDlgItem( WIDC_GUILDWARGIVEUP );
		strText.Format( prj.GetText( TID_GAME_GUILD_WAR_DATA_SURRENDER ), pGuild->m_nSurrender );
		pWndText->SetTitle( strText );		// x 회
	}
}


/****************************************************
	WndId : APP_GUILD_WARREQUEST - 길드전요청창
	CtrlId : WIDC_EDIT1 -
	CtrlId : WIDC_YES - Button
	CtrlId : WIDC_NO - No
****************************************************/

void CWndGuildWarRequest::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CGuild * pEnemyGuild = g_GuildMng.GetGuild(m_idEnemyGuild);
	if (pEnemyGuild) {
		CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);

		CString strText;
		strText.Format(prj.GetText(TID_GAME_GUILDWARREQUEST), pEnemyGuild->m_szGuild, m_szMaster);
		pWndEdit->SetString(strText);
		pWndEdit->EnableWindow(FALSE);
	}

	MoveParentCenter();
}

BOOL CWndGuildWarRequest::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_GUILD_WARREQUEST, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndGuildWarRequest::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_YES:
			g_DPlay.SendPacket<PACKETTYPE_ACPT_GUILD_WAR, u_long>(m_idEnemyGuild);
			Destroy();
			break;
		case WIDC_NO:
			Destroy();
			break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


/****************************************************
	WndId : APP_GUILD_WARPEACECONFIRM - 휴전승인창
	CtrlId : WIDC_YES - Yes
	CtrlId : WIDC_NO - No
	CtrlId : WIDC_STATIC1 - 휴전요청이 들어왔습니다.
	CtrlId : WIDC_STATIC2 - 승인하시겠습니까?
****************************************************/

CWndGuildWarPeaceConfirm::CWndGuildWarPeaceConfirm() {
}
CWndGuildWarPeaceConfirm::~CWndGuildWarPeaceConfirm() {
}
void CWndGuildWarPeaceConfirm::OnDraw(C2DRender * p2DRender) {
}
void CWndGuildWarPeaceConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildWarPeaceConfirm::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(APP_GUILD_WARPEACECONFIRM, pWndParent, 0, CPoint(0, 0));
}
/*
	직접 윈도를 열때 사용
BOOL CWndGuildWarPeaceConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( _T( "title" ) );
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
*/
BOOL CWndGuildWarPeaceConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase) {
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildWarPeaceConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildWarPeaceConfirm::OnLButtonUp(UINT nFlags, CPoint point) {
}
void CWndGuildWarPeaceConfirm::OnLButtonDown(UINT nFlags, CPoint point) {
}
BOOL CWndGuildWarPeaceConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_YES:
			// 휴전을 승락 한다는것을 서버로 보냄.
			if (g_pPlayer)
				g_DPlay.SendAcptTruce(g_pPlayer->m_idPlayer);
			Destroy();
			break;
		case WIDC_NO:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


/****************************************************
	WndId : APP_GUILD_WARPEACE - 휴전창
	CtrlId : WIDC_YES - Yes
	CtrlId : WIDC_NO - No
	CtrlId : WIDC_STATIC1 - 상대길드에 휴전을 요청하겠습니까?
****************************************************/

CWndGuildWarPeace::CWndGuildWarPeace() {
}
CWndGuildWarPeace::~CWndGuildWarPeace() {
}
void CWndGuildWarPeace::OnDraw(C2DRender * p2DRender) {
}
void CWndGuildWarPeace::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildWarPeace::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(APP_GUILD_WARPEACE, pWndParent, 0, CPoint(0, 0));
}
/*
	직접 윈도를 열때 사용
BOOL CWndGuildWarPeace::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( _T( "title" ) );
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
*/
BOOL CWndGuildWarPeace::OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase) {
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildWarPeace::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildWarPeace::OnLButtonUp(UINT nFlags, CPoint point) {
}
void CWndGuildWarPeace::OnLButtonDown(UINT nFlags, CPoint point) {
}
BOOL CWndGuildWarPeace::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_YES:
			// 휴전신청을 서버로 보냄.
			g_DPlay.SendQueryTruce(g_pPlayer->m_idPlayer);
			Destroy();
			break;
		case WIDC_NO:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

/****************************************************
	WndId : APP_GUILD_WARGIVEUP - 항복창
	CtrlId : WIDC_YES - Button
	CtrlId : WIDC_NO - Button
	CtrlId : WIDC_STATIC1 - 길드전에서 항복을 하겠습니까?
****************************************************/

CWndGuildWarGiveUp::CWndGuildWarGiveUp() {
}
CWndGuildWarGiveUp::~CWndGuildWarGiveUp() {
}
void CWndGuildWarGiveUp::OnDraw(C2DRender * p2DRender) {
}
void CWndGuildWarGiveUp::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildWarGiveUp::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(APP_GUILD_WARGIVEUP, pWndParent, 0, CPoint(0, 0));
}
/*
	직접 윈도를 열때 사용
BOOL CWndGuildWarGiveUp::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( _T( "title" ) );
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
*/
BOOL CWndGuildWarGiveUp::OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase) {
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildWarGiveUp::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildWarGiveUp::OnLButtonUp(UINT nFlags, CPoint point) {
}
void CWndGuildWarGiveUp::OnLButtonDown(UINT nFlags, CPoint point) {
}
BOOL CWndGuildWarGiveUp::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_YES:
			// 항복했다는걸 서버로 보냄.
			if (g_pPlayer)
				g_DPlay.SendSurrender(g_pPlayer->m_idPlayer);
			Destroy();
			break;
		case WIDC_NO:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



/****************************************************
	WndId : APP_GUILD_WAR - 길드전선언창
	CtrlId : WIDC_OK - Button
	CtrlId : WIDC_CANCEL - Button
	CtrlId : WIDC_STATIC1 - 상대길드
	CtrlId : WIDC_STATIC2 - 대전페냐
	CtrlId : WIDC_EDIT1 -
	CtrlId : WIDC_EDIT2 -
****************************************************/


CWndGuildWarDecl::CWndGuildWarDecl() {
}
CWndGuildWarDecl::~CWndGuildWarDecl() {
}
void CWndGuildWarDecl::OnDraw(C2DRender * p2DRender) {
}
void CWndGuildWarDecl::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	CWndEdit * pWndName = (CWndEdit *)GetDlgItem(WIDC_EDIT1);	// 상대길드명.
	CWndEdit * pWndPenya = (CWndEdit *)GetDlgItem(WIDC_EDIT2);	// 전쟁자금.

	pWndName->SetTabStop(TRUE);
	pWndPenya->SetTabStop(TRUE);
	pWndName->SetFocus();

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildWarDecl::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(APP_GUILD_WAR, pWndParent, 0, CPoint(0, 0));
}
/*
	직접 윈도를 열때 사용
BOOL CWndGuildWarDecl::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( _T( "title" ) );
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
*/
BOOL CWndGuildWarDecl::OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase) {
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildWarDecl::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildWarDecl::OnLButtonUp(UINT nFlags, CPoint point) {
}
void CWndGuildWarDecl::OnLButtonDown(UINT nFlags, CPoint point) {
}
BOOL CWndGuildWarDecl::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_OK:
		{
			CWndEdit * pWndEdit = (CWndEdit *)GetDlgItem(WIDC_EDIT1);
			if (g_pPlayer) {
				CString strGuild = pWndEdit->GetString();
				if (strGuild.GetLength() >= 3 && strGuild.GetLength() < MAX_G_NAME) {
					strGuild.TrimLeft();	strGuild.TrimRight();
					g_DPlay.SendDeclWar(g_pPlayer->m_idPlayer, (LPSTR)(LPCSTR)strGuild);
					Destroy();
				}
			}
		}
		break;
		case WIDC_CANCEL:
			Destroy();
			break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


