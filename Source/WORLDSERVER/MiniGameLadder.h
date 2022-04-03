#pragma once

#include "minigamebase.h"

class CMiniGameLadder :	public CMiniGameBase
{
public:
	CMiniGameLadder(void);
	CMiniGameLadder( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameLadder(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );
};

