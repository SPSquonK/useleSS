#pragma once

#include <boost/container/flat_set.hpp>
#include "ar.h"

using WorldId = DWORD;

class CServerDesc final {
public:
	u_long	m_uIdofMulti = 0;
	char	m_szAddr[16] = "";
	boost::container::flat_set<WorldId> m_lspJurisdiction;

private:
	u_long	m_uKey = 0;

public:
	void SetKey(const u_long uKey) {
		m_uKey	= uKey;
		m_uIdofMulti = uKey % 100;
	}

	[[nodiscard]] u_long GetKey() const { return m_uKey; }
	[[nodiscard]] u_long GetIdofMulti() const { return m_uIdofMulti; }

	[[nodiscard]] bool IsIntersected(WorldId dwWorldID) const {
		return m_lspJurisdiction.contains(dwWorldID);
	}

	friend CAr & operator<<(CAr & ar, const CServerDesc & self);
	friend CAr & operator>>(CAr & ar, CServerDesc & self);
};

class CServerDescArray final : public std::map<u_long, CServerDesc*> {
public:
	~CServerDescArray();
	CServerDesc * GetAt(ULONG uKey);
};

