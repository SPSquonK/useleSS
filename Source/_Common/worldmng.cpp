#include "stdafx.h"
#include "defineObj.h"
#include "WorldMng.h"
#include "World.h"
#ifdef __CLIENT
#include "Sfx.h"
#include "DialogMsg.h"
#ifdef __WINDOW_INTERFACE_BUG
#include "ResData.h"
#endif // __WINDOW_INTERFACE_BUG
#ifdef __MAP_SECURITY
#include "DPClient.h"
#endif // __MAP_SECURITY
#endif // CLIENT

#ifdef __MAP_SECURITY
#include "tools.h"
#endif // __MAP_SECURITY

#ifdef __WORLDSERVER
#include "ServerDesc.h"
#include "DPCoreClient.h"
#include "DPSrvr.h"
#include "User.h"

CWorldMng	g_WorldMng;
#endif	// __WORLDSERVER

CWorldMng::CWorldMng()
{
	m_nSize = 0;
}


CWorldMng::~CWorldMng()
{
	Free();
}

void CWorldMng::Free() {
#ifdef __WORLDSERVER
	m_worlds.clear();
#endif
#ifdef __CLIENT
	m_currentWorld.reset();
#endif
	m_aWorld.RemoveAll();
}


#ifdef __WORLDSERVER

u_long CWorldMng::Respawn() {
	u_long uRespawned = 0;
	
	for (auto & pWorld : m_worlds) {
		uRespawned += pWorld->Respawn();
	}

	return uRespawned;
}

void CWorldMng::ReadObject() {
	for (auto & pWorld : m_worlds) {
	#ifdef __LAYER_1021
		pWorld->LoadObject( nDefaultLayer );
	#else	// __LAYER_1021
		pWorld->LoadObject();
	#endif	// __LAYER_1021
	#ifndef __S1108_BACK_END_SYSTEM
		pWorld->LoadRegion();	// x
	#endif // __S1108_BACK_END_SYSTEM
		pWorld->LoadPatrol();
	}
}

void CWorldMng::Add(const WorldId pJurisdiction)
{
	const WORLD * lpWorld	= GetWorldStruct( pJurisdiction );
	ASSERT( lpWorld );

	CWorld* pWorld = GetWorld( pJurisdiction );
	if( pWorld )
	{
		pWorld->ReadWorld();
	}
	else
	{
		pWorld	= new CWorld;

		pWorld->m_dwWorldID	= pJurisdiction;
		pWorld->OpenWorld( MakePath( DIR_WORLD, lpWorld->m_szFileName ), TRUE );
		pWorld->ReadWorld();

		m_worlds.emplace(m_worlds.begin(), pWorld); // emplace front to mimic the base flyff code
		// TODO: do we really care about the order?
	}
	strcpy( pWorld->m_szWorldName, lpWorld->m_szWorldName );
}

CWorld * CWorldMng::GetWorld(const DWORD dwWorldID) {
	const auto it = std::ranges::find_if(m_worlds,
		[dwWorldID](const auto & pWorld) {
			return pWorld->GetID() == dwWorldID;
		});

	return it != m_worlds.end() ? it->get() : nullptr;
}

#ifdef __LAYER_1015
BOOL CWorldMng::AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId, int nLayer )
#else	// __LAYER_1015
BOOL CWorldMng::AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId )
#endif	// __LAYER_1015
{
	BOOL bResult = FALSE;
	CWorld* pWorld = GetWorld( dwWorldID );
	if( pWorld )
		bResult = pWorld->ADDOBJ( pObj, bAddItToGlobalId, nLayer );
	return bResult;
}

#else	// __WORLDSERVER

CWorld* CWorldMng::Open( LPDIRECT3DDEVICE9 pd3dDevice, OBJID idWorld ) {
	WORLD * lpWorld = GetWorldStruct(idWorld);
	if (!lpWorld) return nullptr;

	CWorld* pWorld	= Open( pd3dDevice, lpWorld->m_szFileName );
	pWorld->m_dwWorldID	= idWorld;
	pWorld->InProcessing( );			//added by gmpbigsun
	strcpy( pWorld->m_szWorldName, lpWorld->m_szWorldName );
	return pWorld;
}

void CWorldMng::DestroyCurrentWorld()
{
	if (!m_currentWorld) return;

	m_currentWorld->OutProcessing( );			//added by gmpbigsun

#ifdef __WINDOW_INTERFACE_BUG
	CWndBase* pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
	if(pWndBase != NULL)
		((CWndInventory*)pWndBase)->BaseMouseCursor();
#endif // __WINDOW_INTERFACE_BUG
	m_currentWorld->InvalidateDeviceObjects();
	m_currentWorld->DeleteDeviceObjects();
	g_pMoveMark = NULL;
	g_DialogMsg.RemoveAll();

	m_currentWorld.reset();
}
CWorld* CWorldMng::Open( LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR lpszWorld ) {
	DestroyCurrentWorld();

	m_currentWorld = std::make_unique<CWorld>();
	m_currentWorld->InitDeviceObjects( pd3dDevice );
	m_currentWorld->OpenWorld( MakePath( DIR_WORLD, lpszWorld ), TRUE );

	return m_currentWorld.get();
}
#endif	// __WORLDSERVER

// common
BOOL CWorldMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName ) == FALSE )
		return FALSE;

	LPWORLD lpWorld;
	
	m_nSize		= 0;
	int nBrace	= 1;
	scanner.SetMark();
	int i	= scanner.GetNumber();	// folder or id
	while( nBrace )
	{
		if( *scanner.token == '}' || scanner.tok == FINISHED )
		{
			nBrace--;
			if( nBrace > 0 )
			{
				scanner.SetMark();
				i	= scanner.GetNumber();	// folder or id
				if( i > m_nSize )	m_nSize	= i;
				continue;
			}
			if( nBrace == 0 )
				continue;
		}
//		i	= _ttoi( scanner.token );
		scanner.GetToken();		// { or filename

		if( scanner.Token == "SetTitle" )
		{
			WORLD* pWorld = m_aWorld.GetAt( i );
			if( pWorld != NULL )
			{
				scanner.GetToken(); // (
				strcpy(	pWorld->m_szWorldName, prj.GetLangScript( scanner ) );				
			}
			else
			{
				Error( "CWorldMng::LoadScript -> Load WorldName" );
			}
		}
		else
		{
			scanner.GoMark();
			i	= scanner.GetNumber(); // id
			if( i > m_nSize )	m_nSize		= i;


			WORLD world;
			::memset( &world, 0, sizeof(world) );

			world.m_dwId = i;
			scanner.GetToken();		// fileName
			strcpy( world.m_szFileName, scanner.token );
			TRACE( "Load World %s %d\n", world.m_szFileName, i );
			m_aWorld.SetAtGrow( i, world );
		}

		scanner.SetMark();
		i	= scanner.GetNumber();	// texture fileName
		if( i > m_nSize )	m_nSize		= i;
	}
	m_aWorld.Optimize();
	m_nSize++;

	for( i = 0; i < m_nSize; i++ )
	{
		lpWorld	= GetWorldStruct( i );
		if( lpWorld && lpWorld->IsValid() )
		{
			LoadRevivalPos( lpWorld->m_dwId, lpWorld->m_szFileName );
		}
	}

	return TRUE;
}

void CWorldMng::LoadRevivalPos( DWORD dwWorldId, LPCTSTR lpszWorld )
{
#ifdef __WORLDSERVER
	TCHAR lpFileName[MAX_PATH];
	sprintf( lpFileName, "%s%s\\%s.rgn", DIR_WORLD, lpszWorld, lpszWorld );
	CScanner s;
	if( s.Load( lpFileName ) == TRUE )
	{
		DWORD dwType, dwIndex;
		D3DXVECTOR3 vPos;

		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "region" ) || s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
			{
				BOOL bNewFormat = FALSE;
				BOOL bNewFormat3 = FALSE;
				if( s.Token == _T( "region2" ) )
					bNewFormat = TRUE;
				if( s.Token == _T( "region3" ) )
				{
					bNewFormat3 = TRUE;
					bNewFormat = TRUE;
				}
				dwType	= s.GetNumber();
				dwIndex	= s.GetNumber();
				vPos.x	= s.GetFloat();
				vPos.y	= s.GetFloat();
				vPos.z	= s.GetFloat();
				
				REGIONELEM rgnElem, *pRgnElem	= &rgnElem;
				memset( &rgnElem, 0, sizeof(rgnElem) );
				
				rgnElem.m_uItemId		= 0xffffffff;
				rgnElem.m_uiItemCount	= 0xffffffff;
				rgnElem.m_uiMinLevel	= 0xffffffff;
				rgnElem.m_uiMaxLevel	= 0xffffffff;
				rgnElem.m_iQuest		= 0xffffffff;
				rgnElem.m_iQuestFlag	= 0xffffffff;
				rgnElem.m_iJob			= 0xffffffff;
				rgnElem.m_iGender		= 0xffffffff;
				rgnElem.m_bCheckParty	= FALSE;
				rgnElem.m_bCheckGuild	= FALSE;
				rgnElem.m_bChaoKey		= FALSE;

				pRgnElem->m_dwWorldId	= dwWorldId;
				pRgnElem->m_dwAttribute	= s.GetNumber();
				pRgnElem->m_dwIdMusic	= s.GetNumber();
				pRgnElem->m_bDirectMusic	= (bool)(s.GetNumber() == 1);
				s.GetToken();	_tcscpy( pRgnElem->m_szScript, s.Token );
				s.GetToken();	_tcscpy( pRgnElem->m_szSound, s.Token );
				pRgnElem->m_dwIdTeleWorld	= s.GetNumber();
				pRgnElem->m_vPos		= vPos;
				pRgnElem->m_vTeleWorld.x		= s.GetFloat();
				pRgnElem->m_vTeleWorld.y		= s.GetFloat();
				pRgnElem->m_vTeleWorld.z		= s.GetFloat();
				pRgnElem->m_rect.left	= (LONG)s.GetNumber();
				pRgnElem->m_rect.top		= (LONG)s.GetNumber();
				pRgnElem->m_rect.right	= (LONG)s.GetNumber();
				pRgnElem->m_rect.bottom	= (LONG)s.GetNumber();
				s.GetToken();	_tcscpy( pRgnElem->m_szKey, s.Token );
				pRgnElem->m_bTargetKey	= (bool)(s.GetNumber() == 1);

				if( bNewFormat3 )
				{
					pRgnElem->m_uItemId		= (u_long)s.GetNumber();
					pRgnElem->m_uiItemCount	= (UINT)s.GetNumber();
					pRgnElem->m_uiMinLevel	= (UINT)s.GetNumber();
					pRgnElem->m_uiMaxLevel	= (UINT)s.GetNumber();
					pRgnElem->m_iQuest		= s.GetNumber();
					pRgnElem->m_iQuestFlag	= s.GetNumber();
					pRgnElem->m_iJob		= s.GetNumber();
					pRgnElem->m_iGender		= s.GetNumber();
					pRgnElem->m_bCheckParty	= (BOOL)s.GetNumber();
					pRgnElem->m_bCheckGuild	= (BOOL)s.GetNumber();
					pRgnElem->m_bChaoKey	= (BOOL)s.GetNumber();
				}

				if( bNewFormat == FALSE )
				{
					pRgnElem->m_cDescSize	= (char)( (LONG)s.GetNumber() );
					if( pRgnElem->m_cDescSize )
					{
						pRgnElem->m_cDescSize = 1;
						CString string;
						s.GetToken(); // {
						int nLang;
						nLang = s.GetNumber();
						do
						{
							s.GetLastFull();
							if( nLang == ::GetLanguage() )
								strcpy( pRgnElem->m_szDesc, s.m_mszToken );
							nLang = s.GetNumber();
						} while( *s.token != '}' );
					}
				}
				else
				{
					s.GetToken(); // title
					if( s.GetNumber() )
					{
						CString string;
						s.GetToken(); // {
						int nLang;
						nLang = s.GetNumber();
						do
						{
							s.GetLastFull();
							if( nLang == ::GetLanguage() )
								strcpy( pRgnElem->m_szDesc, s.m_mszToken );
							nLang = s.GetNumber();
						} while( *s.token != '}' );
					}
					s.GetToken(); // desc
					if( s.GetNumber() )
					{
						CString string;
						s.GetToken(); // {
						int nLang;
						nLang = s.GetNumber();
						do
						{
							s.GetLastFull();
							if( nLang == ::GetLanguage() )
								strcpy( pRgnElem->m_szDesc, s.m_mszToken );
							nLang = s.GetNumber();
						} while( *s.token != '}' );
					}
				}

				if( dwIndex == RI_REVIVAL || dwIndex == RI_PLACE )
				{
					pRgnElem->m_dwIndex		= dwIndex;

					if( FALSE == pRgnElem->m_bTargetKey )
					{
						m_aRevivalPos.Add( *pRgnElem );
					}
					else
					{
						ASSERT( strlen( pRgnElem->m_szKey ) );
						m_aRevivalRgn.Add( *pRgnElem );
					}
				}
			}

			else if( s.Token == _T( "respawn" ) )
			{
				// type, index, x, y, z, cb, time, left, top, right, bottom
				s.GetNumber();	s.GetNumber();	s.GetFloat();	s.GetFloat();	s.GetFloat();	s.GetNumber();	s.GetNumber();
				s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();
			}
			else if( s.Token == _T( "region3" ) )
			{
				
			}

			s.GetToken();
		}
	}
#endif	// __WORLDSERVER
}
void CWorldMng::LoadAllMoverDialog()
{
#ifdef __WORLDSERVER
	for (auto & pWorld : m_worlds) {
		pWorld->LoadAllMoverDialog();
	}
#endif //__WORLDSERVER
}


#ifdef __WORLDSERVER

const REGIONELEM * CWorldMng::GetRevivalPosChao( DWORD dwWorldId, LPCTSTR sKey ) const
{
	return m_aRevivalPos.FindAny([&](const REGIONELEM & pRgnElem) {
		return dwWorldId == pRgnElem.m_dwWorldId
			&& strcmp(sKey, pRgnElem.m_szKey) == 0
			&& pRgnElem.m_bChaoKey;
		});
}

const REGIONELEM * CWorldMng::GetNearRevivalPosChao( DWORD dwWorldId, const D3DXVECTOR3 & vPos ) const
{
	const REGIONELEM * ptr = m_aRevivalPos.FindClosest(vPos, [&](const REGIONELEM & pRgnElem) {
		return dwWorldId == pRgnElem.m_dwWorldId && pRgnElem.m_bChaoKey;
		});

	if (ptr) return ptr;

	// 같은 서버에 찾지를 못했을 경우 다른서버검색
	return m_aRevivalPos.FindAny([&](const REGIONELEM & pRgnElem) {
		// 거리계산은 필요없음
		return dwWorldId != pRgnElem.m_dwWorldId && pRgnElem.m_bChaoKey;
		});
}

const REGIONELEM * CWorldMng::GetRevivalPos(DWORD dwWorldId, LPCTSTR sKey) const {
	return m_aRevivalPos.FindAny([&](const REGIONELEM & pRgnElem) {
		return dwWorldId == pRgnElem.m_dwWorldId
			&& strcmp(sKey, pRgnElem.m_szKey) == 0
			&& pRgnElem.m_bChaoKey == FALSE;
	});
}

const REGIONELEM * CWorldMng::GetNearRevivalPos( DWORD dwWorldId, const D3DXVECTOR3 & vPos ) const
{
	const CPoint point(static_cast<long>(vPos.x), static_cast<long>(vPos.z));
	const REGIONELEM * revivalRegion = m_aRevivalRgn.FindAny(
		[&](const REGIONELEM & pRgnElem) {
			return dwWorldId == pRgnElem.m_dwWorldId
				&& pRgnElem.m_dwIndex == RI_REVIVAL
				&& pRgnElem.m_bChaoKey == FALSE
				&& pRgnElem.m_rect.PtInRect(point);
		}
	);

	if (revivalRegion) {
		return GetRevivalPos(dwWorldId, revivalRegion->m_szKey);
	}

	return m_aRevivalPos.FindClosest(vPos, [&](const REGIONELEM & pRgnElem) {
		return dwWorldId == pRgnElem.m_dwWorldId && pRgnElem.m_bChaoKey == FALSE;
		});

	// tfw the code does not try to send to any respawn point if none in this world :|
}

const REGIONELEM * CWorldMng::GetRevival(CMover * pUser) {
	const CWorld * pWorld = pUser->GetWorld();
	if (!pWorld) return nullptr;
	return GetRevival(*pWorld, pUser->GetPos(), pUser->IsChaotic());
}

const REGIONELEM * CWorldMng::GetRevival(const CWorld & world, const D3DXVECTOR3 & vPos, const bool isChaotic) {
	const REGIONELEM * retval = nullptr;

	const bool differentWorldRevival =
		world.GetID() != world.m_dwIdWorldRevival
		&& world.m_dwIdWorldRevival != 0;

	if (isChaotic) {
		retval = GetRevivalPosChao(world.m_dwIdWorldRevival, world.m_szKeyRevival);
	} else {
		retval = GetRevivalPos(world.m_dwIdWorldRevival, world.m_szKeyRevival);
	}

	if (!retval) {
		if (isChaotic) {
			retval = GetNearRevivalPosChao(world.GetID(), vPos);
		} else {
			retval = GetNearRevivalPos(world.GetID(), vPos);
		}
	}

	return retval;
}
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
void CWorldMng::Process() {
	for (auto & pWorld : m_worlds) {
		pWorld->Process();
	}
}

bool CWorldMng::HasSomeoneGoingTo(const DWORD dwWorldId, const int nLayer) const {
	const auto IsGoingTo = [&](const REPLACEOBJ & replaceObj) {
		return replaceObj.dwWorldID == dwWorldId && replaceObj.nLayer == nLayer;
	};

	return std::ranges::any_of(m_worlds,
		[&](const auto & pWorld) {
			return std::ranges::any_of(pWorld->m_ReplaceObj, IsGoingTo);
		}
	);
}

// 모든 월드의 오브젝트 카운트를 얻는다.
DWORD CWorldMng::GetObjCount() const noexcept {
	DWORD dwCount = 0;

	for (const auto & pWorld : m_worlds) {
		dwCount += pWorld->GetRunnableObjectCount();
	}

	return dwCount;
}

#endif	// __WORLDSERVER

#ifdef __MAP_SECURITY
BOOL CWorldMng::SetMapCheck( const char* szFileName )
{
	CResFile file;
	if( file.Open( szFileName, "rb" ) )
	{
		size_t nSize = file.GetLength() / sizeof( DWORD );
		DWORD dwTotal = 0;
		DWORD* dwData = new DWORD[nSize];
		file.Read( dwData, sizeof( DWORD ), nSize );
		file.Close();
		for( size_t i=0; i<nSize; i++ )
			dwTotal += dwData[i];
		delete [] dwData;

		char szFileNameOut[33] = {0,};
		md5( szFileNameOut, szFileName );

		char szIn[16] = {0,}, szOut[33] = {0,};
		sprintf( szIn, "%u", dwTotal );
		md5( szOut, szIn );

#ifdef __WORLDSERVER
		m_mapMapKey.emplace( szFileNameOut, szOut );
#endif // __WORLDSERVER
#ifdef __CLIENT
		g_DPlay.SendMapKey( szFileNameOut, szOut );
#endif // __CLIENT

		return TRUE;
	}

	return FALSE;
}

void CWorldMng::AddMapCheckInfo( const char* szFileName )
{
#ifndef __INTERNALSERVER
#ifdef __WORLDSERVER
	SetMapCheck( szFileName );
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_vecstrFileName.push_back( szFileName );
#endif // __CLIENT
#endif // __INTERNALSERVER
}

#ifdef __CLIENT
void CWorldMng::CheckMapKey()
{
	if( m_vecstrFileName.size() > 0 )
	{
		SetMapCheck( m_vecstrFileName.back().c_str() );
		m_vecstrFileName.pop_back();
	}
}
#endif // __CLIENT
#ifdef __WORLDSERVER
void CWorldMng::CheckMapKey( CUser* pUser, const char* szFileName, const char* szMapKey )
{	
	const auto it = m_mapMapKey.find( szFileName );
	if( it != m_mapMapKey.end() )
	{
		if( !strcmpi( szMapKey, it->second.c_str() ) )	// 맵 인증 성공!
			return;
	}

	
	// 맵 인증 실패 접속을 끊어버려야 한다.
	int x = int( pUser->GetPos().x ) / ( MAP_SIZE * pUser->GetWorld()->m_iMPU );
	int z = int( pUser->GetPos().z ) / ( MAP_SIZE * pUser->GetWorld()->m_iMPU );
	char szLandFile[MAX_PATH] = {0,};
	if( pUser->GetWorld() )
	{
		strncpy( szLandFile, pUser->GetWorld()->m_szFileName, strlen( pUser->GetWorld()->m_szFileName ) - 4 );
	}

	Error( "Invalid Map Key - [User : %s, %07d], [Land : %s%02d-%02d]", pUser->GetName(), pUser->m_idPlayer, szLandFile, x, z );

	//	mulcom	BEGIN100330	맵변조 확인 독일은 제외 시킴
	if( ::GetLanguage() != LANG_GER )
	{
		g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
	}
	//	mulcom	BEGIN100330	맵변조 확인 독일은 제외 시킴
}
#endif // __WORLDSERVER
#endif // __MAP_SECURITY