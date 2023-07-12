#pragma once

#include <DPlay.h>
#include <map>
#include <string>
#include "MyMap.h"
#include "mempooler.h"

#include "dpmng.h"

#define	AUTH_PERIOD		300

class CLoginUser final {
public:
	DPID		m_dpid;
	DWORD		m_dwTime;
	char		m_pKey[MAX_ACCOUNT];
	DWORD		m_idPlayer;
	DWORD		m_tPingRecvd;
	DWORD		m_dwAuthKey;
	BOOL		m_bIllegal;
	time_t	m_tAuth;
	int			m_nWrite;
	int			m_nRead;
	BOOL		m_bAuth;

public:
	// Constructions
	CLoginUser( DPID dpid );

	void	SetExtra( const char* pKey, DWORD dwAuthKey );
	void	SetAuth( BOOL bAuth ) { m_bAuth = bAuth; }
	BOOL	GetAuth() { return m_bAuth; }

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CLoginUser>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CLoginUser::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CLoginUser::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{ CLoginUser::m_pPool->Free( (CLoginUser *)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{ CLoginUser::m_pPool->Free( (CLoginUser*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

using C2User = std::map<DPID, CLoginUser *>;

class CLoginUserMng
{
private:
	C2User	m_dpid2User;
	std::map<std::string, CLoginUser *>	m_ac2User;
	u_long	m_uCount = 0;

public:
	CMclCritSec		m_AddRemoveLock;
	
public:
//	Constructions
	virtual	~CLoginUserMng();
//	Operations

	void	Free( void );
	BOOL	AddUser( DPID dpid, CLoginUser * pUser );
	BOOL	AddUser( const char* pKey, CLoginUser * pUser );
	BOOL	RemoveUser( DPID dpid );
	CLoginUser *	GetUser( const char* pKey );
	CLoginUser *	GetUser( DPID dpid );
	u_long	GetCount( void );

	void	DestroyAbnormalPlayer( void );
};

inline u_long CLoginUserMng::GetCount( void )
{	
	CMclAutoLock	Lock( m_AddRemoveLock );
	return m_uCount;
}

extern CLoginUserMng g_LoginUserMng;
