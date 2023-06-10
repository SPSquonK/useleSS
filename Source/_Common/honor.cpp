#include "stdafx.h"
#include "..\_Common\honor.h"


CTitleManager::CTitleManager()
{
	m_mapMonster.clear();
	m_mapItem.clear();
	m_mapEtc.clear();
	m_mapAll.clear();
	m_nCurrentTitleCount = 0;
}

CTitleManager* CTitleManager::Instance( void )
{
	static CTitleManager tManager;
	return &tManager;
}

BOOL	CTitleManager::LoadTitle(LPCTSTR lpszFileName)
{
	HonorData	TempData;
	CScript		script;


	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	script.tok = 0;


	while(true) {
		HonorData TempData = HonorData();
		TempData.nID = script.GetNumber(); // ID
		if (script.tok == FINISHED) {
			break;
		}
		
		TempData.nLGrouping = script.GetNumber();	// 대분류
		TempData.nSGrouping = script.GetNumber();	// 소분류
		TempData.nNeed		= script.GetNumber();	// 획득 조건
		script.GetToken();
		TempData.strTitle	= script.Token;	// 타이틀 이름
		switch(TempData.nLGrouping)
		{
			case HI_HUNT_MONSTER:
				m_mapMonster.emplace(TempData.nSGrouping, TempData);
				break;

			case HI_USE_ITEM:
				m_mapItem.emplace(TempData.nSGrouping, TempData);
				break;

			default:
				m_mapEtc.emplace(TempData.nSGrouping, TempData);
				break;
		};

		m_mapAll.emplace(TempData.nID, TempData);
		m_nCurrentTitleCount++;
	}
	
	return TRUE;
}
const char	*CTitleManager::GetTitle(int nId)		// 타이틀을 반환
{
	if(nId == -1) return NULL; 

	auto iter = m_mapAll.find(nId);	// 몬스터 아이디로 검색
	if(iter != m_mapAll.end()) 
	{
		return iter->second.strTitle.GetString();
	}

	return NULL;
}

int	CTitleManager::GetNeedCount(int nId, int nGroup)		// 획득가능 카운트 리턴
{
	switch(nGroup)
	{
		case HI_HUNT_MONSTER: {
			const auto iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if (iter != m_mapMonster.end()) {
				return iter->second.nNeed;
			} else
				return -1;
			break;
		}

		case HI_USE_ITEM: {
			const auto iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if (iter != m_mapItem.end()) {
				return iter->second.nNeed;
			} else
				return -1;
			break;
		}

		case -1: {		// 전부다 검색
			const auto iter = m_mapAll.find(nId);	// 몬스터 아이디로 검색
			if (iter != m_mapAll.end()) {
				return iter->second.nNeed;
			} else
				return -1;
			break;
		}

		default: {
			const auto iter = m_mapEtc.find(nId);	// 아이템 아이디로 검색
			if (iter != m_mapEtc.end()) {
				return iter->second.nNeed;
			} else
				return -1;
			break;
		}
	};
	return -1;
}

HonorData*		CTitleManager::GetHonorDataByID(int nId,int nGroup)
{
	std::map<int, HonorData>::iterator iter;

	switch(nGroup)
	{
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				return &iter->second;
			}
			else
				return NULL;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				return &iter->second;
			}
			else
				return NULL;
			break;

		default:
			iter = m_mapEtc.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapEtc.end()) 
			{
				return &iter->second;
			}
			else
				return NULL;
			break;
	};
	return NULL;
}
int	CTitleManager::GetIdx(int nId, int nGroup) {
	std::map<int, HonorData> * map;
	switch (nGroup) {
		case HI_HUNT_MONSTER: map = &m_mapMonster; break;
		case HI_USE_ITEM:     map = &m_mapItem;    break;
		default:              map = &m_mapEtc;     break;
	}

	const auto iter = map->find(nId);
	if (iter == map->end()) return -1;

	return iter->second.nID;
}


int		CTitleManager::GetIdxType(int nId)
{
	const auto iter = m_mapAll.find(nId); // 몬스터 아이디로 검색

	if(iter != m_mapAll.end()) 
	{
		return iter->second.nLGrouping;
	}
	
	return -1;
}


int CTitleManager::GetIdxByName(LPCTSTR chName)		// 
{
	const auto it = std::find_if(
		m_mapAll.begin(), m_mapAll.end(),
		[&](const auto & pair) {
			return lstrcmp(pair.second.strTitle, chName) == 0;
		}
	);

	return it != m_mapAll.end() ? it->second.nID : -1;
}
int     CTitleManager::GetIdxByGroup(int nLarge,int nSmall)
{
	for( auto iter = m_mapEtc.begin(); iter != m_mapEtc.end(); ++iter )
	{
		HonorData	TempData = iter->second;
		if( TempData.nLGrouping == nLarge && TempData.nSGrouping == nSmall  )
			return TempData.nID;
	}
	return -1;
}	

#ifdef __CLIENT

bool CTitleManager::UpdateEarned(int nId, int nCurrentCount) {
	const int nNeed = CTitleManager::Instance()->GetNeedCount(nId, -1);

	const auto it = m_vecEarned.find(nId);

	if (nCurrentCount >= nNeed && nNeed > 0) {
		if (it == m_vecEarned.end()) {
			m_vecEarned.emplace(nId);
			return true;
		} else {
			return false;
		}
	} else {
		if (it != m_vecEarned.end()) m_vecEarned.erase(it);
		return false;
	}
}

void	CTitleManager::InitEarned() {
	m_vecEarned.clear();
}
#endif	// __CLIENT

