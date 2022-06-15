#pragma once

#include <boost/container/small_vector.hpp>
#include "Misc.h"

class CAr;
class CScanner;

class CListedServers {
private:
  static constexpr size_t MaxLength = 128;

  boost::container::static_vector<SERVER_DESC, MaxLength> m_servers;


public:

  friend CAr & operator<<(CAr & ar, const CListedServers & self);
  friend CAr & operator>>(CAr & ar,       CListedServers & self);

  [[nodiscard]]       SERVER_DESC * GetFromUId(u_long uKey);
  [[nodiscard]] const SERVER_DESC * GetFromUId(u_long uKey) const;

  [[nodiscard]]       SERVER_DESC * Get(DWORD dwParent, DWORD dwId);
  [[nodiscard]] const SERVER_DESC * Get(DWORD dwParent, DWORD dwId) const;

#ifdef __ACCOUNT
  void EmplaceNew(CScanner & scanner);
#endif

  using Channels = boost::container::small_vector<const SERVER_DESC *, MaxLength>;

  template<typename F>
  void ForEachServerAndChannels(F f) const
    requires (std::is_invocable_v<F, const SERVER_DESC &, const Channels &>) {

    for (const SERVER_DESC & server : m_servers) {
      if (server.dwParent != NULL_ID) continue;

      Channels channels;
      for (const SERVER_DESC & channel : m_servers) {
        if (channel.dwParent == server.dwID) {
          channels.push_back(&channel);
        }
      }

      f(server, channels);
    }
  }

};


