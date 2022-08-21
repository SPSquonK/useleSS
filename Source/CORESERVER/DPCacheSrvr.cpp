#include "StdAfx.h"
#include "dpcoresrvr.h"
#include "dpdatabaseclient.h"
#include "dploginsrvr.h"
#include "dpcachesrvr.h"
#include "msghdr.h"
#include "mytrace.h"
#include "player.h"
#include "misc.h"
#include "party.h"
#include "definetext.h"
#include "guild.h"
#include "guildwar.h"
#include "lang.h"

// %Y%m%d%H%M%S

CDPCacheSrvr::CDPCacheSrvr() 
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_JOIN, &CDPCacheSrvr::OnAddPlayer );
	ON_MSG( PACKETTYPE_DESTROY_PLAYER, &CDPCacheSrvr::OnQueryRemovePlayer );
	ON_MSG(PACKETTYPE_ADDPARTYMEMBER_NeuzCore, &CDPCacheSrvr::OnAddPartyMember );
	ON_MSG(PACKETTYPE_REMOVEPARTYMEMBER_NeuzCore, &CDPCacheSrvr::OnRemovePartyMember );
	ON_MSG( PACKETTYPE_CHANGETROUP, &CDPCacheSrvr::OnPartyChangeTroup );
	ON_MSG( PACKETTYPE_CHANPARTYNAME, &CDPCacheSrvr::OnPartyChangeName );
	ON_MSG( PACKETTYPE_PARTYCHANGEITEMMODE, &CDPCacheSrvr::OnPartyChangeItemMode );
	ON_MSG( PACKETTYPE_PARTYCHANGEEXPMODE, &CDPCacheSrvr::OnPartyChangeExpMode );
	ON_MSG(PACKETTYPE_NC_ADDFRIEND, &CDPCacheSrvr::OnAddFriend );
	ON_MSG( PACKETTYPE_GETFRIENDSTATE, &CDPCacheSrvr::OnGetFriendState );
	ON_MSG( PACKETTYPE_SETFRIENDSTATE, &CDPCacheSrvr::OnSetFrinedState );
	ON_MSG( PACKETTYPE_FRIENDINTERCEPTSTATE, &CDPCacheSrvr::OnFriendInterceptState );
	ON_MSG( PACKETTYPE_REMOVEFRIEND, &CDPCacheSrvr::OnRemoveFriend );
	ON_MSG( PACKETTYPE_GUILD_SETNAME, &CDPCacheSrvr::OnGuildSetName );
	ON_MSG( PACKETTYPE_GUILD_PENYA, &CDPCacheSrvr::OnGuildPenya );
	ON_MSG( PACKETTYPE_GUILD_AUTHORITY, &CDPCacheSrvr::OnGuildAuthority );
	ON_MSG( PACKETTYPE_DESTROY_GUILD, &CDPCacheSrvr::OnDestroyGuild );
	ON_MSG( PACKETTYPE_ADD_GUILD_MEMBER, &CDPCacheSrvr::OnAddGuildMember );
	ON_MSG( PACKETTYPE_REMOVE_GUILD_MEMBER, &CDPCacheSrvr::OnRemoveGuildMember );
	ON_MSG( PACKETTYPE_GUILD_MEMBER_LEVEL, &CDPCacheSrvr::OnGuildMemberLv );
	ON_MSG( PACKETTYPE_GUILD_CLASS, &CDPCacheSrvr::OnGuildClass );
	ON_MSG( PACKETTYPE_GUILD_NICKNAME, &CDPCacheSrvr::OnGuildNickName );
	ON_MSG( PACKETTYPE_DECL_GUILD_WAR, &CDPCacheSrvr::OnDeclWar );
	ON_MSG( PACKETTYPE_ACPT_GUILD_WAR, &CDPCacheSrvr::OnAcptWar );
	ON_MSG( PACKETTYPE_SURRENDER, &CDPCacheSrvr::OnSurrender );
	ON_MSG( PACKETTYPE_QUERY_TRUCE, &CDPCacheSrvr::OnQueryTruce );
	ON_MSG( PACKETTYPE_ACPT_TRUCE, &CDPCacheSrvr::OnAcptTruce );
	ON_MSG( PACKETTYPE_NC_ADDVOTE, &CDPCacheSrvr::OnAddVote );
	ON_MSG( PACKETTYPE_NC_REMOVEVOTE, &CDPCacheSrvr::OnRemoveVote );
	ON_MSG( PACKETTYPE_NC_CLOSEVOTE, &CDPCacheSrvr::OnCloseVote );
	ON_MSG( PACKETTYPE_NC_CASTVOTE, &CDPCacheSrvr::OnCastVote );
	ON_MSG( PACKETTYPE_PARTYCHANGELEADER, &CDPCacheSrvr::OnPartyChangeLeader );
	ON_MSG( PACKETTYPE_SENDTAG, &CDPCacheSrvr::OnSendTag );
	ON_MSG( PACKETTYPE_CHG_MASTER, &CDPCacheSrvr::OnChgMaster );
}

void CDPCacheSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
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

void CDPCacheSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg + sizeof(DPID), dwMsgSize - sizeof(DPID) );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg, dwMsgSize - sizeof(DPID) - sizeof(DWORD) );
	} else {
		TRACE( "Handler not found(%08x)\n", lpMsg->dwType );
	}
}

void CDPCacheSrvr::SendProcServerList( DPID dpid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PROCSERVER_LIST, DPID_UNKNOWN );

	ar << static_cast<std::uint32_t>(g_dpCoreSrvr.m_apServer.size());
	for (const CServerDesc * serverDesc : g_dpCoreSrvr.m_apServer | std::views::values) {
		ar << *serverDesc;
	}

	SEND( ar, this, dpid );
}

void CDPCacheSrvr::OnAddConnection(DPID dpid) {
	if (!m_clientInfo) {
		m_clientInfo.emplace(dpid);

		SendProcServerList(dpid);

		GetPlayerAddr(dpid, m_clientInfo->ipv4Address);
		g_MyTrace.Add(CMyTrace::Key(m_clientInfo->ipv4Address), FALSE, "%s", m_clientInfo->ipv4Address);
	} else {
		char ipv4Addr[16];
		GetPlayerAddr(dpid, ipv4Addr);
		static_assert(std::same_as<DPID, unsigned long>);
		Error("Other Cache Connection - IP : %s ~ DPID : %lu", ipv4Addr, dpid);
	}
}

void CDPCacheSrvr::OnRemoveConnection(DPID dpid) {
	if (m_clientInfo && m_clientInfo->dpid == dpid) {
		ClientInfo old = m_clientInfo.value();
		m_clientInfo.reset();

		g_MyTrace.Add(CMyTrace::Key(old.ipv4Address), TRUE, "%s", old.ipv4Address);

		g_PlayerMng.RemoveCache(dpid);
	}
}

void CDPCacheSrvr::OnAddPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	char lpszPlayer[MAX_PLAYER];
	DWORD dwAuthKey;
	u_long idPlayer, dpidSocket;

	ar >> dwAuthKey;
	ar >> idPlayer;
	ar >> dpidSocket;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );	// m_ulong2
	if( pPlayer )
	{
		// 해당 소켓번호를 가진 플레이어를 찾아서 없앤다.
		// 그 플레이어는 CACHE서버나 LOGIN서버에 접속 감지가 되지 않는 상태
		CPlayer* p;
		p = g_PlayerMng.GetPlayerBySocket( dpidSocket );	// m_players
		if( p )
			g_PlayerMng.UnregisterPlayerInfo( p, FALSE );
		
		pPlayer->m_dwSerial = dpidUser;
		pPlayer->dpidCache	= dpidCache;
		pPlayer->dpidUser	= dpidSocket;
		ar >> pPlayer->m_uPartyId;

		g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
		ar >> pPlayer->m_idGuild;
		if( NULL == g_GuildMng.GetGuild( pPlayer->m_idGuild ) )
			pPlayer->m_idGuild	= 0;

		ar >> pPlayer->m_idWar;
		if( NULL == g_GuildWarMng.GetWar( pPlayer->m_idWar ) )
			pPlayer->m_idWar	= WarIdNone;

		g_GuildMng.m_AddRemoveLock.Leave( theLineFile );

		ar >> pPlayer->m_uIdofMulti;
		pPlayer->m_RTMessenger.Serialize( ar );
		ar.ReadString( lpszPlayer, MAX_PLAYER );
		ar.ReadString( pPlayer->lpAddr, 16 );

		g_PlayerMng.RegisterPlayerInfo( pPlayer );

		BEFORESENDSOLE( out, PACKETTYPE_JOIN, pPlayer->dpidUser );	// result
		out << pPlayer->m_dwSerial ;	// serial key로 구분하게 한다.
		out << (BYTE)1;					// 항상 성공 
		SEND( out, this, dpidCache );
	}
	else
	{
		WriteLog( "OnAddPlayer(): pPlayer is NULL, %d", idPlayer );
	}
}

void CDPCacheSrvr::OnQueryRemovePlayer( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	if( pPlayer )
	{
		if( *pPlayer->lpszAccount != '\0' )
			g_dpLoginSrvr.SendQueryRemovePlayer( pPlayer->lpszAccount );
		else
			WriteLog( "OnQueryRemovePlayer(): lpszAccount == '\0'" );
		g_PlayerMng.RemovePlayer( pPlayer );
	}
}

void CDPCacheSrvr::SendWhisper( const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch )
{
	if( !pTo )
		return;
//	ASSERT( pTo );

	BEFORESENDSOLE( ar, PACKETTYPE_WHISPER, pTo->dpidUser );
	ar.WriteString( sPlayerFrom );
	ar.WriteString( sPlayerTo );
	ar.WriteString( lpString );
	ar << idFrom << idTo;
	ar << nSearch;
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendSay( const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch )
{
	if( !pTo )
		return;
//	ASSERT( pTo );

	BEFORESENDSOLE( ar, PACKETTYPE_SAY, pTo->dpidUser );
	ar.WriteString( sPlayerFrom );
	ar.WriteString( sPlayerTo );
	ar.WriteString( lpString );
	ar << idFrom << idTo;
	ar << nSearch;
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendGMSay( const CHAR* sPlayerFrom, const CHAR* sPlayerTo, const CHAR* lpString, CPlayer* pTo )
{
	if( !pTo )
		return;
//	ASSERT( pTo );
	
	BEFORESENDSOLE( ar, PACKETTYPE_GMSAY, pTo->dpidUser );
	ar.WriteString( sPlayerFrom );
	ar.WriteString( sPlayerTo );
	ar.WriteString( lpString );
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendFriendState(CPlayer * pTo) {
	BEFORESENDSOLE(ar, PACKETTYPE_GETFRIENDSTATE, pTo->dpidUser);

	std::vector<u_long> vecIdFriend;
	pTo->Lock();
	auto range = pTo->m_RTMessenger | std::views::keys;
	vecIdFriend.assign(range.begin(), range.end());
	pTo->Unlock();

	ar << static_cast<std::uint32_t>(vecIdFriend.size());

	for (const u_long friendId : vecIdFriend) {
		CPlayer * pPlayer = g_PlayerMng.GetPlayer(friendId);
		if (!pPlayer) {
			ar << friendId << FriendStatus::OFFLINE;
			continue;
		}

		pPlayer->Lock();
		Friend * const pFriend = pPlayer->m_RTMessenger.GetFriend(pTo->uKey);
		FriendStatus dwState;
		if (pFriend && !pFriend->bBlock) {
			dwState = pPlayer->m_RTMessenger.GetState();
		} else {
			dwState = FriendStatus::OFFLINE;
		}
		pPlayer->Unlock();

		ar << friendId << dwState;
	}

	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendSetFriendState( CPlayer* pTo )
{
	std::vector< u_long > vecIdFriend;

	pTo->Lock();
	u_long idPlayer = pTo->uKey;
	FriendStatus dwState = pTo->m_RTMessenger.GetState();
	for (const auto & [friendId, friendState] : pTo->m_RTMessenger) {
		if( !friendState.bBlock )
			vecIdFriend.push_back( friendId );
	}

	BEFORESENDSOLE( ar, PACKETTYPE_SETFRIENDSTATE, pTo->dpidUser );
	ar << idPlayer;
	ar << dwState;
	SEND( ar, this, pTo->dpidCache );
	
	pTo->Unlock();

	for (const u_long friendId : vecIdFriend) {
		CPlayer* pPlayer = g_PlayerMng.GetPlayer(friendId);
		if( pPlayer )
		{
			pPlayer->Lock();
			Friend* pFriend		= pPlayer->m_RTMessenger.GetFriend( idPlayer );
			if( pFriend )
			{
				pFriend->dwState	= dwState;
				BEFORESENDSOLE( ar, PACKETTYPE_SETFRIENDSTATE, pPlayer->dpidUser );
				ar << idPlayer; 
				ar << dwState;
				SEND( ar, this, pPlayer->dpidCache );
			}
			pPlayer->Unlock();
		}
	}
}

void CDPCacheSrvr::SendFriendJoin( CPlayer* pTo, CPlayer* pFriend )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ADDFRIENDJOIN, pTo->dpidUser );

	ar << pFriend->uKey;
	ar << pFriend->m_RTMessenger.GetState();

	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendFriendLogOut( CPlayer* pTo, u_long uidPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ADDFRIENDLOGOUT, pTo->dpidUser );
	ar << uidPlayer;
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendFriendNoIntercept(CPlayer * pTo, u_long uFriendid) {
	BEFORESENDSOLE(ar, PACKETTYPE_FRIENDNOINTERCEPT, pTo->dpidUser);
	ar << uFriendid;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendFriendIntercept( CPlayer* pPlayer, CPlayer* pFriend )
{
	{
		BEFORESENDSOLE( ar, PACKETTYPE_FRIENDINTERCEPTSTATE, pPlayer->dpidUser );
		ar << pPlayer->uKey << pFriend->uKey;
		SEND( ar, this, pPlayer->dpidCache );
	}
	{
		BEFORESENDSOLE( ar, PACKETTYPE_FRIENDINTERCEPTSTATE, pFriend->dpidUser );
		ar << pPlayer->uKey << pFriend->uKey;
		SEND( ar, this, pFriend->dpidCache );
	}
}

void CDPCacheSrvr::SendFriendIntercept( CPlayer* pPlayer, u_long uFriendid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_FRIENDINTERCEPTSTATE, pPlayer->dpidUser );
	ar << pPlayer->uKey << uFriendid;
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendKillPlayer( CPlayer* pPlayer )
{
	SendHdr( PACKETTYPE_KILLPLAYER, pPlayer->dpidCache, pPlayer->dpidUser );
}

void CDPCacheSrvr::SendGetPlayerAddr( const CHAR* lpszPlayer, const CHAR* lpAddr, CPlayer* pOperator )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GETPLAYERADDR, pOperator->dpidUser );
	ar.WriteString( lpszPlayer );
	ar.WriteString( lpAddr );
	SEND( ar, this, pOperator->dpidCache );
}

void CDPCacheSrvr::SendGetPlayerCount( u_short uCount, CPlayer* pOperator )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GETPLAYERCOUNT, pOperator->dpidUser );
	ar << uCount;
	SEND( ar, this, pOperator->dpidCache );
}

void CDPCacheSrvr::SendGetCorePlayer( CPlayer* pOperator )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GETCOREPLAYER, pOperator->dpidUser );
	g_PlayerMng.PackName( ar );
	SEND( ar, this, pOperator->dpidCache );
}

void CDPCacheSrvr::SendSystem( const CHAR* lpString )
{
	BEFORESENDSOLE( ar, PACKETTYPE_SYSTEM, DPID_ALLPLAYERS );
	ar.WriteString( lpString );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendCaption( const CHAR* lpString, DWORD dwWorldId, BOOL bSmall )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CAPTION, DPID_ALLPLAYERS );
	ar << bSmall;
	ar << dwWorldId;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCacheSrvr::SendGameRate( FLOAT fRate, BYTE nFlag )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GAMERATE, DPID_ALLPLAYERS );
	ar << fRate;
	ar << nFlag;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendDefinedText( int dwText, DPID dpidCache, DPID dpidUser, LPCSTR lpszFormat, ... )
{
	va_list args;
	va_start( args, lpszFormat );
	int nBuf;
	TCHAR szBuffer[128];
	nBuf	= _vsntprintf( szBuffer, 128, lpszFormat, args );
	va_end( args );

	BEFORESENDSOLE( ar, PACKETTYPE_DEFINEDTEXT, dpidUser );
	ar << dwText;
	ar.WriteString( szBuffer );
	SEND( ar, this, dpidCache );
}

void CDPCacheSrvr::OnSendTag( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	const auto [idTo, szString] = ar.Extract<u_long, TCHAR[256]>();

	TRACE( "CDPCacheSrvr::OnSendTag - %d %s\n", idTo, szString );
	
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pFrom	=  g_PlayerMng.GetPlayerBySerial( dpidUser );
	if (!pFrom) return; // 보내는자 없으면 무시

	pFrom->Lock();

	TAG_RESULT result = pFrom->IsTagSendable( idTo );  // db에 쪽지를 넣는다.
	switch (result) {
		case TAG_RESULT::BLOCKED:
			SendTagResult(pFrom, false);		// 상대방이 blocked시킨 상태이면 full(0의 의미)되어서 보내지 못했다고 알려준다.
			break;

		case TAG_RESULT::OK:
			if (g_PlayerMng.GetPlayer(idTo)) {
				pFrom->Unlock();
				return;					// 받는자 online이면 무시 
			}

			g_dpDatabaseClient.SendTag(pFrom->uKey, idTo, szString);
			break;
	}

	pFrom->Unlock();
}

// cbResult -  결과: 0 - 실패(20개 초과의 경우), 1 - 성공 
void CDPCacheSrvr::SendTagResult(CPlayer * pPlayer, bool cbResult) {
	BEFORESENDSOLE(ar, PACKETTYPE_INSERTTAG_RESULT, pPlayer->dpidUser);
	ar << cbResult;
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::OnPartyChangeLeader( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idLeader, idChangeLeader;
	ar >> _idLeader >> idChangeLeader;
	
	CPlayer* pLeader;
	CPlayer* pChangeLeader;
	
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	pLeader = g_PlayerMng.GetPlayerBySerial( dpidUser );
	pChangeLeader = g_PlayerMng.GetPlayer( idChangeLeader );
	
	if( !pLeader )
	{
		//		player not found
		return;
	}
	
	CParty* pParty;
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
	
	pParty	= g_PartyMng.GetParty( pLeader->m_uPartyId );
	if( !pParty || !pParty->IsMember( pLeader->uKey ) )
	{
		// leader's party not found
		// OR is not party member
		pLeader->m_uPartyId		= 0;
		return;
	}
	if( pLeader->uKey != idChangeLeader )
	{
		if( FALSE == pParty->IsLeader( pLeader->uKey ) )
		{
			// is not leader
			return;
		}
		if( !pParty->IsMember( idChangeLeader ) )
		{
			// pMember is not pLeader's party member
			SendErrorParty( ERROR_NOTMAMBER, pLeader );
			return;
		}
	}
	else
	{
		SendErrorParty( ERROR_SAMLEADER, pLeader );
		return;
	}

	int nFind = pParty->FindMember( idChangeLeader );
	if( nFind == -1 || pParty->m_aMember[nFind].m_remove )
		return;

	for( int j = 0 ; j < MAX_PARTYMODE ; j++ )
	{
		if( pParty->m_nModeTime[j] )
		{
			if( j == PARTY_PARSKILL_MODE)
				continue;
			pParty->m_nModeTime[j] = 0;
			g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, j, FALSE );
		}
	}
	pParty->ChangeLeader( idChangeLeader );
	g_dpCoreSrvr.SendChangeLeader( pParty->m_uPartyId, idChangeLeader );
}

void CDPCacheSrvr::OnAddPartyMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long idLeader; ar >> idLeader;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer * pLeader	= g_PlayerMng.GetPlayer( idLeader );
	CPlayer * pMember = g_PlayerMng.GetPlayerBySerial( dpidUser );

	if( !pLeader || !pMember )
		return;

	CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );

	CParty* pParty	= g_PartyMng.GetParty( pMember->m_uPartyId );	// member's party
	if( pParty && pParty->IsMember( pMember->uKey ) )
	{
		SendErrorParty( ERROR_DIFFRENTPARTY, pLeader );
		return;
	}

	pMember->m_uPartyId	= 0;
	pParty	= g_PartyMng.GetParty( pLeader->m_uPartyId );	// leader's party
	if( pParty && pParty->IsMember( pLeader->uKey ) )	// add
	{
		if( FALSE == pParty->IsLeader( pLeader->uKey ) )	// is not leader
		{
			SendErrorParty( ERROR_NOLEADER, pLeader );
			return;
		}
		int nMax = MAX_PTMEMBER_SIZE_SPECIAL;
		if( pParty->m_nSizeofMember >= nMax )
		{
			SendErrorParty( ERROR_FULLPARTY, pLeader );
			return;
		}
		if( pParty->m_idDuelParty > 0 )
		{
			return;
		}
		if( FALSE == pParty->NewMember( pMember->uKey ) )
		{
			// error
		}
		else	// ok
		{
			pMember->m_uPartyId		= pParty->m_uPartyId;
			g_dpCoreSrvr.SendAddPartyMember(pParty->m_uPartyId, pLeader->uKey, pMember->uKey);
			g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );
		}
	}
	else	// new
	{
		const u_long uPartyId = g_PartyMng.NewParty(pLeader->uKey, pMember->uKey);
		if( uPartyId != 0 )
		{
			pLeader->m_uPartyId		= pMember->m_uPartyId	= uPartyId;
			g_dpCoreSrvr.SendAddPartyMember(uPartyId, pLeader->uKey, pMember->uKey);
		}
		else
		{
//			error
		}
	}
}

void CDPCacheSrvr::OnRemovePartyMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long idMember; ar >> idMember;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer * pLeader	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	CPlayer * pMember = g_PlayerMng.GetPlayer( idMember );

	if( !pLeader )
		return;
	
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
	
	CParty * pParty	= g_PartyMng.GetParty( pLeader->m_uPartyId );
	if( !pParty || !pParty->IsMember( pLeader->uKey ) )
	{
		// leader's party not found
		// OR is not party member
		Error( "OnRemovePartyMember::Party Not Found OR IsMember Not Found" );
		pLeader->m_uPartyId		= 0;
		return;
	}

	if( pParty->m_idDuelParty > 0 )
		return;


	if( pLeader->uKey != idMember )
	{
		if( FALSE == pParty->IsLeader( pLeader->uKey ) )
		{
			// is not leader
			Error( "OnRemovePartyMember::Not IsLeader" );
			return;
		}
		if( !pParty->IsMember( idMember ) )
		{
			// pMember is not pLeader's party member
			Error( "OnRemovePartyMember::Not IsMember" );
			SendErrorParty( ERROR_DIFFRENTPARTY, pLeader );
			return;
		}
	}
	BOOL bLeader = pParty->IsLeader( idMember );
	if( pParty->DeleteMember( idMember ) )
	{
		if( pMember )
			pMember->m_uPartyId		= 0;
		g_dpCoreSrvr.SendRemoveParty( pParty->m_uPartyId, pLeader->uKey, idMember );
		if( pParty->GetSizeofMember() < 2 )
		{
			CPlayer* pLeadertmp	= g_PlayerMng.GetPlayer( pParty->GetPlayerId( 0 ) );
			if( pLeadertmp )
				pLeadertmp->m_uPartyId	= 0;
			g_PartyMng.DeleteParty( pParty->m_uPartyId );
		}
		else
		{
			if( bLeader )
			{
				for( int j = 0 ; j < MAX_PARTYMODE ; j++ )
				{
					if( pParty->m_nModeTime[j] )
					{
						if( j == PARTY_PARSKILL_MODE)
							continue;
						pParty->m_nModeTime[j] = 0;
						g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, j, FALSE );
					}
				}
				u_long idChangeLeader =  pParty->GetPlayerId(0);
				g_dpCoreSrvr.SendChangeLeader( pParty->m_uPartyId, idChangeLeader );
			}
			// 090917 mirchang - 파스킬 아이템 수정
			if( pParty->m_nModeTime[PARTY_PARSKILL_MODE] )
				g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );
		}
	}
	else
	{
		Error( "OnRemovePartyMember::Not DeleteMember" );
	}
}

void CDPCacheSrvr::OnPartyChangeName( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	if( uBufSize > 40 )	return;		// 4 + 4 + 32

	u_long _uidPlayer;
	TCHAR	sParty[33];	// 32
	ar >> _uidPlayer;	// 4
	ar.ReadString( sParty, 33 );
	CPlayer* pPlayer;
		
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	
	if( !pPlayer )
		return;
	
	CParty* pParty;
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
	
	pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
	if( !pParty || FALSE == pParty->IsMember( pPlayer->uKey ) )
	{
		// party not found
		SendErrorParty( ERROR_NOPARTY, pPlayer );
		return;
	}
	
	if( pParty->IsLeader( pPlayer->uKey ) )
	{	
		if( pParty->m_nKindTroup == 1 )
		{
			if(!g_PartyMng.IsPartyName( sParty ))
			{
				strcpy( pParty->m_sParty, sParty );
				g_PartyMng.AddPartyName( pPlayer->uKey, sParty );
				g_dpDatabaseClient.SendAddPartyName( pPlayer->uKey, sParty );
				g_dpCoreSrvr.SendPartyChangeName( pParty->m_uPartyId, sParty );
			}
			else
			{
				// 이미 그런 이름은 있다~ 다른사람이 사용중임
				SendErrorParty( ERROR_DIFFERNTUSERNAME, pPlayer );
			}
			
		}
		else
		{
			// 이미 순회극단이 아니므로 이름을 설정할수 없따~
		}
	}
	else
	{
		// 단장이 아니다 그러므로 할수가 없따~
	}
}



void CDPCacheSrvr::OnPartyChangeItemMode( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CParty::ShareItemMode nItemMode; ar >> nItemMode;
	if (!CParty::IsValidMode(nItemMode)) return;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer * pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	
	if (!pPlayer) return;
	
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
	
	CParty * pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
	if( NULL == pParty )
	{
		// 파티 찾기 실패
		SendErrorParty( ERROR_NOPARTY, pPlayer );
		return;
	}

	if( pParty->IsLeader( pPlayer->uKey ) )
	{	
		pParty->m_nTroupeShareItem = nItemMode;
		g_dpCoreSrvr.SendPartyChangeItemMode( pPlayer->m_uPartyId, nItemMode );
	}
	else
	{
		// 단장이 아니다 그러므로 할수가 없따~
	}
}

void CDPCacheSrvr::OnPartyChangeExpMode( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CParty::ShareExpMode nExpMode; ar >> nExpMode;
	if (!CParty::IsValidMode(nExpMode)) return;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer * pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (!pPlayer) return;
	
	CParty* pParty;
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
	
	pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
	if (NULL == pParty) {
		// 파티 찾기 실패
		SendErrorParty(ERROR_NOPARTY, pPlayer);
		return;
	}

	if( pParty->IsLeader( pPlayer->uKey ) )
	{	
		if( pParty->m_nKindTroup == 1 )
		{
			pParty->m_nTroupsShareExp = nExpMode;
			g_dpCoreSrvr.SendPartyChangeExpMode( pPlayer->m_uPartyId, nExpMode );
		}
		else
		{
			// 이미 순회극단이 아니므로 이름을 설정할수 없따~
		}
	}
	else
	{
		// 단장이 아니다 그러므로 할수가 없따~
	}
}

void CDPCacheSrvr::OnPartyChangeTroup( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _uidPlayer;
	BOOL   bSendName;
	char szPartyName[33];
	ar >> _uidPlayer;
	ar >> bSendName;
	if( bSendName )
		ar.ReadString( szPartyName, 33 );

	CPlayer* pPlayer;
		
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	
	if( !pPlayer )
		return;
	
	CParty* pParty;
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
	
	pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
	if( NULL == pParty )
	{
		// 파티 찾기 실패
		SendErrorParty( ERROR_NOPARTY, pPlayer );
		return;
	}
	
	if( pParty->IsLeader( pPlayer->uKey ) )
	{	
		if( pParty->m_nKindTroup != 1 && pParty->GetLevel() >= 10 )
		{
			LPCSTR lpszPlayer = g_PartyMng.GetPartyString( pPlayer->uKey );
			if( lpszPlayer != NULL && 0 == strcmp( lpszPlayer, szPartyName ) )
			{
				strcpy( pParty->m_sParty, lpszPlayer );
				pParty->m_nKindTroup = 1;
				g_dpCoreSrvr.SendPartyChangeTroup( pParty->m_uPartyId, pParty->m_sParty );
				g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );
			}
			else
			{
				if(!g_PartyMng.IsPartyName( szPartyName ))		
				{
					pParty->m_nKindTroup = 1;
					g_dpCoreSrvr.SendPartyChangeTroup( pParty->m_uPartyId, pParty->m_sParty );						
					g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );
					
					if( lpszPlayer != NULL )
						g_PartyMng.RemovePartyName( pPlayer->uKey, lpszPlayer );

					g_PartyMng.AddPartyName( pPlayer->uKey, szPartyName );
					g_dpDatabaseClient.SendAddPartyName( pPlayer->uKey, szPartyName );
					g_dpCoreSrvr.SendPartyChangeName( pParty->m_uPartyId, szPartyName );
					strcpy( pParty->m_sParty, szPartyName );
				}
				else
				{
					// 이미 그런 이름은 있다~ 다른사람이 사용중임
					SendErrorParty( ERROR_DIFFERNTUSERNAME, pPlayer );
				}
			}
		}
		else
		{
			// 이미 순회극단이다
		}
	}
	else
	{
		// 단장이 아니다 그러므로 할수가 없따~
	}
}


void CDPCacheSrvr::OnDestroyGuild( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idMaster;
	ar >> _idMaster;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial( dpidUser );
	if( !pMaster )
	{
		// player not found
		return;
	}

	CGuild* pGuild;
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	pGuild	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pGuild || FALSE == pGuild->IsMember( pMaster->uKey ) )
	{
		// guild not found
		// OR is not member
		SendDefinedText( TID_GAME_COMNOHAVECOM, pMaster->dpidCache, pMaster->dpidUser, "" );
		pMaster->m_idGuild	= 0;
		return;
	}

	if( FALSE == pGuild->IsMaster( pMaster->uKey ) )
	{
		// is not leader
		SendDefinedText( TID_GAME_COMDELNOTKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	if( pGuild->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNODISMISS, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	CGuildMember* pMember;
	CPlayer* pPlayer;
	for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
	{
		pMember		= i->second;
		pPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
		if( pPlayer )
		{
			pPlayer->m_idGuild	= 0;
			pPlayer->m_tGuildMember = CTime::GetCurrentTime();
			pPlayer->m_tGuildMember += CTimeSpan( 2, 0, 0, 0 );
		}
		
	}
	u_long idGuild	= pGuild->m_idGuild;
	g_GuildMng.RemoveGuild( idGuild );
	g_dpDatabaseClient.SendDestroyGuild( idGuild, pMaster->uKey );	// databaseserver
	g_dpCoreSrvr.SendDestroyGuild( idGuild );	// worldserver
}

// fixme - raiders
void CDPCacheSrvr::OnAddGuildMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long idMaster;//, idPlayer;
	GUILD_MEMBER_INFO	info;
	ar >> idMaster;// >> idPlayer;
	ar.Read( &info, sizeof(GUILD_MEMBER_INFO) );
	CPlayer* pMaster, *pPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	pMaster	= g_PlayerMng.GetPlayer( idMaster );
	pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );

	if( !pPlayer )
		return;
	if( pPlayer->uKey != info.idPlayer )
		return;
	if( !pMaster )
	{
		SendDefinedText( TID_GAME_GUILDCHROFFLINE, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

	CTime tCurrent = CTime::GetCurrentTime();

	if( !g_PlayerMng.IsOperator( info.idPlayer ) && tCurrent < pPlayer->m_tGuildMember )
	{
		if( ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_GER )	// 유럽 - 문장이 길어서 2줄로 나누 텍스트...
		{
			SendDefinedText( TID_GAME_GUILDNOTINCLUDE_01, pMaster->dpidCache, pMaster->dpidUser, "" );
			SendDefinedText( TID_GAME_GUILDNOTINCLUDE_01, pMaster->dpidCache, pMaster->dpidUser, "" );
			SendDefinedText( TID_GAME_GUILDNOTINCLUDE_02, pPlayer->dpidCache, pPlayer->dpidUser, "" );
			SendDefinedText( TID_GAME_GUILDNOTINCLUDE_02, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		}
		else
		{
			SendDefinedText( TID_GAME_GUILDNOTINCLUDE, pMaster->dpidCache, pMaster->dpidUser, "" );
			SendDefinedText( TID_GAME_GUILDNOTINCLUDE, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		}
		return;
	}
		

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
//	1
	CGuild* pGuild	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pGuild || !pGuild->IsMember( pMaster->uKey ) )
	{
		// guild not found
		SendDefinedText( TID_GAME_COMNOEXCOM, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

	if( pGuild->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNOINVATE, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

//	2
	CGuild* pGuildtmp	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuildtmp && pGuildtmp->IsMember( pPlayer->uKey ) )
	{
		// guild exists
		SendDefinedText( TID_GAME_COMHAVECOM, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}
	pPlayer->m_idGuild	= 0;
//	3
//	if( pGuild->GetSize() >= MAX_GM_SIZE )
	if( pGuild->GetSize() >= pGuild->GetMaxMemberSize() )
	{
		// is full
		SendDefinedText( TID_GAME_COMOVERMEMBER, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}
//
	CGuildMember* pMember	= new CGuildMember;
	pMember->m_idPlayer	= pPlayer->uKey;
	pMember->m_nMemberLv	= GUD_ROOKIE;
	if( pGuild->AddMember( pMember ) )
	{
		pPlayer->m_idGuild	= pGuild->m_idGuild;
		g_dpCoreSrvr.SendAddGuildMember( info, pGuild->m_idGuild );
		g_dpDatabaseClient.SendAddGuildMember( pPlayer->uKey, pGuild->m_idGuild, pMaster->uKey );
		CPlayer* pPlayertmp;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pPlayertmp	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayertmp )
			{
				if( pPlayertmp == pPlayer )
					SendGuild( pGuild, pPlayer );
				else
					SendAddGuildMember( info, pPlayer->lpszPlayer, pGuild->m_idGuild, pPlayertmp );
			}
		}
	}
	else
	{
		SAFE_DELETE( pMember );
	}
}

void CDPCacheSrvr::OnRemoveGuildMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idMaster, idPlayer;
	ar >> _idMaster >> idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pMaster	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );

	if( !pMaster )
		return;

	CGuild* pGuild;
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
//	1
	pGuild	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pGuild || FALSE == pGuild->IsMember( pMaster->uKey ) )
	{
		// guild not found
		// OR is not member
		SendDefinedText( TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "" );
		pMaster->m_idGuild		= 0;
		return;
	}

	if( pGuild->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNOMEMBER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	
	if( pMaster->uKey != idPlayer )
	{
//		2
		if( FALSE == pGuild->IsMember( idPlayer ) )
		{
			// is not member
			SendDefinedText( TID_GAME_GUILDNOTMEMBER, pMaster->dpidCache, pMaster->dpidUser, "%s", pGuild->m_szGuild );
			return;
		}
//		3
		if( FALSE == pGuild->IsMaster( pMaster->uKey ) )
		{
			// is not leader
			SendDefinedText( TID_GAME_COMLEAVENOKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "" );
			return;
		}
	}
	else
	{
//		4
		if( pGuild->IsMaster( pMaster->uKey ) )
		{
			// do not remove leader
			SendDefinedText( TID_GAME_COMLEAVEKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "" );
			return;
		}
	}
//
	if( pGuild->RemoveMember( idPlayer ) )
	{
		if( pPlayer )
		{
			pPlayer->m_tGuildMember = CTime::GetCurrentTime();
			pPlayer->m_tGuildMember += CTimeSpan( 2, 0, 0, 0 );
			pPlayer->m_idGuild	= 0;
			SendRemoveGuildMember( idPlayer, pGuild->m_idGuild, pPlayer );
		}
//		
		CPlayer* pPlayertmp;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pPlayertmp	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayertmp )
				SendRemoveGuildMember( idPlayer, pGuild->m_idGuild, pPlayertmp );
		}
//
		g_dpCoreSrvr.SendRemoveGuildMember( idPlayer, pGuild->m_idGuild );
		g_dpDatabaseClient.SendRemoveGuildMember( idPlayer, pGuild->m_idGuild, pMaster->uKey );
	}
}

void CDPCacheSrvr::OnGuildMemberLv( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idMaster, idPlayer;
	int nMemberLv;
	ar >> _idMaster >> idPlayer >> nMemberLv;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pMaster	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );

	if( !pMaster )
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	
	CGuild* pGuild	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pGuild || FALSE == pGuild->IsMember( pMaster->uKey ) )
	{
		// guild not found
		// OR is not member
		SendDefinedText( TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "" );
		pMaster->m_idGuild		= 0;
		return;
	}

	if( pGuild->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNOMEMBER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	if( FALSE == pGuild->IsMember( idPlayer ) )
	{
		// is not member
		SendDefinedText( TID_GAME_GUILDNOTMEMBER, pMaster->dpidCache, pMaster->dpidUser, "%s", pGuild->m_szGuild );
		return;
	}
	CGuildMember* pMember1	= pGuild->GetMember( pMaster->uKey );
	CGuildMember* pMember2	= pGuild->GetMember( idPlayer );
	if( pMember1->m_nMemberLv >= pMember2->m_nMemberLv )
	{
		SendDefinedText( TID_GAME_GUILDAPPOVER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	if( pMember1->m_nMemberLv >= nMemberLv )
	{
		SendDefinedText( TID_GAME_GUILDWARRANTREGOVER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	if( !pGuild->IsCmdCap( pMember1->m_nMemberLv, PF_MEMBERLEVEL ) )
	{
		SendDefinedText( TID_GAME_GUILDAPPNOTWARRANT, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	if( nMemberLv < 0 || nMemberLv >= 5 )
	{
		SendDefinedText( TID_GAME_GUILDWARRANTREGOVER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	if( ( pGuild->GetMemberLvSize( nMemberLv ) + 1 ) > pGuild->GetMaxMemberLvSize( nMemberLv ) )
	{
		SendDefinedText( TID_GAME_GUILDAPPNUMOVER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	pMember2->m_nMemberLv	= nMemberLv;
	pMember2->m_nClass = 0;

	g_dpCoreSrvr.SendGuildMemberLv( idPlayer, nMemberLv, pGuild->m_idGuild );
	g_dpDatabaseClient.SendGuildMemberLv( idPlayer, nMemberLv );

	CGuildMember* pMember;
	CPlayer* pPlayertmp;
	for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
	{
		pMember		= i->second;
		pPlayertmp	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
		if( pPlayertmp )
			SendGuildMemberLv( idPlayer, nMemberLv, pPlayertmp );
	}
}

void CDPCacheSrvr::OnGuildAuthority( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _uidPlayer, _uGuildId;
	DWORD dwAuthority[MAX_GM_LEVEL];
	
	ar >> _uidPlayer >> _uGuildId;
	
	ar.Read( dwAuthority, sizeof(dwAuthority) );
	
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );	

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( pPlayer == NULL )
		return;

	CGuild* pGuild = g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMaster( pPlayer->uKey ) )
	{
		if( pGuild->GetWar() )
		{
			SendDefinedText( TID_GAME_GUILDWARNOMEMBER, pPlayer->dpidCache, pPlayer->dpidUser, "" );
			return;
		}
		memcpy( pGuild->m_adwPower, dwAuthority, sizeof(dwAuthority) );
		g_dpCoreSrvr.SendGuildAuthority( pPlayer->m_idGuild, dwAuthority );

		// GUILD DB AUTHORITY UPDATE
		g_dpDatabaseClient.SendGuildAuthority( pPlayer->m_idGuild, pGuild->m_adwPower );
	}
}

void CDPCacheSrvr::OnGuildSetName( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _uidPlayer, _uGuildId;
	char szName[MAX_G_NAME];
	
	ar >> _uidPlayer >> _uGuildId;
	ar.ReadString( szName, MAX_G_NAME );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );	

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( pPlayer == NULL )
		return;

	CGuild* pGuild = g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMaster( pPlayer->uKey ) )
	{
		CString str1, str2;
		str1 = pGuild->m_szGuild;
		str2	= "";
//		str2 = pPlayer->lpszPlayer;
		if( str1 == str2 )
		{
			if( g_GuildMng.SetName( pGuild, szName ) )
			{
				g_dpCoreSrvr.SendGuildSetName( pPlayer->m_idGuild, pGuild->m_szGuild );
				g_dpDatabaseClient.SendGuildSetName( pPlayer->m_idGuild, pGuild->m_szGuild );
				SendGuildSetName( pPlayer->m_idGuild, pGuild->m_szGuild );
			}
			else
			{
				// duplicated
				SendGuildError( pPlayer, 1 );
			}
		}
		else
		{
			//
		}
	}
	else
	{
		// is not master
		SendDefinedText( TID_GAME_COMDELNOTKINGPIN, pPlayer->dpidCache, pPlayer->dpidUser, "" );
	}
}

void CDPCacheSrvr::OnGuildPenya( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	const auto [_playerId, _guildId, dwType, dwPenya] = ar.Extract<u_long, u_long, DWORD, DWORD>();

	if (dwType >= MAX_GM_LEVEL) return;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );	

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( pPlayer == NULL )
		return;

	CGuild* pGuild = g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMaster( pPlayer->uKey ) )
	{
		if( 0 <= dwPenya && dwPenya < 1000000 )
		{
			pGuild->m_adwPenya[dwType] = dwPenya;
			g_dpCoreSrvr.SendGuildPenya( pPlayer->m_idGuild, dwType, dwPenya );
			
			// GUILD DB AUTHORITY UPDATE
			g_dpDatabaseClient.SendGuildPenya( pPlayer->m_idGuild, pGuild->m_adwPenya );
		}
		else
		{
			SendGuildError( pPlayer, 2 );
		}
	}
}

void CDPCacheSrvr::OnGuildClass( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	BYTE nFlag;
	u_long _idMaster, idPlayer;
	ar >> nFlag >> _idMaster >> idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer* pMaster	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	
	if( !pMaster )
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	
	CGuild* pGuild	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pGuild || FALSE == pGuild->IsMember( pMaster->uKey ) )
	{
		// guild not found
		// OR is not member
		SendDefinedText( TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "" );
		pMaster->m_idGuild		= 0;
		return;
	}
	
	if( pGuild->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNOMEMBER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	
	if( FALSE == pGuild->IsMember( idPlayer ) )
	{
		// is not member
		SendDefinedText( TID_GAME_GUILDNOTMEMBER, pMaster->dpidCache, pMaster->dpidUser, "%s", pGuild->m_szGuild );
		return;
	}
	
	CGuildMember* pMember1	= pGuild->GetMember( pMaster->uKey );
	CGuildMember* pMember2	= pGuild->GetMember( idPlayer );

	if( !pGuild->IsCmdCap( pMember1->m_nMemberLv, PF_LEVEL ) )	
	{
		SendDefinedText( TID_GAME_GUILDAPPNOTWARRANT, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	int nClass = pMember2->m_nClass;
	if( nFlag == 1 ) // UP
	{
		++nClass;
	}
	else	// DOWN
	{
		--nClass;
	}
	
	if( nClass < 0 || nClass > 2 )
	{
		return;
	}
	
	pMember2->m_nClass	= nClass;
	
	g_dpCoreSrvr.SendGuildClass( idPlayer, nClass, pGuild->m_idGuild );
	g_dpDatabaseClient.SendGuildClass( idPlayer, nClass );
	
	CGuildMember* pMember;
	CPlayer* pPlayertmp;
	for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
	{
		pMember		= i->second;
		pPlayertmp	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
		if( pPlayertmp )
			SendGuildClass( idPlayer, nClass, pPlayertmp );
	}
}

void CDPCacheSrvr::OnChgMaster( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idPlayer, idPlayer2;
	ar >> _idPlayer >> idPlayer2;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	CPlayer* pPlayer2	= g_PlayerMng.GetPlayer( idPlayer2 );
	if( !pPlayer || !pPlayer2 )
		return;
	if( pPlayer == pPlayer2 )
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	
	CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( !pGuild || FALSE == pGuild->IsMember( pPlayer->uKey ) || FALSE == pGuild->IsMember( pPlayer2->uKey ) )
	{
		// guild not found
		// OR is not member
		SendDefinedText( TID_GAME_COMLEAVENOCOM, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		pPlayer->m_idGuild		= 0;
		return;
	}
	if( pGuild->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNOMEMBER, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

	if( pGuild->IsMaster( pPlayer->uKey ) )
	{
		pGuild->m_idMaster	= idPlayer2;	// set master to guild

		g_dpCoreSrvr.SendChgMaster( pPlayer->uKey, idPlayer2, pGuild->m_idGuild );
		g_dpDatabaseClient.SendChgMaster( pPlayer->uKey, idPlayer2 );

		CGuildMember* pMember;
		pMember	= pGuild->GetMember( pPlayer->uKey );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_ROOKIE;
			pMember->m_nClass = 0;
		}
		pMember	= pGuild->GetMember( idPlayer2 );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_MASTER;
			pMember->m_nClass = 0;
		}

		CPlayer* pPlayertmp;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pPlayertmp	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
			if( pPlayertmp )
				SendChgMaster( pPlayer->uKey, idPlayer2, pPlayertmp );
		}
	}
}

void CDPCacheSrvr::OnGuildNickName( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idMaster, idPlayer;
	char strNickName[MAX_GM_ALIAS] = {0,};
	ar >> _idMaster >> idPlayer;
	ar.ReadString( strNickName, MAX_GM_ALIAS );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	
	if( !pMaster )
		return;
	
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	
	CGuild* pGuild	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pGuild || FALSE == pGuild->IsMember( pMaster->uKey ) )
	{
		// guild not found
		// OR is not member
		SendDefinedText( TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "" );
		pMaster->m_idGuild		= 0;
		return;
	}
	
	if( pGuild->m_nLevel < 10 )
	{
		SendDefinedText( TID_GAME_GUILDNOTLEVEL, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	
	if( pGuild->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNOMEMBER, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	if( pGuild->IsMaster( pMaster->uKey ) )
	{
//		int nLength = GetStrLen( strNickName );
		int nLen	= lstrlen( strNickName );

		if( nLen < 2 || nLen > 12 )
		{
			SendDefinedText( TID_DIAG_0011_01, pMaster->dpidCache, pMaster->dpidUser, "" );
			//				g_WndMng.OpenMessageBox( _T( "명칭에 한글 1글자 이상, 6글자 이하로 입력 입력하십시오." ) );
			return;
		}
		
		CGuildMember* pMember2	= pGuild->GetMember( idPlayer );
		if( !pMember2 )		// 대만 - 코어 서버 크래시	// 2007/01/21
			return;

		strcpy( pMember2->m_szAlias, strNickName );
		
		g_dpCoreSrvr.SendGuildNickName( idPlayer, pMember2->m_szAlias, pGuild->m_idGuild );
		g_dpDatabaseClient.SendGuildNickName( idPlayer, pMember2->m_szAlias );
		
		CGuildMember* pMember;
		CPlayer* pPlayertmp;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pPlayertmp	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
			if( pPlayertmp )
				SendGuildNickName( idPlayer, pMember2->m_szAlias, pPlayertmp );
		}
	}
}

void CDPCacheSrvr::SendGuild( CGuild* pGuild, CPlayer* pPlayer )
{
	if( !pPlayer )
		return;
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD, pPlayer->dpidUser );
	ar << pGuild->m_idGuild;
	pGuild->Serialize( ar, FALSE );
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendAddGuildMember( const GUILD_MEMBER_INFO & info, const char* lpszPlayer, u_long idGuild, CPlayer* pPlayer )
{
	if( !pPlayer )
		return;

	BEFORESENDSOLE( ar, PACKETTYPE_ADD_GUILD_MEMBER, pPlayer->dpidUser );
//	ar << idPlayer << idGuild;
	ar.Write( &info, sizeof(GUILD_MEMBER_INFO) );
	ar << idGuild;
	ar.WriteString( lpszPlayer );
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendRemoveGuildMember( u_long idPlayer, u_long idGuild, CPlayer* pPlayer )
{
	if( !pPlayer )
		return;

	BEFORESENDSOLE( ar, PACKETTYPE_REMOVE_GUILD_MEMBER, pPlayer->dpidUser );
	ar << idPlayer << idGuild;
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendGuildMemberLv( u_long idPlayer, int nMemberLv, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_MEMBER_LEVEL, pPlayer->dpidUser );
	ar << idPlayer << nMemberLv;
	SEND( ar, this, pPlayer->dpidCache );
}


void CDPCacheSrvr::SendGuildClass( u_long idPlayer, int nClass, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_CLASS, pPlayer->dpidUser );
	ar << idPlayer << nClass;
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendGuildNickName( u_long idPlayer, LPCTSTR strNickName, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_NICKNAME, pPlayer->dpidUser );
	ar << idPlayer;
	ar.WriteString( strNickName );
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendChgMaster( u_long idPlayer, u_long idPlayer2, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CHG_MASTER, pPlayer->dpidUser );
	ar << idPlayer << idPlayer2;
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendGuildSetName( u_long idGuild, const char* szName )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_SETNAME, DPID_ALLPLAYERS );
	ar << idGuild;
	ar.WriteString( szName );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendGuildMsgControl( GUILD_MSG_HEADER* pHeader, DWORD pPenya, BYTE cbCloak )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_ALLPLAYERS );

	ar.Write( pHeader, sizeof(GUILD_MSG_HEADER) );
	ar << pPenya;
	ar << cbCloak;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendGuildChat( const char* lpszPlayer, const char* sChat, CPlayer* pPlayer, OBJID objid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_CHAT, pPlayer->dpidUser );
	ar << objid;
	ar.WriteString( lpszPlayer );
	ar.WriteString( sChat );
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendGuildMemberLogin( CPlayer* pTo, BYTE nLogin, u_long uPlayerId, u_long uMultiNo )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_GAMELOGIN, pTo->dpidUser );
	ar << nLogin << uPlayerId << uMultiNo;
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendGuildMemberGameJoin( CPlayer * pTo, int nMaxLogin, u_long uLoginPlayerId[], u_long uLoginGuildMulti[] )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_GAMEJOIN, pTo->dpidUser );
	ar << nMaxLogin;
	ar.Write( uLoginPlayerId, sizeof( u_long ) * nMaxLogin );
	ar.Write( uLoginGuildMulti, sizeof( u_long) * nMaxLogin );
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendGuildError( CPlayer * pTo, int nError )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILD_ERROR, pTo->dpidUser );
	ar << nError;
	SEND( ar, this, pTo->dpidCache );
}

// fixme - raiders
void CDPCacheSrvr::OnAddFriend( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize ) {
	u_long uidSend; ar >> uidSend;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer * pSender = g_PlayerMng.GetPlayer( uidSend );
	CPlayer * pFriend	= g_PlayerMng.GetPlayerBySerial( dpidUser );

	if (!pFriend) return;
	if (!pSender) return;

	pSender->Lock();
	const bool bFullA	= pSender->m_RTMessenger.size() >= CRTMessenger::MaxFriend;
	pSender->Unlock();

	pFriend->Lock();
	const bool bFullB	= pFriend->m_RTMessenger.size() >= CRTMessenger::MaxFriend;
	pFriend->Unlock();
	if (bFullA) {
		// TODO: send TID_GAME_MSGMAXUSER
		return;
	}
	if (bFullB) {
		// TODO: send TID_GAME_MSGMAXUSER
		return;
	}
	pSender->Lock();
	pSender->m_RTMessenger.SetFriend(pFriend->uKey);
	pSender->Unlock();
	pFriend->Lock();
	pFriend->m_RTMessenger.SetFriend(uidSend);
	pFriend->Unlock();
	g_dpDatabaseClient.QueryAddMessenger(uidSend, pFriend->uKey);
	g_dpCoreSrvr.BroadcastPacket<PACKETTYPE_CW_ADDFRIEND, u_long, u_long>(uidSend, pFriend->uKey);
}

void CDPCacheSrvr::OnGetFriendState(CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize) {
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	
	if (CPlayer * pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser)) {
		SendFriendState(pPlayer);
	}
}

void CDPCacheSrvr::OnSetFrinedState( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer * pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	if (!pPlayer) return;
	
	FriendStatus state; ar >> state;
	if (!IsValid(state)) return;

	pPlayer->m_RTMessenger.SetState( state );
	SendSetFriendState( pPlayer );
	g_dpCoreSrvr.SendSetFriendState(pPlayer->uKey, pPlayer->m_RTMessenger.GetState());
}

void CDPCacheSrvr::OnFriendInterceptState( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	const auto [uidFriend] = ar.Extract<u_long>();

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer * pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	CPlayer * pFriendUser = g_PlayerMng.GetPlayer(uidFriend);

	if (!pPlayer) return;
	pPlayer->Lock();
	Friend* pFriend		= pPlayer->m_RTMessenger.GetFriend( uidFriend );
	if( pFriend )
	{
		if( pFriend->bBlock )	// release block
		{
			pFriend->bBlock		= FALSE;
			pFriend->dwState = pFriendUser ? pFriendUser->m_RTMessenger.GetState() : FriendStatus::OFFLINE;
			
			SendFriendNoIntercept(pPlayer, uidFriend);
			if (pFriendUser) {
				SendFriendJoin(pFriendUser, pPlayer);
			}

			g_dpDatabaseClient.QueryUpdateMessenger( pPlayer->uKey, uidFriend, FALSE );
		}
		else	// set block
		{
			pFriend->bBlock		= TRUE;
			if( pFriendUser )
				SendFriendIntercept( pPlayer, pFriendUser );	
			else
				SendFriendIntercept( pPlayer, uidFriend );
			g_dpDatabaseClient.QueryUpdateMessenger( pPlayer->uKey, uidFriend, TRUE );
		}
	}
	pPlayer->Unlock();
	g_dpCoreSrvr.SendFriendInterceptState( pPlayer->uKey, uidFriend );
}



void CDPCacheSrvr::OnRemoveFriend( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _uidPlayer, uidFriend;
	ar >> _uidPlayer >> uidFriend;

	CPlayer* pPlayer, *pFriendUser;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );

	if( pPlayer )
	{
		pPlayer->Lock();
		pPlayer->m_RTMessenger.RemoveFriend( uidFriend );
		pPlayer->Unlock();
		g_dpCoreSrvr.SendRemoveFriend( pPlayer->uKey, uidFriend );
		g_dpDatabaseClient.QueryDeleteMessenger( pPlayer->uKey, uidFriend );

		pFriendUser	= g_PlayerMng.GetPlayer( uidFriend );
		if( !pFriendUser )
			return;
		pFriendUser->Lock();
		pFriendUser->m_RTMessenger.RemoveFriend( pPlayer->uKey );
		pFriendUser->Unlock();
		// TODO: this is fishy. Current code mean that removing a friend on another
		// channel would lead them to see us as online? Shouldn't we be removed
		// instead?
		BEFORESENDSOLE( ar, PACKETTYPE_REMOVEFRIENDSTATE, pFriendUser->dpidUser );
		ar << pPlayer->uKey;
		SEND( ar, this, pFriendUser->dpidCache );
	}
}

void CDPCacheSrvr::SendErrorParty( DWORD dwError, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ERRORPARTY, pPlayer->dpidUser );
	ar << dwError;
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendAddFriend( u_long uLeader, u_long uMember, LONG nLeaderJob, BYTE nLeaderSex, char * szLeaderName, CPlayer* pMember )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ADDFRIENDNAMEREQEST, pMember->dpidUser );
	ar << uLeader << uMember;
	ar << nLeaderJob << nLeaderSex;
	ar.WriteString( szLeaderName );
	SEND( ar, this, pMember->dpidCache );
}

void CDPCacheSrvr::SendAddFriendNotFound( char * szMemberName, CPlayer* pLeader )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ADDFRIENDNAMENOTFOUND, pLeader->dpidUser );
	ar.WriteString( szMemberName );
	SEND( ar, this, pLeader->dpidCache );
}

void CDPCacheSrvr::SendBlock( BYTE nGu, char *szName, CPlayer* pTo )
{
	BEFORESENDSOLE( ar, PACKETTYPE_BLOCK, pTo->dpidUser );
	ar << nGu;
	ar.WriteString( szName );
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::OnSurrender( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idPlayer;
	ar >> _idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( !pPlayer )
		return;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( pPlayer->m_idWar );
	if( !pWar )
	{
//		not in war
		SendDefinedText( TID_GAME_GUILDWARNOETC, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}
	
	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;

	if( pDecl->m_idGuild == pPlayer->m_idGuild )
	{
		CGuildMember* pMember	= pDecl->GetMember( pPlayer->uKey );
		if( pMember )
		{
			pMember->m_nSurrender++;
			pWar->m_Decl.nSurrender++;

			g_dpCoreSrvr.SendSurrender( pWar->m_idWar, pPlayer->uKey, TRUE/*bDecl*/ );	// worldserver
			g_dpDatabaseClient.SendSurrender( pWar->m_idWar, pPlayer->uKey, pDecl->m_idGuild );	// trans
			SendSurrender( pWar->m_idWar, pDecl, pAcpt, pPlayer->uKey, pPlayer->lpszPlayer, TRUE/*bDecl*/ );	// neuz

			if( pDecl->IsMaster( pPlayer->uKey ) || ( ( pWar->m_Decl.nSurrender * 100 ) / pWar->m_Decl.nSize ) > 70 )
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_SR );
		}
	}
	else if( pAcpt->m_idGuild == pPlayer->m_idGuild )
	{
		CGuildMember* pMember	= pAcpt->GetMember( pPlayer->uKey );
		if( pMember )
		{
			pMember->m_nSurrender++;
			pWar->m_Acpt.nSurrender++;

			g_dpCoreSrvr.SendSurrender( pWar->m_idWar, pPlayer->uKey, FALSE/*bDecl*/ );	// world
			g_dpDatabaseClient.SendSurrender( pWar->m_idWar, pPlayer->uKey, pAcpt->m_idGuild );	// trans
			SendSurrender( pWar->m_idWar, pDecl, pAcpt, pPlayer->uKey, pPlayer->lpszPlayer, FALSE/*bDecl*/ );	// neuz

			if( pAcpt->IsMaster( pPlayer->uKey ) ||	( ( pWar->m_Acpt.nSurrender * 100 ) / pWar->m_Acpt.nSize ) > 70 )
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_SR );
		}
	}
	pPlayer->m_idWar	= WarIdNone;
}

void CDPCacheSrvr::SendSurrender(WarId idWar, CGuild* pDecl, CGuild* pAcpt, u_long idPlayer, const char* sPlayer, BOOL bDecl )
{
	for( auto i = pDecl->m_mapPMember.begin(); i != pDecl->m_mapPMember.end(); ++i )
	{
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
		if( pPlayer )
			SendSurrender( idWar, idPlayer, sPlayer, bDecl, pPlayer );
	}
	for( auto i	= pAcpt->m_mapPMember.begin(); i != pAcpt->m_mapPMember.end(); ++i )
	{
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
		if( pPlayer )
			SendSurrender( idWar, idPlayer, sPlayer, bDecl, pPlayer );
	}
}

void CDPCacheSrvr::SendSurrender(WarId idWar, u_long idPlayer, const char* sPlayer, BOOL bDecl, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_SURRENDER, pPlayer->dpidUser );
	ar << idWar << idPlayer;
	ar.WriteString( sPlayer );
	ar << bDecl;
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::OnQueryTruce( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idPlayer;
	ar >> _idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( !pPlayer )
		return;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( pPlayer->m_idWar );
	if( !pWar )
		return;

	CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( !pGuild )
		return;

	if( !pGuild->IsMaster( pPlayer->uKey ) )
	{
		// is not master
		return;
	}

	CGuild* pGuild2	= NULL;
	if( pWar->IsDecl( pGuild->m_idGuild ) )
		pGuild2		= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	else
		pGuild2		= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	if( !pGuild2 )
		return;
	CPlayer* pMaster	= g_PlayerMng.GetPlayer( pGuild2->m_idMaster );
	if( !pMaster )
	{
		// master not found
		return;
	}
	SendQueryTruce( pMaster );
}

void CDPCacheSrvr::OnAcptTruce( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idPlayer;
	ar >> _idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CPlayer* pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	if( !pPlayer )
		return;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( pPlayer->m_idWar );
	if( !pWar )
		return;

	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;

	g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_TRUCE );
}

void CDPCacheSrvr::OnDeclWar( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idMaster;
	char szGuild[MAX_G_NAME];
	ar >> _idMaster;
	ar.ReadString( szGuild, MAX_G_NAME );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( !pMaster )
		return;
	
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	CGuild* pDecl	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pDecl )
	{
		SendDefinedText( TID_GAME_COMNOHAVECOM, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	if( !pDecl->IsMaster( pMaster->uKey ) )
	{
		SendDefinedText( TID_GAME_COMDELNOTKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
#ifndef __INTERNALSERVER
	if( pDecl->m_nLevel < 6 )
	{
		SendDefinedText( TID_GAME_GUILDWARREQLV6, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
#endif	// __INTERNALSERVER

	if( pDecl->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARSTILLNOWAR, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	CGuild* pAcpt	= g_GuildMng.GetGuild( szGuild );
	if( !pAcpt )
	{
		SendDefinedText( TID_GAME_GUILDWARNOTHINGGUILD, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
#ifndef __INTERNALSERVER
	if( pAcpt->m_nLevel < 6 )
	{
		SendDefinedText( TID_GAME_GUILDWAROHTERLV6, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
#endif	// __INTERNALSERVER
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( pAcpt->m_idMaster );
	if( !pPlayer )
	{
		SendDefinedText( TID_GAME_GUILDWARMASTEROFF, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
#ifndef __INTERNALSERVER
	if( pAcpt->GetSize() < 10 )
	{
		SendDefinedText( TID_GAME_GUILDWARMEMBER10, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
#endif	// __INTERNALSERVER
	if( pAcpt->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWAROTHERWAR, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	if( pDecl == pAcpt )
		return;

	SendDeclWar( pDecl->m_idGuild, pMaster->lpszPlayer, pPlayer );
}

// fixme	- raiders
void CDPCacheSrvr::OnAcptWar( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idMaster, idDecl;
	ar >> _idMaster >> idDecl;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( !pMaster )
		return;
	
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pMaster->m_idGuild );
	if( !pAcpt )
	{
		SendDefinedText( TID_GAME_COMNOHAVECOM, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	if( !pAcpt->IsMaster( pMaster->uKey ) )
	{
		SendDefinedText( TID_GAME_COMDELNOTKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	if( pAcpt->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWARNOREQUEST, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	CGuild* pDecl	= g_GuildMng.GetGuild( idDecl );
	if( !pDecl )
	{
		SendDefinedText( TID_GAME_GUILDWARNOFINDGUILD, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}

	if( pDecl == pAcpt )
	{
		return;
	}

	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( pDecl->m_idMaster );
	if( !pPlayer )
	{
		// master is absent
		SendDefinedText( TID_GAME_GUILDWARMASTEROFF, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	if( pDecl->GetWar() )
	{
		SendDefinedText( TID_GAME_GUILDWAROTHERWAR, pMaster->dpidCache, pMaster->dpidUser, "" );
		return;
	}
	
	CGuildWar* pWar		= new CGuildWar;
	pWar->m_Decl.idGuild	= pDecl->m_idGuild;
	pWar->m_Decl.nSize	= pDecl->GetSize();
	pWar->m_Acpt.idGuild	= pAcpt->m_idGuild;
	pWar->m_Acpt.nSize	= pAcpt->GetSize();
	pWar->m_nFlag	= WF_WARTIME;
	pWar->m_time	= CTime::GetCurrentTime();

	WarId idWar	= g_GuildWarMng.AddWar( pWar );
	if( idWar != WarIdNone )
	{
		pDecl->m_idWar	= idWar;
		pDecl->m_idEnemyGuild	= pAcpt->m_idGuild;
		pAcpt->m_idWar	= idWar;
		pAcpt->m_idEnemyGuild	= pDecl->m_idGuild;

		for( auto i = pDecl->m_mapPMember.begin(); i != pDecl->m_mapPMember.end(); ++i )
		{
			CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayer )
				pPlayer->m_idWar	= idWar;
		}
		for( auto i	= pAcpt->m_mapPMember.begin(); i != pAcpt->m_mapPMember.end(); ++i )
		{
			CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayer )
				pPlayer->m_idWar	= idWar;
		}
		g_dpDatabaseClient.SendAcptWar( idWar, idDecl, pAcpt->m_idGuild );
		g_dpCoreSrvr.SendAcptWar( idWar, idDecl, pAcpt->m_idGuild );
		SendAcptWar( idWar, idDecl, pAcpt->m_idGuild );
	}
	else
	{
		SAFE_DELETE( pWar );
	}
}

void CDPCacheSrvr::SendDeclWar( u_long idDecl, const char* pszMaster, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DECL_GUILD_WAR, pPlayer->dpidUser );
	ar << idDecl;
	ar.WriteString( pszMaster );
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendAcptWar(WarId idWar, u_long idDecl, u_long idAcpt )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ACPT_GUILD_WAR, DPID_ALLPLAYERS );
	ar << idWar << idDecl << idAcpt;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendWarEnd(WarId idWar, int nWptDecl, int nWptAcpt, int nType )
{
	BEFORESENDSOLE( ar, PACKETTYPE_WAR_END, DPID_ALLPLAYERS );
	ar << idWar << nWptDecl << nWptAcpt << nType;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendWarDead(WarId idWar, const char* lpszPlayer, BOOL bDecl, CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_WAR_DEAD, pPlayer->dpidUser );
	ar << idWar;
	ar.WriteString( lpszPlayer );
	ar << bDecl;
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::SendQueryTruce( CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERY_TRUCE, pPlayer->dpidUser );
	SEND( ar, this, pPlayer->dpidCache );
}

void CDPCacheSrvr::OnAddVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	char szTitle[MAX_BYTE_VOTETITLE];
	char szQuestion[MAX_BYTE_VOTEQUESTION];
	char szSelections[4][MAX_BYTE_VOTESELECT];

	u_long _idGuild, _idPlayer;

	ar >> _idGuild;
	ar >> _idPlayer;
	ar.ReadString( szTitle, MAX_BYTE_VOTETITLE );
	ar.ReadString( szQuestion, MAX_BYTE_VOTEQUESTION );
	for( int i=0; i<4; ++i )
	{
		ar.ReadString( szSelections[i], MAX_BYTE_VOTESELECT );
	}

	////////////////////////////////////////////////////////////////////
	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( !pPlayer )
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );	
	
	CGuild* pGuild = g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMaster( pPlayer->uKey ) )
	{
		g_dpDatabaseClient.SendAddVote( pPlayer->m_idGuild, szTitle, szQuestion, szSelections );
		// 디비의 응답을 받아서 처리한다. CDPDatabaseClient::OnAddVoteResult()
	}	
}

void CDPCacheSrvr::OnRemoveVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long idVote;
	u_long _idGuild, _idPlayer;

	ar >> _idGuild;
	ar >> _idPlayer;
	ar >> idVote;
	
	////////////////////////////////////////////////////////////////////
	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( !pPlayer )
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );	
	
	CGuild* pGuild = g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMaster( pPlayer->uKey ) && pGuild->FindVote( idVote ) )
	{
		g_dpDatabaseClient.SendRemoveVote( idVote );
		pGuild->ModifyVote( idVote, REMOVE_VOTE, 0 );
		
		g_dpCoreSrvr.SendAddRemoveVote( pPlayer->m_idGuild, idVote );		// 모든 월드서버에 알린다.
	}	
}

void CDPCacheSrvr::OnCloseVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idGuild, _idPlayer;
	u_long idVote;

	ar >> _idGuild;
	ar >> _idPlayer;
	ar >> idVote;

	////////////////////////////////////////////////////////////////////
	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial( dpidUser );	
	if( !pPlayer )
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );	
	
	CGuild* pGuild = g_GuildMng.GetGuild( pPlayer->m_idGuild );
	CGuildVote* pVote = pGuild->FindVote( idVote );
	if( pGuild && pGuild->IsMaster( pPlayer->uKey ) && pVote )
	{
		if( pVote->IsCompleted() )	// 완료된 상태인가?
			return;

		g_dpDatabaseClient.SendCloseVote( idVote );
		pVote->SetComplete();

		g_dpCoreSrvr.SendAddCloseVote( pPlayer->m_idGuild, idVote );		// 모든 월드서버에 알린다.
	}	
}

void CDPCacheSrvr::OnCastVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	u_long _idGuild, _idPlayer;
	u_long idVote;
	BYTE cbSelection;

	ar >> _idGuild;
	ar >> _idPlayer;
	ar >> idVote;
	ar >> cbSelection;

	if( cbSelection > 3 )
		return;

	////////////////////////////////////////////////////////////////////

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );	
	
	CPlayer* pPlayer	= g_PlayerMng.GetPlayerBySerial( dpidUser );
	if( !pPlayer )
		return;

	CGuild* pGuild = g_GuildMng.GetGuild( pPlayer->m_idGuild );
	CGuildVote* pVote = pGuild->FindVote( idVote );
	CGuildMember* pMember = pGuild->GetMember( pPlayer->uKey );

	if( pGuild && pMember && pVote )
	{
		if( pVote->IsCompleted() )					// 완료된 상태인가?
			return;

		if( pMember->m_idSelectedVote == idVote )	// 이미 투표했던가?
			return;

		GUILD_VOTE_SELECT& select = pVote->GetVoteSelect( cbSelection );
		if( select.szString[0] == '\0')				// 빈문자열은 투표불가 		
			return;

		g_dpDatabaseClient.SendCastVote( idVote, cbSelection );
		
		pMember->m_idSelectedVote = idVote;

		g_dpCoreSrvr.SendAddCastVote( pPlayer->m_idGuild, idVote, cbSelection );		// 모든 월드서버에 알린다.
	}	
}

void CDPCacheSrvr::SendUpdateGuildRank()
{
	BEFORESENDSOLE( ar, PACKETTYPE_UPDATE_GUILD_RANKING, DPID_ALLPLAYERS );
	ar << CGuildRank::Instance;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendSetPlayerName( u_long idPlayer, const char* lpszPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_ALLPLAYERS );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCacheSrvr::SendSnoop( const char* lpszString, CPlayer* pSnoop )
{
	BEFORESENDSOLE( ar, PACKETTYPE_SNOOP, pSnoop->dpidUser );
	ar.WriteString( lpszString );
	SEND( ar, this, pSnoop->dpidCache );
}

void CDPCacheSrvr::SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData, CPlayer* pTo )
{
	BEFORESENDSOLE( ar, PACKETTYPE_UPDATE_PLAYER_DATA, pTo->dpidUser );
	ar << idPlayer;
	ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
	SEND( ar, this, pTo->dpidCache );
}

void CDPCacheSrvr::SendLogout( u_long idPlayer, CPlayer* pTo )
{
	BEFORESENDSOLE( ar, PACKETTYPE_LOGOUT, pTo->dpidUser );
	ar << idPlayer;
	SEND( ar, this, pTo->dpidCache );
}

#ifdef __QUIZ
void CDPCacheSrvr::SendQuizSystemMessage( int nDefinedTextId, BOOL bAll, int nChannel, int nTime )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUIZ_NOTICE, DPID_ALLPLAYERS );
	ar << nDefinedTextId << bAll << nChannel << nTime;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif // __QUIZ

CDPCacheSrvr	g_DPCacheSrvr;