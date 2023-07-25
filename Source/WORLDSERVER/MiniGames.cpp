#include "StdAfx.h"
#include "MiniGames.h"


CMiniGameDiceplay::CMiniGameDiceplay()
: m_nTargetNum( xRandom( 2, 13 ) )
{
}


BOOL CMiniGameDiceplay::Excute( CUser* pUser, __MINIGAME_PACKET * pMiniGamePacket )
{
	BOOL bReturn = FALSE;		// 현재 미니 게임이 완료되는건지...
	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		MP.nParam1 = m_nTargetNum;
		SendPacket( pUser, MP );
		return bReturn;
	}
	
	MP.nParam1 = xRandom( 6 ) + 1;
	MP.nParam2 = xRandom( 6 ) + 1;

	if( ( MP.nParam1 + MP.nParam2 ) == m_nTargetNum )
	{
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	}
	else
	{
		MP.nState = MP_FAIL;
		SendPacket( pUser, MP );
		m_nTargetNum = xRandom( 2, 13 );
		MP.nParam1 = m_nTargetNum;
		MP.nParam2 = -1;	// 실패시 새로 바뀐 목표값이라는 걸 알려줌..(-1)
	}

	SendPacket( pUser, MP );
	return bReturn;
}



///////////////////////////////////////////////////////////////////////////////

BOOL CMiniGameGawibawibo::Excute( CUser* pUser, __MINIGAME_PACKET * pMiniGamePacket )
{
	BOOL bReturn = FALSE;

	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		m_nWinCount = 0;
		MP.nParam1 = m_nWinCount;
		SendPacket( pUser, MP );
		return bReturn;
	}

	const DWORD nRandom = xRandom( 3 );

	if (nRandom == 0) { // win
		MP.nState = MP_TRUE;
		m_nWinCount++;
	} else if (nRandom == 1) { // draw
		MP.nState = MP_FALSE;
	} else { // lose
		MP.nState = MP_FAIL;
		m_nWinCount = 0;
	}

	MP.nParam1 = m_nWinCount;
	if (m_nWinCount == 3) {
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	}

	SendPacket( pUser, MP );    
	return bReturn;
}



///////////////////////////////////////////////////////////////////////////////

BOOL CMiniGameLadder::Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) {
	BOOL bReturn = FALSE;

	__MINIGAME_PACKET MP(pMiniGamePacket->wNowGame);
	if (pMiniGamePacket->nState == MP_OPENWND) {
		MP.nState = MP_OPENWND;
		SendPacket(pUser, MP);
		return bReturn;
	}

	if (xRandom(10) == 0) { // 1/10 확률로 성공한다.
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	} else {
		MP.nState = MP_FAIL;
	}

	SendPacket(pUser, MP);
	return bReturn;
}


///////////////////////////////////////////////////////////////////////////////

BOOL CMiniGamePairgame::Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) {
	BOOL bReturn = FALSE;
	if (pMiniGamePacket->nState == MP_OPENWND) {
		__MINIGAME_PACKET MP(pMiniGamePacket->wNowGame);
		MP.nState = MP_OPENWND;
		m_nCorrectCount = 0;
		MP.vecszData.push_back(MakeGame());
		SendPacket(pUser, MP);
		return bReturn;
	}

	__MINIGAME_PACKET MP(pMiniGamePacket->wNowGame);

	const size_t nPos_1 = static_cast<size_t>(pMiniGamePacket->nParam1);
	const size_t nPos_2 = static_cast<size_t>(pMiniGamePacket->nParam2);

	if (nPos_1 < m_strCardStream.size() && m_strCardStream[nPos_1] != 'x'
		&& nPos_2 < m_strCardStream.size() && m_strCardStream[nPos_2] != 'x'
		&& nPos_1 != nPos_2
		&& m_strCardStream[nPos_1] == m_strCardStream[nPos_2]
		) {
		m_nCorrectCount++;
		m_strCardStream[nPos_1] = m_strCardStream[nPos_2] = 'x';

		if (m_nCorrectCount == 9) {
			MP.nState = MP_FINISH;
			bReturn = TRUE;
		} else {
			MP.nState = MP_TRUE;
		}
	} else {
		MP.nState = MP_FALSE;
	}

	SendPacket(pUser, MP);
	return bReturn;
}

std::string CMiniGamePairgame::MakeGame() {
	m_strCardStream = "";

	for (char c = 1; c < 10; ++c) {
		m_strCardStream += c;
		m_strCardStream += c;
	}

	for (size_t i = 0; i != m_strCardStream.size(); ++i) {
		DWORD pos = xRandom(static_cast<DWORD>(i), static_cast<DWORD>(m_strCardStream.size()));
		std::swap(m_strCardStream[i], m_strCardStream[pos]);
	}

	return m_strCardStream;
}

///////////////////////////////////////////////////////////////////////////////

int	CMiniGameStopwatch::SetTargetTime() {
	m_nTargetTime = xRandom(300, 1000) * 10;
	return m_nTargetTime;
}

BOOL CMiniGameStopwatch::Excute( CUser* pUser, __MINIGAME_PACKET * pMiniGamePacket ) {
	BOOL bReturn = FALSE;

	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		MP.nParam1 = SetTargetTime();
		SendPacket( pUser, MP );
		return bReturn;
	}

	// 0.05초 오차범위 인정..
	const int delta = pMiniGamePacket->nParam1 - m_nTargetTime;
	if (delta >= -30 && delta <= 30) {
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	} else {
		MP.nState = MP_FAIL;
		MP.nParam1 = SetTargetTime();
	}

	SendPacket( pUser, MP );
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CMiniGameTyping::Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) {
	BOOL bReturn = FALSE;

	__MINIGAME_PACKET MP(pMiniGamePacket->wNowGame);
	if (pMiniGamePacket->nState == MP_OPENWND) {
		MP.nState = MP_OPENWND;
		MakeQuestion();
		MP.vecszData = m_vecTyping;
		SendPacket(pUser, MP);
		return bReturn;
	}

	MP.nState = MP_FINISH;
	bReturn = TRUE;
	for (size_t i = 0; i < m_vecTyping.size(); i++) {
		if (m_vecTyping[i] != pMiniGamePacket->vecszData[i]) {
			MP.nState = MP_FAIL;
			bReturn = FALSE;
			break;
		}
	}

	SendPacket(pUser, MP);
	return bReturn;
}

void CMiniGameTyping::MakeQuestion() {
	m_vecTyping.clear();
	for (int i = 0; i < 3; i++) {
		std::string strTemp;
		strTemp.reserve(21);
		for (int j = 0; j < 20; j++) {
			switch (xRandom(3)) {
				case 0:	strTemp += static_cast<char>(xRandom('0', '9' + 1));	break;
				case 1:	strTemp += static_cast<char>(xRandom('a', 'm' + 1));	break;
				case 2:	strTemp += static_cast<char>(xRandom('n', 'z' + 1));	break;
			}
		}
		m_vecTyping.push_back(strTemp);
	}
}