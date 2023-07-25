#pragma once

#include "minigamebase.h"

class CMiniGameGawibawibo :	public CMiniGameBase
{
public:
	CMiniGameGawibawibo(void);
	CMiniGameGawibawibo( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameGawibawibo(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET * MiniGamePacket );


private:
	int m_nWinProb;
	int m_nDrawProb;
	int m_nLostProb;
	
	int m_nWinCount;	// ¿¬½Â
};

