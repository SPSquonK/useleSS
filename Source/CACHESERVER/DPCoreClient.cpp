#include "StdAfx.h"
#include "DPCacheSrvr.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include "ServerDesc.h"
#include "MyTrace.h"
#include "Player.h"

#ifdef __TRACE1027

#endif	// __TRACE1027

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

CDPCoreClient::CDPCoreClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_PROCSERVER_LIST, &CDPCoreClient::OnProcServerList );
	ON_MSG( PACKETTYPE_LOAD_WORLD, &CDPCoreClient::OnProcServer );
	ON_MSG( PACKETTYPE_JOIN, &CDPCoreClient::OnJoin );
	ON_MSG( PACKETTYPE_LEAVE, &CDPCoreClient::OnLeave );
	ON_MSG( PACKETTYPE_DESTROY_ALLPLAYERS, &CDPCoreClient::OnDestroyAllPlayers );
	ON_MSG( PACKETTYPE_KILLPLAYER, &CDPCoreClient::OnKillPlayer );
}

CDPCoreClient::~CDPCoreClient()
{

}

void CDPCoreClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				g_MyTrace.Add( CMyTrace::Key( "main" ), TRUE, "destroy player or group." );
				break;
			}
	}
}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xEEEEEEEE );
#endif	// __CRASH_0404

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpidUser	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, dw );
#endif	// __CRASH_0404

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn )
		( this->*( pfn ) )( ar, dpidUser );
	else
	{
		g_DPCacheSrvr.Send( lpBuf, uBufSize, dpidUser );
	}
#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xFFFFFFFF );
#endif	// __CRASH_0404
}

void CDPCoreClient::SendDestroyPlayer(const CCachePlayer & pPlayer) {
	BEFORESENDSOLE(ar, PACKETTYPE_DESTROY_PLAYER, pPlayer.GetSerial());
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendAddPlayer(const CCachePlayer & pPlayer, CRTMessenger & rtmessenger) {
	BEFORESENDSOLE(ar, PACKETTYPE_JOIN, pPlayer.GetSerial());
	ar << pPlayer.GetAuthKey();
	ar << pPlayer.GetPlayerId();
	ar << pPlayer.GetNetworkId();
	ar << pPlayer.GetParty();
	ar << pPlayer.GetGuild();
	ar << pPlayer.GetWar();
	ar << pPlayer.GetChannel();
	rtmessenger.Serialize(ar);
	ar.WriteString(pPlayer.GetPlayer());
	ar.WriteString(pPlayer.GetAddr());
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::OnProcServerList( CAr & ar, DPID )
{
	std::uint32_t nSize; ar >> nSize;

	for (std::uint32_t i = 0; i != nSize; ++i) {
		auto pServer = std::make_unique<CServerDesc>();
		ar >> *pServer;
		g_DPClientArray.Connect(std::move(pServer));
	}

	g_MyTrace.AddLine( '-' );
}

void CDPCoreClient::OnProcServer(CAr & ar, DPID) {
	auto pServer = std::make_unique<CServerDesc>();
	ar >> *pServer;
	g_DPClientArray.Connect(std::move(pServer));
}

void CDPCoreClient::OnJoin( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock(g_CachePlayerMng.m_AddRemoveLock );

	DWORD dwSerial;
	BYTE _byData;
	ar >> dwSerial;
	ar >> _byData;	// 사용하지 않는다.

	// TODO: We look for the player by serial, then we destroy them by dpid?
	// It is at least a code smell: is the dwSerial a value only managed server side?
	// Are they unique?

	CCachePlayer * pPlayer	= g_CachePlayerMng.GetPlayerBySerial( dwSerial );
	if (!pPlayer) {
		WriteLog("CDPCoreClient::OnJoin - player not found");
		return;
	}

	CDPClient* pClient = g_DPClientArray.GetClient( pPlayer->GetChannel() );
	
	if (!pClient) {
		WriteLog( "CDPCoreClient.OnJoin: server not found id: %d account: %s player: %s",
			      pPlayer->GetPlayerId(), pPlayer->GetAccount(), pPlayer->GetPlayer() );
		g_DPCacheSrvr.DestroyPlayer( dpid );
		return;
	}

	pPlayer->SetClient(pClient);
	pClient->SendJoin(pPlayer);
}

void CDPCoreClient::OnLeave( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock(g_CachePlayerMng.m_AddRemoveLock );

	DWORD dwSerial;
	ar >> dwSerial;
	CCachePlayer * pPlayer = g_CachePlayerMng.GetPlayerBySerial( dwSerial );  // dpid가 serial번호이다.
	if (pPlayer) g_DPCacheSrvr.DestroyPlayer(dpid);
}

void CDPCoreClient::OnDestroyAllPlayers( CAr & ar, DPID dpid )
{
	g_CachePlayerMng.DestroyPlayer( NULL );
}

void CDPCoreClient::OnKillPlayer( CAr & ar, DPID dpid )
{
	g_DPCacheSrvr.DestroyPlayer( dpid );
}

void CDPCoreClient::SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize )
{
	CCachePlayer * pPlayer	= g_CachePlayerMng.GetPlayer( dpidUser );
	if (!pPlayer) return;

	*reinterpret_cast<UNALIGNED DPID*>( lpMsg )		= pPlayer->GetSerial();
	Send( lpMsg, dwMsgSize, DPID_SERVERPLAYER );
}

CDPCoreClient	g_DPCoreClient;