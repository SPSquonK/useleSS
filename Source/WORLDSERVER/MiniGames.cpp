#include "StdAfx.h"
#include "MiniGames.h"







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

