#include "StdAfx.h"
#include "ar.h"
#include "Misc.h"
#include "ListedServer.h"
#ifdef __ACCOUNT
#include "account.h"
#endif

CAr & operator<<(CAr & ar, const CListedServers::Server & self) {
  ar << self.dwID << self.lEnable << self.lpAddr << self.lpName;
  ar << self.channels;
  return ar;
}

CAr & operator>>(CAr & ar, CListedServers::Server & self) {
  ar >> self.dwID >> self.lEnable >> self.lpAddr >> self.lpName;
  ar >> self.channels;
  return ar;
}

CAr & operator<<(CAr & ar, const CListedServers::Channel & self) {
  ar << self.dwID << self.lEnable << self.lpName << self.b18 << self.lCount << self.lMax;
  return ar;
}

CAr & operator>>(CAr & ar, CListedServers::Channel & self) {
  ar >> self.dwID >> self.lEnable >> self.lpName >> self.b18 >> self.lCount >> self.lMax;
  return ar;
}

CAr & operator<<(CAr & ar, const CListedServers & self) { return ar << self.m_servers; }
CAr & operator>>(CAr & ar,       CListedServers & self) { return ar >> self.m_servers; }


CListedServers::Server * CListedServers::GetServer(DWORD serverId) {
  const auto itServer = std::ranges::find_if(m_servers,
    [serverId](const CListedServers::Server & server) {
      return server.dwID == serverId;
    });

  if (itServer == m_servers.end()) return nullptr;

  return &*itServer;
}

const CListedServers::Server * CListedServers::GetServer(DWORD serverId) const {
  const auto itServer = std::ranges::find_if(m_servers,
    [serverId](const CListedServers::Server & server) {
      return server.dwID == serverId;
    });

  if (itServer == m_servers.end()) return nullptr;

  return &*itServer;
}

CListedServers::Channel * CListedServers::GetChannel(DWORD serverId, DWORD channelId) {
  CListedServers::Server * server = GetServer(serverId);
  if (!server) return nullptr;

  const auto itChannel = std::ranges::find_if(server->channels,
    [channelId](const CListedServers::Channel & channel) {
      return channel.dwID == channelId;
    });
  
  if (itChannel == server->channels.end()) return nullptr;

  return &*itChannel;
}

const CListedServers::Channel * CListedServers::GetChannel(DWORD serverId, DWORD channelId) const {
  const CListedServers::Server * server = GetServer(serverId);
  if (!server) return nullptr;

  const auto itChannel = std::ranges::find_if(server->channels,
    [channelId](const CListedServers::Channel & channel) {
      return channel.dwID == channelId;
    });

  if (itChannel == server->channels.end()) return nullptr;

  return &*itChannel;
}

#ifdef __ACCOUNT
CListedServers::Channel * CListedServers::GetChannel(const CAccount & account) {
  return GetChannel(account.m_dwIdofServer, account.m_uIdofMulti);
}

const CListedServers::Channel * CListedServers::GetChannel(const CAccount & account) const {
  return GetChannel(account.m_dwIdofServer, account.m_uIdofMulti);
}
#endif


/*
SERVER_DESC * CListedServers::GetFromUId(u_long uKey) {
  const auto it = std::ranges::find_if(m_servers,
    [uKey](const SERVER_DESC & desc) { return desc.dwParent * 100 + desc.dwID; }
  );
  return it != m_servers.end() ? &*it : nullptr;
}

const SERVER_DESC * CListedServers::GetFromUId(u_long uKey) const {
  const auto it = std::ranges::find_if(m_servers,
    [uKey](const SERVER_DESC & desc) { return desc.dwParent * 100 + desc.dwID; }
  );
  return it != m_servers.end() ? &*it : nullptr;
}

SERVER_DESC * CListedServers::Get(DWORD dwParent, DWORD dwId) {
  const auto it = std::ranges::find_if(m_servers,
    [dwParent, dwId](const SERVER_DESC & desc) { return desc.dwParent == dwParent && dwId == desc.dwID; }
  );
  return it != m_servers.end() ? &*it : nullptr;
}

const SERVER_DESC * CListedServers::Get(DWORD dwParent, DWORD dwId) const {
  const auto it = std::ranges::find_if(m_servers,
    [dwParent, dwId](const SERVER_DESC & desc) { return desc.dwParent == dwParent && dwId == desc.dwID; }
  );
  return it != m_servers.end() ? &*it : nullptr;
}

*/

#ifdef __CLIENT
CListedServers::Channel * CListedServers::GetChannelFromPos(int serverPos, int channelPos) {
  if (serverPos >= m_servers.size()) return nullptr;
  if (channelPos >= m_servers[serverPos].channels.size()) return nullptr;
  return &m_servers[serverPos].channels[channelPos];
}
#endif
