#pragma once

#include "query.h"
#include "ar.h"
#include "mempool.h"
#include <dplay.h>
#include "account.h"
#include <thread>

#include <map>
#include <set>
#include <string>

enum QUERYMODE
{	
	ACCOUNT_ON,			// 유저로그인 
	ACCOUNT_OFF,		// 유저로그아웃  
	LOG_SM_ITEM,		// 유료화아이템 관련 
	RELOAD_PROJECT,		
};

// 길드쿼리문장을 만든다.
struct LOG_SM_ITEM_INFO
{
	LPCTSTR pszType;
	LPCTSTR	pszState = "";
	u_long  uidPlayer = 0;
	int		nserverindex = 0;
	int		nExp1 = 0;
	int		nLevel = 0;
	int		nJob = 0;
	int		JobLv = 0;
	int		nFlightLv = 0;
	int		nStr = 0;
	int		nDex = 0;
	int		nInt = 0;
	int		nSta = 0;
	int		nRemainGP = 0;
	int		nRemainLP = 0;
	int		nWorldID = 0;
	LPCTSTR pszkilled_szName = "";
	float	vPos_x = 0.f;
	float	vPos_y = 0.f;
	float	vPos_z = 0.f;
	int		nAttackPower = 0;
	int		nMax_HP = 0;
	LPCTSTR	pszGetidPlayer = "";
	DWORD	dwGold = 0;
	int		nRemainGold = 0;
	SERIALNUMBER nItem_UniqueNo = 0;
	LPCTSTR pszItem_Name = "";
	int		nItem_durability = 0;
	int		nItem_count = 0;
	int		nItemAddLv = 0;
	LPCTSTR pszStart_Time = "";
	int		nTotalPlayerTime = 0;
	LPCTSTR pszRemoteIP = "";
	int		nQuest_Index = 0;
	u_long	uKill_idPlayer = 0;
	
	explicit LOG_SM_ITEM_INFO(LPCTSTR pszQueryType) {
		pszType = pszQueryType;
	};
};



typedef	struct tagDB_OVERLAPPED_PLUS
{
	OVERLAPPED		Overlapped;
	int				nQueryMode;
	char			szQuery[4096];
	SAccountName szAccount;
	DWORD			dwKey;		// 요청, 로그아웃, 로그인, 응답의 케이스에 account를 구분하기 위해서 사용한다.
	int				nExtra;
	BYTE			lpBuf[2048];
	u_long			uBufSize;

}	DB_OVERLAPPED_PLUS, *LPDB_OVERLAPPED_PLUS;

class CDbManager
{
protected:
	static constexpr size_t DEFAULT_DB_WORKER_THREAD_NUM = 8;
	std::array<std::thread, DEFAULT_DB_WORKER_THREAD_NUM> m_hDbWorkerThreadTerminate;

public:
	CMemPool<DB_OVERLAPPED_PLUS>*	m_pDbIOData;
	HANDLE							m_hDbCompletionPort;
	BOOL							m_bTracking;
	BOOL							m_bLogItem;		// 상용화 아이템 로그 ( 예전 - __NOLOG )

public:
//	Constructions
	CDbManager();
	~CDbManager();

//	Operations
	void	CloseDbWorkers( void );
	void	CreateDbWorkers( void );

	void	SetTracking( BOOL bEnable );
	void	SetLogging( BOOL bEnable );
	void	DBQryAccount( char* qryAccount, const char* Gu, const char* szAccount = "" );
	void	UpdateTracking( BOOL bON, LPCTSTR lpszAccount );
	BOOL	AllOff();
	void	AccountOn( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	AccountOff( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogSMItem( CQuery & qryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	DBQryLogSMItem( char* szSql, const LOG_SM_ITEM_INFO & info);
};

extern CDbManager g_DbManager;

u_int	DbWorkerThread(CDbManager * lpDbManager);	// DbWorkerThread

extern CQuery::Credentials dbLogin;
extern CQuery::Credentials dbLog;
extern CQuery::Credentials dbBilling;

