#include "stdafx.h"
#include "user.h"
#include "dploginsrvr.h"
#include "dpcoreclient.h"
#include "msghdr.h"

CDPCoreClient::CDPCoreClient() {
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, &CDPCoreClient::OnQueryTickCount );
	ON_MSG( PACKETTYPE_PRE_JOIN      , &CDPCoreClient::OnPreJoin );
	ON_MSG( PACKETTYPE_DESTROY_PLAYER, &CDPCoreClient::OnQueryRemovePlayer );
}

void CDPCoreClient::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom) {
}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID ) {
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	DWORD dw; ar >> dw;

	if (Handle(ar, dw)) {
		if (ar.IsOverflow()) Error("Login-Core: Packet %08x overflowed", dw);
	}
}

HANDLE	hWait;
void CDPCoreClient::QueryTickCount( void )
{
	hWait	= CreateEvent( NULL, FALSE, FALSE, NULL );

	BEFORESEND( ar, PACKETTYPE_QUERYTICKCOUNT );
	ar << (DWORD)timeGetTime();
	SEND( ar, this, DPID_SERVERPLAYER );

	WaitForSingleObject( hWait, INFINITE );
	CloseHandle( hWait );
}

void CDPCoreClient::OnQueryTickCount( CAr & ar )
{
	DWORD dwTime;
	__int64 nTickCount;
	ar >> dwTime >> nTickCount;

	dwTime	= timeGetTime() - dwTime;
	nTickCount	+= ( dwTime * 5000 );	// ( dwTime / 2 ) * 10000

	g_TickCount.Start( nTickCount );

	SetEvent( hWait );
}

void CDPCoreClient::SendPreJoin( DWORD dwAuthKey, const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer )
{
	BEFORESEND( ar, PACKETTYPE_PRE_JOIN );
	ar << dwAuthKey;
	ar.WriteString( lpszAccount );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnPreJoin( CAr & ar )
{
	CHAR lpszAccount[MAX_ACCOUNT], lpszPlayer[MAX_PLAYER];
	DWORD dwAuthKey;
	u_long idPlayer;
	BYTE f;
	ar >> dwAuthKey;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> f;

	CMclAutoLock	Lock(g_LoginUserMng.m_AddRemoveLock );

	CLoginUser * pUser	= g_LoginUserMng.GetUser( lpszAccount );
	if( pUser )
	{
		if( pUser->m_dwAuthKey == dwAuthKey )
		{
			if( f )	// o
				g_dpLoginSrvr.SendHdr( PACKETTYPE_PRE_JOIN, pUser->m_dpid );
			else
			{
				WriteLog( "OnPreJoin(): recv 0 from CORE, %s", lpszAccount );
				pUser->m_idPlayer	= 0;
				g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
			}
		}
		else
		{
			WriteLog( "OnPreJoin(): Different dwAuthKey" );
			g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
		}
	}
	else
	{
		WriteLog( "OnPreJoin(): Account not found, %s", lpszAccount );
		//
	}
}

void CDPCoreClient::OnQueryRemovePlayer( CAr & ar )
{
	const auto [lpszAccount] = ar.Extract<StaticString<MAX_ACCOUNT>>();

	CMclAutoLock	Lock(g_LoginUserMng.m_AddRemoveLock );
	CLoginUser * pUser	= g_LoginUserMng.GetUser(lpszAccount);
	if( pUser )
	{
		pUser->m_idPlayer	= 0;
		g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
	}
	else
	{
		WriteLog( "OnQueryRemovePlayer(): lpszAccount not found, %s", lpszAccount );
	}
}

void CDPCoreClient::SendLeave(u_long idPlayer) {
	SendPacket<PACKETTYPE_LEAVE, u_long>(idPlayer);
}

CDPCoreClient	g_dpCoreClient;