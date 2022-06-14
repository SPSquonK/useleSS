#pragma once

#include <boost/container/static_vector.hpp>
#include <vector>

enum class ACCOUNT_CHECK {
	CHECK_OK,
	CHECK_1TIMES_ERROR,
	CHECK_3TIMES_ERROR,
};


class CAccountMgr final {
public:
	struct IPAddressCache {
		DWORD   m_dwIP;
		int	    m_nError;
		time_t  m_tmError;
	};

private:
	boost::container::static_vector<IPAddressCache, 3> m_cache;

public:
	[[nodiscard]] ACCOUNT_CHECK Check(DWORD dwIP);
	void SetError(bool isError);

private:
	ACCOUNT_CHECK MoveBackAndCheck(decltype(m_cache)::iterator iterator);
	void AddNewIp(DWORD dwIP);
};
