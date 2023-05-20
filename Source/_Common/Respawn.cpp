#include "stdafx.h"
#include "Respawn.h"

#include "CommonCtrl.h"
#include "defineObj.h"

///////////////////////////////////////////////////////////////////////////////////////////
// CRespawnInfo
///////////////////////////////////////////////////////////////////////////////////////////

CRespawnInfo::CRespawnInfo()
{
	m_dwType	= 0;
	ZeroMemory( &m_CtrlElem, sizeof( m_CtrlElem ) );
	m_CtrlElem.m_dwMinItemNum = 1;
	m_CtrlElem.m_dwMaxiItemNum = 1;

	m_bRemove = FALSE;
		
#ifdef __WORLDSERVER
	memset( m_aResPoint, 0, sizeof(POINT) * MAX_RESPOINT_PER_REGION );
#endif
}

#ifdef __WORLDSERVER

#define	MAX_FAIL_TO_RESPAWN	10000
BOOL CRespawnInfo::GenResPoint( CWorld* pWorld )
{
	int nCheckAttr	= HATTR_NOWALK;
	if( m_dwType == OT_MOVER && prj.GetMoverProp( m_dwIndex )->dwFlying )
		nCheckAttr	= HATTR_NOFLY;
	D3DXVECTOR3 v;
	int nSize	= 0;
	int nFail	= 0;
	while( 1 )
	{
		GetPos( v, FALSE );
		int nAttr	= pWorld->GetHeightAttribute( v.x, v.z );
		if( nAttr == -1 || nAttr == HATTR_NOMOVE || nAttr == nCheckAttr ) 
		{
			if( ++nFail > MAX_FAIL_TO_RESPAWN )
				return FALSE;
			continue;
		}
		m_aResPoint[nSize].x	= (LONG)( v.x );
		m_aResPoint[nSize++].y	= (LONG)( v.z );

		if( nSize >= MAX_RESPOINT_PER_REGION )
			break;
	}
//	TRACE( "nFail=%d\n", nFail );
	return TRUE;
}

void CRespawnInfo::GetPos( D3DXVECTOR3 & v, BOOL bRespawn )
{
	if( m_nMaxcb == 1 )
	{
		if( m_dwAiState == 8 || m_dwAiState == 9 )
		{
			v = m_vPos;
			return;
		}
	}

	if( bRespawn )
	{
		int r	= xRandom( 0, MAX_RESPOINT_PER_REGION );
		v.x		= (float)( m_aResPoint[r].x );
		v.z		= (float)( m_aResPoint[r].y );
		v.y		= m_vPos.y;
	}
	else
	{
		int nWidth = m_rect.right - m_rect.left;
		if( nWidth <= 0 ) nWidth = 1;
		int nHeight = m_rect.bottom - m_rect.top;
		if( nHeight <= 0 ) nHeight = 1;
		v.x		= (float)( m_rect.left + xRandom( nWidth ) );
		v.z		= (float)( m_rect.top + xRandom( nHeight ) );
		v.y		= m_vPos.y;
	}

}

void CRespawnInfo::Increment(const BOOL bActiveAttack) {
	m_cb -= 1;
	if (bActiveAttack) m_nActiveAttackNum -= 1;
}


///////////////////////////////////////////////////////////////////////////////////////////
// CRespawner
///////////////////////////////////////////////////////////////////////////////////////////

int CRespawner::Add( CRespawnInfo & ri, SpawnType nType ) {
	if (nType == SpawnType::Region) {
		ri.m_uTime /= 2;
		ri.m_nGMIndex = m_vRespawnInfoRegion.size();
		m_vRespawnInfoRegion.push_back(ri);
		return m_vRespawnInfoRegion.size() - 1;
	} else if (nType == SpawnType::Script) {
		const auto i = std::find_if(
			m_vRespawnInfoScript.begin(), m_vRespawnInfoScript.end(),
			[nRespawnNo = ri.m_nGMIndex](const CRespawnInfo & self) {
				return self.m_nGMIndex == nRespawnNo;
			}
		);

		if (i != m_vRespawnInfoScript.end()) {
			Error("CRespawner::Add 같은 ID 발견 : %d, %d, %f, %f, %d\n", ri.m_dwIndex, ri.m_dwType, ri.m_vPos.x, ri.m_vPos.z, nType);
			return -1;
		}

		m_vRespawnInfoScript.push_back(ri);
		return m_vRespawnInfoScript.size() - 1;
	}

	return -1;
}

// 여기서는 Remove 상태만 세팅 
bool CRespawner::RemoveRegionSpawn(const int nRespawnNo) {
	if (nRespawnNo < 0)	return false;

	const auto i = std::find_if(
		m_vRespawnInfoScript.begin(), m_vRespawnInfoScript.end(),
		[nRespawnNo](const CRespawnInfo & respawnInfo) {
			return respawnInfo.m_nGMIndex == nRespawnNo;
		}
	);

	if (i == m_vRespawnInfoScript.end()) return false;

	i->m_bRemove = true;
	return true;
}

bool CRespawner::IsSpawnInDeletion(CtrlSpawnInfo ctrlSpawnInfo) const {
	if (ctrlSpawnInfo.type == SpawnType::Script) {
		const auto i = std::find_if(
			m_vRespawnInfoScript.begin(), m_vRespawnInfoScript.end(),
			[nRespawnNo = ctrlSpawnInfo.spawnId](const CRespawnInfo & self) {
				return self.m_nGMIndex == nRespawnNo;
			}
		);

		if (i == m_vRespawnInfoScript.end()) return false;

		return i->m_bRemove;
	}

	return false;
}

template<typename Predicate>
	requires (std::is_invocable_r_v<bool, Predicate, const CRespawnInfo &>)
bool CRespawner::IncrementIf(
	const CtrlSpawnInfo ctrlSpawnInfo,
	const BOOL bActiveAttack,
	Predicate && predicate
) {
	if (ctrlSpawnInfo.type == SpawnType::Region) {
		if (ctrlSpawnInfo.spawnId >= 0 && std::cmp_less(ctrlSpawnInfo.spawnId, m_vRespawnInfoRegion.size())) {
			auto & respawnInfo = m_vRespawnInfoRegion[ctrlSpawnInfo.spawnId];

			if (predicate(respawnInfo)) {
				respawnInfo.Increment(bActiveAttack);
				return true;
			}
		}
	} else if (ctrlSpawnInfo.type == SpawnType::Script) {
		const auto i = std::find_if(
			m_vRespawnInfoScript.begin(), m_vRespawnInfoScript.end(),
			[nRespawnNo = ctrlSpawnInfo.spawnId](const CRespawnInfo & self) {
				return self.m_nGMIndex == nRespawnNo;
			}
		);

		if (i != m_vRespawnInfoScript.end() && predicate(*i)) {
			i->Increment(bActiveAttack);
			return true;
		}
	}

	return false;
}

void CRespawner::Increment(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack) {
	IncrementIf(ctrlSpawnInfo, bActiveAttack, [](const auto &) { return true; });
}

bool CRespawner::IncrementIfAlone(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack) {
	static constexpr auto IsAlone = [](const CRespawnInfo & self) {
		return self.m_nMaxcb == 1;
	};

	return IncrementIf(ctrlSpawnInfo, bActiveAttack, IsAlone);
}

u_long CRespawner::Spawn( CWorld* pWorld, int nLayer )
{
	if( g_eLocal.GetState( EVE_RESPAWN ) == 0 )
		return 0;

	ASSERT( pWorld );

	u_long uRespawned	= 0;

	for (CRespawnInfo & pi : m_vRespawnInfoRegion) {
		if (pi.m_dwIndex != 0) {
			uRespawned += pi.ProcessRespawn(pWorld, nLayer, SpawnType::Region);
		}
	}

	{
		auto pi = m_vRespawnInfoScript.begin();
		while (pi != m_vRespawnInfoScript.end()) {
			// 04.10.11 - 480개 정도 이다.

			// 삭제 명령이 내려졌고, 리스폰된 오브젝트가 없다면?
			const bool mustErase = (pi->m_dwIndex == 0)
				|| (pi->m_bRemove && pi->m_cb == 0);

			if (mustErase) {
				pi = m_vRespawnInfoScript.erase(pi);
			} else {
				uRespawned += pi->ProcessRespawn(pWorld, nLayer, SpawnType::Script);
				++pi;
			}
		}
	}

	return uRespawned;
}

u_long CRespawnInfo::ProcessRespawn(CWorld * const pWorld, const int nLayer, const SpawnType spawnType) {
	u_long uRespawned = 0;

	if (!IsInTime()) return 0;

	m_cbTime--;									// 리스폰 타이머 시간 감소 
	if (m_cbTime >= 0) return 0;

	// 0 이면 리스폰 시작

	short nTime = (short)((m_uTime * xRandom(50, 150)) / 100);


	long cb = 0;
	if (m_nGMIndex != 0 || pWorld->GetID() != WI_WORLD_MADRIGAL) {
		m_cbTime = nTime;					// 타이머 reset	
		cb = std::max(m_nMaxcb - m_cb, 0l);
	} else {
		m_cbTime = (short)(nTime * prj.m_fMonsterRebirthRate);
		cb = std::max(static_cast<long>(m_nMaxcb * prj.m_fMonsterRespawnRate) - m_cb, 0l);
	}

	m_bHalf = !m_bHalf;

	if (!m_bHalf) {
		if (m_nMaxcb / 2 <= m_nMaxcb - cb) {
			return 0;
		}

		cb = (m_nMaxcb / 2) - (m_nMaxcb - cb);
	}

	DWORD dwFlying = 0;
	const MoverProp * pMoverProp = nullptr;
	if (m_dwType == OT_MOVER) {
		pMoverProp = prj.GetMoverProp(m_dwIndex);
		if (pMoverProp)
			dwFlying = pMoverProp->dwFlying;
	}

	for (int iCb = 0; iCb != cb; ++iCb) {
		CCtrl * pObj = static_cast<CCtrl *>(CreateObj(D3DDEVICE, m_dwType, m_dwIndex));

		if (!pObj) {
			WriteLog("SPAWN: %lu, %lu", m_dwType, m_dwIndex);
			return uRespawned;
		}

		if (m_dwType == OT_ITEM) {
			CItemElem * pItemElem = new CItemElem;
			pItemElem->m_dwItemId = m_dwIndex;
			pItemElem->m_nItemNum = (short)(m_nItemMin + xRandom(m_nItemMax - m_nItemMin + 1));
			const ItemProp * pItemProp = pItemElem->GetProp();
			if (!pItemProp) {
				char lpOutputString[100] = { 0, };
				sprintf(lpOutputString, "%lu item property not found", m_dwIndex);
				OutputDebugString(lpOutputString);
				delete pItemElem;
				return uRespawned;
			}
			pItemElem->m_nHitPoint = pItemProp->dwEndurance;
			pItemElem->SetSerialNumber();
			((CItem *)pObj)->m_pItemBase = pItemElem;

			pItemElem->m_nItemNum = std::min(pItemElem->m_nItemNum, static_cast<short>(pItemProp->dwPackMax));
		} else if (m_dwType == OT_CTRL) {
			CCommonCtrl * pCtrl = static_cast<CCommonCtrl *>(pObj);
			memcpy(&(pCtrl->m_CtrlElem), &(m_CtrlElem), sizeof(CCtrlElem));
			pCtrl->SetAngle((float)(xRandom(360)));
		} else if (m_dwType == OT_MOVER) {
			CMover * pMover = static_cast<CMover *>(pObj);
			pMover->SetGold(pMover->GetLevel() * 15);  // 몬스터 생성시 기본 페냐를 설정
		}


		pObj->m_dwAIInterfaceState = m_dwAiState;
		pObj->m_dwPatrolIndex = m_dwPatrolIndex;
		pObj->m_bPatrolCycle = m_bPatrolCycle;

		if ((pObj->m_dwAIInterfaceState == 9 || pObj->m_dwAIInterfaceState == 8) && m_nMaxcb == 1)
			pObj->SetAngle(m_fAngle);

		D3DXVECTOR3	v; GetPos(v);

		if (dwFlying) {
			v.y += xRandom(21);
			float y = pWorld->GetLandHeight(v.x, v.z);
			if (v.y < y)
				v.y = y + 10.0f;
		} else {
			v.y = m_fY + 1.0f;	// 약간 더해주자. 안그러면 뚫고 들어갈 수 있음 
			v.y = pWorld->GetUnderHeight(v);
		}
		pObj->SetPos(v);

		pObj->SetSpawner(CtrlSpawnInfo{ m_nGMIndex, spawnType });

		// 거대 몬스터? 몬스터 마리수가 1명이고 선공 몬스터시 무조건 선공 몬스터 임
		if (m_dwType == OT_MOVER) {
			int nMaxAttckNum;
			if (m_nMaxAttackNum == 1 && m_nMaxcb == 1) {
				nMaxAttckNum = m_nActiveAttackNum + 1;
			} else if (pWorld->GetID() == WI_WORLD_MADRIGAL) {
				nMaxAttckNum = int((m_nMaxcb * prj.m_fMonsterRespawnRate) * prj.m_fMonsterAggressiveRate);
			} else {
				nMaxAttckNum = m_nMaxAttackNum;
			}

			if (m_nActiveAttackNum < nMaxAttckNum) {
				m_nActiveAttackNum += 1;
				if (pMoverProp->dwLevel >= 7)	// 레벨이 7 이상인것만 선공으로 리스폰됨.
					((CMover *)pObj)->m_bActiveAttack = TRUE;
			}
		}

		pWorld->ADDOBJ(pObj, TRUE, nLayer);
		
		uRespawned++;
		m_cb += 1;
	}

	return uRespawned;
}

bool CRespawnInfo::IsInTime() const {
	return IsInTimeRange(g_GameTimer.m_nDay, m_nDayMin, m_nDayMax)
		&& IsInTimeRange(g_GameTimer.m_nHour, m_nHourMin, m_nHourMax);
}

bool CRespawnInfo::IsInTimeRange(const int now, const int min, const int max) {
	if (min < max) {
		return now >= min && now <= max;
	} else {
		return now <= min || now >= max;
	}
}

////////////////////////////////////////////////////////////////////////////////
#ifdef __LAYER_1021

int CLayerdRespawner::Add( CRespawnInfo & ri, SpawnType nType ) {
	return m_proto.Add( ri, nType );
}

bool CLayerdRespawner::RemoveRegionSpawn(const int nRespawnNo) {
	for (const auto & [nLayer, pRespawner] : m_mapRespawners) {
		pRespawner->RemoveRegionSpawn(nRespawnNo);
	}

	return m_proto.RemoveRegionSpawn(nRespawnNo);
}

bool CLayerdRespawner::IsSpawnInDeletion(CtrlSpawnInfo ctrlSpawnInfo, int nLayer) const {
	if (ctrlSpawnInfo.type == SpawnType::Region) {
		return false;
	}

	if (const auto i = m_mapRespawners.find(nLayer); i != m_mapRespawners.end()) {
		return i->second->IsSpawnInDeletion(ctrlSpawnInfo);
	}
	return false;
}

u_long CLayerdRespawner::Spawn(CWorld * pWorld) {
	u_long uRespawn = 0;
	for (const auto & [nLayer, pRespawner] : m_mapRespawners) {
		uRespawn += pRespawner->Spawn(pWorld, nLayer);
	}
	return uRespawn;
}

void CLayerdRespawner::Increment(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack, int nLayer) {
	if (const auto i = m_mapRespawners.find(nLayer); i != m_mapRespawners.end()) {
		i->second->Increment(ctrlSpawnInfo, bActiveAttack);
	}
}

bool CLayerdRespawner::IncrementIfAlone(CtrlSpawnInfo ctrlSpawnInfo, BOOL bActiveAttack, int nLayer) {
	if (const auto i = m_mapRespawners.find(nLayer); i != m_mapRespawners.end()) {
		return i->second->IncrementIfAlone(ctrlSpawnInfo, bActiveAttack);
	}

	return false;
}

void CLayerdRespawner::Expand(int nLayer) {
	if (!m_mapRespawners.contains(nLayer)) {
		m_mapRespawners.emplace(nLayer, std::make_unique<CRespawner>(m_proto));
	}
}

void CLayerdRespawner::Release(const int nLayer) {
	m_mapRespawners.erase(nLayer);
}
#endif	// __LAYER_1021

#endif
