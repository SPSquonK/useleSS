#pragma once

// Rainbow race minigames

#include "minigamebase.h"








class CMiniGamePairgame : public CMiniGameBase {
public:
	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;

private:
	std::string	MakeGame();

	std::string	m_strCardStream;
	int		m_nCorrectCount = 0;
};


