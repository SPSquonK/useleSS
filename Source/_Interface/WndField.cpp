#include "stdafx.h"
#include <ranges>
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

#include "DPClient.h"
#include "MsgHdr.h"

#include "wndvendor.h"
#include "wndwebbox.h"
#include "WndBagEx.h"

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
#define MAX_MAIL_LIST_PER_PAGE 6
#define MAX_GUILDCOMBAT_LIST		  100

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ���÷� 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ResetRenderState(IDirect3DDevice9 * const pd3dDevice) {
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

static D3DVIEWPORT9 BuildViewport(C2DRender * const render, WNDCTRL * const control) {
	D3DVIEWPORT9 viewport;
	viewport.X = render->m_ptOrigin.x + control->rect.left;
	viewport.Y = render->m_ptOrigin.y + control->rect.top;
	viewport.Width = control->rect.Width();
	viewport.Height = control->rect.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	return viewport;
}


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
	pWndOk->SetDefault( TRUE );
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
BOOL CWndDropItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
	pWndOk->SetDefault( TRUE );

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndDropConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
  WndId : APP_RANDOMSCROLL_CONFIRM - ���� �ɼ�
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_YES - Button
  CtrlId : WIDC_NO - Button
****************************************************/

void CWndRandomScrollConfirm::SetItem( OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf )
{
	objid = objidBuf;
	objid1 = objid1Buf;
	bFlag = bFlagBuf;
}

void CWndRandomScrollConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CItemElem* pItemElem0	= g_pPlayer->m_Inventory.GetAtId( objid );
	CItemElem* pItemElem1	= g_pPlayer->m_Inventory.GetAtId( objid1 );

	if( pItemElem0 && pItemElem1 )
	{
		CString strMessage;
		if( bFlag )
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_CONFIRM1 ), pItemElem0->GetProp()->szName );
		else
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_CONFIRM ), pItemElem0->GetProp()->szName );

		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		pWndText->SetString( strMessage );
		pWndText->EnableWindow( FALSE );
	}
	else
	{
		Destroy();
	}

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndRandomScrollConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_RANDOMSCROLL_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
  ���� ������ ���� ���? 
BOOL CWndRandomScrollConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/

BOOL CWndRandomScrollConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		g_DPlay.SendRandomScroll( objid, objid1 );
	}
	Destroy();

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
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_QUEITMWARNING, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
���� ������ ���� ���? 
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
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
			g_WndMng.m_pWndDropConfirm->Initialize( NULL, APP_DROP_CONFIRM );
		}
		else
		{
			//SAFE_DELETE( g_WndMng.m_pWndDropItem );
			g_WndMng.m_pWndDropItem = new CWndDropItem;
			g_WndMng.m_pWndDropItem->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropItem->m_vPos = m_vPos;
			g_WndMng.m_pWndDropItem->Initialize( NULL, APP_DROP_ITEM );
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
	m_texture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ITEM, "itm_GolGolSeed.dds" ), 0xffff00ff );
}

//////////////////////////////////////////////
CWndQueryEquip::CWndQueryEquip(CMover & mover, std::unique_ptr<std::array<CItemElem, MAX_HUMAN_PARTS>> aEquipInfoAdd) {
	m_InvenRect.fill(CRect());

	m_OldPos = CPoint(0, 0);

	// Set mover
	m_ObjID = mover.GetId();

	const int nMover = (mover.GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = std::unique_ptr<CModelObject>(dynamic_cast<CModelObject *>(
		prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE
		)));
	prj.m_modelMng.LoadMotion(m_pModel.get(), OT_MOVER, nMover, MTI_STAND);
	CMover::UpdateParts(mover.GetSex(), mover.m_dwSkinSet, mover.m_dwFace, mover.m_dwHairMesh, mover.m_dwHeadMesh, mover.m_aEquipInfo, m_pModel.get(), NULL);
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());

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
	
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

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

	ResetRenderState(pd3dDevice);

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
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
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

BOOL CWndQueryEquip::Initialize(CWndBase * pWndParent, DWORD) {
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
	SAFE_DELETE( m_pModel );	
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
	SAFE_DELETE( m_pModel );
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
				
				CString strText;

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

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

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

	ResetRenderState(pd3dDevice);

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
		g_pPlayer->OverCoatItemRenderCheck(m_pModel);
		
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
		
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}

	return;
}

void CWndInventory::UpDateModel()
{
	SAFE_DELETE( m_pModel );
	
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );	
}


void CWndInventory::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;


	InitializeInvenRect(m_InvenRect, *this);

	SAFE_DELETE( m_pModel );

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );

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

	m_TexRemoveItem = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndInventoryGarbage.dds" ), 0xffff00ff );
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 112 + 48 );
	Move( point );
}
BOOL CWndInventory::Initialize(CWndBase * pWndParent, DWORD dwWndId) {
	m_InvenRect.fill(CRect());
	return CWndNeuz::InitDialog(dwWndId, pWndParent, 0, CPoint(792, 130));
}

BOOL CWndInventory::Process()
{
	if( m_pModel )
		m_pModel->FrameMove();

	if( m_dwEnchantWaitTime < g_tmCurrent )
	{
		m_dwEnchantWaitTime = 0xffffffff;
		
		if( m_pSfxUpgrade )
		{
			m_pSfxUpgrade->Delete();
			m_pSfxUpgrade = NULL;
		}
		
		CItemElem* pItemElem = m_pUpgradeItem;
		if( pItemElem && m_pUpgradeMaterialItem )
		{
			CItemElem* pItemMaterialElem = m_pUpgradeMaterialItem;
			// ��Ŷ ����
			
			if( pItemMaterialElem->GetProp() )
			{
				// ��þƮ�� ���� �������̳�?
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD 
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ENCHANT
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_PIERDICE
					)
				{
					if(pItemMaterialElem->m_dwItemId == II_GEN_MAT_ORICHALCUM02)
						g_DPlay.SendUltimateEnchantWeapon( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					else if(pItemMaterialElem->m_dwItemId == II_GEN_MAT_MOONSTONE || pItemMaterialElem->m_dwItemId == II_GEN_MAT_MOONSTONE_1)
					{
						if( pItemElem->IsCollector( TRUE ) || pItemElem->GetProp()->dwItemKind2 == IK2_JEWELRY )
							g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					}
					else
						g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
				}
				else
				// �Ǿ�̿�? ���� �������̳�?
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD2
					)
				{
					g_DPlay.SendPacket<PACKETTYPE_PIERCING, DWORD, DWORD>(
						pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId
						);
				}
				else if( IsNeedTarget( pItemMaterialElem->GetProp() ) )
				{
					g_DPlay.SendDoUseItemTarget( m_pUpgradeMaterialItem->m_dwObjId, pItemElem->m_dwObjId );
				}
				else
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_RANDOM_SCROLL )
				{
					SAFE_DELETE( g_WndMng.m_pWndRandomScrollConfirm );
					g_WndMng.m_pWndRandomScrollConfirm = new CWndRandomScrollConfirm;
					if( 0 < pItemElem->GetRandomOpt() )
						g_WndMng.m_pWndRandomScrollConfirm->SetItem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId, TRUE );
					else
						g_WndMng.m_pWndRandomScrollConfirm->SetItem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					g_WndMng.m_pWndRandomScrollConfirm->Initialize();
				}
			}
		}
	}
	return TRUE;
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
							m_pWndConfirmBuy->Initialize( this, APP_CONFIRM_BUY_ );
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
							m_pWndConfirmBuy->Initialize( this, APP_CONFIRM_BUY_ );
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
					pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor, APP_VENDOR_BUY );
				}
				bForbid		= FALSE;
			}
			else
			// ��ũ���� �°Ŷ��? ����
			if( pWndFrame->GetWndId() == APP_COMMON_BANK )
			{
				CWndBase* pWndTaget = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[0]) )
					{					
						nSlot = 0;
					}
					else if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[1]) )
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}
					
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					
					for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}

					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if( itemElem->m_nItemNum > 1 )
					{ 
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD; // �κ��丮 �ε� ���� �־ Gold�� ��.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"��� �̵��Ͻðڽ��ϱ�?";
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" ���� : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
						g_DPlay.SendGetItemBank( nSlot, (BYTE)( lpShortcut->m_dwId ), 1 );
					}
				}
				else
				{
					if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[0]) )
					{					
						nSlot = 0;
					}
					else if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[1]) )
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}

					// ���? (��)
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->m_dwGoldBank[nSlot];
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD;	// �κ��丮 �ε� ���� �־ Gold�� ��.
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					
					g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
					CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"�󸶸� �̵��Ͻðڽ��ϱ�?";
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
				bForbid = FALSE;
			}
			// �޴밡�濡�� �� ���?
			if( pWndFrame->GetWndId() == APP_BAG_EX )
			{
				
				CWndBase* pWndFrom = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
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
					
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					
					for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}

					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if( itemElem->m_nItemNum > 1 )
					{ 
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BAG_EX; // �κ��丮 �ε� ���� �־ Gold�� ��.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->m_nSlot = -1;
						g_WndMng.m_pWndTradeGold->m_nPutSlot = nSlot;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"��� �̵��Ͻðڽ��ϱ�?";
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" ���� : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
						g_DPlay.SendMoveItem_Pocket( nSlot, lpShortcut->m_dwId, 1 , -1);
					}
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
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsGetItem( g_pPlayer->m_idPlayer ))
					{
						CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
						
						for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
						{
							int nItem = pWndItemCtrl->GetSelectedItem( i );
							pWndItemCtrl->GetItem( nItem );
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;	// ****�߿�****
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							
							g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
							CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
							CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"��� �̵��Ͻðڽ��ϱ�?";
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" ���� : ";
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
							g_DPlay.SendGetItemGuildBank( (BYTE)( lpShortcut->m_dwId ), 1, 1 );
						}
					}
				}
				else
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsGetPenya( g_pPlayer->m_idPlayer ))
					{
						// ���? (��)
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = pGuild->m_nGoldGuild;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;	// �κ��丮 �ε� ���� �־ Gold�� ��.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"�󸶸� �̵��Ͻðڽ��ϱ�?";
						CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
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
							|| IsNeedTarget( pProp )
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
							m_pSfxUpgrade = CreateSfx( g_Neuz.m_pd3dDevice, XI_INT_INCHANT, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
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
						if(g_WndMng.m_pWndChangeAttribute)
							SAFE_DELETE(g_WndMng.m_pWndChangeAttribute);

						g_WndMng.m_pWndChangeAttribute = new CWndChangeAttribute;
						g_WndMng.m_pWndChangeAttribute->SetChangeItem(pFocusItem);
						g_WndMng.m_pWndChangeAttribute->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
						bAble = FALSE;
					}

					if(pProp && pProp->dwID == II_SYS_SYS_SCR_RECCURENCE || pProp->dwID == II_SYS_SYS_SCR_RECCURENCE_LINK)
					{	// ����ų �������� ���? Ȯ�� â ����.
						if(g_pPlayer->m_nSkillPoint < g_pPlayer->GetCurrentMaxSkillPoint())
						{
							SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
							g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
							g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
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
						g_WndMng.m_pWndRestateConfirm->Initialize(NULL);
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
									g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
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
//								g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
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
								g_WndMng.m_pWndCommItemDlg->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
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
					if(pFocusItem->GetProp() && (pFocusItem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pFocusItem->IsFlag( CItemElem::binds ) )
					{
						SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK);
						g_WndMng.m_pWndEquipBindConfirm->SetInformationDoubleClick(pFocusItem, dwObjId);
						g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
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

BOOL CWndInventory::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	SetEnchantCursor();

	return TRUE;
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
					if( pItemElem->IsUndestructable() == TRUE )
					{
						g_WndMng.PutString(TID_GAME_ERROR_UNDESTRUCTABLE_ITEM);
						return FALSE;
					}

					if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER  )
						return FALSE;

					SAFE_DELETE( g_WndMng.m_pWndInvenRemoveItem );
					g_WndMng.m_pWndInvenRemoveItem = new CWndInvenRemoveItem;
					g_WndMng.m_pWndInvenRemoveItem->Initialize( &g_WndMng, APP_INVEN_REMOVE_ITEM );
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
				CRect rect;
				LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				rect = lpWndCtrl->rect;
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				rect.bottom = lpWndCtrl->rect.bottom;
				rect.right = lpWndCtrl->rect.right;

				if(rect.PtInRect( point ))
				{
					if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( CItemElem::binds ) )
					{
						SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DRAG_AND_DROP);
						g_WndMng.m_pWndEquipBindConfirm->SetInformationDragAndDrop(pItemElem);
						g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
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

CWndTradeGold::CWndTradeGold() 
{ 
	m_nIdWndTo = APP_INVENTORY;
} 

BOOL CWndTradeGold::Process( void )
{
	CItemElem * pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
	if (!pItemBase) return TRUE;

	if (pItemBase->GetExtra() > 0) {
		Destroy();
	}

	return TRUE;
}

void CWndTradeGold::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_OK );	
	pWndOk->SetDefault( TRUE );
	pWndEdit->SetFocus();

	if( m_dwGold > INT_MAX )
		m_dwGold = INT_MAX;
	
	TCHAR szNumber[ 64 ];
	_itot( m_dwGold, szNumber, 10 );
	pWndEdit->SetString( szNumber );
}

BOOL CWndTradeGold::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return InitDialog( APP_TRADE_GOLD, pWndParent, WBS_MODAL, 0 );
}

BOOL CWndTradeGold::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CRect rect = GetClientRect();
		CWorld* pWorld	= g_WorldMng.Get();
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

		CString string = pWndEdit->GetString();
		int nCost = _ttoi( string );
		if( nCost < 1 )
			nCost = 1;
		if( m_nIdWndTo == APP_INVENTORY )
		{
			if( nCost > g_pPlayer->GetGold() )
				nCost = g_pPlayer->GetGold();

			g_DPlay.SendDropGold( nCost, g_pPlayer->GetPos(), m_vPos );
		}
		else
		if( m_nIdWndTo == APP_TRADE )
		{
			if( m_Shortcut.m_dwData == 0 ) // ��
			{
				if( nCost > g_pPlayer->GetGold() )
					nCost = g_pPlayer->GetGold();

				if( nCost > 0 )
				{
					g_DPlay.SendTradePutGold( nCost );
				}
				else
				{
					// error message
				}
			}
			else // ������
			{
				CItemElem * pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if (nCost > pItemBase->m_nItemNum) {
						nCost = pItemBase->m_nItemNum;
					}

					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendTradePut( (BYTE)( m_Shortcut.m_dwData ), 0, (BYTE)( m_Shortcut.m_dwId ), nCost );
				}
			}
		}
		else
		if( m_nIdWndTo == APP_BANK )
		{
			if( m_Shortcut.m_dwData == 0 ) // ��
			{
				if( nCost > g_pPlayer->GetGold() )
					nCost = g_pPlayer->GetGold();

				g_DPlay.SendPutGoldBank( m_nSlot, nCost );
			}
			else // ������
			{
				CItemElem * pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if (nCost > pItemBase->m_nItemNum) {
						nCost = pItemBase->m_nItemNum;
					}
					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendPutItemBank( m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
				}
			}
		}
		else
		if( m_nIdWndTo == APP_BAG_EX )
		{
			if( m_Shortcut.m_dwData != 0 )
			{
				
				if( nCost > (int)( ( m_dwGold ) ) )
				{
					nCost = m_dwGold;
				}
				m_Shortcut.m_dwData -= 100;
				//g_DPlay.SendPutItemBank( m_nSlot, m_Shortcut.m_dwId, nCost );
				g_DPlay.SendMoveItem_Pocket( m_nPutSlot, m_Shortcut.m_dwId, nCost , m_nSlot);
				
			}
		}
		// �ڽ��� �κ��丮���� ����?���� �������� �̵�
		else
		if (m_nIdWndTo == APP_GUILD_BANK)
		{
			if( m_Shortcut.m_dwData != 0 ) // ������
			{
				CItemElem * pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if (nCost > pItemBase->m_nItemNum) {
						nCost = pItemBase->m_nItemNum;
					}
					
					m_Shortcut.m_dwData -= 100;

					g_DPlay.SendPutItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 1 );

				}
			}
		}
		//	����?������ �ڽ��� �κ����� �����ö�
		//	APP_MESSENGER�� �׳� ����
		else
		if (m_nIdWndTo == APP_MESSENGER)
		{
			if (g_pPlayer->GetGuild())
			{
				if( m_Shortcut.m_dwData == 0 ) // ��
				{
					if( (DWORD)nCost > ( g_pPlayer->GetGuild()->m_nGoldGuild ) )
						nCost = g_pPlayer->GetGuild()->m_nGoldGuild;
					
					if( nCost > 0 )
					{
						g_DPlay.SendGetItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 0); // 0�� ��İ�? ����ƴٴ�? ���� �ǹ��Ѵ�.
					}
				}
				else // ������
				{
					CItemElem * pItemBase = nullptr;
						pItemBase = g_pPlayer->GetGuild()->m_GuildBank.GetAtId( m_Shortcut.m_dwId );


					if( pItemBase )
					{
						if (nCost > pItemBase->m_nItemNum) {
							nCost = pItemBase->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 1 ); // 1�� �������� ����ƴٴ�? ���� �ǹ��Ѵ�.
					}
				}
			}
		}

		else
		// �κ��丮 �ε� ���� �־ Gold�� ��.
		if( m_nIdWndTo == APP_GOLD ) // ��ũ���� �κ��丮�� ����
		{
			if( m_Shortcut.m_dwData == 0 ) // ��
			{
				if( nCost > (int)( g_pPlayer->m_dwGoldBank[m_nSlot] ) )
					nCost = g_pPlayer->m_dwGoldBank[m_nSlot];
				
				if( nCost > 0 )
				{
					g_DPlay.SendGetGoldBank( m_nSlot, nCost );
				}
				else
				{
					// error message
				}
			}
			else // ������
			{
				CItemElem * pItemBase = NULL;

					pItemBase = g_pPlayer->GetItemBankId( m_nSlot, m_Shortcut.m_dwId );

					if( pItemBase )
					{
						if( nCost > pItemBase->m_nItemNum )
						{
							nCost = pItemBase->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemBank( m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
					}

			}
		}
		else
		if( m_nIdWndTo == APP_COMMON_BANK )
		{
			if( m_Shortcut.m_dwData == 0 ) // ��
			{
				if( nCost > (int)( g_pPlayer->m_dwGoldBank[m_nPutSlot] ) )
					nCost = g_pPlayer->m_dwGoldBank[m_nPutSlot];
				
				if( nCost > 0 ) 
				{
					g_DPlay.SendPutGoldBankToBank( m_nPutSlot, m_nSlot, nCost );
				}
				else
				{
					// error message
				}
			}
			else // ������
			{
				CItemElem * pItemBase = NULL;

					pItemBase = g_pPlayer->GetItemBankId( m_nPutSlot, m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if (nCost > pItemBase->m_nItemNum) {
							nCost = pItemBase->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendPutItemBankToBank( m_nPutSlot, m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
					}

			}
		}
		else
		if( m_nIdWndTo == APP_POST_SEND )
		{
			if( m_Shortcut.m_dwData == 0 ) // ��
			{
				if( nCost > g_pPlayer->GetGold()  )
					nCost = g_pPlayer->GetGold();
				
				if( nCost > 0 ) 
				{
					// ������ â�� �ִ´�.
					if( g_WndMng.m_pWndPost )
					{
						g_WndMng.m_pWndPost->m_PostTabSend.SetCost(nCost);
					}
				}
				else
				{
					// error message
				}
			}
			else // ������
			{
				CItemElem * pItemBase = NULL;

					pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if (nCost > pItemBase->m_nItemNum) {
							nCost = pItemBase->m_nItemNum;
						}
						
						pItemBase->SetExtra( nCost );		

						m_Shortcut.m_dwData -= 100;

						if( g_WndMng.m_pWndPost )
						{
							g_WndMng.m_pWndPost->m_PostTabSend.SetItemId( (BYTE)( m_Shortcut.m_dwId ) );
							g_WndMng.m_pWndPost->m_PostTabSend.SetCount(nCost);
						}						
					}

			}
		}			
		Destroy();
	}
	if( nID == WIDC_CANCEL )
	{
		if( m_nIdWndTo == APP_POST_SEND )
		{
			g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(0xff);
			g_WndMng.m_pWndPost->m_PostTabSend.SetCount(0);
		}
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
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
BOOL CWndConfirmTrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
CWndTradeConfirm::CWndTradeConfirm() 
{ 
} 
CWndTradeConfirm::~CWndTradeConfirm() 
{ 
} 
void CWndTradeConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTradeConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CWndButton * pWndButtonOk = (CWndButton*)GetDlgItem( WIDC_YES );
	pWndButtonOk->SetVisible( TRUE );
	CWndButton * pWndButtonNO = (CWndButton*)GetDlgItem( WIDC_NO );
	pWndButtonNO->SetVisible( TRUE );

	bMsg = FALSE;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pWndStatic->SetTitle( prj.GetText( TID_DIAG_0083 ) );
					
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndTradeConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_TRADE_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndTradeConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( bMsg )
	{
		return( TRUE );
	}

	if( nID == WIDC_NO || nID == WTBID_CLOSE ) // ���? 
	{
		g_DPlay.SendTradeCancel();
	}
	else if ( nID == WIDC_YES )
	{
		g_DPlay.SendPacket<PACKETTYPE_TRADECONFIRM>();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
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
BOOL CWndTrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_TRADE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndTrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase( APP_TRADE_CONFIRM );
	if( pWndTradeConfirm )
	{
		return( TRUE );
	}
	if( message == WIN_ITEMDROP && ( nID == 10001 || nID == 10002 ) )
	{
		BOOL TradeGold = FALSE;
		SAFE_DELETE( g_WndMng.m_pWndTradeGold );
		g_WndMng.m_pWndTradeGold = new CWndTradeGold;
		memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
		if( g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwData == 0 ) // ��
		{
			TradeGold = TRUE;
 			g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->GetGold();
		}
		else // ������
		{
			CItemElem * pItemBase = g_pPlayer->GetItemId( g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwId );
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemBase->m_nItemNum;
		}
		g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_TRADE;
		g_WndMng.m_pWndTradeGold->m_pWndBase = this;
		
		if( TradeGold )
		{
			if( m_nGoldI == 0 )
			{
				g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
				g_WndMng.m_pWndTradeGold->MoveParentCenter();
				CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
				CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
				g_WndMng.m_pWndTradeGold->SetTitle( "Penya" );
				CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"�󸶸� �̵��Ͻðڽ��ϱ�?";
				CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
				pStatic->m_strTitle = strMain;
				pStaticCount->m_strTitle = strCount;
			}
			else
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
			}
		}
		else
		{
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();
			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			g_WndMng.m_pWndTradeGold->SetTitle( "Item" );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"��� �̵��Ͻðڽ��ϱ�?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" ���� : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
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
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
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

		FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
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
		END_LAND
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
		m_texNavObjs.Render( m_pApp->m_pd3dDevice, vertex, ( (int) pVertices - (int) vertex ) / sizeof( TEXTUREVERTEX ) );

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
	m_billArrow.Render( pd3dDevice );

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

	m_wndPlace.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtNavLeft.tga" ), TRUE );
	m_wndPlace.FitTextureSize();
	m_wndZoomIn.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtNavZoomIn.tga" ), TRUE );
	m_wndZoomIn.FitTextureSize();
	m_wndZoomOut.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtNavZoomOut.tga" ), TRUE );
	m_wndZoomOut.FitTextureSize();

	m_texDunFog.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "NavDunFog.tga" ), 0 , 1 );

	m_pDestinationPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtDestination.bmp"), 0xffff00ff );

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

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.top );
	Move( point );

	ResizeMiniMap();

}
BOOL CWndNavigator::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 0, 0, 115, 110 ); // 1024 768

	m_texNavObjs.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"Navigator.inc") );
	m_texArrow.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"ImgNavArrow.bmp"), 0xffff00ff );
	m_texNavPos.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "NavPosition.inc") );
	ZeroMemory( &m_billboard, sizeof( m_billboard ) );
	m_billboard.rect.SetRect( 0, 0, m_texArrow.m_size.cx, m_texArrow.m_size.cy );
	m_billboard.ptCenter = CPoint( m_texArrow.m_size.cx / 2, m_texArrow.m_size.cy / 2 );
	m_billArrow.InitDeviceObjects( g_Neuz.m_pd3dDevice, &m_billboard, &m_texArrow );
	m_billArrow.RestoreDeviceObjects();

	m_GuildCombatTextureMask.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_WORLD_GUILDCOMBAT, "WdGuildWar_Mask.dds" ), 0  );
	
	SetTitle( GETTEXT( TID_APP_NAVIGATOR ) );
	return CWndNeuz::InitDialog( dwWndId, pWndParent, 0, CPoint( 792, 130 ) );
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
					CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
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
					CLandscape* pLand;

					FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
					{
						for (CObj * pObj : pLand->m_apObjects[OT_MOVER].ValidObjs()) {
							CMover * mover = static_cast<CMover *>(pObj);
							CWndButton * pWndButton = m_wndMenuMover.AddButton(mover->GetId(), mover->GetName(TRUE));
							pWndButton->m_shortcut.m_dwShortcut = ShortcutType::Object;
						}
					}
					END_LAND
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
	CLandscape* pLand;

	bool hasTarget = false;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
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
	END_LAND

	if(hasTarget)
	{
		CRect rect = GetWindowRect( TRUE );
		CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
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
	
	CLandscape* pLand;
//	CWorld* pWorld	= g_WorldMng();

	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
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
	END_LAND

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
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x64ff0000, m_pVBHPGauge, &m_texGauFillNormal );
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
			m_bVBMPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x640000ff, m_pVBMPGauge, &m_texGauFillNormal );
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
			m_bVBFPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x6400ff00, m_pVBFPGauge, &m_texGauFillNormal );
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
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x847070ff, m_pVBEXPGauge, &m_texGauFillSmall );
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
			m_bVBAEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x84e6ce19, m_pVBAEXPGauge, &m_texGauFillSmall );
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
		m_pTheme->RenderWndBaseFrame( p2DRender, &rect );
		if( IsWndStyle( WBS_CAPTION ) )
		{
			// Ÿ��Ʋ �� 
			rect.bottom = 21;
			{
				m_pTheme->RenderWndBaseTitleBar( p2DRender, &rect, m_strTitle, m_dwColor );
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
//#ifdef __VCRITICAL
				m_bHPVisible = TRUE;
//#else
//			m_bHPVisible = TRUE;
//#endif
			if( m_bHPVisible )
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_texGauFillNormal );
		}
		if( m_bVBMPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBMPGauge, &m_texGauFillNormal );
		if( m_bVBFPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBFPGauge, &m_texGauFillNormal );
		if( g_pPlayer->IsAfterDeath() )
			m_bExpVisible = !m_bExpVisible;
		else
			m_bExpVisible = TRUE;

		if( m_bVBEXPGauge )
		{
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillSmall );
		}
		if( m_bExpVisible )
		{
/*
#if __VER < 8 // #ifndef __SKILL_WITHOUT_EXP
			if( m_bVBPXPGauge )
			{
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBPXPGauge, &m_texGauFillSmall );

			}
#endif // __VER < 8
*/
		}

		if( m_bVBAEXPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBAEXPGauge, &m_texGauFillSmall );
		


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
		//p2DRender->SetFont( m_pTheme->m_pFontWndTitle );

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


	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	ResetRenderState(pd3dDevice);

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

		g_pBipedMesh->Render(pd3dDevice,&matWorld);

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
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBMPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBFPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBAEXPGauge, NULL );
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
//	m_pModel = prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice,g_pPlayer->m_dwType,g_pPlayer->m_dwIndex);
//	prj.m_modelMng.LoadMotion(m_pModel,g_pPlayer->m_dwType,g_pPlayer->m_dwIndex,0);
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	//m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( DRAWVERTEX ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauEmptySmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillSmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	
	//m_texGauEmptySmall  
	//m_texGauFillNormal  
	//m_texGauFillSmall   
	
	// ����, �������� ���� ĳ���� ������Ʈ ���� 
	if( g_pBipedMesh == NULL )
	{
		int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		g_pBipedMesh = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( g_pBipedMesh,  OT_MOVER, nMover, 0 );
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, g_pBipedMesh, &g_pPlayer->m_Inventory );
	}
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top );
	Move( point );

}

BOOL CWndStatus::Initialize(CWndBase* pWndParent,DWORD dwWndId)
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
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );


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

BOOL CWndLogOut::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );

	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0069) ) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS����϶�?
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}

	// ������ �������� ������ �ϱ�
	Move70();

	return CWndMessageBox::Initialize( pWndParent, dwWndId );
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
						g_WndMng.m_pLogOutWaitting->Initialize( NULL );
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

BOOL CWndQuit::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
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
	
	return CWndMessageBox::Initialize( pWndParent, dwWndId );
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
						g_WndMng.m_pLogOutWaitting->Initialize( NULL );	// ˬ
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
BOOL CWndRevival::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
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
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODELIGHT );
				
				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );

				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
		case WIDC_REVIVAL_TO_LODESTAR:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODESTAR );

				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );
				
				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
		case WIDC_REVIVAL_STAND:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL );

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
class CWantedMessageBox : public CWndMessageBox
{ 
public: 
	int m_nGold;
	CString m_strMsg;

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWantedMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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

BOOL CWndReWanted::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
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
				g_WndMng.OpenCustomBox( "", pBox );
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

BOOL CWndWantedConfirm::Initialize(CWndBase *, DWORD) {
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

	if(!m_aList.empty())
	{
		const int listSize = static_cast<int>(m_aList.size());
		m_wndScrollBar.SetScrollRange(0, listSize);
		m_wndScrollBar.SetScrollPage(std::min(listSize, MAX_WANTED_PER_PAGE));
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	CString strTitle = GetTitle();
	CTime tm( m_recvTime );
	strTitle += tm.Format( prj.GetText(TID_PK_DATA_SHOWTIME) );
	SetTitle( strTitle );

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 

BOOL CWndWanted::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
		m_pWantedConfirm->Initialize( this, 0 );

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
BOOL CWndResurrectionConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
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

BOOL CWndCommItemDlg::Initialize(CWndBase * pWndParent, DWORD) {
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

BOOL CWndChangeClass1::Initialize(CWndBase * pWndParent, DWORD) {
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
		std::strcat(jobName, " ");
		if (prj.jobs.info[item].dwJobType == JTYPE_MASTER) {
			std::strcat(jobName, "[M]");
		} else {
			std::strcat(jobName, "[H]");
		}
		p2DRender->TextOut(rect.left, rect.top, jobName, color);
	}
}



void CWndInventory::RunUpgrade( CItemElem * pItem )
{
	if( m_bIsUpgradeMode )
	{
		m_bIsUpgradeMode = FALSE;

		if( pItem == NULL || m_pUpgradeMaterialItem == NULL )
		{
			g_WndMng.PutString(TID_UPGRADE_CANCLE);
			BaseMouseCursor();
			return;
		}
		
		ItemProp* pItemProp = m_pUpgradeMaterialItem->GetProp();

		if( !pItemProp )
			return;

		if( pItemProp->dwItemKind3 == IK3_SOCKETCARD || pItemProp->dwItemKind3 == IK3_SOCKETCARD2 )
		{
			if( !pItem->IsPierceAble( pItemProp->dwItemKind3 ) )
			{
				g_WndMng.PutString( prj.GetText( TID_PIERCING_POSSIBLE_ITEM ) );
				BaseMouseCursor();
				return;
			}
			
			int nCount = 0;
			for( int j = 0; j < pItem->GetPiercingSize(); j++ )
			{
				if(pItem->GetPiercingItem( j ) != 0 )
					nCount++;
			}

			// �����? ������ �ߴ�
			if( nCount == pItem->GetPiercingSize() )
			{
				g_WndMng.PutString(TID_PIERCING_ERROR_NOPIERCING);
				BaseMouseCursor();
				return;
			}
		}
		else
		if( pItemProp->dwItemKind3 == IK3_ELECARD )
		{
			if( !CItemElem::IsEleRefineryAble( pItem->GetProp()) )
			{
				g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
				BaseMouseCursor();
				return;
			}

			if(pItem->m_bItemResist != SAI79::NO_PROP && pItem->m_bItemResist != pItemProp->eItemType )
			{
				g_WndMng.PutString(TID_UPGRADE_ERROR_TWOELEMENT);
				BaseMouseCursor();
				return;
			}

			DWORD dwWantedCard	= 0;
			DWORD dwItemAO	= pItem->m_nResistAbilityOption;

			switch (pItemProp->eItemType)
			{
			case SAI79::FIRE:
				dwWantedCard	= II_GEN_MAT_ELE_FLAME;	break;
			case SAI79::WATER:
				dwWantedCard	= II_GEN_MAT_ELE_RIVER;	break;
			case SAI79::ELECTRICITY:
				dwWantedCard	= II_GEN_MAT_ELE_GENERATOR;	break;
			case SAI79::EARTH:
				dwWantedCard	= II_GEN_MAT_ELE_DESERT;	break;
			case SAI79::WIND:
				dwWantedCard	= II_GEN_MAT_ELE_CYCLON; break;
			default:
				dwWantedCard	= 0;	break;
			}

			if( pItemProp->dwID != dwWantedCard )
			{
				g_WndMng.PutString(TID_UPGRADE_ERROR_WRONGUPLEVEL);
				BaseMouseCursor();
				return;					
			}
		}
		else
		if( pItemProp->dwItemKind3 == IK3_ENCHANT )
		{
			if(m_pWndRemoveJewelConfirm != NULL)
			{
				if(m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId)
				{
					g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
					BaseMouseCursor();
					return;
				}
			}
			
			if(m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM02)
			{
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase( APP_SMELT_JEWEL );
				if(pWndSmeltJewel != NULL)
				{
					if(pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
						BaseMouseCursor();
						return;
					}
				}
			
				if(pItem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
				{
					g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
					BaseMouseCursor();
					return;
				}
			}
			else if( !CItemElem::IsDiceRefineryAble( pItem->GetProp()) )				
			{
				g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
				BaseMouseCursor();
				return;
			}
		}
		else
		if( pItemProp->dwItemKind3 == IK3_RANDOM_SCROLL )
		{
			if( !( pItem->GetProp()->dwItemKind1 == IK1_WEAPON || pItem->GetProp()->dwItemKind2 == IK2_ARMOR || pItem->GetProp()->dwItemKind2 == IK2_ARMORETC ) )
			{
				BaseMouseCursor();
				g_WndMng.PutString(TID_GAME_RANDOMSCROLL_ERROR);
				return;	
			}
		}
		else if( IsNeedTarget( pItemProp ) )
		{
			m_pUpgradeItem	= pItem;
			m_dwEnchantWaitTime		= g_tmCurrent + SEC(4);
			return;
		}
		else
		if( pItemProp->dwItemKind3 == IK3_PIERDICE )
		{
			if(m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE || m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE_1)
			{
				if(pItem->IsCollector( TRUE ) || pItem->GetProp()->dwItemKind2 == IK2_JEWELRY )
				{
					m_pUpgradeItem = pItem;
					m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
					return;
				}

				g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
				BaseMouseCursor();
				return;			

				BOOL checkJewel = FALSE;
					
				for(int i=0; i<5; i++)
				{
					if(pItem->GetUltimatePiercingItem( i ) != 0)
						checkJewel = TRUE;
				}
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase( APP_SMELT_JEWEL );
				if(pWndSmeltJewel != NULL)
				{
					if(pWndSmeltJewel->m_pItemElem && pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
						BaseMouseCursor();
						return;
					}
				}
				if(m_pWndRemoveJewelConfirm != NULL)
				{
					if(m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
						BaseMouseCursor();
						return;
					}
				}
				if( pItem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
				{
					g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
					BaseMouseCursor();
					return;
				}
				else
				{
					if(!checkJewel)
					{
						g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
						BaseMouseCursor();
						return;
					}
				}
				
				if(m_pWndRemoveJewelConfirm == NULL)
				{
					m_pWndRemoveJewelConfirm = new CWndRemoveJewelConfirm;
					m_pWndRemoveJewelConfirm->Initialize(this);
					m_pWndRemoveJewelConfirm->SetItem(pItem);

					m_bRemoveJewel = TRUE;
					return;
				}
			}
		}
		// ��þƮ�� �Ǵ� ������ - ���? ���?
		m_pUpgradeItem = pItem;
		m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
	}
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
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��ų ��й�? 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndPostItemWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 

void CWndPostItemWarning::SetString( char* string )
{
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( string );
		pWndEdit->EnableWindow( FALSE );	
	}
}

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPostItemWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_QUEITMWARNING );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_QUEITMWARNING ); 
} 

BOOL CWndPostItemWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		g_DPlay.SendQueryGetMailItem( m_nMailIndex );					
		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 





CWndPost::~CWndPost() 
{ 
#ifdef __MAIL_REQUESTING_BOX
	//SaveLastMailBox();
	CloseMailRequestingBox();
#endif // __MAIL_REQUESTING_BOX
} 

void CWndPost::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	EnableWindow( FALSE );

	//	���⿡ �ڵ��ϸ� �˴ϴ�
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	Windows::DestroyIfOpened(APP_BAG_EX);

	m_PostTabSend.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_SEND );
	m_PostTabReceive.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_RECEIVE );

	pWndTabCtrl->InsertItem(&m_PostTabSend, prj.GetText(TID_APP_POST_SEND));
	pWndTabCtrl->InsertItem(&m_PostTabReceive, prj.GetText(TID_APP_POST_RECEIVE));
	pWndTabCtrl->SetCurSel(1);

	MoveParentCenter();	

	// ������ ���� ���? ��û
	g_DPlay.SendQueryMailBox();	

#ifdef __MAIL_REQUESTING_BOX
	//CMailBox* pMailBox = CMailBox::GetInstance();
	//if( pMailBox )
	//{
	//	pMailBox->Clear();
	//}
	//LoadLastMailBox();
	//m_PostTabReceive.UpdateScroll();

	CloseMailRequestingBox();
	m_pWndMailRequestingBox = new CWndMailRequestingBox;
	m_pWndMailRequestingBox->Initialize();
#endif // __MAIL_REQUESTING_BOX
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPost::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( dwWndId, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndPost::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WNM_SELCHANGE )
	{
		CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

		if (pWndTabCtrl->GetSelectedTab() == &m_PostTabReceive) {
#ifndef __MAIL_REQUESTING_BOX
			m_PostTabSend.ClearData();
#endif // __MAIL_REQUESTING_BOX
			g_DPlay.SendQueryMailBox();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#ifdef __MAIL_REQUESTING_BOX
void CWndPost::LoadLastMailBox( void )
{
	CString strFileName = _T( "" );
	if( g_pPlayer == NULL )
	{
		return;
	}
	strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
	FILE* fp = fopen( strFileName, "rb" );
	if( fp == NULL )
	{
		Error( "LoadLastMailBox : fp == NULL" );
		return;
	}

	while( feof( fp ) == 0 )
	{
		CMail* pMail = new CMail;
		fread( &( pMail->m_nGold ), sizeof( DWORD ), 1, fp );
		fread( &( pMail->m_byRead ), sizeof( BYTE ), 1, fp );
		fread( &( pMail->m_szTitle ), sizeof( char ), MAX_MAILTITLE, fp );
		CMailBox::GetInstance()->AddMail( pMail );
	}

	fclose( fp );
}

void CWndPost::SaveLastMailBox( void )
{
	CMailBox* pMailBox = CMailBox::GetInstance();
	if( pMailBox == NULL )
	{
		return;
	}

	CMailBox& MailBox = *pMailBox;
	if( MailBox.empty() == true )
	{
		CString strFileName = _T( "" );
		if( g_pPlayer == NULL )
		{
			return;
		}
		strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
		DeleteFile( strFileName );

		return;
	}

	CString strFileName = _T( "" );
	if( g_pPlayer == NULL )
	{
		return;
	}
	strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
	FILE* fp = fopen( strFileName.GetBuffer(0), "wb" );
	if( fp == NULL )
	{
		Error( "SaveLastMailBox : fp == NULL" );
		return;
	}

	for( size_t i = 0; i < MailBox.size(); ++i )
	{
		fwrite( &( MailBox[ i ]->m_nGold ), sizeof( DWORD ), 1, fp );
		fwrite( &( MailBox[ i ]->m_byRead ), sizeof( BYTE ), 1, fp );
		fwrite( &( MailBox[ i ]->m_szTitle ), sizeof( char ), MAX_MAILTITLE, fp );
	}

	fclose( fp );
}
void CWndPost::CloseMailRequestingBox( void )
{
	if( m_pWndMailRequestingBox )
	{
		SAFE_DELETE( m_pWndMailRequestingBox );
	}
}
#endif // __MAIL_REQUESTING_BOX


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CWndPostSendMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndPostSendMessageBox : public CWndMessageBox
{ 
public: 
	CWndText	m_wndText;
	
	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];	
	char		m_szText[MAX_MAILTEXT];		
	void		SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndPostSendMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CString str;
	str.Format( prj.GetText(TID_MAIL_SEND_CONFIRM), MAX_KEEP_MAX_DAY );
	
	return CWndMessageBox::Initialize( str, 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndPostSendMessageBox::SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText )
{
	m_nItem		= nItem;
	m_nItemNum	= nItemNum;
	strcpy( m_szReceiver, lpszReceiver );
	strcpy( m_szTitle, lpszTitle );
	strcpy( m_szText, lpszText );
	m_nGold		= nGold;
}

BOOL CWndPostSendMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 1 )
	{
		if( g_pPlayer )
		{
			g_DPlay.SendQueryPostMail( m_nItem, m_nItemNum, m_szReceiver, m_nGold, m_szTitle, m_szText );			
			
			CWndPost * pWndPost = (CWndPost *)g_WndMng.GetWndBase( APP_POST );
			
			if( pWndPost )
			{
				pWndPost->m_PostTabSend.ClearData();
			}
		}
		
		Destroy();
	}
	else if( nID == 2 )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



CWndPostSend::~CWndPostSend() 
{ 
	ClearData();
} 

void CWndPostSend::ClearData()
{
	CItemElem * pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
	
	if(pItemElem)
	{
		// Ʈ���̵忡 �ɸ� �������� ����
		if( !g_pPlayer->m_vtInfo.IsTrading( pItemElem ) )
			pItemElem->SetExtra( 0 );
	}
	
	m_nItem		= 0xff;		
	m_nCost		= 0;
	m_nCount	= 0;
	/*
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
	
	pWndEdit2->SetString("");
	pWndEdit3->SetString("");
	pWndEdit4->SetString("");
	*/
}

void CWndPostSend::SetReceive( const char* pchar )
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );	
	pWndCombo->SetString( pchar );
}
void CWndPostSend::SetTitle( const char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::SetText( const char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nItem != 0xff )
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if( pItemElem )
		{
			LPWNDCTRL pCustom = NULL;
			pCustom = GetWndCtrl( WIDC_STATIC3 );
			
			pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

			if( pItemElem->GetProp()->dwPackMax > 1 && pItemElem->GetExtra() > 0 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
				
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", pItemElem->GetExtra() );
				p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff0000ff );
				p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xffb0b0f0 );

				p2DRender->SetFont( pOldFont );				
			}	
			
			//*
			CRect hitrect = pCustom->rect;
			CPoint point = GetMousePoint();
			if( hitrect.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( pItemElem, point2, &hitrect );
			}
			/**/
		}
	}
} 
void CWndPostSend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pNotice = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
	pNotice->AddWndStyle( EBS_WANTRETURN );
	pNotice->AddWndStyle( EBS_AUTOVSCROLL );

	// �Ʒ��� �����? �������̴�. CWndEdit�� SetWndRect���� ��ũ�ѹ��� visible���¿� ���� �����ϰ� ����
	// �θ�Ŭ������ CWndText�� OnInitialUpdate ȣ��Ǿ�? ��ũ�ѹ��� visible���¸� �����Ѵ�. �׷��� SetWndRect�� ��ȣ��....�Ѥ�;;;; �������� �����ϰ� ���Ҵ�.
	// �������ϴ� �޽��� �޼��� â�� ���? OnSize���� SetWndRect�� ȣ���ؼ� ���������� �����ߴ� ��.
	LPWNDCTRL pWndCtrl = GetWndCtrl( WIDC_EDIT3 );
	if(pWndCtrl)
		pNotice->SetWndRect(pWndCtrl->rect);

	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndCombo->SetTabStop( TRUE );
	pWndEdit2->SetTabStop( TRUE );
	pWndEdit3->SetTabStop( TRUE );


	for (const u_long idPlayer : g_WndMng.m_RTMessenger | std::views::keys) {
		pWndCombo->AddString(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
	}

	if( ::GetLanguage() != LANG_KOR )
	{
		CWndStatic* pStatic2	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
		CRect rect	= pStatic2->GetWndRect();
		rect.left	-= 14;
		rect.right	-= 14;
		pStatic2->SetWndRect( rect, TRUE );

		CWndEdit* pEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
		rect	= pEdit4->GetWndRect();
		rect.left	-= 14;
		rect.right	+= 10;
		pEdit4->SetWndRect( rect, TRUE );
	}

	pWndCombo->SetFocus();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPostSend::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( APP_POST_SEND, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndPostSend::SetItemId( BYTE nId )
{ 
	// ���� �������� �־����� Ȯ�� ����Ÿ�� �ʱ�ȭ �Ѵ�.
	if( nId != m_nItem && m_nItem != 0xff )
	{
		CItemElem* pItemElem  = g_pPlayer->m_Inventory.GetAtId( m_nItem );
		if( pItemElem )
		{
			pItemElem->SetExtra( 0 );				
		}
	}
	
	m_nItem = nId; 
}

BOOL CWndPostSend::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	
	// �������� �κ��丮���� �Դ°�?
	if( !(pShortcut->m_dwShortcut == ShortcutType::Item) && !(pWndFrame->GetWndId() == APP_INVENTORY) )
		return FALSE;

	if( pShortcut->m_dwData == 0 )
	{
		SetForbid( TRUE );		
		return FALSE;
	}
	
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString(TID_GAME_EQUIPPUT);
		SetForbid( TRUE );
		return FALSE;
	}
	
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt( pShortcut->m_dwIndex );
	
	if(pItemElem == NULL)
		return FALSE;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
		if( pItemElem->IsCharged() )//&& pItemElem->GetProp()->dwItemRare == 200 || pItemElem->GetProp()->dwItemRare == 300 )
		{
			g_WndMng.PutString(TID_GAME_CHARGED_NOTUSE);
			pItemElem = NULL;
			return FALSE;
		}
		
		if( pItemElem->m_nItemNum > 1 )
		{ 
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );

			g_WndMng.m_pWndTradeGold = new CWndTradeGold;
			memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pShortcut, sizeof(SHORTCUT) );
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemElem->m_nItemNum;
			g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_POST_SEND; // �κ��丮 �ε� ���� �־ Gold�� ��.
			g_WndMng.m_pWndTradeGold->m_pWndBase = this;
			g_WndMng.m_pWndTradeGold->m_nSlot = 0;
			
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->AddWndStyle( WBS_MODAL );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();

			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"��� �̵��Ͻðڽ��ϱ�?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" ���� : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
			g_WndMng.m_pWndTradeGold->SetTitle("");

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}
		}
		else
		{
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );
			pItemElem->SetExtra( 1 );				

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}			
		}
	}		
			
	return TRUE;
}

void CWndPostSend::OnRButtonUp( UINT nFlags, CPoint point )
{
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if(pItemElem)
		{
			pItemElem->SetExtra( 0 );				
		}

		m_nItem = 0xff;	
	}		
}

void CWndPostSend::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
}


#define MAX_BYTE_POST		128

BOOL CWndPostSend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_SEND:	
		{
			CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
			CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
			
			LPCTSTR szstr1 = pWndCombo->GetString();
			LPCTSTR szstr2 = pWndEdit2->GetString();
			LPCTSTR szstr3 = pWndEdit3->GetString();
			LPCTSTR szstr4 = pWndEdit4->GetString();
			
			// �����? �޴»����? ������ ������
			if( strlen( szstr1 ) <=0 || strlen( szstr2 ) <=0 )
			{
				return FALSE;
			}
			
			// �����۰� ���? �ΰ��� �ϳ��� ��� ������
			if( m_nItem == NULL_ID && strlen( szstr4 ) <=0 )
			{
				return FALSE;
			}

			// �ݾ׶��� ���ڰ� �ƴϸ� ������
			int nlen = strlen(szstr4);
			BOOL bisdigit = TRUE;

			if( nlen > 0 )
			{
				for( int i = 0 ; i < nlen ; i++ )
				{
					if( (isdigit2( szstr4[i] ) == FALSE) )
					{
						bisdigit = FALSE;
						break;
					}
				}
			}
			
			if( bisdigit == FALSE )
			{
				//g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_GUILDONLYNUMBER) );
				return FALSE;
			}

			__int64 liGold	= _atoi64(szstr4);
			if( liGold > 1000000000 )
			{
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LIMITPENYA) );
				return FALSE;				
			}
			
			if( strlen(szstr1) < 3 || strlen(szstr1) > 16 )
			{
				//�̸��� �ʹ� ��ϴ�?. �ٽ� �Է����ֽʽÿ�.
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_DIAG_0057) );
				return FALSE;				
			}

			if( strlen(szstr3) > ( MAX_MAILTEXT - 1 ) )		//gmpbigsun: ������ �������� 
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TEXT_MAX), MAX_MAILTEXT );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			if( strlen(szstr2) > ( MAX_MAILTITLE - 1 ) )	//gmpbigsun: ������ ��������
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TITLE_MAX), MAX_MAILTITLE );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			CWndPostSendMessageBox* pBox = new CWndPostSendMessageBox;

			if( pBox )
			{
				g_WndMng.OpenCustomBox( "", pBox );
				
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
				
				if( pItemElem )
				{
					pBox->SetValue( m_nItem, pItemElem->GetExtra(), (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
				else
				{
					pBox->SetValue( -1, 0, (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
			}
				
			break;
		}
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPostRead::CWndPostRead():m_nMailIndex(-1), m_bDrag(0), m_pWndPostItemWarning(0)
{ 
	m_pDeleteConfirm = NULL;
} 
CWndPostRead::~CWndPostRead() 
{ 
	ClearData();
//	SAFE_DELETE(m_pWndPostItemWarning);
} 

void CWndPostRead::ClearData()
{
	m_nMailIndex = -1;
	m_bDrag = FALSE;
}

void CWndPostRead::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( !mailbox[m_nMailIndex] )
		return;
	
	CItemElem* m_pItemElem = mailbox[m_nMailIndex]->m_pItemElem;

	if( m_pItemElem && m_pItemElem->GetTexture() )
	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC3 );

		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
			g_WndMng.PutToolTip_Item( m_pItemElem, point2, &hitrect );
		}

		m_pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

		if( m_pItemElem->GetProp()->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
			
			TCHAR szTemp[32];
			_stprintf( szTemp, "%d", m_pItemElem->m_nItemNum  );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
			p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff0000ff );
			p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xffb0b0f0 );

			p2DRender->SetFont( pOldFont );				
		}	
	}

	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC4 );
		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) && mailbox[m_nMailIndex]->m_nGold > 0 )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
		}
	}
} 
void CWndPostRead::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CRect rect	= pWndEdit3->GetWndRect();
	rect.right	+= 32;
	pWndEdit3->SetWndRect( rect, TRUE );

	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );
	

	pWndEdit1->EnableWindow(FALSE);
	pWndEdit2->EnableWindow(FALSE);
	pWndEdit3->EnableWindow(FALSE);
//	pWndText->EnableWindow(FALSE);

	pWndButton->SetFocus();
	MoveParentCenter();
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_STATIC4 );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_STATIC4 );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );
	
} 

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPostRead::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_READ );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_READ ); 
} 

BOOL CWndPostRead::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostRead::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostRead::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_bDrag = FALSE;
} 
void CWndPostRead::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
		return;	
	
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
		m_bDrag = TRUE;
	}
} 
void CWndPostRead::SetValue( int nMailIndex )
{
	m_nMailIndex	= nMailIndex;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail == NULL )
		return;

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	TCHAR szCutTitle[128];

	LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
	char lpszPlayer[MAX_PLAYER]		= { 0,};
	if( pMail->m_idSender == 0 )
		lstrcpy( lpszPlayer, "FLYFF" );
	else
		lstrcpy( lpszPlayer, lpszPlayerString );

	memset( szCutTitle, 0, sizeof(szCutTitle) );
	GetStrCut( lpszPlayer, szCutTitle, 16 );
	
	if( GetStrLen( lpszPlayer ) > 16 )
		_tcscat( szCutTitle, "..." );
	
	if( lstrlen( lpszPlayer ) > 0 )
		pWndEdit1->SetString(szCutTitle);
	else
		pWndEdit1->SetString("Unknown");
	
	memset( szCutTitle, 0, sizeof(szCutTitle) );
	GetStrCut( pMail->m_szTitle, szCutTitle, 13 );
	
	if( GetStrLen( pMail->m_szTitle ) >= 13 )
	{
		_tcscat( szCutTitle, "..." );
	}
	
	if( lstrlen( lpszPlayer ) > 0 )
	{
		pWndEdit2->SetString(szCutTitle);
		pWndText->SetString(pMail->m_szText);
	}
	else
	{
		pWndEdit2->SetString( "" );
		pWndText->SetString( "" );
	}

	g_DPlay.SendQueryReadMail( pMail->m_nMail );

	char szbuffer[128] = {0};
	sprintf( szbuffer, "%d", pMail->m_nGold );
	pWndEdit3->SetString(GetNumberFormatEx(szbuffer));
}
BOOL CWndPostRead::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}

void CWndPostRead::OnMouseMove(UINT nFlags, CPoint point )
{
	if( m_bDrag )
	{
		if( m_nMailIndex == -1 )
			return;
		
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nMailIndex];
		
		if( pMail && pMail->m_pItemElem && pMail->m_pItemElem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = ShortcutType::Item;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwId       = pMail->m_pItemElem->m_dwObjId;
			m_GlobalShortcut.m_pTexture   = pMail->m_pItemElem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) pMail->m_pItemElem;
			_tcscpy( m_GlobalShortcut.m_szString, pMail->m_pItemElem->GetProp()->szName);
		}
	}	
}

void CWndPostRead::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
		return;	

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
		MailReceiveItem();
	}
	
	pCustom = GetWndCtrl( WIDC_STATIC4 );

	if( PtInRect(&(pCustom->rect), point) )
	{
		MailReceiveGold();
	}
}
void CWndPostRead::MailReceiveItem()
{
	// �ش� �������� ���Ͽ��� �����´�.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail && pMail->m_pItemElem )
	{
		// �⺻ �����ϼ� ���������� �˻��Ͽ� ������ �ΰ��Ѵ�.
		int nDay = 0;
		DWORD dwTime = 0;
		pMail->GetMailInfo( &nDay, &dwTime );

		// �⺻ �����ϼ��� ������!!!
		if( (MAX_KEEP_MAX_DAY*24) - dwTime > (MAX_KEEP_BASIC_DAY*24) )
		{
			FLOAT fCustody = 0.0f;
			FLOAT fPay = 0.0f;
			fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );

			m_bDrag = FALSE;
//			SAFE_DELETE(m_pWndPostItemWarning);
			m_pWndPostItemWarning = new CWndPostItemWarning;
			m_pWndPostItemWarning->Initialize( this, 0 );
			m_pWndPostItemWarning->SetIndex( pMail->m_nMail );

			fPay = pMail->m_pItemElem->GetCost() * fCustody;
			if( fPay < 0.0f )
				fPay = 0.0f;

			TCHAR szChar[256] = { 0 };
			sprintf( szChar, prj.GetText(TID_GAME_MAILBOX_KEEP_PAY), MAX_KEEP_BASIC_DAY, (int)fPay );

			m_pWndPostItemWarning->SetString( szChar );
		}
		else
		{
			g_DPlay.SendQueryGetMailItem( pMail->m_nMail );								
		}
	}
}
void CWndPostRead::MailReceiveGold()
{
	// �ش� ��ĸ�? ���Ͽ��� �����´�.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( pMail && pMail->m_nMail > 0 && pMail->m_nGold > 0 )
	{
		g_DPlay.SendQueryGetMailGold( pMail->m_nMail );		
		
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
		pWndEdit->SetString("0");
	}	
}
BOOL CWndPostRead::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];

	if( !pMail )
		return FALSE;
	
	CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);

	if( pWndPost == NULL )
		return FALSE;

	switch( nID )
	{
	case WIDC_BUTTON1:	
		{
//			SAFE_DELETE(m_pDeleteConfirm);

			if(pMail->m_pItemElem && pMail->m_pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
				return FALSE;

			m_pDeleteConfirm = new CWndPostDeleteConfirm;

			if( m_pDeleteConfirm )
			{
				m_pDeleteConfirm->Initialize(this);
				m_pDeleteConfirm->SetValue(m_nMailIndex);			
			}
		}
		break;
	case WIDC_BUTTON2:	
		{
			CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			
			if( pWndEdit1 && pWndEdit2 )
			{
				if( stricmp( pWndEdit1->GetString(), "Unknow" ) == 0 )
				{
					g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_MAIL_UNKNOW) );
					return FALSE;
				}

				pWndPost->m_PostTabSend.SetItemId(0xff);
				pWndPost->m_PostTabSend.SetReceive("");
				pWndPost->m_PostTabSend.SetText("");
				pWndPost->m_PostTabSend.SetTitle("");
				
				CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)pWndPost->GetDlgItem( WIDC_TABCTRL1 );
				pWndTabCtrl->SetCurSel(0);	

				LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
				char	lpszPlayer[MAX_PLAYER]	= { 0, };
				if( pMail->m_idSender == 0 )
					lstrcpy( lpszPlayer, "FLYFF" );
				else
					lstrcpy( lpszPlayer, lpszPlayerString );

				pWndPost->m_PostTabSend.SetReceive( (char*)lpszPlayer );

				CString str = "Re:";
				str += pWndEdit2->GetString();
				pWndPost->m_PostTabSend.SetTitle(str.GetBuffer(0));
				Destroy();
			}
		}
		break;
	case WIDC_BUTTON3:	
		{
			Destroy();
		}
		break;
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndPostDeleteConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	CWndText* pWndText = (CWndText*)(GetDlgItem( WIDC_TEXT1 ));
	pWndText->SetString(_T( prj.GetText(TID_MAIL_DELETE_CONFIRM) ));	
	
	MoveParentCenter();
} 

BOOL CWndPostDeleteConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_DELETE_CONFIRM );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_DELETE_CONFIRM ); 
} 

BOOL CWndPostDeleteConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nIndex];
		
		if( pMail )
		{
			g_DPlay.SendQueryRemoveMail( pMail->m_nMail );						
			Destroy(TRUE);

			CWndPostRead* pWndPost = (CWndPostRead*)g_WndMng.GetWndBase(APP_POST_READ);
			if(pWndPost)
				pWndPost->Destroy();
			
		}
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPostReceive::~CWndPostReceive() 
{ 
#ifdef __FIX_WND_1109
	DeleteDeviceObjects();
#endif	// __FIX_WND_1109
} 
HRESULT CWndPostReceive::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	m_Texture[0].DeleteDeviceObjects();
	m_Texture[1].DeleteDeviceObjects();
	m_Texture[2].DeleteDeviceObjects();

#ifdef __FIX_WND_1109
	SAFE_DELETE( m_wndPostRead );
#endif	// __FIX_WND_1109

	return TRUE;
}
int CWndPostReceive::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y-23) / 40;
	
	if( 0 <= nIndex && nIndex < MAX_MAIL_LIST_PER_PAGE ) 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}
void CWndPostReceive::UpdateScroll()
{
	CMailBox* pMailBox	= CMailBox::GetInstance();
	m_nMax = pMailBox->size();
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}	
}
void CWndPostReceive::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	
	sx = 9;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	TCHAR szTemp[32];

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture[0], 150  );

	// �Ⱓ ���� ��ȭ
	D3DXVECTOR2 v2_1 = D3DXVECTOR2( 255, 0 );
	D3DXVECTOR2 v2_2 = D3DXVECTOR2( 0, 150 );
	D3DXVECTOR2 v2Result;

	TCHAR szCutTitle[128];
	

	COleDateTime dt;
	CString strDateTime;

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( (int)( mailbox.size() ) <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 32, 190, 0 , 0 ) );
		}
		
		// �������� ������ ���?
		if( mailbox[i]->m_pItemElem && mailbox[i]->m_pItemElem->m_pTexture )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), mailbox[i]->m_pItemElem->m_pTexture );

			if( mailbox[i]->m_pItemElem->GetProp()->dwPackMax > 1 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
				
				_stprintf( szTemp, "%d", mailbox[i]->m_pItemElem->m_nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xff0000ff );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xffb0b0f0 );
				
				p2DRender->SetFont( pOldFont );			
			}
		}
		else
		// �������� ���� ���� ������ �����? ���?
		if( mailbox[i]->m_pItemElem == NULL && mailbox[i]->m_nGold > 0 )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), &m_Texture[2] );
		}

		// ������ �о��ٸ� ���� ǥ��
		if( mailbox[i]->m_byRead )
		{
			p2DRender->RenderTexture( CPoint( sx+10, sy+1 ), &m_Texture[1], 110 );
		}		

		const char* lpszPlayerString	= CPlayerDataCenter::GetInstance()->GetPlayerString( mailbox[i]->m_idSender );
		char lpszPlayer[MAX_PLAYER]	= { 0, };
		if( mailbox[i]->m_idSender == 0 )
			lstrcpy( lpszPlayer, "FLYFF" );
		else
			lstrcpy( lpszPlayer, lpszPlayerString );
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( lpszPlayer, szCutTitle, 16 );
		
		if( GetStrLen( lpszPlayer ) > 16 )
			_tcscat( szCutTitle, "..." );

		if( lstrlen( lpszPlayer ) > 0 )
		{
			p2DRender->TextOut( sx + 60,  sy+3, szCutTitle ,  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 60,  sy+3, "Unknown",  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}

		int nDay = 0;
		DWORD dwTime = 0;
		mailbox[i]->GetMailInfo( &nDay, &dwTime );  // �������� �˻�

		CString szDay;

		// �Ϸ� ���� ������ �ð����� ǥ�����ش�.
		if( nDay <= 1 )
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_HOUR), dwTime );
		}
		else
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_DAY), nDay );
		}
		
		FLOAT f = (FLOAT)( (FLOAT)nDay / (FLOAT)MAX_KEEP_BASIC_DAY );
		D3DXVec2Lerp( &v2Result, &v2_1, &v2_2, f );

		if( nDay <= 0 )
			p2DRender->TextOut( sx + 190,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		else
			p2DRender->TextOut( sx + 193,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( mailbox[i]->m_szTitle, szCutTitle, 13 );
		
		if( GetStrLen( mailbox[i]->m_szTitle ) >= 13 )
		{
			_tcscat( szCutTitle, "..." );
		}
		
//		if( GetFocusWnd() == this )
		{
			CRect  rect;
			rect.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			CPoint point	= GetMousePoint();
			if( PtInRect( rect, point ) )
			{
				ClientToScreen( &point );
				ClientToScreen( &rect );
				dt = mailbox[i]->m_tmCreate;
				strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
				g_toolTip.PutToolTip( (DWORD)this, strDateTime, rect, point );
			}		
		}
		
		if( lstrlen( lpszPlayer ) > 0 )
			p2DRender->TextOut( sx + 60,  sy+18, szCutTitle,  D3DCOLOR_XRGB( 0, 0, 190 ) );
		else
			p2DRender->TextOut( sx + 60,  sy+18, "",  D3DCOLOR_XRGB( 0, 0, 190 ) );

		sy += 40;
	}	
} 

void CWndPostReceive::OnMouseWndSurface( CPoint point )
{
	int	sx, sy;
	
	sx = 15;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	COleDateTime dt;
	CString strDateTime;

	CPoint cpoint	= point;

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( (int)( mailbox.size() ) <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

//		if( GetFocusWnd() == this )
		{			
			rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			if( PtInRect( rc, cpoint ) )
			{
				ClientToScreen( &cpoint );
				ClientToScreen( &rc );
				dt = mailbox[i]->m_tmCreate;
				strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
				g_toolTip.PutToolTip( (DWORD)this, strDateTime, rc, cpoint );
				break;
			}		
		}
		
		sy += 40;
	}	
}

void CWndPostReceive::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect == -1 )
		return;
	
	m_nSelect = nSelect;	

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( mailbox[m_nSelect] == NULL )
		return;

#ifdef __FIX_WND_1109
	SAFE_DELETE( m_wndPostRead );
#endif	// __FIX_WND_1109

	m_wndPostRead = new CWndPostRead;

	if( m_wndPostRead )
	{
#ifdef __FIX_WND_1109
		m_wndPostRead->Initialize( this );
#else	// __FIX_WND_1109
		m_wndPostRead->Initialize();
#endif	// __FIX_WND_1109
		m_wndPostRead->SetValue( m_nSelect );
	}
}

void CWndPostReceive::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 15;
	rect.bottom -= 50;
	rect.left    = rect.right - 40;
	rect.right   -= 30;
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}
	
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
	
	m_Texture[0].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndPostTable.tga" ), 0xffff00ff );	
	m_Texture[1].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndNotUse.tga" ), 0xffff00ff );	
	m_Texture[2].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "item\\", "itm_GolGolSeed.dds" ), 0xffff00ff );	
	
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPostReceive::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( APP_POST_RECEIVE, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndPostReceive::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;	
} 
BOOL CWndPostReceive::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndPostReceive::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// CWndGuildCombatOfferMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndGuildCombatOfferMessageBox : public CWndMessageBox
{ 
public: 
	DWORD m_nCost;
	void	SetValue( CString str, DWORD nCost );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndGuildCombatOfferMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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

BOOL CWndGuildCombatOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
			if( pMsg )
			{
				g_WndMng.OpenCustomBox( "", pMsg, this );
				CString str;

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

BOOL CWndGuildCombatBoard::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

BOOL CGuildCombatInfoMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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

BOOL CGuildCombatInfoMessageBox2::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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
BOOL CGuildCombatSelectionClearMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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

			if( pGuildCombatSelection )
				pGuildCombatSelection->Reset();

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

GuildCombatPlayer::GuildCombatPlayer(u_long playerId)
	: playerId(playerId) {
	PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(playerId);
	display.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.jobs.info[pPlayerData->data.nJob].szName);
}

void GuildCombatPlayer::Render(
	C2DRender * p2DRender, CRect rect,
	DWORD color, const WndTListBox::DisplayArgs & misc
) const {
	p2DRender->TextOut(rect.left, rect.top, display.GetString(), color);
}

// ���? �Ĺ� ������ ����
CWndGuildCombatSelection::CWndGuildCombatSelection() {
	m_uidDefender = -1;

	nMaxJoinMember = 0;
	nMaxWarMember = 0;
}

CWndTListBox<GuildCombatPlayer> & CWndGuildCombatSelection::SelectablePlayers() {
	return *GetDlgItem<CWndTListBox<GuildCombatPlayer>>(WIDC_LISTBOX1);
}

CWndTListBox<GuildCombatPlayer> & CWndGuildCombatSelection::CombatPlayers() {
	return *GetDlgItem<CWndTListBox<GuildCombatPlayer>>(WIDC_LISTBOX2);
}

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

void CWndGuildCombatSelection::UpDateGuildListBox() {
	auto & selectableListbox = SelectablePlayers();
	selectableListbox.ResetContent();

	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return;

	for (const u_long playerId : pGuild->m_mapPMember | std::views::keys) {
		PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(playerId);
		if (pPlayerData->data.uLogin <= 0) continue;

		selectableListbox.Add(GuildCombatPlayer(playerId));
	}
}

void CWndGuildCombatSelection::ReceiveLineup(const std::vector<u_long> & members, u_long defenderId) {
	CWndTListBox<GuildCombatPlayer> & connectedPlayers = CombatPlayers();
	CWndTListBox<GuildCombatPlayer> & connectedMmebers = SelectablePlayers();
	connectedPlayers.ResetContent();

	for (const u_long member : members) {
		connectedPlayers.Add(GuildCombatPlayer(member));

		const auto old = connectedMmebers.Find(GuildCombatPlayer::ById(member));
		if (old != -1) {
			connectedMmebers.Erase(old);
		}
	}

	m_uidDefender = defenderId;
}

void CWndGuildCombatSelection::OnDraw( C2DRender* p2DRender )  {
	if (CWndBase * pWndStatic = GetDlgItem(WIDC_BUTTON8)) {
		pWndStatic->EnableWindow(FALSE);

		if (m_uidDefender <= 0) {
			pWndStatic->SetVisible(FALSE);
		} else {
			auto & combatPlayers = CombatPlayers();

			const int position = combatPlayers.Find(
				GuildCombatPlayer::ById(m_uidDefender)
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

void CWndGuildCombatSelection::EnableFinish(BOOL bFlag) {
	if (CWndBase * pWndButton = GetDlgItem(WIDC_FINISH)) {
		pWndButton->EnableWindow(bFlag);
	}
}

void CWndGuildCombatSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();

	ReplaceListBox<GuildCombatPlayer>(WIDC_LISTBOX1);
	ReplaceListBox<GuildCombatPlayer>(WIDC_LISTBOX2);

	// �ð� ���������� �Ǵ�
	if (g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP)); //������ �����ۼ��� �� �� �����ϴ�.
		Destroy();
		return;
	}
	
	MoveParentCenter();
	UpDateGuildListBox();
} 

BOOL CWndGuildCombatSelection::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_GUILDCOMBAT_SELECTION, pWndParent, 0, CPoint(0, 0));
}

void CWndGuildCombatSelection::Reset() {
	m_uidDefender = 0;
	UpDateGuildListBox();
	CombatPlayers().ResetContent();
}

BOOL CWndGuildCombatSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) { 
	static constexpr auto ToBOOL = [](bool b) -> BOOL { return b ? TRUE : FALSE; };

	switch (nID) {
		case WIDC_BUTTON1: return ToBOOL(OnConnectedToCombat());
		case WIDC_BUTTON2: return ToBOOL(OnCombatToConnected());
		case WIDC_BUTTON3: return ToBOOL(OnMoveUp());
		case WIDC_BUTTON4: return ToBOOL(OnMoveDown());
		case WIDC_RESET: {
			CGuildCombatSelectionClearMessageBox * pBox = new CGuildCombatSelectionClearMessageBox;
			g_WndMng.OpenCustomBox("", pBox);
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

bool CWndGuildCombatSelection::OnConnectedToCombat() {
	CWndTListBox<GuildCombatPlayer> & connectedPlayers = SelectablePlayers();
	const auto [nCurSel, selPlayer] = connectedPlayers.GetSelection();
	if (nCurSel == -1) return false;

	if (connectedPlayers.GetSize() > nMaxJoinMember) {
		CString str;
		str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_SELECTION_MAX), nMaxJoinMember);
		g_WndMng.OpenMessageBox(str);
		return false;
	}

	CGuild * pGuild = g_pPlayer->GetGuild();

	if (pGuild) {
		CGuildMember * pGuildMember = pGuild->GetMember(selPlayer->playerId);

		if (!pGuildMember) {
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_MEMBER));	//�������� �����ϴ� �����? �ɹ��� �ƴմϴ�.			
			return false;
		}

		if (CPlayerDataCenter::GetInstance()->GetPlayerData(selPlayer->playerId)->data.nLevel < 30) {
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_LEVEL_NOTICE)); //������ �����? ���� 30�̻��� �Ǿ��? �մϴ�.
			return false;
		}
	}

	CWndTListBox<GuildCombatPlayer> & combatPlayers = CombatPlayers();
	const int combatPos = combatPlayers.Find(GuildCombatPlayer::ById(selPlayer->playerId));
	if (combatPos != -1) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_ALREADY_ENTRY));
		return false;
	}

	combatPlayers.Add(GuildCombatPlayer(selPlayer->playerId)); // Rebuild to update text
	connectedPlayers.Erase(nCurSel);

	return true;
}

bool CWndGuildCombatSelection::OnCombatToConnected() {
	// Find selected player in combat players
	CWndTListBox<GuildCombatPlayer> & combatPlayers = CombatPlayers();
	const auto [nCurSel, selPlayer] = combatPlayers.GetSelection();
	if (nCurSel == -1) return false;

	// Add back in connected players
	CWndTListBox<GuildCombatPlayer> & connectedPlayers = SelectablePlayers();
	const int coPlayer = connectedPlayers.Find(
		GuildCombatPlayer::ById(selPlayer->playerId)
	);

	if (coPlayer == -1) {
		SelectablePlayers().Add(GuildCombatPlayer(selPlayer->playerId));
	}

	// Remove defender
	if (m_uidDefender == selPlayer->playerId) {
		m_uidDefender = -1;
	}

	// Remove from list
	combatPlayers.Erase(nCurSel);
	return true;
}

bool CWndGuildCombatSelection::OnMoveUp() {
	CWndTListBox<GuildCombatPlayer> & combatPlayers = CombatPlayers();

	const int nCurSel = combatPlayers.GetCurSel();
	if (nCurSel == -1 || nCurSel == 0) return false;

	std::swap(combatPlayers[nCurSel - 1], combatPlayers[nCurSel]);
	combatPlayers.SetCurSel(nCurSel - 1);
	return true;
}

bool CWndGuildCombatSelection::OnMoveDown() {
	CWndTListBox<GuildCombatPlayer> & combatPlayers = CombatPlayers();

	const int nCurSel = combatPlayers.GetCurSel();
	if (nCurSel == -1 || std::cmp_equal(nCurSel + 1, combatPlayers.GetSize()))
		return false;

	std::swap(combatPlayers[nCurSel], combatPlayers[nCurSel + 1]);
	combatPlayers.SetCurSel(nCurSel + 1);
	return true;
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

	CWndTListBox<GuildCombatPlayer> & combatPlayers = CombatPlayers();
	if (combatPlayers.GetSize() == 0) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_PLAYER)); //�����ڰ� �����ϴ�. �����ڸ� �������ּ���.
		return false;
	}

	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) {
		return false;
	}

	std::vector<u_long> selected;
	for (size_t i = 0; i != combatPlayers.GetSize(); ++i) {
		const u_long p = combatPlayers[static_cast<int>(i)].playerId;
		selected.emplace_back(p);
	}

	g_DPlay.SendGCSelectPlayer(selected, m_uidDefender);
	Destroy();
	return true;

	// Message if no guild master or general -> we do not care
	// g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_MASTER)); //������ ���ܿ� ��帶���ͳ�? ŷ���� �������� �ʽ��ϴ�.
	// return FALSE;
}

bool CWndGuildCombatSelection::OnChooseDefender() {
	CWndTListBox<GuildCombatPlayer> & combatPlayers = CombatPlayers();

	const int nCurSel = combatPlayers.GetCurSel();
	if (nCurSel == -1)
		return false;

	SetDefender(combatPlayers[nCurSel].playerId);
	return true;
}

//������ ���� ���? ���� ���?
CWndGuildCombatState::CWndGuildCombatState(int nCombatType)
{ 
	Init( 0 );

	m_tEndTime    = 0;
	m_tCurrentTime = 0;
	m_nCombatType = nCombatType;
}

CWndGuildCombatState::~CWndGuildCombatState() 
{ 
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

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

void CWndGuildCombatState::Init( time_t lTime )
{
}

void CWndGuildCombatState::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 

void CWndGuildCombatState::InsertTitle( const char szTitle[] )
{
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle(), szTitle );
	SetTitle( strTitle );
}

BOOL CWndGuildCombatState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_1TO1_OFFERSTATE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombatState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatState::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// ���õ� �ε����� ��´�?.
int CWndGuildCombatState::GetSelectIndex( const CPoint& point )
{
	return -1;
}

void CWndGuildCombatState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatState::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildCombatState::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatState::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
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
		m_ct = m_tCurrentTime;//long)(m_dwCurrentTime / 1000.0f) );
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

BOOL CWndGuildCombatJoinSelection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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

BOOL CWndGuildWarAppConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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

BOOL CWndGuildWarCancelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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

BOOL CWndGuildWarJoinConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
	

//������ ���� ���? ���� ���?

CWndGuildWarState::CWndGuildWarState() 
{ 
	Init( 0 );
} 

CWndGuildWarState::~CWndGuildWarState() 
{ 
} 

void CWndGuildWarState::Init( time_t lTime )
{
	memset( m_aList, 0, sizeof(m_aList) );
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildWarState::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 

void CWndGuildWarState::InsertTitle( const char szTitle[] )
{
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle(), szTitle );
	SetTitle( strTitle );
}

BOOL CWndGuildWarState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILD_WAR_STATE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildWarState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarState::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// ���õ� �ε����� ��´�?.
int CWndGuildWarState::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE ) // 0 - 19���� 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndGuildWarState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildWarState::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildWarState::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarState::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildWarState::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildWarState::OnMouseWndSurface( CPoint point )
{
}	
void CWndGuildWarState::InsertGuild( const char szGuild[], const char szName[], int nNum )
{
	if( m_nMax >= MAX_GUILDCOMBAT_LIST )
	{
		Error( "CWndGuildWarState::InsertGuild - range over" );
		return;
	}
	
	if( nNum == 0 )
		return;

	GetStrCut( szGuild, m_aList[m_nMax].szGuild, 8 );

	if( GetStrLen( szGuild ) >= 8 )
	{
		_tcscat( m_aList[m_nMax].szGuild, "..." );
	}
	else
	{
		_tcscpy( m_aList[m_nMax].szGuild, szGuild );
	}

	_tcscpy( m_aList[m_nMax].szName, szName );
	m_aList[m_nMax].nNum = nNum;

	m_nMax++;
}

void CWndGuildWarState::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+310, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		sprintf( szNum, "%3d", i+1 );
		sprintf( szCount, "%2d", m_aList[i].nNum );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+295, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		p2DRender->TextOut( sx + 4,    sy, szNum,             dwColor );
		p2DRender->TextOut( sx + 30,   sy, m_aList[i].szGuild, dwColor );
		p2DRender->TextOut( sx + 160,  sy, m_aList[i].szName,  dwColor );
		p2DRender->TextOut( sx + 260,  sy, szCount,    dwColor );

		sy += 18;
	}
} 
BOOL CWndGuildWarState::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}




//����Ĺ�? ��ŷ

CWndGuildCombatRanking::CWndGuildCombatRanking() 
{ 
	Init( 0 );
} 

CWndGuildCombatRanking::~CWndGuildCombatRanking() 
{ 
} 

void CWndGuildCombatRanking::Init( time_t lTime )
{
	m_multimapRanking.clear();
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildCombatRanking::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 

BOOL CWndGuildCombatRanking::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_RANKING, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombatRanking::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatRanking::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatRanking::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// ���õ� �ε����� ��´�?.
int CWndGuildCombatRanking::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE ) // 0 - 19���� 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndGuildCombatRanking::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildCombatRanking::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildCombatRanking::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatRanking::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatRanking::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildCombatRanking::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatRanking::OnMouseWndSurface( CPoint point )
{
}	
void CWndGuildCombatRanking::InsertGuild( const char szGuild[], int nWinCount )
{
	if( m_nMax >= MAX_GUILDCOMBAT_LIST )
	{
		Error( "CWndGuildWarState::InsertGuild - range over" );
		return;
	}

	GUILDNAME p;
	memset( &p, 0, sizeof(GUILDNAME) );
	GetStrCut( szGuild, p.szGuild, 8 );
	
	if( GetStrLen( szGuild ) >= 8 )
	{
		_tcscat( p.szGuild, "..." );
	}
	else
	{
		_tcscpy( p.szGuild, szGuild );
	}
	
	m_multimapRanking.emplace(nWinCount, p);

	m_nMax++;
}

void CWndGuildCombatRanking::OnDraw( C2DRender* p2DRender ) 
{ 
	DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+230, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	int nRanking = 0;
	int nOldRanking = -1;
	auto iterRobotBegin = m_multimapRanking.rbegin();
	
	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		auto & refValue = *iterRobotBegin;
		
		if( nOldRanking != refValue.first )
			nRanking++;

		sprintf( szNum, "%3d", nRanking );
		sprintf( szCount, "%2d", refValue.first );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+215, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		if( i == 0 )
			dwColor = D3DCOLOR_XRGB( 200 , 0 , 0 );
		else
			dwColor = D3DCOLOR_XRGB( 0 , 0 , 0 );		

		if( nOldRanking != refValue.first )
		{
			p2DRender->TextOut( sx + 4,    sy, szNum,  dwColor );
		}
		else
		{
			p2DRender->TextOut( sx + 5,    sy, "   ",  dwColor );
		}

		if( i == 0 )
		{
			p2DRender->TextOut( sx + 40,  sy, refValue.second.szGuild,  D3DCOLOR_XRGB( 200 , 0 , 0 ) );
			p2DRender->TextOut( sx + 180,  sy, szCount,    D3DCOLOR_XRGB( 200 , 0 , 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 40,  sy, refValue.second.szGuild,  dwColor );
			p2DRender->TextOut( sx + 180,  sy, szCount,    dwColor );
		}
		

		sy += 18;

		iterRobotBegin++;
		nOldRanking = refValue.first;			
	}
} 
BOOL CWndGuildCombatRanking::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

void CWndGuildCombatRanking::SortRanking()
{

}


/****************************************************
  WndId : APP_GUILDCOMBAT_RESULT_POINT - ������ ����?
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndGuildCombatResult::CWndGuildCombatResult() 
{ 
} 
CWndGuildCombatResult::~CWndGuildCombatResult() 
{ 
} 
void CWndGuildCombatResult::OnDraw( C2DRender* p2DRender ) 
{ 
} 

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
BOOL CWndGuildCombatResult::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_RESULT_POINT, pWndParent, 0, CPoint( 0, 0 ) );
} 
BOOL CWndGuildCombatResult::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildCombatResult::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildCombatResult::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildCombatResult::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildCombatResult::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatResult::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatResult::InsertGuildRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX1 ));
	pWndList->AddString( str );	
}

void CWndGuildCombatResult::InsertPersonRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX2 ));
	pWndList->AddString( str );		
}

void CWndGuildCombatResult::InsertLog(CString str)
{
//	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_LISTBOX1 ));
//	pWndList->AddString( str );	
	
	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_TEXT1 ));
	pWndText->AddString( str );			
//	pWndList->AddString( str );			
	
}


/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT
****************************************************/
void CWndGuildCombatTabResultRate::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
BOOL CWndGuildCombatTabResultRate::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_TAB_RESULT, pWndParent, 0, CPoint( 0, 0 ) );
} 

	
/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT_LOG
****************************************************/
void CWndGuildCombatTabResultLog::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
BOOL CWndGuildCombatTabResultLog::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_TAB_RESULT_LOG, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndGuildCombatTabResultLog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
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
CWndGuildCombatRank_Person::CWndGuildCombatRank_Person() 
{ 
	m_mTotalRanking.clear();
} 
CWndGuildCombatRank_Person::~CWndGuildCombatRank_Person() 
{ 
} 
void CWndGuildCombatRank_Person::OnDraw( C2DRender* p2DRender ) 
{ 
} 

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
BOOL CWndGuildCombatRank_Person::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_RANK_P, pWndParent, 0, CPoint( 0, 0 ) );
} 
BOOL CWndGuildCombatRank_Person::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildCombatRank_Person::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildCombatRank_Person::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildCombatRank_Person::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildCombatRank_Person::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatRank_Person::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatRank_Person::InsertRank( int nJob, u_long	uidPlayer, int nPoint )
{
	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	GcRankInfo.nJob      = nJob;
	GcRankInfo.uidPlayer = uidPlayer;
	m_mTotalRanking.emplace(nPoint, GcRankInfo);
}

void CWndGuildCombatRank_Person::DivisionList()
{
	if( m_mTotalRanking.size() <= 0 )
		return;
	
	int nPoint;

	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	CWndGuildCombatRank_Class * pRankTot;
	CWndGuildCombatRank_Class * pRank;

	// ��ü��Ͽ�? ���?
	pRankTot = &(m_WndGuildCombatTabClass_Tot);

	// �������� ���?
	pRank = NULL;
	for( auto i = m_mTotalRanking.rbegin(); i != m_mTotalRanking.rend(); ++i )
	{ 
		nPoint			= i->first;
		GcRankInfo		= i->second;
		
		pRank = __GetJobKindWnd( GcRankInfo.nJob );
		
		if( pRank == NULL )
			continue;

		pRank->InsertRank( GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint );
		pRankTot->InsertRank( GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint );
	}
}

void CWndGuildCombatRank_Person::UpdateList()
{

}

void CWndGuildCombatRank_Person::UpdatePlayer(int nJob, u_long idPlayer)
{

}

CWndGuildCombatRank_Class* CWndGuildCombatRank_Person::__GetJobKindWnd(const int nJob) {
	switch (prj.jobs.GetProJob(nJob)) {
		case Project::ProJob::Mercenary: return &m_WndGuildCombatTabClass_Mer;
		case Project::ProJob::Acrobat:   return &m_WndGuildCombatTabClass_Acr;
		case Project::ProJob::Assist:    return &m_WndGuildCombatTabClass_Ass;
		case Project::ProJob::Magician:  return &m_WndGuildCombatTabClass_Mag;
		default:                          return nullptr;
	}
}

/****************************************************
  WndId : APP_GUILDCOMBAT_RANKINGCLASS
****************************************************/
CWndGuildCombatRank_Class::CWndGuildCombatRank_Class() 
{ 
	m_nRate = 0;
	m_nOldRate = -1;
	m_nMax = 0;
	m_nSelect = -1;	
} 
CWndGuildCombatRank_Class::~CWndGuildCombatRank_Class() 
{ 
} 
void CWndGuildCombatRank_Class::OnDraw( C2DRender* p2DRender ) 
{ 
	DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];
	
	sx = 8;
	sy = 35;	
	
	CRect rc( sx, 5, sx+330, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );
	
	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	int nRanking = 0;
	int nOldRanking = -1;
	
	__GUILDCOMBAT_RANK_INFO2 GCRankInfo;
	__GUILDCOMBAT_RANK_INFO2 GCRankInfoMy;
	int	nMyRanking = 0;
	
	CString szName;
	CString szJob;

	BOOL	bMyRanking = FALSE;
	
	p2DRender->TextOut( sx + 4,      10, prj.GetText(TID_GAME_RATE),  dwColor );
	p2DRender->TextOut( sx + 40,     10, prj.GetText(TID_GAME_NAME),  dwColor );
	p2DRender->TextOut( sx + 180,    10, prj.GetText(TID_GAME_JOB),  dwColor );
	p2DRender->TextOut( sx + 275,    10, prj.GetText(TID_GAME_POINT),  dwColor );
	
	for( int k = 0; k < nBase; k++ )
	{
		GCRankInfo = m_listRank[k];

		if( nOldRanking != GCRankInfo.nPoint )
			nRanking++;

		if( GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer ) 
		{
			bMyRanking   = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking   = nRanking;
		}		

		nOldRanking = GCRankInfo.nPoint;					
	}
	
	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_RANK_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		GCRankInfo = m_listRank[i];

		if( nOldRanking != GCRankInfo.nPoint )
			nRanking++;

		if( GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer ) 
		{
			bMyRanking   = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking   = nRanking;
		}

		sprintf( szNum, "%3d", nRanking );
		sprintf( szCount, "%2d", GCRankInfo.nPoint );
		
		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+320, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0, 0, 0 ) );
		}
		
		if( i == 0 )
			dwColor = D3DCOLOR_XRGB( 200, 0, 0 );
		else
			dwColor = D3DCOLOR_XRGB( 0, 0, 0 );		
		
		if( nOldRanking != GCRankInfo.nPoint )
		{
			p2DRender->TextOut( sx + 4, sy, szNum,  dwColor );
		}
		else
		{
			p2DRender->TextOut( sx + 5, sy, "   ",  dwColor );
		}


		CString strName;
		const char* lpName	= CPlayerDataCenter::GetInstance()->GetPlayerString( GCRankInfo.uidPlayer );
		if( lpName )
			strName	= lpName;

		if( i == 0 )
		{
			p2DRender->TextOut( sx + 40,  sy, strName,  D3DCOLOR_XRGB( 200, 0, 0 ) );
			p2DRender->TextOut( sx + 180, sy, GCRankInfo.strJob,  D3DCOLOR_XRGB( 200, 0, 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 40,  sy, strName,  dwColor );
			p2DRender->TextOut( sx + 180, sy, GCRankInfo.strJob,    dwColor );
		}
		
		p2DRender->TextOut( sx + 275,  sy, szCount,    dwColor );
		
		sy += 18;
		
		nOldRanking = GCRankInfo.nPoint;					
	}	

	// ���� ��ŷ ǥ��...
	if( bMyRanking )
	{
		sprintf( szNum, "%3d", nMyRanking );
		sprintf( szCount, "%2d", GCRankInfoMy.nPoint );
		
		dwColor = D3DCOLOR_XRGB( 0 , 0 , 255 );		
		
		p2DRender->TextOut( sx + 4,    250, szNum,  dwColor );

		p2DRender->TextOut( sx + 40,  250, GCRankInfoMy.strName,  dwColor );
		p2DRender->TextOut( sx + 180, 250, GCRankInfoMy.strJob,   dwColor );		
		p2DRender->TextOut( sx + 275, 250, szCount, dwColor );
	}
} 
// ���õ� �ε����� ��´�?.
int CWndGuildCombatRank_Class::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;
	
	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_RANK_PER_PAGE ) // 0 - 19���� 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}
BOOL CWndGuildCombatRank_Class::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

void CWndGuildCombatRank_Class::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
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
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_RANK_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}
	
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
BOOL CWndGuildCombatRank_Class::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_RANKINGCLASS, pWndParent, 0, CPoint( 0, 0 ) );
} 
BOOL CWndGuildCombatRank_Class::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndGuildCombatRank_Class::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatRank_Class::InsertRank( int nJob, u_long uidPlayer, int nPoint )
{
	if( m_nMax >= MAX_GUILDCOMBAT_RANK )
	{
		Error( "CWndGuildCombatRank_Class::InsertRank - range over" );
		return;
	}

	m_listRank[m_nMax].strName    = CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer );
	m_listRank[m_nMax].strJob     = prj.jobs.info[ nJob ].szName;	
	m_listRank[m_nMax].uidPlayer  = uidPlayer;
	m_listRank[m_nMax].nPoint     = nPoint;
	
	m_nMax++;	

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_RANK_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}		
}


/****************************************************
  WndId : APP_FONTEDIT
****************************************************/
CWndFontEdit::CWndFontEdit() 
{ 
	m_pTexture = NULL;
	m_pWndText = NULL;
	memset( m_ColorRect, 0, sizeof(CRect)*3 );	
} 
CWndFontEdit::~CWndFontEdit() 
{ 
} 
void CWndFontEdit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "yellowbuttten.tga" ), 0xffff00ff, TRUE );

	m_ColorRect[0].left   = 38;
	m_ColorRect[0].top    = 46;
	m_ColorRect[0].right  = 156;
	m_ColorRect[0].bottom = 63;
	
	m_ColorRect[1].left   = 38;
	m_ColorRect[1].top    = 68;
	m_ColorRect[1].right  = 156;
	m_ColorRect[1].bottom = 83;
	
	m_ColorRect[2].left   = 38;
	m_ColorRect[2].top    = 89;
	m_ColorRect[2].right  = 156;
	m_ColorRect[2].bottom = 103;

	m_ColorScrollBar[0] = 0;
	m_ColorScrollBar[1] = 0;
	m_ColorScrollBar[2] = 0;

	m_bLButtonClick = FALSE;

	m_fColor[0] = m_fColor[1] = m_fColor[2] = 0.0f;

	ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );	
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
BOOL CWndFontEdit::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_FONTEDIT, pWndParent, 0, CPoint( 0, 0 ) );
} 
BOOL CWndFontEdit::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndFontEdit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( m_pWndText == NULL )
		return FALSE;

	CWndButton* pWndCheck = NULL;

	switch( nID )
	{
		case WIDC_CHECK1:
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);

			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_BOLD);
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_BOLD);
			}
			break;
		case WIDC_CHECK2:
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);			

			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_UNDERLINE);
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_UNDERLINE);
			}
			break;
		case WIDC_CHECK3:			
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);	
			
			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_STRIKETHROUGH);
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_STRIKETHROUGH);
			}
			break;
		case WIDC_CHECK4:			
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);	

			if( pWndCheck->GetCheck() )
			{
				DWORD dwR, dwG, dwB, dwColor;
				dwR = (DWORD)( m_fColor[0] * 255 );
				dwG = (DWORD)( m_fColor[1] * 255 );
				dwB = (DWORD)( m_fColor[2] * 255 );
				
				dwColor = D3DCOLOR_XRGB(dwR, dwG, dwB);
				
				CString str;
				str.Format( "#c%x", dwColor );
						
				m_pWndText->BlockSetColor( dwColor );
			}
			else
			{
				m_pWndText->BlockSetColor( 0xff000000 );
			}

			break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndFontEdit::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bLButtonClick = FALSE;
}
void CWndFontEdit::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_bLButtonClick = TRUE;
}
void CWndFontEdit::OnMouseWndSurface( CPoint point )
{
	if( g_pPlayer == NULL )
		return;
	
	CRect rect = CRect( 17, 17, 186, 148 );
	
	if( !rect.PtInRect( point ) )
		m_bLButtonClick = FALSE;
	
	for( int i=0; i<3; i++ )
	{
		CRect DrawRect = m_ColorRect[i];
		
		DrawRect.top    -= 22;
		DrawRect.bottom -= 22;
		
		if( DrawRect.PtInRect( point ) && m_bLButtonClick )
		{
			point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
			
			LONG Width = DrawRect.right - DrawRect.left;
			LONG Pos   = point.x - DrawRect.left;
			
			FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));
			
			D3DXVECTOR2 vec1= D3DXVECTOR2( 0.0f, 1.0f );
			D3DXVECTOR2 vec2= D3DXVECTOR2( 1.0f, 1.0f );
			D3DXVECTOR2 vec3;
			
			D3DXVec2Lerp( &vec3, &vec1, &vec2, p );
			
			m_fColor[i] = vec3.x;
			
			m_ColorScrollBar[i].x = point.x;
		}
	}
}

void CWndFontEdit::OnDraw( C2DRender* p2DRender )
{	
	for( int i=0; i<3; i++ )
	{
		CPoint pt = CPoint( m_ColorScrollBar[i].x - ( m_pTexture->m_size.cx / 2 ), m_ColorScrollBar[i].y );
		
		m_pTexture->Render( p2DRender, pt );
	}
/*	
	for( int j=0; j<3; j++ )
	{
		if( m_ColorScrollBar[j].x != m_OriginalColorScrollBar[j].x )
			m_pTexture->Render( p2DRender, CPoint( m_OriginalColorScrollBar[j].x - ( m_pTexture->m_size.cx / 2 ), m_OriginalColorScrollBar[j].y ), 160 );
	}
*/

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	DWORD dwR, dwG, dwB;
	dwR = (DWORD)( m_fColor[0] * 255 );
	dwG = (DWORD)( m_fColor[1] * 255 );
	dwB = (DWORD)( m_fColor[2] * 255 );

	p2DRender->RenderFillRect( lpFace->rect, D3DCOLOR_XRGB( dwR, dwG, dwB ) );
}	

void CWndFontEdit::ReSetBar( FLOAT r, FLOAT g, FLOAT b )
{
	FLOAT fR = ((r-0.0f)/(1.0f - 0.0f)) * 100.0f;
	FLOAT fG = ((g-0.0f)/(1.0f - 0.0f)) * 100.0f;
	FLOAT fB = ((b-0.0f)/(1.0f - 0.0f)) * 100.0f;
	
	m_ColorScrollBar[0].x = (LONG)( (((m_ColorRect[0].right-m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left );
	m_ColorScrollBar[0].y = m_ColorRect[0].top - 20;
	m_ColorScrollBar[1].x = (LONG)( (((m_ColorRect[1].right-m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left );
	m_ColorScrollBar[1].y = m_ColorRect[1].top - 20;
	m_ColorScrollBar[2].x = (LONG)( (((m_ColorRect[2].right-m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left );
	m_ColorScrollBar[2].y = m_ColorRect[2].top - 20;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewel
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMixJewel::CWndOrichalcumReceiver::CWndOrichalcumReceiver()
	: CWndItemReceiver(
		CWndItemReceiver::Features{
			.colorWhenHoverWithItem = 0x60FFFF00,
			.shadow = std::pair<const ItemProp *, DWORD>(prj.GetItemProp(II_GEN_MAT_ORICHALCUM01), 50)
		}
	) {
}

CWndMixJewel::CWndMoonstoneReceiver::CWndMoonstoneReceiver()
	: CWndItemReceiver(
		CWndItemReceiver::Features{
			.colorWhenHoverWithItem = 0x60FFFF00,
			.shadow = std::pair<const ItemProp *, DWORD>(prj.GetItemProp(II_GEN_MAT_MOONSTONE), 50)
		}
	) {
}

void CWndMixJewel::OnDestroy() {
	if (CWndInventory * pWnd = GetWndBase<CWndInventory>(APP_INVENTORY)) {
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
	}
}

void CWndMixJewel::OnDestroyChildWnd(CWndBase * pWndChild) {
	if (m_pConfirm.get() == pWndChild) {
		m_pConfirm = nullptr;
	}
}

void CWndMixJewel::UpdateStartButton() {
	CWndButton * button = GetDlgItem<CWndButton>(WIDC_START);

	if (m_bStart) {
		button->EnableWindow(FALSE);
	} else {
		const BOOL startable = GetAllObjidIfFilled() ? TRUE : FALSE;
		button->EnableWindow(startable);
	}
}

void CWndMixJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	//Ctrl Initialize.

	int StaticID[/* 10 */] = {
		WIDC_PIC_SLOT1, WIDC_PIC_SLOT2, WIDC_PIC_SLOT3, WIDC_PIC_SLOT4, WIDC_PIC_SLOT5,
		WIDC_PIC_SLOT6,	WIDC_PIC_SLOT7, WIDC_PIC_SLOT8, WIDC_PIC_SLOT9, WIDC_PIC_SLOT10
	};

	static_assert(MaxSlotPerItem * 2 == (sizeof(StaticID) / sizeof(StaticID[0])));

	for (unsigned int i = 0; i != MaxSlotPerItem; ++i) {
		m_oriReceivers [i].Create(0, GetWndCtrl(StaticID[i                 ])->rect, this, StartOffsetWidcSlots + i                 );
		m_moonReceivers[i].Create(0, GetWndCtrl(StaticID[i + MaxSlotPerItem])->rect, this, StartOffsetWidcSlots + i + MaxSlotPerItem);
	}

	m_pText = GetDlgItem<CWndText>(WIDC_TEXT1);
	CWndText::SetupDescription(m_pText, _T("SmeltMixJewel.inc"));

	CWndButton* pButton = GetDlgItem<CWndButton>(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	m_pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(m_pWndInventory != NULL)
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
	
	MoveParentCenter();
}

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndMixJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_MIXJEWEL, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndMixJewel::SetJewel(CItemElem * pItemElem) {
	if (ItemProps::IsOrichalcum(*pItemElem)) {
		CWndItemReceiver::TryReceiveIn(*pItemElem, m_oriReceivers);
	} else if (ItemProps::IsMoonstone(*pItemElem)) {
		CWndItemReceiver::TryReceiveIn(*pItemElem, m_moonReceivers);
	}
}

std::optional<std::array<OBJID, MAX_JEWEL>> CWndMixJewel::GetAllObjidIfFilled() const {
	std::array<OBJID, MAX_JEWEL> itemobjid = { 0, };

	for (unsigned int i = 0; i != MaxSlotPerItem; ++i) {
		CItemElem * ori = m_oriReceivers[i].GetItem();
		CItemElem * moon = m_moonReceivers[i].GetItem();

		if (!ori || !moon) return std::nullopt;

		itemobjid[i] = ori->m_dwObjId;
		itemobjid[i + MaxSlotPerItem] = moon->m_dwObjId;
	}

	return itemobjid;
}

BOOL CWndMixJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if (nID == WIDC_START) {
		static_assert(MaxSlotPerItem * 2 == MAX_JEWEL);

		const auto maybeItemObjid = GetAllObjidIfFilled();

		if (maybeItemObjid) {
			m_bStart = TRUE;
			g_DPlay.SendPacket<PACKETTYPE_ULTIMATE_MAKEITEM, std::array<OBJID, MAX_JEWEL>>(
				maybeItemObjid.value()
			);
			UpdateStartButton();
		} else {
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SUMMONANGEL_ERROR2));
		}
	} else if (nID >= StartOffsetWidcSlots && nID < StartOffsetWidcSlots + 2 * MaxSlotPerItem) {
		UpdateStartButton();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndMixJewel::ReceiveResult(const CUltimateWeapon::Result nResult)
{
	//Server���� �����? ������ �ʱ�ȭ�� �����ϰ� â�� ����.
	//�ʱ�ȭ
	
	ForEachReceiver([](auto & receiver) { receiver.ResetItemWithNotify(); });
	
	m_bStart = FALSE;
	UpdateStartButton();
	
	switch(nResult) 
	{
		case CUltimateWeapon::Result::Success:
			{
				m_pConfirm = std::make_unique<CWndMixJewelConfirm>();
				m_pConfirm->Initialize( this );
			}
			break;
		case CUltimateWeapon::Result::Cancel:
			Destroy();
			break;
		case CUltimateWeapon::Result::Inventory:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );
			break;			
	}
}

void CWndMixJewel::SetConfirmInit() {
	// TODO: isn't that a memory leak ?
	[[maybe_unused]] CWndBase * thing = m_pConfirm.release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewelConfirm Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMixJewelConfirm::CWndMixJewelConfirm() 
{ 
}
 
CWndMixJewelConfirm::~CWndMixJewelConfirm() 
{ 
} 

void CWndMixJewelConfirm::OnDestroy()
{
	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
	if(pWndMixJewel != NULL)
		pWndMixJewel->SetConfirmInit();
}

void CWndMixJewelConfirm::OnDraw( C2DRender* p2DRender ) 
{
	int itemID = II_GEN_MAT_ORICHALCUM02;
	ItemProp* pItemProp;
	CTexture* pTexture;

	pItemProp = prj.GetItemProp( itemID );
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	if(pItemProp != NULL)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
} 

void CWndMixJewelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pText->SetString(prj.GetText( TID_GAME_MIXJEWEL_SUCCESS ));
	
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndMixJewelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_MIXJEWEL_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 
BOOL CWndMixJewelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMixJewelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMixJewelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMixJewelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMixJewelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndExtraction Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CWndExtraction::Receiver::CanReceiveItem(const CItemElem & itemElem, bool) {
	const ItemProp * pItemProp = itemElem.GetProp();
	if (!pItemProp) return false;

	return pItemProp->dwItemKind1 == IK1_WEAPON
		&& (
			(pItemProp->dwReferStat1 == WEAPON_GENERAL && pItemProp->dwLimitLevel1 >= 60)
			|| pItemProp->dwReferStat1 == WEAPON_UNIQUE
		);
}

void CWndExtraction::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	
	m_receiver.Create(0, GetWndCtrl(WIDC_PIC_SLOT)->rect, this, WIDC_Receiver);

	CWndButton * pButton = GetDlgItem<CWndButton>(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("SmeltExtraction.inc"));

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndExtraction::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_EXTRACTION, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndExtraction::SetWeapon(CItemElem & pItemElem) {
	m_receiver.SetAnItem(&pItemElem, CWndItemReceiver::SetMode::Silent);
}

BOOL CWndExtraction::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//������ ������ �˸���.
		if(CItemElem * item = m_receiver.GetItem()) {
			CWndButton * pButton = GetDlgItem<CWndButton>(WIDC_START);
			pButton->EnableWindow(FALSE);

			g_DPlay.SendUltimateMakeGem(item->m_dwObjId);
		}
	} else if (nID == WIDC_Receiver) {
		const bool hasItem = m_receiver.GetItem();
		CWndButton * pButton = GetDlgItem<CWndButton>(WIDC_START);
		pButton->EnableWindow(hasItem ? TRUE : FALSE);
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndExtraction::ReceiveResult(const CUltimateWeapon::MakeGemAnswer result) {
	//���? ���� ���� ó��
	//1. ���� : Destroy Window
	//2. ���� : Destroy Window
	//3. ���? : �κ��丮 ���� ���� ���� �� ��Ÿ ���� ����.
	
	// ULTIMATE_SUCCESS : ����
	// ULTIMATE_FAILED : ����
	// ULTIMATE_CANCEL : ���� ó��
	// ULTIMATE_ISULTIMATE : ���Ⱑ �ƴϰų� �Ϲ�, ����ũ�� �ƴ� ��
	// ULTIMATE_INVENTORY : �κ��丮�� ���� �� ��
	// ULTIMATE_ISNOTULTIMATE : ���͸� ������ �ƴ� ��

	std::visit(std_::overloaded{
		[&](CUltimateWeapon::MakeGemSuccess makeGem) {
			const ItemProp * pItemProp = prj.GetItemProp(makeGem.createdItem);

			CString message;
			message.Format(prj.GetText(TID_GAME_EXTRACTION_SUCCESS), pItemProp->szName, makeGem.createdQuantity);
			g_WndMng.OpenMessageBox(message);

			m_receiver.ResetItemWithNotify();
		},

		[&](CUltimateWeapon::Answer::Fail) {
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_EXTRACTION_FAILED));
			Destroy();
		},

		[&](CUltimateWeapon::Answer::Cancel) {
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_EXTRACTION_FAILED));
			Destroy();
		},

		[&](CUltimateWeapon::Answer::Inventory) {
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_EXTRACTION_ERROR));
			GetDlgItem(WIDC_START)->EnableWindow(TRUE);
		}
	}, result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSmeltJewel Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndSmeltJewel::CWndSmeltJewel()
{
	m_pMainItem = NULL;
	m_pJewelElem = NULL;
	m_pItemElem = NULL;
	m_pText = NULL;
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_objJewelId = -1;
	
	for(int i=0; i<5; i++)
	{
		m_nJewelSlot[i] = -1;
		m_dwJewel[i] = -1;
	}
	m_fRotate = 0.0f;
	m_fAddRot = 2.0f;
	
	m_nStatus = 0;
	m_nCount = 0;
	m_nDelay = 25;
	m_nAlpha = 0;
	m_nEyeYPos = 0.0f;
	m_bFlash = FALSE;
}

CWndSmeltJewel::~CWndSmeltJewel()
{
// Todo::Model �ʱ�ȭ �ʿ�
//	if(m_pMainItem != NULL)
//		SAFE_DELETE(m_pMainItem);
}

void CWndSmeltJewel::OnDestroy()
{
	if(m_pJewelElem)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJewelElem ) )
			m_pJewelElem->SetExtra(0);
	}
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);		
	}
}

void CWndSmeltJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// ������ �߾����� �ű��? �κ�.
	m_nJewelSlot[0] = WIDC_STATIC1;
	m_nJewelSlot[1] = WIDC_STATIC2;
	m_nJewelSlot[2] = WIDC_STATIC3;
	m_nJewelSlot[3] = WIDC_STATIC4;
	m_nJewelSlot[4] = WIDC_STATIC5;

	for(int i=0; i<5; i++)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(m_nJewelSlot[i]);
		pStatic->EnableWindow(FALSE);
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	CWndText::SetupDescription(m_pText, _T("SmeltJewel.inc"));

	MoveParentCenter();
} 

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndSmeltJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_JEWEL, pWndParent, 0, CPoint( 0, 0 ) );
} 


BOOL CWndSmeltJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndSmeltJewel::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
}

void CWndSmeltJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}

void CWndSmeltJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

BOOL CWndSmeltJewel::Process()
{
	if(m_pItemElem != NULL)
	{
		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem(WIDC_START);
		
		if(m_nStatus != 1)
		{
			if( (m_nUsableSlot >= 0 && m_nUsableSlot < 5) && m_dwJewel[m_nUsableSlot] != -1)
				pButton->EnableWindow(TRUE);
			else
				pButton->EnableWindow(FALSE);
		}
		else
			pButton->EnableWindow(FALSE);
	}

	if(m_nStatus == 1) //Start!
	{
		if(m_nStatus == 1) //Start��ư ���� ���? ��ǻ���� ������ ȸ���ϵ��� ��.
		{
			if(m_nCount > m_nDelay)
			{
				m_fAddRot += 4.0f;
				//( m_fRotate < 1 ) ? m_fRotate = 1 : m_fRotate;
				
				if(m_nDelay <= 25 && m_nDelay > 16) //ȸ���� �����? �κп��� �÷����� ����.
					m_bFlash = TRUE;
				
				if(m_nDelay < 10)
				{
					m_nEyeYPos -= 0.4f;
					m_nAlpha += 18;
					(m_nAlpha > 230 ) ? m_nAlpha = 230 : m_nAlpha;
				}

				m_nDelay -= 1;				
				if(m_nDelay < 0)
				{
					m_nStatus = 0;
					m_nDelay = 25;
					m_fRotate = 0.0f;
					m_fAddRot = 2.0f;
					m_nAlpha = 0;
					m_nEyeYPos = 0.0f;
					
					if(m_pItemElem != NULL && m_objJewelId != -1)
						g_DPlay.SendUltimateSetGem(m_pItemElem->m_dwObjId, m_objJewelId);
				}
				
				m_nCount = 0;
			}
			m_nCount++;
		}
	}

	return TRUE;
}

void CWndSmeltJewel::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_pItemElem == NULL )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	ResetRenderState(pd3dDevice);

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255,255,255,255 ) );
	
	CRect rect = GetClientRect();

	// ����Ʈ ���� 
	D3DVIEWPORT9 viewport;

	// ���� 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// Color
	D3DXCOLOR color;
	
	// �ʱ�ȭ
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
	
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	// ���⿡ ���� Camera Angle����
	if(m_pItemElem->GetProp()->dwItemKind3 == IK3_AXE ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_SWD)
	{
		if(m_pItemElem->GetProp()->dwHanded == HD_ONE)
		{
			vEyePt.x = 0.0f;
			vEyePt.y = 3.0f;
			vEyePt.z = 0.0f;
			
			vLookatPt.x = 0.6f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
		else if(m_pItemElem->GetProp()->dwHanded == HD_TWO)
		{
			vEyePt.x = 1.0f;
			vEyePt.y = 5.0f;
			vEyePt.z = 0.0f;
			
			vLookatPt.x = 1.2f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_YOYO ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_BOW)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_WAND)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.4f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_CHEERSTICK ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_STAFF)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 4.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}

	if(m_nStatus == 1) //Start�� �̹��� ȸ�� �� ������ ���� ����.
	{
		vEyePt.y += m_nEyeYPos;
	}

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Rotation
	m_fRotate += m_fAddRot;
	D3DXMatrixRotationX(&matRot, D3DXToRadian( m_fRotate ) );
	
	// Scaling
	D3DXMatrixScaling(&matScale, 1.5f, 1.5f, 1.5f);	

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	// ������ ��ġ ����

	viewport = BuildViewport(p2DRender, lpFace);


	pd3dDevice->SetViewport(&viewport);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 );

	// Matrix Multiply
	matWorld = matWorld * matScale * matRot * matTrans;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
	// ������ 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		
	::SetLight( FALSE );
	::SetFog( FALSE );
	SetDiffuse( 1.0f, 1.0f, 1.0f );
	SetAmbient( 1.0f, 1.0f, 1.0f );

	D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	::SetTransformView( matView );
	::SetTransformProj( matProj );
	
/*	if(m_nStatus == 1) //������.
	{
		if(m_bFlash)
		{
			color =  D3DCOLOR_ARGB( 255, 255, 255, 255 );
			p2DRender->RenderFillRect( rect, color );
			m_bFlash = FALSE;
		}
	}
*/
	m_pMainItem->Render( p2DRender->m_pd3dDevice, &matWorld );

	viewport.X      = p2DRender->m_ptOrigin.x;
	viewport.Y      = p2DRender->m_ptOrigin.y;
	viewport.Width  = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );	

	//Jewel Rendering
	CTexture* pTexture;
	ItemProp* pItemProp;
	if(m_pItemElem != NULL)
	{
		for(int i=0; i<5; i++)
		{
			LPWNDCTRL pWndCtrl = GetWndCtrl( m_nJewelSlot[i] );
			if( i < m_pItemElem->GetUltimatePiercingSize() ) //�ո� ����
			{
				if(m_dwJewel[i] != -1) //���� ����
				{
					if(i != m_nUsableSlot) //���� ���� ���������� ��ĥ���� ����.
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndDisableBlue.bmp"), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
						//p2DRender->RenderFillRect( pWndCtrl->rect, 0x609370db );
					}

					pItemProp = prj.GetItemProp( m_dwJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
					}
				}
				else
				{
					if(i != m_nUsableSlot) //�ո� ���� �� ������ ���� �� �ִ� ù��° ���Ը� ���� �������� ȸ������.
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndDisableBlack.bmp"), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
						//p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0a8a8a8 );
					}
				}
			}
			else //�� �ո� ����
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndDisableRed.bmp"), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
				//p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0ff0000 );
			}
		}
	}

	if(m_nStatus == 1)
	{
		color =  D3DCOLOR_ARGB( m_nAlpha, 240, 255, 255 );
				
		CRect rect;
		LPWNDCTRL lpWndCtrl;
					
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		rect = lpWndCtrl->rect;
		p2DRender->RenderFillRect( rect, color );
	}
}

BOOL CWndSmeltJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if(m_nStatus != 0)
		return FALSE;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CUSTOM1 );		

	//Set Weapon
	CItemElem* pItemElem = g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(pItemElem && (pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC) &&
		pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
	{	
		if( wndCtrl->rect.PtInRect( point ) )
		{
			//�ʱ�ȭ.
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			InitializeJewel(pItemElem);
			
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(pItemElem->GetExtra()+1);
			
			m_pMainItem = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_ITEM, m_pItemElem->m_dwItemId );
			m_pMainItem->InitDeviceObjects( g_Neuz.GetDevice() );
		}
	} 
	
	//SetJewel
	if(m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0)
	{
		if(pItemElem && (pItemElem->m_dwItemId == II_GEN_MAT_RUBY || 
			pItemElem->m_dwItemId == II_GEN_MAT_DIAMOND ||
			pItemElem->m_dwItemId == II_GEN_MAT_EMERALD ||
			pItemElem->m_dwItemId == II_GEN_MAT_SAPPHIRE ||
			pItemElem->m_dwItemId == II_GEN_MAT_TOPAZ))
		{
			int checkslot = -1;
			for(int i=0; i<5; i++)
			{
				LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[i] );
				if(wndCtrl->rect.PtInRect( point ))
				{
					checkslot = i;
					i = 5;
				}
			}
			if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1 && checkslot == m_nUsableSlot)
			{
				m_dwJewel[checkslot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				m_pJewelElem = pItemElem;
			}
		}
	}
	
	return TRUE;
}

void CWndSmeltJewel::SetJewel(CItemElem* pItemElem)
{
	//SetJewel
	if( m_nStatus == 0 && m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0 )
	{
		if(pItemElem && (pItemElem->m_dwItemId == II_GEN_MAT_RUBY || 
			pItemElem->m_dwItemId == II_GEN_MAT_DIAMOND ||
			pItemElem->m_dwItemId == II_GEN_MAT_EMERALD ||
			pItemElem->m_dwItemId == II_GEN_MAT_SAPPHIRE ||
			pItemElem->m_dwItemId == II_GEN_MAT_TOPAZ))
		{
			if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1)
			{
				m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				m_pJewelElem = pItemElem;
			}
		}
	}
}

void CWndSmeltJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[m_nUsableSlot] );		
	if( m_nStatus == 0 && wndCtrl != NULL && wndCtrl->rect.PtInRect( point ) )
	{
		if(m_dwJewel[m_nUsableSlot] != -1)
		{
			if(m_pJewelElem)
				m_pJewelElem->SetExtra(0);
			m_dwJewel[m_nUsableSlot] = -1;
		}
	}
}

BOOL CWndSmeltJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(message == WNM_CLICKED)
	{
		if(nID == WIDC_START) 
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);
			
			m_nStatus = 1;
			PLAYSND( "PcSkillD-Counter01.wav" );
		}
	}
			
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSmeltJewel::ReceiveResult(int result)
{
	//���? ���� ���� ó��
	//1. ���� : �ʱ�ȭ
	//2. ���� : �ʱ�ȭ
	//3. ���? : ��Ÿ.
	
	switch(result) 
	{
	case CUltimateWeapon::ULTIMATE_SUCCESS:
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_SUCCESS ) );
		PLAYSND( "InfUpgradeSuccess.wav" );
		//�ʱ�ȭ.
		InitializeJewel(m_pItemElem);
		Destroy();
		break;
	case CUltimateWeapon::ULTIMATE_FAILED:
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_FAIL ) );
		//�ʱ�ȭ.
		InitializeJewel(m_pItemElem);		
		break;
	case CUltimateWeapon::ULTIMATE_CANCEL:
		Destroy();
		break;			
	}
}

void CWndSmeltJewel::InitializeJewel(CItemElem* pItemElem)
{
	//���� ����Ÿ �ʱ�ȭ.
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_dwJewel[m_nUsableSlot] = -1;
	m_objJewelId = -1;

	m_pItemElem = pItemElem;
	
	for(int i=0; i<5; i++)
	{
		m_dwJewel[i] = -1;
		if(i < m_pItemElem->GetUltimatePiercingSize() )
		{
			if(m_pItemElem->GetUltimatePiercingItem( i ) != 0)
			{
				m_dwJewel[i] = m_pItemElem->GetUltimatePiercingItem( i );
				m_nJewelCount++;
			}
		}
	}

	//�� ������ ���Ҵ��� Ȯ���Ͽ� ��밡��? ���� ��ȣ ����.
	int m_nSlot = pItemElem->GetUltimatePiercingSize();
	if(m_nJewelCount < m_nSlot)
		m_nUsableSlot = m_nJewelCount;
	else
		m_nUsableSlot = -1;
	
	if(m_pJewelElem)
		m_pJewelElem->SetExtra(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndChangeWeapon Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndChangeWeapon::CWndChangeWeapon(int nType) 
{
	m_nWeaponType = nType;
	m_bIsSendChange = FALSE;
}

void CWndChangeWeapon::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	
	SetupText();

	m_weaponReceiver    .Create(0, GetWndCtrl(WIDC_STATIC1)->rect, this, WIDC_WeaponReceiver);
	m_orichalcumReceiver.Create(0, GetWndCtrl(WIDC_STATIC2)->rect, this, WIDC_OrichalcumReceiver);
	m_orichalcumReceiver.SetTooltipId(TID_TOOLTIP_CHANGEW_ORICALCUM);
	m_orichalcumReceiver.ChangeShadowTexture(prj.GetItemProp(II_GEN_MAT_ORICHALCUM02), 50);
	m_jewelReceiver     .Create(0, GetWndCtrl(WIDC_STATIC3)->rect, this, WIDC_JewelReceiver);
	m_jewelReceiver     .SetTooltipId(TID_TOOLTIP_CHANGEW_JEWEL);
	
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndChangeWeapon::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_CHANGEWEAPON, pWndParent, 0, CPoint( 0, 0 ) );
}

void CWndChangeWeapon::SetupText() {
	CWndText * pText1 = GetDlgItem<CWndText>(WIDC_TEXT1);
	CWndText * pText2 = GetDlgItem<CWndText>(WIDC_TEXT2);

	const auto Inner = [&](
		const char * text1File, const char * text2File,
		DWORD titleTId, DWORD weaponReceiverTooltip) {
		CWndText::SetupDescription(pText1, text1File);
		CWndText::SetupDescription(pText2, text2File);
		SetTitle(prj.GetText(titleTId));
		m_weaponReceiver.SetTooltipId(weaponReceiverTooltip);
	};

	if (m_nWeaponType == WEAPON_GENERAL) {
		Inner(
			_T("SmeltChangeUniqueWeapon.inc"),
			_T("ChangeUniqueWeaponInfo.inc"),
			TID_GAME_CHANGEWEAPON_UNIQUE,
			TID_TOOLTIP_CHANGEW_GENERAL
		);
	} else /* if (m_nWeaponType == WEAPON_UNIQUE) */ {
		Inner(
			_T("SmeltChangeUltimateWeapon.inc"),
			_T("ChangeUltimateWeaponInfo.inc"),
			TID_GAME_CHANGEWEAPON_ULTIMATE,
			TID_TOOLTIP_CHANGEW_UNIQUE
		);
	}
}

bool CWndChangeWeapon::CWeaponReceiver::CanReceiveItem(const CItemElem & itemElem, bool) {
	const ItemProp * const pItemProp = itemElem.GetProp();
	if (!pItemProp) return false;

	if (pItemProp->dwItemKind1 != IK1_WEAPON) return false;
	if (pItemProp->dwLimitLevel1 < 60) return false;

	if (pItemProp->dwReferStat1 != WEAPON_UNIQUE && pItemProp->dwReferStat1 != WEAPON_GENERAL) {
		return false;
	}

	if (pItemProp->dwReferStat1 == WEAPON_UNIQUE && itemElem.GetAbilityOption() < 10) return false;

	return true;
}

bool CWndChangeWeapon::COrichalcum2Receiver::CanReceiveItem(const CItemElem & itemElem, bool) {
	return itemElem.m_dwItemId == II_GEN_MAT_ORICHALCUM02;
}

bool CWndChangeWeapon::CJewelReceiver::CanReceiveItem(const CItemElem & itemElem, bool) {
	CWndChangeWeapon * parent = dynamic_cast<CWndChangeWeapon *>(m_pParentWnd);
	if (!parent) return false;

	CItemElem * weapon = parent->m_weaponReceiver.GetItem();
	if (!weapon) return false;

	const DWORD rightGemKind = prj.m_UltimateWeapon.GetGemKind(weapon->GetProp()->dwLimitLevel1);
	return itemElem.m_dwItemId == rightGemKind;
}

void CWndChangeWeapon::SetItem(CItemElem & pItemElem) {
	CWndItemReceiver::TryReceiveIn(pItemElem, m_weaponReceiver, m_orichalcumReceiver, m_jewelReceiver);
}

void CWndChangeWeapon::UpdateStartButtonStatus() {
	CWndButton * pButton = GetDlgItem<CWndButton>(WIDC_START);

	if (!m_bIsSendChange
		&& m_weaponReceiver.GetItem()
		&& m_orichalcumReceiver.GetItem()
		&& m_jewelReceiver.GetItem()) {
		pButton->EnableWindow(TRUE);
	} else {
		pButton->EnableWindow(FALSE);
	}
}

BOOL CWndChangeWeapon::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//������ ������ �˸���.
		CItemElem * w = m_weaponReceiver.GetItem();
		CItemElem * o = m_orichalcumReceiver.GetItem();
		CItemElem * j = m_jewelReceiver.GetItem();
		
		if(!m_bIsSendChange && w && o && j)
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_bIsSendChange = TRUE;
			
			g_DPlay.SendPacket<PACKETTYPE_ULTIMATE_TRANSWEAPON, OBJID, OBJID, OBJID>(
				w->m_dwObjId, j->m_dwObjId, o->m_dwObjId
				);
			UpdateStartButtonStatus();
		}
	} else if (nID == WIDC_WeaponReceiver) {
		const CItemElem * w = m_weaponReceiver.GetItem();

		if (w) {
			const ItemProp * prop = w->GetProp();
			m_nWeaponType = prop->dwReferStat1;
			SetupText();

			const DWORD rightGemKind = prj.m_UltimateWeapon.GetGemKind(prop->dwLimitLevel1);
			const ItemProp * itemProp = prj.GetItemProp(rightGemKind);
			m_jewelReceiver.ChangeShadowTexture(itemProp, 50);
		}

		const CItemElem * j = m_jewelReceiver.GetItem();
		if (w && j && !m_jewelReceiver.CanReceiveItem(*j, false)) {
			m_jewelReceiver.ResetItemWithNotify();
		}
		UpdateStartButtonStatus();
	} else if (nID == WIDC_OrichalcumReceiver || nID == WIDC_JewelReceiver) {
		UpdateStartButtonStatus();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndChangeWeapon::ReceiveResult(int result)
{
	//���? ���� ���� ó��
	//1. ���� : Destroy Window
	//2. ���� : Destroy Window
	//3. ���? : �κ��丮 ���� ���� ���� �� ��Ÿ ���� ����.
	
	switch(result) 
	{
		case CUltimateWeapon::ULTIMATE_SUCCESS:
			if(m_nWeaponType == WEAPON_UNIQUE)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CUWEAPON_SUCCESS ) );
			else if(m_nWeaponType == WEAPON_GENERAL)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CGWEAPON_SUCCESS ) );
			break;
		case CUltimateWeapon::ULTIMATE_FAILED:
			if(m_nWeaponType == WEAPON_UNIQUE)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CUWEAPON_FAILED ) );
			else if(m_nWeaponType == WEAPON_GENERAL)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CGWEAPON_FAILED ) );
			break;
		case CUltimateWeapon::ULTIMATE_CANCEL:
			Destroy();
			break;
		case CUltimateWeapon::ULTIMATE_INVENTORY:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );
			break;			
	}

	m_bIsSendChange = FALSE;

	m_weaponReceiver.ResetItemWithNotify();
	m_orichalcumReceiver.ResetItemWithNotify();
	m_jewelReceiver.ResetItemWithNotify();
	UpdateStartButtonStatus();
}

//////////////////////////////////////////////////////////////////////////
//	CWndRemoveJewelConfirm
//////////////////////////////////////////////////////////////////////////

CWndRemoveJewelConfirm::CWndRemoveJewelConfirm() 
{
	m_pInventory = NULL;
	m_pUpgradeItem = NULL;
}
 
CWndRemoveJewelConfirm::~CWndRemoveJewelConfirm() 
{
} 

void CWndRemoveJewelConfirm::OnDestroy()
{
	if(m_pInventory != NULL)
	{
		m_pInventory->m_pWndRemoveJewelConfirm = NULL;
		m_pInventory->m_bRemoveJewel = FALSE;
	}
}

void CWndRemoveJewelConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRemoveJewelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pText->m_string.AddParsingString( prj.GetText( TID_GAME_REMOVEJEWEL ) );
	pText->ResetString();
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndRemoveJewelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	m_pInventory = (CWndInventory*)pWndParent;
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_REMOVE_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndRemoveJewelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(m_pInventory != NULL)
		{
			// ��þƮ�� �Ǵ� ������ - ���? ���?
			m_pInventory->m_pUpgradeItem = m_pUpgradeItem;
			m_pInventory->m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
		}
		Destroy();
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		if(m_pInventory != NULL)
			m_pInventory->BaseMouseCursor();
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveJewelConfirm::SetItem(CItemElem *	m_pItem)
{
	m_pUpgradeItem = m_pItem;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeroSkillUp Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeroSkillUp::CWndHeroSkillUp() 
	: m_rDiamond(II_GEN_MAT_DIAMOND, GetDrawFeatures()),
	m_rEmerald(II_GEN_MAT_EMERALD, GetDrawFeatures()),
	m_rSapphire(II_GEN_MAT_SAPPHIRE, GetDrawFeatures()),
	m_rRuby(II_GEN_MAT_RUBY, GetDrawFeatures()),
	m_rTopaz(II_GEN_MAT_TOPAZ, GetDrawFeatures())
{
}

void CWndHeroSkillUp::OnDraw(C2DRender * const p2DRender) {
	for (const IconDraw & legend : m_legend) {
		if (legend.texture) {
			legend.texture->Render(p2DRender, legend.topLeft);
		}
	}
}

void CWndHeroSkillUp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	
	GetDlgItem<CWndButton>(WIDC_OK)->EnableWindow(FALSE);

	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("HeroSkillUp.inc"));

	using InitData = std::tuple<UINT, CWndOnlyOneItemReceiver *, UINT, DWORD>;

	std::array<InitData, 5> list {
		InitData( WIDC_PIC_SLOT1, &m_rDiamond,  WIDC_CUSTOM1, II_GEN_MAT_DIAMOND ),
		InitData( WIDC_PIC_SLOT2, &m_rEmerald,  WIDC_CUSTOM2, II_GEN_MAT_EMERALD ),
		InitData( WIDC_PIC_SLOT3, &m_rSapphire, WIDC_CUSTOM3, II_GEN_MAT_SAPPHIRE),
		InitData( WIDC_PIC_SLOT4, &m_rRuby,     WIDC_CUSTOM4, II_GEN_MAT_RUBY    ),
		InitData( WIDC_PIC_SLOT5, &m_rTopaz,    WIDC_CUSTOM5, II_GEN_MAT_TOPAZ   )
	};

	for (size_t i = 0; i != 5; ++i) {
		const auto & [slotId, pReceiver,  customId, itemId] = list[i];

		LPWNDCTRL slot = GetWndCtrl(slotId);
		pReceiver->Create(0, slot->rect, this, WIDC_Receivers + i);

		LPWNDCTRL custom = GetWndCtrl(customId);
		const ItemProp * itemProp = prj.GetItemProp(itemId);
		m_legend[i].topLeft = custom->rect.TopLeft();
		m_legend[i].texture = itemProp->GetTexture();
	}

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndHeroSkillUp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_HERO_SKILLUP, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndHeroSkillUp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		if(!m_bSendHeroSkillup)
		{
			const auto objids = ReceiversToObjid();
			
			if (objids) {
				m_bSendHeroSkillup = true;
				g_DPlay.SendPacket<PACKETTYPE_LEGENDSKILLUP_START, std::array<OBJID, 5>>(objids.value());
			}
		}
	} else if (nID == WIDC_CANCEL) {
		Destroy();
	} else if (nID >= WIDC_Receivers && nID < WIDC_Receivers + 5) {
		UpdateOkButton();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndHeroSkillUp::UpdateOkButton() {
	BOOL value;
	if (!m_bSendHeroSkillup && ReceiversToObjid().has_value()) {
		value = TRUE;
	} else {
		value = FALSE;
	}

	GetDlgItem<CWndButton>(WIDC_OK)->EnableWindow(value);
}

std::optional<std::array<OBJID, 5>> CWndHeroSkillUp::ReceiversToObjid() const {
	std::array<const CWndOnlyOneItemReceiver *, 5> receivers = {
		&m_rDiamond, &m_rEmerald, &m_rSapphire, &m_rRuby, &m_rTopaz
	};
	
	std::array<OBJID, 5> values = { 0, };
	for (size_t i = 0; i != 5; ++i) {
		const CWndOnlyOneItemReceiver * const receiver = receivers[i];
		const CItemElem * const item = receiver->GetItem();
		if (!item) return std::nullopt;
		
		values[i] = item->m_dwObjId;
	}
	
	return values;
}

void CWndHeroSkillUp::SetJewel(CItemElem * pItemElem) {
	CWndItemReceiver::TryReceiveIn(*pItemElem,
		m_rDiamond, m_rEmerald, m_rSapphire, m_rRuby, m_rTopaz
	);
}

void CWndHeroSkillUp::ReceiveResult(int nresult)
{
	switch(nresult) 
	{
		case -1:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_MAX ) );
			break;
		case 0:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_FAIL ) );
			break;
		case 1:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_SUCCESS ) );
			break;
	}

	Destroy();
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
BOOL CWndDialogEvent::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
BOOL CWndHeavenTower::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
				if( pWndEntranceConfirm )
				{
					g_WndMng.OpenCustomBox( "", pWndEntranceConfirm, this );
					
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

BOOL CWndHeavenTowerEntranceConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemoveAttribute Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemoveAttribute::~CWndRemoveAttribute() 
{
	if(m_pWndConfirm != NULL)
		m_pWndConfirm->Destroy();

	SAFE_DELETE(m_pWndConfirm);
}

void CWndRemoveAttribute::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("RemoveAttribute.inc"));

	const WNDCTRL * const wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	m_receiver.Create(0, wndCtrl->rect, this, WIDC_PIC_SLOT + 1);

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndRemoveAttribute::Initialize( CWndBase* pWndParent, DWORD) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_REMOVE_ATTRIBUTE, pWndParent, 0, CPoint( 0, 0 ) );
} 

bool CWndRemoveAttribute::CWndAttributedItem::CanReceiveItem(const CItemElem & itemElem, bool verbose) {
	if (itemElem.m_nResistAbilityOption <= 0) {
		if (verbose) g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_REMOVE_ERROR));
		return false;
	}

	const ItemProp * pItemProp = itemElem.GetProp();
	if (!pItemProp) return false;

	// TODO: why tf isn't IsEleRefineryAble a method of pItemProp?
	return CItemElem::IsEleRefineryAble(pItemProp);
}

void CWndRemoveAttribute::SetWeapon(CItemElem* pItemElem) {
	m_receiver.SetAnItem(pItemElem, CWndItemReceiver::SetMode::Verbose);
}

BOOL CWndRemoveAttribute::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_START) {
		if (!m_pWndConfirm) {
			m_pWndConfirm = new CWndConfirm;
			m_pWndConfirm->Initialize(this);
		}
	} else if (nID == WIDC_PIC_SLOT + 1) {
		CWndBase * pButton = GetDlgItem(WIDC_START);
		pButton->EnableWindow(m_receiver.GetItem() ? TRUE : FALSE);
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndRemoveAttribute::ReceiveResult(BOOL result) {
	if (result) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_REMOVE_ATTRIBUTE_CONFIRM));
	}

	Destroy();
}

//////////////////////////////////////////////////////////////////////////
//	CWndRemoveAttributeConfirm
//////////////////////////////////////////////////////////////////////////

void CWndRemoveAttribute::CWndConfirm::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndText * pText = GetDlgItem<CWndText>(WIDC_TEXT1);
	pText->m_string.AddParsingString(prj.GetText(TID_GAME_REMOVE_ATTRIBUTE));
	pText->ResetString();

	MoveParentCenter();
} 

BOOL CWndRemoveAttribute::CWndConfirm::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_REMOVE_ATTRIBUTE_CONFIRM, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndRemoveAttribute::CWndConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	CWndRemoveAttribute * pParentwnd = dynamic_cast<CWndRemoveAttribute *>(m_pParentWnd);

	if (nID == WIDC_YES) {
		if (pParentwnd) {
			CItemElem * item = pParentwnd->m_receiver.GetItem();
			if (item) {
				g_DPlay.SendRemoveAttribute(item->m_dwObjId);
			}
		}
	} else if (nID == WIDC_NO || nID == WTBID_CLOSE) {
		// TODO: isn't that a memory leak?
		// - pParentwnd->m_pWndConfirm loses the pointer to this so nobody points
		// to this anymore
		// - Destroy() does not free this -> memleak
		pParentwnd->m_pWndConfirm = nullptr;
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemovePiercing Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndRemovePiercing::OnDraw(C2DRender * p2DRender) {
	static constexpr std::array<UINT, 10> WIDOfStatics = {
		WIDC_STATIC_PIERCING1, WIDC_STATIC_PIERCING2, WIDC_STATIC_PIERCING3,
		WIDC_STATIC_PIERCING4, WIDC_STATIC_PIERCING5, WIDC_STATIC_PIERCING6,
		WIDC_STATIC_PIERCING7, WIDC_STATIC_PIERCING8, WIDC_STATIC_PIERCING9,
		WIDC_STATIC_PIERCING10
	};

	CItemElem * item = m_receiver.GetItem();
	if (!item) return;

	ItemProp* pItemProp = item->GetProp();
	if (!pItemProp) return;


	// Render greyed out slots
	const ItemProps::PiercingType piercingType = item->GetPiercingType();

	unsigned int uiMax = piercingType.GetNumberOfPiercings();
	if (uiMax > WIDOfStatics.size()) uiMax = WIDOfStatics.size();

	const D3DXCOLOR irrelevantSlotColor = D3DCOLOR_ARGB(60, 240, 0, 0);
	for (unsigned int i = 0; i != uiMax; ++i) {
		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDOfStatics[i]);
		CRect rect = lpWndCtrl->rect;
		p2DRender->RenderFillRect(rect, irrelevantSlotColor);
	}
	
	// Render Piercing Options
	const unsigned int nPiercingSize = static_cast<unsigned int>(item->GetPiercingSize());

	for (int i = 0; i < nPiercingSize; i++) {
		if (nPiercingSize > uiMax) break;

		const PIERCINGAVAIL * ptr = g_PiercingAvail.GetPiercingAvail(item->GetPiercingItem(i));

		if (!ptr) continue;

		CString textOpt;
		for (const auto & singleDst : ptr->params) {
			textOpt = singleDst.ToString() + "  ";
		}

		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDOfStatics[i]);
		p2DRender->TextOut(lpWndCtrl->rect.left + 10, lpWndCtrl->rect.top + 8, textOpt, D3DCOLOR_ARGB(255, 0, 0, 0));
	}
}

void CWndRemovePiercing::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	
	
	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("SmeltRemovePiercing.inc"));

	m_receiver.Create(0, GetWndCtrl(WIDC_PIC_SLOT)->rect, this, WIDC_Receiver);
	m_receiver.SetTooltipId(TID_GAME_TOOLTIP_PIERCINGITEM);

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndRemovePiercing::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_REMOVE_PIERCING_EX, pWndParent, 0, CPoint( 0, 0 ) );
} 


bool CWndRemovePiercing::CWndPiercedItemReceiver::CanReceiveItem(
	const CItemElem & itemElem, bool verbose
) {
	const ItemProp * itemProp = itemElem.GetProp();
	if (!itemProp) return false;

	const ItemProps::PiercingType piercingType = itemElem.GetPiercingType();

	if (!piercingType.IsOnEquipement() || itemElem.GetPiercingItem(0) == 0) {
		if (verbose) g_WndMng.PutString(TID_GAME_REMOVE_PIERCING_ERROR_EX);
		return false;
	}

	return true;
}

void CWndRemovePiercing::SetItem(CItemElem* pItemElem) {
	m_receiver.SetAnItem(pItemElem, CWndItemReceiver::SetMode::Verbose);
}

BOOL CWndRemovePiercing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//������ ������ �˸���.
		if(CItemElem * equipement = m_receiver.GetItem())
		{
			CWndBase * pButton = GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);

			g_DPlay.SendPacket<PACKETTYPE_PIERCINGREMOVE, DWORD>(equipement->m_dwObjId);
			Destroy();
		}
	} else if (nID == WIDC_Receiver) {
		CWndBase * pButton = GetDlgItem(WIDC_START);
		pButton->EnableWindow(m_receiver.GetItem() ? TRUE : FALSE);
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemoveJewel Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////// Components of CWndRemoveJewel

bool CWndRemoveJewel::CWndJeweledItem::IsAWeapon(const ItemProp * itemProp) {
	if (!itemProp) return false;

	return itemProp->dwItemKind2 == IK2_WEAPON_DIRECT || itemProp->dwItemKind2 == IK2_WEAPON_MAGIC;
}

bool CWndRemoveJewel::CWndJeweledItem::CanReceiveItem(const CItemElem & itemElem, bool verbose) {
	const ItemProp * prop = itemElem.GetProp();
	if (!IsAWeapon(itemElem.GetProp())) return false;

	const bool ok = prop->dwReferStat1 == WEAPON_ULTIMATE && itemElem.GetUltimatePiercingSize() > 0;

	if (!ok && verbose) {
		g_WndMng.PutString(TID_GAME_REMOVE_JEWEL_ERROR2);
	}

	return ok;
}

CWndRemoveJewel::CWndMoonstoneReceiver::CWndMoonstoneReceiver()
	: CWndItemReceiver(
		CWndItemReceiver::Features{
			.shadow = std::pair(prj.GetItemProp(II_GEN_MAT_MOONSTONE), 50)
		}
	) {
}

bool CWndRemoveJewel::CWndMoonstoneReceiver::CanReceiveItem(const CItemElem & itemElem, bool verbose) {
	if (!ItemProps::IsMoonstone(itemElem)) {
		if (verbose) g_WndMng.PutString(TID_GAME_REMOVE_JEWEL_ERROR1);
		return false;
	}

	return true;
}


////// CWndRemoveJewel

CWndRemoveJewel::CWndRemoveJewel() { ResetJewel(); }

void CWndRemoveJewel::OnDraw( C2DRender* p2DRender )  {
	const CItemElem * weapon = m_weaponReceiver.GetItem();
	if (!weapon) return;

	const ItemProp * const pItemProp = weapon->GetProp();

	for (DisplayedJewel & displayedJewel : m_displayed) {
		if (displayedJewel.jewelItemID == 0) continue;

		// Icon
		LPWNDCTRL slotWndCtrl = GetWndCtrl(displayedJewel.slotWID);

		if (slotWndCtrl && displayedJewel.texture) {
			displayedJewel.texture->Render(p2DRender, slotWndCtrl->rect.TopLeft());
		}

		// Info
		const auto [color, textId] = GetTextAndColorOfJewel(displayedJewel.jewelItemID);
		const char * dstText = textId ? prj.GetText(textId) : "???";

		LPWNDCTRL infoWndCtrl = GetWndCtrl(displayedJewel.infoWID);
		p2DRender->TextOut(infoWndCtrl->rect.left + 10, infoWndCtrl->rect.top + 10, dstText, color);
	}
}

std::pair<DWORD, DWORD> CWndRemoveJewel::GetTextAndColorOfJewel(const DWORD jewelId) {
	const ToolTipItemTextColor & theme = g_WndMng.dwItemColor[g_Option.m_nToolTipText];

	// DiamondItemId, TopazItemId, color position in ToolTipItemTextColor::dwAddedOpt, tooltip id
	using JewelLine = std::tuple<DWORD, DWORD, size_t, DWORD>;

	constexpr static std::array<JewelLine, 9> lines = {
		JewelLine(II_GEN_MAT_DIAMOND01, II_GEN_MAT_TOPAZ01, 0, TID_TOOLTIP_DST_HP_MAX),
		JewelLine(II_GEN_MAT_DIAMOND02, II_GEN_MAT_TOPAZ02, 1, TID_TOOLTIP_DST_ATKPOWER),
		JewelLine(II_GEN_MAT_DIAMOND03, II_GEN_MAT_TOPAZ03, 2, TID_TOOLTIP_DST_ADJDEF),
		JewelLine(II_GEN_MAT_DIAMOND04, II_GEN_MAT_TOPAZ04, 3, TID_TOOLTIP_DST_MELEE_STEALHP),
		JewelLine(II_GEN_MAT_DIAMOND05, II_GEN_MAT_TOPAZ05, 4, TID_TOOLTIP_DST_PVP_DMG),
		JewelLine(II_GEN_MAT_DIAMOND06, II_GEN_MAT_TOPAZ06, 5, TID_TOOLTIP_STR),
		JewelLine(II_GEN_MAT_DIAMOND07, II_GEN_MAT_TOPAZ07, 6, TID_TOOLTIP_STA),
		JewelLine(II_GEN_MAT_DIAMOND08, II_GEN_MAT_TOPAZ08, 7, TID_TOOLTIP_DEX),
		JewelLine(II_GEN_MAT_DIAMOND09, II_GEN_MAT_TOPAZ09, 8, TID_TOOLTIP_INT),
	};

	for (const auto & [diamondId, topazId, addedOptIndex, tooltipId] : lines) {
		if (jewelId >= diamondId && jewelId <= topazId) {
			return std::pair<DWORD, DWORD>(theme.dwAddedOpt[addedOptIndex], tooltipId);
		}
	}

	return std::pair<DWORD, DWORD>(0, 0);
}

void CWndRemoveJewel::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	
	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("SmeltRemoveJewel.inc"));

	m_displayed[0].slotWID = WIDC_JEWEL_SLOT1; m_displayed[0].infoWID = WIDC_JEWEL_SLOT_INFO1;
	m_displayed[1].slotWID = WIDC_JEWEL_SLOT2; m_displayed[1].infoWID = WIDC_JEWEL_SLOT_INFO2;
	m_displayed[2].slotWID = WIDC_JEWEL_SLOT3; m_displayed[2].infoWID = WIDC_JEWEL_SLOT_INFO3;
	m_displayed[3].slotWID = WIDC_JEWEL_SLOT4; m_displayed[3].infoWID = WIDC_JEWEL_SLOT_INFO4;
	m_displayed[4].slotWID = WIDC_JEWEL_SLOT5; m_displayed[4].infoWID = WIDC_JEWEL_SLOT_INFO5;

	m_weaponReceiver.Create(0, GetWndCtrl(WIDC_PIC_SLOT)->rect, this, WIDC_Weapon);
	m_weaponReceiver.SetTooltipId(TID_GAME_TOOLTIP_REMOVEJEWEL1);
	m_moonstoneReceiver.Create(0, GetWndCtrl(WIDC_PIC_SLOT1)->rect, this, WIDC_Moon);
	m_moonstoneReceiver.SetTooltipId(TID_GAME_TOOLTIP_REMOVEJEWEL2);

	MoveParentCenter();
} 

BOOL CWndRemoveJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_REMOVE_JEWEL, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndRemoveJewel::OnMouseWndSurface(CPoint point) {
	const CPoint topLeft = GetWndCtrl(WIDC_JEWEL_SLOT1)->rect.TopLeft();
	const CPoint bottomRight = GetWndCtrl(WIDC_JEWEL_SLOT_INFO5)->rect.BottomRight();

	CRect rect(topLeft, bottomRight);

	if (rect.PtInRect(point)) {
		ClientToScreen(&point);
		ClientToScreen(&rect);
		g_toolTip.PutToolTip((DWORD)this, prj.GetText(TID_GAME_TOOLTIP_REMOVEJEWEL3), rect, point);
	}
}

void CWndRemoveJewel::SetItem(CItemElem* pItemElem) {
	if (!pItemElem) return;

	if (ItemProps::IsMoonstone(*pItemElem)) {
		if (!m_moonstoneReceiver.GetItem()) {
			m_moonstoneReceiver.SetAnItem(pItemElem, CWndItemReceiver::SetMode::Verbose);
		}
	} else if (CWndJeweledItem::IsAWeapon(pItemElem->GetProp())) {
		if (!m_weaponReceiver.GetItem()) {
			m_weaponReceiver.SetAnItem(pItemElem, CWndItemReceiver::SetMode::Verbose);
		}
	}
}

BOOL CWndRemoveJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult) {
	if (nID == WIDC_START) {
		CItemElem * weapon = m_weaponReceiver.GetItem();
		CItemElem * moon = m_moonstoneReceiver.GetItem();
		if (weapon && moon) {
			GetDlgItem<CWndButton>(WIDC_START)->EnableWindow(FALSE);
			g_DPlay.SendUltimateRemoveGem(weapon->m_dwObjId, moon->m_dwObjId);
			Destroy();
		}
	} else if (nID == WIDC_Weapon) {
		UpdateDisplayedJewel();
		UpdateStartButtonStatus();
	} else if (nID == WIDC_Moon) {
		UpdateStartButtonStatus();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveJewel::ResetJewel() {
	for (DisplayedJewel & displayed : m_displayed) {
		displayed.jewelItemID = NULL;
		displayed.texture = nullptr;
	}
}

void CWndRemoveJewel::UpdateDisplayedJewel() {
	const CItemElem * weapon = m_weaponReceiver.GetItem();

	int actualSize = weapon ? weapon->GetUltimatePiercingSize() : 0;
	if (actualSize > m_displayed.size()) actualSize = m_displayed.size();

	for (int i = 0; i < std::tuple_size<decltype(m_displayed)>::value; ++i) {
		if (i < actualSize) {
			m_displayed[i].jewelItemID = weapon->GetUltimatePiercingItem(i);
			m_displayed[i].texture = ItemProps::GetItemIconTexture(m_displayed[i].jewelItemID);
		} else {
			m_displayed[i].jewelItemID = 0;
			m_displayed[i].texture = nullptr;
		}
	}
}

void CWndRemoveJewel::UpdateStartButtonStatus() {
	GetDlgItem(WIDC_START)->EnableWindow(
		(m_weaponReceiver.GetItem() && m_moonstoneReceiver.GetItem()) ? TRUE : FALSE
	);
}



//////////////////////////////////////////////////////////////////////////
// Change Attribute Window
//////////////////////////////////////////////////////////////////////////

CWndChangeAttribute::CWndChangeAttribute()
{
	m_nAttributeNum = -1;
	m_pItemElem = NULL;
	m_pChangeItem = NULL;
	m_pTexture = NULL;
}

CWndChangeAttribute::~CWndChangeAttribute()
{
	if( m_pItemElem )
		m_pItemElem->SetExtra( 0 );
	if( m_pChangeItem )
		m_pChangeItem->SetExtra( 0 );
}

void CWndChangeAttribute::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CWndText::SetupDescription(
		GetDlgItem<CWndText>(WIDC_TEXT1),
		_T("ChangeAttribute.inc")
	);

	m_nAttributeStaticID[0] = WIDC_CUSTOM1;
	m_nAttributeStaticID[1] = WIDC_CUSTOM2;
	m_nAttributeStaticID[2] = WIDC_CUSTOM3;
	m_nAttributeStaticID[3] = WIDC_CUSTOM4;
	m_nAttributeStaticID[4] = WIDC_CUSTOM5;

	m_nTooltipTextIndx[0] = TID_TOOLTIP_CA_INSLOT;
	m_nTooltipTextIndx[1] = TID_TOOLTIP_CA_FIRE;
	m_nTooltipTextIndx[2] = TID_TOOLTIP_CA_WATER;
	m_nTooltipTextIndx[3] = TID_TOOLTIP_CA_ELEC;
	m_nTooltipTextIndx[4] = TID_TOOLTIP_CA_WIND;
	m_nTooltipTextIndx[5] = TID_TOOLTIP_CA_EARTH;

	MoveParentCenter();
} 

BOOL CWndChangeAttribute::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_CHANGE_ATTRIBUTE, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndChangeAttribute::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(nID == WIDC_OK)
	{
		if (m_pItemElem != nullptr && m_pChangeItem != nullptr && m_nAttributeNum >= 0 && m_nAttributeNum < 5) {
			g_DPlay.SendPacket<PACKETTYPE_CHANGE_ATTRIBUTE, OBJID, OBJID, int>(
				m_pItemElem->m_dwObjId, m_pChangeItem->m_dwObjId, m_nAttributeNum + 1
				);
		}

		if(m_pItemElem)
			m_pItemElem->SetExtra(0);

		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndChangeAttribute::OnDraw(C2DRender* p2DRender)
{
	// Draw Item
	if(m_pItemElem != NULL)
	{
		if(m_pTexture != NULL)
			m_pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC1 )->rect.left, GetWndCtrl( WIDC_STATIC1 )->rect.top ) );
	}

	// Draw Attribute & Draw Rect Round Lamp
	for(int i=0; i<5; i++)
	{
		CRect rect = GetWndCtrl( m_nAttributeStaticID[i] )->rect;
		g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, CPoint(rect.left, rect.top), i, 255, 1.5f, 1.5f );
		
		if(i == m_nAttributeNum)
			FillRect( p2DRender, GetWndCtrl( m_nAttributeStaticID[i] )->rect, 0xffbb00ff, 0x00bb00ff );
	}
}

BOOL CWndChangeAttribute::Process()
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	CPoint ptMouse = GetMousePoint();

	lpWndCtrl = GetWndCtrl(WIDC_STATIC1);
	rect = lpWndCtrl->rect;
	if(rect.PtInRect(ptMouse))
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( 10000, prj.GetText(m_nTooltipTextIndx[0]), rect, ptMouse, 1 );
	}

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if(rect.PtInRect(ptMouse))
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 10000, prj.GetText(m_nTooltipTextIndx[i+1]), rect, ptMouse, 1 );
		}
	}
	
	return TRUE;
}

void CWndChangeAttribute::OnLButtonUp( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if(rect.PtInRect(point))
			m_nAttributeNum = i;
	}
}

BOOL CWndChangeAttribute::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	BOOL bOnTitle = FALSE;
	CPoint point = GetMousePoint();

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl( m_nAttributeStaticID[i] );
		rect = lpWndCtrl->rect;
		if( rect.PtInRect( point ) )
			bOnTitle = TRUE;
	}

	if(bOnTitle)
		SetMouseCursor( CUR_SELECT );
	else
	{
		SetMouseCursor( CUR_BASE );
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );
	}

	return TRUE;
}

void CWndChangeAttribute::SetChangeItem( CItemElem* pItemElem )
{
	pItemElem->SetExtra( pItemElem->GetExtra() + 1 );
	m_pChangeItem = pItemElem;
}

void CWndChangeAttribute::FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend)
{
	CPoint Point = CPoint( rectBg.TopLeft().x, rectBg.TopLeft().y );
	float fVar = 0.04f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

	float fLerp = 1.0f;

	D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp );
	int nthick = 8;	
	CRect Rect = CRect( 0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.top-2, Point.x+Rect.left+nthick-2, Point.y+Rect.bottom+2 ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.top+nthick-2, Point.x+Rect.right+2, Point.y+Rect.top-2 ),
		dwColor1, dwColor1, dwColor2, dwColor2 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.right+2, Point.y+Rect.top-2, Point.x+Rect.right-nthick+2, Point.y+Rect.bottom+2 ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.bottom+2, Point.x+Rect.right+2, Point.y+Rect.bottom-nthick+2 ),
		dwColor2, dwColor2, dwColor1, dwColor1 );
}

BOOL CWndChangeAttribute::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame == NULL )
		return FALSE;

	if( pWndFrame->GetWndId() != APP_INVENTORY )
	{
		SetForbid( TRUE );
		return FALSE;
	}

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	CRect rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		CItemElem * pTempElem  = g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			const ItemProp * pItemProp = pTempElem->GetProp();
			
			if(pTempElem && CItemElem::IsEleRefineryAble(pItemProp) && pTempElem->m_nResistAbilityOption > 0)
			{
				m_pItemElem = pTempElem;
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			}
			else
				g_WndMng.PutString(TID_GAME_NOTELEMENT);
		}
	}

	return TRUE;
}

void CWndChangeAttribute::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			m_pTexture = NULL;
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// Couple Message Window
//////////////////////////////////////////////////////////////////////////
BOOL CWndCoupleMessage::Initialize( CWndBase* pWndParent, DWORD dwWndId )
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
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
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
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if(pWndButton)
	{
		if(::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtBreakUp.bmp" ), 0xffff00ff );
		else
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtBreakUp.bmp" ), 0xffff00ff );
	}

	MoveParentCenter();
} 

BOOL CWndCoupleTabInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_COUPLE_TAB_INFO, pWndParent, 0, CPoint( 0, 0 ) );
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
					g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
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

	m_pCouple = CCoupleHelper::Instance()->GetCouple();
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

				pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
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
		m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
		m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
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

	m_pSkillBgTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "Bg_Couple_Skill.tga"), 0xffff00ff );

	MoveParentCenter();
} 

BOOL CWndCoupleTabSkill::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_COUPLE_TAB_SKILL, pWndParent, 0, CPoint( 0, 0 ) );
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
	CCouple* pCouple = CCoupleHelper::Instance()->GetCouple();
	if(pCouple)
		nLevel = pCouple->GetLevel();

	VS& vSkills	= CCoupleProperty::Instance()->GetSkill( nLevel );
	VSK& vSkillKinds = CCoupleProperty::Instance()->GetSKillKinds();

	if(vSkills.size() == vSkillKinds.size())
	{
		for(int i=0; i<(int)( vSkills.size() ); i++)
		{
			if(vSkills[i] > 0)
			{
				ItemProp* pItemProp = prj.GetItemProp( vSkillKinds[i] );
				if(pItemProp)
				{
					CTexture* pTex = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pItemProp->szIcon), 0xffff00ff );

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

BOOL CWndCoupleManager::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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
BOOL CWndFunnyCoinConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
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

CWndSmeltSafety::CWndSmeltSafety(CWndSmeltSafety::WndMode eWndMode) : 
m_eWndMode(eWndMode), 
m_pItemElem(NULL), 
m_pItemTexture(NULL), 
m_pNowGaugeTexture(NULL), 
m_pSuccessTexture(NULL), 
m_pFailureTexture(NULL), 
m_nMaterialCount(0), 
m_nScroll1Count(0), 
m_nScroll2Count(0), 
m_nResultCount(0), 
m_bStart(FALSE), 
m_bResultSwitch(false), 
m_dwEnchantTimeStart(0xffffffff), 
m_dwEnchantTimeEnd(0xffffffff), 
m_fGaugeRate(0), 
m_nValidSmeltCounter(0), 
m_nCurrentSmeltNumber(0), 
m_pVertexBufferGauge(NULL), 
m_pVertexBufferSuccessImage(NULL), 
m_pVertexBufferFailureImage(NULL)
, 
m_pSelectedElementalCardItemProp( NULL )
{
}

CWndSmeltSafety::~CWndSmeltSafety()
{
	for(int i = 0; i < SMELT_MAX; ++i)
	{
		if(m_Material[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Material[i].pItemElem ) )
				m_Material[i].pItemElem->SetExtra(0);
		}

		if(m_Scroll1[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Scroll1[i].pItemElem ) )
				m_Scroll1[i].pItemElem->SetExtra(0);
		}

		if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && m_Scroll2[i].pItemElem != NULL )
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Scroll2[i].pItemElem ) )
				m_Scroll2[i].pItemElem->SetExtra(0);
		}
	}

	if(m_pItemElem != NULL)
	{
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory != NULL)
	{
		pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
	}
}

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndSmeltSafety::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_SAFETY, pWndParent, 0, CPoint( 0, 0 ) );
}

void CWndSmeltSafety::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	// ���⿡ �ڵ��ϼ���

	int StaticMaterialID[SMELT_MAX] = {WIDC_STATIC11, WIDC_STATIC12, WIDC_STATIC13, WIDC_STATIC14, WIDC_STATIC15, WIDC_STATIC16,
								WIDC_STATIC17, WIDC_STATIC18, WIDC_STATIC19, WIDC_STATIC20};
	int StaticScrollID1[SMELT_MAX] = {WIDC_STATIC41, WIDC_STATIC42, WIDC_STATIC43, WIDC_STATIC44, WIDC_STATIC45, WIDC_STATIC46,
								WIDC_STATIC47, WIDC_STATIC48, WIDC_STATIC49, WIDC_STATIC50};
	int StaticScrollID2[SMELT_MAX] = {WIDC_STATIC61, WIDC_STATIC62, WIDC_STATIC63, WIDC_STATIC64, WIDC_STATIC65, WIDC_STATIC66,
								WIDC_STATIC67, WIDC_STATIC68, WIDC_STATIC69, WIDC_STATIC70};

	for(int i = 0; i < SMELT_MAX; ++i)
	{
		m_Material[i].wndCtrl = GetWndCtrl( StaticMaterialID[i] );
		m_Material[i].staticNum = StaticMaterialID[i];
		m_Material[i].isUse = FALSE;
		m_Material[i].pItemElem = NULL;
		m_Scroll1[i].wndCtrl = GetWndCtrl( StaticScrollID1[i] );
		m_Scroll1[i].staticNum = StaticScrollID1[i];
		m_Scroll1[i].isUse = FALSE;
		m_Scroll1[i].pItemElem = NULL;
		m_Scroll2[i].wndCtrl = GetWndCtrl( StaticScrollID2[i] );
		m_Scroll2[i].staticNum = StaticScrollID2[i];
		m_Scroll2[i].isUse = FALSE;
		m_Scroll2[i].pItemElem = NULL;
	}

	m_nResultStaticID[0] = WIDC_STATIC31;
	m_nResultStaticID[1] = WIDC_STATIC32;
	m_nResultStaticID[2] = WIDC_STATIC33;
	m_nResultStaticID[3] = WIDC_STATIC34;
	m_nResultStaticID[4] = WIDC_STATIC35;
	m_nResultStaticID[5] = WIDC_STATIC36;
	m_nResultStaticID[6] = WIDC_STATIC37;
	m_nResultStaticID[7] = WIDC_STATIC38;
	m_nResultStaticID[8] = WIDC_STATIC39;
	m_nResultStaticID[9] = WIDC_STATIC40;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_NOW_GRADE);
	assert(pWndStatic != NULL);
	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY || m_eWndMode == WND_ELEMENT )
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_NOW_GRADE));
	}
	else if(m_eWndMode == WND_PIERCING)
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_NOW_PIERCING));
	}

	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_MAX_GRADE);
	assert(pWndStatic != NULL);
	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY || m_eWndMode == WND_ELEMENT )
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_MAX_GRADE));
	}
	else
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_MAX_PIERCING));
	}

	RefreshInformation();

	switch(m_eWndMode)
	{
		case WND_NORMAL:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_NORMAL));
			break;
		case WND_ACCESSARY:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_ACCESSARY));
			break;
		case WND_PIERCING:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_PIERCING));
			break;
		case WND_ELEMENT:
			SetTitle( prj.GetText( TID_GAME_SMELTSAFETY_ELEMENT ) );
			break;
	}

	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT )
	{
		CRect rect;
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_nResultStaticID[i]);
			assert(pWndStatic != NULL);
			rect = pWndStatic->GetWndRect();
			pWndStatic->Move(rect.left + EXTENSION_PIXEL, rect.top);
		}

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_NOW_GRADE);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_MAX_GRADE);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NOW_GRADE);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
		assert(pWndEdit != NULL);
		rect = pWndEdit->GetWndRect();
		pWndEdit->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_PLUS);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_MINUS);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_START);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_STOP);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_RESET);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CRect wndrect = GetWndRect();
		wndrect.right = wndrect.right + EXTENSION_PIXEL;
		SetWndRect(wndrect);
	}
	else
	{
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_Scroll2[i].staticNum);
			assert(pWndStatic != NULL);
			pWndStatic->SetVisible(FALSE);
			pWndStatic->EnableWindow(FALSE);
		}
	}

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	assert(pWndEdit != NULL);
	pWndEdit->AddWndStyle(EBS_NUMBER);
	pWndEdit->EnableWindow(FALSE);

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
	assert(pWndInventory != NULL);
	pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);

	m_pNowGaugeTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetyGauge.bmp"), 0xffff00ff);
	m_pSuccessTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetySuccess.bmp"), 0xffff00ff);
	m_pFailureTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetyFailure.bmp"), 0xffff00ff);

	MoveParentCenter();
}

BOOL CWndSmeltSafety::Process()
{
	if(g_pPlayer->m_vtInfo.GetOther() || g_pPlayer->m_vtInfo.VendorIsVendor() )
	{
		Destroy();
	}

	if(m_bStart != FALSE && m_pItemElem != NULL)
	{
		if(m_bResultSwitch != false)
		{
			m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwEnchantTimeStart) / static_cast<float>(m_dwEnchantTimeEnd - m_dwEnchantTimeStart);
		}
		else
		{
			m_fGaugeRate = 0.0f;
		}

		if(m_nValidSmeltCounter == 0 || GetNowSmeltValue() == GetDefaultMaxSmeltValue())
		{
			StopSmelting();
		}

		if(m_eWndMode == WND_PIERCING)
		{
			CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
			assert(pWndInventory != NULL);
			CWndStatic* pGoldNumberStatic = (CWndStatic*)pWndInventory->GetDlgItem(WIDC_GOLD_NUM);
			assert(pGoldNumberStatic != NULL);
			int nGoldNumber(atoi(pGoldNumberStatic->GetTitle()));
			if(nGoldNumber < 100000)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_LACKMONEY), NULL, prj.GetTextColor(TID_GAME_LACKMONEY));
				StopSmelting();
			}
		}
		if(m_dwEnchantTimeEnd < g_tmCurrent)
		{
			m_bResultSwitch = false;
			m_dwEnchantTimeStart = 0xffffffff;
			m_dwEnchantTimeEnd = 0xffffffff;

			// Send to Server...
			GENMATDIEINFO* pTargetMaterial = &m_Material[m_nCurrentSmeltNumber];
			GENMATDIEINFO* pTargetScroll1 = &m_Scroll1[m_nCurrentSmeltNumber];
			GENMATDIEINFO* pTargetScroll2 = &m_Scroll2[m_nCurrentSmeltNumber];
			if(pTargetMaterial->isUse != FALSE && m_Scroll1[m_nCurrentSmeltNumber].isUse != FALSE)
			{
				g_DPlay.SendSmeltSafety(
					m_pItemElem->m_dwObjId, 
					pTargetMaterial->pItemElem->m_dwObjId, 
					pTargetScroll1->pItemElem->m_dwObjId, 
					pTargetScroll2->isUse != FALSE ? pTargetScroll2->pItemElem->m_dwObjId : NULL_ID);
				
				pTargetMaterial->isUse = FALSE;
				if(pTargetMaterial->pItemElem != NULL)
				{
					pTargetMaterial->pItemElem->SetExtra(pTargetMaterial->pItemElem->GetExtra() - 1);
					pTargetMaterial->pItemElem = NULL;
				}
				pTargetScroll1->isUse = FALSE;
				if(pTargetScroll1->pItemElem != NULL)
				{
					pTargetScroll1->pItemElem->SetExtra(pTargetScroll1->pItemElem->GetExtra() - 1);
					pTargetScroll1->pItemElem = NULL;
				}
				if(pTargetScroll2->isUse != FALSE)
				{
					pTargetScroll2->pItemElem->SetExtra(pTargetScroll2->pItemElem->GetExtra() - 1);
					pTargetScroll2->isUse = FALSE;
					if(pTargetScroll2->pItemElem != NULL)
					{
						pTargetScroll2->pItemElem = NULL;
					}
				}
			}
			m_dwEnchantTimeStart = g_tmCurrent;
			m_dwEnchantTimeEnd = g_tmCurrent + SEC(ENCHANT_TIME);
		}
	}
	else
	{
		m_dwEnchantTimeStart = 0xffffffff;
		m_dwEnchantTimeEnd = 0xffffffff;
	}

	return TRUE;
}

void CWndSmeltSafety::OnDraw(C2DRender* p2DRender)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	assert(pWndStatic != NULL);
	CRect rectSmeltItem = pWndStatic->GetWndRect();

	if(m_pItemElem != NULL && m_pItemTexture != NULL)
	{
		// Draw Item
		m_pItemTexture->Render( p2DRender, CPoint( rectSmeltItem.left, rectSmeltItem.top ) );

		// Draw Enchant Effect
		if( m_dwEnchantTimeStart != 0xffffffff && m_dwEnchantTimeEnd != 0xffffffff )
		{
			RenderEnchant( p2DRender, CPoint(rectSmeltItem.left, rectSmeltItem.top) );
		}
	}

	// Draw Tooltip
	CPoint pointMouse = GetMousePoint();
	if(rectSmeltItem.PtInRect(pointMouse) != FALSE)
	{
		if(m_pItemElem != NULL)
		{
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectSmeltItem );
			g_WndMng.PutToolTip_Item(m_pItemElem, pointMouse, &rectSmeltItem);
		}
		else
		{
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectSmeltItem );
			CString strEmptyTooltip;
			strEmptyTooltip.Format("%s", prj.GetText(TID_GAME_TOOLTIP_SMELT_SAFETY_ITEM));
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectSmeltItem, pointMouse);
		}
	}

	DrawListItem(p2DRender);

	for(int i = 0; i < m_nCurrentSmeltNumber; ++i)
	{
		const int nExtensionPixel( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) ? EXTENSION_PIXEL : 0 );
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(m_nResultStaticID[i]);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		rectStaticTemp.BottomRight().x = lpStatic->rect.right + nExtensionPixel;
		if(m_bResultStatic[i] != false)
		{
			m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferSuccessImage, m_pSuccessTexture);
		}
		else
		{
			m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferSuccessImage, m_pFailureTexture);
		}
	}

	if(m_bStart != NULL && m_bResultSwitch != false)
	{
		const int nExtensionPixel( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) ? EXTENSION_PIXEL : 0 );
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(m_nResultStaticID[m_nCurrentSmeltNumber]);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth + nExtensionPixel;
		assert(m_pVertexBufferGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferGauge, m_pNowGaugeTexture);
	}
}

BOOL CWndSmeltSafety::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WIDC_BUTTON_START:
		{
			if(m_bStart != FALSE)
				break;

			if(m_pItemElem == NULL)
			{
				// ���� ���� ������ �õ��� �������� ����ؾ�? �մϴ�.
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR07), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR07));
				break;
			}

			if(m_nValidSmeltCounter > 0)
			{
				if(m_eWndMode == WND_PIERCING)
				{
					CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
					assert(pWndInventory != NULL);
					CWndStatic* pGoldNumberStatic = (CWndStatic*)pWndInventory->GetDlgItem(WIDC_GOLD_NUM);
					assert(pGoldNumberStatic != NULL);
					int nGoldNumber(atoi(pGoldNumberStatic->GetTitle()));
					if(nGoldNumber < 100000)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_LACKMONEY), NULL, prj.GetTextColor(TID_GAME_LACKMONEY));
						break;
					}
				}
				m_bStart = TRUE;
				m_bResultSwitch = true;
				m_dwEnchantTimeStart = g_tmCurrent;
				m_dwEnchantTimeEnd = g_tmCurrent + SEC(ENCHANT_TIME);
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				assert(pWndEdit != NULL);
				pWndEdit->EnableWindow(FALSE);
			}
			else
			{
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				assert(pWndEdit != NULL);

				if(GetNowSmeltValue() == GetDefaultMaxSmeltValue())
				{
					// �̹� �ִ�ġ���� ���õǾ� �� �̻� ������ �� �����ϴ�.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR13), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR13));
				}
				else if(m_nCurrentSmeltNumber >= SMELT_MAX)
				{
					// ���� ������ �����մϴ�. ���? �����ϱ⸦ ���Ͻø� Reset ��ư�� ���� �ʱ�ȭ�ؾ� �մϴ�.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR15), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR15));
				}
				else if(m_nMaterialCount <= m_nCurrentSmeltNumber || m_nScroll1Count <= m_nCurrentSmeltNumber)
				{
					switch(m_eWndMode)
					{
					case WND_NORMAL:
						{
							if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
							{
								// ����Į���? �Ϲ� ��ȣ�� �η縶���� �ϳ��� ��ϵ�? ���°� �ƴϸ� ������ �� �����ϴ�.
								g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR08), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR08));
							}
							else
							{
								// ������ ����Į���? �ֻ��? ��ȣ�� �η縶���� �ϳ��� ��ϵ�? ���°� �ƴϸ� ������ �� �����ϴ�.
								g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR09), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR09));
							}
							break;
						}
					case WND_ACCESSARY:
						{
							// �������? �׼����� ��ȣ�� �η縶���� �ϳ��� ��ϵ�? ���°� �ƴϸ� ������ �� �����ϴ�.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR10), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR10));
							break;
						}
					case WND_PIERCING:
						{
							// �������? �Ǿ��? ��ȣ�� �η縶���� �ϳ��� ��ϵ�? ���°� �ƴϸ� ������ �� �����ϴ�.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR11), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR11));
							break;
						}
					case WND_ELEMENT:
						{
							// �Ӽ� ī���? �Ϲ� ��ȣ�� �η縶���� �ϳ��� ��ϵ�? ���°� �ƴϸ� ������ �� �����ϴ�.
							g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR20 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR20 ) );
							break;
						}
					}
				}
				else if(GetNowSmeltValue() >= atoi(pWndEdit->GetString()))
				{
					// ������ ������ �� ���� �����Դϴ�.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR14), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR14));
				}
			}
			break;
		}
	case WIDC_BUTTON_STOP:
		{
			if(m_bStart != FALSE)
			{
				StopSmelting();
			}
			break;
		}
	case WIDC_BUTTON_RESET:
		{
			if(m_bStart == FALSE)
			{
				ResetData();
				RefreshInformation();
			}
			break;
		}
	case WIDC_EDIT_MAX_GRADE:
		{
			if(m_bStart == FALSE && m_pItemElem != NULL)
			{
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				assert(pWndEdit != NULL);
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = std::min(nMaxSmeltNumber, GetDefaultMaxSmeltValue());
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	case WIDC_BUTTON_PLUS:
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			assert(pWndEdit != NULL);
			if(pWndEdit->IsWindowEnabled() != FALSE && m_bStart == FALSE && m_pItemElem != NULL)
			{
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = (++nMaxSmeltNumber < GetDefaultMaxSmeltValue()) ? nMaxSmeltNumber : GetDefaultMaxSmeltValue();
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	case WIDC_BUTTON_MINUS:
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			assert(pWndEdit != NULL);
			if(pWndEdit->IsWindowEnabled() != FALSE && m_bStart == FALSE && m_pItemElem != NULL)
			{
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = (--nMaxSmeltNumber > 0) ? nMaxSmeltNumber : 0;
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndSmeltSafety::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame == NULL )
		return FALSE;

	if( pWndFrame->GetWndId() != APP_INVENTORY )
	{
		SetForbid( TRUE );
		return FALSE;
	}

	CRect WndRect = GetClientRect();
	if(WndRect.PtInRect(point))
	{
		CItemElem* pTempElem = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if(pTempElem == NULL)
			return TRUE;

		ItemProp* pItemProp = pTempElem->GetProp();
		if(pItemProp == NULL)
			return TRUE;

		SetItem(pTempElem);
	}

	return TRUE;
}

void CWndSmeltSafety::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(m_bStart != FALSE)
		return;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if(pWndStatic == NULL)
		return;

	CRect rect = pWndStatic->GetWndRect();
	if(rect.PtInRect(point))
	{
		// ���� �������� ����Ŭ���ϸ� Reset ��ư���� ��Ŀ���� ������ �ű��?
		CWndBase* pWndResetButtonBase = GetDlgItem(WIDC_BUTTON_RESET);
		assert(pWndResetButtonBase != NULL);
		pWndResetButtonBase->SetFocus();

		if(m_pItemElem != NULL)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
		}
		m_pItemTexture = NULL;

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
		assert(pWndEdit != NULL);
		pWndEdit->SetString(TEXT(""));
		pWndEdit->EnableWindow(FALSE);

		ResetData();
		RefreshInformation();
	}
	else
	{
		if(IsDropMaterialZone(point) != FALSE && m_nMaterialCount > m_nCurrentSmeltNumber)
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nMaterialCount > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Material[m_nMaterialCount - 1]);
					--m_nMaterialCount;

					while(m_nScroll1Count > m_nMaterialCount)
					{
						SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
						--m_nScroll1Count;
					}

					if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT )
					{
						while(m_nScroll2Count > m_nMaterialCount)
						{
							SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
							--m_nScroll2Count;
						}
					}
				}
			}
			else
			{
				SubtractListItem(&m_Material[m_nMaterialCount - 1]);
				--m_nMaterialCount;

				if(m_nScroll1Count > m_nMaterialCount)
				{
					SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
					--m_nScroll1Count;
				}

				if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && m_nScroll2Count > m_nMaterialCount )
				{
					SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
					--m_nScroll2Count;
				}
			}
			RefreshValidSmeltCounter();
			if( m_nMaterialCount == m_nCurrentSmeltNumber )
				m_pSelectedElementalCardItemProp = NULL;
		}
		else if(IsDropScroll1Zone(point) != FALSE && m_nScroll1Count > m_nCurrentSmeltNumber)
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nScroll1Count > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
					--m_nScroll1Count;
				}
			}
			else
			{
				SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
				--m_nScroll1Count;
			}
			RefreshValidSmeltCounter();
		}
		else if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && IsDropScroll2Zone(point) != FALSE && m_nScroll2Count > m_nCurrentSmeltNumber )
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nScroll2Count > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
					--m_nScroll2Count;
				}
			}
			else
			{
				SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
				--m_nScroll2Count;
			}
			RefreshValidSmeltCounter();
		}
	}
}

HRESULT CWndSmeltSafety::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVertexBufferGauge == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferGauge, 
												 NULL);
	assert(m_pVertexBufferGauge != NULL);

	if( m_pVertexBufferSuccessImage == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferSuccessImage, 
												 NULL);
	assert(m_pVertexBufferSuccessImage != NULL);

	if( m_pVertexBufferFailureImage == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferFailureImage, 
												 NULL);
	assert(m_pVertexBufferFailureImage != NULL);
	return S_OK;
}
HRESULT CWndSmeltSafety::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVertexBufferGauge );
	SAFE_RELEASE( m_pVertexBufferSuccessImage );
	SAFE_RELEASE( m_pVertexBufferFailureImage );
	return S_OK;
}
HRESULT CWndSmeltSafety::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	SAFE_RELEASE( m_pVertexBufferSuccessImage );
	SAFE_RELEASE( m_pVertexBufferFailureImage );
	return S_OK;
}

void CWndSmeltSafety::SetItem(CItemElem* pItemElem)
{
	assert(pItemElem != NULL);

	if(m_bStart != FALSE)
	{
		// ���� ���� ���߿��� �������� �����? �� �����ϴ�.
		g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR12), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR12));
		return;
	}

	ItemProp* pItemProp = pItemElem->GetProp();
	if(pItemProp == NULL)
		return;

	if(m_pItemElem == NULL) // ���� �������� �÷��� ���� ������
	{
		BOOL bAcceptableItem = FALSE;
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				if(CItemElem::IsDiceRefineryAble(pItemProp))
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// ������ �� ���� �������Դϴ�.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
				}
				break;
			}
		case WND_ACCESSARY:
			{
				if(pItemElem->IsCollector(TRUE) || pItemProp->dwItemKind2 == IK2_JEWELRY)
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// ������ �� ���� �������Դϴ�.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
				}
				break;
			}
		case WND_PIERCING:
			{
				if(pItemElem->IsPierceAble())
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// �Ǿ��? �� �� ���� �������Դϴ�.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR02), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR02));
				}
				break;
			}
		case WND_ELEMENT:
			{
				if( CItemElem::IsEleRefineryAble( pItemProp ) )
					bAcceptableItem = TRUE;
				else
				{
					// ������ �� ���� �������Դϴ�.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR01 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR01 ) );
				}
				break;
			}
		}

		if(bAcceptableItem && m_pItemElem == NULL && pItemElem != NULL)
		{
			m_pItemElem = pItemElem;
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);

			assert(pItemProp != NULL);
			m_pItemTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			assert(pWndEdit != NULL);
			CString strDefaultSmeltMaxValue;
			strDefaultSmeltMaxValue.Format("%d", GetDefaultMaxSmeltValue());
			pWndEdit->EnableWindow(TRUE);
			pWndEdit->SetString(strDefaultSmeltMaxValue);

			RefreshInformation();
		}
	}
	else
	{
		if(IsAcceptableMaterial(pItemProp) != FALSE)
		{
			if(m_nMaterialCount < SMELT_MAX)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					while(m_nMaterialCount < SMELT_MAX && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Material[m_nMaterialCount], pItemElem);
						++m_nMaterialCount;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Material[m_nMaterialCount], pItemElem);
						++m_nMaterialCount;
					}
				}
			}
		}
		else if(IsAcceptableScroll1(pItemProp) != FALSE)
		{
			if(m_nScroll1Count < m_nMaterialCount)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					while(m_nScroll1Count < m_nMaterialCount && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll1[m_nScroll1Count], pItemElem);
						++m_nScroll1Count;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll1[m_nScroll1Count], pItemElem);
						++m_nScroll1Count;
					}
				}
			}
			else if(m_nScroll1Count != SMELT_MAX)
			{
				switch(m_eWndMode)
				{
				case WND_NORMAL:
					{
						if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
						{
							// ���� ����Į���� ����ؾ�? �մϴ�.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR03), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR03));
						}
						else
						{
							// ���� ������ ����Į���� ����ؾ�? �մϴ�.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR04), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR04));
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// ���� �������� ����ؾ�? �մϴ�.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR05), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR05));
						break;
					}
				case WND_ELEMENT:
					{
						// ���� �Ӽ� ī�带 ����ؾ�? �մϴ�.
						g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR17 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR17 ) );
						break;
					}
				}
			}
		}
		else if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && IsAcceptableScroll2( pItemProp ) != FALSE )
		{
			if(m_nScroll2Count < m_nMaterialCount)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					m_nScroll2Count = (m_nCurrentSmeltNumber > m_nScroll2Count) ? m_nCurrentSmeltNumber : m_nScroll2Count;
					while(m_nScroll2Count < m_nMaterialCount && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll2[m_nScroll2Count], pItemElem);
						++m_nScroll2Count;
					}
				}
				else
				{
					m_nScroll2Count = (m_nCurrentSmeltNumber > m_nScroll2Count) ? m_nCurrentSmeltNumber : m_nScroll2Count;
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll2[m_nScroll2Count], pItemElem);
						++m_nScroll2Count;
					}
				}
			}
			else if(m_nScroll2Count != SMELT_MAX)
			{
				switch(m_eWndMode)
				{
				case WND_NORMAL:
					{
						if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
						{
							// ���� ����Į���� ����ؾ�? �մϴ�.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR03), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR03));
						}
						else
						{
							// ���� ������ ����Į���� ����ؾ�? �մϴ�.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR04), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR04));
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// ���� �������� ����ؾ�? �մϴ�.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR05), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR05));
						break;
					}
				case WND_ELEMENT:
					{
						// ���� �Ӽ� ī�带 ����ؾ�? �մϴ�.
						g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR17 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR17 ) );
						break;
					}
				}
			}
		}
		else
		{
			if( m_eWndMode == WND_ELEMENT && CItemElem::IsElementalCard( pItemProp->dwID ) == TRUE )
			{
				if( m_pItemElem->GetItemResist() != SAI79::NO_PROP )
				{
					// �̹� �����ۿ� �Ӽ� ������ �Ǿ� �ֽ��ϴ�. �� �����ۿ� �� �Ӽ��� ���? �� �����ϴ�.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR18 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR18 ) );
				}
				else if( pItemProp != m_pSelectedElementalCardItemProp )
				{
					// �̹� �ٸ� ������ �Ӽ� ī�尡 ��ϵǾ�? �ֽ��ϴ�.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR19 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR19 ) );
				}
			}
			else
			{
				// ���� �����ۿ� �´� ���? �η縶���� �ƴմϴ�.
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR06), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR06));
			}
		}
		RefreshValidSmeltCounter();
	}
}

void CWndSmeltSafety::RefreshInformation(void)
{
	RefreshText();
	RefreshValidSmeltCounter();
}

void CWndSmeltSafety::RefreshText(void)
{
	CString strItemAbility;
	if(m_pItemElem != NULL)
	{
		strItemAbility.Format("%d", GetNowSmeltValue());
	}
	else
	{
		strItemAbility.Format(TEXT("--"));
	}
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NOW_GRADE);
	assert(pWndStatic != NULL);
	pWndStatic->SetTitle(strItemAbility);
}

void CWndSmeltSafety::RefreshValidSmeltCounter(void)
{
	// ��ȿ�� ���� ī���͸� (�ٽ�) ����Ͽ�? �����ϴ� �ڵ�
	m_nValidSmeltCounter = m_nMaterialCount;
	m_nValidSmeltCounter = (m_nScroll1Count < m_nValidSmeltCounter) ? m_nScroll1Count : m_nValidSmeltCounter;
	m_nValidSmeltCounter -= m_nCurrentSmeltNumber;
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	assert(pWndEdit != NULL);
	int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
	int nNowSmeltNumber(GetNowSmeltValue());
	int nSmeltNumber(nMaxSmeltNumber - nNowSmeltNumber);
	nSmeltNumber = (nSmeltNumber > 0) ? nSmeltNumber : 0;
	m_nValidSmeltCounter = (nSmeltNumber < m_nValidSmeltCounter) ? nSmeltNumber : m_nValidSmeltCounter;
}

void CWndSmeltSafety::StopSmelting(void)
{
	m_bStart = FALSE;
	m_bResultSwitch = false;
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	assert(pWndEdit != NULL);
	pWndEdit->EnableWindow(TRUE);
}

void CWndSmeltSafety::DisableScroll2(void)
{
	assert(m_pItemElem != NULL);
	if( ( m_eWndMode == WND_NORMAL && ( m_pItemElem->GetAbilityOption() >= GENERAL_NON_USING_SCROLL2_LEVEL || m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE ) ) || 
		( m_eWndMode == WND_ELEMENT && m_pItemElem->GetResistAbilityOption() >= ELEMENTAL_NON_USING_SCROLL2_LEVEL ) )
	{
		m_nScroll2Count = 0;
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			GENMATDIEINFO* pTargetScroll2 = &m_Scroll2[i];
			if(pTargetScroll2->pItemElem != NULL)
			{
				pTargetScroll2->isUse = FALSE;
				pTargetScroll2->pItemElem->SetExtra(pTargetScroll2->pItemElem->GetExtra() - 1);
				pTargetScroll2->pItemElem = NULL;
			}
		}
	}
}

void CWndSmeltSafety::ResetData(void)
{
	m_nMaterialCount = 0;
	m_nScroll1Count = 0;
	m_nScroll2Count = 0;
	m_nCurrentSmeltNumber = 0;
	for(int i = 0; i < SMELT_MAX; ++i)
	{
		m_Material[i].isUse = FALSE;
		if(m_Material[i].pItemElem != NULL)
		{
			m_Material[i].pItemElem->SetExtra(0);
			m_Material[i].pItemElem = NULL;
		}
		m_Scroll1[i].isUse = FALSE;
		if(m_Scroll1[i].pItemElem != NULL)
		{
			m_Scroll1[i].pItemElem->SetExtra(0);
			m_Scroll1[i].pItemElem = NULL;
		}
		m_Scroll2[i].isUse = FALSE;
		if(m_Scroll2[i].pItemElem != NULL)
		{
			m_Scroll2[i].pItemElem->SetExtra(0);
			m_Scroll2[i].pItemElem = NULL;
		}

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_nResultStaticID[i]);
		assert(pWndStatic != NULL);
		pWndStatic->SetTitle("");
	}
	m_pSelectedElementalCardItemProp = NULL;
}

void CWndSmeltSafety::AddListItem(GENMATDIEINFO* pListItem, CItemElem* pItemElem)
{
	assert(pListItem->isUse == FALSE && pListItem->pItemElem == NULL);
	pListItem->isUse = TRUE;
	pListItem->pItemElem = pItemElem;
	pListItem->pItemElem->SetExtra(pItemElem->GetExtra() + 1);
}
void CWndSmeltSafety::SubtractListItem(GENMATDIEINFO* pListItem)
{
	assert(pListItem->isUse != FALSE && pListItem->pItemElem != NULL);
	pListItem->isUse = FALSE;
	pListItem->pItemElem->SetExtra(pListItem->pItemElem->GetExtra() - 1);
	pListItem->pItemElem = NULL;
}

void CWndSmeltSafety::DrawListItem(C2DRender* p2DRender)
{
	if(m_eWndMode == WND_NORMAL && (m_pItemElem == NULL || m_pItemTexture == NULL))
		return;

	static const int NORMAL_ALPHA(255);
	static const int TRANSLUCENT_ALPHA(75);
	static int nAlphaBlend(NORMAL_ALPHA);
	ItemProp* pItemProp = NULL;
	CTexture* pTexture = NULL;

	for(int i = m_nCurrentSmeltNumber; i < SMELT_MAX; ++i)
	{
		assert(m_Material[i].wndCtrl != NULL);
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				pItemProp = (m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE) ? prj.GetItemProp(II_GEN_MAT_ORICHALCUM02) : prj.GetItemProp(II_GEN_MAT_ORICHALCUM01);
				break;
			}
		case WND_ACCESSARY:
			{
				pItemProp = prj.GetItemProp( II_GEN_MAT_MOONSTONE );
				break;
			}
		case WND_PIERCING:
			{
				pItemProp = prj.GetItemProp( II_GEN_MAT_MOONSTONE );
				break;
			}
		case WND_ELEMENT:
			{
				pItemProp = m_pSelectedElementalCardItemProp;
				if( m_pItemElem )
				{
					switch( m_pItemElem->GetItemResist() )
					{
					case SAI79::FIRE:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_FLAME );
							break;
						}
					case SAI79::WATER:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_RIVER );
							break;
						}
					case SAI79::ELECTRICITY:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_GENERATOR );
							break;
						}
					case SAI79::WIND:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_CYCLON );
							break;
						}
					case SAI79::EARTH:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_DESERT );
							break;
						}
					}
				}
				break;
			}
		}
		if( m_eWndMode != WND_ELEMENT || pItemProp )
		{
			assert(pItemProp != NULL);
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			assert(pTexture != NULL);
			nAlphaBlend = (m_Material[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Material[i].wndCtrl->rect.left, m_Material[i].wndCtrl->rect.top ), nAlphaBlend );
		}

		assert(m_Scroll1[i].wndCtrl != NULL);
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				pItemProp = (m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE) ? prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT3) : prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT);
				break;
			}
		case WND_ACCESSARY:
			{
				pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT4 );
				break;
			}
		case WND_PIERCING:
			{
				pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PIEPROT );
				break;
			}
		case WND_ELEMENT:
			{
				pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT );
				break;
			}
		}
		assert(pItemProp != NULL);
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		assert(pTexture != NULL);
		nAlphaBlend = (m_Scroll1[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render( p2DRender, CPoint( m_Scroll1[i].wndCtrl->rect.left, m_Scroll1[i].wndCtrl->rect.top ), nAlphaBlend );

		if( m_eWndMode == WND_NORMAL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE && m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL )
		{
			assert(m_Scroll2[i].wndCtrl != NULL);
			pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_SMELTING);
			assert(pItemProp != NULL);
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			assert(pTexture != NULL);
			nAlphaBlend = (m_Scroll2[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Scroll2[i].wndCtrl->rect.left, m_Scroll2[i].wndCtrl->rect.top ), nAlphaBlend );
		}
		if( m_eWndMode == WND_ELEMENT && m_pItemElem && m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
		{
			assert( m_Scroll2[ i ].wndCtrl != NULL );
			pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELTING2 );
			assert( pItemProp != NULL );
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );
			assert( pTexture != NULL );
			nAlphaBlend = ( m_Scroll2[ i ].isUse != FALSE ) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Scroll2[ i ].wndCtrl->rect.left, m_Scroll2[ i ].wndCtrl->rect.top ), nAlphaBlend );
		}
	}
}

BOOL CWndSmeltSafety::IsDropMaterialZone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Material[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsDropScroll1Zone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Scroll1[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsDropScroll2Zone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Scroll2[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsAcceptableMaterial(ItemProp* pItemProp)
{
	assert(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
			{
				if(pItemProp->dwID == II_GEN_MAT_ORICHALCUM01 || pItemProp->dwID == II_GEN_MAT_ORICHALCUM01_1)
					bAcceptableItem = TRUE;
			}
			else
			{
				if(pItemProp->dwID == II_GEN_MAT_ORICHALCUM02)
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ACCESSARY:
	case WND_PIERCING:
		{
			if(pItemProp->dwID == II_GEN_MAT_MOONSTONE || pItemProp->dwID == II_GEN_MAT_MOONSTONE_1)
				bAcceptableItem = TRUE;
			break;
		}
	case WND_ELEMENT:
		{
			switch( m_pItemElem->GetItemResist() )
			{
			case SAI79::FIRE:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_FLAME )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::WATER:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_RIVER )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::ELECTRICITY:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_GENERATOR )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::WIND:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_CYCLON )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::EARTH:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_DESERT )
						bAcceptableItem = TRUE;
					break;
				}
			default:
				{
					if( m_nMaterialCount == m_nCurrentSmeltNumber )
					{
						if( CItemElem::IsElementalCard( pItemProp->dwID ) == TRUE )
						{
							m_pSelectedElementalCardItemProp = pItemProp;
							bAcceptableItem = TRUE;
						}
					}
					else
					{
						if( pItemProp == m_pSelectedElementalCardItemProp )
							bAcceptableItem = TRUE;
					}
				}
			}
			break;
		}
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll1(ItemProp* pItemProp)
{
	assert(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
			{
				if(pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT)
					bAcceptableItem = TRUE;
			}
			else
			{
				if(pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT3)
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ACCESSARY:
		{
			if(pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT4)
				bAcceptableItem = TRUE;
			break;
		}
	case WND_PIERCING:
		{
			if(pItemProp->dwID == II_SYS_SYS_SCR_PIEPROT)
				bAcceptableItem = TRUE;
			break;
		}
	case WND_ELEMENT:
		{
			if( pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT )
				bAcceptableItem = TRUE;
			break;
		}
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll2(ItemProp* pItemProp)
{
	assert(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch( m_eWndMode )
	{
	case WND_NORMAL:
		{
			if( m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
			{
				if( pItemProp->dwID == II_SYS_SYS_SCR_SMELTING )
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ELEMENT:
		{
			if( m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
			{
				if( pItemProp->dwID == II_SYS_SYS_SCR_SMELTING2 )
					bAcceptableItem = TRUE;
			}
			break;
		}
	}
	return bAcceptableItem;
}

int CWndSmeltSafety::GetNowSmeltValue(void)
{
	int nNowSmeltValue(0);
	if(m_pItemElem != NULL)
	{
		if(m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY)
		{
			nNowSmeltValue = m_pItemElem->GetAbilityOption();
		}
		else if(m_eWndMode == WND_PIERCING)
		{
			nNowSmeltValue = m_pItemElem->GetPiercingSize();
		}
		else if( m_eWndMode == WND_ELEMENT )
			nNowSmeltValue = m_pItemElem->GetResistAbilityOption();
	}
	return nNowSmeltValue;
}

int CWndSmeltSafety::GetDefaultMaxSmeltValue(void)
{
	assert(m_pItemElem != NULL);
	int nDefaultMaxSmeltValue(0);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			nDefaultMaxSmeltValue = 10;
			break;
		}
	case WND_ACCESSARY:
		{
			nDefaultMaxSmeltValue = 20;
			break;
		}
	case WND_PIERCING:
		{
			if(m_pItemElem->GetProp()->dwItemKind3 == IK3_SUIT)
			{
				nDefaultMaxSmeltValue = 4;
			}
			else
			{
				nDefaultMaxSmeltValue = 10;
			}
			break;
		}
	case WND_ELEMENT:
		{
			nDefaultMaxSmeltValue = 20;
			break;
		}
	}
	return nDefaultMaxSmeltValue;
}

CWndSmeltSafetyConfirm::CWndSmeltSafetyConfirm(ErrorMode eErrorMode) : 
m_eErrorMode(eErrorMode), 
m_pItemElem(NULL)
{
}

CWndSmeltSafetyConfirm::~CWndSmeltSafetyConfirm()
{
}

BOOL CWndSmeltSafetyConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( APP_SMELT_SAFETY_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
}

void CWndSmeltSafetyConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_CONFIRM_TEXT);
	assert(pWndText != NULL);

	switch(m_eErrorMode)
	{
	case WND_ERROR1:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_NORMAL));
			break;
		}
	case WND_ERROR2:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_HIGHEST));
			break;
		}
	case WND_ERROR3:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_ACCESSARY));
			break;
		}
	}

	MoveParentCenter();
}

BOOL CWndSmeltSafetyConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);

	if(pWndInventory != NULL)
	{
		if(nID == WIDC_SMELT_YES)
		{
			if(m_pItemElem != NULL)
				pWndInventory->RunUpgrade(m_pItemElem);

			Destroy();
		}
		else if(nID == WIDC_SMELT_NO)
		{
			pWndInventory->BaseMouseCursor();
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSmeltSafetyConfirm::OnDestroy()
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory != NULL)
		pWndInventory->BaseMouseCursor();
}

void CWndSmeltSafetyConfirm::SetWndMode(CItemElem* pItemElem)
{
	m_pItemElem = pItemElem;	
}

CWndEquipBindConfirm::CWndEquipBindConfirm(EquipAction eEquipAction) : 
m_eEquipAction(eEquipAction), 
m_pItemBase(NULL), 
m_dwObjId(0), 
m_pItemElem( NULL )
{
}

CWndEquipBindConfirm::~CWndEquipBindConfirm(void)
{
}

BOOL CWndEquipBindConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( APP_EQUIP_BIND_CONFIRM, pWndParent, WBS_MODAL | WBS_KEY, CPoint( 0, 0 ) );
}

void CWndEquipBindConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_EQUIP_BIND_TEXT);
	assert(pWndText != NULL);
	pWndText->AddString(prj.GetText(TID_TOOLTIP_EQUIPBIND_CONFIRM));

	CWndEdit* pWndEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT_FOCUS );
	assert( pWndEdit );
	pWndEdit->Move( -100, -100 );
	pWndEdit->SetFocus();

	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_EQUIP_BIND_YES );
	assert( pWndButton );
	pWndButton->SetDefault( TRUE );

	MoveParentCenter();
}

BOOL CWndEquipBindConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT_FOCUS )
	{
		EquipItem();
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}

	switch( nID )
	{
	case WIDC_EQUIP_BIND_YES:
		{
			switch( m_eEquipAction )
			{
			case EQUIP_DOUBLE_CLICK:
				{
					EquipItem();
					Destroy();
					break;
				}
			case EQUIP_DRAG_AND_DROP:
				{
					g_DPlay.SendDoEquip( m_pItemElem );
					Destroy();
					break;
				}
			default:
				{
					Destroy();
				}
			}
			break;
		}
	case WIDC_EQUIP_BIND_NO:
		{
			Destroy();
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndEquipBindConfirm::SetInformationDoubleClick(CItemElem * pItemBase, DWORD dwObjId)
{
	m_eEquipAction = EQUIP_DOUBLE_CLICK;
	m_pItemBase = pItemBase;
	assert(m_pItemBase != NULL);
	m_dwObjId = dwObjId;
}

void CWndEquipBindConfirm::SetInformationDragAndDrop(CItemElem* pItemElem)
{
	m_eEquipAction = EQUIP_DRAG_AND_DROP;
	m_pItemElem = pItemElem;
	assert(m_pItemElem != NULL);
}

void CWndEquipBindConfirm::EquipItem( void )
{
	ItemProp* pItemProp = m_pItemBase->GetProp();
	if( pItemProp != NULL)
	{
		int nPart = pItemProp->dwParts;
		BOOL bEquiped = g_pPlayer->m_Inventory.IsEquip( m_pItemBase->m_dwObjId );
		if( bEquiped != FALSE )
			nPart = m_pItemBase->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;
		g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_pItemBase->m_dwObjId ), m_dwObjId, nPart );
	}
}


CWndRestateConfirm::CWndRestateConfirm(DWORD dwItemID) : 
m_dwItemID(dwItemID), 
m_ObjID(0), 
m_nPart(-1)
{
}

CWndRestateConfirm::~CWndRestateConfirm(void)
{
}

BOOL CWndRestateConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
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
BOOL CWndCampusInvitationConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
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
	pOk->SetDefault( TRUE );

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
BOOL CWndCampusSeveranceConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
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
	pOk->SetDefault( TRUE );

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
