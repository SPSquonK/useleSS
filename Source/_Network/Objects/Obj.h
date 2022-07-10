#ifndef __OBJ_H__
#define __OBJ_H__

#pragma once

#include <d3dx9math.h>
#include "Define.h"
#include "Data.h"
#include "Ar.h"
#include <string>
#include <map>

#include "SkillInfluence.h"
#include "UserTaskBar.h"
#include "FlyFFTypes.h"

#if defined(__DBSERVER) || defined(__VPW)
  #include "..\DatabaseServer\Project.h"
  extern	CProject	prj;
#endif	//__DBSERVER || __VPW

#if defined(__DBSERVER)
#include "dbmanager.h"
#endif	//

#ifdef __RT_1025
#include "rtmessenger.h"
#else	// __RT_1025
#include "messenger.h"
#endif	// __RT_1025

#ifdef __BUFF_1107
#include "buff.h"
#endif	// __BUFF_1107

#include "pet.h"

#include "Piercing.h"

typedef	DWORD	OBJID;

#define ITYPE_ITEM      0
#define ITYPE_CARD      1
#define ITYPE_CUBE      2 
#define ITYPE_PET		3

#define	MAX_INVENTORY	42

enum 
{
	METHOD_NONE, METHOD_EXCLUDE_ITEM
};


#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL )

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CObj
{
public:
	DWORD	m_dwType;
	DWORD	m_dwIndex;
	D3DXVECTOR3		m_vScale;
	DWORD	m_dwMotion;
	D3DXVECTOR3		m_vPos;
	FLOAT	m_fAngle;
public:
//	Constructions
	CObj();
	virtual	~CObj()		{}
//	Operations
	virtual	void	Serialize( CAr & ar );
public:
static int	m_nMethod;
static int	GetMethod( void )	{	return m_nMethod;	}

#ifdef __LAYER_1015
protected:
	int		m_nLayer;
public:
	void	SetLayer( int nLayer )	{	m_nLayer	= nLayer;	}
	[[nodiscard]] int		GetLayer() const	{	return m_nLayer;	}
#endif	// __LAYER_1015
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CCtrl : public CObj
{
public:
	OBJID	m_objid;

public:
//	Constructions
	CCtrl();
	virtual	~CCtrl()	{}
//	Operations
	void	SetId( OBJID objid )	{	m_objid		= objid;	}
	OBJID	GetId( void )	{	return m_objid;	}
	virtual	void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "ItemElem.h"

inline void CItemElem::Empty() {
	m_szItemText[0] = '\0';
	m_dwItemId = 0;
	SAFE_DELETE(m_pPet);
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef	struct	tagSKILL
{
	DWORD	dwSkill;
	DWORD	dwLevel;
	void Init()	{};
}
SKILL, *LPSKILL;


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "ItemContainer.h"

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CItem : public CCtrl
{
public:
	CItemElem * m_pItemBase;

//	Constructions
	CItem();
	virtual ~CItem();
//	Operations
	virtual void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CAction
{
public:
	DWORD	m_dwState;
	DWORD	m_dwStateFlag;

public:
	CAction()
		{
			m_dwState	= 0;
			m_dwStateFlag		= 0;
		}
	void	ClearStateFlag() { m_dwStateFlag = 0; }
	void	AddStateFlag( DWORD dwStateFlag ) { m_dwStateFlag |= dwStateFlag; }
	DWORD	GetStateFlag() { return m_dwStateFlag; }
	virtual	~CAction()	{}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
class CMover;
class CActionMover : public CAction
{
public:
	CActionMover()	{}
	CActionMover( CMover* pMover )	{}
	virtual	~CActionMover()	{}
	DWORD	GetState( void )	{	return m_dwState;	}
	void	__ForceSetState( DWORD dwState )	{	m_dwState	= dwState;	}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct tagQuest
{
	static constexpr bool Archivable = true;

	BYTE		m_nState;
	WORD		m_wTime;
	QuestId		m_wId;
	// 추가 
	//BYTE		m_nKillNPCNum[ 2 ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	WORD		m_nKillNPCNum[ 2 ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	BYTE		m_bPatrol   : 1;
	BYTE		m_bReserve2 : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;
} QUEST,* LPQUEST;

//#ifdef __DBSERVER
#if defined(__DBSERVER)
typedef		MemPooler<CMover>	CMoverPool;
#endif	// __DBSERVER

typedef	struct	_EQUIP_INFO
{
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
}	EQUIP_INFO,	*PEQUIP_INFO;

#include "pocket.h"
#include "MoverCommon.h"

class CMover: public CCtrl
{
private:
	DWORD			m_dwGold;

public:
/// 몬스터에 해당되는 것 
	TCHAR			m_szCharacterKey[ 32 ];
	BOOL			m_bActiveAttack;
	DWORD			m_dwBelligerence;

	int				m_nMovePattern;
	int				m_nMoveEvent;
	int				m_nMoveEventCnt;
/// 몬스터에 해당되는 것 

	TCHAR			m_szName[32];
	BOOL			m_bPlayer;
	u_long			m_idPlayer;
	LONG			m_nHitPoint;
	LONG			m_nManaPoint;
	LONG			m_nFatiguePoint;
	DWORD			m_dwSkinSet;
	DWORD			m_dwHairMesh;
	DWORD			m_dwHairColor;
	DWORD			m_dwHeadMesh;

private:
	int		m_nTutorialState;
public:
	LONG	GetFlightLv( void )	{	return ( m_nLevel >= 20? 1: 0 );		}
	void	SetFlightLv( LONG nFlightLv )	{}
	int		GetTutorialState( void )	{	return m_nTutorialState;	}
	void	SetTutorialState( int nTutorialState )	{	m_nTutorialState	= nTutorialState;	}

	BOOL	IsEquipableNPC()	{	return FALSE;	}

	LONG			m_nFxp;
	BYTE			m_bySex;
	DWORD			m_dwRideItemIdx;
	int				m_nFuel;
	LONG			m_nJob;
	BYTE			m_nSlot;
	char			m_szBankPass[5];
	int				m_idGuildCloak;

	LONG			m_nStr,	m_nSta,	m_nDex,	m_nInt;
	LONG			m_nLevel, m_nDeathLevel;
	EXPINTEGER		m_nExp1, m_nDeathExp;
	SKILL			m_aJobSkill[ MAX_SKILL_JOB ];

	OBJID			m_idMarkingWorld;
	D3DXVECTOR3		m_vMarkingPos;
	long			m_nRemainGP;
	DWORD			m_dwAuthorization;
	DWORD			m_dwMode;

	CTaskbar	m_UserTaskBar;
	int			m_nUsedSkillQueue;
#ifdef __RT_1025
	CRTMessenger	m_RTMessenger;
#else	// __RT_1025
	CMessenger		m_Messenger;
#endif	// __RT_1025

	u_long			m_idparty;
	u_long			m_idGuild;
	WarId			m_idWar;
	DWORD			m_dwPKTime;					/// 핑크 상태 시간
	int				m_nPKValue;					/// PK 수치
	DWORD			m_dwPKPropensity;			/// PK 성향
	DWORD			m_dwPKExp;					/// PK 성향 습득 경험치
	EXPINTEGER		m_nAngelExp;				/// 엔젤 경험치
	LONG			m_nAngelLevel;				/// 엔젤 Level

	int				m_nFame;
	u_long			m_idMurderer;
	int				m_nDuel;			// 듀얼중인가.( 0:없음 1:개인듀얼중 2:파티듀얼중.
	u_long			m_idDuelParty;		// 파티듀얼중이면 상대방 파티의 id
	int				m_nGuildCombatState;
	int				m_nSkillLevel;						// 지금까지 올린 스킬레벨
	int				m_nSkillPoint;						// SP

	CActionMover*					m_pActMover; 
	CItemContainer	m_Inventory ;

	CPocketController	m_Pocket;

	u_long							m_idPlayerBank[3];
	CItemContainer	m_Bank[ 3 ] ;
	DWORD							m_dwGoldBank[ 3 ];

	D3DXVECTOR3		m_vReturnPos;						//'귀환 두루마리'관련 
	DWORD			m_dwReturnWorldID;
	DWORD			m_tmAccFuel;
#ifdef __DBSERVER
#ifdef __JEFF_9_20
	DWORD	m_dwMute;
#else	// __JEFF_9_20
	DWORD			m_dwFlyTime;
#endif	// __JEFF_9_20
	DWORD			m_dwWorldID;
	CMclCritSec		m_AccessLock;
#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	__int64 m_nEventFlag; // 64비트 경과 일 번째 비트
#else // __EVENT_1101_2
	DWORD	m_dwEventFlag;	//	경과 일 번째 비트
#endif // __EVENT_1101_2
	DWORD	m_dwEventTime;	// 분 단위 계수
	DWORD	m_dwEventElapsed;	// 경과 일
#endif	// __EVENT_1101
#endif // __DBSERVER

private:
	OBJID	m_objIdVisPet;	// 소환중인 비스펫의 인벤토리 위치
	DWORD	m_dwMoverSfxId; // 무버에 붙는 이펙트
private:
	DWORD	m_dwPetId;	// 소환중이 펫의 인벤토리 위치(自), 소환중인 펫 인덱스(他)
public:
	DWORD	GetPetId( void )	{	return m_dwPetId;	}
	void	SetPetId( DWORD dwPetId )		{	m_dwPetId	= dwPetId;	}
	CItemElem*	GetPetItem( void );
	FLOAT	m_fSpeedFactor;

#ifdef __EXP_ANGELEXP_LOG
	int		m_nExpLog;
	int		m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	int		m_nCoupon;
#endif // __EVENTLUA_COUPON

	int				m_nHonor;					// 달인선택 
	int				m_aHonorTitle[MAX_HONOR_TITLE];			// 달인수치
	void			SetHonorCount(int nIdx , int nCount )	{	m_aHonorTitle[nIdx] = nCount;	}

	LONG			m_nPlusMaxHitPoint;
	DWORD			m_dwSMTime[SM_MAX];
	BYTE			m_nAttackResistLeft;
	BYTE			m_nAttackResistRight;
	BYTE			m_nDefenseResist;

#ifdef __BUFF_1107
	CBuffMgr	m_buffs;
#else	// __BUFF_1107
	CSkillInfluence m_SkillState;				// 지속성을 갖는 스킬의 상태 처리(예:프로텍션 ??초동안 방어력 얼마 증가)
#endif	// __BUFF_1107

	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];
	void	ClearEquipInfo( void )
	{
		memset( m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
			m_aEquipInfo[i].dwId	= NULL_ID;
	}
	
	DWORD			m_dwStateMode;
	OBJID			m_dwUseItemId;					// 비행 시전 시간
	
	std::unique_ptr<MoverSub::Quests> m_quests;

	DWORD			m_dwPeriodicTick;
	DWORD			m_dwTickCheer;
	int				m_nCheerPoint;
	DWORD			m_dwPCBangClass;
private:
	u_long	m_idCampus;
	int		m_nCampusPoint;
public:
	u_long	GetCampusId()	{	return m_idCampus;	}
	void	SetCampusId( u_long idCampus )	{	m_idCampus = idCampus;	}
	int		GetCampusPoint()	{	return m_nCampusPoint;	}
	void	SetCampusPoint( int nMPPoint )		{	m_nCampusPoint = nMPPoint;	}

public:
	int		m_nRestPoint;
	time_t	m_tLogOut;

public:
//	Constructions
	CMover();
	virtual	~CMover();

//	Operations
	void	InitProp( void );
	int		InitSkillExp( void );
#if defined(__DBSERVER)
	MoverProp*	GetProp( void )		{	return( prj.GetMoverProp( m_dwIndex ) );	}
#endif	// __DBSERVER

	BOOL	RemoveQuest( QuestId nQuestId );

	BOOL	AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, int nCharged = 0, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwItemId4 = 0, DWORD dwKeepTime = 0, __int64 iRandomOptItemId = 0 );
	BOOL	AddItemBank( int nSlot, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber );
	virtual void	Serialize( CAr & ar );
//	int		GetMaxHitPoint()	    { return( ( m_nLevel * 16 ) + ( m_nSta * 6 ) + ( m_nStr  * 3 ) );		}
//	int		GetMaxManaPoint()		{ return( ( m_nLevel + m_nInt ) * 2 + 20 );		}
//	int		GetMaxFatiguePoint()	{ return(int)( ( m_nLevel * 2 ) + ( m_nSta * m_nLevel / 40 ) + ( ( m_nStr + m_nSta + m_nDex ) * 2.5 ) ); }
	BOOL	IsAbnormalPlayerData();
	BOOL	NormalizePlayerData();
	BOOL	RemoveItemIK3();
	BYTE	GetSex() const { return m_bySex; }
	void	SetSex( BYTE bySex ) { m_bySex = bySex; }
	int		GetGold();
	void	SetGold( int nGold );
	BOOL	AddGold( int nGold, BOOL bSend = TRUE );

	void	Copy( CMover* pMover, BOOL bCopyAll );
	void	PeriodTick();

#if defined(__DBSERVER)
	void	Lock( void )	{	m_AccessLock.Enter();	}
	void	Unlock( void )	{	m_AccessLock.Leave();	}
#ifdef __INVALID_LOGIN_0612
	BOOL	m_bLogout;
	char	m_szAccount[MAX_ACCOUNT];
#endif	// __INVALID_LOGIN_0612
#ifndef __VM_0820
#ifndef __MEM_TRACE
public:
	static CMoverPool*	m_pPool;
	void*	operator new( size_t nSize )	{	return CMover::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CMover::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CMover::m_pPool->Free( (CMover*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CMover::m_pPool->Free( (CMover*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __DBSERVER
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

inline int CMover::GetGold()
{
	ASSERT( m_dwGold <= INT_MAX );
	return m_dwGold;
}

inline void CMover::SetGold( int nGold )
{
	ASSERT( nGold >= 0 );
	m_dwGold = (DWORD)nGold;
}

inline BOOL CMover::AddGold( int nGold, BOOL bSend )
{
	if( nGold == 0 )
		return TRUE;

#ifdef __PERIN_BUY_BUG
	float fTotal = static_cast<float>( GetGold() ) + static_cast<float>( nGold );
	if( fTotal > static_cast<float>( INT_MAX ) )
		fTotal = static_cast<float>( INT_MAX );
	else if( fTotal < 0.0f )
		return FALSE;

	SetGold( static_cast<int>( fTotal ) );
#else // __PERIN_BUY_BUG
	int nTotal = GetGold() + nGold;

	if( nGold > 0 )
	{
		if( nTotal < 0 )		// overflow?
			nTotal = INT_MAX;
	}
	else
	{
		if( nTotal < 0 )		// underflow?
			return FALSE;
	}

	SetGold( nTotal );
#endif // __PERIN_BUY_BUG
	if( bSend )
	{
	#ifdef __WORLDSERVER
		g_UserMng.AddSetPointParam( this, DST_GOLD, nTotal );
	#endif	// __WORLDSERVER
	}

	return TRUE;
}

#endif	// __OBJ_H__