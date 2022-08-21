#include "stdafx.h"

#ifdef __MA_VER11_07	// cache 서버에서 loginserver로 유저접속끊낌을 알려주는 부분
#include "DPCoreClient.h"
#endif // __MA_VER11_07	// cache 서버에서 loginserver로 유저접속끊낌을 알려주는 부분

#include "DPCacheSrvr.h"
#include "DPClient.h"
#include "Player.h"
#include "ServerDesc.h"
#include "MyTrace.h"
#ifdef __TRACE1027

#endif	// __TRACE1027

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

CDPClientArray	g_DPClientArray;

void CDPClient::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD, DPID) {
	if (lpMsg->dwType == DPSYS_DESTROYPLAYERORGROUP) {
		g_DPClientArray.Remove(this);
	}
}

void CDPClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static constexpr size_t nSize = sizeof(DPID);

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xEEEEEEEE );
#endif	// __CRASH_0404

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	const DPID dpidUser = *(UNALIGNED LPDPID)lpMsg;

	CAr ar(lpBuf, uBufSize);
	GETTYPE( ar );
		
#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, dw );
#endif	// __CRASH_0404

	// Hard coded dispatch
	if (dw == PACKETTYPE_QUERY_DESTROY_PLAYER) {
		OnQueryDestroyPlayer(ar, dpidUser, lpMsg, dwMsgSize);
	} else {
		// Send to Neuz
		g_DPCacheSrvr.Send(lpBuf, uBufSize, dpidUser);
	}

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xFFFFFFFF );
#endif	// __CRASH_0404
}


void CDPClient::OnReplace(CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize) {
}

void CDPClient::SendJoin(CCachePlayer * pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_JOIN, pPlayer->GetSerial() );
	ar << pPlayer->GetAuthKey();
	ar << pPlayer->GetPlayerId();
	ar << pPlayer->GetSlot();
	ar << pPlayer->GetNetworkId();
	ar.WriteString( pPlayer->GetAccount() );
	ar.WriteString( pPlayer->GetPass() );
	ar.WriteString( pPlayer->GetAddr() );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPClient::OnQueryDestroyPlayer( CAr & ar, DPID dpid, LPVOID lpBuffer, u_long uBufSize )
{
	DWORD dwSerial;
	ar >> dwSerial;
	u_long idPlayer;
	ar >> idPlayer;

	CMclAutoLock	Lock(g_CachePlayerMng.m_AddRemoveLock);

	CCachePlayer * pPlayer	= g_CachePlayerMng.GetPlayerBySerial( dwSerial );
	if( pPlayer )
	{
		// 성공적으로 찾았으니까 접속을 끊는다. 
		g_DPCacheSrvr.DestroyPlayer( pPlayer->GetNetworkId() );	// 소켓연결이 끊기고, PACKETTYPE_LEAVE가 월드서버로 간다.
#ifdef __MA_VER11_07	// cache 서버에서 loginserver로 유저접속끊낌을 알려주는 부분
		g_DPCoreClient.SendDestroyPlayer(*pPlayer);// coresvr를 통해 loginsvr 에도 끊낌을 알려준다
#endif // __MA_VER11_07	// cache 서버에서 loginserver로 유저접속끊낌을 알려주는 부분
		return;	
	}

	WriteError( "OnQueryDestroyPlayer - idPlayer:%d GetPlayer return NULL.", idPlayer );

	// 찾지 못해도, 끊었다고 알려준다.
	BEFORESENDSOLE( out, PACKETTYPE_LEAVE, DPID_UNKNOWN );
	out << idPlayer;
	LPBYTE lpBuf = out.GetBuffer( &nBufSize );
	g_DPClientArray.SendToServer( dpid, lpBuf, nBufSize );
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CDPClientArray::~CDPClientArray() {
	Free();
}

void CDPClientArray::Free() {
	CMclAutoLock Lock(m_AddRemoveLock);

	for (CDPClient *& pClient : m_active) {
		pClient->DeleteDPObject();
		SAFE_DELETE(pClient);
	}

	for (CDPClient *& pClient : m_free) {
		pClient->DeleteDPObject();
		SAFE_DELETE(pClient);
	}
}

bool CDPClientArray::Connect(std::unique_ptr<CServerDesc> pServer)
{
	CMclAutoLock Lock( m_AddRemoveLock );
	CServerDescArray::iterator i = m_apServer.find( pServer->GetKey() );
	if( i == m_apServer.end() )
	{
		CDPClient* pClient;
		if(m_free.empty()) {
			pClient		= new CDPClient;
		} else {
			pClient = m_free.back();
			m_free.erase(m_free.end() - 1);
		}
		pClient->m_pServer	= pServer.get();
		g_MyTrace.Add( pServer->GetKey(), TRUE, "World: %04lu", pServer->GetKey() );
		if( pClient->CreateDPObject( FALSE ) && pClient->InitConnection( (LPVOID)pServer->m_szAddr, PN_WORLDSRVR + (u_short)pServer->GetKey() ) && pClient->JoinSession( NULL ) )
		{
			const u_long key = pServer->GetKey();
			bool bResult	= m_apServer.emplace( key, pServer.release() ).second;
			ASSERT( bResult );
			m_active.emplace_back(pClient);
			g_MyTrace.Add(key, FALSE, "World: %04lu", key);
			return TRUE;
		}
		SAFE_DELETE( pClient );
	}
	return FALSE;
}

bool CDPClientArray::Remove(CDPClient * pClient) {
	g_CachePlayerMng.DestroyPlayer( pClient );
	CMclAutoLock Lock( m_AddRemoveLock );

	const auto it = std::ranges::find(m_active, pClient);
	if (it == m_active.end()) return false;

	m_active.erase(it);

	auto itServer = m_apServer.find( pClient->m_pServer->GetKey() );
	if (itServer != m_apServer.end()) {
		m_apServer.erase(itServer);
		g_MyTrace.Add(pClient->m_pServer->GetKey(), TRUE, "World: %04lu", pClient->m_pServer->GetKey());
		SAFE_DELETE(pClient->m_pServer);
	}

	m_free.emplace_back(pClient);

	return true;
}

CDPClient* CDPClientArray::GetClient( const u_long uIdofMulti )
{
	if( uIdofMulti == NULL_ID )
		return nullptr;

	CMclAutoLock Lock( m_AddRemoveLock );
	
	const auto it = std::ranges::find_if(m_active, [&](CDPClient * pClient) {
		return pClient->m_pServer->GetIdofMulti() == uIdofMulti;
		});

	return it != m_active.end() ? *it : nullptr;
}

void CDPClientArray::SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize )
{
	CMclAutoLock Lock( m_AddRemoveLock );

	CCachePlayer * pPlayer = g_CachePlayerMng.GetPlayer( dpidUser );
	if( pPlayer )
	{
		if( pPlayer->GetClient() )
			pPlayer->GetClient()->SendToServer( pPlayer->GetSerial(), lpMsg, dwMsgSize );
	}
}
