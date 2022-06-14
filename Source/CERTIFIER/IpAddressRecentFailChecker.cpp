#include "stdafx.h"
#include "IpAddressRecentFailChecker.h"
#include <algorithm>
#include "..\Resource\lang.h"

//////////////////////////////////////////////////////////////////////

ACCOUNT_CHECK IpAddressRecentFailChecker::Check(const DWORD dwIP) {
	// Is the IP in the list?
	const auto it = std::ranges::find_if(m_cache,
		[dwIP](const IPAddressCache & ptr) {
			return ptr.m_dwIP == dwIP;
		});
	
	if (it != m_cache.end()) {
		// Yes, use the already stored data for this IP
		return MoveBackAndCheck(it);
	} else {
		// No: add in the list
		AddNewIp(dwIP);
		return ACCOUNT_CHECK::Ok;
	}
}

ACCOUNT_CHECK IpAddressRecentFailChecker::MoveBackAndCheck(
	const decltype(IpAddressRecentFailChecker::m_cache)::iterator iterator
) {
	const IPAddressCache pInfo = *iterator;
	m_cache.erase(iterator);
	m_cache.emplace_back(pInfo);
	return pInfo.GetState();
}

ACCOUNT_CHECK IpAddressRecentFailChecker::IPAddressCache::GetState() const {
	static constexpr DWORD localhost = 0xFF000001;
	if (m_dwIP == localhost) return ACCOUNT_CHECK::Ok;

	const time_t tmCur = time(nullptr);

	if (m_nError >= 3) {
		long nSec = tmCur - m_tmError;
		if (nSec <= (15 * 60)) return ACCOUNT_CHECK::x3TimeError;
	} else if (m_nError >= 1 && ::GetLanguage() != LANG_KOR) {
		long nSec = tmCur - m_tmError;
		if (nSec <= 15) return ACCOUNT_CHECK::x1TimeError;
	}

	return ACCOUNT_CHECK::Ok;
}

void IpAddressRecentFailChecker::AddNewIp(const DWORD dwIP) {
	// Ensure has space
	if (m_cache.size() == m_cache.max_size()) {
		m_cache.erase(m_cache.begin());
	}
	
	// Add
	const time_t tmCur = time(nullptr);
	IPAddressCache pInfo = {
		.m_dwIP = dwIP,
		.m_nError = 0,
		.m_tmError = tmCur
	};

	m_cache.emplace_back(pInfo);
}

void IpAddressRecentFailChecker::SetError(const bool isError) {
	// TODO: this API is hell ("modifies the last call of CHECK" = terrible). Fix it
	IPAddressCache & pInfo = m_cache.front();
	
	if (isError) {
		++pInfo.m_nError;
	} else {
		pInfo.m_nError = 0;
	}

	pInfo.m_tmError = time(NULL);
}


//////////////////////////////////////////////////////////////////////

[[maybe_unused]] static void test_AccountMgr() {
	static constexpr auto testAssert = [](const bool ok) {
		if (!ok) {
			throw "Assert failed";
		}
	};

	const DWORD testerIp = 0x26137822; // Arbitrary value because we need one

	IpAddressRecentFailChecker mgr;
	ACCOUNT_CHECK check = mgr.Check(testerIp);
	testAssert(check == ACCOUNT_CHECK::Ok);
	mgr.SetError(true);
	check = mgr.Check(testerIp);
	testAssert(check == ACCOUNT_CHECK::x1TimeError);

	Sleep(1000 * 16);

	check = mgr.Check(testerIp);
	testAssert(check == ACCOUNT_CHECK::Ok);

	mgr.SetError(true);
	mgr.SetError(true);
	mgr.SetError(true);

	check = mgr.Check(testerIp);
	testAssert(check == ACCOUNT_CHECK::x3TimeError);

	Sleep(1000 * 60 * 15 + 1000);

	check = mgr.Check(testerIp);
	testAssert(check == ACCOUNT_CHECK::Ok);
}
