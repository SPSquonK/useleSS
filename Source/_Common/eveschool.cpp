#include "stdafx.h"

#include "eveschool.h"
#ifdef __WORLDSERVER
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "guild.h"
#include "dpdatabaseclient.h"
#include "user.h"
#include "WorldMng.h"
#include "definetext.h"
#include "defineobj.h"
#include "definesound.h"

#include "playerdata.h"

CEveSchool::CEveSchool()
{
	m_pSchoolMng	= &g_GuildMng;
	m_dwsbstart2	= GetTickCount();
}

BOOL CEveSchool::Ready( void )
{
// "/sbready
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_END )
		return FALSE;

	g_UserMng.ModifyMode( OBSERVE_MODE );	// Observe mode	// all

	for( auto i = m_pSchoolMng->m_mapPGuild.begin(); i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
		pSchool->m_nDead	= 0;
		for( auto i2 = pSchool->m_mapPMember.begin(); i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				pUser->Replace( WI_WORLD_EVENT01, GetPos( pSchool->m_idGuild ), REPLACE_NORMAL, nDefaultLayer );
				pUser->SetNotMode( OBSERVE_MODE );	// Not observe mode
				g_UserMng.AddModifyMode( pUser );
			}
		}
	}

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_READY ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_READY );

	return TRUE;
}

BOOL CEveSchool::Start( void )
{
// "/sbstart"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_READY )
		return FALSE;

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_START ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_START );
	return TRUE;
}

BOOL CEveSchool::Start2( void )
{
// "/sbstart2"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_START )
		return FALSE;

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_START2 ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_START2 );

	m_dwsbstart2	= GetTickCount();
	return TRUE;
}

BOOL CEveSchool::Report( void )
{
//	"/sbreport"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) == SBS_END )
		return FALSE;

	SCHOOL_ENTRY	school[MAX_SCHOOL];

	int iIndex	= 0;
	for( auto i = m_pSchoolMng->m_mapPGuild.begin(); i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
		
		school[iIndex].id	= pSchool->m_idGuild;
		lstrcpy( school[iIndex].lpName, pSchool->m_szGuild );
		school[iIndex].nDead	= pSchool->m_nDead;

		for( auto i2 = pSchool->m_mapPMember.begin(); i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				school[iIndex].nSize++;
				if( pUser->IsRegionAttr( RA_FIGHT ) )
				{
					if( !pUser->IsMode( OBSERVE_MODE ) )
					{
						school[iIndex].nSurvivor++;
						school[iIndex].nLevel	+= pUser->GetLevel();
					}
				}
			}
		}
		iIndex++;
	}
	// sort
	for( int j = 0; j < MAX_SCHOOL-1; j++ )
	{
		for( int k = j + 1; k < MAX_SCHOOL; k++ )
		{
			int nAvg1, nAvg2;
			if( school[j].nSurvivor == 0 )
				nAvg1	= 0;
			else
				nAvg1	= school[j].nLevel / school[j].nSurvivor;
			if( school[k].nSurvivor == 0 )
				nAvg2	= 0;
			else
				nAvg2	= school[k].nLevel / school[k].nSurvivor;

			if( ( school[j].nSurvivor < school[k].nSurvivor ) ||
				( school[j].nSurvivor == school[k].nSurvivor && nAvg1 < nAvg2 ) )
			{	// swap
				SCHOOL_ENTRY tmp;
				memcpy( &tmp, &school[j], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[j], &school[k], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[k], &tmp, sizeof( SCHOOL_ENTRY) );
			}
		}
	}

	short nElapse	= (short)( (int)( GetTickCount() - m_dwsbstart2 ) / (int)MIN( 1 ) );
	g_UserMng.AddSchoolReport( school, nElapse );	// neuz

	return TRUE;
}

BOOL CEveSchool::End( void )
{
// "/sbend"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_START2 )	// '/sbstart2'
		return FALSE;

	SCHOOL_ENTRY	school[MAX_SCHOOL];

	int iIndex	= 0;
	for (CGuild * pSchool : m_pSchoolMng->m_mapPGuild | std::views::values) {
		
		school[iIndex].id	= pSchool->m_idGuild;
		lstrcpy( school[iIndex].lpName, pSchool->m_szGuild );
		school[iIndex].nDead	= pSchool->m_nDead;

		for (CGuildMember * pMember : pSchool->m_mapPMember | std::views::values) {
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( pUser ) )
			{
				if( pUser->IsRegionAttr( RA_FIGHT ) )
				{
					if( !pUser->IsMode( OBSERVE_MODE ) )
					{
						school[iIndex].nSurvivor++;
						school[iIndex].nLevel	+= pUser->GetLevel();
					}
				}
				else
				{
					pUser->SetMode( OBSERVE_MODE );		// MAKE observe mode
					g_UserMng.AddModifyMode( pUser );
				}
			}
		}
		iIndex++;
	}

	// sort
	for( int j = 0; j < MAX_SCHOOL-1; j++ )
	{
		for( int k = j + 1; k < MAX_SCHOOL; k++ )
		{
			int nAvg1, nAvg2;
			if( school[j].nSurvivor == 0 )
				nAvg1	= 0;
			else
				nAvg1	= school[j].nLevel / school[j].nSurvivor;
			if( school[k].nSurvivor == 0 )
				nAvg2	= 0;
			else
				nAvg2	= school[k].nLevel / school[k].nSurvivor;

			if( ( school[j].nSurvivor < school[k].nSurvivor ) ||
				( school[j].nSurvivor == school[k].nSurvivor && nAvg1 < nAvg2 ) )
			{	// swap
				SCHOOL_ENTRY tmp;
				memcpy( &tmp, &school[j], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[j], &school[k], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[k], &tmp, sizeof( SCHOOL_ENTRY) );
			}
		}
	}

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_END ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_END );

	g_dpDBClient.SendSchoolReport( school );	// log
	g_UserMng.AddSchoolReport( school );	// neuz	

	return TRUE;
}

BOOL CEveSchool::LoadPos( LPCSTR lpszFileName )
{
	CScanner s;

	if( !s.Load( lpszFileName ) )
		return FALSE;
	
	u_long id;
	D3DXVECTOR3 vPos;

	id	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		vPos.x	= s.GetFloat();
		vPos.y	= s.GetFloat();
		vPos.z	= s.GetFloat();

		if( id > MAX_SCHOOL || id < 1 )
			m_vDefault	= vPos;
		else
			m_avPos[id-1]	= vPos;
		id	= s.GetNumber();
	}
	return TRUE;
}

D3DXVECTOR3 CEveSchool::GetPos( u_long id )
{
	if( id > MAX_SCHOOL || id < 1 )
//		return D3DXVECTOR3( 0, 0, 0 );
		return m_vDefault;
	return m_avPos[id-1];
}

CEveSchool* CEveSchool::GetInstance()
{
	static	CEveSchool	sEveSchool;
	return & sEveSchool;
}
#endif // __WORLDSERVER

namespace sqktd {
	template<typename Element, typename Score>
	struct BestFinder {
		std::vector<Element> m_values;
		std::optional<Score> m_currentBestScore;

		void Add(const Element & value, const Score score) {
			if (m_currentBestScore && m_currentBestScore.value() > score) {
				return;
			}

			if (m_currentBestScore && m_currentBestScore.value() < score) {
				m_currentBestScore = std::nullopt;
				m_values.clear();
			}

			m_currentBestScore = score;
			m_values.emplace_back(value);
		}

		template<typename Comparator>
		std::optional<Element> Finalize(Comparator comparator = std::less<Element>()) {
			if (m_values.empty()) return std::nullopt;
			return *std::min_element(m_values.begin(), m_values.end(), comparator);
		}
	};
}

CGuildCombat::CGuildCombat()
{
	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;
	m_nGuildCombatIndex = 0;
	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	m_uBestPlayer = 0;
	m_vecGCGetPoint.clear();
	m_vecGCPlayerPoint.clear();
#ifdef __WORLDSERVER
	m_vecGuildCombatMem.clear();
	vecRequestRanking.clear();
	m_dwTime = 0;
	m_nStopWar = 0;
	m_nJoinPanya = 0;
	m_nGuildLevel = 0;
	m_nMaxJoinMember = 0;
	m_nMaxPlayerLife = 0;
	m_nMaxWarPlayer = 0;
	m_nMaxPlayerPercent = 0;
	m_nMaxGuildPercent = 0;
	m_nRequestCanclePercent = 0;
	m_nNotRequestPercent = 0;
	m_nItemPenya = 0;
	m_bMutex     = FALSE;
	m_bMutexMsg  = FALSE;

	memset( __AutoOpen, 0, sizeof(__AUTO_OPEN) * 7 );	

	m_nDay	= 0;
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_bRequest = FALSE;
#endif // __CLIENT
}

CGuildCombat::~CGuildCombat()
{
	GuildCombatClear( 99 );
}

void CGuildCombat::GuildCombatClear( int nState )
{
	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;

	if( nState == 99 || nState == 1 )
	{
		m_nGuildCombatIndex = 0;
		m_uWinGuildId = 0;
		m_nWinGuildCount = 0;
		m_uBestPlayer = 0;
	}	

	m_vecGCGetPoint.clear();
#ifdef __WORLDSERVER

	for( int gcmi = 0 ; gcmi < (int)( m_vecGuildCombatMem.size() ) ; ++gcmi )
		safe_delete( m_vecGuildCombatMem[gcmi] );
	m_vecGuildCombatMem.clear();
	vecRequestRanking.clear();
	m_dwTime = 0;
	m_nStopWar = 0;
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( CLOSE_STATE );		

	if( nState == 99 || nState == 3 )
	{
		m_nJoinPanya = 0;
		m_nGuildLevel = 0;
		m_nMaxJoinMember = 0;
		m_nMaxPlayerLife = 0;
		m_nMaxWarPlayer = 0;
		m_nMaxPlayerPercent = 0;
		m_nMaxGuildPercent = 0;
		m_nRequestCanclePercent = 0;
		m_nNotRequestPercent = 0;
		m_nItemPenya = 0;
	}
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_bRequest = FALSE;
#endif // __CLIENT
}

void CGuildCombat::GuildCombatGameClear()
{
	GuildCombatClear( 2 );
}

void CGuildCombat::AddvecGCGetPoint(__GCGETPOINT gcGetPoint)
{
	m_vecGCGetPoint.emplace_back(gcGetPoint);
#ifdef __WORLDSERVER
	g_UserMng.AddGCLogRealTimeWorld(gcGetPoint);
#endif // __WORLDSERVER
}

void CGuildCombat::AddvecGCPlayerPoint( u_long uidPlayer, int nJob, int nPoint )
{
	__GCPLAYERPOINT GCPlayerPoint{
		.uidPlayer = uidPlayer,
		.nJob = nJob,
		.nPoint = nPoint
	};
	m_vecGCPlayerPoint.push_back( GCPlayerPoint );
}

#ifdef __WORLDSERVER
void CGuildCombat::SelectPlayerClear(const u_long uidGuild) {
	if (__GuildCombatMember * pGCMember = FindGuildCombatMember(uidGuild)) {
		pGCMember->SelectMemberClear();
	}
}

// 참가 신청
void CGuildCombat::JoinGuildCombat( u_long idGuild, DWORD dwPenya, BOOL bRequest )
{
	__GuildCombatMember * pGCMember = FindGuildCombatMember( idGuild );
	
	if (pGCMember == nullptr) {
		pGCMember = new __GuildCombatMember(idGuild);
		m_vecGuildCombatMem.push_back(pGCMember);
	}

	pGCMember->dwPenya = dwPenya;		
	pGCMember->bRequest = bRequest;

	SetRequestRanking();
}
// 참가 탈퇴
void CGuildCombat::OutGuildCombat( u_long idGuild )
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		return; 
	
	__GuildCombatMember* pGCMember = FindGuildCombatMember( idGuild );
	if( pGCMember != NULL ) // 취소시 바로 우편함으로 지급
	{
		CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
		if( pGuild && pGCMember->bRequest )
		{
			CItemElem itemElem;
			LPCTSTR szMsg = prj.GetText( TID_GAME_GC_CANCELREQUEST );
			LPCTSTR szMsg1 = prj.GetText( TID_GAME_GC_CANCELREQUEST1 );
			g_dpDBClient.SendQueryPostMail( pGuild->m_idMaster, 0, itemElem, MulDiv( pGCMember->dwPenya, m_nRequestCanclePercent, 100 ), szMsg, szMsg1 );
		}		
		pGCMember->bRequest = FALSE;
		pGCMember->dwPenya = 0;
	}
	SetRequestRanking();
}

// 참가한 길드가 전투에 참가할 유저를 선택
void CGuildCombat::AddSelectPlayer( u_long idGuild, u_long uidPlayer )
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember( idGuild );
	if( pGCMember != NULL )
	{
		__JOINPLAYER* pJoinPlayer = new __JOINPLAYER;
		pJoinPlayer->nlife = m_nMaxPlayerLife;	
		pJoinPlayer->uidPlayer = uidPlayer;
		pJoinPlayer->nMap = 99;
		pJoinPlayer->nPoint = 0;
		pJoinPlayer->dwTelTime = 0;
		pGCMember->vecGCSelectMember.push_back( pJoinPlayer );
	}
}
void CGuildCombat::GetSelectPlayer( u_long idGuild, std::vector<__JOINPLAYER> &vecSelectPlayer )
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember( idGuild );
	if (!pGCMember) return;

	for (const __JOINPLAYER* pJoinPlayer : pGCMember->vecGCSelectMember) {
		vecSelectPlayer.push_back( *pJoinPlayer );
	}
}

void CGuildCombat::JoinWar( CUser* pUser, int nMap , BOOL bWar)
{
	D3DXVECTOR3 v3Pos[4];
	v3Pos[0] = D3DXVECTOR3(1263, 83, 1303);
	v3Pos[1] = D3DXVECTOR3(1310, 83, 1303);
	v3Pos[2] = D3DXVECTOR3(1265, 83, 1254);
	v3Pos[3] = D3DXVECTOR3(1314, 83, 1253);

	int nPos = xRandom(4);
	if( nMap == 99 )
		nMap = nPos;

	if( m_nState == WAR_STATE )
	{
		if( m_nGCState == WAR_WAR_STATE )
			pUser->m_nGuildCombatState = 1;
		else if( m_nGCState == MAINTENANCE_STATE )
			pUser->m_nGuildCombatState = 2;

		if( m_nGCState == WAR_WAR_STATE || m_nGCState == MAINTENANCE_STATE )
			g_UserMng.AddGuildCombatUserState( pUser );
	}
	
	pUser->Replace( WI_WORLD_GUILDWAR, v3Pos[nMap], REPLACE_NORMAL, nDefaultLayer );
	pUser->m_vtInfo.SetOther( NULL );

	if( bWar )
	{
	}
	else
	{
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_WELCOME) );
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_MASTER_MAP) );
	}
}

void CGuildCombat::OutWar( CUser* pUser, CUser* pLeader, BOOL bLogOut )
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if( pGCMember != NULL )
	{
		// 먼저 다른 유저를 찾음
		SetPlayerChange( pUser, pLeader );
		// 디펜더가 로그아웃중이면 다른 출전자가 자동으로 디펜더가 됨

		if( bLogOut )
		{
			if( pUser->m_idPlayer == pGCMember->m_uidDefender )	// 디펜더 검사
			{
				CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild  );
				if( pGuild )
				{
					std::vector<u_long> vecDefender;
					
					for (const __JOINPLAYER* pJoinPlayer3 : pGCMember->vecGCSelectMember) {
						CMover* pMover = prj.GetUserByID( pJoinPlayer3->uidPlayer );
						if( IsValidObj( pMover ) )
						{						
							if( pGuild->IsMaster( pMover->m_idPlayer ) == FALSE && pGCMember->m_uidDefender != pMover->m_idPlayer && 0 < pJoinPlayer3->nlife )
							{
								vecDefender.push_back(pJoinPlayer3->uidPlayer);
							}
						}
					}

					if(!vecDefender.empty())
					{
						int nPlayerPos = xRandom( vecDefender.size() );
						u_long nDefender = vecDefender[nPlayerPos];
						pGCMember->m_uidDefender = nDefender;
					}
				}
			}	// if( pUser->m_idPlayer == pGCMember->m_uidDefender )	// 디펜더 검사
		}	// if( bLogOut )
	}
	g_UserMng.AddGCGuildStatus( pUser->m_idGuild );
	g_UserMng.AddGCWarPlayerlist();

	// 중간에 게임 중단해도 되는지 검사
	UserOutGuildCombatResult( pUser );
}
void CGuildCombat::JoinObserver( CUser* pUser )
{
	if( pUser->IsBaseJob() )
	{
		pUser->AddDiagText( prj.GetText(TID_GAME_GUILDCOMBAT_WAR_ENTER) );
		return;
	}
	
	// 버프 없애기
	pUser->RemoveCommonBuffs();
	g_UserMng.AddRemoveAllSkillInfluence( pUser );
	pUser->Replace( WI_WORLD_GUILDWAR, D3DXVECTOR3( 1361.6f, 0.0f, 1273.3f ), REPLACE_NORMAL, nDefaultLayer );
	pUser->m_vtInfo.SetOther( NULL );
	// 길드대전에 오신걸 환영합니다.
	pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_WELCOME) );
	// 관전자 모드로 입장하셨습니다.
	pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_ZOOM_USE) );

	if( m_nGCState <= ENTER_STATE )
		pUser->AddGCWarPlayerlist();

	if( IsSelectPlayer( pUser ) )
	{
		pUser->m_nGuildCombatState		= 2;
		g_UserMng.AddGuildCombatUserState( (CMover*)pUser );
		if( ENTER_STATE <= m_nGCState && m_nGCState <= WAR_CLOSE_STATE )
			g_DPCoreClient.SendGCRemoveParty( pUser->GetPartyId(), pUser->m_idPlayer );
	}
	g_UserMng.AddGCGuildStatus( pUser->m_idGuild, pUser );
	g_UserMng.AddGCGuildPrecedence( pUser );
	g_UserMng.AddGCPlayerPrecedence( pUser );	
}

void CGuildCombat::GuildCombatRequest( CUser* pUser, DWORD dwPenya )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		// 시간 검사 ( 월 ~ 금 )
		//길드대전중에는 신청할 수 없습니다.
		if( m_nState != CGuildCombat::CLOSE_STATE )
		{
			pUser->AddDiagText( prj.GetText(TID_GAME_GUILDCOMBAT_REQUESTEND) );
			return;
		}

		// 길드레벨 m_nGuildLevel의 값 이상인지 검사
		if( pGuild->m_nLevel < m_nGuildLevel )
		{
			CString strMsg;
			strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_LEVEL ), m_nGuildLevel );
			pUser->AddDiagText( strMsg );
			return;
		}

		// 기존에 신청 한 페냐
		DWORD dwExistingPenya = 0;
		__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
		if( pGCMember != NULL )
			dwExistingPenya = pGCMember->dwPenya;

		// INT_MAX값에 넘지 않아야 한다
		if( dwPenya > INT_MAX )
			return;

		// 기본 페냐(m_nJoinPenya) 보다 더 많이 가지고 있는지 확인
		if( (int)( dwPenya ) < m_nJoinPanya )
		{
			CString strMsg;
			strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_BASEPENYA ), m_nJoinPanya );
			pUser->AddDiagText( strMsg );
			pUser->AddText( prj.GetText(TID_GAME_LACKMONEY) );	
			return;
		}
		
		// 전에 신청한 페냐 보다 더 많아야 한다.
		if( dwPenya <= dwExistingPenya )
		{
			CString strMsg;
			strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_PENYA ), dwPenya );
			pUser->AddDiagText( strMsg );
			return;
		}


		// 신청한 페냐가 가지고 있는지 확인
		if( pUser->GetGold() < (int)( (dwPenya-dwExistingPenya) ) )
		{
			pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_NOTPENYA ) );
			return;
		}

		LogItemInfo aLogItem;
		aLogItem.Action = "9";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "GUILDCOMBAT";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold() -(dwPenya-dwExistingPenya);
		//aLogItem.ItemName = "SEED";
		_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
		aLogItem.itemNumber = dwPenya-dwExistingPenya;
		g_DPSrvr.OnLogItem( aLogItem );

		pUser->AddGold( (int)( (-1) * (int)( dwPenya-dwExistingPenya ) ) );
		g_dpDBClient.SendInGuildCombat( pUser->m_idGuild, dwPenya, dwExistingPenya );
	}
	else
	{
		//길드가 없거나 길드장이 아닙니다.
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER) );
	}
}

void CGuildCombat::GuildCombatCancel( CUser* pUser )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		// 시간 검사 ( 월 ~ 금 )
		__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
		if( pGCMember != NULL )
		{
			if( pGCMember->bRequest )
			{
				if( m_nState != CGuildCombat::CLOSE_STATE )
				{
					pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));	// 지금은 길드대전중에는 길드대전 탈퇴를 할수 없습니다
					return;
				}
				g_dpDBClient.SendOutGuildCombat( pUser->m_idGuild );
			}
			else
			{
				pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP) );
			}
		}
		else
		{
			//길드대전 신청을 하지 않았습니다.
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP) );
		}
	}
	else
	{
		//길드가 없거나 길드장이 아닙니다.
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER) );
	}
}
// 중간에 캐릭터가 죽거나 로그아웃시에 승리길드가 나오면 대전 종료
void CGuildCombat::UserOutGuildCombatResult( CUser* pUser )
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if (!pGCMember) return;
	
	const auto hasAliveTeammate = std::ranges::any_of(pGCMember->vecGCSelectMember,
		[](const __JOINPLAYER * pJoinPlayer) { return pJoinPlayer->nlife > 0; }
		);
	if (hasAliveTeammate) return;

	// 다른길드의 상태 파악( 길드의 전투작 1개이상인지 검사 1개의 길드만 나오면 게임 종료 )
	int nCount = 0;
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
			
		__GuildCombatMember * pGCMember2 = vecRequestRanking[nVeci];
		if (pUser->m_idGuild == pGCMember2->uGuildId) continue;

		const bool bLive = std::ranges::any_of(pGCMember2->vecGCSelectMember,
			[](const __JOINPLAYER * pJoinPlayer) { return pJoinPlayer->nlife > 0; }
			);

		if( bLive )
		{
			++nCount;
			if( nCount == 2 )
			{
				return; // 2개 이상이므로 계속 진행
			}
		}
	}
	// 길드가 하나밖에 없으므로 게임 종료
	if( nCount == 1 )
		m_nStopWar = 1;
	
}

CGuildCombat::Rankings CGuildCombat::ComputeRankings() const {
	sqktd::BestFinder<u_long, int> bestPlayer;

	struct GuildWithPoints {
		u_long guildId;
		std::pair<int, int> points;
	};

	std::vector<GuildWithPoints> guilds;
	std::map<u_long, float> guildAverageLevels;
	const auto GetGuildAverageLevel = [&](const u_long guildId) {
		auto it = guildAverageLevels.find(guildId);
		if (it != guildAverageLevels.end()) return it->second;

		float averageLevel = 0.0f;
		int nbOfMembers = 0;

		if (const __GuildCombatMember * pGCMember = FindGuildCombatMember(guildId)) {
			for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
				CUser * pUser = prj.GetUserByID(pJoinPlayer->uidPlayer);
				if (IsValidObj(pUser)) {
					averageLevel += pUser->GetLevel();
					++nbOfMembers;
				}
			}
		}

		if (nbOfMembers > 0) {
			averageLevel /= nbOfMembers;
		}

		guildAverageLevels.emplace(guildId, averageLevel);
		return averageLevel;
	};


	int seenGuilds = 0;
	for (const __GuildCombatMember * pGCMember : vecRequestRanking) {
		if (seenGuilds >= m_nMaxGuild) break;
		++seenGuilds;

		int points = 0;
		int lifes = 0;
		for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
			bestPlayer.Add(pJoinPlayer->uidPlayer, pJoinPlayer->nPoint);
			points += pJoinPlayer->nPoint;
			lifes += pJoinPlayer->nlife;
		}
		
		guilds.emplace_back(GuildWithPoints{ pGCMember->uGuildId, std::pair(points, lifes) });
	}

	if (guilds.empty()) return Rankings{};

	const auto thirdPlace = guilds.begin() + std::min<size_t>(3, guilds.size());

	std::partial_sort(
		guilds.begin(),
		thirdPlace,
		guilds.end(),
		[&](const GuildWithPoints & lhs, const GuildWithPoints & rhs) {
			if (lhs.points > rhs.points) return true;
			if (lhs.points < rhs.points) return false;

			const float leftAverage  = GetGuildAverageLevel(lhs.guildId);
			const float rightAverage = GetGuildAverageLevel(rhs.guildId);

			if (leftAverage > rightAverage) return true;
			if (leftAverage < rightAverage) return false;

			return lhs.guildId < rhs.guildId;
		}
	);


	Rankings retval;
	retval.numberOfParticipantGuilds = guilds.size();

	for (auto it = guilds.begin(); it != thirdPlace; ++it) {
		retval.bestGuilds.emplace_back(it->guildId);
	}

	retval.bestPlayer = bestPlayer.Finalize(
		[](const u_long lhsPlayerId, const u_long rhsPlayerId) {
			CUser * lhsUser = prj.GetUserByID(lhsPlayerId);
			CUser * rhsUser = prj.GetUserByID(rhsPlayerId);

			if (IsValidObj(lhsUser) && IsValidObj(rhsUser)) {
				return std::tuple(lhsUser->GetLevel(), lhsUser->GetExp1())
					< std::tuple(rhsUser->GetLevel(), rhsUser->GetExp1());
			} else if (IsValidObj(lhsUser)) {
				return true;
			} else if (IsValidObj(rhsUser)) {
				return false;
			} else {
				return lhsPlayerId < rhsPlayerId;
			}
		}
	).value_or(0);

	return retval;
}

void CGuildCombat::GuildCombatResult(const Rankings & rankings)
{
	if (rankings.bestGuilds.empty()) return;


	// 걍 맨처음 길드가 우승( 공동우승이어도.. )
	const u_long bestGuildId = rankings.bestGuilds[0];
	if(CGuild * pGuild = g_GuildMng.GetGuild(bestGuildId))
	{
		if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		{
			g_UserMng.AddWorldCreateSfxObj( XI_NAT_ROCKET02, 1291.0f, 85.0f, 1279.0f, FALSE, WI_WORLD_GUILDWAR );
			g_DPCoreClient.SendPlayMusic( WI_WORLD_GUILDWAR, BGM_IN_FITUP );
		}
			
		// 연승 계산
		if( m_uWinGuildId == bestGuildId)
			++m_nWinGuildCount;
		else
			m_nWinGuildCount = 1;
			
		m_uWinGuildId = bestGuildId;

		const char * guildCombatEndMessage = prj.GetText(TID_GAME_GUILDCOMBAT_END);
		g_DPCoreClient.SendSystem(guildCombatEndMessage);
		g_DPCoreClient.SendCaption(guildCombatEndMessage);

		char str[512] = { 0, };
		sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_WINNER), pGuild->m_szGuild );
		int nBufWinGuildCount = m_nWinGuildCount;
		if( m_nMaxGCSendItem < m_nWinGuildCount )
			nBufWinGuildCount = m_nMaxGCSendItem;
		// 연승 아이템 주기
		
		for (const __GCSENDITEM & gcSendItem : vecGCSendItem) {
			if (gcSendItem.dwItemId != nBufWinGuildCount) {
				continue;
			}

			// 길드창고에 넣기
			CItemElem itemElem;
			itemElem.m_dwItemId = gcSendItem.dwItemId;
			itemElem.m_nItemNum	= gcSendItem.nItemNum;
			itemElem.m_nHitPoint = itemElem.GetProp()->dwEndurance;
			itemElem.SetSerialNumber();
			if ( pGuild->m_GuildBank.Add( &itemElem ) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "W";
				aLogItem.SendName = "GUILDCOMBAT";
				aLogItem.RecvName = "GUILDBANK";
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, gcSendItem.nItemNum );
					
				g_DPSrvr.UpdateGuildBank( pGuild, GUILD_PUT_ITEM, 0, 0, &itemElem, 0, gcSendItem.nItemNum );
				g_UserMng.AddPutItemElem(bestGuildId, &itemElem );

				CString strItemMsg;
				strItemMsg.Format( prj.GetText(TID_UPGRADE_SUPPORTM), itemElem.m_nItemNum );					
				
				CString strGuildMsg;
				strGuildMsg.Format("  - %s %s", itemElem.GetProp()->szName, strItemMsg.GetString() );
				m_vecstrGuildMsg.push_back( strGuildMsg );
			}
			else
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "W";
				aLogItem.SendName = "GUILDCOMBAT_NOT";
				aLogItem.RecvName = "GUILDBANK";
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, gcSendItem.nItemNum );
			}
		}
	}

	m_uBestPlayer = rankings.bestPlayer;

		
	++m_nGuildCombatIndex;
	g_UserMng.AddGCWinGuild();
	g_UserMng.AddGCBestPlayer();

	g_UserMng.AddGCLogWorld();
}

void CGuildCombat::GuildCombatResultRanking(const Rankings & rankings) {
	// 아이템(레드칩) 지급
	for (size_t i = 0; i < rankings.bestGuilds.size(); ++i) {
		if (i >= 3) break;

		__GuildCombatMember* pGCMember = FindGuildCombatMember(rankings.bestGuilds[i]);
		if (!pGCMember) {
			Error("GuildCombatResultRanking() - pGCMember is NULL");
			continue;
		}

		CItemElem itemElem;
		itemElem.m_dwItemId = II_CHP_RED;
		float fChipNum = m_nJoinPanya * rankings.numberOfParticipantGuilds * 0.9f * 0.00001f * 0.1f;
		switch(i) {
			case 0: fChipNum *= 0.7f; break;
			case 1: fChipNum *= 0.2f; break;
			case 2: fChipNum *= 0.1f; break;
		}
		itemElem.m_nItemNum = std::max(1, static_cast<int>(fChipNum));
		itemElem.m_bCharged = itemElem.GetProp()->bCharged;

		LogItemInfo aLogItem;
		aLogItem.Action = "W";
		aLogItem.Gold_1 = i;
		
		for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
			CUser* pUsertmp = prj.GetUserByID( pJoinPlayer->uidPlayer );
			if( IsValidObj( pUsertmp ) )
			{
				aLogItem.SendName = pUsertmp->GetName();
				// 아이템 지급
				itemElem.SetSerialNumber();
				if( pUsertmp->CreateItem( &itemElem ) )
				{
					aLogItem.RecvName = "GUILDCOMBAT_CHIP";
					pUsertmp->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_WARGUILDWINCHIP, "%d", itemElem.m_nItemNum );
				}
				else
				{
					g_dpDBClient.SendQueryPostMail( pUsertmp->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, itemElem.GetProp()->szName );
					//pUsertmp->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST, "" );
					aLogItem.RecvName = "GUILDCOMBAT_CHIP_POST";
				}
				int cbChip	= pUsertmp->GetItemNum( II_CHP_RED );
				aLogItem.Gold	= cbChip;
				aLogItem.Gold2	= cbChip + itemElem.m_nItemNum;
				aLogItem.Gold_1 = itemElem.m_nItemNum;
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
				TRACE( "GC REDCHIP - %s, %d위, %s : %d개\n", pUsertmp->GetName(), i+1, itemElem.GetProp()->szName, itemElem.m_nItemNum );
			}
		}
	}
}

void CGuildCombat::GuildCombatCloseTeleport()
{
	if( m_nStopWar != 200 )
		g_dpDBClient.SendGuildCombatResult();
	
	if(!g_WorldMng.GetRevivalPos(WI_WORLD_MADRIGAL, "flaris"))
		return;

	g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f, nDefaultLayer);

	for (const CString & guildMessage : m_vecstrGuildMsg) {
		g_UserMng.AddGuildMsg(m_uWinGuildId, guildMessage);
	}

	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;
	m_dwTime = 0;
	m_nStopWar = 0;
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( CLOSE_STATE );	

	m_bMutex = FALSE;	

	SetNpc();
}

void CGuildCombat::SetNpc( void )
{
	CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, MI_MADA_ACHABEN );
	lstrcpy( pMover->m_szCharacterKey, "Mada_Guildcombatshop" );
	pMover->InitNPCProperty();
	pMover->InitCharacter( pMover->GetCharacter() );
	pMover->SetPos( D3DXVECTOR3(1343, 94, 1215) );
	pMover->InitMotion( MTI_STAND );
	pMover->UpdateLocalMatrix();
	CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_GUILDWAR );
	if( pWorld )
		pWorld->ADDOBJ( pMover, TRUE, nDefaultLayer );
	
	CMover* pMover1	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, MI_MADA_ACHABEN );
	lstrcpy( pMover1->m_szCharacterKey, "Mada_Guildcombatshop" );
	pMover1->InitNPCProperty();
	pMover1->InitCharacter( pMover1->GetCharacter() );
	pMover1->SetPos( D3DXVECTOR3(1288, 98, 1356) );
	pMover1->InitMotion( MTI_STAND );
	pMover1->UpdateLocalMatrix();
	if( pWorld )
		pWorld->ADDOBJ( pMover1, TRUE, nDefaultLayer );
}

void CGuildCombat::GuildCombatOpen( void )
{
	// 길드전쟁 서버에 있는 모든 캐릭터들을 모두 나가게함..
	if( m_nState != CLOSE_STATE )
		return;

	m_vecstrGuildMsg.clear();
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( OPEN_STATE );	

	m_vecGCGetPoint.clear();
	m_GCResultValueGuild.clear();
	m_GCResultValuePlayer.clear();
	g_dpDBClient.SendGuildCombatStart();
	m_nGCState = NOTENTER_STATE;
	m_nProcessGo	= 0;
	LPCTSTR strOK = prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_OK);
	LPCTSTR strCancle = prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_CANCLE);

	for( int veci = 0 ; veci < (int)( vecRequestRanking.size() ) ; ++veci )
	{
		const __GuildCombatMember * RequestGuild = vecRequestRanking[veci];

		CGuild* pGuild = g_GuildMng.GetGuild( RequestGuild->uGuildId );
		if( pGuild )
		{
			for (const CGuildMember * pGuildMember : pGuild->m_mapPMember | std::views::values) {
				if( pGuildMember->m_nMemberLv == GUD_MASTER || pGuildMember->m_nMemberLv == GUD_KINGPIN )
				{
					CUser* pUser	= prj.GetUserByID( pGuildMember->m_idPlayer );
					if( IsValidObj( pUser ) )
					{
						if( veci < m_nMaxGuild )
							pUser->AddGCDiagMessage( strOK );
						else
							pUser->AddGCDiagMessage( strCancle );
					}
				}
			}
		}
	}
}

void CGuildCombat::SetRequestRanking( void ) {
	vecRequestRanking.clear();
	for (__GuildCombatMember * pGCMember : m_vecGuildCombatMem) {
		if (pGCMember->bRequest) {
			vecRequestRanking.emplace_back(pGCMember);
		}
	}

	std::ranges::stable_sort(
		vecRequestRanking,
		[](const __GuildCombatMember * lhs, const __GuildCombatMember * rhs) {
			return lhs->dwPenya > rhs->dwPenya;
		}
	);
}
void CGuildCombat::SetDefender(u_long uidGuild, u_long uidDefender) {
	if (__GuildCombatMember * pGCMember = FindGuildCombatMember(uidGuild)) {
		pGCMember->m_uidDefender = uidDefender;
	}
}

u_long CGuildCombat::GetDefender(const u_long uidGuild) {
	__GuildCombatMember * pGCMember = FindGuildCombatMember(uidGuild);
	return pGCMember ? pGCMember->m_uidDefender : 0;
}

// 지금까지의 총 상금
__int64 CGuildCombat::GetPrizePenya( int nFlag )
{
	// 상금에 포함할 길드들..
	__int64 nPrizePenya = 0;
	for( int veci = 0 ; veci < (int)( vecRequestRanking.size() ) ; ++veci )
	{
		if( veci >= m_nMaxGuild )
			break;
		
		nPrizePenya += vecRequestRanking[veci]->dwPenya;
	}
	__int64 nResult;

	switch( nFlag )
	{
	case 0:
		nResult = MulDiv( (int)( nPrizePenya ), m_nMaxGuildPercent, 100 );
		break;
	case 1:
		nResult = MulDiv( (int)( nPrizePenya ), m_nMaxPlayerPercent, 100 );
		break;
	case 2:
		nResult = nPrizePenya;
	}

	return nResult;
}
// 길드의 신청한 Penya
DWORD CGuildCombat::GetRequstPenya( u_long uidGuild )
{
	DWORD dwRequestPenya = 0;
	__GuildCombatMember* pGCMember = FindGuildCombatMember( uidGuild );
	if( pGCMember != NULL )
		dwRequestPenya = pGCMember->dwPenya;
	return dwRequestPenya;
}
// 지금부터 다음 대전이 시작될 남은시간
CTime CGuildCombat::GetNextGuildCobmatTime()
{
	CTime tCurrentTime = CTime::GetCurrentTime();
	CTime tNextCombat;
	int nDayofWeek = m_nDay - tCurrentTime.GetDayOfWeek();
	if( 0 < nDayofWeek )
		tNextCombat = tCurrentTime + CTimeSpan( nDayofWeek, 0, 0, 0 ); 
	else if( 0 == nDayofWeek )
	{
		CTimeSpan tCTime = CTimeSpan( 0, tCurrentTime.GetHour(), tCurrentTime.GetMinute(), tCurrentTime.GetSecond() );
		CTimeSpan tNTime = CTimeSpan( 0, __AutoOpen[m_nDay-1].nHour, __AutoOpen[m_nDay-1].nMinute, 0 );
		if( tCTime <= tNTime )
			tNextCombat = tCurrentTime;
		else
			tNextCombat = tCurrentTime + CTimeSpan( m_nDay, 0, 0, 0 );
	}
	else if( 0 > nDayofWeek )
	{
		nDayofWeek = 7 + nDayofWeek;
		tNextCombat = tCurrentTime + CTimeSpan( nDayofWeek, 0, 0, 0 ); 
	}
	tNextCombat = CTime( tNextCombat.GetYear(), tNextCombat.GetMonth(), tNextCombat.GetDay(), __AutoOpen[m_nDay-1].nHour, __AutoOpen[m_nDay-1].nMinute, 0 );
	return tNextCombat;
}

CGuildCombat::__GuildCombatMember * CGuildCombat::FindGuildCombatMember(const u_long GuildId) {
	const auto it = std::find_if(
		m_vecGuildCombatMem.begin(),
		m_vecGuildCombatMem.end(),
		[GuildId](const __GuildCombatMember * gcMember) {
			return gcMember->uGuildId == GuildId;
		}
	);

	return it != m_vecGuildCombatMem.end() ? *it : nullptr;
}

const CGuildCombat::__GuildCombatMember * CGuildCombat::FindGuildCombatMember(const u_long GuildId) const {
	const auto it = std::find_if(
		m_vecGuildCombatMem.begin(),
		m_vecGuildCombatMem.end(),
		[GuildId](const __GuildCombatMember * gcMember) {
			return gcMember->uGuildId == GuildId;
		}
	);

	return it != m_vecGuildCombatMem.end() ? *it : nullptr;
}

void CGuildCombat::SetSelectMap( CUser* pUser, int nMap )
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if (!pGCMember) return;
	
	__JOINPLAYER * pJoinPlayer = pGCMember->FindByPlayerId(pUser->m_idPlayer);
	if (!pJoinPlayer) return;

	pJoinPlayer->nMap = nMap;
}
void CGuildCombat::GuildCombatEnter( CUser* pUser )
{
	BOOL bJoin = TRUE;
	if( m_nState == OPEN_STATE || m_nState == CLOSE_STATE || m_nState == WAR_STATE )
	{
		BOOL bJoin = TRUE;
		if( m_nState == OPEN_STATE && m_nGCState != ENTER_STATE )
		{
			bJoin = FALSE;
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_JOIN) );
		}

		if( bJoin )
		{
			JoinObserver( pUser );
		}
	}
	else if( m_nState == COMPLET_CLOSE_STATE )
	{
		// 아무도 못들어옴.. 지금은 닫혀있는 상태
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_OPEN) );
		bJoin = FALSE;
	}
	else
	{
		bJoin = FALSE;
	}

	if( bJoin )
	{
		if( 0 < m_dwTime )
		{		
			pUser->AddGuildCombatNextTime( m_dwTime - GetTickCount(), GuildCombatProcess[m_nProcessGo - 1].dwParam );
		}
	}
}
// 전쟁 준비 시간
void CGuildCombat::SetMaintenance()
{
	LPCTSTR strMsgMaster = prj.GetText( TID_GAME_GUILDCOMBAT_JOIN_MSG_MASTER );
	LPCTSTR strMsgDefender = prj.GetText( TID_GAME_GUILDCOMBAT_JOIN_MSG_DEFENDER );
	LPCTSTR strMsgGeneral = prj.GetText( TID_GAME_GUILDCOMBAT_JOIN_MSG_GENERAL );
	
	BOOL bWinGuild_Continue = FALSE;		// 요번게임이 취소 되었다면 연승을 연장 여부
	int nCount = 0;
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		// 최대로 들어갈수 있는 길드만 들만 참여가능
		if( nVeci >= m_nMaxGuild )
			break;

		__GuildCombatMember * pGCMember = vecRequestRanking[nVeci];

		if(pGCMember->uGuildId == m_uWinGuildId )
			bWinGuild_Continue = TRUE;

			CGuild* pGuild = g_GuildMng.GetGuild( pGCMember->uGuildId );
			if( pGuild )
			{
				pGCMember->nJoinCount = 0;
				pGCMember->nWarCount = 0;
				BOOL bMaxWarCount = FALSE;

				for (__JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
					if (pJoinPlayer->nlife > 0) {
						pGCMember->lspFifo.push_back(pJoinPlayer);
					}
				}

				RefreshFifo(*pGCMember);

				g_UserMng.AddGCGuildStatus( pGCMember->uGuildId );

				for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
					CUser * pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						// Message
						if( pGuild->IsMaster( pMover->m_idPlayer ) )	// Master
							pMover->AddDiagText( strMsgMaster );
						else if( pGCMember->m_uidDefender == pMover->m_idPlayer )	// Defender
							pMover->AddDiagText( strMsgDefender );
						else
							pMover->AddDiagText( strMsgGeneral );
					}
				}

				if( !pGCMember->vecGCSelectMember.empty() )
					++nCount;
			}
		
	}

	// 참가한 길드가 2개 이상일때만 길드대전이 시작하게함.
	if( nCount < m_nMinGuild )
	{
		// 시작이 안되었을때
		// 연승한 길드가 참가하고 있으면 승수 이어감
		// 참가하지 않았으면 승수 없어짐( 승리한 길드가 없음 )
		if( bWinGuild_Continue == FALSE )
		{
			g_dpDBClient.SendGuildcombatContinue( m_nGuildCombatIndex-1, m_uWinGuildId, 0 );
			m_uWinGuildId = 0;
			m_nWinGuildCount = 0;

			g_UserMng.AddGCWinGuild();
		}
		SetGuildCombatCloseWait( TRUE );
	}
}

void CGuildCombat::SetEnter()
{
	LPCTSTR str = prj.GetText(TID_GAME_GUILDCOMBAT_ENTER);
	g_DPCoreClient.SendSystem( str );
	g_DPCoreClient.SendCaption( str, 0, TRUE );

	LPCTSTR guildCombatTeleText = prj.GetText(TID_GAME_GUILDCOMBAT_TELE);

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		// 최대로 들어갈수 있는 길드만 들만 참여가능
		if( nVeci >= m_nMaxGuild )
			break;
		
		__GuildCombatMember * pGCMember = vecRequestRanking[nVeci];

			for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
				CUser * pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pMover ) )
				{
					pMover->AddGCTele(guildCombatTeleText);
				}
			}
	}
}
// 대전 시작
void CGuildCombat::SetGuildCombatStart()
{
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
	
		__GuildCombatMember * pGuildCombatMem = vecRequestRanking[nVeci];

			for (__JOINPLAYER * pJoinPlayer : pGuildCombatMem->vecGCSelectMember) {
				const bool bFind = pGuildCombatMem->IsInFifo(pJoinPlayer);
				
				if( pJoinPlayer->nlife > 0 && !bFind )
				{
					CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) )
					{
						pMover->m_nGuildCombatState		= 1;
						g_UserMng.AddGuildCombatUserState( pMover );
					}
				}
			}
		
	}
}
// 전쟁 종료
void CGuildCombat::SetGuildCombatClose( BOOL bGM )
{
	if( m_nState == CLOSE_STATE )
		return;

	if( bGM )
	{
		m_nStopWar = 1;	
		return;
	}

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__GuildCombatMember * pGCMember = vecRequestRanking[nVeci];
			for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
				CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pMover ) )
				{
					pMover->m_nGuildCombatState = 0;
					g_UserMng.AddGuildCombatUserState( pMover );
				}
			}
		
	}

	// 길드 부활 포인트 얻은 획득 주기
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__GuildCombatMember * pGCMember = vecRequestRanking[nVeci];
			int nRevivalPoint = 0;
			for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
				nRevivalPoint += pJoinPlayer->nlife;
			}
			pGCMember->nGuildPoint += nRevivalPoint;

			CGuild * pGuildMsg = g_GuildMng.GetGuild(pGCMember->uGuildId);
			if( pGuildMsg )
			{
				for (const CGuildMember * pMember : pGuildMsg->m_mapPMember | std::views::values) {
					CUser * pUsertmp	= prj.GetUserByID( pMember->m_idPlayer );
					if( IsValidObj( pUsertmp ) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
						pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_REVIVAL, "%d", nRevivalPoint );
				}
			}
		
	}
	g_UserMng.AddGCGuildPrecedence();
}
// 전쟁 종료후 결과값
void CGuildCombat::SetGuildCombatCloseWait( BOOL bGM )
{
	if( m_nState == CLOSE_STATE )
		return;

	if( bGM )
	{
		m_nStopWar = 2;
		return;
	}
	else
	{
		Rankings rankings = ComputeRankings();
		GuildCombatResult(rankings);
		GuildCombatResultRanking(rankings);
	}	
}
// 신청한 길드중에 출전할수 있는 길드인지?
BOOL CGuildCombat::IsRequestWarGuild( u_long uidGuild, BOOL bAll )
{
	for( int veci = 0 ; veci < (int)( vecRequestRanking.size() ) ; ++veci )
	{
		if( bAll == FALSE )
		{
			if( m_nMaxGuild <= veci )
				break;
		}

		__GuildCombatMember * pGuildCombatMem = vecRequestRanking[veci];
		if( uidGuild == pGuildCombatMem->uGuildId )
			return TRUE;
	}
	return FALSE;
}

// 참가한 선수인지?
BOOL CGuildCombat::IsSelectPlayer( CUser* pUser )
{
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;

		__GuildCombatMember * pGuildCombatMem = vecRequestRanking[nVeci];
		if(pGuildCombatMem->uGuildId == pUser->m_idGuild )
		{
				__JOINPLAYER * pJoinPlayer = pGuildCombatMem->FindByPlayerId(pUser->m_idPlayer);

				if (pJoinPlayer && pJoinPlayer->nlife > 0) {
					return TRUE;
				}
			break;
		}
	}
	return FALSE;
}

void CGuildCombat::Process()
{
#ifdef __WORLDSERVER
#ifdef _DEBUG	
//	TRACE( "GuildWarFlyff::Process <%d>, <%d>\n", m_nState, m_nGuildCombatState);
#endif //_DEBUG
	if( m_nState != CLOSE_STATE )
	{
		if( m_nProcessGo < m_nProcessCount[0] )
			ProcessCommand();
		ProcessJoinWar();	//
	}
	else
	{
		CTime ctime = CTime::GetCurrentTime();

		if( __AutoOpen[ctime.GetDayOfWeek()-1].bUseing )
		{
			// 스킵 될 수 있으므로 수정을 필요로 함
			if( __AutoOpen[ctime.GetDayOfWeek()-1].nHour == ctime.GetHour() &&
				__AutoOpen[ctime.GetDayOfWeek()-1].nMinute == ctime.GetMinute() && m_bMutex == FALSE )
			{
				if( (int)( m_vecGuildCombatMem.size() ) >= m_nMinGuild )
				{				
					GuildCombatOpen();
				}
				else
				{
					m_ctrMutexOut.Set( SEC(60) );
					m_bMutexMsg = TRUE;
					g_DPCoreClient.SendSystem( prj.GetText( TID_GAME_GUILDCOMBAT_NEXT_COMBAT ) );	
					g_DPCoreClient.SendSystem( prj.GetText( TID_GAME_GUILDCOMBAT_ENJOY ) );	
				}

				m_bMutex = TRUE;				
			}

			if( m_bMutexMsg && m_ctrMutexOut.IsTimeOut() )
			{
				m_bMutexMsg = FALSE;
				m_bMutex    = FALSE;
			}
		}
	}
#endif //__WORLDSERVER
}

void CGuildCombat::ProcessJoinWar()
{
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__GuildCombatMember* pGCMember = vecRequestRanking[nVeci];
			for (__JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
				if( pJoinPlayer->dwTelTime != 0 )
				{
					if( pJoinPlayer->dwTelTime + m_nMaxMapTime * 1000 <= timeGetTime()  )
					{
						pJoinPlayer->dwTelTime = 0;
						CUser * pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );					
						if( IsValidObj( pMover ) )
						{
							JoinWar( pMover, pJoinPlayer->nMap );
						}
					}
				}
			}
		
	}
}
void CGuildCombat::ProcessCommand()
{
	if( m_nStopWar == 1 )
	{
		m_nProcessGo = m_nProcessCount[COMPLET_CLOSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 0;
	}
	else if( m_nStopWar == 2 )
	{
		m_nProcessGo = m_nProcessCount[GM_COLSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 200;
	}
	if( m_dwTime <= GetTickCount() )
	{
		if( GuildCombatProcess[m_nProcessGo].nState == WARSTATE )
		{
			if( GuildCombatProcess[m_nProcessGo].dwCommand == NOTENTER_COUNT_STATE )
#ifdef __LAYER_1015
				g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f, nDefaultLayer );
#else	// __LAYER_1015
				g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f );
#endif	// __LAYER_1015
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == MAINTENANCE_STATE )
			{
				// 전쟁 준비 시작
				SetMaintenance();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == ENTER_STATE )
			{
				// 들어올수 있는 상태
				SetEnter();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_WAR_STATE )
			{
				// 전쟁 시작 
				SetGuildCombatStart();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_STATE )
			{
				// 전쟁 종료
				SetGuildCombatClose();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_WAIT_STATE )
			{
				// 전쟁 종료후 결과값 나오기 대기
				SetGuildCombatCloseWait();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_TELEPORT_STATE )
			{
				// 전쟁 종료후 텔레포트
				GuildCombatCloseTeleport();
			}
		}

		switch( GuildCombatProcess[m_nProcessGo].nState )
		{
		case ALLMSG:
			{
				if( GuildCombatProcess[m_nProcessGo].dwCommand != TID_GAME_GUILDCOMBAT_OPEN_MSG )
				{
					CString str;
					str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
					g_DPCoreClient.SendSystem( str );
				}

				if( GuildCombatProcess[m_nProcessGo].dwCommand == TID_GAME_GUILDCOMBAT_OPEN_MSG )
					SendGuildCombatEnterTime( );
			}
			break;
		case GUILDMSG:
			{
			}
			break;
		case JOINMSG:
			{
				CString str;
				str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
//				SendJoinMsg( str );
			}
			break;
		case WORLDMSG:
			{
				CString str;
				str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
				g_UserMng.AddWorldMsg( WI_WORLD_GUILDWAR, str );
			}
			break;
		case STATE:
			{
				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
					g_DPCoreClient.SendGuildCombatState( GuildCombatProcess[m_nProcessGo].dwCommand );
//				m_nState = GuildCombatProcess[m_nProcessGo].dwCommand;
			}
			break;
		case WARSTATE:
			{
				m_nGCState = GuildCombatProcess[m_nProcessGo].dwCommand;
				g_UserMng.AddGuildCombatState();
			}
			break;
		}
		m_dwTime = GetTickCount() + GuildCombatProcess[m_nProcessGo].dwTime;
		if( GuildCombatProcess[m_nProcessGo].dwParam != 0 )
			g_UserMng.AddGuildCombatNextTimeWorld( GuildCombatProcess[m_nProcessGo].dwTime, GuildCombatProcess[m_nProcessGo].dwParam );
		++m_nProcessGo;
	}
}


BOOL CGuildCombat::LoadScript( LPCSTR lpszFileName )
{
	CScanner s;
	
	if( !s.Load( lpszFileName ) )
		return FALSE;

	ZeroMemory( m_nProcessCount, sizeof( int ) * 25 );
	ZeroMemory( GuildCombatProcess, sizeof( __GuildCombatProcess ) * 250 );
	int		nCount;
	int		nAutoOpenCnt = 0;

	while( s.tok != FINISHED )
	{
		nCount = -1;

#if defined(__MAINSERVER)
		if( s.Token == _T( "AUTO_OPEN_IDC" ) )
		{
			s.GetToken();	// {
			s.GetToken();	// {
			
			int nDay, nTime1, nTime2;
			while( *s.token != '}' )
			{
				if( nAutoOpenCnt >= 7 )
				{
					Error( "CGuildCombat::LoadScript() 자동 시작 설정 갯수가 넘음!!" );
					return FALSE;
				}

				nDay = atoi( s.Token );
				m_nDay = nDay;
				if( nDay <= 0 || nDay > 7 )
				{
					Error( "CGuildCombat::LoadScript() Day Error = %d", nDay );
					return FALSE;
				}
				__AutoOpen[nDay-1].bUseing = TRUE;

				nTime1 = s.GetNumber();
				if( nTime1 < 0 || nTime1 > 24 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime1 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nHour = nTime1;
				
				nTime2 = s.GetNumber();
				if( nTime2 < 0 || nTime2 > 59 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime2 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nMinute = nTime2;

				nAutoOpenCnt++;

				s.GetToken();
			}		
		}
#else
		if( s.Token == _T( "AUTO_OPEN_TEST" ) )
		{
			s.GetToken();	// {
			s.GetToken();	// {
			
			int nDay, nTime1, nTime2;
			while( *s.token != '}' )
			{
				if( nAutoOpenCnt >= 7 )
				{
					Error( "CGuildCombat::LoadScript() 자동 시작 설정 갯수가 넘음!!" );
					return FALSE;
				}
				
				nDay = atoi( s.Token );
				m_nDay = nDay;
				if( nDay <= 0 || nDay > 7 )
				{
					Error( "CGuildCombat::LoadScript() Day Error = %d", nDay );
					return FALSE;
				}
				__AutoOpen[nDay-1].bUseing = TRUE;
				
				nTime1 = s.GetNumber();
				if( nTime1 < 0 || nTime1 > 24 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime1 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nHour = nTime1;
				
				nTime2 = s.GetNumber();
				if( nTime2 < 0 || nTime2 > 59 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime2 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nMinute = nTime2;
				
				nAutoOpenCnt++;
				
				s.GetToken();
			}		
		}
#endif
		else if( s.Token == _T( "JOINPENYA" ) )
		{
			m_nJoinPanya = s.GetNumber();
		}
		else if( s.Token == _T( "GUILDLEVEL" ) )
		{
			m_nGuildLevel = s.GetNumber();
		}
		else if( s.Token == _T( "MINJOINGUILDSIZE" ) )
		{
			m_nMinGuild = s.GetNumber();
		}
		else if( s.Token == _T( "MAXJOINGUILDSIZE" ) )
		{
			m_nMaxGuild = s.GetNumber();
		}
		else if( s.Token == _T( "MAXPLAYERLIFE" ) )
		{
			m_nMaxPlayerLife = s.GetNumber();
		}
		else if( s.Token == _T( "MAXWARPLAYER" ) )
		{
			m_nMaxWarPlayer = s.GetNumber();
		}
		else if( s.Token == _T( "MAXMAPTIME" ) )
		{
			m_nMaxMapTime = s.GetNumber();
		}
		else if( s.Token == _T( "MAXJOINMEMBERSIZE" ) )
		{
			m_nMaxJoinMember = s.GetNumber();
		}
		else if( s.Token == _T( "MAXGUILDPERCENT" ) )
		{
			m_nMaxGuildPercent = s.GetNumber();
		}
		else if( s.Token == _T( "MAXPLAYERPERCENT" ) )
		{
			m_nMaxPlayerPercent = s.GetNumber();
		}
		else if( s.Token == _T( "REQUESTCANCLEPERCENT" ) )
		{
			m_nRequestCanclePercent = s.GetNumber();
		}
		else if( s.Token == _T( "NOTREQUESTPERCENT" ) )
		{
			m_nNotRequestPercent = s.GetNumber();
		}
		else if( s.Token == _T( "ITEMPENYA" ) )
		{
			m_nItemPenya = s.GetNumber();
		}
		else if( s.Token == _T( "MAX_GCSIENDITEM" ) )
		{
			m_nMaxGCSendItem = s.GetNumber();
		}
		else if( s.Token == _T( "GCSENDITEM" ) )
		{
			int nWinCount = s.GetNumber();
			s.GetToken();
			DWORD dwItemId = CScript::GetDefineNum( s.Token );
			if( dwItemId == -1 )
			{
				Error( "CGuildCombat::LoadScript() 아이템이 없는것을 설정했음" );
				return FALSE;
			}
			int nItemNum = s.GetNumber();
			__GCSENDITEM GCSendItem{
				.nWinCount = nWinCount,
				.dwItemId = dwItemId,
				.nItemNum = nItemNum
			};
			vecGCSendItem.push_back( GCSendItem );
		}
		else if( s.Token == _T( "OPEN" ) )
		{
			nCount = 0;
		}
		else if( s.Token == _T( "MAINTENANCE" ) )
		{
			nCount = 1;
		}
		else if( s.Token == _T( "WAR" ) )
		{
			nCount = 2;
		}

		if( nCount != -1 )
		{
			m_nProcessCount[nCount] = s.GetNumber();
			s.GetToken();	// {
			s.GetToken();	// {
			int nSoCount = 0;
			while( *s.token != '}' )
			{
				if( s.Token == _T( "ALLMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= ALLMSG;
				}
				else if( s.Token == _T( "GUILDMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= GUILDMSG;
				}
				else if( s.Token == _T( "JOINMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= JOINMSG;
				}
				else if( s.Token == _T( "WORLDMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WORLDMSG;
				}
				else if( s.Token == _T( "STATE" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= STATE;
				}
				else if( s.Token == _T( "WARSTATE" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WARSTATE;
				}
				else if( s.Token == _T( "WAIT" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WAIT;
				}
				GuildCombatProcess[25*nCount+nSoCount].dwTime	= s.GetNumber();
				GuildCombatProcess[25*nCount+nSoCount].dwCommand	= s.GetNumber();
				GuildCombatProcess[25*nCount+nSoCount].dwParam		= s.GetNumber();
				if( GuildCombatProcess[25*nCount+nSoCount].nState == STATE )
					m_nProcessCount[GuildCombatProcess[25*nCount+nSoCount].dwCommand] = 25*nCount+nSoCount;
				
				++nSoCount;
				s.GetToken();
			}
		}

		s.GetToken();
	}	
	return TRUE;
}

void CGuildCombat::SendGCLog( void )
{
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__GuildCombatMember* pGCMember = vecRequestRanking[nVeci];

			for (const __JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
				CUser * pUser = prj.GetUserByID(pJoinPlayer->uidPlayer);
				if (IsValidObj(pUser))
					pUser->AddGCLog();
			}

	}
}

void CGuildCombat::SendJoinMsg( LPCTSTR lpszString  )
{

}

void CGuildCombat::SendGuildCombatEnterTime( void )
{
	g_UserMng.AddGuildCombatEnterTime( GuildCombatProcess[m_nProcessGo].dwTime );
}

void CGuildCombat::SetPlayerChange( CUser* pUser, CUser* pLeader )
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if (!pGCMember) return;

	__JOINPLAYER * pJoinPlayer = pGCMember->FindByPlayerId(pUser->m_idPlayer);
	if (!pJoinPlayer) return;

	// 대기열에 존재 하는  pUser는 전투와 무관하므로 무시
	if (pGCMember->IsInFifo(pJoinPlayer)) return;

	pJoinPlayer->dwTelTime	= 0;
	pUser->m_nGuildCombatState	= 0;
	g_UserMng.AddGuildCombatUserState( pUser );
	--pGCMember->nWarCount;
	pJoinPlayer->nlife--;
	if( pJoinPlayer->nlife > 0 )
		pGCMember->lspFifo.push_back( pJoinPlayer );
	if( pJoinPlayer->nlife < 0 )
		pJoinPlayer->nlife	= 0;

	RefreshFifo(*pGCMember);
}

void CGuildCombat::RefreshFifo(__GuildCombatMember & gcMember) const {
	while (!gcMember.lspFifo.empty() && gcMember.nWarCount < m_nMaxWarPlayer) {
		__JOINPLAYER * pJoinPlayer = gcMember.lspFifo.front();
		
		CUser * pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
		if (IsValidObj(pMover) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR) {
			gcMember.nWarCount++;
			pJoinPlayer->dwTelTime = timeGetTime();
			pMover->AddGCJoinWarWindow(pJoinPlayer->nMap, m_nMaxMapTime);
			gcMember.lspFifo.pop_front();
		} else {
			gcMember.lspFifo.pop_front();
			pJoinPlayer->nlife--;
			if (pJoinPlayer->nlife > 0)
				gcMember.lspFifo.push_back(pJoinPlayer);
			if (pJoinPlayer->nlife < 0)
				pJoinPlayer->nlife = 0;
		}
	}
}

void CGuildCombat::GetPoint( CUser* pAttacker, CUser* pDefender )
{
	// 선택한 넘들 가지고 오기
	CGuild * pGuild = g_GuildMng.GetGuild( pAttacker->m_idGuild );
	const BOOL bMaster = (pGuild && pGuild->IsMaster(pAttacker->m_idPlayer)) ? TRUE : FALSE;

	__GuildCombatMember * pGCMemberAttacker = FindGuildCombatMember( pAttacker->m_idGuild );
	if (!pGCMemberAttacker) return;
	
	__JOINPLAYER * pJoinPlayerAttacker = pGCMemberAttacker->FindByPlayerId(pAttacker->m_idPlayer);
	if (!pJoinPlayerAttacker) return;

	const BOOL bLastLife = pJoinPlayerAttacker->nlife == 1 ? TRUE : FALSE;

	__GuildCombatMember* pGCMember = FindGuildCombatMember( pDefender->m_idGuild );
	if (!pGCMember) return;
	
	__JOINPLAYER * pJoinPlayerDefender = pGCMember->FindByPlayerId(pDefender->m_idPlayer);
	if (!pJoinPlayerDefender) return;

	const BOOL bDefender = (pDefender->m_idPlayer == pGCMember->m_uidDefender) ? TRUE : FALSE;

	int nGetPoint = 2;

	// 길드마스터가 적 길드원을 Kill 했을 경우
	if( bMaster )
		++nGetPoint;
	// 디펜더를 Kill 했을 경우
	if( bDefender )
		++nGetPoint;
	// 부활포인트 0일 때 Kill 했을 경우
	if( bLastLife )
		++nGetPoint;

	pJoinPlayerAttacker->nPoint += nGetPoint;
	pGCMemberAttacker->nGuildPoint += nGetPoint;
	
	CGuild* pGuildMsg = pAttacker->GetGuild();
	if( pGuildMsg )
	{
		for (CGuildMember * pMember : pGuildMsg->m_mapPMember | std::views::values) {
			CUser * pUsertmp	= prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj( pUsertmp ) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			{
				if( bMaster )
					pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_MASTER, "%d", nGetPoint );
				else 
					pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_GENERAL, "%d", nGetPoint );
			}
		}
	}

	if( bDefender )
	{
		CGuild * pGuildMsg = pDefender->GetGuild();
		if( pGuildMsg )
		{
			for (CGuildMember * pMember : pGuildMsg->m_mapPMember | std::views::values) {
				CUser * pUsertmp	= prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
				{
					if( pGuild )
						pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_DEFENDER, "\"%s\"", pGuild->m_szGuild );	
				}
			}
		}
	}
	AddvecGCGetPoint(__GCGETPOINT{
			.uidGuildAttack = pAttacker->m_idGuild,
			.uidGuildDefence = pDefender->m_idGuild,
			.uidPlayerAttack = pAttacker->m_idPlayer,
			.uidPlayerDefence = pDefender->m_idPlayer,
			.nPoint = nGetPoint,
			.bMaster = bMaster,
			.bDefender = bDefender,
			.bLastLife = bLastLife
		});
	g_UserMng.AddGCGuildPrecedence();
	g_UserMng.AddGCPlayerPrecedence();
}

void CGuildCombat::SerializeGCWarPlayerList( CAr & ar )
{
	if( m_nMaxGuild <= (int)( vecRequestRanking.size() ) )
		ar << m_nMaxGuild;
	else
		ar << (int)vecRequestRanking.size();

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__GuildCombatMember* pGCMember = vecRequestRanking[nVeci];

			ar << pGCMember->m_uidDefender;
			ar << (int)pGCMember->vecGCSelectMember.size();
			for (const __JOINPLAYER * pJoinPlayer1 : pGCMember->vecGCSelectMember) {

				ar << pJoinPlayer1->uidPlayer;
				if( 0 < pJoinPlayer1->nlife )
				{
					const bool bFind = pGCMember->IsInFifo(pJoinPlayer1);
				
					if( bFind )	// 대기중
					{
						ar << (int)0;
					}
					else // 전투자
					{
						ar << (int)1;
					}
				}
				else // 대기중
				{
					ar << (int)0;
				}
			}

	}
}

CGuildCombat::__JOINPLAYER * CGuildCombat::__GuildCombatMember::FindByPlayerId(u_long playerId) {
	const auto it = std::find_if(
		vecGCSelectMember.begin(),
		vecGCSelectMember.end(),
		[playerId](const __JOINPLAYER * pJoinPlayer) { return pJoinPlayer->uidPlayer == playerId; }
	);

	return it != vecGCSelectMember.end() ? *it : nullptr;
}

bool CGuildCombat::__GuildCombatMember::IsInFifo(const __JOINPLAYER * pJoinPlayer) const {
	return std::find(lspFifo.begin(), lspFifo.end(), pJoinPlayer) != lspFifo.end();
}

#endif	// __WORLDSERVER

CGuildCombat	g_GuildCombatMng;
