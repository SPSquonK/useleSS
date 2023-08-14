#include "stdafx.h"
#include <ranges>
#include "sqktd/util.hpp"
#include "defineObj.h"
#include "DefineItem.h"
#include "defineText.h"
#include "defineskill.h"
#include "defineSound.h"
#include "AppDefine.h"
#include "DialogMsg.h"
#include "WndField.h"
#include "Party.h"
#include "WndManager.h"
#include <afxdisp.h>
#include "defineNeuz.h"
#include "PlayerLineup.h"

#include "DPClient.h"
#include "MsgHdr.h"

#include "wndvendor.h"
#include "wndwebbox.h"
#include "WndBagEx.h"
#include "WndPost.h"
#include "WndSmelt.h"

#include "playerdata.h"

#include "GuildHouse.h"
#include "Vector3Helper.h"
#include "MsgHdr.h"

extern float g_fHairLight;

#include "eveschool.h"
#include "dpcertified.h"

#include <algorithm>

#ifdef __NEW_WEB_BOX
#include "WndHelperWebBox.h"
#endif // __NEW_WEB_BOX

//extern TCHAR g_szExpertName[ MAX_EXPERT ][ 32 ];
//extern TCHAR g_szJobName[ MAX_JOB ][ 32 ];
//extern const JOB prj.m_aJob[ MAX_JOB ];

#define WND_WIDTH 210
#define TASKBAR_HEIGHT 44

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;


#define MAX_GUILDCOMBAT_LIST_PER_PAGE 11
#define MAX_GUILDCOMBAT_LIST		  100

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ���÷� 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void WndFieldUtil::ResetRenderState() {
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	
}

D3DVIEWPORT9 WndFieldUtil::BuildViewport(C2DRender * const render, WNDCTRL * const control) {
	D3DVIEWPORT9 viewport;
	viewport.X = render->m_ptOrigin.x + control->rect.left;
	viewport.Y = render->m_ptOrigin.y + control->rect.top;
	viewport.Width = control->rect.Width();
	viewport.Height = control->rect.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	return viewport;
}

using namespace WndFieldUtil;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ӿ�����?
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CWndDropItem::OnDraw( C2DRender* p2DRender ) 
{ 
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi( szNumber );

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
	}
	else
	{
		if( m_pItemElem->m_nItemNum < nNumber )
		{
			char szNumberbuf[16] = {0, };
			nNumber = m_pItemElem->m_nItemNum;
			_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
			m_pEdit->SetString( szNumberbuf );
		}
		for( int i = 0 ; i < 8 ; i++ )
		{
			char szNumberbuf[8] = {0, };
			strncpy( szNumberbuf, szNumber, 8 );
			
			// 0 : ����, 48 : ���� 0, 57 : ���� 9
			if( 47 >= szNumberbuf[i] || szNumberbuf[i] >= 58 )
			{
				if( szNumberbuf[i] != 0 )
				{
					nNumber = m_pItemElem->m_nItemNum;
					_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
					m_pEdit->SetString( szNumberbuf );
					break;												
				}
			}
		}
	}
} 
void CWndDropItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	m_pEdit   = (CWndEdit  *)GetDlgItem( WIDC_EDIT1 );
	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_OK );	
	pWndOk->SetDefault();
	m_pEdit->SetFocus();

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
		//		m_pEdit->SetVisible( FALSE );
	}
	else
	{
		TCHAR szNumber[ 64 ];
		_itot( m_pItemElem->m_nItemNum, szNumber, 10 );
		m_pEdit->SetString( szNumber );
	}	

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndDropItem::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_DROP_ITEM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndDropItem::Process( void )
{
	if(m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}

BOOL CWndDropItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		short DropNum = 0;

		if( m_pItemElem->m_nItemNum >= 1 )
		{
			DropNum = atoi( m_pEdit->GetString() );
		}
		if( DropNum != 0 )
		{
			g_DPlay.SendDropItem( m_pItemElem->m_dwObjId, DropNum, m_vPos );
		}

		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����?��
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndDropConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_YES );	
	pWndOk->SetDefault();

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndDropConfirm::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_DROP_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndDropConfirm::Process( void )
{
	if(m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}

BOOL CWndDropConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES || message == EN_RETURN )
	{
		g_DPlay.SendDropItem( m_pItemElem->m_dwObjId, 1, m_vPos );
		Destroy();
	}
	else if( nID == WIDC_NO )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_QUEITMWARNING - ������ ���?
CtrlId : WIDC_CONTEXT - 
CtrlId : WIDC_BTN_YES - Button
CtrlId : WIDC_BTN_NO - Button
CtrlId : WIDC_WARNING - Picture
****************************************************/

void CWndQuestItemWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( _T( prj.GetText( TID_GAME_QUEITMWARNING ) ) );
		pWndEdit->EnableWindow( FALSE );	
	}
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_QUEITMWARNING, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
���� ������ ���� ���? 
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = g_WndMng.GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/

BOOL CWndQuestItemWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		if( m_pItemElem->m_nItemNum == 1 )	// ������ �ϳ����? �׳� ������.
		{
			g_WndMng.m_pWndDropConfirm = new CWndDropConfirm;
			g_WndMng.m_pWndDropConfirm->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropConfirm->m_vPos = m_vPos;
			g_WndMng.m_pWndDropConfirm->Initialize();
		}
		else
		{
			//SAFE_DELETE( g_WndMng.m_pWndDropItem );
			g_WndMng.m_pWndDropItem = new CWndDropItem;
			g_WndMng.m_pWndDropItem->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropItem->m_vPos = m_vPos;
			g_WndMng.m_pWndDropItem->Initialize();
		}
		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// �κ��丮 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "WndShop.h"

void CWndGold::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_GlobalShortcut.m_dwShortcut = ShortcutType::Item;
	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_pTexture = &m_texture; //.m_pFromWnd   = this;
	m_GlobalShortcut.m_dwData = 0;
}
void CWndGold::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndGold::OnInitialUpdate()
{
	m_texture.LoadTexture( MakePath( DIR_ITEM, "itm_GolGolSeed.dds" ), 0xffff00ff );
}

//////////////////////////////////////////////
CWndQueryEquip::CWndQueryEquip(CMover & mover, std::unique_ptr<std::array<CItemElem, MAX_HUMAN_PARTS>> aEquipInfoAdd) {
	m_InvenRect.fill(CRect());

	m_OldPos = CPoint(0, 0);

	// Set mover
	m_ObjID = mover.GetId();

	const int nMover = (mover.GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = prj.m_modelMng.LoadModel<std::unique_ptr<CModelObject>>(
			OT_MOVER, nMover, TRUE
		);
	m_pModel->LoadMotionId(MTI_STAND);
	CMover::UpdateParts(mover.GetSex(), mover.m_skin, mover.m_aEquipInfo, m_pModel.get(), NULL);
	m_pModel->InitDeviceObjects();

	// Set Equip Info add
	m_aEquipInfoAdd = std::move(aEquipInfoAdd);

	for (CItemElem & item : *m_aEquipInfoAdd) {
		if (!item.IsEmpty()) {
			item.SetTexture();
		}
	}
}

BOOL CWndQueryEquip::Process() {
	CMover * pMover = GetMover();

	if (IsInvalidObj(pMover)) {
		Destroy();
		return FALSE;
	}

	if (m_pModel)
		m_pModel->FrameMove();

	return TRUE;
}


void CWndQueryEquip::OnMouseWndSurface(CPoint point) {
	if (IsInvalidObj(GetMover())) return;

	for (int i = 2; i < MAX_HUMAN_PARTS; i++) {
		/* const */ CItemElem & itemElem = (*m_aEquipInfoAdd)[i];
		if (itemElem.IsEmpty()) continue;

		CRect DrawRect = m_InvenRect[i];
		CPoint point = GetMousePoint();

		if (DrawRect.PtInRect(point)) {
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&DrawRect);

			g_WndMng.PutToolTip_Item(&itemElem, point2, &DrawRect, APP_QUERYEQUIP);
		}
	}
}

void CWndQueryEquip::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = GetMover();
	
	if( IsInvalidObj(pMover) )
		return ;
	
	// ����Ʈ ���� 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 5;//100;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y + 5;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width() - 100;//105;
	viewport.Height = 182;//p2DRender->m_clipRect.Height() - 200;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//



	for (int i = 2; i < MAX_HUMAN_PARTS; i++) {
		CItemElem & itemElem = (*m_aEquipInfoAdd)[i];
		if (itemElem.IsEmpty()) continue;
		if (!itemElem.GetTexture()) continue;

		const ItemProp * const pItemProp = itemElem.GetProp();
		if (!pItemProp) continue;

		DWORD dwAlpha = 255;

		if( i == PARTS_LWEAPON )		// �޼չ��� �׸�Ÿ�̹��϶�
		{
			if( pItemProp->dwHanded == HD_TWO )	// ���ڵ� �����?
			{
				dwAlpha   = 100;
			}
		}
		
		CPoint drawPoint = m_InvenRect[i].TopLeft();

		FLOAT displayRatio = 1.0f;
		if (i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2) {
			displayRatio = 0.8f;
		} else if (i >= PARTS_HAT && i <= PARTS_BOOTS) {
			displayRatio = 0.9f;
		} else {
			drawPoint += CPoint(6, 6);
		}
		
		if (itemElem.IsFlag(CItemElem::expired)) {
			p2DRender->RenderTexture2(drawPoint, itemElem.GetTexture(), displayRatio, displayRatio, D3DCOLOR_XRGB(255, 100, 100));
		} else {
			p2DRender->RenderTexture(drawPoint, itemElem.GetTexture(), dwAlpha, displayRatio, displayRatio);
		}
	}

	if( m_pModel == NULL )
		return;

	ResetRenderState();

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// ����Ʈ ���� 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// ī�޶� 
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );

	D3DXMATRIX matView = D3DXR::LookAtLH010(vecPos, vecLookAt);
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
	#endif //__YENV
	
	// ���� ���� �� ������
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM20 );
		viewport = BuildViewport(p2DRender, lpFace);

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
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixRotationY( &matRot, D3DXToRadian( m_fRot ) );
		
		D3DXMatrixScaling(&matScale,1.2f,1.2f,1.2f);
		D3DXMatrixTranslation(&matTrans,0.0f,-0.6f,0.0f);
		
		matWorld = matWorld * matScale * matRot * matTrans;
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// ������ 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
	pMover->OverCoatItemRenderCheck(m_pModel.get());
		
	// �����? �Ӹ�ī�� �������ϴ°��̳�?  // �κ��� ���°��?
			DWORD dwId	= pMover->m_aEquipInfo[PARTS_CAP].dwId;
			O3D_ELEMENT*	pElement = NULL;
			ItemProp*		pItemProp = NULL;
			
			if( dwId != NULL_ID )
			{
				pItemProp	= prj.GetItemProp( dwId );
				
				if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
				{
					pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
				}
				
				// �����ǻ��� �Ծ������? �Ӹ��������ΰ��� ������ ���� ���ڸ� �������� �ٲ۴�
				dwId	= pMover->m_aEquipInfo[PARTS_HAT].dwId;
				if( dwId != NULL_ID )
				{
					if( !(pMover->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
							
							m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
						else
						{
							if( pElement )
								pElement->m_nEffect &= ~XE_HIDE;
						}
					}
				}
									
			}
			else
			{
				// �����ǻ��� �Ծ������? �Ӹ��������ΰ��� ������ ���� ���ڸ� �������� �ٲ۴�
				dwId	= pMover->m_aEquipInfo[PARTS_HAT].dwId;
				if( dwId != NULL_ID )
				{
					if( !(pMover->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{						
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );

							m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
					}
				}							
			}
			
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = pMover->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = pMover->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = pMover->m_fHairColorB;
			
		m_pModel->SetGroup( 0 );	
		m_pModel->Render( &matWorld );
	}

	return;
}
void CWndQueryEquip::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDownRot )
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}
	
	m_OldPos = point;
}
void CWndQueryEquip::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();
	
}
void CWndQueryEquip::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
	if( lpWndCtrl->rect.PtInRect( point ) )
	{
		m_OldPos          = point;
		m_bLButtonDownRot = TRUE;
	}
}	
	
void CWndQueryEquip::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;
	
	CWndInventory::InitializeInvenRect(m_InvenRect, *this);
	
	MoveParentCenter();
}

BOOL CWndQueryEquip::Initialize(CWndBase * pWndParent) {
	m_InvenRect.fill(CRect());
	return CWndNeuz::InitDialog(APP_QUERYEQUIP, pWndParent, 0, CPoint(792, 130));
}






//////////////////////////////////////////////
CWndInventory::CWndInventory()
{
	m_bReport = FALSE;
	m_pSelectItem = NULL;
	m_pWndConfirmBuy = NULL;

	m_bIsUpgradeMode = FALSE;
	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_pSfxUpgrade = NULL;
	m_dwEnchantWaitTime = 0xffffffff;
	m_TexRemoveItem = NULL;

	m_pModel = NULL;
	m_OldPos = CPoint(0,0);
	
	m_InvenRect.fill(CRect());
	m_pWndRemoveJewelConfirm = NULL;
	m_bRemoveJewel = FALSE;
}
CWndInventory::~CWndInventory()
{
	SAFE_DELETE( m_pWndConfirmBuy );

	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
	SAFE_DELETE(m_pWndRemoveJewelConfirm);
}

void CWndInventory::OnDestroy( void )
{
	m_pModel = nullptr;
	SAFE_DELETE( m_pWndConfirmBuy );	
	Windows::DestroyIfOpened(APP_SUMMON_ANGEL);

#ifdef __EVE_MINIGAME
	Windows::DestroyIfOpened(APP_MINIGAME_WORD, APP_MINIGAME_PUZZLE);
#endif //__EVE_MINIGAME

	Windows::DestroyIfOpened(APP_SMELT_MIXJEWEL, APP_SMELT_EXTRACTION);

#ifdef __WINDOW_INTERFACE_BUG
	Windows::DestroyIfOpened(
		APP_PIERCING, APP_REMOVE_ATTRIBUTE,
		APP_SMELT_REMOVE_PIERCING_EX, APP_SMELT_REMOVE_JEWEL,
		APP_LVREQDOWN, APP_CANCEL_BLESSING, APP_TEST
		);
#endif // __WINDOW_INTERFACE_BUG

	Windows::DestroyIfOpened(
		APP_SMELT_SAFETY, APP_SMELT_SAFETY_CONFIRM,
		APP_EQUIP_BIND_CONFIRM, APP_RESTATE_CONFIRM, APP_PET_FOODMILL,
		APP_SHOP_, APP_COMMON_BANK
	);
}
void CWndInventory::OnMouseWndSurface( CPoint point )
{
	int nTemp = 0;
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		CItemElem * pItemBase = g_pPlayer->GetEquipItem( i );
		
		CRect DrawRect = m_InvenRect[i];
		
		CPoint point = GetMousePoint();
		// ����
		if( DrawRect.PtInRect( point ) )
		{
			nTemp = i;
			
			if( i == PARTS_LWEAPON )
			{
				pItemBase = g_pPlayer->GetEquipItem( PARTS_SHIELD );
				
				if( pItemBase )
				{
					nTemp = PARTS_SHIELD;
				}
				else
				{
					pItemBase = g_pPlayer->GetEquipItem( PARTS_LWEAPON );
					
					if( pItemBase == NULL )
						pItemBase = g_pPlayer->GetEquipItem( PARTS_RWEAPON );
				}
			}			

			if( pItemBase )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &DrawRect );
				
				// ����?�� �ִ°� ����
				g_toolTip.SetSubToolTipNumber( 0 );
				g_WndMng.PutToolTip_Item( pItemBase, point2, &DrawRect, APP_INVENTORY );
				break;
			}
			else
			{
				// ����?�� ���°� ����
				
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &DrawRect );
				
				LPCTSTR strText = "";

				switch( nTemp )
				{
					case PARTS_UPPER_BODY:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_SUIT);
						break;
					case PARTS_HAND:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_GAUNTLET);
						break;
					case PARTS_FOOT:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_BOOTS );
						break;
					case PARTS_CAP:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_HELMET );
						break;
					case PARTS_CLOAK:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLO);
						break;
					case PARTS_LWEAPON:
					case PARTS_RWEAPON:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_WEAPON);
						break;
					case PARTS_SHIELD:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_GUARD  );
						break;
					case PARTS_MASK:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_MASK );
						break;
					case PARTS_RIDE:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_RID );
						break;
					case PARTS_NECKLACE1:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_NECKLACE  );
						break;
					case PARTS_RING1:
					case PARTS_RING2:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_RING   );
						break;
					case PARTS_EARRING1:
					case PARTS_EARRING2:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_EARRING   );
						break;
					case PARTS_BULLET:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_BULLET    );
						break;							
					case PARTS_HAT:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_01     );
						break;	
					case PARTS_CLOTH:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_02     );
						break;	
					case PARTS_GLOVE:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_03     );
						break;	
					case PARTS_BOOTS:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_04     );
						break;							
				}

				g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );
				break;
			}
//			break;
		}
	}
}
void CWndInventory::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = g_pPlayer;
	if( !pMover )
		return;

	// ����Ʈ ���� 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y + 5;
	viewport.Width  = p2DRender->m_clipRect.Width() - 100;
	viewport.Height = 182;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	CString strGoldNum;
	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
	CWndTrade * pWndTrade = (CWndTrade *)g_WndMng.GetWndBase( APP_TRADE );
	strGoldNum.Format( _T( "%d" ), g_pPlayer->GetGold() );

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM21 );
	if( lpWndCtrl1 && m_TexRemoveItem )
	{
		CPoint point = GetMousePoint();
		float fScal = 1.0f;
		if( lpWndCtrl1->rect.PtInRect( point ) )
		{
			fScal = 1.2f;
			CPoint point2 = point;
			CRect DrawRect = lpWndCtrl1->rect;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			g_toolTip.PutToolTip( 100, prj.GetText(TID_GAME_INVEONTORY_REMOVE_TOOL), DrawRect, point2, 0 );
		}
		if(m_TexRemoveItem)
			m_TexRemoveItem->Render( p2DRender, lpWndCtrl1->rect.TopLeft(), CPoint( 27, 27 ), 255, fScal, fScal );
	}

	pGoldNum->SetTitle( strGoldNum );

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		DWORD dwAlpha = 255;
		CItemElem * pItemBase = g_pPlayer->GetEquipItem( i );
		if( i == PARTS_LWEAPON )		// �޼չ��� �׸�Ÿ�̹��϶�
		{
			CItemElem * pRWeapon = g_pPlayer->GetEquipItem( PARTS_RWEAPON );		// ������ ���⸦ ��������
			if( pRWeapon && pRWeapon->GetProp()->dwHanded == HD_TWO )	// ���ڵ� �����?
			{
				pItemBase = pRWeapon;	// �����չ����? ������ �׸���.
				dwAlpha   = 100;
			}
		}

		FLOAT sx = 1.0f;
		FLOAT sy = 1.0f;

		CPoint cpAdd = CPoint(6,6);
		CRect DrawRect = m_InvenRect[i];

		if( i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2 )
		{
			cpAdd = CPoint(0,0);

			sx = 0.8f;
			sy = 0.8f;
		}
		else
		if( i >= PARTS_HAT && i <= PARTS_BOOTS )
		{
			cpAdd = CPoint(0,0);
			
			sx = 0.9f;
			sy = 0.9f;
		}
		
		
		if( pItemBase && pItemBase->GetTexture() )
		{
			if( pItemBase->IsFlag( CItemElem::expired ) )
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_XRGB( 255, 100, 100 ), sx, sy );
			}
			else
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_ARGB( dwAlpha, 255, 255, 255 ), sx, sy );
			}

			if(pItemBase->GetProp()->dwPackMax > 1 )		// ���� �������̳�?
			{
				short nItemNum	= pItemBase->m_nItemNum;

				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				int x = DrawRect.left;	
				int y = DrawRect.top;
				m_p2DRender->TextOut( x + 42 - size.cx, y + 42 - size.cy, szTemp, 0xff0000ff );
				m_p2DRender->TextOut( x + 41 - size.cx, y + 41 - size.cy, szTemp, 0xffb0b0f0 );
			}
		} 
	}

	if( g_pPlayer == NULL || m_pModel == NULL )
		return;

	ResetRenderState();

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// ����Ʈ ���� 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// ī�޶� 
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
	
	D3DXMATRIX matView = D3DXR::LookAtLH010(vecPos, vecLookAt);
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
	#endif //__YENV
	
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM20 );
		viewport = BuildViewport(p2DRender, lpFace);

		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
/*		
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/		
		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
	    D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixRotationY( &matRot, D3DXToRadian( m_fRot ) );
		
		D3DXMatrixScaling(&matScale,1.2f,1.2f,1.2f);
		D3DXMatrixTranslation(&matTrans,0.0f,-0.6f,0.0f);
		
		matWorld = matWorld * matScale * matRot * matTrans;
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// ������ 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(m_pModel.get());
		
		// �����? �Ӹ�ī�� �������ϴ°��̳�?  // �κ��� �ִ� ���? 
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );
		if( pItemElem )
		{
			O3D_ELEMENT* pElement = NULL;
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			// �����ǻ��� �Ծ������? �Ӹ��������ΰ��� ������ ���� ���ڸ� �������� �ٲ۴�
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( CItemElem::expired )) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
				else
				{
					//m_pModel->SetEffectOff(PARTS_HAIR, XE_HIDE );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					
					m_pModel->SetEffectOff(PARTS_HEAD, XE_HIDE );
				}
			}						
		}
		else
		{
			// �����ǻ��� �Ծ������? �Ӹ��������ΰ��� ������ ���� ���ڸ� �������� �ٲ۴�
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( CItemElem::expired )) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		}
		
		m_pModel->Render( &matWorld );
	}

	return;
}

void CWndInventory::UpDateModel()
{
	const int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = prj.m_modelMng.LoadModel<std::unique_ptr<CModelObject>>( OT_MOVER, nMover, TRUE );
	m_pModel->LoadMotionId(MTI_STAND);
	UpdateParts();
	m_pModel->InitDeviceObjects();	
}


void CWndInventory::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;


	InitializeInvenRect(m_InvenRect, *this);

	UpDateModel();

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );
	m_wndItemCtrl.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 11 );
	m_wndItemCtrl.InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );

	pTabCtrl->InsertItem(&m_wndItemCtrl, prj.GetText(TID_GAME_ITEM));

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_GOLD );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_GOLD );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );

	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
	pGoldNum->AddWndStyle(WSS_MONEY);

	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_bIsUpgradeMode = FALSE;
	m_dwEnchantWaitTime = 0xffffffff;

	m_TexRemoveItem = m_textureMng.AddTexture( MakePath( DIR_THEME, "WndInventoryGarbage.dds" ), 0xffff00ff );
	
	CRect rectRoot = g_WndMng.GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 112 + 48 );
	Move( point );
}
BOOL CWndInventory::Initialize(CWndBase * pWndParent) {
	m_InvenRect.fill(CRect());
	return CWndNeuz::InitDialog(APP_INVENTORY, pWndParent, 0, CPoint(792, 130));
}

BOOL CWndInventory::Process()
{
	if( m_pModel )
		m_pModel->FrameMove();
	
	ProcessEnchant();

	return TRUE;
}

void CWndInventory::ProcessEnchant() {
	if (m_dwEnchantWaitTime >= g_tmCurrent) return;

	m_dwEnchantWaitTime = 0xffffffff;

	if (m_pSfxUpgrade) {
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = nullptr;
	}

	CItemElem * pItemElem = m_pUpgradeItem;
	if (!pItemElem) return;

	CItemElem * pItemMaterialElem = m_pUpgradeMaterialItem;
	if (!m_pUpgradeMaterialItem) return;

	const ItemProp * pItemMaterialProp = pItemMaterialElem->GetProp();
	if (!pItemMaterialProp) return;

	// --- Enchant
	if (sqktd::is_among(pItemMaterialProp->dwItemKind3, IK3_ELECARD, IK3_ENCHANT, IK3_PIERDICE)) {
		
		if (pItemMaterialElem->m_dwItemId == II_GEN_MAT_ORICHALCUM02) {
			g_DPlay.SendUltimateEnchantWeapon(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
		} else if (pItemMaterialElem->m_dwItemId == II_GEN_MAT_MOONSTONE || pItemMaterialElem->m_dwItemId == II_GEN_MAT_MOONSTONE_1) {
			if (pItemElem->IsCollector(TRUE) || pItemElem->GetProp()->dwItemKind2 == IK2_JEWELRY) {
				g_DPlay.SendEnchant(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
			}
		} else {
			g_DPlay.SendEnchant(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
		}

		return;
	}

	// --- Socket card
	if (sqktd::is_among(pItemMaterialProp->dwItemKind3, IK3_SOCKETCARD, IK3_SOCKETCARD2)) {
		g_DPlay.SendPacket<PACKETTYPE_PIERCING, DWORD, DWORD>(
			pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId
		);

		return;
	}

	// --- Need Target
	if (pItemMaterialProp->IsNeedTarget()) {
		g_DPlay.SendDoUseItemTarget(m_pUpgradeMaterialItem->m_dwObjId, pItemElem->m_dwObjId);
		
		return;
	}

	// --- Random scroll
	if (pItemMaterialProp->dwItemKind3 == IK3_RANDOM_SCROLL) {
		SAFE_DELETE(g_WndMng.m_pWndRandomScrollConfirm);
		g_WndMng.m_pWndRandomScrollConfirm = new CWndRandomScrollConfirm;
		if (pItemElem->GetRandomOpt() > 0)
			g_WndMng.m_pWndRandomScrollConfirm->SetItem(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId, TRUE);
		else
			g_WndMng.m_pWndRandomScrollConfirm->SetItem(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
		g_WndMng.m_pWndRandomScrollConfirm->Initialize();

		return;
	}
}

BOOL CWndInventory::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	// �κ��丮�� �������� Drop�Ǿ���. ���? �Դ��� Ȯ������ ������ �����϶�.
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		if( lpShortcut->m_pFromWnd == NULL )
		{
			return CWndNeuz::OnChildNotify( message, nID, pLResult );
		}
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

		if( pWndFrame == NULL )
		{
			Error( "CWndInventory::OnChildNotify : pWndFrame==NULL" );
		}
		BOOL bForbid = TRUE;
		if( pWndFrame && nID == 11 ) // item
		{
			if( pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwData == 0 )
			{
				SetForbid( TRUE );
				return FALSE;
			}
			
			// ������ Ż���� �̷����? ���̴�.
			if( pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwIndex != II_GOLD_SEED1 )
			{

				int nPart = -1;
				if( m_pSelectItem )
					nPart = m_pSelectItem->m_dwObjIndex - MAX_INVENTORY;

				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, lpShortcut->m_dwId ), 0, nPart );
				bForbid = FALSE;
			}
			else
			// �󿡼� �°Ŷ��? ���� 
			
			if( pWndFrame->GetWndId() == APP_SHOP_ )
			{
				CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
				for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
				{
					int nItem = pWndItemCtrl->GetSelectedItem( i );
					pWndItemCtrl->GetItem( nItem );
				}
				CWndShop* pwndShop = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
				if(pwndShop)
				{
					LPCHARACTER lpCharacter = pwndShop->m_pMover->GetCharacter();
					if(lpCharacter->m_vendor.m_type == CVendor::Type::RedChip)
					{
						if(g_pPlayer->m_Inventory.GetAtItemNum( II_CHP_RED ) - (int)((CItemElem*)lpShortcut->m_dwData)->GetChipCost() >= 0) //�����Ϸ��� ǰ���� Ĩ���� �̻��� ������ �ִ��� Ȯ��.
						{
							SAFE_DELETE( m_pWndConfirmBuy );
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->m_nBuyType = 1;
							m_pWndConfirmBuy->Initialize( this );
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_CANNT_BY_REDCHIP) ) );
					}
					else if(lpCharacter->m_vendor.m_type == CVendor::Type::Penya)
					{
						if( g_pPlayer->GetGold() - ( ( ( CItemElem* )lpShortcut->m_dwData )->GetCost() * prj.m_fShopBuyRate ) >= 0 )
						{
							SAFE_DELETE( m_pWndConfirmBuy );
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->Initialize( this );
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0048) ) );
					}
				}
			}
			else if( pWndFrame->GetWndId() == APP_VENDOR_REVISION )
			{
				CWndVendor* pWndVendor	= (CWndVendor*)pWndFrame;
				if( pWndVendor->m_pVendor == g_pPlayer )
				{
					g_DPlay.SendUnregisterPVendorItem( (BYTE)( lpShortcut->m_dwIndex ) );
				}
				else
				{
					SAFE_DELETE( pWndVendor->m_pWndVendorBuy );
					pWndVendor->m_pWndVendorBuy		= new CWndVendorBuy( (CItemElem *)lpShortcut->m_dwData, lpShortcut->m_dwIndex/*nItem*/ );
					pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor );
				}
				bForbid		= FALSE;
			}
			else
			// ��ũ���� �°Ŷ��? ����
			if( pWndFrame->GetWndId() == APP_COMMON_BANK )
			{
				CWndBase* pWndTaget = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				if( lpShortcut->m_dwData != 0 )
				{
					nSlot = g_WndMng.m_pWndBank->GetPosOfItemCtrl(pWndTaget);
					
					CWndTradeGold::Create<SHORTCUT::Source::Bank>(
						{ nSlot, lpShortcut->m_dwId },
						[](auto source, int quantity) {
							g_DPlay.SendGetItemBank(source.slot, source.itemPos, quantity);
						}
					);
				} else {
					nSlot = g_WndMng.m_pWndBank->GetPosOfGold(pWndTaget);

					CWndTradeGold::Create<SHORTCUT::Source::BankPenya>(
						{ nSlot },
						[](auto source, int quantity) {
							g_DPlay.SendGetGoldBank(source.slot, quantity);
						}
					);
				}
				bForbid = FALSE;
			}
			// �޴밡�濡�� �� ���?
			if( pWndFrame->GetWndId() == APP_BAG_EX )
			{
				
				CWndBase* pWndFrom = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndFrom->m_pParentWnd->GetWndId() == WIDC_BASIC )
					{					
						nSlot = 0;
					}
					else if( pWndFrom->m_pParentWnd->GetWndId() == WIDC_EXBAG1 )
					{
						nSlot = 1;
						if(!g_pPlayer->m_Pocket.IsAvailable(1)) return FALSE;
					}
					else
					{
						nSlot = 2;
						if(!g_pPlayer->m_Pocket.IsAvailable(2)) return FALSE;
					}
					
					CWndTradeGold::Create<SHORTCUT::Source::Bag>(
						{ nSlot, lpShortcut->m_dwId },
						[](auto source, int quantity) {
							g_DPlay.SendMoveItem_Pocket(source.bagId, source.itemPos, quantity, -1);
						}
					);
				}
				
				bForbid = FALSE;
			}
			else
			if( pWndFrame->GetWndId() == APP_POST_READ )
			{
				CWndPostRead* pWndPostRead = (CWndPostRead*)pWndFrame;
				
				if( pWndPostRead )
				{
					if( lpShortcut->m_dwData != 0 )
					{
						pWndPostRead->MailReceiveItem();
					}
					else
					{
						pWndPostRead->MailReceiveGold();
					}
					
					bForbid = FALSE;
				}
			}
			else	
			if( pWndFrame->GetWndId() == APP_GUILD_BANK )
			{
				if( lpShortcut->m_dwData != 0 )
				{
					CWndTradeGold::Create<SHORTCUT::Source::Guild>(
						{ lpShortcut->m_dwId },
						[](auto source, int quantity) {
							g_DPlay.SendGetItemGuildBank(source.itemPos, quantity, 1);
						}
					);
				}
				else
				{
					CWndTradeGold::Create<SHORTCUT::Source::GuildMoney>(
						{},
						[](auto, int quantity) {
							g_DPlay.SendGetItemGuildBank(0, quantity, 0);
						}
					);
				}
				bForbid = FALSE;
			}
		}
		SetForbid( bForbid );
	}
	else
	// ���ϵ�κ���? ����Ŭ�� �˸� �޽����� �Դ�. ������ ��Ʈ�ѿ��� �������� ����ϰڴٴ�? �޽�����.
	if( message == WIN_DBLCLK )
	{
		if( nID == 11 ) // item
		{
			if( m_dwEnchantWaitTime != 0xffffffff || GetWndBase(APP_SMELT_SAFETY_CONFIRM) != NULL )
			{
				g_WndMng.PutString(TID_MMI_NOTUPGRADE);
				return 0;
			}

			if( GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL )
			{
				g_WndMng.PutString(TID_TOOLTIP_EQUIPBIND_ERROR01);
				return 0;
			}

			if( GetWndBase(APP_COMMITEM_DIALOG) != NULL )
			{
				g_WndMng.PutString(TID_TOOLTIP_ITEM_USING_ERROR);
				return 0;
			}
			
			CCtrl* pCtrl = (CCtrl*)g_WorldMng()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();
			CItemElem * pFocusItem = (CItemElem *) pLResult;
			BOOL	bAble = TRUE;
			if( pFocusItem )
			{
				ItemProp *pProp = pFocusItem->GetProp();
				// �Һ��������? 1�� �������� ����Ŭ���ϰ� ������ ���ٰ� 
				// �����Ǳ� ���� �� ����Ŭ���ϸ� pFocusItem->m_dwItemId�� 0�� �Ǹ鼭
				// ������Ƽ�� �εǼ� �״´�.
				if( pProp )
				{					
					if (CWndSummonAngel * pWndSummonAngel = g_WndMng.GetWndBase<CWndSummonAngel>(APP_SUMMON_ANGEL)) {
						const bool isRightMaterial = ItemProps::IsOrichalcum(*pProp) || ItemProps::IsMoonstone(*pProp);
						const bool andCanBeUsed = isRightMaterial && (pFocusItem->GetExtra() < pFocusItem->m_nItemNum);

						if (andCanBeUsed) {
							pWndSummonAngel->SetDieFromInventory(*pFocusItem);
							return TRUE;
						}
					}
#ifdef __EVE_MINIGAME
					if(g_WndMng.GetWndBase( APP_MINIGAME_WORD ))
					{
						if(pProp->dwID >= II_SYS_SYS_EVE_A_CARD && pProp->dwID <= II_SYS_SYS_EVE_Z_CARD)
						{
							if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
							{								
								CWndFindWordGame* pWndFindWordGame = (CWndFindWordGame*)g_WndMng.GetWndBase( APP_MINIGAME_WORD );
								pWndFindWordGame->SetWord(pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE ))
					{
						if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
						{			
							CWndPuzzleGame* pWndPuzzleGame = (CWndPuzzleGame*)g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE );
							pWndPuzzleGame->SetPicture(pFocusItem);
							return TRUE;
						}
					}
#endif //__EVE_MINIGAME
					if(g_WndMng.GetWndBase( APP_REMOVE_ATTRIBUTE ))
					{
						if( CItemElem::IsEleRefineryAble(pProp) )
						{
							if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
							{			
								CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase( APP_REMOVE_ATTRIBUTE );
								pWndRemoveAttribute->SetWeapon(pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_SMELT_REMOVE_PIERCING_EX ))
					{
						if( CItemElem::IsEleRefineryAble(pProp) )
						{
							if( IsUsableItem( pFocusItem ) )
							{			
								CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
								pWndRemovePiercing->SetItem(pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_SMELT_REMOVE_JEWEL ))
					{
						if( pProp->dwReferStat1 == WEAPON_ULTIMATE || pProp->dwItemKind2 == IK2_MATERIAL )
						{
							if( IsUsableItem( pFocusItem ) )
							{			
								CWndRemoveJewel* pWndRemoveJewel = (CWndRemoveJewel*)GetWndBase( APP_SMELT_REMOVE_JEWEL );
								pWndRemoveJewel->SetItem(pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_PET_TRANS_EGGS ))
					{	
						if(g_pPlayer->IsUsing(pFocusItem))
						{
							g_WndMng.PutString( prj.GetText( TID_GAME_TRANS_EGGS_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_TRANS_EGGS_ERROR2 ) );
							return TRUE;
						}
							
						if( (pProp->dwItemKind3 == IK3_EGG && pFocusItem->m_pPet == NULL) ||
							(pProp->dwItemKind3 == IK3_EGG && pFocusItem->m_pPet && pFocusItem->m_pPet->GetLevel() == PL_EGG) )
						{
							if( IsUsableItem( pFocusItem ) )
							{			
								CWndPetTransEggs* pWndPetTransEggs = (CWndPetTransEggs*)GetWndBase( APP_PET_TRANS_EGGS );
								pWndPetTransEggs->SetItem(pFocusItem);
								return TRUE;
							}
						}
						else
						{
							g_WndMng.PutString(TID_GAME_TRANS_EGGS_ERROR1);
							return TRUE;
						}
					}
					if(g_WndMng.GetWndBase( APP_SMELT_SAFETY ))
					{
						if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
						{			
							CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase( APP_SMELT_SAFETY );
							pWndSmeltSafety->SetItem(pFocusItem);
							return TRUE;
						}
					}
					if(g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL ))
					{
						if(pProp->dwID == II_GEN_MAT_ORICHALCUM01 || pProp->dwID == II_GEN_MAT_MOONSTONE ||
							pProp->dwID == II_GEN_MAT_ORICHALCUM01_1 || pProp->dwID == II_GEN_MAT_MOONSTONE_1)
						{
							if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
							{			
								CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
								pWndMixJewel->SetJewel(pFocusItem);
								return TRUE;
							}
						}
					}

					if(g_WndMng.GetWndBase( APP_SMELT_JEWEL ))
					{
						if( pProp->dwID == II_GEN_MAT_DIAMOND ||
							pProp->dwID == II_GEN_MAT_EMERALD ||
							pProp->dwID == II_GEN_MAT_SAPPHIRE ||
							pProp->dwID == II_GEN_MAT_RUBY ||
							pProp->dwID == II_GEN_MAT_TOPAZ )
						{
							if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
							{			
								CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)GetWndBase( APP_SMELT_JEWEL );
								pWndSmeltJewel->SetJewel(pFocusItem);
								return TRUE;
							}
						}
					}

					if(g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON ))
					{
						if( pProp->dwItemKind1 == IK1_WEAPON ||
							pProp->dwID == II_GEN_MAT_ORICHALCUM02 ||
							pProp->dwID == II_GEN_MAT_DIAMOND ||
							pProp->dwID == II_GEN_MAT_EMERALD ||
							pProp->dwID == II_GEN_MAT_SAPPHIRE ||
							pProp->dwID == II_GEN_MAT_RUBY ||
							pProp->dwID == II_GEN_MAT_TOPAZ )
						{
							if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
							{			
								CWndChangeWeapon* pWndChangeWeapon = (CWndChangeWeapon*)GetWndBase( APP_SMELT_CHANGEWEAPON );
								pWndChangeWeapon->SetItem(*pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_HERO_SKILLUP ))
					{
						if( pProp->dwID == II_GEN_MAT_DIAMOND ||
							pProp->dwID == II_GEN_MAT_EMERALD ||
							pProp->dwID == II_GEN_MAT_SAPPHIRE ||
							pProp->dwID == II_GEN_MAT_RUBY ||
							pProp->dwID == II_GEN_MAT_TOPAZ )
						{
							if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
							{			
								CWndHeroSkillUp* pWndHeroSkillUp = (CWndHeroSkillUp*)GetWndBase( APP_HERO_SKILLUP );
								pWndHeroSkillUp->SetJewel(pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_SMELT_EXTRACTION ))
					{
						if( pProp->dwItemKind1 == IK1_WEAPON )
						{
							if(pFocusItem->GetExtra() < pFocusItem->m_nItemNum)
							{			
								CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase( APP_SMELT_EXTRACTION );
								pWndExtraction->SetWeapon(*pFocusItem);
								return TRUE;
							}
						}
					}
					// �Ӽ�ī�峪 �ֳ����� ����Ŭ���������? ��þƮ���� �����Ѵ�.
					if( (pFocusItem->GetExtra() < pFocusItem->m_nItemNum) 
						&& ( 
							pProp->dwItemKind3 == IK3_ELECARD
							|| pProp->dwItemKind3 == IK3_ENCHANT 
							|| pProp->dwItemKind3 == IK3_SOCKETCARD
							|| pProp->dwItemKind3 == IK3_RANDOM_SCROLL
							|| pProp->dwItemKind3 == IK3_PIERDICE 
							|| pProp->IsNeedTarget()
							|| pProp->dwItemKind3 == IK3_SOCKETCARD2
							) 
						)
					{
#ifdef __QUIZ
						if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
						{
							g_WndMng.PutString(TID_SBEVE_NOTUSEITEM);
							return FALSE;
						}
#endif // __QUIZ
						bAble = FALSE;
						m_bIsUpgradeMode = TRUE;
						m_pUpgradeMaterialItem = pFocusItem;	

						if((g_pPlayer->IsMode( TRANSPARENT_MODE ) ) == 0)
							m_pSfxUpgrade = CreateSfx( XI_INT_INCHANT, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
					}
					
					if( pProp->dwItemKind1 == IK1_RIDE )	// �����? �������� Ż���ΰ�.
					{
						if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// Ground�����ΰ�?
							if( g_pPlayer->m_pActMover->m_bGround == 0 )		// ���� ���� �Ⱥ��̰� �ִٸ� ��ź��.
								bAble = FALSE;	// �����ۻ��? ����.
					}

					if( pFocusItem->m_dwItemId == II_SYS_SYS_MAP_FLARIS )
					{
						g_WndMng.OpenMap( "map_flaris.tga" );	// ������ ��� ���? ���Ƶ�
					}
					else
					if( pFocusItem->m_dwItemId == II_SYS_SYS_MAP_SAINTMORNING )
					{
						g_WndMng.OpenMap( "map_saintmorning.tga" );
					}
					
					if( pFocusItem->m_dwItemId == II_SYS_SYS_SCR_SOKCHANG )
					{
						SAFE_DELETE(g_WndMng.m_pWndChangeAttribute);
						g_WndMng.m_pWndChangeAttribute = new CWndChangeAttribute;
						g_WndMng.m_pWndChangeAttribute->SetChangeItem(pFocusItem);
						g_WndMng.m_pWndChangeAttribute->Initialize();
						bAble = FALSE;
					}

					if(pProp && pProp->dwID == II_SYS_SYS_SCR_RECCURENCE || pProp->dwID == II_SYS_SYS_SCR_RECCURENCE_LINK)
					{	// ����ų �������� ���? Ȯ�� â ����.
						if(g_pPlayer->m_nSkillPoint < g_pPlayer->GetCurrentMaxSkillPoint())
						{
							SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
							g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
							g_WndMng.m_pWndCommItemDlg->Initialize();
							g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_SKILLINIT, pFocusItem->m_dwObjId, dwObjId );
						}
						else
							g_WndMng.PutString(TID_GAME_ERROR_SKILLRECCURENCE);

						bAble = FALSE;
					}

					if( pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STR || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STA || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_DEX || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_INT
#ifdef __ADD_RESTATE_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STR_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STA_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_DEX_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_INT_LOW
#endif // __ADD_RESTATE_LOW
						)
					{
						SAFE_DELETE( g_WndMng.m_pWndRestateConfirm );
						g_WndMng.m_pWndRestateConfirm = new CWndRestateConfirm(pFocusItem->m_dwItemId);
						g_WndMng.m_pWndRestateConfirm->SetInformation(MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId, pFocusItem->GetProp()->dwParts);
						g_WndMng.m_pWndRestateConfirm->Initialize();
						bAble = FALSE;
					}

						switch( pProp->dwID )
						{
						case II_SYS_SYS_SCR_BLESSING:
							{
								if( g_pPlayer->IsSMMode( SM_REVIVAL ) )
								{
									g_WndMng.PutString(TID_GAME_LIMITED_USE);
								}
								else
								{
									SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
									g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
									g_WndMng.m_pWndCommItemDlg->Initialize();
									g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_DEPEN_USE, pFocusItem->m_dwObjId, dwObjId );
								}
								bAble = FALSE;
							}
							break;
//						case II_SYS_SYS_SCR_RECCURENCE:
//						case II_SYS_SYS_SCR_RECCURENCE_LINK:
//							{
//								SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
//								g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
//								g_WndMng.m_pWndCommItemDlg->Initialize();
//								g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_SKILLINIT, pFocusItem->m_dwObjId, dwObjId );
//								bAble = FALSE;
//							}
//							break;
						case II_SYS_SYS_SCR_CHACLA: {
							if (g_pPlayer->IsBaseJob()) {
								g_WndMng.PutString(TID_GAME_NOTUSEVAG);
							} else {
								SAFE_DELETE(g_WndMng.m_pWndCommItemDlg);
								g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
								g_WndMng.m_pWndCommItemDlg->Initialize();
								g_WndMng.m_pWndCommItemDlg->SetItem(TID_GAME_WARNINGCCLS, pProp->dwID, pFocusItem->m_dwObjId);
								bAble = FALSE;
							}
							break;
						}
						}

				}
			}
			if( bAble )	// �������� �������� ����.
			{
				if( g_WndMng.GetWndBase( APP_SHOP_ )  ||
					g_WndMng.GetWndBase( APP_BANK )  ||
					g_WndMng.GetWndBase( APP_TRADE ) )
				{
					g_WndMng.PutString(TID_GAME_TRADELIMITUSING);
				}
				else if(g_WndMng.GetWndBase( APP_SUMMON_ANGEL ))
				{
					g_WndMng.PutString(TID_GAME_TRADELIMITUSING);
				}
#ifdef __EVE_MINIGAME
				else if(Windows::IsOpen(
					APP_MINIGAME_DICE,
					APP_MINIGAME_KAWIBAWIBO,
					APP_MINIGAME_KAWIBAWIBO_WIN,
					APP_MINIGAME_PUZZLE,
					APP_MINIGAME_WORD
					))
				{
					g_WndMng.PutString(TID_SBEVE_NOTUSEITEM);
				}
#endif //__EVE_MINIGAME
				else if(Windows::IsOpen(
					APP_SMELT_EXTRACTION,
					APP_SMELT_JEWEL,
					APP_SMELT_MIXJEWEL,
					APP_PET_FOODMILL,
					APP_SMELT_SAFETY
				))
				{
					g_WndMng.PutString(TID_SBEVE_NOTUSEITEM);
				}
#ifdef __QUIZ
				else if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
				{
					g_WndMng.PutString(TID_SBEVE_NOTUSEITEM);
				}
#endif // __QUIZ
				else if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					g_WndMng.PutString(TID_SBEVE_NOTUSEITEM);
				}
				else
				{
					if(pFocusItem->GetProp() && (pFocusItem->GetProp()->dwFlag[IP_FLAG::EQUIP_BIND] ) && !pFocusItem->IsFlag( CItemElem::binds ) )
					{
						SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK);
						g_WndMng.m_pWndEquipBindConfirm->SetInformationDoubleClick(pFocusItem, dwObjId);
						g_WndMng.m_pWndEquipBindConfirm->Initialize();
					}
					else
					{
						// Ż�� ������ ���?, nPart�� ���� �����Ǿ� �ִ� �κа� ��ġ�ؾ� �ϹǷ� ������Ƽ���� ������ �ʴ´�.
						ItemProp* pItemProp = pFocusItem->GetProp();
						
						if( pItemProp )
						{
							int nPart	= pItemProp->dwParts;
							
							BOOL bEquiped	= g_pPlayer->m_Inventory.IsEquip( pFocusItem->m_dwObjId );
							if( bEquiped )
								nPart	= pFocusItem->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;
							if( !g_WndMng.CheckConfirm( pFocusItem ) )		//gmpbigsun: ��Ŷ�������� Ȯ�ε��� ó�����? 
							{
								g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId, nPart );
							}
						}
					}
				}
			}
		}
	}
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
			case 100: // icon
			//	m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
				break;
			//case 101: // report
			//	m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
			//	m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
			//	break;
			case WTBID_REPORT:
				/*
				if( m_bReport )
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				}
				else
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
				}
				m_bReport = !m_bReport;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
*/
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndInventory::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();
	
}
void CWndInventory::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
	if( lpWndCtrl->rect.PtInRect( point ) )
	{
		m_OldPos          = point;
		m_bLButtonDownRot = TRUE;
	}
		
	if( m_bLButtonDown )
	{
		for( int i=2; i<MAX_HUMAN_PARTS; i++ )
		{
			CRect DrawRect = m_InvenRect[i];
			
			if( DrawRect.PtInRect( point ) )
			{
				CItemElem * pItemBase = g_pPlayer->GetEquipItem( i );

				if( pItemBase )
				{
					m_pSelectItem  = pItemBase;
					return;
				}
			} 
		}
	}
	m_pSelectItem = NULL;
}

void CWndInventory::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		CRect DrawRect = m_InvenRect[i];

		if( DrawRect.PtInRect( point ) )
		{
			CItemElem* pItemElem = g_pPlayer->GetEquipItem( i );
			
			if( pItemElem )
			{
				g_DPlay.SendDoEquip( pItemElem, i );		// ������ȣ�� ���� ������. ��Į����.
				// �����κ� ������ ����Ŭ���ؼ� ����ų�?
				// �巡��&�������? �����Ϸ� �ϰų� �������? �Ҷ� ������ȣ�� ������.
			}
		}
	}
}

void CWndInventory::OnRButtonDown(UINT nFlags, CPoint point)
{
	BaseMouseCursor();
}

void CWndInventory::OnSetCursor() {
	SetEnchantCursor();
}

void CWndInventory::SetEnchantCursor() {
	// �κ�â�� �����ְ� ��þƮ ����̸�? Ŀ�����? ����
	if (m_bIsUpgradeMode) {
		SetMouseCursor(CUR_HAMMER);
	} else {
		SetMouseCursor(CUR_BASE);
	}
}


void CWndInventory::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDownRot )
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}
	
	m_OldPos = point;
	
	if( m_bLButtonDown && IsPush() )
	{
		if( m_bPickup == TRUE )
		{
			m_pSelectItem = NULL;
			return;
		}

		if( m_pSelectItem && m_pSelectItem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = ShortcutType::Item;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwId       = m_pSelectItem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_pTexture   = m_pSelectItem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectItem;
			_tcscpy( m_GlobalShortcut.m_szString, m_pSelectItem->GetProp()->szName);
		}
	}
}
BOOL CWndInventory::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if(pWndFrame == NULL)
		return FALSE;
	
	if( g_WndMng.GetWndBase( APP_SHOP_ ) ||
		g_WndMng.GetWndBase( APP_BANK ) ||
		g_WndMng.GetWndBase( APP_TRADE ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			SetForbid( TRUE );
			g_WndMng.PutString(TID_GAME_TRADELIMITUSING);
			
			return FALSE;
		}
	}
	else if( g_WndMng.GetWndBase( APP_REPAIR ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			SetForbid( TRUE );
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
			
			return FALSE;
		}
	}
	else if( g_WndMng.GetWndBase( APP_INVENTORY ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM21 );
			if( lpWndCtrl1->rect.PtInRect( point ) )
			{
				if( pShortcut->m_dwData ) // dwData�� 0�̸� ���?
				{
					CItemElem* pItemElem = g_pPlayer->GetItemId( pShortcut->m_dwId );
					if( !pItemElem )
						return FALSE;
					if(pItemElem->GetExtra() > 0)
						return FALSE;
					if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) ) 
						return FALSE;
					if( g_pPlayer->IsUsing( pItemElem ) )
						return FALSE;
					if( pItemElem->IsUndestructable() )
					{
						g_WndMng.PutString(TID_GAME_ERROR_UNDESTRUCTABLE_ITEM);
						return FALSE;
					}

					if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER  )
						return FALSE;

					SAFE_DELETE( g_WndMng.m_pWndInvenRemoveItem );
					g_WndMng.m_pWndInvenRemoveItem = new CWndInvenRemoveItem;
					g_WndMng.m_pWndInvenRemoveItem->Initialize();
					g_WndMng.m_pWndInvenRemoveItem->InitItem( pItemElem );
					return TRUE;
				}				
			}
		}
	}
	
	if( pShortcut->m_dwShortcut == ShortcutType::Item && pShortcut->m_pFromWnd != this && pWndFrame->GetWndId() == APP_INVENTORY )
	{
		if( pShortcut->m_dwData ) // dwData�� 0�̸� ���? 
		{
			CItemElem* pItemElem = g_pPlayer->GetItemId( pShortcut->m_dwId );
			if( pItemElem && pItemElem->GetProp()->dwParts != NULL_ID ) //&& pItemElem->GetProp()->dwParts == i * 3 + j )
			{
				//�κ��丮 �׵θ��� �÷����� �������� ������ ���� �ʵ��� ����.
        const CRect rect = CRect(
          GetWndCtrl(WIDC_CUSTOM5)->rect.TopLeft(),
          GetWndCtrl(WIDC_CUSTOM10)->rect.BottomRight()
        );

				if(rect.PtInRect( point ))
				{
					if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag[IP_FLAG::EQUIP_BIND] ) && !pItemElem->IsFlag( CItemElem::binds ) )
					{
						SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DRAG_AND_DROP);
						g_WndMng.m_pWndEquipBindConfirm->SetInformationDragAndDrop(pItemElem);
						g_WndMng.m_pWndEquipBindConfirm->Initialize();
					}
					else
					{
						g_DPlay.SendDoEquip( pItemElem );
					}
					return TRUE;
				}
			}
		}
	}
	return CWndBase::OnDropIcon( pShortcut, point );
}
void CWndInventory::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndConfirmBuy )
		SAFE_DELETE( m_pWndConfirmBuy );
}


void CWndInventory::InitializeInvenRect(std::array<CRect, MAX_HUMAN_PARTS> & invenRect, /* const */ CWndBase & self) {
	// Not displayed parts:
	// PARTS_HEAD, PARTS_HAIR, PARTS_LOWER_BODY, PARTS_ROBE,
	// PARTS_LOWER2, PARTS_PROPERTY, PARTS_CLOAK2
	
	const auto AffectRect = [&](UINT widgetCtrlId, int partId) {
		invenRect[partId] = self.GetWndCtrl(widgetCtrlId)->rect;
	};

	AffectRect(WIDC_CUSTOM1 , PARTS_CAP);
	AffectRect(WIDC_CUSTOM1 , PARTS_CAP2);
	AffectRect(WIDC_CUSTOM2 , PARTS_UPPER_BODY);
	AffectRect(WIDC_CUSTOM2 , PARTS_UPPER2);
	AffectRect(WIDC_CUSTOM3 , PARTS_HAND);
	AffectRect(WIDC_CUSTOM3 , PARTS_HAND2);
	AffectRect(WIDC_CUSTOM4 , PARTS_FOOT);
	AffectRect(WIDC_CUSTOM4 , PARTS_FOOT2);
	AffectRect(WIDC_CUSTOM5 , PARTS_RWEAPON);
	AffectRect(WIDC_CUSTOM6 , PARTS_LWEAPON);
	AffectRect(WIDC_CUSTOM6 , PARTS_SHIELD);
	AffectRect(WIDC_CUSTOM7 , PARTS_BULLET);
	AffectRect(WIDC_CUSTOM8 , PARTS_CLOAK);
	AffectRect(WIDC_CUSTOM9 , PARTS_MASK);
	AffectRect(WIDC_CUSTOM10, PARTS_RIDE);

	AffectRect(WIDC_CUSTOM11, PARTS_RING1);
	AffectRect(WIDC_CUSTOM12, PARTS_EARRING1);
	AffectRect(WIDC_CUSTOM13, PARTS_NECKLACE1);
	AffectRect(WIDC_CUSTOM14, PARTS_EARRING2);
	AffectRect(WIDC_CUSTOM15, PARTS_RING2);

	AffectRect(WIDC_CUSTOM16, PARTS_HAT);
	AffectRect(WIDC_CUSTOM17, PARTS_CLOTH);
	AffectRect(WIDC_CUSTOM18, PARTS_GLOVE);
	AffectRect(WIDC_CUSTOM19, PARTS_BOOTS);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// �� �ŷ� 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTradeGold * CWndTradeGold::CreateGeneric(
	SHORTCUT::Sources::ItemOrMoney source,
	std::function<void(int)> m_onValidation,
	ExtraCreateParam extra
) {
	SAFE_DELETE(g_WndMng.m_pWndTradeGold);

	if (!g_pPlayer) {
		extra.onCancel();
		return nullptr;
	}

	const auto [initialQuantity, skipIf1, titleTID, countTID] = GetInitialValueOf(source);

	if (initialQuantity <= 0) {
		extra.onCancel();
		return nullptr;
	}

	if (initialQuantity == 1 && skipIf1) {
		m_onValidation(1);
		return nullptr;
	}

	g_WndMng.m_pWndTradeGold = new CWndTradeGold();
	g_WndMng.m_pWndTradeGold->Initialize();

	g_WndMng.m_pWndTradeGold->SetInitialValue(initialQuantity);
	g_WndMng.m_pWndTradeGold->m_source = source;
	g_WndMng.m_pWndTradeGold->m_onValidation = std::move(m_onValidation);
	g_WndMng.m_pWndTradeGold->m_onCancel = std::move(extra.onCancel);

	g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC)
		->m_strTitle = prj.GetText(titleTID);
	g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1)
		->m_strTitle = prj.GetText(countTID);

	return g_WndMng.m_pWndTradeGold;
}


BOOL CWndTradeGold::Initialize(CWndBase * pWndParent) {
	return InitDialog(APP_TRADE_GOLD, pWndParent, WBS_MODAL, 0);
}

BOOL CWndTradeGold::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_OK || message == EN_RETURN) {
		CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT);

		LPCTSTR string = pWndEdit->GetString();
		int nCost = std::atoi(string);

		const auto initializer = GetInitialValueOf(m_source);

		if (initializer.initialQuantity <= 0) {
			nCost = 0;
		} else if (initializer.skipIf1) {
			// Item: at least one
			nCost = std::clamp<int>(nCost, 1, initializer.initialQuantity);
		} else {
			// Money: 0 can be considered
			nCost = std::clamp<int>(nCost, 0, initializer.initialQuantity);
		}

		if (nCost > 0) {
			m_onValidation(nCost);
		} else {
			m_onCancel();
		}

		Destroy();
	}

	if (nID == WIDC_CANCEL) {
		m_onCancel();
		Destroy();
	} else if (nID == WTBID_CLOSE) {
		m_onCancel();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndTradeGold::Initializer CWndTradeGold::GetInitialValueOf(SHORTCUT::Sources::ItemOrMoney source) {
	using Source = SHORTCUT::Source;
	
	struct VisitorQuantity {
		int operator()(Source::Inventory sourceItem) {
			const CItemElem * pItemBase = g_pPlayer->GetItemId(sourceItem.itemPos);
			return pItemBase && pItemBase->GetExtra() == 0 ? pItemBase->m_nItemNum : 0;
		}

		int operator()(Source::Penya sourceItem) {
			return g_pPlayer->GetGold();
		}

		int operator()(Source::Bag sourceBag) {
			const CItemElem * pItemBase = g_pPlayer->m_Pocket.GetAt(sourceBag.bagId, sourceBag.itemPos);
			return pItemBase && pItemBase->GetExtra() == 0 ? pItemBase->m_nItemNum : 0;
		}

		int operator()(Source::Bank sourceBank) {
			const CItemElem * pItemBase = g_pPlayer->GetItemBankId(sourceBank.slot, sourceBank.itemPos);
			return pItemBase && pItemBase->GetExtra() == 0 ? pItemBase->m_nItemNum : 0;
		}

		int operator()(Source::BankPenya source) {
			return g_pPlayer->m_dwGoldBank[source.slot];
		}

		int operator()(Source::Guild source) {
			CGuild * pGuild = g_pPlayer->GetGuild();
			if (!pGuild) return 0;
			if (!pGuild->IsGetItem(g_pPlayer->m_idPlayer)) return 0;

			const CItemElem * pItemBase = pGuild->m_GuildBank.GetAtId(source.itemPos);
			return pItemBase && pItemBase->GetExtra() == 0 ? pItemBase->m_nItemNum : 0;
		}

		int operator()(Source::GuildMoney source) {
			CGuild * pGuild = g_pPlayer->GetGuild();
			if (!pGuild) return 0;
			if (!pGuild->IsGetPenya(g_pPlayer->m_idPlayer)) return 0;

			return pGuild->m_nGoldGuild;
		}
	};

	const int initialQuantiy = std::visit(VisitorQuantity{}, source);

	if (std::holds_alternative<Source::Inventory>(source)
		|| std::holds_alternative<Source::Bank>(source)
		|| std::holds_alternative<Source::Guild>(source)
		|| std::holds_alternative<Source::Bag>(source)
		) {
		return Initializer{
			.initialQuantity = initialQuantiy,
			.skipIf1 = true,
			.titleTID = TID_GAME_MOVECOUNT,
			.countTID = TID_GAME_NUMCOUNT
		};
	} else {
		return Initializer{
			.initialQuantity = initialQuantiy,
			.skipIf1 = false,
			.titleTID = TID_GAME_MOVEPENYA,
			.countTID = TID_GAME_PENYACOUNT
		};
	}
}

BOOL CWndTradeGold::Process() {
	using Source = SHORTCUT::Source;

	if (const Source::Inventory * srcItem = std::get_if<Source::Inventory>(&m_source)) {
		const CItemElem * pItemBase = g_pPlayer->GetItemId(srcItem->itemPos);
		if (!pItemBase || pItemBase->GetExtra() > 0) {
			Destroy();
		}
	}

	return TRUE;
}

void CWndTradeGold::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	GetDlgItem(WIDC_EDIT)->SetFocus();
	GetDlgItem(WIDC_OK)->SetDefault();

	MoveParentCenter();
}

void CWndTradeGold::SetInitialValue(int value) {
	value = std::max(value, 0);
	const std::string szNumber = std::to_string(value);

	CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT);
	pWndEdit->SetString(szNumber.c_str());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// �ŷ� Ȯ��
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CWndConfirmTrade::OnSetName( const char* szName, OBJID objid )
{
	m_objid = objid;
	CWndStatic *m_pName   = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CString sName;
	sName = szName;
	sName.Format( prj.GetText(TID_GAME_FROM),  szName ); // " �Կ���"
	m_pName->SetTitle( sName );
}

void CWndConfirmTrade::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	Move70();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndConfirmTrade::Initialize( CWndBase* pWndParent )
{ 
	m_objid = 0;
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_CONFIRM_TRADE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndConfirmTrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CMover* pTrader	= prj.GetMover( m_objid );
		if( pTrader != NULL )
		{
			g_DPlay.SendTrade( pTrader );
		}
		Destroy();		// �����ı��� �ٲ�.
	}
	else 
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		g_DPlay.SendPacket<PACKETTYPE_CONFIRMTRADECANCEL, OBJID>(m_objid);
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// �ŷ� ���� Ȯ��
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CWndTradeConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	GetDlgItem<CWndButton>(WIDC_YES)->SetVisible(TRUE);
	GetDlgItem<CWndButton>(WIDC_NO)->SetVisible(TRUE);
	GetDlgItem(WIDC_STATIC1)->SetTitle(prj.GetText(TID_DIAG_0083));

	bMsg = false;

	MoveParentCenter();
}

BOOL CWndTradeConfirm::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_TRADE_CONFIRM, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndTradeConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (bMsg) return TRUE;

	if (nID == WIDC_NO || nID == WTBID_CLOSE) {
		g_DPlay.SendTradeCancel();
	} else if (nID == WIDC_YES) {
		g_DPlay.SendPacket<PACKETTYPE_TRADECONFIRM>();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndTradeConfirm::OnTradelastConfirmOk() {
	bMsg = true;

	GetDlgItem(WIDC_YES)->SetVisible(FALSE);
	GetDlgItem(WIDC_NO)->SetVisible(FALSE);
	GetDlgItem(WIDC_STATIC1)->SetTitle(prj.GetText(TID_GAME_WAITCOMFIRM));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// �ŷ� 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTrade::CWndTrade()
{
	SetPutRegInfo( FALSE );
}
CWndTrade::~CWndTrade()
{
	g_DPlay.SendTradeCancel();
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
}

void CWndTrade::OnDraw(C2DRender* p2DRender)
{
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
	if( pMover && pMover->GetType() == OT_MOVER )
	{
		CWndStatic* pWndNameYou = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		CWndStatic* pWndNameI = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		CWndStatic* pWndGoldYou = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		CWndStatic* pWndGoldI = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );

		pWndNameYou->SetTitle( pMover->GetName( TRUE ) );
		pWndNameI->SetTitle( g_pPlayer->GetName() );

		CString string;
		int nLength;
		string.Format( "%d", m_nGoldYou );
		
		nLength = string.GetLength();
		while(nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldYou->SetTitle( string );

		string.Format( "%d", m_nGoldI );
		nLength = string.GetLength();
		while(nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldI->SetTitle( string );
		
	}
	else
	{
		g_pPlayer->m_vtInfo.TradeClear();
	}
}
void CWndTrade::OnInitialUpdate()
{
	const auto youRect = CRect(   5, 25 + 15,   5 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 );
	m_wndItemCtrlYou.Create( WLVS_ICON|WBS_NODRAWFRAME, youRect, this, 10001 );
	const auto iRect   = CRect( 175, 25 + 15, 175 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 );
	m_wndItemCtrlI.  Create( WLVS_ICON|WBS_NODRAWFRAME, iRect, this, 10002 );

	m_nGoldI = 0;
	m_nGoldYou = 0;

	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();

	if( pMover )
	{
		pMover->m_vtInfo.TradeClear();
		pMover->m_vtInfo.SetOther( g_pPlayer );
		m_wndItemCtrlYou.InitItem( pMover );
		m_wndItemCtrlI.InitItem( g_pPlayer );
	}

	MoveParentCenter();
	
	CWndNeuz::OnInitialUpdate();

	CWndButton* pClearButton = (CWndButton*)GetDlgItem(WIDC_CLEAR);
	if(pClearButton)
	{
		pClearButton->EnableWindow(FALSE);
		pClearButton->SetVisible(FALSE);
	}
	if(GetWndBase( APP_BAG_EX )) GetWndBase( APP_BAG_EX )->Destroy();
	if( GetWndBase(APP_WEBBOX) || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase() )
	{
		Destroy();
		return;
	}
}

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndTrade::Initialize( CWndBase* pWndParent )
{ 
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_TRADE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndTrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if (g_WndMng.GetWndBase(APP_TRADE_CONFIRM)) {
		return TRUE;
	}

	if( message == WIN_ITEMDROP && ( nID == 10001 || nID == 10002 ) )
	{
		SHORTCUT & shortcut = reinterpret_cast<SHORTCUT &>(*pLResult);

		if (shortcut.m_dwData == 0) {
			if (m_nGoldI == 0) {
				CWndTradeGold::Create<SHORTCUT::Source::Penya>(
					{}, [](auto, int quantity) { g_DPlay.SendTradePutGold(quantity); }
				);
			}
		} else {
			CWndTradeGold::Create<SHORTCUT::Source::Inventory>(
				{ shortcut.m_dwId },
				[tradePos = shortcut.m_dwData - 100](auto source, int quantity) {
					g_DPlay.SendTradePut((BYTE)(tradePos), 0, source.itemPos, quantity);
				}
			);
		}

	}
	else
	{
		if( nID == WIDC_CLEAR )	// clear
		{
		}
		else
		{
			if( nID == WIDC_OK_I )	// Ȯ��
			{
				g_DPlay.SendTradeOk();
			}
			else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE ) // ���? 
			{
				g_DPlay.SendTradeCancel();
				return( TRUE );
			}
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndTrade::DoCancel()
{
	CMover* pTrader	= g_pPlayer->m_vtInfo.GetOther();
	if( pTrader )
		pTrader->m_vtInfo.TradeClear();
	g_pPlayer->m_vtInfo.TradeClear();
	
	Destroy();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// �׺������? 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define OBJFILTER_PLAYER   0
#define OBJFILTER_PARTY    1 
#define OBJFILTER_NPC      2
#define OBJFILTER_MONSTER  3 

void CWndNavigator::SetRegionName( const TCHAR *tszName )
{
	CString strTitle = tszName;

	if( strTitle.IsEmpty() )
		strTitle = prj.GetText( TID_APP_NAVIGATOR );

	SetTitle( strTitle );
}

BOOL CWndNavigator::OnEraseBkgnd(C2DRender* p2DRender)
{
	if( g_pPlayer == NULL ) return TRUE;
	CWorld* pWorld	= g_WorldMng();
	CRect rect = GetClientRect();

	// ��Ŀ�� ������Ʈ ���? 
	CObj* pObjFocus = pWorld->GetObjFocus();
		
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	CLandscape* pLand = pWorld->GetLandscape( pWorld->m_pCamera->m_vPos );
	if( pLand == NULL ) return 1;
	int nImageBlock = pLand->m_texMiniMap.m_size.cx;

	CRectClip clipRect( 0, 0, p2DRender->m_clipRect.Width(), p2DRender->m_clipRect.Height() );
	CPoint point;
	CRect rectCur;

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );
	D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );
	vCenter.x *= fx;
	vCenter.z *= fy;
	int xCenter = (int)( vCenter.x );
	int yCenter = (int)( - vCenter.z );
	int i;

	BOOL  bDrawGuildWarNum = FALSE;
	DWORD dwWorldID = pWorld->GetID();

	if( dwWorldID == WI_WORLD_GUILDWAR )
		bDrawGuildWarNum = TRUE;
	
	for( int z = 0; z < pWorld->m_nLandHeight; z++ )
	{
		for( int x = 0; x < pWorld->m_nLandWidth; x++ )
		{
			point = CPoint ( x * nImageBlock, z * nImageBlock );
			point.y -= ( nImageBlock * pWorld->m_nLandWidth );
			point.x += rect.Width() / 2;
			point.y += rect.Height() / 2;
			point.x -= xCenter;
			point.y -= yCenter;
			rectCur.SetRect( point.x, point.y, point.x + nImageBlock, point.y + nImageBlock );
			CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
			if( pLand )
			{
				pLand->m_texMiniMap.m_size = m_size;
				if( clipRect.RectLapRect( rectCur ) )
				{
					if( pLand->m_texMiniMap.m_pTexture )
					{
//						pLand->m_texMiniMap.m_size = m_size;
						pLand->m_texMiniMap.Render( p2DRender, point, 200 );//CWndBase::m_nAlpha );

						if( bDrawGuildWarNum )
						{
							m_GuildCombatTextureMask.m_size = pLand->m_texMiniMap.m_size;
							m_GuildCombatTextureMask.Render( p2DRender, point, 200 );
						}
					}
				} 
			}
		}
	}

	AccuFrame();	//	������ ������ �����? ���ش�.

	if( pWorld->m_bIsIndoor == FALSE 
		&& !pWorld->IsWorldInstanceDungeon()
		)
	{
		// ��ü �����? ���� ���� 
		TEXTUREVERTEX vertex[ 1000 * 6 ];
		TEXTUREVERTEX* pVertices = vertex; 
		int nCount = 0;
		int xu = 0, yv = 0;
		CRect aRcArray[ 100 ];
		int nRcCount = 0;
		
		////////////////////////////////////////////////////
		// ������Ʈ ���? 
		////////////////////////////////////////////////////
		int nIndex = 0;
		int nPartyMap[ MAX_PTMEMBER_SIZE ];
		CMover* apQuest[ 100 ];
		int nQuestNum = 0;
		ZeroMemory( nPartyMap, sizeof(nPartyMap) );
		int nPartyMapCount = 0;
		float fDistMap = rect.Width() / 2 / fx;

		for (CLandscape * pLand : pWorld->GetVisibleLands())
		{
			for (CObj * pObj : pLand->m_apObjects[OT_MOVER].ValidObjs()) {
//				BOOL bPartyMap = FALSE;
				if( pObj != g_pPlayer )
				{
					CMover* pMover = (CMover*)pObj;						

					if( g_pPlayer->m_idMurderer != pMover->m_idPlayer )	// ���� Ÿ���� �� ���γ��̸� ���Ǿ��� ������ ��´�?.
					{
						// �÷��̾����Ͱ� �ƴѵ�, �÷��̾��� 
						if( m_bObjFilterPlayer == FALSE && pMover->IsPlayer() == TRUE )
						{
							// �Դٰ� ��Ƽ���͵� �ƴ϶��? ��ŵ 
							if( m_bObjFilterParty == FALSE )
								continue;
							if( g_Party.IsMember( pMover->m_idPlayer ) == FALSE ) 
								continue;
						}
						if( m_bObjFilterMonster == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == FALSE ) )
							continue;
						if( m_bObjFilterNPC == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == TRUE ) )
						{
							// ������ �ƴ϶��? ��Ƽ��.
							LPCHARACTER lpCharacter = pMover->GetCharacter();
							if( lpCharacter == NULL || lpCharacter->m_nStructure == -1 ) 
								continue;
								
						}
						if( pMover->IsMode( TRANSPARENT_MODE ) || pMover->IsAuthHigher( AUTH_GAMEMASTER ) )	// �����? ��������̰ų�? GM�ϰ��? Ÿ�� �ȵ�.
							continue;
						
					}
					vPos = pObj->GetPos();
					vPos.x *= fx;
					vPos.z *= fy;
					point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
					point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
					point.x -= xCenter;
					point.y -= yCenter;
					
					CRect rectHittest( point.x, point.y, point.x + 5, point.y + 5);
					//rectHittest.InflateRect( 4, 4 );
					CPoint ptMouse = GetMousePoint();
					if( rectHittest.PtInRect( ptMouse ) )
					{
						ClientToScreen( &ptMouse );
						ClientToScreen( &rectHittest );
						g_toolTip.PutToolTip( 10000 + ((CMover*)pObj)->GetId(), ((CMover*)pObj)->GetName( TRUE ), rectHittest, ptMouse, 0 );
					}
					// ����Ʈ �̸�Ƽ�� 
					if( pMover->IsNPC() && pMover->m_nQuestEmoticonIndex != -1 )
					{
						apQuest[ nQuestNum++ ] = pMover;
					}
					else
					if( g_pPlayer->m_idMurderer && g_pPlayer->m_idMurderer == pMover->m_idPlayer )		// ���� Ÿ���� �� ���γ��̸� ������ ǥ��
						nIndex = 5;
					else
					if( pMover->IsNPC() && pMover->IsPeaceful() )
					{
						nIndex = 1;
					}
					else
					if( g_pPlayer->m_idGuild && g_pPlayer->m_idGuild == pMover->m_idGuild )
						nIndex = 3;
					else
					if( pMover->IsNPC() && pMover->IsPeaceful() == FALSE )
						nIndex = 2;
					else
					if( pMover->IsPlayer() )
					{
						nPartyMap[nPartyMapCount] = g_Party.FindMember( pMover->m_idPlayer );
						if( nPartyMap[nPartyMapCount] != -1 )
						{
							g_Party.m_aMember[ nPartyMap[nPartyMapCount] ].m_vPos = pMover->GetPos();
							++nPartyMapCount;
							nIndex = 4;
						}
						else
							nIndex = 0;
					}
					if( nCount < 1000 )
					{
						nCount++;
						
						LPCHARACTER lpCharacter = pMover->GetCharacter();
						if( lpCharacter && lpCharacter->m_nStructure != -1 ) 
							m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpCharacter->m_nStructure, &pVertices );
						else
						{
							// ����Ʈ�� ���� �͸� ���? 
							if( pMover->m_nQuestEmoticonIndex == -1 )
								m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), nIndex, &pVertices );
						}
					}
				}
			}
		}

		////////////////////////////////////////////////////////
		// ���� ������Ʈ�� ������ ��Ʈ���� �̸�Ƽ�� ���? 
		////////////////////////////////////////////////////////
		for (const REGIONELEM & lpRegionElem : pWorld->m_aStructure.AsSpan()) {
			vPos = lpRegionElem.m_vPos;
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;
			m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpRegionElem.m_dwStructure, &pVertices );
		}
		////////////////////////////////////////////////////////
		// ����Ʈ �̸�Ƽ�� ���? 
		// ���? ������ ���� �� ���� ������ �ǹǷ� ������ ���Ƽ� ��´�?.
		////////////////////////////////////////////////////////
		for( i = 0; i < nQuestNum; i++ )
		{
			CMover* pMover = apQuest[ i ];
			vPos = pMover->GetPos();
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;

			nIndex = pMover->m_nQuestEmoticonIndex + 20;
			m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );		
		}

		////////////////////////////////////////////////////
		// ��Ŀ�� ������Ʈ ���? (���� �и��ϰ� ���̵���)
		////////////////////////////////////////////////////
		if( pObjFocus )
		{
			vPos = pObjFocus->GetPos();
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;

			xu = 9, yv = 9;

			point.x -= 3;
			point.y -= 3;

			if( nCount < 1000 )
			{
				nCount++;
				m_texNavObjs.MakeVertex( p2DRender, point, 5, &pVertices );
			}
		}
		m_texNavObjs.Render( vertex, ( (int) pVertices - (int) vertex ) / sizeof( TEXTUREVERTEX ) );

		D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
		if( rDestinationArrow != D3DXVECTOR3( -1.0F, 0.0F, -1.0F ) )
		{
			vPos = rDestinationArrow;
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;

			xu = 9, yv = 9;

			point.x -= 3;
			point.y -= 3;

			m_pDestinationPositionTexture->RenderScal( p2DRender, point, 255, 0.4F, 0.4F );
		}
	}
	else
	{
		m_texDunFog.m_size = CSize( rect.Width(), rect.Height() );
		p2DRender->RenderTexture( CPoint( 0, 0 ), &m_texDunFog, 255 );
	}
	m_wndPlace.EnableWindow( !pWorld->m_bIsIndoor );

	RenderMarkAll(p2DRender, g_pPlayer);

	// ȭ�� ���� ������ ���Ƿ� ���簢�� ����Ʈ�� ������ ���´�. �ȱ׷��� ȭ��ǥ �����? ��׷���?.
	D3DVIEWPORT9 viewport;
	viewport.X      = p2DRender->m_clipRect.left + ( p2DRender->m_clipRect.Width()  / 2 );
	viewport.Y      = p2DRender->m_clipRect.top  + ( p2DRender->m_clipRect.Height() / 2 );
	viewport.Width  = 32;
	viewport.Height = 32;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	viewport.X      -= 16;
	viewport.Y      -= 16;

	pd3dDevice->SetViewport( &viewport );

	// �������� 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	// ī�޶� 
	D3DXMATRIX  matView;
	D3DXMatrixIdentity( &matView );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity( &matWorld );

	D3DXMatrixScaling( &mat, 1.2f, 1.2f, 1.2f );
	matWorld = matWorld * mat;

	D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 vDestNor  = g_pPlayer->GetPos() - g_Neuz.m_camera.m_vPos;
	D3DXVECTOR3 vAxis;
	D3DXQUATERNION   qRot;
	
	FLOAT       fTheta;
	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	D3DXVECTOR3 vYPW;
	
	QuaternionRotationToYPW( qRot, vYPW );
	D3DXMatrixRotationZ( &mat, -(vYPW.y) );
	
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		

	// ȭ��ǥ ���? 
	m_billArrow.Render( );

	return TRUE;
}

//	�̰� �־� �ش����� ������ ���ؼ� NaviPoint���� ������ �����ͷ� �޴� ���·� �ٽ� �����? �־��? �Ұ��̴�.
void CWndNavigator::RenderMarkAll(C2DRender * p2DRender, CMover * Player) {
	RenderNaviPoint(p2DRender, Player->m_nvPoint);

	auto nvi = g_pPlayer->m_vOtherPoint.begin();
	while (nvi != g_pPlayer->m_vOtherPoint.end()) {
		if (RenderNaviPoint(p2DRender, *nvi)) {
			nvi++;
		} else {
			nvi = g_pPlayer->m_vOtherPoint.erase(nvi);
		}
	}
}

bool CWndNavigator::RenderNaviPoint(C2DRender * p2DRender, NaviPoint & naviPoint) {
	if (naviPoint.Time == 0) return false;

	D3DXVECTOR3 Pos = g_pPlayer->GetPos() - naviPoint.Pos;
	Pos.x *= (FLOAT)m_size.cx / (MAP_SIZE * MPU);
	Pos.z *= -(FLOAT)m_size.cy / (MAP_SIZE * MPU);
	naviPoint.Time--;

	CRect rect = GetClientRect();

	const CPoint point(
		rect.Width()  / 2 - Pos.x - 2,
		rect.Height() / 2 - Pos.z - 2
	);

	m_texNavPos.Render(p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

	CRect rectHit(point.x - 8, point.y - 8, point.x + 8, point.y + 8);
	CPoint ptMouse = GetMousePoint();
	if (rectHit.PtInRect(ptMouse)) {
		CString toolTip = prj.GetText(TID_GAME_NAV_MARK);
		toolTip += naviPoint.Name.c_str();
		ClientToScreen(&ptMouse);
		ClientToScreen(&rectHit);
		g_toolTip.PutToolTip(10000 + naviPoint.objid, toolTip, rectHit, ptMouse, 0);
	}

	return true;
}

HRESULT CWndNavigator::DeleteDeviceObjects()
{
	//CWndNeuz::DeleteDeviceObjects();
	m_texArrow.DeleteDeviceObjects();
	m_texNavObjs.DeleteDeviceObjects();
	m_billArrow.DeleteDeviceObjects();
	m_texNavPos.DeleteDeviceObjects();
	m_GuildCombatTextureMask.DeleteDeviceObjects();
#ifdef __YDEBUG
	m_texDunFog.DeleteDeviceObjects();
#endif //__YDEBUG
	
	return CWndNeuz::DeleteDeviceObjects();
}
HRESULT CWndNavigator::InvalidateDeviceObjects()
{	
	m_billArrow.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndNavigator::RestoreDeviceObjects()
{
	ResizeMiniMap();
	m_billArrow.RestoreDeviceObjects();
	return CWndNeuz::RestoreDeviceObjects();
}
void CWndNavigator::OnDraw(C2DRender* p2DRender)
{
	// Rainbow Race Time ���?
	DWORD dwRainbowRaceTime = CRainbowRace::GetInstance()->m_dwRemainTime;
	if(dwRainbowRaceTime > 0)
	{
		char szMsg[256] = { 0 };
		CTimeSpan ct( (dwRainbowRaceTime -  GetTickCount()) / 1000 );
		sprintf( szMsg, "%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
		CRect rectWindow = GetClientRect();
		p2DRender->TextOut(rectWindow.right - 50, rectWindow.bottom - 16, szMsg, 0xffffff00);
	}
}
void CWndNavigator::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
		}
		else
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
			ar >> m_size.cx >> m_size.cy;
		}
		m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
		m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
		m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
		m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );
	}
	else
	{
		dwVersion = 1;
		ar << m_bObjFilterPlayer << m_bObjFilterParty << m_bObjFilterNPC << m_bObjFilterMonster;
		ar << m_size.cx << m_size.cy;
	}
}
void CWndNavigator::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectClient = GetClientRect();
	rectClient.right = 13;
	m_wndPlace.Create  ( "P", 0, CRect( rectClient.left,   1, rectClient.left + 16,   1 + 16 ), this, 100000 );
	m_wndZoomIn.Create ( "+", 0, CRect( rectClient.left,  54, rectClient.left + 16,  54 + 16 ), this, 100005 );
	m_wndZoomOut.Create( "-", 0, CRect( rectClient.left,  70, rectClient.left + 16,  70 + 16 ), this, 100006 );

	m_wndPlace.SetTexture( MakePath( DIR_THEME, "ButtNavLeft.tga" ), TRUE );
	m_wndPlace.FitTextureSize();
	m_wndZoomIn.SetTexture( MakePath( DIR_THEME, "ButtNavZoomIn.tga" ), TRUE );
	m_wndZoomIn.FitTextureSize();
	m_wndZoomOut.SetTexture( MakePath( DIR_THEME, "ButtNavZoomOut.tga" ), TRUE );
	m_wndZoomOut.FitTextureSize();

	m_texDunFog.LoadTexture( MakePath( DIR_THEME, "NavDunFog.tga" ), 0 , 1 );

	m_pDestinationPositionTexture = CWndBase::m_textureMng.AddTexture( MakePath( DIR_THEME, "ButtDestination.bmp"), 0xffff00ff );

	m_wndMenuPlace.CreateMenu( this );	
	m_wndMenuPlace.AddButton(0, prj.GetText(TID_GAME_PLAYER));
	m_wndMenuPlace.AddButton(1, prj.GetText(TID_GAME_PARTYTEXT));
	m_wndMenuPlace.AddButton(2, prj.GetText(TID_GAME_NPC));
	m_wndMenuPlace.AddButton(3, prj.GetText(TID_GAME_MONSTER));
	

	m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
	m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
	m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
	m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );

	m_wndMenuMover.CreateMenu( this );	

	m_size = CSize( 256, 256) ;//MINIMAP_SIZE, MINIMAP_SIZE );
	m_nSizeCnt = 0;

	CRect rectRoot = g_WndMng.GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.top );
	Move( point );

	ResizeMiniMap();

}
BOOL CWndNavigator::Initialize(CWndBase* pWndParent)
{
	CRect rectWindow = g_WndMng.GetWindowRect();
	CRect rect( 0, 0, 115, 110 ); // 1024 768

	m_texNavObjs.LoadScript( MakePath( DIR_THEME,"Navigator.inc") );
	m_texArrow.LoadTexture( MakePath( DIR_THEME,"ImgNavArrow.bmp"), 0xffff00ff );
	m_texNavPos.LoadScript( MakePath( DIR_THEME, "NavPosition.inc") );
	ZeroMemory( &m_billboard, sizeof( m_billboard ) );
	m_billboard.rect.SetRect( 0, 0, m_texArrow.m_size.cx, m_texArrow.m_size.cy );
	m_billboard.ptCenter = CPoint( m_texArrow.m_size.cx / 2, m_texArrow.m_size.cy / 2 );
	m_billArrow.InitDeviceObjects( &m_billboard, &m_texArrow );
	m_billArrow.RestoreDeviceObjects();

	m_GuildCombatTextureMask.LoadTexture( MakePath( DIR_WORLD_GUILDCOMBAT, "WdGuildWar_Mask.dds" ), 0  );
	
	SetTitle( GETTEXT( TID_APP_NAVIGATOR ) );
	return CWndNeuz::InitDialog( APP_NAVIGATOR, pWndParent, 0, CPoint( 792, 130 ) );
}
void CWndNavigator::ResizeMiniMap()
{
	CWorld* pWorld	= g_WorldMng();
	if( pWorld == NULL )
		return;
	for( int z = 0; z < pWorld->m_nLandHeight; z++ )
	{
		for( int x = 0; x < pWorld->m_nLandWidth; x++ )
		{
			CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
			if( pLand )
				pLand->m_texMiniMap.m_size = m_size;
		}
	}
}
BOOL CWndNavigator::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CRect rect = GetWindowRect( TRUE );

	CWndBase* pWndBase = (CWndBase*) pLResult;

	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
		int nState = !m_wndMenuPlace.GetMenuState( nID );
		m_wndMenuPlace.CheckMenuItem( nID, nState );
		SetFocus();
		switch( nID )
		{
		case OBJFILTER_PLAYER:
			m_bObjFilterPlayer = nState;
			break;
		case OBJFILTER_PARTY:
			m_bObjFilterParty = nState;
			break;
		case OBJFILTER_NPC:
			m_bObjFilterNPC = nState;
			break;
		case OBJFILTER_MONSTER:
			m_bObjFilterMonster = nState;
			break;
		}
	}
	else
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case 100000: // ���? ã��
				{
					CRect rectRootLayout = g_WndMng.GetLayoutRect();
					int nMenuPlaceLeft = rect.left - m_wndMenuPlace.GetWindowRect().Width();
					if( nMenuPlaceLeft < rectRootLayout.left )
						m_wndMenuPlace.Move( CPoint( rect.right, rect.top ) );
					else
						m_wndMenuPlace.Move( CPoint( nMenuPlaceLeft, rect.top ) );
					m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
					m_wndMenuPlace.SetFocus();
				}
				break;
			case 100001: // ���? ã��
				{
					m_wndMenuMover.DeleteAllMenu();
					CWorld* pWorld	= g_WorldMng();

					for (CLandscape * pLand : pWorld->GetVisibleLands())
					{
						for (CObj * pObj : pLand->m_apObjects[OT_MOVER].ValidObjs()) {
							CMover * mover = static_cast<CMover *>(pObj);
							CWndButton * pWndButton = m_wndMenuMover.AddButton(mover->GetId(), mover->GetName(TRUE));
							pWndButton->m_shortcut.m_dwShortcut = ShortcutType::Object;
						}
					}

					m_wndMenuMover.Move( CPoint( rect.left - m_wndMenuMover.GetWindowRect().Width(), rect.top ) );
					m_wndMenuMover.SetVisible( !m_wndMenuMover.IsVisible() );
					m_wndMenuMover.SetFocus();
				}
				break;
			case 100005: // zoom in
				m_nSizeCnt = 1;
				m_size.cx += 32;
				m_size.cy += 32;
				m_size.cx = std::min(m_size.cx, 352l);
				m_size.cy = std::min(m_size.cy, 352l);

				ResizeMiniMap();
				break;
			case 100006: // zoom out 
				m_nSizeCnt = -1;
				m_size.cx -= 32;
				m_size.cy -= 32;
				m_size.cx = std::max(m_size.cx, 128l);
				m_size.cy = std::max(m_size.cy, 128l);
				ResizeMiniMap();
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndNavigator::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL ) 
		return;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		CRect rect = GetClientRect();
		CPoint pt = point;
		
		pt.x -= ( rect.Width() / 2 );
		pt.y -= ( rect.Height() / 2 );
		
		FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );//(FLOAT)m_size.cx / (FLOAT)MINIMAP_SIZE;// * 2;
		FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );//(FLOAT)m_size.cy / (FLOAT)MINIMAP_SIZE;// * 2;
		
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		vPos.x += ( pt.x / fx );
		vPos.z -= ( pt.y / fy );

		CString string;
		string.Format( "/teleport %d %f %f", g_WorldMng()->m_dwWorldID, vPos.x, vPos.z );
		g_DPlay.SendChat( string );
	}
}

BOOL CWndNavigator::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	if( pWndBase == &m_wndMenuMover )
	{
		if( nID >= 0 && nID < 100000 )
		{
			CMover* pMover = prj.GetMover( nID );
			g_WorldMng()->SetObjFocus( pMover );
			SetFocus();
			g_WndMng.m_pWndWorld->m_vDestinationArrow = pMover->m_vPos;
		}
	}
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndNavigator::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 6, 16 * 7 );
	AdjustMaxRect( &rectWnd, 16 * 12, 16 * 13 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndNavigator::OnSize(UINT nType, int cx, int cy)
{
	m_wndPlace.Move( CPoint( 0, 0 ) );
	m_wndZoomIn.Move( CPoint(0, cy - 32 ) );
	m_wndZoomOut.Move( CPoint(0, cy - 16 ) );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndNavigator::OnRButtonDown(UINT nFlags, CPoint point)
{				
	CWorld* pWorld = g_WorldMng();
	pWorld->SetObjFocus(  NULL );
	g_pPlayer->ClearDest();

	m_wndMenuMover.DeleteAllMenu();

	bool hasTarget = false;
	for (CLandscape * pLand : pWorld->GetVisibleLands())
	{
		for (CObj * pObj : pLand->m_apObjects[OT_MOVER].ValidObjs()) {
			CMover * pMover = ( CMover* )pObj;
			if( !pMover->IsPlayer( ) && pMover->GetCharacter( ) )		//NPC�ΰ��? 
			{
				CWndButton * pWndButton = m_wndMenuMover.AddButton(pMover->GetId(), pMover->GetName(TRUE));
				pWndButton->m_shortcut.m_dwShortcut = ShortcutType::Object;
				hasTarget = true;
			}
		}
	}

	if(hasTarget)
	{
		CRect rect = GetWindowRect( TRUE );
		CRect rectRootLayout = g_WndMng.GetLayoutRect();
		int nMenuMoverLeft = rect.left - m_wndMenuMover.GetWindowRect().Width();
		if( nMenuMoverLeft < rectRootLayout.left )
			m_wndMenuMover.Move( CPoint( rect.right, rect.top ) );
		else
			m_wndMenuMover.Move( CPoint( nMenuMoverLeft, rect.top ) );
		m_wndMenuMover.SetVisible( !m_wndMenuMover.IsVisible() );
		m_wndMenuMover.SetFocus();
	}
}

void CWndNavigator::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndNavigator::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetClientRect();
	D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0 , 0 ) );
	// 128 : m_texture.m_size.cx = 1 : x
	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	CWorld* pWorld	= g_WorldMng();

	g_pPlayer->m_nvPoint.Pos.x = vCenter.x + ( (float)( point.x - (rect.right / 2) ) / fx );
	g_pPlayer->m_nvPoint.Pos.z = vCenter.z - ( (float)( point.y - (rect.bottom / 2) ) / fy );
	//	�� �ð��� �׺�����Ϳ�? ���� �ִ� �ð����� �������̳� ���Ϸ� ������ ������������ �켱 �ϵ� �ڵ��̴�
	g_pPlayer->m_nvPoint.Time = 200;	
	g_pPlayer->m_nvPoint.objid = g_pPlayer->GetId();
	g_pPlayer->m_nvPoint.Name = g_pPlayer->GetName(TRUE);
	if( pWorld )
	{
		CObj* pObj	= pWorld->GetObjFocus();
		if( IsValidObj( pObj ) && pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() )
		{
				g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, ( (CCtrl*)pObj )->GetId() );
		}
		else
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, NULL_ID );
		}
	}

	vCenter.x *= fx;
	vCenter.z *= fy;
	int xCenter = (int)( vCenter.x );
	int yCenter = (int)( - vCenter.z );
	
	for (CLandscape * pLand : pWorld->GetVisibleLands())
	{
		for (CObj * pObj : pLand->m_apObjects[OT_MOVER].ValidObjs()) {
			if( pObj->GetType() == OT_MOVER )
			{
				CMover * pMover = (CMover *)pObj;
				if( pMover->IsMode( TRANSPARENT_MODE ) == FALSE )
				{
					vPos = pObj->GetPos();
					vPos.x *= fx;
					vPos.z *= fy;
					int x = (int)( ( rect.Width() / 2 ) + vPos.x );
					int y = (int)( ( rect.Height() / 2 ) - vPos.z );
					x -= xCenter + 2;
					y -= yCenter + 2;
					CRect rectHittest( x, y, x + 5, y + 5);
					if( rectHittest.PtInRect( point ) )
					{
						if(g_pPlayer != pObj )
							pWorld->SetObjFocus( pObj );
						return;
					}
				}
			}
		}
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ������ 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndStatus::CWndStatus()
{
	m_nDisplay = 2;
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;

	m_pVBHPGauge = NULL;
	m_pVBMPGauge = NULL;
	m_pVBFPGauge = NULL;
	m_pVBEXPGauge = NULL;

	m_bVBHPGauge = TRUE;
	m_bVBMPGauge = TRUE;
	m_bVBFPGauge = TRUE;
	
	m_bVBAEXPGauge = TRUE;
	m_pVBAEXPGauge = NULL;

	m_bHPVisible = TRUE;
	m_bExpVisible = TRUE;
}

CWndStatus::~CWndStatus()
{
	DeleteDeviceObjects();
}
void CWndStatus::MakeGaugeVertex()
{
	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpMP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpFP   = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpPxp  = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );
	CMover* pMover = g_pPlayer;

	if( pMover )
	{
		CRect rect = GetClientRect();
		CRect rectTemp;
		
		//	pMover->m_nHitPoint = 50;
		int nWidthClient = lpHP->rect.Width();
		//100 : rect = hp : x
		int nWidth;

		// HP
		nWidth = nWidthClient * pMover->GetHitPoint() / pMover->GetMaxHitPoint();
		if( m_nHPWidth != nWidth ) 
		{
			m_nHPWidth = nWidth;
			rect = lpHP->rect;//.SetRect( 105, 6, 105 + nWidthClient, 6 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBHPGauge = m_Theme.MakeGaugeVertex( &rectTemp, 0x64ff0000, m_pVBHPGauge, &m_texGauFillNormal );
		}
		//else m_bVBHPGauge = TRUE;
		// MP
		nWidth	= nWidthClient * pMover->GetManaPoint() / pMover->GetMaxManaPoint();
		if( m_nMPWidth != nWidth ) 
		{
			m_nMPWidth = nWidth;
			rect = lpMP->rect;//rect.SetRect( 105, 20, 105 + nWidthClient, 20 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBMPGauge = m_Theme.MakeGaugeVertex( &rectTemp, 0x640000ff, m_pVBMPGauge, &m_texGauFillNormal );
		}
		//else m_bVBMPGauge = TRUE;
		// FP
		nWidth	= nWidthClient * pMover->GetFatiguePoint() / pMover->GetMaxFatiguePoint();
		if( m_nFPWidth != nWidth ) 
		{
			m_nFPWidth = nWidth;
			rect = lpFP->rect;//rect.SetRect( 105, 32, 105 + nWidthClient, 32 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBFPGauge = m_Theme.MakeGaugeVertex( &rectTemp, 0x6400ff00, m_pVBFPGauge, &m_texGauFillNormal );
		}
	//lse m_bVBFPGauge = TRUE;
		//nWidth	= ( ( hyper ) nWidthClient * pMover->GetExp1() ) / pMover->GetMaxExp1();
		//nWidth	= ( ( hyper ) nWidthClient * pMover->GetExp1() ) / pMover->GetMaxExp1();
		nWidth	= (int)( ( (float) nWidthClient / pMover->GetMaxExp1() ) * pMover->GetExp1() );

		if( m_nEXPWidth != nWidth ) 
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;//rect.SetRect( 105, 48, 105 + nWidthClient, 48 + 8 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBEXPGauge = m_Theme.MakeGaugeVertex( &rectTemp, 0x847070ff, m_pVBEXPGauge, &m_texGauFillSmall );
		}

		if( m_nPXPWidth != GuildHouse->m_nExtraExp )
		{
			m_nPXPWidth = GuildHouse->m_nExtraExp;
			rect = lpPxp->rect;
			rectTemp = rect;
			
			if( 0 < m_nPXPWidth && m_nPXPWidth < 9 )
				m_nPXPWidth = 9;
			rectTemp.right = rectTemp.left + m_nPXPWidth;
			ClientToScreen( rectTemp );
			m_bVBAEXPGauge = m_Theme.MakeGaugeVertex( &rectTemp, 0x84e6ce19, m_pVBAEXPGauge, &m_texGauFillSmall );
		}
	}
}
void CWndStatus::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	if( m_pTexture && g_pPlayer )
	{
		RenderWnd();
		// �����? Ÿ��Ʋ ���� �ؽ�Ʈ�� ����ϴ�? �� 
		if( IsWndStyle( WBS_CAPTION ) )	
		{
			int y = 4;
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
			p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

//			CHAR szNum[32];
//			itoa( g_pPlayer->GetLevel(), szNum, 10 );
			
//			CHAR szLevel[32];
//			strcpy( szLevel, " Lv. " );
//			strcat( szLevel, szNum );
//			p2DRender->TextOut( 57, y, szLevel, 0xffffffd0 );

			char szNameLevel[128] = {0,};
				sprintf( szNameLevel, prj.GetText( TID_GAME_WND_STATUS_PLAYER_INFORMATION ), g_pPlayer->GetName(), g_pPlayer->GetLevel() );
			SetTitle( szNameLevel );
			
			p2DRender->SetFont( pOldFont );
		}				
	}
	else
	{
		m_Theme.RenderWndBaseFrame( p2DRender, &rect );
		if( IsWndStyle( WBS_CAPTION ) )
		{
			// Ÿ��Ʋ �� 
			rect.bottom = 21;
			{
				m_Theme.RenderWndBaseTitleBar( p2DRender, &rect, m_strTitle, m_dwColor );
			}
		}
	}
}

void CWndStatus::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = g_pPlayer;
	if( pMover == NULL )
		return;

	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;// - 30;

	CRect rectTemp;
	//SetTitle( "" );
	/////////////////////////////////////

	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpMP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpFP   = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );

	LPWNDCTRL lpAExp = GetWndCtrl( WIDC_CUSTOM5 );
	
	if( pMover )
	{
		MakeGaugeVertex();

		if( m_bVBHPGauge )
		{
			m_bHPVisible = TRUE;

			if( m_bHPVisible )
				m_Theme.RenderGauge( m_pVBHPGauge, &m_texGauFillNormal );
		}
		if( m_bVBMPGauge )
			m_Theme.RenderGauge( m_pVBMPGauge, &m_texGauFillNormal );
		if( m_bVBFPGauge )
			m_Theme.RenderGauge( m_pVBFPGauge, &m_texGauFillNormal );
		if( g_pPlayer->IsAfterDeath() )
			m_bExpVisible = !m_bExpVisible;
		else
			m_bExpVisible = TRUE;

		if( m_bVBEXPGauge )
		{
			m_Theme.RenderGauge( m_pVBEXPGauge, &m_texGauFillSmall );
		}
		if( m_bExpVisible )
		{
/*
#if __VER < 8 // #ifndef __SKILL_WITHOUT_EXP
			if( m_bVBPXPGauge )
			{
				m_Theme.RenderGauge( m_pVBPXPGauge, &m_texGauFillSmall );

			}
#endif // __VER < 8
*/
		}

		if( m_bVBAEXPGauge )
			m_Theme.RenderGauge( m_pVBAEXPGauge, &m_texGauFillSmall );
		


/*
		//p2DRender->TextOut( 10, 65, 1, 1, "Lv. : ", 0xff101010 );
		CHAR szNum[32];
		itoa( g_pPlayer->GetLevel(), szNum, 10 );
		//p2DRender->TextOut( 35, 65, 1, 1, szNum, 0xff606060 );

		CHAR szLevel[32];
		strcpy( szLevel, "Status - Lv. " );
		strcat( szLevel, szNum );

		m_strTitle = szLevel;
*/
	}		
	if( m_nDisplay != 0 )
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255 );
		//p2DRender->SetFont( m_Theme.m_pFontWndTitle );

		char cbufHp[16] = {0,};
		char cbufMp[16] = {0,};
		char cbufFp[16] = {0,};
		char cbufExp[16] = {0,};
		char cbufPxp[16] = {0,};
		
		int nCharHP, nCharMP, nCharFP, nCharEXP; //, nCharPXP;
		CSize size = p2DRender->m_pFont->GetTextExtent("8");
		int nMaxHeight = p2DRender->m_pFont->GetMaxHeight();
		int nTopGap = 0;

		if( ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_GER )
			nTopGap = 2;
		else
		{
			if(nMaxHeight > 14)
				nTopGap = nMaxHeight - 14;
		}

		if(m_nDisplay == 1)
		{
			wsprintf(cbufHp, "%d%%", pMover->GetHitPointPercent());
			wsprintf(cbufMp, "%d%%", pMover->GetManaPointPercent());
			wsprintf(cbufFp, "%d%%", pMover->GetFatiguePointPercent());

			p2DRender->TextOut( lpHP->rect.right - 90, lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - 90, lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - 90, lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );

			nCharHP = wsprintf(cbufHp, prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetFatiguePoint());

			int x = lpHP->rect.right-17; // -40
			p2DRender->TextOut( x - (int)(nCharHP*5.8f) , lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharMP*5.8f), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharFP*5.8f), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
		}
		else
		{
			nCharHP = wsprintf(cbufHp, "%d", pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetFatiguePoint());

			int x = lpHP->rect.right - 82;
			p2DRender->TextOut( x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
			
			nCharHP = wsprintf(cbufHp, "%d", pMover->GetMaxHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetMaxManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetMaxFatiguePoint());
			
			x = lpHP->rect.right - 30;
			p2DRender->TextOut( x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
				
			LPWNDCTRL lpHP = GetWndCtrl( WIDC_CUSTOM1 );
			int nWidthCustom = lpHP->rect.Width();
			int nGap = (int)( ((nWidthCustom / 2.0f) + (size.cx / 2.0f)) );

			p2DRender->TextOut( lpHP->rect.right - nGap, lpHP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - nGap, lpMP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - nGap, lpFP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
		}

		const double fExp = pMover->GetExpPercent() / 100.0;
		if( fExp >= 99.99 )
			nCharEXP = sprintf( cbufExp, "99.99%%" );
		else
			nCharEXP = sprintf( cbufExp, "%.2lf%%", fExp );

		int x = lpHP->rect.right-7; // -40
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );

		nCharEXP = sprintf( cbufExp, "%d%%", GuildHouse->m_nExtraExp );
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpAExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );
	}

	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	ResetRenderState();

	// ����Ʈ ���� 
	D3DVIEWPORT9 viewport = BuildViewport(p2DRender, lpFace);

	pd3dDevice->SetViewport(&viewport);

	// �������� 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
/*
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/
	FLOAT fov = D3DX_PI/4.0f;//796.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX  matView;

	// ���� 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	// �ʱ�ȭ 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	{	
		D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
		D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
		matView = D3DXR::LookAtLH010(vecPos, vecLookAt);
		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

		D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
		D3DXMatrixTranslation(&matTrans,0.0f,-7.8f,0.0f);

		D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
	}
	
	matWorld = matWorld * matScale * matRot1 * matTrans;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// ������ 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;
	if( g_pBipedMesh )
	{
		//m_pModel->FrameMove();
		g_pBipedMesh->SetGroup( 0 );
		::SetTransformView( matView );
		::SetTransformProj( matProj );

		O3D_ELEMENT* pElem = g_pBipedMesh->GetParts( PARTS_HAIR );
		
		if( pElem && pElem->m_pObject3D && g_pPlayer )
		{
			pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR; // / (CWorld::m_light.Diffuse.r*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG; // / (CWorld::m_light.Diffuse.g*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB; // / (CWorld::m_light.Diffuse.b*g_fHairLight);
		}
//#ifdef __VCRITICAL
		if( g_pPlayer->IsDie() )
		{
			D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
			::SetLight( TRUE );
			SetDiffuse( 1.0f, 0.3f, 0.3f );
#ifdef __YENV
			SetAmbient( 0.9f, 0.9f, 0.9f );
#else //__YENV
			SetAmbient( 0.3f, 0.3f, 0.3f );
#endif //__YENV
			SetLightVec( vDir );		}
		else
		{	
			{	
#ifdef __YENV
				D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
				SetLightVec( vDir );
				SetDiffuse( 1.0f, 1.0f, 1.0f );
#else //__YENV
				SetDiffuse( 0.0f, 0.0f, 0.0f );
#endif //__YENV
				
				SetAmbient( 1.0f, 1.0f, 1.0f );
			}			
		}
//#else
//		SetDiffuse( 0.0f, 0.0f, 0.0f );
//		SetAmbient( 1.0f, 1.0f, 1.0f );
//#endif
#ifdef __YENV
		if( g_pPlayer->m_pModel )
		{
			O3D_ELEMENT *pElem = ((CModelObject*)g_pPlayer->m_pModel)->GetParts( PARTS_HAIR );
			
			if( pElem )
			{
				if( pElem->m_pObject3D )
				{
					pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
					pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
					pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB;
				}
			}
		}	
#endif //__YENV

	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(g_pBipedMesh);
		
		// �����? �Ӹ�ī�� �������ϴ°��̳�?  // �κ��� �ִ� ���? 
		O3D_ELEMENT* pElement   = NULL;
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );

		if( pItemElem )
		{
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			// �����ǻ��� �Ծ������? �Ӹ��������ΰ��� ������ ���� ���ڸ� �������� �ٲ۴�
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							g_pBipedMesh->SetEffect(PARTS_HAIR, XE_HIDE );
						
						g_pBipedMesh->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
				else
				{
					//g_pBipedMesh->SetEffectOff(PARTS_HAIR, XE_HIDE );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					
					g_pBipedMesh->SetEffectOff(PARTS_HEAD, XE_HIDE );					
				}
			}
						
			
		}
		else
		{
			// �����ǻ��� �Ծ������? �Ӹ��������ΰ��� ������ ���� ���ڸ� �������� �ٲ۴�
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{					
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							g_pBipedMesh->SetEffect(PARTS_HAIR, XE_HIDE );
						
						g_pBipedMesh->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}
							
		}

//gmpbigsun : ���� ���̽� ��������Ʈ
	::SetLight( FALSE );
 	::SetFog( FALSE );
 	SetDiffuse( 1.0f, 1.0f, 1.0f );
 	SetAmbient( 1.0f, 1.0f, 1.0f );

		g_pBipedMesh->Render(&matWorld);

		SetDiffuse( 0.0f, 0.0f, 0.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );
	}

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
}
HRESULT CWndStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBHPGauge == NULL )
	{
		m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL );
		m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBMPGauge, NULL );
		m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBFPGauge, NULL );
		m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL );
		m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBAEXPGauge, NULL );
		m_nHPWidth = -1;
		m_nMPWidth = -1;
		m_nFPWidth = -1;
		m_nEXPWidth = -1;
		m_nPXPWidth = -1;
	}
		
	return S_OK;
}
HRESULT CWndStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBHPGauge );
    SAFE_RELEASE( m_pVBMPGauge );
    SAFE_RELEASE( m_pVBFPGauge );
    SAFE_RELEASE( m_pVBEXPGauge );
	SAFE_RELEASE( m_pVBAEXPGauge );
		
	return S_OK;//return S_OK;
}
HRESULT CWndStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CWndStatus::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();

	m_texGauEmptyNormal.LoadTexture( MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauEmptySmall.LoadTexture( MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillSmall.LoadTexture( MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	
	//m_texGauEmptySmall  
	//m_texGauFillNormal  
	//m_texGauFillSmall   
	
	// ����, �������� ���� ĳ���� ������Ʈ ���� 
	if( g_pBipedMesh == NULL )
	{
		int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		g_pBipedMesh = (CModelObject*)prj.m_modelMng.LoadModel( OT_MOVER, nMover, TRUE );
		g_pBipedMesh->LoadMotionId(MTI_STAND);
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_skin, g_pPlayer->m_aEquipInfo, g_pBipedMesh, &g_pPlayer->m_Inventory );
	}
	CRect rectRoot = g_WndMng.GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top );
	Move( point );

}

BOOL CWndStatus::Initialize(CWndBase* pWndParent)
{
	return InitDialog( APP_STATUS1 );
}

BOOL CWndStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 100: // wnd2
			/*
			g_Music.m_nMusicVolume = !g_Music.m_nMusicVolume;
			if ( g_Music.m_nMusicVolume == 0 )
				g_Music.Stop();
			else
				g_Music.Play();
				*/
			break;
		case 101: // wnd1 
			g_SoundMng.m_nSoundVolume = !g_SoundMng.m_nSoundVolume;
			break;
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndStatus::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndStatus::SetWndRect( CRect rectWnd, BOOL bOnSize  )
{
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;
	
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndStatus::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndStatus::OnLButtonDown(UINT nFlags, CPoint point)
{	
	if(++m_nDisplay > 2)
		m_nDisplay = 0;
}
BOOL CWndStatus::OnEraseBkgnd(C2DRender* p2DRender)
{
	return CWndBase::OnEraseBkgnd( p2DRender );
	//oint pt = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	CRect rect = GetClientRect();
	D3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );


	//p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 255, 70, 70, 170 ) );
	p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 100, 0, 0, 0 ) );

	return TRUE;
}

void CWndStatus::OnMouseWndSurface( CPoint point )
{
	static LPWNDCTRL lpPxp  = GetWndCtrl( WIDC_CUSTOM5 );
	BOOL bInPoint = FALSE;
	if( lpPxp )
		bInPoint = PtInRect( &lpPxp->rect, point );
	else return;

	if( bInPoint )	
	{
		CRect DrawRect = lpPxp->rect;
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
		g_toolTip.PutToolTip( 100000, GETTEXT( TID_TOOLTIP_RESTPOINT_GAGE ), DrawRect, point2 );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����Ʈ ������ ����
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWndQuestItemInfo::Initialize( CWndBase* pWndParent, CItemElem * pItemBase )
{
	return Create( pItemBase, APP_QUESTITEM_INFO, pWndParent );
}
BOOL CWndQuestItemInfo::Create(CItemElem * pItemBase, UINT nID, CWndBase* pWndParent )
{
	m_pItemBase = pItemBase;
	CWndNeuz::InitDialog( nID, pWndParent, 0, m_Position );
	GetDlgItem( WIDC_EDIT1 )->SetVisible( FALSE );

	MoveParentCenter();
	return 1;
}

void CWndQuestItemInfo::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_EDIT1 );
	CRect rectCtrl = lpWndCtrl->rect;
	rectCtrl.top += 10;
	rectCtrl.left += 10;
	rectCtrl.right -= 10;
	rectCtrl.bottom -= 10;

	const ItemProp * itemProp = m_pItemBase->GetProp();

	m_pItemBase->GetTexture()->Render(p2DRender, rectCtrl.TopLeft(), 200);
	p2DRender->TextOut(rectCtrl.left + 34, rectCtrl.top + 16 - 5, 1, 1, itemProp->szName, 0xff0000ff);

	CEditString	string = "";
	string.Init( m_pFont, &rectCtrl );
	string = itemProp->szCommand;
	p2DRender->TextOut_EditString( rectCtrl.left , rectCtrl.top + 40/* '+40'�� �ǹ̴� �������ִ� ������ ���� �Ʒ��� ��ڴٴ�? �ǹ� */, string);
}

void CWndQuestItemInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	if (CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1)) {
		pWndEdit->EnableWindow(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndLogOut : ���� ȭ������, ĳ���� ���� ȭ������ 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CWndLogOut::Initialize(CWndBase* pWndParent)
{
	CRect rect = g_WndMng.MakeCenterRect( 250, 130 );

	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0069) ) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS����϶�?
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}

	// ������ �������� ������ �ϱ�
	Move70();

	return TRUE;
}
BOOL CWndLogOut::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:    
				if( ::GetLanguage() == LANG_JAP )
				{
					if( g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}

				g_Neuz.ResetStaticValues( );

				if( g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		

				if( g_pPlayer->IsRegionAttr( RA_SAFETY ) || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
						g_WndMng.m_pLogOutWaitting->Initialize();
						g_WndMng.m_pLogOutWaitting->SetIsLogOut(TRUE);
						SetVisible(FALSE);
					}
				}

				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


BOOL CWndLogOutWaitting::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_MESSAGE_TEXT );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_MESSAGE_TEXT ); 
} 


BOOL CWndLogOutWaitting::Process()
{
	DWORD dwCurrentTime = g_tmCurrent;
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	DWORD dwEndWaitTime = dwTime - dwCurrentTime;
	dwEndWaitTime /= 1000;

	CString str;
	str.Format( prj.GetText( TID_PK_TIME_END ), dwEndWaitTime );
	
	if( dwEndWaitTime > 0 )
	{
		pWndText->SetString( str );
	}
	
	if( dwCurrentTime > dwTime )
	{
		if( m_bIsLogOut )
		{
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
		}
		else
		{
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
		}

		Destroy(); 
	}
	return TRUE;
}

void CWndLogOutWaitting::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	m_wndTitleBar.SetVisible( FALSE );
	
	m_bIsLogOut = FALSE;
	
	dwTime = g_tmCurrent+SEC(TIMEWAIT_CLOSE);

	// ������ �������� ������ �ϱ�
	Move70();

	// ���� ��Ŷ ���� ����
	g_DPlay.SendReqLeave();	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndQuit  : ���α׷� ����
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CWndQuit::Initialize( CWndBase* pWndParent )
{
	CRect rect = g_WndMng.MakeCenterRect( 250, 130 );
	/*
	Create( _T( "�Ž��� �ڽ�" ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( "���α׷��� �����Ͻðڽ��ϱ�?" ) );
	*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0070) ) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS����϶�?
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}

	// ������ �������� ������ �ϱ�
	Move70();

	m_bFlag = FALSE;
	
	return TRUE;
}
BOOL CWndQuit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:       
				if( m_bFlag )
					return TRUE;

				m_bFlag = TRUE;

				if( ::GetLanguage() == LANG_JAP )
				{
					if( g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}

				if( g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		
				
				if( g_pPlayer->IsRegionAttr( RA_SAFETY ))
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
#ifdef __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->Initialize();	// ˬ
#else	// __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->Initialize( this );	// ˬ
#endif	// __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->SetIsLogOut(FALSE);
						SetVisible(FALSE);
					}
				}

				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndRevival : ��Ȱ 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndRevival::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __JEFF_11_4
	BOOL	bArena	= g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena(); 
#endif	// __JEFF_11_4
	if( NULL == g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION )
#ifdef __JEFF_11_4
		|| bArena
#endif	// __JEFF_11_4
		)
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );
} 
void CWndRevival::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pLodeLight = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODELIGHT );
	m_pLodeStar = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODESTAR );
	m_pRevival = (CWndButton*)GetDlgItem( WIDC_REVIVAL_STAND );
	m_pShop = (CWndButton*)GetDlgItem( WIDC_REVIVAL_SHOP );
	m_pLodeLight->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODELIGHT) );//"�ε�����?�� �����ڰ� ���� ������ ��Ȱ ��ġ�Դϴ�." );
	m_pLodeStar->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODESTAR) );//"�ε彺Ÿ�� �α� ������ ��Ȱ ��ġ�Դϴ�." );
	m_pRevival->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_OGNPOINT) );
	m_pShop->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_SHOP) );

#ifdef __JEFF_11_4
	BOOL	bArena	= g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena(); 
#endif	// __JEFF_11_4
	if( NULL == g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION )
#ifdef __JEFF_11_4
		|| bArena
#endif	// __JEFF_11_4
		)
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );
	
	CWndWebBox* pWndWebBox = (CWndWebBox*)g_WndMng.GetApplet( APP_WEBBOX );

	if(pWndWebBox)
		pWndWebBox->Destroy();

#ifdef __NEW_WEB_BOX
	CWndHelperWebBox* pWndHelperWebBox = ( CWndHelperWebBox* )g_WndMng.GetApplet( APP_WEBBOX2 );

	if( pWndHelperWebBox )
		pWndHelperWebBox->Destroy();
#else // __NEW_WEB_BOX
	CWndWebBox2* pWndWebBox2 = (CWndWebBox2*)g_WndMng.GetApplet( APP_WEBBOX2 );

	if(pWndWebBox2)
		pWndWebBox2->Destroy();
#endif // __NEW_WEB_BOX

	m_wndTitleBar.SetVisible( FALSE );

	if( m_pLodeLight )
	{
		CRect RevivalRect = m_pRevival->GetWindowRect(TRUE);
		CRect LodeLightRect = m_pLodeLight->GetWindowRect(TRUE);
		m_pRevival->SetWndRect( LodeLightRect, TRUE );
		m_pShop->SetWndRect( RevivalRect, TRUE );

		m_pLodeLight->EnableWindow( FALSE );
		m_pLodeLight->SetVisible( FALSE );		

//		crect = m_pLodeStar->GetWindowRect(TRUE);
//		crect.top += 2;
//		crect.bottom += 2;
//		m_pLodeStar->SetWndRect( crect, TRUE );
//		crect = m_pRevival->GetWindowRect(TRUE);
//		crect.top += 8;
//		crect.bottom += 8;
//		m_pRevival->SetWndRect( crect, TRUE );
		
//		crect = GetWndRect();
//		crect.bottom -= 31;
//		SetWndRect( crect, TRUE );
	}

	Move70();
} 
BOOL CWndRevival::Initialize( CWndBase* pWndParent ) 
{ 
	InitDialog( APP_REVIVAL, pWndParent, WBS_MODAL );

	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS����϶�?
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	
	return TRUE;
} 

void CWndRevival::EnableButton( int nButton, BOOL bEnable )
{
	switch( nButton )
	{
	case WIDC_REVIVAL_TO_LODELIGHT:
		{
			m_pLodeLight->EnableWindow(bEnable);
			break;
		}
	case WIDC_REVIVAL_TO_LODESTAR:
		{
			m_pLodeStar->EnableWindow(bEnable);
			break;
		}
	}
}

BOOL CWndRevival::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bClose = TRUE;
	switch( nID )
	{
		case WIDC_REVIVAL_TO_LODELIGHT:
			{
				g_DPlay.SendPacket<PACKETTYPE_REVIVAL_TO_LODELIGHT>();
				
				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );

				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
		case WIDC_REVIVAL_TO_LODESTAR:
			{
				g_DPlay.SendPacket<PACKETTYPE_REVIVAL_TO_LODESTAR>();

				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );
				
				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
		case WIDC_REVIVAL_STAND:
			{
				g_DPlay.SendPacket<PACKETTYPE_REVIVAL>();

				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );
				
				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
		case WIDC_REVIVAL_SHOP:
			{
				bClose = FALSE;
				g_WndMng.OpenApplet( APP_WEBBOX );
				break;
			}
	}	
	if( bClose )
		Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}



///////////////////////////////////////////////////////////////////////////
// CWantedMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWantedMessageBox : public CWndCustomMessageBox
{ 
public: 
	int m_nGold;
	CString m_strMsg;

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWantedMessageBox::Initialize( CWndBase* pWndParent )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_PK_INPUT_TIMEWARN), 
	                                   pWndParent, 
									   MB_OKCANCEL );

}

BOOL CWantedMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
 	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendWantedGold( m_nGold, m_strMsg );
			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////
// CWndReWanted
//////////////////////////////////////////////////////////////////////////////
void CWndReWanted::SetWantedName( LPCTSTR szWanted )
{
	m_strWanted = szWanted;
}

void CWndReWanted::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetFocus();	

	GetDlgItem( WIDC_STATIC_TARGETNAME )->SetTitle( m_strWanted );

	MoveParentCenter();	// ������ �߾����� �ű��? �κ�.
} 

BOOL CWndReWanted::Initialize( CWndBase* ) 
{ 
	InitDialog( APP_REWARD_INPUT );
	return TRUE;
} 

BOOL CWndReWanted::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 || message == EN_RETURN )
	{
		CWndEdit* pWndGold = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CWndEdit* pWndMsg  = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		if( pWndGold && pWndMsg && g_pPlayer )
		{
			if( strlen(pWndGold->GetString()) <= 0 )
				return FALSE;
			CString strMsg = pWndMsg->GetString();
			int nGold = atoi( pWndGold->GetString() ); 
			
			g_WndMng.WordChange( strMsg );

			if( CheckWantedInfo( nGold, strMsg ) == TRUE )
			{
				CWantedMessageBox* pBox = new CWantedMessageBox;
				pBox->m_nGold = nGold;
				pBox->m_strMsg = strMsg;
				g_WndMng.OpenCustomBox( pBox );
				Destroy();
			}
			else
				return FALSE;
		}

		Destroy();
	}
	else if( nID == WIDC_BUTTON2 )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndReWanted::CheckWantedInfo( int nGold, LPCTSTR szMsg )
{
	if( szMsg[0] == '\0' )
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_PK_REPU_INPUT) );
		return FALSE;
	}

	if( strlen(szMsg) > WANTED_MSG_MAX )
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GUILD_NOTICE_ERROR) );
		return FALSE;
	}

	if( nGold < MIN_INPUT_REWARD || nGold > MAX_INPUT_REWARD )			// �������? �ּ� 1000�гĿ��� �ִ� 2�� �гı��� �� �� �ִ�. 
	{
		char szWarning[256];
		wsprintf( szWarning, prj.GetText(TID_PK_MONEY_RANGE), MIN_INPUT_REWARD, MAX_INPUT_REWARD );		
		g_WndMng.OpenMessageBoxUpper( szWarning );
		return FALSE;
	}

	int nTax = MulDiv( nGold, 10, 100 );					// �� �������? 10%�� �������? ���޵ȴ�. 
	if( g_pPlayer->GetGold() < (nGold + nTax) ) 
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LACKMONEY) );	// �κ��� ���̺���
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// CWndWantedConfirm
////////////////////////////////////////////////////////////////////////////////////////

void CWndWantedConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

BOOL CWndWantedConfirm::Initialize(CWndBase *) {
	InitDialog(APP_WANTED_CONFIRM, nullptr, WBS_MODAL);
	return TRUE;
}

void CWndWantedConfirm::SetInfo(const char szName[], const int nGold)
{
	m_nGold  = nGold;
	_tcscpy( m_szName, szName );
	
	CWndStatic* pWndStatic = GetDlgItem<CWndStatic>( WIDC_STATIC1 );
	pWndStatic->SetTitle(szName);

	pWndStatic = GetDlgItem<CWndStatic>(WIDC_STATIC2);

	CString strMsg = prj.GetText(TID_PK_POINT_SHOW);
	strMsg.AppendFormat(prj.GetText(TID_PK_WASTE_SHOW), REQ_WANTED_GOLD);

	pWndStatic->SetTitle(strMsg);	
}

BOOL CWndWantedConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_BUTTON1:
			if (strlen(m_szName) > 0) {
				g_DPlay.SendWantedInfo(m_szName);
			}
			Destroy();
			break;
		case WIDC_BUTTON2:
			Destroy();
			break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////////////////////////
// CWndWanted
////////////////////////////////////////////////////////////////////////////////////////

const int MAX_WANTED_PER_PAGE = 20;

CWndWanted::CWndWanted(const time_t lTime) {
	m_recvTime = lTime;
}

CWndWanted::~CWndWanted() {
	SAFE_DELETE(m_pWantedConfirm);
}

void CWndWanted::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	m_wndScrollBar.SetScrollFromSize(static_cast<int>(m_aList.size()), MAX_WANTED_PER_PAGE);
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	CString strTitle = GetTitle();
	CTime tm( m_recvTime );
	strTitle += tm.Format( prj.GetText(TID_PK_DATA_SHOWTIME) );
	SetTitle( strTitle );

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 

BOOL CWndWanted::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_WANTED, pWndParent, 0, CPoint( 0, 0 ) );
} 


// ���õ� �ε����� ��´�?.
int CWndWanted::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_WANTED_PER_PAGE ) // 0 - 19���� 
	{
		int nSelect = nBase + nIndex;
		if( nSelect >= 0 && std::cmp_less(nSelect, m_aList.size()))
			return nSelect;
	}
	return -1;
}

void CWndWanted::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if (m_aList.empty()) return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndWanted::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	// ���� ����Ŭ���� ��ǥ�� ���õ� �ε����ΰ�?
	if( m_nSelect >= 0 && GetSelectIndex( point ) == m_nSelect )	
	{
		SAFE_DELETE(m_pWantedConfirm);
		m_pWantedConfirm = new CWndWantedConfirm;
		m_pWantedConfirm->Initialize( this );

		// ǥ���� �̸�, ���������? ����
		m_pWantedConfirm->SetInfo( m_aList[m_nSelect].szName, (int)( m_aList[m_nSelect].nGold ) );
	}
}

void CWndWanted::InsertWanted(const WANTED_ENTRY & entry) {
	if (m_aList.size() < m_aList.max_size()) {
		m_aList.emplace_back(entry);
	} else {
		Error("CWndWanted::InsertWanted - range over");
	}
}

WANTEDLIST::WANTEDLIST(const WANTED_ENTRY & entry) {
	_tcscpy(szName, entry.szPlayer);
	nGold = entry.nGold;

	CTime cTime((time_t)entry.nEnd);
	SYSTEMTIME st;
	st.wYear = cTime.GetYear();
	st.wMonth = cTime.GetMonth();
	st.wDay = cTime.GetDay();

	if (GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szDate, sizeof(szDate)) == 0)
		szDate[0] = '\0';

	_tcscpy(szMsg, entry.szMsg);
}

void CWndWanted::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sy;
	char szNum[8], szGold[16];
	CString strGold;

	const int sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+570, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	for( int i=nBase; i<nBase + MAX_WANTED_PER_PAGE; ++i )
	{
		if (std::cmp_greater_equal(i, m_aList.size())) {
			break;
		}
		
		sprintf( szNum, "%3d", i+1 );
		sprintf( szGold, "%I64d", m_aList[i].nGold );
		strGold = GetNumberFormatEx( szGold );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+570, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		p2DRender->TextOut( sx + 4,    sy, szNum,             dwColor );
		p2DRender->TextOut( sx + 33,   sy, m_aList[i].szName, dwColor );
		p2DRender->TextOut( sx + 190,  sy, strGold,           dwColor );
		p2DRender->TextOut( sx + 268,  sy, m_aList[i].szDate, dwColor );
		p2DRender->TextOut( sx + 350,  sy, m_aList[i].szMsg,  dwColor );

		sy += 18;
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////
// CWndResurrectionConfirm
///////////////////////////////////////////////////////////////////////////////////////////
void CWndResurrectionConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	m_wndTitleBar.SetVisible( FALSE );
	Move70();
} 
BOOL CWndResurrectionConfirm::Initialize( CWndBase* pWndParent ) 
{ 
	InitDialog( APP_RESURRECTION, nullptr, WBS_MODAL );
	return TRUE;
} 
BOOL CWndResurrectionConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case  WIDC_BUTTON1:
		{
			g_DPlay.SendResurrectionOK();
			Destroy();
		}
		break;
	case  10000:
	case  WIDC_BUTTON2:
		{
			g_DPlay.SendResurrectionCancel();
			Destroy();
		}
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


/****************************************************
  WndId : APP_COMMITEM_DIALOG - ������ ���?
  CtrlId : WIDC_EDIT_COMMUSE - 
  CtrlId : WIDC_BUTTON_OK - Button
****************************************************/

void CWndCommItemDlg::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	m_pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT_COMMUSE);
	if (m_pWndEdit)  m_pWndEdit->EnableWindow(FALSE);

	MoveParentCenter();
}

void CWndCommItemDlg::SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId )
{
	m_pWndEdit->AddString( prj.GetText( dwDefindText ) );
	m_dwObjId = dwObjId;
	m_dwCtrlId = dwCtrlId;
} 

BOOL CWndCommItemDlg::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_COMMITEM_DIALOG, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndCommItemDlg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK || message == EN_RETURN )
	{
		if (Windows::IsOpen(APP_SHOP_, APP_BANK, APP_TRADE)) {
			g_WndMng.PutString(TID_GAME_TRADELIMITUSING);
		} else if (Windows::IsOpen(APP_REPAIR)) {
			g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING));
		} else {

			if (m_dwObjId == II_SYS_SYS_SCR_CHACLA) {
				CWndChangeClass1::OpenWindow(m_dwCtrlId);
			} else if (m_dwCtrlId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE) {
				g_DPlay.SendPetTamerMistake(m_dwObjId);
			} else {

				CItemElem * pItem = g_pPlayer->GetItemId(m_dwObjId);
				if (pItem) {
					if (pItem->GetProp()->dwItemKind3 == IK3_TICKET)	// +
						g_DPlay.SendDoUseItemInput(MAKELONG(ITYPE_ITEM, m_dwObjId), "0");
					else
						g_DPlay.SendDoUseItem(MAKELONG(ITYPE_ITEM, m_dwObjId), m_dwCtrlId, -1, FALSE);
				}

			}
		}
		Destroy();
	}
	else if( nID == WIDC_BUTTON_CANCEL || nID == WTBID_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_CHANGECLASS_1 - ��������(1��)
CtrlId : WIDC_RADIO_MER - �Ӽ��ʸ�
CtrlId : WIDC_RADIO_ACR - ��ũ�ι�
CtrlId : WIDC_RADIO_MAG - ������
CtrlId : WIDC_RADIO_ASS - ��ý��?
CtrlId : WIDC_STATIC1 - ������ ���ϴ� ������ �����Ͻʽÿ�
CtrlId : WIDC_STATIC2 - ���� ���?
CtrlId : WIDC_BUTTON_OK - Button
CtrlId : WIDC_BUTTON_CANCEL - 
****************************************************/

BOOL CWndChangeClass1::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_BUTTON_OK || message == EN_RETURN) {
		OnSendModifiedJob();
	} else if (nID == WIDC_BUTTON_CANCEL || nID == WTBID_CLOSE) {
		Destroy();
	} else if (nID == WIDC_LISTBOX) {
		OnModifiedJob();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#include <algorithm>

void CWndChangeClass1::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	
	auto & jobList = ReplaceListBox<JobId, JobDisplayer>(WIDC_LISTBOX);

	const auto currentJob = g_pPlayer->GetJob();
	const auto currentJobType = prj.jobs.info[g_pPlayer->GetJob()].dwJobType;

	for (int i = 0; i != MAX_JOB; ++i) {
		if (i == JOB_PUPPETEER || i == JOB_DOPPLER || i == JOB_GATEKEEPER) continue;

		if (!m_usedScroll || currentJobType == prj.jobs.info[i].dwJobType) {
			jobList.Add(i, currentJob != i);
		}
	}

	if (jobList.GetSize() >= 3) {
		const size_t numberOfDisplay = std::clamp<size_t>(jobList.GetSize(), 3lu, 10lu);
		
		CRect jobRect = jobList.GetWndRect();
		const int originalHeight = jobRect.Height();
		const int size = static_cast<int>((numberOfDisplay - 3) * jobList.GetLineHeight()) + originalHeight;
		const int diff = size - originalHeight;

		jobRect.bottom = jobRect.top + size;
		jobList.SetWndRect(jobRect, TRUE);

		CRect thisRect = GetWindowRect(TRUE);
		thisRect.bottom += diff;
		SetWndRect(thisRect);

		for (const UINT buttonId : { WIDC_BUTTON_OK, WIDC_BUTTON_CANCEL }) {
			CWndBase * button = GetDlgItem(buttonId);
			CRect rect = button->GetWndRect();
			button->Move(rect.left, rect.top + diff);
		}
	}

	MoveParentCenter();
}

void CWndChangeClass1::OnSendModifiedJob() {
	if (CWndJobList * list = GetDlgItem<CWndJobList>(WIDC_LISTBOX)) {
		const JobId * picked = list->GetCurSelItem();
		if (picked) {
			g_DPlay.SendPacket<PACKETTYPE_SEND_TO_SERVER_CHANGEJOB, int, std::optional<OBJID>>(*picked, m_usedScroll);
		}
	}
	
	Destroy();
}

void CWndChangeClass1::OnModifiedJob() {
	static constexpr auto ToBOOL = [](bool b) -> BOOL { return b ? TRUE : FALSE; };

	if (CWndJobList * list = GetDlgItem<CWndJobList>(WIDC_LISTBOX)) {
		const JobId * picked = list->GetCurSelItem();
		
		CWndButton * button = GetDlgItem<CWndButton>(WIDC_BUTTON_OK);
		button->EnableWindow(ToBOOL(picked && *picked != g_pPlayer->GetJob()));
	}
}

BOOL CWndChangeClass1::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_CHANGECLASS_1, pWndParent, 0, CPoint(0, 0));
}

void CWndChangeClass1::OpenWindow(std::optional<OBJID> scrollPos) {
	SAFE_DELETE(g_WndMng.m_pWndChangeClass1);
	g_WndMng.m_pWndChangeClass1 = new CWndChangeClass1(scrollPos);
	g_WndMng.m_pWndChangeClass1->Initialize();
}

void CWndChangeClass1::JobDisplayer::Render(
	C2DRender * const p2DRender, CRect rect,
	const CWndChangeClass1::JobId & item, DWORD color, const WndTListBox::DisplayArgs & misc
) const {
	if (prj.jobs.info[item].dwJobType != JTYPE_MASTER && prj.jobs.info[item].dwJobType != JTYPE_HERO) {
		p2DRender->TextOut(rect.left, rect.top, prj.jobs.info[item].szName, color);
	} else {
		TCHAR jobName[64];
		std::strcpy(jobName, prj.jobs.info[item].szName);
		if (prj.jobs.info[item].dwJobType == JTYPE_MASTER) {
			std::strcat(jobName, " [M]");
		} else {
			std::strcat(jobName, " [H]");
		}
		p2DRender->TextOut(rect.left, rect.top, jobName, color);
	}
}



void CWndInventory::RunUpgrade(CItemElem * pItem) {
	if (!m_bIsUpgradeMode) return;
	m_bIsUpgradeMode = FALSE;

	const auto result = RunUpgradeDecide(pItem);
	if (result.has_value()) {
		m_pUpgradeItem = pItem;
		m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
	} else {
		const DWORD errorMessage = result.error();
		if (errorMessage != 0) g_WndMng.PutString(errorMessage);
		BaseMouseCursor();
	}
}

std::expected<void, DWORD> CWndInventory::RunUpgradeDecide(CItemElem * pItem) {
	using OK = std::expected<void, DWORD>;

	if (!pItem || !m_pUpgradeMaterialItem) return std::unexpected(TID_UPGRADE_CANCLE);

	const ItemProp * pTargetProp = pItem->GetProp();
	if (!pTargetProp) return std::unexpected(0);

	const ItemProp * pItemProp = m_pUpgradeMaterialItem->GetProp();
	if (!pItemProp) return std::unexpected(0);

	// --- Piercing
	if (sqktd::is_among(pItemProp->dwItemKind3, IK3_SOCKETCARD, IK3_SOCKETCARD2)) {
		if (!pItem->IsPierceAble(pItemProp->dwItemKind3)) {
			return std::unexpected(TID_PIERCING_POSSIBLE_ITEM);
		}

		bool hasFreeSlot = false;
		for (int j = 0; j < pItem->GetPiercingSize(); j++) {
			if (pItem->GetPiercingItem(j) == 0) {
				hasFreeSlot = true;
				break;
			}
		}

		if (!hasFreeSlot) {
			return std::unexpected(TID_PIERCING_ERROR_NOPIERCING);
		}

		return OK();
	}

	// --- Ele cards
	if (pItemProp->dwItemKind3 == IK3_ELECARD) {
		if (!CItemElem::IsEleRefineryAble(pTargetProp)) {
			return std::unexpected(TID_GAME_NOTEQUALITEM);
		}

		if (pItem->m_bItemResist != SAI79::NO_PROP && pItem->m_bItemResist != pItemProp->eItemType) {
			return std::unexpected(TID_UPGRADE_ERROR_TWOELEMENT);
		}

		const DWORD dwWantedCard = SAI79::GetEleCard(pItemProp->eItemType);

		if (pItemProp->dwID != dwWantedCard) {
			return std::unexpected(TID_UPGRADE_ERROR_WRONGUPLEVEL);
		}

		return OK();
	}

	// --- Upgrade
	if (pItemProp->dwItemKind3 == IK3_ENCHANT) {
		if (m_pWndRemoveJewelConfirm ) {
			if (m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId) {
				return std::unexpected(TID_GAME_NOTEQUALITEM);
			}
		}

		if (m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM02) {
			if (CWndSmeltJewel * pWndSmeltJewel = g_WndMng.GetWndBase<CWndSmeltJewel>(APP_SMELT_JEWEL)) {
				if (pWndSmeltJewel->m_pItemElem && pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId) {
					return std::unexpected(TID_GAME_NOTEQUALITEM);
				}
			}

			if (pTargetProp->dwReferStat1 != WEAPON_ULTIMATE) {
				return std::unexpected(TID_GAME_NOTEQUALITEM);
			}
		} else if (!CItemElem::IsDiceRefineryAble(pTargetProp)) {
			return std::unexpected(TID_GAME_NOTEQUALITEM);
		}

		return OK();
	}

	// --- Random scroll
	if (pItemProp->dwItemKind3 == IK3_RANDOM_SCROLL) {
		if (sqktd::is_among(pTargetProp->dwItemKind1, IK1_WEAPON, IK2_ARMOR, IK2_ARMORETC)) {
			return OK();
		} else {
			return std::unexpected(TID_GAME_RANDOMSCROLL_ERROR);
		}
	}

	// Other things
	if (pItemProp->IsNeedTarget()) {
		return OK();
	}

	// Moonstone
	if (ItemProps::IsMoonstone(*m_pUpgradeMaterialItem)) {
		if (pItem->IsCollector(TRUE) || pItem->GetProp()->dwItemKind2 == IK2_JEWELRY) {
			return OK();
		}

		if constexpr (true /* Remove jewel from inventory */) {
			return std::unexpected(TID_GAME_NOTEQUALITEM);
		} else {
			bool hasJewel = false;
			for (int i = 0; i < 5; i++) {
				if (pItem->GetUltimatePiercingItem(i) != 0) {
					hasJewel = true;
					break;
				}
			}

			if (!hasJewel) {
				return std::unexpected(TID_GAME_NOTEQUALITEM);
			}

			if (CWndSmeltJewel * pWndSmeltJewel = g_WndMng.GetWndBase<CWndSmeltJewel>(APP_SMELT_JEWEL)) {
				if (pWndSmeltJewel->m_pItemElem && pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId) {
					return std::unexpected(TID_GAME_NOTEQUALITEM);
				}
			}

			if (m_pWndRemoveJewelConfirm) {
				if (m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId) {
					return std::unexpected(TID_GAME_NOTEQUALITEM);
				}
			}

			if (m_pWndRemoveJewelConfirm == NULL) {
				m_pWndRemoveJewelConfirm = new CWndRemoveJewelConfirm;
				m_pWndRemoveJewelConfirm->Initialize(this);
				m_pWndRemoveJewelConfirm->SetItem(pItem);

				m_bRemoveJewel = TRUE;
				return std::unexpected(0);
			}
		}
	}
	

	return OK();
}

void CWndInventory::BaseMouseCursor()
{
	m_bIsUpgradeMode = FALSE;

	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
}

void CWndInventory::UpdateParts()
{
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_skin,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
}

///////////////////////////////////////////////////////////////////////////
// CWndGuildCombatOfferMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndGuildCombatOfferMessageBox : public CWndCustomMessageBox
{ 
public: 
	DWORD m_nCost;
	void	SetValue( CString str, DWORD nCost );
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndGuildCombatOfferMessageBox::Initialize( CWndBase* pWndParent )
{
	return CWndMessageBox::Initialize( "", 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndGuildCombatOfferMessageBox::SetValue( CString str, DWORD nCost )
{
	m_wndText.SetString( str );
	m_nCost = nCost;	
}

BOOL CWndGuildCombatOfferMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendGuildCombatApp( m_nCost );
			SAFE_DELETE( g_WndMng.m_pWndGuildCombatOffer);
			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



// ���? �Ĺ� ��û�ϱ�

void CWndGuildCombatOffer::EnableAccept( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_ACCEPT );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );	
	}
}

void CWndGuildCombatOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString( prj.GetText(TID_GAME_GUILDCOMBAT_PRESENT_TEXT) );
	pWndText->EnableWindow( TRUE );
	
	MoveParentCenter();
} 

BOOL CWndGuildCombatOffer::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_OFFER, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndGuildCombatOffer::SetGold( DWORD nCost )
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	if( pWndEdit )
	{
		CString str;
		str.Format( "%d", nCost );
		pWndEdit->SetString(str);
	}				
}

void CWndGuildCombatOffer::SetTotalGold( __int64 nCost )
{
	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	pStatic->AddWndStyle(WSS_MONEY);
	
	if( pStatic )
	{
		CString str;
		str.Format( "%I64d", nCost );
		pStatic->SetTitle(str);
	}
}

BOOL CWndGuildCombatOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ACCEPT )
	{
		if( g_pPlayer )
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			
			DWORD nCost;
			CString str = pWndEdit->GetString();
			nCost = atoi( str );

			if( m_dwReqGold != 0 )
			{
				if( nCost <= m_dwBackupGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_CURRENT_REQUEST) ); //���� ��ĺ���? �� ���� �ݾ����� ��û�� �Ͻñ� �ٶ��ϴ�.
					return FALSE;
				}
				
			}
			else
			{
				if( nCost < m_dwMinGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_MIN) ); //�ּұݾ׺��� �� ���� ��ķ�? ��û�Ͻñ� �ٶ��ϴ�.
					return FALSE;
				}
			}


			CWndGuildCombatOfferMessageBox* pMsg = new CWndGuildCombatOfferMessageBox;

				g_WndMng.OpenCustomBox( pMsg );

				if( m_dwReqGold == 0 )
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), 0, nCost ); //������ ��û�� %d��Ŀ���? �߰��� %d��ĸ�? ��û�ϰڽ��ϱ�?
				}
				else
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), m_dwBackupGold, nCost-m_dwBackupGold ); //������ ��û�� %d��Ŀ���? �߰��� %d��ĸ�? ��û�ϰڽ��ϱ�?
				}

				pMsg->SetValue( str, nCost );
			
		}
		
//		Destroy();
	}
	else if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// ���? �������?
CWndGuildCombatBoard::CWndGuildCombatBoard(int nCombatType)
{
	m_nCombatType = nCombatType;
}

void CWndGuildCombatBoard::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// �����? Ÿ��Ʋ ���� �ؽ�Ʈ�� ����ϴ�? �� 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_BOARD) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_BOARD) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

void CWndGuildCombatBoard::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildCombatBoard::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	MoveParentCenter();
} 

BOOL CWndGuildCombatBoard::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_BOARD, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombatBoard::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndGuildCombatBoard::SetString( const CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar );
	pWndText->ResetString();	
}

// ����Ĺ�? ���� ����â...�ð��� �ߴ� �޼���â...
CGuildCombatInfoMessageBox::CGuildCombatInfoMessageBox(int nCombatType)
{
	m_nCombatType = nCombatType;
}

void CGuildCombatInfoMessageBox::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// �����? Ÿ��Ʋ ���� �ؽ�Ʈ�� ����ϴ�? �� 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		const char * szNameLevel;

		if (m_nCombatType == 0)
			szNameLevel = prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE);
		else if (m_nCombatType == 1)
			szNameLevel = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE);
		else
			szNameLevel = "???";

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

BOOL CGuildCombatInfoMessageBox::Initialize( CWndBase* pWndParent )
{
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_MSG, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CGuildCombatInfoMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_YES:
		{
			if(m_nCombatType == 0)
				g_DPlay.SendGCTele();
			else if(m_nCombatType == 1)
				g_DPlay.SendGC1to1TeleportToNPC();
			Destroy();
		}
		break;
	case WIDC_NO:
		{
			Destroy();
		}
	case 10000:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CGuildCombatInfoMessageBox::SetString( const CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}

void CGuildCombatInfoMessageBox::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	MoveParentCenter();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// ����Ĺ�? ���� ����â...�ð��� �ߴ� �޼���â...

void CGuildCombatInfoMessageBox2::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();	
}

BOOL CGuildCombatInfoMessageBox2::Initialize( CWndBase* pWndParent )
{
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_MSG2, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CGuildCombatInfoMessageBox2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_OK:
		{
			Destroy();
		}
		break;
	case 10000:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CGuildCombatInfoMessageBox2::SetString( CHAR* szChar  )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////




// ���? ������ ���� ���� ����
BOOL CGuildCombatSelectionClearMessageBox::Initialize( CWndBase* pWndParent )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_GAME_GUILDCOMBAT_REMAKE_MAKEUP), //�����ۼ��� �ٽ� �Ͻðڽ��ϱ�?
		pWndParent, 
		MB_OKCANCEL );
}

BOOL CGuildCombatSelectionClearMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case IDOK:
		{
			CWndGuildCombatSelection *pGuildCombatSelection = (CWndGuildCombatSelection*)g_WndMng.GetWndBase( APP_GUILDCOMBAT_SELECTION );

			if (pGuildCombatSelection) {
				pGuildCombatSelection->ReceiveLineup({}, 0);
			}

			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndGuildCombatSelection::SetDefender(const u_long uiPlayer) {
	m_uidDefender = uiPlayer;
}

void CWndGuildCombatSelection::SetMemberSize(int nMaxJoin, int nMaxWar) {
	nMaxJoinMember = nMaxJoin;
	nMaxWarMember = nMaxWar;

	CWndBase * pWndStatic = GetDlgItem(WIDC_STATIC3);

	CString str;
	str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_OFFER_INFO), nMaxWarMember, nMaxJoinMember);
	pWndStatic->SetTitle(str);
}

void CWndGuildCombatSelection::ReceiveLineup(std::span<const u_long> members, u_long defenderId) {
	PlayerLineup::DoubleGCListManager(
		GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX1),
		GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX2)
	).Reset(members);

	m_uidDefender = defenderId;
}

void CWndGuildCombatSelection::OnDraw( C2DRender* p2DRender )  {
	if (CWndBase * pWndStatic = GetDlgItem(WIDC_BUTTON8)) {
		pWndStatic->EnableWindow(FALSE);

		if (m_uidDefender <= 0) {
			pWndStatic->SetVisible(FALSE);
		} else {
			auto * combatPlayers = GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX2);

			const int position = combatPlayers->Find(
				[&](const PlayerLineup & player) {
					return player.playerId == m_uidDefender;
				}
			);
			
			if (position >= 0) {
				pWndStatic->SetVisible(TRUE);
				pWndStatic->Move(330, (position * 17) + 75);
			} else {
				pWndStatic->SetVisible(FALSE);
			}
		}
	}

	CRect crect;
	crect.left = 349;
	crect.top  = 74;
	crect.right = 587;
	crect.bottom = crect.top+(nMaxWarMember*17);
	p2DRender->RenderFillRect( crect, D3DCOLOR_ARGB( 40, 0, 0, 220 ) );

	crect.left = 349;
	crect.top  = crect.bottom;
	crect.right = 587;
	crect.bottom = 377;
	p2DRender->RenderFillRect( crect, D3DCOLOR_ARGB( 40, 220, 0, 0 ) );
}

void CWndGuildCombatSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();

	ReplaceListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>(WIDC_LISTBOX1);
	ReplaceListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>(WIDC_LISTBOX2);

	// �ð� ���������� �Ǵ�
	if (g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP)); //������ �����ۼ��� �� �� �����ϴ�.
		Destroy();
		return;
	}
	
	MoveParentCenter();
	ReceiveLineup({}, 0);
} 

BOOL CWndGuildCombatSelection::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_GUILDCOMBAT_SELECTION, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndGuildCombatSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) { 
	static constexpr auto ToBOOL = [](bool b) -> BOOL { return b ? TRUE : FALSE; };

	const auto Manager = [&]() {
		return PlayerLineup::DoubleGCListManager(
			GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX1),
			GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX2)
		);
	};

	switch (nID) {
		case WIDC_BUTTON1: OnConnectedToCombat(); return TRUE;
		case WIDC_BUTTON2:
		{
			const auto removedId = Manager().ToGuild();
			if (removedId && removedId.value() == m_uidDefender) {
				m_uidDefender = -1;
			}
			return TRUE;
		}
		case WIDC_BUTTON3: Manager().MoveUp();    return TRUE;
		case WIDC_BUTTON4: Manager().MoveDown();  return TRUE;
		case WIDC_RESET: {
			g_WndMng.OpenCustomBox(new CGuildCombatSelectionClearMessageBox());
			break;
		}
		case WIDC_FINISH:  return ToBOOL(OnFinish());
		case WIDC_BUTTON7: return ToBOOL(OnChooseDefender());
		case WIDC_CLOSE:
			Destroy();
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndGuildCombatSelection::OnConnectedToCombat() {
	const PlayerLineup::RuleSet ruleSet {
		.maxSelect = static_cast<size_t>(nMaxJoinMember),
			.minimumLevel = 30
	};

	const PlayerLineup::SelectReturn result = PlayerLineup::DoubleGCListManager(
		GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX1),
		GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX2)
	).ToSelect(ruleSet);

	switch (result) {
		using enum PlayerLineup::SelectReturn;
		case FullLineup:
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_SELECTION_MAX), nMaxJoinMember);
			g_WndMng.OpenMessageBox(str);
			break;
		}
		case NotAMember:
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_MEMBER));
			break;
		case TooLowLevel:
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_LEVEL_NOTICE));
			break;
		case AlreadyInLineup:
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_ALREADY_ENTRY));
			break;
	}
}

bool CWndGuildCombatSelection::OnFinish() {

#ifndef _DEBUG
	if (m_uidDefender == -1 || m_uidDefender == 0) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_ASSIGN_DEFENDER)); //�������? �������� �ʾҽ��ϴ�. ��帶���͸�? ������ 1���� �������? ������ �ּ���.
		return false;
	}
#endif //_DEBUG

	// �ð� ���������� �Ǵ�
	if (g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP)); //������ �����ۼ��� �� �� �����ϴ�.
		Destroy();
		return false;
	}

	auto * combatPlayers = GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX2);
	if (combatPlayers->GetSize() == 0) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_PLAYER)); //�����ڰ� �����ϴ�. �����ڸ� �������ּ���.
		return false;
	}

	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) {
		return false;
	}

	std::vector<u_long> selected;
	for (size_t i = 0; i != combatPlayers->GetSize(); ++i) {
		const u_long p = (*combatPlayers)[static_cast<int>(i)].playerId;
		selected.emplace_back(p);
	}

	g_DPlay.SendGCSelectPlayer(selected, m_uidDefender);
	Destroy();
	return true;
}

bool CWndGuildCombatSelection::OnChooseDefender() {
	auto * combatPlayers = GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>>(WIDC_LISTBOX2);

	const PlayerLineup * nCurSelItem = combatPlayers->GetCurSelItem();
	if (nCurSelItem) {
		SetDefender(nCurSelItem->playerId);
		return true;
	} else {
		return false;
	}
}

//������ ���� ���? ���� ���?
CWndGuildCombatState::CWndGuildCombatState(int nCombatType) {
	m_tEndTime = 0;
	m_tCurrentTime = 0;
	m_nCombatType = nCombatType;
}

void CWndGuildCombatState::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// �����? Ÿ��Ʋ ���� �ؽ�Ʈ�� ����ϴ�? �� 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		const char * szNameLevel;
		if (m_nCombatType == 0)
			szNameLevel = prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE);
		else if (m_nCombatType == 1)
			szNameLevel = prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE);
		else
			szNameLevel = "???";

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

void CWndGuildCombatState::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

BOOL CWndGuildCombatState::Initialize( CWndBase* pWndParent )
{
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_1TO1_OFFERSTATE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombatState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	else
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
		if(m_nCombatType == 0)
			g_DPlay.SendGuildCombatWindow();
		else if(m_nCombatType == 1)
			g_DPlay.SendGC1to1TenderOpenWnd();
	}
	else
	if( nID == WIDC_BUTTON2 )
	{
		if(m_nCombatType == 0)
			g_DPlay.SendGCRequestStatusWindow();
		else if(m_nCombatType == 1)
			g_DPlay.SendGC1to1TenderView();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildCombatState::InsertGuild( const char szGuild[], const char szName[], int nNum )
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
	CString str;
	str.Format( "No.%.2d		%s", nNum, szGuild );
	pWndList->AddString(str);
}

void CWndGuildCombatState::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_tCurrentTime > 0 )
	{
		CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );

		CString str1;
		CString str2;
		CString str3;
		CString str4;
		CString str;
		
		str1.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(m_ct.GetDays()) );
		str2.Format(prj.GetText(TID_PK_LIMIT_HOUR), m_ct.GetHours() );
		str3.Format(prj.GetText(TID_PK_LIMIT_MINUTE), m_ct.GetMinutes() );
		str4.Format(prj.GetText(TID_PK_LIMIT_SECOND), m_ct.GetSeconds() );

		str.Format( "%s %s %s %s", str1, str2, str3, str4 );
		pStatic->SetTitle(str);
	}
} 

void CWndGuildCombatState::SetRate( int nRate )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	
	CString str;
	str.Format( "%d", nRate );
	
	if( nRate == -1 )
		str = " ";

	pWndStatic->SetTitle( str );
	if( 8 < nRate )
		pWndStatic->m_dwColor = 0xFFFF0000;	
}

BOOL CWndGuildCombatState::Process() 
{
	m_tCurrentTime = ( m_tEndTime - time_null() );
	
	if( m_tEndTime && m_tCurrentTime > 0 )
	{
		m_ct = m_tCurrentTime;
	}
	
	return TRUE;
}


void CWndGuildCombatState::SetGold( int nGold )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pWndStatic->AddWndStyle(WSS_MONEY);	

	CString str;
	str.Format( "%d", nGold );
	pWndStatic->SetTitle( str );	
}



// �������? �������� ���� â

void CWndGuildCombatJoinSelection::OnDraw( C2DRender* p2DRender ) 
{
	DWORD dwLeftTime = m_dwOldTime - g_tmCurrent;

	CWndStatic* pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );
	
	CString str;
	CString strstr;
	
	if( m_nMapNum == 99 )
	{
		strstr = "Random";
	}
	else
	{
		strstr.Format( "%d", m_nMapNum+1 );
	}

	str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_MESSAGE), (dwLeftTime/1000)+1, strstr );
	
	pStatic->SetTitle( str );

	if( m_nMapNum != 99 )
	{
		CWndButton* pWndButton = NULL;

		if( m_nMapNum == 0 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON1 );
		}
		else
		if( m_nMapNum == 1 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON2 );
		}
		else
		if( m_nMapNum == 2 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON3 );
		}
		else
		if( m_nMapNum == 3 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON4 );
		}
		
		if( pWndButton )
		{
			CRect rect = pWndButton->GetWndRect();
			rect.DeflateRect( 1, 1, 1, 1 );
			p2DRender->RenderRect( rect, 0xFF0000FF );
		}
	}
}

void CWndGuildCombatJoinSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();

	m_wndTitleBar.SetVisible( FALSE );	
} 

BOOL CWndGuildCombatJoinSelection::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_SELSTART, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombatJoinSelection::Process ()
{
	if( m_dwOldTime <= g_tmCurrent )
	{
		g_DPlay.SendGCSelectMap(m_nMapNum);		
		Destroy();
	}

	return 1;
}

BOOL CWndGuildCombatJoinSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		g_DPlay.SendGCSelectMap(0);	
		m_nMapNum = 0;
	}
	else if( nID == WIDC_BUTTON2 )
	{
		g_DPlay.SendGCSelectMap(1);		
		m_nMapNum = 1;
	}
	else if( nID == WIDC_BUTTON3 )
	{
		g_DPlay.SendGCSelectMap(2);		
		m_nMapNum = 2;
	}
	else if( nID == WIDC_BUTTON4 )
	{
		g_DPlay.SendGCSelectMap(3);		
		m_nMapNum = 3;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������ ��û
void CWndGuildWarAppConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_APP) ), 0xff000000 );//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	MoveParentCenter();
} 

BOOL CWndGuildWarAppConfirm::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_CLOSE_EXISTING_CONNECTION, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildWarAppConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
			g_DPlay.SendGuildCombatApp( 80000 );

		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// ������ ���?
CWndGuildWarCancelConfirm::CWndGuildWarCancelConfirm(int nCombatType)
{
	m_nCombatType = nCombatType;
}

void CWndGuildWarCancelConfirm::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// �����? Ÿ��Ʋ ���� �ؽ�Ʈ�� ����ϴ�? �� 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL_CONFIRM) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_CANCEL_CONFIRM) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

void CWndGuildWarCancelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_CANCEL) ), 0xff000000 );//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	MoveParentCenter();
} 

BOOL CWndGuildWarCancelConfirm::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_CANCEL_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildWarCancelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(m_nCombatType == 0)
		{
			if( g_pPlayer )
				g_DPlay.SendGuildCombatCancel();
		}
		else if(m_nCombatType == 1)
		{
			if( g_pPlayer )
				g_DPlay.SendGC1to1TenderCancel();
		}
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// ������ ����
CWndGuildWarJoinConfirm::CWndGuildWarJoinConfirm(int nCombatType) 
{
	m_nCombatType = nCombatType;
}

void CWndGuildWarJoinConfirm::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// �����? Ÿ��Ʋ ���� �ؽ�Ʈ�� ����ϴ�? �� 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_ENTRANCE) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

void CWndGuildWarJoinConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	if(m_nCombatType == 0)
		m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_JOIN) ), prj.GetTextColor(TID_GAME_GUILDWAR_JOIN) );
	else if(m_nCombatType == 1)
		m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE_MSG) ), prj.GetTextColor(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE_MSG) );
	m_wndText.ResetString();
	
	MoveParentCenter();
} 

BOOL CWndGuildWarJoinConfirm::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_CLOSE_EXISTING_CONNECTION, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildWarJoinConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
		{
			if( g_pPlayer->IsBaseJob() )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOTENTER_VAGRANT) ); //����ڴ�? �����Ҽ� �����ϴ�. 1�� ���� �Ŀ� ������ ���ֽñ� �ٶ��ϴ�.
				Destroy( TRUE );
				return FALSE;
			}
			if(m_nCombatType == 0)
				g_DPlay.SendGCJoin();
			else if(m_nCombatType == 1)
			{
				if(g_WndMng.m_pWndGuildCombatInfoMessageBox)
					SAFE_DELETE( g_WndMng.m_pWndGuildCombatInfoMessageBox );
				
				g_DPlay.SendGC1to1TeleportToStage();
			}
		}
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

/****************************************************
  WndId : APP_GUILDCOMBAT_RESULT_POINT - ������ ����?
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/

void CWndGuildCombatResult::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;

	m_WndGuildCombatTabResultRate.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT );
	m_WndGuildCombatTabResultLog.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT_LOG );
	
	pWndTabCtrl->InsertItem(&m_WndGuildCombatTabResultRate, prj.GetText(TID_GAME_TOOLTIP_LOG1));
	pWndTabCtrl->InsertItem(&m_WndGuildCombatTabResultLog, prj.GetText(TID_GAME_TOOLTIP_LOG2));
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
	
	CWndListBox* pWndList1 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX1 ));
	CWndListBox* pWndList2 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX2 ));
	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_TEXT1 ));
	
	pWndList1->ResetContent();
	pWndList2->ResetContent();	
	pWndText->ResetString();
}

BOOL CWndGuildCombatResult::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_GUILDCOMBAT_RESULT_POINT, pWndParent, 0, CPoint(0, 0));
}

void CWndGuildCombatResult::InsertGuildRate(LPCTSTR str) {
	CWndListBox * pWndList = m_WndGuildCombatTabResultRate.GetDlgItem<CWndListBox>(WIDC_LISTBOX1);
	pWndList->AddString(str);
}

void CWndGuildCombatResult::InsertPersonRate(LPCTSTR str) {
	CWndListBox * pWndList = m_WndGuildCombatTabResultRate.GetDlgItem<CWndListBox>(WIDC_LISTBOX2);
	pWndList->AddString(str);
}

void CWndGuildCombatResult::InsertLog(LPCTSTR str) {
	CWndText * pWndText = m_WndGuildCombatTabResultLog.GetDlgItem<CWndText>(WIDC_TEXT1);
	pWndText->AddString(str);
}


/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT
****************************************************/
void CWndGuildCombatResult::CWndTabRate::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

	
/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT_LOG
****************************************************/
void CWndGuildCombatResult::CWndTabLog::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

BOOL CWndGuildCombatResult::CWndTabLog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{ 
	if( nID == WIDC_BUTTON1 )
	{
		CTime time = CTime::GetCurrentTime();
		
		CString str;
		str.Format( "GuildLog_%d%.2d%.2d.txt", time.GetYear(), time.GetMonth(), time.GetDay() );

		CFileIO file;

		if( file.Open( str, _T( "wt" ) ) == FALSE )
			return FALSE;

		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
				
		file.PutString( _T( pWndText->m_string ) ); 

		CString strTemp;
		strTemp.Format( prj.GetText(TID_GAME_GUILDCOMBAT_LOG_SAVE), str );

		g_WndMng.OpenMessageBox( strTemp );
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/****************************************************
  WndId : APP_GUILDCOMBAT_RANK_P - ������ ��ŷâ
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/

void CWndGuildCombatRank_Person::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	
	m_WndGuildCombatTabClass_Tot.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mer.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mag.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Acr.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Ass.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	
	pWndTabCtrl->InsertItem(&m_WndGuildCombatTabClass_Tot, prj.GetText(TID_GAME_TOOLTIP_LOG1));
	pWndTabCtrl->InsertItem(&m_WndGuildCombatTabClass_Mer, prj.jobs.info[JOB_MERCENARY].szName);
	pWndTabCtrl->InsertItem(&m_WndGuildCombatTabClass_Mag, prj.jobs.info[JOB_MAGICIAN].szName);
	pWndTabCtrl->InsertItem(&m_WndGuildCombatTabClass_Acr, prj.jobs.info[JOB_ACROBAT].szName);
	pWndTabCtrl->InsertItem(&m_WndGuildCombatTabClass_Ass, prj.jobs.info[JOB_ASSIST].szName);


	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
BOOL CWndGuildCombatRank_Person::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_RANK_P, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombatRank_Person::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_CLOSE) {
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndGuildCombatRank_Person::SetList(std::vector<GuildCombatRankInfo> ranking)
{
	if (ranking.empty()) return;
	
	std::ranges::sort(
		ranking,
		[](const GuildCombatRankInfo & lhs, const GuildCombatRankInfo & rhs) {
			if (lhs.nPoint > rhs.nPoint) return true;
			if (lhs.nPoint < rhs.nPoint) return false;

			return lhs.strName < rhs.strName;
		}
	);

	// ��ü��Ͽ�? ���?
	CWndGuildCombatRank_Class * pRankTot = &m_WndGuildCombatTabClass_Tot;

	for (const GuildCombatRankInfo & rankInfo : ranking) { 
		// �������� ���?
		
		if (CWndGuildCombatRank_Class * pRank = __GetJobKindWnd(rankInfo.nJob)) {
			pRank->InsertRank(rankInfo);
			pRankTot->InsertRank(rankInfo);
		}
	}
}

CWndGuildCombatRank_Class * CWndGuildCombatRank_Person::__GetJobKindWnd(const int nJob) {
	switch (prj.jobs.GetProJob(nJob)) {
		case Project::ProJob::Mercenary: return &m_WndGuildCombatTabClass_Mer;
		case Project::ProJob::Acrobat:   return &m_WndGuildCombatTabClass_Acr;
		case Project::ProJob::Assist:    return &m_WndGuildCombatTabClass_Ass;
		case Project::ProJob::Magician:  return &m_WndGuildCombatTabClass_Mag;
		default:                         return nullptr;
	}
}

/****************************************************
  WndId : APP_GUILDCOMBAT_RANKINGCLASS
****************************************************/

GuildCombatRankInfo::GuildCombatRankInfo(int nJob, u_long uidPlayer, int nPoint) {
	this->uidPlayer = uidPlayer;

	const char * name = CPlayerDataCenter::GetInstance()->GetPlayerString(uidPlayer);
	this->strName = name ? name : "???";

	this->nJob    = nJob;
	this->nPoint  = nPoint;
}

void GuildCombatRankInfo::Render(
	C2DRender * p2DRender,
	CPoint point, DWORD dwColor, std::optional<size_t> rank
) const {
	char buffer[8];

	if (rank) {
		std::sprintf(buffer, "%3zu", *rank);
		p2DRender->TextOut(point.x + 4, point.y, buffer, dwColor);
	}

	p2DRender->TextOut( point.x + 40,  point.y, strName                   , dwColor );
	p2DRender->TextOut( point.x + 180, point.y, prj.jobs.info[nJob].szName, dwColor );

	std::sprintf(buffer, "%2d", nPoint);
	p2DRender->TextOut( point.x + 275, point.y, buffer, dwColor );
}


void CWndGuildCombatRank_Class::OnDraw( C2DRender* p2DRender ) 
{ 
	constexpr int sx = 8;
	
	CRect rc( sx, 5, sx+330, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );
	
	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );
	

	constexpr DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	p2DRender->TextOut( sx + 4,      10, prj.GetText(TID_GAME_RATE),  dwColor );
	p2DRender->TextOut( sx + 40,     10, prj.GetText(TID_GAME_NAME),  dwColor );
	p2DRender->TextOut( sx + 180,    10, prj.GetText(TID_GAME_JOB),  dwColor );
	p2DRender->TextOut( sx + 275,    10, prj.GetText(TID_GAME_POINT),  dwColor );

	if (m_listRank.empty()) return;

	const size_t nBase = static_cast<size_t>(std::max(m_wndScrollBar.GetScrollPos(), 0));

	const GuildCombatRankInfo * GCRankInfoMy = nullptr;
	int	nMyRanking = 0;
	
	int sy = 35;

	size_t currentRanking = 1;
	int lastSeenPoints = m_listRank[0].nPoint;

	for (size_t i = 0; i != m_listRank.size(); ++i) {
		const GuildCombatRankInfo & GCRankInfo = m_listRank[i];

		if (lastSeenPoints != GCRankInfo.nPoint) {
			currentRanking = i + 1;
			lastSeenPoints = GCRankInfo.nPoint;
		}

		// Display current item
		if (i >= nBase && i < nBase + MAX_GUILDCOMBAT_RANK_PER_PAGE) {
			// Selection
			if (m_nSelect && *m_nSelect == i) {
				rc.SetRect(sx, sy - 4, sx + 320, sy + 16);
				p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(64, 0, 0, 0));
			}

			// Display
			const DWORD dwColor = currentRanking == 1 ? D3DCOLOR_XRGB(200, 0, 0) : D3DCOLOR_XRGB(0, 0, 0);

			std::optional<size_t> rank;
			if (i == nBase || currentRanking == i + 1) {
				rank = currentRanking;
			}

			GCRankInfo.Render(p2DRender, CPoint(sx, sy), dwColor, rank);

			sy += 18;
		}

		if (GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer) {
			GCRankInfoMy = &GCRankInfo;
			nMyRanking = currentRanking;
		}
	}

	// ���� ��ŷ ǥ��...
	if (GCRankInfoMy) {
		GCRankInfoMy->Render(p2DRender, CPoint(sx, 250), D3DCOLOR_XRGB(0, 0, 255), nMyRanking);
	}
} 


// ���õ� �ε����� ��´�?.
std::optional<size_t> CWndGuildCombatRank_Class::GetSelectIndex(const CPoint & point) const {
	const int nBase = m_wndScrollBar.GetScrollPos();
	const int nIndex = nBase + (point.y - 32) / 18;

	if (nIndex >= 0 && std::cmp_less(nIndex, MAX_GUILDCOMBAT_RANK_PER_PAGE)) {
		const size_t zIndex = static_cast<size_t>(nIndex);
		if (zIndex >= m_listRank.size()) return std::nullopt;
		return zIndex;
	} else {
		return std::nullopt;
	}
}

BOOL CWndGuildCombatRank_Class::OnMouseWheel(UINT, short zDelta, CPoint) {
	m_wndScrollBar.MouseWheel(zDelta);
	return TRUE;
}

void CWndGuildCombatRank_Class::OnLButtonDown(UINT, CPoint point) {
	m_nSelect = GetSelectIndex(point);
}

void CWndGuildCombatRank_Class::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CRect rect = GetWindowRect();
	rect.top    += 30;
	rect.bottom -= 80;
	rect.left    = rect.right - 30;
	rect.right  -= 30;

	m_wndScrollBar.SetScrollFromSize(static_cast<int>(m_listRank.size()), MAX_GUILDCOMBAT_RANK_PER_PAGE);
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
}

BOOL CWndGuildCombatRank_Class::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_GUILDCOMBAT_RANKINGCLASS, pWndParent, 0, CPoint(0, 0));
}

void CWndGuildCombatRank_Class::InsertRank(GuildCombatRankInfo rankInfo)
{
	if (m_listRank.size() >= MAX_GUILDCOMBAT_RANK) {
		Error("CWndGuildCombatRank_Class::InsertRank - range over");
		return;
	}

	m_listRank.emplace_back(std::move(rankInfo));

	m_wndScrollBar.SetScrollFromSize(static_cast<int>(m_listRank.size()), MAX_GUILDCOMBAT_RANK_PER_PAGE);
}


/****************************************************
  WndId : APP_FONTEDIT
****************************************************/

CWndFontEdit::CWndFontEdit() {
	m_managed.fill(ManagedColor{});
}

void CWndFontEdit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pTexture = m_textureMng.AddTexture( MakePath( DIR_THEME, "yellowbuttten.tga" ), 0xffff00ff, TRUE );

	m_managed.fill(ManagedColor{});
	m_managed[0].rect = CRect(CPoint(38, 46), CPoint(156,  63));
	m_managed[1].rect = CRect(CPoint(38, 68), CPoint(156,  83));
	m_managed[2].rect = CRect(CPoint(38, 89), CPoint(156, 103));

	for (ManagedColor & managed : m_managed) {
		managed.xColor = managed.rect.left;
	}

	m_bLButtonClick = false;
	
	MoveParentCenter();
}

BOOL CWndFontEdit::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_FONTEDIT, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndFontEdit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( m_pWndText == NULL )
		return FALSE;

	switch (nID) {
		case WIDC_CHECK1:
			if (GetDlgItem<CWndButton>(WIDC_CHECK1)->GetCheck()) {
				m_pWndText->BlockSetStyle(ESSTY_BOLD);
			} else {
				m_pWndText->BlockClearStyle(ESSTY_BOLD);
			}
			break;
		case WIDC_CHECK2:
			if (GetDlgItem<CWndButton>(WIDC_CHECK2)->GetCheck()) {
				m_pWndText->BlockSetStyle(ESSTY_UNDERLINE);
			} else {
				m_pWndText->BlockClearStyle(ESSTY_UNDERLINE);
			}
			break;
		case WIDC_CHECK3:
			if (GetDlgItem<CWndButton>(WIDC_CHECK3)->GetCheck()) {
				m_pWndText->BlockSetStyle(ESSTY_STRIKETHROUGH);
			} else {
				m_pWndText->BlockClearStyle(ESSTY_STRIKETHROUGH);
			}
			break;
		case WIDC_CHECK4:
			if (GetDlgItem<CWndButton>(WIDC_CHECK4)->GetCheck()) {
				m_pWndText->BlockSetColor(GetSelectedColor());
			} else {
				m_pWndText->BlockSetColor(0xff000000);
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

DWORD CWndFontEdit::GetSelectedColor() const {
	const DWORD dwR = static_cast<DWORD>(m_managed[0].fColor * 255);
	const DWORD dwG = static_cast<DWORD>(m_managed[1].fColor * 255);
	const DWORD dwB = static_cast<DWORD>(m_managed[2].fColor * 255);

	return D3DCOLOR_XRGB(dwR, dwG, dwB);
}

void CWndFontEdit::OnLButtonUp(UINT, CPoint) {
	m_bLButtonClick = false;
}

void CWndFontEdit::OnLButtonDown(UINT, CPoint) {
	m_bLButtonClick = true;
}

void CWndFontEdit::OnMouseWndSurface( CPoint point )
{
	if (!g_pPlayer) return;
	
	const CRect rect = CRect( 17, 17, 186, 148 );
	
	if( !rect.PtInRect( point ) )
		m_bLButtonClick = false;

	if (!m_bLButtonClick) return;
	
	for (ManagedColor & manager : m_managed) {
		CRect DrawRect = manager.rect;
		DrawRect.top    -= 22;
		DrawRect.bottom -= 22;
		
		if( DrawRect.PtInRect( point ) )
		{
			point.x = std::clamp(point.x, DrawRect.left, DrawRect.right);

			const LONG Width = DrawRect.Width();
			const LONG Pos   = point.x - DrawRect.left;
			const FLOAT p = (FLOAT)Pos / (FLOAT)Width;
			manager.fColor = p;
			manager.xColor = point.x;
		}
	}
}

void CWndFontEdit::OnDraw(C2DRender * p2DRender) {
	for (const ManagedColor & manager : m_managed) {
		const CPoint pt = CPoint(
			manager.xColor - m_pTexture->m_size.cx / 2,
			manager.rect.top - 20
		);
		m_pTexture->Render(p2DRender, pt);
	}

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);
	const DWORD color = GetSelectedColor();
	p2DRender->RenderFillRect(lpFace->rect, color);
}



#ifdef __TRADESYS

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndDialogEvent Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndDialogEvent::CWndDialogEvent() 
{
	m_nChoiceNum = -1;
	m_nListCount = 0;
	m_nDescCount = 0;
	m_nCurrentPage = 0;
	m_nMMI = -1;
	m_nListOffset = 0;

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( "123" );

	m_nGap = size.cy + 2;

	if(m_nGap >= 16)
		m_nListOffset = m_nGap - 16;
	else
		m_nListOffset = 0;
}

CWndDialogEvent::~CWndDialogEvent() 
{ 
} 

void CWndDialogEvent::OnDestroy()
{
}

void CWndDialogEvent::OnDraw( C2DRender* p2DRender ) 
{
	if(m_nChoiceNum > -1)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;
		D3DXCOLOR color =  D3DCOLOR_ARGB( 60, 240, 0, 0 );
		
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
		rect = lpWndCtrl->rect;
		rect.top += m_nChoiceNum * m_nGap + m_nListOffset;
		rect.bottom += m_nChoiceNum * m_nGap + m_nListOffset;
		//���� ������ â ũ�� ����
		rect.right += 80;
		p2DRender->RenderFillRect( rect, color );
	}
}

void CWndDialogEvent::SetMMI(int mmi)
{
	m_nMMI = mmi;
}

void CWndDialogEvent::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	if(m_nMMI != -1)
	{
		m_nVecList = prj.m_Exchange.GetListTextId(m_nMMI);
		m_nListCount = m_nVecList.size();
		if( !m_nListCount ) // ����Ʈ�� ���� ���?
		{	
			Destroy();
			return;
		}
		SetDescription(NULL);
	}
	else
		Destroy();

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndDialogEvent::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_DIALOG_EVENT, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndDialogEvent::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID) 
	{
		case WIDC_BUTTON1:
			//Text Page Down
			if(m_nCurrentPage < m_nDescCount - 1)
			{
				m_nCurrentPage++;
				CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
				pText->m_string.SetString(prj.GetText(m_nDescList[m_nCurrentPage]));
				
				if(m_nCurrentPage == m_nDescCount - 1)
				{
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
					pButton->SetVisible(FALSE);
					pButton->EnableWindow(FALSE);
				}
			}
			break;
		case WIDC_OK:
			if(m_nChoiceNum > -1)
			{
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
				pButton->EnableWindow(FALSE);		
				//Send to Server....
				g_DPlay.SendExchange(m_nMMI, m_nChoiceNum);

			}
			else
				Destroy();
			break;
		case WIDC_CANCEL:
			Destroy();
			break;
		case WIDC_LISTBOX1:
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			int nCurSel = pWndListBox->GetCurSel();
			if( nCurSel != -1 )
				m_nChoiceNum = nCurSel;
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndDialogEvent::SetDescription( CHAR* szChar )
{
	//Set List
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
	rect = lpWndCtrl->rect;

	for(int i=0; i<m_nListCount; i++)
	{
		pWndList->AddString( prj.GetText(m_nVecList[i]));
	}

	//Set Description
	m_nDescList = prj.m_Exchange.GetDescId(m_nMMI);
	m_nDescCount = m_nDescList.size();
	if( !m_nDescCount )
		return;
	
	if(m_nDescCount == 1)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);
		pButton->EnableWindow(FALSE);
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_nCurrentPage = 0;
	pText->m_string.AddParsingString( prj.GetText(m_nDescList[m_nCurrentPage]) );
	pText->ResetString();	

	//Size Control
	if(m_nListCount <= MAX_LIST_COUNT)
	{
		int nListBoxBottom = 0;
		int nListBoxHeight = 0;
		int nButtonHeight = 0;
		//ListBox
		rect.bottom = rect.top + m_nListCount * m_nGap + 10;
		nListBoxBottom = rect.bottom;
		nListBoxHeight = rect.bottom - rect.top;
		//���� ������ â ũ�� ����
		rect.right += 80;
		pWndList->SetWndRect(rect);
		//Window
		rect = GetWindowRect(TRUE);
		lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
		CRect rectText = lpWndCtrl->rect;
		rect.bottom = rect.top + nListBoxHeight + (rectText.bottom - rectText.top) + 80;
		SetWndRect(rect);
		//Button
		CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_OK );		
		lpWndCtrl = GetWndCtrl( WIDC_OK );
		rect = lpWndCtrl->rect;
		nButtonHeight = rect.bottom - rect.top;
		rect.top = nListBoxBottom + 8;
		rect.bottom = rect.top + nButtonHeight;
		//���� ������ â ũ�� ����
		rect.left += 40;
		rect.right += 40;
		pWndButton->SetWndRect(rect);

		pWndButton = (CWndButton*)GetDlgItem( WIDC_CANCEL );		
		lpWndCtrl = GetWndCtrl( WIDC_CANCEL );
		rect = lpWndCtrl->rect;
		nButtonHeight = rect.bottom - rect.top;
		rect.top = nListBoxBottom + 8;
		rect.bottom = rect.top + nButtonHeight;
		//���� ������ â ũ�� ����
		rect.left += 40;
		rect.right += 40;
		pWndButton->SetWndRect(rect);
	}

	//���� ������ â ũ�� ����
	//Window
	rect = GetWindowRect(TRUE);
	rect.right += 80;
	SetWndRect(rect);

	//Button
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );		
	lpWndCtrl = GetWndCtrl( WIDC_BUTTON1 );
	rect = lpWndCtrl->rect;
	rect.left += 80;
	rect.right += 80;
	pWndButton->SetWndRect(rect);

	//Text
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	lpWndCtrl = GetWndCtrl( WIDC_TEXT1 );
	rect = lpWndCtrl->rect;
	rect.right += 80;
	pWndText->SetWndRect(rect);
}

void CWndDialogEvent::ReceiveResult(int result)
{
	const std::vector<int> vResult = prj.m_Exchange.GetResultMsg(m_nMMI, m_nChoiceNum);
	
	switch(result) 
	{
		case CExchange::EXCHANGE_SUCCESS:
			// �������� �ý��� �޼��� �������? ����
			if( vResult.size() == 2 )
				g_WndMng.OpenMessageBox( prj.GetText(vResult[0]) );
			break;
		case CExchange::EXCHANGE_INVENTORY_FAILED:
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_LACKSPACE) );
			break;
		case CExchange::EXCHANGE_CONDITION_FAILED:
			// �������� �ý��� �޼��� �������? ����
			if( vResult.size() == 2 )
				g_WndMng.OpenMessageBox( prj.GetText(vResult[1]) );
			break;
		case CExchange::EXCHANGE_FAILED:
			g_WndMng.OpenMessageBox( "FAILED" );
			break;			
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(TRUE);
}

#endif //__TRADESYS


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeavenTower Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeavenTower::CWndHeavenTower() 
{
	m_nChoiceNum = -1;
	m_nListCount = 0;
	m_nDescCount = 0;
	m_nCurrentPage = 0;
	m_nListOffset = 0;

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( "123" );

	m_nGap = size.cy + 2;

	if(m_nGap >= 16)
		m_nListOffset = m_nGap - 16;
	else
		m_nListOffset = 0;
}

CWndHeavenTower::~CWndHeavenTower() 
{ 
} 

void CWndHeavenTower::OnDestroy()
{
}

void CWndHeavenTower::OnDraw( C2DRender* p2DRender ) 
{
	if(m_nChoiceNum > -1)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;
		D3DXCOLOR color =  D3DCOLOR_ARGB( 60, 240, 0, 0 );
		
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
		rect = lpWndCtrl->rect;
		rect.top += m_nChoiceNum * m_nGap + m_nListOffset;
		rect.bottom += m_nChoiceNum * m_nGap + m_nListOffset;
		//���� ������ â ũ�� ����
		rect.right += 80;
		p2DRender->RenderFillRect( rect, color );
	}
}

void CWndHeavenTower::InitText()
{
	m_nDescList.push_back(TID_TOOLTIP_HEAVENTOWER);

	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR01);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR02);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR03);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR04);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR05);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR06);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR07);

	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR01_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR02_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR03_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR04_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR05_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR06_MSG);

	m_nCost.push_back(10000);
	m_nCost.push_back(30000);
	m_nCost.push_back(50000);
	m_nCost.push_back(70000);
	m_nCost.push_back(100000);

	m_nListCount = m_nVecList.size();

	//Set List
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	for(int i=0; i<m_nListCount; i++)
	{
		CString strText;
		strText.Format("%d. %s", i+1, prj.GetText(m_nVecList[i]));
		pWndList->AddString(strText);
	}

	//Set Description
	m_nDescCount = m_nDescList.size();
	if( m_nDescCount == 0 )
		return;
	
	if(m_nDescCount == 1)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);
		pButton->EnableWindow(FALSE);
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_nCurrentPage = 0;
	pText->m_string.AddParsingString( prj.GetText(m_nDescList[m_nCurrentPage]) );
	pText->ResetString();
}

void CWndHeavenTower::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	InitText();

	if( m_nListCount == 0 )
	{	
		Destroy();
		return;
	}
	InitWnd();

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndHeavenTower::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_HEAVEN_TOWER, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndHeavenTower::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID) 
	{
		case WIDC_BUTTON1:
			//Text Page Down
			if(m_nCurrentPage < m_nDescCount - 1)
			{
				m_nCurrentPage++;
				CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
				pText->m_string.SetString(prj.GetText(m_nDescList[m_nCurrentPage]));
				
				if(m_nCurrentPage == m_nDescCount - 1)
				{
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
					pButton->SetVisible(FALSE);
					pButton->EnableWindow(FALSE);
				}
			}
			break;
		case WIDC_LISTBOX1:
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			int nCurSel = pWndListBox->GetCurSel();
			if( nCurSel != -1 )
			{
				m_nChoiceNum = nCurSel;

				if(m_nChoiceNum == 6)
				{
					Destroy();
					break;
				}

				CString strMsg;
				CWndHeavenTowerEntranceConfirm* pWndEntranceConfirm = new CWndHeavenTowerEntranceConfirm;

					g_WndMng.OpenCustomBox( pWndEntranceConfirm );
					
					if(m_nChoiceNum != 5)
					{
						strMsg.Format( prj.GetText(m_nMsgList[m_nChoiceNum]), m_nCost[m_nChoiceNum] );
						pWndEntranceConfirm->SetValue( strMsg, m_nChoiceNum+1 );
					}
					else
					{
						strMsg.Format( prj.GetText(m_nMsgList[m_nChoiceNum]) );
						pWndEntranceConfirm->SetValue( strMsg, m_nChoiceNum+1 );
					}

				
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndHeavenTower::InitWnd()
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	//Size Control
	if(m_nListCount <= MAX_FLOOR_COUNT)
	{
		CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
		lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
		rect = lpWndCtrl->rect;

		int nListBoxBottom = 0;
		int nListBoxHeight = 0;
		int nButtonHeight = 0;
		//ListBox
		rect.bottom = rect.top + m_nListCount * m_nGap + 10;
		nListBoxBottom = rect.bottom;
		nListBoxHeight = rect.bottom - rect.top;
		//���� ������ â ũ�� ����
		rect.right += 80;

		pWndList->SetWndRect(rect);
		//Window
		rect = GetWindowRect(TRUE);
		lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
		CRect rectText = lpWndCtrl->rect;
		rect.bottom = rect.top + nListBoxHeight + (rectText.bottom - rectText.top) + 60;
		SetWndRect(rect);
	}

	//���� ������ â ũ�� ����
	//Window
	rect = GetWindowRect(TRUE);
	rect.right += 80;
	SetWndRect(rect);

	//Button
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );		
	lpWndCtrl = GetWndCtrl( WIDC_BUTTON1 );
	rect = lpWndCtrl->rect;
	rect.left += 80;
	rect.right += 80;
	pWndButton->SetWndRect(rect);

	//Text
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	lpWndCtrl = GetWndCtrl( WIDC_TEXT1 );
	rect = lpWndCtrl->rect;
	rect.right += 80;
	pWndText->SetWndRect(rect);
}

//////////////////////////////////////////////////////////////////////////
// CWndHeavenTowerEntranceConfirm Class
//////////////////////////////////////////////////////////////////////////

BOOL CWndHeavenTowerEntranceConfirm::Initialize( CWndBase* pWndParent )
{
	m_nFloor = 0;
	return CWndMessageBox::Initialize( "", pWndParent, MB_OKCANCEL );	
}

void CWndHeavenTowerEntranceConfirm::SetValue( CString str, DWORD nFloor )
{
	m_wndText.SetString( str );
	m_nFloor = nFloor;	
}

BOOL CWndHeavenTowerEntranceConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			if(m_nFloor > 0 && m_nFloor != 6)
			{
				g_DPlay.SendTeleportToHeavenTower(m_nFloor);
				SAFE_DELETE(g_WndMng.m_pWndHeavenTower);
			}
			Destroy();
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


//////////////////////////////////////////////////////////////////////////
// Couple Message Window
//////////////////////////////////////////////////////////////////////////
BOOL CWndCoupleMessage::Initialize( CWndBase* pWndParent )
{
	return CWndMessageBox::Initialize( m_strText, pWndParent, MB_OKCANCEL );
}

BOOL CWndCoupleMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( m_nMode )
	{
		case CM_SENDPROPOSE:
			{
				if(nID == IDOK)
				{
					if(m_pTargetObj)
						g_DPlay.SendPropose(((CMover*)m_pTargetObj)->GetName());
					Destroy();
				}
				else if(nID == IDCANCEL)
					Destroy();					
			}
			break;
		case CM_RECEIVEPROPOSE:
			{
				if(nID == IDOK)
				{
					g_DPlay.SendCouple();
					Destroy();
				}
				else if(nID == IDCANCEL)
				{
					g_DPlay.SendRefuse();
					Destroy();
				}
			}
			break;
		case CM_CANCELCOUPLE:
			{
				if(nID == IDOK)
				{
					g_DPlay.SendDecouple();
					Destroy();
				}
				else if(nID == IDCANCEL)
					Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleMessage::SetMessageMod(CString strText, int nMode, CObj* pTargetObj)
{
	m_strText = strText;
	m_nMode = nMode;
	m_pTargetObj = pTargetObj;
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Tab Information Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleTabInfo::CWndCoupleTabInfo()
{
	m_pCouple = NULL;
	m_pVBGauge = NULL;
}

CWndCoupleTabInfo::~CWndCoupleTabInfo()
{
}

HRESULT CWndCoupleTabInfo::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndCoupleTabInfo::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndCoupleTabInfo::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}

void CWndCoupleTabInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	m_texGauEmptyNormal.LoadTexture( MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtBreakUp.bmp" ), 0xffff00ff );
		else
			pWndButton->SetTexture( MakePath( DIR_THEME, "ButtBreakUp.bmp" ), 0xffff00ff );
	}

	MoveParentCenter();
} 

BOOL CWndCoupleTabInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_BUTTON1)
	{
		if(m_pCouple)
		{
			// �������� ���� �޼��� â ���?
			if(g_WndMng.m_pWndCoupleMessage)
				SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);

			g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
			if(g_WndMng.m_pWndCoupleMessage)
			{
				u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
				PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);

				if(pData)
				{
					CString strText;
					strText.Format(prj.GetText(TID_GAME_COUPLECANCEL), pData->szPlayer);
					g_WndMng.m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_CANCELCOUPLE);
					g_WndMng.m_pWndCoupleMessage->Initialize();
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleTabInfo::OnDraw(C2DRender* p2DRender)
{
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[6];
	TEXTUREVERTEX2* pVertices = pVertex;

	//Couple Info & Exp Gauge Draw
	CRect rect, rectTemp;
	LPWNDCTRL lpStatic = GetWndCtrl( WIDC_STATIC4 );
	rect.TopLeft().y = lpStatic->rect.top + 36;
	rect.TopLeft().x = lpStatic->rect.left + 70;
	rect.BottomRight().y = lpStatic->rect.top + 48;
	rect.BottomRight().x = lpStatic->rect.right - 10;

	m_pCouple = CCoupleHelper::Instance.GetCouple();
	if(m_pCouple)
	{
		u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
		PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);

		if(pData)
		{
			CString strTemp;
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			pWndStatic->SetTitle(pData->szPlayer);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			pWndStatic->SetTitle(prj.jobs.info[ pData->data.nJob ].szName);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", pData->data.nLevel);
			pWndStatic->SetTitle(strTemp);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", m_pCouple->GetLevel());
			pWndStatic->SetTitle(strTemp);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", m_pCouple->GetLevel());
			pWndStatic->SetTitle(strTemp);

			// Draw Master/Hero Icon
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

			if(pWndWorld)
			{
				LPWNDCTRL lpCtrl = GetWndCtrl(WIDC_LEVEL);
				CPoint ptJobType;
				ptJobType.x = lpCtrl->rect.right - 60;
				ptJobType.y = lpCtrl->rect.top - 1;

				const int nMasterIndex = Project::Jobs::PlayerDataIcon(pData->data.nJob, pData->data.nLevel).master;
				if (nMasterIndex != 0) {
					pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, ptJobType, nMasterIndex, &pVertices, 0xffffffff);
				}

				pWndWorld->m_texPlayerDataIcon.Render( pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
			}
		}

		float fRate = CCoupleProperty::Instance()->GetExperienceRate( m_pCouple->GetLevel(), m_pCouple->GetExperience() );

		//ó���� ������ ������ ����
		if(fRate > 0.00f && fRate < 0.04f)
			fRate = 0.05f;
		else if(fRate >= 0.04f && fRate <= 0.075f)
			fRate = 0.075f;

		int nWidth	= (int)( rect.Width() * fRate );
		rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		if( rect.right < rectTemp.right )
			rectTemp.right = rect.right;

//		CString stTrace;
//		stTrace.Format("fRate = %f nWidth = %d \n", fRate, nWidth);
//		TRACE(stTrace);
	}
	else
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
	}

	if(m_pVBGauge)
	{
		m_Theme.RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
		m_Theme.RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
	}

	SAFE_DELETE_ARRAY( pVertex );
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Tab Skill Window
//////////////////////////////////////////////////////////////////////////

void CWndCoupleTabSkill::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CWndText::SetupDescription(
		GetDlgItem<CWndText>(WIDC_TEXT1),
		_T("CoupleSkillInfo.inc")
	);

	m_pSkillBgTexture = CWndBase::m_textureMng.AddTexture( MakePath( DIR_THEME, "Bg_Couple_Skill.tga"), 0xffff00ff );

	MoveParentCenter();
} 

BOOL CWndCoupleTabSkill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleTabSkill::OnDraw(C2DRender* p2DRender)
{
	CPoint point;

	LPWNDCTRL lpWndStatic = GetWndCtrl(WIDC_STATIC1);
	
	if(lpWndStatic)
	{
		point.x = lpWndStatic->rect.left + 4;
		point.y = lpWndStatic->rect.top + 2;
	}

	// Draw Background Image
	if(m_pSkillBgTexture)
		p2DRender->RenderTexture( point, m_pSkillBgTexture );

	// Draw Active SKill
	int nLevel = 0;
	
	if (CCouple * pCouple = CCoupleHelper::Instance.GetCouple()) {
		nLevel = pCouple->GetLevel();
	}

	std::vector<int> & vSkills	= CCoupleProperty::Instance()->GetSkill( nLevel );
	std::vector<int> & vSkillKinds = CCoupleProperty::Instance()->GetSKillKinds();

	if(vSkills.size() == vSkillKinds.size())
	{
		for(int i=0; i<(int)( vSkills.size() ); i++)
		{
			if(vSkills[i] > 0)
			{
				ItemProp* pItemProp = prj.GetItemProp( vSkillKinds[i] );
				if(pItemProp)
				{
					CTexture* pTex = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, pItemProp->szIcon), 0xffff00ff );

					if(pTex)
						p2DRender->RenderTexture( point, pTex );
				}
			}
			point.x += 32;
		}
	}

	// Make Tooltip
	CPoint ptMouse = GetMousePoint();
	CRect rect = lpWndStatic->rect;

	rect.left += 4;
	rect.top += 2;
	rect.right = rect.left + 32;
	rect.bottom = rect.top + 32;

	for(int i=0; i<(int)( vSkillKinds.size() ); i++)
	{
		if(rect.PtInRect(ptMouse))
		{
			// Tooltip
			CString strSkillName;
			ItemProp* pItemProp = prj.GetItemProp( vSkillKinds[i] );

			if(pItemProp)
				strSkillName.Format("%s", pItemProp->szName);

			ClientToScreen( &ptMouse );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 10000, strSkillName, rect, ptMouse, 1 );
		}

		rect.left += 32;
		rect.right += 32;
	}
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleManager::CWndCoupleManager()
{
}

CWndCoupleManager::~CWndCoupleManager()
{
}

void CWndCoupleManager::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	
	m_wndCoupleTabInfo.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_INFO );
	m_wndCoupleTabSkill.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_SKILL );

	pWndTabCtrl->InsertItem(&m_wndCoupleTabInfo, prj.GetText(TID_GAME_COUPLE_INFO));
	pWndTabCtrl->InsertItem(&m_wndCoupleTabSkill, prj.GetText(TID_GAME_COUPLE_SKILL));

	MoveParentCenter();
} 

BOOL CWndCoupleManager::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_COUPLE_MAIN, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndCoupleManager::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}



#ifdef __FUNNY_COIN

CWndFunnyCoinConfirm::CWndFunnyCoinConfirm() 
{
	m_dwItemId = 0;
	m_pItemElem = NULL;
}
 
CWndFunnyCoinConfirm::~CWndFunnyCoinConfirm() 
{ 
} 

void CWndFunnyCoinConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	if(m_pItemElem != NULL)
	{
		CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		if(pText)
		{
			CString strText;
			ItemProp* pItemProp = prj.GetItemProp( m_pItemElem->m_dwItemId );
			if(pItemProp)
				strText.Format(prj.GetText( TID_GAME_FUNNYCOIN_ASKUSE ), pItemProp->szName);

			pText->AddString(strText);
		}
	}
	else
		Destroy();

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndFunnyCoinConfirm::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_FUNNYCOIN_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndFunnyCoinConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(static_cast<int>(m_dwItemId) > -1)
			g_DPlay.SendDoUseItem(MAKELONG( ITYPE_ITEM, m_dwItemId ), NULL_ID, -1, FALSE);
	}

	Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndFunnyCoinConfirm::SetInfo(DWORD dwItemId, CItemElem* pItemElem)
{
	m_dwItemId = dwItemId;
	m_pItemElem = pItemElem;
}

#endif //__FUNNY_COIN


CWndRestateConfirm::CWndRestateConfirm(DWORD dwItemID) : 
m_dwItemID(dwItemID), 
m_ObjID(0), 
m_nPart(-1)
{
}

CWndRestateConfirm::~CWndRestateConfirm(void)
{
}

BOOL CWndRestateConfirm::Initialize( CWndBase* pWndParent )
{
	return CWndNeuz::InitDialog( APP_RESTATE_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
}

void CWndRestateConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_RESTATE_CONFIRM_TEXT);
	assert(pWndText != NULL);
	switch(m_dwItemID)
	{
	case II_CHR_SYS_SCR_RESTATE:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_ALL_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_STR:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STR_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_STA:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STA_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_DEX:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_DEX_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_INT:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_INT_CONFIRM));
			break;
		}
#ifdef __ADD_RESTATE_LOW
	case II_CHR_SYS_SCR_RESTATE_STR_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_STR_LOW_CONFIRM ) );
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_STA_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_STA_LOW_CONFIRM ) );
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_DEX_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_DEX_LOW_CONFIRM ) );
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_INT_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_INT_LOW_CONFIRM ) );
			break;
		}
#endif // __ADD_RESTATE_LOW
	}

	MoveParentCenter();
}

BOOL CWndRestateConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if(pWndInventory != NULL)
	{
		switch(nID)
		{
		case WIDC_RESTATE_CONFIRM_YES:
			{
				g_DPlay.SendDoUseItem(m_dwItemObjID, m_ObjID, m_nPart);
				Destroy();
				break;
			}
		case WIDC_RESTATE_CONFIRM_NO:
			{
				Destroy();
				break;
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRestateConfirm::SetInformation(DWORD dwItemObjID, OBJID m_ObjID, int nPart)
{
	m_dwItemObjID = dwItemObjID;
	m_ObjID = m_ObjID;
	m_nPart = nPart;
}

//-----------------------------------------------------------------------------
CWndCampusInvitationConfirm::CWndCampusInvitationConfirm( u_long idSender, const CString& rstrSenderName ) : 
m_idSender( idSender ), 
m_strSenderName( rstrSenderName )
{
}
//-----------------------------------------------------------------------------
CWndCampusInvitationConfirm::~CWndCampusInvitationConfirm( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampusInvitationConfirm::Initialize( CWndBase* pWndParent )
{
	return CWndNeuz::InitDialog( APP_CONFIRM_ENTER, pWndParent, WBS_KEY, 0 );
}
//-----------------------------------------------------------------------------
void CWndCampusInvitationConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = ( CWndText* )GetDlgItem( WIDC_TEXT1 );
	if( pWndText )
	{
		CString strMessage = _T( "" );
		strMessage.Format( prj.GetText( TID_GAME_CAMPUS_INVITATION_CONFIRM ), m_strSenderName ); // %s ���� ���� ��û�� �ϼ̽��ϴ�. �����Ͻðڽ��ϱ�?
		pWndText->AddString( strMessage );
	}
	CWndButton* pOk = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pCancel = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );
	CWndEdit* pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	AddWndStyle( WBS_MODAL );
	pOk->SetDefault();

	//����Ʈâ�� �� ���̴� ������ ���� ������ ENTER ���� �غ�
	pEdit->Move( -100, -100 );
	pEdit->SetFocus( );

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndCampusInvitationConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT1 )
	{
		g_DPlay.SendAcceptCampusMember( m_idSender );
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			g_DPlay.SendAcceptCampusMember( m_idSender );
			Destroy();
			break;
		}
	case WIDC_BUTTON2:
		{
			g_DPlay.SendRefuseCampusMember( m_idSender );
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
CWndCampusSeveranceConfirm::CWndCampusSeveranceConfirm( u_long idTarget, const CString& rstrTargetName ) : 
m_idTarget( idTarget ), 
m_strTargetName( rstrTargetName )
{
}
//-----------------------------------------------------------------------------
CWndCampusSeveranceConfirm::~CWndCampusSeveranceConfirm( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampusSeveranceConfirm::Initialize( CWndBase* pWndParent )
{
	return CWndNeuz::InitDialog( APP_CONFIRM_ENTER, pWndParent, WBS_KEY, 0 );
}
//-----------------------------------------------------------------------------
void CWndCampusSeveranceConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = ( CWndText* )GetDlgItem( WIDC_TEXT1 );
	if( pWndText )
	{
		CString strMessage = _T( "" );
		strMessage.Format( prj.GetText( TID_GAME_CAMPUS_SEVERANCE_CONFIRM ), m_strTargetName ); // %s �԰� ���� ���踦 ���ڽ��ϱ�?
		pWndText->AddString( strMessage );
	}
	CWndButton* pOk = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pCancel = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );
	CWndEdit* pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	AddWndStyle( WBS_MODAL );
	pOk->SetDefault();

	//����Ʈâ�� �� ���̴� ������ ���� ������ ENTER ���� �غ�
	pEdit->Move( -100, -100 );
	pEdit->SetFocus( );

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndCampusSeveranceConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT1 )
	{
		g_DPlay.SendRemoveCampusMember( m_idTarget );
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}
	
	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			g_DPlay.SendRemoveCampusMember( m_idTarget );
			Destroy();
			break;
		}
	case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
