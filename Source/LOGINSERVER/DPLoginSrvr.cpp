#include "StdAfx.h"
#include "DPDatabaseClient.h"
#include "dpcoreclient.h"
#include "DPLoginSrvr.h"
#include "User.h"
#include "LoginProtect.h"

extern  char				g_szMSG_VER[];

CDPLoginSrvr::CDPLoginSrvr()
{
	m_handlers.AddHandler( PACKETTYPE_PRE_JOIN, &CDPLoginSrvr::OnPreJoin );
	m_handlers.AddHandler( PACKETTYPE_PING, &CDPLoginSrvr::OnPing );
	m_handlers.AddHandler( PACKETTYPE_QUERYTICKCOUNT, &CDPLoginSrvr::OnQueryTickCount );
	m_handlers.AddHandler( PACKETTYPE_GETPLAYERLIST, &CDPLoginSrvr::OnAddConnection );
}

void CDPLoginSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
			DPID dpid = lpCreatePlayer->dpId;

			CLoginUser* pUser = new CLoginUser( dpid );
		
			if( g_LoginUserMng.AddUser( dpid, pUser ) )
			{
			}
			else
			{
				DestroyPlayer( dpid );
				safe_delete( pUser );
			}
			break;
		}
	case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			OnRemoveConnection( lpDestroyPlayer->dpId );
			break;
		}
	}
}

void CDPLoginSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);
	CAr ar( (LPBYTE)lpMsg + nSize, dwMsgSize - nSize );		// Skip dpid

	if( dwMsgSize < 8 )		// INVALID PACKET
	{
		WriteError( "PACKET//0" );
		return;
	}

	DWORD dw; ar >> dw;
	
	if (m_handlers.Handle(*this, ar, dw, idFrom))
	{
		if (ar.IsOverflow()) Error("Login-Neuz: Packet %08x overflowed", dw);
	}
	else
	{
		if (dw == PACKETTYPE_CREATE_PLAYER || dw == PACKETTYPE_DEL_PLAYER) {
			g_dpDBClient.SendToServer(idFrom, lpMsg, dwMsgSize);
		}
	}
}

void CDPLoginSrvr::SendCacheAddr(DPID dpid) {
	BEFORESEND(ar, PACKETTYPE_CACHE_ADDR);
	ar.WriteString(g_lpAddr.cache);
	SEND(ar, this, dpid);
}

void CDPLoginSrvr::SendError( LONG lError, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_ERROR );
	ar << lError;
	SEND( ar, this, dpid );
}

void CDPLoginSrvr::SendHdr( DWORD dwHdr, DPID dpid )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, dpid );
}

void CDPLoginSrvr::OnAddConnection( CAr & ar, DPID dpid )
{
	char lpszAccount[MAX_ACCOUNT], lpszPassword[MAX_PASSWORD], lpVer[32];
	DWORD dwAuthKey, dwID;

	ar.ReadString( lpVer, 32 );
	ar >> dwAuthKey;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar.ReadString( lpszPassword, MAX_PASSWORD );
	ar >> dwID;	

	if( dwAuthKey == 0 )
	{
		DestroyPlayer( dpid );
		return;
	}

	if( strcmp( lpVer, g_szMSG_VER ) != 0 )
	{
		SendError( ERROR_ILLEGAL_VER, dpid );	
		return;
	}

	CMclAutoLock Lock(g_LoginUserMng.m_AddRemoveLock );

	CLoginUser* pUser = g_LoginUserMng.GetUser( dpid );
	if( pUser == NULL )
		return;

	if( pUser->GetAuth() == FALSE )
	{
		TRACE( "pUser->GetAuth() == FALSE\n" );
		return;
	}

	pUser->SetExtra( lpszAccount, dwAuthKey );
	if(g_LoginUserMng.AddUser( lpszAccount, pUser ) )
	{
		g_dpLoginSrvr.SendCacheAddr( dpid ); 
		g_dpDBClient.SendGetPlayerList( dpid, lpszAccount, lpszPassword, dwAuthKey, (u_long)dwID );
		SendNumPadId( dpid );
	}
	else
	{
		WriteError( "OnAddUser(): AddUser() return 0, %s", lpszAccount );
		CLoginUser* pUser = g_LoginUserMng.GetUser( lpszAccount );
		if( pUser )
			DestroyPlayer( pUser->m_dpid );
		else
			WriteLog( "OnAddUser(): pUser is NULL, %s", lpszAccount );	
		DestroyPlayer( dpid );
	}
}

void CDPLoginSrvr::OnRemoveConnection( DPID dpid )
{
	g_LoginUserMng.RemoveUser( dpid );
}

void CDPLoginSrvr::OnPreJoin( CAr & ar, DPID dpid )
{
	char lpszAccount[MAX_ACCOUNT], lpszPlayer[MAX_PLAYER];
	u_long idPlayer;
	int nSecretNum = 0;

	CMclAutoLock	Lock(g_LoginUserMng.m_AddRemoveLock );
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> nSecretNum;
	CLoginUser* pUser	= g_LoginUserMng.GetUser( dpid );
	if( pUser )
	{
		if( lstrcmp( lpszAccount, pUser->m_pKey ) == 0 )
		{
			if( ::IsUse2ndPassWord() )
			{
				LOGIN_CHECK result = LOGIN_OK;
				result = CLoginProtect::GetInstance()->Check( idPlayer );
				if( result == LOGIN_3TIMES_ERROR )
				{
					SendError( ERROR_15MIN_PREVENT, dpid );
					return;
				}
				u_long idNumPad = CLoginProtect::GetInstance()->GetNumPadId( dpid );
				if( idNumPad <= MAX_NUMPADID && nSecretNum >= 0 && nSecretNum <= MAX_SECRETNUM )
				{
					int nBankPW = CLoginProtect::GetInstance()->GetNumPad2PW( idNumPad, nSecretNum );
					if( nBankPW <= MAX_SECRETNUM )
						g_dpDBClient.SendLoginProtect( lpszAccount, lpszPlayer, idPlayer, nBankPW, dpid );
					else
						SendLoginProtect( FALSE, dpid );
				}
				else
					SendLoginProtect( FALSE, dpid );
			}
			else
			{
				pUser->m_idPlayer	= idPlayer;
				g_dpCoreClient.SendPreJoin( pUser->m_dwAuthKey, lpszAccount, idPlayer, lpszPlayer );	// o
			}
		}
		else
		{
			WriteLog( "OnPreJoin(): Different lpszAccount, %s", lpszAccount );
		}
	}
	else
	{
		WriteLog( "OnPreJoin(): dpid not found, %s", lpszAccount );
	}
}

void CDPLoginSrvr::OnPing( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock(g_LoginUserMng.m_AddRemoveLock );
	CLoginUser* pUser	= g_LoginUserMng.GetUser( dpid );
	if( pUser )
	{
		pUser->m_tPingRecvd		= timeGetTime();

		DWORD dwPingTime;
		ar >> dwPingTime;
		
		BEFORESEND( ar1, PACKETTYPE_PING );
		ar1 << dwPingTime;
		SEND( ar1, this, dpid );
	}
}

void CDPLoginSrvr::OnQueryTickCount( CAr & ar, DPID dpid )
{
	DWORD dwTime;
	ar >> dwTime;

	BEFORESEND( ar1, PACKETTYPE_QUERYTICKCOUNT );
	ar1 << dwTime << g_TickCount.GetTickCount();
	SEND( ar1, this, dpid );
}

void CDPLoginSrvr::SendNumPadId( DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_LOGIN_PROTECT_NUMPAD );
	u_long idNumPad = CLoginProtect::GetInstance()->SetNumPadId( dpid, FALSE );
	ar << idNumPad;
	SEND( ar, this, dpid );
}

void CDPLoginSrvr::SendLoginProtect( BOOL bLogin, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_LOGIN_PROTECT_CERT );
	u_long idNumPad = CLoginProtect::GetInstance()->SetNumPadId( dpid, FALSE );
	ar << bLogin << idNumPad;
	SEND( ar, this, dpid );
}

CDPLoginSrvr	g_dpLoginSrvr;