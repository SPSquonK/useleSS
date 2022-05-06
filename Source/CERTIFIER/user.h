#pragma once

#include <dplay.h>
#include <map>
#include <string>
#include "dpmng.h"
#include <memory>
#include <mutex>

class CCertUser final {
public:
	DPID	m_dpid;
	DWORD	m_dwTick;
	BOOL	m_bValid;

public:
	explicit CCertUser(DPID dpid);

	void SetAccount(const char * const szAccount ) {
		strncpy(m_pszAccount, szAccount, MAX_ACCOUNT);
		m_pszAccount[MAX_ACCOUNT - 1] = '\0';
	}

	const char * GetAccount(void) { return m_pszAccount; }

private:
	char	m_pszAccount[MAX_ACCOUNT];
};

class CCertUserMng
{
public:
	CCertUserMng() = default;
	CCertUserMng(const CCertUserMng &) = delete;
	CCertUserMng & operator=(const CCertUserMng &) = delete;
	virtual	~CCertUserMng();

	bool AddUser(DPID dpid);
	bool RemoveUser(DPID dpid);
	void ClearDum(CDPMng * pdp);

	bool AccountCheckOk(DPID dpid, const char * szBak);
	bool SetAccount(DPID dpid, const char * accountName);
	void KeepAlive(DPID dpid);
	void PrintUserAccount(DPID dpid);

private:
	CCertUser * GetUser( DPID dpid );

	std::mutex m_AddRemoveLock;
	std::map<DPID, std::unique_ptr<CCertUser>> m_users;
};

extern CCertUserMng g_CertUserMng;
