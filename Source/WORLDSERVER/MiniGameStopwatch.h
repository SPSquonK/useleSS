#pragma once

#include "minigamebase.h"

class CMiniGameStopwatch :	public CMiniGameBase
{
public:
	CMiniGameStopwatch(void);
	CMiniGameStopwatch( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameStopwatch(void);

	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;

private:
	int		SetTargetTime();		// 소수점 2자리 까지
	int		GetTargetTime() { return m_nTargetTime; }
	
	int m_nTargetTime;
};

