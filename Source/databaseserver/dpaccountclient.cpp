#include "StdAfx.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dpaccountclient.h"
#include "serialnumber.h"

CDPAccountClient::CDPAccountClient()
{
	ON_MSG( PACKETTYPE_GETPLAYERLIST, &CDPAccountClient::OnGetPlayerList );
#ifdef __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_DEL_PLAYER, &CDPAccountClient::OnRemovePlayer );
#endif	// __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_JOIN, &CDPAccountClient::OnJoin );
	ON_MSG( PACKETTYPE_PLAYER_COUNT, &CDPAccountClient::OnPlayerCount );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPAccountClient::OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, &CDPAccountClient::OnOneHourNotify );
	ON_MSG( PACKETTYPE_FAIL, &CDPAccountClient::OnFail );
	ON_MSG( PACKETTYPE_BUYING_INFO, &CDPAccountClient::OnBuyingInfo );
}

CDPAccountClient::~CDPAccountClient()
{
}

void CDPAccountClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
				break;
	}
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	DWORD dw; ar >> dw;
	const bool ok = Handle( ar, dw, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
	ASSERT(ok);
	if (ar.IsOverflow()) Error("Database-Account: Packet %08x overflowed", dw);
}

#ifdef __SERVERLIST0911
void CDPAccountClient::SendServerEnable( int nMulti, long lEnable )
{
	BEFORESEND( ar, PACKETTYPE_SERVER_ENABLE  );
	ar << (int)g_appInfo.dwSys << nMulti << lEnable;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif	// __SERVERLIST0911

void CDPAccountClient::SendAddIdofServer( DWORD dwIdofServer )
{
	SendPacket<PACKETTYPE_MYREG, DWORD>(dwIdofServer);
}

void CDPAccountClient::SendRemoveAccount( char* lpszAccount )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_ACCOUNT );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAllAccounts()
{
	SendPacket<PACKETTYPE_REMOVE_ALLACCOUNTS>();
}

#ifdef __REMOVE_PLAYER_0221
void CDPAccountClient::OnRemovePlayer( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode = REMOVEPLAYER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
/*
	account
	password
	szNo
	idPlayer
	dwAuthKey
	dpid
*/
}
#endif	// __REMOVE_PLAYER_0221

void CDPAccountClient::OnGetPlayerList( CAr & ar, LPBYTE, u_long )
{
	DWORD dwAuthKey;
	ar >> dwAuthKey;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT );
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, MAX_PASSWORD );
	ar >> lpDbOverlappedPlus->dpid;
	lpDbOverlappedPlus->nQueryMode	= SENDPLAYERLIST;
	lpDbOverlappedPlus->dwAuthKey	= dwAuthKey;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnJoin( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= JOIN;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnPlayerCount( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= SAVECONCURRENTUSERNUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnCloseExistingConnection( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	LONG lError = 0;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	g_dpLoginSrvr.SendCloseExistingConnection( lpszAccount, lError );
}


void CDPAccountClient::OnOneHourNotify( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	g_dpLoginSrvr.SendOneHourNotify( lpszAccount );
}

void CDPAccountClient::OnFail( CAr & ar, LPBYTE, u_long )
{
	long lError;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar >> lError;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	switch( lError )
	{
		case ERROR_FLYFF_ACCOUNT:
			{
				g_dpLoginSrvr.SendFail( lError, lpszAccount );
				break;
			}
	}
}

void CDPAccountClient::OnBuyingInfo( CAr & ar, LPBYTE lpBuf, u_long uBufSize )	// 2
{
	OutputDebugString( "DATABASESERVER.EXE\t// recv from account\n" );

	BUYING_INFO2 bi2;
	ar.Read( &bi2, sizeof(BUYING_INFO2) );
	if( bi2.dwServerIndex == g_appInfo.dwSys )
		g_dpCoreSrvr.SendBuyingInfo( &bi2 );
}

void CDPAccountClient::SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber )
{
	BEFORESEND( ar, PACKETTYPE_BUYING_INFO );
	ar.Write( (void*)pbi2, sizeof(BUYING_INFO2) );
	ar << iSerialNumber;
	SEND( ar, this, DPID_SERVERPLAYER );
}

CDPAccountClient	g_dpAccountClient;