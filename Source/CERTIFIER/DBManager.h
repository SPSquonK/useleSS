#pragma once

#include <boost/pool/object_pool.hpp>

#include <thread>
#include "query.h"
#include "ar.h"
#include "mempool.h"
#include <dplay.h>
#include <map>
#include <mutex>
#include <memory>
#include <string>
#include <set>

namespace sqktd {
	using shared_ptr_timed_mutex = std::shared_ptr<std::timed_mutex>;
}

enum QUERYMODE
{	
	NEW_ACCOUNT, 
	CERTIFY, 
	CLOSE_EXISTING_CONNECTION,	
};

struct ACCOUNT_INFO {
	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_PASSWORD];
#ifdef __TWN_LOGIN0816
	char	szSessionPwd[MAX_SESSION_PWD];
#endif	// __TWN_LOGIN0816
#ifdef __EUROPE_0514
	char	szBak[MAX_ACCOUNT];
#endif	// __EUROPE_0514
	DWORD	dwPCBangClass;
};

typedef	struct tagDB_OVERLAPPED_PLUS
{
	int				nQueryMode;
	ACCOUNT_INFO	AccountInfo;
	DWORD			dpId;
	DWORD			dwIP;		//connected ip
}	DB_OVERLAPPED_PLUS, *LPDB_OVERLAPPED_PLUS;

class IpAddressRecentFailChecker;

#ifdef __GPAUTH
struct GPAUTH_RESULT {
	int		nResult         = -1;
	char	szResult[256]   = "";
	char	szGPotatoID[20] = "";
	int		nGPotatoNo      = 0;
	char	szNickName[20]  = "";
	BOOL	bGM             = FALSE;
#ifdef __GPAUTH_02
	char	szCheck[255]    = "";
#endif	// __GPAUTH_02
};
#endif	// __GPAUTH


class CDbManager {
public:
	static constexpr size_t DEFAULT_DB_WORKER_THREAD_NUM = 8;

	struct Worker {
		std::jthread thread;
		HANDLE ioCompletionPort = nullptr;
	};

private:
	boost::object_pool<DB_OVERLAPPED_PLUS>           m_memoryPool{ 512 };
	std::array<Worker, DEFAULT_DB_WORKER_THREAD_NUM> m_workers;

public:
	std::set<std::string>						m_eveSchoolAccount;
	char							m_szLoginPWD[256] = "";

public:
	~CDbManager();

	BOOL	CreateDbWorkers();
	void	GetStrTime( CTime *time, char *strbuf );
	void	Certify( CQuery & query, LPDB_OVERLAPPED_PLUS pData, IpAddressRecentFailChecker & accountMgr );
	void	CloseExistingConnection( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#ifdef __GPAUTH
	void	Certify2( CQuery & query, LPDB_OVERLAPPED_PLUS pov, IpAddressRecentFailChecker & mgr );
	void	CloseExistingConnection2( CQuery & query, LPDB_OVERLAPPED_PLUS pov );

	#ifdef __GPAUTH_03
	void	SQLAddAccount( CQuery & query, char* szAccount, char* szPassword, BOOL bGM );
	#else	// __GPAUTH_03
	void	SQLAddAccount( CQuery & query, char* szAccount, char* szPassword );
	#endif	// __GPAUTH_03

#endif	// __GPAUTH


	void	DBQryAccount( char* qryAccount, LPDB_OVERLAPPED_PLUS pData );
	BOOL	LoadEveSchoolAccount( void );
	[[nodiscard]] bool IsEveSchoolAccount(const char * pszAccount) const;
	BYTE	GetAccountFlag( int f18, LPCTSTR szAccount );
	void	OnCertifyQueryOK( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, const char* szCheck = "" );
	void	PostQ( LPDB_OVERLAPPED_PLUS pData );

	[[nodiscard]] DB_OVERLAPPED_PLUS * Alloc() { return m_memoryPool.construct(); }
	void DeAlloc(DB_OVERLAPPED_PLUS * const overlappedPlus) { m_memoryPool.destroy(overlappedPlus); }
};

extern CDbManager g_DbManager;

void	DbWorkerThread(HANDLE hIOCP, sqktd::shared_ptr_timed_mutex mutex);

#ifdef __GPAUTH
void	GetGPAuthResult( const char* szUrl, int nMode, int nGameMode, const char* sAccount, const char* sPassword, const char* sAddr, GPAUTH_RESULT & result );
void GPotatoAuthWorker(HANDLE hIOCP, sqktd::shared_ptr_timed_mutex mutex);
#endif	// __GPAUTH

