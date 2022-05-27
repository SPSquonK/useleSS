#pragma once

#ifdef __CORESERVER
#include "Ar.h"
#include "projectcmn.h"
#endif // __CORESERVER

#if defined(__CLIENT) || defined(__WORLDSERVER)
#include <ranges>
#endif

#include "mempooler.h"
#include <map>


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
#define PARTY_MAP_LENGTH	10

struct PartyMember final {	// 플레이어 아이디만 가지고 있음
	u_long	m_uPlayerId = 0;
	CTime	m_tTime = CTime::GetCurrentTime();
	BOOL	m_bRemove = FALSE;
#if defined(__WORLDSERVER) || defined(__CLIENT)
	D3DXVECTOR3	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
#endif
};

class CParty final {
private:
public:
	u_long	m_uPartyId = 0;								// 극단 ID
	TCHAR	m_sParty[33];							// 극단 명칭( 단막극단 : NO, 순회극단 : YES )
	PartyMember	m_aMember[MAX_PTMEMBER_SIZE];		// 한개의 극단의 극단원 정보
	int		m_nSizeofMember = 0;						// 극단원 숫자	( 2 ~ 8 )
	LONG	m_nLevel = 1, m_nExp = 0, m_nPoint = 0;				// 극단 레벨, 경험치, 포인트
	int		m_nTroupsShareExp = 0, m_nTroupeShareItem = 0;	// 경험치 분배방식, 아이템 분배방식
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
	CMover* GetLeader( void );
#endif // __CORESERVER

	void SetPartyId(const u_long uPartyId) { m_uPartyId = uPartyId; }
	BOOL	NewMember( u_long uPlayerId );
	BOOL	DeleteMember( u_long uPlayerId );

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	void	Lock( void )	{	m_AddRemoveLock.Enter();	}
	void	Unlock( void )	{	m_AddRemoveLock.Leave();	}
#endif

	void	ChangeLeader( u_long uLeaderId );

	void	Serialize( CAr & ar );

	void	SwapPartyMember( int first, int Second );

	int		GetPartyModeTime( int nMode );
	void	SetPartyMode( int nMode, DWORD dwSkillTime ,int nCachMode );

	void	DoUsePartySkill( u_long uPartyId, u_long nLeaderid, int nSkill );
#ifdef __WORLDSERVER
	void	SetPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp );
	void	DoDuelPartyStart( CParty *pDst );
	void	DoDuelResult( CParty *pParty, BOOL bWin, int nAddFame, float fSubFameRatio );
	void	DoUsePartyReCall( u_long uPartyId, u_long nLeaderid, int nSkill );
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
typedef std::map<std::string, u_long>	STRING2ULONG;

class CPlayer;

class CPartyMng
{
private:
	u_long		m_id;	// 새로 생성되는 파티에 순차적으로 아이디를 할당하기 위한 변수다.
//	CMapParty	m_2Party;
	C2PartyPtr	m_2PartyPtr;
#ifdef __WORLDSERVER
	int			m_nSecCount;
#endif // __WORLDSERVER
public:
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CRIT_SEC	m_AddRemoveLock;
#endif
#ifdef __CORESERVER
	EXPPARTY	m_aExpParty[MAX_PARTYLEVEL];
#endif	// __CORESERVER

	ULONG2STRING	m_2PartyNameLongPtr;
	STRING2ULONG	m_2PartyNameStringPtr;

public:
//	Constructions
	CPartyMng();
	~CPartyMng();
	void	Clear( void );
//	Operations
	u_long	NewParty( u_long uLeaderId, LONG nLeaderLevel, LONG nLeaderJob, BYTE nLeaderSex, LPSTR szLeaderName, u_long uMemberId, LONG nMemberLevel, LONG nMemberJob, BYTE nMemberSex, LPSTR szMembername, u_long uPartyId = 0 );
	BOOL	DeleteParty( u_long uPartyId );
	[[nodiscard]] CParty * GetParty(u_long uPartyId);
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	void	Lock( void )	{	m_AddRemoveLock.Enter();	}
	void	Unlock( void )	{	m_AddRemoveLock.Leave();	}
#endif

	void	Serialize( CAr & ar );

//	Attributs
	int		GetSize( void );

#ifdef __CORESERVER
public:
	HANDLE	m_hWorker;
	HANDLE	m_hCloseWorker;

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

#if defined(__WORLDSERVER) || defined(__CORESERVER)
extern CPartyMng g_PartyMng;
#endif

#ifdef __CLIENT
extern CParty g_Party;
#endif
