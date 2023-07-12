#pragma once

#include "DPMng.h"
#include "ServerDesc.h"
#include <memory>

class CCachePlayer;
class CDPClient final : public CDPMng {
public:
	CServerDesc * m_pServer = nullptr;

public:
	// We do not really want this class to be copied because of m_pServer
	// that has a weird ownership model.
	CDPClient() = default;
	CDPClient(const CDPClient &) = delete;
	CDPClient & operator=(const CDPClient &) = delete;
	~CDPClient() override = default;

	// Operations
	void SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) override;
	void UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) override;

	void	SendToServer(DPID dpidUser, LPVOID pData, DWORD dwDataSize) {
		*reinterpret_cast<UNALIGNED DPID *>(pData) = dpidUser;
		Send(pData, dwDataSize, DPID_SERVERPLAYER);
	}

	void	SendJoin(CCachePlayer * pPlayer);

	// Handlers
	void	OnReplace(CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize);
	void	OnQueryDestroyPlayer(CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize);
};

class CDPClientArray final {
public:
	CMclCritSec		m_AddRemoveLock;
	CServerDescArray	m_apServer;
	std::vector<CDPClient *> m_active;
	std::vector<CDPClient *> m_free;

public:
	// Constructions
	CDPClientArray() = default;
	CDPClientArray(const CDPClientArray &) = delete;
	CDPClientArray & operator=(const CDPClientArray &) = delete;
	virtual	~CDPClientArray();
	void Free();

	// Operations
	bool Connect(std::unique_ptr<CServerDesc> pServer);
	bool Remove(CDPClient * pRemove);

	CDPClient*	GetClient( u_long uIdofMulti );
	void	SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize );
};

extern CDPClientArray g_DPClientArray;
