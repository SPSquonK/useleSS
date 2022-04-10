#pragma once

#include <dplay.h>
#include <map>
#include <string>
#include "dpmng.h"

class CCertUser
{
public:
	DPID	m_dpid;
	DWORD	m_dwTick;
	BOOL	m_bValid;

public:
	CCertUser( DPID dpid );
	virtual	~CCertUser()	{}

	void SetAccount( const char* szAccount )
		{	strncpy( m_pszAccount, szAccount, MAX_ACCOUNT );	m_pszAccount[MAX_ACCOUNT-1]		= '\0';		}
	const char*	GetAccount( void )	{	return m_pszAccount;	}

private:
	char	m_pszAccount[MAX_ACCOUNT];
};

class CCertUserMng : public std::map<DPID, CCertUser*>
{
public:
	CMclCritSec		m_AddRemoveLock;
public:
	CCertUserMng();
	virtual	~CCertUserMng();

	BOOL	AddUser( DPID dpid );
	BOOL	RemoveUser( DPID dpid );
	CCertUser*	GetUser( DPID dpid );
	void	ClearDum( CDPMng* pdp );

	static	CCertUserMng*	GetInstance( void );
};
