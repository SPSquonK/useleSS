#pragma once

#define RESPAWNTYPE_REGION   0
#define RESPAWNTYPE_SCRIPT   1
#define RESPAWNTYPE_BACKEND  2

#include "commonCtrl.h"

#ifdef __WORLDSERVER
#include <array>

#define	MAX_RESPOINT_PER_REGION		100
class CWorld;
#endif

class CRespawnInfo
{
public:
	DWORD			m_dwType;
	DWORD			m_dwIndex;
	volatile long	m_cb;
	volatile long	m_nActiveAttackNum;
	CRect			m_rect;
	D3DXVECTOR3		m_vPos; // flying일 경우만 의미가있음 
	u_short			m_uTime;
	short			m_cbTime;

	u_short			m_cbRespawn;
	BOOL			m_bHalf;
	BOOL			m_bRemove;
	int m_nItemMin; // 아이템 최소 갯수  
	int m_nItemMax; // 아이템 최대 갯수  
	int m_nDayMin;
	int m_nDayMax;
	int m_nHourMin;
	int m_nHourMax;
	float	m_fY; // 발생할 y 좌표
	CCtrlElem m_CtrlElem;

#ifdef __S1108_BACK_END_SYSTEM
	long m_nMaxcb;
	long m_nMaxAttackNum;
	int	 m_nGMIndex;
#endif // __S1108_BACK_END_SYSTEM

	DWORD	m_dwAiState;
	float	m_fAngle;	
	DWORD	m_dwPatrolIndex;
	BYTE    m_bPatrolCycle: 1;			// 전체 순환이냐? 끝->처음->끝 방향이냐
#ifdef __WORLDSERVER
	POINT	m_aResPoint[MAX_RESPOINT_PER_REGION];
#endif
	
public:
	CRespawnInfo();

#ifdef __WORLDSERVER
	void			Clear( void )	{	m_cb	= 0;	}
	u_long			Get( void )		{	return m_cb;	}
	void			Increment( BOOL bActiveAttack );	
	void			GetPos( D3DXVECTOR3 & v, BOOL bRespawn=TRUE );
	BOOL	GenResPoint( CWorld* pWorld );
#endif
};

#ifdef __WORLDSERVER

typedef std::vector<CRespawnInfo>	VRI;
class CRespawner final {
public:
	std::array<VRI, 3>	m_vRespawnInfo;

private:
	BOOL	DoRemove( int nRespawnNo, int nType ); // 실제 Remove를 수행 
public:
	int		Add( CRespawnInfo & ri, int nType = RESPAWNTYPE_REGION );
	CRespawnInfo*	GetRespawnInfo( int nRespawnNo, int nType );
	BOOL	Remove( int nRespawnNo, int nType );
	u_long	Spawn( CWorld* pWorld, int nLayer );
	void	Increment( int nRespawnNo, int nType, BOOL bActiveAttack );
};
////////////////////////////////////////////////////////////////////////////////

typedef	std::map<int, CRespawner*>	MRP;
class CLayerdRespawner
{
public:
	virtual	~CLayerdRespawner();
	int		Add( CRespawnInfo & ri, int nType = RESPAWNTYPE_REGION );
	BOOL	Remove( int nRespawn, int nType );
	CRespawnInfo*	GetRespawnInfo( int nRespawn, int nType, int nLayer );
	u_long Spawn( CWorld* pWorld );
	void	Increment( int nRespawn, int nType, BOOL bActiveAttack, int nLayer );
	void	Expand( int nLayer );
	void	Release( int nLayer );
	CRespawner*	Proto( void )	{	return &m_proto;	}
private:
	void	Clear();
private:
	CRespawner	m_proto;
	MRP	m_mapRespawners;
};

#endif
