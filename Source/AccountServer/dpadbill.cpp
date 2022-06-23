#include "stdafx.h"
#include "dpdbsrvr.h"
#include "dpadbill.h"
#include <ranges>

CDPAdbill g_DPAdbill;
CBuyingInfoMng g_BuyingInfoMng;

//________________________________________________________________________________

void CDPAdbill::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CBuyingInfoMng::BUYING_INFO3 pbi3;
	memcpy(&pbi3.buyingInfo, lpMsg, sizeof(BUYING_INFO));
	pbi3.buyingInfo.dpid	= idFrom;
	pbi3.buyingInfo.dwKey	= m_dwKey++;
	pbi3.dwTickCount	= GetTickCount();

	g_BuyingInfoMng.Add(pbi3);
	g_dpDbSrvr.SendBuyingInfo(&pbi3.buyingInfo);
}


//________________________________________________________________________________

void CBuyingInfoMng::Add(const BUYING_INFO3 & pbi3) {
	std::lock_guard lock(m_mutex);
	m_mapPbi3.emplace(pbi3.buyingInfo.dwKey, pbi3);
}

bool CBuyingInfoMng::Remove(const DWORD dwKey) {
	std::lock_guard lock(m_mutex);
	return m_mapPbi3.erase(dwKey) != 0;
}

void CBuyingInfoMng::Process() {
	const DWORD dwTickCount = GetTickCount();

	std::vector<DWORD> adwKey;

	std::lock_guard lock(m_mutex);

	// Find to remove
	for (BUYING_INFO3 pbi3 : m_mapPbi3 | std::views::values) {
		if (dwTickCount - pbi3.dwTickCount > SEC(3)) {
			g_DPAdbill.Send(&pbi3, sizeof(BUYING_INFO), pbi3.buyingInfo.dpid);
			adwKey.push_back(pbi3.buyingInfo.dwKey);
		}
	}

	// Remove
	for (const DWORD key : adwKey) {
		const auto it = m_mapPbi3.find(key);
		
		if (it != m_mapPbi3.end()) {
			const BUYING_INFO2 & ptr = it->second.buyingInfo;

			BUYING_INFO2	bi2;
			bi2.dwServerIndex = ptr.dwServerIndex;
			bi2.dwPlayerId = ptr.dwPlayerId;
			// No sender id
			bi2.dwItemId = ptr.dwItemId;
			bi2.dwItemNum = ptr.dwItemNum;
			strcpy(bi2.szBxaid, ptr.szBxaid);
			bi2.dwRetVal = ptr.dwRetVal;
			// no buying info 2 member
			CAr ar;
			ar.Accumulate<BUYING_INFO2, SERIALNUMBER>(bi2, 0);

			int nBufSize;
			LPBYTE lpData = ar.GetBuffer(&nBufSize);
			LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.m_pDbIOData->Alloc();
			memcpy(lpDbOverlappedPlus->lpBuf, lpData, nBufSize);
			lpDbOverlappedPlus->uBufSize = nBufSize;
			lpDbOverlappedPlus->nQueryMode = LOG_SM_ITEM;
			PostQueuedCompletionStatus(g_DbManager.m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped);
		}

		m_mapPbi3.erase(it);
	}
}
