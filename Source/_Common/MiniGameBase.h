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

	void Serialize( CAr & ar )
	{
		if (ar.IsStoring())
			ar << wNowGame << nState << nParam1 << nParam2;
		else
			ar >> wNowGame >> nState >> nParam1 >> nParam2;

		if( ar.IsStoring() )
		{
			ar << vecszData.size();
			for( DWORD i=0; i<vecszData.size(); i++ )
				ar.WriteString( vecszData[i].c_str() );

			if (vecszData.size() >= 5) {
				Error(__FUNCTION__ " just sent %lu elements which is not supported by reception", vecszData.size());
			}
		}
		else
		{
			int nSize = 0; vecszData.clear();
			ar >> nSize;
			if (nSize >= 5) {
				nSize = 5;
			}

			for( int i=0; i<nSize; i++ )
			{
				char szTemp[256] = {0,};
				ar.ReadString( szTemp, 256 );
				vecszData.push_back( szTemp );
			}
		}
	}
};


#ifdef __WORLDSERVER
class CMiniGameBase
{
public:
		
	CMiniGameBase(void);
	virtual ~CMiniGameBase(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET * pMiniGamePacket ) = FALSE;
	void	SendPacket( CUser* pUser, __MINIGAME_PACKET MiniGamePacket );	// 클라이언트로 미니게임 패킷을 보낸다.
	void	SendExtPacket( CUser* pUser, __MINIGAME_PACKET MiniGameExtPacket );
};
#endif // __WORLDSERVER

