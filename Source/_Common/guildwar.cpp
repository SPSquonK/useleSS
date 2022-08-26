#include "stdafx.h"

#include "guildwar.h"
#include "guild.h"
#include "sqktd/algorithm.h"

#ifdef __CORESERVER
#include "dpcoresrvr.h"
#include "dpcachesrvr.h"
#include "dpdatabaseclient.h"
#include "player.h"
#endif	// __CORESERVER
#ifdef __WORLDSERVER
#include "user.h"
#include "dpcoreclient.h"
#endif	// __WORLDSERVER

#ifdef __CLIENT
#include "definetext.h"
#endif	// __CLIENT

CGuildWar::CGuildWar()
{
	ZeroMemory( &m_Decl, sizeof(m_Decl) );
	ZeroMemory( &m_Acpt, sizeof(m_Acpt) );
}

CAr & operator<<(CAr & ar, const CGuildWar & self) {
	ar << self.m_idWar;
	ar.Write(&self.m_Decl, sizeof(self.m_Decl));
	ar.Write(&self.m_Acpt, sizeof(self.m_Acpt));
	ar << self.m_nFlag;
	ar << static_cast<time_t>(self.m_time.GetTime());
	return ar;
}

CAr & operator>>(CAr & ar, CGuildWar & self) {
	ar >> self.m_idWar;
	ar.Read(&self.m_Decl, sizeof(self.m_Decl));
	ar.Read(&self.m_Acpt, sizeof(self.m_Acpt));
	ar >> self.m_nFlag;
	time_t time;
	ar >> time;
	self.m_time = CTime(time);
	return ar;
}

#ifdef __WORLDSERVER
void CGuildWar::Process( CTime & time )
{
	if( m_nFlag == WF_WARTIME )
	{
		if( GetEndTime() < time )
		{
			m_nFlag	= WF_END;
			g_DPCoreClient.SendWarTimeout( m_idWar );
			TRACE( "SendWarTimeout, %d\n", m_idWar );
		}
		else
		{
			CGuild* pDecl	= g_GuildMng.GetGuild( m_Decl.idGuild );
			CGuild* pAcpt	= g_GuildMng.GetGuild( m_Acpt.idGuild );
			if( pDecl && pAcpt )
			{
				CUser* pMaster	= g_UserMng.GetUserByPlayerID( pDecl->m_idMaster );
				if( IsInvalidObj( pMaster ) )
					g_DPCoreClient.SendWarMasterAbsent( m_idWar, TRUE );
				pMaster	= g_UserMng.GetUserByPlayerID( pAcpt->m_idMaster );
				if( IsInvalidObj( pMaster ) )
					g_DPCoreClient.SendWarMasterAbsent( m_idWar, FALSE );
			}
		}
	}
}
#endif	// __WORLDSERVER

CGuildWarMng::~CGuildWarMng()
{
	Clear();
}

void CGuildWarMng::Clear( void )
{
	for( auto i = m_mapPWar.begin(); i != m_mapPWar.end(); ++i )
		safe_delete( i->second );
	m_mapPWar.clear();
}

WarId	CGuildWarMng::AddWar(CGuildWar * const pWar) {
	if (pWar->m_idWar != WarIdNone) {
		m_id = pWar->m_idWar;
	} else {
		m_id = WarId(m_id.get() + 1);
	}

	if (GetWar(m_id)) return WarIdNone;

	pWar->m_idWar	= m_id;
	m_mapPWar.emplace(m_id, pWar);
	return m_id;
}

BOOL CGuildWarMng::RemoveWar(const WarId idWar) {
	const auto it = m_mapPWar.find(idWar);
	if (it == m_mapPWar.end()) return FALSE;

	safe_delete(it->second);
	m_mapPWar.erase(it);

	return TRUE;
}

CGuildWar * CGuildWarMng::GetWar(const WarId idWar) {
	return sqktd::find_in_map(m_mapPWar, idWar, nullptr);
}

CAr & operator<<(CAr & ar, const CGuildWarMng & self) {
	ar << self.m_id;
	ar << static_cast<std::uint32_t>(self.m_mapPWar.size());
	for (const CGuildWar * pWar : self.m_mapPWar | std::views::values) {
		ar << *pWar;
	}

	return ar;
}

CAr & operator>>(CAr & ar, CGuildWarMng & self) {
#ifdef __CLIENT
	self.Clear();
#endif	// __CLIENT
	ar >> self.m_id;
	
	std::uint32_t nSize; ar >> nSize;
	for (std::uint32_t i = 0; i < nSize; ++i) {
		CGuildWar * pWar = new CGuildWar;
		ar >> *pWar;
		self.m_mapPWar.emplace(pWar->m_idWar, pWar);
	}

	return ar;
}

void CGuildWarMng::Result( CGuildWar* pWar, CGuild* pDecl, CGuild* pAcpt, int nType, int nWptDecl, int nWptAcpt )
{
	CGuild* pWin, *pLose;
	switch( nType )
	{
		case WR_DECL_GN:
		case WR_DECL_SR:
		case WR_DECL_AB:
		case WR_DECL_DD:
		case WR_TRUCE:
		case WR_DRAW:
			{
				pWin	= pDecl;
				pLose	= pAcpt;
				break;
			}
		case WR_ACPT_GN:
		case WR_ACPT_SR:
		case WR_ACPT_AB:
		case WR_ACPT_DD:
			{
				pWin	= pAcpt;
				pLose	= pDecl;
				break;
			}
	}

	int nWinPointDecl = 0;
	int nWinPoint = 0;
	
	if( nType < WR_TRUCE )
	{
#ifdef __CORESERVER
		// nWinPoint
		nWinPoint	= (int)( pWin->m_nWinPoint + ( (float)( 10 + pWin->m_nWinPoint ) / (float)( ( (float)( pWin->m_nWinPoint + 4 ) / (float)( pLose->m_nWinPoint + 1 ) ) * 0.05 ) ) + 1 );
		int nMaxWinPoint	= (int)( ( 10 + pWin->m_nWinPoint * 0.05 ) * 5 );
		if( nWinPoint > nMaxWinPoint )
		{
			pWin->m_nWinPoint	+= nMaxWinPoint;
			nWinPointDecl = nMaxWinPoint;
		}
		else
		{
			pWin->m_nWinPoint	+= nWinPoint;
			nWinPointDecl = nWinPoint;
		}
		pWin->m_nWin++;
		nWinPoint	= (int)( ( 10 + pLose->m_nWinPoint * 0.02 ) );
		pLose->m_nWinPoint	-= nWinPoint;
		if( pLose->m_nWinPoint < 0 )
			pLose->m_nWinPoint	= 0;
		pLose->m_nLose++;
#else	// __CORESERVER
		pDecl->m_nWinPoint	= nWptDecl;
		pAcpt->m_nWinPoint	= nWptAcpt;
		pWin->m_nWin++;
		pLose->m_nLose++;
#endif	// __CORESERVER
	}

//	Clean up
	pWin->m_idWar	= WarIdNone;
	pWin->m_idEnemyGuild	= 0;
	pLose->m_idWar	= WarIdNone;
	pLose->m_idEnemyGuild	= 0;

	for( auto i = pWin->m_mapPMember.begin(); i != pWin->m_mapPMember.end(); ++i )
	{
#ifdef __CORESERVER
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
		if( pPlayer )	pPlayer->m_idWar	= WarIdNone;
#endif	// __CORESERVER
#ifdef __WORLDSERVER
		CUser* pUser	= g_UserMng.GetUserByPlayerID( i->second->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			pUser->m_idWar	= WarIdNone;
			g_UserMng.AddSetWar( pUser, WarIdNone);
			pUser->SetPKTargetLimit( 10 );
		}
#endif	// __WORLDSERVER
	}
	for( auto i	= pLose->m_mapPMember.begin(); i != pLose->m_mapPMember.end(); ++i )
	{
#ifdef __CORESERVER
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
		if( pPlayer )	pPlayer->m_idWar	= WarIdNone;
#endif	// __CORESERVER
#ifdef __WORLDSERVER
		CUser* pUser	= g_UserMng.GetUserByPlayerID( i->second->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			pUser->m_idWar	= WarIdNone;
			g_UserMng.AddSetWar( pUser, WarIdNone);
			pUser->SetPKTargetLimit( 10 );
		}
#endif	// __WORLDSERVER
	}
#ifdef __CORESERVER
	g_dpCoreSrvr.SendWarEnd( pWar->m_idWar, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nType );
	g_DPCacheSrvr.SendWarEnd( pWar->m_idWar, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nType );
	int nDBType	= nType;
	switch( nType )
	{
		case WR_DECL_AB:
		case WR_DECL_DD:
			nDBType	= WR_DECL_GN;
			break;
		case WR_ACPT_AB:
		case WR_ACPT_DD:
			nDBType	= WR_ACPT_GN;
			break;
	}
	g_dpDatabaseClient.SendWarEnd( pWar->m_idWar, pDecl->m_idGuild, pAcpt->m_idGuild, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nDBType, pWar->m_time, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nWinPointDecl, nWinPoint );
#endif	// __CORESERVER
#ifdef __CLIENT
	char lpString[1024]	= { 0, };
	switch ( nType )
	{
		case WR_DECL_GN:
		case WR_ACPT_GN:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_MASTER ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_DECL_SR:
		case WR_ACPT_SR:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_GIVEUP ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_DECL_AB:
		case WR_ACPT_AB:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_TIMEUP ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_ACPT_DD:
		case WR_DECL_DD:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_KILLNUM ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_TRUCE:
			{
				char lpString[1024]	= { 0, };
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARPEACE ), pDecl->m_szGuild, pAcpt->m_szGuild );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARPEACE ) );
				break;
			}
		case WR_DRAW:
			{
				char lpString[1024]	= { 0, };
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARDRAW ), pDecl->m_szGuild, pAcpt->m_szGuild );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARDRAW ) );
				break;
			}
	}
#endif	// __CLIENT
	RemoveWar( pWar->m_idWar );
}

#ifdef __WORLDSERVER
void CGuildWarMng::Process( void )
{
	CTime time	= CTime::GetCurrentTime();
	for( auto i = m_mapPWar.begin(); i != m_mapPWar.end(); ++i )
	{
		CGuildWar* pWar	= i->second;
		pWar->Process( time );
	}
}
#endif	// __WORLDSERVER

CGuildWarMng	g_GuildWarMng;
