#pragma once

#include <map>
#include <memory>
#include "dpclient.h"

class CCachePlayer final {
public:
	CCachePlayer(DPID dpidUser, DWORD dwSerial);

	[[nodiscard]] auto GetAuthKey()   const noexcept -> DWORD        { return m_dwAuthKey; }
	[[nodiscard]] auto GetNetworkId() const noexcept -> DPID         { return m_dpid; }
	[[nodiscard]] auto GetPlayerId()  const noexcept -> u_long       { return m_idPlayer; }
	[[nodiscard]] auto GetSerial()    const noexcept -> DWORD        { return m_dwSerial; }
	[[nodiscard]] auto GetChannel()   const noexcept -> u_long       { return m_uChannel; }
	[[nodiscard]] auto GetParty()     const noexcept -> u_long       { return m_idParty; }
	[[nodiscard]] auto GetGuild()     const noexcept -> u_long       { return m_idGuild; }
	[[nodiscard]] auto GetWar()       const noexcept -> WarId        { return m_idWar; }
	[[nodiscard]] auto GetSlot()      const noexcept -> BYTE         { return m_nSlot; }
	[[nodiscard]] auto GetPlayer()    const noexcept -> const char * { return m_szPlayer; }
	[[nodiscard]] auto GetAccount()   const noexcept -> const char * { return m_szAccount; }
	[[nodiscard]] auto GetPass()      const noexcept -> const char * { return m_szPass; }
	[[nodiscard]] auto GetAddr()      const noexcept -> const char * { return m_lpAddr; }

	[[nodiscard]] bool IsTimeover(DWORD dwTick) const noexcept { return !m_pClient && m_dwCreation < dwTick; }

	void	Join(CAr & ar);
	void	SetAddr(CDPMng * pdpMng) { pdpMng->GetPlayerAddr(GetNetworkId(), m_lpAddr); }

	[[nodiscard]] auto GetClient() const noexcept -> CDPClient * { return m_pClient; }
	void SetClient(CDPClient * const client) noexcept { m_pClient = client; }

	[[nodiscard]] bool IsAlive() const noexcept { return m_bAlive; }
	void SetAlive(bool value) noexcept { m_bAlive = value; }

private:
	// readonly data
	DWORD	m_dwSerial;
	DPID	m_dpid;
	DWORD	m_dwCreation;
	DWORD	m_dwAuthKey = 0;
	char	m_szPlayer[MAX_PLAYER]   = "";
	char	m_szAccount[MAX_ACCOUNT] = "";
	char	m_szPass[MAX_PASSWORD]   = "";
	u_long	m_idPlayer = 0;
	char	m_lpAddr[16] = "";
	u_long	m_uChannel = 0;
	u_long	m_idParty  = 0;
	u_long	m_idGuild  = 0;
	WarId	m_idWar      = WarIdNone;
	BYTE	m_nSlot      = 0;

	// Not read only but yay getters and setters
	CDPClient * m_pClient = nullptr;
	bool        m_bAlive  = true;
};

class CCachePlayerMng final {
public:
	CMclCritSec	m_AddRemoveLock;

public:
	// Locks the mutex
	~CCachePlayerMng() { Clear(); }
	void Clear();
	bool AddPlayer(DPID dpid);
	bool RemovePlayer(DPID dpid);
	
	CCachePlayer *	GetPlayerBySerial( DWORD dwSerial );
	int		GetCount( void )	{	return m_mapPlayers.size();		}
	void	DestroyPlayer(const CDPClient * pClient);
	void	DestroyGarbage( void );

	// Does not lock the mutex
	CCachePlayer * GetPlayer(DPID dpid);

private:
	void	DestroyGarbagePlayer(CCachePlayer * pPlayer);
	void	SendKeepAlive(CCachePlayer * pPlayer);
private:
	long	m_lSerial = 0;
	std::map<DPID, std::unique_ptr<CCachePlayer>> m_mapPlayers; // never points to null
};

extern CCachePlayerMng g_CachePlayerMng;
