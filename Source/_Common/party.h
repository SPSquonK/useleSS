#pragma once

#ifdef __CORESERVER
#include "projectcmn.h"
#endif // __CORESERVER

#include <optional>
#include <map>
#include <ranges>

#include "Ar.h"
#include "mempooler.h"


// 모드(데미지 증가, 유니크, 휴식, 아이템)
// 1, 2, 4, 8, 10, 12
#define PARTY_LINKATTACK_MODE		0	// 링크어텍 상태	: 데미지 증가
#define PARTY_FORTUNECIRCLE_MODE	1	// 포춘서클 상태	: 유니크 드롭확률
#define PARTY_STRETCHING_MODE		2	// 스트레칭 상태	: 휴식 2배 : 단장 어시스트 3배
#define PARTY_GIFTBOX_MODE			3	// 기프트박스 상태  : 아이템 양 2배

#define PARTY_PARSKILL_MODE			4	// parskill 상태  :단장에게서 멀리떨어져도 사용가능
#define MAX_PARTYMODE				5

#define	MAX_PTMEMBER_SIZE			15
#define MAX_PTMEMBER_SIZE_SPECIAL	8

#define PARTY_MAP_SEC	10

struct PartyMember final {	// 플레이어 아이디만 가지고 있음
	u_long	m_uPlayerId = 0;
#ifdef __CORESERVER
	std::optional<CTime> m_remove = std::nullopt;
#else
	bool m_remove = false;
#endif
#if defined(__WORLDSERVER) || defined(__CLIENT)
	D3DXVECTOR3	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
#endif

	PartyMember() = default;
	explicit PartyMember(const u_long playerId) : m_uPlayerId(playerId) {}
};

class CParty final {
public:
	enum class ShareExpMode { Level, Contribution, /* EqualDistribution */ };

	[[nodiscard]] static bool IsValidMode(const ShareExpMode value) {
		return value == ShareExpMode::Level || value == ShareExpMode::Contribution;
	}

	enum class ShareItemMode { Self, RoundRobin, Leader, Random };
	[[nodiscard]] static bool IsValidMode(const ShareItemMode value) {
		return value == ShareItemMode::Self || value == ShareItemMode::Leader
			|| value == ShareItemMode::Random || value == ShareItemMode::RoundRobin;
	}

private:
public:
	u_long	m_uPartyId = 0;								// 극단 ID
	TCHAR	m_sParty[33];							// 극단 명칭( 단막극단 : NO, 순회극단 : YES )
	PartyMember	m_aMember[MAX_PTMEMBER_SIZE];		// 한개의 극단의 극단원 정보
	int		m_nSizeofMember = 0;						// 극단원 숫자	( 2 ~ 8 )
	LONG	m_nLevel = 1, m_nExp = 0, m_nPoint = 0;				// 극단 레벨, 경험치, 포인트
	ShareExpMode  m_nTroupsShareExp = ShareExpMode::Level;
	ShareItemMode m_nTroupeShareItem = ShareItemMode::Self;	// 경험치 분배방식, 아이템 분배방식
	int		m_nKindTroup = 0;							// 극단 종류 : 단막극단, 순회극단
	int		m_nReferens = 0;							// 극단에 포함되어 있는 상태일때 게임에 나갔을경우 10분후에 탈퇴 검색할 파티
	int		m_nModeTime[MAX_PARTYMODE];				// 모드 시간
	int		m_nGetItemPlayerId = 0;						// 아이템 얻은 캐릭터
	u_long	m_idDuelParty = 0;							// 파티 듀얼중이면 상대방 파티의 ID, 아니면 0

#ifdef __WORLDSERVER
	DWORD	m_dwWorldId = 0;
#endif // __WORLDSERVER

public:
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CRIT_SEC	m_AddRemoveLock;
#endif

public:
//	Constructions
	CParty();

//	Operations
	void	InitParty();
	BOOL	IsMember( u_long uPlayerId )		{	return( FindMember( uPlayerId ) >= 0 );	}
	int		IsLeader( u_long uPlayerId )		{	return( m_aMember[0].m_uPlayerId == uPlayerId );	}
	int		GetSizeofMember() const					{	return m_nSizeofMember;		}
	int		GetLevel()							{	return m_nLevel;			}
	int		GetExp()							{	return m_nExp;			}
	int		GetPoint()							{	return m_nPoint;		}
	u_long	GetPlayerId( int i )	{	return m_aMember[i].m_uPlayerId;	}
#if defined( __WORLDSERVER ) || defined( __CLIENT )
	D3DXVECTOR3	GetPos( int i )		{	return m_aMember[i].m_vPos;			}
	void	SetPos( int i, D3DXVECTOR3 vPos )	{ m_aMember[i].m_vPos = vPos; }
#endif // #if defined( __WORLDSERVER ) || defined( __CLIENT )
#ifndef __CORESERVER
	CMover * GetLeader( void );
#endif // __CORESERVER

	void SetPartyId(const u_long uPartyId) { m_uPartyId = uPartyId; }
	BOOL	NewMember( u_long uPlayerId );
	BOOL	DeleteMember( u_long uPlayerId );

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	void	Lock( void )	{	m_AddRemoveLock.Enter();	}
	void	Unlock( void )	{	m_AddRemoveLock.Leave();	}
#endif

	void	ChangeLeader( u_long uLeaderId );

	friend CAr & operator<<(CAr & ar, const CParty & self);
#ifndef __CORESERVER
	friend CAr & operator>>(CAr & ar,       CParty & self);
#endif

	void	SwapPartyMember( int first, int Second );

	int		GetPartyModeTime( int nMode );
	void	SetPartyMode( int nMode, DWORD dwSkillTime ,int nCachMode );

	void	DoUsePartySkill( u_long uPartyId, u_long nLeaderid, int nSkill );
#ifdef __WORLDSERVER
	void	SetPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp );
	void	DoDuelPartyStart( CParty *pDst );
	void	DoDuelResult( CParty *pParty, BOOL bWin, int nAddFame, float fSubFameRatio );
	void	Replace(DWORD dwWorldId, const D3DXVECTOR3 & vPos, BOOL) const;
	void	Replace(DWORD dwWorldId, LPCTSTR sKey) const;
	[[nodiscard]] bool ReplaceChkLv(int Lv) const;
	void	ReplaceLodestar(const CRect & rect) const;
#endif

	void	DoDuelPartyCancel( CParty* pDuelParty );
	void	GetPoint( int nTotalLevel, int nMemberSize, int nDeadLeavel );
//	Attributes
	int		GetSize( void );
	int		FindMember( u_long uPlayerId );

#ifdef __WORLDSERVER
	/* Adds a new snapshot to all party members centered on no target */
	template<WORD snapshotID, typename... Ts>
	void SendSnapshotNoTarget(Ts ... ts) const;

	/* Adds a snapshot to all party members centered on a certain object */
	template<WORD snapshotID, typename... Ts>
	void SendSnapshotWithTarget(DWORD targetId, Ts ... ts) const;
#endif

private:


public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CParty>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CParty::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CParty::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CParty::m_pPool->Free( (CParty*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CParty::m_pPool->Free( (CParty*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};


typedef	std::map<u_long, CParty*>	C2PartyPtr;
typedef std::map<u_long, std::string>	ULONG2STRING;

class CPlayer;

#ifndef __CLIENT
class CPartyMng final {
public:
	static constexpr __time64_t AllowedLimboTime = 60 * 10 /* seconds */;

private:
	u_long		m_id = 0;	// 새로 생성되는 파티에 순차적으로 아이디를 할당하기 위한 변수다.
	C2PartyPtr	m_2PartyPtr;
#ifdef __WORLDSERVER
	int			m_nSecCount = 0;
#endif // __WORLDSERVER

public:
#ifdef __CORESERVER
	mutable CRIT_SEC	m_AddRemoveLock;

	EXPPARTY	m_aExpParty[MAX_PARTYLEVEL];

	ULONG2STRING	m_2PartyNameLongPtr;
	std::map<std::string, u_long>	m_2PartyNameStringPtr;
#endif	// __CORESERVER


public:
//	Constructions
	CPartyMng() = default;
	~CPartyMng() { Clear(); }

	CPartyMng(const CPartyMng &) = delete;
	CPartyMng & operator=(const CPartyMng &) = delete;

	void	Clear();
//	Operations
	u_long	NewParty(u_long uLeaderId, u_long uMemberId, u_long uPartyId = 0);
	BOOL	DeleteParty( u_long uPartyId );
	[[nodiscard]] CParty * GetParty(u_long uPartyId);
#ifdef __CORESERVER
	void	Lock( void )	{	m_AddRemoveLock.Enter();	}
	void	Unlock( void )	{	m_AddRemoveLock.Leave();	}
#endif

	friend CAr & operator<<(CAr & ar, const CPartyMng & self);
#ifndef __CORESERVER
	friend CAr & operator>>(CAr & ar,       CPartyMng & self);
#endif

//	Attributs
	int		GetSize( void );

#ifdef __CORESERVER
public:
	HANDLE	m_hWorker = nullptr;
	HANDLE	m_hCloseWorker = nullptr;

public:
	[[nodiscard]] bool IsPartyName(const char * szPartyName) const {
		return m_2PartyNameStringPtr.contains(szPartyName);
	}
	LPCSTR  GetPartyString( u_long uidPlayer );
	[[nodiscard]] u_long  GetPartyID(const char * szPartyName) const;
	void	AddPartyName( u_long uidPlayer, const char* szPartyName );
	BOOL	CreateWorkers( void );
	void	CloseWorkers( void );
	static	UINT	_Worker( LPVOID pParam );
	void	Worker( void );
	void	RemovePartyName( u_long uidPlayer, const char* szPartyName );

	void	AddConnection( CPlayer* pPlayer );
	void	RemoveConnection( CPlayer* pPlayer );
#endif // __CORESERVERE
#ifdef __WORLDSERVER
	void	PartyMapInfo( void );
#endif // __WORLDSERVER
};
#endif

#if defined(__WORLDSERVER) || defined(__CORESERVER)
extern CPartyMng g_PartyMng;
#endif

#ifdef __CLIENT
extern CParty g_Party;
#endif
