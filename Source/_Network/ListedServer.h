#pragma once

#include <boost/container/small_vector.hpp>
#include <algorithm>
#include <ranges>
#include <span>
#include "Misc.h"

class CAr;
class CScanner;

/// The list of servers.
/// 
/// The list is build by AccountServer and shared with Certifier and Neuz
/// through CAr.
class CListedServers {
private:
  static constexpr size_t MaxLength = 128;

  boost::container::static_vector<SERVER_DESC, MaxLength> m_servers;


  [[nodiscard]] std::span<      SERVER_DESC> asSpan()       { return m_servers; }
  [[nodiscard]] std::span<const SERVER_DESC> asSpan() const { return m_servers; }

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




public:
  // Ranges

  [[nodiscard]] auto GetServers() const { return asSpan() | std::views::filter(IsAServer); }
  [[nodiscard]] auto GetServers()       { return asSpan() | std::views::filter(IsAServer); }

  [[nodiscard]] auto GetChannels(const SERVER_DESC & server) const { return asSpan() | std::views::filter(HasParent(server.dwID)); }
  [[nodiscard]] auto GetChannels(const SERVER_DESC & server)       { return asSpan() | std::views::filter(HasParent(server.dwID)); }

private:
  static bool IsAServer(const SERVER_DESC & srv) {
    return srv.dwParent == NULL_ID;
  }

  struct HasParent {
    DWORD m_id;

    explicit HasParent(DWORD id) : m_id(id) {}

    [[nodiscard]] bool operator()(const SERVER_DESC & srv) const {
      return srv.dwParent == m_id;
    }
  };
};


