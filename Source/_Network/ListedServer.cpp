#include "StdAfx.h"
#include "ar.h"
#include "Misc.h"
#include "ListedServer.h"



CAr & operator<<(CAr & ar, const SERVER_DESC & self) {
  ar << self.dwParent;
  ar << self.dwID;
  ar.WriteString(self.lpName);
  ar.WriteString(self.lpAddr);
  ar << self.b18;
  ar << self.lCount;
  ar << self.lEnable;
  ar << self.lMax;
  return ar;
}

CAr & operator>>(CAr & ar, SERVER_DESC & self) {
  ar >> self.dwParent;
  ar >> self.dwID;
  ar.ReadString(self.lpName);
  ar.ReadString(self.lpAddr);
  ar >> self.b18;
  ar >> self.lCount;
  ar >> self.lEnable;
  ar >> self.lMax;
  return ar;
}

CAr & operator<<(CAr & ar, const CListedServers & self) {
  return ar << self.m_servers;
}


CAr & operator>>(CAr & ar, CListedServers & self) {
  return ar >> self.m_servers;
}

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
