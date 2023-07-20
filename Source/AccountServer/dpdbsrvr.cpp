#include "stdafx.h"
#include "dpsrvr.h"
#include "dpadbill.h"
#include "dpdbsrvr.h"
#include "account.h"
#include "misc.h"
#include "dbmanager.h"


CDPDBSrvr::CDPDBSrvr()
{
	ON_MSG( PACKETTYPE_MYREG, &CDPDBSrvr::OnAddConnection );
	ON_MSG( PACKETTYPE_REMOVE_ACCOUNT, &CDPDBSrvr::OnRemoveAccount );
	ON_MSG( PACKETTYPE_GETPLAYERLIST, &CDPDBSrvr::OnGetPlayerList );
	ON_MSG( PACKETTYPE_DEL_PLAYER, &CDPDBSrvr::OnRemovePlayer );
	ON_MSG( PACKETTYPE_JOIN_DbAccDb, &CDPDBSrvr::OnJoin );
	ON_MSG( PACKETTYPE_REMOVE_ALLACCOUNTS, &CDPDBSrvr::OnRemoveAllAccounts );
	ON_MSG( PACKETTYPE_BUYING_INFO, &CDPDBSrvr::OnBuyingInfo );
}

void CDPDBSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPDBSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	DWORD dw; ar >> dw;

	const bool handled = Handle(ar, dw, dpId, (LPBYTE)lpMsg, (u_long)dwMsgSize);
	ASSERT(handled);

	if (ar.IsOverflow()) Error("Account-Database: Packet %08x overflowed", dw);
}

void CDPDBSrvr::OnAddConnection( CAr & ar, DPID dpid, LPBYTE, u_long )
{
	DWORD dwIdofServer;
	ar >> dwIdofServer;
	g_AccountMng.AddIdofServer( dpid, dwIdofServer );

#ifdef __SERVERLIST0911
	g_dpSrvr.EnableServer( (DWORD)-1, dwIdofServer, 1L );
	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "EnableServer, %d, %d, %d", -1, dwIdofServer, 1L );
	OutputDebugString( lpOutputString );
#endif	// __SERVERLIST0911
}

void CDPDBSrvr::OnRemoveConnection( DPID dpid )
{
#ifdef __SERVERLIST0911
	DWORD dwIdofServer	= g_AccountMng.RemoveIdofServer( dpid );
	if( dwIdofServer == (DWORD)-1 )
		return;
	g_dpSrvr.EnableServer( (DWORD)-1, dwIdofServer, 0L );
	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "EnableServer, %d, %d, %d", -1, dwIdofServer, 0L );
	OutputDebugString( lpOutputString );
#else	// __SERVERLIST0911
	g_AccountMng.RemoveIdofServer( dpid );
#endif	// __SERVERLIST0911
}

void CDPDBSrvr::OnRemoveAccount(CAr & ar, DPID dpid, LPBYTE, u_long) {
	const auto [lpszAccount] = ar.Extract<SAccountName>();
	g_AccountMng.RemoveAccount(lpszAccount);
}

#ifdef __REMOVE_PLAYER_0221
void CDPDBSrvr::OnRemovePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	const auto [szAccount, _1, _2, idPlayer, dwAuthKey] = ar.Extract<
		SAccountName, char[255], char[255], u_long, DWORD
	>();

	size_t nSize; u_long uTemp;
	ar >> nSize;
	for( size_t i=0; i<nSize; i++ )
		ar >> uTemp;	// 그냥 받기만 한다.(쓸모없음)

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( szAccount );
	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 1/*step : select character*/ )
	{
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
		OutputDebugString( "ACCOUNTSERVER.EXE\t// PACKETTYPE_DEL_PLAYER" );
	}
}
#endif	// __REMOVE_PLAYER_0221

void CDPDBSrvr::OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize ) {
	const auto [dwAuthKey, uIdofMulti, lpszAccount] = ar.Extract<DWORD, u_long, SAccountName>();

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );

	CAccount* pAccount	= g_AccountMng.GetAccount( lpszAccount );
	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 0 )
	{
		pAccount->m_cbRef++;
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
		pAccount->m_fRoute	= TRUE;
		pAccount->m_dwIdofServer	= g_AccountMng.GetIdofServer( dpid );
		pAccount->m_uIdofMulti	= uIdofMulti;

		g_dpSrvr.m_servers.read([&](const CListedServers & servers) {
			if (servers.GetChannel(*pAccount)) {
				g_dpSrvr.DestroyPlayer(pAccount->m_dpid1, pAccount->m_dpid2);
			}
			});
	}
	else
	{
		WriteLog( "^OnGetPlayerList(), %s", lpszAccount );
		SendFail( ERROR_FLYFF_ACCOUNT, lpszAccount, dpid );		// ^
	}
}

void CDPDBSrvr::OnJoin( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	const auto [dwAuthKey, lpszAccount] = ar.Extract<DWORD, SAccountName>();

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( lpszAccount );
	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 1 )
	{
		pAccount->m_cbRef++;
		*(UNALIGNED DWORD*)( lpBuf + uBufSize - sizeof(DWORD) )	= g_AccountMng.PopPCBangPlayer( dwAuthKey );
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
		OutputDebugString( "ACCOUNTSERVER.EXE\t// PACKETTYPE_JOIN_Db[Acc]Db" );
		
		// 동접을 보낸다.

		g_dpSrvr.m_servers.write([&](CListedServers & servers) {
			if (CListedServers::Channel * channel = servers.GetChannel(*pAccount)) {
				channel->lCount += 1;
			}
			});
	}
}

void CDPDBSrvr::SendCloseExistingConnection( const char* lpszAccount, LONG lError )
{
	BEFORESEND( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION );
	ar.WriteString( lpszAccount );

	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPDBSrvr::SendPlayerCount() {
	// 1/ Compute idOfServer to dpid map
	std::array<DWORD, 64> adpid;
	adpid.fill(DPID_UNKNOWN);

	g_AccountMng.m_AddRemoveLock.Enter();
	for (const auto & [dpid, idOfServer] : g_AccountMng.m_2IdofServer) {
		if (idOfServer >= 0 && idOfServer < 64) {
			adpid[idOfServer] = dpid;
		}
	}
	g_AccountMng.m_AddRemoveLock.Leave();


	// 2/ Log count for each channel
	static bool foundOneBadServer = false;

	g_dpSrvr.m_servers.read([&](const CListedServers & servers) {
		for (const CListedServers::Server & server : servers.GetServers()) {
			boost::container::static_vector<int, CListedServers::MaxChannels + 1> anCount;

			for (const CListedServers::Channel & channel : server.channels) {
				anCount.emplace_back(channel.lCount);
			}

			if (server.dwID >= 0 && server.dwID < 64 && adpid[server.dwID] != DPID_UNKNOWN) {
				BEFORESEND(ar, PACKETTYPE_PLAYER_COUNT);
				ar << server.dwID << anCount;
				SEND(ar, this, adpid[server.dwID]);
			}
		}
		});
}

void CDPDBSrvr::OnRemoveAllAccounts(CAr & ar, DPID dpid, LPBYTE, u_long) {
	g_AccountMng.RemoveIdofServer(dpid, FALSE);
}


void CDPDBSrvr::SendFail( long lError, const char* lpszAccount, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_FAIL );
	ar << lError;
	ar.WriteString( lpszAccount );
	SEND( ar, this, dpid );
}




void CDPDBSrvr::SendOneHourNotify( CAccount* pAccount )
{
	BEFORESEND( ar, PACKETTYPE_ONE_HOUR_NOTIFY );
	ar.WriteString( pAccount->m_lpszAccount );
	
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPDBSrvr::SendBuyingInfo( PBUYING_INFO2 pbi2 )
{
	BEFORESEND( ar, PACKETTYPE_BUYING_INFO );
	ar.Write( pbi2, sizeof(BUYING_INFO2) );
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPDBSrvr::OnBuyingInfo( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	auto [bi2, iSerialNumber] = ar.Extract<BUYING_INFO2, SERIALNUMBER>();

	if (g_BuyingInfoMng.Remove(bi2.dwKey)) {
		g_DPAdbill.Send(&bi2, sizeof(BUYING_INFO), bi2.dpid);

		LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.m_pDbIOData->Alloc();
		memcpy( lpDbOverlappedPlus->lpBuf, (LPBYTE)lpBuf + sizeof(DWORD), uBufSize - sizeof(DWORD) );
		lpDbOverlappedPlus->uBufSize	= uBufSize - sizeof(DWORD);
		lpDbOverlappedPlus->nQueryMode	= LOG_SM_ITEM;
		PostQueuedCompletionStatus( g_DbManager.m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped );
	}
	else
	{
		// 치명적 오류 : 어카운트 서버에는 지급 명령에 대한 정보가 없다.
	}

}

void CDPDBSrvr::SendLogSMItem()
{
}

#ifdef __SERVERLIST0911
void CDPDBSrvr::OnServerEnable( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	int nServer, nMulti;
	long lEnable;
	ar >> nServer >> nMulti >> lEnable;
	g_dpSrvr.EnableServer( (DWORD)nServer, (DWORD)nMulti, lEnable );

	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "EnableServer, %d, %d, %d", nServer, nMulti, lEnable );
	OutputDebugString( lpOutputString );
}
#endif	// __SERVERLIST0911

CDPDBSrvr	g_dpDbSrvr;