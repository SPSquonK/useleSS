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

BOOL CRespawnInfo::GenResPoint( CWorld* pWorld )
{
	if (m_nMaxcb == 1 && (m_dwAiState == 8 || m_dwAiState == 9)) {
		for (POINT & point : m_aResPoint) {
			point = POINT{
				.x = static_cast<long>(m_vPos.x),
				.y = static_cast<long>(m_vPos.z),
			};
		}
		return TRUE;
	}

	int nCheckAttr	= HATTR_NOWALK;
	if( m_dwType == OT_MOVER && prj.GetMoverProp( m_dwIndex )->dwFlying )
		nCheckAttr	= HATTR_NOFLY;

	static constexpr size_t MAX_FAIL_TO_RESPAWN = 10000;

	size_t nSize = 0;
	size_t nFail = 0;
	while (nSize < MAX_RESPOINT_PER_REGION) {
		const D3DXVECTOR3 v = GetRandomPositionWithoutCache();
		int nAttr	= pWorld->GetHeightAttribute( v.x, v.z );
		if( nAttr == -1 || nAttr == HATTR_NOMOVE || nAttr == nCheckAttr ) 
		{
			if( ++nFail > MAX_FAIL_TO_RESPAWN )
				return FALSE;
			continue;
		}

		m_aResPoint[nSize++] = POINT{
			.x = static_cast<LONG>(v.x),
			.y = static_cast<LONG>(v.z)
		};
	}

	return TRUE;
}

D3DXVECTOR3 CRespawnInfo::GetRandomPosition() const {
	if (m_nMaxcb == 1 && (m_dwAiState == 8 || m_dwAiState == 9)) {
		return m_vPos;
	}

	if (m_aResPoint[MAX_RESPOINT_PER_REGION - 1].x != 0 && m_aResPoint[MAX_RESPOINT_PER_REGION - 1].y) {
		const size_t r = xRandom(MAX_RESPOINT_PER_REGION);
		return D3DXVECTOR3(
			static_cast<float>(m_aResPoint[r].x),
			m_vPos.y,
			static_cast<float>(m_aResPoint[r].y)
		);
	} else {
		return GetRandomPositionWithoutCache();
	}
}


D3DXVECTOR3 CRespawnInfo::GetRandomPositionWithoutCache() const {
	const int nWidth  = std::max(m_rect.Width(), 1);
	const int nHeight = std::max(m_rect.Height(), 1);
	
	return D3DXVECTOR3(
		(float)( m_rect.left + xRandom( nWidth ) ),
		m_vPos.y,
		(float)( m_rect.top + xRandom( nHeight ) )
	);
}

void CRespawnInfo::Increment(const BOOL bActiveAttack) {
	m_cb -= 1;
	if (bActiveAttack) m_nActiveAttackNum -= 1;
}


///////////////////////////////////////////////////////////////////////////////////////////
// CRespawner
///////////////////////////////////////////////////////////////////////////////////////////

void CRespawner::AddRegionSpawn(CRespawnInfo ri) {
	ri.m_uTime /= 2;
	ri.m_nGMIndex = m_vRespawnInfoRegion.size();
	m_vRespawnInfoRegion.emplace_back(ri);
}

bool CRespawner::AddScriptSpawn(CRespawnInfo & ri) {
	const bool spawnNoAlreadyExists = std::any_of(
		m_vRespawnInfoScript.begin(), m_vRespawnInfoScript.end(),
		[nRespawnNo = ri.m_nGMIndex](const CRespawnInfo & self) {
			return self.m_nGMIndex == nRespawnNo;
		}
	);

	if (spawnNoAlreadyExists) {
		Error("CRespawner::AddScriptSpawn : Collision with %d, %d, %f, %f\n", ri.m_dwIndex, ri.m_dwType, ri.m_vPos.x, ri.m_vPos.z);
		return false;
	}

	m_vRespawnInfoScript.push_back(ri);
	return true;
}

// 여기서는 Remove 상태만 세팅 
bool CRespawner::RemoveScriptSpawn(const int nRespawnNo) {
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
		CCtrl * pObj = static_cast<CCtrl *>(CreateObj(m_dwType, m_dwIndex));

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

		D3DXVECTOR3	v = GetRandomPosition();

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

		pWorld->AddObj(pObj, TRUE, nLayer);
		
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

void CLayerdRespawner::AddRegionSpawn(CRespawnInfo & ri) {
	m_proto.AddRegionSpawn(ri);
}

bool CLayerdRespawner::AddScriptSpawn(CRespawnInfo & ri) {
	for (const auto & [nLayer, pRespawner] : m_mapRespawners) {
		pRespawner->AddScriptSpawn(ri);
	}

	return m_proto.AddScriptSpawn(ri);
}

bool CLayerdRespawner::RemoveScriptSpawn(const int nRespawnNo) {
	for (const auto & [nLayer, pRespawner] : m_mapRespawners) {
		pRespawner->RemoveScriptSpawn(nRespawnNo);
	}

	return m_proto.RemoveScriptSpawn(nRespawnNo);
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
