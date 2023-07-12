#include "stdafx.h"
#include "defineObj.h"
#include "AppDefine.h"
#include "WndChangeFace.h"
#include "defineText.h"
#ifdef __CLIENT
#include "dpclient.h"
#endif
#include "Vector3Helper.h"
#include "ItemMorph.h"


/****************************************************
  WndId : APP_CHANGEFACE - 얼굴변경
  CtrlId : WIDC_STATIC1 - 얼굴을 선택하세요. 총 2회 변경이 가능합니다.
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

CWndChangeFace::CWndChangeFace() 
{ 
	m_nSelectFace = 0;
} 
CWndChangeFace::~CWndChangeFace() 
{ 
} 
void CWndChangeFace::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nSelectFace >=0 && m_nSelectFace < 5 )
	{
		CRect rect( 0, 0, 86, 170 );
		rect.OffsetRect( 10, 5 );

		rect.OffsetRect( m_nSelectFace * 86, 0 );

		//point.x -= 10;
		//point.y -= 25;
		//point.x /= 86;

		p2DRender->RenderRect( rect, 0xff0000ff );	

		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0xff0000ff );
	}
} 
void CWndChangeFace::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeFace::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_CHANGEFACE, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndChangeFace::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = g_WndMng.GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndChangeFace::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeFace::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeFace::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	// 10,25 
	point.x -= 10;
	point.x /= 86;

	if( point.x >=0 && point.x < 5 && point.y >= 0 && point.y < 180 )
	{
		m_nSelectFace = point.x;
	}
} 
void CWndChangeFace::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeFace::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CString strCommand;
		strCommand.Format( "/ChangeFace %d", m_nSelectFace );
		g_DPlay.SendChat( strCommand );
		Destroy( TRUE );
	}
	else
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		nID = WTBID_CLOSE;
		Destroy( TRUE );
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


CWndChangeSex::CWndChangeSex() 
{ 
	m_nSelectFace = 0;
	m_pModel          = NULL;
//	m_dwItemId	= 0;
	m_dwItemId	= NULL_ID;
	m_dwObjId	= NULL_ID;
} 

void CWndChangeSex::OnDraw( C2DRender* p2DRender ) 
{ 
	if( g_pPlayer == NULL  )
		return;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
	
	D3DXMATRIX matView = D3DXR::LookAtLH010(vecPos, vecLookAt);
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	// 왼쪽 원본 모델 랜더링
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_STATIC1 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;//2;
		viewport.X     -= 6;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;//5;
		viewport.Width  = lpFace->rect.Width();//p2DRender->m_clipRect.Width();
		viewport.Height = lpFace->rect.Height();// - 10;//p2DRender->m_clipRect.Height();

		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
	    D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixScaling(&matScale, 4.5f, 4.5f, 4.5f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.0f,-5.6f,0.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-5.8f,0.0f);

		matWorld = matWorld * matScale * matTrans;
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
		D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
		SetLightVec( vDir );
		
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
				
		m_pModel->Render( &matWorld );
	}
} 
void CWndChangeSex::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	// 성전환이니깐 반대로 하자~
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_FEMALE:MI_MALE );
	m_pModel = prj.m_modelMng.LoadModel<std::unique_ptr<CModelObject>>( OT_MOVER, nMover, TRUE );
	m_pModel->LoadMotionId(MTI_STAND2);

	UpdateModelParts();
	
	m_pModel->InitDeviceObjects( );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndChangeSex::UpdateModelParts() {
	MoverSub::SkinMeshs skin = g_pPlayer->m_skin;
	skin.headMesh = m_nSelectFace;
	CMover::UpdateParts(!g_pPlayer->GetSex(), skin, g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory);
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeSex::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_CHANGESEX, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndChangeSex::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		if( m_dwItemId == NULL_ID )
		{
			CString strCommand;
			strCommand.Format( "/ChangeFace %d", m_nSelectFace );
			g_DPlay.SendChat( strCommand );
		}
		else
		{
			g_DPlay.SendDoUseItem( m_dwItemId, m_dwObjId, m_nSelectFace );
		}
		Destroy( TRUE );
	}
	else
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		nID = WTBID_CLOSE;
		Destroy( TRUE );
		return TRUE;
	}
	else
	if( nID == WIDC_LEFT )
	{
		m_nSelectFace = m_nSelectFace == 0 ? MAX_DEFAULT_HEAD - 1 : m_nSelectFace - 1;
		UpdateModelParts();
	}
	else
	if( nID == WIDC_RIGHT )
	{
		m_nSelectFace = (m_nSelectFace + 1) % MAX_DEFAULT_HEAD;
		UpdateModelParts();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_ITEM_TRANSY - 아이템 트랜지(ITM)
  CtrlId : WIDC_STATIC1 - 
  CtrlId : WIDC_STATIC2 - 아이템 트랜지(ITM)
  CtrlId : WIDC_STATIC3 - 아이템 : 
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

void CWndItemTransy::Init(CItemElem * pItemElem) {
	m_scroll = pItemElem;
	if (m_scroll) {
		m_scroll->SetExtra(1);
	}
}

void CWndItemTransy::OnDestroy( void ) {
	if (m_scroll) m_scroll->SetExtra(0);
}

void CWndItemTransy::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	const WNDCTRL * const pWndCtrl = GetWndCtrl(WIDC_STATIC4);
	m_itemDisplayer.Create(0, pWndCtrl->rect, this, 500);
	m_itemDisplayer.SetTooltipId(TID_GAME_TRANSITEM_PUT1);

	const WNDCTRL * const receiverCtrl = GetWndCtrl(WIDC_STATIC1);
	m_itemReceiver.Create(0, receiverCtrl->rect, this, WIDC_Receiver);
	m_itemReceiver.SetTooltipId(TID_GAME_TRANSITEM_PUT);
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndItemTransy::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_ITEM_TRANSY, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndItemTransy::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch (nID) {
		case WIDC_OK: {
			if (CItemElem * toTransform = m_itemReceiver.GetItem()) {

				const OBJID scrollPos = m_scroll ? m_scroll->m_dwObjId : NULL_ID;

				g_DPlay.SendItemTransy(
					toTransform->m_dwObjId,
					scrollPos,
					m_itemDisplayer.GetItemId(),
					m_scroll ? TRUE : FALSE
				);

				Destroy();
			}
		}
		break;
		case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
		case WIDC_Receiver: {
			if (message == WNM_ItemReceiver_Changed) {
				CItemElem * put = m_itemReceiver.GetItem();
				if (!put) {
					m_itemDisplayer.ResetItem();
				} else {
					CItemElem copy = *put;

					      ItemProp * const myItemProp = copy.GetProp();
					const ItemProp * const transyProp = myItemProp ? ItemMorph::GetTransyItem(*myItemProp) : nullptr;
					if (transyProp) {
						copy.m_dwItemId = transyProp->dwID;
						copy.m_nHitPoint = transyProp->dwEndurance;
					}

					m_itemDisplayer.SetItem(copy);
				}
			}
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

bool CWndItemTransy::CWndEquipementReceiver::CanReceiveItem(
	const CItemElem & itemElem, bool verbose
) {
	ItemProp * pItemProp = itemElem.GetProp();
	if (!pItemProp) return false;

	// == Transable?
	// TODO: why isn't CMover::GetTransyItem static?
	if (!g_pPlayer) return false;
	const ItemProp * pItemPropChange = ItemMorph::GetTransyItem(*pItemProp);

	if (!pItemPropChange) {
		if (verbose) g_WndMng.PutString(TID_GAME_ITEM_TRANSY);
		return false;
	}


	// == Right scroll?
	const CWndItemTransy * parent = dynamic_cast<CWndItemTransy *>(m_pParentWnd);
	if (!parent) {
		return false;
	}

	if (parent->m_scroll) {
		const CItemElem * const scroll = parent->m_scroll;
		const ItemProp * const scrollProp = scroll->GetProp();
		if (!scrollProp) return false;

		if (scrollProp->dwID == II_CHR_SYS_SCR_ITEMTRANSY_A) {
			if (pItemProp->dwLimitLevel1 >= 61) {
				if (verbose) g_WndMng.PutString(TID_GAME_ITEM_TRANSY_NOT_LEVEL_0, scrollProp->szName);
				return false;
			}
		} else {
			if (pItemProp->dwLimitLevel1 < 61) {
				if (verbose) g_WndMng.PutString(TID_GAME_ITEM_TRANSY_NOT_LEVEL_1, scrollProp->szName);
				return false;
			}
		}
	}

	return true;
}
