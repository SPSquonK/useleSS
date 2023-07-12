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
	LPCTSTR	pszState;
	u_long  uidPlayer;
	int		nserverindex;
	int		nExp1;
	int		nLevel;
	int		nJob;
	int		JobLv;
	int		nFlightLv;
	int		nStr;
	int		nDex;
	int		nInt;
	int		nSta;
	int		nRemainGP;
	int		nRemainLP;
	int		nWorldID;
	LPCTSTR pszkilled_szName;
	float	vPos_x;
	float	vPos_y;
	float	vPos_z;
	int		nAttackPower;
	int		nMax_HP;
	LPCTSTR	pszGetidPlayer;
	DWORD	dwGold;
	int		nRemainGold;
	SERIALNUMBER nItem_UniqueNo;
	LPCTSTR pszItem_Name;
	int		nItem_durability;
	int		nItem_count;
	int		nItemAddLv;
	LPCTSTR pszStart_Time;
	int		nTotalPlayerTime;
	LPCTSTR pszRemoteIP;
	int		nQuest_Index;
	u_long	uKill_idPlayer;
	
	LOG_SM_ITEM_INFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		pszState = "";
		uidPlayer = nserverindex = 0;
		nExp1 = nLevel = nJob = JobLv = nFlightLv = 0;
		nStr = nDex = nInt = nSta = 0;
		nRemainGP = nRemainLP = nWorldID = 0;
		pszkilled_szName = "";
		vPos_x = vPos_y = vPos_z = 0.0f;
		nAttackPower = nMax_HP = 0;
		pszGetidPlayer = "";
		dwGold = nRemainGold = nItem_UniqueNo = 0;
		pszItem_Name = "";
		nItem_durability = nItem_count = nItemAddLv = 0;
		pszStart_Time = "";
		nTotalPlayerTime = 0;
		pszRemoteIP = "";
		nQuest_Index = uKill_idPlayer = 0;
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
/*
#ifdef __S0114_RELOADPRO
	void	QueryReloadProject( CQuery& query, LPDB_OVERLAPPED_PLUS pOV );
	void	Load_ReloadAccount();
#endif // __S0114_RELOADPRO
	*/
};

extern CDbManager g_DbManager;

u_int	DbWorkerThread(CDbManager * lpDbManager);	// DbWorkerThread

extern CQuery::Credentials dbLogin;
extern CQuery::Credentials dbLog;
extern CQuery::Credentials dbBilling;

