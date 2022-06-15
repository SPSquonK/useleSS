#pragma once

#include <boost/container/small_vector.hpp>
#include <algorithm>
#include <ranges>
#include <span>
#include "Misc.h"

class CAr;
class CScanner;

#ifdef __ACCOUNT
class CAccount;
#endif

/// The list of servers.
/// 
/// The list is build by AccountServer and shared with Certifier and Neuz
/// through CAr.
class CListedServers {
public:
  static constexpr size_t MaxServers = 12;
  static constexpr size_t MaxChannels = 12;

  struct Channel {
    DWORD	dwID = NULL_ID;
    char	lpName[36] = "";
//    char	lpAddr[16] = "";
    BOOL	b18 = FALSE;
    long	lCount = 0;
    long	lEnable = 0;
    long	lMax = 0;

    friend CAr & operator<<(CAr & ar, const Channel & self);
    friend CAr & operator>>(CAr & ar, Channel & self);
  };

  struct Server {
    DWORD	dwID = NULL_ID;
    char	lpName[36] = "";
    char	lpAddr[16] = "";
//    BOOL	b18 = FALSE;
//    long	lCount = 0;
    long	lEnable = 0;
//    long	lMax = 0;
    boost::container::static_vector<Channel, MaxChannels> channels;

    friend CAr & operator<<(CAr & ar, const Server & self);
    friend CAr & operator>>(CAr & ar, Server & self);
  };



private:
  boost::container::static_vector<Server, MaxServers> m_servers;

public:

  [[nodiscard]] std::span<      Server> GetServers()       { return m_servers; }
  [[nodiscard]] std::span<const Server> GetServers() const { return m_servers; }

  friend CAr & operator<<(CAr & ar, const CListedServers & self);
  friend CAr & operator>>(CAr & ar,       CListedServers & self);

#ifdef __ACCOUNT
  void EmplaceNew(CScanner & s);
#endif 
  
  [[nodiscard]]       Server * GetServer(DWORD serverId);
  [[nodiscard]] const Server * GetServer(DWORD serverId) const;

  [[nodiscard]]       Channel * GetChannel(DWORD serverId, DWORD channelId);
  [[nodiscard]] const Channel * GetChannel(DWORD serverId, DWORD channelId) const;

#ifdef __ACCOUNT
  [[nodiscard]]       Channel * GetChannel(const CAccount & account);
  [[nodiscard]] const Channel * GetChannel(const CAccount & account) const;  
#endif

  Channel * GetFromUId(DWORD uId) { return GetChannel(uId / 100, uId % 100); }

#ifdef __CLIENT
  Channel * GetChannelFromPos(int serverPos, int channelPos);
#endif
};


