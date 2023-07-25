#include "stdafx.h"

#include ".\minigameladder.h"

CMiniGameLadder::CMiniGameLadder(void)
{
}

CMiniGameLadder::CMiniGameLadder( CMiniGameBase* pMiniGame )
{
	CMiniGameLadder* pMiniGameLadder = static_cast<CMiniGameLadder*>( pMiniGame );

}

CMiniGameLadder::~CMiniGameLadder(void)
{
}

BOOL CMiniGameLadder::Excute( CUser* pUser, __MINIGAME_PACKET * pMiniGamePacket )
{
	BOOL bReturn = FALSE;
	
	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		SendPacket( pUser, MP );
		return bReturn;
	}	

	
	if( xRandom( 10 ) == 0 ) // 1/10 확률로 성공한다.
	{
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	}
	else
		MP.nState = MP_FAIL;

	SendPacket( pUser, MP );
	return bReturn;
}

