#pragma once

#include "minigamebase.h"

class CMiniGamePairgame : public CMiniGameBase
{
public:
	CMiniGamePairgame(void);
	CMiniGamePairgame( CMiniGameBase* pMiniGame );
	virtual ~CMiniGamePairgame(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );

private:
	std::string	MakeGame();

	std::string	m_strCardStream;
	int		m_nCorrectCount;
};

