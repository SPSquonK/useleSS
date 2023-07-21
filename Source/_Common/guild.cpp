#include "stdafx.h"

#include "sqktd/algorithm.h"
#include <ranges>

#include "guild.h"
#ifdef __CORESERVER
#include "player.h"
#include "dpcachesrvr.h"
#include "DPCoreSrvr.h"
#endif	// __CORESERVER
#ifdef __WORLDSERVER
#include "DPDatabaseClient.h"
#include "DPCoreClient.h"
#include "user.h"
#include "worldmng.h"
#include "defineQuest.h"
#include "GroupedEmission.h"
#endif // __WORLDSERVER


#include "guildwar.h"

//________________________________________________________________________________
CAr&  operator<<(CAr& ar, const VOTE_INSERTED_INFO& info)
{
	ar << info.idGuild << info.idVote;
	ar.WriteString( info.szTitle );
	ar.WriteString( info.szQuestion );
	for( int i=0; i<4; ++i )
	{
		ar.WriteString( info.szSelections[i]);
	}			
	return ar;
}

CAr&  operator>>(CAr& ar, VOTE_INSERTED_INFO& info)
{
	ar >> info.idGuild >> info.idVote;
	ar.ReadString( info.szTitle, MAX_BYTE_VOTETITLE );
	ar.ReadString( info.szQuestion, MAX_BYTE_VOTEQUESTION );
	for( int i=0; i<4; ++i )
	{
		ar.ReadString( info.szSelections[i], MAX_BYTE_VOTESELECT );
	}			
	return ar;
}

//________________________________________________________________________________
CAr&  operator<<(CAr& ar, const CONTRIBUTION_CHANGED_INFO& info)
{
	ar << info.idGuild << info.idPlayer << info.dwPxpCount << info.dwPenya;
	ar << info.dwGuildPxpCount << info.dwGuildPenya << info.nGuildLevel;
	return ar;
}

CAr&  operator>>(CAr& ar, CONTRIBUTION_CHANGED_INFO& info)
{
	ar >> info.idGuild >> info.idPlayer >> info.dwPxpCount >> info.dwPenya;
	ar >> info.dwGuildPxpCount >> info.dwGuildPenya >> info.nGuildLevel;
	return ar;
}

//________________________________________________________________________________

CGuildVote::CGuildVote()
{
	m_idVote = 0;				
	m_bCompleted = false;		
	m_szTitle[0] = '\0';		
	m_szQuestion[0] = '\0';		
	
	for( int i=0; i<4; ++i )
	{
		m_selects[i].szString[0] = '\0';
		m_selects[i].cbCount = 0;
	}
}

// VOTE_INSERTED_INFO 구조체를 이용해서 초기화하기 
void CGuildVote::Init( const VOTE_INSERTED_INFO& info, bool bCompleted, BYTE* cbCounts )
{
	m_idVote = info.idVote;						
	m_bCompleted = bCompleted;							
	memcpy( m_szTitle, info.szTitle, MAX_BYTE_VOTETITLE );			
	memcpy( m_szQuestion, info.szQuestion, MAX_BYTE_VOTEQUESTION );
	for( int i=0; i<4; i++ )
	{
		if( cbCounts )
			m_selects[i].cbCount = cbCounts[i];
		else
			m_selects[i].cbCount = 0;
		memcpy( m_selects[i].szString, info.szSelections[i], MAX_BYTE_VOTESELECT );
	}
}

// 투표하기 
void CGuildVote::Cast( BYTE cbSelect )
{
	ASSERT( cbSelect < 4 );
	m_selects[cbSelect].cbCount++;
}

CAr & operator<<(CAr & ar, const CGuildVote & self) {
	ar << self.m_idVote << (BYTE)self.m_bCompleted;
	ar.WriteString(self.m_szTitle);
	ar.WriteString(self.m_szQuestion);

	for (int i = 0; i < 4; ++i) {
		ar.WriteString(self.m_selects[i].szString);
		ar << self.m_selects[i].cbCount;
	}

	return ar;
}

CAr & operator>>(CAr & ar, CGuildVote & self) {
	BYTE complete;
	ar >> self.m_idVote >> complete;
	ar.ReadString(self.m_szTitle, MAX_BYTE_VOTETITLE);
	ar.ReadString(self.m_szQuestion, MAX_BYTE_VOTEQUESTION);

	for (int i = 0; i < 4; ++i) {
		ar.ReadString(self.m_selects[i].szString, MAX_BYTE_VOTESELECT);
		ar >> self.m_selects[i].cbCount;
	}

	self.m_bCompleted = (complete == 1);

	return ar;
}

//________________________________________________________________________________
CGuildTable& CGuildTable::GetInstance()
{
	static CGuildTable table;
	return table;
}

// ar에 읽기, 쓰기 - database서버에서 core서버에 socket을 통해서 보내기 위해서 
CAr & operator<<(CAr & ar, const CGuildTable & self) {
	ar << self.m_nCount;
	for (int i = 0; i < self.m_nCount; i++) {
		ar << self.m_table[i];
	}

	return ar;
}

CAr & operator>>(CAr & ar, CGuildTable & self) {
	ar >> self.m_nCount;
	for (int i = 0; i < self.m_nCount; i++) {
		ar >> self.m_table[i];
	}

	return ar;
}

// 스크립트 파일에서 읽기 
BOOL CGuildTable::ReadBlock( CScript & script )
{
	script.GetToken(); // { 
	DWORD dwVal = script.GetNumber();

	int i = NULL;
	for( ; *script.token != '}' ; ++i )
	{
		m_table[ i ].dwPxpCount = dwVal;
		m_table[ i ].dwPenya    = script.GetNumber();
		m_table[ i ].nMaxMember = script.GetNumber();

		dwVal = script.GetNumber();
	}

	ASSERT( i <= MAX_GUILD_LEVEL );
	m_nCount = i;
	return TRUE;
}

// 레벨업에 필요한 PXP공헌횟수  
DWORD CGuildTable::GetPxpCount(WORD nLevel) const
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].dwPxpCount;
}

// 레벨업에 필요한 페냐 
DWORD CGuildTable::GetPenya(WORD nLevel) const 
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].dwPenya;
}

// 최대인원수 
WORD CGuildTable::GetMaxMemeber(WORD nLevel) const
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].nMaxMember;
}


//________________________________________________________________________________
CGuildMember::CGuildMember()
{
	m_idPlayer		= 0;
	m_nPay			= 0;
	m_nGiveGold		= 0;
	m_dwGivePxpCount= 0;
	m_nWin			= 0;
	m_nLose			= 0;
	*m_szAlias		= '\0';
	m_nMemberLv		= 0;
	m_idSelectedVote = 0; 
	m_nSurrender	= 0;
	m_nClass = 0;
}

CAr & operator<<(CAr & ar, const CGuildMember & self) {
	ar << self.m_idPlayer << self.m_nPay
		<< self.m_nGiveGold << self.m_dwGivePxpCount
		<< self.m_nWin << self.m_nLose << self.m_nMemberLv;
	ar << self.m_idSelectedVote;
	ar << self.m_nSurrender;
	ar << self.m_nClass;
	ar.WriteString(self.m_szAlias);

	return ar;
}

CAr & operator>>(CAr & ar, CGuildMember & self) {
	ar >> self.m_idPlayer >> self.m_nPay
		>> self.m_nGiveGold >> self.m_dwGivePxpCount
		>> self.m_nWin >> self.m_nLose >> self.m_nMemberLv;
	ar >> self.m_idSelectedVote;
	ar >> self.m_nSurrender;
	ar >> self.m_nClass;
	ar.ReadString(self.m_szAlias, MAX_GM_ALIAS);

	return ar;
}

//________________________________________________________________________________

int	CGuild::sm_anMaxMemberSize[MAX_GUILD_LEVEL]	=
	{	30, 30, 32, 32, 34, 34, 36, 36, 38, 38, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 80 };

int	CGuild::sm_anMaxMemberLvSize[MAX_GM_LEVEL]	=
	{	GM_MASTER, GM_KINGPIN, GM_CAPTAIN, GM_SUPPORTER, GM_ROOKIE	};

CGuild::CGuild()
{
	m_idGuild				= 0;
	*m_szGuild				= '\0';
	m_idMaster				= 0;
	m_nLevel				= 1;
	m_bActive				= FALSE;
	m_dwLogo				= 0;
	m_dwContributionPxp		= 0;
	*m_szNotice				= '\0';

#if !defined( __CORESERVER)	
	m_GuildBank.SetItemContainer(CItemContainer::ContainerTypes::GUILDBANK);
#endif
	
	m_aPower.fill(sqktd::EnumSet<GuildPower>());
	m_aPenya.fill(0);
	
	m_nGoldGuild = 0;

	m_bSendPay = FALSE;
	m_nWin	= 0;
	m_nLose	= 0;
	m_nSurrender	= 0;
	m_idEnemyGuild = 0;
	m_nWinPoint = 0;
	m_nDead		= 0;

#ifdef __CORESERVER
	m_bLog	= FALSE;
#endif	// __CORESERVER
}

CGuild::~CGuild()
{
	Clear();
}

void CGuild::Clear( void )
{
	CGuildMember* pMember;
	for( auto i	= m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		SAFE_DELETE( pMember );
	}
	m_mapPMember.clear();

	for (auto it = m_votes.begin() ;it != m_votes.end(); ++it )
		SAFE_DELETE( *it );
	m_votes.clear();
}

BOOL CGuild::AddMember( CGuildMember* pMember )
{
	if( GetMember( pMember->m_idPlayer ) )
		return FALSE;	// already exists
	m_mapPMember.emplace(pMember->m_idPlayer, pMember);
	return TRUE;	//
}

BOOL CGuild::RemoveMember( u_long idPlayer )
{
	CGuildMember* pMember	= GetMember( idPlayer );
	if( pMember )
	{
		safe_delete( pMember );
		m_mapPMember.erase( idPlayer );
		return TRUE;
	}
	return FALSE;
}

void CGuild::Serialize( CAr & ar, BOOL bDesc )
{
	if( ar.IsStoring() )
	{
		ar << m_idGuild << m_idMaster << m_nLevel;
		ar.WriteString( m_szGuild );
		ar << m_dwLogo;
		ar << m_nGoldGuild;
		ar << m_nWin << m_nLose << m_nSurrender;
		if( !bDesc )
		{
			ar << m_aPower << m_aPenya;
			ar << m_szNotice; // 공지사항 
			ar << m_dwContributionPxp;		// 공헌된 PXP
			ar << m_nLevel;					// 레벨 
			ar << m_idEnemyGuild;
			ar << (short)GetSize();
			for( auto i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
				ar << *( i->second );

			ar << (short)m_votes.size();
			for ( auto it = m_votes.begin(); it!=m_votes.end(); ++it )
				ar << **it;

			ar << m_quests.size();
			for (const std::pair<int, int> & p : m_quests) {
				ar << p.first << p.second;
			}
		}
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_idGuild >> m_idMaster >> m_nLevel;
		ar.ReadString( m_szGuild, MAX_G_NAME );
		ar >> m_dwLogo;
		ar >> m_nGoldGuild;
		ar >> m_nWin >> m_nLose >> m_nSurrender;
		if( !bDesc )
		{
			ar >> m_aPower >> m_aPenya;
			ar >> m_szNotice; // 공지사항 
			ar >> m_dwContributionPxp;		// 공헌된 PXP
			ar >> m_nLevel;					// 레벨 
			ar >> m_idEnemyGuild;

			short i, nSize	= 0;
			ar >> nSize;
			for( i = 0; i < nSize; i++ )
			{
				CGuildMember* pMember	= new CGuildMember;
				ar >> *pMember;
				m_mapPMember.emplace(pMember->m_idPlayer, pMember);
			}

			ar >> nSize;
			for( i = 0; i < nSize; i++ )
			{
				CGuildVote* pVote = new CGuildVote;
				ar >> *pVote;
				m_votes.push_back( pVote );
			}
			
			m_quests.clear();
			size_t questSize;
			ar >> questSize;
			for (size_t i = 0; i != questSize; ++i) {
				int a, b;
				ar >> a >> b;
				m_quests.emplace(a, b);
			}
		}
	}
}

CGuildMember* CGuild::GetMember( u_long idPlayer )
{
	const auto i	= m_mapPMember.find( idPlayer );
	if( i != m_mapPMember.end() )
		return i->second;
	return NULL;
}

int CGuild::GetMemberLvSize(const int nMemberLv) const {
	return static_cast<int>(std::ranges::count_if(
		m_mapPMember | std::views::values,
		[nMemberLv](const CGuildMember * const gm) { return gm->m_nMemberLv == nMemberLv; }
	));
}

int CGuild::GetMaxMemberLvSize( int nMemberLv )
{
	if( nMemberLv < 0 || nMemberLv >= MAX_GM_LEVEL )
		return 0;
	if( nMemberLv == GUD_ROOKIE )
		return GetMaxMemberSize();
	return( CGuild::sm_anMaxMemberLvSize[nMemberLv] );
}

int	CGuild::GetMaxMemberSize( void )
{
//	if( m_nLevel < 1 || m_nLevel > MAX_GUILD_LEVEL )
//		return 0;
//	return( CGuild::sm_anMaxMemberSize[m_nLevel-1] );
	return (int)CGuildTable::GetInstance().GetMaxMemeber( m_nLevel );
}

bool CGuild::SetLogo(const DWORD dwLogo) {
	if constexpr (!useless_params::CanChangeLogo) {
		if (m_dwLogo) return false;
	}

	m_dwLogo = dwLogo;
	return true;
}

void CGuild::SetContribution( CONTRIBUTION_CHANGED_INFO& info )
{
	m_nLevel			= info.nGuildLevel;
	m_nGoldGuild		= info.dwGuildPenya;
	m_dwContributionPxp	= info.dwGuildPxpCount;

	CGuildMember* pMember = GetMember( info.idPlayer );
	if( pMember == NULL )	// 멤버 탈퇴?
	{
		TRACE("CGuild::SetContribution - Guild[%d] idPlayer[%d] not found \n", m_idGuild, info.idPlayer );
		return;
	}
	pMember->m_dwGivePxpCount += info.dwPxpCount;
	pMember->m_nGiveGold += (int)info.dwPenya;
}

void CGuild::DecrementMemberContribution( u_long idPlayer, DWORD dwPenya, DWORD dwPxpCount )
{
	CGuildMember* pMember = GetMember( idPlayer );
	if( pMember == NULL )	// 멤버 탈퇴?
	{
		TRACE("CGuild::SetContribution - Guild[%d] idPlayer[%d] not found \n", m_idGuild, idPlayer );
		return;
	}
	pMember->m_dwGivePxpCount -= dwPxpCount;
	pMember->m_nGiveGold -= (int)dwPenya;
}

//공헌가능한가?
CONTRIBUTION_RESULT CGuild::CanContribute( DWORD dwPxp, DWORD dwPenya, u_long idPlayer )
{
	if( dwPxp > 0 && m_nLevel >= CGuildTable::GetInstance().GetMaxLevel() )  // pxp만 최대레벨에서 체크 
		return CONTRIBUTION_FAIL_MAXLEVEL;

	if( m_dwContributionPxp + dwPxp < m_dwContributionPxp )  
		return CONTRIBUTION_FAIL_GUILD_OVERFLOW_PXP;

	if( m_nGoldGuild + dwPenya < m_nGoldGuild )  
		return CONTRIBUTION_FAIL_GUILD_OVERFLOW_PENYA;
	
	CGuildMember* pMember = GetMember( idPlayer );
	if( pMember == NULL )
	{
		TRACE("CanContribute - Guild[%d] idPlayer[%d] not found \n", m_idGuild, idPlayer );
		return CONTRIBUTION_FAIL_INVALID_CONDITION;
	}

	if( pMember->m_dwGivePxpCount + dwPxp < pMember->m_dwGivePxpCount )  
		return CONTRIBUTION_FAIL_OVERFLOW_PXP;

	if( pMember->m_nGiveGold + (int)dwPenya < pMember->m_nGiveGold )  
		return CONTRIBUTION_FAIL_OVERFLOW_PENYA;

	return CONTRIBUTION_OK;
}

// 공헌하기 (CORE서버만 사용한다.)
BOOL CGuild::AddContribution( DWORD dwPxp, DWORD dwPenya, u_long idPlayer )
{
	if( CanContribute( dwPxp, dwPenya, idPlayer) != CONTRIBUTION_OK )
		return FALSE;

	CGuildMember* pMember = GetMember( idPlayer );
	pMember->m_dwGivePxpCount += dwPxp;
	pMember->m_nGiveGold += (int)dwPenya;

	m_dwContributionPxp += dwPxp;
	m_nGoldGuild += dwPenya;

	if( m_nLevel >= CGuildTable::GetInstance().GetMaxLevel() )
		return TRUE;
	
	DWORD dwMaxPxp = CGuildTable::GetInstance().GetPxpCount( m_nLevel + 1 );
	DWORD dwMaxPenya = CGuildTable::GetInstance().GetPenya( m_nLevel + 1);
	if( m_dwContributionPxp >= dwMaxPxp && m_nGoldGuild >= dwMaxPenya )
	{
		m_dwContributionPxp -= dwMaxPxp;
		m_nGoldGuild -= dwMaxPenya;
		++m_nLevel;  // LevelUP
	}

	return TRUE;
}

void CGuild::SetNotice( const char* szNotice )
{
	ASSERT( szNotice );
	strncpy(m_szNotice, szNotice, MAX_BYTE_NOTICE);
	m_szNotice[MAX_BYTE_NOTICE-1] = '\0';
}

// 투표삽입 
void CGuild::AddVote( const VOTE_INSERTED_INFO& info, bool bCompleted, BYTE* cbCounts )
{
	if( m_votes.size() >= MAX_VOTE_ENTRY )
	{
		safe_delete( m_votes.front() );
		m_votes.pop_front();
	}

	CGuildVote* pVote = new CGuildVote;
	pVote->Init( info, bCompleted, cbCounts );
	m_votes.push_back( pVote );
}

// 투표 찾기 
CGuildVote * CGuild::FindVote(const u_long idVote) {
	const auto it = std::ranges::find_if(m_votes,
		[idVote](const CGuildVote * const vote) { return vote->GetID() == idVote; }
	);
	return it != m_votes.end() ? *it : nullptr;
}

// idVote투표의 데이타를 변경하기 
bool CGuild::ModifyVote( u_long idVote, BYTE cbOperation, BYTE cbExtra ) {
	CGuildVote* pVote = FindVote( idVote );
	if( pVote == NULL )
		return false;

	switch( cbOperation )
	{
	case REMOVE_VOTE:
		//RemoveVote( idVote );
		{
			for (auto it = m_votes.begin() ; it != m_votes.end() ; ++it )
			{
				if ( (*it)->GetID() == idVote )
				{
					m_votes.erase( it );
					break;
				}
			}
		}
		break;
	case CLOSE_VOTE:
		pVote->SetComplete();
		break;
	case CAST_VOTE:
		pVote->Cast( cbExtra );
		break;
	default:
		ASSERT( 0 );
	}

	return true;
}

CGuildWar* CGuild::GetWar( void )
{
// must lock g_GuildMng
	return g_GuildWarMng.GetWar( m_idWar );
}

//________________________________________________________________________________
CGuildMng::CGuildMng()
{
	m_id	= 0;
#ifdef __WORLDSERVER
	m_bSendPay = FALSE;
#endif // __WORLDSERVER
}

CGuildMng::~CGuildMng()
{
	Clear();
}

void CGuildMng::Clear( void )
{
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	m_AddRemoveLock.Enter( theLineFile );	// lock1
#endif

	for( auto i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
		safe_delete( i->second );
	m_mapPGuild.clear();
	m_mapPGuild2.clear();

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
#endif
}

BOOL CGuildMng::SetName( CGuild* pGuild, const char* szName )
{
	CGuild* ptmp	= GetGuild( szName );
	if( ptmp )
		return FALSE;
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.erase( pGuild->m_szGuild );	// erase
	lstrcpy( pGuild->m_szGuild, szName );	// set
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.emplace(pGuild->m_szGuild, pGuild);	// insert
	return TRUE;
}

u_long CGuildMng::AddGuild( CGuild* pGuild )
{
//	locked
	m_id	= ( pGuild->m_idGuild != 0? pGuild->m_idGuild: m_id + 1 );
	if( GetGuild( m_id ) )
		return 0;
	pGuild->m_idGuild	= m_id;
	m_mapPGuild.emplace( m_id, pGuild );
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.emplace( pGuild->m_szGuild, pGuild );
	return m_id;
}

BOOL CGuildMng::RemoveGuild( u_long idGuild )
{
//	locked
	CGuild* pGuild	= GetGuild( idGuild );
	if( pGuild )
	{
		m_mapPGuild.erase( pGuild->m_idGuild );
		if( lstrlen( pGuild->m_szGuild ) > 0 )
			m_mapPGuild2.erase( pGuild->m_szGuild );
		safe_delete( pGuild );
		return TRUE;
	}
	return FALSE;
}

CGuild * CGuildMng::GetGuild(const u_long idGuild) {
	return sqktd::find_in_map(m_mapPGuild, idGuild, nullptr);
}

CGuild * CGuildMng::GetGuild(const char * const szGuild) {
	return sqktd::find_in_map(m_mapPGuild2, szGuild, nullptr);
}

void CGuildMng::Serialize( CAr & ar, BOOL bDesc )
{
	if( ar.IsStoring() )
	{
		ar << m_id;
		ar << GetSize();
		for( auto i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
			( i->second )->Serialize( ar, bDesc );
		
		// 길드랭크 정보 로드
		ar << CGuildRank::Instance;
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_id;
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CGuild* pGuild	= new CGuild;
			pGuild->Serialize( ar, bDesc );
			m_mapPGuild.emplace( pGuild->m_idGuild, pGuild );
			if( lstrlen( pGuild->m_szGuild ) > 0 )
				m_mapPGuild2.emplace( pGuild->m_szGuild, pGuild );
		}

		// 길드랭크 정보  쓰기
		ar >> CGuildRank::Instance;
	}
}

#ifdef __CORESERVER
void CGuildMng::AddConnection( CPlayer* pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	CGuild* pGuild	= GetGuild( pPlayer->m_idGuild );
	if( !pGuild || !pGuild->IsMember( pPlayer->uKey ) )
	{
		// 길드에 추가 되어 있지 않다
		pPlayer->m_idGuild	= 0;
	}
	else
	{
		if( pGuild->GetSize() > MAX_GM_SIZE )
			return;
		// 길드에 추가 되어 있음
		// 내가 들어왔으므로 길드원들에게 로그인 상태와 나의 멀티번호를 주어야 한다
		// 나에게는 길드원들의 로그인 상태와 멀티번호를 주어야 한다.
		int nMaxLogin = 0;
		u_long uLoginPlayerId[MAX_GM_SIZE];
		u_long uLoginGuildMulti[MAX_GM_SIZE];
		CGuildMember* pMember;
		CPlayer* pSendPlayer;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pSendPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
			if( pSendPlayer )
			{
				uLoginPlayerId[nMaxLogin]	= pSendPlayer->uKey;
				uLoginGuildMulti[nMaxLogin]	= pSendPlayer->m_uIdofMulti;
				++nMaxLogin;
				if( pMember->m_idPlayer != pPlayer->uKey )
					g_DPCacheSrvr.SendGuildMemberLogin( pSendPlayer, 1, pPlayer->uKey, pPlayer->m_uIdofMulti );
			}
		}
		if( nMaxLogin > 0 )
			g_DPCacheSrvr.SendGuildMemberGameJoin( pPlayer, nMaxLogin, uLoginPlayerId, uLoginGuildMulti );
	}
}

void CGuildMng::RemoveConnection( CPlayer* pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	CGuild* pGuild	= GetGuild( pPlayer->m_idGuild );
	if( pGuild )
	{
		// 내가 나가므로 길드원들에게 로그아웃 상태
		CGuildMember* pMember;
		CPlayer* pSendPlayer;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			if( pMember->m_idPlayer != pPlayer->uKey )
			{
				pSendPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
				// 클라들에게 통보
				if( pSendPlayer )
					g_DPCacheSrvr.SendGuildMemberLogin( pSendPlayer, 0, pPlayer->uKey, 100 );
			}
		}
		g_dpCoreSrvr.SendGuildMemberLogOut( pGuild->m_idGuild, pPlayer->uKey );
	}
}
#endif	// __CORESERVER

void CGuild::SetQuest( int nQuestId, int nState ) {
	const auto it = m_quests.find(nQuestId);

	if (it != m_quests.end()) {
		it->second = nState;
	} else {
		m_quests[nQuestId] = nState;
	}

#ifdef __WORLDSERVER
	SendSnapshotNoTarget<SNAPSHOTTYPE_SETGUILDQUEST, int, int>(nQuestId, nState);
#endif	// __WORLDSERVER
}

BOOL CGuild::RemoveQuest( int nQuestId )
{
	const auto it = m_quests.find(nQuestId);
	if (it == m_quests.end()) return FALSE;

	m_quests.erase(it);
#ifdef __WORLDSERVER
	SendSnapshotNoTarget<SNAPSHOTTYPE_REMOVEGUILDQUEST, int>(nQuestId);
#endif	// __WORLDSERVER
	return TRUE;
}

std::optional<int> CGuild::GetStateOfQuest( int nQuestId ) const {
	const auto itQuest = m_quests.find(nQuestId);
	if (itQuest == m_quests.end()) return std::nullopt;
	return itQuest->second;
}

#ifdef __WORLDSERVER
void CGuild::ReplaceLodestar( const CRect &rect )
{
// locked
	CUser* pUser;
	CGuildMember* pMember;
	for( auto i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				POINT point	= { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
				if( rect.PtInRect( point ) )
				{
					const REGIONELEM * pRgnElem = g_WorldMng.GetRevival(*pWorld, pUser->GetPos(), true);

					if( pRgnElem )
						pUser->Replace( *pRgnElem, REPLACE_NORMAL, nRevivalLayer );
				}
			}
		}
	}
}

void CGuild::Replace( DWORD dwWorldId, D3DXVECTOR3 & vPos, BOOL bMasterAround )
{
//	locked
	CUser* pMaster	= NULL;
	if( bMasterAround )
	{
		pMaster	= (CUser*)prj.GetUserByID( m_idMaster );
		if( IsInvalidObj( (CObj*)pMaster ) )
			return;
	}

	for (CGuildMember * pMember : m_mapPMember | std::views::values) {

		CUser * pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			if( pMaster && !pMaster->IsNearPC( pUser->GetId() ) )
				continue;

			if( GetStateOfQuest( QUEST_WARMON_LV1 ) ) // 클락워크 퀘스트면 비행 해제
				pUser->UnequipRide();
			pUser->Replace( dwWorldId, vPos, REPLACE_NORMAL, nTempLayer );
		}
	}
}
#endif	// __WORLDSERVER

static CGuildRank g_sGuildRank;
CGuildRank & CGuildRank::Instance = g_sGuildRank;


#ifdef __DBSERVER
/*
 *	TRANS 서버일때에만 Rank함수를 Call 할수 있다.
 */
BOOL CGuildRank::GetRanking(CQuery * pQuery, LPCTSTR p_strQuery) {
	m_Lock.Enter(theLineFile);

	m_UpdateTime = CTime::GetCurrentTime();

	m_Version++;
	m_Ranking.fill({});

	pQuery->Clear();

	for (int i = R1; i < RANK_END; ++i) {
		sprintf(const_cast<char *>(p_strQuery), "USELESS_RANKING_DBF.dbo.RANKING_STR 'R%d','%02d'", i + 1, g_appInfo.dwSys);
		if (!pQuery->Exec(p_strQuery)) {
			Error("CDbManager::UpdateGuildRanking에서 (%s) 실패", p_strQuery);
			m_Lock.Leave(theLineFile);
			return FALSE;
		}

		while (pQuery->Fetch()) {
			GUILD_RANKING & ranking = m_Ranking[i].emplace_back();

			ranking.m_dwLogo = pQuery->GetInt("m_dwLogo");

			pQuery->GetStr("m_szGuild", ranking.m_szGuild);
			pQuery->GetStr("m_szName", ranking.m_szName);

			ranking.m_nWin = pQuery->GetInt("m_nWin");
			ranking.m_nLose = pQuery->GetInt("m_nLose");
			ranking.m_nSurrender = pQuery->GetInt("m_nSurrender");
			ranking.m_AvgLevel = pQuery->GetFloat("m_AvgLevel");
			ranking.m_nWinPoint = pQuery->GetInt("m_nWinPoint");
		}

		pQuery->Clear();
	}

	m_Lock.Leave(theLineFile);

	return TRUE;
}


BOOL CGuildRank::RankingDBUpdate(CQuery * pQuery, LPCTSTR p_strQuery) {
	m_Lock.Enter(theLineFile);

	pQuery->Clear();

	sprintf(const_cast<char *>(p_strQuery), "MAKE_RANKING_STR '%02d'", g_appInfo.dwSys);
	if (FALSE == pQuery->Exec(p_strQuery)) {
		Error("CDbManager::RankingDBUpdate (%s) 실패", p_strQuery);
		m_Lock.Leave(theLineFile);
		return FALSE;
	}

	m_Lock.Leave(theLineFile);

	return TRUE;
}
#endif//__DBSERVER

CAr & operator<<(CAr & ar, const CGuildRank & self) {
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CMclAutoLock lock(self.m_Lock);
#endif

	return ar << self.m_Version << self.m_Ranking;
}


CAr & operator>>(CAr & ar, CGuildRank & self) {
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CMclAutoLock lock(self.m_Lock);
#endif

	return ar >> self.m_Version >> self.m_Ranking;
}

#ifndef __CORESERVER
CItemElem* CGuild::GetItem( DWORD dwItemId, SERIALNUMBER iSerialNumber )
{
	int cbMax	= m_GuildBank.m_dwItemMax;
	for( int i = 0; i < cbMax ; i++ )
	{
		CItemElem* pItemElem	= m_GuildBank.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId && pItemElem->GetSerialNumber() == iSerialNumber )
			return pItemElem;
	}
	return NULL;
}

short CGuild::RemoveItem( DWORD dwId, short nItemNum )
{
	if( nItemNum < 0 )
		nItemNum	= 0;

	short nRemoved	= 0;
	CItemElem* pItemElem	= m_GuildBank.GetAtId( dwId );
	if( pItemElem )
	{
		if( pItemElem->m_nItemNum > nItemNum )
		{
			nRemoved	= nItemNum;
			pItemElem->m_nItemNum	-= nItemNum;
		}
		else
		{
			nRemoved	= pItemElem->m_nItemNum;
			m_GuildBank.RemoveAtId( dwId );
		}
	}
	return nRemoved;
}
#endif	// __CORESERVER

#ifdef __WORLDSERVER
void CGuildMng::Process( void )
{
	CTime timeCurr	= CTime::GetCurrentTime();
	int nHour = timeCurr.GetHour();
	int nMin = timeCurr.GetMinute();
	if( m_bSendPay == FALSE && nHour == 21 )
	{
		m_bSendPay = TRUE;
		CGuild * pGuild;
		DWORD dwPay;
		for( auto i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
		{
			pGuild = i->second;
			if( pGuild->m_bSendPay == FALSE )
			{
				dwPay = 0;		
				CGuildMember* pMember;
				for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
				{
					pMember		= i->second;
					dwPay += pGuild->m_aPenya[ pMember->m_nMemberLv ];
				}
				
				if( 0 < dwPay && dwPay <= pGuild->m_nGoldGuild )
				{
					pGuild->m_bSendPay = TRUE;
					pGuild->m_nGoldGuild -= dwPay;					
					g_dpDBClient.SendGuildGetPay( pGuild->GetGuildId(), pGuild->m_nGoldGuild, dwPay );
					g_DPCoreClient.SendGuildGetPay( pGuild->GetGuildId(), pGuild->m_nGoldGuild );
				}
			}
		}
	}
	else
	if( m_bSendPay && nHour == 22 )//1 )
	{
		CGuild * pGuild;
		for( auto i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
		{
			pGuild = i->second;
			pGuild->m_bSendPay = FALSE;
		}
		m_bSendPay = FALSE;
	}
}
#endif	// __WORLDSERVER


bool CGuild::IsValidName(const char * szGuild) {
	constexpr auto IsValidChar = [](char c) {
		return (c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z')
			|| (c >= '0' && c <= '9')
			|| c == '-'
			|| c == '_';
	};

	size_t nbChars = 0;

	while (szGuild[nbChars] != '\0') {
		if (!IsValidChar(szGuild[nbChars])) return false;

		++nbChars;
	}

	return nbChars >= 2 && nbChars <= 16;
}

CGuildMng	g_GuildMng;



