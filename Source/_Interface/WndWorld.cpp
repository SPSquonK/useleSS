#include "stdafx.h"
#include "defineObj.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineItem.h"
#include "AppDefine.h"

#include "DialogMsg.h"  
#include "ClientMsg.h"  
#include "DPClient.h"
#include "MsgHdr.h"
#include "WndMessenger.h"
#include "WndIndirectTalk.h"
#include "WndPost.h"

#include "guild.h"

#include "WorldMap.h"
#include "Commonctrl.h"

#include "SecretRoom.h"
#include "Vector3Helper.h"

#include "party.h"
#include "eveschool.h"
#include "WndSummonFriend.h"

#include "langman.h"

#include "yUtil.h"
#include "defineskill.h"
#include "playerdata.h"
#include "honor.h"

#ifdef __SYS_ITEMTRANSY
#include "wndchangeface.h"
#endif //__SYS_ITEMTRANSY

#include "PCBang.h"

#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ

#include "GuildHouse.h"
#include "WndGuildHouse.h"
#include "WndCommItem.h"

#include "Campus.h"
#include "CampusHelper.h"

#ifdef __IMPROVE_MAP_SYSTEM
#include "WndMapEx.h"
#endif // __IMPROVE_MAP_SYSTEM

const int MAX_POWER_GAUGE = 1800;
const DWORD POWER_ATK_DELAY = 1800;

#ifndef __HELP_BUG_FIX
//헬프 이중생성 방지
vector<CString> g_vecHelpInsKey;
#endif //__HELP_BUG_FIX

void CCaption::RemoveAll() {
	m_aCaption.clear();
	m_nAlpha = 255;
	m_bEnd = false;
}

void CCaption::Process()
{
	if( m_bEnd )
	{
		if( m_timer.IsTimeOut() )
		{
			m_nAlpha -= 3;
			if (m_nAlpha < 0) {
				m_nAlpha = 0;
			}
		}
		return;
	}
	
	for (CAPTION & caption : m_aCaption) {
		caption.m_fAddScale += 0.002f;
		caption.m_fScale += caption.m_fAddScale;
		if (caption.m_fScale > 1.0f)
			caption.m_fScale = 1.0f;

		if (caption.m_fScale <= 0.1f) break;
	}

	if (!m_aCaption.empty()) {
		if (m_aCaption.back().m_fScale >= 1.0f) {
			m_bEnd = TRUE;
			m_timer.Set(SEC(5));
		}
	}
}
HRESULT CCaption::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
HRESULT CCaption::DeleteDeviceObjects()
{
	RemoveAll();
	return S_OK;
}
HRESULT CCaption::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CCaption::InvalidateDeviceObjects()
{
	RemoveAll();
	return S_OK;
}
void CCaption::Render( CPoint ptBegin, C2DRender* p2DRender )
{
	if (m_nAlpha == 0) {
		return;
	}

	for (CAPTION & lpCaption : m_aCaption) {
		const CRect rect = p2DRender->m_clipRect;
		CPoint point = CPoint( rect.Width() / 2, 0 );

		const CSize size = lpCaption.m_size;

		const FLOAT fScale = ( 7.0f - lpCaption.m_fScale * 6.0f ); // 최대 7배 사이즈 
		point.x	= (LONG)( point.x - ( ( size.cx / 2 ) * fScale ) );
		point += ptBegin;


		if( ::GetLanguage() == LANG_JAP || g_osVersion <= WINDOWS_ME ) {
			CWndBase::m_Theme.m_pFontCaption->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), fScale, fScale, D3DCOLOR_ARGB(  (int)(lpCaption.m_fScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), lpCaption.m_szCaption );
		} else if( lpCaption.m_texture.m_pTexture ) {
			p2DRender->RenderTexture( point, &lpCaption.m_texture, (int)(lpCaption.m_fScale * 255) - ( 255 - m_nAlpha), fScale, fScale  );
		} else {
			lpCaption.m_pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), fScale, fScale, D3DCOLOR_ARGB(  (int)(lpCaption.m_fScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), lpCaption.m_szCaption );
		}
		
		ptBegin.y += size.cy;

		if (lpCaption.m_fScale <= 0.1f) break;
	}
}

void CCaption::AddCaption( LPCTSTR lpszCaption, CD3DFontAPI* pFont, BOOL bChatLog, DWORD dwColor )
{
	if( m_bEnd && m_timer.IsTimeOut() )
		RemoveAll();
	CSize size;

	if( ::GetLanguage() != LANG_JAP )
	{
		if( g_osVersion <= WINDOWS_ME )
			CWndBase::m_Theme.m_pFontCaption->GetTextExtent( lpszCaption, &size );
		else
			pFont->GetTextExtent( lpszCaption, &size );
	}
	else
		CWndBase::m_Theme.m_pFontCaption->GetTextExtent( lpszCaption, &size );	

	m_nAlpha = 255;

	CAPTION * lpCaption = &m_aCaption.emplace_back();
	strcpy( lpCaption->m_szCaption, lpszCaption );
	lpCaption->m_pFont = pFont;
	lpCaption->m_fScale = 0.0f;
	lpCaption->m_fAddScale = 0.0f;
	lpCaption->m_size = size;
	
	if( bChatLog && g_WndMng.m_pWndChatLog )
	{
		g_WndMng.m_pWndChatLog->PutString( lpszCaption );
	}
	m_bEnd = FALSE;

	if( ::GetLanguage() != LANG_JAP )
	{
		if( g_osVersion <= WINDOWS_ME )
			return;
	}
	else
	{
		return;
	}

	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;

	// 여분을 만들자 
	size.cx += 16 + 64; 
	size.cy += 16;

	D3DVIEWPORT9 viewportOld;
	pd3dDevice->GetViewport(&viewportOld);

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = size.cx;
	viewport.Height = size.cy;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	D3DDeviceCombo* pDeviceCombo = g_Neuz.m_d3dSettings.PDeviceCombo();
	HRESULT hr = g_Neuz.m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      pDeviceCombo->AdapterFormat,
                                      D3DUSAGE_RENDERTARGET,
                                      D3DRTYPE_TEXTURE,
                                      D3DFMT_A8R8G8B8 );
	if( hr == D3D_OK )
	{
		AdjustSize( &size );
		if( lpCaption->m_texture.CreateTexture( pd3dDevice, size.cx, size.cy, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT ) )
		{
			IDirect3DSurface9* pOldSurface;
			IDirect3DSurface9* pOldSurfaceZ;
			IDirect3DSurface9* pDstSurface; 
			pd3dDevice->GetRenderTarget( 0, &pOldSurface );
			pd3dDevice->GetDepthStencilSurface( &pOldSurfaceZ );
			lpCaption->m_texture.m_pTexture->GetSurfaceLevel( 0, &pDstSurface );
			pd3dDevice->SetRenderTarget( 0, pDstSurface );//, pOldSurfaceZ );

			pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000/*CWorld::m_dwBgColor*/, 1.0f, 0);
			if( pd3dDevice->BeginScene() == S_OK )
			{
				lpCaption->m_pFont->DrawText( 8, 8, 1.0f, 1.0f, dwColor, lpCaption->m_szCaption );
				pd3dDevice->EndScene();
			}
			pd3dDevice->SetRenderTarget( 0, pOldSurface );//, pOldSurfaceZ );
			pOldSurface->Release();
			pOldSurfaceZ->Release();
			pDstSurface->Release();
			pd3dDevice->SetViewport(&viewportOld);
		}
	}
}
HRESULT CCapTime::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
HRESULT CCapTime::DeleteDeviceObjects()
{
	m_texture.DeleteDeviceObjects();
	return S_OK;
}
HRESULT CCapTime::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CCapTime::InvalidateDeviceObjects()
{
	m_texture.DeleteDeviceObjects();
	return S_OK;
}

CCapTime::CCapTime()
{
	m_fXScale = 1.0f;
	m_fYScale= 1.0f;
	m_fAddScale = 0;
	m_nAlpha = 255;
	m_nTime = 0;
	m_bRender = FALSE;
	m_nStep = 0;
//	m_size
}
CCapTime::~CCapTime()
{
}
void CCapTime::Render( CPoint ptBegin, C2DRender* p2DRender )
{
	if( m_bRender == FALSE )
		return;

	if( ::GetLanguage() != LANG_JAP )
	{
		if( m_texture.m_pTexture == NULL )
			return;
	}

	// CEditString에서 폰트 class가 다르기 때문에 Init를 할 수 없다. 
	CEditString strTemp;
	CString strTime;
	strTime.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), m_nTime );
	strTemp.AddParsingString( strTime );
	strTime = strTemp;

	//p2DRender->SetFont( lpCaption->m_pFont );
	CRect rect = p2DRender->m_clipRect;//GetWndRect();
	CPoint point = CPoint( rect.Width() / 2, 0 );
	FLOAT fXScale = m_fXScale; // 최대 7배 사이즈 
	FLOAT fYScale = m_fYScale; // 최대 7배 사이즈 
	point.x	= (LONG)( point.x - ( ( m_size.cx / 2 ) * fXScale ) );
	point.y	= (LONG)( point.y - ( ( m_size.cy / 2 ) * fYScale ) );
	point += ptBegin;

	//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	if(  m_nAlpha )
	{
		if( ::GetLanguage() != LANG_JAP )
		{
			if( g_osVersion <= WINDOWS_ME )
				CWndBase::m_Theme.m_pFontCaption->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), fXScale, fYScale, D3DCOLOR_ARGB(  (int)(m_fXScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), strTime );
			else
			{
				if( m_texture.m_pTexture )
					p2DRender->RenderTexture( CPoint( point.x, point.y ), &m_texture, (int) m_nAlpha, fXScale, fYScale  );
				else
				{
					m_pFont->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), fXScale, fYScale, D3DCOLOR_ARGB(  (int)(m_fXScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), strTime );
				}
			}
		}
		else
		{
			CWndBase::m_Theme.m_pFontCaption->DrawText( (FLOAT)( point.x ), (FLOAT)( point.y ), fXScale, fYScale, D3DCOLOR_ARGB(  (int)(m_fXScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), strTime );		
		}
	}
}
void CCapTime::SetTime( int nTime, CD3DFontAPI* pFont )
{
 	m_nTime = nTime;
	CEditString strTemp;
	CString strTime;
	strTime.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), m_nTime );
	strTemp.AddParsingString( strTime );
	strTime = strTemp;
	m_bRender = TRUE;

//	CSize size;
	m_fAddScale = 0.001f;
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_nStep = 0;
	m_nAlpha = 0;

	if( ::GetLanguage() != LANG_JAP )
	{
		if( g_osVersion <= WINDOWS_ME )
			CWndBase::m_Theme.m_pFontCaption->GetTextExtent( strTime, &m_size );
		else
			pFont->GetTextExtent( strTime, &m_size );

		if( g_osVersion <= WINDOWS_ME )
			return;
	}
	else
	{
		CWndBase::m_Theme.m_pFontCaption->GetTextExtent( strTime, &m_size );
		return;	
	}


	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;

	m_pFont = pFont;
	m_size.cx += 16;// + 64; 
	m_size.cy += 16;

	D3DVIEWPORT9 viewportOld;
	pd3dDevice->GetViewport(&viewportOld);

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = m_size.cx;
	viewport.Height = m_size.cy;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	D3DDeviceCombo* pDeviceCombo = g_Neuz.m_d3dSettings.PDeviceCombo();
	HRESULT hr = g_Neuz.m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      pDeviceCombo->AdapterFormat,
                                      D3DUSAGE_RENDERTARGET,
                                      D3DRTYPE_TEXTURE,
                                      D3DFMT_A8R8G8B8 );
	if( hr == D3D_OK )
	{
		CSize size = m_size;
		AdjustSize( &size );
		if( m_texture.CreateTexture( pd3dDevice, size.cx, size.cy, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT ) )
		{
			IDirect3DSurface9* pOldSurface;
			IDirect3DSurface9* pOldSurfaceZ;
			IDirect3DSurface9* pDstSurface; 
			pd3dDevice->GetRenderTarget( 0, &pOldSurface );
			pd3dDevice->GetDepthStencilSurface( &pOldSurfaceZ );
			m_texture.m_pTexture->GetSurfaceLevel( 0, &pDstSurface );
			pd3dDevice->SetRenderTarget( 0, pDstSurface );//, pOldSurfaceZ );

			pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000/*CWorld::m_dwBgColor*/, 1.0f, 0);
			if( pd3dDevice->BeginScene() == S_OK )
			{
				pFont->DrawText( 8, 8, 1.0f, 1.0f, D3DCOLOR_ARGB(  255, 255, 255, 255 ), strTime );
				pd3dDevice->EndScene();
			}
			pd3dDevice->SetRenderTarget( 0, pOldSurface );//, pOldSurfaceZ );
			pOldSurface->Release();
			pOldSurfaceZ->Release();
			pDstSurface->Release();
			pd3dDevice->SetViewport(&viewportOld);
		}
	}
}
void CCapTime::Process()
{

	if( m_nStep == 0 )
	{
		m_nAlpha += 3;
		if( m_nAlpha >= 255 )
		{
			m_nAlpha = 255;
			m_nStep++;
		}
	}
	else
	{	m_fAddScale += 0.001f;
		m_fXScale += m_fAddScale;
		m_fYScale -= m_fAddScale;
		m_nAlpha -= 5;
		if( m_nAlpha < 0 )
			m_nAlpha = 0;
		if( m_fXScale > 3.0f ) 
			m_fXScale = 3.0f; 
		if( m_fYScale < 0.0f ) 
			m_fYScale = 0.0f;
		if( m_fXScale > 0.1f ) 
		{ 
		/*f( i == m_nCount - 1 ) 
			{ 
				m_nCount++; 
				if( m_nCount > m_aCaption.GetSize() ) 
				{
					m_nCount = m_aCaption.GetSize();
					if( m_fYScale >= 1.0f ) 
					{
						m_bEnd = TRUE;
						m_timer.Set( SEC( 3 ) );
					}
				}
				break; 
			}*/
		}	
	}
}
CCapTime g_CapTime;
CCaption g_Caption1;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
D3DXVECTOR3 CWndWorld::m_vTerrainPoint;

#ifdef __YCLOTH
CCloth g_Cloth;
#endif

CWndWorld::CWndWorld()
:
m_buffs( NULL )
{
	m_bFreeMove = FALSE;
	m_bBGM = FALSE;
	SetFullMax( TRUE );
//	m_pWndWorld = this;
	g_WndMng.m_pWndWorld = this;
	m_fHigh = -1.0f;
	m_fLow  = 999999.0f;
	//m_bCameraLock=FALSE;
	//m_pSelectObj = NULL;
	m_bNewSelect = TRUE;
	m_bFlyMove = FALSE;
	//m_pWndDialog = NULL;
	m_nMouseMode = 0;

	s_bUped = s_bDowned = s_bLefted = s_bRighted = FALSE;
	s_bCombatKeyed = s_bFlyKeyed = s_bAccKeyed = FALSE;
	s_bTempKeyed = FALSE;
	m_bTemp2ed = FALSE;
	m_bTemp3ed	= FALSE;
	s_bBoarded = FALSE;
	
	m_bSelectTarget = FALSE;
	m_timerFocusAttack.Set( 500 );

	m_dwIdBgmMusic = 0;
//#if __VER >= 9
//	m_dwIdBgmMusicOld	= 0;
//#endif	//
#ifdef __VRCAMERA
	m_bCameraMode = FALSE;
#endif

	m_pVBGauge = NULL;
	m_vTerrainPoint = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bAutoAttack = FALSE;
	m_dwNextSkill = NEXTSKILL_NONE;

	//m_pCaptureObj = NULL;

	m_objidTracking		= NULL_ID;
	m_dwDropTime	= GetTickCount();
	
	m_vTelePos = D3DXVECTOR3( 0.0f, 0.0f ,0.0f );

	m_nSelect = 0;
	ClearFlyTarget();
	m_fRollAng = 0;
	m_pFontAPICaption = NULL;
	m_pFontAPITitle = NULL;
	m_pFontAPITime = NULL;

	m_bFirstFlying = FALSE;
	m_pBuffTexture.clear();
	m_pBuffTexture.resize(3);

	m_dwOneSecCount = GetTickCount();

	for( int j = 0 ; j < SM_MAX ; ++j )
	{
		m_dwSMItemTexture[j] = NULL;
		m_bSMFlsh[j] = FALSE;
		m_nSMAlpha[j] = 192;
	}

#ifndef __HELP_BUG_FIX
	// 헬프 이중 생성 방지
	g_vecHelpInsKey.clear();
#endif //__HELP_BUG_FIX

	m_pWndGuideSystem = NULL;	
	m_nDubleUp = 0;
	m_timerAutoRunPush.Set( 200 );
	m_timerAutoRun.Set( 200 );
	m_timerAutoRunBlock.Set( 200 );

	m_dwPowerTick = 0;	
	m_idLastTarget	= NULL_ID;
	
	n_nMoverSelectCount = nullptr;
	m_dwGuildCombatTime = 0xffffffff;
	memset( &m_QuestTime, 0, sizeof(m_QuestTime) );
	memset( m_szGuildCombatStr, 0, sizeof(char) * 64 );	
	m_pSelectRenderObj= NULL;
	m_bCtrlInfo = FALSE;
	m_bCtrlPushed = FALSE;
	m_bRenderFPS  = FALSE;

	m_rcCheck.reserve( 4 );
	
#ifdef __YAUTOATTACK
	m_bAttackDbk = FALSE;
#endif //__YAUTOATTACK

	m_pWndBuffStatus = NULL;

	m_bShiftPushed = FALSE;
	m_bAngelFinish = FALSE;

	m_bGuildCombat1to1Wait = FALSE;
	m_pNextTargetObj = NULL;
	m_pRenderTargetObj = NULL;

	memset( m_szSecretRoomStr, 0, sizeof(char) * 256 );
	
	for(int i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
	{
		m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
		m_stKillCountCiphers[i].szMyGuildKillCount = '0';
		m_stKillCountCiphers[i].ptPos = CPoint(0,0);
		m_stKillCountCiphers[i].fScaleX = 1.0f;
		m_stKillCountCiphers[i].fScaleY = 1.0f;
		m_stKillCountCiphers[i].nAlpha = 255;
	}

	m_bFlashBackground = FALSE;
	m_bSetQuestNPCDest = FALSE;
	m_vDestinationArrow = D3DXVECTOR3( -1.0F, 0.0F, -1.0F );
}

CWndWorld::~CWndWorld()
{
	//SAFE_DELETE( m_pWndDialog );
	g_ClientMsg.ClearAllMessage();
	DeleteDeviceObjects();
}
int nColor;
BOOL g_bAlpha;
FLOAT g_fXScale, g_fYScale;
FLOAT g_fAddScale;
int		g_nFlySpeed = 0;
int		g_nDrift = 0;
#ifdef __XUZHU
float _g_fReg[16];
#endif
void CWndWorld::OnDraw( C2DRender* p2DRender )
{
	_PROFILE("CWndWorld::OnDraw()");
#if defined(__XUZHU) || defined(__PROF) || defined(_DEBUG)
	TCHAR strDebug[128];
	strDebug[0] = 0;
#endif
#ifdef _DEBUG
	TCHAR str[256] = { 0 };
	sprintf( str, "%f   %f", g_Neuz.m_camera.m_fCurRotx, g_Neuz.m_camera.m_fRotx );
	p2DRender->TextOut( 2, 20 , str , D3DCOLOR_ARGB( 255, 255, 0, 255 ) );
#endif //_DEBUG	
	

#ifdef __XUZHU
	p2DRender->TextOut( 2, 210 ,g_Neuz.m_strFrameStats , D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	if( g_pPlayer ) {
		if( g_pPlayer->GetIAObjLink() )
			p2DRender->TextOut( 2, 200 ,1 , D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		else
			p2DRender->TextOut( 2, 200 ,0 , D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

		_stprintf( strDebug, _T("Obj:%zu  Face:%d   LFace:%d" ), g_pPlayer->GetWorld()->m_objCull.size(), g_nMaxTri, 0);
		p2DRender->TextOut( 2, 230, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("%f %f %f %f %f" ), _g_fReg[0], _g_fReg[1], _g_fReg[2], _g_fReg[3], _g_fReg[4]  );
		p2DRender->TextOut( 2, 250, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("%d %d %d %d" ), m_ptMouseOld.x, m_ptMouseOld.y, g_WndMng.m_pWndTaskBar->m_nExecute, 0  );
		p2DRender->TextOut( 2, 270, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	}

	_stprintf( strDebug, _T("bloom %d" ), g_Option.m_nBloom  );
	p2DRender->TextOut( 2, 355, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	
#endif

	if( g_pPlayer && m_bRenderFPS )
	{
		TCHAR strFPS[32];
		_stprintf( strFPS, "%.02f FPS", g_Neuz.m_fFPS );
		p2DRender->TextOut( 5,  105 , strFPS, D3DCOLOR_ARGB( 255, 0, 255, 255 ) );				
	}

/*
#ifdef _DEBUG
	if( g_pPlayer )
	{
		_stprintf( strDebug, _T("전기저항 %d" ), g_pPlayer->GetParam( DST_RESIST_ELECTRICITY, 0 )  );
		p2DRender->TextOut( 2, 180, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("불저항 %d" ), g_pPlayer->GetParam( DST_RESIST_FIRE, 0 )  );
		p2DRender->TextOut( 2, 195, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("바람저항 %d" ), g_pPlayer->GetParam( DST_RESIST_WIND, 0 )  );
		p2DRender->TextOut( 2, 210, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("물저항 %d" ), g_pPlayer->GetParam( DST_RESIST_WATER, 0 )  );
		p2DRender->TextOut( 2, 225, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("땅저항 %d" ), g_pPlayer->GetParam( DST_RESIST_EARTH, 0 )  );
		p2DRender->TextOut( 2, 240, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("시련모드 %d" ), g_pPlayer->IsMode(RECOVERCHAO_MODE)  );
		p2DRender->TextOut( 2, 255, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	}
#endif // DEBUG	
*/
	
#ifdef __XUZHU
	if( g_pPlayer )
	{
		_stprintf( strDebug, _T("%d"), g_pPlayer->m_tmAccFuel );
		p2DRender->TextOut( 400, 270, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	}
#endif
#ifdef __PROF
	//p2DRender->TextOut( 600, 120, g_Prof.m_szResult, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	extern DWORD	_dwA, _dwB, _dwC;
	_stprintf( strDebug, _T("%d %d %d" ), _dwA, _dwB, _dwC );
	p2DRender->TextOut( 2, 140, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

	if( g_Neuz.m_fFPS < 10.0f )
	{
		int a;
		a++;
	}
	
#endif 
#ifdef __J
	_stprintf( strDebug, _T("%d %d" ), g_lBytesRecvd, g_lBytesProcessed );
	p2DRender->TextOut( 2, 140, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

#endif

	g_DialogMsg.Render( p2DRender );

	g_Caption1.Render( CPoint( 0, 184 * p2DRender->m_clipRect.Height() / 768 ), p2DRender );
	g_CapTime.Render( CPoint( 0, 50 * p2DRender->m_clipRect.Height() / 768 ), p2DRender );

#ifdef _DEBUG
	CString strTime;
	if( g_GameTimer.m_nHour > 12 )
		strTime.Format( "pm %d시, %d분, %d초\n", g_GameTimer.m_nHour - 12, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	else
		strTime.Format( "am %d시, %d분, %d초\n", g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	//p2DRender->TextOut( 0, 500, strTime );
#endif	

#ifdef __YCLOTH
	if( g_pPlayer )
	{
		D3DXMATRIX matWorld;

		D3DXMatrixScaling( &matWorld, 0.1f, 0.1f, 0.1f );
		//matWorld *= g_pPlayer->GetMatrixTrans();

		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );

		g_Cloth.Render(p2DRender->m_pd3dDevice, &matWorld );
	}
#endif
	
/*
	// 비행중 십자선.
	if( g_pPlayer->m_pActMover->IsFly() && g_Neuz.m_camera.m_fZoom <= 2.0f )
	{
		CPoint pt1, pt2;
		int		nCx = g_Neuz.GetCreationWidth() / 2;
		int		nCy = g_Neuz.GetCreationHeight() / 2;
		pt1.x = nCx - 8;		pt1.y = nCy;		// 가로선
		pt2.x = nCx + 8;		pt2.y = pt1.y;
		p2DRender->RenderLine( pt1, pt2, D3DCOLOR_ARGB( 192, 255, 0, 0) );
		pt1.x = nCx - 0;		pt1.y = nCy - 8;	// 세로선
		pt2.x = pt1.x;			pt2.y = nCy + 8;
		p2DRender->RenderLine( pt1, pt2, D3DCOLOR_ARGB( 192, 255, 0, 0) );
	}
*/
}
/*
void CWndWorld::ProjectionAndView( LPDIRECT3DDEVICE9 pd3dDevice )
{
	Projection( D3DDEVICE );
	g_Neuz.m_camera.Process( D3DDEVICE );
	g_Neuz.m_camera.Transform( D3DDEVICE, g_WorldMng() );
}
*/
void CWndWorld::Projection( LPDIRECT3DDEVICE9 pd3dDevice )
{
	// Frame Window와 관련된 Viewport 세팅 
	CRect rectRoot = m_pWndRoot->GetWindowRect();
	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = rectRoot.Width();
	viewport.Height = rectRoot.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	if( 0 ) //bFull ) 
	{
		/*
		CWndBase* pWndFull 
			= g_WndMng.FindFullWnd();
		if(pWndFull)
			viewport.Width = CLIENT_WIDTH - pWndFull->GetWindowRect().Width();
		else
			viewport.Width = CLIENT_WIDTH;
		pd3dDevice->SetViewport( &viewport );
		*/
	}
	else
	{
		//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
		CRect rect = GetClientRect();
		viewport.Width = rect.Width();
		viewport.Height = rect.Height();
		//m_pd3dDevice->SetViewport(&viewport);
	}
	// 프로젝션 
	g_WorldMng.Get()->Projection( pd3dDevice, viewport.Width, viewport.Height );
}

BOOL CWndWorld::OnEraseBkgnd(C2DRender* p2DRender)
{
	_PROFILE("CWndWorld::OnEraseBkgnd()");
	CHECK1();

	Projection( D3DDEVICE );
	// CNeuzApp:Render()에도 Clear가 있어 중복되므로 지웠음. -XuZhu-
	//p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, CWorld::m_dwBgColor, 1.0f, 0 ) ;
	//if( m_nWinSize != WSIZE_MAX )
		//p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 90, 146, 222 ), 1.0f, 0 ) ;
	DWORD dwColor = CWorld::GetDiffuseColor();
	p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;

	if( g_pPlayer == NULL )		return FALSE;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	CWorld* pWorld = g_WorldMng.Get();
	// 필드 출력 
	pWorld->SetCamera( &g_Neuz.m_camera );
	CHECK1();
/*
	if( pFocus  )
	{
		CMover* pMover = (CMover*)pFocus;
		CModelObject* pModel = (CModelObject*)pMover->m_pModel;
		for( int i=0; i<MAX_ELEMENT; i++ )
			pModel->SetEffect( i, XE_HIGHLIGHT_OBJ );
	}	
*/			
	pWorld->Render( pd3dDevice, m_Theme.m_pFontWorld );
	CHECK2("Render World" );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
	
	CObj *pFocus = pWorld->GetObjFocus();

	int nGaugeValue;
	int nPower = GetGaugePower( &nGaugeValue );	
	BOOL bRenderGauge = FALSE;
	DWORD dwShootItemKind = 0;

	// 완드나 스테프를 들고 있을 때는 마법 게이지 출력 
	CItemElem* pItemElem = g_pPlayer->GetWeaponItem();

	// 보우 공격
	if( pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_BOW ) 
	{
		// 그리고 왼쪽 버튼을 누르고 포커스가 있어야한다. 그래야 마법 타이밍 작동하니까.
		if( pFocus && pFocus->GetType() == OT_MOVER && g_pPlayer->IsLive() )
		{
			bRenderGauge = TRUE;
			if( m_bLButtonDown && m_bSelectTarget )
			{
				if( nPower >= MAX_CHARGE_LEVEL )			// 맥스치가 되면 자동으로 발사됨.
					dwShootItemKind = IK3_BOW;
			}
		}
	}

	if( pItemElem &&  pItemElem->GetProp()->dwItemKind3 == IK3_WAND )
	{
		// 그리고 왼쪽 버튼을 누르고 포커스가 있어야한다. 그래야 마법 타이밍 작동하니까.
		if( pFocus && pFocus->GetType() == OT_MOVER && g_pPlayer->IsLive() )
		{
			bRenderGauge = TRUE;
			if( m_bLButtonDown && m_bSelectTarget )
			{
				if( g_pPlayer->GetManaPoint() >= 5 )
				{
					if( nPower >= MAX_CHARGE_LEVEL )	// 최고 단계
						dwShootItemKind = IK3_WAND;
				} 
				else
				{	// 마나가 모자른상황이면 최고 단계까지 올라가지 않고 최고 -1단계 짜리를 쏜다. 
					if( nPower >= (MAX_CHARGE_LEVEL-1) )	
						dwShootItemKind = IK3_WAND;
				}
			}
		}
	} // IK3_WAND
	
	if( dwShootItemKind )
	{
		ASSERT( nPower > 0 );
		if( g_pPlayer->IsAttackAble(pFocus) )
		{
			OBJID idTarget = ((CCtrl*)pFocus)->GetId();
			switch( dwShootItemKind )
			{
			case IK3_WAND:
				m_dwPowerTick = ::GetTickCount() + POWER_ATK_DELAY;
				g_pPlayer->CMD_SetMagicAttack( idTarget, nPower );		
				break;
			case IK3_BOW:
				m_dwPowerTick = ::GetTickCount() + POWER_ATK_DELAY;
				g_pPlayer->CMD_SetRangeAttack( idTarget, nPower );		
				break;
			}
		}
	}

	if( bRenderGauge )
	{
		if( nPower == -1 )	// 초고단계 공격 후 딜레이 상태?
			nPower = MAX_CHARGE_LEVEL;		// 풀 게이지로 그린다
		else if( m_bLButtonDown == FALSE || m_bSelectTarget == FALSE)	
			nPower = 0;					// 그리지 않는다.

		g_pPlayer->RenderGauge( pd3dDevice, nPower );	// 0-4
	}

	// 비행시 터보게이지
	if( g_pPlayer->IsFly() )
	{
		// 최대 12초 가속을 기준으로한 가속 게이지.
		g_pPlayer->RenderTurboGauge( pd3dDevice, 0xff0000ff, g_pPlayer->m_tmAccFuel, 12 * 1000 );
	}
	
	D3DXVECTOR3 v3CameraDir, v3PartyMemberDir;
	v3CameraDir = g_Neuz.m_camera.m_vLookAt - g_Neuz.m_camera.m_vPos;
	D3DXVec3Normalize( &v3CameraDir, &v3CameraDir );

	for( int i = 0 ; i < g_Party.GetSizeofMember() ; i++ )
	{
		u_long idPlayer = g_Party.GetPlayerId( i );
		CMover* pMover  = prj.GetUserByID( idPlayer );
		
		if( pMover == g_pPlayer )
			continue;
		
		if( !IsValidObj(pMover) )
			continue;
		
		v3PartyMemberDir = pMover->GetPos() - g_Neuz.m_camera.m_vPos;
		D3DXVec3Normalize( &v3PartyMemberDir, &v3PartyMemberDir );

		if( D3DXVec3Dot( &v3CameraDir, &v3PartyMemberDir ) < 0.0f )
			continue;
		
		pMover->RenderHP( g_Neuz.m_pd3dDevice );
	}
	
	if( IsValidObj(g_pPlayer) )
	{
		if( CWorld::m_nZoomLevel == 0 )
		{
			_PROFILE("Render Player HP, Casting, Gauge, ...");

			g_pPlayer->RenderHP( g_Neuz.m_pd3dDevice );
			g_pPlayer->RenderCasting( g_Neuz.m_pd3dDevice );
			g_pPlayer->RenderPVPCount( g_Neuz.m_pd3dDevice );
			g_pPlayer->RenderCtrlCasting( g_Neuz.m_pd3dDevice );
			g_pPlayer->RenderSkillCasting( g_Neuz.m_pd3dDevice );
			g_pPlayer->RenderCltGauge( g_Neuz.m_pd3dDevice );
		}
	}
	
	if( pFocus && pFocus->GetType() == OT_MOVER )
	{
		if( ((CMover *)pFocus)->IsMode( TRANSPARENT_MODE ) )		// 상대가 투명화 상태면
			pWorld->SetObjFocus( NULL );							// 타겟잡은거 풀림.
	}

	RenderSelectObj( p2DRender, pWorld->GetObjFocus() );	

	if( IsValidObj(m_pSelectRenderObj) )
	{
		RenderSelectObj( &(g_Neuz.m_2DRender), m_pSelectRenderObj );
	}

	// 날기 모드가 아니면 고도계 출력 안하고 종료 
	if( g_pPlayer && g_pPlayer->m_pActMover->IsFly() == TRUE )
		RenderAltimeter();

	CRect rectClient = GetClientRect();
	g_Neuz.m_camera.Transform( g_Neuz.m_pd3dDevice, g_WorldMng.Get() );

#ifdef __CLIENT
	// 머리위에 뜨는 데미지 숫자를 위해 게임화면 뷰표트를 받아둠.
	g_DamageNumMng.m_matView = g_Neuz.m_camera.m_matView;
	g_DamageNumMng.m_matProj = g_WorldMng.Get()->m_matProj;
	pd3dDevice->GetViewport( &g_DamageNumMng.m_viewport );
#endif
	
	if( g_pPlayer->IsDie() )	// 죽었을땐 타겟팅을 못하도록 하자. 이것땜에 팅기더라.
		CObj::m_pObjHighlight = NULL;
	else
		HighlightObj( GetMousePoint() );

	CRect rect = GetClientRect();
	D3DXVECTOR3 vRayEnd;
	
	g_DamageNumMng.Render();
		
	CHECK2( "Render WndWorld" );
	RenderArrow();

	RenderWantedArrow();

	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	// LIGHT / FOG가 꺼진상태에서 드로우 되어야 한다,.
	// 비행중 사경반경이내에 들어오는 플레이어들은 사각테두리가 쳐진다.
	{
		int	i, nSize = m_aFlyTarget.GetSize();
		CRect rect;
		CMover *pMover;
		OBJID idMover;
		
		for( i = 0; i < nSize; i ++ )
		{
			idMover = m_aFlyTarget.GetAt(i);
			pMover = prj.GetMover( idMover );
			if( IsValidObj(pMover) )
			{
				GetBoundRect( pMover, &rect );		// 화면상에서의 바운드 렉트를 구함.
				g_Neuz.m_2DRender.RenderRect( rect, D3DCOLOR_ARGB(0xff, 255, 32, 32) );
			}
		}
	}

	g_Flash.RenderFlash( p2DRender );

	if(!g_WndMng.m_clearFlag)
		RenderBuff( p2DRender );

#ifdef __INTERNALSERVER
	if( m_QuestTime.bFlag )
	{
		CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
		g_Neuz.m_2DRender.SetFont( CWndBase::m_Theme.m_pFontGuildCombatText );
		
		CTimeSpan ct( ((m_QuestTime.dwQuestTime - GetTickCount()) / 1000) );
		
		char szMsg[256] = { 0 };			
		
		sprintf( szMsg, "%s %.2d:%.2d:%.2d", m_szGuildCombatStr, ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
		
		if( ct.GetHours() <=0 && ct.GetMinutes() <=0 && ct.GetSeconds() <=0 )
			m_QuestTime.bFlag = FALSE;
		
		int nX = m_rectWindow.Width() / 2;
		int nY = 110;
		
		CSize cSize = p2DRender->m_pFont->GetTextExtent( szMsg );
		nX -= (cSize.cx / 2);
		
		p2DRender->TextOut( nX, nY, szMsg, 0xffffffff, 0xff990099 );
		g_Neuz.m_2DRender.SetFont( pOldFont );		
	}
#endif //__INTERNALSERVER

	if( m_dwGuildCombatTime != 0xffffffff )
	{
		CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
		g_Neuz.m_2DRender.SetFont( CWndBase::m_Theme.m_pFontGuildCombatText );
		
		CTimeSpan ct( ((m_dwGuildCombatTime - GetTickCount()) / 1000) );

		char szMsg[256] = { 0 };			
		
		if(m_bGuildCombat1to1Wait)
		{
			CString strMsg, strSec;
			strSec.Format(prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_WAITSEC), ct.GetSeconds());
			strMsg.Format("%s %s", m_szGuildCombatStr, strSec);				
			sprintf( szMsg, strMsg );
		}
		else
			sprintf( szMsg, "%s %.2d:%.2d:%.2d", m_szGuildCombatStr, ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );

		if( ct.GetHours() <=0 && ct.GetMinutes() <=0 && ct.GetSeconds() <=0 )
			m_dwGuildCombatTime = 0xffffffff;

		int nX = m_rectWindow.Width() / 2;
		int nY = 110;

		CSize cSize = p2DRender->m_pFont->GetTextExtent( szMsg );
		nX -= (cSize.cx / 2);
		
		p2DRender->TextOut( nX, nY, szMsg, 0xffffffff, 0xff990099 );
		g_Neuz.m_2DRender.SetFont( pOldFont );		
	}

	if( g_pPlayer && g_pPlayer->GetWorld() && g_GuildCombat1to1Mng.IsPossibleMover(g_pPlayer) )
	{
		DrawGuildCombat1to1PlayerInfo(p2DRender);
		DrawGuildCombat1ot1GuildInfo(p2DRender);
	}

	const DWORD dwSecretTime = CSecretRoomMng::GetInstance()->m_dwRemainTime;

	if( dwSecretTime != 0 )
	{
		CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
		g_Neuz.m_2DRender.SetFont( CWndBase::m_Theme.m_pFontGuildCombatText );

		CTimeSpan ct( ((dwSecretTime - GetTickCount()) / 1000) );

		char szMsg[256] = { 0 };			

		sprintf( szMsg, "%s %.2d:%.2d:%.2d", m_szSecretRoomStr, ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );

		if( ct.GetHours() <=0 && ct.GetMinutes() <=0 && ct.GetSeconds() <=0 )
		{
			CSecretRoomMng::GetInstance()->m_dwRemainTime = 0;
		}

		int nX = m_rectWindow.Width() / 2;
		int nY = 110;

		CSize cSize = p2DRender->m_pFont->GetTextExtent( szMsg );
		nX -= (cSize.cx / 2);

		p2DRender->TextOut( nX, nY, szMsg, 0xffffffff, 0xff990099 );
		g_Neuz.m_2DRender.SetFont( pOldFont );		
	}

	if( g_pPlayer && CSecretRoomMng::GetInstance()->m_nState >= SRCONT_WAR )
	{
		if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( g_pPlayer ) )
			DrawSecretRoomInfo(p2DRender);
	}


	if (g_pPlayer) {
		if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR) {
			m_GCprecedence.Render(p2DRender, GetClientRect());
		}

		if (g_pPlayer->GetGuild()) {
			m_infoGC.RenderMyGuildStatus(p2DRender);
		}
	}
	
	{
		char szMsg[128] = {0,};
		int nX = 20;
		int nY = (m_rectWindow.Height()/2) - 80;
		CSize cSize = p2DRender->m_pFont->GetTextExtent( m_szGuildCombatStr );
				
		CRect crect;

		int nCount = 0;
		for( auto  itv = m_vGuildCombatSort.begin() ; itv != m_vGuildCombatSort.end() ; ++itv )
		{
			if( 9 < nCount )
				break;

			BOOL bFind = FALSE;
			__GuildCombatJoin GuildCombatJoin;
			for( int gci = 0 ; gci < (int)( m_vecGuildCombatJoin.size() ) ; ++gci )
			{
				if( ((__GuildCombatJoin)*itv).uidGuild == m_vecGuildCombatJoin[gci].uidGuild )
				{
					bFind = TRUE;
					GuildCombatJoin = m_vecGuildCombatJoin[gci];
					break;
				}
			}

			if( bFind )
			{
				if( g_GuildCombatMng.m_nState == CGuildCombat::WAR_STATE )
				{
					if( GuildCombatJoin.nJoinSize == 0 )
						continue;
				}

				nY += (cSize.cy+10);

				char szMsgGuild[MAX_NAME] = {0,};
				ComputeShortenName(szMsgGuild, GuildCombatJoin.szJoinGuildName, 8);
				
				sprintf( szMsg, "%.2d/%.2d", GuildCombatJoin.nJoinSize, GuildCombatJoin.nJoinSize + GuildCombatJoin.nOutSize );

				crect = CRect(nX-10, nY-5, nX+160, nY+18);
				p2DRender->RenderFillRect(crect, D3DCOLOR_ARGB( 30, 0, 0, 200 ) );

				if( nCount < 1 )
				{
					p2DRender->TextOut( nX, nY, szMsgGuild, 0xFFFF0000, 0xFF000000 );
					p2DRender->TextOut( nX + 120, nY, szMsg, 0xFFFF0000, 0xFF000000 );
				}
				else if( nCount < 3 )
				{
					p2DRender->TextOut( nX, nY, szMsgGuild, 0xFF26F4F6, 0xFF000000 );
					p2DRender->TextOut( nX + 120, nY, szMsg, 0xFF26F4F6, 0xFF000000 );
				}
				else
				{
					p2DRender->TextOut( nX, nY, szMsgGuild, 0xFFFFFFFF, 0xFF000000 );
					p2DRender->TextOut( nX + 120, nY, szMsg, 0xFFFFFFFF, 0xFF000000 );
				}

				++nCount;
			}
		}
	}

	return TRUE;
}

static bool prKCountAsce(const __SRGUILDINFO & guild1, const __SRGUILDINFO & guild2) {
	if (guild1.nWarState > guild2.nWarState)
		return true;
	if (guild1.nWarState < guild2.nWarState)
		return false;

	return guild1.nKillCount > guild2.nKillCount;
}

void CWndWorld::DrawSecretRoomGuildInfo(C2DRender *p2DRender, BOOL bIsMyGuild, int nRank, __SRGUILDINFO stGuildInfo, CPoint ptRank, CPoint ptLogo, 
										CPoint ptGName, CPoint ptHypoon, CPoint ptState, float fLogoScaleX, float fLogoScaleY, CRect rectBg)
{
	CString strFormat;
	DWORD dwColor = 0xFFFFFFFF;
	// Draw Rank
	strFormat.Format("%d.", nRank+1);
	p2DRender->TextOut( ptRank.x, ptRank.y, strFormat, 0xFFFFFFFF );

	// Draw Logo
	if(g_GuildMng.GetGuild(stGuildInfo.dwGuildId)->m_dwLogo > 0)
		this->m_pTextureLogo[g_GuildMng.GetGuild(stGuildInfo.dwGuildId)->m_dwLogo-1].RenderScal( &g_Neuz.m_2DRender, ptLogo, 255, fLogoScaleX, fLogoScaleY );

	// Draw Guild Name
	strFormat = strings::CStringMaxSize(g_GuildMng.GetGuild(stGuildInfo.dwGuildId)->m_szGuild, 14);
	p2DRender->TextOut( ptGName.x, ptGName.y, strFormat, 0xFFFEBB1B );

	// Draw Hypoon
	if(stGuildInfo.nWarState == MONSTER_MIDBOSS || stGuildInfo.nWarState == MONSTER_BOSS)
		p2DRender->TextOut( ptHypoon.x - 3, ptHypoon.y, "vs", 0xFFFFFFFF );
	else
		p2DRender->TextOut( ptHypoon.x, ptHypoon.y, "-", 0xFFFFFFFF );

	// Draw State
	int nMax = CSecretRoomMng::GetInstance()->m_mapMonsterNum.find(stGuildInfo.nWarState)->second;
	
	switch( stGuildInfo.nWarState )
	{
		case MONSTER_WIN:
			DrawOutLineLamp(p2DRender, rectBg, 0x00FFFF00, 0xBEFFFF00, stGuildInfo.nWarState, nRank, bIsMyGuild);
			strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_WIN));
			dwColor = 0xFFFFFF00;
			p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );
			break;
		case MONSTER_FAILED:
			strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_LOSS));
			dwColor = 0xFFFF0000;
			p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );
			break;
		case MONSTER_NORMAL:
			{
				if(bIsMyGuild)
				{
					DrawMyGuildKillCount(p2DRender, stGuildInfo, ptState, nMax);
					DrawOutLineFlash(p2DRender, rectBg, 0x00FFFFFF, 0xBEFFFFFF);
				}
				else
				{
					strFormat.Format("%03d", stGuildInfo.nKillCount);

					p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );

					ptState.x += 21;
					p2DRender->TextOut( ptState.x , ptState.y, "/", 0xFFC2D1FF );

					strFormat.Format("%03d", nMax);
					ptState.x += 7;
					p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );
				}
			}
			break;
		case MONSTER_MIDBOSS:
			DrawOutLineLamp(p2DRender, rectBg, 0x008BE3E2, 0xBE8BE3E2, stGuildInfo.nWarState, nRank, bIsMyGuild);
			strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_MID_BOSS));
			dwColor = 0xFF8BE3E2;
			p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );
			break;
		case MONSTER_BOSS:
			DrawOutLineLamp(p2DRender, rectBg, 0x00F67608, 0xBEF67608, stGuildInfo.nWarState, nRank, bIsMyGuild);
			strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_FIN_BOSS));
			dwColor = 0xFFF67608;
			p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );
			break;
	}
}

void CWndWorld::DrawMyGuildKillCount(C2DRender *p2DRender, __SRGUILDINFO stGuildInfo, CPoint ptState, int nMax)
{
	CString strFormat;
	int i;
	int nCipher = 0;
	char strTempKillCount[MAX_KILLCOUNT_CIPHERS+1];
	char strMyGuildKillCount[MAX_KILLCOUNT_CIPHERS+1];
	DWORD dwColor = 0xFFFFFFFF;

	sprintf(strTempKillCount, "%03d", stGuildInfo.nKillCount);

	for(i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
	{
		strMyGuildKillCount[i] = m_stKillCountCiphers[i].szMyGuildKillCount;
		if(strTempKillCount[i] > '0' && nCipher == 0)
			nCipher = MAX_KILLCOUNT_CIPHERS - i;
	}
	
	strMyGuildKillCount[MAX_KILLCOUNT_CIPHERS] = NULL;
	int nMyGuildKillCount = atoi(strMyGuildKillCount);

	if(nMyGuildKillCount != stGuildInfo.nKillCount)
		m_bFlashBackground = TRUE;

	for(i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
	{
		if(nMyGuildKillCount != stGuildInfo.nKillCount)
		{
			if(MAX_KILLCOUNT_CIPHERS - i <= nCipher)
			{
				m_stKillCountCiphers[i].bDrawMyGuildKillCount = FALSE;
				m_stKillCountCiphers[i].fScaleX = 2.5f;
				m_stKillCountCiphers[i].fScaleY = 2.5f;
			}

			m_stKillCountCiphers[i].szMyGuildKillCount = strTempKillCount[i];
		}

		m_stKillCountCiphers[i].ptPos = ptState;
		
		if(m_stKillCountCiphers[i].bDrawMyGuildKillCount)
		{
			strFormat.Format("%c", m_stKillCountCiphers[i].szMyGuildKillCount);
			p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );
		}

		ptState.x += 9;
	}

	p2DRender->TextOut( ptState.x, ptState.y, "/", 0xFFC2D1FF );

	ptState.x += 7;
	strFormat.Format("%03d", nMax);
	p2DRender->TextOut( ptState.x, ptState.y, strFormat, dwColor );

	// Kill Count Animation
	for(i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
	{
		if(m_stKillCountCiphers[i].bDrawMyGuildKillCount == FALSE)
		{
			m_stKillCountCiphers[i].fScaleX -= 0.15f;
			m_stKillCountCiphers[i].fScaleY -= 0.15f;

			if(m_stKillCountCiphers[i].fScaleX <= 1.0f && m_stKillCountCiphers[i].fScaleY <= 1.0f)
			{
				m_stKillCountCiphers[i].fScaleX = 1.0f;
				m_stKillCountCiphers[i].fScaleY = 1.0f;
				m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;					
			}
			
			strFormat.Format("%c", m_stKillCountCiphers[i].szMyGuildKillCount);
			int nPosx = (int)( m_stKillCountCiphers[i].ptPos.x - (6*(m_stKillCountCiphers[i].fScaleX - 1.0f)) );
			int nPosy = (int)( m_stKillCountCiphers[i].ptPos.y - (8*(m_stKillCountCiphers[i].fScaleY - 1.0f)) );
			dwColor = D3DCOLOR_ARGB((m_stKillCountCiphers[i].nAlpha -= 10), 255, 255, 255);

			p2DRender->TextOut(nPosx, nPosy, m_stKillCountCiphers[i].fScaleX, 
				m_stKillCountCiphers[i].fScaleY, strFormat, dwColor);
		}
	}
}

void CWndWorld::DrawOutLineLamp(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend, int nState, int nRank, BOOL bIsMyGuild)
{
	CPoint Point = CPoint( rectBg.TopLeft().x, rectBg.TopLeft().y );
	LONG thick;
	float fVar = 0.01f;

	if(bIsMyGuild)
		thick = 4;
	else
		thick = 2;

	if(nState == MONSTER_BOSS || nState == MONSTER_MIDBOSS)
		fVar = 0.04f;
	else if(nState == MONSTER_WIN)
		fVar = 0.01f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

	static BOOL  bReverse[8] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
	static FLOAT fLerp[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

	if( bReverse[nRank] )
		fLerp[nRank] -= fVar;
	else
		fLerp[nRank] += fVar;

	if( fLerp[nRank] > 1.0f )
	{
		bReverse[nRank] = TRUE;
		fLerp[nRank] = 1.0f;
	}
	else if( fLerp[nRank] < 0.0f )
	{
		bReverse[nRank] = FALSE;
		fLerp[nRank] = 0.0f;
	}
	
	D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp[nRank]);
	
	CRect Rect = CRect( 0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top );
	
	p2DRender->RenderFillRect( CRect( (Point.x+Rect.left)-thick-1, Point.y+Rect.top-(thick/2), (Point.x+Rect.left), Point.y+Rect.bottom+(thick/2) ),
		dwColor1, dwColor2, dwColor1, dwColor2 );
	
	p2DRender->RenderFillRect( CRect( (Point.x+Rect.left), (Point.y+Rect.top)-thick-1, (Point.x+Rect.right), Point.y+Rect.top ),
		dwColor1, dwColor1, dwColor2, dwColor2 );
	
	p2DRender->RenderFillRect( CRect( (Point.x+Rect.right), Point.y+Rect.top-(thick/2), (Point.x+Rect.right)+thick, Point.y+Rect.bottom+(thick/2) ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( (Point.x+Rect.left), (Point.y+Rect.bottom), (Point.x+Rect.right), (Point.y+Rect.bottom)+thick ),
		dwColor2, dwColor2, dwColor1, dwColor1 );
}

void CWndWorld::DrawOutLineFlash(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend)
{
	if(m_bFlashBackground)
	{
		CPoint Point = CPoint( rectBg.TopLeft().x, rectBg.TopLeft().y );
		LONG thick = 6;

		D3DXCOLOR dwColorDest2 = dwColorstart;
		D3DXCOLOR dwColor = dwColorend;
		D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
		D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

		static BOOL  bReverse = FALSE;
		static FLOAT fLerp = 0.0f;

		if( bReverse )
			fLerp -= 0.15f;
		else
			fLerp += 0.15f;

		if( fLerp > 1.0f )
		{
			bReverse = TRUE;
			fLerp = 1.0f;
			m_bFlashBackground = FALSE;
		}
		else if( fLerp < 0.0f )
		{
			bReverse = FALSE;
			fLerp = 0.0f;
		}
		
		D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp);
		
		CRect Rect = CRect( 0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top );
		
		p2DRender->RenderFillRect( CRect( (Point.x+Rect.left)-thick, Point.y+Rect.top-(thick/2), (Point.x+Rect.left), Point.y+Rect.bottom+(thick/2) ),
			dwColor1, dwColor2, dwColor1, dwColor2 );
		
		p2DRender->RenderFillRect( CRect( (Point.x+Rect.left), (Point.y+Rect.top)-thick, (Point.x+Rect.right), Point.y+Rect.top ),
			dwColor1, dwColor1, dwColor2, dwColor2 );
		
		p2DRender->RenderFillRect( CRect( (Point.x+Rect.right), Point.y+Rect.top-(thick/2), (Point.x+Rect.right)+thick, Point.y+Rect.bottom+(thick/2) ),
			dwColor2, dwColor1, dwColor2, dwColor1 );
		
		p2DRender->RenderFillRect( CRect( (Point.x+Rect.left), (Point.y+Rect.bottom), (Point.x+Rect.right), (Point.y+Rect.bottom)+thick ),
			dwColor2, dwColor2, dwColor1, dwColor1 );
	}
}

void CWndWorld::DrawSecretRoomInfo(C2DRender *p2DRender)
{
#ifdef __CLIENT
	CPoint cPoint;
	CRect crBoard, crMyGuildBoard;
	DWORD dwBgColor;

	// Background Board Setting
	crBoard.left = 20;
	crBoard.top  = 120;
	crBoard.right = crBoard.left + 270;
	crBoard.bottom = crBoard.top + 20;

	crMyGuildBoard.left = 10;
	crMyGuildBoard.top  = 120;
	crMyGuildBoard.right = crMyGuildBoard.left + 305;
	crMyGuildBoard.bottom = crMyGuildBoard.top;

	if( g_pPlayer && g_pPlayer->GetGuild() )
	{
		CString strFormat;
		__SRGUILDINFO stGuildInfo;
		CSecretRoomContinent* pSRCont = CSecretRoomMng::GetInstance()->m_pSRCont;
		
		CD3DFont* pOldFont = p2DRender->GetFont();

		if(pSRCont)
		{
			int size = pSRCont->m_vecSecretRoomTender.size();
			
			// Set Guild List & Sort by KillCount
			m_vecGuildList.clear();
			for(int i=0; i<size; i++)
			{
				stGuildInfo.dwGuildId = pSRCont->m_vecSecretRoomTender[i].dwGuildId;
				stGuildInfo.nWarState = pSRCont->m_vecSecretRoomTender[i].nWarState;
				stGuildInfo.nKillCount = pSRCont->m_vecSecretRoomTender[i].nKillCount;

				m_vecGuildList.push_back(stGuildInfo);
			}

			std::sort(m_vecGuildList.begin(), m_vecGuildList.end(), prKCountAsce);

			auto iter = m_vecGuildList.begin();

			for(int j=0; j<(int)( m_vecGuildList.size() ); j++)
			{
				if(iter != m_vecGuildList.end())
				{
					stGuildInfo = (*iter);
					// Darw Background

					if(g_GuildMng.GetGuild(stGuildInfo.dwGuildId) == NULL)
					{
						iter++;
						continue;
					}

					if(stGuildInfo.dwGuildId == g_pPlayer->GetGuild()->m_idGuild)
					{
						if(stGuildInfo.nWarState == MONSTER_FAILED)
							dwBgColor = D3DCOLOR_ARGB( 150, 255, 0, 0 );
						else
							dwBgColor = D3DCOLOR_ARGB( 100, 0, 100, 255 );

						p2DRender->SetFont( CWndBase::m_Theme.m_pFontSRMyGiuld );

						crMyGuildBoard.top = crBoard.top + 18;
						crMyGuildBoard.bottom = crMyGuildBoard.top + 25;

						crBoard.top += 15;
						crBoard.bottom += 15;

						cPoint.x = crMyGuildBoard.left + 6;
						cPoint.y = crMyGuildBoard.top + 4;

						p2DRender->RenderFillRect( crMyGuildBoard, dwBgColor );

						DrawSecretRoomGuildInfo(p2DRender, TRUE, j, stGuildInfo, CPoint(cPoint.x, cPoint.y), CPoint(crBoard.left + 16, crMyGuildBoard.top), 
							CPoint(cPoint.x + 50, cPoint.y), CPoint(cPoint.x + 210, cPoint.y), CPoint(cPoint.x + 230, cPoint.y), 1.0f, 1.0f, crMyGuildBoard);
					}
					else
					{
						if(stGuildInfo.nWarState == MONSTER_FAILED)
							dwBgColor = D3DCOLOR_ARGB( 150, 255, 0, 0 );
						else
							dwBgColor = D3DCOLOR_ARGB( 100, 245, 204, 176 );

						p2DRender->SetFont( CWndBase::m_Theme.m_pFontSRGiuld );

						crBoard.top += 25;
						crBoard.bottom += 25;

						cPoint.x = crBoard.left + 4;
						cPoint.y = crBoard.top + 4;
						
						p2DRender->RenderFillRect( crBoard, dwBgColor );

						DrawSecretRoomGuildInfo(p2DRender, FALSE, j, stGuildInfo, CPoint(cPoint.x, cPoint.y), CPoint(crBoard.left + 18, crBoard.top), 
							CPoint(cPoint.x + 36, cPoint.y), CPoint(cPoint.x + 180, cPoint.y), CPoint(cPoint.x + 200, cPoint.y), 0.8f, 0.8f, crBoard);
					}

					iter++;
				}
				p2DRender->SetFont( pOldFont );
			}
		}
	}
#endif //__CLIENT
}

void CWndWorld::DrawGuildCombat1to1PlayerInfo(C2DRender *p2DRender)
{
	if( g_pPlayer && g_pPlayer->GetGuild() )
	{
		CPoint cPoint;
		CRect crBoard;
		int nGap = 18;
		int nRate = 0;
		__GC1TO1_PLAYER_INFO stPlayerInfo;
		CString strTemp, strResult;
		char szBuf[MAX_NAME] = {0,};

		// Draw Background
		cPoint.x = 16;
		cPoint.y = 150;

		crBoard.left = cPoint.x - 5;
		crBoard.top  = cPoint.y - 30;

		switch( ::GetLanguage() )
		{
			case LANG_KOR:
			case LANG_TWN:
				crBoard.right = cPoint.x + 155;
				break;
			case LANG_FRE:
				crBoard.right = cPoint.x + 185;
				break;
			case LANG_ENG:
			case LANG_VTN:
				crBoard.right = cPoint.x + 195;
				break;
			case LANG_GER:
				crBoard.right = cPoint.x + 210;
				break;
			case LANG_JAP:
				crBoard.right = cPoint.x + 175;
				break;
			default:
				crBoard.right = cPoint.x + 200;
				break;
		}
		//crBoard.right = cPoint.x + 155;
		
		crBoard.bottom = crBoard.top + ((g_GuildCombat1to1Mng.m_vecGuildCombat1to1_Players.size()+2) * 18);
		p2DRender->RenderFillRect( crBoard, D3DCOLOR_ARGB( 60, 192, 217, 217 ) );

		// Draw Guild Name
		CString strFormat = strings::CStringMaxSize(g_pPlayer->GetGuild()->m_szGuild, 16);
		p2DRender->TextOut( cPoint.x, cPoint.y-(nGap+5) ,strFormat, 0xFFEBAD18, 0xFF000000 );

		// Draw Player Info
		DWORD dwStatusColor, dwNameColor;

		for( int i=0; i < (int)( g_GuildCombat1to1Mng.m_vecGuildCombat1to1_Players.size() ); i++ )
		{
			dwNameColor = D3DCOLOR_ARGB( 255, 255, 255, 99 );
			stPlayerInfo = g_GuildCombat1to1Mng.m_vecGuildCombat1to1_Players[i];
			
			nRate++;
			strTemp = strings::CStringMaxSize(
				CPlayerDataCenter::GetInstance()->GetPlayerString( stPlayerInfo.m_uidPlayer ),
				10
			);
			strcpy( szBuf, strTemp.GetString() );

			CGuildMember* pMember = g_pPlayer->GetGuild()->GetMember(stPlayerInfo.m_uidPlayer);
			if(pMember)
			{
				switch(stPlayerInfo.m_nState)
				{
					case CGuildCombat1to1Mng::GC1TO1_PLAYER_READY:
						strResult = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_READY);
						dwStatusColor = D3DCOLOR_ARGB( 255, 200, 145, 200 );
						break;
					case CGuildCombat1to1Mng::GC1TO1_PLAYER_WAR:
						strResult = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_WAR);
						dwStatusColor = D3DCOLOR_ARGB( 255, 255, 127, 0 );
						break;
					case CGuildCombat1to1Mng::GC1TO1_PLAYER_WIN:
						strResult = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_WIN);
						dwStatusColor = D3DCOLOR_ARGB( 255, 147, 112, 219 );
						break;
					case CGuildCombat1to1Mng::GC1TO1_PLAYER_LOSE:
						strResult = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_LOSE);
						dwStatusColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
						break;
					case CGuildCombat1to1Mng::GC1TO1_PLAYER_DRAW:
						strResult = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_DRAW);
						dwStatusColor = D3DCOLOR_ARGB( 255, 105, 219, 147 );
						break;
					default:
						strResult = "....";
						dwStatusColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
						break;
				}

				CMover* pPlayer = pMember->GetMover();
				if(!pPlayer || !g_GuildCombat1to1Mng.IsPossibleMover(pPlayer))
					dwNameColor = D3DCOLOR_ARGB( 255, 168, 168, 168 );

				strTemp.Format( "%2d.", nRate );
				p2DRender->TextOut( cPoint.x, cPoint.y, strTemp, D3DCOLOR_ARGB( 255, 112, 147, 219 ), 0xFF000000 );
					
				strTemp = szBuf;
				p2DRender->TextOut( cPoint.x+25, cPoint.y, strTemp, dwNameColor, 0xFF000000 );
				
				p2DRender->TextOut( cPoint.x+120, cPoint.y, strResult, dwStatusColor, 0xFF000000 );

				cPoint.y += nGap;
			}
		}

	}
}

void CWndWorld::DrawGuildCombat1ot1GuildInfo(C2DRender *p2DRender)
{
	// Draw Background
	CRect crBoard;
	CPoint cPoint = CPoint(GetClientRect().Width() - 150, 200);

	crBoard.left = cPoint.x - 10;
	crBoard.top  = cPoint.y - 10;
	crBoard.right = cPoint.x + 140;
	crBoard.bottom = crBoard.top + 100;
	p2DRender->RenderFillRect( crBoard, D3DCOLOR_ARGB( 60, 192, 217, 217 ) );

	// Draw Title
	p2DRender->TextOut( cPoint.x+22, cPoint.y, prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_WINCOUNT), 0xFFADEAEA, 0xFF000000 );

	// Draw Guild Info
	CString strFormat = strings::CStringMaxSize(g_pPlayer->GetGuild()->m_szGuild, 16);
	p2DRender->TextOut( cPoint.x, cPoint.y+30 ,strFormat, 0xFFEBAD18, 0xFF000000 );
	strFormat.Format("%d", g_GuildCombat1to1Mng.m_nMyGuildCount);
	p2DRender->TextOut( cPoint.x+120, cPoint.y+30 ,strFormat, 0xFFF5CCB0, 0xFF000000 );

	if(g_GuildCombat1to1Mng.m_nGuildCombat1to1Guild != 0)
	{
		CGuild* pGuild = g_GuildMng.GetGuild( g_GuildCombat1to1Mng.m_nGuildCombat1to1Guild );
		if(pGuild)
		{
			strFormat = strings::CStringMaxSize(pGuild->m_szGuild, 16);
			p2DRender->TextOut( cPoint.x, cPoint.y+60 ,strFormat, 0xFFEBAD18, 0xFF000000 );

			strFormat.Format("%d", g_GuildCombat1to1Mng.m_nVsGuildCount);
			p2DRender->TextOut( cPoint.x+120, cPoint.y+60 ,strFormat, 0xFFF5CCB0, 0xFF000000 );
		}
	}
}

void CWndWorld::RenderArrow()
{
	// 타겟의 방향을 가리키는 화살표를 그린다.
	CWorld* pWorld = g_WorldMng();
	if( !g_pPlayer || !pWorld ) 
		return; // 플레이어가 없으면 렌더 안한다
	D3DXVECTOR3 vSrc = g_pPlayer->GetPos();
	D3DXVECTOR3 vDest( 0.0F, 0.0F, 0.0F );
	LPDIRECT3DDEVICE9 pd3dDevice = m_pApp->m_pd3dDevice;

	int nBlend = 255;
	if( m_vDestinationArrow == D3DXVECTOR3( -1.0F, 0.0F, -1.0F ) || g_pPlayer->GetWorld()->GetID() != WI_WORLD_MADRIGAL )
		nBlend = 0;
	else
	{
		vDest = m_vDestinationArrow;
		vDest.y = g_pPlayer->GetPos().y;
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	// 화살표의 위치, 회전값을 결정한다.
	matWorld = D3DXR::LookAtLH010(vDest, vSrc);
	D3DXMatrixInverse (&matWorld,NULL,&matWorld);

	matWorld._41 = g_pPlayer->GetPos().x; 
	matWorld._42 = g_pPlayer->GetPos().y + 2.0f; 
	matWorld._43 = g_pPlayer->GetPos().z;

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	m_meshArrow.SetBlendFactor( nBlend );
	m_meshArrow.Render( pd3dDevice, &matWorld );

	if( m_bSetQuestNPCDest )
		RenderArrow_Text( pd3dDevice, vDest, matWorld);		//gmpbisgun : refactoring 2009_10_20

}

void CWndWorld::RenderArrow_Text( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3& vDest, const D3DXMATRIX& matWorld  )
{
	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos, vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_meshArrow.GetBBVector();

	pd3dDevice->GetViewport( &vp );

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

 	CWorld* pWorld	= g_WorldMng.Get();
 	if(pWorld)
 	{
 		D3DXVec3Project( &vOut, &vPosHeight, &vp, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &matWorld);
 	}

	CPoint point;
	point.x = (LONG)( vOut.x );
	point.y = (LONG)( vOut.y );

	D3DXVECTOR3 vSrc = g_pPlayer->GetPos( );
	D3DXVECTOR3 vDist = vDest - vSrc;
	float fDistSq = D3DXVec3Length( &vDist );
	CString strTemp;
	int nMeter = (int)fDistSq;

	strTemp.Format("%dm", nMeter);
	if(nMeter < 1000)
		strTemp.Format("%dm", nMeter);
	else
	{
		int nKMeter = nMeter / 1000;
		strTemp.Format("%dKm", nKMeter);
	}
	
	int nCount = 0;
	do 
	{
		nMeter = nMeter / 10;
		nCount++;
	} while(nMeter > 0);

#ifdef __BS_PUTNAME_QUESTARROW
	strTemp = m_strDestName + CString( "(" ) + strTemp + CString( ")" );
#endif
	
	CD3DFont* pFont = g_WndMng.m_Theme.m_pFontWorld;
	if( pFont )
	{
		CSize size = pFont->GetTextExtent( strTemp );
		pFont->DrawText( (FLOAT)( point.x - size.cx * 0.5f ), (FLOAT)( point.y - 30 ), D3DCOLOR_XRGB( 50, 150, 200 ), strTemp );
	}
}

BOOL CWndWorld::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	DWORD dwCursor = 0xffffffff;
#ifdef __VRCAMERA
	if( m_bRButtonDown &&/* m_bCameraMode &&*/ g_WorldMng()->GetObjFocus() != CObj::m_pObjHighlight )
#else
	if( m_bMButtonDown )
#endif
	{
		dwCursor = CUR_CAMERA;
	}
	else
	if( CObj::m_pObjHighlight )
	{
		if( CObj::m_pObjHighlight->IsDelete() == FALSE )
		{
			if( CObj::m_pObjHighlight->GetType() == OT_MOVER )
			{
				CMover* pMover = (CMover*) CObj::m_pObjHighlight;
				if( pMover->IsPeaceful() )
				{
					if( pMover->IsPlayer() == FALSE )
					{
						dwCursor = CUR_NPC;
					}
				}
				else
					dwCursor = CUR_ATTACK;
			}
			else
			if( CObj::m_pObjHighlight->GetType() == OT_ITEM )
				dwCursor = CUR_GETITEM;
			else if( CObj::m_pObjHighlight->GetType() == OT_CTRL )
			{
				ObjProp* pObjProp = CObj::m_pObjHighlight->GetProp();

				if( pObjProp )
				{
					CtrlProp* pCtrlProp;
					pCtrlProp = prj.GetCtrlProp(pObjProp->dwID);

					if( pCtrlProp )
					{
						switch(pCtrlProp->dwCtrlKind1) 
						{
							case CK1_HOUSING:
								break;

							case CK1_GUILD_HOUSE:
								dwCursor = CUR_CONTROL;
							break;
							case CK1_CHEST:
							default:
								dwCursor = CUR_CONTROL;
								break;
						}
					}
				}
			}
			else
				dwCursor = CUR_NPC;
		}
	}
	else
	{
		if( (g_pPlayer && g_pPlayer->m_vtInfo.VendorIsVendor() ) || g_WndMng.GetWndBase(APP_WEBBOX) || g_WndMng.GetWndBase(APP_WEBBOX2) || g_WndMng.GetWndVendorBase() )
			dwCursor = CUR_NO;

#ifdef __EVE_MINIGAME
		if( g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO) || g_WndMng.GetWndBase(APP_MINIGAME_DICE) )
			dwCursor = CUR_NO;
#endif //__EVE_MINIGAME

		if( g_WndMng.GetWndBase(APP_RR_MINIGAME_KAWIBAWIBO) || g_WndMng.GetWndBase(APP_RR_MINIGAME_DICE) || g_WndMng.GetWndBase(APP_RR_MINIGAME_ARITHMETIC) 
			|| g_WndMng.GetWndBase(APP_RR_MINIGAME_STOPWATCH) || g_WndMng.GetWndBase(APP_RR_MINIGAME_TYPING) || g_WndMng.GetWndBase(APP_RR_MINIGAME_CARD)
			|| g_WndMng.GetWndBase(APP_RR_MINIGAME_LADDER) )
			dwCursor = CUR_NO;

		if( g_WndMng.GetWndBase(APP_SMELT_JEWEL) )
			dwCursor = CUR_NO;

#ifdef __S_SERVER_UNIFY
		if( g_WndMng.m_bAllAction == FALSE )
			dwCursor = CUR_NO;
#endif // __S_SERVER_UNIFY
		
	}
	if( dwCursor == 0xffffffff )
		CWndNeuz::OnSetCursor( pWndBase, nHitTest, message );
	else
		SetMouseCursor( dwCursor );
	return TRUE;
}
void CWndWorld::GetBoundRect( CObj* pObj, CRect* pRect )
{
	CWorld* pWorld	= g_WorldMng.Get();
	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;
	CModel* pModel = pObj->m_pModel;
	D3DXVECTOR3 vMin, vMax, vPos;

	//소환수만 GetScrPos로 위치를 구한다.
	if(pObj->GetType() == OT_ITEM)
		vPos = pObj->GetPos();
	else
	{
		MoverProp* pMoverProp = ((CMover*)pObj)->GetProp();
		if( pMoverProp && ( pMoverProp->dwAI == AII_PET || pMoverProp->dwAI == AII_EGG ) )
			vPos = pObj->GetScrPos();
		else
			vPos = pObj->GetPos();
	}

	D3DVIEWPORT9 vp;
	pd3dDevice->GetViewport( &vp );
	
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y , vPos.z);
	matWorld = matWorld * pObj->GetMatrixScale() * pObj->GetMatrixRotation() * matTrans;

	const BOUND_BOX* pBB = pModel->GetBBVector();
	D3DXVECTOR3 vOut[ 8 ];
	for( int i = 0; i < 8; i++ )
		D3DXVec3Project( &vOut[ i ], &pBB->m_vPos[ i ], &vp, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &matWorld );	

	CRect rectClient = GetClientRect();
	//m_rectBound.SetRect( rectClient.right, rectClient.bottom, rectClient.left, rectClient.top );
	pRect->SetRect( 65535, 65535, -65535, -65535 );
	for( int i = 0; i < 8; i++ )
	{
		vPos = vOut[ i ];
		if( vPos.x < pRect->left )
			pRect->left = (LONG)( vPos.x );
		if( vPos.x > pRect->right )
			pRect->right = (LONG)( vPos.x );
		if( vPos.y < pRect->top )
			pRect->top = (LONG)( vPos.y );
		if( vPos.y > pRect->bottom )
			pRect->bottom = (LONG)( vPos.y );
	}
}

void CWndWorld::RenderSelectObj( C2DRender* p2DRender, CObj* pObj )
{
	CWorld* pWorld	= g_WorldMng.Get();	

	if(m_pRenderTargetObj)
		pObj = m_pRenderTargetObj;
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pObj;
		//		if( pMover->m_pActMover->IsDie() )		
		//			pObj = NULL;	// 무버가 죽은상태면 타겟팅 표시 안되게 함
		if( IsValidObj( pMover ) && ( pMover->IsPeaceful() == FALSE || pMover->IsPlayer() ) )
		{
#ifdef _DEBUG
#ifdef __XUZHU
			{
				D3DXVECTOR3 vDest = g_pPlayer->GetPos();
				D3DXVECTOR3 vSrc  = pMover->GetPos();		// 몬스터쪽에서 플레이어쪽의벡터.
				_g_fReg[4] = GetDegreeX( vDest, vSrc );
			}
#endif
#endif			
			BOOL bSkip = FALSE;

			// 운영자이고 변신중이면 HP바 안그린다.
			if( pMover->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
			{
				bSkip = TRUE;
				
				if( pMover->HasBuffByIk3(IK3_TEXT_DISGUISE) )
					bSkip = TRUE;
			}

			// 자신이 운영자 이면 모두 그린다.
			if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
				bSkip = FALSE;
				
			if( bSkip == FALSE )
			{
				// 게이지 출력
				CRect rect;// = GetClientRect();
				//	pMover->m_nHitPoint = 50;
				int nWidthClient = 200;
				//100 : rect = hp : x
				int nWidth;

				CString strTemp;
				
				// HP
				//nWidth = nWidthClient * ((float)pMover->GetHitPoint() / pMover->GetMaxHitPoint());
				nWidth = (int)( (__int64)nWidthClient * (__int64)pMover->GetHitPoint() / (__int64)pMover->GetMaxHitPoint() );
				//if( m_nHPWidth != nWidth ) 
				{
					//m_nHPWidth = nWidth;
					int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
					//if( )
					rect = CRect( nPos, 30, nPos + nWidthClient, 0 );
					CPoint point = rect.TopLeft();
					CRect rectTemp = rect; 
					rectTemp.right = rectTemp.left + nWidth;

					ClientToScreen( rect );
					ClientToScreen( rectTemp );
					m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rect, D3DCOLOR_ARGB( 200, 255, 255, 255 ), m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB( 128, 255, 15, 15 ), m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
					
					//p2DRender->RenderTexture( CPoint( nPos-60, 7 ), &m_texTargetGauge );
					
					//int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
					
					CD3DFont* pOldFont = p2DRender->GetFont();
					p2DRender->SetFont( m_Theme.m_pFontWorld );
					TCHAR	szText[128];
					if( pMover->IsPlayer() )
					{
						// 타겟이 자신보다 10레벨 이하면 레벨을 볼수 있음.
						if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE || g_pPlayer->GetLevel() + 20 > pMover->GetLevel() )
							if( pMover->IsJobTypeOrBetter(JTYPE_HERO) )
								sprintf( szText, "%s <Lvl %d-H>", pMover->GetName(), pMover->GetLevel() );
							else if( pMover->IsMaster() )
								sprintf( szText, "%s <Lvl %d-M>", pMover->GetName(), pMover->GetLevel() );
							else
								sprintf( szText, "%s <Lvl %d>", pMover->GetName(), pMover->GetLevel() );
						else
							sprintf( szText, "%s <Lvl \?\?>", pMover->GetName() );
					}
					else
					{
						BOOL bViewLevel = TRUE;
						if( ::GetLanguage() == LANG_THA && pMover->GetIndex() == MI_MRPUMPKIN5 )	// 태국의 경우	// 괴수 미스터펌킨은
							bViewLevel = FALSE;
						
						if( pMover->GetProp()->dwHideLevel == 1 )
							bViewLevel = FALSE;

						if( bViewLevel )
							sprintf( szText, "%s <Lvl %d>", pMover->GetName(), pMover->GetLevel() );
						else
							sprintf( szText, "%s", pMover->GetName() );
					}
					if( pMover->IsPlayer() )
					{
						// 중앙 정렬
						int n = 0;
						int ntextlen = strlen(szText);
						if(ntextlen <= 16)
							n = (16 - ntextlen) * 3;

						p2DRender->TextOut( point.x + 50 + n, point.y - 18, szText, COLOR_PLAYER );
					}
					else
					{
						DWORD dwC = COLOR_MONSTER;
						if( pMover->m_bActiveAttack )
						{
							dwC = 0xffff0000;
						}
														
						p2DRender->TextOut( point.x + 50, point.y - 18, szText, dwC );
					}

					int nLevelIntv;
					if( pMover->IsPlayer() )
						nLevelIntv = pMover->GetLevel() - g_pPlayer->GetLevel();
					else
						nLevelIntv = pMover->GetProp()->dwLevel - g_pPlayer->GetLevel();
					// 타겟이 플레이어가 아닐 경우 레벨 화실표 표시 
					if( pMover->IsPlayer() == FALSE )
					{
						BOOL bAble = TRUE;
						if( ::GetLanguage() == LANG_THA && pMover->GetIndex() == MI_MRPUMPKIN5 )	// 태국의 경우	// 괴수 미스터펌킨은
							bAble = FALSE;		// 화살표 표시 안함.

						if( bAble )
						{
							if( nLevelIntv > 0 )	// 높은쪽은 1이라도 높으면 표시
							{
								if( nLevelIntv > 5 )
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvUp2 );
								else
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvUp );
							}
							else
							if( nLevelIntv < -1 )	// 낮은쪽은 2단계 이상 차이나야 표시
							{
								if( nLevelIntv < -5 )
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvDn2 );
								else
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvDn );
							}
						}

						if( pMover->GetProp() )
						{
							if( pMover->GetProp()->eElementType )
							{
								point.y -= 25;
								
								m_texAttrIcon.Render( p2DRender, point, pMover->GetProp()->eElementType-1, 255, 1.5f, 1.5f );
							}
						}
					}
					// 운영자라면 선택된 캐릭터의 최대HP와 현재 남은 HP를 표시해준다.
					if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
					{
						CString string;
						if( pMover->IsPlayer() )
							string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_INFORMATION_PLAYER ), pMover->GetHitPoint(), pMover->GetMaxHitPoint(), pMover->GetGold(), (float)pMover->GetExpPercent()/100.0f, pMover->GetExp1(), pMover->GetMaxExp1() );
						else
							string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_INFORMATION_MONSTER ), pMover->GetHitPoint(), pMover->GetMaxHitPoint() );
						strTemp += string;
					}
					RenderMoverBuff( pMover, p2DRender );
					p2DRender->TextOut( rect.left, rect.top + 12, strTemp );
					p2DRender->SetFont( pOldFont );	
				}
			}
		}
	}
	
	if( pObj && pObj->GetType() == OT_CTRL )
	{
		CCommonCtrl* pCtrl = (CCommonCtrl*)pObj;
		
		if( pCtrl && m_bCtrlInfo == FALSE )
		{
			if( pCtrl->m_dwDelete == 0xffffffff )
			{
				g_DPlay.SendPacket<PACKETTYPE_EXPBOXINFO, OBJID>(pCtrl->GetId());
			}
			
			m_bCtrlInfo = TRUE;
		}
		
		if( pCtrl && m_bCtrlInfo )
		{
			if( pCtrl->m_dwDelete != 0xffffffff )
			{
				CString str;
				CTimeSpan ct( (pCtrl->m_dwDelete - GetTickCount()) / 1000 );
				
				if( pCtrl->m_CtrlElem.m_dwSet & UA_PLAYER_ID )
				{
					CString strName;					
					strName		= CPlayerDataCenter::GetInstance()->GetPlayerString( pCtrl->m_idExpPlayer );
					str.Format( prj.GetText(TID_GAME_EXPBOX_INFO), strName );
		
					// 게이지 출력
					CRect rect;
					int nWidthClient = 200;
					int nWidth;
					// HP
					nWidth = (int)( nWidthClient * ((float)200 / 200) );
					int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
					rect = CRect( nPos, 30, nPos + nWidthClient, 0 );
					CPoint point = rect.TopLeft();
					CRect rectTemp = rect; 
					rectTemp.right = rectTemp.left + nWidth;
					ClientToScreen( rect );
					ClientToScreen( rectTemp );
					m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rect, D3DCOLOR_ARGB( 200, 255, 255, 255 ), m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB( 128, 255, 15, 15 ), m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );

					CD3DFont* pOldFont = p2DRender->GetFont();
					p2DRender->SetFont( m_Theme.m_pFontWorld );
					p2DRender->TextOut( point.x + 50, point.y - 18, str, COLOR_MONSTER );
					
					str.Format( "%s %.2d:%.2d:%.2d", prj.GetText(TID_GAME_EXP_COUTMSG0), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
					p2DRender->TextOut( rect.left, rect.top + 12, str );

					p2DRender->SetFont( pOldFont );						
				}
			}
		}
	}
	else
	{
		m_bCtrlInfo = FALSE;
	}

	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() != OT_CTRL )
	{
		if( pObj->IsCull() == TRUE )	// 재조정할것도 없고 컬링되서 안보이면 안찍음.
		{
		} 
		else
		{
			if( pObj == pWorld->GetObjFocus() )
			{
				CRect rectBound;
				GetBoundRect( pObj, &rectBound );
				RenderFocusObj( pObj, rectBound, D3DCOLOR_ARGB( 100, 255,  0,  0 ), 0xffffff00 );
			}
		}
	}

	//gmpbigsun: 길드하우스 안에서는 선택된 가구에 대해서 표시를 해야한다.
	if( IsValidObjID( GuildHouse->m_dwSelectedObjID ) )
	{
		CCtrl* pCtrl = prj.GetCtrl( GuildHouse->m_dwSelectedObjID );
		if( pCtrl )
		{
			CRect recBound;
			GetBoundRect( pCtrl, &recBound );
			RenderFocusObj( pCtrl, recBound, D3DCOLOR_ARGB( 100, 255,  0,  0 ), 0xffffff00 );
		}
	}
	

	if( m_bAutoAttack && m_pNextTargetObj && m_pNextTargetObj->GetType() != OT_CTRL )
	{
		if( m_pNextTargetObj->IsCull() == TRUE )	// 재조정할것도 없고 컬링되서 안보이면 안찍음.
		{
		} 
		else
		{
			if( m_pNextTargetObj )
			{
				CRect rectBound;
				GetBoundRect( m_pNextTargetObj, &rectBound );
				RenderFocusObj( m_pNextTargetObj, rectBound, D3DCOLOR_ARGB( 100, 255,  0,  0 ), 0xffffff00 );
			}
		}
	}

/* chipi_090814 - 핑 출력을 디버그 창으로 이동
	// 핑(Ping) 출력
	if( g_pPlayer && g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( m_Theme.m_pFontWorld );
		CRect rect;
		int nWidthClient = 200;
		int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
		rect = CRect( nPos, 30, nPos + nWidthClient, 0 );
		ClientToScreen( rect );
		CString string;
		string.Format( "Ping(%d ms)", g_Neuz.m_dwPingTime );
		p2DRender->TextOut( rect.left, rect.top + 24, string );
		p2DRender->SetFont( pOldFont );	
	}
*/
}

void CWndWorld::RenderFocusObj( CObj* pObj, CRect rect, DWORD dwColor1, DWORD dwColor2 )
{
	BOOL bFly = g_pPlayer->m_pActMover->IsFly();		

	CPoint pt1, pt2, pt3, pt4;
	if( bFly )
	{
		pt1 = CPoint( rect.left - 16, rect.top - 32   );
		pt2 = CPoint( rect.right - 16, rect.top - 32   );
		pt3 = CPoint( rect.left -16 , rect.bottom  );
		pt4 = CPoint( rect.right - 16, rect.bottom );
	} else
	{
		pt1 = CPoint( rect.left - 32, rect.top - 32   );
		pt2 = CPoint( rect.right, rect.top - 32   );
		pt3 = CPoint( rect.left -32 , rect.bottom );
		pt4 = CPoint( rect.right, rect.bottom );
	}
	
	CWorld* pWorld = g_WorldMng.Get();

	// 타겟표시의 4귀퉁이 그림.
	if( pObj && pObj != m_pNextTargetObj && (m_bSelectTarget || m_bAutoAttack || g_pPlayer->GetCmd() == OBJACT_USESKILL) && 
		 pObj->GetType() == OT_MOVER && (((CMover*)pObj)->IsPeaceful() == FALSE || ((CMover*)pObj)->IsPlayer()) ) // 플레이어 PK시에도 붉은색 표시 가능
	{
		if( bFly )
		{
			m_texTargetFly.GetAt( 8 )->Render( &g_Neuz.m_2DRender, pt1 );		// 붉은색 표시.
			m_texTargetFly.GetAt( 9 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTargetFly.GetAt( 10)->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTargetFly.GetAt( 11)->Render( &g_Neuz.m_2DRender, pt4 );
		} else
		{
			m_texTarget.GetAt( 8 )->Render( &g_Neuz.m_2DRender, pt1 );		// 붉은색 표시.
			m_texTarget.GetAt( 9 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTarget.GetAt( 10)->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTarget.GetAt( 11)->Render( &g_Neuz.m_2DRender, pt4 );
		}
	}
	else
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			m_texTargetFly.GetAt( 0 )->Render( &g_Neuz.m_2DRender, pt1 );		// 흰색표시
			m_texTargetFly.GetAt( 1 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTargetFly.GetAt( 2 )->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTargetFly.GetAt( 3 )->Render( &g_Neuz.m_2DRender, pt4 );
		} else
		if( IsValidObjID( GuildHouse->m_dwSelectedObjID ) && pObj->GetType( ) == OT_CTRL )
		{
			if( GUILDHOUSE_PCKTTYPE_RESET == GuildHouse->m_iMode )
			{
				m_texTargetFly.GetAt( 8 )->Render( &g_Neuz.m_2DRender, pt1 );		// 붉은색 표시.
				m_texTargetFly.GetAt( 9 )->Render( &g_Neuz.m_2DRender, pt2 );
				m_texTargetFly.GetAt( 10)->Render( &g_Neuz.m_2DRender, pt3 );
				m_texTargetFly.GetAt( 11)->Render( &g_Neuz.m_2DRender, pt4 );

			}else
			{
				m_texTargetFly.GetAt( 0 )->Render( &g_Neuz.m_2DRender, pt1 );		// 흰색표시
				m_texTargetFly.GetAt( 1 )->Render( &g_Neuz.m_2DRender, pt2 );
				m_texTargetFly.GetAt( 2 )->Render( &g_Neuz.m_2DRender, pt3 );
				m_texTargetFly.GetAt( 3 )->Render( &g_Neuz.m_2DRender, pt4 );	
			}
		}
		else 
		{
			m_texTarget.GetAt( 0 )->Render( &g_Neuz.m_2DRender, pt1 );		// 흰색표시
			m_texTarget.GetAt( 1 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTarget.GetAt( 2 )->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTarget.GetAt( 3 )->Render( &g_Neuz.m_2DRender, pt4 );
		}
	}
}

// 화면중심에서 pt방향으로 화살표 표시.
void CWndWorld::RenderFocusArrow( CPoint pt )
{
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj == NULL )	return;

	BOOL bAdjust = FALSE;		// 타겟방향 화살표 표시검사 대상이다.
	BOOL bAdjust2 = FALSE;		// 방향 화살표를 표시해야하는 상황이냐.
	CPoint	ptOut;				// 타겟이 화면을 벗어났을경우 화면테두리 좌표.
	CRect rectBound;
	if( g_pPlayer->m_pActMover->IsFly() )	// 주인공이 비행중이고
	{
		if( pObj->GetType() == OT_MOVER || pObj->GetType() == OT_SHIP ) // 잡은 타겟이 무버라면.
			bAdjust = TRUE;		// 타겟이 화면을 벗어나지 않게 함.
	}
	if( bAdjust )
	{
		GetBoundRect( pObj, &rectBound );
		int nWidth = (rectBound.right - rectBound.left);	// 타겟사각형의 가로세로 폭.
		int nHeight = (rectBound.bottom - rectBound.top);
		// 타겟의 중심좌표.
		ptOut = CPoint( rectBound.left + nWidth / 2, rectBound.top + nHeight / 2 );	
		if( (int)ptOut.x > (int)g_Neuz.GetCreationWidth() )	// 오른쪽으로 벗어남.
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.x = (LONG)g_Neuz.GetCreationWidth() - 32;		// 경계좌표를 설정.
		}
		if( (int)ptOut.x < 0 )	// 왼쪽으로 벗어났을때.
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.x = 32;
		}
		if( (int)ptOut.y > (int)g_Neuz.GetCreationHeight() )
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.y = (int)g_Neuz.GetCreationHeight() - 32;
		}
		if( (int)rectBound.top < 0 )		// 위로 벗어났을때.
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.y = 32;
		}
	}
	
	if( bAdjust2 == FALSE)	// 타겟이 화면내에 있으면 안찍음.
		return;

	pt = ptOut;

	D3DXVECTOR2	vC;
	vC.x = (FLOAT)g_Neuz.GetCreationWidth() / 2;
	vC.y = (FLOAT)g_Neuz.GetCreationHeight() / 2;
	FLOAT xDist = pt.x - vC.x;
	FLOAT yDist = pt.y - vC.y;
	FLOAT fTheta = atan2( yDist, xDist );
#ifdef _DEBUG
	FLOAT fAng = D3DXToDegree( fTheta );
#endif
	
	// 가장큰 화살표
	m_texTargetArrow.GetAt( 0 )->RenderRotate( &g_Neuz.m_2DRender, pt, fTheta );

	D3DXVECTOR2 vOut, vPt;	// 실제 화살표가 찍힐 좌표
//	CPoint	ptOut;			// 실제 화살표가 찍힐 좌표 CPoint버전.
	vPt.x = (FLOAT)( pt.x );
	vPt.y = (FLOAT)( pt.y );

}

void CWndWorld::RenderGauFlight( C2DRender* p2DRender )
{
	if( g_pPlayer && !g_pPlayer->IsVisible() )
		return;
}


void CWndWorld::RenderAltimeter()
{
	return;
	// 고도계
	CRect rect = GetClientRect();

	if( m_nWinSize == WSIZE_MAX )
	{
		int nHeight =  rect.Height() * 60 / 100;
		rect.left = rect.right - 30;
		rect.top += 130;
		rect.bottom -= 135;
	}
	else
	{
		//int nHeight =  rect.Height() * 60 / 100;
		rect.left = rect.right - 30;
		//rect.bottom = rect.top + nHeight;
	}
	g_Neuz.m_2DRender.RenderFillRect( rect, D3DCOLOR_ARGB( 0x30, 0, 0, 0 ) );
	rect.top += 10;
	rect.bottom -= 10;

	FLOAT fHigh1 = -1.0f, fLow1 = 999999.0f;
	FLOAT fHigh2 = -1.0f, fLow2 = 999999.0f;
	CObj* pObj;
	CLandscape* pLand;
//	FOR_LAND( &g_World, pLand, g_World.m_nVisibilityLand, FALSE )
	FOR_LAND( g_WorldMng.Get(), pLand, g_WorldMng.Get()->m_nVisibilityLand, FALSE )
	{
		for (CObj * pObj : pLand->m_apObjects[OT_MOVER].ValidObjs()) {
			D3DXVECTOR3 vPos = pObj->GetPos();
			if (fHigh1 < vPos.y) fHigh1 = vPos.y;
			if (fLow1 > vPos.y) fLow1 = vPos.y;
		}
	}
	END_LAND

	if( fHigh1 == -1.0f && fLow1 == 999999.0f )
		return;
	fHigh2 = fHigh1;
	fLow2  = fLow1;
	if( m_fHigh == -1.0f )
	{
		m_fHigh = fHigh2 = fHigh1;
		m_fLow = fLow2  = fLow1;
	}
	else
	{
		if( fHigh1 < m_fHigh )
		{
			fHigh2 = m_fHigh - 2;
			if( fHigh2 < fHigh1 )
				fHigh2 = fHigh1;
		}
		else
		if( fHigh1 > m_fHigh )
		{
			fHigh2 = m_fHigh + 2;
			if( fHigh2 > fHigh1 )
				fHigh2 = fHigh1;
		}
		if( fLow1 > m_fLow )
		{
			fLow2 = m_fLow + 2;
			if( fLow2 > fLow1 )
				fLow2 = fLow1;
		}
		else
		if( fLow1 < m_fLow )
		{
			fLow2 = m_fLow - 2;
			if( fLow2 < fLow1 )
				fLow2 = fLow1;
		}
	}
//	CWorld* pWorld = &g_World;
	CWorld* pWorld	= g_WorldMng.Get();

//	FOR_LAND( &g_World, pLand, g_World.m_nVisibilityLand, FALSE )
	FOR_LAND( g_WorldMng.Get(), pLand, g_WorldMng.Get()->m_nVisibilityLand, FALSE )
	{
		for (CObj * pObj : pLand->m_apObjects[OT_MOVER].ValidObjs()) {
			CMover* pMover = (CMover*) pObj;
			/*
			if( g_WndMng.m_nObjectFilter == OBJFILTER_PLAYER && pMover->IsPlayer() == FALSE )
				continue;
			if( g_WndMng.m_nObjectFilter == OBJFILTER_MONSTER && ( pMover->IsPlayer() == TRUE || pMover->IsPeaceful() ) )
				continue;
			if( g_WndMng.m_nObjectFilter == OBJFILTER_NPC && ( pMover->IsPlayer() == TRUE || pMover->IsPeaceful() ) )
				continue;
				*/
			if( pMover->IsMode( TRANSPARENT_MODE ) )	// 대상이 투명모드일땐 타겟 안됨.			
				continue;	
			DWORD dwColor = 0xffffffff;
			if( pObj == g_pPlayer )
				dwColor = 0xffffff00;

			D3DXVECTOR3 vPos = pObj->GetPos();
			vPos.y -= fLow2;
			// ( fHigh - fLow ) : vPos.z = rect.Height() : z
			int y = (int)( vPos.y * (FLOAT) rect.Height() / ( fHigh2 - fLow2 ) );
			g_Neuz.m_2DRender.RenderLine( CPoint( rect.left + 7, rect.bottom - y ), CPoint( rect.right, rect.bottom - y ), dwColor );
			int nFontMid = g_Neuz.m_2DRender.m_pFont->GetMaxHeight() / 2;
			CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( pMover->GetName() );
			size.cx += 5;
			g_Neuz.m_2DRender.TextOut( rect.left - size.cx + 1, rect.bottom - y + 1 - nFontMid, pMover->GetName(), 0xff000000 );
			g_Neuz.m_2DRender.TextOut( rect.left - size.cx , rect.bottom - y - nFontMid, pMover->GetName(), dwColor );
			g_Neuz.m_2DRender.RenderFillTriangle( 
				CPoint( rect.left, rect.bottom - y - 5 ), 
				CPoint( rect.left, rect.bottom - y + 5 ), 
				CPoint( rect.left + 5, rect.bottom - y ), 
				dwColor	);
		}
	}
	END_LAND

	pObj = pWorld->GetObjFocus();
	if( pObj )
	{
		DWORD dwColor = dwColor = 0xffff0000;
		CMover* pMover = (CMover*)pObj;
		D3DXVECTOR3 vPos = pObj->GetPos();
		vPos.y -= fLow2;
		// ( fHigh - fLow ) : vPos.z = rect.Height() : z
		int y = (int)( vPos.y * (FLOAT) rect.Height() / ( fHigh2 - fLow2 ) );
		g_Neuz.m_2DRender.RenderLine( CPoint( rect.left + 7, rect.bottom - y ), CPoint( rect.right, rect.bottom - y ), dwColor );
		int nFontMid = g_Neuz.m_2DRender.m_pFont->GetMaxHeight() / 2;
		CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( pMover->GetName() );
		size.cx += 5;
		g_Neuz.m_2DRender.TextOut( rect.left - size.cx + 1, rect.bottom - y + 1 - nFontMid, pMover->GetName(), 0xff000000 );
		g_Neuz.m_2DRender.TextOut( rect.left - size.cx , rect.bottom - y - nFontMid, pMover->GetName(), dwColor );
		g_Neuz.m_2DRender.RenderFillTriangle( 
			CPoint( rect.left, rect.bottom - y - 5 ), 
			CPoint( rect.left, rect.bottom - y + 5 ), 
			CPoint( rect.left + 5, rect.bottom - y ), 
			dwColor	);
	}
	FLOAT y = (FLOAT) rect.Height() / ( fHigh2 - fLow2 );
	for( FLOAT j = 0; j < rect.Height(); j += y )
	{
		g_Neuz.m_2DRender.RenderLine( CPoint( rect.left + 15, (int)( rect.bottom - j ) ), CPoint( rect.right, (int)( rect.bottom - j ) ), 0x80ffffff );
	}
	m_fHigh = fHigh2;
	m_fLow  = fLow2;
}
/*
#define MMI_DIALOG        0
#define MMI_TRADE         1
#define MMI_FIGHT         2
#define MMI_MESSAGE       3
#define MMI_ADD_MESSANGER 4
#define MMI_INVITE_PARTY    5

#define MAX_MOVER_MENU    6 
*/
/*
TCHAR g_aszMoverMenu[ MAX_MOVER_MENU ][ 32 ] =
{
	_T( "대화" ),
	_T( "퀘스트" ),
	_T( "거래" ),
	_T( "대전" ),
	_T( "메시지" ),
	_T( "메신저 추가" ),
	_T( "극단 초청" ),
	_T( "컴퍼니 초청" ),
	_T( "장소 지정" ),
	_T( "보관" ),
	_T( "듀얼신청" )

};
*/

void CWndWorld::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	AddWndStyle( WBS_THICKFRAME );  

	m_strTexture = "WndTile200.tga";

	SetSizeMax();
	CRect rectClient = GetClientRect();

	if( ::GetLanguage() != LANG_JAP )
	{
		const LANGFONT & plfCaption	= CLangMan::GetInstance()->GetLangData( ::GetLanguage() )->font.lfCaption;

		const char * strFont = plfCaption.szFontFirst;
		if( IsFontInstalled( strFont ) == FALSE )
			strFont = plfCaption.szFontSecond;

		if( g_osVersion > WINDOWS_ME )
		{
			constexpr auto MakeFont = [](const TCHAR * strFontName, DWORD dwHeight) -> CD3DFontAPI * {
				if (::GetLanguage() != LANG_VTN) {
					return new CD3DFontAPI(strFontName, dwHeight);
				} else {
					return new CD3DFontAPIVTN(strFontName, dwHeight);
				}
			};

			m_pFontAPICaption = MakeFont( strFont, rectClient.Width() / plfCaption.nDivCaption );
			m_pFontAPICaption->m_nOutLine = 2;
			m_pFontAPICaption->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
			m_pFontAPICaption->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
			m_pFontAPICaption->m_dwFlags = D3DFONT_FILTERED;
			m_pFontAPICaption->InitDeviceObjects( m_pApp->m_pd3dDevice );

			m_pFontAPITitle	= MakeFont( strFont, rectClient.Width() / plfCaption.nDivCaption );
			m_pFontAPITitle->m_nOutLine = 2;
			m_pFontAPITitle->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
			m_pFontAPITitle->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
			m_pFontAPITitle->m_dwFlags = D3DFONT_FILTERED;
			m_pFontAPITitle->InitDeviceObjects( m_pApp->m_pd3dDevice );

			m_pFontAPITime	= MakeFont( plfCaption.szFontSecond, rectClient.Width() / 40 );
			m_pFontAPITime->m_nOutLine = 2;
			m_pFontAPITime->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
			m_pFontAPITime->m_dwBgColor = D3DCOLOR_ARGB( 255, 220, 100, 40 );
			m_pFontAPITime->m_dwFlags = D3DFONT_FILTERED;
			m_pFontAPITime->InitDeviceObjects( m_pApp->m_pd3dDevice );
		}
	}

	m_wndMenuMover.CreateMenu( this );	

	m_texTarget.LoadScript( D3DDEVICE, MakePath( DIR_ICON, "icon_target.inc" ) );
	m_texTargetFly.LoadScript( D3DDEVICE, MakePath( DIR_ICON, "icon_FlightTargetB.inc" ) );			// 비행모드시 타겟 4귀퉁이.		sun!!
	m_texTargetArrow.LoadScript( D3DDEVICE, MakePath( DIR_ICON, "icon_FlightTargetArrow.inc" ) );	// 비행모드시 타겟방향을 가르키는 화살표
	m_texTargetArrow.GetAt(0)->m_ptCenter.x += 32;
	m_texTargetArrow.GetAt(0)->m_ptCenter.y += 32;
	m_texGauFlight.LoadScript( D3DDEVICE, MakePath( DIR_THEME, "Theme_GauFlight.inc" ) );		// 비행모드시 게이지 인터페이스
	m_texFontDigital.LoadScript( D3DDEVICE, MakePath( DIR_THEME, "Theme_FontDigital1.inc" ) );		// 디지탈모양의 폰트.
	
	//m_texFlaris.LoadTexture( D3DDEVICE, MakePath( DIR_EFFECT, "WelcomeToFlaris.tga" ), 0xff000000 );
	//m_texFlaris.m_ptCenter = CPoint( m_texFlaris.m_size.cx / 2, m_texFlaris.m_size.cy / 2 );

	m_meshArrow.InitDeviceObjects( m_pApp->m_pd3dDevice );
	m_meshArrow.LoadModel( "etc_arrow.o3d" );

	m_meshArrowWanted.InitDeviceObjects( m_pApp->m_pd3dDevice );
	m_meshArrowWanted.LoadModel( "arrow.o3d" );
	m_bRenderArrowWanted = FALSE;
	m_dwRenderArrowTime  = 0;
	
	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texLvUp.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvUp.bmp" ), 0xffff00ff, TRUE );
	m_texLvDn.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvDn.bmp" ), 0xffff00ff, TRUE );
	m_texLvUp2.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvUp2.bmp" ), 0xffff00ff, TRUE );
	m_texLvDn2.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvDn2.bmp" ), 0xffff00ff, TRUE );


	for (const AddSkillProp & pAddSkill : prj.m_aPropAddSkill) {

		if( pAddSkill.dwSkillTime != -1 )
			{
				ItemProp* pItem = prj.GetSkillProp(pAddSkill.dwName);

				if( pItem )
				{
					BUFFSKILL buffskill;
					buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ICON, pItem->szIcon ), 0xffff00ff );

					m_pBuffTexture[0].emplace(pItem->dwID, buffskill);
				}
			}

	}

	for (const ItemProp & pItemProp : prj.m_aPartySkill) {
			if( pItemProp.dwSkillTime != -1 )
			{
				BUFFSKILL buffskill;
				buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ICON, pItemProp.szIcon ), 0xffff00ff );
				m_pBuffTexture[1].emplace(pItemProp.dwID, buffskill);
			}
	}

	for (const ItemProp & pItemProp : prj.m_aPropItem) {
			if( pItemProp.dwSkillTime != -1 
#ifdef __DST_GIFTBOX
				|| pItemProp.dwDestParam[0] == DST_GIFTBOX
				|| pItemProp.dwDestParam[1] == DST_GIFTBOX
				|| pItemProp.dwDestParam[2] == DST_GIFTBOX 
#endif // __DST_GIFTBOX
				|| pItemProp.dwItemKind3 == IK3_EGG
				|| pItemProp.dwItemKind3 == IK3_PET
			)
			{

				BUFFSKILL buffskill;

				CString strIcon	= pItemProp.szIcon;
				if( pItemProp.dwItemKind3 == IK3_EGG && pItemProp.dwID != II_PET_EGG )
				{
					strIcon.Replace( ".", "_00." );
					buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, strIcon ), 0xffff00ff );
					m_pBuffTexture[2].emplace( MAKELONG( (WORD)pItemProp.dwID, 0 ), buffskill );
					strIcon.Replace( "0.", "1." );
					buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, strIcon ), 0xffff00ff );
					m_pBuffTexture[2].emplace( MAKELONG( (WORD)pItemProp.dwID, 1 ), buffskill );
					strIcon.Replace( "1.", "2." );
					buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, strIcon ), 0xffff00ff );
					m_pBuffTexture[2].emplace( MAKELONG( (WORD)pItemProp.dwID, 2 ), buffskill );
				}
				else
				{
#ifdef __DST_GIFTBOX
					if(pItemProp.dwDestParam[0] == DST_GIFTBOX || pItemProp.dwDestParam[1] == DST_GIFTBOX || pItemProp.dwDestParam[2] == DST_GIFTBOX)
						buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ICON, "Skill_TroGiftbox02.dds" ), 0xffff00ff );
					else
						buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, pItemProp.szIcon ), 0xffff00ff );
#else //__DST_GIFTBOX
					buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, pItemProp.szIcon ), 0xffff00ff );
#endif //__DST_GIFTBOX
					m_pBuffTexture[2].emplace( pItemProp.dwID, buffskill );
				}
			}

	}

	switch( m_rectWindow.Width() )
	{
	default:
	case 800:
		m_nLimitBuffCount = 7;
		break;
	case 1024:
		m_nLimitBuffCount = 10;
		break;
	case 1280:
		m_nLimitBuffCount = 13;
		break;
	case 1360:
		m_nLimitBuffCount = 14;
		break;
	case 1400:
		m_nLimitBuffCount = 15;
		break;
	case 1440:
		m_nLimitBuffCount = 15;
		break;
	case 1600:
		m_nLimitBuffCount = 17;
		break;
	case 1680:
		m_nLimitBuffCount = 18;
		break;
	}
	
	// 아이콘 텍스쳐 로딩
	CString str;
	for( int i=0; i<CUSTOM_LOGO_MAX; i++ )
	{
		str.Format( "Icon_CloakSLogo%02d.jpg", i+1 );
		
		if( !m_pTextureLogo[i].LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_ICON, str ), D3DCOLOR_XRGB(0,0,0), FALSE ) )
		{
			Error( "길드 로고 텍스쳐 로딩 실패 : %s", str );
		}
	}

#ifdef __YCLOTH
	if( g_pPlayer )
	{
		D3DXMATRIX  mat1;
		
		mat1 = ((CModelObject*)g_pPlayer->m_pModel)->m_mUpdateBone[10];

		mat1 = mat1 * g_pPlayer->GetMatrixTrans();

		g_Cloth.DeleteDeviceObjects();
		g_Cloth.Init( 13, D3DXVECTOR3( 0.0f, -0.98f, 0.0f), 2.0f, 0.1f, 0.03f, 0.9f, 5, m_pTextureLogo[0].m_pTexture, &mat1 );
		
	}
#endif
	
	m_texMsgIcon.LoadScript( m_pApp->m_pd3dDevice, "icon\\icon_IconMessenger.inc" );
	
	m_texAttrIcon.LoadScript( m_pApp->m_pd3dDevice, "icon\\Icon_MonElemantkind.inc" );
	
	m_texPlayerDataIcon.LoadScript( m_pApp->m_pd3dDevice, "icon\\icon_PlayerData.inc" );
	for( int j = 0 ; j < SM_MAX ; ++j )
	{
		if( j != SM_RESIST_ATTACK_LEFT && j != SM_RESIST_ATTACK_RIGHT && j != SM_RESIST_DEFENSE )
		{
			ItemProp* pItem = prj.GetItemProp( g_AddSMMode.dwSMItemID[j] );
			if( pItem )
				m_dwSMItemTexture[j] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, pItem->szIcon ), 0xffff00ff );
		}
	}

	DWORD dwArry[10] = { II_CHR_SYS_SCR_FIREASTONE,
						II_CHR_SYS_SCR_WATEILSTONE,
						II_CHR_SYS_SCR_LIGHTINESTONE,
						II_CHR_SYS_SCR_WINDYOSTONE,
						II_CHR_SYS_SCR_EARTHYSTONE,
						II_CHR_SYS_SCR_DEFIREASTONE,
						II_CHR_SYS_SCR_DEWATEILSTONE,
						II_CHR_SYS_SCR_DELIGHTINESTONE,
						II_CHR_SYS_SCR_DEWINDYOSTONE,
						II_CHR_SYS_SCR_DEEARTHYSTONE
						};
	
	ItemProp* pItem = NULL;
	for( int kk = 0; kk < 10; kk++ )
	{
		pItem = prj.GetItemProp( dwArry[kk] );

		if( pItem )
			m_dwSMResistItemTexture[kk] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, pItem->szIcon ), 0xffff00ff );	
	}
	
	m_wndTitleBar.SetVisible( FALSE );

	g_DialogMsg.ClearVendorObjMsg();
	
	Projection( D3DDEVICE );

	SAFE_DELETE(m_pWndGuideSystem);
	m_pWndGuideSystem = new CWndGuideSystem;
#ifdef __FIX_WND_1109
	m_pWndGuideSystem->Initialize( this );
#else	// __FIX_WND_1109
	m_pWndGuideSystem->Initialize();
#endif	// __FIX_WND_1109

	if( g_pPlayer->m_dwMode & FRESH_MODE )
	{
		m_pWndGuideSystem->m_dwGuideLevel = *g_Option.m_pGuide = 0;
		if(g_pPlayer->m_nLevel <= 5) m_pWndGuideSystem->GuideStart(FALSE);
	}
	else
	{
		m_pWndGuideSystem->m_dwGuideLevel = *g_Option.m_pGuide;

		if( m_pWndGuideSystem->m_dwGuideLevel == 2 )
			m_pWndGuideSystem->m_bVisible = FALSE;
								
		if(g_pPlayer->m_nLevel <= 5) m_pWndGuideSystem->GuideStart(FALSE);
	}

	if( g_pPlayer->IsMode( MODE_MAILBOX ) )
	{
		CWndInstantMsg* pWndInstantMsg = g_WndMng.OpenInstantMsg( "Mail" );
		if( pWndInstantMsg )
		{
			pWndInstantMsg->AddPostMessage(prj.GetText(TID_MAIL_RECEIVE));
			pWndInstantMsg->m_timer.Set( MIN(1) );		//chipi_080808 - 우편 메세지 창 유지 1분으로 변경
		}

	}

	m_AdvMgr.Init( this );
	m_GCprecedence.Clear();
	m_infoGC.ClearGuildStatus();
	// 일단 노가다다...추후 비스트 고쳐서 해야함...-_-
	m_bViewMap = FALSE;	
	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->Init(); 
	m_TexGuildWinner.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "TexGuildCombatWinner.bmp" ), 0xffff00ff );
	m_TexGuildBest.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "TexGuildCombatBest.bmp" ), 0xffff00ff );
	InitEyeFlash();

#ifdef __Y_CAMERA_SLOW_8
	g_Neuz.m_camera.m_fCurRotx = -g_pPlayer->GetAngle();
#endif //__Y_CAMERA_SLOW_8

}

BOOL CWndWorld::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 100,100,500,400 );
	SetTitle( _T( "World" ) );
	return CWndNeuz::Create(WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME|WBS_MANAGER|WBS_MAXIMIZEBOX,rect,pWndParent,dwWndId);
}
BOOL CWndWorld::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{
		m_AdvMgr.RunButton( nID );
	}
	return FALSE;

	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case WTBID_CLOSE:
				return FALSE;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndWorld::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	CCtrl* pFocusObj = (CCtrl*)g_WorldMng()->GetObjFocus();
	CMover* pFocusMover = (CMover*)pFocusObj;
	if( pWndBase == &m_wndMenuMover && pFocusObj )
	{
		switch( nID )
		{
		// 길드대전 신청
		case MMI_GUILDWAR_APP:
			{
				g_DPlay.SendGuildCombatWindow();
			}
			break;
		// 신청현황
		case  MMI_GUILDWAR_STATE:
			{
				g_DPlay.SendGCRequestStatusWindow();
			}
			break;
		// 취소하기
		case MMI_GUILDWAR_CANCEL:
			{
				CWndGuildWarCancelConfirm* pWndGuildCombat = new CWndGuildWarCancelConfirm(0);

				if(pWndGuildCombat)
					pWndGuildCombat->Initialize( NULL );
			}
			break;
		// 입장하기
		case MMI_GUILDWAR_JOIN:
			{
				CWndGuildWarJoinConfirm* pWndGuildCombat = new CWndGuildWarJoinConfirm(0);
				
				if(pWndGuildCombat)
					pWndGuildCombat->Initialize( NULL );
			}
			break;
		// 랭킹
		case MMI_GUILDCOMBAT_RANKING:
			{
				g_DPlay.SendGCPlayerPoint();
				//g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_RANKING_TEST) );
				//g_DPlay.SendGuildCombatRanking( g_pPlayer->GetId() );
			}
			break;
		// 출전자 구성
		case MMI_GUILDCOMBAT_SELECTION:
			{
				g_DPlay.SendGCSelectPlayerWindow();				
			}
			break;
		// 수수료 안내
		case MMI_GUILDCOMBAT_INFO_TEX:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(0);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_TEXT_6 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}								
			}
			break;
		// 신청금 반환
		case MMI_GUILDCOMBAT_PENYA_RETURN:
			{
			}
			break;
		// 상금받기
		case MMI_GUILDCOMBAT_JACKPOT:
			{
				CGuild* pGuild = g_pPlayer->GetGuild();

				if( pGuild )
				{
					if( pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
						g_DPlay.SendGCGetPenyaGuild();				
				}
			}
			break;
		// 상금&상품 안내
		case MMI_GUILDCOMBAT_JACKPOT2:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);

				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(0);

				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();

					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_TEXT_5 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		// 베스트 플레이어 상금받기
		case MMI_GUILDCOMBAT_BESTPLAYER:
			{
				g_DPlay.SendGCGetPenyaPlayer();
			}
			break;
		// 참가안내
		case MMI_GUILDCOMBAT_INFO_BOARD1:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(0);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_TEXT_1 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		// 기본규칙
		case MMI_GUILDCOMBAT_INFO_BOARD2:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(0);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_TEXT_2 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		// 승리조건
		case MMI_GUILDCOMBAT_INFO_BOARD3:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(0);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_TEXT_3 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_OFFER:
			g_DPlay.SendGC1to1TenderOpenWnd();
			break;
		case MMI_GUILDCOMBAT_1TO1_CANCEL:
			{
				CWndGuildWarCancelConfirm* pWndGuildCombat = new CWndGuildWarCancelConfirm(1);

				if(pWndGuildCombat)
					pWndGuildCombat->Initialize( NULL );
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_OFFERSTATE:
			g_DPlay.SendGC1to1TenderView();
			break;
		case MMI_GUILDCOMBAT_1TO1_SELECTION:
			g_DPlay.SendGC1to1MemberLienUpOpenWnd();
			break;
		case MMI_GUILDCOMBAT_1TO1_ENTRANCE:
			{
				CWndGuildWarJoinConfirm* pWndGuildCombat = new CWndGuildWarJoinConfirm(1);
				
				if(pWndGuildCombat)
					pWndGuildCombat->Initialize( NULL );
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_GUIDE_TEX:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(1);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_1TO1_TEXT_4 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_GUIDE_PRIZE:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(1);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_1TO1_TEXT_5 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_GUIDE_ENTRY:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(1);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_1TO1_TEXT_1 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_GUIDE_RULE:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(1);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_1TO1_TEXT_2 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_GUIDE_WIN:
			{
				SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
				g_WndMng.m_pWndGuildCombatBoard = new CWndGuildCombatBoard(1);
				
				if( g_WndMng.m_pWndGuildCombatBoard )
				{
					g_WndMng.m_pWndGuildCombatBoard->Initialize();
					
					CString strnotice;
					strnotice = GetLangFileName( ::GetLanguage(), FILE_GUILDCOMBAT_1TO1_TEXT_3 );
					
					CScript scanner;
					if( scanner.Load( "Client\\"+strnotice ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					
					g_WndMng.m_pWndGuildCombatBoard->SetString( scanner.m_pProg  );					
				}
			}
			break;
		case MMI_GUILDCOMBAT_1TO1_REPAYMENT:
			{
				g_DPlay.SendGC1to1TenderFailed();
			}
			break;
		case MMI_SECRET_OFFER:
			{
				g_DPlay.SendSecretRoomTenderOpenWnd();
			}
			break;
		case MMI_SECRET_CANCEL:
			{
				CWndSecretRoomCancelConfirm* pWndSecretRoomCancelConfirm = new CWndSecretRoomCancelConfirm;

				if(pWndSecretRoomCancelConfirm)
					pWndSecretRoomCancelConfirm->Initialize( NULL );
			}
			break;
		case MMI_SECRET_OFFERSTATE:
			{
				g_DPlay.SendSecretRoomTenderView();
			}
			break;
		case MMI_SECRET_SELECTION:
			{
				g_DPlay.SendSecretRoomLineUpOpenWnd();
			}
			break;
		case MMI_SECRET_BOARD:
			{
				SAFE_DELETE(g_WndMng.m_pWndSecretRoomBoard);
				g_WndMng.m_pWndSecretRoomBoard = new CWndSecretRoomBoard;
				
				if( g_WndMng.m_pWndSecretRoomBoard )
				{
					g_WndMng.m_pWndSecretRoomBoard->Initialize();			
					g_WndMng.m_pWndSecretRoomBoard->SetString();					
				}								
				
			}
			break;
		case MMI_SECRET_ENTRANCE:
			{
				for(int i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
				{
					m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
					m_stKillCountCiphers[i].szMyGuildKillCount = '0';
					m_stKillCountCiphers[i].ptPos = CPoint(0,0);
					m_stKillCountCiphers[i].fScaleX = 1.0f;
					m_stKillCountCiphers[i].fScaleY = 1.0f;
					m_stKillCountCiphers[i].nAlpha = 255;
				}

				if(g_WndMng.m_pWndSecretRoomMsg)
					SAFE_DELETE( g_WndMng.m_pWndSecretRoomMsg );

				g_DPlay.SendSecretRoomEntrance();
			}
			break;
		case MMI_SECRET_TAXRATES_CHECK:
			{
				SAFE_DELETE(g_WndMng.m_pWndSecretRoomCheckTaxRate);
				g_WndMng.m_pWndSecretRoomCheckTaxRate = new CWndSecretRoomCheckTaxRate;

				if(g_WndMng.m_pWndSecretRoomCheckTaxRate)
					g_WndMng.m_pWndSecretRoomCheckTaxRate->Initialize(NULL);
			}
			break;
		case MMI_SECRET_ENTRANCE_1:
			{
				g_DPlay.SendTeleportToSecretRoomDungeon();
			}
			break;

		case MMI_LVREQDOWN_CANCEL:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_LVREQDOWN );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndLvReqDown );
				g_WndMng.m_pWndLvReqDown = new CWndLvReqDown;
				//g_WndMng.m_pWndLvReqDown->Initialize();
				g_WndMng.m_pWndLvReqDown->Initialize( &g_WndMng, APP_LVREQDOWN );
			}
			break;

		case MMI_ITEM_AWAKENING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_AWAKENING );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndAwakening );
				g_WndMng.m_pWndAwakening = new CWndAwakening;
				//g_WndMng.m_pWndAwakening->Initialize();	
				g_WndMng.m_pWndAwakening->Initialize( &g_WndMng, APP_AWAKENING );
			}
			break;

		case MMI_BLESSING_CANCEL:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_CANCEL_BLESSING );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndBlessingCancel );
				g_WndMng.m_pWndBlessingCancel = new CWndBlessingCancel;
				//g_WndMng.m_pWndAwakening->Initialize();	
				g_WndMng.m_pWndBlessingCancel->Initialize( &g_WndMng, APP_CANCEL_BLESSING );
			}
			break;
#ifdef __JEFF_11
		case MMI_PET_RES:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_PET_RES);
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE(g_WndMng.m_pPetRes);
				g_WndMng.m_pPetRes = new CWndPetRes;
				//g_WndMng.m_pPetRes->Initialize();
				g_WndMng.m_pPetRes->Initialize( &g_WndMng, APP_PET_RES );
			}
			break;
#endif
		case MMI_LORD_RAINBOWAPPLICATION:
			{
				g_DPlay.SendRainbowRaceApplicationOpenWnd();
			}
			break;
		case MMI_LORD_RAINBOWWAIT:
			{
				if(g_WndMng.m_pWndRainbowRaceInfo)
					SAFE_DELETE(g_WndMng.m_pWndRainbowRaceInfo);

				g_WndMng.m_pWndRainbowRaceInfo = new CWndRainbowRaceInfo;

				if(g_WndMng.m_pWndRainbowRaceInfo)
					g_WndMng.m_pWndRainbowRaceInfo->Initialize(NULL);
			}
			break;
		case MMI_LORD_RAINBOWRULE:
			{
				if(g_WndMng.m_pWndRainbowRaceRule)
					SAFE_DELETE(g_WndMng.m_pWndRainbowRaceRule);

				g_WndMng.m_pWndRainbowRaceRule = new CWndRainbowRaceRule;

				if(g_WndMng.m_pWndRainbowRaceRule)
					g_WndMng.m_pWndRainbowRaceRule->Initialize(NULL);
			}
			break;
		case MMI_LORD_RAINBOWTOPTEN:
			{
				g_DPlay.SendRainbowRacePrevRankingOpenWnd();
			}
			break;
		case MMI_LORD_RAINBOWWIN:
			{
				if(g_WndMng.m_pWndRainbowRacePrize)
					SAFE_DELETE(g_WndMng.m_pWndRainbowRacePrize);

				g_WndMng.m_pWndRainbowRacePrize = new CWndRainbowRacePrize;

				if(g_WndMng.m_pWndRainbowRacePrize)
					g_WndMng.m_pWndRainbowRacePrize->Initialize(NULL);
			}
			break;
		case MMI_LORD_RAINBOWEND:
			{
				g_DPlay.SendRainbowRaceReqFinish();
			}
			break;
		case MMI_LORD_RAINBOW_KAWIBAWIBO:
			{
				CRainbowRace::GetInstance()->SendMinigamePacket( RMG_GAWIBAWIBO, MP_OPENWND );
			}
			break;
		case MMI_LORD_RAINBOW_DICE:
			{
				CRainbowRace::GetInstance()->SendMinigamePacket( RMG_DICEPLAY, MP_OPENWND );
			}
			break;
		case MMI_LORD_RAINBOW_ARITHMETIC:
			{
				CRainbowRace::GetInstance()->SendMinigamePacket( RMG_ARITHMATIC, MP_OPENWND );
			}
			break;		
		case MMI_LORD_RAINBOW_STOPWATCH:
			{
				CRainbowRace::GetInstance()->SendMinigamePacket( RMG_STOPWATCH, MP_OPENWND );
			}
			break;
		case MMI_LORD_RAINBOW_TYPING:
			{
				CRainbowRace::GetInstance()->SendMinigamePacket( RMG_TYPING, MP_OPENWND );
			}
			break;
		case MMI_LORD_RAINBOW_CARD:
			{
				CRainbowRace::GetInstance()->SendMinigamePacket( RMG_PAIRGAME, MP_OPENWND );
			}
			break;
		case MMI_LORD_RAINBOW_LADDER:
			{
				CRainbowRace::GetInstance()->SendMinigamePacket( RMG_LADDER, MP_OPENWND );
			}
			break;
		case MMI_POST:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTPOST ), NULL, prj.GetTextColor(TID_GAME_TRADENOTPOST ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_POST );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_MAIL_OTHERWORK ), NULL, prj.GetTextColor(TID_MAIL_OTHERWORK ) );
					break;
				}		
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_BANKTRADESELF ), NULL, prj.GetTextColor(TID_GAME_BANKTRADESELF ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 은행을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText( TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "아이템 수리중이므로 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "상거래중이므로 헤어샵을 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTBEAUTYSHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 메이크업을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 헤어샵을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					//Summon Angel Using
					g_WndMng.PutString( prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING) );
					break;
				}
				g_WndMng.CreateApplet( APP_INVENTORY );
				
				SAFE_DELETE( g_WndMng.m_pWndPost );
				g_WndMng.m_pWndPost = new CWndPost;
				g_WndMng.m_pWndPost->Initialize( NULL );	
			}
			break;
		case MMI_BEAUTYSHOP:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTBEAUTYSHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTBEAUTYSHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					//Summon Angel Using
					g_WndMng.PutString( prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING) );
					break;
				}
				
				if(g_pPlayer->HasEquippedSlot(PARTS_HAT) || g_pPlayer->HasEquippedSlot(PARTS_CAP))
				{
					//가발 및 기타 머리에 쓰는 아이템 착용 시 진입이 안됨.
					g_WndMng.PutString( prj.GetText(TID_GAME_NOTREADY_USESHOP), NULL, prj.GetTextColor(TID_GAME_NOTREADY_USESHOP) );
					break;					
				}

#ifdef __NEWYEARDAY_EVENT_COUPON
				/*
				//헤어 체인지 교환권을 소지하였을 경우.
				if(g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_HAIRCHANGE ))
				{
					if(g_WndMng.m_pWndUseCouponConfirm == NULL)
					{
						g_WndMng.m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
						g_WndMng.m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_EX, 0);
						g_WndMng.m_pWndUseCouponConfirm->Initialize(this);
					}
				}
				else
				*/
				{
					g_WndMng.CreateApplet( APP_INVENTORY );			
					SAFE_DELETE( g_WndMng.m_pWndBeautyShop );
					g_WndMng.m_pWndBeautyShop = new CWndBeautyShop;
					g_WndMng.m_pWndBeautyShop->Initialize( NULL, APP_BEAUTY_SHOP_EX );
				}
#else //__NEWYEARDAY_EVENT_COUPON
				g_WndMng.CreateApplet( APP_INVENTORY );				
				SAFE_DELETE( g_WndMng.m_pWndBeautyShop );				
				g_WndMng.m_pWndBeautyShop = new CWndBeautyShop;
				g_WndMng.m_pWndBeautyShop->Initialize( NULL, APP_BEAUTY_SHOP_EX );
#endif //__NEWYEARDAY_EVENT_COUPON
			}
			break;
		case MMI_BEAUTYSHOP_SKIN:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTFACESHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTFACESHOP ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTFACESHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTFACESHOP  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTFACESHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTFACESHOP ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					//Summon Angel Using
					g_WndMng.PutString( prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				
				if(g_pPlayer->HasEquippedSlot(PARTS_HAT) || g_pPlayer->HasEquippedSlot(PARTS_CAP))
				{
					//가발 및 기타 머리에 쓰는 아이템 착용 시 진입이 안됨.
					g_WndMng.PutString( prj.GetText(TID_GAME_NOTREADY_USESHOP), NULL, prj.GetTextColor(TID_GAME_NOTREADY_USESHOP) );
					break;					
				}

#ifdef __NEWYEARDAY_EVENT_COUPON
				/*
				//메이크업 무료 이용권을 소지하였을 경우.
				if(g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_FACEOFFFREE ))
				{
					if(g_WndMng.m_pWndUseCouponConfirm == NULL)
					{
						g_WndMng.m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
						g_WndMng.m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_SKIN, 0);
						g_WndMng.m_pWndUseCouponConfirm->Initialize(this);
					}
				}
				else
				*/
				{
					g_WndMng.CreateApplet( APP_INVENTORY );
					SAFE_DELETE( g_WndMng.m_pWndFaceShop );
					g_WndMng.m_pWndFaceShop = new CWndFaceShop;
					g_WndMng.m_pWndFaceShop->Initialize( NULL, APP_BEAUTY_SHOP_SKIN );
				}
#else //__NEWYEARDAY_EVENT_COUPON
				g_WndMng.CreateApplet( APP_INVENTORY );				
				SAFE_DELETE( g_WndMng.m_pWndFaceShop );
				g_WndMng.m_pWndFaceShop = new CWndFaceShop;
				g_WndMng.m_pWndFaceShop->Initialize( NULL, APP_BEAUTY_SHOP_SKIN );
#endif //__NEWYEARDAY_EVENT_COUPON
			}
			break;
		case MMI_SUMMON_ANGEL:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTSUMMONANGEL), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTSUMMONANGEL), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_POST );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL ) );
					break;
				}		
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTSUMMONANGEL ), NULL, prj.GetTextColor(TID_GAME_TRADENOTSUMMONANGEL ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_SUMMONANGELUSING ), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING ) );
					break;
				}
				
				g_WndMng.CreateApplet( APP_INVENTORY );
				SAFE_DELETE( g_WndMng.m_pWndSummonAngel );
				g_WndMng.m_pWndSummonAngel = new CWndSummonAngel;

				if( g_WndMng.m_pWndSummonAngel )
				{
					CScript scanner;
				
					g_WndMng.m_pWndSummonAngel->Initialize();
					if( scanner.Load( MakePath( DIR_CLIENT,  _T( "summonAngelQuest.inc" ) ) ) == FALSE )
					{
						SAFE_DELETE(g_WndMng.m_pWndGuildCombatBoard);
						break;
					}
					g_WndMng.m_pWndSummonAngel->SetQuestText( scanner.m_pProg  );
				}
			}
			break;
#ifdef __EVE_MINIGAME
		case MMI_KAWIBAWIBO:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_MINIGAMEUSING), NULL, prj.GetTextColor(TID_GAME_MINIGAMEUSING) );
					break;
				}

				if(g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_EVE_KAWIBAWIBO ))
				{
					if(g_WndMng.m_pWndKawiBawiBoGameConfirm == NULL)
					{
						g_WndMng.m_pWndKawiBawiBoGameConfirm = new CWndKawiBawiBoGameConfirm;
						g_WndMng.m_pWndKawiBawiBoGameConfirm->Initialize();
					}
				}
				else
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_KAWIBAWIBO_STARTGUID1) ) );				
			}
			break;
		case MMI_FINDWORD:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_MINIGAME_WORD );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_MINIGAMEUSING), NULL, prj.GetTextColor(TID_GAME_MINIGAMEUSING) );
					break;
				}
				
				g_WndMng.CreateApplet( APP_INVENTORY );
				
				SAFE_DELETE( g_WndMng.m_pWndFindWordGame );
				g_WndMng.m_pWndFindWordGame = new CWndFindWordGame;
				g_WndMng.m_pWndFindWordGame->Initialize();					
			}
			break;
		case MMI_FIVESYSTEM:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_MINIGAME_DICE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_MINIGAMEUSING), NULL, prj.GetTextColor(TID_GAME_MINIGAMEUSING) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndDiceGame );
				g_WndMng.m_pWndDiceGame = new CWndDiceGame;
				g_WndMng.m_pWndDiceGame->Initialize();					
			}
			break;
		case MMI_REASSEMBLE:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_MINIGAMEUSING), NULL, prj.GetTextColor(TID_GAME_MINIGAMEUSING) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndPuzzleGame );
				g_WndMng.m_pWndPuzzleGame = new CWndPuzzleGame;
				g_WndMng.m_pWndPuzzleGame->Initialize();					
			}
			break;
#endif //__EVE_MINIGAME
		case MMI_SMELT_MIXJEWEL:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}

				g_WndMng.CreateApplet( APP_INVENTORY );
				
				SAFE_DELETE( g_WndMng.m_pWndSmeltMixJewel );
				g_WndMng.m_pWndSmeltMixJewel = new CWndMixJewel;
				g_WndMng.m_pWndSmeltMixJewel->Initialize();					
			}
			break;
		case MMI_SMELT_JEWEL:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_JEWEL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndSmeltJewel );
				g_WndMng.m_pWndSmeltJewel = new CWndSmeltJewel;
				g_WndMng.m_pWndSmeltJewel->Initialize();					
			}
			break;
		case MMI_SMELT_EXTRACTION:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_EXTRACTION );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndExtraction );
				g_WndMng.m_pWndExtraction = new CWndExtraction;
				g_WndMng.m_pWndExtraction->Initialize();					
			}
			break;
		case MMI_SMELT_CHANGEGWEAPON:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_CHANGE_NOWUSING), NULL, prj.GetTextColor(TID_GAME_CHANGE_NOWUSING) );
					break;
				}	

				pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndChangeWeapon );
				g_WndMng.m_pWndChangeWeapon = new CWndChangeWeapon(WEAPON_GENERAL);
				g_WndMng.m_pWndChangeWeapon->Initialize();					
			}
			break;
		case MMI_SMELT_CHANGEUWEAPON:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_CHANGE_NOWUSING), NULL, prj.GetTextColor(TID_GAME_CHANGE_NOWUSING) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}	
				
				SAFE_DELETE( g_WndMng.m_pWndChangeWeapon );
				g_WndMng.m_pWndChangeWeapon = new CWndChangeWeapon(WEAPON_UNIQUE);
				g_WndMng.m_pWndChangeWeapon->Initialize();					
			}
			break;
		case MMI_LEGEND_SKILLUP:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_HERO_SKILLUP );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndHeroSkillUp );
				g_WndMng.m_pWndHeroSkillUp = new CWndHeroSkillUp;
				g_WndMng.m_pWndHeroSkillUp->Initialize();					
			}
			break;
			case MMI_ATTRIBUTE:
			{
				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_REMOVE_ATTRIBUTE );
				if( pWndBase )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndRemoveAttribute );
				g_WndMng.m_pWndRemoveAttribute = new CWndRemoveAttribute;
				g_WndMng.m_pWndRemoveAttribute->Initialize();					
			}
			break;
			case MMI_SMELT_REMOVE_PIERCING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndRemovePiercing );
				g_WndMng.m_pWndRemovePiercing = new CWndRemovePiercing;
				g_WndMng.m_pWndRemovePiercing->Initialize();
			}
			break;
			case MMI_SMELT_REMOVE_JEWEL:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SMELT_REMOVE_JEWEL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}

				SAFE_DELETE( g_WndMng.m_pWndRemoveJewel );
				g_WndMng.m_pWndRemoveJewel = new CWndRemoveJewel;
				g_WndMng.m_pWndRemoveJewel->Initialize();
			}
			break;
			case MMI_PET_EGG01:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_PET_TRANS_EGGS );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}

				g_WndMng.CreateApplet( APP_INVENTORY );
				
				SAFE_DELETE( g_WndMng.m_pWndPetTransEggs );
				g_WndMng.m_pWndPetTransEggs = new CWndPetTransEggs;
				g_WndMng.m_pWndPetTransEggs->Initialize();
			}
			break;
			case MMI_HEAVEN_TOWER:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_HEAVEN_TOWER );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED) );
					break;
				}

				SAFE_DELETE( g_WndMng.m_pWndHeavenTower );
				g_WndMng.m_pWndHeavenTower = new CWndHeavenTower;
				g_WndMng.m_pWndHeavenTower->Initialize();
			}
			break;
#ifdef __NPC_BUFF
			case MMI_NPC_BUFF:
			{
				g_DPlay.SendNPCBuff(pFocusMover->m_szCharacterKey);
			}
			break;
#endif //__NPC_BUFF
		case MMI_MESSAGE:
			g_WndMng.OpenMessage( pFocusMover->GetName() );
			break;
		case MMI_ADD_MESSENGER:
			{
				if( g_WndMng.m_RTMessenger.GetFriend( pFocusMover->m_idPlayer ) )
				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0051) ) );
				}
				else
				{
					g_DPlay.SendAddFriendReqest( pFocusMover->m_idPlayer );
					CString str;
					str.Format( prj.GetText(TID_GAME_MSGINVATE), pFocusMover->GetName() );
					g_WndMng.PutString( str, NULL, prj.GetTextColor(TID_GAME_MSGINVATE) );
				}
			}
			break;
		case MMI_MARKING: // 장소 마킹 
			g_DPlay.SendHdr( PACKETTYPE_SETLODELIGHT );
			break;
		case MMI_DIALOG: // dialog
			if( GetTickCount() - m_dwDropTime < 3000 )
				break;

			{
				CWndBase* pWndBase	= g_WndMng.GetWndVendorBase();
				if( pWndBase )
					break;
			}
			g_DPlay.SendScriptDialogReq( ( (CMover*)pFocusObj )->GetId(), NULL, 0, 0, 0, 0 );
			SAFE_DELETE( g_WndMng.m_pWndDialog );
			g_WndMng.m_pWndDialog = new CWndDialog;
			g_WndMng.m_pWndDialog->m_idMover = ((CMover*)pFocusObj)->GetId();
			g_WndMng.m_pWndDialog->Initialize( &g_WndMng, APP_DIALOG_EX );
			break;
	case MMI_LORD_STATE:
		SAFE_DELETE( g_WndMng.m_pWndLordState );
		g_WndMng.m_pWndLordState = new CWndLordState;
		g_WndMng.m_pWndLordState->Initialize(&g_WndMng);
		break;
	case MMI_LORD_TENDER:
		SAFE_DELETE( g_WndMng.m_pWndLordTender );
		g_WndMng.m_pWndLordTender = new CWndLordTender;
		g_WndMng.m_pWndLordTender->Initialize(&g_WndMng);
		break;
	case MMI_LORD_VOTE:
		SAFE_DELETE( g_WndMng.m_pWndLordVote );
		g_WndMng.m_pWndLordVote = new CWndLordVote;
		g_WndMng.m_pWndLordVote->Initialize(&g_WndMng);
		break;
	case MMI_LORD_EVENT:
		SAFE_DELETE( g_WndMng.m_pWndLordEvent );
		g_WndMng.m_pWndLordEvent = new CWndLordEvent;
		g_WndMng.m_pWndLordEvent->Initialize(&g_WndMng);
		break;
	case MMI_LORD_INFO:
		SAFE_DELETE( g_WndMng.m_pWndLordInfo );
		g_WndMng.m_pWndLordInfo = new CWndLordInfo;
		g_WndMng.m_pWndLordInfo->Initialize(&g_WndMng);
		break;
	case MMI_LORD_RPINFO:
		SAFE_DELETE( g_WndMng.m_pWndLordRPInfo );
		g_WndMng.m_pWndLordRPInfo = new CWndLordRPInfo;
		g_WndMng.m_pWndLordRPInfo->Initialize(&g_WndMng);
		break;
	case MMI_VISIT_FRIEND:
		SAFE_DELETE( g_WndMng.m_pWndRoomList );
		g_WndMng.m_pWndRoomList = new CWndRoomList;
		g_WndMng.m_pWndRoomList->Initialize(&g_WndMng);
		break;
	case MMI_RETURNTO_WORLD:
		SAFE_DELETE( g_WndMng.m_pWndQuitRoom );
		g_WndMng.m_pWndQuitRoom = new CWndQuitRoom;
		g_WndMng.m_pWndQuitRoom->Initialize(&g_WndMng);
		break;
	case MMI_VISIT_MYROOM:
		if(g_pPlayer)
			g_DPlay.SendHousingVisitRoom(g_pPlayer->m_idPlayer);
		break;
	case MMI_PET_AWAK_CANCEL:
		SAFE_DELETE( g_WndMng.m_pWndPetAwakCancel );
		g_WndMng.m_pWndPetAwakCancel = new CWndPetAwakCancel;
		g_WndMng.m_pWndPetAwakCancel->Initialize(&g_WndMng);
		break;
	case MMI_RENAME_CANCEL:
		g_DPlay.SendClearPetName();
		break;
		case MMI_QUERYEQUIP: // 살펴보기
			{
				if( pFocusMover && pFocusMover->IsPlayer() )
				{					
					g_DPlay.SendQueryEquip( pFocusMover->GetId() );
				}
			}
			break;

		case MMI_TRADE: //  거래 
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
								
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKTRADESELF ), NULL, prj.GetTextColor(TID_GAME_BANKTRADESELF ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_POST );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_MAIL_OTHERWORK ), NULL, prj.GetTextColor(TID_MAIL_OTHERWORK ) );
					break;
				}		
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 은행을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 메이크업을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 헤어샵을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING ), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					//Summon Angel Using
					g_WndMng.PutString( prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "아이템 수리중이므로 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor( TID_GAME_REPAIR_NOACTION ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_TRADE );
				if( pWndBaseBuf )
				{
			#ifndef __THAI_0525VER
					g_WndMng.PutString( prj.GetText( TID_GAME_YETTRADE ), NULL, prj.GetTextColor( TID_GAME_YETTRADE ) );
			#endif//__THAI_0525VER
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );	
				if( pWndBaseBuf )
				{
			#ifndef __THAI_0525VER
					g_WndMng.PutString( prj.GetText( TID_GAME_YETTRADE ), NULL, prj.GetTextColor( TID_GAME_YETTRADE ) );
			#endif//__THAI_0525VER
					break;
				}
				
				if( pFocusMover->IsPlayer() )
				{
					g_DPlay.SendConfirmTrade( pFocusMover );
					CString sMessageBuf;
					sMessageBuf.Format( prj.GetText(TID_GAME_TRADEREQUEST ), pFocusMover->GetName() );
					g_WndMng.PutString( sMessageBuf, NULL, prj.GetTextColor(TID_GAME_TRADEREQUEST) );
				}
				else
				{
					if( g_pPlayer->m_vtInfo.GetOther() == NULL )
					{
						if( g_pPlayer->IsChaotic() )
						{
							CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( g_pPlayer->GetPKPropensity() );
							if( !Propensity.nShop )
							{
								g_WndMng.PutString( prj.GetText( TID_GMAE_CHAOTIC_NOT_SHOP ), NULL, prj.GetTextColor( TID_GMAE_CHAOTIC_NOT_SHOP ) );
								break;
							}
						}
						g_DPlay.SendOpenShopWnd( pFocusMover->GetId() );						
					}
				}
				break;
			}
		case MMI_REPAIR:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKEBANK ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKEBANK ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKETRADE ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKETRADE ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKESHOP  ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKESHOP  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKEBEAUTY ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKEBEAUTY ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					//Summon Angel Using
					g_WndMng.PutString( prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndVendorBase();
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TRADEING ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TRADEING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_STILL ), NULL, prj.GetTextColor(TID_GAME_REPAIR_STILL ) );
					break;
				}
				g_WndMng.m_pWndRepairItem = new CWndRepairItem;
				g_WndMng.m_pWndRepairItem->Initialize( &g_WndMng, APP_REPAIR );
				break;
			}
		case MMI_BANKING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOBANK ), NULL, prj.GetTextColor(TID_GAME_TRADENOBANK ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor( TID_GAME_REPAIR_NOACTION ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKNOTRADE  ), NULL, prj.GetTextColor(TID_GAME_BANKNOTRADE  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING   ), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING   ), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					//Summon Angel Using
					g_WndMng.PutString( prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING) );
					break;
				}
				if( g_pPlayer->IsChaotic() )
				{
					CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( g_pPlayer->GetPKPropensity() );
					if( !Propensity.nBank )
					{
						g_WndMng.PutString( prj.GetText(TID_GMAE_CHAOTIC_NOT_BANK) , NULL, prj.GetTextColor(TID_GMAE_CHAOTIC_NOT_BANK) );
						break;
					}
				}
				g_DPlay.SendOpenBankWnd( NULL_ID );
				break;
			}
		case	MMI_GUILDBANKING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOBANK ), NULL, prj.GetTextColor(TID_GAME_TRADENOBANK ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor( TID_GAME_REPAIR_NOACTION ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKNOTRADE  ), NULL, prj.GetTextColor(TID_GAME_BANKNOTRADE  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 메이크업을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING   ), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING  ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					//Summon Angel Using
					g_WndMng.PutString( prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				g_DPlay.SendOpenGuildBankWnd();
				break;
			}
		case MMI_RANK_GUILD:
			{
				SAFE_DELETE( g_WndMng.m_pWndRankGuild );
				g_WndMng.m_pWndRankGuild = new CWndRankGuild;
				g_WndMng.m_pWndRankGuild->Initialize( &g_WndMng, APP_RANK_GUILD );
				
				g_DPlay.SendGuildRank( CGuildRank::Instance.m_Version );
				break;
			}
		case MMI_RANK_WAR:
			{
				SAFE_DELETE( g_WndMng.m_pWndRankWar );
				g_WndMng.m_pWndRankWar = new CWndRankWar;
				g_WndMng.m_pWndRankWar->Initialize( &g_WndMng, APP_RANK_WAR );
				g_DPlay.SendGuildRank( CGuildRank::Instance.m_Version );
				break;
			}
		case MMI_RANK_INFO:
			{
				SAFE_DELETE( g_WndMng.m_pWndRankInfo );
				g_WndMng.m_pWndRankInfo = new CWndRankInfo;
				g_WndMng.m_pWndRankInfo->Initialize( &g_WndMng, APP_RANK_INFO );
				g_DPlay.SendGuildRank( CGuildRank::Instance.m_Version );
				break;
			}
		case MMI_INVITE_PARTY: // 파티참여
			{ 
				if( pFocusMover->IsPlayer() )
					InviteParty( pFocusMover->m_idPlayer );
				break;
			}
		case MMI_INVITE_COMPANY:
			{
				if( pFocusMover->IsPlayer() )
					InviteCompany( pFocusMover->GetId() );
				break;
			}
		case MMI_DUEL:
			if( pFocusMover->IsPlayer() )
			{
				if( pFocusMover->IsDie() )
					break;
				g_DPlay.SendDuelRequest( g_pPlayer, pFocusMover );		// 상대에게 맞짱뜨자고 메시지 날림.
			}
			break;
		case MMI_DUEL_PARTY:
			if( pFocusMover->IsPlayer() )
			{
				if( pFocusMover->IsDie() )
					break;
				g_DPlay.SendDuelPartyRequest( g_pPlayer, pFocusMover );		// 상대에게 한판뜨자고 메시지 날림.
			}
			break;
		case MMI_TRACE:
			if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			{
				m_objidTracking	= pFocusMover->GetId();
				g_pPlayer->SetDestObj( pFocusMover );
			}
			break;
		case MMI_CHEER:
			if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			{
				if( pFocusMover->IsPlayer() )
					g_DPlay.SendCheering( pFocusMover->GetId() );				
			}
			break;
		case MMI_PIERCING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_TEST );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REMOVE_ELEM );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_PIERCING );
				if( pWndBaseBuf )
					break;
				SAFE_DELETE( g_WndMng.m_pWndPiercing );
				g_WndMng.m_pWndPiercing = new CWndPiercing;
				g_WndMng.m_pWndPiercing->Initialize( &g_WndMng, APP_PIERCING );
			}
			break;
		case MMI_UPGRADE:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_TEST );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REMOVE_ELEM );
				if( pWndBaseBuf )
					break;
				SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
				g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
				g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
			}
			break;
		case MMI_CHANGEELEM:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
				if( pWndBaseBuf )
					break;
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_TEST );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REMOVE_ELEM );
				if( pWndBaseBuf )
					break;

				SAFE_DELETE( g_WndMng.m_pWndCommerialElem );
				g_WndMng.m_pWndCommerialElem = new CWndCommercialElem;
				g_WndMng.m_pWndCommerialElem->Initialize( &g_WndMng, APP_COMMERCIAL_ELEM );
				
			}
			break;

		case MMI_INPUT_REWARD: // 현상금 걸기
			{
			// 현상범 리스트 요청 패킷
				g_DPlay.SendWantedName();
			}
			break;
		case MMI_SHOW_REWARD:  // 현상범 목록보기
			{
				SAFE_DELETE( g_WndMng.m_pWanted );
			// 현상범 리스트 요청 패킷
				g_DPlay.SendWantedList();
			}
			break;
		case MMI_PET_FOODMILL:	//먹이 제조기
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_PET_FOODMILL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_NOWUSING), NULL, prj.GetTextColor(TID_GAME_NOWUSING) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndPetFoodMill );
				g_WndMng.m_pWndPetFoodMill = new CWndPetFoodMill;
				g_WndMng.m_pWndPetFoodMill->Initialize();
			}
			break;
		case MMI_PET_STATUS:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_PET_STATUS );
				if( pWndBaseBuf )
				{
					g_WndMng.m_pWndPetStatus->Destroy();
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndPetStatus );
				g_WndMng.m_pWndPetStatus = new CWndPetStatus;
				g_WndMng.m_pWndPetStatus->Initialize();				
			}
			break;
		case MMI_PET_RELEASE:
			{
				g_DPlay.SendPetRelease();
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_PET_STATUS );
				if( pWndBaseBuf )
					g_WndMng.m_pWndPetStatus->Destroy();
			}
			break;
		case MMI_BUFFPET_STATUS:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_BUFFPET_STATUS );
				if( pWndBaseBuf )
				{
					g_WndMng.m_pWndBuffPetStatus->Destroy();
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndBuffPetStatus );
				g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
				g_WndMng.m_pWndBuffPetStatus->Initialize();	
			}

			break;

		case MMI_BUFFPET_RELEASE:
			{
				CItemElem* pItem = g_pPlayer->GetVisPetItem( );
				if( pItem )
					g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pItem->m_dwObjId ), g_pPlayer->GetId() );
			}
			break;
#ifdef __JEFF_11_4
		case MMI_ARENA_ENTER:
				g_DPlay.SendHdr( PACKETTYPE_ARENA_ENTER );
				break;
		case MMI_ARENA_EXIT:
				g_DPlay.SendHdr( PACKETTYPE_ARENA_EXIT );
				break;
#endif	// __JEFF_11_4
#ifdef __SYS_ITEMTRANSY
		case MMI_ITEM_TRANSY:
			{
				CWndItemTransy* pWndItemTransy = (CWndItemTransy*)g_WndMng.GetWndBase( APP_ITEM_TRANSY );
				if( !pWndItemTransy )
				{
					pWndItemTransy = new CWndItemTransy;
					pWndItemTransy->Initialize( &g_WndMng );
					pWndItemTransy->Init( nullptr );
				}
			}
			break;
#endif //__SYS_ITEMTRANSY
		case MMI_TELEPORTER:
			{
#ifdef __IMPROVE_MAP_SYSTEM
				CWndMapEx* pWndMapEx = ( CWndMapEx* )g_WndMng.CreateApplet( APP_MAP_EX );
				if( pWndMapEx == NULL )
				{
					break;
				}

				pWndMapEx->SetConstructionMode( CWndMapEx::TELEPORTATION );
				pWndMapEx->InitializeTeleportationInformation( pFocusMover );
#else // __IMPROVE_MAP_SYSTEM
				//sun!! DO: 전체맵을 띄우고 원하는 지역을 클릭 -> 해당 인덱스 전송 
				CWorldMap::GetInstance()->SetTelMode( TRUE );
				CWorldMap::GetInstance()->LoadWorldMap();
				CWorldMap::GetInstance()->ProcessingTeleporter( pFocusMover );

				//g_DPlay.SendTeleporterReq( pFocusMover->m_szCharacterKey, 0 );
#endif // __IMPROVE_MAP_SYSTEM
			}
			break;
#ifdef __TRADESYS
		default:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_DIALOG_EVENT );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_NOWUSING), NULL, prj.GetTextColor(TID_GAME_NOWUSING) );
					break;
				}
				
				SAFE_DELETE( g_WndMng.m_pWndDialogEvent );
				g_WndMng.m_pWndDialogEvent = new CWndDialogEvent;
				g_WndMng.m_pWndDialogEvent->SetMMI(nID);
				g_WndMng.m_pWndDialogEvent->Initialize();			
			}
			break;
#endif //__TRADESYS
		case MMI_SMELT_SAFETY_GENERAL:
			{
				if( g_pPlayer->m_vtInfo.GetOther() || g_pPlayer->m_vtInfo.VendorIsVendor() )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16) );
					break;
				}

				if(g_WndMng.m_pWndSmeltSafety != NULL)
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
				g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_NORMAL);
				if(g_WndMng.m_pWndSmeltSafety != NULL)
					g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
				break;
			}
		case MMI_SMELT_SAFETY_ACCESSORY:
			{
				if(g_pPlayer->m_vtInfo.GetOther() || g_pPlayer->m_vtInfo.VendorIsVendor() )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16) );
					break;
				}

				if(g_WndMng.m_pWndSmeltSafety != NULL)
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
				g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_ACCESSARY);
				if(g_WndMng.m_pWndSmeltSafety != NULL)
					g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
				break;
			}
		case MMI_SMELT_SAFETY_PIERCING:
			{
				if(g_pPlayer->m_vtInfo.GetOther() || g_pPlayer->m_vtInfo.VendorIsVendor() )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16) );
					break;
				}

				if(g_WndMng.m_pWndSmeltSafety != NULL)
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
				g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_PIERCING);
				if(g_WndMng.m_pWndSmeltSafety != NULL)
					g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
				break;
			}
		case MMI_SMELT_SAFETY_ELEMENT:
			{
				if(g_pPlayer->m_vtInfo.GetOther() || g_pPlayer->m_vtInfo.VendorIsVendor() )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR16 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR16 ) );
					break;
				}
				if( g_WndMng.m_pWndSmeltSafety )
					SAFE_DELETE( g_WndMng.m_pWndSmeltSafety );
				g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety( CWndSmeltSafety::WND_ELEMENT );
				if( g_WndMng.m_pWndSmeltSafety )
					g_WndMng.m_pWndSmeltSafety->Initialize( NULL );
				break;
			}
#ifdef __QUIZ
		case MMI_QUIZ_ENTRANCE:
			{
				SAFE_DELETE( g_WndMng.m_pWndQuizEventConfirm );
				g_WndMng.m_pWndQuizEventConfirm = new CWndQuizEventConfirm( TRUE );
				if( g_WndMng.m_pWndQuizEventConfirm )
				{
					g_WndMng.m_pWndQuizEventConfirm->Initialize(&g_WndMng);
					g_WndMng.m_pWndQuizEventConfirm->SetString( _T( prj.GetText( TID_GAME_QUIZ_TELEPORT_QUIZ_IN ) ) );
				}
				break;
			}
		case MMI_QUIZ_TELE_QUIZZONE:
			{
				if( CQuiz::GetInstance()->GetState() == CQuiz::QE_OPEN || CQuiz::GetInstance()->GetState() == CQuiz::QE_WATCHINGZONE_OPEN )
					g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_QUIZ );
				else
					g_WndMng.PutString( prj.GetText( TID_GAME_QUIZ_DO_NOT_TELEPORT ), NULL, prj.GetTextColor( TID_GAME_QUIZ_DO_NOT_TELEPORT ) );
				break;
			}
		case MMI_QUIZ_EXIT:
			{
				SAFE_DELETE( g_WndMng.m_pWndQuizEventConfirm );
				g_WndMng.m_pWndQuizEventConfirm = new CWndQuizEventConfirm( FALSE );
				if( g_WndMng.m_pWndQuizEventConfirm )
				{
					g_WndMng.m_pWndQuizEventConfirm->Initialize(&g_WndMng);
					g_WndMng.m_pWndQuizEventConfirm->SetString( _T( prj.GetText( TID_GAME_QUIZ_TELEPORT_QUIZ_OUT ) ) );
				}
				break;
			}
#endif // __QUIZ

		case MMI_GUILDHOUSE_CHARGE:	//유지비 
			{
				bool bAuthority = false;
				CGuild* pGuild = g_pPlayer->GetGuild( );
				if( pGuild )
					bAuthority = pGuild->IsAuthority( g_pPlayer->m_idPlayer, GuildPower::GuildHouseUpKeep );

				if( !bAuthority )		//유지비 권한이 없으면 
				{
					g_WndMng.PutString( GETTEXT( TID_GAME_GUILDHOUSE_TAX_LEVEL ) );
					break;
				}

				//기존창 있으면 날려버리고 
				if( g_WndMng.m_pWndUpkeep )
					SAFE_DELETE( g_WndMng.m_pWndUpkeep );

				if( !g_WndMng.m_pWndUpkeep )
				{
					g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
					g_WndMng.m_pWndUpkeep->Initialize( &g_WndMng, APP_CONFIRM_BUY_);	//gmpbigsun( 2010_05_12 ) : 종료버그수정 , 이녀석의 부모가 월드mng여서 접속종료하다 뻗음.
					
				}
			}
			break;

 		case MMI_GUILDHOUSE_ENTER:  
			g_DPlay.SendPacket<PACKETTYPE_GUILDHOUSE_ENTER>();
 			break;

		case MMI_GUILDHOUSE_OUT :
			g_DPlay.SendPacket<PACKETTYPE_GUILDHOUSE_GOOUT>();
			break;

		case MMI_GUILDHOUSE_SALE:		//길드하우스 구입
			g_DPlay.SendPacket<PACKETTYPE_GUILDHOUSE_BUY>();
			break;
#ifdef __GUILD_HOUSE_MIDDLE
		case MMI_GUILDHOUSE_AUCTION01:	//중형 길드하우스 입찰
			{
				if( g_WndMng.m_pWndGHBid )
					SAFE_DELETE( g_WndMng.m_pWndGHBid );

				g_WndMng.m_pWndGHBid = new CWndGuildHouseBid;
				g_WndMng.m_pWndGHBid->Initialize( );
			}
			break;
#endif //__GUILD_HOUSE_MIDDLE
	
		case MMI_INVITE_CAMPUS:			// 사제 맺기
			{
				g_DPlay.SendInviteCampusMember( pFocusMover->m_idPlayer );
				g_DPlay.SendQueryPlayerData( pFocusMover->m_idPlayer );
				break;
			}
		case MMI_REMOVE_CAMPUS:
			{
				if( g_WndMng.m_pWndCampusSeveranceConfirm )
					SAFE_DELETE( g_WndMng.m_pWndCampusSeveranceConfirm );
				g_WndMng.m_pWndCampusSeveranceConfirm = new CWndCampusSeveranceConfirm( pFocusMover->m_idPlayer, pFocusMover->GetName() );
				g_WndMng.m_pWndCampusSeveranceConfirm->Initialize( NULL );
				break;
			}

		} // switch
		// 포커스를 윈도로 돌려주어야 매뉴가 닫힌다.
		SetFocus();
	}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndWorld::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );

}
void CWndWorld::OnSize( UINT nType, int cx, int cy )
{
	if( m_nWinSize != WSIZE_MAX )
	{
		CRect rectWnd = GetWndRect();
		CSize size2( rectWnd.Width(), rectWnd.Height() );
		CSize sizeDiv = size2;
		sizeDiv.cx %= 16;
		sizeDiv.cy %= 16;
		size2.cx /= 16; size2.cx *= 16;
		size2.cy /= 16; size2.cy *= 16;
		if( sizeDiv.cx ) size2.cx += 16;
		if( sizeDiv.cy ) size2.cy += 16;
		rectWnd.bottom = rectWnd.top + size2.cy;
		rectWnd.right = rectWnd.left + size2.cx;
		SetWndRect( rectWnd, FALSE );
	}
	AdjustWndBase();
	m_wndTitleBar.Replace(); 

	CWndBase::OnSize( nType, cx, cy );
}
LRESULT CWndWorld::WndMsgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

BOOL CWndWorld::UseFocusObj( CCtrl* pFocusObj )
{
	if( g_pPlayer->m_pActMover->IsFly() )	return FALSE;	// 날고있는중이었으면 취소
	if( g_pPlayer->m_pActMover->IsActJump() )	return FALSE;	// 점프중이면 취소
	if( g_pPlayer->m_pActMover->IsActAttack() )	return FALSE;	// 어택 동작중이면 취소.
	if( g_pPlayer->m_pActMover->IsDie() )		return FALSE;	// 플레이어가 죽은상태면 취소.
	if( IsInvalidObj(pFocusObj) )			return FALSE;	// 타겟이 거시기한 상태면 취소
	if( pFocusObj->GetType() == OT_OBJ )	return FALSE;	// 타겟이 OT_OBJ면 취소
	if( g_pPlayer->m_dwMode & DONMOVE_MODE )	return FALSE;	// 돈무브 모드면 암것도 못함.
//	if( g_pPlayer->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)	return FALSE;
	if( g_pPlayer->GetAdjParam( DST_CHRSTATE ) & CHS_SETSTONE)	return FALSE;

//	DEBUG_CHATMSG( "타겟사용", NULL, 0xffffffff );
	
	CWorld* pWorld = g_WorldMng();
	CRect rect = GetClientRect();

	if( pFocusObj->GetType() == OT_ITEM || pFocusObj->GetType() == OT_CTRL )
	{
		if( m_bLButtonDown && !m_bLButtonDowned )
			g_pPlayer->CMD_SetUseItem( (CCtrl*)pFocusObj );	// 목표가 주어지면 자동 이동 
		m_bLButtonDowned = m_bLButtonDown;
	}
	else if( pFocusObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pFocusObj;
		MoverProp* pMoverProp = pMover->GetProp();

		// 여기는 무조건 공격인데.. 단거리 공격은 목표만 세팅해주고 장거리 공격은 실제 공격 코드 삽입 

		CItemElem* pItemElem = g_pPlayer->GetWeaponItem();
		if( pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_WAND ) 
		{
			// 만약 플레이어가 완드류를 들고 있었다면 여기서 처리하지 않고 LBUTTON_UP, 자동공격 이모티콘에서 처리해준다. 
		}
		else
		{
			if( g_pPlayer->m_pActMover->IsSit() )	// 앉아있는 상태였으면 해제
			{
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_STANDUP );
				}
			} 
			else
			{
				// 연속공격을 여기서 시작 
				if( pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_BOW )
				{
					if( !m_bLButtonDown )
						g_pPlayer->CMD_SetRangeAttack( pFocusObj->GetId(), 0 );		// 원거리공격 패턴에 들어감 
				}
				else
					g_pPlayer->CMD_SetMeleeAttack( pFocusObj->GetId() );		// 근접패턴공격 들어감.
			}
		}
	}
	else
	{
		return FALSE;	// 그외의 오브젝트타입은 일단 실패로 하자.
	}

	return TRUE;
}

BOOL CWndWorld::UseSkillToFocusObj( CCtrl* pFocusObj )
{
//	if( pFocusObj && IsValidObj( pFocusObj ) && pFocusObj->GetType() != OT_OBJ )
//	if( pFocusObj && pFocusObj->GetType() != OT_OBJ )
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		BOOL bRet = pTaskBar->UseSkillQueue( (CCtrl*)pFocusObj );
		LPSKILL pSkill = pTaskBar->GetCurrentSkillQueue();
		if( bRet )
		{
			const ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
			if( pItemProp )
			{   
				ItemProp* pSkillProp;
				if(pSkill != NULL)				
					pSkillProp = prj.GetSkillProp( pSkill->dwSkill );

				// 손에 들고 있는게 스태프나 치어스틱이 아닐때만 자동공격.
				if( pItemProp->dwItemKind3 != IK3_STAFF && pItemProp->dwItemKind3 != IK3_CHEERSTICK && 
					pSkillProp != NULL && pSkillProp->dwExeTarget != EXT_SELFCHGPARAMET )
				{
					if( g_Option.m_bAutoAttack )		// 여기야여기 - 자동공격 ON
						m_bAutoAttack = TRUE;		// 스킬공격 시작되면 자동공격도 시작.
				}
			}
		}
		return bRet;
	}
	
	return FALSE;
}
CObj* CWndWorld::PickObj( POINT point, BOOL bOnlyNPC )
{
	CWorld* pWorld = g_WorldMng();
	CRect rectClient = GetClientRect();
	CObj* pObj;

	// 박스 피킹먼저.
	pObj = pWorld->PickObject_Fast( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, 
									OF_MOVER|OF_ITEM|OF_CTRL, g_pPlayer, TRUE, bOnlyNPC );
	if( pObj )
	{
		if( pObj->GetType() == OT_CTRL )
		{
			if( !pWorld->GetObjFocus() )
				m_pSelectRenderObj = pObj;
			else
				m_pSelectRenderObj = NULL;
		}
		else if( pObj->GetType() == OT_MOVER )
		{
			DWORD dwClass = ((CMover *)pObj)->GetProp()->dwClass;
			switch( dwClass )
			{
			case RANK_MIDBOSS:
			case RANK_SUPER:
			case RANK_BOSS:
			case RANK_MATERIAL:
				// 대형몹의 경우엔 정밀피킹을 한번더 한다
				pObj = pWorld->PickObject( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, OF_MOVER|OF_ITEM, g_pPlayer, NULL, FALSE, bOnlyNPC );
				break;
			}

	#if defined(__CLIENT)
			if( pObj && pObj->GetType() == OT_MOVER )
			{
				// 길드컴뱃 피킹 옵션...
				CMover* pSelectMover = (CMover *)pObj;
				
				if( pSelectMover->IsMode( GCWAR_NOT_CLICK_MODE ) )
				{
					return NULL;
				}
			}
	#endif //defined(__CLIENT)
			
			if( !pWorld->GetObjFocus() )
				m_pSelectRenderObj = pObj;
			else
				m_pSelectRenderObj = NULL;
		}
 	}
	return pObj;
}

CObj* CWndWorld::SelectObj( POINT point )
{
	CWorld* pWorld = g_WorldMng();

	BOOL bOnlyNPC = FALSE;
	if( GetAsyncKeyState( VK_TAB ) & 0x8000 )	// 탭키를 누르고 있으면 NPC만 셀렉트 된다.
		bOnlyNPC = TRUE;
	
	CObj* pObj = PickObj( point, bOnlyNPC );
	if( pObj )
	{
		if( pObj->GetType() == OT_MOVER )
		{
			// 길드컴뱃중에는 죽은케릭을 피킹 금지
			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			{
				if( ((CMover*)pObj)->IsDie() )
					return NULL;
			}
			if( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE )	) // 대상이 투명모드일땐 타겟 안됨.
			{
				return NULL;
			}			
		}

		if(!m_bAutoAttack)
			pWorld->SetObjFocus( pObj );
	}
	return pObj;
}

void CWndWorld::SetNextTarget()
{
	CWorld* pWorld = g_WorldMng();

	if(m_pNextTargetObj && pWorld)
	{
		if( m_pNextTargetObj->GetType() == OT_MOVER )
		{
			// 길드컴뱃중에는 죽은케릭을 피킹 금지
			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			{
				if( ((CMover*)m_pNextTargetObj)->IsDie() )
					return;
			}
			if( ((CMover*)m_pNextTargetObj)->IsMode( TRANSPARENT_MODE )	) // 대상이 투명모드일땐 타겟 안됨.
			{
				return;
			}			
		}
		pWorld->SetObjFocus( m_pNextTargetObj );
		if(m_pNextTargetObj)
		{
			CObj* pObj = pWorld->GetObjFocus();
			m_pNextTargetObj = NULL;
			CMover* pMover = (CMover*)pObj;
			if(pMover->IsPeaceful() == FALSE)
				m_pRenderTargetObj = pObj;
		}
	}
	m_bAutoAttack = FALSE;
}

CObj* CWndWorld::HighlightObj( POINT point )
{
	/*
	CWorld* pWorld = g_WorldMng();
	CRect rectClient = GetClientRect();
	CObj* pObj = pWorld->PickObject_Fast( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, OF_MOVER|OF_ITEM,  CMover::GetActiveMover(), TRUE );
	if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsDie() )
		pObj = NULL;
		*/
	BOOL bOnlyNPC = FALSE;
	if( GetAsyncKeyState( VK_TAB ) & 0x8000 )	// 탭키를 누르고 있으면 NPC만 셀렉트 된다.
		bOnlyNPC = TRUE;
	CObj* pObj = PickObj( point, bOnlyNPC );	
	CObj::m_pObjHighlight = pObj;
	if( pObj && pObj->GetType() == OT_ITEM )
	{
		CRect rect;
		GetBoundRect( pObj, &rect );
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_WndMng.PutToolTip_Item( ((CItem*)pObj)->m_pItemBase, point, &rect );
	}
	
	if(pObj == NULL)
		m_pSelectRenderObj = NULL;
	return pObj;
}

// 누적된 공격게이지를 구한다. ( 게이지 단계:0, 1, 2, 3, 4)
// 예외:  0  - 모으지 않은 상태, -1 - 3단계 발사후 딜레이 상태
int CWndWorld::GetGaugePower( int* pnValue )
{
	int nTime = GetTickCount() - m_dwPowerTick;
	if( nTime < 0 )
	{
		if( pnValue )
			*pnValue = 0;
		return -1;
	}

	nTime = std::min( nTime, MAX_POWER_GAUGE );
	if( pnValue )
		*pnValue = nTime;

	return MulDiv( nTime, 4, MAX_POWER_GAUGE );		
}

void CWndWorld::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL || g_pPlayer->IsDie() ) 
		return;

	if( m_nMouseMode == 0 && m_bRButtonDown == FALSE )
 		ReleaseCapture();

	CWorld* pWorld = g_WorldMng.Get();
	m_bFlyMove = FALSE;

	// 완드,활 어택 
	CObj* pFocusObj = pWorld->GetObjFocus();
	if( pFocusObj && m_bSelectTarget && m_timerFocusAttack.IsTimeOut() == TRUE )
	{
		if( g_pPlayer->IsAttackAble(pFocusObj) )	// pFocusObj를 공격할수있는지 검사.
		{
			if( pFocusObj->GetType() == OT_MOVER )
			{
				OBJID idTarget = ((CCtrl*)pFocusObj)->GetId();		
	
				int nPower = GetGaugePower( NULL );	
				if( nPower >= 0 )
				{
					m_dwPowerTick = GetTickCount();

					const ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
					if( pItemProp->dwItemKind3 == IK3_WAND )
						g_pPlayer->CMD_SetMagicAttack( idTarget, nPower );
					else if( pItemProp->dwItemKind3 == IK3_BOW )
						g_pPlayer->CMD_SetRangeAttack( idTarget, nPower );	

				}	
			}
		}
	}
	if(!CDeployManager::GetInstance()->IsReady())
	ControlPlayer( WM_LBUTTONUP, point );

	m_bSelectTarget = FALSE;
}


// pTarget에 맞는 메뉴를 띄운다.
void CWndWorld::ShowMoverMenu( CMover* pTarget )
{
	// 메뉴를 띄우기에 적당한 거리인지 확인.
	D3DXVECTOR3 vDist = pTarget->GetPos() - g_pPlayer->GetPos();
	FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
	if( (fDistSq < 20.0f * 20.0f) && ( m_bRButtonDown == FALSE || ( pTarget->IsPlayer() && m_bRButtonDown == TRUE ) ) )
	{
		g_pPlayer->ClearDest();
		m_wndMenuMover.DeleteAllMenu();
		m_wndMenuMover.SetLargeWidth( 0 );

		// 이걸 안해주면 이미 SetCapture가 된 상태라서 매시지가 매뉴에 안간다.
		// 처음 눌렀을 때 그냥 닫혀버리게 된다.
		ReleaseCapture();
		BOOL bView = FALSE;

		if( pTarget->IsPlayer() )
		{

			if( pTarget->m_vtInfo.IsVendorOpen() )
			{
				m_wndMenuMover.AddButton( MMI_QUERYEQUIP , prj.GetText(TID_MMI_QUERYEQUIP) );
			}
			else
			{
			m_wndMenuMover.AddButton( MMI_TRADE        , prj.GetText( TID_MMI_TRADE ) );
			m_wndMenuMover.AddButton( MMI_ADD_MESSENGER, prj.GetText( TID_MMI_ADD_MESSENGER ) );
			m_wndMenuMover.AddButton( MMI_INVITE_PARTY , prj.GetText( TID_MMI_INVITE_PARTY ) );

			
			if (CGuild * pGuild = g_pPlayer->GetGuild()) {
				if (pGuild->IsAuthority(g_pPlayer->m_idPlayer, GuildPower::Invitation)) {
					m_wndMenuMover.AddButton( MMI_INVITE_COMPANY, prj.GetText(TID_MMI_INVITE_COMPANY));
				}
			}


/*#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
			if( !g_pPlayer->IsBaseJob() && pTarget->IsDie() == FALSE )	// 레벨이 15이상일때만 메뉴가 나타남.
			{
				BOOL bInsert = TRUE;
				if(g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
					bInsert = FALSE;
				if(fDistSq < 10.0f)
					bInsert = FALSE;

				if( bInsert )
				{
					m_wndMenuMover.AddButton( MMI_DUEL, prj.GetText( TID_MMI_DUEL ) );
				}
			}
#else	// __VER >= 8  */
			if( g_eLocal.GetState( EVE_18 ) == 1 )
			{
				if( pTarget->IsDie() == FALSE )	// 레벨이 15이상일때만 메뉴가 나타남.
				{
					BOOL bInsert = FALSE;

					//if( ::GetLanguage() == LANG_KOR )	
					//	bInsert = ( g_pPlayer->IsRegionAttr( RA_SAFETY ) == FALSE );
					//else						
						bInsert = ( g_pPlayer->IsRegionAttr( RA_FIGHT ) && pTarget->IsRegionAttr( RA_FIGHT ) );	// __JAPAN_PVP


					if( bInsert )
					{
						m_wndMenuMover.AddButton( MMI_DUEL, "Duel - 1 vs 1" );
						if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) == TRUE )
							m_wndMenuMover.AddButton( MMI_DUEL_PARTY, "Duel - Party vs Party" );
					}
				}
			}
//#endif	// __VER >= 8  

			m_wndMenuMover.AddButton( MMI_TRACE, prj.GetText( TID_MMI_TRACE ) );
			m_wndMenuMover.AddButton( MMI_QUERYEQUIP , prj.GetText(TID_MMI_QUERYEQUIP) );
			m_wndMenuMover.AddButton( MMI_CHEER , prj.GetText( TID_MMI_CHEER ) );
			CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus();
			if( pCampus == NULL )
				m_wndMenuMover.AddButton( MMI_INVITE_CAMPUS , prj.GetText(TID_GAME_MENU_CAMPUS_INVITATION) );
			else
			{
				if( pCampus->IsMember( pTarget->m_idPlayer ) )
					m_wndMenuMover.AddButton( MMI_REMOVE_CAMPUS , prj.GetText(TID_GAME_MENU_CAMPUS_SEVERANCE) );
				else
					m_wndMenuMover.AddButton( MMI_INVITE_CAMPUS , prj.GetText(TID_GAME_MENU_CAMPUS_INVITATION) );
			}
			}
			bView = TRUE;
		}
		else
		{
			LPCHARACTER lpCharacter = pTarget->GetCharacter();
			if( lpCharacter )
			{
				int nCount = 0;
				UINT nMenu = 0; 
				for( int j = 0; j < MAX_MOVER_MENU ; ++j )
				{
					if( lpCharacter->m_abMoverMenu[ j ] )
					{
						if( nCount == 0 )
							nMenu = j;

						++nCount;
						if( 1 < nCount )
						{
							break;
						}
					}
				}

				if( 1 < nCount )
				{
					for( int i = 0; i < MAX_MOVER_MENU; i++ )
					{
						if( i == MMI_QUEST )
						{
							// 이 캐릭터에게 퀘스트가 있나?
							// 퀘스트prop를 뒤져서 동일 이름의 prop이 있는지 확인.
							// 확인 되었으면 일단 퀘스트는 존재하는 것.
							// prop에서 해당 캐릭터에 해당하는 퀘스트를 수집한다.
							// 수집이 완료되면, 내기수행한 퀘스트 갯수와 비교한다.
							// 내가 해당 캐릭터에 해당하는 퀘스트보다 적은 갯수이면 더 할 수 있으므로
							// 퀘스트 메뉴 활성화. 
						}
						if( lpCharacter->m_abMoverMenu[ i ] )
						{
							if( i == MMI_GUILDBANKING )
							{
								CGuild* pGuild	= g_pPlayer->GetGuild();
								if( pGuild && g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
									m_wndMenuMover.AddButton( i, prj.GetText( TID_MMI_DIALOG + i ) );
								bView = TRUE;
							}
#ifdef __JEFF_11_4
							else if( i == MMI_ARENA_ENTER )
							{
								if( g_pPlayer && !g_pPlayer->IsBaseJob() )
									m_wndMenuMover.AddButton( i, prj.GetText( TID_MMI_DIALOG + i ) );
								bView = TRUE;
							}
#endif	// __JEFF_11_4
							else
							{
								m_wndMenuMover.AddButton( i, prj.GetText( TID_MMI_DIALOG + i ) );
								bView = TRUE;
							}
						}
					}
				}
				else
				{
					OnCommand( nMenu, 0, &m_wndMenuMover );
				}
			}
		}

		if( bView )
		{
			CRect rectBound;
			GetBoundRect( pTarget, &rectBound );
			m_wndMenuMover.Move( CPoint( rectBound.right, rectBound.top ) );
			m_wndMenuMover.SetVisible( TRUE );
			m_wndMenuMover.SetFocus();
		} else
		{
			m_wndMenuMover.SetVisible( FALSE );
		}
	}
	else
	{
		if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			g_pPlayer->SetDestObj( pTarget );	// 목표가 주어지면 자동 이동 
	}
}

void CWndWorld::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_bViewMap )
		return;
	
	m_objidTracking	= NULL_ID;

	if( g_pPlayer == NULL || g_pPlayer->IsDie() ) 
		return;
	if( g_pPlayer->m_vtInfo.VendorIsVendor() )
		return;
#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
		return;
#endif // __S_SERVER_UNIFY
	if( g_WndMng.GetWndVendorBase() )
		return;
	if( g_pPlayer->m_dwMode & DONMOVE_MODE )	// 돈무브 모드면 암것도 못함.
		return;
//	if( g_pPlayer->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return;
	if( g_pPlayer->GetAdjParam( DST_CHRSTATE ) & CHS_SETSTONE)		return;

#ifdef __EVE_MINIGAME
	if( g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO) )
		return;
	if( g_WndMng.GetWndBase(APP_MINIGAME_DICE) )
		return;
#endif

	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_KAWIBAWIBO) )
		return;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_DICE) )
		return;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_ARITHMETIC) )
		return;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_STOPWATCH) )
		return;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_TYPING) )
		return;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_CARD) )
		return;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_LADDER) )
		return;	

	if( g_WndMng.GetWndBase(APP_SMELT_JEWEL) )
		return;

	if( GetBuffIconRect( II_SYS_SYS_SCR_RETURN, point ) )	// 귀환의 두루마리 아이콘을 클릭하면 더블클릭까지 검사 
		return;
	if( GetBuffIconRect( II_SYS_SYS_SCR_PARTYSUMMON, point ) )	// 귀환의 두루마리 아이콘을 클릭하면 더블클릭까지 검사 
		return;

	if(g_eLocal.GetState( EVE_PK ))
	{
		if( GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_RED, point) ||
			GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_GREEN, point) ||
			GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_BLUE, point) ||
			GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_WHITE, point) )
		{
			//Create 100 % Angel Item.
			g_DPlay.SendPacket<PACKETTYPE_ANGELBUFF>();
			return;
		}
	}


	if( GetBuffIconRect(II_SYS_SYS_SCR_PET_FEED_POCKET, point) )
		return;		

	if( g_WndMng.GetWndBase(APP_WEBBOX) || g_WndMng.GetWndBase(APP_WEBBOX2) )
		return;

	m_timerLButtonDown.Set( 0 );
	// R버튼클릭 200ms이내라면 동시에 눌러진 것으로 판단 -> 스킬 사용 또는 포커싱을 스킵함.
	if( m_timerRButtonDown.GetLeftTime() < 200 )
		return;
	if(CDeployManager::GetInstance()->IsReady())	
	{
		if(CDeployManager::GetInstance()->IsCollide())
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_HOUSING_INSTALL_FAIL01 ), NULL, prj.GetTextColor( TID_GAME_HOUSING_INSTALL_FAIL01 ) );
				return;
		}
		HOUSINGINFO *phousingInfo;
		phousingInfo = CDeployManager::GetInstance()->EndDeploy();
		g_DPlay.SendHousingReqSetupFurniture( *phousingInfo );
		return;
	}

	if( GuildDeploy()->IsReady( ) )
	{
		if( GuildDeploy()->IsCollide( ) )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_HOUSING_INSTALL_FAIL01 ), NULL, prj.GetTextColor( TID_GAME_HOUSING_INSTALL_FAIL01 ) );
				return;
		}
		
		HOUSING_ITEM* pItem = GuildDeploy()->EndDeploy();
		if( GUILDHOUSE_PCKTTYPE_SETUP == GuildHouse->GetMode( ) )
			GuildHouse->Setup( *pItem );
		else 
		if( GUILDHOUSE_PCKTTYPE_RESET == GuildHouse->GetMode( ) )
			GuildHouse->Reset( *pItem );
		return;
	}
	
	SetCapture();
	m_ptMouseOld = point;
	CWorld* pWorld = g_WorldMng.Get();

	CObj* pSelectOld = pWorld->GetObjFocus();
	CObj* pSelectObj = CObj::m_pObjHighlight;	// 현재 커서 대고 있는 오브젝트.
	if(m_pNextTargetObj == NULL && pSelectOld == NULL)
		m_bAutoAttack = FALSE;
	g_pPlayer->ClearCmd();
	if( pSelectObj && pSelectObj->GetType() == OT_MOVER )
	{
		CMover* pSelectMover = (CMover *)pSelectObj;
		if( m_bRButtonDown || m_nMouseMode == 1 )	// 우버튼 누르고 있거나 / 마우스 커서 사라진 모드일땐 무조건 안잡힌다.
		{
			m_bAutoAttack = FALSE;
			pSelectObj = NULL;
		}
		else
		{
			if( g_pPlayer->m_pActMover->IsFly() )
			{
				m_bAutoAttack = FALSE;
				if( pSelectMover->IsNPC() )
				{
					if( pSelectMover->IsPeaceful() == FALSE && pSelectMover->IsFlyingNPC() == FALSE )	
						pSelectObj = NULL;			// 비행못하는 몬스터는 셀렉트 안됨.
				}
				else if( pSelectMover->IsPlayer() )		
					pSelectObj = NULL;			// 플레이어도 셀렉트 안됨.
				else
					pSelectObj = SelectObj( point ); 
			} 
			else
			{
				pSelectObj = SelectObj( point );
				CMover* pMover = (CMover*)pSelectObj;
				if(pMover && (pMover->IsPlayer() || pMover->IsPeaceful() == FALSE)) //NPC를 제외한 몬스터 or 플레이어만 다음 타겟으로 지정
					m_pRenderTargetObj = pSelectObj;
				if( pSelectOld && pSelectOld == pSelectObj )  // 이미 셀렉트 되어 있고 새로 셀렉트한게 그넘이면
				{
					m_bAutoAttack = FALSE;
					if(m_pNextTargetObj != NULL) //다음 타겟이 있을 경우 공격하는 놈을 다시 선택하면 다음 타겟 제거
						m_pNextTargetObj = NULL;
					//if( g_Option.m_bAutoAttack )		// 여기야여기 - 자동공격 ON
					//	m_bAutoAttack = TRUE;					  // 자동 공격.
					const ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
					if( pItemProp )
					{   
						if( pItemProp->dwItemKind3 != IK3_WAND ) //Wand일 경우 AutoAttack을 하지 않음.
						{
							if( g_Option.m_bAutoAttack )		// 여기야여기 - 자동공격 ON
								m_bAutoAttack = TRUE;		// 스킬공격 시작되면 자동공격도 시작.
						}
					}	
				}
				else if( pSelectOld != NULL && m_bAutoAttack && pSelectOld != pSelectObj ) //이미 타겟이 잡혀 공격상태에서 다른 타겟을 잡을 경우
				{
					m_pNextTargetObj = pSelectObj;
				}
				else 
					m_bAutoAttack = FALSE;
			}
		}
	} 
	else
	{
		pSelectObj = SelectObj( point );
		m_bAutoAttack = FALSE;
		m_pNextTargetObj = NULL;
		m_pRenderTargetObj = NULL;
	}
	

		

//#ifdef __YAUTOATTACK
#if 0
	m_bAutoAttack = FALSE;
/*
	if( m_bAttackDbk && pSelectObj && pSelectObj->GetType() == OT_MOVER )
		m_bAutoAttack = TRUE;
	
	if( m_bAutoAttack == FALSE && pSelectObj && pSelectObj->GetType() == OT_MOVER )
		m_bAttackDbk = TRUE;
	else
		m_bAttackDbk  = FALSE;
*/
	if( m_bAttackDbk && pSelectObj && pSelectObj->GetType() == OT_MOVER )
		m_bAutoAttack = TRUE;
	
	if( m_bAutoAttack == FALSE && pSelectObj && pSelectObj->GetType() == OT_MOVER )
		m_bAttackDbk = TRUE;
	else
		m_bAttackDbk = FALSE;
	
	if( pSelectObj == NULL )
	{
		m_bAutoAttack = FALSE;
	}
#else //__YAUTOATTACK

//	m_bAutoAttack = FALSE;
//	if( (nFlags & MK_CONTROL) == MK_CONTROL )	// 컨트롤 누르면서 클릭했다.
//		if( pSelectObj && pSelectObj->GetType() == OT_MOVER )
//			m_bAutoAttack = TRUE;
#endif //__YAUTOATTACK

	m_dwNextSkill = NEXTSKILL_NONE;	// 왼쪽버튼 누르면 스킬예약 취소.

#ifdef __CLIENT
	// 스킬바 실행중에 왼쪽버튼 클릭했다면 스킬바 실행 취소
	CWndTaskBar* pTaskBar = (CWndTaskBar*)g_WndMng.m_pWndTaskBar;
	if( pTaskBar->m_nExecute )
		g_DPlay.SendEndSkillQueue();		// 스킬큐 실행 중단하라고 서버로 보냄.
#endif // Client

	// 스킬바 사용중에 이동하면 취소된다.
	if( g_pPlayer->m_dwFlag & MVRF_SKILL )		
		g_pPlayer->m_dwFlag &= (~MVRF_SKILL);

	if( pSelectObj == NULL ) 
	{
		m_bFreeMove = TRUE;
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			s_bAccKeyed	= FALSE;
			m_bFlyMove	= TRUE;
		}
	}
	else 
	{
		if( pSelectObj->GetType() == OT_MOVER )
		{

			if( g_eLocal.GetState( EVE_SCHOOL ) )
			{
				if( ( (CMover*)pSelectObj )->IsPlayer() )
					return;
			}

			CMover* pMover = (CMover*)pSelectObj;
			float	fDistant;

			if(g_Option.m_bZoomLimit)
				fDistant = 20.0f;
			else
				fDistant = 40.0f;

			if( pMover->IsPlayer() &&
				!pMover->IsActiveMover() &&
				pMover->m_vtInfo.IsVendorOpen() &&
				g_pPlayer->m_vtInfo.GetOther() == NULL &&
				pMover->m_fDistCamera < fDistant &&
				!(GetAsyncKeyState(VK_MENU) & 0x8000) ) //Alt Key 안 눌렀을 경우에만 진입
			{
				if( pMover->IsRegionAttr( RA_SAFETY ) )
				{
					g_DPlay.SendQueryPVendorItem( pMover->GetId() );				
				}
				else if( ( GetAsyncKeyState(VK_CONTROL) & 0x8000 ) == FALSE )
				{
					g_DPlay.SendQueryPVendorItem( pMover->GetId() );
				}
			}
			else if( pMover->IsPlayer() && FALSE == pMover->IsActiveMover() && g_bKeyTable[g_Neuz.Key.chTrace] )
			{
				if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// 비행중이 아닐때만 따라가기 된다.
				{	
					m_objidTracking	= pMover->GetId();
					g_pPlayer->SetDestObj( pMover );
				} 
			}	
			else
			{
				PLAYSND( pMover->GetProp()->dwSndIdle1 );
				// 메뉴를 띄우기
				if( pMover->IsPeaceful() && !g_pPlayer->IsFly() &&
				   ( pMover->IsPlayer() == FALSE || ( pMover->IsPlayer() && (GetAsyncKeyState(VK_MENU) & 0x8000) ) ) ) 
				{
					// 1:1길드 대전장 안에서는 플레이어 메뉴 사용 불가
					if(g_GuildCombat1to1Mng.IsPossibleMover(g_pPlayer) && g_GuildCombat1to1Mng.IsPossibleMover(pMover))
					{
						if(!pMover->IsPlayer())
							ShowMoverMenu( pMover );
					}
					else
					{
						m_bAutoAttack = FALSE;
						ShowMoverMenu( pMover );

// 						Acmd_Test* pNewAcmd = new Acmd_Test;
// 						pNewAcmd->_pModel = &m_meshArrow;
// 						InsertAcmd( pNewAcmd );
					}
				} 
			}

			m_bSelectTarget = TRUE;
			if( pSelectObj != pSelectOld )
			{
				m_timerFocusAttack.Reset();
				//static int a; a++;
				//TRACE( "fuck %d \n", a );
			}
			m_dwPowerTick = ::GetTickCount();
		}
		else if( pSelectObj->GetType() == OT_ITEM )
		{
			m_bSelectTarget = TRUE;
		}
		else if( pSelectObj->GetType() == OT_CTRL )
		{
			m_bSelectTarget = TRUE;
			ShowCCtrlMenu( (CCtrl*)pSelectObj );	
		}
	}

	ControlPlayer( WM_LBUTTONDOWN, point );
}

void CWndWorld::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( m_nMouseMode == 0 )	// 모드1일땐 릴리즈 해선 안된다.
		ReleaseCapture();
	ControlPlayer( WM_RBUTTONUP, point );
	if( point == m_ptMouseSpot && MenuException( point ) == TRUE )
	{
		CObj* pSelectObj = CObj::m_pObjHighlight;	// 현재 커서 대고 있는 오브젝트
		CMover* pMover = ( CMover* )pSelectObj;
		if( IsValidObj( pSelectObj ) && pSelectObj->GetType() == OT_MOVER && pMover->IsPlayer() )
		{
			m_bAutoAttack = FALSE;
			m_bSelectTarget = TRUE;
			g_WorldMng()->SetObjFocus( pSelectObj );
			ShowMoverMenu( pMover );
		}
	}
	CMover* pMover = g_pPlayer;
//	pMover->SendActMsg( OBJMSG_ACC_STOP );	// 가속중지
#ifndef __VRCAMERA
	if( g_pPlayer && g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock() )
		g_Neuz.m_camera.Unlock();
#endif
#ifdef __VRCAMERA
	if( m_bCameraMode == TRUE )
	{
		if( g_pPlayer && g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock() )
			g_Neuz.m_camera.Unlock();
		m_bCameraMode = FALSE;
	}
#endif
}
void CWndWorld::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( g_Option.m_nInterface == 0 )	// 구버전일때만
		m_objidTracking		= NULL_ID;	// 오른버튼 누르면 따라가기 해제된다.
	if( IsFullMax() && m_wndTitleBar.GetWindowRect( TRUE ).PtInRect( point ) )
		return;
	if( g_pPlayer == NULL ) 
		return;
	if(g_pPlayer->m_vtInfo.VendorIsVendor() )
		return;
	if( g_WndMng.GetWndBase(APP_WEBBOX) || g_WndMng.GetWndBase(APP_WEBBOX2) )
		return;
#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
		return;
#endif // __S_SERVER_UNIFY
	

#ifdef __VRCAMERA
	if( CObj::m_pObjHighlight == NULL )
	{
		SetCapture();
		m_ptMouseOld = point;
		m_bCameraMode = TRUE;
	}
#endif
//	m_bAutoAttack = FALSE;
	m_ptMouseSpot = point;
	SetCapture();
	m_timerRButtonDown.Set( 0 );;
	// 좌측 버튼이 눌러진지 1000분의 200초 내라면 동시에 눌러진 것으로 판단, 동시 눌러진 것은 점프, 
	// 따라서 스킬 사용 또는 포커싱을 스킵함.
	if( m_timerLButtonDown.GetLeftTime() < 200 )
		return;
//	CRect rect = GetClientRect();
	if( g_Option.m_nInterface == 0 )	// 구버전인터페이스 방식에서만 우클릭이 스킬.
	{
		CWorld* pWorld = g_WorldMng();
		CObj* pTargetObj = CObj::m_pObjHighlight;		// 커서를 대고 있던 오브젝트가 하이라이트 오브젝이다.
		if( g_pPlayer->m_pActMover->IsFly() == FALSE )
		{
			if( pTargetObj )
			{
				pWorld->SetObjFocus( pTargetObj );

				CMover* pMover = (CMover*)pTargetObj;
				if( pMover->GetType() == OT_MOVER )
					m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
			} else
				m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
		}
	}
}	

//
//
void CWndWorld::OnMButtonUp(UINT nFlags, CPoint point)
{
	if( m_nMouseMode == 0 )
		ReleaseCapture();
#ifndef __VRCAMERA
	if( g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock() )
		g_Neuz.m_camera.Unlock();
#endif
}
void CWndWorld::OnMButtonDown(UINT nFlags, CPoint point)
{
	if( IsFullMax() && m_wndTitleBar.GetWindowRect( TRUE ).PtInRect( point ) )
		return;
#ifndef __VRCAMERA
	SetCapture();
	m_ptMouseOld = point;
#endif
}

BOOL CWndWorld::GetBuffIconRect( DWORD dwID, const CPoint& point )
{
	auto it = m_rcCheck.begin();
	for( ; it != m_rcCheck.end(); ++it )
	{
		BUFFICONRECT_INFO &info = *it;
		if( PtInRect( &info.rc, point ) == TRUE && info.dwID == dwID )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CWndWorld::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	m_objidTracking		= NULL_ID;
	if(!CDeployManager::GetInstance()->IsReady())
	ControlPlayer( WM_LBUTTONDBLCLK, point );
	CRect rect = GetClientRect();
	CWorld* pWorld = g_WorldMng.Get();
	CObj* pFocusObj	= g_WorldMng.Get()->GetObjFocus();
	if( pFocusObj && CObj::m_pObjHighlight == pFocusObj )
	{
		const ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
		if( pItemProp )
		{   
			if( pItemProp->dwItemKind3 != IK3_WAND ) //Wand일 경우 AutoAttack을 하지 않음.
			{
				m_bAutoAttack = TRUE;
			}
		}
	}

	if( GetBuffIconRect( II_SYS_SYS_SCR_RETURN, point ) )
		g_DPlay.SendReturnScroll( -1 );

	if( GetBuffIconRect( II_SYS_SYS_SCR_PARTYSUMMON, point ) ) // "단장의 부름" 버프 더블클릭
	{
		if( g_Neuz.m_dwSummonPartyObjid == 0 || g_pPlayer->GetId() == g_Neuz.m_dwSummonPartyObjid )
			return;

		CWndSummonPartyUse* pWndSummonPartyUse = (CWndSummonPartyUse*)g_WndMng.GetWndBase( APP_SUMMON_PARTY_USE );
		if( !pWndSummonPartyUse )
		{
			pWndSummonPartyUse = new CWndSummonPartyUse;
			pWndSummonPartyUse->Initialize( &g_WndMng );
		}
	}		
	if( GetBuffIconRect( II_SYS_SYS_SCR_PET_FEED_POCKET, point ) )	// II_SYS_SYS_SCR_PET_FEED_POCKET 버프 더블 클릭
		g_DPlay.SendFeedPocketInactive();
}

void CWndWorld::OnMButtonDblClk( UINT nFlags, CPoint point)
{
	if( g_Option.m_nInterface == 0 )
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			if( g_Neuz.m_camera.IsLock() ) 
				g_Neuz.m_camera.Unlock();
			else 
				g_Neuz.m_camera.Lock();
		}
		else
		{
			g_Neuz.m_camera.m_fRotx=-g_pPlayer->GetAngle();
		}
	}
}
void CWndWorld::OnRButtonDblClk( UINT nFlags, CPoint point)
{

	//	CRect rect = GetClientRect();
	CWorld* pWorld = g_WorldMng.Get();
	// 포커스 타겟 해제
	CObj* pTargetObj = CObj::m_pObjHighlight;

	if( g_pPlayer->m_pActMover->IsFly() )
	{
		m_nMouseMode ^= 1;		// 비행중 마우스 이동모드 토글.
		if( m_nMouseMode == 0 )
		{
			ReleaseCapture();
			ClipCursor( NULL ); // 윈도우를 rcWindow를 벗어나지 않게 한다.
		}
		else
		{
			SetCapture();
			RECT rcWindow;
			::GetWindowRect( g_Neuz.GetSafeHwnd(), &rcWindow );
			ClipCursor( &rcWindow ); // 윈도우를 rcWindow를 벗어나지 않게 한다.
		}
	}
	else
	{
		
		if( g_Option.m_nInterface == 1 )	
		{
			g_Neuz.m_camera.m_fRotx = -g_pPlayer->GetAngle();
		}
	}
	
}
BOOL CWndWorld::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( CWorld::m_nZoomLevel != 0 )
		return FALSE;
	if(CDeployManager::GetInstance()->IsReady())
		return FALSE;
	// zoom
	if(zDelta < 0)
	{
		g_Neuz.m_camera.m_fZoom -= 0.5f;

		if( g_Neuz.m_camera.m_fZoom > 2 )
		{
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamZoomed = true;
		}
		if(g_Neuz.m_camera.m_fZoom < -1.5f )
			g_Neuz.m_camera.m_fZoom = -1.5f;
	}
	else
	{
		g_Neuz.m_camera.m_fZoom += 0.5f;

		if( g_Neuz.m_camera.m_fZoom > 5 )
		{
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamZoomed = true;
		}


#if defined(_DEBUG) || defined(__INTERNALSERVER)
	if(g_Neuz.m_camera.m_fZoom > 15)
		g_Neuz.m_camera.m_fZoom = 15;
#else //_DEBUG
	if(g_Option.m_bZoomLimit)
	{
		if(g_Neuz.m_camera.m_fZoom > 7)
			g_Neuz.m_camera.m_fZoom = 7;	
	}
	else
	{
		if(g_Neuz.m_camera.m_fZoom > 15)
			g_Neuz.m_camera.m_fZoom = 15;
	}
#endif //_DEBUG
	}

	return TRUE;
}
void CWndWorld::OnMouseMove(UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL )
		return;

#ifdef __VRCAMERA
	if( m_bRButtonDown ) //&& m_bCameraMode )
	{
		g_Neuz.m_camera.m_fRotx += point.x - m_ptMouseOld.x;
		if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
		if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;

		//if(g_Neuz.m_camera.m_fRotx>180) g_Neuz.m_camera.m_fRotx=-180;
		//if(g_Neuz.m_camera.m_fRotx<-180) g_Neuz.m_camera.m_fRotx=180;

		g_Neuz.m_camera.m_fRoty += point.y - m_ptMouseOld.y;
		if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
		if(g_Neuz.m_camera.m_fRoty<-80) g_Neuz.m_camera.m_fRoty=-80;
	}
	else
	if( m_bLButtonDown )
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			CRect rect= GetClientRect();
			// rect.Height() : 100 = point.y : x
			int a = 100 * point.y / rect.Height();
			g_Neuz.m_camera.m_fRoty = a - 60;
			TRACE( "%f \n",g_Neuz.m_camera.m_fRoty );

		}
		else
		{

		}
	}	

#else // VRCAMERA
	CPoint pt2 = point;
	// m_nMouseMove==0 이면 우버튼 누르고 있어야 하고
	// m_nMouseMove==1 이면 안누르고 있으도 움직인다.
	// 신/구 버전에 관계 없다.
#ifdef __Y_INTERFACE_VER3

	bool* bpButtonDown;

	if( g_Option.m_nInterface == 2 )
		bpButtonDown = &m_bLButtonDown;
	else
		bpButtonDown = &m_bRButtonDown;
	
	if( (m_nMouseMode == 0 && (*bpButtonDown)) || (m_nMouseMode == 1) )
#else //__Y_INTERFACE_VER3
	if( (m_nMouseMode == 0 && m_bRButtonDown) || (m_nMouseMode == 1) )			
#endif //__Y_INTERFACE_VER3
	{
		SetCapture();

		if(g_pPlayer->m_pActMover->IsFly() /*&& m_bMButtonDown == FALSE*/ )
		{
			int nWidth = (int)g_Neuz.GetCreationWidth();
			int nHeight = (int)g_Neuz.GetCreationHeight();
			int nLeft = 0, nTop = 0;
			if( g_Neuz.IsWindow() )	// 윈도우 모드로 실행중이면.
			{
				RECT rrr;
				::GetWindowRect( g_Neuz.GetSafeHwnd(), &rrr ); 
				nLeft = rrr.left;		nTop = rrr.top;

			}
#ifndef __XUZHU			
			g_Neuz.m_pd3dDevice->ShowCursor( FALSE );
#endif
			ControlFlying( WM_MOUSEMOVE, point );
			if( (int)point.x > nWidth - 10 || point.x < 10 )
			{
				g_Neuz.m_pd3dDevice->SetCursorPosition( nLeft + nWidth / 2, nTop + point.y, 0 );
				point.x = nWidth / 2;
			}
			if( point.y < 10 || point.y > nHeight - 10 )
			{
				g_Neuz.m_pd3dDevice->SetCursorPosition( nLeft + point.x, nTop + nHeight / 2, 0 );
				point.y = nHeight / 2;
			}
		}
		else
			g_Neuz.m_pd3dDevice->ShowCursor( TRUE );
	
	}
	else
		g_Neuz.m_pd3dDevice->ShowCursor( TRUE );

//	if( pt2.x > 800 )
//		g_Neuz.m_pd3dDevice->ShowCursor( TRUE );
	
	
	
	
	BOOL bCamera     = FALSE;
	BOOL bLockCamera = FALSE;
	// 휠버튼드래그는 카메라만 회전.
	if( g_pPlayer->m_pActMover->IsFly() )
	{
		if( m_bMButtonDown )	// 비행중엔 무조건 휠이 카메라회전이다.
			bCamera = TRUE;
	} else
	{	// 지상에서만.
		if( g_Option.m_nInterface == 0 )	// 구 버전.
		{
			if( m_bMButtonDown )		// 휠로 카메라를 돌림.
				bCamera = TRUE;
		} 
		else
		if( g_Option.m_nInterface == 1 )	// 구 버전.
		{		
			if( m_bRButtonDown )		// 우클릭으로 카메라를 돌림.
				bCamera = TRUE;
		} 
		// 신버전.
#ifdef __Y_INTERFACE_VER3
		else
		if( g_Option.m_nInterface == 2 )
		{		
			if( m_bLButtonDown )		// 좌클릭으로 카메라를 돌림.
			{
				bCamera = TRUE;
			}
			
			if( m_bRButtonDown )
			{
				bCamera = TRUE;
				bLockCamera = TRUE;
			}
		}
#endif //__Y_INTERFACE_VER3
	}
	if( bCamera )
	{
		CWndGuideSystem* pWndGuide = NULL;
		pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
		if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamMove = true;
		FLOAT fRotSpeed = 1.0f;

		switch( g_Option.m_MouseSpeed )
		{
			case 0:
				fRotSpeed = 1.0f; break;
			case 1:
				fRotSpeed = 0.2f; break;
			case 2:
				fRotSpeed = 0.1f; break;
			default:
				fRotSpeed = 0.2f; break;
		}
		
		g_Neuz.m_camera.m_fRotx += ((point.x - m_ptMouseOld.x) * fRotSpeed );

#ifdef __Y_CAMERA_SLOW_8
		if(g_Neuz.m_camera.m_fRotx < -360.0f ) 
			g_Neuz.m_camera.m_fRotx = (g_Neuz.m_camera.m_fRotx + 360);

		if(g_Neuz.m_camera.m_fRotx > 0.0f ) 
			g_Neuz.m_camera.m_fRotx = (g_Neuz.m_camera.m_fRotx - 360);
#else //__Y_CAMERA_SLOW_8
		if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
		if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;
#endif //__Y_CAMERA_SLOW_8		

		//if(g_Neuz.m_camera.m_fRotx>180) g_Neuz.m_camera.m_fRotx=-180;
		//if(g_Neuz.m_camera.m_fRotx<-180) g_Neuz.m_camera.m_fRotx=180;
//		g_Neuz.m_camera.m_fRotx = -g_Neuz.m_camera.m_fRotx;
		
		g_Neuz.m_camera.m_fRoty += ((point.y - m_ptMouseOld.y) * fRotSpeed );
		if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) 
			g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
		if(g_Neuz.m_camera.m_fRoty<-80) 
			g_Neuz.m_camera.m_fRoty=-80;

#ifdef __Y_INTERFACE_VER3
		if( bLockCamera )
		{
			g_pPlayer->SetAngle( -g_Neuz.m_camera.m_fRotx );
			g_DPlay.SendPlayerAngle();
		}
#endif //__Y_INTERFACE_VER3

	}
	else
	if( m_bMButtonDown || m_bLButtonDown )
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
		}
		else
		{
		}
	}	
#endif // not VRCAMERA
	m_ptMouseOld = point;
}

void CWndWorld::OnMouseWndSurface( CPoint point )
{
	
}


void CWndWorld::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef __CLIENT
	if( m_bCtrlPushed && nChar == 'F' )
		m_bRenderFPS  = !m_bRenderFPS;
	
#ifdef __IMPROVE_MAP_SYSTEM
	if( nChar == VK_CONTROL && m_bCtrlPushed == TRUE )
	{
		m_bCtrlPushed = FALSE;
	}
#else // __IMPROVE_MAP_SYSTEM
	m_bCtrlPushed = FALSE;
#endif // __IMPROVE_MAP_SYSTEM
	if(nChar == VK_SHIFT && m_bShiftPushed != FALSE)
	{
		m_bShiftPushed = FALSE;
	}
	if( nChar == VK_SCROLL )
	{
		CWorld::m_nZoomLevel++;

		if( CWorld::m_nZoomLevel > 3 )
		{
			CWorld::m_nZoomLevel = 0;
		}

		if( CWorld::m_nZoomLevel != 0 )
		{
			g_Neuz.m_camera.m_fZoom = 0.0f;
			CString strCameraZoomText = _T( "" );
			strCameraZoomText.Format( prj.GetText( TID_GAME_CAMERA_ZOOM_LEVEL ), CWorld::m_nZoomLevel ); // %dX Zoom
			g_WndMng.PutString( strCameraZoomText, NULL, 0xff00ff00 );
		}
	}
	else if( nChar == VK_TAB )
	{		
		const auto ppMover = std::ranges::find_if(CWorld::m_amvrSelect,
			[&](CMover * candidate) {
				return candidate
					&& candidate != n_nMoverSelectCount
					&& !candidate->IsMode(TRANSPARENT_MODE)
					&& !candidate->IsDie();
			});

		if (ppMover != CWorld::m_amvrSelect.end()) {
			CMover * pMover = *ppMover;
			g_WorldMng.Get()->SetObjFocus(pMover);
			n_nMoverSelectCount = pMover;
		} else {
			n_nMoverSelectCount = nullptr;
		}
	}
	if( nChar == VK_ESCAPE )	
	{
		CWorldMap* pWorldMap = CWorldMap::GetInstance();

		if(pWorldMap->IsRender()) pWorldMap->DeleteWorldMap();
	}
	
	CWorld* pWorld = g_WorldMng.Get();

	if( g_Option.m_nInterface == 1 )		// 신버전 인터페이스 방식은 X 가 스킬사용이다.
	{
		if( nChar == 'C' )	
		{
			//			CObj* pTargetObj = CObj::m_pObjHighlight;		// 커서를 대고 있던 오브젝트가 하이라이트 오브젝이다.
			CObj* pTargetObj = pWorld->GetObjFocus();		// 선택된 오브젝트.
			if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			{
				if( pTargetObj )	// 커서를 대고 있던 오브젝트가 있으면
				{
					if(pTargetObj->GetType() == OT_MOVER )
						m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
				} else
					m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
			}
		}
	}

#endif //__CLIENT	
	
	if( g_pPlayer )
		g_pPlayer->m_SkillTimerStop = TRUE;
}
void CWndWorld::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 키 이동시 오토어택상태 풀림
	if(nChar == 'A' || nChar == 'S' || nChar == 'D' || nChar == 'W') 
		m_bAutoAttack = FALSE;
	if( nChar == VK_CONTROL )
		m_bCtrlPushed = TRUE;

#ifdef __IMPROVE_MAP_SYSTEM
	if( nChar == 'C' ) // 일단 임시적으로 이걸로 함
	{
		prj.m_bMapTransparent = !prj.m_bMapTransparent;
	}
#endif // __IMPROVE_MAP_SYSTEM

	if( nChar == VK_SHIFT )
		m_bShiftPushed = TRUE;	
	if( g_pPlayer )
		g_pPlayer->m_SkillTimerStop = FALSE;
}

BOOL CWndWorld::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if( pWndFrame == NULL )
	{
		LPCTSTR szErr = Error( "CWndWorld::OnDropIcon : pWndFrame==NULL" );
		//ADDERRORMSG( szErr );
	}

	if( g_WndMng.GetWndBase( APP_TEST ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITITEM), NULL, prj.GetTextColor(TID_GAME_TRADELIMITITEM) );
		SetForbid( TRUE );
		return FALSE;
	}		
	
	if( g_WndMng.GetWndBase( APP_SHOP_ ) ||
		g_WndMng.GetWndBase( APP_GUILD_MERIT1 ) ||
		g_WndMng.GetWndBase( APP_GUILD_BANK ) ||
		g_WndMng.GetWndBase( APP_COMMON_BANK ) ||
		g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX ) ||
		g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN ) ||
		g_WndMng.GetWndBase( APP_SUMMON_ANGEL ) ||
		g_WndMng.GetWndBase( APP_TRADE ) )
	{
		if( pWndFrame && pWndFrame->GetWndId() == APP_INVENTORY )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITITEM), NULL, prj.GetTextColor(TID_GAME_TRADELIMITITEM) );
			//g_WndMng.PutString( "거래중에 아이템을 버릴수 없어요.", NULL, 0xffff0000 );
			SetForbid( TRUE );
			return FALSE;
		}
	}
	else
	if( g_WndMng.GetWndBase( APP_REPAIR ) )
	{
		if( pWndFrame && pWndFrame->GetWndId() == APP_INVENTORY )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTDROP), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOTDROP) );
			//g_WndMng.PutString( "수리중에 아이템을 버릴수 없어요.", NULL, 0xffff0000 );
			SetForbid( TRUE );
			return FALSE;
		}
	}

	
	if( pShortcut->m_dwShortcut == ShortcutType::Item ) //&& pWndFrame->GetWndId() == APP_INVENTORY )
	{
		CRect rect = GetClientRect();
		CWorld* pWorld	= g_WorldMng.Get();
		D3DXVECTOR3 vPos, vOut;
		pWorld->ClientPointToVector( NULL, rect, point, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &vPos, FALSE );
		vOut = vPos - g_pPlayer->GetPos();	// 플레이어에서 찍은곳을 향하는 벡터
		D3DXVec3Normalize( &vPos, &vOut );	// 그 것을 노말라이즈.
		vPos += g_pPlayer->GetPos();			// 절대좌표로 변환.
		vPos.y += 1000.0f;		// 최상위 높이를 검사하기 위해.
		FLOAT fHeight	= pWorld->GetFullHeight( vPos );	// 절대좌표의 y높이 구함.
		if( fabs(fHeight - g_pPlayer->GetPos().y) > 1.0f )	// 아이템놓을곳과 플레이어의 좌표가 1미터 이상차이가 나면
			vPos = g_pPlayer->GetPos();		// 걍 제자리에 놓음.
		else
			vPos.y	= fHeight + 0.2f;	// 경사면에선 땅에 파묻히는 현상이 약간 있어서 좀 올려 놓이게 했다.
		
		if( pWndFrame && pWndFrame->GetWndId() == APP_INVENTORY )
		{
			PLAYSND( SND_INF_GROUNDDROP );
			if( pShortcut->m_dwData == 0 )
			{
				g_WndMng.PutString(TID_GAME_CANNOT_DROPMONEY);
				SetForbid( TRUE );
				return FALSE;


			}
			else
			{
				CItemElem * pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
				if( pItemBase == NULL )
					return FALSE;

				if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && pItemBase->m_idGuild != 0 )
					return FALSE;

				if( IsUsingItem( pItemBase ) )
				{
					SetForbid( TRUE );
					return FALSE;
				}

				CItemElem* pItemElem = (CItemElem*)pShortcut->m_dwData;
				if( pItemElem->IsUndestructable() == TRUE )
				{
					g_WndMng.PutString(TID_GAME_ERROR_UNDESTRUCTABLE_ITEM);
					return FALSE;
				}
				
				if( pItemElem->IsQuest() )
				{
					g_WndMng.m_pWndQuestItemWarning = new CWndQuestItemWarning;
					g_WndMng.m_pWndQuestItemWarning->m_pItemElem = pItemElem;
					g_WndMng.m_pWndQuestItemWarning->m_vPos = vPos;
					g_WndMng.m_pWndQuestItemWarning->Initialize( NULL, APP_QUEITMWARNING );
				}
				else
				{
					if( pItemElem->m_nItemNum == 1 )	// 갯수가 하나라면 그냥 떨군다.
					{
						g_WndMng.m_pWndDropConfirm = new CWndDropConfirm;
						g_WndMng.m_pWndDropConfirm->m_pItemElem = pItemElem;
						g_WndMng.m_pWndDropConfirm->m_vPos = vPos;
						g_WndMng.m_pWndDropConfirm->Initialize( NULL, APP_DROP_CONFIRM );
						
						if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
						{
							CWndStatic* pWndStatic = (CWndStatic*)(g_WndMng.m_pWndDropConfirm->GetDlgItem( WIDC_STATIC1 ));
							pWndStatic->SetTitle( prj.GetText(TID_GAME_DROPITEMREMOVE) );
						}
					}
					else
					{
						//SAFE_DELETE( g_WndMng.m_pWndDropItem );
						g_WndMng.m_pWndDropItem = new CWndDropItem;
						g_WndMng.m_pWndDropItem->m_pItemElem = pItemElem;
						g_WndMng.m_pWndDropItem->m_vPos = vPos;
						g_WndMng.m_pWndDropItem->Initialize( NULL, APP_DROP_ITEM );

						if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
						{
							CWndStatic* pWndStatic = (CWndStatic*)(g_WndMng.m_pWndDropItem->GetDlgItem( WIDC_STATIC1 ));
							pWndStatic->SetTitle( prj.GetText(TID_GAME_DROPITEMREMOVE2) );
						}
					}
				}
				
				return TRUE;
			}
		}
		else
		{
			// 이곳부터는 드랍된 곳이 인벤이 아니다 
			if( pWndFrame && APP_BUFFPET_STATUS == pWndFrame->GetWndId( ) ) // 버프펫 윈도우로 드랍됨.
			{
				CWndBuffPetStatus* pWnd = (CWndBuffPetStatus*)pWndFrame;
				pWnd->DoModal_ConfirmQuestion( 0, 0, pShortcut->m_dwIndex, pShortcut->m_dwData, CWndConfirmVis::CVS_UNEQUIP_VIS );

				return FALSE;
			}

		}
	}
	SetForbid( TRUE );
	return FALSE;
}

int CWndWorld::GetGMLogoIndex()
{
	int nrtn_val = -1;
	switch(::GetLanguage())
	{
		case LANG_SPA:
			nrtn_val = GM_LOGO_CH;
			break;
		case LANG_GER:
		case LANG_FRE:
			nrtn_val = GM_LOGO_EU;
			break;
		case LANG_JAP:
			nrtn_val = GM_LOGO_JP;
			break;
		case LANG_ENG:
			if(::GetSubLanguage() == LANG_SUB_USA)
				nrtn_val = GM_LOGO_US;
			else if(::GetSubLanguage() == LANG_SUB_PHP)
				nrtn_val = GM_LOGO_PH;
			break;
		case LANG_THA:
			nrtn_val = GM_LOGO_TH;
			break;
		case LANG_TWN:
			nrtn_val = GM_LOGO_TW;
			break;
//		case LANG_POR:
		default:
			nrtn_val = -1;
			break;
	}

	return nrtn_val;
}

void ProcessQuestEmoticon()
{
	// 퀘스트 아이콘 확축 프로세스 
	if( CMover::m_bQuestEmoticonAdd )
	{
		CMover::m_fQuestEmoticonScale += 0.01f;
		if( CMover::m_fQuestEmoticonScale >= 1.2f )
			CMover::m_bQuestEmoticonAdd = FALSE;
	}
	else
	if( CMover::m_bQuestEmoticonAdd == FALSE )
	{
		CMover::m_fQuestEmoticonScale -= 0.01f;
		if( CMover::m_fQuestEmoticonScale <= 1.0f )
			CMover::m_bQuestEmoticonAdd = TRUE;
	}
}

BOOL CWndWorld::Process()
{
	_PROFILE("CWndWorld::Process()");

	if( g_pPlayer )
	{
		if( g_Option.m_bCameraLock && !m_bRButtonDown && 
			( g_bKeyTable[g_Neuz.Key.chUp] || g_bKeyTable['S'] || g_bKeyTable[g_Neuz.Key.chLeft] || g_bKeyTable['D'] ) && g_pPlayer->IsFly() == FALSE )
		{
			g_Neuz.m_camera.m_fRotx = -g_pPlayer->GetAngle();
		}
		// 퀘스트 제한 시간 체크 및 시간 출력 세팅 
		::ProcessQuestEmoticon();

		if( m_dwIdBgmMusic )
		{
			if( IsStopMusic() == TRUE )
			{
				PlayBGM( m_dwIdBgmMusic );
				if( m_bBGM == FALSE )
				{
					m_dwIdBgmMusic = 0;
//#if __VER >= 9
//					if( m_dwIdBgmMusicOld )
//						m_dwIdBgmMusic	= m_dwIdBgmMusicOld;
//#endif	//
				}
			}
		}
		else
		{
			m_dwIdBgmMusic	= g_WorldMng()->m_dwIdMusic;
			if( m_dwIdBgmMusic == 0 )
				m_dwIdBgmMusic	= BGM_TH_GENERAL;
			/*
			if( m_dwIdBgmMusic )
			{
				m_dwIdBgmMusicOld	= m_dwIdBgmMusic;
			}
			else
			{
				if( m_dwIdBgmMusicOld )
					m_dwIdBgmMusic	= m_dwIdBgmMusicOld;
				else
					m_dwIdBgmMusic	= m_dwIdBgmMusicOld	= BGM_TH_GENERAL;				
			}
			*/
		}

		CWorld* pWorld = g_WorldMng();
		// 캐릭터가 움직이면 대화, 상인, 거래 창을 무조건 닫는다.
		if( g_pPlayer->m_pActMover->IsState( OBJSTA_STAND ) == FALSE || g_pPlayer->m_pActMover->GetMoveState() == OBJSTA_BMOVE )
		{
			if(CWndBase::m_GlobalShortcut.IsEmpty() == FALSE)
			{
				CWndBase* pWndFrame = CWndBase::m_GlobalShortcut.m_pFromWnd->GetFrameWnd();

				if(pWndFrame && pWndFrame->GetWndId() != APP_INVENTORY) // 인벤토리 제외 다른 창에서 드래그된 아이템이 있다면 지워준다.
					CWndBase::m_GlobalShortcut.Empty();
			}

			if( g_pPlayer->m_dwCtrlReadyTime != 0xffffffff )
			{
				g_pPlayer->m_dwCtrlReadyTime = 0xffffffff;
				g_pPlayer->m_dwCtrlReadyId   = NULL_ID;
				g_DPlay.SendCtrlCoolTimeCancel();
			}
			m_wndMenuMover.SetVisible(FALSE);

			CWndBase* pWndBase = g_WndMng.GetWndBase( APP_DIALOG_EX );
			if( pWndBase ) 
				pWndBase->Destroy();

			if( g_pPlayer->m_vtInfo.GetOther() )
			{
				if( g_pPlayer->m_vtInfo.GetOther()->IsPlayer() )	
					g_DPlay.SendTradeCancel();

				g_DPlay.SendCloseShopWnd();
				g_pPlayer->m_vtInfo.SetOther( NULL );
			}
			
			pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
			if( pWndBase )
			{
				((CWndInventory*)pWndBase)->BaseMouseCursor();
				if(g_WndMng.m_pWndSmeltSafetyConfirm != NULL)
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)
			}

			pWndBase	= g_WndMng.GetWndVendorBase();
			if( pWndBase )
			{
				pWndBase->Destroy();
				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase )
					pWndBase->Destroy();
			}

			pWndBase	= g_WndMng.GetWndBase( APP_SHOP_ );
			if( pWndBase )
			{
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_TRADE_GOLD );
				if( pWndBase )	
					pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase ) 
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN );
			if( pWndBase )
			{
				pWndBase->Destroy();
				
				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
				
				if( pWndBase )
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX );
			if( pWndBase )
			{
				pWndBase->Destroy();
				
				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
				
				if( pWndBase )
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
			if( pWndBase )
			{
				pWndBase->Destroy();

				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
				
				if( pWndBase )
					pWndBase->Destroy();
			}	
#ifdef __EVE_MINIGAME
			if(g_WndMng.m_pWndKawiBawiBoGameConfirm != NULL)
			{
				g_WndMng.m_pWndKawiBawiBoGameConfirm->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_MINIGAME_WORD );
			if( pWndBase )
			{
				pWndBase->Destroy();

				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
				
				if( pWndBase )
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_MINIGAME_DICE );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
#endif //__EVE_MINIGAME
			pWndBase	= g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL );
			if( pWndBase )
			{
				pWndBase->Destroy();

				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
				
				if( pWndBase )
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_SMELT_JEWEL );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_SMELT_EXTRACTION);
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_PET_FOODMILL );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
			if(pWndBase)
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_SMELT_REMOVE_JEWEL );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_PET_TRANS_EGGS );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
#ifdef __JEFF_11
			pWndBase	= g_WndMng.GetWndBase( APP_PET_RES );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
#endif
			pWndBase	= g_WndMng.GetWndBase( APP_AWAKENING );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_LVREQDOWN );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_CANCEL_BLESSING );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_HERO_SKILLUP );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
#ifdef __TRADESYS
			pWndBase	= g_WndMng.GetWndBase( APP_DIALOG_EVENT );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
#endif //__TRADESYS
			pWndBase	= g_WndMng.GetWndBase( APP_HEAVEN_TOWER );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_REMOVE_ATTRIBUTE );
			if( pWndBase )
			{
				pWndBase->Destroy();
			}
			if( pWndBase )
			{
				pWndBase->Destroy();

				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );

				if( pWndBase )
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_COMMON_BANK );
			if( pWndBase )
			{
				g_DPlay.SendCloseBankWnd();
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_TRADE_GOLD );
				if( pWndBase )	
					pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase )	
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_REPAIR );
			if( pWndBase )
			{
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase )	pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_BANK_PASSWORD );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_CONFIRM_BANK );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_DROP_CONFIRM );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_DROP_ITEM );
			if( pWndBase )
				pWndBase->Destroy();

			pWndBase	= g_WndMng.GetWndBase( APP_GUILD_BANK );
			if( pWndBase )
			{
				g_DPlay.SendCloseGuildBankWnd();
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_RANK_WAR );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RANK_INFO );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RANK_GUILD );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_PIERCING );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_TEST );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_REMOVE_ELEM );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_CHANGECLASS_1 );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_CHANGECLASS_2 );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_POST );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_POST_SEND );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_POST_RECEIVE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_POST_READ );
			if( pWndBase )
				pWndBase->Destroy();
			
			pWndBase	= g_WndMng.GetWndBase( APP_GUILDCOMBAT_RANK_P );
			if( pWndBase )
				pWndBase->Destroy();			
			pWndBase	= g_WndMng.GetWndBase( APP_GUILDCOMBAT_RANKING );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_GUILD_WAR_STATE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_CLOSE_EXISTING_CONNECTION );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_GUILDCOMBAT_SELECTION );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_GUILDCOMBAT_1TO1_SELECTION );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_GUILDCOMBAT_1TO1_OFFERSTATE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_GUILDCOMBAT_1TO1_OFFER );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_SECRETROOM_SELECTION );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_SECRETROOM_OFFER );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_SECRETROOM_OFFERSTATE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_SECRETROOM_TEXRATE_CHECK );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_SECRETROOM_BOARD );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RAINBOWRACE_OFFER );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RAINBOWRACE_INFO );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RAINBOWRACE_RULE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RAINBOWRACE_RANKING );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RAINBOWRACE_PRIZE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RR_MINIGAME_KAWIBAWIBO );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RR_MINIGAME_DICE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RR_MINIGAME_ARITHMETIC );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RR_MINIGAME_STOPWATCH );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RR_MINIGAME_TYPING );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RR_MINIGAME_CARD );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RR_MINIGAME_LADDER );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_CHANGE_ATTRIBUTE );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_SMELT_SAFETY );
			if( pWndBase )
				pWndBase->Destroy();
		}
		g_SoundMng.SetListener( g_pPlayer->GetPos(), g_Neuz.m_camera.m_fCurRotx );
		g_WorldMng()->SetCamera( &g_Neuz.m_camera );
		g_WorldMng()->Process(); // 월드를 처리함 (오브젝트, 기타 등등 )

		// 지상에서 상대를 공격하거나 해당 오브젝트 사용하기.
		if( g_pPlayer->m_pActMover && g_pPlayer->m_pActMover->IsFly() == FALSE )
		{
			CObj* pObj = pWorld->GetObjFocus();
			if( m_dwNextSkill == NEXTSKILL_ACTIONSLOT )	// 걍 1이면 액션슬롯 사용 
			{
				UseSkillToFocusObj( (CCtrl*)pObj );		// 스킬을 사용함.
			} else
			if( m_dwNextSkill != NEXTSKILL_NONE )		// 그외는 예약된 스킬을 실행.
			{
				g_WndMng.UseSkillShortCut( m_dwNextSkill );
			} else
			{	// 일반 공격.
				if( (m_bLButtonDown || m_bAutoAttack) && (g_pPlayer->GetCmd() != OBJACT_USESKILL) )	// 누른 순간에만 처리하기 위해서...
				{
					if( m_bAutoAttack || (m_bSelectTarget && m_timerFocusAttack.IsTimeOut() == TRUE ) )
					{
						if( pObj )
						{
							// 컨트롤/무버/아이템 일 경우만...
							if( pObj->GetType() == OT_CTRL || pObj->GetType() == OT_MOVER || pObj->GetType() == OT_ITEM )
							{	// 걍 일반 공격/사용 들어감.  스킬응답중일땐 실행하지 말자.
								if( g_pPlayer->IsAttackAble(pObj) && (g_pPlayer->m_dwReqFlag & REQ_USESKILL) == 0 )	
								{					
									UseFocusObj( (CCtrl*) pObj );	// 대상을 Use한다.
								}
							}
						} else
							ControlPlayer( 0, GetMousePoint() );
					}
				} 
				else
				{
					// 플레이어 마우스 및 키보드 조작 처리 
					ControlPlayer( 0, GetMousePoint() );
					m_bSelectTarget = FALSE;
				}
			}
		} else
		{
			CObj* pObj = pWorld->GetObjFocus();
			if( m_bLButtonDown && m_bSelectTarget )
			{
				if( pObj )
				{
					if( pObj->GetType() == OT_ITEM || pObj->GetType() == OT_CTRL )
					{
						if( m_bLButtonDown && !m_bLButtonDowned )
							g_pPlayer->CMD_SetUseItem( (CCtrl*)pObj );	// 목표가 주어지면 자동 이동 
						m_bLButtonDowned = m_bLButtonDown;
					}
				}
			} else
			{
				// 플레이어 마우스 및 키보드 조작 처리 
				ControlPlayer( 0, GetMousePoint() );
				m_bSelectTarget = FALSE;
			}
		}
		m_bLButtonDowned = m_bLButtonDown;
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		
		for (REGIONELEM & regionElem : pWorld->m_aRegion.AsSpan()) {
			REGIONELEM * lpRegionElem = &regionElem;
			if( lpRegionElem->m_rect.PtInRect( CPoint( (int)( vPos.x ), (int)( vPos.z ) ) ) )
			{
				if( lpRegionElem->m_bInside == FALSE )
				{
					CWndNavigator* pWndNavigator = (CWndNavigator*)g_WndMng.GetWndBase( APP_NAVIGATOR );
					if( pWndNavigator && lpRegionElem->m_szTitle[0] == 0 )
						pWndNavigator->SetRegionName( "" );

					//if( lpRegionElem->m_bDirectMusic )
					//	lpRegionElem->m_bPlayDirectMusic = TRUE;

					m_dwIdBgmMusic = lpRegionElem->m_dwIdMusic;
					if( lpRegionElem->m_bDirectMusic == TRUE )
					{
//#if __VER >= 9
//						m_dwIdBgmMusicOld	= m_dwIdBgmMusic;
//#endif	//
						m_bBGM = TRUE;
					}
					else
					{
						m_bBGM = FALSE;
					}
//					if( IsStopMusic()  ) //|| lpRegionElem->m_bDirectMusic ) ) //&& !lpRegionElem->m_bPlayDirectMusic )
					{
						PlayMusic( lpRegionElem->m_dwIdMusic );
						if( m_bBGM == FALSE )
						{
							m_dwIdBgmMusic = 0;
//#if __VER >= 9
//							if( m_dwIdBgmMusicOld )
//								m_dwIdBgmMusic	= m_dwIdBgmMusicOld;
//#endif	//
						}
					}
					lpRegionElem->m_bInside = TRUE;

					//if( lpRegionElem->m_szDesc[ 0 ] )
					{
						BOOL bResult = TRUE;
						if( lpRegionElem->m_dwAttribute & RA_INN )
						{
							if( (int)g_pPlayer->GetPos().y != (int)g_WorldMng()->GetLandHeight( g_pPlayer->GetPos() ) )
								bResult = FALSE;
						}
						if( bResult )
						{
							CHAR szDesc[ MAX_REGIONDESC ];
							CHAR* pDescSrc = lpRegionElem->m_szDesc;
							CHAR* pDescDsc = szDesc;
							int nNumCaption = 0;
							while( 1 )
							{
								if( ( *pDescSrc == '\r' && *(pDescSrc + 1 ) == '\n' ) || *pDescSrc == 0 )
								{
									*pDescDsc = 0;
									if( szDesc[ 0 ] )
										g_ClientMsg.AddMessage( szDesc, 0xffffffff );//.AddCaption( szDesc, m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
									nNumCaption++;
									pDescDsc = szDesc;
									pDescSrc+=2;
									if( *pDescSrc == 0 )
										break;
								}
								else
								{
									*pDescDsc++ = *pDescSrc++;
									if( IsDBCSLeadByte( *pDescSrc ) )
										*pDescDsc++ = *pDescSrc++;
								}
							}
							pDescSrc = lpRegionElem->m_szTitle;
							pDescDsc = szDesc;
							int nCnt = 0;
							while( 1 )
							{
								if( ( *pDescSrc == '\r' && *(pDescSrc + 1 ) == '\n' ) || *pDescSrc == 0 )
								{
									*pDescDsc = 0;
									if( szDesc[ 0 ] )
									{
										if( nCnt == 0 )
										{
											CWndNavigator* pWndNavigator = (CWndNavigator*)g_WndMng.GetWndBase( APP_NAVIGATOR );
											if( pWndNavigator )
												pWndNavigator->SetRegionName( szDesc );
											if( ::GetLanguage() != LANG_JAP )
												g_Caption1.AddCaption( szDesc, m_pFontAPITitle );// CWndBase::m_Theme.m_pFontCaption );
											else
												g_Caption1.AddCaption( szDesc, NULL );// CWndBase::m_Theme.m_pFontCaption );
										}
										else
										{
											if( ::GetLanguage() != LANG_JAP )
												g_Caption1.AddCaption( szDesc, m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
											else
												g_Caption1.AddCaption( szDesc, NULL );// CWndBase::m_Theme.m_pFontCaption );
										}
									}
									pDescDsc = szDesc;
									pDescSrc+=2;
									nCnt++;
									if( *pDescSrc == 0 )
										break;
								}
								else
								{
									*pDescDsc++ = *pDescSrc++;
									if( IsDBCSLeadByte( *pDescSrc ) )
										*pDescDsc++ = *pDescSrc++;
								}
							}
						}
						lpRegionElem->m_bInside = (bResult == TRUE);
						break;
					}
				}
			}
			else
				lpRegionElem->m_bInside = FALSE;
		}
		if( g_bKeyTable[ VK_LEFT ] )
		{
			g_Neuz.m_camera.m_fRotx -= 4;
#ifdef __Y_CAMERA_SLOW_8
			if(g_Neuz.m_camera.m_fRotx < -360.0f ) 
				g_Neuz.m_camera.m_fRotx = (g_Neuz.m_camera.m_fRotx + 360);
			
			if(g_Neuz.m_camera.m_fRotx > 0.0f ) 
				g_Neuz.m_camera.m_fRotx = (g_Neuz.m_camera.m_fRotx - 360);			
#else //__Y_CAMERA_SLOW_8
			if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
			if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;
#endif //__Y_CAMERA_SLOW_8
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamMove = true;
			
		}	
		if( g_bKeyTable[ VK_RIGHT ] )
		{
			g_Neuz.m_camera.m_fRotx += 4;
#ifdef __Y_CAMERA_SLOW_8
			if(g_Neuz.m_camera.m_fRotx < -360.0f ) 
				g_Neuz.m_camera.m_fRotx = (g_Neuz.m_camera.m_fRotx + 360);
			
			if(g_Neuz.m_camera.m_fRotx > 0.0f ) 
				g_Neuz.m_camera.m_fRotx = (g_Neuz.m_camera.m_fRotx - 360);
#else //__Y_CAMERA_SLOW_8
			if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
			if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;
#endif //__Y_CAMERA_SLOW_8
				CWndGuideSystem* pWndGuide = NULL;
				pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
				if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamMove = true;
		}	
		if( g_bKeyTable[ VK_UP ] )
		{
			g_Neuz.m_camera.m_fRoty += 2;
			if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
			if(g_Neuz.m_camera.m_fRoty<-80) g_Neuz.m_camera.m_fRoty=-80;
				CWndGuideSystem* pWndGuide = NULL;
				pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
				if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamMove = true;
		}
		if( g_bKeyTable[ VK_DOWN ] )
		{
			g_Neuz.m_camera.m_fRoty -= 2;
			if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
			if(g_Neuz.m_camera.m_fRoty<-80) g_Neuz.m_camera.m_fRoty=-80;
				CWndGuideSystem* pWndGuide = NULL;
				pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
				if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamMove = true;
		}	

		if( g_bKeyTable[ VK_PRIOR ])
		{
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamZoomed = true;
			if(!CDeployManager::GetInstance()->IsReady())
			if( !GuildDeploy()->IsReady( ) )
			g_Neuz.m_camera.m_fZoom-=0.6f;
			if(g_Neuz.m_camera.m_fZoom < 0)
				g_Neuz.m_camera.m_fZoom = 0;
		}
		if( g_bKeyTable[ VK_NEXT ])
		{
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.bIsCamZoomed = true;
			if(!CDeployManager::GetInstance()->IsReady())
			if( !GuildDeploy()->IsReady( ) )
			g_Neuz.m_camera.m_fZoom+=0.6f;
			if(g_Neuz.m_camera.m_fZoom > 7)
				g_Neuz.m_camera.m_fZoom = 7;
		}

		if( g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->m_bIsIndoor == 1 )
			SetLODDetail( 0 );

#ifdef __YAUTOATTACK
/*
		if( CObj::m_pObjHighlight == NULL )
		{
			m_bAttackDbk  = FALSE;
		}
*/
#endif //__YAUTOATTACK

	}
	if( nColor > 0 )
	{
		g_fAddScale += 0.002f;
		g_fXScale += g_fAddScale;
		g_fYScale += g_fAddScale;
		if( g_fXScale > 1.0f ) g_fXScale = 1.0f;
		if( g_fYScale > 1.0f ) g_fYScale = 1.0f;

		/*
		if( g_bAlpha == FALSE ) nColor++;
		if( g_bAlpha == TRUE ) nColor--;
		if( nColor > 255 ) { g_bAlpha = TRUE; }
		*/
	}

#ifdef __EVE_NEWYEAR
	//	c.12/31일 11시59분 부터 프리프 인터페이스에 공지처럼 시간이 나오는것.
	//	예)23시59분때 게임창에 59초 58초 57초 56초 55초 54초........01초 happy new year
	if( g_DPlay.m_nCountdown > 0 && ( g_nProcessCnt & 63 ) == 0 )
	{
		g_DPlay.m_nCountdown--;
		if( g_DPlay.m_nCountdown == 0 )
		{
			//			게임창에 happy new year
			g_Caption1.RemoveAll();
			g_Caption1.AddCaption( "Happy New Year", m_pFontAPITitle );
			D3DXVECTOR3	vPos	= g_pPlayer->GetPos();
			vPos.y	+= 2.0f;
			CreateSfx( g_Neuz.m_pd3dDevice, XI_NAT_MAGICBOMB01, vPos, NULL_ID );	// g_pPlayer->GetId() );
		}
		else
		{
			//			게임창에 59초 58초 57초 56초 55초 54초........01초
			
			char buffer[64];
			_itoa( g_DPlay.m_nCountdown, buffer, 10 );
			
			g_Caption1.RemoveAll();
			g_Caption1.AddCaption( buffer, m_pFontAPITitle );
		}
	}
#endif	// __EVE_NEWYEAR
	
	g_Caption1.Process();
	g_CapTime.Process();

	if( g_pPlayer )
		m_buffs.Process();

	DWORD dwBufCount = GetTickCount();
	if( g_pPlayer && dwBufCount >= m_dwOneSecCount + 1000 )
	{
		int Mod = ( dwBufCount - m_dwOneSecCount ) % 1000;
		m_dwOneSecCount = GetTickCount() - Mod;
		for( int j = 0 ; j < SM_MAX ; ++j )
		{
			if( 0 < g_pPlayer->m_dwSMTime[j] && g_pPlayer->m_dwSMTime[j] != 1 )
			{
				--g_pPlayer->m_dwSMTime[j];
			}
		}
	}

#ifdef __YCLOTH
	if( g_pPlayer )
	{
		D3DXMATRIX  mat1, mat2;
		D3DXVECTOR3 Pos1, Pos2;

		mat1 = ((CModelObject*)g_pPlayer->m_pModel)->m_mUpdateBone[10];
		mat2 = ((CModelObject*)g_pPlayer->m_pModel)->m_mUpdateBone[14];

		mat1 = mat1 * g_pPlayer->GetMatrixWorld();
		mat2 = mat2 * g_pPlayer->GetMatrixWorld();
		
		Pos1 = D3DXVECTOR3( mat1._41, mat1._42, mat1._43 );
		Pos2 = D3DXVECTOR3( mat2._41, mat2._42, mat2._43 );

		g_Cloth.Process(0,Pos2, Pos1);
	}
#endif

	bool buffstatus = false;
	if (g_pPlayer) {
		buffstatus = std::ranges::any_of(g_pPlayer->m_buffs.m_mapBuffs,
			[](const decltype(g_pPlayer->m_buffs.m_mapBuffs)::value_type & pair) {
				return pair.second->GetType() == BUFF_SKILL;
			}
		);
	}
	
	if (buffstatus) {
		if (!m_pWndBuffStatus)
			m_pWndBuffStatus = g_WndMng.CreateApplet(APP_BUFF_STATUS);
	} else {
		if (m_pWndBuffStatus) {
			m_pWndBuffStatus->Destroy();
			m_pWndBuffStatus = nullptr;
			CWndBase * pWndBaseFocus = CWndBase::GetFocusWnd();
			if (pWndBaseFocus && pWndBaseFocus->GetWndApplet() && pWndBaseFocus->GetWndApplet()->dwWndId != APP_COMMUNICATION_CHAT)
				SetFocus();
		}
	}
	
	if( g_pPlayer && CRainbowRace::GetInstance()->m_dwRemainTime != 0 )
	{
		if(g_WndMng.m_pWndRainbowRaceMiniGameButton == NULL)
		{
			CRainbowRace::GetInstance()->m_bRRFinishMsg = FALSE;
			CRainbowRace::GetInstance()->m_bCheckEnd = FALSE;

			g_WndMng.m_pWndRainbowRaceMiniGameButton = new CWndRainbowRaceMiniGameButton;
			
			if(g_WndMng.m_pWndRainbowRaceMiniGameButton)
				g_WndMng.m_pWndRainbowRaceMiniGameButton->Initialize(NULL);
		}
		else
		{			
			if(CRainbowRace::GetInstance()->IsAllCompleted())
			{
				if(!CRainbowRace::GetInstance()->m_bRRFinishMsg && CRainbowRace::GetInstance()->m_bCheckEnd)
				{
					CRainbowRace::GetInstance()->m_bRRFinishMsg = TRUE;
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_RAINBOWRACE_MINIGAMEFINISH) ) );
				}
			}
			else
				CRainbowRace::GetInstance()->m_bCheckEnd = FALSE;
		}
	}
	else
	{
		if(g_WndMng.m_pWndRainbowRaceMiniGameButton)
			SAFE_DELETE(g_WndMng.m_pWndRainbowRaceMiniGameButton);
		if(g_WndMng.m_pWndRainbowRaceMiniGame)
			SAFE_DELETE(g_WndMng.m_pWndRainbowRaceMiniGame);
	}
#ifdef __QUIZ
	if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
	{
		if( CQuiz::GetInstance()->GetState() == CQuiz::QE_QUESTION )
		{
			if( g_WndMng.m_pWndQuizEventButton == NULL )
			{
				g_WndMng.m_pWndQuizEventButton = new CWndQuizEventButton;
				if( g_WndMng.m_pWndQuizEventButton )
					g_WndMng.m_pWndQuizEventButton->Initialize();
			}
		}
		else
		{
			if( g_WndMng.m_pWndQuizEventButton )
				SAFE_DELETE( g_WndMng.m_pWndQuizEventButton );
			if( g_WndMng.m_pWndQuizEventQuestionOX )
				SAFE_DELETE( g_WndMng.m_pWndQuizEventQuestionOX );
			if( g_WndMng.m_pWndQuizEventQuestion4C )
				SAFE_DELETE( g_WndMng.m_pWndQuizEventQuestion4C );
		}
		if( CQuiz::GetInstance()->m_nQCount > 0 && ( g_nProcessCnt & 63 ) == 0 && CQuiz::GetInstance()->GetState() == CQuiz::QE_QUESTION )
		{
			--CQuiz::GetInstance()->m_nQCount;
			
			char buffer[64];
			_itoa( CQuiz::GetInstance()->m_nQCount, buffer, 10 );
			
			g_Caption1.RemoveAll();
			g_Caption1.AddCaption( buffer, m_pFontAPITitle, FALSE );
			if( CQuiz::GetInstance()->m_nQCount == 0 )
				g_Caption1.RemoveAll();
		}
	}
	else
	{
		if( g_WndMng.m_pWndQuizEventButton )
			SAFE_DELETE( g_WndMng.m_pWndQuizEventButton );
		if( g_WndMng.m_pWndQuizEventQuestionOX )
			SAFE_DELETE( g_WndMng.m_pWndQuizEventQuestionOX );
		if( g_WndMng.m_pWndQuizEventQuestion4C )
			SAFE_DELETE( g_WndMng.m_pWndQuizEventQuestion4C );
	}
	
#endif // __QUIZ

	return CWndNeuz::Process();
}
HRESULT CWndWorld::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	m_meshArrow.RestoreDeviceObjects();

	m_meshArrowWanted.RestoreDeviceObjects();
	if( m_pFontAPICaption )
		m_pFontAPICaption->RestoreDeviceObjects();
	if( m_pFontAPITitle )
		m_pFontAPITitle->RestoreDeviceObjects();
	if( m_pFontAPITime )
		m_pFontAPITime->RestoreDeviceObjects();
	
#ifdef __YDEBUG
	m_texTarget.RestoreDeviceObjects(m_pApp->m_pd3dDevice);		// 지상에서의 4귀퉁이 타겟그림
	m_texTargetFly.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행중에서의 4귀퉁이 타겟그림.
	m_texTargetArrow.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 타겟이 화면을 벗어났을때 화살표방향표시.
	m_texGauFlight.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.

	m_texMsgIcon.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.
	m_texAttrIcon.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.
	m_texFontDigital.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.
#endif //__YDEBUG	

	return S_OK;
}
HRESULT CWndWorld::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	m_meshArrow.InvalidateDeviceObjects();
	m_meshArrowWanted.InvalidateDeviceObjects();
	
//	m_texTarget.InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );

	if( m_pFontAPICaption )
		m_pFontAPICaption->InvalidateDeviceObjects();
	if( m_pFontAPITitle )
		m_pFontAPITitle->InvalidateDeviceObjects();
	if( m_pFontAPITime )
		m_pFontAPITime->InvalidateDeviceObjects();
	g_CapTime.InvalidateDeviceObjects();
	g_Caption1.InvalidateDeviceObjects();

#ifdef __YDEBUG
	m_texTarget.InvalidateDeviceObjects();		// 지상에서의 4귀퉁이 타겟그림
	m_texTargetFly.InvalidateDeviceObjects();	// 비행중에서의 4귀퉁이 타겟그림.
	m_texTargetArrow.InvalidateDeviceObjects();	// 타겟이 화면을 벗어났을때 화살표방향표시.
	m_texGauFlight.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.


	m_texMsgIcon.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.
	m_texAttrIcon.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.
	m_texFontDigital.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.
#endif //__YDEBUG	

	return S_OK;
}
HRESULT CWndWorld::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	
	m_meshArrow.DeleteDeviceObjects();
	m_meshArrowWanted.DeleteDeviceObjects();
	
	m_texTarget.DeleteDeviceObjects();
	m_texTargetFly.DeleteDeviceObjects();
	m_texLvUp.DeleteDeviceObjects();
	m_texLvDn.DeleteDeviceObjects();
	m_texLvUp2.DeleteDeviceObjects();
	m_texLvDn2.DeleteDeviceObjects();
	m_texGauEmptyNormal.DeleteDeviceObjects();
	m_texGauFillNormal.DeleteDeviceObjects();
	if( m_pFontAPICaption )
		m_pFontAPICaption->DeleteDeviceObjects();
	SAFE_DELETE( m_pFontAPICaption );
	if( m_pFontAPITitle )
		m_pFontAPITitle->DeleteDeviceObjects();
	SAFE_DELETE( m_pFontAPITitle );
	if( m_pFontAPITime )
		m_pFontAPITime->DeleteDeviceObjects();
	SAFE_DELETE( m_pFontAPITime );
	g_CapTime.DeleteDeviceObjects();

	g_Caption1.DeleteDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	
	for(int i=0; i<CUSTOM_LOGO_MAX; i++)
	{
		m_pTextureLogo[i].DeleteDeviceObjects();
	}

	SAFE_DELETE(m_pWndGuideSystem);
#ifdef __YDEBUG
	m_texTargetArrow.DeleteDeviceObjects();	// 타겟이 화면을 벗어났을때 화살표방향표시.
	m_texGauFlight.DeleteDeviceObjects();	// 비행 게이지 인터페이스.		
	m_texMsgIcon.DeleteDeviceObjects();	// 비행 게이지 인터페이스.
	m_texAttrIcon.DeleteDeviceObjects();	// 비행 게이지 인터페이스.
	m_texFontDigital.DeleteDeviceObjects();	// 비행 게이지 인터페이스.
#endif //__YDEBUG	
	m_TexGuildWinner.DeleteDeviceObjects();
	m_TexGuildBest.DeleteDeviceObjects();
	return S_OK;
}

void CWndWorld::UseSkill()
{
	CWorld* pWorld = g_WorldMng.Get();
	
//	CObj* pTargetObj = CObj::m_pObjHighlight;		// 커서를 대고 있던 오브젝트가 하이라이트 오브젝이다.
	CObj* pTargetObj = pWorld->GetObjFocus();		// 선택되어 있는타겟에게 사용
	if( g_pPlayer->m_pActMover->IsFly() == FALSE )
	{
		if( pTargetObj )	// 커서를 대고 있던 오브젝트가 있으면
		{
			pWorld->SetObjFocus( pTargetObj );	// 그놈을 셀렉트 하는 동시에.
			
			CMover* pMover = (CMover*)pTargetObj;
			if( pMover->GetType() == OT_MOVER )
				m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
		} else
			m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
	}
}

DWORD CWndWorld::GetSystemPetTextureKey( IBuff* pBuff )
{
	WORD wId	= pBuff->GetId();
	CPet* pPet	= g_pPlayer->GetPet();
	if( pPet )
	{
		BYTE nLevel		= pPet->GetLevel();
		if( nLevel == PL_D || nLevel == PL_C )
			return MAKELONG( wId, 0 );
		else if( nLevel == PL_B || nLevel == PL_A )
			return MAKELONG( wId, 1 );
		else if( nLevel == PL_S )
			return MAKELONG( wId, 2 );
	}
	return static_cast<DWORD>( wId );
}

void CWndWorld::RenderBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse )
{
	RECT rectHittest;
	std::multimap< DWORD, BUFFSKILL >::value_type* pp = NULL;

	pInfo->pt.x		+= pInfo->nDelta;
	ItemProp* pItem		= NULL;
	WORD wID	= pBuff->GetId();
	int nTexture	= bPlayer? 0: 1;

	if( pBuff->GetType() == BUFF_SKILL )
	{
		if(m_pBuffTexture[nTexture].find(pBuff->GetId()) != m_pBuffTexture[nTexture].end())
			pp	= &( *( m_pBuffTexture[nTexture].find( pBuff->GetId() ) ) );
		pItem	= bPlayer? prj.GetSkillProp( pBuff->GetId() ): prj.GetPartySkill( pBuff->GetId() );
	}
	else if( pBuff->GetType() == BUFF_ITEM || pBuff->GetType() == BUFF_ITEM2 || pBuff->GetType() == BUFF_EQUIP )
	{
		if(m_pBuffTexture[2].find(pBuff->GetId()) != m_pBuffTexture[2].end())
			pp	= &( *( m_pBuffTexture[2].find( pBuff->GetId() ) ) );
		pItem	= prj.GetItemProp( pBuff->GetId() );
	}
	else if( pBuff->GetType() == BUFF_PET )
	{
		if(m_pBuffTexture[2].find(GetSystemPetTextureKey(pBuff)) != m_pBuffTexture[2].end())
			pp	= &( *( m_pBuffTexture[2].find( GetSystemPetTextureKey( pBuff ) ) ) );
		pItem	= prj.GetItemProp( pBuff->GetId() );
	}
	if(pp == NULL)
		return;
	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;

	BOOL bFlash	= FALSE;
	DWORD dwOddTime	= 0;
	if( pBuff->GetTotal() > 0 )
	{	
		dwOddTime	= pBuff->GetTotal() - ( g_tmCurrent - pBuff->GetInst() );
		bFlash	= ( dwOddTime < 20 * 1000 );	// 20초 이하 남았으면 깜빡거림
		if( pItem->dwID == II_SYS_SYS_SCR_RETURN )	// 귀환의 두루마리는 깜빡거림
			bFlash	= TRUE;
	}

	int nAngel = 100;
	__int64 nPercent = 0;
	
	if( pItem->dwItemKind3 == IK3_ANGEL_BUFF )
	{
		IBuff* ptr	= g_pPlayer->m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		if( ptr )
		{
			ItemProp* pItemProp = prj.GetItemProp( ptr->GetId() );
			if( pItemProp )
				nAngel = (int)( (float)pItemProp->nAdjParamVal1 );
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;
		
		EXPINTEGER maxExp = prj.m_aExpCharacter[g_pPlayer->m_nAngelLevel].nExp1 / 100 * nAngel;
		if( maxExp > 0)
		{
			nPercent = ( g_pPlayer->m_nAngelExp * (EXPINTEGER)100 ) / maxExp;
		}
		if(nPercent == 100)
		{
			if(!m_bAngelFinish)
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_ANGELEXP_END ) );
				m_bAngelFinish = TRUE;
			}
			bFlash = TRUE;
		}
		else
			m_bAngelFinish = FALSE;
	}

	D3DXCOLOR color;
	
	if( bFlash )		
	{		
		const int alpha = pp->second.m_flasher.Increment();

		if( pItem->nEvildoing < 0 )							// 나쁜마법은
			color =  D3DCOLOR_ARGB(alpha, 255, 120, 255 );		// 빨간 색 
		else
			color =  D3DCOLOR_ARGB(alpha, 255, 255, 255 );
	}
	else
	{
		if( pItem->nEvildoing < 0 )							// 나쁜마법은
			color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );		// 빨간 색 
		else
			color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );
	}

	p2DRender->RenderTexture2(pInfo->pt, pp->second.m_pTexture, 1, 1, color);
	
	//지속시간이 있는 Buff의 경우 현재 남은 시간을 항상 표시하도록 변경
	if( dwOddTime > 0 && pItem->dwSkillTime != 999999999
		&& pBuff->GetType() != BUFF_EQUIP
		)
	{
		CTimeSpan ct( (long)(dwOddTime / 1000.0f) );
		RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
	}
	else if( pItem->dwItemKind2 == IK2_BUFF2 )
	{
		time_t	t = (time_t)pBuff->GetLevel() - time_null();
		if( t < 0 )
			t	= 0;
		CTimeSpan ts( t );
		RenderOptBuffTime( p2DRender, pInfo->pt, ts, D3DCOLOR_XRGB( 240, 240, 0 ) );
	}
	
	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+32, pInfo->pt.y+32 );
	ClientToScreen( &rectHittest );
	
	CRect rc	= rectHittest;
	if( rc.PtInRect( ptMouse ) )
	{
		CEditString strEdit;
		if( pItem->dwItemRare == 102 )
			strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		else if( pItem->dwItemRare == 103 )
			strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
		else if( pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET )
			strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 46, 112, 169 ), ESSTY_BOLD );
		else if( pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET02 )
			strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 46, 112, 169 ), ESSTY_BOLD );
		else
			strEdit.AddString( pItem->szName, 0xff2fbe6d, ESSTY_BOLD );

		BOOL bItemKind3 = FALSE;
		CString str;
		if( pItem->dwItemKind3 == IK3_ANGEL_BUFF )
		{
			BUFFICONRECT_INFO info;
			CString strPercent;
			CopyRect( &info.rc, &rectHittest );
			info.dwID = pItem->dwID;
			m_rcCheck.push_back( info );

			strPercent.Format("   %d%%", nPercent);
			strEdit.AddString( strPercent, D3DCOLOR_XRGB( 100, 100, 255 ), ESSTY_BOLD );
			bItemKind3	= TRUE;
		}
		else if( pItem->dwItemKind3 == IK3_EGG )
		{
			CItemElem* pItemElem	= g_pPlayer->GetPetItem();
			if( pItemElem != NULL )
			{
				PutPetTooltipInfo( pItemElem, &strEdit );
				bItemKind3	= TRUE;
			}
		}
		else if( pItem->dwItemKind3 == IK3_COUPLE_BUFF )
		{
			bItemKind3	= TRUE;
		}

		else if( pItem->dwItemKind3 == IK3_TS_BUFF )
			bItemKind3	= TRUE;

		if( pItem->dwItemKind3 == IK3_PET )
		{
			CItemElem* pItemElem = g_pPlayer->GetVisPetItem( );
			if( pItemElem && ( pItemElem->m_dwItemId == pItem->dwID ) )
			{
				g_WndMng.PutVisPetInfo( *pItemElem, strEdit );
				bItemKind3 = TRUE;
			}
		}

		switch( pItem->dwID )
		{
			case II_SYS_SYS_SCR_SMELPROT:
#ifdef __SM_ITEM_2ND_EX
			case II_SYS_SYS_SCR_SMELPROT2:
#endif	// __SM_ITEM_2ND_EX
			case II_SYS_SYS_SCR_SMELPROT3:
			case II_SYS_SYS_SCR_SMELPROT4:
			case II_SYS_SYS_SCR_SMELTING:
			case II_SYS_SYS_SCR_SMELTING2:
				str.Format( "\n%s", prj.GetText( TID_GAME_DEMOL_USE ) );
				break;
			case II_SYS_SYS_SCR_RETURN:
				{
					BUFFICONRECT_INFO info;
					CopyRect( &info.rc, &rectHittest );
					info.dwID = pItem->dwID;
					m_rcCheck.push_back( info );			
				}
				break;
			case II_SYS_SYS_SCR_SUPERSMELTING:
				str.Format( "\n%s", prj.GetText( TID_GAME_DEMOL_USE ) );
				break;
			case II_SYS_SYS_SCR_PARTYSUMMON:
				{
					BUFFICONRECT_INFO info;
					CopyRect( &info.rc, &rectHittest );
					info.dwID = pItem->dwID;
					m_rcCheck.push_back( info );

					CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
					if( ct.GetHours() >= 1 )
						str.Format( "\n%.2d:%.2d:%.2d\n%s", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds(), prj.GetText( TID_GAME_SUMMON_BUFF_ICON ) );	//시분초 
					else
						str.Format( "\n%.2d:%.2d\n%s", ct.GetMinutes(), ct.GetSeconds(), prj.GetText( TID_GAME_SUMMON_BUFF_ICON ) );						// 분초
					RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
				}
				break;
			case II_SYS_SYS_SCR_PET_FEED_POCKET:
				{
					//검색해서 활성화 된 먹이 주머니를 찾는다.
					CItemElem* ptr;
					CItemElem* pItemElem = NULL;

					int nMax = g_pPlayer->m_Inventory.GetMax();
					for( int i = 0 ; i < nMax; i++ )
					{
						ptr	= g_pPlayer->m_Inventory.GetAtId( i );
						if( IsUsableItem( ptr ) && ptr->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET &&
							ptr->m_dwKeepTime > 0 && !ptr->IsFlag( CItemElem::expired ) )		// 활성화한 먹이 주머니일 경우
						{
							BUFFICONRECT_INFO info;
							CopyRect( &info.rc, &rectHittest );
							info.dwID = pItem->dwID;
							m_rcCheck.push_back( info );			
							
							pItemElem = ptr;
							i = nMax;
						}
					}

					if(pItemElem != NULL)
					{
						CString strTemp;
						//사용 제한 시한
						time_t t = pItemElem->m_dwKeepTime - time_null();
						{
							if( t > 0 )
							{
								CTimeSpan time( t );
								if( time.GetDays() )
									str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays()+1) );
								else if( time.GetHours() )
									str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
								else if( time.GetMinutes() > 1 )
									str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
								else
									str.Format( prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds() );
							}
							strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
							strEdit.AddString( "\n" );
							strEdit.AddString( strTemp, D3DCOLOR_XRGB( 255, 20, 20 ) );
						}
						//사료 개수
						strEdit.AddString( "\n" );	
						strTemp.Format( "%s %d", prj.GetText( TID_GAME_PET_FEED_COUNT ), g_pPlayer->GetItemNumForClient( II_SYS_SYS_FEED_01 ) );
						strEdit.AddString( strTemp, D3DCOLOR_XRGB( 50, 50, 205 ) );
						//설명
						strEdit.AddString( "\n" );
						strTemp.Format( "%s", pItem->szCommand );
						strEdit.AddString( strTemp, D3DCOLOR_XRGB( 178, 0, 255 ) ); 
					}
				}
				break;
			
#ifdef __PROTECT_AWAKE
			case II_SYS_SYS_SCR_AWAKESAFE:		
				break;
#endif

			default:
				{
					if( bItemKind3 )	// 위쪽에서 처리
						break;
					if( pBuff->GetType() == BUFF_EQUIP )
						break;
					if( pBuff->GetTotal() > 0 )						
					{
						const CTimeSpan ct((long)(dwOddTime / 1000.0f));		// 남은시간을 초단위로 변환해서 넘겨줌
						str = TimeSpanToString::DHMmSs(ct);
					}
					else if( prj.GetItemProp( pBuff->GetId() )->dwItemKind2 == IK2_BUFF2 )
					{
						time_t	t = (time_t)pBuff->GetLevel() - time_null();
						if( t < 0 )
							t	= 0;
						const CTimeSpan ts(t);
						str = TimeSpanToString::DHMmSs(ts);
					}
				}
				break;
		}	// switch

		CString strTemp;
		strTemp.Format( "\n%s", pItem->szCommand );

		if(pItem->dwID != II_SYS_SYS_SCR_PET_FEED_POCKET && pItem->dwID != II_PET_EGG)
			strEdit.AddString( strTemp );

		if( pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET02 )
		{
			strEdit.AddString( "\n" );	
			CString str;
			str.Format( "%s %d", prj.GetText( TID_GAME_PET_FEED_COUNT ), g_pPlayer->GetItemNumForClient( II_SYS_SYS_FEED_01 ) );
			strEdit.AddString( str, D3DCOLOR_XRGB( 50, 50, 205 ) );
		}
		g_WndMng.PutDestParam( pItem->dwDestParam[0], pItem->dwDestParam[1], 
			pItem->nAdjParamVal[0], pItem->nAdjParamVal[1], strEdit );
		g_WndMng.PutDestParam( pItem->dwDestParam[2], 0, pItem->nAdjParamVal[2], 0, strEdit );

		if( pBuff->GetType() == BUFF_SKILL )
		{
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pItem->dwSubDefine, pBuff->GetLevel() );

			if( pAddSkillProp )
			{
				g_WndMng.PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}

		if(pItem->dwID != II_SYS_SYS_SCR_PET_FEED_POCKET 
			&& pItem->dwID != II_PET_EGG)
			strEdit.AddString( str );
		if( pItem->dwID == II_PCBANG_BUFF01 )
		{
			CTimeSpan ts( CPCBangInfo::GetInstance()->GetConnectTime() );
			strTemp.Format( prj.GetText( TID_GAME_PCBANGINFO_TIME ), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds() );
			strTemp = '\n' + strTemp;	strEdit.AddString( strTemp, prj.GetTextColor( TID_GAME_PCBANGINFO_TIME ) );
			
			strTemp.Format( prj.GetText( TID_GAME_PCBANGINFO_EXP ), ( CPCBangInfo::GetInstance()->GetExpFactor()-1.0f ) * 100.0f );
			strTemp = '\n' + strTemp;	strEdit.AddString( strTemp, prj.GetTextColor( TID_GAME_PCBANGINFO_EXP ) );
			
			strTemp.Format( prj.GetText( TID_GAME_PCBANGINFO_ITEMDROP ),( CPCBangInfo::GetInstance()->GetPieceItemDropFactor()-1.0f ) * 100.0f );
			strTemp = '\n' + strTemp;	strEdit.AddString( strTemp, prj.GetTextColor( TID_GAME_PCBANGINFO_ITEMDROP ) );
		}
#ifdef __VTN_TIMELIMIT
		//	mulcom	BEGIN100315	베트남 시간 제한
		if( ::GetLanguage() == LANG_VTN )
		{
			if( g_pPlayer->m_nAccountPlayTime > -1 && pItem->dwID == II_VIETNAM_BUFF01 )
			{
				DWORD dwTime = g_tmCurrent - g_pPlayer->m_nAccountPlayTime;

				CTimeSpan ts( dwTime / SEC( 1 ) );
				strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_PLAYTIME_1 ), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds() );
				//strTemp = '\n' + strTemp;
				strEdit.AddString( strTemp, prj.GetTextColor( TID_GAME_TOOLTIP_PLAYTIME_1 ) );

				float	fGettingRate = 100.0f;

				if( dwTime > MIN( 300 ) )
				{
					fGettingRate	= 0.0f;
				}
				else if( dwTime <= MIN( 300 ) && dwTime >= MIN( 180 ) )
				{
					fGettingRate	= 50.0f;
				}
				else
				{
					fGettingRate	= 100.0f;
				}

				strTemp.Format( prj.GetText( TID_GAME_PCBANGINFO_EXP ), fGettingRate );
				strTemp = '\n' + strTemp;
				strEdit.AddString( strTemp, prj.GetTextColor( TID_GAME_PCBANGINFO_EXP ) );

				strTemp.Format( prj.GetText( TID_GAME_PCBANGINFO_ITEMDROP ), fGettingRate );
				strTemp = '\n' + strTemp;
				strEdit.AddString( strTemp, prj.GetTextColor( TID_GAME_PCBANGINFO_ITEMDROP ) );

				if( fGettingRate != 100.0f )
				{
					strTemp.Format( prj.GetText( TID_GAME_TOOLTIP_PLAYTIME_2 ), fGettingRate );
					strTemp = '\n' + strTemp;
					strEdit.AddString( strTemp, prj.GetTextColor( TID_GAME_TOOLTIP_PLAYTIME_2 ) );
				}
			}
		}
		//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT

		if( pItem->dwItemKind3 == IK3_PET )
		{
			// 각성 정보 출력 
			CItemElem* pItemElem = g_pPlayer->GetVisPetItem( );
			if( pItemElem && ( pItemElem->m_dwItemId == pItem->dwID ) )
				g_WndMng.PutAwakeningBlessing( *pItemElem, strEdit );
		}

		g_toolTip.PutToolTip( wID, strEdit, rectHittest, ptMouse, 1 );	

	}

	++pInfo->nCount;
	if( (pInfo->nCount % m_nLimitBuffCount) == 0 )
	{
		if( pBuff->GetType() == BUFF_SKILL )
			pInfo->pt.x  = (m_rectWindow.Width() / 2) - 100;
		else
			pInfo->pt.x  = (m_rectWindow.Width() / 2) + 112;

		pInfo->pt.y += GetBuffTimeGap();
	}		
}

void CWndWorld::PutPetTooltipInfo( CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem == NULL || pEdit == NULL )
		return;
	
	CString strTemp;
	pEdit->Empty();
	//Name
//	strTemp.Format( "%s", pItemElem->GetProp()->szName );
	strTemp		= pItemElem->GetName();
	pEdit->AddString( strTemp, D3DCOLOR_XRGB(46, 112, 169), ESSTY_BOLD );
	
	//Level
	if( pItemElem->m_pPet )
	{
		const PETLEVEL nLevel = pItemElem->m_pPet->GetPetLevel();
		const DWORD dwLevelText = CPetProperty::GetTIdOfLevel(nLevel);
		strTemp.Format( "%s : %s", prj.GetText(TID_GAME_CHARACTER_02), prj.GetText(dwLevelText) );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, D3DCOLOR_XRGB(0, 158, 0) );
		
		//Ability value
		if(nLevel != PL_EGG)
		{
			const SINGLE_DST dst = pItemElem->m_pPet->GetAvailDestParam();
			const DWORD dwTooltip = CPetProperty::GetTIdOfDst(dst);
			
			strTemp.Format("%s : %s +%d", prj.GetText(TID_GAME_ABILITY), prj.GetText(dwTooltip), dst.nAdj);
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, D3DCOLOR_XRGB(255, 0, 0) );
		}
		//Level History
		if(nLevel > PL_EGG)
		{
			pEdit->AddString( "\n" );
			pEdit->AddString( "(", D3DCOLOR_XRGB(0, 200,255) );
			for(int i=PL_D; i<=nLevel; i++)
			{
				BYTE bLevel = pItemElem->m_pPet->GetAvailLevel(i);
				strTemp.Format("Lv%d", bLevel);
				pEdit->AddString( strTemp, D3DCOLOR_XRGB(0, 127,255) );
				if(i != nLevel)
					pEdit->AddString( "/", D3DCOLOR_XRGB(0, 200,255) );
			}
			pEdit->AddString( ")", D3DCOLOR_XRGB(0, 200,255) );
		}
		
		//Description
//		strTemp.Format( "%s", pItemElem->GetProp()->szCommand );
//		pEdit->AddString( "\n" );
//		pEdit->AddString( strTemp, D3DCOLOR_XRGB(178, 0, 255) );
	}
}

CString TimeSpanToString::DHMmSs(const CTimeSpan timeSpan) {
	CString str;
	if (timeSpan.GetDays() != 0) {
		str.Format(
			"\n%.2d:%.2d:%.2d:%.2d",
			static_cast<int>(timeSpan.GetDays()),
			timeSpan.GetHours(), timeSpan.GetMinutes(), timeSpan.GetSeconds()
		);
	} else if (timeSpan.GetHours() >= 1) {
		str.Format("\n%.2d:%.2d:%.2d", timeSpan.GetHours(), timeSpan.GetMinutes(), timeSpan.GetSeconds());
	} else {
		str.Format("\n%.2d:%.2d", timeSpan.GetMinutes(), timeSpan.GetSeconds());
	}

	return str;
}

void CWndWorld::RenderExpBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BUFFICON_INFO* pInfo, CPoint ptMouse, DWORD dwItemID )
{
	// 경험치 중복 아이템 : 루프를 돌아서 몇개인지 확인. 버프아이콘은 한개, 설명은 3개면 3개.
	int nExpCount = 0;
	DWORD dwExpTime[3];
	ZeroMemory( dwExpTime, sizeof(dwExpTime) );
	BOOL IsOverlap = TRUE;
	BOOL bSpecial = FALSE;

	if( dwItemID == II_SYS_SYS_SCR_AMPESA || dwItemID == II_SYS_SYS_SCR_AMPESA1 || dwItemID == II_SYS_SYS_SCR_AMPESA2 )
		dwItemID = II_SYS_SYS_SCR_AMPESA;
	else if( dwItemID == II_SYS_SYS_SCR_AMPESB || dwItemID == II_SYS_SYS_SCR_AMPESB1 || dwItemID == II_SYS_SYS_SCR_AMPESB2 )
		dwItemID = II_SYS_SYS_SCR_AMPESB;
	else if( dwItemID == II_SYS_SYS_SCR_AMPESC || dwItemID == II_SYS_SYS_SCR_AMPESC1 || dwItemID == II_SYS_SYS_SCR_AMPESC2 )
		dwItemID = II_SYS_SYS_SCR_AMPESC;
	else if( dwItemID == II_SYS_SYS_SCR_AMPESD || dwItemID == II_SYS_SYS_SCR_AMPESD1 || dwItemID == II_SYS_SYS_SCR_AMPESD2 )
		dwItemID = II_SYS_SYS_SCR_AMPESD;
#ifdef __S_ADD_EXP
	else if( dwItemID == II_SYS_SYS_SCR_AMPESS || dwItemID == II_SYS_SYS_SCR_AMPESS1 || dwItemID == II_SYS_SYS_SCR_AMPESS2 )
	{
		dwItemID = II_SYS_SYS_SCR_AMPESS;	bSpecial = TRUE;
	}
#endif // __S_ADD_EXP
	else if( dwItemID == II_SYS_SYS_SCR_AMPESE)
		IsOverlap = FALSE;

	for( MAPBUFF::iterator it = g_pPlayer->m_buffs.m_mapBuffs.begin(); it != g_pPlayer->m_buffs.m_mapBuffs.end(); ++it )
	{
		IBuff* ptr	= it->second;
		if( dwItemID == II_SYS_SYS_SCR_AMPESE ) // 중복되지 않는 ES증폭의 두루마리 defineitem에서 처리 잘못되어 따로 분기
		{
			if( ptr->GetId() == dwItemID )
			{
				dwExpTime[0]	= ptr->GetTotal() - ( g_tmCurrent - ptr->GetInst() );
				nExpCount = 1;
				break;
			}
		}
		else
		{
			if( ptr->GetId() == dwItemID || ptr->GetId() == dwItemID + 1 || ptr->GetId() == dwItemID + 2 )	
			{
				if(IsOverlap)
				{
					dwExpTime[nExpCount]	= ptr->GetTotal() - ( g_tmCurrent - ptr->GetInst() );
					++nExpCount;				
				}
				else
				{
					dwExpTime[0]	= ptr->GetTotal() - ( g_tmCurrent - ptr->GetInst() );
					nExpCount	= 1;
					break;
				}
			}
		}
	}

	int nTexture;
	RECT rectHittest;	
	std::multimap< DWORD, BUFFSKILL >::value_type* pp = NULL;
	
	nTexture = 0;
	pInfo->pt.x += pInfo->nDelta;
	
	ItemProp* pItem = NULL;

	if( pBuff->GetType() == BUFF_SKILL )
	{
		if(m_pBuffTexture[nTexture].find(pBuff->GetId()) != m_pBuffTexture[nTexture].end())
			pp = &(*(m_pBuffTexture[nTexture].find( pBuff->GetId() )));
		pItem = prj.GetSkillProp( pBuff->GetId() );
	}
	else
	{
		if(m_pBuffTexture[2].find(dwItemID) != m_pBuffTexture[2].end())
			pp = &(*(m_pBuffTexture[2].find( dwItemID )));
		pItem = prj.GetItemProp( dwItemID );
	}
	if(pp == NULL)
		return;
	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;

	D3DXCOLOR color;

	if( pItem->nEvildoing < 0 )	
		color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );
	else
		color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );

	
	for( int j=0 ; j < nExpCount ; ++j )
	{
		if( j == 0 )
			p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1, 1, color );
		else if( j == 1 )
			p2DRender->RenderTexture2( CPoint( pInfo->pt.x + 5, pInfo->pt.y+30 ), pp->second.m_pTexture, 0.5f, 0.5f, color );
		else 
			p2DRender->RenderTexture2( CPoint( pInfo->pt.x + 22, pInfo->pt.y+30 ), pp->second.m_pTexture, 0.5f, 0.5f, color );
	}

	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+32, pInfo->pt.y+32 );
	ClientToScreen( &rectHittest );
	
	CEditString strEdit;
	strEdit.AddString( pItem->szName, 0xff0000ff );
	
	float fFactor = 1;
	int nLevel = g_pPlayer->GetLevel();
	switch( nExpCount )
	{
	case 1:
		{
			fFactor *= 1.5f;
		}
		break;
	case 2:
		{
			if( bSpecial || nLevel < 61 )
				fFactor *= 2.0f;
			else
				fFactor *= 1.5f;
		}
		break;
	case 3:
		{
			if( bSpecial || nLevel < 41 )
				fFactor *= 2.5f;
			else if( nLevel < 61 )
				fFactor *= 2.0f;
			else
				fFactor *= 1.5f;
		}
		break;
	}
	
	int nMaxExpCount = 1;
	if( nLevel < 41 )
		nMaxExpCount = 3;
	else if( nLevel < 61 )
		nMaxExpCount = 2;
	
	DWORD dwColor;
	if( nExpCount == 1 )
		dwColor = D3DCOLOR_XRGB( 240, 240, 0 );
	else if( nExpCount == 2 )
		dwColor = D3DCOLOR_XRGB( 0, 240, 0 );
	
	if( nMaxExpCount <= nExpCount )
		dwColor = D3DCOLOR_XRGB( 240, 0, 0 );
	
	CString strTemp;
	//	strTemp.Format( "X %d", nExpCount );
	//	p2DRender->TextOut(  pInfo->pt.x+5, pInfo->pt.y+32, strTemp, dwColor );
	strEdit.AddString( '\n' );
	int nPercent = (int)( (fFactor - 1.0f) * 100.0f );
	strEdit.AddString( prj.GetText( TID_GAME_EXPITEM_TOOLTIP ) );
	strTemp.Format( " %d%% ", nPercent );
	strEdit.AddString( strTemp, D3DCOLOR_ARGB( 255, 255, 90, 80 ) );
	strEdit.AddString( prj.GetText( TID_GAME_EXPITEM_TOOLTIP1 ) );
	strEdit.AddString( '\n' );
	strEdit.AddString( prj.GetText( TID_GAME_EXP_COUTMSG0 ));//, 0xff99cc00 );
	for( int i = 0 ; i < nExpCount ; ++i )
	{
		DWORD dwMsg = TID_GAME_EXP_COUTMSG3;
		if( i == 0 )
			dwMsg = TID_GAME_EXP_COUTMSG1;
		else if( i == 1 )
			dwMsg = TID_GAME_EXP_COUTMSG2;
		
		strEdit.AddString( "\n  " );
		strEdit.AddString( prj.GetText( dwMsg ) );
		
		CString strTime;
		DWORD dwOddTime = dwExpTime[i];		
		CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
		if( ct.GetHours() >= 1 )
			strTime.Format( " %.2d:%.2d:%.2d ", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
		else
			strTime.Format( " %.2d:%.2d ", ct.GetMinutes(), ct.GetSeconds() );						// 분초
		strEdit.AddString( strTime );
		//		if( i+1 < nExpCount )
		//			strEdit.AddString( '/', D3DCOLOR_ARGB( 255, 220, 80, 200 ) );
		//		RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
	}
	
	
	g_toolTip.PutToolTip( dwItemID, strEdit, rectHittest, ptMouse, 1 );	
	
	++pInfo->nCount;
	if( (pInfo->nCount % m_nLimitBuffCount) == 0 )
	{
		if( pBuff->GetType() == BUFF_SKILL )
			pInfo->pt.x  = (m_rectWindow.Width() / 2) - 100;
		else
			pInfo->pt.x  = (m_rectWindow.Width() / 2) + 75;

		pInfo->pt.y += GetBuffTimeGap();
	}
}

//TODO m_rcCheck를 넣을 것. 
void CWndWorld::RenderSMBuff( C2DRender *p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse )
{
	RECT rectHittest;
	
	for( int i = 0; i < SM_MAX ; ++i )
	{
		if( g_pPlayer->m_dwSMTime[i] <= 0 )
			continue;
		
		int nResistTexture = 1000;
		if( i == SM_RESIST_ATTACK_LEFT )
			nResistTexture = g_pPlayer->m_nAttackResistLeft - 1;
		else if( i == SM_RESIST_ATTACK_RIGHT )
			nResistTexture = g_pPlayer->m_nAttackResistRight - 1;
		else if( i == SM_RESIST_DEFENSE )
			nResistTexture = g_pPlayer->m_nDefenseResist + 5 - 1;
		
		ItemProp* pItem = prj.GetItemProp( g_AddSMMode.dwSMItemID[i] );
		
		if( pItem != NULL &&
			( m_dwSMItemTexture[i] != NULL || 
			( ( i == SM_RESIST_ATTACK_LEFT || i== SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE ) && m_dwSMResistItemTexture[nResistTexture] != NULL && SAI79::END_PROP > nResistTexture ) 
			) )
		{
			CEditString strEdit;
			CTimeSpan ct( g_pPlayer->m_dwSMTime[i] );
			if( 0 == ct.GetDays() && 0 == ct.GetHours() && ct.GetMinutes() <= 30 || 
				i == SM_STR_DOWN || i == SM_STA_DOWN || i == SM_INT_DOWN || i == SM_DEX_DOWN )
			{
				pInfo->pt.x += (32+5);
				
				if( g_pPlayer->m_dwSMTime[i] < 60 && i != SM_REVIVAL )		// 20초 이하 남았으면 깜빡거림.
				{
					if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE )
						p2DRender->RenderTexture( pInfo->pt, m_dwSMResistItemTexture[nResistTexture], m_nSMAlpha[i] );
					else
						p2DRender->RenderTexture( pInfo->pt, m_dwSMItemTexture[i], m_nSMAlpha[i] );
					
					if( m_bSMFlsh[i] == TRUE )
					{
						m_nSMAlpha[i]+=6;
						
						if( m_nSMAlpha[i] > 192 )
						{
							m_nSMAlpha[i] = 192;
							m_bSMFlsh[i] = FALSE;
						}
					}
					else
					{
						m_nSMAlpha[i]-=6;
						
						if( m_nSMAlpha[i] < 64 )
						{
							m_nSMAlpha[i] = 64;
							m_bSMFlsh[i] = TRUE;
						}
					}
				}
				else
				{
					if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE )
						p2DRender->RenderTexture( pInfo->pt, m_dwSMResistItemTexture[nResistTexture], 192 );
					else
						p2DRender->RenderTexture( pInfo->pt, m_dwSMItemTexture[i], 192 );
				}
				
				SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+32, pInfo->pt.y+32 );
				ClientToScreen( &rectHittest );
				if( pItem->dwItemRare == 102 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
				else if( pItem->dwItemRare == 103 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
				else
					strEdit.AddString( pItem->szName, 0xff2fbe6d, ESSTY_BOLD );
				
				CString str;
				if( pItem->dwCircleTime == 1 )
					str.Format( "\n%s", prj.GetText( TID_GAME_COND_USE ) );
				else
				{
					if( ct.GetHours() >= 1 )
						str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
					else
						str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );
				}
				strEdit += str;

				//시간
				g_toolTip.PutToolTip( g_AddSMMode.dwSMItemID[i], strEdit, rectHittest, ptMouse, 1 );
				
				pInfo->nCount++;
				
				if( pItem->dwCircleTime != 1 )
					RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
				
				if( (pInfo->nCount % m_nLimitBuffCount) == 0 )
				{
					pInfo->pt.x  = (m_rectWindow.Width() / 2) + 75;
					pInfo->pt.y += GetBuffTimeGap();
				}
			}
		}
	}
}

// 스킬버프는 모두 왼쪽에, 아이템 버프는 오른쪽에 표시한다.
void CWndWorld::RenderBuff(C2DRender *p2DRender)
{
	_PROFILE("CWndWorld::RenderBuff()");

	m_rcCheck.clear();

	if( g_pPlayer == NULL )
		return;
	int i=0, nCount = 0;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO left, right;
	
	left.pt = CPoint( (m_rectWindow.Width() / 2) - 100, 10 );
	left.nCount = 0;
	left.nDelta = -(32+5);
	right.pt = CPoint( (m_rectWindow.Width() / 2) + 112, 10 );
	right.nCount = 0;	
	right.nDelta = (32+5);

	BUFFICON_INFO* pInfo;
	BOOL bExpRander[6];
	ZeroMemory( bExpRander, sizeof( bExpRander ) );
	for( MAPBUFF::iterator it = g_pPlayer->m_buffs.m_mapBuffs.begin(); it != g_pPlayer->m_buffs.m_mapBuffs.end(); ++it )
	{
		IBuff* pBuff	= it->second;
		WORD wType	= pBuff->GetType();
		WORD wId	= pBuff->GetId();

		if( wId  )	
		{
			if( wType == BUFF_SKILL )
				pInfo = &left;
			else
				pInfo = &right;

			if( wType == BUFF_ITEM )
			{
				ItemProp* pItemProp		= prj.GetItemProp( wId );
				if( pItemProp && pItemProp->dwItemKind3 == IK3_TICKET )
					continue;
				if( pItemProp && pItemProp->dwItemKind1 == IK1_HOUSING )
					continue;
			}

			int nExpkind = 100;
			if( wId == II_SYS_SYS_SCR_AMPESA || wId == II_SYS_SYS_SCR_AMPESA1 || wId == II_SYS_SYS_SCR_AMPESA2 )
				nExpkind = 0;
			else if( wId == II_SYS_SYS_SCR_AMPESB || wId == II_SYS_SYS_SCR_AMPESB1 || wId == II_SYS_SYS_SCR_AMPESB2 )
				nExpkind = 1;
			else if( wId == II_SYS_SYS_SCR_AMPESC || wId == II_SYS_SYS_SCR_AMPESC1 || wId == II_SYS_SYS_SCR_AMPESC2 )
				nExpkind = 2;
			else if( wId == II_SYS_SYS_SCR_AMPESD || wId == II_SYS_SYS_SCR_AMPESD1 || wId == II_SYS_SYS_SCR_AMPESD2 )
				nExpkind = 3;
			else if( wId == II_SYS_SYS_SCR_AMPESE )
				nExpkind = 4;
			else if( wId == II_SYS_SYS_SCR_AMPESS || wId == II_SYS_SYS_SCR_AMPESS1 || wId == II_SYS_SYS_SCR_AMPESS2 )
				nExpkind = 5;
			
			if( nExpkind != 100 )
			{
				if( bExpRander[nExpkind] == FALSE )
					RenderExpBuffIcon( p2DRender, pBuff, pInfo, ptMouse, wId );
				bExpRander[nExpkind] = TRUE;
			}
			else
			{
				if( wType != BUFF_SKILL )
					RenderBuffIcon( p2DRender, pBuff, TRUE, pInfo, ptMouse );
			}
		}
	}

	BOOL	bNearByLeader = false;
	CMover *pLeader = prj.GetUserByID( g_Party.m_aMember[0].m_uPlayerId );
	if( IsValidObj( (CObj*)pLeader ) )
        bNearByLeader = true;


	if( g_Party.m_nModeTime[PARTY_PARSKILL_MODE] || bNearByLeader )
	{
		for( MAPBUFF::iterator it2 = m_buffs.m_mapBuffs.begin(); it2 != m_buffs.m_mapBuffs.end(); ++it2 )
		{
			IBuff* ptr2	= it2->second;
			pInfo = &left;
			RenderBuffIcon( p2DRender, ptr2, FALSE, pInfo, ptMouse );		// 
		}
	}

	// 상용화 아이템 버프
	RenderSMBuff( p2DRender, &right, ptMouse );

	RenderEventIcon( p2DRender, &right, ptMouse );
}

#define	TTI_LORD_EVENT	123456789
void CWndWorld::RenderEventIcon( C2DRender* p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse )
{
	RECT rectHittest;
	ILordEvent* pEvent	= CCLord::Instance()->GetEvent();
	for( int i = 0; i < pEvent->GetComponentSize(); i++ )
	{
		CLEComponent* pComponent	= pEvent->GetComponentAt( i );
		pInfo->pt.x	+= ( 32 + 5 );
		p2DRender->RenderTexture( pInfo->pt, pComponent->GetTexture(), 192 );
		SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x + 32, pInfo->pt.y + 32 );
		ClientToScreen( &rectHittest );
		// 군주 %s님이 경험치 %3.1f%%, 드롭률 %3.1f%% 상승 이벤트를 진행 중 입니다."
		CEditString editString;
		char szTooltip[255]	= { 0,};
		sprintf( szTooltip, prj.GetText( TID_GAME_LORD_EVENT_TOOLTIP ),
			CPlayerDataCenter::GetInstance()->GetPlayerString( pComponent->GetIdPlayer() ),
			pComponent->GetEFactor() * 100, pComponent->GetIFactor() * 100 );
		editString.AddString( szTooltip, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		CString strRest;
		CTimeSpan timeSpan( 0, 0, pComponent->GetTick(), 0 );
		strRest.Format( "\n%d", timeSpan.GetTotalMinutes() );
		editString	+= strRest;
		g_toolTip.PutToolTip( TTI_LORD_EVENT, editString, rectHittest, ptMouse, 1 );
		pInfo->nCount++;
		RenderOptBuffTime( p2DRender, pInfo->pt, timeSpan, D3DCOLOR_XRGB( 240, 240, 0 ) );
		if( ( pInfo->nCount % m_nLimitBuffCount ) == 0 )
		{
			pInfo->pt.x		= ( m_rectWindow.Width() / 2 ) + 75;
			pInfo->pt.y		+= GetBuffTimeGap();
		}
	}
}


void CWndWorld::RenderCasting(C2DRender *p2DRender)
{
	if( g_pPlayer && g_pPlayer->IsStateMode( STATE_BASEMOTION_MODE ) )
	{
		if( g_pPlayer->m_nReadyTime )
		{
			CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( g_pPlayer->m_dwUseItemId );
			if( pItemElem )
			{
				DWORD dwTime = g_tmCurrent - g_pPlayer->m_nReadyTime;
				// 게이지 출력
				CRect rect;// = GetClientRect();
				int nWidthClient = 200;
				int nWidth;

				nWidth = (int)( nWidthClient * ( (float)dwTime / (float)pItemElem->GetProp()->dwSkillReadyType ) );
				TRACE( "%d / %d : %f\n", pItemElem->GetProp()->dwSkillReadyType, dwTime, ( (float)dwTime / (float)pItemElem->GetProp()->dwSkillReadyType ) );
				int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
				rect = CRect( nPos, GetClientRect().Height() / 2, nPos + nWidthClient, 0 );
				CRect rectTemp = rect; 
				rectTemp.right = rectTemp.left + nWidth;
				ClientToScreen( rect );
				ClientToScreen( rectTemp );
				m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rect, D3DCOLOR_ARGB( 200, 255, 255, 255 ), m_pVBGauge, &m_texGauEmptyNormal );
				m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
				m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB( 128, 255, 15, 15 ), m_pVBGauge, &m_texGauEmptyNormal );
				m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
			}
		}
	}
}


void CWndWorld::RenderMoverBuff( CMover* pMover, C2DRender *p2DRender)
{
	if( pMover == NULL || p2DRender == NULL )
		return;

	int nIconSize = 24;
	
	CPoint Lpoint = CPoint( ( GetClientRect().Width() - 200 ) / 2 - 20, 44 );
	
	int Count = 0;
	RECT rectHittest;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );

	BOOL bExpRander = FALSE;
	
	// 일반 스킬 버프 표시
	for( MAPBUFF::iterator i = pMover->m_buffs.m_mapBuffs.begin(); i != pMover->m_buffs.m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		WORD wType	= pBuff->GetType();
		DWORD dwSkillID	= pBuff->GetId();
		ItemProp* pItem = NULL;
		if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESA1 || dwSkillID == II_SYS_SYS_SCR_AMPESA2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESA;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESB || dwSkillID == II_SYS_SYS_SCR_AMPESB1 || dwSkillID == II_SYS_SYS_SCR_AMPESB2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESB;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESC1 || dwSkillID == II_SYS_SYS_SCR_AMPESC2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESC;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESD || dwSkillID == II_SYS_SYS_SCR_AMPESD1 || dwSkillID == II_SYS_SYS_SCR_AMPESD2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESD;
#ifdef __S_ADD_EXP
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESS || dwSkillID == II_SYS_SYS_SCR_AMPESS1 || dwSkillID == II_SYS_SYS_SCR_AMPESS2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESS;
#endif // __S_ADD_EXP
		
		if( wType == BUFF_SKILL )
			pItem = prj.GetSkillProp( dwSkillID );
		else
			pItem = prj.GetItemProp( dwSkillID );

		std::multimap< DWORD, BUFFSKILL >::value_type* pp = NULL;

		if( wType == BUFF_ITEM 
#ifdef __JEFF_11_1
			|| wType == BUFF_ITEM2 
#endif	// __JEFF_11_1
#ifdef __DST_GIFTBOX
			|| wType == BUFF_EQUIP
#endif // __DST_GIFTBOX
			)
		{
			if(m_pBuffTexture[2].find(dwSkillID) != m_pBuffTexture[2].end())
				pp = &(*(m_pBuffTexture[2].find(dwSkillID)));
		}
		else if( wType == BUFF_PET )
		{
			if(m_pBuffTexture[2].find(dwSkillID) != m_pBuffTexture[2].end())
				pp = &(*(m_pBuffTexture[2].find(dwSkillID)));
		}
		else if( wType == BUFF_SKILL )
		{
			if(m_pBuffTexture[0].find(dwSkillID) != m_pBuffTexture[0].end())
				pp = &(*(m_pBuffTexture[0].find(dwSkillID)));
		}
		else
		{
			if(m_pBuffTexture[1].find(dwSkillID) != m_pBuffTexture[1].end())
				pp = &(*(m_pBuffTexture[1].find(dwSkillID)));
		}
		
		BOOL bCharged = FALSE;
		if( wType == BUFF_ITEM 
#ifdef __JEFF_11_1
			|| wType == BUFF_ITEM2 
#endif	// __JEFF_11_1
#ifdef __DST_GIFTBOX
//			|| wType == BUFF_EQUIP
#endif // __DST_GIFTBOX
			)
		{
			ItemProp* pItemProp = prj.GetItemProp( dwSkillID );
			if( pItemProp )
			{
				if( pItemProp->bCharged )	// 상용화 아이템 이면 안그려줌
					bCharged = TRUE;
				else if( pItemProp->dwItemKind1 == IK1_HOUSING )	// 하우징 버프는 안그린다.
					bCharged = TRUE;
			}
		}

		if( pp != NULL && pp->second.m_pTexture != NULL && bCharged == FALSE )
		{
			DWORD dwOddTime = pBuff->GetTotal() - ( g_tmCurrent - pBuff->GetInst() );

			if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESA1 || dwSkillID == II_SYS_SYS_SCR_AMPESA2 
				|| dwSkillID == II_SYS_SYS_SCR_AMPESB || dwSkillID == II_SYS_SYS_SCR_AMPESB1 || dwSkillID == II_SYS_SYS_SCR_AMPESB2 
				|| dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESC1 || dwSkillID == II_SYS_SYS_SCR_AMPESC2 
				|| dwSkillID == II_SYS_SYS_SCR_AMPESD || dwSkillID == II_SYS_SYS_SCR_AMPESD1 || dwSkillID == II_SYS_SYS_SCR_AMPESD2 
#ifdef __S_ADD_EXP
				|| dwSkillID == II_SYS_SYS_SCR_AMPESS || dwSkillID == II_SYS_SYS_SCR_AMPESS1 || dwSkillID == II_SYS_SYS_SCR_AMPESS2 
#endif // __S_ADD_EXP
				|| dwSkillID == II_SYS_SYS_SCR_AMPESE 
				)
				
			{
				if( bExpRander == FALSE )
				{
					Lpoint.x += nIconSize;
					pp->second.m_pTexture->Render( p2DRender, Lpoint, CPoint(nIconSize,nIconSize), 192 );
				}
			}
			else
			{
				Lpoint.x += nIconSize;
				if( pBuff->GetTotal() > 0 && dwOddTime < 20 * 1000 )		// 20초 이하 남았으면 깜빡거림.					
				{
					const int alpha = pp->second.m_flasher.Get();
					pp->second.m_pTexture->Render( p2DRender, Lpoint, CPoint(nIconSize,nIconSize), alpha );
					pp->second.m_flasher.Increment();
				}
				else
				{
					pp->second.m_pTexture->Render( p2DRender, Lpoint, CPoint(nIconSize,nIconSize), 192 );
				}				
			}

			BOOL bExpMsg = TRUE;
			if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESA1 || dwSkillID == II_SYS_SYS_SCR_AMPESA2
				|| dwSkillID == II_SYS_SYS_SCR_AMPESB || dwSkillID == II_SYS_SYS_SCR_AMPESB1 || dwSkillID == II_SYS_SYS_SCR_AMPESB2
				|| dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESC1 || dwSkillID == II_SYS_SYS_SCR_AMPESC2
				|| dwSkillID == II_SYS_SYS_SCR_AMPESD || dwSkillID == II_SYS_SYS_SCR_AMPESD1 || dwSkillID == II_SYS_SYS_SCR_AMPESD2
#ifdef __S_ADD_EXP
				|| dwSkillID == II_SYS_SYS_SCR_AMPESS || dwSkillID == II_SYS_SYS_SCR_AMPESS1 || dwSkillID == II_SYS_SYS_SCR_AMPESS2 
#endif // __S_ADD_EXP
				|| dwSkillID == II_SYS_SYS_SCR_AMPESE 
				)
			{
				if( bExpRander )
					bExpMsg = FALSE;
				bExpRander = TRUE;
			}
			
			if( bExpMsg )
			{
				SetRect( &rectHittest, Lpoint.x, Lpoint.y, Lpoint.x+nIconSize, Lpoint.y+nIconSize );
				ClientToScreen( &rectHittest );
				
				CString str;
				str.Format( "\n%s", pItem->szCommand );	
				
				CEditString strEdit;
				
				if( pItem->dwItemRare == 102 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
				else if( pItem->dwItemRare == 103 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
				else
					strEdit.AddString( pItem->szName, 0xff2fbe6d, ESSTY_BOLD );
				
				strEdit += str;

				BOOL bTime = TRUE;
				if( dwSkillID == II_SYS_SYS_SCR_AMPES || dwSkillID == II_SYS_SYS_SCR_SMELPROT || dwSkillID == II_SYS_SYS_SCR_SMELPROT2 
					|| dwSkillID == II_SYS_SYS_SCR_SMELTING || dwSkillID == II_SYS_SYS_SCR_RETURN || dwSkillID == II_SYS_SYS_SCR_SUPERSMELTING
					|| dwSkillID == II_SYS_SYS_SCR_SMELPROT3
					|| dwSkillID == II_SYS_SYS_SCR_SMELPROT4
					|| dwSkillID == II_SYS_SYS_SCR_SMELTING2
#ifdef __DST_GIFTBOX
					|| wType == BUFF_EQUIP
#endif // __DST_GIFTBOX
					)
					bTime = FALSE;
				
				if( bTime && pBuff->GetTotal() > 0  )					
				{
					CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
					str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
					strEdit += str;
				}
				else
				{
					g_WndMng.PutDestParam( pItem->dwDestParam[0], pItem->dwDestParam[1],
						pItem->nAdjParamVal[0], pItem->nAdjParamVal[1], strEdit );
				#ifdef __DST_GIFTBOX
					g_WndMng.PutDestParam( pItem->dwDestParam[2], 0,
						pItem->nAdjParamVal[2], 0, strEdit );
				#endif // __DST_GIFTBOX
					
					if( wType == BUFF_SKILL )
					{
						AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pItem->dwSubDefine, pBuff->GetLevel() );
						
						if( pAddSkillProp )
						{
							g_WndMng.PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
								pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
						}
					}					
				}
				
				if( bTime && pBuff->GetTotal() > 0 )
				{
					if( dwOddTime > pBuff->GetTotal() )
						g_toolTip.CancelToolTip();
					else
						g_toolTip.PutToolTip( dwSkillID, strEdit, rectHittest, ptMouse, 1 );
				}
				else
					g_toolTip.PutToolTip( dwSkillID, strEdit, rectHittest, ptMouse, 1 );
				
				Count++;
				if( (Count % 8) == 0 )
				{
					Lpoint.x = ( GetClientRect().Width() - 200 ) / 2 - 20;
					Lpoint.y += nIconSize;
				}
			}
		}
	}	
}


void CWndWorld::RenderWantedArrow()
{
	CWorld* pWorld = g_WorldMng();
	if( !g_pPlayer || !pWorld ) 
		return; // 플레이어가 없으면 렌더 안한다

	if( !m_bRenderArrowWanted )
		return;

	if( g_tmCurrent < m_dwRenderArrowTime + MIN(60) )
	{
		D3DXVECTOR3 vSrc = g_pPlayer->GetPos();
		D3DXVECTOR3 vDest = m_v3Dest;

		LPDIRECT3DDEVICE9 pd3dDevice = m_pApp->m_pd3dDevice;

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );

		// 화살표의 위치, 회전값을 결정한다.
		matWorld = D3DXR::LookAtLH010(vDest, vSrc);
		D3DXMatrixInverse (&matWorld,NULL,&matWorld);
		matWorld._41 = g_pPlayer->GetPos().x; matWorld._42 = g_pPlayer->GetPos().y + 2.0f; matWorld._43 = g_pPlayer->GetPos().z;

		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		m_meshArrowWanted.SetBlendFactor( 128 );
		m_meshArrowWanted.Render( pd3dDevice, &matWorld );
	}
	else
	{
		m_bRenderArrowWanted = FALSE;
	}
}

void CWndWorld::RenderOptBuffTime(C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor )
{
	if(g_Option.m_bVisibleBuffTimeRender)
	{
		CString str;

		if( ct.GetDays() >= 1 )
		{
			str.Format( prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(ct.GetDays()) );		
			p2DRender->TextOut(  point.x+5, point.y+32, str, dwColor );
		}
		else
		if( ct.GetHours() >= 1 )
		{

			str.Format( prj.GetText(TID_PK_LIMIT_HOUR), ct.GetHours() );				
			p2DRender->TextOut(  point.x+5, point.y+32, str, dwColor );
		}
		else
		if( ct.GetMinutes() >= 1 )
		{
			str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), ct.GetMinutes() );	
			p2DRender->TextOut(  point.x+5, point.y+32, str, dwColor );
		}
		else
		{
			str.Format( prj.GetText(TID_PK_LIMIT_SECOND), ct.GetSeconds() );	
			p2DRender->TextOut(  point.x+3, point.y+32, str, dwColor );
		}
	}		
}

int CWndWorld::GetBuffTimeGap()
{
	if(g_Option.m_bVisibleBuffTimeRender)
		return 40+5;
	else
		return 32+5;
}

void CWndWorld::InviteParty(const u_long uidPlayer) {
	if (g_pPlayer->m_nDuel == 2) {
		// 극단 듀얼중 초청불갑니다~
		g_WndMng.PutString(TID_GAME_PPVP_ADDPARTY);
		return;
	}

	const bool canInvite = g_Party.m_aMember[0].m_uPlayerId == 0
		|| g_Party.IsLeader(g_pPlayer->m_idPlayer);

	if (!canInvite) {
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0007)));
		return;
	}

	if (g_Party.m_nSizeofMember + 1 > MAX_PTMEMBER_SIZE_SPECIAL) {
		const char * szMessageBuf = prj.GetText(TID_GAME_FULLPARTY3);
		g_WndMng.PutString(szMessageBuf, NULL, prj.GetTextColor(TID_GAME_FULLPARTY1));
		return;
	}

	g_DPlay.SendPacket<PACKETTYPE_MEMBERREQUEST, u_long>(uidPlayer);
}

void CWndWorld::InviteCompany( OBJID objId )
{
	CGuild* pGuild	= g_pPlayer->GetGuild();
	if( pGuild )
	{
		if( g_GuildCombatMng.m_bRequest && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE )
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_INVITATION_GUILD) ); //길드대전에 신청한 길드는 대전중에는 길드원 초대를 할 수 없습니다.
		else if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_CLOSE )
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_NOTINVITEGUILD) );
		else
			g_DPlay.SendGuildInvite( objId );
	}
}

CAdvMgr::CAdvMgr() {
	m_nIndex = 0;
	m_pParentWnd = NULL;
	m_vecButton.reserve(MAX_ADVBUTTON);
}

void CAdvMgr::Init(CWndBase * pParentWnd) {
	m_pParentWnd = pParentWnd;
}

// 도움말 버튼을 추가한다.
void CAdvMgr::AddAdvButton(const DWORD dwid) {
	if (!m_pParentWnd) return;

	// 버튼이 실행할 창이 이미 띄워져 있으면 버튼추가를 안한다.
	if (g_WndMng.GetWndBase(dwid)) return;

	// 기존 같은 버튼기능의 버튼이 존재하면 그냥 리턴
	const bool alreadyHasButton = std::any_of(
		m_vecButton.begin(), m_vecButton.end(),
		[dwid](const BUTTON_INFO & vecButton) {
			return vecButton.m_dwRunWindow == dwid;
		});
	if (alreadyHasButton) return;

	if (m_vecButton.size() >= MAX_ADVBUTTON) {
		Error("CAdvMgr::AddAdvButton reached� MAX_ADVBUTTON : %d", m_nIndex);
		return;
	}

	BUTTON_INFO & button = m_vecButton.emplace_back();

	button.m_pwndButton = std::make_unique<CWndButton>();
	button.m_pwndButton->Create("", 0, CRect(CPoint(0, 0), CSize(25, 25)), m_pParentWnd, m_nIndex + 2000);
	button.m_pwndButton->SetTexture(m_pParentWnd->m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtAdvPlus.bmp")), TRUE);

	button.m_dwRunWindow = dwid;
	m_nIndex++;		

	// 버튼을 추가한후 소팅을 한다...
	MoveButtons();
}

// 해당버튼을 눌렀을경우 연결된 창을 화면에 띄운후 버튼은 삭제된다.
bool CAdvMgr::RunButton( DWORD dwID )
{
	const auto it = std::ranges::find_if(m_vecButton,
		[&](const BUTTON_INFO & vecButton) {
			return vecButton.m_pwndButton->m_nIdWnd == dwID;
		});

	if (it == m_vecButton.end()) return false;

	const DWORD windowId = it->m_dwRunWindow;

	it->m_pwndButton->DeleteDeviceObjects();
	it->m_pwndButton->Destroy();
	m_vecButton.erase(it);

	g_WndMng.OpenApplet(windowId);

	MoveButtons();
	return true;
}

// 버튼 위치를 소팅한다...기준은 화면 아래로 부터 위로 정렬
void CAdvMgr::MoveButtons() {
	int y = 40 + 90;

	for (const BUTTON_INFO & buttonInfo : m_vecButton) {
		buttonInfo.m_pwndButton->Move(10, y);
		y += 40;
	}
}

#define PARTSMESH_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.o3d" ) : _T( "Part_femaleHead%02d.o3d" ) )

void CWndWorld::InitEyeFlash()
{
	CObject3D* pObject3D;
	GMOBJECT*  pGmObj;
	CString str1;
	CString strTexture;

	MATERIAL	*pMtrl;
	D3DMATERIAL9	mMtrl;
	
	TCHAR lpszTemp[64];
	for( int nSex = 0; nSex < 2; nSex++ )
	{
		for( int i=0; i<MAX_HEAD; i++ )
		{
			_stprintf( lpszTemp, PARTSMESH_HEAD( nSex ), i + 1 );
			pObject3D = g_Object3DMng.LoadObject3D( g_Neuz.m_pd3dDevice, lpszTemp );
			pGmObj = pObject3D->GetGMOBJECT();
			str1 = pGmObj->m_MaterialAry[0].strBitMapFileName;
			CMover::m_pTextureEye[nSex][i] = *(pGmObj->m_pMtrlBlkTexture);
			strTexture = str1.Mid( 0, strlen(pGmObj->m_MaterialAry[0].strBitMapFileName) - 4 );	
			strTexture += "_Flash";
			strTexture += str1.Right(4);

			pMtrl = g_TextureMng.AddMaterial( g_Neuz.m_pd3dDevice, &mMtrl, strTexture );
			CMover::m_pTextureEyeFlash[nSex][i] = pMtrl->m_pTexture;
		}
	}
}

void CWndWorld::ShowCCtrlMenu( CCtrl* pCCtrl )
{
	if( !pCCtrl )
	{
		assert( 0 );
		return;
	}

	CtrlProp* pProp = prj.GetCtrlProp( pCCtrl->GetIndex( ) );
	if( !pProp )
		return;

	if( !pProp->IsGuildHousingObj( ) ) //길드 하우징 가구일경우만!		//sun!!
		return;

	//gmpbigsun : 콘트롤에 대한 pop-up menu로 15차 길드하우스관련해서 추가댐 
	m_wndMenuMover.DeleteAllMenu();
	ReleaseCapture();

	OBJID objId = pCCtrl->GetId();
	GH_Fntr_Info* pInfo = GuildHouse->Find( objId );
	if( !pInfo )
		return;

	int slotIndex = GuildHouse->FindIndex( objId );
	if( slotIndex < 0 )			//해당 아이디를 가진 녀석을 못찾음
		return;

	m_wndMenuMover.AddButton( MMI_GHOUSE_INFO, GETTEXT(TID_APP_INFOMATION) );
	m_wndMenuMover.AddButton( MMI_GHOUSE_REINSTALL, GETTEXT(TID_MMI_GHOUSE_REINSTALL) );
	m_wndMenuMover.AddButton( MMI_GHOUSE_RECALL, GETTEXT(TID_MMI_GHOUSE_RECALL) );

	CRect rectBound;
	GetBoundRect( pCCtrl, &rectBound );
	m_wndMenuMover.Move( CPoint( rectBound.right, rectBound.top ) );
	m_wndMenuMover.SetVisible( TRUE );
}


BOOL CWndWorld::MenuException( CPoint point )
{
	if( m_bViewMap )
		return FALSE;
	if( g_pPlayer == NULL || g_pPlayer->IsDie() )
		return FALSE;
	if( g_pPlayer->m_vtInfo.VendorIsVendor() )
		return FALSE;
#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
		return FALSE;
#endif // __S_SERVER_UNIFY
	if( g_WndMng.GetWndVendorBase() )
		return FALSE;
	if( g_pPlayer->m_dwMode & DONMOVE_MODE )	// 돈무브 모드면 암것도 못함.
		return FALSE;
	if( g_pPlayer->GetAdjParam( DST_CHRSTATE ) & CHS_SETSTONE)
		return FALSE;
#ifdef __EVE_MINIGAME
	if( g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_MINIGAME_DICE) )
		return FALSE;
#endif
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_KAWIBAWIBO) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_DICE) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_ARITHMETIC) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_STOPWATCH) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_TYPING) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_CARD) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_RR_MINIGAME_LADDER) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_SMELT_JEWEL) )
		return FALSE;
	if( GetBuffIconRect( II_SYS_SYS_SCR_RETURN, point ) )	// 귀환의 두루마리 아이콘을 클릭하면 더블클릭까지 검사
		return FALSE;
	if( GetBuffIconRect( II_SYS_SYS_SCR_PARTYSUMMON, point ) )	// 귀환의 두루마리 아이콘을 클릭하면 더블클릭까지 검사
		return FALSE;

	if(g_eLocal.GetState( EVE_PK ))
	{
		if( GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_RED, point) ||
			GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_GREEN, point) ||
			GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_BLUE, point) ||
			GetBuffIconRect(II_SYS_SYS_QUE_ANGEL_WHITE, point) )
		{
			//Create 100 % Angel Item.
			g_DPlay.SendPacket<PACKETTYPE_ANGELBUFF>();
			return FALSE;
		}
	}

	if( GetBuffIconRect(II_SYS_SYS_SCR_PET_FEED_POCKET, point) )
		return FALSE;
	if( g_WndMng.GetWndBase(APP_WEBBOX) || g_WndMng.GetWndBase(APP_WEBBOX2) )
		return FALSE;
	if(CDeployManager::GetInstance()->IsReady())
	{
		if(CDeployManager::GetInstance()->IsCollide())
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_HOUSING_INSTALL_FAIL01 ), NULL, prj.GetTextColor( TID_GAME_HOUSING_INSTALL_FAIL01 ) );
			return FALSE;
		}
		HOUSINGINFO *phousingInfo;
		phousingInfo = CDeployManager::GetInstance()->EndDeploy();
		g_DPlay.SendHousingReqSetupFurniture( *phousingInfo );
		return FALSE;
	}
	if( GuildDeploy()->IsReady( ) )
	{
		if( GuildDeploy()->IsCollide( ) )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_HOUSING_INSTALL_FAIL01 ), NULL, prj.GetTextColor( TID_GAME_HOUSING_INSTALL_FAIL01 ) );
			return FALSE;
		}
		HOUSING_ITEM* pItem = GuildDeploy()->EndDeploy();
		if( GUILDHOUSE_PCKTTYPE_SETUP == GuildHouse->GetMode( ) )
			GuildHouse->Setup( *pItem );
		else if( GUILDHOUSE_PCKTTYPE_RESET == GuildHouse->GetMode( ) )
			GuildHouse->Reset( *pItem );
		return FALSE;
	}
	CObj* pSelectObj = CObj::m_pObjHighlight;	// 현재 커서 대고 있는 오브젝트.
	if( pSelectObj )
	{
		if( pSelectObj->GetType() == OT_MOVER )
		{
			if( g_eLocal.GetState( EVE_SCHOOL ) )
			{
				if( ( (CMover*)pSelectObj )->IsPlayer() )
					return FALSE;
			}
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

namespace WndWorld {

void GuildCombatInfo::ClearGuildStatus() {
	GuildStatus.clear();
}

void GuildCombatInfo::OnPlayerList(CAr & ar) {
	m_gc_defenders.clear();
	m_gc_warstates.clear();

	int nSizeGuild; ar >> nSizeGuild;
	for (int i = 0; i < nSizeGuild; ++i) {
		u_long uidDefender; ar >> uidDefender;		// 디펜더

		m_gc_defenders.emplace(uidDefender);

		int nSizeMember;    ar >> nSizeMember;
		for (int j = 0; j < nSizeMember; ++j) {
			u_long uidPlayer; ar >> uidPlayer;
			int nStatus;      ar >> nStatus;		// 전쟁 상태 == 1 ; 대기자 == 0

			m_gc_warstates.emplace(uidPlayer, nStatus);
		}
	}
}

void GuildCombatInfo::ClearPlayerList() {
	m_gc_defenders.clear();
	m_gc_warstates.clear();
}

bool GuildCombatInfo::IsGCStatusDefender(u_long uidDefender) const {
	return m_gc_defenders.contains(uidDefender);
}

int GuildCombatInfo::IsGCStatusPlayerWar(u_long uidPlayer) const {
	// 아무것도 없을시..-2 리턴
	if (m_gc_warstates.empty())
		return -2;

	const auto itWarState = m_gc_warstates.find(uidPlayer);
	if (itWarState == m_gc_warstates.end()) return -1;

	return itWarState->second;
}

void GuildCombatInfo::RenderMyGuildStatus(C2DRender * p2DRender) {
	static constexpr int nGap = 16;

	const int nState = IsGCStatusPlayerWar(g_pPlayer->m_idPlayer);

	if (nState == -1 || nState == -2) return;			
	
	CPoint cPoint(10, 150);
		
	{
		CRect  crBoard{
			CPoint(cPoint.x - 5, cPoint.y - 30),
			CSize( 5 + 130     , (GuildStatus.size() + 2) * 16)
		};
		p2DRender->RenderFillRect(crBoard, D3DCOLOR_ARGB(30, 0, 0, 0));
	}

	LPCTSTR szGuildName = g_pPlayer->GetGuild()->m_szGuild;
	p2DRender->TextOut(cPoint.x, cPoint.y - (nGap + 5), szGuildName, 0xFFEBAD18, 0xFF000000);

	char szBuf[MAX_NAME] = { 0, };
	CString strFormat;

	bool bJoinMessage = false;
	bool bSpace       = false;
	
	int nRate = 0;
	for (const WndWorld::GuildCombatInfo::GUILDRATE & GuildRate : GuildStatus) {
		nRate++;

		LPCTSTR str	= CPlayerDataCenter::GetInstance()->GetPlayerString( GuildRate.m_uidPlayer );
		
		ComputeShortenName(szBuf, str, 5);

		DWORD dwFontColor;
		if (GuildRate.m_uidPlayer == g_pPlayer->m_idPlayer) {
			dwFontColor = 0xFF9ED3FF;

			// 주인공이고 들어갈 차례이면서 라이프가 남아 있으면 메세지 출력
			if( GuildRate.bJoinReady && GuildRate.nLife > 0 )
				bJoinMessage = true;
		} else {
			dwFontColor = 0xFFFFFF99;
		}
				
		// 다음차례에 출전할 경우라면 메세지를 띄워준다.
		bool bJoinReady = false;
		if (GuildRate.bJoinReady && GuildRate.nLife > 0) {
			cPoint.y += nGap;
			bJoinReady = true;
		}
				
		// 부활기회가 없는 사람들은 이름을 회색으로 표시한다.
		if (GuildRate.nLife <= 0 && !bSpace) {
			cPoint.y += nGap;
			bSpace = true;
		}

		if (bJoinReady) {
			dwFontColor = 0xFF00CB00;
		} else if (bSpace) {
			dwFontColor = 0xFFCBCBCB;
		}

		if (IsGCStatusDefender(GuildRate.m_uidPlayer)) {
			p2DRender->TextOut(cPoint.x - 6, cPoint.y, "D", dwFontColor, 0xFF000000);
		}

		strFormat.Format("%2d", nRate);
		p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwFontColor, 0xFF000000);
		
		p2DRender->TextOut(cPoint.x + 25, cPoint.y, szBuf, dwFontColor, 0xFF000000);
				
		const int nLeftTemp = std::max(GuildRate.nLife - 1, 0);
				
		strFormat.Format( "%d", nLeftTemp );
		p2DRender->TextOut(cPoint.x + 110, cPoint.y, strFormat, dwFontColor, 0xFF000000);

		// HP바를 그린다.
		CMover * pObjMember = prj.GetUserByID( GuildRate.m_uidPlayer );
		const FLOAT fPersent	= ( IsValidObj( pObjMember ) ? (FLOAT)pObjMember->GetHitPoint() / (FLOAT)pObjMember->GetMaxHitPoint() : 0 );
				
		static constexpr FLOAT fFullWidth = 60;
		const FLOAT fDrawHP = fFullWidth*fPersent;

		CRect cRectHP(cPoint + CPoint(140, 0), CSize(static_cast<int>(fFullWidth), 11));
		p2DRender->RenderFillRect( cRectHP, D3DCOLOR_ARGB( 100, 0, 0, 0)  );

		cRectHP.DeflateRect( 1, 1, static_cast<int>( (fFullWidth-fDrawHP)+1 ), 1 );
		p2DRender->RenderFillRect( cRectHP, D3DCOLOR_ARGB( 100, 0, 255, 0)  );
		////////////////////////////////////
				
		cPoint.y += nGap;			
	}
			
	if( bJoinMessage && g_GuildCombatMng.m_nState == CGuildCombat::WAR_STATE )
	{
		CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
		g_Neuz.m_2DRender.SetFont( CWndBase::m_Theme.m_pFontGuildCombatText );				

		LPCTSTR strFormat;
		strFormat    = prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_READY);
		int nCenter  = p2DRender->m_pFont->GetTextExtent( strFormat ).cx / 2;
		const int nY = p2DRender->m_pFont->GetTextExtent( strFormat ).cy;
		p2DRender->TextOut( (g_Option.m_nResWidth / 2 ) - nCenter, (g_Option.m_nResHeight / 2 )-150, strFormat, 0xFFFFFF00, 0xFF000000 );
				
		strFormat    = prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_READY2);
		nCenter      = p2DRender->m_pFont->GetTextExtent( strFormat ).cx / 2;
		p2DRender->TextOut( (g_Option.m_nResWidth / 2 ) - nCenter, ((g_Option.m_nResHeight / 2 )-148)+nY, strFormat, 0xFFFFFF00, 0xFF000000 );

		g_Neuz.m_2DRender.SetFont( pOldFont );		
	}
}

void GuildCombatPrecedence::Update(const CGuildCombat::__GCGETPOINT & getPoint) {
	static constexpr auto Update_ = [](
		std::vector<ParticipantWithPoint> & participants,
		u_long attackerId, int earnedPoints
	) {
		auto it = std::find_if(
			participants.begin(), participants.end(),
			[attackerId](const ParticipantWithPoint & p) { return p.id == attackerId; }
		);

		if (it != participants.end()) {
			it->points += earnedPoints;
		} else {
			participants.emplace_back(ParticipantWithPoint{ attackerId, earnedPoints });
			it = participants.end() - 1;
		}

		Sort(participants.begin(), it + 1);
	};

	Update_(guilds, getPoint.uidGuildAttack, getPoint.nPoint);
	Update_(players, getPoint.uidPlayerAttack, getPoint.nPoint);
}

void GuildCombatPrecedence::Clear() {
	players.clear();
	guilds.clear();
}

void GuildCombatPrecedence::Render(C2DRender * p2DRender, const CRect clientRect) const {
	int		nGap  = 16;
	int		nRate = 0;
	CString strFormat;
	CPoint  cPoint = CPoint(clientRect.Width() - 150, 200);
	char szBuf[MAX_NAME] = {0,};		
		
	// 길드순위 출력
	CRect crBoard;

	crBoard.left = cPoint.x - 10;
	crBoard.top  = cPoint.y - 10;
	crBoard.right = cPoint.x + 140;
	crBoard.bottom = crBoard.top + ((guilds.size()+3) * 16);
	p2DRender->RenderFillRect( crBoard, D3DCOLOR_ARGB( 30, 0, 0, 0 ) );

	DWORD dwFontColor = 0xFF9ED3FF;
	int     nOldPoint = 0xffffffff;

	p2DRender->TextOut( cPoint.x+10, cPoint.y, prj.GetText(TID_GAME_GUILDCOMBAT_RATE), 0xFFEBAD18, 0xFF000000 );
	cPoint.y += ( nGap + (nGap / 2) );

	for (const auto & [guildId, nPoint] : guilds) {
		if( nOldPoint != nPoint )
			nRate++;


		if (!g_pPlayer->GetGuild()) {
			dwFontColor = 0xFFFFFF99;
		} else if (g_pPlayer->GetGuild()->m_idGuild == guildId) {
			dwFontColor = 0xFF9ED3FF;
		} else {
			dwFontColor = 0xFFFFFF99;
		}				

			memset( szBuf, 0, sizeof(CHAR)*MAX_NAME );

			LPCSTR str = GetGuildName(guildId);
			ComputeShortenName(szBuf, str, 5);

			if( nOldPoint != nPoint )
			{
				strFormat.Format( "%2d", nRate );
				p2DRender->TextOut( cPoint.x, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
			}
			else
			{					
				strFormat = "  ";
				p2DRender->TextOut( cPoint.x, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
			}

			p2DRender->TextOut( cPoint.x+25, cPoint.y, szBuf, dwFontColor, 0xFF000000 );
				
			strFormat.Format( "%d", nPoint );
			p2DRender->TextOut( cPoint.x+110, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
			
			

		cPoint.y += nGap;
		nOldPoint = nPoint;
	}

	// 개인순위 출력
	dwFontColor = 0xFFFFFF99;
	nOldPoint = 0xffffffff;
	nRate = 0;
	cPoint.y += 50;

	crBoard.left = cPoint.x - 10;
	crBoard.top  = cPoint.y - 10;
	crBoard.right = cPoint.x + 140;
	crBoard.bottom = crBoard.top + ((players.size()+3) * 16);
	p2DRender->RenderFillRect( crBoard, D3DCOLOR_ARGB( 30, 0, 0, 0 ) );
		
	BOOL bPlayerRender = FALSE;
	int  nPlayerRate   = 0;
	int  nPlayerPoint  = 0;
	static constexpr int nMaxRender = 10;
	int nMaxIndex = 0;
	p2DRender->TextOut( cPoint.x+10, cPoint.y, prj.GetText(TID_GAME_GUILDCOMBAT_PERSON_RATE), 0xFFEBAD18, 0xFF000000 );
	cPoint.y += ( nGap + (nGap / 2) );

	for (const auto & [uiPlayer, nPoint] : players) {
			
		if( nOldPoint != nPoint )
			nRate++;

		nMaxIndex++;		
			
		if( nMaxIndex > nMaxRender )
		{
			if( uiPlayer == g_pPlayer->m_idPlayer )
			{
				nPlayerPoint  = nPoint;
				nPlayerRate   = nRate;
				bPlayerRender = FALSE;
				break;
			}
			else
			{
				continue;
			}
		}
		else	
		if( uiPlayer == g_pPlayer->m_idPlayer )
		{
			bPlayerRender = TRUE;
		}				

		{
			LPCTSTR str	= CPlayerDataCenter::GetInstance()->GetPlayerString( uiPlayer );
			ComputeShortenName(szBuf, str, 5);

			if( uiPlayer == g_pPlayer->m_idPlayer )
			{
				dwFontColor = 0xFF9ED3FF;
			}
			else
			{
				dwFontColor = 0xFFFFFF99;
			}
				
			if( nOldPoint != nPoint )
			{
				strFormat.Format( "%2d", nRate );
				p2DRender->TextOut( cPoint.x, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
			}
			else
			{					
				strFormat = "  ";
				p2DRender->TextOut( cPoint.x, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
			}
				
			p2DRender->TextOut( cPoint.x+25, cPoint.y, szBuf, dwFontColor, 0xFF000000 );
				
			strFormat.Format( "%d", nPoint );
			p2DRender->TextOut( cPoint.x+110, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
		}

		cPoint.y += nGap;
		nOldPoint = nPoint;			
	}

	if( !players.empty() && bPlayerRender == FALSE )
	{
		cPoint.y += nGap;

		LPCTSTR str = g_pPlayer->GetName();
		ComputeShortenName(szBuf, str, 5);

		dwFontColor = 0xFF9ED3FF;
		strFormat.Format( "%2d", nPlayerRate );
		p2DRender->TextOut( cPoint.x, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
		p2DRender->TextOut( cPoint.x+25, cPoint.y, szBuf, dwFontColor, 0xFF000000 );
		strFormat.Format( "%d", nPlayerPoint );
		p2DRender->TextOut( cPoint.x+110, cPoint.y,strFormat, dwFontColor, 0xFF000000 );
	}
}

LPCTSTR GuildCombatPrecedence::GetGuildName(u_long guildId) const {
	const auto it = idToGuildName.find(guildId);
	if (it == idToGuildName.end()) return "???";
	return it->second.c_str();
}

void GuildCombatPrecedence::Sort(
	std::vector<ParticipantWithPoint>::iterator & participantsBegin,
	std::vector<ParticipantWithPoint>::iterator & participantsEnd
) {
	std::stable_sort(
		participantsBegin, participantsEnd,
		[](const ParticipantWithPoint & lhs, const ParticipantWithPoint & rhs) {
			return lhs.points > rhs.points;
		}
	);
}

}
