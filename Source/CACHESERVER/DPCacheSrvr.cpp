#include "StdAfx.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include "DPCacheSrvr.h"
#include "Player.h"
#include <set>

#ifdef __PL_0917
#include "packetlog.h"
#endif	// __PL_0917
#include "mytrace.h"

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

void CDPCacheSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static	int	cbPlayer	= 0;

	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
#ifdef __CRASH_0404
				CCrashStatus::GetInstance()->SetLastPacket( this, 0xCCCCCCCC );
#endif	// __CRASH_0404
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				g_MyTrace.Add( CMyTrace::Key( "so" ), FALSE, "Nb of players: %04d", ++cbPlayer );
				g_CachePlayerMng.AddPlayer(lpCreatePlayer->dpId);
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
#ifdef __CRASH_0404
				CCrashStatus::GetInstance()->SetLastPacket( this, 0xDDDDDDDD );
#endif	// __CRASH_0404
				g_MyTrace.Add( CMyTrace::Key( "so" ), FALSE, "Nb of players: %04d", --cbPlayer );
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xFFFFFFFF );
#endif	// __CRASH_0404
}


void CDPCacheSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xEEEEEEEE );
#endif	// __CRASH_0404

	CAr ar( (LPBYTE)lpMsg + nSize, dwMsgSize - nSize );		// Skip dpid

	if( dwMsgSize < 8 )
		return;

	DWORD dw; ar >> dw;
	
#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, dw );
#endif	// __CRASH_0404

#ifdef __PL_0917
	CPacketLog::Instance()->Add( idFrom, dw, dwMsgSize );
#endif	// __PL_0917


	if (dw == PACKETTYPE_JOIN_NeuzCache) {
		OnAddConnection(ar, idFrom, (BYTE *)lpMsg + sizeof(DPID), dwMsgSize - sizeof(DPID));
		if (ar.IsOverflow()) Error("Cache-World: Packet %08x overflowed", dw);
	} else if (dw == PACKETTYPE_KEEP_ALIVE) {
		OnKeepAlive(ar, idFrom, (BYTE *)lpMsg + sizeof(DPID), dwMsgSize - sizeof(DPID));
		if (ar.IsOverflow()) Error("Cache-World: Packet %08x overflowed", dw);
	} else {
		switch( dw )
		{
			case PACKETTYPE_ADDPARTYMEMBER_NeuzCore:
			case PACKETTYPE_REMOVEPARTYMEMBER_NeuzCore:
			case PACKETTYPE_CHANGETROUP:
			case PACKETTYPE_CHANPARTYNAME:
			case PACKETTYPE_PARTYCHANGEITEMMODE:
			case PACKETTYPE_PARTYCHANGEEXPMODE:
			case PACKETTYPE_NC_ADDFRIEND:
			case PACKETTYPE_GETFRIENDSTATE:
			case PACKETTYPE_SETFRIENDSTATE:
			case PACKETTYPE_FRIENDINTERCEPTSTATE:
			case PACKETTYPE_REMOVEFRIEND:
			case PACKETTYPE_PARTYCHANGELEADER:
			case PACKETTYPE_DESTROY_GUILD:
			case PACKETTYPE_ADD_GUILD_MEMBER:
			case PACKETTYPE_REMOVE_GUILD_MEMBER:
			case PACKETTYPE_GUILD_MEMBER_LEVEL:
			case PACKETTYPE_GUILD_AUTHORITY:	
			case PACKETTYPE_GUILD_PENYA:
			case PACKETTYPE_GUILD_SETNAME:
			case PACKETTYPE_GUILD_CLASS:
			case PACKETTYPE_GUILD_NICKNAME:
			case PACKETTYPE_DECL_GUILD_WAR:		// 선전 포고
			case PACKETTYPE_ACPT_GUILD_WAR:		// 개전
			case PACKETTYPE_SURRENDER:	// 항복
			case PACKETTYPE_QUERY_TRUCE:	// 휴전
			case PACKETTYPE_ACPT_TRUCE:		// 휴전

#ifdef __GUILDVOTE
			case PACKETTYPE_NC_ADDVOTE:	   // 투표 입력 
			case PACKETTYPE_NC_REMOVEVOTE: // 투표 취소 
			case PACKETTYPE_NC_CLOSEVOTE:  // 투표 종료 
			case PACKETTYPE_NC_CASTVOTE:   // 투표하기 
#endif  // __GUILDVOTE
			case PACKETTYPE_CHG_MASTER:   // 길드 마스터 변경 
			case PACKETTYPE_SENDTAG:
				g_DPCoreClient.SendToServer( idFrom, lpMsg, dwMsgSize );
				break;
			case PACKETTYPE_SQUONK_ARBITRARY_PACKET:
			{
				// SquonK Arbitrary Packet can only be used from localhost
				CMclAutoLock	Lock(g_CachePlayerMng.m_AddRemoveLock);
				CCachePlayer * pPlayer = g_CachePlayerMng.GetPlayer(idFrom);
				if (!pPlayer) break;
				if (pPlayer->GetAddr() != std::string_view("127.0.0.1")) break;
			}
			[[fallthrough]];
			default:
				g_DPClientArray.SendToServer( idFrom, lpMsg, dwMsgSize );
				break;
		}
	}

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xFFFFFFFF );
#endif	// __CRASH_0404
}


void CDPCacheSrvr::OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	CMclAutoLock	Lock(g_CachePlayerMng.m_AddRemoveLock );
	CCachePlayer * pPlayer	= g_CachePlayerMng.GetPlayer( dpid );
	if (!pPlayer) {
		DestroyPlayer(dpid);
		return;
	}

	pPlayer->SetAddr( this );
	pPlayer->Join( ar ); // TODO: this is bad and whoever wrote this should feel bad
	CRTMessenger rtmessenger;
	int nSize	= rtmessenger.Serialize( ar );
	if (nSize) {
		Error("CRTMessenger.Serialize: %s(%d)", pPlayer->GetAddr(), nSize);
		DestroyPlayer(dpid);
	} else {
		if (!IsValid(rtmessenger.GetState())) {
			rtmessenger.SetState(FriendStatus::ONLINE);
		}

		g_DPCoreClient.SendAddPlayer(*pPlayer, rtmessenger);
	}
}

void CDPCacheSrvr::OnRemoveConnection( DPID dpid )
{
	CMclAutoLock	Lock(g_CachePlayerMng.m_AddRemoveLock);
	
	CCachePlayer * pPlayer	= g_CachePlayerMng.GetPlayer( dpid );
	if( pPlayer )
	{
		if( pPlayer->GetClient() )	// JOIN패킷을 보낸 플레이어만 LEAVE패킷을 보낸다.
		{
			BEFORESENDSOLE( ar, PACKETTYPE_LEAVE, DPID_UNKNOWN );
			ar << pPlayer->GetPlayerId();
			LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
			g_DPClientArray.SendToServer( dpid, lpBuf, nBufSize );
		}

		g_CachePlayerMng.RemovePlayer( dpid );
	}
	else
	{
		WriteError( "CDPCacheSrvr::OnRemoveConnection GetPlayer return NULL." );
	}
}

void CDPCacheSrvr::OnKeepAlive( CAr & ar, DPID dpid, LPBYTE, u_long )
{
	CMclAutoLock	Lock(g_CachePlayerMng.m_AddRemoveLock);
	CCachePlayer * pPlayer = g_CachePlayerMng.GetPlayer(dpid);
	if (pPlayer) pPlayer->SetAlive(true);
}

CDPCacheSrvr	g_DPCacheSrvr;
