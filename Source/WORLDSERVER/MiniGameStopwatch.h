#pragma once

#include "minigamebase.h"

class CMiniGameStopwatch :	public CMiniGameBase
{
public:
	CMiniGameStopwatch(void);
	CMiniGameStopwatch( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameStopwatch(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );

private:
	int		SetTargetTime();		// 소수점 2자리 까지
	int		GetTargetTime() { return m_nTargetTime; }
	
	int m_nTargetTime;
};

