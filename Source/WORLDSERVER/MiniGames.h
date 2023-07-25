#pragma once

// Rainbow race minigames

#include "minigamebase.h"


class CMiniGameDiceplay : public CMiniGameBase {
public:
	CMiniGameDiceplay();

	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;

private:
	int m_nTargetNum;
};


class CMiniGameGawibawibo : public CMiniGameBase {
public:
	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * MiniGamePacket) override;

private:
	int m_nWinCount = 0;
};


class CMiniGameLadder : public CMiniGameBase {
public:
	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;
};


class CMiniGamePairgame : public CMiniGameBase {
public:
	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;

private:
	std::string	MakeGame();

	std::string	m_strCardStream;
	int		m_nCorrectCount = 0;
};


class CMiniGameStopwatch : public CMiniGameBase {
public:
	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;

private:
	int SetTargetTime();
	int m_nTargetTime = 0;
};


class CMiniGameTyping : public CMiniGameBase {
public:
	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;

private:
	void MakeQuestion();
	std::vector<std::string>	m_vecTyping;
};
