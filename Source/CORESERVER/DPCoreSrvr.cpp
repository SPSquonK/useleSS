#include "stdafx.h"
#include "dpcachesrvr.h"
#include "dpdatabaseclient.h"
#include "dpcoresrvr.h"
#include "msghdr.h"
#include "player.h"
#include "mytrace.h"
#include "misc.h"
#include "definetext.h"
#include "party.h"
#include "guild.h"
#include "guildwar.h"
#include "WantedList.h"
#include "Environment.h"
extern	DWORD				s_dwEvent;

#ifdef __EVENT0913
extern	BOOL	s_bEvent0913;
#endif	// __EVENT0913
#ifdef __EVENT1206
extern	BOOL	s_bEvent1206;
#endif	// __EVENT1206

#include "InstanceDungeonParty.h"

CDPCoreSrvr		g_dpCoreSrvr;
		
CDPCoreSrvr::CDPCoreSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_MYREG, &CDPCoreSrvr::OnAddConnection )
	ON_MSG( PACKETTYPE_RECHARGE_IDSTACK, &CDPCoreSrvr::OnRecharge )
	ON_MSG( PACKETTYPE_JOIN, &CDPCoreSrvr::OnJoin )
	ON_MSG( PACKETTYPE_WHISPER, &CDPCoreSrvr::OnWhisper );
	ON_MSG( PACKETTYPE_SAY, &CDPCoreSrvr::OnSay );
	ON_MSG( PACKETTYPE_MODIFYMODE, &CDPCoreSrvr::OnModifyMode );
	ON_MSG( PACKETTYPE_SHOUT, &CDPCoreSrvr::OnShout );
	ON_MSG( PACKETTYPE_GMSAY, &CDPCoreSrvr::OnGMSay );
	ON_MSG( PACKETTYPE_PLAYMUSIC, &CDPCoreSrvr::OnPlayMusic );
	ON_MSG( PACKETTYPE_PLAYSOUND, &CDPCoreSrvr::OnPlaySound );
	ON_MSG( PACKETTYPE_KILLPLAYER, &CDPCoreSrvr::OnKillPlayer );
	ON_MSG( PACKETTYPE_GETPLAYERADDR, &CDPCoreSrvr::OnGetPlayerAddr );
	ON_MSG( PACKETTYPE_GETPLAYERCOUNT, &CDPCoreSrvr::OnGetPlayerCount );
	ON_MSG( PACKETTYPE_GETCOREPLAYER, &CDPCoreSrvr::OnGetCorePlayer );
	ON_MSG( PACKETTYPE_SYSTEM, &CDPCoreSrvr::OnSystem );	
	ON_MSG( PACKETTYPE_CAPTION, &CDPCoreSrvr::OnCaption );	
	ON_MSG( PACKETTYPE_ADDPARTYEXP, &CDPCoreSrvr::OnAddPartyExp );
	ON_MSG( PACKETTYPE_REMOVEPARTYPOINT, &CDPCoreSrvr::OnRemovePartyPoint );
	ON_MSG( PACKETTYPE_FALLRAIN, &CDPCoreSrvr::OnFallRain );
	ON_MSG( PACKETTYPE_STOPRAIN, &CDPCoreSrvr::OnStopRain );
	ON_MSG( PACKETTYPE_PARTYCHAT, &CDPCoreSrvr::OnPartyChat );
	ON_MSG( PACKETTYPE_PARTYSKILLUSE, &CDPCoreSrvr::OnPartySkillUse );
	ON_MSG( PACKETTYPE_PARTYLEVEL, &CDPCoreSrvr::OnPartyLevel );

	ON_MSG( PACKETTYPE_LOADCONSTANT, &CDPCoreSrvr::OnLoadConstant );
	ON_MSG( PACKETTYPE_GAMERATE, &CDPCoreSrvr::OnGameRate );
	ON_MSG( PACKETTYPE_SETMONSTERRESPAWN, &CDPCoreSrvr::OnSetMonsterRespawn );

	ON_MSG( PACKETTYPE_CREATE_GUILD, &CDPCoreSrvr::OnCreateGuild );
	ON_MSG( PACKETTYPE_GUILD_CHAT, &CDPCoreSrvr::OnGuildChat );
	ON_MSG( PACKETTYPE_GUILD_MSG_CONTROL, &CDPCoreSrvr::OnGuildMsgControl );

	ON_MSG( PACKETTYPE_ADDFRIENDNAMEREQEST, &CDPCoreSrvr::OnAddFriendNameReqest );
	ON_MSG( PACKETTYPE_BLOCK, &CDPCoreSrvr::OnBlock );
	ON_MSG( PACKETTYPE_WC_GUILDLOGO, &CDPCoreSrvr::OnGuildLogo );				// 로고 변경 
	ON_MSG( PACKETTYPE_WC_GUILDCONTRIBUTION, &CDPCoreSrvr::OnGuildContribution );	// 공헌도 
	ON_MSG( PACKETTYPE_WC_GUILDNOTICE, &CDPCoreSrvr::OnGuildNotice );			// 공지사항  
	ON_MSG( PACKETTYPE_GUILD_DB_REALPENYA, &CDPCoreSrvr::OnGuildRealPenya );	// 월급을 주었다는것을 코어와 유저에게 알려줌
	ON_MSG( PACKETTYPE_WAR_DEAD, &CDPCoreSrvr::OnWarDead );
	ON_MSG( PACKETTYPE_WAR_MASTER_ABSENT, &CDPCoreSrvr::OnWarMasterAbsent );
	ON_MSG( PACKETTYPE_WAR_TIMEOUT, &CDPCoreSrvr::OnWarTimeout );
	ON_MSG( PACKETTYPE_WC_WANTED_GOLD, &CDPCoreSrvr::OnWCWantedGold );
	ON_MSG( PACKETTYPE_WC_WANTED_REWARD, &CDPCoreSrvr::OnWCWantedReward );
	ON_MSG( PACKETTYPE_SETPARTYDUEL, &CDPCoreSrvr::OnSetPartyDuel );
	ON_MSG( PACKETTYPE_QUERYSETGUILDNAME, &CDPCoreSrvr::OnQuerySetGuildName );
	ON_MSG( PACKETTYPE_SETSNOOP, &CDPCoreSrvr::OnSetSnoop );
	ON_MSG( PACKETTYPE_SETSNOOPGUILD, &CDPCoreSrvr::OnSetSnoopGuild );
	ON_MSG( PACKETTYPE_CHAT, &CDPCoreSrvr::OnChat );
	ON_MSG( PACKETTYPE_PING, &CDPCoreSrvr::OnPing );
	ON_MSG( PACKETTYPE_GUILDCOMBAT_STATE, &CDPCoreSrvr::OnGCState );
	ON_MSG( PACKETTYPE_REMOVEPARTY_GUILDCOMBAT, &CDPCoreSrvr::OnGCRemoveParty );
	ON_MSG( PACKETTYPE_ADDPARTY_GUILDCOMBAT, &CDPCoreSrvr::OnGCAddParty );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_CREATE, &CDPCoreSrvr::OnInstanceDungeonCreate );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DESTROY, &CDPCoreSrvr::OnInstanceDungeonDestroy );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, &CDPCoreSrvr::OnInstanceDungeonSetCoolTimeInfo );
#ifdef __QUIZ
	ON_MSG( PACKETTYPE_QUIZ_NOTICE, &CDPCoreSrvr::OnQuizSystemMessage );
#endif // __QUIZ
	m_nGCState = 1;
}

CDPCoreSrvr::~CDPCoreSrvr()
{

}

void CDPCoreSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPCoreSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static	DPID	dpid;
	static	DWORD	dwWorldID;
	static	D3DXVECTOR3		vPos;
	static	BYTE	lptmp[8];
	static	size_t	nSize	= sizeof(DPID);
	
	CAr ar( (LPBYTE)lpMsg + nSize + nSize, dwMsgSize - ( nSize + nSize ) );
	GETTYPE( ar );	// dw
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) 
	{
		( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + nSize ), ( dwMsgSize - nSize - nSize ) );
	}
	else {
		switch( dw )
		{
			case PACKETTYPE_BROADCAST:
				{
					Send( lpMsg, dwMsgSize, DPID_ALLPLAYERS );
					break;
				}
			default:
				TRACE( "Handler not found(%08x)\n", dw );
				break;
		}
	}
}

void CDPCoreSrvr::OnAddConnection( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	ULONG uWorldSrvr;
	ar >> uWorldSrvr;

	DWORD dwTime;
	ar >> dwTime;
	SendQueryTickCount( dwTime, dpid, g_GameTimer.GetCurrentTime() );

	CMclAutoLock Lock( m_AccessLock );

	auto i	= m_apSleepServer.find( uWorldSrvr );
	if( i != m_apSleepServer.end() )
	{
		CServerDesc* pServerDesc	= i->second;
		m_apSleepServer.erase( i );

		pServerDesc->SetKey( uWorldSrvr );
		GetPlayerAddr( dpid, pServerDesc->m_szAddr );

		
		bool bResult	= m_apServer.emplace(dpid, pServerDesc).second;
		ASSERT( bResult );
		m_multiIdToDpid[pServerDesc->GetIdofMulti()] = dpid;
		SendRecharge( (u_long)10240, dpid );

		BEFORESENDDUAL( ar, PACKETTYPE_LOAD_WORLD, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << *pServerDesc;
		ar << g_PartyMng;
		g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
		g_GuildMng.Serialize( ar, FALSE );
		ar << g_GuildWarMng;
		g_GuildMng.m_AddRemoveLock.Leave( theLineFile );

		ar << *CEnvironment::GetInstance();

#ifdef __EVENT0913
		ar << s_bEvent0913;
#endif	// __EVENT0913
#ifdef __EVENT1206
		ar << s_bEvent1206;
#endif	// __EVENT1206

		ar << s_dwEvent;

		SEND( ar, this, dpid );
		g_DPCacheSrvr.Send( lpBuf + sizeof(DPID), nBufSize - sizeof(DPID), DPID_ALLPLAYERS );

		g_MyTrace.Add( uWorldSrvr, FALSE, "World: %04lu ON", uWorldSrvr );

	#if defined(__INTERNALSERVER)
		SendCWWantedList( dpid );
	#endif

#ifdef __SERVERLIST0911
		g_dpDatabaseClient.SendServerEnable( uWorldSrvr, 1L );
#endif	// __SERVERLIST0911

		CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonAllInfo( dpid );
	}
	
	CHAR lpAddr[16];
	GetPlayerAddr( dpid, lpAddr );
}

void CDPCoreSrvr::OnRemoveConnection( DPID dpid )
{
	CMclAutoLock Lock( m_AccessLock );

	auto i = m_apServer.find(dpid);
	if (i == m_apServer.end()) return;

	CServerDesc * pServerDesc = i->second;

	const ULONG uWorldSrvr = pServerDesc->GetKey();

	m_apServer.erase(i);
	m_apSleepServer.emplace( uWorldSrvr, pServerDesc ).second;
	g_dpCoreSrvr.m_multiIdToDpid[pServerDesc->GetIdofMulti()] = DPID_UNKNOWN;
	g_MyTrace.Add( uWorldSrvr, TRUE, "World: %04lu OFF", uWorldSrvr );

#ifdef __SERVERLIST0911
	g_dpDatabaseClient.SendServerEnable( uWorldSrvr, 0L );
#endif	// __SERVERLIST0911

	CInstanceDungeonHelper::GetInstance()->DestroyAllDungeonByMultiKey( uWorldSrvr );
}

void CDPCoreSrvr::SendQueryTickCount( DWORD dwTime, DPID dpid, double dCurrentTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYTICKCOUNT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwTime << g_TickCount.GetTickCount() << dCurrentTime;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::SendRecharge( u_long uBlockSize, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RECHARGE_IDSTACK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << m_objmap.m_idStack.GetIdBlock( uBlockSize );	// idBase
	ar << uBlockSize;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnRecharge( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	u_long uBlockSize;
	ar >> uBlockSize;
	SendRecharge( uBlockSize, dpid );
}

void CDPCoreSrvr::OnJoin( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	u_long idPlayer;
	char szPlayer[MAX_PLAYER]	= { 0,};
	ar >> idPlayer;
	ar.ReadString( szPlayer, MAX_PLAYER );
	BOOL bOperator;
	ar >> bOperator;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( !pPlayer || lstrcmp( szPlayer, pPlayer->lpszPlayer ) != 0 )
	{
		SendDestroyPlayer( idPlayer, dpid );
		return;
	}
	if( bOperator )
		g_PlayerMng.AddItToSetofOperator( idPlayer );

	if( pPlayer )
	{
		CParty* pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
		if( pParty )
			g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );
	}
}

void CDPCoreSrvr::OnWhisper( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CHAR lpString[260], lpSnoopString[512];
	u_long idFrom, idTo;

	ar >> idFrom >> idTo;
	ar.ReadString( lpString, 260 );

	CPlayer* pFrom, *pTo;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pFrom	= g_PlayerMng.GetPlayer( idFrom );
	pTo	= g_PlayerMng.GetPlayer( idTo );

	if( pFrom && pTo )
	{
		g_DPCacheSrvr.SendWhisper( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pTo );		// 상대방에게 귓말 보내기
		g_DPCacheSrvr.SendWhisper( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pFrom );	// 나에게 보냄

		CPlayer* pSnoop;
		if( pFrom->m_idSnoop > 0 )
		{
			pSnoop	= g_PlayerMng.GetPlayer( pFrom->m_idSnoop );
			if( pSnoop )
			{
				sprintf( lpSnoopString, "[w][%s->%s] %s", pFrom->lpszPlayer, pTo->lpszPlayer, lpString );
				g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
			}
		}
		if( pTo->m_idSnoop > 0 )
		{
			pSnoop	= g_PlayerMng.GetPlayer( pTo->m_idSnoop );
			if( pSnoop )
			{
				sprintf( lpSnoopString, "[w][%s->%s] %s", pFrom->lpszPlayer, pTo->lpszPlayer, lpString );
				g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
			}
		}
	}
	else
	{
		// player not found
		g_DPCacheSrvr.SendSay( "", "", idFrom, idTo, "", pFrom, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

// Operator commands
void CDPCoreSrvr::OnSay( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CHAR	lpString[260];
	u_long idFrom, idTo;

	ar >> idFrom >> idTo;
	ar.ReadString( lpString, 260 );

	CPlayer* pFrom, *pTo;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pFrom	= g_PlayerMng.GetPlayer( idFrom );
	pTo	= g_PlayerMng.GetPlayer( idTo );

	if( pFrom && pTo )
	{
		g_DPCacheSrvr.SendSay( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pTo );		// 상대방에게 귓말 보내기
		g_DPCacheSrvr.SendSay( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pFrom );	// 나에게 보냄
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idFrom, idTo, "", pFrom, 1 ); // 플레이어가 접속중이지 않습니다. 
		// player not found
	}
}

void CDPCoreSrvr::OnModifyMode(CAr & ar, DPID, DPID, DPID, u_long) {
	const auto [dwMode, f, idFrom, idTo] = ar.Extract<DWORD, bool, u_long, u_long>();

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	CPlayer * pFrom = g_PlayerMng.GetPlayer(idFrom);
	CPlayer * pTo = g_PlayerMng.GetPlayer(idTo);

	if (pFrom && pTo) {
		SendPacketAbout<PACKETTYPE_MODIFYMODE, DWORD, bool, u_long>(*pTo, dwMode, f, idFrom);
	} else {
		g_DPCacheSrvr.SendSay("", "", idFrom, idTo, "", pFrom, 1); // 플레이어가 접속중이지 않습니다. 
	}
}

void CDPCoreSrvr::OnShout( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[1024];
	u_long idPlayer;
	ar >> idPlayer;
	ar.ReadString( lpString, 1024 );
	SendShout( idPlayer, lpString, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnGMSay( CAr& ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[260];

	DWORD	dwWorldID;
	u_long idPlayer;
	
	ar >> idPlayer;
	ar >> dwWorldID;
	ar.ReadString( lpString, 260 );

	CPlayer* pFrom, *pTo	= NULL;
	CMclAutoLock Lock( g_PlayerMng.m_AddRemoveLock );
	pFrom	= g_PlayerMng.GetPlayer( idPlayer );

	for( auto i = g_PlayerMng.m_set.begin(); i != g_PlayerMng.m_set.end(); ++i )
	{
		pTo	= g_PlayerMng.GetPlayer( u_long(*i) );
		
		if( pFrom && pTo )
		{
			g_DPCacheSrvr.SendGMSay( pFrom->lpszPlayer, pTo->lpszPlayer, lpString, pTo );
		}
	}
	if( pFrom && pTo )
		g_DPCacheSrvr.SendGMSay( "건의", pTo->lpszPlayer, lpString, pFrom );	// 나에게 보냄
}

void CDPCoreSrvr::OnPlayMusic( CAr & ar, DPID, DPID, DPID, u_long )
{
	DWORD dwWorldID;
	u_long idmusic;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	ar >> dwWorldID;
	ar >> idmusic;

	CMclAutoLock	Lock( m_AccessLock );

	const DPID targetWorldDPID = GetWorldSrvrDPID(uIdofMulti);
	if (targetWorldDPID != DPID_UNKNOWN) return;

	SendPlayMusic(idmusic, dwWorldID, targetWorldDPID);
}

void CDPCoreSrvr::OnPlaySound( CAr & ar, DPID, DPID, DPID, u_long )
{
	DWORD dwWorldID;
	u_long idsound;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	ar >> dwWorldID;
	ar >> idsound;

	CMclAutoLock	Lock( m_AccessLock );

	const DPID targetWorldDPID = GetWorldSrvrDPID(uIdofMulti);
	if (targetWorldDPID != DPID_UNKNOWN) return;

	SendPlaySound(idsound, dwWorldID, targetWorldDPID);
}

void CDPCoreSrvr::OnKillPlayer( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idOperator, idPlayer;
	ar >> idOperator >> idPlayer;

	CPlayer* pOperator, *pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	pPlayer		= g_PlayerMng.GetPlayer( idPlayer );
	if( pOperator && pPlayer )
	{
		g_DPCacheSrvr.SendKillPlayer( pPlayer );
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idOperator, idPlayer, "", pOperator, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

void CDPCoreSrvr::OnGetPlayerAddr( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long )
{
	u_long idOperator, idPlayer;
	ar >> idOperator >> idPlayer;

	CPlayer* pOperator, *pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	pPlayer		= g_PlayerMng.GetPlayer( idPlayer );
	if( pOperator && pPlayer )
	{
		g_DPCacheSrvr.SendGetPlayerAddr( pPlayer->lpszPlayer, pPlayer->lpAddr, pOperator );
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idOperator, idPlayer, "", pOperator, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

void CDPCoreSrvr::OnGetPlayerCount( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idOperator;
	ar >> idOperator;

	CPlayer* pOperator;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	if( pOperator )
	{
		u_short uCount	= (u_short)g_PlayerMng.GetCount();
		g_DPCacheSrvr.SendGetPlayerCount( uCount, pOperator );
	}
	else
	{
		// operator not found
	}
}

void CDPCoreSrvr::OnSystem( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[512];
	ar.ReadString( lpString, 512 );
	g_DPCacheSrvr.SendSystem( lpString );
}

void CDPCoreSrvr::OnCaption( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[256];
	DWORD dwWorldId;
	BOOL	bSmall;
	
	ar >> bSmall;
	ar >> dwWorldId;
	ar.ReadString( lpString, 256 );
	
	g_DPCacheSrvr.SendCaption( lpString, dwWorldId, bSmall );
}


void CDPCoreSrvr::OnGetCorePlayer( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idOperator;
	ar >> idOperator;

	CPlayer* pOperator;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	if( pOperator )
	{
		g_DPCacheSrvr.SendGetCorePlayer( pOperator );
	}
	else
	{
		// operator not found
	}
}

void CDPCoreSrvr::OnAddPartyExp( CAr & ar, DPID, DPID, DPID, u_long )
{
	const auto [uPartyId, nMonLv, bSuperLeader, bLeaderSMPartyExpUp] = ar.Extract<u_long, int, BOOL, BOOL>();
	
	CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );
	CParty* pParty	= g_PartyMng.GetParty( uPartyId );
	if (!pParty) return;

	constexpr auto CanLevelUp = [](CParty & party) {
		return party.m_nKindTroup != 0 || party.m_nLevel < MAX_PARTYLEVEL;
	};

	if (!CanLevelUp(*pParty)) return;

	int nAddExp = (int)((nMonLv / 25 + 1) * 10);
	nAddExp = (int)(nAddExp * s_fPartyExpRate);
	if (bSuperLeader) nAddExp *= 2;
	if (bLeaderSMPartyExpUp) nAddExp = (int)(nAddExp * 1.5);

	pParty->m_nExp += nAddExp;

	while (true) {
		if (!CanLevelUp(*pParty)) break;

		EXPPARTY expParty;
		if (pParty->m_nKindTroup == 0) {
			expParty = g_PartyMng.m_aExpParty[pParty->m_nLevel];
		} else {
			expParty.Exp = static_cast<int>(((50 + pParty->GetLevel()) * pParty->GetLevel() / 13) * 10);
			expParty.Point = 15;
		}

		if (pParty->m_nExp < expParty.Exp) {
			break;
		}

		pParty->m_nExp -= expParty.Exp;
		pParty->m_nPoint += expParty.Point;
		++pParty->m_nLevel;
	}

	{
		BEFORESENDDUAL(ar, PACKETTYPE_SETPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << uPartyId << pParty->m_nExp << pParty->m_nPoint << pParty->m_nLevel;
		SEND(ar, this, DPID_ALLPLAYERS);
	}
}

void CDPCoreSrvr::OnRemovePartyPoint( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uPartyId;
	int nRemovePoint;
	ar >> uPartyId;
	ar >> nRemovePoint;
	
	CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nPoint -= nRemovePoint;
		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYPOINT, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uPartyId << pParty->m_nPoint;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}


void CDPCoreSrvr::OnPartyLevel( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );

	u_long idParty, idPlayer;
	DWORD dwLevel, dwPoint, dwExp;
	ar >> idParty >> idPlayer >> dwLevel >> dwPoint >> dwExp;

	CParty* pParty = g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		pParty->m_nLevel = dwLevel;
		pParty->m_nPoint = dwPoint;				// 극단 레벨, 경험치, 포인트
		pParty->m_nExp = dwExp;

		if( pParty->m_nLevel < MAX_PARTYLEVEL )
		{
			if( (int)( g_PartyMng.m_aExpParty[pParty->m_nLevel].Exp ) <= pParty->m_nExp )
				pParty->m_nExp   = g_PartyMng.m_aExpParty[pParty->m_nLevel].Exp - 1;
		}
		else
		{
			if( int( ( ( 50 + pParty->GetLevel() ) * pParty->GetLevel() / 13 ) * 10 ) <= pParty->m_nExp )
				pParty->m_nExp = int( ( ( 50 + pParty->GetLevel() ) * pParty->GetLevel() / 13 ) * 10 ) - 1;
		}
		BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idParty << pParty->m_nExp << pParty->m_nPoint << pParty->m_nLevel;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

void CDPCoreSrvr::OnGameRate( CAr & ar, DPID, DPID, DPID, u_long )
{
	FLOAT fRate;
	BYTE nFlag;
	ar >> fRate;
	ar >> nFlag;

	{
		BEFORESENDDUAL( ar, PACKETTYPE_GAMERATE, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << fRate;
		ar << nFlag;
		SEND( ar, this, DPID_ALLPLAYERS );
	}

	g_DPCacheSrvr.SendGameRate( fRate, nFlag );	
}

void CDPCoreSrvr::OnLoadConstant( CAr & ar, DPID, DPID, DPID, u_long )
{
	BEFORESENDDUAL( ar1, PACKETTYPE_LOADCONSTANT, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar1, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnSetMonsterRespawn( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uidPlayer;
	DWORD dwMonsterID;
	DWORD dwRespawnNum;
	DWORD dwAttackNum;
	DWORD dwRect;
	DWORD dwRespawnTime;
	BOOL  bFlying;
	ar >> uidPlayer;
	ar >> dwMonsterID >> dwRespawnNum >> dwAttackNum >> dwRect >> dwRespawnTime;
	ar >> bFlying;
	
	{
		BEFORESENDDUAL( ar, PACKETTYPE_SETMONSTERRESPAWN, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uidPlayer;
		ar << dwMonsterID << dwRespawnNum << dwAttackNum << dwRect << dwRespawnTime;
		ar << bFlying;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}


void CDPCoreSrvr::OnPartySkillUse( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	u_long uidPlayer;
	int nMode;
	DWORD dwSkillTime;
	int nRemovePoint;
	int nCachMode;

	ar >> uidPlayer;
	ar >> nMode;
	ar >> dwSkillTime;
	ar >> nRemovePoint;
	ar >> nCachMode;

	CPlayer* pSender;
	
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pSender = g_PlayerMng.GetPlayer( uidPlayer );
	
	if( !pSender )
	{
		// 사용자 찾기 실패
		return;		
	}
	
	if( pSender->m_uPartyId != 0 )
	{
		CParty* pParty;
		
		CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );
		pParty	= g_PartyMng.GetParty( pSender->m_uPartyId );
		
		if( pParty )
		{
			if( pParty->m_nPoint < nRemovePoint)
				return;
			if( nCachMode == 0 && pParty->m_nModeTime[nMode] != 0 )
				return;

			pParty->m_nPoint -= nRemovePoint;
			if( nMode == PARTY_PARSKILL_MODE && dwSkillTime == 0 )
			{
			    pParty->SetPartyMode( nMode, dwSkillTime , 0 );	
				SendSetPartyMode( pParty->m_uPartyId, nMode, FALSE );
				return;
			}
            pParty->SetPartyMode( nMode, dwSkillTime , nCachMode );		
			SendSetPartyMode( pParty->m_uPartyId, nMode, TRUE, pParty->m_nPoint, (DWORD)pParty->GetPartyModeTime(nMode) );
		}
		else
		{
			// 극단이 없거나 극단에 소속되어 있지 않다.
		}
	}
	else
	{
		//극단에 소속되어 있지 않다
	}
}

void CDPCoreSrvr::OnPartyChat( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[260];

	u_long idPlayer;
		
	OBJID objid;
	ar >> objid;
	ar >> idPlayer;
	ar.ReadString( lpString, 260 );

	CPlayer* pSender;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pSender = g_PlayerMng.GetPlayer( idPlayer );

	if( !pSender )
	{
		// 사용자 찾기 실패
		return;		
	}

	if( pSender->m_uPartyId != 0 )
	{
		CParty* pParty;
		
		CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );
		pParty	= g_PartyMng.GetParty( pSender->m_uPartyId );
		
		if( pParty && pParty->IsMember( idPlayer ) )
		{
			SendPartyChat( pParty->m_uPartyId, pSender->lpszPlayer, lpString, objid );
		}
		else
		{
			// 극단이 없거나 극단에 소속되어 있지 않다.
		}
	}
	else
	{
		//극단에 소속되어 있지 않다
	}
}


void CDPCoreSrvr::OnFallRain( CAr & ar, DPID, DPID, DPID, u_long )
{
	CEnvironment::GetInstance()->m_Authority = TRUE;
	CEnvironment::GetInstance()->SetEnvironmentEffect( TRUE );
}

void CDPCoreSrvr::OnStopRain( CAr & ar, DPID, DPID, DPID, u_long )
{
	CEnvironment::GetInstance()->m_Authority = FALSE;
	CEnvironment::GetInstance()->SetEnvironmentEffect( FALSE );
	SendEnvironmentEffect();
}


void CDPCoreSrvr::SendShout( u_long idPlayer, const CHAR* lpString, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SHOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::SendPartyChat( u_long idParty, const CHAR* lpName, const CHAR* lpString, OBJID objid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << objid;
	ar << idParty;
	ar.WriteString( lpName );
	ar.WriteString( lpString );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPlayMusic( u_long idmusic, DWORD dwWorldID, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYMUSIC, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idmusic;
	ar << dwWorldID;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::SendPlaySound( u_long idsound, DWORD dwWorldID, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYSOUND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idsound;
	ar << dwWorldID;
	SEND( ar, this, dpid );
}
void CDPCoreSrvr::SendSetPartyMode( u_long idParty, int nMode, BOOL bOnOff, LONG nPoint ,DWORD dwSkillTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty << nMode << dwSkillTime << bOnOff;
	if( bOnOff == TRUE )
	{
		ar << nPoint;
	}
	SEND( ar, this, DPID_ALLPLAYERS );
}
void CDPCoreSrvr::SendRemoveParty( u_long idParty, u_long idLeader, u_long idMember )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYMEMBER_CoreWorld, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty << idLeader << idMember;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeTroup( u_long idParty, const char * szPartyName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANGETROUP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty;
	ar.WriteString( szPartyName );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeName( u_long idParty, const char * szPartyName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANPARTYNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty;
	ar.WriteString( szPartyName );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeExpMode( u_long idParty, const CParty::ShareExpMode nExpMode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHANGEEXPMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty << nExpMode;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeItemMode( u_long idParty, CParty::ShareItemMode nItemMode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHANGEITEMMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty << nItemMode;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddPartyMember(const u_long uPartyId, const u_long idLeader, const u_long idMember) {
	BEFORESENDDUAL(ar, PACKETTYPE_ADDPARTYMEMBER_CoreWorld, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uPartyId << idLeader << idMember;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPCoreSrvr::SendRemoveFriend( u_long uidSender, u_long uidFriend )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEFRIEND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidSender << uidFriend;
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::SendEnvironmentEffect()
{
	BEFORESENDDUAL( ar, PACKETTYPE_ENVIRONMENTALL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << *CEnvironment::GetInstance();
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::OnGuildMsgControl( CAr & ar, DPID, DPID, DPID, u_long )
{//*
	GUILD_MSG_HEADER	Header;

	ar.Read(&Header, sizeof(GUILD_MSG_HEADER));

	if ( Header.HeadBMain == GUILD_MSG_HEADER::GUILD_BANK )
	{
		DWORD		dwPenya = 0;
		BYTE		cbCloak;
//		CItemElem	ItemElem;

		if ( Header.HeadBSub & GUILD_MSG_HEADER::PENYA )
		{
			ar >> dwPenya;
			ar >> cbCloak;
			g_DPCacheSrvr.SendGuildMsgControl( &Header, dwPenya, cbCloak );
		}
	}//*/
}

void CDPCoreSrvr::OnCreateGuild( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nSize;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];
	char szGuild[MAX_G_NAME];

	ar >> nSize;
	if( nSize == 0 )
		return;
	ar.Read( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar.ReadString( szGuild, MAX_G_NAME );

	CPlayer* pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pPlayer		= g_PlayerMng.GetPlayer( info[0].idPlayer );	// master
	if( !pPlayer )	// Player X
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMember( pPlayer->uKey ) )
	{
		// guild exists
		g_DPCacheSrvr.SendDefinedText( TID_GAME_COMCREATECOM, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}
	pPlayer->m_idGuild	= 0;

	pGuild	= g_GuildMng.GetGuild( szGuild );
	if( pGuild )
	{
		// guild name exists
		g_DPCacheSrvr.SendDefinedText( TID_GAME_COMOVERLAPNAME, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

	pGuild	= new CGuild;
	lstrcpy( pGuild->m_szGuild, szGuild );
	pGuild->m_idMaster		= pPlayer->uKey;
	pGuild->m_aPower[ GUD_MASTER ].SetAll();
	u_long idGuild	= g_GuildMng.AddGuild( pGuild );
	if( idGuild > 0 )
	{
		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= pPlayer->uKey;
		pMember->m_nMemberLv	= GUD_MASTER;
		if( pGuild->AddMember( pMember ) )	// master
		{
			pPlayer->m_idGuild	= idGuild;
//			apPlayer[cb++]	= pPlayer;
			// member
			for( int i = 1; i < nSize; i++ )
			{
				pMember		= new CGuildMember;
				pMember->m_idPlayer		= info[i].idPlayer;
				pMember->m_nMemberLv	= GUD_ROOKIE;
				if( pGuild->AddMember( pMember ) )
				{
					CPlayer* pPlayertmp		= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
					if( pPlayertmp )
					{
						pPlayertmp->m_idGuild	= idGuild;
//						apPlayer[cb++]	= pPlayertmp;
					}
				}
				else
				{
					info[i].idPlayer	= 0;
					SAFE_DELETE( pMember );
				}
			}
			SendCreateGuild( info, nSize, idGuild, szGuild );
			g_dpDatabaseClient.SendCreateGuild( info, nSize, idGuild, szGuild );
		}
		else
		{
			SAFE_DELETE( pMember );
			g_GuildMng.RemoveGuild( idGuild );
		}
	}
	else
	{
		SAFE_DELETE( pGuild );
	}
//	for( int i = 0; i < cb; i++ )
//		g_DPCacheSrvr.SendGuild( pGuild, apPlayer[i] );
}

void CDPCoreSrvr::OnGuildChat( CAr & ar, DPID, DPID, DPID, u_long )
{
	char	lpSnoopString[2048];

	u_long idPlayer;
	OBJID objid;
	ar >> objid;
	ar >> idPlayer;
	char sChat[1024]	= { 0, };

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( pPlayer )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
		if( pGuild && pGuild->IsMember( pPlayer->uKey ) )
		{
			ar.ReadString( sChat, 1024 );

			CGuildMember* pMember;
			CPlayer* pPlayertmp;
			for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pPlayertmp	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
				if( pPlayertmp )
				{
					g_DPCacheSrvr.SendGuildChat( pPlayer->lpszPlayer, sChat, pPlayertmp, objid );

					if( pPlayertmp->m_idSnoop > 0 )
					{
						CPlayer* pSnoop		= g_PlayerMng.GetPlayer( pPlayertmp->m_idSnoop );
						if( pSnoop )
						{
							sprintf( lpSnoopString, "[g][%s-%s] %s", pPlayer->lpszPlayer, pPlayertmp->lpszPlayer, sChat );
							g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
						}
					}
				}
			}
			if( pGuild->m_bLog )
				g_dpDatabaseClient.SendSnoopGuild( pGuild->m_idGuild, idPlayer, sChat );
		}
	}
}

void CDPCoreSrvr::SendGuildAuthority(u_long uGuildId, const GuildPowerss & dwAuthority) {
	//SendGuildAuthority
	BEFORESENDDUAL(ar, PACKETTYPE_GUILD_AUTHORITY, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uGuildId << dwAuthority;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPCoreSrvr::SendGuildPenya( u_long uGuildId, DWORD dwType, DWORD dwPenya )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_PENYA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId;
	ar << dwType << dwPenya;	
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendChgMaster( u_long idPlayer, u_long idPlayer2, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHG_MASTER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idPlayer2 << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildSetName( u_long idGuild, const char* lpszGuild, u_long idPlayer, BYTE nId, BYTE nError )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_SETNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar.WriteString( lpszGuild );
	ar << idPlayer << nId << nError;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, u_long idGuild, const char* szGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CREATE_GUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nSize;
	ar.Write( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar << idGuild;
	ar.WriteString( szGuild );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendDestroyGuild( u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DESTROY_GUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddGuildMember( const GUILD_MEMBER_INFO & info, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_GUILD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &info, sizeof(GUILD_MEMBER_INFO) );
	ar << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendRemoveGuildMember( u_long idPlayer, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVE_GUILD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildMemberLv( u_long idPlayer, int nMemberLv, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MEMBER_LEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nMemberLv << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildClass( u_long idPlayer, int nClass, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_CLASS, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nClass << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildNickName( u_long idPlayer, LPCTSTR strNickName, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_NICKNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idGuild;
	ar.WriteString( strNickName );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildMemberLogOut( u_long idGuild, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MEMBERLOGOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPCoreSrvr::SendAcptWar(WarId idWar, u_long idDecl, u_long idAcpt )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ACPT_GUILD_WAR, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << idDecl << idAcpt;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendWarDead(WarId idWar, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_DEAD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << bDecl;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendWarEnd(WarId idWar, int nWptDecl, int nWptAcpt, int nType )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_END, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << nWptDecl << nWptAcpt << nType;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendSurrender( WarId idWar, u_long idPlayer, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SURRENDER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << idPlayer << bDecl;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnWarDead( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idPlayer;
	ar >> idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( !pPlayer )
		return;
	
	CGuildWar* pWar	= g_GuildWarMng.GetWar( pPlayer->m_idWar );
	if( !pWar )
		return;

	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;

	BOOL bDecl	= pWar->IsDecl( pPlayer->m_idGuild );
	if( pDecl->IsMaster( pPlayer->uKey ) || pAcpt->IsMaster( pPlayer->uKey ) )
	{
		g_GuildWarMng.Result( pWar, pDecl, pAcpt, (int)bDecl/*nType*/ );
	}
	else
	{
		if( bDecl )
		{
			pWar->m_Decl.nDead++;
			g_dpDatabaseClient.SendWarDead( pWar->m_idWar, pDecl->m_idGuild );	// trans
		}
		else
		{
			pWar->m_Acpt.nDead++;
			g_dpDatabaseClient.SendWarDead( pWar->m_idWar, pAcpt->m_idGuild );	// trans
		}

		SendWarDead( pWar->m_idWar, bDecl );	// worldserver
		for( auto i = pDecl->m_mapPMember.begin(); i != pDecl->m_mapPMember.end(); ++i )
		{
			CPlayer* pPlayertmp	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayertmp )
				g_DPCacheSrvr.SendWarDead( pWar->m_idWar, pPlayer->lpszPlayer, bDecl, pPlayertmp );
		}
		for( auto i	= pAcpt->m_mapPMember.begin(); i != pAcpt->m_mapPMember.end(); ++i )
		{
			CPlayer* pPlayertmp	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayertmp )
				g_DPCacheSrvr.SendWarDead( pWar->m_idWar, pPlayer->lpszPlayer, bDecl, pPlayertmp );
		}
	}
}

void CDPCoreSrvr::OnWarMasterAbsent( CAr & ar, DPID, DPID, DPID, u_long )
{
	WarId idWar;
	BOOL bDecl;
	ar >> idWar >> bDecl;
	
	CMclAutoLock	Lock( g_GuildMng.m_AddRemoveLock );
	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		if( bDecl )
		{
			pWar->m_Decl.nAbsent++;
			g_dpDatabaseClient.SendWarMasterAbsent( idWar, pWar->m_Decl.idGuild );
		}
		else
		{
			pWar->m_Acpt.nAbsent++;
			g_dpDatabaseClient.SendWarMasterAbsent( idWar, pWar->m_Acpt.idGuild );
		}
	}
}

void CDPCoreSrvr::OnWarTimeout( CAr & ar, DPID, DPID, DPID, u_long )
{
	WarId idWar;
	ar >> idWar;

	TRACE( "OnWarTimeout, %d\n", idWar );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
		return;

	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;

//	if( pWar )
	{
		if( pWar->m_Decl.nAbsent > pWar->m_Acpt.nAbsent )
		{
			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_AB );
//			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_GN );
		}
		else if( pWar->m_Decl.nAbsent < pWar->m_Acpt.nAbsent )
		{
			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_AB );
//			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_GN );
		}
		else
		{
			if( pWar->m_Decl.nDead > pWar->m_Acpt.nDead )
			{
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_DD );
//				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_GN );
			}
			else if( pWar->m_Decl.nDead < pWar->m_Acpt.nDead )
			{
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_DD );
//				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_GN );
			}
			else
			{
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DRAW );
			}
		}
	}
}

void CDPCoreSrvr::SendWarMasterAbsent(WarId idWar, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_MASTER_ABSENT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << bDecl;
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::SendSetFriendState( u_long uidPlayer, FriendStatus dwState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETFRIENDSTATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer << dwState;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendFriendInterceptState( u_long uIdPlayer, u_long uIdFriend )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FRIENDINTERCEPTSTATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uIdPlayer << uIdFriend;
	SEND( ar , this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendChangeLeader( u_long uPartyId, u_long uidChangeLeader )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHANGELEADER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyId << uidChangeLeader;
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::OnAddFriendNameReqest( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uLeader, uMember;
	LONG nLeaderJob;
	BYTE nLeaderSex;
	char szLeaderName[MAX_PLAYER] = {0,};
	char szMemberName[MAX_PLAYER] = {0,};
	ar >> uLeader >> uMember;
	ar >> nLeaderJob >> nLeaderSex;
	ar.ReadString( szLeaderName, MAX_PLAYER );
	ar.ReadString( szMemberName, MAX_PLAYER );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pLeader = g_PlayerMng.GetPlayer( uLeader );
	CPlayer* pMember = g_PlayerMng.GetPlayer( uMember );
	if( pLeader && pMember )
	{
		// 물어봄 친구등록을 할껀지
		g_DPCacheSrvr.SendAddFriend( uLeader, uMember, nLeaderJob, nLeaderSex, szLeaderName, pMember );
	}
	else
	{
		// 접속되어 있지 않습니다.
		if( pLeader )
			g_DPCacheSrvr.SendAddFriendNotFound( szMemberName, pLeader );
	}
}

void CDPCoreSrvr::OnBlock( CAr & ar, DPID, DPID, DPID, u_long )
{
	BYTE nGu;
	u_long uidPlayerTo, uidPlayerFrom;
	char szNameTo[MAX_PLAYER] = {0,};
	ar >> nGu;
	ar >> uidPlayerTo >> uidPlayerFrom;
	ar.ReadString( szNameTo, MAX_PLAYER );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer* pTo = g_PlayerMng.GetPlayer( uidPlayerFrom );
	if( pTo )
	{
		g_DPCacheSrvr.SendBlock( nGu, szNameTo, pTo );
	}
}


void CDPCoreSrvr::SendAddVoteResult( VOTE_INSERTED_INFO& info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_ADDVOTERESULT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << info;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddRemoveVote( u_long idGuild, u_long idVote )		
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_MODIFYVOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << idVote;
	ar << REMOVE_VOTE;
	ar << (BYTE)0;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddCloseVote( u_long idGuild, u_long idVote )		
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_MODIFYVOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << idVote;
	ar << CLOSE_VOTE;
	ar << (BYTE)0;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddCastVote( u_long idGuild, u_long idVote, BYTE cbSelection )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_MODIFYVOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << idVote;
	ar << CAST_VOTE;
	ar << (BYTE)cbSelection;
	SEND( ar, this, DPID_ALLPLAYERS );
}

// 길드로고 변경 
void CDPCoreSrvr::OnGuildLogo( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	u_long idGuild, idPlayer;
	DWORD dwLogo;

	ar >> idGuild >> idPlayer;
	ar >> dwLogo;
	
	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild == NULL )
		return;

	if( pGuild->m_nLevel < 2 )						// 2미만?
		return;

	if( pGuild->IsMaster( idPlayer ) == FALSE )		// 길마?
		return;

	if( pGuild->GetWar() )
	{
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
		if( pPlayer )
			g_DPCacheSrvr.SendDefinedText( TID_GAME_GUILDWARNOLOGO, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

	if( pGuild->SetLogo( dwLogo ) == FALSE )
		return;

	g_dpDatabaseClient.SendGuildLogo( idGuild, dwLogo );

	{
		BEFORESENDDUAL( ar, PACKETTYPE_CW_GUILDLOGO, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idGuild << dwLogo;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

// 길드공헌도
void CDPCoreSrvr::OnGuildContribution( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	ULONG uServerID;
	CONTRIBUTION_CHANGED_INFO info;

	ar >> uServerID;
	ar >> info;

	CGuild* pGuild = g_GuildMng.GetGuild( info.idGuild );
	if( pGuild == NULL )
		return;

	if( pGuild->m_szGuild[0] == '\0' )	// 이름이 없으면 공헌할 수 없다.
		return;
	
	pGuild->SetContribution( info );

	{
		BEFORESENDDUAL( ar, PACKETTYPE_CW_GUILDCONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uServerID;
		ar << info;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

// 길드 공지사항 변경 
void CDPCoreSrvr::OnGuildNotice( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	u_long idGuild, idPlayer;
	char szNotice[MAX_BYTE_NOTICE];
	ar >> idGuild >> idPlayer;
	ar.ReadString( szNotice, MAX_BYTE_NOTICE );
	
	g_dpDatabaseClient.SendGuildNotice( idGuild, szNotice );

	{
		BEFORESENDDUAL( ar, PACKETTYPE_CW_GUILDNOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idGuild;
		ar.WriteString(szNotice);
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

void CDPCoreSrvr::OnGuildRealPenya( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idGuild;
	int nGoldGuild;
	ar >> idGuild >> nGoldGuild;


	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->m_nGoldGuild = nGoldGuild;
	}
	
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILD_DB_REALPENYA, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idGuild << nGoldGuild;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}


void CDPCoreSrvr::UpdateWantedList()
{
#ifdef __INTERNALSERVER
	SendCWWantedList();
#endif
}

// 현상금을 누적 요청
void CDPCoreSrvr::OnWCWantedGold( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long	idPlayer;
	int		nGold;
	char    szPlayer[MAX_PLAYER];
	char    szMsg[WANTED_MSG_MAX + 1];

	ar.ReadString( szPlayer, MAX_PLAYER );
	ar >> idPlayer >> nGold;
	ar.ReadString( szMsg, WANTED_MSG_MAX + 1 );

	ASSERT( nGold > 0 );

	CWantedList& wantedList = CWantedList::GetInstance();
	if( wantedList.SetEntry( idPlayer, szPlayer, nGold, szMsg ) )
		UpdateWantedList();
}

// 코어서버에 (현상금얻기 & 현상금 clear) 요청  
void CDPCoreSrvr::OnWCWantedReward( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	u_long	idPlayer, idAttacker;
	DWORD	dwWorldID;		// 월드아이디 
	D3DXVECTOR3 vPos;		// 아이템을 떨굴 위치  
	char szFormat[256];
	char szReward[64];

	ar >> idPlayer >> idAttacker >> dwWorldID >> vPos;
	ar.ReadString( szFormat, 256 );
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	CWantedList& wantedList = CWantedList::GetInstance();
	BOOL bUpdatable;
	__int64 nGold = wantedList.GetReward( idPlayer, szReward, bUpdatable );
	
	if( nGold > 0 )
	{
		BEFORESENDDUAL( out, PACKETTYPE_CW_WANTED_REWARD, DPID_UNKNOWN, DPID_UNKNOWN );
		out << idPlayer << idAttacker << nGold << dwWorldID << vPos;
#ifdef __LAYER_1015
		out << nLayer;
#endif	// __LAYER_1015
		SEND( out, this, dpid );		

		char szMsg[1024];
		char szGold[64];
		sprintf( szGold, "%I64d", nGold );

		CString strGold = GetNumberFormatEx( szGold );
		//sprintf( szMsg, "%s님이 현상범 %s를 잡아 현상금 %s페냐를 얻었습니다.", szAttacker, szReward, strGold );
		sprintf( szMsg, szFormat, szReward, strGold );
		g_DPCacheSrvr.SendSystem( szMsg );

		if( bUpdatable )
			UpdateWantedList();
	}
}

// 현상금 리스트를 월드서버들에 보낸다.
void CDPCoreSrvr::SendCWWantedList( DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_WANTED_LIST, DPID_UNKNOWN, DPID_UNKNOWN );
	
	CWantedList& wantedList = CWantedList::GetInstance();
	wantedList.Write( ar );

	SEND( ar, this, dpid );		
}

void CDPCoreSrvr::OnSetPartyDuel( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idParty1, idParty2;
	BOOL bDuel;
	ar >> idParty1 >> idParty2 >> bDuel;
	
	CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );

	CParty* pParty	= g_PartyMng.GetParty( idParty1 );
	if( pParty )
		pParty->m_idDuelParty	= ( bDuel? idParty2: 0 );
	pParty	= g_PartyMng.GetParty( idParty2 );
	if( pParty )
		pParty->m_idDuelParty	= ( bDuel? idParty1: 0 );
}

void CDPCoreSrvr::SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwObjId, BOOL f )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << dwObjId << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendDestroyPlayer( u_long idPlayer, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DESTROY_PLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnQuerySetGuildName( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idPlayer;
	u_long idGuild;
	char lpszGuild[MAX_G_NAME];
	BYTE nId;

	ar >> idPlayer >> idGuild;
	ar.ReadString( lpszGuild, MAX_G_NAME );
	ar >> nId;

	BYTE err	= 0;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );

	if( !pPlayer )
		return;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->IsMaster( idPlayer )  )
		{
			if( g_GuildMng.SetName( pGuild, lpszGuild ) )
			{
				g_dpDatabaseClient.SendGuildSetName( idGuild, pGuild->m_szGuild );	// trans
				g_DPCacheSrvr.SendGuildSetName( idGuild, pGuild->m_szGuild );	// client
			}
			else
			{
				err		= 3;	// already exists
			}
		}
		else
		{
			err		= 2;	// is not kingpin
		}
	}
	else
	{
		err		= 1;		// guild not found
	}
	g_dpCoreSrvr.SendGuildSetName( idGuild, lpszGuild, idPlayer, nId, err );		// world
}


void CDPCoreSrvr::OnSetSnoop( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idPlayer, idSnoop;
	BOOL bRelease;

	ar >> idPlayer >> idSnoop >> bRelease;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( pPlayer )
	{
		OutputDebugString( "CORESERVER.EXE\t// PACKETTYPE_SETSNOOP" );
		pPlayer->m_idSnoop	= ( bRelease? 0: idSnoop );
		SendSetSnoop( idPlayer, idSnoop, bRelease );
	}
	else
	{
		CPlayer* pSnoop	= g_PlayerMng.GetPlayer( idSnoop );
		if( pSnoop )
			g_DPCacheSrvr.SendSay( "", "", idSnoop, idPlayer, "", pSnoop, 1 );
	}
}

void CDPCoreSrvr::OnChat( CAr & ar, DPID, DPID, DPID, u_long )
{
	char lpSnoopString[2048];

	u_long idPlayer1, idPlayer2;
	char lpszChat[1024]		= {  0, };
	
	ar >> idPlayer1 >> idPlayer2;
	ar.ReadString( lpszChat, 1024 );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer1	= g_PlayerMng.GetPlayer( idPlayer1 );
	CPlayer* pPlayer2	= g_PlayerMng.GetPlayer( idPlayer2 );
	if( pPlayer1 && pPlayer2 && pPlayer2->m_idSnoop > 0 )
	{
		CPlayer* pSnoop	= g_PlayerMng.GetPlayer( pPlayer2->m_idSnoop );
		if( pSnoop )
		{
			sprintf( lpSnoopString, "[c][%s->%s] %s", pPlayer1->lpszPlayer, pPlayer2->lpszPlayer, lpszChat );
			g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
		}
	}
}

void CDPCoreSrvr::OnGCState( CAr & ar, DPID, DPID, DPID, u_long )
{
	ar >> m_nGCState;
	SendGuildCombatState();
}
void CDPCoreSrvr::OnGCRemoveParty( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uPartyid;
	u_long idMember;
	
	ar >> uPartyid >> idMember;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );
	CParty* pParty	= g_PartyMng.GetParty( uPartyid );
	if( pParty )
	{
		if( pParty->DeleteMember( idMember ) )
		{
			g_dpCoreSrvr.SendRemoveParty( pParty->m_uPartyId, pParty->m_aMember[0].m_uPlayerId, idMember );
			
			if( pParty->GetSizeofMember() < 2 )
			{
				CPlayer* pMember		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( 0 ) );
				if( pMember )
					pMember->m_uPartyId	= 0;
				g_PartyMng.DeleteParty( pParty->m_uPartyId );
			}
		}
	}
}
void CDPCoreSrvr::OnGCAddParty( CAr & ar, DPID, DPID, DPID, u_long )
{
	// TODO: see wtf is going on here
	u_long idLeader, idMember;
	LONG nLeaderLevel, nMemberLevel, nLeaderJob, nMemberJob;
	DWORD dwLSex, dwMSex;
	ar >> idLeader >> nLeaderLevel >> nLeaderJob >> dwLSex;
	ar >> idMember >> nMemberLevel >> nMemberJob >> dwMSex;
	CPlayer* pLeader;
	CPlayer* pMember;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	pLeader = g_PlayerMng.GetPlayer( idLeader );
	pMember = g_PlayerMng.GetPlayer( idMember );

	if( !pLeader || !pMember )
	{
//		player not found
		return;
	}

	CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );

	CParty* pParty	= g_PartyMng.GetParty( pMember->m_uPartyId );	// member's party
	if( pParty && pParty->IsMember( pMember->uKey ) )
	{
		g_DPCacheSrvr.SendErrorParty( ERROR_DIFFRENTPARTY, pLeader );
		return;
	}

	pMember->m_uPartyId	= 0;
	pParty	= g_PartyMng.GetParty( pLeader->m_uPartyId );	// leader's party
	if( pParty && pParty->IsMember( pLeader->uKey ) )	// add
	{
		if( pParty->m_nSizeofMember >= MAX_PTMEMBER_SIZE )
		{
			g_DPCacheSrvr.SendErrorParty( ERROR_FULLPARTY, pLeader );
			return;
		}
		if( pParty->m_idDuelParty > 0 )
		{
			return;
		}
		if( FALSE == pParty->NewMember( idMember ) )
		{
			// error
		}
		else	// ok
		{
			pMember->m_uPartyId		= pParty->m_uPartyId;
			g_dpCoreSrvr.SendAddPartyMember(pParty->m_uPartyId, idLeader, idMember);
		}
	}
	else	// new
	{
		const u_long uPartyId = g_PartyMng.NewParty(idLeader, idMember);
		if( uPartyId != 0 )
		{
			pLeader->m_uPartyId		= pMember->m_uPartyId	= uPartyId;
			g_dpCoreSrvr.SendAddPartyMember(uPartyId, idLeader, idMember);
		}
		else
		{
//			error
		}
	}
}

void CDPCoreSrvr::SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idSnoop << bRelease;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnSetSnoopGuild( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idGuild;
	BOOL bRelease;

	ar >> idGuild >> bRelease;

	CMclAutoLock	Lock( g_GuildMng.m_AddRemoveLock );
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		OutputDebugString( "CORESERVER.EXE\t// PACKETTYPE_SETSNOOPGUILD" );
		pGuild->m_bLog	= !bRelease;
	}
}


void CDPCoreSrvr::SendRemoveUser( DWORD dwSerial )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UNIFY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwSerial;
	SEND( ar, this, DPID_ALLPLAYERS );
}


#ifdef __EVENT0913
void CDPCoreSrvr::SendEvent0913( BOOL f )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT0913, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif	// __EVENT0913
#ifdef __EVENT1206
void CDPCoreSrvr::SendEvent1206( BOOL f )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT1206, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif	// __EVENT1206

void CDPCoreSrvr::SendEvent( DWORD dwEvent )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwEvent;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildCombatState( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDCOMBAT_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << m_nGCState;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnPing( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	time_t tSend;
	ar >> tSend;

	{
		BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << tSend << time( NULL );
		SEND( ar, this, dpid );
	}
}

void CDPCoreSrvr::SendInstanceDungeonAllInfo( int nType, CInstanceDungeonBase* pID, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_ALLINFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType;
	pID->SerializeAllInfo( ar );
	SEND( ar, this, dpId );
}

void CDPCoreSrvr::OnInstanceDungeonCreate( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );

	ar >> nType >> dwDungeonId >> ID_Info;
	
	CInstanceDungeonHelper::GetInstance()->OnCreateDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreSrvr::SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId << ID_Info;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnInstanceDungeonDestroy( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );
	
	ar >> nType >> dwDungeonId >> ID_Info;
	
	CInstanceDungeonHelper::GetInstance()->OnDestroyDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreSrvr::SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_DESTROY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId << ID_Info;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnInstanceDungeonSetCoolTimeInfo( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nType;
	DWORD dwPlayerId;
	COOLTIME_INFO CT_Info;
	ULONG uKey;
	
	ar >> uKey >> nType >> dwPlayerId >> CT_Info;
	
	CInstanceDungeonHelper::GetInstance()->OnSetDungeonCoolTimeInfo( uKey, nType, CT_Info, dwPlayerId );
}

void CDPCoreSrvr::SendInstanceDungeonSetCoolTimeInfo( ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO CT_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uKey << nType << dwPlayerId << CT_Info;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendInstanceDungeonDeleteCoolTimeInfo( int nType, DWORD dwPlayerId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_DELETECOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwPlayerId;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#ifdef __QUIZ
void CDPCoreSrvr::OnQuizSystemMessage( CAr & ar, DPID, DPID, DPID, u_long )
{
	BOOL bAll;
	int nDefinedTextId, nChannel, nTime;
	ar >> nDefinedTextId >> bAll >> nChannel >> nTime;
	g_DPCacheSrvr.SendQuizSystemMessage( nDefinedTextId, bAll, nChannel, nTime );
}
#endif // __QUIZ
