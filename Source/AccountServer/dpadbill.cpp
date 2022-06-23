#include "stdafx.h"
#include "dpdbsrvr.h"
#include "dpadbill.h"
#include <ranges>

CDPAdbill g_DPAdbill;
CBuyingInfoMng g_BuyingInfoMng;

//________________________________________________________________________________

void CDPAdbill::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	BUYING_INFO3 * pbi3	= new BUYING_INFO3;
	memcpy(pbi3, lpMsg, sizeof(BUYING_INFO));
	pbi3->dpid	= idFrom;
	pbi3->dwKey	= m_dwKey++;
	pbi3->dwTickCount	= GetTickCount();

	g_BuyingInfoMng.Add(pbi3);
	g_dpDbSrvr.SendBuyingInfo((PBUYING_INFO2)pbi3);
}


//________________________________________________________________________________

void CBuyingInfoMng::Clear() {
	std::lock_guard lock(m_mutex);
	
	for (BUYING_INFO3 * ptr : m_mapPbi3 | std::views::values) {
		safe_delete(ptr);
	}
	m_mapPbi3.clear();
}

void CBuyingInfoMng::Add(BUYING_INFO3 * pbi3) {
	std::lock_guard lock(m_mutex);

	m_mapPbi3.emplace(pbi3->dwKey, pbi3);
}

void CBuyingInfoMng::Remove(const DWORD dwKey) {
	std::lock_guard lock(m_mutex);

	m_mapPbi3.erase(dwKey);
}

BUYING_INFO3 * CBuyingInfoMng::Get(DWORD dwKey) {
	auto i = m_mapPbi3.find(dwKey);
	return i != m_mapPbi3.end() ? i->second : nullptr;
}

void CBuyingInfoMng::Process() {
	const DWORD dwTickCount = GetTickCount();

	std::vector<DWORD>	adwKey;

	std::lock_guard lock(m_mutex);

	for (/* const */ BUYING_INFO3 * const pbi3 : m_mapPbi3 | std::views::values) {
		if (dwTickCount - pbi3->dwTickCount > SEC(3)) {
			g_DPAdbill.Send(pbi3, sizeof(BUYING_INFO), pbi3->dpid);
			adwKey.push_back(pbi3->dwKey);
		}
	}

	// Remove

	for (const DWORD key : adwKey) {
		BUYING_INFO3 * ptr = Get(key);
		
		if (ptr) {
			BUYING_INFO2	bi2;
			bi2.dwServerIndex = ptr->dwServerIndex;
			bi2.dwPlayerId = ptr->dwPlayerId;
			// No sender id
			bi2.dwItemId = ptr->dwItemId;
			bi2.dwItemNum = ptr->dwItemNum;
			strcpy(bi2.szBxaid, ptr->szBxaid);
			bi2.dwRetVal = ptr->dwRetVal;
			// no buying info 2 member
			CAr ar;
			ar.Write((void *)&bi2, sizeof(BUYING_INFO2));

			DWORD sn = 0;
			ar << sn;

			int nBufSize;
			LPBYTE lpData = ar.GetBuffer(&nBufSize);
			LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.m_pDbIOData->Alloc();
			memcpy(lpDbOverlappedPlus->lpBuf, lpData, nBufSize);
			lpDbOverlappedPlus->uBufSize = nBufSize;
			lpDbOverlappedPlus->nQueryMode = LOG_SM_ITEM;
			PostQueuedCompletionStatus(g_DbManager.m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped);
		}

		Remove(key);
		SAFE_DELETE(ptr);
	}
}
