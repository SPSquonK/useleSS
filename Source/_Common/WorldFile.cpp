#include "stdafx.h"  
#include "defineObj.h"
#include "region.h"
#include "WorldMng.h"
#include "lang.h"
#include "respawn.h"
#include "PathObj.h"

#ifdef __WORLDSERVER
#include "user.h"
#endif	// __WORLDSERVER

#ifdef	__YENV	
extern float s_fFogStart;
extern float s_fFogEnd;
#endif //__YENV
#include "InstanceDungeonBase.h"


BOOL CWorld::OpenWorld( OBJID idWorld, BOOL bDir ) {
	const WORLD * lpWorld = g_WorldMng.GetWorldStruct(idWorld);
	ASSERT( lpWorld->IsValid() );
	return OpenWorld(lpWorld->m_szFileName, bDir);
}

const char* DEFAULT_FNAME = "default";

BOOL CWorld::OpenWorld( LPCTSTR lpszFileName, BOOL bDir )
{
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	TCHAR szPathName[ _MAX_PATH ];

	if( bDir == FALSE )
	{
		strcpy( szPathName, lpszFileName );
	}
	else
	{
		_splitpath( lpszFileName, drive, dir, name, ext );
		strcpy( szPathName, lpszFileName );
		strcat( szPathName, "\\" );
		strcat( szPathName, name );
		strcat( szPathName, ".wld" );
	}

	CScript script;	
	if( script.Load( szPathName ) == FALSE )
		return FALSE;

	bool bLoaded_MPU = false;

	do 
	{
		script.GetToken();
		if( script.Token == _T( "size" ) )
		{
			m_nLandWidth = script.GetNumber();
			script.GetToken();
			m_nLandHeight = script.GetNumber();
			CalcBound();
		}
		else
		if( script.Token == _T( "ambient" ) )
		{
		#ifndef __WORLDSERVER
			m_dwAmbient = script.GetHex();
		#endif	// __WORLDSERVER
		}
		else
		if( script.Token == _T( "revival" ) )
		{
			m_dwIdWorldRevival = script.GetNumber();
			script.GetToken();
			strcpy( m_szKeyRevival, script.token );
		}
		else
		if( script.Token == _T( "indoor" ) )
		{
			m_bIsIndoor = script.GetNumber();
		#if defined( __CLIENT ) 
			m_bViewSkybox = m_bViewWeather = !m_bIsIndoor;
		#endif //__CLIENT
		}
		else
		if( script.Token == _T( "fly" ) )
		{
			m_bFly = script.GetNumber();
		}
		else
		if( script.Token == _T( "bgm" ) )
		{
			m_dwIdMusic = script.GetNumber();
		}
		else
		if( script.Token == _T( "pkmode" ) )
		{
			m_nPKMode = script.GetNumber();

		}
		else
		if( script.Token == _T( "diffuse" ) )
		{
			m_dwDiffuse = script.GetHex();
		}
		else
		if( script.Token == _T( "lightDir" ) )
		{
			m_v3LightDir.x = script.GetFloat();
			m_v3LightDir.y = script.GetFloat();
			m_v3LightDir.z = script.GetFloat();
		}
		else
		if( script.Token == _T( "fogSetting" ) )
		{
		#if defined( __CLIENT )
			m_fFogStartValue = script.GetFloat();
			m_fFogEndValue   = script.GetFloat();
			m_fFogDensity    = script.GetFloat();
			
			#ifdef __YENV			
			s_fFogStart      = m_fFogStartValue;	// 셰이더 넘기기용
			s_fFogEnd        = m_fFogEndValue;		// 셰이더 넘기기용
			#endif //__YENV
		#endif //__CLIENT
		}			
		else 
		if( script.Token == __T("MPU") )
		{
			bLoaded_MPU = true;
			g_MPU = script.GetNumber( );
		}
		

		else
		if( script.Token == __T( "sky" ) )
		{
			for( int i=0; i<3; ++i)
			{
				script.GetToken();
				if( DEFAULT_FNAME != script.Token )
				{
					m_kWorldEnvir._kSky[i]._bUse = TRUE;
					m_kWorldEnvir._kSky[i]._strFileName = script.token;
				}
			}
			
		}
		else
		if( script.Token == __T( "cloud" ) )
		{
			for( int i=0; i<3; ++i)
			{
				script.GetToken();
				if( DEFAULT_FNAME != script.Token )
				{
					m_kWorldEnvir._kCloud[i]._bUse = TRUE;
					m_kWorldEnvir._kCloud[i]._strFileName = script.token;
				}
			}
		}
		else
		if( script.Token  == __T( "sun" ) )
		{
			script.GetToken();
			if( DEFAULT_FNAME != script.Token )
			{
				m_kWorldEnvir._kSun._bUse = TRUE;
				m_kWorldEnvir._kSun._strFileName = script.token;
			}
		}
		else
		if( script.Token == __T( "moon" ) )
		{
			script.GetToken();
			if( DEFAULT_FNAME != script.Token )
			{
				m_kWorldEnvir._kSun._bUse = TRUE;
				m_kWorldEnvir._kSun._strFileName = script.token;
			}
		}
		else
		if( script.Token == __T( "use24light" ) )
		{
			m_bUsing24Light = script.GetNumber( );
		}
#ifdef __BS_CHANGEABLE_WORLD_SEACLOUD
		else
		if( script.Token == __T( "seacloud" ) )
		{
			script.GetToken( );
			if( DEFAULT_FNAME != script.Token )
			{
				m_kSeaCloud._bUse = TRUE;
				m_kSeaCloud._strFileName = script.token;
			}
		}
#endif //__BS_CHANGEABLE_WORLD_SEACLOUD

	} while( script.tok != FINISHED );

	if( !bLoaded_MPU )
		g_MPU = OLD_MPU;

#ifdef __CLIENT
	m_nVisibilityLand	= (int)( m_fFarPlane / ( MAP_SIZE * MPU ) );
#endif

	m_iMPU = g_MPU;						// __DYNAMIC_MPU가 설정된 경우 ".wld"파일에서 읽은 MPU값을 사용한다.!!!!!!!!!!!!!!!!!!!!!!!!

	// 파일명 보관 
	_splitpath( szPathName, drive, dir, name, ext );
	strcpy( m_szFilePath, dir );
	strcpy( m_szFileName, name );
	strcat( m_szFileName, ext );

#ifdef __WORLDSERVER
	_tcscpy( m_lpszWorld, m_szFileName );

	int nView = 1;								// 시야 1 = 64미터 
	if( m_dwWorldID == WI_WORLD_GUILDWAR )	// 길드대전 맵은 시야 2배 
		nView = 2;

	nView = nView + ( OLD_MPU - m_iMPU );

	m_linkMap.Init( m_nLandWidth, m_nLandHeight, nView, TRUE, m_iMPU );
	m_apHeightMap = new float[ WORLD_WIDTH * WORLD_HEIGHT ];
	memset( m_apHeightMap, 0, sizeof( float ) * WORLD_WIDTH * WORLD_HEIGHT );
	m_apWaterHeight = new WATERHEIGHT[ NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight ];
	memset( m_apWaterHeight, 0, sizeof( WATERHEIGHT) * NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight );
#else	// __WORLDSERVER
	m_apLand = new CLandscape * [ m_nLandWidth * m_nLandHeight ];
	memset( m_apLand, 0, sizeof( CLandscape*) * m_nLandWidth * m_nLandHeight );
	LoadRegion();

	InitWorldEnvir( szPathName );
	InitContinent( szPathName );

	

#endif	// __WORLDSERVER

	return TRUE;
}

#ifdef __WORLDSERVER


BOOL CWorld::LoadPatrol()
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.pat", m_szFilePath, strLandName );
	return CPatrolPath::GetInstance()->LoadPatrol( strLandTemp );
}

#ifdef __LAYER_1021
void CWorld::DeleteLayerControls( int nLayer )
{
	for (CObj * pObj : m_Objs.ValidObjs()) {
		if( pObj->GetType() == OT_CTRL && pObj->GetLayer() == nLayer )
			pObj->Delete();
	}
}

#endif	// __LAYER_1021

// 동적 오브젝트 스크립트 로드
#ifdef __LAYER_1021
BOOL CWorld::LoadObject( int nLayer )
#else	// __LAYER_1021
BOOL CWorld::LoadObject()
#endif	// __LAYER_1021
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.dyo", m_szFilePath, strLandName );
	CResFile file;

	if( file.Open( strLandTemp,"rb" ) == FALSE )
		return FALSE;

	while( 1 )
	{
		CObj* pObj = ::ReadObj( &file );
		if( pObj == NULL )
			break;

		D3DXVECTOR3 vPos = pObj->GetPos();
		pObj->SetPos( vPos );
		pObj->SetIndex( NULL, pObj->m_dwIndex );
		if( pObj->GetType() == OT_MOVER )
			pObj->InitMotion( ((CMover *)pObj)->m_dwMotion );
		pObj->UpdateLocalMatrix();
		
		if( IsUsableDYO( pObj ) )
		{
			ADDOBJ( pObj, TRUE, nLayer );
			if( pObj->GetType() == OT_MOVER && lstrlen( static_cast<CMover*>(pObj)->GetCharacter()->m_szKey ) )
			{
				LPCHARACTER lpChar = static_cast<CMover*>(pObj)->GetCharacter();
				lpChar->m_dwWorldId = GetID();
				lpChar->m_vPos = pObj->GetPos();
			}
		}
		else
		{
			safe_delete( pObj );
			continue;
		}
	} 
	
	// __XSHIPBUS
#ifdef __IAOBJ0622	
	if( GetID() == WI_WORLD_MADRIGAL )	// 마드리갈 읽을때 테스트비공정띄우기
	{
		CObj *pObj	= CreateObj( D3DDEVICE, OT_SHIP, 4 );
		D3DXVECTOR3 vPos = D3DXVECTOR3( 6997.2f, 124.2f, 3243.7f );
		pObj->SetPos( vPos );
		pObj->SetAngle( 180.0f );
		ADDOBJ( pObj, TRUE, nLayer );

		CObj *pObj2	= CreateObj( D3DDEVICE, OT_SHIP, 4 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( 7508.74f, 90.2f, 3168.28f );
		pObj2->SetPos( vPos2 );
		pObj2->SetAngle( 180.0f );
		ADDOBJ( pObj2, TRUE, nLayer );

		CObj *pObj3	= CreateObj( D3DDEVICE, OT_SHIP, 1 );
		D3DXVECTOR3 vPos3 = D3DXVECTOR3( 7528.74f, 90.2f, 3178.28f );
		pObj3->SetPos( vPos3 );
		pObj3->SetAngle( 180.0f );
		ADDOBJ( pObj3, TRUE, nLayer );

		CObj *pObj4	= CreateObj( D3DDEVICE, OT_SHIP, 2 );
		D3DXVECTOR3 vPos4 = D3DXVECTOR3( 7558.74f, 90.2f, 3188.28f );
		pObj4->SetPos( vPos4 );
		pObj4->SetAngle( 180.0f );
		ADDOBJ( pObj4, TRUE, nLayer );

		CObj *pObj5	= CreateObj( D3DDEVICE, OT_SHIP, 3 );
		D3DXVECTOR3 vPos5 = D3DXVECTOR3( 7598.74f, 90.2f, 3198.28f );
		pObj5->SetPos( vPos5 );
		pObj5->SetAngle( 180.0f );
		ADDOBJ( pObj5, TRUE, nLayer );

	}
#endif // IAOBJ0622

	return TRUE;
}
#endif // __WORLDSERVER

// 저장 포맷을 실제 스트링으로 변환 
void RegionDesc_SaveFormatToString( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\\' && *(pwszSrc + 1 )== 'n')
		{
			*(pwszSrc + 0) = '\r';
			*(pwszSrc + 1) = '\n';
		}
		pwszSrc++;
	}
}
// 실제 스트링을 저장 포맷으로 변경 
void RegionDesc_StringToSaveFormat( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\r' && *(pwszSrc + 1 )== '\n')
		{
			*(pwszSrc + 0) = '\\';
			*(pwszSrc + 1) = 'n';
		}
		pwszSrc++;
	}	
}

BOOL CWorld::ReadRegion( CScript& s )
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
	DWORD dwType = s.GetNumber();
	DWORD dwIndex = s.GetNumber();
	D3DXVECTOR3 vPos;
	vPos.x = s.GetFloat();
	vPos.y = s.GetFloat();
	vPos.z = s.GetFloat();

	REGIONELEM re,* pRe = &re;
	ZeroMemory( &re, sizeof( re ) );

	pRe->m_uItemId		= 0xffffffff;
	pRe->m_uiItemCount	= 0xffffffff;
	pRe->m_uiMinLevel	= 0xffffffff;
	pRe->m_uiMaxLevel	= 0xffffffff;
	pRe->m_iQuest		= 0xffffffff;
	pRe->m_iQuestFlag	= 0xffffffff;
	pRe->m_iJob			= 0xffffffff;
	pRe->m_iGender		= 0xffffffff;
	pRe->m_bCheckParty	= FALSE;
	pRe->m_bCheckGuild	= FALSE;
	pRe->m_bChaoKey		= FALSE;
	pRe->m_dwAttribute	= s.GetNumber();
	pRe->m_dwIdMusic = s.GetNumber();
	pRe->m_bDirectMusic = (bool)(s.GetNumber() == 1);

	s.GetToken(); _tcscpy( pRe->m_szScript, s.token );
	s.GetToken(); _tcscpy( pRe->m_szSound, s.token );
	pRe->m_dwIdTeleWorld = s.GetNumber();

	pRe->m_vPos        = vPos;
	pRe->m_vTeleWorld.x = s.GetFloat();
	pRe->m_vTeleWorld.y = s.GetFloat();
	pRe->m_vTeleWorld.z = s.GetFloat();
	pRe->m_rect.left   = (LONG)s.GetNumber();
	pRe->m_rect.top	   = (LONG)s.GetNumber();
	pRe->m_rect.right  = (LONG)s.GetNumber();
	pRe->m_rect.bottom = (LONG)s.GetNumber();
	s.GetToken();
	strcpy( pRe->m_szKey, s.Token );
	pRe->m_bTargetKey  =  (bool)(s.GetNumber() == 1);
	if( bNewFormat3 )
	{
		pRe->m_uItemId		= (u_long)s.GetNumber();
		pRe->m_uiItemCount	= (UINT)s.GetNumber();
		pRe->m_uiMinLevel	= (UINT)s.GetNumber();
		pRe->m_uiMaxLevel	= (UINT)s.GetNumber();
		pRe->m_iQuest		= s.GetNumber();
		pRe->m_iQuestFlag	= s.GetNumber();
		pRe->m_iJob			= s.GetNumber();
		pRe->m_iGender		= s.GetNumber();
		pRe->m_bCheckParty	= (BOOL)s.GetNumber();
		pRe->m_bCheckGuild	= (BOOL)s.GetNumber();
		pRe->m_bChaoKey		= (BOOL)s.GetNumber();
	}

	if( bNewFormat == FALSE )
	{
		pRe->m_cDescSize = (char)( (LONG)s.GetNumber() );
		if( pRe->m_cDescSize )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			strcpy( pRe->m_szDesc, s.Token );	

			s.GetToken();	// }

		}
	}
	else
	{
		s.GetToken(); // get "title"
		BOOL bDesc = s.GetNumber();
		if( bDesc )
		{
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			strcpy( pRe->m_szTitle, s.Token );

			s.GetToken();	// }
		}

		s.GetToken(); // get "desc"
		bDesc = s.GetNumber();
		if( bDesc )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			strcpy( pRe->m_szDesc, s.Token );

			s.GetToken(); // }
		}
	}

	
	if( ::GetLanguage() == LANG_KOR )	//__JAPAN_PVP
		pRe->m_dwAttribute &= ~RA_FIGHT;


	switch( dwIndex )
	{
	case RI_BEGIN:
	case RI_REVIVAL:
		break;
	case RI_STRUCTURE:
		m_aStructure.Add( *pRe );
		break;
	default:
		{
			m_aRegion.Add( *pRe );	
			break;
		}
	}
	return TRUE;
}

BOOL CWorld::ReadRespawn( CScript& s )
{
	int nVersion = 1;
	if( s.Token == _T( "respawn2" ) )
		nVersion = 2;
	if( s.Token == _T( "respawn3" ) )
		nVersion = 3;
	//__YAIMONSTER_EX
	//__Y_PATROL				
	if( s.Token == _T( "respawn4" ) )
		nVersion = 4;	
	if( s.Token == _T( "respawn5" ) )
		nVersion = 5;	
	if( s.Token == _T( "respawn6" ) )
		nVersion = 6;
	if( s.Token == _T( "respawn7" ) )
		nVersion = 7;
	
	
	CRespawnInfo ri,* pInfo = &ri;
	DWORD dwType;
	DWORD dwIndex;
	D3DXVECTOR3 vPos;
	dwType = s.GetNumber();

	dwIndex = s.GetNumber();
	vPos.x = s.GetFloat();
	vPos.y = s.GetFloat();
	vPos.z = s.GetFloat();
	
	pInfo->m_dwIndex     = dwIndex;
	pInfo->m_dwType	= dwType;

	pInfo->m_nMaxcb		 = s.GetNumber();
	pInfo->m_cb          = 0;

	pInfo->m_uTime = (short)s.GetNumber();

	pInfo->m_nMaxAttackNum = (long)s.GetNumber();
	pInfo->m_nActiveAttackNum = 0;

	pInfo->m_vPos	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	pInfo->m_fY = vPos.y;
	if( dwType == OT_MOVER )
	{
		MoverProp* pMoverProp	= prj.GetMoverProp( dwIndex );
#ifdef __WORLDSERVER
		if( pMoverProp )
		{
			// PK서버가 아니면 가드를 skip한다.
			if( ::GetLanguage() != LANG_KOR )	// 한국의 경우 skip하지 않는다.
				if( pMoverProp->dwClass == RANK_GUARD && g_eLocal.GetState( EVE_PK ) == FALSE ) 
					return TRUE;
			
			if( pMoverProp->dwClass == RANK_MATERIAL )
				return TRUE;

			if( pMoverProp->dwClass != RANK_BOSS )
			{
				switch( pMoverProp->dwID )
				{
					case MI_AIBATT1:
					case MI_AIBATT2:
					case MI_AIBATT3:
					case MI_MUSHPANG1:
					case MI_MUSHPANG2:
					case MI_MUSHPANG3:
					case MI_BURUDENG1:
					case MI_BURUDENG2:
					case MI_BURUDENG3:
						pInfo->m_uTime	= (u_short)( pInfo->m_uTime	* 0.5 );
						break;
					default:
						pInfo->m_uTime	= (u_short)( pInfo->m_uTime * 0.7 );
						break;
				}						
			}
		if( pMoverProp->dwFlying )
			pInfo->m_vPos	= vPos;
		}
#endif	// __WORLDSERVER
	}
	pInfo->m_rect.left   = (LONG)s.GetNumber();
	pInfo->m_rect.top    = (LONG)s.GetNumber();
	pInfo->m_rect.right  = (LONG)s.GetNumber();
	pInfo->m_rect.bottom = (LONG)s.GetNumber();
	if( nVersion >= 2 )
	{
		pInfo->m_nDayMin = s.GetNumber();
		pInfo->m_nDayMax = s.GetNumber();
		pInfo->m_nHourMin = s.GetNumber();
		pInfo->m_nHourMax = s.GetNumber();
		pInfo->m_nItemMin = s.GetNumber();
		pInfo->m_nItemMax = s.GetNumber();
	}

	if( nVersion >= 4 )
	{		
		pInfo->m_dwAiState = s.GetNumber();
		pInfo->m_fAngle    = s.GetFloat();	
		
		if( pInfo->m_nMaxcb == 1 )
		{
			if( pInfo->m_dwAiState == 8 || pInfo->m_dwAiState == 9 )
			{
				pInfo->m_vPos = vPos;
			}
		}
	}

	if( nVersion >= 5 )
	{		
		pInfo->m_dwPatrolIndex = s.GetNumber();
	}
	
	if( nVersion >= 6 )
	{		
		pInfo->m_bPatrolCycle  = s.GetNumber();
	}
	
	if( nVersion >= 3 )
	{	
		int nContrl = s.GetNumber();
		if( nContrl )
		{
			pInfo->m_CtrlElem.m_dwSet = s.GetNumber(); 
			pInfo->m_CtrlElem.m_dwSetItem = s.GetNumber();  
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = s.GetNumber();  
			}
			else
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = 1;
			}
			pInfo->m_CtrlElem.m_dwSetLevel = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwSetQuestNum = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwSetFlagNum = s.GetNumber();  
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetQuestNum1 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetFlagNum1 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetQuestNum2 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetFlagNum2 = s.GetNumber();  
			}
			pInfo->m_CtrlElem.m_dwSetGender = s.GetNumber();

			int maxjob = 0;
			if(nVersion <= 6)
				maxjob = 16;
			else
				maxjob = 32;

			for( int i = 0; i < maxjob; i++ )
				pInfo->m_CtrlElem.m_bSetJob[ i ] = s.GetNumber();

			pInfo->m_CtrlElem.m_dwSetEndu = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMinItemNum = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMaxiItemNum = s.GetNumber();  
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemKind[ i ] = s.GetNumber();  
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemPer[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResKind[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResNum[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonActAttack[ i ] = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapOperType = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapRandomPer = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapDelay = s.GetNumber();  
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapKind[ i ] = s.GetNumber();  
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapLevel[ i ] = s.GetNumber(); 
			
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwTeleWorldId = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleX = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleY = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleZ = s.GetNumber();  
			}
		}
	}

#ifdef __WORLDSERVER
	if( FALSE == ri.GenResPoint( this ) )
	{
		char lpOutputString[256]	=	{ 0,};
		sprintf( lpOutputString, "type=%d, index=%d, left=%d, top=%d, right=%d, bottom=%d, num=%d",
			ri.m_dwType, ri.m_dwIndex, ri.m_rect.left, ri.m_rect.top, ri.m_rect.right, ri.m_rect.bottom, ri.m_nMaxcb );
		OutputDebugString( lpOutputString );
		return FALSE;
	}

	m_respawner.Add( ri, SpawnType::Region );
#endif	// __WORLDSERVER

	return TRUE;
}

BOOL CWorld::LoadRegion()
{
#if defined( __WORLDSERVER ) || defined(__CLIENT)
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );

	CScript s;
	if( s.Load( strLandTemp, FALSE ) )
	{
		D3DXVECTOR3 vPos;
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "region" ) || s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
			{
				ReadRegion( s );
			}
			else
			if( s.Token == _T( "respawn" ) || s.Token == _T( "respawn2" ) || s.Token == _T( "respawn3" ) || s.Token == _T( "respawn4" ) || s.Token == _T( "respawn5" ) || s.Token == _T( "respawn6" ) || s.Token == _T( "respawn7" ))
			{
				ReadRespawn( s );
			}
			s.GetToken();
		}
#ifdef __LAYER_1021
		Expand();
#endif	// __LAYER_1021
		return TRUE;
	}
#endif

	return FALSE;
}


#ifdef __WORLDSERVER
BOOL CWorld::ReadWorld()
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	FLOAT aHeightMap[ ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) ];
	WATERHEIGHT aWaterHeight[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BYTE aLocation[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	int xLand, yLand;
	D3DXVECTOR3	vPos;

	CObj* pBkgnd;
	DWORD dwObjNum;
	
	for( int y = 0; y < m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_nLandWidth; x++ )
		{
			strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, x , y );

			CFileIO f;
			if( f.Open( strLandTemp, "rb" ) )
			{
				DWORD dwVersion;
				f.Read( &dwVersion, sizeof( DWORD) );
				if( dwVersion >= 1 )
				{
					f.Read( &xLand, sizeof( xLand ) );
					f.Read( &yLand, sizeof( yLand ) );
				}
				f.Read( (void*)aHeightMap, sizeof(FLOAT), ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) );

				for( int y2 = 0; y2 < MAP_SIZE; y2++ )
				{
					for( int x2 = 0; x2 < MAP_SIZE; x2++ )
					{
						m_apHeightMap[ ( ( ( y * MAP_SIZE ) + y2 ) * WORLD_WIDTH ) + ( ( x * MAP_SIZE ) + x2 ) ]
							= aHeightMap[ ( y2 * ( MAP_SIZE + 1 ) ) + x2 ];
					}
				}
				
				f.Read( (void*)aWaterHeight, sizeof( WATERHEIGHT ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
				for( int y2 = 0; y2 < NUM_PATCHES_PER_SIDE; y2++ )
				{
					for( int x2 = 0; x2 < NUM_PATCHES_PER_SIDE; x2++ )
					{
						m_apWaterHeight[ ( ( ( y * NUM_PATCHES_PER_SIDE ) + y2 ) * ( NUM_PATCHES_PER_SIDE * m_nLandWidth ) ) + ( ( x * NUM_PATCHES_PER_SIDE ) + x2 ) ]
							= aWaterHeight[ ( y2 * ( NUM_PATCHES_PER_SIDE ) ) + x2 ];
					}
				}
				if( dwVersion >= 2 )
				{
					// skip
					f.Read( aLocation, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
				}
				BYTE nLayer;
				f.Read( &nLayer, sizeof(BYTE), 1 );
				for( BYTE u = 0; u < nLayer; u++ ) {
					f.Seek( sizeof(WORD), SEEK_CUR );
					f.Seek( ( sizeof(BOOL) * NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), SEEK_CUR );
					f.Seek( ( sizeof(DWORD) * MAP_SIZE * MAP_SIZE ), SEEK_CUR );
				}

				// read objects
				f.Read( (void*)&dwObjNum, sizeof(DWORD), 1 );
				for( DWORD dw =  0; dw < dwObjNum; dw++ )
				{
					pBkgnd	= ReadObj( &f );
					vPos	= pBkgnd->GetPos();

				//	vPos.x	+= x * LANDREALSCALE;
				//	vPos.z	+= y * LANDREALSCALE;

					vPos.x	+= x * MAP_SIZE * m_iMPU;
					vPos.z	+= y * MAP_SIZE * m_iMPU;

					pBkgnd->SetPos( vPos );

					if( pBkgnd->m_dwIndex <= 10 )
						pBkgnd->SetAngle( (float)( rand() % 360 ) );

					pBkgnd->SetIndex( D3DDEVICE, pBkgnd->m_dwIndex );

					if( pBkgnd->GetType() != OT_OBJ )
					{
						SAFE_DELETE( pBkgnd );
						continue;
					}
					
					if( ::GetLanguage() == LANG_JAP && pBkgnd->m_dwIndex == 360 )
					{
						SAFE_DELETE( pBkgnd );
					}
					else
					{
						if( pBkgnd->m_pModel && TRUE == pBkgnd->m_pModel->IsHaveCollObj() )
						{
//							pBkgnd->InitMotion( pBkgnd->m_dwMotion );
/*
#ifdef __YAIMONSTER_EX
							if( pBkgnd->GetType() == OT_MOVER )
							{
								CRespawnInfo *pInfo = NULL;	
								((CMover*)pBkgnd)->m_resp
								pInfo = &((CMover*)pBkgnd)-> m_respawnInfo;
								
								if( pInfo->m_cb == 1 )
									((CMover*)pBkgnd)->AddAngle(pInfo->m_fAngle);
							}
#endif //__YAIMONSTER_EX
*/							
							pBkgnd->SetWorld( this );
							InsertObjLink( pBkgnd );
							m_vecBackground.push_back( pBkgnd );
							pBkgnd->UpdateLocalMatrix();
						}
						else
						{
							SAFE_DELETE( pBkgnd );
						}
					}
				}
			}
			f.Close();

#ifdef __MAP_SECURITY
			g_WorldMng.AddMapCheckInfo( strLandTemp );
#endif // __MAP_SECURITY
		}
	}
	return TRUE;
}
#endif	// __WORLDSERVER

#if defined( __CLIENT ) 

BOOL CWorld::ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand  )
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );

	const auto [x, z] = WorldPosToLand(vPos);

	// 랜드 이동이 없다. 리턴. 
	if( x == m_nCharLandPosX && z == m_nCharLandPosZ )
		return FALSE;

	m_nCharLandPosX = x;
	m_nCharLandPosZ = z;
		
	for(int i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
	{
		for(int j = x - m_nVisibilityLand; j <= ( x + m_nVisibilityLand ); j++)
		{
			if( LandInWorld( j, i ) )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];
				if( pLand == NULL )
				{
					//! gmpbigsun : pLand는 할당이 이미 끝난 녀석이다 왜 지우지 않았나;;;
					// delete pLand;

					strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, j , i );
					pLand = new CLandscape;
					pLand->m_nWorldX = j * MAP_SIZE;
					pLand->m_nWorldY = i * MAP_SIZE;
					pLand->InitDeviceObjects( m_pd3dDevice, this );
					if( pLand->LoadLandscape( strLandTemp, j, i ) == FALSE )
					{
						safe_delete( pLand );
						continue;
					}
					pLand->RestoreDeviceObjects( m_pd3dDevice );

					m_apLand[ i * m_nLandWidth + j] = pLand;

#ifdef __MAP_SECURITY
					g_WorldMng.AddMapCheckInfo( strLandTemp );
#endif // __MAP_SECURITY
				}
			}
		}
	}

	if( bEraseOldLand )
	{
		// 현재 보이지 않는 랜드는 파괴 읽을 때는 3*3을 읽고, 파괴는 현재 위치에서 4*4를 벗어난 것들.
		// 만약 파괴할 때 3*3를 벗어난 것을 파괴하면 캐릭터는 조금만 움직여도 재로딩을 하게 될지 모르기 때문이다.
		// 즉, 3*3을 벗어난 것을 파괴하게 되면 늘 3*3 정도가 로딩을 유지하게 되는데, 
		// 그렇게 되면 로딩 빈도가 잦아져 속도가 느려지게 되는 것이다. 로딩 데이타를 유지하는 것은
		// 4*4로 하여 로딩 빈도를 낮춘다.
		CRect rect( x - m_nVisibilityLand - 1, z - m_nVisibilityLand - 1, x + m_nVisibilityLand + 2, z + m_nVisibilityLand + 2 );
		for( int i = 0; i < m_nLandHeight; i++ )
		{
			for( int j = 0; j < m_nLandWidth; j++ )
			{
				CLandscape *& pLand = m_apLand[i * m_nLandWidth + j];
				if( pLand )
				{
					if( rect.PtInRect( CPoint( j, i ) ) == FALSE )
					{
						pLand->InvalidateDeviceObjects();
						pLand->DeleteDeviceObjects();
						SAFE_DELETE(pLand);
						m_objCull.clear();
					}
					else
						pLand->SetUsedAllObjects();
				}
				
			}			
		}
		// 사용하지 않는 모델을 파괴한다.
		// 정적인것만 이리로 들어와야 한다. 동적인것(스키닝을 쓰는것)이 여기서 파괴되면 안된다.
		for( auto itor = prj.m_modelMng.m_mapFileToMesh.begin(); itor != prj.m_modelMng.m_mapFileToMesh.end(); /*itor++*/ )
		{
			CModelObject * pModel = itor->second;
			pModel->DeleteDeviceObjects();
			if( pModel->m_pModelElem->m_bUsed == FALSE && pModel->m_pModelElem->m_dwType != OT_ITEM )
			{
				SAFE_DELETE( pModel );
				prj.m_modelMng.m_mapFileToMesh.erase( itor++ );
			}
			else
			{
				pModel->m_pModelElem->m_bUsed = FALSE;
				++itor;
			}
		}
	}
	return TRUE;
}


#endif // __CLIENT


/*
#ifdef __IVERSION_NODARKON
				// x가 11 이하면 완전히 갈 수 없도록 하자.
				if( x <= 11 || ( x == 12 && y == 9 ) || ( x == 12 && y == 8 ) )
				{
					for( int i = 0; i < ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ); i++ )
					{
						if( aHeightMap[ i ] >= HGT_DIE )
							aHeightMap[ i ] -= HGT_DIE;
						else
						if( aHeightMap[ i ] >= HGT_NOMOVE )
							aHeightMap[ i ] -= HGT_NOMOVE;
						else
						if( aHeightMap[ i ] >= HGT_NOFLY )
							aHeightMap[ i ] -= HGT_NOFLY;
						else
						if( aHeightMap[ i ] >= HGT_NOWALK )
							aHeightMap[ i ] -= HGT_NOWALK;

						aHeightMap[ i ] += HGT_NOMOVE;
					}
				}
				if( x == 12 && y == 7 ) // 대각선 
				{
					int x2max = 0;
					for( int y2  = 0; y2 < ( MAP_SIZE + 1 ); y2++ )
					{
						for( int x2  = 0; x2 < x2max; x2++ )
						{
							int i = y2 * ( MAP_SIZE + 1 ) + x2;
							if( aHeightMap[ i ] >= HGT_DIE )
								aHeightMap[ i ] -= HGT_DIE;
							else
							if( aHeightMap[ i ] >= HGT_NOMOVE )
								aHeightMap[ i ] -= HGT_NOMOVE;
							else
							if( aHeightMap[ i ] >= HGT_NOFLY )
								aHeightMap[ i ] -= HGT_NOFLY;
							else
							if( aHeightMap[ i ] >= HGT_NOWALK )
								aHeightMap[ i ] -= HGT_NOWALK;

							aHeightMap[ i ] += HGT_NOMOVE;
						}
						x2max++;
						if( x2max >= ( MAP_SIZE + 1 ) ) 
							x2max = ( MAP_SIZE + 1 );
					}
				}
#endif
*/

#ifdef __WORLDSERVER
BOOL CWorld::IsUsableDYO( CObj* pObj )
{
	if( pObj->GetType() == OT_MOVER )
	{
		LPCHARACTER pCharacter	= ( (CMover*)pObj )->GetCharacter();
		if( pCharacter )
		{
			// pk
			if( stricmp( pCharacter->m_szKey, "Npc_Reward" ) == 0
				|| stricmp( pCharacter->m_szKey, "MaMa_PKNPC01" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_PK ) == FALSE )
					return FALSE;
			}
			// 길드 대전
			if( stricmp( pCharacter->m_szKey, "MaFl_GuildWar" ) == 0
					|| stricmp( pCharacter->m_szKey, "MaFl_Donaris" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == FALSE )
					return FALSE;
			}
 			if( stricmp( pCharacter->m_szKey, "MaFl_Annie" ) == 0
 					|| stricmp( pCharacter->m_szKey, "MaFl_Amos" ) == 0 )
 			{
 				if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) == FALSE )
 					return FALSE;
 			}

#ifdef __JEFF_11_4
			if( stricmp( pCharacter->m_szKey, "MaFl_Ray" ) == 0 )
 			{
 				if( g_eLocal.GetState( EVE_ARENA ) == FALSE )
 					return FALSE;
 			}
#endif // __JEFF_11_4

			if( stricmp( pCharacter->m_szKey, "MaFl_SecretRoom_EAST" ) == 0
				|| stricmp( pCharacter->m_szKey, "MaDa_SecretRoom_WEST" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_SECRETROOM ) == FALSE )
					return FALSE;
			}

			// don't load designer & artist in taiwan & japan
			// 일본, 대만에서 제외
			if( ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK )// || ::GetLanguage() == LANG_JAP )
			{
				if( stricmp( pCharacter->m_szKey, "MaFl_HairShop" ) == 0
					|| stricmp( pCharacter->m_szKey, "MaFl_FaceOff" ) == 0 )
					return FALSE;
			}

			// 13차 레인보우 레이스 - 진행 채널에서만 표시하게...
			if( stricmp( pCharacter->m_szKey, "MaFl_RainbowStart" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_RAINBOWRACE ) == FALSE )
					return FALSE;
			}

#ifdef __CHIPI_DYO
			return IsUsableDYO2( pCharacter );
#endif // __CHIPI_DYO

		}
	}
	return TRUE;
}

// 특정 국가만 출력하도록 스크립트에서 설정[character.inc - SetLang(LANG_...)]
#ifdef __CHIPI_DYO
BOOL CWorld::IsUsableDYO2( LPCHARACTER pCharacter )
{
	if( pCharacter->m_vecdwLanguage.empty() )
		return pCharacter->bOutput;
	
	for( DWORD i=0; i<pCharacter->m_vecdwLanguage.size(); i++ )
	{
		if( HIWORD(pCharacter->m_vecdwLanguage[i]) == ::GetLanguage()
			&& LOWORD(pCharacter->m_vecdwLanguage[i]) == ::GetSubLanguage() )
			return pCharacter->bOutput;
	}

	return !(pCharacter->bOutput);
}
#endif // __CHIPI_DYO
#endif	// __WORLDSERVER

#ifdef __LAYER_1021
BOOL CWorld::CreateLayer( int nLayer )
{
//	ASSERT( IsExpandable() );
//	if( !IsExpandable() )
//		return FALSE;
	ASSERT( nLayer != 0 );
	if( nLayer == 0 )
		return FALSE;
	if( !m_linkMap.CreateLinkMap( nLayer ) )
		return FALSE;
	if( !LoadObject( nLayer ) )
	{
		m_linkMap.DeleteLinkMap( nLayer );
		return FALSE;
	}
	m_respawner.Expand( nLayer );
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
bool CWorld::HasSomeone(const int nLayer) const {
	constexpr auto HasSomeoneInWorld = [](const CWorld & world, const int nLayer) {
		for (CObj * pObj : world.m_Objs.ValidObjs()) {
			if (IsLayerPlayer(pObj, nLayer)) {
				return true;
			}
		}

		return false;
	};

	constexpr auto HasSomeoneToAdd = [](const CWorld & world, const int nLayer) {
		return std::ranges::any_of(world.m_aAddObjs,
			[nLayer](const AddRequest & add) {
				return add.pObj && add.pObj->GetLayer() == nLayer;
			}
		);
	};

	return HasSomeoneInWorld(*this, nLayer)
		|| HasSomeoneToAdd(*this, nLayer)
		|| g_WorldMng.HasSomeoneGoingTo(GetID(), nLayer);
}


bool CWorld::IsLayerPlayer(CObj * pObj, int nLayer) {
	return (pObj && pObj->GetLayer() == nLayer && pObj->GetType() == OT_MOVER && static_cast<CMover *>(pObj)->IsPlayer());
}

void CWorld::DriveOut( int nLayer )
{
#ifdef __WORLDSERVER
	for (CObj * pObj : m_Objs.ValidObjs()) {
		if( IsLayerPlayer( pObj, nLayer ) )
		{
			CUser* pUser	= static_cast<CUser*>(pObj);
			switch( GetID() )
			{
				case WI_WORLD_MINIROOM : CHousingMng::GetInstance()->GoOut( pUser ); break;
				default :
					if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( GetID() ) )
					{
						CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
						break;
					}
					else if( GuildHouseMng->IsGuildHouse( GetID() ) )
					{
						Invalidate( nLayer, FALSE );
						break;
					}
					pUser->Replace( WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_FORCE, nDefaultLayer );
					break;
			}
		}
	}
#endif	// __WORLDSERVER
}

BOOL CWorld::ReleaseLayer( int nLayer )
{
	DestroyObj( nLayer );
	m_linkMap.DeleteLinkMap( nLayer );
	m_respawner.Release( nLayer );
	return TRUE;
}

void CWorld::DestroyObj( int nLayer )
{
	for (size_t i = 0; i != m_Objs.realSize; ++i) {
		CObj * pObj = m_Objs.objects[i];
		if (pObj && pObj->GetLayer() == nLayer) {
			DestroyObj(pObj);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
#endif	// __LAYER_1021