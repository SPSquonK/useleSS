#include "stdafx.h"

#include "AppDefine.h"
#include "Material.h" 
#include "DialogMsg.h"
#include "party.h"
#include "../_Common/ModelObject.h" 
#include "../_Common/HwOption.h"
#include "../_Common/ParticleMng.h"
#include "TailEffectMng.h"
#include "Sfx.h"
#include "definetext.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "dpcertified.h"
#include "guild.h"
#include "guildwar.h"
#include "resource.h"
#include "Chatting.h"
#include "webbox.h"
#include "network.h"
#include "defineSound.h"
#include "ClientMsg.h"

#include "WndBase.h"
#include "WndCharacter.h"
#include "WndGuideSystem.h"

#include "WorldMap.h"

#include "tools.h"

#include "GuildHouse.h"


//////////////////////////////////////////////////////////////////////////////
// global variable
//////////////////////////////////////////////////////////////////////////////

extern CSfxTexture      g_SfxTex;
extern CSfxMng          g_SfxMng;
extern CSfxMeshMng      g_SfxMeshMng;
extern HANDLE			g_hMutex;

CDamageNumMng			g_DamageNumMng;
CModelObject			g_Shadow;

//////////////////////////////////////////////////////////////////////////////
// global enum
//////////////////////////////////////////////////////////////////////////////

#ifdef __GAME_GRADE_SYSTEM
#ifdef __CLIENT
const FLOAT CNeuzApp::GAME_GRADE_MARK_FADING_SPEED( 800.0F );
#endif // __CLIENT
#endif // __GAME_GRADE_SYSTEM

//////////////////////////////////////////////////////////////////////////////
// global function
//////////////////////////////////////////////////////////////////////////////
void SetMouseCursor( DWORD dwCursor )
{
	g_Neuz.SetDeviceCursor( g_Neuz.m_ahCursor[ dwCursor ] );
}

//////////////////////////////////////////////////////////////////////////////
// CNeuzApp 
//////////////////////////////////////////////////////////////////////////////
CNeuzApp::CNeuzApp()
{
	m_dwTimeOutDis = 0xffffffff;
	m_dwTimeReSkillPoint = 0xffffffff;

#ifdef _DEBUG
	m_bStartFullscreen = FALSE;
#else
 	m_bStartFullscreen = TRUE;
#endif
	
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_d3dEnumeration.AppMinDepthBits = 16;
    m_d3dEnumeration.AppUsesMixedVP = TRUE;

#ifdef __XUZHU
	g_GameTimer.m_bFixed = TRUE;
	g_GameTimer.m_nHour = 12;		// 언제나 낮이 되도록
#endif
#ifdef __TWN_LOGIN0816
	m_szSessionPwd[0]	= '\0';
#endif	// __TWN_LOGIN0816

	m_dwCreationWidth  = FULLSCREEN_WIDTH;
	m_dwCreationHeight = FULLSCREEN_HEIGHT - GetSystemMetrics( SM_CYMENU );

	m_strWindowTitle  = _T( "FLYFF" );
	g_pD3dApp = this;

	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bFrameSkip = TRUE;
	m_dwTempMessage = 0;

	m_lpCertifierAddr[0] = '\0';
	memset( m_lpCacheAddr, 0, sizeof(TCHAR) * 16 );
	m_uCachePort = PN_CACHESRVR;
	memset( &m_loginSI, 0x00, sizeof(m_loginSI) );

	m_dwShoutLimitSecond = 0;
	m_nShoutLimitCount = 0;

	m_szAccount   [ 0 ] = 0; // 어카운트
	m_szPassword  [ 0 ] = 0; // password
#ifdef __GPAUTH_01
	m_bGPotatoAuth	= FALSE;
	*m_szGPotatoNo	= '\0';
	#ifdef __GPAUTH_02
	*m_szCheck	= '\0';
	#endif	// __GPAUTH_02
#endif	// __GPAUTH_01
	m_szCaptureFileName[ 0 ] = 0;
	m_bCapture = FALSE;
	m_nCaptureCnt = 0;
	g_pBipedMesh = NULL;
	m_dwSys	= 0;
	m_bQuit = FALSE;

	m_uIdofMulti	= NULL_ID;
	m_b18Server     = FALSE;
	m_dwAuthKey		= 0;
	m_cbAccountFlag	= 0X00;

	m_dwTimeLeft = 0;
	m_dwCurTick	 = 0;		
	m_nLeftTimeNotifyStatus = std::nullopt;
	
	m_bActiveNeuz = TRUE;
	
	m_dwSummonPartyData = 0;
	m_dwSummonPartyObjid = 0;
	ZeroMemory( m_szSummonPartyWorldName, sizeof( m_szSummonPartyWorldName ) );
	
	m_vCursorPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_pCreateMonItem = NULL;

	m_szWhisperName[0] = '\0';

#ifdef __THROUGHPORTAL0810
	m_bThroughPortal	= FALSE;
#else	// __THROUGHPORTAL0810
	m_bHanGame = FALSE;
#endif	// __THROUGHPORTAL0810
	m_bEncryptPWD = TRUE;
	m_idPlayer	= 0;
	m_hThread = INVALID_HANDLE_VALUE;
	memset( m_apPlayer, 0, sizeof(m_apPlayer) );
	m_savedInven.fill(SavedSoldItem{});
	for(int i = 0; i < 6; i++) m_pMasterIcon[i] = NULL;
	m_pHeroIcon = NULL;
	m_n2ndPasswordNumber = -1;

#ifdef __GAME_GRADE_SYSTEM
#ifdef __CLIENT
	m_dwTimeGameGradeMarkRendering = 0xffffffff;
	m_bGameGradeMarkRenderingSwitch = FALSE;
	m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MINIMUM_ALPHA;
	m_eGameGradeMarkState = FADE_IN;
	m_dwTimeOld = g_tmCurrent;
	m_dwTimeGameGradeMarkShowing = 0xffffffff;
#endif // __CLIENT
#endif // __GAME_GRADE_SYSTEM

	pOldModalWindow = NULL;
}

BOOL CNeuzApp::LoadOption()
{
	g_Option.Load( "neuz.ini" );
	
#ifdef _DEBUG
	g_Option.LoadToolTip( "ToolTip.ini" );
#endif // _DEBUG

	// 화면 사이즈 세팅 
	FULLSCREEN_WIDTH	= g_Option.m_nResWidth;
	FULLSCREEN_HEIGHT	= g_Option.m_nResHeight;

	SetLODDetail( g_Option.m_nViewArea );
	
	m_dwCreationWidth  = FULLSCREEN_WIDTH;
	m_dwCreationHeight = FULLSCREEN_HEIGHT - GetSystemMetrics( SM_CYMENU );

	return TRUE;
}


HRESULT CNeuzApp::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	// 스키닝 코드 
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE

	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )	// 버텍스쉐이더 1.1을 지원 못하는가.
	{
		g_bUsableVS = FALSE;		// 버텍스쉐이더 사용안됨.
		if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )	// hw vp는 사용못함
			return E_FAIL;
	}
    // Check that the device can blend vertices with at least two matrices
    // (Software can always do up to 4 blend matrices)
	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
	{
        if( pCaps->MaxPointSize <= 1.0f )
            return E_FAIL;
		if( pCaps->MaxVertexBlendMatrices < 2 )
			return E_FAIL;
	}

    // If this is a TnL device, make sure it supports directional lights
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( !(pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ) )
            return E_FAIL;
    }

	if( pCaps->RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
		return S_OK;

	return S_OK;
}


//
// 디바이스 생성후 단 한번 실행한다.
//
HRESULT CNeuzApp::OneTimeSceneInit()
{
	CNetwork::GetInstance().StartLog();

	// 사운드 초기화
	HRESULT hr = g_SoundMng.Initialize( GetSafeHwnd(), DSSCL_PRIORITY, 2, 22050, 16 );
	InitCustomSound( FAILED(hr) ? FALSE : TRUE );

	g_SoundMng.m_nSoundVolume = (int)( (-1) * (1.0f - g_Option.m_fEffectVolume) * 5000 );

	::SetVolume( g_Option.m_fBGMVolume );

	m_ahCursor[CUR_ATTACK] = LoadCursorFromFile("Cursor\\curAttack.cur"); 		
	m_ahCursor[CUR_BASE] = LoadCursorFromFile("Cursor\\curBase.cur"); 			
	m_ahCursor[CUR_CAMERA] = LoadCursorFromFile("Cursor\\curCamera.cur"); 		
	m_ahCursor[CUR_CONTROL] = LoadCursorFromFile("Cursor\\curControl.cur"); 	
	m_ahCursor[CUR_EDIT] = LoadCursorFromFile("Cursor\\edit.cur"); 				
	m_ahCursor[CUR_GETITEM] = LoadCursorFromFile("Cursor\\curGetItem.cur");		
	m_ahCursor[CUR_HELP] = LoadCursorFromFile("Cursor\\curHelp.cur"); 			
	m_ahCursor[CUR_NO] = LoadCursorFromFile("Cursor\\curNo.cur");  				
	m_ahCursor[CUR_NPC] = LoadCursorFromFile("Cursor\\curNpc.cur"); 			
	m_ahCursor[CUR_SELECT] = LoadCursorFromFile("Cursor\\curSelect.cur"); 		
	m_ahCursor[CUR_RESIZE_HORI] = LoadCursorFromFile("Cursor\\hori.cur");		
	m_ahCursor[CUR_RESIZE_VERT] = LoadCursorFromFile("Cursor\\resize_h.cur");	
	m_ahCursor[CUR_RESIZE_HV1] = LoadCursorFromFile("Cursor\\cur00001.cur");	
	m_ahCursor[CUR_RESIZE_HV2] = LoadCursorFromFile("Cursor\\cur00002.cur"); 	
	m_ahCursor[CUR_HAMMER] = LoadCursorFromFile("Cursor\\anvil.cur"); 			
	m_ahCursor[CUR_DELAY] = LoadCursorFromFile("Cursor\\curDelay.cur");			

	CWndBase::m_hDefaultCursor = m_ahCursor[ CUR_BASE ];
	CWndEdit::m_hEditCursor = m_ahCursor[ CUR_EDIT ];

	return S_OK;
}  


///////////////////////////////////////////////////////
HRESULT CNeuzApp::RestoreDeviceObjects()
{
#ifdef __YENV
	if( m_pEffect != NULL )
		m_pEffect->OnResetDevice();

	
	if( FAILED( LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, "Env.dds" ), &m_pEnvTex ) ) )
	{
		Error( "Env.dds 텍스쳐 못읽음" );
	}
#endif //__YENV
	
    if( ( m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) || ( m_dwCreateFlags & D3DCREATE_MIXED_VERTEXPROCESSING ) ||
        m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1) )
    {
		CreateSkinningVS( m_pd3dDevice, _T("skining2.vsh") );
    }
	
	if( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS )	// d3d9.0에 새로 추가된 기능인데 이걸 지원못하는 카드가 있다. 그래서 이렇게 검사한다.
		g_ModelGlobal.m_bDepthBias = TRUE;
	if( g_Option.m_nShadow < 2 && m_d3dCaps.MaxSimultaneousTextures <= 2 )		// 실시간그림자를 세팅하고 하드웨어가 딸리면
	{
		g_Option.m_nShadow = 2;		// 실시간 그림자 기능을 끈다.
		MessageBox( GetSafeHwnd(), "Shadows are not available on this graphics card. Change the shadow function to low.", "Guide", MB_OK );
	}
	UINT nMem = m_pd3dDevice->GetAvailableTextureMem();
	if( g_Option.m_nTextureQuality == 0 && nMem <= (1024 * 1024 * 64) )
	{
		g_Option.m_nTextureQuality = 1;		// 64메가 이하의 텍스쳐메모리에선 텍스쳐품질을 낮춘다.
		//MessageBox( GetSafeHwnd(), "Lower texture quality due to insufficient texture memory available.", "Warning", MB_OK );
	}
	if( g_Option.m_nShadow < 2 )
	{
		if( nMem <= (1024 * 1024 * 64) )
		{
			g_Option.m_nShadow = 2;		// 64메가 이하의 텍스쳐메모리에선 그림자기능을 사용하지 못한다.
			//MessageBox( GetSafeHwnd(), "Disable shadows because there is not enough texture memory available.", "Warning", MB_OK  );
		} else
		{
//			CreateShadowMap( m_pd3dDevice, m_d3dpp.BackBufferFormat );
			BOOL bSuccess = CreateShadowMap( m_pd3dDevice, D3DFMT_R5G6B5 );
			if( bSuccess == FALSE )
			{	
				// 실패하면 비됴메모리 부족이라 판단하고 옵션 강제로 낮춤.
				if( g_Option.m_nShadow < 2 )
					g_Option.m_nShadow ++;
				if( g_Option.m_nTextureQuality < 2 )
					g_Option.m_nTextureQuality ++;
			}
		}
	}

	if( g_Option.m_nBloom == 1 )
	{
		if( m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
			g_Glare.Create( m_pd3dDevice, D3DFMT_R5G6B5, g_Option.m_nResWidth, g_Option.m_nResHeight - 48 );
		else
			g_Option.m_nBloom = 0;
	}

	g_ParticleMng.RestoreDeviceObjects( m_pd3dDevice );
	g_TailEffectMng.RestoreDeviceObjects( m_pd3dDevice );

    // Initialize the particle system
//	HRESULT hr;

	extern LPDIRECT3DTEXTURE9 g_pReflectMap;
	if( FAILED( LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, "etc_reflect.tga" ), &g_pReflectMap ) ) )
	{
		Error( "etc_Reflect.tga 텍스쳐 못읽음" );
	}

	g_ModelGlobal.RestoreDeviceObjects( m_pd3dDevice );

	m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,   D3DSHADE_GOURAUD ); 
	CWndBase::m_Theme.RestoreDeviceObjects();

	g_WndMng.RestoreDeviceObjects();
	if( m_pd3dDevice )
	{
		m_2DRender.RestoreDeviceObjects(&m_d3dsdBackBuffer);
		if( g_WorldMng.Get() )
			g_WorldMng.Get()->RestoreDeviceObjects( m_pd3dDevice );
		CWorld::StaticRestoreDeviceObjects( m_pd3dDevice );
		prj.m_modelMng.RestoreDeviceObjects();
	}
	if( g_pBipedMesh )
		g_pBipedMesh->RestoreDeviceObjects();	

	g_SfxMng.RestoreDeviceObjects();
	g_SfxMeshMng.RestoreDeviceObjects();
	
	g_DialogMsg.RestoreDeviceObjects();
	
	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->RestoreDeviceObjects(); 

	return S_OK;
}

///////////////////////////////////////////////////
HRESULT CNeuzApp::InvalidateDeviceObjects()
{
#ifdef __YENV
	if( m_pEffect != NULL )
		m_pEffect->OnLostDevice();

	SAFE_RELEASE(m_pEnvTex);	

#endif //__YENV	

	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->InvalidateDeviceObjects(); 

	g_Glare.DeleteDeviceObjects();
	DeleteShadowMap( m_pd3dDevice );
	DeleteVertexShader( m_pd3dDevice );
	CWndBase::m_Theme.InvalidateDeviceObjects();
	if( g_WorldMng.Get() )
		g_WorldMng.Get()->InvalidateDeviceObjects();
	CWorld::StaticInvalidateDeviceObjects();
	g_WndMng.InvalidateDeviceObjects();
	prj.m_modelMng.InvalidateDeviceObjects();
	m_2DRender.InvalidateDeviceObjects();
	if( g_pBipedMesh )
		g_pBipedMesh->InvalidateDeviceObjects();

	g_SfxMng.InvalidateDeviceObjects();
	g_SfxMeshMng.InvalidateDeviceObjects();
	
	extern LPDIRECT3DTEXTURE9 g_pReflectMap;
	SAFE_RELEASE( g_pReflectMap	);

	g_ParticleMng.InvalidateDeviceObjects();
	g_TailEffectMng.InvalidateDeviceObjects();

	g_ModelGlobal.InvalidateDeviceObjects( m_pd3dDevice );

	g_DialogMsg.InvalidateDeviceObjects();
	
	return S_OK;
}


///////////////////////////////////////////////////////
HRESULT CNeuzApp::DeleteDeviceObjects()
{
	RestoreFakeDisplayMode();
	
#ifdef __YENV
	SAFE_RELEASE(m_pEffect);
#endif //__YENV

	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->DeleteDeviceObjects(); 
	g_toolTip.Delete();
	g_toolTipSub1.Delete();
	g_toolTipSub2.Delete();
	g_Glare.DeleteDeviceObjects();
	CWndBase::m_Theme.DeleteDeviceObjects();
	g_WndMng.DestroyApplet();
	g_WndMng.Free();

	if( g_WorldMng.Get() )
		g_WorldMng.Get()->DeleteDeviceObjects();

	g_WorldMng.Free();
	CWorld::StaticDeleteDeviceObjects();
	g_WndMng.DeleteDeviceObjects();
	prj.m_modelMng.DeleteDeviceObjects();
	prj.m_terrainMng.DeleteDeviceObjects();
	m_2DRender.DeleteDeviceObjects();
	g_DamageNumMng.DeleteDeviceObjects();
	g_DialogMsg.DeleteDeviceObjects();
	if( g_pBipedMesh )
		g_pBipedMesh->DeleteDeviceObjects();

	g_TextureMng.DeleteDeviceObjects();
 
	SAFE_DELETE( g_pBipedMesh );		// g_Object3DMng가 파괴되기전에 부를것
	g_Shadow.DeleteDeviceObjects();
 
	// 여기서 이것을 따로 불러준 이유는 게임종료시에 Release되지 않은 수를 판정할때 
	// g_Object3DMng의 파괴자가 늦게 발동되어 누락되기 때문이다.  그래서 여기서 먼저 파괴시켜준다.  결과는 같다.
	// Destroy를 다른데서 호출하지 말것!
	 g_Object3DMng.DeleteDeviceObjects();
#ifdef __PROF
	g_Prof.Show( m_fFPS );
#endif

	g_SfxMng.DeleteDeviceObjects();
	g_SfxTex.DeleteAll();
	g_SfxMeshMng.DeleteAll();

	g_Object3DMng.Destroy();	
	CWndNeuz::FreeTileTexture();

	m_TexCltGauge[0].DeleteDeviceObjects();
	m_TexCltGauge[1].DeleteDeviceObjects();
	m_TextureGauge[0].DeleteDeviceObjects();
	m_TextureGauge[1].DeleteDeviceObjects();
	m_TextureGauge[2].DeleteDeviceObjects();
	m_TextureGauge[3].DeleteDeviceObjects();
	m_TextureGauge[4].DeleteDeviceObjects();

	m_TextureHPGauge[0].DeleteDeviceObjects();
	m_TextureHPGauge[1].DeleteDeviceObjects();
	m_TextureHPGauge[2].DeleteDeviceObjects();

	m_TextureTurboGauge[0].DeleteDeviceObjects();
	m_TextureTurboGauge[1].DeleteDeviceObjects();
	m_TextureCastingGauge[0].DeleteDeviceObjects();
	m_TextureCastingGauge[1].DeleteDeviceObjects();	

	m_TextureAngelGauge[0].DeleteDeviceObjects();
	m_TextureAngelGauge[1].DeleteDeviceObjects();
	m_TexturePackPVP.DeleteDeviceObjects();
	m_TexLoading.DeleteDeviceObjects();
	m_texQuestEmoticon.DeleteDeviceObjects();
	return S_OK;
}
// 
// 종료하기 전 객체를 파괴하는 곳
//
HRESULT CNeuzApp::FinalCleanup()
{
#ifdef _DEBUG
	WaitLoading();
#endif

	if( g_hMutex )
		::CloseHandle( g_hMutex );
	
	// 다시 알트탭 되게
	::SystemParametersInfo (SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);		// 95, 98, Me면 API를 쓴다.

	Drv_SetGamma(GetSafeHwnd(), 1.0f, 10, 1.0f);
	
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( m_apPlayer[i] )
			m_apPlayer[i]->m_pModel = NULL; 
		SAFE_DELETE( m_apPlayer[i] );
	}
	g_dpCertified.DeleteDPObject();
	g_dpLoginClient.DeleteDPObject();
	g_DPlay.DeleteDPObject();
	g_GuildWarMng.Clear();
	g_GuildMng.Clear();

	SAFE_DELETE( CParty::m_pPool );
	SAFE_DELETE( CGuildMember::sm_pPool );
	SAFE_DELETE( CGuild::sm_pPool );
	SAFE_DELETE( CMover::m_pPool );
	SAFE_DELETE( CActionMover::m_pPool );
	SAFE_DELETE( CItem::m_pPool );
	SAFE_DELETE( CSfxGenMoveMark::m_pPool );
	SAFE_DELETE( CSfxGenWaterCircle::m_pPool );
	SAFE_DELETE( CSfxGenNormalDmg01::m_pPool );
	SAFE_DELETE( CSfxModel::m_pPool );
	SAFE_DELETE( CSfxGenRainCircle::m_pPool );

	// 군주 협력 객체의 제거
	CCLord::Instance()->DestroyColleagues();

	UninitializeNetLib();

	CWndNeuz::FreeTileTexture();
	CResFile::FreeResource();
	UnInitCustomSound();

	g_Option.Save( "neuz.ini" );
 	return S_OK;
}

#ifdef __WRITEAVI

#include "..\_multimedia\writeAvi.h"
#pragma comment( lib, "vfw32.lib" ) 
CAVIFile avi( _T( "Capture\\test.avi" ) );

HRESULT CNeuzApp::CaptureBitmap()
{
	IDirect3DSurface8* pDestSurface;
	m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
	RECT  rcSource[] = {  0,  0,  1024,  768 };
	POINT ptDest[]   = {  0,  0 };

	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
	D3DVIEWPORT9 viewport;
	m_pd3dDevice->GetViewport( &viewport );
	//LPBYTE lpData = new BYTE[ viewport.Width * viewport.Height * 3 ];D3DLOCK_DISCARD 

	if( pDestSurface->LockRect( &lockedRect, rectIn, D3DLOCK_READONLY  ) == D3D_OK )// Lock
	{
	//	LPDWORD pWord = (LPDWORD)lockedRect.pBits;
	//	memcpy( bmp.GetData(), pWord, viewport.Width * viewport.Height * 4 );

		//
		//LPWORD pWord = (LPWORD)lockedRect.pBits;

		//for(int y = 0; y < viewport.Height; y++ )
		//{
		//	for(int x = 0; x < viewport.Width; x++)
		//	{
		///		WORD wData = pWord[ y * viewport.Width + x ]; 
		//		// 16 bit color
		//		BYTE byData1 = ( wData & 0xf800 ) >> 8;
		//		BYTE byData2 = ( wData & 0x07e0 ) >> 3;
		//		BYTE byData3 = ( wData & 0x001f ) << 3 ;
		//		DWORD dwOffset = ( y * viewport.Width + x ) * 3;
		//		lpData[ dwOffset + 0 ] = byData3;
		//		lpData[ dwOffset + 1 ] = byData2;
		//		lpData[ dwOffset + 2 ] = byData1;
		//	}
		//}
		//
		


			LPDWORD pWord = (LPDWORD)lockedRect.pBits;
			memcpy( g_bmp.GetData(), pWord, viewport.Width * viewport.Height * 4 );

		pDestSurface->UnlockRect();

	}
	pDestSurface->Release();
	avi.AddFrame( g_bmp );
	//SAFE_DELETE( lpData );
	return S_OK;
}
#endif
/*
	IDirect3DSurface8* pDestSurface;
	m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );

	D3DVIEWPORT9 viewport;
	m_pd3dDevice->GetViewport( &viewport );
	LPBYTE lpData = new BYTE[ viewport.Width * viewport.Height * 3 ];
	ZeroMemory( lpData, viewport.Width * viewport.Height * 3 );
	if( pDestSurface->LockRect( &lockedRect, rectIn, 0 ) == D3D_OK )// Lock
	{
		
		LPWORD pWord = (LPWORD)lockedRect.pBits;
		for(int y = 0; y < viewport.Height; y++ )
		{
			for(int x = 0; x < viewport.Width; x++)
			{
				WORD wData = pWord[ y * viewport.Width + x ]; 
				// 16 bit color
				BYTE byData1 = ( wData & 0xf800 ) >> 8;
				BYTE byData2 = ( wData & 0x07e0 ) >> 3;
				BYTE byData3 = ( wData & 0x001f ) << 3 ;
				DWORD dwOffset = ( y * viewport.Width + x ) * 3;
				lpData[ dwOffset + 0 ] = byData3;
				lpData[ dwOffset + 1 ] = byData2;
				lpData[ dwOffset + 2 ] = byData1;
			}
		}
		pDestSurface->UnlockRect();

		TCHAR szFileName[ 128 ];
		_stprintf( szFileName, _T( "Capture\\%s%06d.bmp" ), m_szCaptureFileName, m_nCaptureCnt );
//	SaveBMP( szFileName, (LPBYTE)lockedRect.pBits, CSize( viewport.Width, viewport.Height ) );
		delete lpData;
		m_nCaptureCnt++;

	}
	pDestSurface->Release();

	return S_OK;
}
*/

// 
// 화면을 랜더링 한다. (출력)
//
#ifdef __VCINEMASCOPE
int m_nCinemaScopeCnt = -100;
#endif
HRESULT CNeuzApp::Render()
{
	_PROFILE("CNeuzApp::Render()");

	if(	m_bActive == FALSE )
		return S_FALSE;
	// Clear the viewport
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, CWndBase::m_Theme.m_d3dcBackground, 1.0f, 0 ) ;
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	
	if (pWndWorld) pWndWorld->m_flyTarget.Clear();		// 비행때 공격가능한 타겟들 리스트 지워줌.	

	//pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 90, 146, 222 ), 1.0f, 0 ) ;
	//m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 255, 255, 255 ), 1.0f, 0 ) ;
	//m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
	// Begin the scene
	
	if( g_Option.m_nShadow < 2 )		// 높음/중간 만 쉐도우 맵을 만든다.
	{
		_PROFILE("Make Shadow Map");
		CHECK1();
		void RenderShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, std::span<CObj *> pList );
		if( g_pPlayer )
		{
			CWorld *pWorld = g_pPlayer->GetWorld();
			if( pWorld )
			{

		if( pWorld->GetID() != WI_WORLD_MINIROOM ) // 7.28기획요청 : 하우징 그림자 제거
			RenderShadowMap( m_pd3dDevice, pWorld->m_objCull );

			}

		}
		CHECK2( " Render ShadowMap" );
	}

	// 뽀샤시 옵션이 켜져있으면 렌더타겟에 게임화면을 렌더함.
	if( g_Option.m_nBloom )
	{
		_PROFILE("Make Bloom");
		CWorld* pWorld = g_WorldMng.Get();
		if( pWorld && g_pPlayer && g_Glare.m_bActive )
		{
			pWorld->Projection( m_pd3dDevice, g_Glare.m_Src.m_nWidth, g_Glare.m_Src.m_nHeight );
			pWorld->SetCamera( &m_camera );
			g_Glare.m_Src.BeginScene();
			DWORD dwColor = CWorld::GetDiffuseColor();
			m_2DRender.m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;
			pWorld->RenderBase( m_pd3dDevice, g_WndMng.m_Theme.m_pFontWorld );		// 지형과 오브젝트를 미리 렌더링 해둠.
			g_Glare.m_Src.EndScene();

			g_Glare.Blur( m_pd3dDevice );		// m_Src를 m_Surface[0]에 블러함.		
		}
	}

	CHECK1();
	if(SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		_PROFILE("BeginScene");

		CHECK2( "Begin" );
		g_nRenderCnt ++;

		CHECK1();

		CWorldMap* pWorldMap = CWorldMap::GetInstance();
		g_WndMng.PaintRoot(&m_2DRender);		
		
#ifdef __VCINEMASCOPE
		CRect rect = g_WndMng.GetWndRect();

		CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG_EX );
//		m_nCinemaScopeCnt = 0;
		if( pWndDialog )
		{
			m_2DRender.RenderFillRect( CRect( 0, 0   + m_nCinemaScopeCnt, rect.Width(), 100  + m_nCinemaScopeCnt ), 0xff000000 );
			m_2DRender.RenderFillRect( CRect( 0, 100 + m_nCinemaScopeCnt, rect.Width(), 150 + m_nCinemaScopeCnt ), 0xff000000, 0xff000000, 0x00000000, 0x00000000 );

			m_2DRender.RenderFillRect( CRect( 0, rect.Height() - 100 - m_nCinemaScopeCnt, rect.Width(), rect.Height()      - m_nCinemaScopeCnt ), 0xff000000 );
			m_2DRender.RenderFillRect( CRect( 0, rect.Height() - 150 - m_nCinemaScopeCnt, rect.Width(), rect.Height() - 100 - m_nCinemaScopeCnt ), 0x00000000, 0x00000000, 0xff000000, 0xff000000 );

		}
#endif
		CHECK2( "Render WndMng" );
		
		if( g_WndMng.m_pWndWorld )
			g_WndMng.m_pWndWorld->RenderFocusArrow( CPoint(0,0) );

		// 드래그 아이템 
		if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		{
			GET_CLIENT_POINT( GetSafeHwnd(), point );
			if( CWndBase::m_GlobalShortcut.m_pTexture )
				CWndBase::m_GlobalShortcut.m_pTexture->Render( &m_2DRender, CPoint( point.x - 16, point.y - 16 ) );
			else
			{
				int nHeight = m_2DRender.m_pFont->GetMaxHeight() / 2;
				m_2DRender.TextOut( point.x - nHeight , point.y - nHeight, CWndBase::m_GlobalShortcut.m_szString );
			}
		}

		// 툴립 관련
		g_toolTip.Paint(&m_2DRender);
		g_toolTipSub1.Paint( &m_2DRender );
		g_toolTipSub2.Paint( &m_2DRender );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_2DRender.SetFont( 	CWndBase::m_Theme.m_pFontWorld );
		g_ClientMsg.Render( CPoint( m_2DRender.m_clipRect.Width() / 2 , 520 * m_2DRender.m_clipRect.Height() / 768 ), &m_2DRender );
		m_2DRender.SetFont( 	CWndBase::m_Theme.m_pFontText );
#ifdef __PROF
//		CHECK1();
//		m_2DRender.TextOut( 600, 120, g_Prof.m_szResult, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
//		CHECK2( "Prof Text" );
#endif		

		// 맵이동간에 이미지 로딩출력
		if( m_bTexLoad && m_bTexLoadAlpha )
		{
			m_nTexAlpha	= (int)( m_nTexAlpha - 6.0f );

			if( m_nTexAlpha < 0 )
			{
				m_nTexAlpha	   = 0;
				m_bTexLoad     = FALSE;
				m_bTexLoadAlpha  = FALSE;
				
				m_TexLoading.DeleteDeviceObjects();	
				
			}
			else
			{
				int wideOffsetX = 0;
				
				if(g_Option.m_nResWidth == 1280 && (g_Option.m_nResHeight == 720 || g_Option.m_nResHeight == 768 || g_Option.m_nResHeight == 800)) //Wide Offset
				{
					m_TexLoading.m_size = CSize(960, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 960) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else if(g_Option.m_nResWidth == 1360 && g_Option.m_nResHeight == 768)
				{
					m_TexLoading.m_size = CSize(1024, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 1024) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else if(g_Option.m_nResWidth == 1440 && g_Option.m_nResHeight == 900)
				{
					m_TexLoading.m_size = CSize(1200, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 1200) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else if(g_Option.m_nResWidth == 1680 && g_Option.m_nResHeight == 1050)
				{
					m_TexLoading.m_size = CSize(1400, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 1400) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else
					m_TexLoading.m_size = CSize(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
				
				m_2DRender.RenderTexture( CPoint(0 + wideOffsetX, 0), &m_TexLoading, m_nTexAlpha );		
			}			
		}

		pWorldMap->RenderWorldMap( &m_2DRender );

#ifdef __GAME_GRADE_SYSTEM
#ifdef __CLIENT
		if( ::GetLanguage() == LANG_KOR )
		{
			g_WndMng.m_pTheme->RenderGameGradeMark( &m_2DRender, static_cast< DWORD >( m_nGameGradeMarkAlpha ) );
		}
#endif // __CLIENT
#endif // __GAME_GRADE_SYSTEM
		
		CHECK1();
		m_pd3dDevice->EndScene();
		CHECK2( "End" );
	}
	
	return S_OK;
}

// 플레이 가능 시간 세팅 
void CNeuzApp::SetLeftTime( long lTimeLeft )
{
	m_dwTimeLeft = lTimeLeft * 1000;
	m_dwCurTick = timeGetTime();
}

void CNeuzApp::NotifyLeftMinute( std::optional<int> nMin )
{
	if( ::GetLanguage() == LANG_THA )
	{
		if (nMin == m_nLeftTimeNotifyStatus )
			return;

		m_nLeftTimeNotifyStatus = nMin;

		if (nMin) {
			char szBuffer[256];
			sprintf(szBuffer, prj.GetText(TID_GAME_REMAINPLAYTIMEMIN), *nMin );
			g_WndMng.OpenMessageBox(szBuffer);
		}
	}
}

// 태국의 경우 플레이 가능 시간을 표시해 준다.
void CNeuzApp::NotifyLeftTime()
{
	if( ::GetLanguage() != LANG_THA )
		return; 

	if( m_dwTimeLeft == 0 )
		return;

	DWORD dw = timeGetTime() - m_dwCurTick;
	if( dw > 1000 ) 
	{
		m_dwCurTick = timeGetTime();

		if( m_dwTimeLeft >= dw )
			m_dwTimeLeft -= dw;
		else
		{
			m_dwTimeLeft = 0;								// 0초는 표시하지 않는다.
			return;
		}

		long lLeftTime = ( m_dwTimeLeft + 500 ) / 1000;		// + 500반올림 
		if( lLeftTime <= 10 )			// 10초 이하 
		{
			char szBuffer[256];
			sprintf( szBuffer, prj.GetText(TID_GAME_REMAINPLAYTIMESEC), lLeftTime );			
			g_WndMng.PutString( szBuffer );
		}
		else if( lLeftTime <= 60 * 15 )	// 15분 이하 
		{
			switch( lLeftTime )
			{
			case 60 * 1:
				NotifyLeftMinute( 1 );
				break;
			case 60 * 2:
				NotifyLeftMinute( 2 );
				break;
			case 60 * 3:
				NotifyLeftMinute( 3 );
				break;
			case 60 * 4:
				NotifyLeftMinute( 4 );
				break;
			case 60 * 5:
				NotifyLeftMinute( 5 );
				break;
			case 60 * 10:
				NotifyLeftMinute( 10 );
				break;
			case 60 * 15:
				NotifyLeftMinute( 15 );
				break;
			}
		}
	}
}

extern int g_nOldVirtKey;

//
// Frame을 움직인다. 기존의 Process와 같은 역할 
//
HRESULT CNeuzApp::FrameMove()
{

#ifdef __VCINEMASCOPE
	CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG_EX );
	if( pWndDialog )
	{
		if( m_nCinemaScopeCnt < 0 )
			m_nCinemaScopeCnt += 2;
	}
	else
		m_nCinemaScopeCnt = -150;
#endif

	_PROFILE("CNeuzApp::FrameMove()");

	g_tmCurrent = timeGetTime();		// 전역으로 사용할 현재 시간.
	g_tCurrent	= time( NULL );
	g_nProcessCnt ++;
	if( g_GameTimer.m_nHour >= 21 || g_GameTimer.m_nHour <= 6 )
		::SetNight( TRUE );
	else
		::SetNight( FALSE );

	NotifyLeftTime();

	CHECK1();
	g_DPlay.ReceiveMessage();
	g_DPlay.SendSnapshot();

	g_DPlay.FlushPlayerAngle();

	g_dpLoginClient.ReceiveMessage();
	g_dpCertified.ReceiveMessage();
	CHECK2( "Receive" );

	if( m_dwTempMessage )
	{
		CWndBase* pWndBase;
		if( m_timerConnect.TimeOut() )
		{
			m_dwTempMessage = 0;
			pWndBase = g_WndMng.GetWndBase( APP_SELECT_CHAR );
			if( pWndBase )
			{
				((CWndSelectChar*)pWndBase)->Connected();
				goto NEXT;
			}
		}
	}

	if( g_pBipedMesh )
		g_pBipedMesh->FrameMove();

	g_WndMng.Process(); // 실제적인 월드와 캐릭터 컨트롤 인터페이스는 CWndWorld::Process가 처리함 
	
	// 위치 바꾸지 마시오!! ata3k
	// 키보드 업했을때 반응하기 위해 클리어 시켜준다....
	if( g_nOldVirtKey != 0 )
		g_bKeyTable[ g_nOldVirtKey ] = FALSE;	

NEXT:
	if( g_WndMng.IsTitle() == TRUE )
		CWndBase::m_Theme.FrameMove();
	GET_CLIENT_POINT( GetSafeHwnd(), point );
	g_toolTip.Process( point, &m_2DRender );
	g_toolTipSub1.Process( point, &m_2DRender );
	g_toolTipSub2.Process( point, &m_2DRender );
	g_DamageNumMng.Process();
#ifdef __FLYFF_INITPAGE_EXT
	if(CWndBase::m_Theme.m_bRenderTitleWorld)
	{
		g_GameTimer.m_bFixed = TRUE;
		g_GameTimer.m_nHour = 10;
	}
	else
		g_GameTimer.Compute();
#else //__FLYFF_INITPAGE_EXT
	g_GameTimer.Compute();
#endif //__FLYFF_INITPAGE_EXT
	ProcessFadeMusic();
	CalcUPS();
	g_Object3DMng.Process();

	g_ParticleMng.Process();
	g_TailEffectMng.Process();

#ifdef __NEW_WEB_BOX
	if( g_WndMng.GetWndBase( APP_WEBBOX ) )
#else // __NEW_WEB_BOX
	if( g_WndMng.GetWndBase(APP_WEBBOX) || g_WndMng.GetWndBase(APP_WEBBOX2)  )
#endif // __NEW_WEB_BOX
	{
		if( CWebBox::GetInstance()->Get_Start() == false )
			CWebBox::GetInstance()->Set_Start();

		CWebBox::GetInstance()->Process( GetSafeHwnd(), m_hInstance
#ifdef __GPAUTH_01
			, m_bGPotatoAuth? m_szGPotatoNo: m_szAccount
#else	// __GPAUTH_01
			, m_szAccount
#endif	// __GPAUTH_01
			, g_pPlayer ? g_pPlayer->m_idPlayer: 0
			, m_dwSys
			, g_pPlayer ? (int)g_pPlayer->GetLevel(): 0
			, g_pPlayer ? (int)g_pPlayer->GetJob(): 0
			, g_pPlayer ? (int)g_pPlayer->GetSex(): 0
			, g_pPlayer ? g_pPlayer->GetName(): ""
			);
	}
	
	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->Process();

#ifdef __GAME_GRADE_SYSTEM
#ifdef __CLIENT
	if( ::GetLanguage() == LANG_KOR )
	{
		CalculateGameGradeMarkAlpha();
	}
#endif // __CLIENT
#endif // __GAME_GRADE_SYSTEM
	
	if( m_bQuit )
	{
		if( g_pPlayer == NULL || m_timerQuit.IsTimeOut() )
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0 );
	}
	return S_OK;
}


void CNeuzApp::HashMD5(char* szResult, LPCTSTR szPWD)
{
	char szBuffer[256];
	const char* szKey = "kikugalanet";
	sprintf(szBuffer, "%s%s", szKey, szPWD);
	md5(szResult, szBuffer);
}


LPCTSTR CNeuzApp::GetDeleteKey( LPCTSTR szNo )
{
	static char szKey[512];
	HashMD5(szKey, szNo);
	return szKey;
}

#ifdef __TWN_LOGIN0816
void CNeuzApp::SetAccountInfo( LPCTSTR lpszAccount, LPCTSTR lpszpw, LPCTSTR lpszSessionPwd )
#else	// __TWN_LOGIN0816
void CNeuzApp::SetAccountInfo( LPCTSTR lpszAccount, LPCTSTR lpszpw )
#endif	// __TWN_LOGIN0816
{
	CString strAccount	= lpszAccount;
	strAccount.Trim();	
	_tcscpy( g_Neuz.m_szAccount, (LPCTSTR)strAccount );
	_tcslwr( g_Neuz.m_szAccount );

#ifdef __TWN_LOGIN0816
	CString strSessionPwd	= lpszSessionPwd;
	strSessionPwd.Trim();	
	_tcscpy( g_Neuz.m_szSessionPwd, (LPCTSTR)strSessionPwd );
#endif	// __TWN_LOGIN0816

	if( m_bEncryptPWD )
		HashMD5( g_Neuz.m_szPassword, lpszpw );
	else
		_tcscpy( g_Neuz.m_szPassword, lpszpw );
}


int g_nOldVirtKey = 0;

// 특정키들은 눌렀을때 반응하도록 한다...
// 나머지 키들은 업했을때 반응
static BOOL IsPushedKey( int nVirtKey )
{
	if( nVirtKey == g_Neuz.Key.chUp || nVirtKey == g_Neuz.Key.chLeft || nVirtKey == 'S' || nVirtKey == 'D' || nVirtKey == VK_SPACE ||
//		nVirtKey == 'Q' || nVirtKey == 'E' ||
		nVirtKey == VK_LEFT || nVirtKey == VK_RIGHT || nVirtKey == VK_UP || nVirtKey == VK_DOWN || 
		nVirtKey == VK_PRIOR || nVirtKey == VK_NEXT )
		return TRUE;

	return FALSE;
}

LRESULT CNeuzApp::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if (msg == WM_SETTEXT || msg == 174) {
		// Shortcutting the WM_SETTEXT to avoid sending messages to window,
		// which may make CWndMgr free some windows even if we are currenly
		// in a MsgProc call / processing the CWndBases.
		// 
		// 174 is a submessage sent by WM_SETTEXT that I could not find
		// anywhere in any documentation. It seems to be the message that
		// will make the default windows proc actually change the title of
		// the window.

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	BOOL bHook = GuildDeploy()->MsgProc( hWnd, msg, wParam, lParam );
	if( bHook )
		return 0;

	CWndEdit* pWndEdit = CWndEdit::GetWndEditCtrl();
	if( pWndEdit )
	{
		if( !( ( msg == WM_KEYDOWN || msg == WM_KEYUP ) && (wParam == VK_ESCAPE || wParam == VK_F1 || wParam == VK_F2 || wParam == VK_F3 ||
			wParam == VK_F4 || wParam == VK_F5 || wParam == VK_F6 || wParam == VK_F7 || wParam == VK_F8 || wParam == VK_F9) ) )
		{
			if( pWndEdit->IsYouMessage( msg, wParam, lParam ) )	
				return 0;
			if( ( msg == WM_KEYDOWN || msg == WM_KEYUP ) && wParam != VK_RETURN )
				return 0;
		}
	}
	else
	{
		if( msg >= WM_IME_SETCONTEXT && msg <= WM_IME_KEYUP ) // 모든 IME 매시지는 스킵 
			return 0;

		if( CWndEdit::m_hIMCBackup == NULL )
		{
			CWndEdit::m_hIMCBackup = ::ImmAssociateContext( CWndEdit::m_hWnd, NULL );
		}
		CWndEdit::SetAlphaNumeric();
	}

	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	if( WM_LBUTTONDOWN == msg && pWorldMap && pWorldMap->IsRender( ) )	
		pWorldMap->OnLButtonDown( );
	else
		g_WndMng.WindowRootProc( msg, wParam, lParam );

	//gmpbigsun(100308): here is stop if activated modal window
	if( CWndBase::IsOpenModalWnd() && msg != WM_LOGOUT )
	{
		switch( msg )
		{
		case WM_KEYDOWN:
			{
				if( g_WndMng.m_pLogOutWaitting )
					break;

				if( KillWindow( wParam ) )
					break;

				CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
				int nVirtKey = ( int )wParam;
				if( pWndChat )
				{
					switch( nVirtKey )
					{
					case VK_RETURN:
						if( pWndEdit == NULL )
						{
							pWndChat->AddWndStyle( WBS_MODAL );
							pOldModalWindow = g_WndMng.m_pWndFocus;

							pWndChat->SetFocus();
							CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;
							pWndEdit->SetFocus();

							break;
						}
					}
				}

				break;
			}
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
		case WM_CLOSE:
			{
				prj.m_MapInformationManager.DeleteAllMapInformation();
				g_Neuz.m_bQuit = TRUE;
				if( wParam == FALSE )
				{
					if( g_pPlayer )
					{
						g_WndMng.m_bConnect = FALSE;
						StopMusic();
						return 1;
					}
				}
				break;
			}
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM
		}
		return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
	}

	switch( msg )
	{
	case WM_KEYDOWN:
		{
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)g_WndMng.GetWndBase( APP_GUIDE );
			if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.nInputKey = (UINT)wParam;
			if( g_WndMng.m_pLogOutWaitting ) // 종료중이면 키보드 입력 불가
				break;
			
			if( KillWindow( wParam ) )
				break;

			CWndChat* pWndChat = (CWndChat*) g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
			int nVirtKey = (int) wParam;    // virtual-key code
			int lKeyData = lParam;
			if( pWndChat ) 
			{
				switch(nVirtKey)
				{
				case VK_RETURN:
					if( pWndChat && pWndEdit == NULL )
					{
						pWndChat->SetFocus();
						CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;
						pWndEdit->SetFocus();
					}
					break;
				}
			}
			if(nVirtKey == VK_ESCAPE)
			{
				if(CDeployManager::GetInstance()->IsReady())
				{
					CDeployManager::GetInstance()->EndDeploy();
					return FALSE;
				}

				GuildHouse->m_dwSelectedObjID = NULL_ID;		//최우선순위이므로 걍 NULL_ID로 ..
			}
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
			if( pWndWorld && !pWndWorld->IsFocusWnd() )
				pWndWorld->OnKeyDown( nVirtKey, 0, 0 );
			
			g_bKeyTable[ nVirtKey ] = TRUE;

#ifdef __BS_CONSOLE
			if( nVirtKey == VK_OEM_3 )
			{
				gConsole()->Show( );			
			}
#endif 

		}
		break;
	case WM_KEYUP:
		{
			int nVirtKey = (int) wParam;    // virtual-key code
			int lKeyData = lParam;
			for( int i = 0; i < MAX_SLOT_ITEM; ++i )
			{
				if( g_bSlotSwitchAboutEquipItem[ i ] == TRUE )
					g_bSlotSwitchAboutEquipItem[ i ] = FALSE;
			}
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
			if( pWndWorld && !pWndWorld->IsFocusWnd() )
				pWndWorld->OnKeyUp( nVirtKey, 0, 0 );
			if(g_bKeyTable[VK_NEXT])
			{
				if(CDeployManager::GetInstance()->IsReady())
				{
					CDeployManager::GetInstance()->m_fAngle += 45.0f;
					if(CDeployManager::GetInstance()->m_fAngle > 359.0f) CDeployManager::GetInstance()->m_fAngle = 0.0f;
					CDeployManager::GetInstance()->m_pTargetObj->SetAngle(CDeployManager::GetInstance()->m_fAngle);
					CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
				}
			}
			else if(g_bKeyTable[VK_PRIOR])
			{
				if(CDeployManager::GetInstance()->IsReady())
				{
					CDeployManager::GetInstance()->m_fAngle -= 45.0f;
					if(CDeployManager::GetInstance()->m_fAngle < -359.0f) CDeployManager::GetInstance()->m_fAngle = 0.0f;
					CDeployManager::GetInstance()->m_pTargetObj->SetAngle(CDeployManager::GetInstance()->m_fAngle);
					CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
				}
			}
			if( g_bKeyTable[ VK_MULTIPLY ] ) //VK_MULTIPLY 임시로....
			{
				if(!g_WndMng.m_clearFlag)
					g_WndMng.ClearAllWnd();
				else
					g_WndMng.RestoreWnd();
			}
			
			g_bKeyTable[ nVirtKey ] = FALSE;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		if(CDeployManager::GetInstance()->IsReady() && msg == WM_MOUSEWHEEL)
		{
			int nDelta = (short)HIWORD(wParam);
			if(nDelta < 0)
			{
				CDeployManager::GetInstance()->m_pTargetObj->AddAngle(2.5f);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
			else
			{
				CDeployManager::GetInstance()->m_pTargetObj->AddAngle(-2.5f);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
		}
		break;
	case WM_CLOSE:
		{
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
			prj.m_MapInformationManager.DeleteAllMapInformation();
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM
			g_Neuz.m_bQuit = TRUE;
			if( wParam == FALSE )
			{
				if( g_pPlayer )
				{
					g_WndMng.m_bConnect = FALSE;
					StopMusic();
					return 1;
				}
			}
		}
		break;
	case WM_LOGOUT:
		{
			g_WndMng.OpenTitle();
			PlayMusic( BGM_TITLE, 0 );
			g_WndMng.m_bConnect = FALSE;
			return 1;
		}
		break;
		
	case WM_MOVE:
		{
			CWebBox* pWebBox	= CWebBox::GetInstance();
			if( pWebBox && pWebBox->m_hDlg )
				::SendMessage( pWebBox->m_hDlg, WM_INITDIALOG, 0, 0 );
		}
		break;

	case WM_COMMAND:
		break;
	case WM_ACTIVATE:
		m_bActiveNeuz = TRUE;
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			m_bActiveNeuz = FALSE;
		}
		break;
	}
	return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}

bool CNeuzApp::KillWindow(int wParam) {
	if (CWndStateConfirm * pWndStateConfirm = g_WndMng.GetWndBase<CWndStateConfirm>(APP_STATE_CONFIRM)) {
		if (wParam == VK_RETURN) {
			pWndStateConfirm->SendYes();
		} else if (wParam == VK_ESCAPE) {
			pWndStateConfirm->CloseTheWindow();
		}

		return true;
	}

	return false;
}

void __cdecl LoadProperty( void* p )
{ 
	try {
		prj.OpenProject("Masquerade.prj");
	} catch (ProjectLoadError & e) {
		Error("OpenProject ERROR: %s", e.what());
		throw e;
	}
	CNeuzApp* pApp = (CNeuzApp*) p;
	pApp->m_hThread = INVALID_HANDLE_VALUE;
}

void CNeuzApp::WaitLoading()
{
	OutputDebugString("WaitLoading start\n");
	if( m_hThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hThread, INFINITE );
		m_hThread = INVALID_HANDLE_VALUE;
	}
	OutputDebugString("WaitLoading end\n");
}

void CNeuzApp::BeginLoadThread()
{
	CResFile::ScanResource( "" );

	prj.LoadPreFiles();

	m_hThread = (HANDLE) _beginthread( LoadProperty, 0, (LPVOID)this );
#if !defined(_DEBUG)
	WaitLoading();
#endif
}

//
// 디바이스와 비독립적인 매쉬, 폴리곤 등을 처음 초기화 하는 곳 
//
HRESULT CNeuzApp::InitDeviceObjects()
{
	BeginLoadThread();

	SetMouseCursor( CUR_DELAY ); 

	g_MusicMng.LoadScript( MakePath( DIR_MUSIC, _T( "default.bgm" ) ) );
	g_SoundMng.LoadScript( MakePath( DIR_CLIENT, _T( "sound.inc" ) ) );

#ifndef _DEBUG
	{
		CString strError;
		int nMem = m_pd3dDevice->GetAvailableTextureMem();
		int nVersion = LOWORD(m_d3dSettings.pWindowed_AdapterInfo->AdapterIdentifier.DriverVersion.u.LowPart);
		strError.Format( "%s, %d(%d)", m_d3dSettings.pWindowed_AdapterInfo->AdapterIdentifier.Description, nMem, nVersion );
	}
#endif 

#ifdef __YENV
	if( m_d3dCaps.PixelShaderVersion < D3DPS_VERSION(1,1) )
		g_dwUsablePS_Level = 0;
	else if( m_d3dCaps.PixelShaderVersion < D3DPS_VERSION(2,0) )
		g_dwUsablePS_Level = 1;
	else 
		g_dwUsablePS_Level = 2;
#endif //__YENV

	if( m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
	{
		MessageBox( GetSafeHwnd(), "This game does not work on computer that only supports square texture", "Guide", MB_OK);
		throw std::exception("m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY is true");
	}

	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	prj.m_modelMng.InitDeviceObjects( );
	prj.m_terrainMng.InitDeviceObjects( m_pd3dDevice );

	CWorld::StaticInitDeviceObjects( m_pd3dDevice );
	
	g_DamageNumMng.LoadTexture();		// 데미지 이펙트 

	g_SfxMng.InitDeviceObjects(m_pd3dDevice);
	g_SfxMeshMng.InitDeviceObjects(m_pd3dDevice);

	g_Shadow.InitDeviceObjects( );
	g_Shadow.LoadElement( "Shadow.o3d" );

	g_ParticleMng.InitDeviceObjects( m_pd3dDevice );

	m_TextureGauge[0].LoadTexture( MakePath( DIR_THEME, "GauMagChargeBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureGauge[1].LoadTexture( MakePath( DIR_THEME, "GauMagChargeHead.TGA" ), 0xffff00ff, TRUE );
	m_TextureGauge[2].LoadTexture( MakePath( DIR_THEME, "GauMagChargeBody.TGA" ), 0xffff00ff, TRUE );
	m_TextureGauge[3].LoadTexture( MakePath( DIR_THEME, "GauMagChargeBody2.tga" ), 0xffff00ff, TRUE );
	m_TextureGauge[4].LoadTexture( MakePath( DIR_THEME, "GauMagChargeTail.TGA" ), 0xffff00ff, TRUE );
	
	m_TextureHPGauge[0].LoadTexture( MakePath( DIR_THEME, "GauPartyHpBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureHPGauge[1].LoadTexture( MakePath( DIR_THEME, "GauPartyHp.TGA" ), 0xffff00ff, TRUE );
	m_TextureHPGauge[2].LoadTexture( MakePath( DIR_THEME, "GauPartyHp_Dbf.TGA" ), 0xffff00ff, TRUE );
	m_TextureTurboGauge[0].LoadTexture( MakePath( DIR_THEME, "GauAccelBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureTurboGauge[1].LoadTexture( MakePath( DIR_THEME, "GauAccelInner.TGA" ), 0xffff00ff, TRUE );
	m_TextureCastingGauge[0].LoadTexture( MakePath( DIR_THEME, "GauAccelBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureCastingGauge[1].LoadTexture( MakePath( DIR_THEME, "GauAccelInner.TGA" ), 0xffff00ff, TRUE );

	m_pMasterIcon[0] = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, "icon_Expert1.dds"), 0xffff00ff );
	m_pMasterIcon[1] = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, "icon_Expert2.dds"), 0xffff00ff );
	m_pMasterIcon[2] = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, "icon_Expert3.dds"), 0xffff00ff );
	m_pMasterIcon[3] = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, "icon_Expert4.dds"), 0xffff00ff );
	m_pMasterIcon[4] = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, "icon_Expert5.dds"), 0xffff00ff );
	m_pMasterIcon[5] = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, "icon_Expert6.dds"), 0xffff00ff );
	m_pHeroIcon		 = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, "icon_Hero.dds"), 0xffff00ff );

	m_TexCltGauge[0].LoadTexture( MakePath( DIR_THEME, "GauAccelBG.TGA" ), 0xffff00ff, TRUE );
	m_TexCltGauge[1].LoadTexture( MakePath( DIR_THEME, "GauAccelInner.TGA" ), 0xffff00ff, TRUE );
	//m_TexCltGauge[2].LoadTexture( MakePath( DIR_THEME, "GauPartyHp_Dbf.TGA" ), 0xffff00ff, TRUE );
	
	m_TextureAngelGauge[0].LoadTexture( MakePath( DIR_THEME, "GauAngelExpBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureAngelGauge[1].LoadTexture( MakePath( DIR_THEME, "GauAngelExp.TGA" ), 0xffff00ff, TRUE );

	m_dwPVPTime = GetTickCount();
	m_nTexAlpha		= 255;
	m_bTexLoad      = FALSE;
	m_bTexLoadAlpha = FALSE;

	g_toolTip.InitTexture();
	g_toolTipSub1.InitTexture();
	g_toolTipSub2.InitTexture();
	
#ifdef __YENV
	HRESULT	hr;
	LPD3DXBUFFER	pErr = NULL;

#ifdef _DEBUG
	if( FAILED(hr = D3DXCreateEffectFromFile( m_pd3dDevice, "hlsl.fx", NULL, NULL, 0, NULL, &m_pEffect, &pErr )))
#else //_DEBUG
	if( FAILED(hr = D3DXCreateEffectFromResource( D3DDEVICE, NULL, MAKEINTRESOURCE(IDR_DATA3), NULL, NULL, 0, NULL, &m_pEffect, &pErr )))
#endif //_DEBUG
	{
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
	}
	else
	{
		m_hTechnique	= m_pEffect->GetTechniqueByName( "TShader" );
		m_hmWVP			= m_pEffect->GetParameterByName( NULL, "mWVP" );
		m_hTexDiffuse   = m_pEffect->GetParameterByName( NULL, "Tex_Diffuse" );
		m_hTexBump      = m_pEffect->GetParameterByName( NULL, "Tex_Bump" );
		m_hTexSpecular  = m_pEffect->GetParameterByName( NULL, "Tex_Specular" );
		m_hvLightDir	= m_pEffect->GetParameterByName( NULL, "vLightDir" );
		m_hvEyePos		= m_pEffect->GetParameterByName( NULL, "vEyePos" );	
		m_hvFog			= m_pEffect->GetParameterByName( NULL, "vFog" );	
		m_hvDiffuse		= m_pEffect->GetParameterByName( NULL, "I_d" );	
		m_hvAmbient		= m_pEffect->GetParameterByName( NULL, "I_a" );			
		m_hvSpecual		= m_pEffect->GetParameterByName( NULL, "I_s" );			
	}
	SAFE_RELEASE( pErr );

	if( g_dwUsablePS_Level < 2 )
		g_Option.m_bSpecBump = FALSE;

	m_pEnvTex = NULL;
#endif //__YENV	

	// 테마 세팅 
	CWndBase::m_Theme.LoadTheme( "Default.thm" );
	CWndBase::m_Theme.InitDeviceObjects( );

	// CWndBase 세팅 
	g_WndMng.m_hasApp = true;
	g_WndMng.m_pTheme = &CWndBase::m_Theme;
	g_WndMng.m_pFont = CWndBase::m_Theme.m_pFontText;
	g_WndMng.Initialize();
	g_WndMng.InitDeviceObjects();
	CWndBase::m_hWnd = GetSafeHwnd();
	CWndBase::SetForbidTexture( MakePath( DIR_ICON, "icon_forbid.dds" ) );

	// 다이얼로그 메시지 세팅 
	g_DialogMsg.InitDeviceObjects();

	// 2DRender 세팅 
	m_2DRender.InitDeviceObjects( m_pd3dDevice );
	m_2DRender.m_pFont = CWndBase::m_Theme.m_pFontText;
	g_WndMng.OpenTitle( TRUE );

	InitFST();		// init frame skipping time
#ifdef	__PROF
	SetFrameSkip( FALSE );
#endif

	m_texQuestEmoticon.LoadScript( MakePath( DIR_ICON, "icon_QuestEmoticon.inc" ) );
	m_TexturePackPVP.LoadScript( MakePath( DIR_SFX, "CountFight.inc" ) );

	TexturePool::Get()->Init( m_pd3dDevice );

	PlayMusic( BGM_TITLE, 0 );

	return S_OK;
}

void CNeuzApp::ResetStaticValues( )
{
	GuildHouse->ResetValues( );
}

void CNeuzApp::Drv_SetGamma(HWND hWnd, float gamma, int overbright, float fContrast)
{
    unsigned short ramp[3*256];
    float f;
	   
    GetDeviceGammaRamp( GetDC(hWnd), ramp );
	   
    for ( int i = 0; i < 256; i++ )
	   {
        f = (float)(255 * pow((float)i/256, 1/gamma));
        f = f * fContrast + overbright;
        if (f < 0)
            f = 0;
        if (f > 255)
            f = 255;
        ramp[i+0] = ramp[i+256] = ramp[i+512] = ((unsigned short)f<<8) ;
    }
	  
		if constexpr (CWndOptVideo::MyServerRecksTheUserScreenParameters) {
			SetDeviceGammaRamp(GetDC(hWnd), ramp);
		}
}
/*
void CNeuzApp::Loop()
{
	   if (GetAsyncKeyState(VK_F5)) 
		   g_fGamma -= 0.1f;
	   if (GetAsyncKeyState(VK_F6)) 
		   g_fGamma += 0.1f;
	   if (GetAsyncKeyState(VK_F7)) 
		   g_nOverBright -= 1;
	   if (GetAsyncKeyState(VK_F8)) 
		   g_nOverBright += 1;
	   if (GetAsyncKeyState(VK_F9)) 
		   g_fContrast -= 0.1f;
	   if (GetAsyncKeyState(VK_F10)) 
		   g_fContrast += 0.1f;
	   
	   Drv_SetGamma(GetSafeHwnd(), g_fGamma, g_nOverBright, g_fContrast);
}
*/


#ifdef __GAME_GRADE_SYSTEM
#ifdef __CLIENT
void CNeuzApp::CalculateGameGradeMarkAlpha( void )
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if( pWndWorld == NULL )
	{
		return;
	}

	if( g_Option.m_bGameGradeRendering == TRUE )
	{
		if( g_tmCurrent > m_dwTimeGameGradeMarkRendering )
		{
			m_bGameGradeMarkRenderingSwitch = TRUE;
			m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MINIMUM_ALPHA;
			m_eGameGradeMarkState = FADE_IN;
			m_dwTimeOld = g_tmCurrent;
			m_dwTimeGameGradeMarkRendering = g_tmCurrent + SEC( GAME_GRADE_MARK_RENDERING_INTERVAL_SECOND );
		}
	}

	if( m_bGameGradeMarkRenderingSwitch == TRUE )
	{
		float fDelta = ( static_cast< float >( g_tmCurrent - m_dwTimeOld ) / 1000.0f );
		m_dwTimeOld = g_tmCurrent;

		switch( m_eGameGradeMarkState )
		{
		case FADE_IN:
			{
				m_nGameGradeMarkAlpha += static_cast< int >( GAME_GRADE_MARK_FADING_SPEED * fDelta );
				if( m_nGameGradeMarkAlpha > GAME_GRADE_MARK_MAXIMUM_ALPHA )
				{
					m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MAXIMUM_ALPHA;
					m_dwTimeGameGradeMarkShowing = g_tmCurrent + SEC( GAME_GRADE_MARK_SHOWING_SECOND );
					m_eGameGradeMarkState = SHOW;
				}
				break;
			}
		case SHOW:
			{
				if( g_tmCurrent > m_dwTimeGameGradeMarkShowing )
				{
					m_eGameGradeMarkState = FADE_OUT;
				}
				break;
			}
		case FADE_OUT:
			{
				m_nGameGradeMarkAlpha -= static_cast< int >( GAME_GRADE_MARK_FADING_SPEED * fDelta );
				if( m_nGameGradeMarkAlpha < GAME_GRADE_MARK_MINIMUM_ALPHA )
				{
					m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MINIMUM_ALPHA;
					m_bGameGradeMarkRenderingSwitch = FALSE;
					m_eGameGradeMarkState = FADE_IN;
				}
				break;
			}
		}
	}
}
#endif // __CLIENT
#endif // __GAME_GRADE_SYSTEM


void CNeuzApp::SetCharacterName(const char * name) {
	if (name) {
		const std::string windowName = std::format("{} - {}", m_strWindowTitle, name);
		::SetWindowText(g_Neuz.GetSafeHwnd(), windowName.c_str());
	} else {
		::SetWindowText(g_Neuz.GetSafeHwnd(), m_strWindowTitle);
	}
}

