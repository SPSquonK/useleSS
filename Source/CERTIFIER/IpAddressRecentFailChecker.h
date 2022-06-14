#pragma once

#include <boost/container/static_vector.hpp>

enum class ACCOUNT_CHECK { Ok, x1TimeError, x3TimeError };

class IpAddressRecentFailChecker final {
public:
	struct IPAddressCache {
		DWORD        m_dwIP;
		unsigned int m_nError;
		time_t       m_tmError;

		[[nodiscard]] ACCOUNT_CHECK GetState() const;
	};

private:
	boost::container::static_vector<IPAddressCache, 3> m_cache;

public:
	/// Checks if the given IP address can try to log in, or if it recently
	/// failed
	[[nodiscard]] ACCOUNT_CHECK Check(DWORD dwIP);
	/// Changes the error field of the IpAddressCache related to the ip of the
	/// last Check call
	void SetError(bool isError);

private:
	ACCOUNT_CHECK MoveBackAndCheck(decltype(m_cache)::iterator iterator);
	void AddNewIp(DWORD dwIP);
};
