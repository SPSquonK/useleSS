#pragma once

#include <boost/container/small_vector.hpp>
#include "Misc.h"

class CAr;

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
};


