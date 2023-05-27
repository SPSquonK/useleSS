#pragma once

#include "commonCtrl.h"

#ifdef __WORLDSERVER
#include <memory>
#include <array>
#include <concepts>

#define	MAX_RESPOINT_PER_REGION		100
class CWorld;
#endif

class CRespawnInfo
{
public:
	DWORD			m_dwType;
	DWORD			m_dwIndex = 0;
	long	m_cb = 0;
	long	m_nActiveAttackNum = 0;
	CRect			m_rect = CRect(0, 0, 0, 0);
	D3DXVECTOR3		m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f); // flying일 경우만 의미가있음 
	u_short			m_uTime = 0;
	short			m_cbTime = 0;

	u_short			m_cbRespawn = 0;
	BOOL			m_bHalf = FALSE;
	BOOL			m_bRemove;
	int m_nItemMin = 1; // 아이템 최소 갯수  
	int m_nItemMax = 1; // 아이템 최대 갯수  
	int m_nDayMin = 1;
	int m_nDayMax = 30;
	int m_nHourMin = 1;
	int m_nHourMax = 24;
	float	m_fY = 0.0f; // 발생할 y 좌표
	CCtrlElem m_CtrlElem;

	long m_nMaxcb = 0;
	long m_nMaxAttackNum = 0;
	int	 m_nGMIndex = 0;

	DWORD	m_dwAiState = 2;
	float	m_fAngle = 0.0f;	
	DWORD	m_dwPatrolIndex = NULL_ID;
	BYTE    m_bPatrolCycle: 1 = 0;			// 전체 순환이냐? 끝->처음->끝 방향이냐
#ifdef __WORLDSERVER
	POINT	m_aResPoint[MAX_RESPOINT_PER_REGION];
#endif
	
public:
	CRespawnInfo();

#ifdef __WORLDSERVER
	void			Increment( BOOL bActiveAttack );
	[[nodiscard]] D3DXVECTOR3 GetRandomPosition() const;
	[[nodiscard]] D3DXVECTOR3 GetRandomPositionWithoutCache() const;
	BOOL	GenResPoint( CWorld* pWorld );
	u_long ProcessRespawn(CWorld * pWorld, int nLayer, SpawnType spawnType);

private:
	[[nodiscard]] bool IsInTime() const;
	[[nodiscard]] static bool IsInTimeRange(int now, int min, int max);
#endif
};

#ifdef __WORLDSERVER

class CRespawner final {
public:
	std::vector<CRespawnInfo> m_vRespawnInfoRegion;
	std::vector<CRespawnInfo> m_vRespawnInfoScript;

public:
	void AddRegionSpawn(CRespawnInfo ri);
	bool AddScriptSpawn(CRespawnInfo & ri);
	bool RemoveScriptSpawn(int nRespawnNo);
	[[nodiscard]] bool IsSpawnInDeletion(CtrlSpawnInfo ctrlSpawnInfo) const;
	u_long	Spawn( CWorld* pWorld, int nLayer );
	void Increment(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack);
	bool IncrementIfAlone(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack);

private:
	template<typename Predicate>
	requires (std::is_invocable_r_v<bool, Predicate, const CRespawnInfo &>)
	bool IncrementIf(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack, Predicate && predicate);
};
////////////////////////////////////////////////////////////////////////////////

class CLayerdRespawner
{
public:
	void AddRegionSpawn(CRespawnInfo & ri);
	bool AddScriptSpawn(CRespawnInfo & ri);
	bool RemoveScriptSpawn(int nRespawnNo);
	[[nodiscard]] bool IsSpawnInDeletion(CtrlSpawnInfo ctrlSpawnInfo, int nLayer) const;
	u_long Spawn( CWorld* pWorld );
	void	Increment( CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack, int nLayer );
	bool IncrementIfAlone(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack, int nLayer);
	void	Expand( int nLayer );
	void	Release( int nLayer );
	CRespawner*	Proto( void )	{	return &m_proto;	}
private:
	CRespawner	m_proto;
	std::map<int, std::unique_ptr<CRespawner>> m_mapRespawners;
};

#endif
