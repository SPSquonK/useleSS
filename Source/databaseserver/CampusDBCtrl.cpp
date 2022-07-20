// CampusDBCtrl.cpp: implementation of the CCampusDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CampusDBCtrl.h"

#include "dptrans.h"
#include "DPCoreSrvr.h"

//////////////////////////////////////////////////////////////////////
// CCampusDBCtrl Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CCampusDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );
	
	switch( pov->nQueryMode )
	{
	case CAMPUS_LOAD:
		CreateLogQuery();
		LoadAllCampus();
		break;

	case CAMPUS_SEND:
		SendAllCampus( dwCompletionKey );
		break;

	case CAMPUS_ADD_MEMBER:
		AddCampusMember( ar );
		break;

	case CAMPUS_REMOVE_MEMBER:
		RemoveCampusMember( ar );
		break;

	case CAMPUS_UPDATE_POINT:
		UpdateCampusPoint( ar );
		break;
	}
}

void CCampusDBCtrl::CreateLogQuery()
{
	m_pLogQuery = std::make_unique<CQuery>();

	const char* pass	= CDbManager::GetInstance().DB_ADMIN_PASS_LOG;
	if( FALSE == m_pLogQuery->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, pass ) )
	{
		::AfxMessageBox( "Can't connect db: CCampusDBCtrl.CreateLogQuery" );
		m_pLogQuery.reset();
		ASSERT( 0 );
	}
}

void CCampusDBCtrl::LoadAllCampus()
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_Campus_Load @serverindex = '%02d'", g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
	
	while( pQuery->Fetch() )
	{
		CCampus* pCampus = new CCampus;
		pCampus->SetCampusId( pQuery->GetInt( "idCampus" ) );
		m_campusMng.AddCampus( pCampus );
	}
	
	sprintf( szQuery, "usp_CampusMember_Load @serverindex = '%02d'", g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{
		m_campusMng.Clear();
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}
	
	while( pQuery->Fetch() )
	{
		const u_long idPlayer = static_cast<u_long>(pQuery->GetInt( "m_idPlayer" ));
		const CampusRole nMemberLv = static_cast<CampusRole>(pQuery->GetInt( "nMemberLv" ));
		const u_long idCampus = pQuery->GetInt( "idCampus" );
		CCampus* pCampus = m_campusMng.GetCampus( idCampus );
		if( pCampus )
		{
			pCampus->AddMember(idPlayer, nMemberLv);
			if( nMemberLv == CampusRole::Master)
				pCampus->SetMaster( idPlayer );
			m_campusMng.AddPlayerId2CampusId( idPlayer, idCampus );
		}
		else
		{
			WriteLog( "LoadAllCampus(): Player's campus not found - %d, %d", idPlayer, idCampus );
		}
	}
}

void CCampusDBCtrl::AddCampusMember( CAr & ar )
{
	u_long idMaster, idPupil;
	int nMasterPoint, nPupilPoint;
	ar >> idMaster >> nMasterPoint >> idPupil >> nPupilPoint;

	if( nMasterPoint < 0 || nPupilPoint < 0
		|| m_campusMng.GetCampus(m_campusMng.GetCampusIdByPlayerId( idPupil ) ) )
		return;

	CCampus* pCampus = m_campusMng.GetCampus(m_campusMng.GetCampusIdByPlayerId( idMaster ) );
	if( pCampus )
	{
		if( pCampus->GetPupilNum() >= CCampus::GetMaxPupilNum(nMasterPoint) )
			return;

		if( pCampus->AddMember(idPupil, CampusRole::Pupil)  )
		{
			if(m_campusMng.AddPlayerId2CampusId( idPupil, pCampus->GetCampusId() ) )
			{
				UpdateCampusId( idPupil, pCampus->GetCampusId() );
				InsertCampusMember( pCampus->GetCampusId(), idPupil, CampusRole::Pupil);
			}
			else
			{
				Error( "AddPlayerId2CampusId() fail" );
				pCampus->RemoveMember( idPupil);
				return;
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		pCampus = new CCampus;
		pCampus->SetMaster( idMaster );
		u_long idCampus = m_campusMng.AddCampus( pCampus );
		if( idCampus > 0 )
		{
			if( pCampus->AddMember(idMaster, CampusRole::Master) && pCampus->AddMember(idPupil, CampusRole::Pupil)
				&& m_campusMng.AddPlayerId2CampusId(idMaster, idCampus)
				&& m_campusMng.AddPlayerId2CampusId( idPupil, idCampus ) )
			{
				UpdateCampusId( idMaster, idCampus );
				UpdateCampusId( idPupil, idCampus );
				InsertCampus( idCampus );
				InsertCampusMember( idCampus, idMaster, CampusRole::Master );
				InsertCampusMember( idCampus, idPupil, CampusRole::Pupil);
			}
			else
			{
				Error( "AddMember failed!" );
				m_campusMng.RemoveCampus( idCampus );
				return;
			}
		}
		else
		{
			Error( "new campus failed! campus exist" );
			SAFE_DELETE( pCampus );
			return;
		}
	}
	LogUpdateCampusMember( pCampus->GetCampusId(), idMaster, idPupil, 'T' );
	CDPTrans::GetInstance()->SendAddCampusMember( pCampus->GetCampusId(), idMaster, idPupil );
}

void CCampusDBCtrl::RemoveCampusMember( CAr & ar )
{
	u_long idCampus, idPlayer;
	ar >> idCampus >> idPlayer;

	CCampus* pCampus = m_campusMng.GetCampus( idCampus );
	if( pCampus && pCampus->IsMember( idPlayer ) )
	{
		if( pCampus->IsMaster( idPlayer ) )
		{
			const auto & vecMember = pCampus->GetAllMemberPlayerId();
			for( auto it = vecMember.begin(); it != vecMember.end(); ++it )
			{
				m_campusMng.RemovePlayerId2CampusId( *it );
				UpdateCampusId( *it, 0 );
				if( *it != idPlayer )
				{
					DeleteCampusMember( *it, CampusRole::Pupil);
					LogUpdateCampusMember( idCampus, idPlayer, *it, 'F' );
				}
			}
			m_campusMng.RemoveCampus( idCampus );
			DeleteCampus( idCampus );
		}
		else
		{
			m_campusMng.RemovePlayerId2CampusId( idPlayer );
			pCampus->RemoveMember( idPlayer );
			UpdateCampusId( idPlayer, 0 );
			DeleteCampusMember( idPlayer, CampusRole::Pupil);
			LogUpdateCampusMember( idCampus, pCampus->GetMaster(), idPlayer, 'F' );
			if( pCampus->GetMemberSize() < 2 )
			{
				m_campusMng.RemovePlayerId2CampusId( pCampus->GetMaster() );
				UpdateCampusId( pCampus->GetMaster(), 0 );
				m_campusMng.RemoveCampus( idCampus );
				DeleteCampus( idCampus );
			}
		}
		CDPTrans::GetInstance()->SendRemoveCampusMember( idCampus, idPlayer );
	}
	else
		Error( "RemoveCampusMember fail" );
}

void CCampusDBCtrl::UpdateCampusPoint( CAr & ar )
{
	u_long idPlayer;
	int nCampusPoint;
	BOOL bAdd;
	char chState;
	ar >> idPlayer >> nCampusPoint >> bAdd >> chState;
	
	int nPrevPoint, nCurrPoint;
	nPrevPoint = nCurrPoint = 0;
	if( bAdd )
	{
		nCurrPoint = UpdateCampusPoint( idPlayer, nCampusPoint );
		nPrevPoint = nCurrPoint - nCampusPoint;
	}
	else
	{
		nCurrPoint = UpdateCampusPoint( idPlayer, -(nCampusPoint) );
		nPrevPoint = nCurrPoint + nCampusPoint;
	}
	LogUpdateCampusPoint( idPlayer, nPrevPoint, nCurrPoint, chState );
	CDPTrans::GetInstance()->SendUpdateCampusPoint( idPlayer, nCurrPoint );
}

void CCampusDBCtrl::SendAllCampus( DPID dpId )
{
	if( !m_campusMng.IsEmpty() )
		CDPTrans::GetInstance()->SendAllCampus( dpId );
}

void CCampusDBCtrl::InsertCampus( u_long idCampus )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_Campus_Insert @idCampus = %d, @serverindex = '%02d'", idCampus, g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::DeleteCampus( u_long idCampus )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_Campus_Delete @idCampus = %d, @serverindex = '%02d'", idCampus, g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::InsertCampusMember( u_long idCampus, u_long idPlayer, CampusRole nMemberLv )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_CampusMember_Insert @idCampus = %d, @serverindex = '%02d', @m_idPlayer = '%07d', @nMemberLv = %d", 
		idCampus, g_appInfo.dwSys, idPlayer, static_cast<int>(nMemberLv) );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::DeleteCampusMember( u_long idPlayer, CampusRole nMemberLv )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_CampusMember_Delete  @serverindex = '%02d', @m_idPlayer = '%07d', @nMemberLv = %d", 
		g_appInfo.dwSys, idPlayer, static_cast<int>(nMemberLv) );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

int CCampusDBCtrl::UpdateCampusPoint( u_long idPlayer, int nCampusPoint )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	g_DbManager.DBQryCharacter( szQuery, "U5", idPlayer, g_appInfo.dwSys, "", "", nCampusPoint );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return INT_MAX; }

	int nTotalPoint = INT_MAX;
	if( pQuery->Fetch() )
		nTotalPoint = pQuery->GetInt( "m_nCampusPoint" );
	
	return nTotalPoint;
}

void CCampusDBCtrl::UpdateCampusId( u_long idPlayer, u_long idCampus )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	g_DbManager.DBQryCharacter( szQuery, "U6", idPlayer, g_appInfo.dwSys, "", "", idCampus );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::LogUpdateCampusMember( u_long idCampus, u_long idMaster, u_long idPupil, char chState )
{
	char szQuery[QUERY_SIZE] = {0, };

	sprintf( szQuery, "usp_CampusLog_Insert  @m_idMaster = '%07d', @serverindex = '%02d', @idCampus = %d, @m_idPupil = '%07d', @chState = '%c'", 
		idMaster, g_appInfo.dwSys, idCampus, idPupil, chState );
	if( m_pLogQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::LogUpdateCampusPoint( u_long idPlayer, int nPrevPoint, int nCurrPoint, char chState )
{
	char szQuery[QUERY_SIZE] = {0, };

	sprintf( szQuery, "usp_CampusPointLog_Insert  @m_idPlayer = '%07d', @serverindex = '%02d', @chState = '%c', @nPrevPoint = %0d, @nCurrPoint = %d", 
		idPlayer, g_appInfo.dwSys, chState, nPrevPoint, nCurrPoint );
	if( m_pLogQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

//////////////////////////////////////////////////////////////////////
// CCampusHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusHelper::CCampusHelper()
	: m_CampusDBCtrl(m_pCampusMng)
{
	if( !m_CampusDBCtrl.CreateDbHandler() )
		Error( "CCampusHelper - m_CampusDBCtrl.CreateDbHandler()" );
}

CCampusHelper::~CCampusHelper()
{
	m_CampusDBCtrl.CloseDbHandler();
}

CCampusHelper* CCampusHelper::GetInstance()
{
	static CCampusHelper sCH;
	return & sCH;
}

void CCampusHelper::RemovePlayerFromCampus(u_long idPlayer) {
	CCampus * pCampus = m_pCampusMng.GetCampus(m_pCampusMng.GetCampusIdByPlayerId(idPlayer));
	if (pCampus && pCampus->IsMember(idPlayer)) {
		CAr ar;
		ar << pCampus->GetCampusId() << idPlayer;
		int nBufSize;
		LPBYTE lpBuf = ar.GetBuffer(&nBufSize);
		PostRequest(CAMPUS_REMOVE_MEMBER, lpBuf, nBufSize);
	}
}
