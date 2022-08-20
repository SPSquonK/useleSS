#include "stdafx.h"
#include "user.h"
#include "dpdatabaseclient.h"
#include "dpcoreclient.h"
#include "dploginsrvr.h"

#ifndef __VM_0820
MemPooler<CLoginUser>*	CLoginUser::m_pPool	= new MemPooler<CLoginUser>( 1024 );
#endif	// __VM_0820

CLoginUser::CLoginUser( DPID dpid )
{
	m_pKey[0] = '\0';
	m_dwAuthKey	= 0;
	m_idPlayer	= 0;
	m_dpid	= dpid;
	m_nIndexOfCache	= 0;
	m_tPingRecvd	= m_dwTime	= timeGetTime();
	m_bIllegal	= FALSE;
	if( g_bNProtectAuth )
	{
		m_nWrite	= m_nRead	= 0;
		SetAuth( FALSE );		// 인증이 되어 있지 않음 
	}
	else
	{
		SetAuth( TRUE );		// 인증이 되어 있음 
	}
	m_tAuth		= time( NULL ) + AUTH_PERIOD;
}

void CLoginUser::SetExtra( const char* pKey, DWORD dwAuthKey )
{
	strcpy( m_pKey, pKey );
	m_dwAuthKey	= dwAuthKey;
}

CLoginUserMng::~CLoginUserMng()
{
	Free();
}

void CLoginUserMng::Free( void )
{
	CMclAutoLock Lock( m_AddRemoveLock );

	for( auto i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CLoginUser* pUser	= i->second;
		SAFE_DELETE( pUser );
	}
	m_dpid2User.clear();

	m_ac2User.clear();
}

// dpid를 키로 콘테이너에 넣는다.
BOOL CLoginUserMng::AddUser( DPID dpid, CLoginUser* pUser )
{
	ASSERT( dpid == pUser->m_dpid );

	CMclAutoLock Lock( m_AddRemoveLock );

	if( GetUser( dpid ) == NULL )
	{
		bool bResult	= m_dpid2User.emplace(dpid, pUser).second;
//		ASSERT( bResult );

		m_uCount++;
		return TRUE;
	}
	else
		return FALSE;
}

// pKey 키로 콘테이너에 넣는다.
BOOL CLoginUserMng::AddUser( const char* pKey, CLoginUser * pUser )
{
	// lock이 걸린 상태라고 가정됨 
	bool result = m_ac2User.emplace(pKey, pUser).second;
	return ( result == true );
}

CLoginUser * CLoginUserMng::GetUser( const char* pKey )
{
	const auto i	= m_ac2User.find( pKey );
	if( i != m_ac2User.end() )
		return i->second;
	return NULL;
}

CLoginUser * CLoginUserMng::GetUser( DPID dpid )
{
	const auto i = m_dpid2User.find( dpid );
	if( i != m_dpid2User.end() )
		return i->second;
	return NULL;
}

BOOL CLoginUserMng::RemoveUser( DPID dpid )
{
	CMclAutoLock Lock( m_AddRemoveLock );

	const auto i = m_dpid2User.find( dpid );
	if( i != m_dpid2User.end() )
	{
		CLoginUser* pUser	= i->second;
		m_dpid2User.erase( i );

		m_ac2User.erase( pUser->m_pKey );

		if( !pUser->m_bIllegal )
		{
			if( *pUser->m_pKey != '\0' )
			{
				g_dpDBClient.SendLeave( pUser->m_pKey, pUser->m_idPlayer, timeGetTime() - pUser->m_dwTime );
			}
			else
				WriteLog( "RemoveUser(): *pUser->m_pKey is '\0'" );

			if( pUser->m_idPlayer > 0 )
				g_dpCoreClient.SendLeave( pUser->m_idPlayer );
		}
		else
		{
			WriteLog( "RemoveUser(): Illegal" );
		}
		g_dpLoginSrvr.CacheOut( pUser->m_nIndexOfCache );

		SAFE_DELETE( pUser );
		m_uCount--;
		return TRUE;
	}
	WriteLog( "RemoveUser(): dpid not found" );
	return FALSE;
}

void CLoginUserMng::DestroyAbnormalPlayer( void )
{
#ifdef __INTERNALSERVER
	return;
#endif	

	DWORD t		= timeGetTime();
	CTime time	= CTime::GetCurrentTime();
	//CString strTime	= time.Format( "%Y/%m/%d %H:%M:%S" );
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( auto i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CLoginUser* pUser	= i->second;
		if( ( t - pUser->m_tPingRecvd ) > 90000 ) // 90
			g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
	}
}

CLoginUserMng	g_LoginUserMng;
