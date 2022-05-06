#include "StdAfx.h"
#include "Player.h"
#include "dpcachesrvr.h"
#include "dpcoreclient.h"
#include "sqktd.h"

extern	CDPCoreClient	g_DPCoreClient;
extern	CDPCacheSrvr	g_DPCacheSrvr;

CCachePlayer::CCachePlayer(DPID dpidUser, DWORD dwSerial)
	:
	m_dpid(dpidUser),
	m_dwSerial(dwSerial),
	m_dwCreation(GetTickCount()) {
}

void CCachePlayer::Join( CAr & ar )
{
	ar >> m_dwWorldId >> m_idPlayer >> m_dwAuthKey;
	ar >> m_idParty >> m_idGuild >> m_idWar >> m_uChannel >> m_nSlot;
	ar.ReadString( m_szPlayer, MAX_PLAYER );
	ar.ReadString( m_szAccount, MAX_ACCOUNT );
	ar.ReadString( m_szPass, MAX_PASSWORD );
}

////////////////////////////////////////////////////////////////////////////////
CPlayerMng::CPlayerMng()
:
m_lSerial( 0 )
{
}

CPlayerMng::~CPlayerMng()
{
	Clear();
}

void CPlayerMng::Clear( void )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	for( auto i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
		safe_delete( i->second );
	m_mapPlayers.clear();
}

BOOL CPlayerMng::AddPlayer( DPID dpid )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	CCachePlayer * pPlayer	= new CCachePlayer( dpid, InterlockedIncrement( &m_lSerial ) );
	bool bResult	= m_mapPlayers.emplace(dpid, pPlayer).second;
	if( !bResult )
		safe_delete( pPlayer );
	return bResult;
}

CCachePlayer * CPlayerMng::GetPlayer(const DPID dpid) {
	return sqktd::find_in_map(m_mapPlayers, dpid);
}

BOOL CPlayerMng::RemovePlayer( DPID dpid )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	const auto i	= m_mapPlayers.find( dpid );
	if( i != m_mapPlayers.end() )
	{
		safe_delete( i->second );
		m_mapPlayers.erase( i );
		return TRUE;
	}
	return FALSE;
}

void CPlayerMng::DestroyPlayer( CDPClient* pClient )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	if( pClient )
		DestroyPlayersOnChannel( pClient );
	else
		DestroyAllPlayers();
}

void CPlayerMng::DestroyPlayersOnChannel(CDPClient * pClient) {
	const auto it = std::ranges::find_if(m_mapPlayers,
		[&](const auto & pair) {
			return pair.second->GetClient() == pClient;
		});

	if (it != m_mapPlayers.end()) {
		CCachePlayer * pPlayer = it->second;
		g_DPCoreClient.SendDestroyPlayer(*pPlayer);
		g_DPCacheSrvr.DestroyPlayer(pPlayer->GetNetworkId());
	}
}

void CPlayerMng::DestroyAllPlayers( void )
{
	for( auto i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
		g_DPCacheSrvr.DestroyPlayer( i->second->GetNetworkId() );
}

CCachePlayer * CPlayerMng::GetPlayerBySerial( DWORD dwSerial )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( auto i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
	{
		CCachePlayer * pPlayer	= i->second;
		if( pPlayer->GetSerial() == dwSerial )
			return pPlayer;
	}
	return NULL;
}

void CPlayerMng::DestroyGarbage( void )
{
	DWORD dwTick	= GetTickCount() - SEC( 60 );
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( auto i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
	{
		CCachePlayer * pPlayer	= i->second;
		if( pPlayer->IsTimeover( dwTick ) )
			g_DPCacheSrvr.DestroyPlayer( pPlayer->GetNetworkId() );
#ifndef _DEBUG
		else if( !pPlayer->IsAlive() )
			DestroyGarbagePlayer( pPlayer );
		else
			SendKeepAlive( pPlayer );
#endif	// _DEBUG
	}
}

void CPlayerMng::DestroyGarbagePlayer(CCachePlayer * pPlayer )
{
	pPlayer->SetAlive(true);
	g_DPCacheSrvr.DestroyPlayer( pPlayer->GetNetworkId() );
}

void CPlayerMng::SendKeepAlive(CCachePlayer * pPlayer )
{
	BEFORESEND( ar, PACKETTYPE_KEEP_ALIVE );
	pPlayer->SetAlive(false);
	SEND( ar, &g_DPCacheSrvr, pPlayer->GetNetworkId() );
}

CPlayerMng*	CPlayerMng::Instance( void )
{
	static	CPlayerMng	sPlayerMng;
	return &sPlayerMng;
}
