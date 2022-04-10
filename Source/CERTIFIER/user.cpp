#include "stdafx.h"
#include "user.h"
#include "msghdr.h"
#ifdef _DEBUG
#include "mytrace.h"
extern	CMyTrace	g_MyTrace;
#endif	// _DEBUG

CCertUser::CCertUser( DPID dpid )
:
m_dpid( dpid ),
m_bValid( TRUE )
{
	m_dwTick	= GetTickCount();
	*m_pszAccount	= '\0';
}

CCertUserMng::CCertUserMng()
{
	
}

CCertUserMng::~CCertUserMng()
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( auto i	= begin(); i != end(); ++i )
		safe_delete( i->second );
	clear();
}

BOOL CCertUserMng::AddUser( DPID dpid )
{
	CCertUser * pUser	= new CCertUser( dpid );

	CMclAutoLock	Lock( m_AddRemoveLock );

	bool bResult	= insert( map<DPID, CCertUser *>::value_type( dpid, pUser ) ).second;
	if( bResult == false )
	{
		WriteError( "ADD//0" );
		SAFE_DELETE( pUser );
		return FALSE;
	}
#ifdef _DEBUG
	g_MyTrace.Add( CMyTrace::Key( "count" ), FALSE, "// %04d", size() );
#endif	// _DEBUG
	return TRUE;
}

BOOL CCertUserMng::RemoveUser( DPID dpid )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	map<DPID, CCertUser *>::iterator i	= find( dpid );
	if( i != end() )
	{
		CCertUser * pRemoved		= i->second;
		SAFE_DELETE( pRemoved );
		erase( dpid );
#ifdef _DEBUG
	g_MyTrace.Add( CMyTrace::Key( "count" ), FALSE, "// %04d", size() );
#endif	// _DEBUG
		return TRUE;
	}
	WriteError( "REMOVE//0" );
	return FALSE;
}

CCertUser * CCertUserMng::GetUser( DPID dpid )
{
	auto i	= find( dpid );
	if( i != end() )
		return i->second;
	return NULL;
}

void CCertUserMng::ClearDum( CDPMng* pdp )
{
	BEFORESEND( ar, PACKETTYPE_KEEP_ALIVE );

	CCertUser * pUsertmp;
	DWORD dwTick	= GetTickCount() - SEC( 10 );

	CMclAutoLock	Lock( m_AddRemoveLock );

	for( auto i = begin(); i != end(); ++i )
	{
		pUsertmp	= i->second;
		if( pUsertmp->m_dwTick < dwTick )
		{
			pdp->DestroyPlayer( pUsertmp->m_dpid );
		}
		else if( pUsertmp->m_bValid == FALSE )
		{
			pUsertmp->m_bValid	= TRUE;
			pdp->DestroyPlayer( pUsertmp->m_dpid );
		}
		else
		{
			pUsertmp->m_bValid	= FALSE;
			SEND( ar, pdp, pUsertmp->m_dpid );
		}
	}
}

CCertUserMng * CCertUserMng::GetInstance( void )
{
	static	CCertUserMng	sUserMng;
	return	&sUserMng;
}