#pragma once

#include "dpmng.h"
#include "dbmanager.h"
#include <mutex>

class CDPAdbill : public CDPMng<CBuffer3> {
private:
	DWORD	m_dwKey = 0;
public:
	void SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) override {}
	void UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) override;
};

extern CDPAdbill g_DPAdbill;


//________________________________________________________________________________

class CBuyingInfoMng
{
private:
	std::map<DWORD, BUYING_INFO3 *>	m_mapPbi3;

public:
	std::recursive_mutex m_mutex;

public:
	CBuyingInfoMng()	{}
	~CBuyingInfoMng()	{	Clear();	}
	
	void Add(BUYING_INFO3 * pbi3);
	void Remove(DWORD dwKey);
	void Clear();

	BUYING_INFO3 * Get(DWORD dwKey);
	
	void Process();
};


extern CBuyingInfoMng g_BuyingInfoMng;
