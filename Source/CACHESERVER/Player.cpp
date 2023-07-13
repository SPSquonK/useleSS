#include "StdAfx.h"
#include "Player.h"
#include "dpcachesrvr.h"
#include "dpcoreclient.h"
#include <ranges>

CCachePlayerMng g_CachePlayerMng;

CCachePlayer::CCachePlayer(DPID dpidUser, DWORD dwSerial)
	:
	m_dpid(dpidUser),
	m_dwSerial(dwSerial),
	m_dwCreation(GetTickCount()) {
}

void CCachePlayer::Join( CAr & ar )
{
	ar >> m_idPlayer >> m_dwAuthKey;
	ar >> m_idParty >> m_idGuild >> m_idWar >> m_uChannel >> m_nSlot;
	ar.ReadString( m_szPlayer, MAX_PLAYER );
	ar.ReadString( m_szAccount, MAX_ACCOUNT );
	ar.ReadString( m_szPass, MAX_PASSWORD );
}

////////////////////////////////////////////////////////////////////////////////

void CCachePlayerMng::Clear(void) {
	CMclAutoLock Lock(m_AddRemoveLock);
	m_mapPlayers.clear();
}

bool CCachePlayerMng::AddPlayer(const DPID dpid) {
	CMclAutoLock Lock(m_AddRemoveLock);
	const auto r = m_mapPlayers.emplace(dpid,
		std::make_unique<CCachePlayer>(dpid, InterlockedIncrement(&m_lSerial))
	);
	return r.second;
}

CCachePlayer * CCachePlayerMng::GetPlayer(const DPID dpid) {
	const auto it = m_mapPlayers.find(dpid);
	if (it == m_mapPlayers.end()) return nullptr;
	return it->second.get();
}

bool CCachePlayerMng::RemovePlayer(const DPID dpid) {
	CMclAutoLock Lock(m_AddRemoveLock);
	return m_mapPlayers.erase(dpid) > 0;
}

void CCachePlayerMng::DestroyPlayer(const CDPClient * const pClient) {
	CMclAutoLock Lock(m_AddRemoveLock);
	// Mutex is locked = m_mapPlayers cannot be modified

	if (pClient) {
		// ~ Destroy players on WorldServer pClient
		for (const auto & pPlayer : m_mapPlayers | std::views::values
			) {
			if (pPlayer->GetClient() == pClient) {
				g_DPCoreClient.SendDestroyPlayer(*pPlayer);
				g_DPCacheSrvr.DestroyPlayer(pPlayer->GetNetworkId());
			}
		}
	} else {
		// ~ DestroyAllPlayers (= Core is dead)
		for (const auto & player : m_mapPlayers | std::views::values) {
			g_DPCacheSrvr.DestroyPlayer(player->GetNetworkId());
		}
	}
}

CCachePlayer * CCachePlayerMng::GetPlayerBySerial(const DWORD dwSerial) {
	const auto rng = m_mapPlayers | std::views::values;
	const auto it = std::ranges::find_if(rng,
		[dwSerial](const std::unique_ptr<CCachePlayer> & player) { return player->GetSerial() == dwSerial; }
	);

	if (it == rng.end()) return nullptr;
	return (*it).get();
}

void CCachePlayerMng::DestroyGarbage() {
	const DWORD dwTick = GetTickCount() - SEC(60);
	CMclAutoLock	Lock(m_AddRemoveLock);

	for (auto & pPlayer : m_mapPlayers | std::views::values) {
		if (pPlayer->IsTimeover(dwTick))
			g_DPCacheSrvr.DestroyPlayer(pPlayer->GetNetworkId());
#ifndef _DEBUG
		else if (!pPlayer->IsAlive())
			DestroyGarbagePlayer(pPlayer.get());
		else
			SendKeepAlive(pPlayer.get());
#endif	// _DEBUG
	}
}

void CCachePlayerMng::DestroyGarbagePlayer(CCachePlayer * pPlayer) {
	pPlayer->SetAlive(true);
	g_DPCacheSrvr.DestroyPlayer(pPlayer->GetNetworkId());
}

void CCachePlayerMng::SendKeepAlive(CCachePlayer * pPlayer) {
	pPlayer->SetAlive(false);

	BEFORESEND(ar, PACKETTYPE_KEEP_ALIVE);
	SEND(ar, &g_DPCacheSrvr, pPlayer->GetNetworkId());
}
