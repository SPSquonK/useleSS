#ifndef __EVE_SCHOOL_H__
#define	__EVE_SCHOOL_H__

#include "guild.h"

#define	MAX_SCHOOL	10

enum
{
	SBS_END, SBS_READY, SBS_START, SBS_START2,
};

typedef	struct	_SCHOOL_ENTRY
{
	u_long	id = 0;
	char	lpName[MAX_G_NAME] = { '\0'};
	int		nSurvivor = 0;
	int		nSize = 0;
	int		nLevel = 0;
	int		nDead = 0;
}
SCHOOL_ENTRY, *PSCHOOL_ENTRY;

class CGuildMng;

class	CEveSchool
{
public:
	CGuildMng*	m_pSchoolMng;
	D3DXVECTOR3	m_avPos[MAX_SCHOOL];
	D3DXVECTOR3	m_vDefault;
	DWORD	m_dwsbstart2;
public:
	CEveSchool();
	~CEveSchool()	{}

	BOOL	Ready( void );
	BOOL	Start( void );
	BOOL	Start2( void );
	BOOL	End( void );
	BOOL	Report( void );

	BOOL	LoadPos( LPCSTR lpszFileName );
	D3DXVECTOR3		GetPos( u_long id );
	D3DXVECTOR3		GetPos( void )	{	return m_vDefault;	}

	static	CEveSchool* GetInstance();
};


typedef struct __AUTO_OPEN
{
	BOOL bUseing;
	BYTE nHour;	
	BYTE nMinute;		
} __AUTO_OPEN;

class CGuildCombat
{
#ifdef __WORLDSERVER 	
	__AUTO_OPEN		__AutoOpen[7];
#endif // __WORLDSERVER
	
public:
	enum { OPEN_STATE, CLOSE_STATE, WAR_STATE, COMPLET_CLOSE_STATE, GM_COLSE_STATE };
	enum { NOTENTER_STATE = 100, NOTENTER_COUNT_STATE, ENTER_STATE, MAINTENANCE_STATE, WAR_WAR_STATE, WAR_CLOSE_STATE, WAR_CLOSE_WAIT_STATE, WAR_TELEPORT_STATE };
	enum { ALLMSG = 1000, GUILDMSG, JOINMSG, WORLDMSG, STATE, WARSTATE, WAIT };
	struct __JOINPLAYER
	{
		u_long	uidPlayer = 0;
		int		nlife       = 0;
		int		nPoint      = 0;
		int		nMap        = 0;
		DWORD	dwTelTime   = 0;
	};

	struct __GCSENDITEM {
		int   nWinCount;
		DWORD dwItemId;
		int   nItemNum;
	};

	struct __GuildCombatMember final {
		u_long	uGuildId;
		std::vector<__JOINPLAYER*> vecGCSelectMember;	// 대전에 선택된 멤버 정보
		DWORD dwPenya         = 0;     // 참가비
		BOOL  bRequest        = FALSE; // 참가 유/무
		u_long	m_uidDefender = 0;     // 디펜더
		int	  nJoinCount      = 0;     // 참가 카운터
		int	  nWarCount       = 0;     // 전투 인원수
		int   nGuildPoint     = 0;     // 길드 포인트
		std::list<__JOINPLAYER*>	lspFifo;

		explicit __GuildCombatMember(u_long guildId) : uGuildId(guildId) {}
		__GuildCombatMember(const __GuildCombatMember &) = delete;
		__GuildCombatMember & operator=(const __GuildCombatMember &) = delete;
		~__GuildCombatMember() { SelectMemberClear(); }

		void SelectMemberClear()
		{
			for (__JOINPLAYER * pJoinPlayer : vecGCSelectMember) {
				SAFE_DELETE( pJoinPlayer );
			}
			vecGCSelectMember.clear();
			lspFifo.clear();
		}

		[[nodiscard]] __JOINPLAYER * FindByPlayerId(u_long playerId);
		[[nodiscard]] const __JOINPLAYER * FindByPlayerId(u_long playerId) const;
		[[nodiscard]] bool IsInFifo(const __JOINPLAYER * pJoinPlayer) const;
	};
	struct __GuildCombatProcess
	{
		int		nState;
		DWORD	dwTime;
		DWORD	dwCommand;
		DWORD	dwParam;
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
	struct __GCGETPOINT {
		static constexpr bool Archivable = true;

		u_long uidGuildAttack;
		u_long uidGuildDefence;
		u_long uidPlayerAttack;
		u_long uidPlayerDefence;
		int nPoint;
		BOOL bMaster;
		BOOL bDefender;
		BOOL bLastLife;
	};
	struct __GCPLAYERPOINT {
		static constexpr bool Archivable = true;

		u_long	uidPlayer = 0;
		int		nJob = 0;
		int		nPoint = 0;
	};
	int		m_nGuildCombatIndex;
	u_long	m_uWinGuildId;
	int		m_nWinGuildCount;
	u_long	m_uBestPlayer;
	std::vector<__GCGETPOINT> m_vecGCGetPoint;
	std::vector<__GCPLAYERPOINT> m_vecGCPlayerPoint;
#ifdef __WORLDSERVER 
	std::vector<__GuildCombatMember*> m_vecGuildCombatMem;
	DWORD	m_dwTime;
	int		m_nProcessGo;
	int	 	m_nProcessCount[ 25 ];
	__GuildCombatProcess GuildCombatProcess[250];
	int		m_nStopWar;	// 1이면 중간에 종료, 2이면 운영자가 종료

	int		m_nJoinPanya;		// 대전에 참가할수 있는 기본 Penya
	int		m_nGuildLevel;		// 대전에 참가할수 있는 최소 길드레벨
	int		m_nMinGuild;		// 최소 전쟁을 할수 있는 길드 개수(최소 참여 길드 조건이 되야 길드 대전이 시작함)
	int		m_nMaxGCSendItem;
	int		m_nMaxGuild;		// 대전에 참가할수 있는 길드
	int		m_nMaxJoinMember;	// 대전에 참가할수 있는 최대 유저
	int		m_nMaxPlayerLife;	// 대전에 참가한 유저의 최대 생명
	int		m_nMaxWarPlayer;	// 최대 선발대 유저
	int		m_nMaxMapTime;		// 대전 위치 설정 시간
	int		m_nMaxGuildPercent;	// 대전길드 상금 퍼센트
	int		m_nMaxPlayerPercent;// 베스트 플레이어 퍼센트 
	int		m_nRequestCanclePercent;	// 참가신청시 취소한 길드에게 돌려줄 퍼센트
	int		m_nNotRequestPercent;		// 참가시 입찰이 안된길드에게 돌려줄 퍼센트
	int		m_nItemPenya;				// 대전 상품 아이템 가격(용망토?)
	BOOL    m_bMutex;					// 길드대전 오픈 한번만...
	BOOL    m_bMutexMsg;					// 길드대전 오픈 한번만...
	CTimer   m_ctrMutexOut;
	std::vector< CString > m_vecstrGuildMsg;
	std::vector<__GCSENDITEM>	vecGCSendItem;
	
	std::vector</* Observing, Not Null */__GuildCombatMember *>	vecRequestRanking;	// 참가 순위
	std::vector<__GCRESULTVALUEGUILD>			m_GCResultValueGuild;		// 길드대전 결과값
	std::vector<__GCRESULTVALUEPLAYER>			m_GCResultValuePlayer;		// 길드대전 결과값
#endif // __WORLDSERVER
	int		m_nState;		// 길드워 상태
	int		m_nGCState;	// 전투 중일 때의 상태
#ifdef __CLIENT
	BOOL	m_bRequest;	// 신청 유무
	BOOL	IsRequest( void ) { return m_bRequest; };
#endif // __CLIENT
public:
	//	Constructions
	CGuildCombat();
	virtual ~CGuildCombat();
	void	GuildCombatClear( int Clear = 1 );
	void	GuildCombatGameClear();
	void	SelectPlayerClear( u_long uidGuild );
	void	AddvecGCGetPoint(__GCGETPOINT gcGetPoint);
	void	AddvecGCPlayerPoint( u_long uidPlayer, int nJob, int nPoint );
#ifdef __WORLDSERVER
	BOOL	LoadScript( LPCSTR lpszFileName );
	void	JoinGuildCombat( u_long idGuild, DWORD dwPenya, BOOL bRequest );
	void	AddSelectPlayer( u_long idGuild, u_long uidPlayer );
	void	GetSelectPlayer( u_long idGuild, std::vector<__JOINPLAYER> &vecSelectPlayer );
	void	OutGuildCombat( u_long idGuild );
	void	SetMaintenance();
	void	SetEnter();
	void	SetGuildCombatStart();
	void	SetGuildCombatClose( BOOL bGM = FALSE );
	void	SetGuildCombatCloseWait( BOOL bGM = FALSE );
	void	GuildCombatCloseTeleport();

	void	SetNpc( void );
	void	SetRequestRanking( void );
	void	SetDefender( u_long uidGuild, u_long uidDefender );
	void	SetPlayerChange( CUser* pUser, CUser* pLeader );
	u_long	GetDefender( u_long uidGuild );

	DWORD	GetRequstPenya( u_long uidGuild );
	void	GetPoint( CUser* pAttacker, CUser* pDefender );
	__int64	GetPrizePenya( int nFlag );
	BOOL	IsRequestWarGuild( u_long uidGuild, BOOL bAll );
	[[nodiscard]] BOOL	IsSelectPlayer(const CUser * pUser) const;
	
	void	JoinWar( CUser* pUser, int nMap = 99, BOOL bWar = TRUE );
	void	OutWar( CUser* pUser, CUser* pLeader, BOOL bLogOut = FALSE );
	void	JoinObserver( CUser* pUser );
	void	GuildCombatRequest( CUser* pUser, DWORD dwPenya );
	void	GuildCombatCancel( CUser* pUser );
	void	GuildCombatOpen( void );
	void	GuildCombatEnter( CUser* pUser );
	void	SetSelectMap( CUser* pUser, int nMap );
	void	UserOutGuildCombatResult( CUser* pUser );
	void	Process();
	void	ProcessCommand();
	void	ProcessJoinWar();
	void	SendJoinMsg( LPCTSTR lpszString );
	void	SendGuildCombatEnterTime( void );
	void	SendGCLog( void );
	void	SerializeGCWarPlayerList( CAr & ar );

	struct Rankings {
		boost::container::small_vector<u_long, 3> bestGuilds;
		size_t numberOfParticipantGuilds;
		u_long bestPlayer = 0;
	};

	[[nodiscard]] Rankings ComputeRankings() const;
	void	GuildCombatResult(const Rankings & rankings);
	void	GuildCombatResultRanking(const Rankings & rankings);

	CTime	GetNextGuildCobmatTime(void);
	int		m_nDay;

	__GuildCombatMember* FindGuildCombatMember( u_long GuildId );
	const __GuildCombatMember * FindGuildCombatMember(u_long GuildId) const;
	void RefreshFifo(__GuildCombatMember & guildCombatMember) const;
	std::span<const __GuildCombatMember * const> GetContenders() const;
	std::span<      __GuildCombatMember * const> GetContenders();
#endif // __WORLDSERVER
};

extern CGuildCombat g_GuildCombatMng;

#endif	// __EVE_SCHOOL_H__