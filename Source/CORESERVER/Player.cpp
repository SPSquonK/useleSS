#include "StdAfx.h"
#include "Player.h"
#include "msghdr.h"
#include "dpcachesrvr.h"
#include "dploginsrvr.h"
#include "party.h"
#include "guild.h"
#include "dpdatabaseclient.h"
#include "dpcoresrvr.h"

CPlayerMng	g_PlayerMng;

#ifndef __VM_0820
MemPooler<CPlayer>*	CPlayer::m_pPool		= new MemPooler<CPlayer>( 512 );
#endif	// __VM_0820

CPlayer::CPlayer( u_long idPlayer, const CHAR* pszPlayer, const CHAR* pszAccount )
{
	dpid = dpidCache = dpidUser	= DPID_UNKNOWN;
	uKey = idPlayer;
	lstrcpy( lpszPlayer, pszPlayer );
	lstrcpy( lpszAccount, pszAccount );
	*lpAddr	= '\0';
	m_tGuildMember = CTime::GetCurrentTime();
}

TAG_RESULT CPlayer::IsTagSendable(u_long idTo) const {
	const Friend * pFriend = m_RTMessenger.GetFriend(idTo);
	if (!pFriend) return TAG_RESULT::NOTFRIEND;
	if (pFriend->bBlock) return TAG_RESULT::BLOCKED;
	return TAG_RESULT::OK;
}


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CPlayerMng::CPlayerMng()
{
	m_uCount	= 0;
}

CPlayerMng::~CPlayerMng()
{
	Free();
}

void CPlayerMng::Free()
{
	CMclAutoLock Lock( m_AddRemoveLock );

	for( auto it = m_players.begin(); it != m_players.end(); ++it )
	{
		safe_delete( it->second );
	}
	m_players.clear();

	m_uCount	= 0;
	m_ulong2.clear();
}

BOOL CPlayerMng::AddCache( DPID dpidCache )
{
	return TRUE;
}

BOOL CPlayerMng::RemoveCache( DPID dpidCache )
{
	CMclAutoLock Lock( m_AddRemoveLock );

	for( auto it = m_players.begin(); it != m_players.end(); ++it )
	{
		CPlayer* pPlayer = it->second;
		if( *pPlayer->lpszAccount != '\0' )
			g_dpLoginSrvr.SendQueryRemovePlayer( pPlayer->lpszAccount );
		else
			WriteLog( "RemoveCache(): *pPlayer->lpszAccount == '\0'" );
	}

	m_uCount = 0;
	return TRUE;
}

BOOL CPlayerMng::AddPlayer( u_long idPlayer, const CHAR* lpszPlayer, const CHAR* lpszAccount )
{
	CPlayer* pPlayer = GetPlayer( idPlayer );
	if( pPlayer )
		return FALSE;
 
	pPlayer		= new CPlayer( idPlayer, lpszPlayer, lpszAccount );	
	m_ulong2.emplace(idPlayer, pPlayer);
	return TRUE;
}


BOOL CPlayerMng::UnregisterPlayerInfo( CPlayer* pPlayer, BOOL bNotify )
{
	Logout( pPlayer );

	g_PartyMng.RemoveConnection( pPlayer );
	g_GuildMng.RemoveConnection( pPlayer );

	pPlayer->Lock();
	
	// messenger
	pPlayer->m_RTMessenger.SetState(FriendStatus::OFFLINE);
	for (const u_long playerId : pPlayer->m_RTMessenger | std::views::keys) {
		if (CPlayer * pPlayertmp = GetPlayer(playerId)) {
			g_DPCacheSrvr.SendFriendLogOut(pPlayertmp, pPlayer->uKey);
		}
	}

	if( pPlayer == GetPlayerBySerial( pPlayer->m_dwSerial ) )
	{
		m_players.erase( pPlayer->m_dwSerial );
		m_uCount--;
		if( bNotify )
		{
			BEFORESENDSOLE( ar, PACKETTYPE_LEAVE, pPlayer->dpidUser );
			ar << pPlayer->m_dwSerial;
			SEND( ar, &g_DPCacheSrvr, pPlayer->dpidCache );
		}
	}
	pPlayer->dpidCache = pPlayer->dpidUser	= DPID_UNKNOWN;	

	pPlayer->Unlock();
	return TRUE;
}

void CPlayerMng::RegisterPlayerInfo(CPlayer * pPlayer) {
	m_players.emplace(pPlayer->m_dwSerial, pPlayer);
	m_uCount++;
	g_PartyMng.AddConnection( pPlayer );
	g_GuildMng.AddConnection( pPlayer );

	std::vector< u_long > vecIdFriend;
	// 여기 루틴은 나를 등록한 사람에게 내가 들어왔다는 메세지를 날려주는 것임
	pPlayer->Lock();
	
	for( auto i = pPlayer->m_RTMessenger.begin(); i != pPlayer->m_RTMessenger.end(); ++i )
	{
		Friend* pFriend		= &i->second;
		if( !pFriend->bBlock )
			vecIdFriend.push_back( i->first );
	}

	pPlayer->Unlock();

	for (const u_long idFriend : vecIdFriend) {
		if (CPlayer * pFriendPlayer = GetPlayer(idFriend)) {
			g_DPCacheSrvr.SendFriendJoin(pFriendPlayer, pPlayer);
		}
	}
}

// bNotify - CACHE서버에게 플레이러 제거를 알릴 것인가?
void CPlayerMng::RemovePlayer( CPlayer* pPlayer, BOOL bNotify )
{
	UnregisterPlayerInfo( pPlayer, bNotify );				// m_players에서 제거 

	m_ulong2.erase( pPlayer->uKey );
	g_dpCoreSrvr.SendRemoveUser( pPlayer->m_dwSerial );		// 월드 서버에서도 제거하게 한다.

	m_set.erase( pPlayer->uKey );				
	SAFE_DELETE( pPlayer );
}

CPlayer* CPlayerMng::GetPlayerBySerial( DWORD dwSerial )
{
	const auto it = m_players.find( dwSerial );
	if( it != m_players.end() )
		return it->second;

	return NULL;
}

CPlayer* CPlayerMng::GetPlayerBySocket( DPID dpidSocket )
{
	for( auto it = m_players.begin(); it != m_players.end(); ++it )
	{
		CPlayer* p = it->second;
		if( p->dpidUser == dpidSocket )
			return p;
	}
	return NULL;
}

CPlayer* CPlayerMng::GetPlayer( u_long uKey )
{
	ULONG2PTR::iterator i	= m_ulong2.find( uKey );
	if( i != m_ulong2.end() )
		return i->second;
	return NULL;
}

void CPlayerMng::PackName( CAr & ar )
{
	CMclAutoLock Lock( m_AddRemoveLock );

	ar << m_uCount;

	for( auto it = m_players.begin(); it != m_players.end(); ++ it )
	{
		CPlayer* pPlayer = it->second;
		ar.WriteString( pPlayer->lpszPlayer );
	}
}

void CPlayerMng::AddItToSetofOperator( u_long uPlayerId )
{
//	CMclAutoLock	Lock( m_AddRemoveLock );
	m_set.insert( uPlayerId );
}

BOOL CPlayerMng::IsOperator( u_long idPlayer )
{
		CMclAutoLock	Lock( m_AddRemoveLock );
		const auto i	= m_set.find( idPlayer );
		return (BOOL)( i != m_set.end() );
}

u_long CPlayerMng::GetCount( void )
{	
	return m_uCount;	
}

void	CPlayerMng::Logout( CPlayer * pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	std::map<u_long, int>	mapIdPlayer;
	if( pPlayer )
	{
		u_long idPlayer	= pPlayer->uKey;
		//
		{
			CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
			CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
			if( pGuild && pGuild->IsMember( idPlayer ) )
			{
				for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
					mapIdPlayer.emplace(i->second->m_idPlayer, 0);
			}
		}
		//
		{
			CMclAutoLock	Lock3( g_PartyMng.m_AddRemoveLock );
			CParty* pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
			if( pParty && pParty->IsMember( idPlayer ) )
			{
				for( int i = 0; i < pParty->m_nSizeofMember; i++ )
					mapIdPlayer.emplace(pParty->m_aMember[i].m_uPlayerId, 0);
			}
		}
		//
		{
			pPlayer->Lock();
			for( auto i = pPlayer->m_RTMessenger.begin(); i != pPlayer->m_RTMessenger.end(); ++i )
				mapIdPlayer.emplace(i->first, 0);
			pPlayer->Unlock();
		}
		for( auto i = mapIdPlayer.begin(); i != mapIdPlayer.end(); ++i )
		{
			CPlayer* pTo	= g_PlayerMng.GetPlayer( i->first );
			if( pTo )
				g_DPCacheSrvr.SendLogout( idPlayer, pTo );
		}
	}


}
