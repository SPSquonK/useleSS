/********************************************************************
	created:	2003/02/26
	created:	26:2:2003   16:49
	filename: 	c:\neurospace\program\_database\dbmanager.h
	file path:	c:\neurospace\program\_database
	file base:	dbmanager
	file ext:	h
	author:		Kim, pan-young
	
	purpose:	데이타 베이스 처리 클래스
*********************************************************************/

#ifndef _DB_MANAGER_H_
#define _DB_MANAGER_H_

#include "Query.h"
#include "DPlay.h"
#include "Ar.h"
#include "DPMng.h"
#include "mempooler.h"
#include "misc.h"
#include "..\_Network\Objects\Obj.h"
#include "AccountCacheMgr.h"

#ifdef __TRANS_0413
	const int	MAX_GETTHREAD_SIZE		= 8;
	const int	MAX_PUTTHREAD_SIZE		= 16;
#else	// __TRANS_0413
	//	mulcom	BEGIN100415
// 	const int	MAX_GETTHREAD_SIZE		= 4;
// 	const int	MAX_PUTTHREAD_SIZE		= 4;
	const int	MAX_GETTHREAD_SIZE		= 8;
	const int	MAX_PUTTHREAD_SIZE		= 16;
	//	mulcom	END100415
#endif	// __TRANS_0413
	const int	MAX_UPDATETHREAD_SIZE	= 8;

const int	MAX_JOIN_SIZE			= 16;
const int	MAX_TASKBAR        = MAX_SHORTCUT_STRING * 2 + 44; 
const int	MAX_APPLETTASKBAR	 = MAX_TASKBAR * MAX_SLOT_APPLET; 
const int	MAX_ITEMTASKBAR    = MAX_TASKBAR * MAX_SLOT_ITEM_COUNT * MAX_SLOT_ITEM;   
const char	NullStr[2] = "$";

#define	QUERY_SIZE	40960

enum QUERYMODE
{ 
	DB_CONNECT, DB_THREAD_END, CREATEACCOUNT, CREATEPLAYER, LOGIN, 
	REMOVEPLAYER, SEL_ECT, SENDPLAYERLIST, JOIN, SAVEPLAYER, SAVECONCURRENTUSERNUMBER,
	SAVE_PLAY_TIME, LOG_PLAY_CONNECT, LOG_ITEM, LOG_PLAY_DEATH, LOG_LEVELUP, 
	LOG_SERVER_DEATH, LOG_UNIQUEITEM, LOG_QUEST,
	CREATE_GUILD, DESTROY_GUILD, ADD_GUILD_MEMBER, REMOVE_GUILD_MEMBER,
	UPDATE_GUILD_LOGO, UPDATE_GUILD_CONTRIBUTION, UPDATE_GUILD_NOTICE,
	UPDATE_GUILD_MEMBER_LEVEL, UPDATE_GUILD_AUTHORITY, UPDATE_GUILD_PENYA, GUILD_SETNAME,
	UPDATE_GUILD_REALPAY, GUILD_BANK_UPDAATE, GUILD_BANK_QUERY, GUILD_RANKING_UPDATE, GUILD_RANKING_UPDATE_DB,
	ADD_GUILD_VOTE, REMOVE_GUILD_VOTE, CLOSE_GUILD_VOTE, CAST_GUILD_VOTE, 
	LOG_GUILD_CREATE, LOG_GUILD_REMOVE,
	ACPT_GUILD_WAR, WAR_END, SURRENDER, WAR_DEAD, WAR_MASTER_ABSENT,
	REMOVE_FRIEND, ADD_PARTYNAME, CHANGE_BANKPASS, GAMEMA_CHAT,
	INSERT_FREQUENCY,
	LOG_PK_PVP,
	INSERT_TAG,
	SCHOOL_REPORT,
	UPDATE_GUILD_CLASS, UPDATE_GUILD_NICKNAME,
	LOG_SCHOOL,
	QM_QUERYGUILDQUEST,
	QM_INSERTGUILDQUEST,
	QM_UPDATEGUILDQUEST,
	BS_TEST,
	QM_DELETEGUILDQUEST,
	WANTED_OP,
	QM_SET_PLAYER_NAME,
	QM_SNOOPGUILD,
	START_GUILDCOMBAT,
	IN_GUILDCOMBAT,
	OUT_GUILDCOMBAT,
	RESULT_GUILDCOMBAT,
	GETPENYAGUILD_GUILDCOMBAT,
	GETPENYAPLAYER_GUILDCOMBAT,
	CONTINUE_GUILDCOMBAT,
	ADD_MAIL,
	REMOVE_MAIL,
	REMOVE_MAIL_ITEM,
	REMOVE_MAIL_GOLD,
	READ_MAIL,
	QM_UPDATEGUILDMASTER,
	QM_DELETE_REMOVE_GUILD_BANK_TBL,
	QM_LOG_EXPBOX,
	QM_CALL_USPLOGGINGQUEST,
	QM_CALL_XXX_MULTI_SERVER,
	LOG_SKILLPOINT,
#ifdef __S_NEW_SKILL_2
	SAVE_SKILL,
#endif // __S_NEW_SKILL_2
	QM_CALL_USP_PET_LOG,
#ifdef __S_RECOMMEND_EVE
	RECOMMEND_EVE,
#endif // __S_RECOMMEND_EVE
#ifdef __GETMAILREALTIME
	QM_GETMAIL_REALTIME,
#endif // __GETMAILREALTIME
#ifdef __RT_1025
	QM_ADD_MESSENGER,
	QM_DELETE_MESSENGER,
	QM_UPDATE_MESSENGER,
#endif	// __RT_1025
	QM_GUILDBANK_LOG_VIEW,
	LOG_GETHONORTIME,
	QM_SEALCHAR,
	QM_SEALCHARCONM,
	QM_SEALCHARGET,
	QM_SEALCHARSET,
	GC1TO1_STATE,
	GC1TO1_LOAD,
	GC1TO1_TENDER,
	GC1TO1_LINEUP,
	GC1TO1_WARPERSON,
	GC1TO1_WARGUILD,
#ifdef __JEFF_FIX_0
	QM_QUERY_MAIL_BOX,
	QM_ALL_PLAYER_DATA,
#endif	// __JEFF_FIX_0
	LOG_INSTANCEDUNGEON,
#ifdef __ERROR_LOG_TO_DB
	LOG_ERROR,
#endif // __ERROR_LOG_TO_DB
	QM_LOGIN_PROTECT, 
	LOG_GUILDFURNITURE
};

typedef struct tagACCOUNT_INFO
{
	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_PASSWORD];
	char	szPlayer[MAX_PLAYER];
	int		nPlayerSize;
	int		nPlayerSlot;
}	ACCOUNT_INFO, *LPACCOUNT_INFO;

typedef struct tagDB_OVERLAPPED_PLUS
{
	OVERLAPPED	Overlapped;
	int		nQueryMode;
	//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	BYTE*	lpBuf;
	//BYTE	lpBuf[CAr::nGrowSize * 2];
	//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	u_long	uBufSize;
	ACCOUNT_INFO	AccountInfo;
	DPID	dpid;
	DPID	dpidCache;
	DPID	dpidUser;
	DWORD	dwAuthKey;
	tagDB_OVERLAPPED_PLUS()
	{
		//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		lpBuf	= NULL;
		//::memset( lpBuf, 0, sizeof(lpBuf) );
		//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		
		uBufSize = 0;
	}
}	DB_OVERLAPPED_PLUS,	*LPDB_OVERLAPPED_PLUS;

struct	MAIL_QUERYINFO
{
	LPCTSTR		pszType;
	int		nMail;
	u_long	idReceiver;
	u_long	idSender;
	int		nGold;
	time_t	tmCreate;
	BYTE	byRead;
	LPCTSTR		szTitle;
	LPCTSTR	szText;
	DWORD	dwItemId;
	int		nItemNum;
	int		nRepairNumber;
	int		nHitPoint;
	int		nMaxHitPoint;
	int		nMaterial;
	BYTE	byFlag;
	SERIALNUMBER	iSerialNumber;
	int		nOption;
	int		bItemResist;
	int		nResistAbilityOption;
	u_long	idGuild;
	int		nResistSMItemId;
	BYTE	bCharged;
	DWORD	dwKeepTime;
	__int64	iRandomOptItemId;
	int		nPiercedSize;
	DWORD	dwItemId1;
	DWORD	dwItemId2;
	DWORD	dwItemId3;
	DWORD	dwItemId4;
	DWORD	dwItemId5;
	BOOL	bPet;
	BYTE	nKind;
	BYTE	nLevel;
	DWORD	dwExp;
	WORD	wEnergy;
	WORD	wLife;
	BYTE	anAvailLevel[PL_MAX];
#ifdef __PET_1024
	char	szPetName[MAX_PET_NAME];
#endif	// __PET_1024

	MAIL_QUERYINFO( LPCTSTR pszQueryType )
		{
			pszType	= pszQueryType;
			nMail	= 0;
			idReceiver	= 0;
			idSender	= 0;
			nGold	= 0;
			tmCreate	= 0;
			byRead	= 0;
			szTitle	= "";
			szText	= "";
			dwItemId	= 0;
			nItemNum	= 0;
			nRepairNumber	= 0;
			nHitPoint	= 0;
			nMaxHitPoint	= 0;
			nMaterial	= 0;
			byFlag	= 0;
			iSerialNumber	= 0;
			nOption	= 0;
			bItemResist	= 0;
			nResistAbilityOption	= 0;
			idGuild	= 0;
			nResistSMItemId	= 0;
			bCharged	= 0;
			dwKeepTime	= 0;
			iRandomOptItemId	= 0;
			nPiercedSize	= 0;
			dwItemId1	= 0;
			dwItemId2	= 0;
			dwItemId3	= 0;
			dwItemId4	= 0;
			dwItemId5	= 0;
			bPet	= FALSE;
			nKind	= nLevel	= 0;
			dwExp	= 0;
			wEnergy	= wLife	= 0;
			memset( anAvailLevel, 0, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
			szPetName[0]	= '\0';
#endif // __PET_1024
		}
};

// 길드쿼리문장을 만든다.
struct GUILD_QUERYINFO
{
	LPCTSTR	pszType;
	u_long	idPlayer;
	u_long	idGuild;
	LPCTSTR	pszGuild;
	DWORD   dwLv1;
	DWORD   dwLv2;
	DWORD   dwLv3;
	DWORD   dwLv4;
	WORD	nLevel;
	int     nGuildGold;
	int     nGuildPxp;
	int     nWin;
	int		nLose;
	int     nSurrender;
	DWORD   dwLogo;
	int		nClass;
	LPCTSTR pszNotice;

	GUILD_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 1;
		idGuild = 1;
		pszGuild = "";
		dwLv1 = dwLv2 = dwLv3 = dwLv4 = 0;
		nLevel = 0;
		nGuildGold = 0;
		nGuildPxp = 0;
		nWin = 0;
		nLose = 0;
		nSurrender = 0;
		dwLogo = 0;
		nClass = 0;
		pszNotice = "";
	};
};

// 길드 투표 쿼리문장을 만든다.
struct VOTE_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idVote;
	u_long		idGuild;
	LPCTSTR		pszTitle;
	LPCTSTR		pszQuestion;
	LPCTSTR		pszSelections[4];
	BYTE		cbSelection;

	VOTE_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idVote = 0;
		idGuild = 0;
		pszTitle = "";
		pszQuestion = "";
		for( int i=0; i<4; ++i )
			pszSelections[i] = "";

		cbSelection = 0;
	};
}; 

// 길드 Log 쿼리문장을 만든다.
struct GUILDLOG_QUERYINFO
{
	LPCTSTR		pszType;			// 로그 타입
	u_long		idGuild;			// 길드 아이디
	u_long		idPlayer;			// 당한자
	u_long		idDoPlayer;			// 시킨자
	int			nGuildGold;			// 길드 골드
	int			nGuildPxp;			// 길드 Pxp
	int			nGuildLv;			// 길드 레벨
	int			nLevel;				// 당한자 레벨
	int 		nItem;				// 아이템 아이디 / 거래 페냐량
	int			nAbilityOption;		// 아이템 옵션
	int			nItem_count;		// 거래 갯수
	SERIALNUMBER	iUniqueNo;			// 유니크 넘버
	LPCTSTR		pszTime;			// 시각
	LPCTSTR		pszGuildBank;		// 길드 뱅크
	LPCTSTR		pszState;
	
	GUILDLOG_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idGuild = 0;	
		idPlayer = 0;	
		idDoPlayer = 0;	
		nGuildGold = 0;	
		nGuildPxp = 0;	
		nGuildLv = 0;	
		nLevel = 0;		
		nItem = 0;
		nAbilityOption = 0;
		nItem_count = 0 ;
		iUniqueNo = 0;
		pszTime = "";
		pszGuildBank = "";
		pszState = "";
	};
}; 

struct ADDBANK_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idPlayer;
	LPCTSTR		pszBank;
	LPCTSTR		pszBankIndex;
	LPCTSTR		pszObjIndexBank;
	LPCTSTR		pszExtBank;
	LPCTSTR		pszPirecingBank;
	DWORD		dwGoldBank;
	LPCTSTR		pszBankPet;

	ADDBANK_QUERYINFO( LPCTSTR pszQueryType )
	{
		pszType = pszQueryType;
		idPlayer = 0;
		pszBank = "";
		pszBankIndex = "";
		pszObjIndexBank = "";
		pszExtBank = "";
		pszPirecingBank = "";
		dwGoldBank = 0;
		pszBankPet	= "";
	};
};

typedef	struct	_PocketParam
{
	u_long		idPlayer;
	int		nPocket;
	const char*	pszItem;
	const char* pszIndex;
	const char* pszObjIndex;
	const char* pszExt;
	const char* pszPiercing;
	const char* pszPet;
	BOOL	bExpired;
	time_t	tExpirationDate;

	_PocketParam()
	{
		idPlayer	= 0;
		nPocket		= 0;
		pszItem		= "";
		pszIndex	= "";
		pszObjIndex		= "";
		pszExt	= "";
		pszPiercing		= "";
		pszPet	= "";
		bExpired	= TRUE;
		tExpirationDate		= 0;
	}
}	PocketParam;

struct WAR_QUERYINFO
{
	LPCTSTR	pszType;
	u_long	idWar;
	u_long	idGuild;
	u_long	idPlayer;
	u_long	f_idGuild;
	int		nWinPoint;
	int		f_nWinPoint;
	char	iState;

	WAR_QUERYINFO( LPCTSTR pszQueryType )
	{
		pszType = pszQueryType;
		idWar	= 0;
		idGuild		= 0;
		iState	= '0';
		f_idGuild	= 0;
		idPlayer	= 0;
		nWinPoint	= 0;
		f_nWinPoint	= 0;
	};
};

///////////////////////////////////////////////////////////////////////
// WANTED_QUERYINFO
///////////////////////////////////////////////////////////////////////

// 길드 투표 쿼리문장을 만든다.
struct WANTED_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idPlayer;
	int			nGold;
	long		nEnd;
	char		szMsg[WANTED_MSG_MAX + 1];

	WANTED_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 0;
		nGold = 0;
		nEnd = 0;
		szMsg[0] = '\0';
	};
}; 




///////////////////////////////////////////////////////////////////////
// ACCOUNT_CACHE
///////////////////////////////////////////////////////////////////////
struct ACCOUNT_CACHE
{
	time_t							m_tmPrevent;
	CMover*							pMover[3];
	u_long							m_idPlayerBank[3];
	CItemContainer< CItemElem  >	m_Bank[3];
	DWORD							m_dwGoldBank[3];
	time_t							m_tmLastAccess;
	string							m_strAccount;

	void	Init();
	void	Clear();
	void	ClearMover( BYTE nSlot, u_long idPlayer );
	BOOL	IsReleaseable();
#ifdef __INVALID_LOGIN_0320
private:
	u_long	m_uMultiServer;
public:
	u_long	GetMultiServer( void )	{	return m_uMultiServer;	}
	void	ChangeMultiServer( u_long uMultiServer )	{ m_uMultiServer	= uMultiServer;	}
	BOOL	IsPlaying( void )	{ return m_uMultiServer != 0;		}
#endif	// __INVALID_LOGIN_0320
};

inline void ACCOUNT_CACHE::Init()
{
	m_tmPrevent = 0;
	m_tmLastAccess = ::time( NULL );

	for( int i = 0 ; i < 3 ; ++i )
	{
		pMover[i] = NULL;
		m_Bank[i].SetItemContainer( ITYPE_ITEM, MAX_BANK ) ;
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
#ifdef __INVALID_LOGIN_0320
	m_uMultiServer	= 0;
#endif	// __INVALID_LOGIN_0320
}


inline void ACCOUNT_CACHE::Clear()
{
	for( int i = 0 ; i < 3 ; ++i )
	{
		SAFE_DELETE( pMover[i] );
	}
}

inline void ACCOUNT_CACHE::ClearMover( BYTE nSlot, u_long idPlayer )
{
	m_Bank[nSlot].Clear();
	m_dwGoldBank[nSlot]	= 0;
	m_idPlayerBank[nSlot] = idPlayer;
	SAFE_DELETE( pMover[nSlot] );
}


inline BOOL ACCOUNT_CACHE::IsReleaseable()
{
	time_t t	= m_tmLastAccess + TIMEWAIT_RELEASE;
	if( ::time( NULL ) > t )
		return TRUE;
	return FALSE;
}


#ifdef __TOOMANY_PENDINGLOG1115
#define	MAX_PENDING		1024
#endif	// __TOOMANY_PENDINGLOG1115

#define	MAX_QUERY_RESERVED	2

//typedef CMyMap2<CMover*>	C2Mover;
//typedef CMyBucket2<CMover*> CMoverBucket;
typedef	set<string>			SET_STRING;
typedef map<u_long, string>	ULONG2STRING;
typedef map<int, string>	INT2STRING;

typedef	struct	_ItemStruct
{
	char	szItem[512];
	char	szExt[64];
	char	szPiercing[256];

#ifdef __PET_1024
	char	szPet[100];
#else	// __PET_1024
	char	szPet[64];
#endif	// __PET_1024

	_ItemStruct()
		{
			szItem[0]	= '\0';
			szExt[0]	= '\0';
			szPiercing[0]	= '\0';
			szPet[0]	= '\0';
		}
}	ItemStruct, *PItemStruct;

typedef	struct	_ItemContainerStruct
{
	char	szItem[6144];
	char	szIndex[512];
	char	szObjIndex[512];
	char	szExt[2048];
	char	szPiercing[8000];
#ifdef __PET_1024
	char	szPet[4200];	// 42 * 100
#else	// __PET_1024
	char	szPet[2688];	// 42 * 64
#endif	// __PET_1024
	_ItemContainerStruct()
		{
			*szItem	= '\0';
			*szIndex	= '\0';
			*szObjIndex	= '\0';
			*szExt	= '\0';
			*szPiercing	= '\0';
			*szPet	= '\0';
		}
}	ItemContainerStruct, *PItemContainerStruct;

typedef	struct	_PocketStruct : public ItemContainerStruct
{
	BOOL	bExpired;
	time_t	tExpirationDate;
	_PocketStruct() : ItemContainerStruct()
	{
		bExpired	= TRUE;
		tExpirationDate		= 0;
	}
}	PocketStruct,	*PPocketStruct;


typedef	struct	_CONV_RESULT_ITEM
{
	DWORD	dwItemId;
	int		nAbilityOption;
	_CONV_RESULT_ITEM()
	{
		dwItemId	= 0;
		nAbilityOption	= 0;
	}
}	CONV_RESULT_ITEM;

class CGuildMng;
class CMail;

typedef	map<DWORD, CONV_RESULT_ITEM>	MDC;

class CDbManager
{
	struct __GUILDCOMBATJOIN
	{
		DWORD dwPenya;
		BOOL  bRequest;
#ifdef __S_BUG_GC
		u_long uGuildId;
#endif // __S_BUG_GC
	};
	struct __SendItemContents
	{
		char OneItem[MAX_PATH];
		int nNo, Item_Count;
		__int64 m_dwKeepTime;
		CItemElem itemElem;
	};
	struct __GCRESULTVALUEGUILD
	{
		int	nCombatID;			// 길드대전 아이디
		u_long uidGuild;		// 길드 아이디
		__int64 nReturnCombatFee;	// 돌려받을 참여금
		__int64 nReward;				// 보상금
	};
	struct __GCRESULTVALUEPLAYER
	{
		int nCombatID;			// 길드대전 아이디
		u_long uidGuild;		// 길드 아이디
		u_long uidPlayer;		// 플레이어 아이디
		__int64 nReward;			// 보상금
	};
	struct __GCPLAYERPOINT
	{
		u_long	uidPlayer;
		int		nJob;
		int		nPoint;
		
		__GCPLAYERPOINT()
		{
			uidPlayer = nJob = nPoint = 0;
		}
	};
	
	struct __MAIL_REALTIME
	{
		int nMail_Before;
		int nMail_After;
		SERIALNUMBER m_liSerialNumber;
		int m_nHitPoint;
	};

public:
#ifdef __ITEM_REMOVE_LIST
	SET_STRING	m_RemoveItem_List;
#endif // __ITEM_REMOVE_LIST
	INT2STRING		m_int2StrItemUpdate;

	CAccountCacheMgr m_AccountCacheMgr;
	CMclCritSec		m_AddRemoveLock;
	ULONG2STRING	m_2PartyNamePtr;
	CMclCritSec		m_joinLock;
	char			m_aszJoin[MAX_JOIN_SIZE][MAX_PLAYER];
	int				m_nJoin;
	CMclCritSec		m_csMapSavePlayer;
	map<u_long, int>	m_mapSavePlayer;		// m_idPlayer, nSlot
	HANDLE			m_hIOCPGet;
	HANDLE			m_hIOCPPut;
	HANDLE			m_hIOCPUpdate;
	HANDLE			m_hIOCPGuild;			// 길드 업뎃용 
#ifdef __S1108_BACK_END_SYSTEM
	HANDLE			m_hWorker;
	HANDLE			m_hCloseWorker;
#endif // __S1108_BACK_END_SYSTEM
	HANDLE			m_hItemUpdateWorker;
	HANDLE			m_hItemUpdateCloseWorker;
	int				m_nItemUpdate;

	DB_OVERLAPPED_PLUS*		AllocRequest( void );
	void	FreeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus );
	void	MakeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus,  LPBYTE lpBuf, u_long uBufSize );

#ifndef __TRANS_0413
	MemPooler<DB_OVERLAPPED_PLUS>*	m_pDbIOData;
#endif	// __TRANS_0413

#ifdef __J0826
	CMclCritSec		m_csCreatePlayer;
#endif	// __J0826

	vector<__GCRESULTVALUEGUILD>			m_GCResultValueGuild;		// 길드대전 결과값
	vector<__GCRESULTVALUEPLAYER>			m_GCResultValuePlayer;		// 길드대전 결과값
	vector<__GCPLAYERPOINT> m_vecGCPlayerPoint;

#ifdef __S_BUG_GC
	vector<__GUILDCOMBATJOIN>	m_vecGuildCombat;	
#else // __S_BUG_GC
	map<u_long, __GUILDCOMBATJOIN>	m_GuildCombat;	
#endif // __S_BUG_GC
	int		m_nGuildCombatIndex;
	u_long	m_uWinGuildId;
	u_long	m_uBestPlayer;
	int		m_nWinGuildCount;
//	CMclCritSec		m_csGuildCombat;

	char DB_ADMIN_PASS_LOG[256];
	char DB_ADMIN_PASS_CHARACTER01[256];
	char DB_ADMIN_PASS_BACKSYSTEM[256];
	char DB_ADMIN_PASS_ITEMUPDATE[256]; 

public:
	static CDbManager & GetInstance();
	~CDbManager();

	BOOL	RemoveQuest( void );

	void	PostSavePlayer( u_long idPlayer, BYTE nSlot );
	void	SavePlayer( CQuery *qry, CQuery* pQueryLog, CMover* pMover, char* szQuery );
	void	SavePlayTime( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );	
#ifdef __S_NEW_SKILL_2
	void	AllSaveSkill( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __S_NEW_SKILL_2	

	void	SaveHonor( CQuery *qry, u_long uidPlayer, int * aHonor, char* szQuery );

#ifdef __SKILL_0205
	void	SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, LPBYTE abUpdateSkill, char* szQuery );
#else	// __SKILL_0205
	void	SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, char* szQuery );
#endif	// __SKILL_0205
	void	SaveJobLv( CMover* pMover, char* szJobLv );
	void	SaveQuest( CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest, char* szCheckedQuest );

	void	SaveInventory( CMover* pMover, PItemContainerStruct pItemContainerStruct );
	void	SaveBank( CMover* pMover, CItemContainer<CItemElem>* pPlayerBank, PItemContainerStruct pItemContainerStruct );
	void	SaveGuildBank( CItemContainer<CItemElem>*  pGuildBank, PItemContainerStruct pItemContainerStruct );

	void	SaveEquipment( CMover* pMover, char* szEquipmen );
	void	SaveCardCube( CMover* pMover, char* szCard, char* szsCardIndex, char* szsCardObjIndex, char* szCube, char* szsCubeIndex, char* szsCubeObjIndex );
	void	SaveTaskBar( CMover* pMover, char* szAppletTaskBar, char* szItemTaskBar, char* szSkillTaskBar );
	void	SaveSMCode( CMover* pMover, char* szszSMTime );
	void	SaveSkillInfluence( CMover* pMover, char* szszSkillInfluence );
	void	MakeQueryPocket( char* szQuery, const PocketParam & p );
	void	DBQryAddBankSave( char* szSql, const ADDBANK_QUERYINFO & info );
	void	SaveOneItem( CItemElem* pItemElem, PItemStruct pItemStruct );
	void	SendPlayerList( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SendJoin( CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey, 
						  DWORD dwBillingClass, LPCTSTR szPartyName, LPCTSTR szBankPass, 
						  DWORD dwPlayTime, DWORD dwFlyTime, int nMaximumLevel, int nTags, TAG_ENTRY* tags,
						  LPCTSTR szTimeGuild );
	void	Join( CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CreatePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemovePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ChangeBankPass( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	Gamema_Chat( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );


//	u_long	GetPlayerID( LPCSTR lpszPlayer );
	BOOL	LoadPlayerData( void );
	BOOL	GetPartyName( void );
	void	SerializeGuildCombat( CAr & ar );
	void	SerializeResultValue( CAr & ar );
	void	SerializePlayerPoint( CAr & ar );
	BOOL	OpenGuildCombat( void );
	BOOL	LoadPost( void );
	void	GetItemFromMail( CQuery* pQuery, CItemElem* pItemElem );
#ifdef __POST_1204
	void	RemoveMail( list<CMail*> & lspMail, time_t t );
#else	// __POST_1204
	void	RemoveMail( list<CMail*> & lspMail );
#endif	// __POST_1204
	CQuery*	m_apQuery[MAX_QUERY_RESERVED];
	BOOL	QueryRemoveGuildBankTbl( void );
	void	DeleteRemoveGuildBankTbl( CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	call_uspLoggingQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
#ifdef __GETMAILREALTIME
	BOOL	QueryGetMailRealTime( CQuery* pQuery );
#endif // __GETMAILREALTIME

	void	LoadGC1to1TenderGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, DPID dpid );
	void	GC1to1State( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1Tender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1LineUp( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1WarPerson( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1WarGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	GuildBankLogView( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealChar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharConm( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharGet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharSet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	CalluspPetLog( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

	BOOL	GetMyPartyName( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, char * szPartyName );
	BOOL	BankToItemSendTbl( LPCSTR lpFileName );
	BOOL	InventoryToItemSendTbl( LPCSTR lpFileName );
	BOOL	ItemRemove0203( LPCSTR lpFileName );
	BOOL	RemoveInvalidItem();

	BOOL	Conv( LPCSTR lpFileName );
	BOOL	ConvInventory( map<DWORD, CONV_RESULT_ITEM> & mConv );
	BOOL	ConvBank( map<DWORD, CONV_RESULT_ITEM> & mConv );
	BOOL	ConvGuildBank( map<DWORD, CONV_RESULT_ITEM> & mConv );

	CQuery*		CreateCharQuery( void );
	BOOL	ConvertPocket( MDC & mConv );

	BOOL	RestorePet( LPCTSTR lpFileName );
	BOOL	RestorePetInventory( map<DWORD, int> & mRestore );
	BOOL	RestorePetBank( map<DWORD, int> & mRestore );
	BOOL	RestorePetGuildBank( map<DWORD, int> & mRestore );

	void	CreateGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	DestroyGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	AddGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenGuild( void );
	void	UpdateGuildMemberLv( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildClass( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildNickName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildMaster( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

#ifdef __TOOMANY_PENDINGLOG1115
	static	LONG	m_lPending;
	static	BOOL	IsTooManyPending( void );
	static	void	AddPending( void )	{	InterlockedIncrement( &m_lPending );	}
	static	void	ReleasePending( void )	{	InterlockedDecrement( &m_lPending );	}
#endif	// __TOOMANY_PENDINGLOG1115

	void	UpdateGuildLogo( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildContribution( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildNotice( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildAuthority( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildPenya( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildRealPay( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildSetName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildBankUpdate( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenQueryGuildBank( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenQueryGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	
	void	AddGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CloseGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CastGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	AcptWar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarEnd( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	Surrender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarDead( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarMasterAbsent( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );


	void	UpdateGuildRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildRankingDB( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	AddPartyName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	BOOL	call_uspLoggingTrade( CQuery* pQuery, int nFlag, int nTradeID,  DWORD dwWorldID = 0, u_long idPlayer = 0, DWORD dwTradeGold = 0, const char* lpAddr = "", int nLevel = 0, int nJob = 0, int nItemIndex = 0, SERIALNUMBER iItemSerialNum = 0, int nItemCnt = 0, int nAbilityOpt = 0, int nItemResist = 0, int nResistAbilityOpt = 0, __int64 iRandomOpt=0 );
	LONG	m_cbTrade;
	int		GetTradeNo( void );
	void	call_uspXXXMultiServer( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

	BOOL	CreateDbWorkers( void );
	void	CloseDbWorkers( void );
	void	GetThread( void );
	void	PutThread( void );
	void	SPThread( void );
	void	UpdateThread( void );
	void	GuildThread( void );
#ifdef __S1108_BACK_END_SYSTEM
	void	BackSystem( void );
#endif // __S1108_BACK_END_SYSTEM
	void	ItemUpdateThread( void );
	void	ChangeItemUpdate( CQuery* pQuery );
	void	ChangeSkillUpdate( CQuery* pQuery );
	void	ChangeMonsterUpdate( CQuery* pQuery );
	void	ChangeQuestUpdate( CQuery* pQuery );
	void	ChangeJobNameUpdate( CQuery* pQuery );
	void	ChangeSexUpdate( CQuery* pQuery );
	//void	GetBank( BOOL bCache, ACCOUNT_CACHE* AccountCache, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nMySlot, char* pszBankPW );
	BOOL	GetBank( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nSlot );
	BOOL	GetInventory( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LoadPiercingInfo( CItemElem & itemElem, char* szPirecingInven, int* pLocation );
	void	GetPiercingInfoFromMail( CQuery* pQuery, CItemElem* pItemElem );
	BOOL	GetPocket( CMover* pMover, CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	SavePocket( CMover* pMover, PPocketStruct pPocketStruct );
	BOOL	GetEquipment( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetTaskBar( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetCardCube( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetBaseCharacter( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetSkill( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetJobLv( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetQuest( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetSMMode( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetSKillInfluence( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetHonor( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	static	UINT	_GetThread( LPVOID pParam );
	static	UINT	_PutThread( LPVOID pParam );
	static	UINT	_SPThread( LPVOID pParam );
	static	UINT	_UpdateThread( LPVOID pParam );
	static	UINT	_GuildThread( LPVOID pParam );
#ifdef __S1108_BACK_END_SYSTEM
	static  UINT	_BackSystem( LPVOID pParam );
#endif // __S1108_BACK_END_SYSTEM
	static	UINT	_ItemUpdateThread( LPVOID pParam );

	void	Clear( void );

	void	DBQryCharacter( char* qryCharacter, const char* Gu, u_long idPlaeyr, int nserverindex, const char* szAccount = "", const char * szName = "",
		int nPlayerslot = 0, DWORD dwWorldID = 0, DWORD dwIndex = 0, float vPos_x = 0.0f, float vPos_y = 0.0f,
		float vPos_z = 0.0f, const char* szCharacterKey = "", DWORD dwSkinSet = 0, DWORD dwHairMesh = 0, DWORD dwHairColor = 0,
		DWORD dwHeadMesh = 0, DWORD dwSex = 0
		);

	void	DBQryLog( char* qryLog, const char* Gu, u_long idPlayer, int nserverindex, EXPINTEGER nExp1 = 0, int nLevel = 0,
						  int nJob = 0, int JobLv = 0, int FlightLv = 0, int nStr = 0, int nDex = 0,
						  int nInt = 0, int nSta = 0, int nRemainGP = 0, int nRemainLP = 0, char szState = 'A',
						  int WorldID = 0, const char* szkilled_szName = "", float vPos_x = 0.0f, float vPos_y = 0.0f, float vPos_z = 0.0f,
						  int nAttackPower = 0, int nMax_HP = 0, const char *uGetidPlayer = "", DWORD dwGold = 0, int nRemainGold = 0,
						  int nItem_UniqueNo = 0, const char *szItem_Name = "", int Item_durability = 0, int Item_count = 0, int ItemAddLv = 0,
						  const char* szStart_Time = "", int TotalPlayerTime = 0, const char * szRemoteIP = "", int nQuest_Index = 0, u_long uKill_idPlayer = 0);

	void	DBQryGuild( char* szSql, const char* szType, u_long idPlayer = 0, u_long idGuild = 0, const char* szGuild = "" );
	void	DBQryGuild( char* szSql, const GUILD_QUERYINFO & info);
	void	DBQryVote( char* szSql, const VOTE_QUERYINFO& info );
	void	DBQryGuildLog( char* szSql, const GUILDLOG_QUERYINFO& info );
	void	DBQryWar( char* szSql, const WAR_QUERYINFO & info );
	void	DBQryNewItemLog( char* qryLog, const LogItemInfo& info );

	int		GetOneItem( CItemElem* pItemElem, char* pstrItem, int *pLocation );
	void	GetOneSkill( LPSKILL pSkill, char* pstrSkill, int *pLocation );
	void	GetOneQuest( LPQUEST pQuest, char* pstrQuest, int *pLocation );

#ifdef __CONV_SKILL
	void	ConvSkill( void );
#endif	// __CONV_SKILL

#ifdef __CONV_SKILL_11_MONTH_JOB1
	void	ConvSkill_11_MONTH_POINT( void );
#endif // __CONV_SKILL_11_MONTH_JOB1
#ifdef __CONV_SKILL_11_MONTH_JOB2
	void	ConvSkill_11_MONTH( void );
#endif // __CONV_SKILL_11_MONTH_JOB2

#ifdef __CONV_SKILL_STOP
	void	ConvSkillStop( void );
#endif // __CONV_SKILL_STOP

#ifdef __VERIFY_PLAYER
	BOOL	VerifyPlayer( void );
#endif	// __VERIFY_PLAYER
	
#ifdef __S0707_ITEM_CONV
	BOOL	ConvItem( void );
#endif // __S0707_ITEM_CONV
	BOOL	ConvItem050615( void );
	void	ConvChangeItem050615( CMover* pMover );
	void	ConvChangeGuild050615( CMover* pMover );
	void	ConvChangeGuild050615( CItemContainer< CItemElem  >*  GuildBank );
	BOOL	GetBankMover( CMover* pMover, CQuery *qry, int nSlot );
	void	GetGuildBank( CItemContainer< CItemElem  >*  GuildBank, CQuery *qry );
	BOOL	RemoveItemInvenBank( CMover* pMover, int* nCountItem0, int* nCountItem1 );
	BOOL	RemoveItemGuildBank( int nGuildId, CItemContainer< CItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 );
	DWORD	GetRemoveItemPanya( int nItem0, int nItem1 );

#ifdef __ITEM_REMOVE_LIST
	void	InitConvItemDialog( void );
	void	UpdateConvItemDialog( const char* pTitle, const char* pString, const char* pPosStirng, int nPos );
	BOOL	ConvItemConnectDB( CQuery* pQueryChar, CQuery* pQuerySave );
	DWORD	GetMaxInventory( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	DWORD	GetMaxGuildBank( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	CreateInvenBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	CreateGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	DeleteInvenBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	DeleteGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetInventoryBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetUserInventoryBank( CMover* pMover, CQuery* pQueryChar );	
	BOOL	InventoryBankConv( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GuildBankConv( char* pszSQL, int nGuildId, CItemContainer< CItemElem >* GuildBank, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	RemoveGuildBankList( char* pszSQL, int nGuildId, CItemContainer< CItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 );
	BOOL 	RemoveInventoryBankList( char* pszSQL, CMover* pMover, int* nCountItem0, int* nCountItem1 );
	BOOL	RemoveGuildBankListPanya( char* pszSQL, int nGuildId, CQuery* pQueryChar, CQuery* pQuerySave, int nCountItem0, int nCountItem1 );
	BOOL	RemoveInventoryBankListPanya( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave, int nCountItem0, int nCountItem1 );
	BOOL	SaveUserInventoryBank( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	SaveConvGuildBank( char* pszSQL, int nGuildId, CItemContainer< CItemElem >* GuildBank, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	PiercingConfirmInventoryBank( CMover* pMover );
	BOOL	PiercingConfirmGuildBank( int nGuildId, CItemContainer< CItemElem >* GuildBank );
	BOOL	ConvItemStart( void );
#endif // __ITEM_REMOVE_LIST

#ifdef __RECOVER0816
	BOOL	RecoverCharacter( void );
#endif	// __RECOVER0816

#ifdef __INITITEMHITPOINT
	BOOL	InitItemHitPoint( void );
#endif	// __INITITEMHITPOINT

	BOOL	OpenWanted( CAr& ar );

	static	void	MakeQueryAddMail( char* szSql, CMail* pMail, u_long idReceiver );

private:
			CDbManager();
#ifdef __RT_1025
	void	LoadMessenger( CMover* pMover, CQuery* pQuery );
	void	AddMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	DeleteMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	UpdateMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
#else	// __RT_1025
	void	SaveMessengerFriend( CQuery *qry, CMover* pMover, char* szQuery );
public:
	BOOL	SetMessenger( void );
private:
	void	GetMessengerFriend( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveFriend( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif	// __RT_1025
	BOOL	IsAbnormalPlayerData( CMover* pMover );
	BOOL	NormalizePlayerData( CMover* pMover );
	int		SelectTag( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, TAG_ENTRY* tags );
	void	WriteTag( CAr &ar, int count, const TAG_ENTRY* tags );
	void	InsertTag( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetStrTime( CTime *time, const char *strbuf );
	BOOL	VerifyString( const char* lpString, const char* lpFileName, int nLine, const char* lpName, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL );
	BOOL	SN( void );
	void	LogPlayConnect(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogPlayDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogLevelUp(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogServerDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogUniqueItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogQuest(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogPkPvp( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogSchool( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogSkillPoint( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogConcurrentUserNumber( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogInstanceDungeon( CQuery *pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#ifdef __ERROR_LOG_TO_DB
	void	LogError( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __ERROR_LOG_TO_DB
	void	LogGuildFurniture( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	SetPlayerName( CQuery* pQueryChar, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SnoopGuild( CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	PutExpLog( CQuery* pQueryLog, CMover* pMover, char ch );
	void	QueryGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	InsertGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus ); 
	void	UpdateGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	BOOL	SendItemtoCharacter( int nSlot, CMover* pMover, CQuery *qry, CQuery *qry1, CQuery * qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	RemoveItemtoCharacter( CMover* pMover, CQuery *qry, CQuery *qry1, CQuery *qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	RemovePenya( CQuery *pQry, char* szSql, CMover* pMover, int nItem_Count, int nNo, char chState );
	SERIALNUMBER	RemoveItem( CQuery *pQry, char* szSql, int nNo, CMover* pMover, char* szItemName, int nItemCount, int nAbilityOption, int nItemResist, int nResistAbilityOption, char chState );
	SERIALNUMBER	RemoveItemBank( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
#ifdef __REMOVEITEM_POCKET
	SERIALNUMBER	RemoveItemPocket( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
#endif // __REMOVEITEM_POCKET

	void	SchoolReport( CQuery* pQuery, LPDB_OVERLAPPED_PLUS po );

	BOOL	GetSendItem( CQuery *pQry, __SendItemContents * pSendItemContents );
	SERIALNUMBER	SendItem( CQuery *pQry, char* szSql, CMover* pMover, __SendItemContents& SendItemContents );
	SERIALNUMBER	RemoveItemInventory( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
	BOOL	GetRemoveItem( CQuery *pQry, int &nNo, char* pOneItem, int &nItem_Count, int &nAbilityOption, int &nItemResist, int &nResistAbilityOption, char &chState );	

	BOOL	SendPenya( CQuery *pQry, char* szSql, CMover* pMover, int nPenya, int nNo );
	BOOL	SendItemDeleteQuery( CQuery *pQry, char* szSql, int nNo );
	BOOL	ItemLogQuery( CQuery *pQryLog, const char *szQueryState, const LogItemInfo& info, u_long uIdPlayer, int nNo, const char *szItemName );
	BOOL	RemoveItemDeleteQuery( CQuery *pQry, char* szSql, int nNo );

#ifdef __S_RECOMMEND_EVE
	void	RecommendEve( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __S_RECOMMEND_EVE

	void	LogGetHonorTime(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	CQuery	m_qryPostProc;
	void	AddMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMailItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMailGold( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	ReadMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	DbQryMail( char* szSql, const MAIL_QUERYINFO & info );
	void	LogExpBox( CQuery *pQuery, LPDB_OVERLAPPED_PLUS pov );

#ifdef __S1108_BACK_END_SYSTEM
	void	GetGemeSettingtime( CQuery* pQuery, int nChat = 0 );
	BOOL	GetBaseGameSetting( CQuery* pQuery, const char* lpstrTime = "");
	void	GetMonsterRespawnSetting( CQuery* pQuery, int& nRemoveMaxRespawn, int aRemoveRespawn[] = NULL, const char* lpstrTime = "");
	void	GetMonsterPropSetting( CQuery* pQuery, const char* lpstrTime = "");
	BOOL	AddMonsterRespawn( MONSTER_RESPAWN BufMonsterRespawn );
	BOOL	RemoveMonsterRespawn( int nIndex, int &nMaxRemove, int aRemoveRespawn[] );
	BOOL	AddMonsterProp( MONSTER_PROP BufMonsterProp );
	BOOL	RemoveMonsterProp( MONSTER_PROP BufMonsterProp );
	BOOL	GetGMChat( CQuery* pQuery, const char * lpstrTime, int& nChatCount );
#endif // __S1108_BACK_END_SYSTEM

	BOOL	OnWantedQuery( CQuery* pQuery, WANTED_QUERYINFO& info );
	void	SerializeWanted( CQuery* pQuery, CAr& ar );
	void	WantedOperation( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	CanJoin( LPCTSTR szAccount, ACCOUNT_CACHE* pAccountCache );

	void	StartGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	InGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OutGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ResultGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetPenyaGuildGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetPenyaPlayerGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ContinueGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

private:
	void	GetStrFromDBFormat( char* szDst, const char* szSrc, int& n );
	void	SetStrDBFormat( char* szDst, const char* szSrc );
#ifdef __PET_1024
	void	GetDBFormatStr( char* szDst, int nMaxLen, const char* szSrc );
	void	SetDBFormatStr( char* szDst, int nMaxLen, const char* szSrc );
#endif	// __PET_1024

	void	LoginProtectCert( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	
	u_long			m_idPlayer;
	HANDLE			m_hThreadGet[MAX_GETTHREAD_SIZE];
	HANDLE			m_hThreadPut[MAX_PUTTHREAD_SIZE];
	HANDLE			m_hSPThread;
	HANDLE			m_hCloseSPThread;
	HANDLE			m_hThreadUpdate[MAX_UPDATETHREAD_SIZE];
	HANDLE			m_hThreadGuild;
	HANDLE			m_hWait;
};

inline BOOL CDbManager::VerifyString( const char* lpString, const char* lpFileName, int nLine, const char* lpName, LPDB_OVERLAPPED_PLUS lpOverlapped )
{
	int len		= strlen( lpString );
	if( len > 0 && lpString[len-1] == '$' )
		return TRUE;
	WriteLog( "%s, %d\t%s", lpFileName, nLine, lpName );
	if( NULL != lpOverlapped )
		FreeRequest( lpOverlapped );
	return FALSE;
}

extern CDbManager & g_DbManager;

#endif	// _DB_MANAGER_H_