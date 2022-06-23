#pragma once

#include "dpmng.h"
#include "dbmanager.h"
#include <mutex>
#include <optional>

class CDPAdbill : public CDPMng<CBuffer3> {
private:
	DWORD	m_dwKey = 0;
public:
	void SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) override {}
	void UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) override;
};

extern CDPAdbill g_DPAdbill;


//________________________________________________________________________________

class CBuyingInfoMng {
public:
	struct BUYING_INFO3 {
		BUYING_INFO2 buyingInfo;
		DWORD	dwTickCount;
	};

private:
	std::map<DWORD, BUYING_INFO3> m_mapPbi3;
	std::recursive_mutex m_mutex;

public:
	void Add(const BUYING_INFO3 & pbi3);
	bool Remove(DWORD dwKey);
	void Process();
};


extern CBuyingInfoMng g_BuyingInfoMng;
