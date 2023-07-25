#pragma once


enum { MP_NONE = 0, MP_OPENWND, MP_TRUE, MP_FALSE, MP_FINISH, MP_FAIL };

struct __MINIGAME_PACKET
{
	WORD wNowGame;
	int nState;
	int nParam1;
	int nParam2;
	std::vector<std::string> vecszData;
	__MINIGAME_PACKET( WORD wG = 0x0000, int nS = MP_NONE, int nP1 = 0, int nP2 = 0 )
	{ wNowGame=wG; nState=nS; nParam1=nP1; nParam2=nP2; }

	friend CAr & operator<<(CAr & ar, const __MINIGAME_PACKET & s) {
		ar << s.wNowGame << s.nState << s.nParam1 << s.nParam2;

		ar << s.vecszData.size();
		for (size_t i = 0; i < s.vecszData.size(); i++)
			ar.WriteString(s.vecszData[i].c_str());

		if (s.vecszData.size() >= 5) {
			Error(__FUNCTION__ " just sent %lu elements which is not supported by reception", s.vecszData.size());
		}

		return ar;
	}

	friend CAr & operator>>(CAr & ar, __MINIGAME_PACKET & s) {
		ar >> s.wNowGame >> s.nState >> s.nParam1 >> s.nParam2;

		s.vecszData.clear();
		size_t nSize = 0; 
		ar >> nSize;
		if (nSize >= 5) {
			nSize = 5;
		}

		for (size_t i = 0; i < nSize; i++) {
			char szTemp[256] = { 0, };
			ar.ReadString(szTemp, 256);
			s.vecszData.push_back(szTemp);
		}

		return ar;
	}
};


#ifdef __WORLDSERVER
class CMiniGameBase {
public:
	CMiniGameBase() = default;
	CMiniGameBase(const CMiniGameBase &) = delete;
	CMiniGameBase & operator=(const CMiniGameBase &) = delete;
	virtual ~CMiniGameBase();

	virtual BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) = 0;
	void	SendPacket(CUser * pUser, const __MINIGAME_PACKET & MiniGamePacket);	// 클라이언트로 미니게임 패킷을 보낸다.
};
#endif // __WORLDSERVER

