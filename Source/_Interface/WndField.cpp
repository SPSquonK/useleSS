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

extern int g_nSkillCurSelect;
extern float g_fHairLight;

#include "eveschool.h"
#include "dpcertified.h"

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
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
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
CWndQueryEquip::CWndQueryEquip()
{
	m_ObjID = NULL_ID;
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );

	m_pModel = NULL;
	m_OldPos = CPoint(0,0);
}
CWndQueryEquip::~CWndQueryEquip()
{
	SAFE_DELETE( m_pModel );
}

BOOL CWndQueryEquip::Process()
{
	CMover* pMover = GetMover();

	if( IsInvalidObj(pMover) )
	{
		Destroy();
		return FALSE;
	}


	if( m_pModel )
		m_pModel->FrameMove();
		
	return TRUE;
}	
void CWndQueryEquip::OnMouseWndSurface( CPoint point )
{
	CMover* pMover = GetMover();

	if( IsInvalidObj(pMover) )
		return ;
	
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		if( pMover->m_aEquipInfo[i].dwId == NULL_ID )
			continue;
		
		CRect DrawRect = m_InvenRect[i];
		
		CPoint point = GetMousePoint();
		// ����
		if( DrawRect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			
			CItemElem itemElem;
			itemElem.m_dwItemId	= pMover->m_aEquipInfo[i].dwId;
			itemElem.m_byFlag	= pMover->m_aEquipInfo[i].byFlag;
			itemElem.SetAbilityOption( pMover->m_aEquipInfo[i].nOption & 0xFF );
			itemElem.m_nResistAbilityOption = m_aEquipInfoAdd[i].nResistAbilityOption;
			itemElem.m_bItemResist	= m_aEquipInfoAdd[i].bItemResist;
			itemElem.SetRandomOptItemId( m_aEquipInfoAdd[i].iRandomOptItemId );
			itemElem.CopyPiercing( m_aEquipInfoAdd[i].piercing );
			
			// ����?�� �ִ°� ����
			g_WndMng.PutToolTip_Item( &itemElem, point2, &DrawRect, APP_QUERYEQUIP );
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

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		FLOAT sx = 1.0f, sy = 1.0f;

		DWORD dwAlpha = 255;

		if( pMover->m_aEquipInfo[i].dwId == NULL_ID )
			continue;

		ItemProp* pItemProp	= prj.GetItemProp( pMover->m_aEquipInfo[i].dwId );
		if( !pItemProp )
			continue;

		if( m_aEquipInfoAdd[i].pTexture == NULL )
			continue;

		if( i == PARTS_LWEAPON )		// �޼չ��� �׸�Ÿ�̹��϶�
		{
			if( pItemProp->dwHanded == HD_TWO )	// ���ڵ� �����?
			{
				dwAlpha   = 100;
			}
		}
		
		CRect DrawRect = m_InvenRect[i];
		CPoint cpAdd = CPoint(6,6);
		
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

		if( pMover->m_aEquipInfo[i].byFlag & CItemElem::expired )
		{
			p2DRender->RenderTexture2( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, sx, sy, D3DCOLOR_XRGB( 255, 100, 100 ) );
		}
		else
		{
			p2DRender->RenderTexture( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, dwAlpha, sx, sy );
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
	
	pMover->OverCoatItemRenderCheck(m_pModel);
		
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
								((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );
							
							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
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
								((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
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
	
	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM6 );
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM7 );
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM8 );
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM9 );
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl( WIDC_CUSTOM10 );
	
	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;
	
	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;
	
	lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM11 );
	lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM12 );
	lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM13 );
	lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM14 );
	lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM15 );
	lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM16 );
	lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM17 );
	lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM18 );
	lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM19 );
	
	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;
	
	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;
	
	MoveParentCenter();
}

void CWndQueryEquip::SetMover( DWORD ObjID )
{
	m_ObjID = ObjID;

	SAFE_DELETE( m_pModel );
	
	if( GetMover() )
	{
		int nMover = (GetMover()->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
		CMover::UpdateParts( GetMover()->GetSex(), GetMover()->m_dwSkinSet, GetMover()->m_dwFace, GetMover()->m_dwHairMesh, GetMover()->m_dwHeadMesh,GetMover()->m_aEquipInfo, m_pModel, NULL );
		m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	}
}

void CWndQueryEquip::SetEquipInfoAdd( EQUIP_INFO_ADD* aEquipInfoAdd )
{
//	memcpy( m_aEquipInfoAdd, aEquipInfoAdd, sizeof(EQUIP_INFO_ADD) * MAX_HUMAN_PARTS );
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		m_aEquipInfoAdd[i]	= aEquipInfoAdd[i];

	CMover* pMover = GetMover();
	
	if( IsInvalidObj(pMover) )
		return ;
	
#ifdef __CLIENT
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( pMover->m_aEquipInfo[i].dwId != NULL_ID )
		{
			ItemProp* pItemProp	= prj.GetItemProp( pMover->m_aEquipInfo[i].dwId );
			if( pItemProp )
				m_aEquipInfoAdd[i].pTexture	= CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		}
	}
#endif	// __CLIENT
}

BOOL CWndQueryEquip::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();

	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768

	// �κ��丮 ���? ��ġ ����
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
	return CWndNeuz::InitDialog( dwWndId, pWndParent, 0, CPoint( 792, 130 ) );
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
	
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
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
	CWndSummonAngel* pWndAngel = (CWndSummonAngel*)GetWndBase( APP_SUMMON_ANGEL );
	if(pWndAngel != NULL)
		pWndAngel->Destroy();

#ifdef __EVE_MINIGAME
	CWndFindWordGame* pWndWordGame = (CWndFindWordGame*)GetWndBase( APP_MINIGAME_WORD );
	if(pWndWordGame != NULL)
		pWndWordGame->Destroy();
	
	CWndPuzzleGame* pWndPuzzleGame = (CWndPuzzleGame*)GetWndBase( APP_MINIGAME_PUZZLE );
	if(pWndPuzzleGame != NULL)
		pWndPuzzleGame->Destroy();
#endif //__EVE_MINIGAME
	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
	if(pWndMixJewel != NULL)
		pWndMixJewel->Destroy();

	CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase( APP_SMELT_EXTRACTION );
	if(pWndExtraction != NULL)
		pWndExtraction->Destroy();

#ifdef __WINDOW_INTERFACE_BUG
	CWndPiercing* pWndPiercing = (CWndPiercing*)GetWndBase( APP_PIERCING );
	if(pWndPiercing != NULL)
		pWndPiercing->Destroy();
	CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase( APP_REMOVE_ATTRIBUTE );
	if(pWndRemoveAttribute != NULL)
		pWndRemoveAttribute->Destroy();
	CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
	if(pWndRemovePiercing != NULL)
		pWndRemovePiercing->Destroy();
	CWndRemoveJewel* pWndRemoveJewel = (CWndRemoveJewel*)GetWndBase( APP_SMELT_REMOVE_JEWEL );
	if(pWndRemoveJewel != NULL)
		pWndRemoveJewel->Destroy();
	CWndLvReqDown* pWndLvReqDown = (CWndLvReqDown*)GetWndBase( APP_LVREQDOWN );
	if(pWndLvReqDown != NULL)
		pWndLvReqDown->Destroy();
	CWndBlessingCancel* pWndBlessingCancel = (CWndBlessingCancel*)GetWndBase( APP_CANCEL_BLESSING );
	if(pWndBlessingCancel != NULL)
		pWndBlessingCancel->Destroy();
	CWndUpgradeBase* pWndUpgradeBase = (CWndUpgradeBase*)GetWndBase( APP_TEST );
	if(pWndUpgradeBase != NULL)
		pWndUpgradeBase->Destroy();
#endif // __WINDOW_INTERFACE_BUG

	CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase( APP_SMELT_SAFETY );
	if(pWndSmeltSafety != NULL)
		pWndSmeltSafety->Destroy();

	CWndSmeltSafetyConfirm* pWndSmeltSafetyConfirm = (CWndSmeltSafetyConfirm*)GetWndBase( APP_SMELT_SAFETY_CONFIRM );
	if(pWndSmeltSafetyConfirm != NULL)
		pWndSmeltSafetyConfirm->Destroy();

	CWndEquipBindConfirm* pWndEquipBindConfirm = (CWndEquipBindConfirm*)GetWndBase(APP_EQUIP_BIND_CONFIRM);
	if(pWndEquipBindConfirm != NULL)
		pWndEquipBindConfirm->Destroy();

	CWndRestateConfirm* pWndRestateConfirm = (CWndRestateConfirm*)GetWndBase(APP_RESTATE_CONFIRM);
	if(pWndRestateConfirm != NULL)
		pWndRestateConfirm->Destroy();

	CWndPetFoodMill* pWndPetFoodMill = ( CWndPetFoodMill* )GetWndBase( APP_PET_FOODMILL );
	if(pWndPetFoodMill != NULL)
		pWndPetFoodMill->Destroy();

	CWndShop* pWndShop = ( CWndShop* )GetWndBase( APP_SHOP_ );
	if( pWndShop != NULL )
		pWndShop->Destroy();

	CWndBank* pWndBank = ( CWndBank* )GetWndBase( APP_COMMON_BANK );
	if( pWndBank != NULL )
		pWndBank->Destroy();
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

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM6 );
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM7 );
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM8 );
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM9 );
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl( WIDC_CUSTOM10 );
	
	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM11 );
	lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM12 );
	lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM13 );
	lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM14 );
	lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM15 );
	lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM16 );
	lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM17 );
	lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM18 );
	lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM19 );
	
	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;

	SAFE_DELETE( m_pModel );

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );
	m_wndItemCtrl.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 11 );
	m_wndItemCtrl.InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = GETTEXT( TID_GAME_ITEM );//"������";
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem( 0, &tabTabItem );

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
BOOL CWndInventory::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768

	// �κ��丮 ���? ��ġ ����
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
	return CWndNeuz::InitDialog( dwWndId, pWndParent, 0, CPoint( 792, 130 ) );
//	return CWndNeuz::Create( WBS_VIEW | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME, rect, pWndParent, dwWndId );
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
			//ADDERRORMSG( "CWndInventory::OnChildNotify : m_pFromWnd �� NULL " );
			return CWndNeuz::OnChildNotify( message, nID, pLResult );
		}
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

		if( pWndFrame == NULL )
		{
			LPCTSTR szErr = Error( "CWndInventory::OnChildNotify : pWndFrame==NULL" );
			//ADDERRORMSG( szErr );
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
				g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );
				return 0;
			}

			if( GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01) );
				return 0;
			}

			if( GetWndBase(APP_COMMITEM_DIALOG) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR) );
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
					if (CWndSummonAngel * pWndSummonAngel = (CWndSummonAngel *)g_WndMng.GetWndBase(APP_SUMMON_ANGEL)) {
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
							g_WndMng.PutString( prj.GetText( TID_SBEVE_NOTUSEITEM ), NULL, prj.GetTextColor( TID_SBEVE_NOTUSEITEM ) );
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
							g_WndMng.PutString( prj.GetText(TID_GAME_ERROR_SKILLRECCURENCE), NULL, prj.GetTextColor(TID_GAME_ERROR_SKILLRECCURENCE) );

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

					if( pFocusItem->m_bCharged != 1 )
					{
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
						case II_SYS_SYS_SCR_CHACLA:
							{
								if( g_pPlayer->IsBaseJob() )
								{
									g_WndMng.PutString( prj.GetText( TID_GAME_NOTUSEVAG ), NULL, prj.GetTextColor( TID_GAME_NOTUSEVAG ) );
									bAble = FALSE;
								}
								else
								{
									for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++ )
									{
										if( dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_RIDE )
											continue;
										CItemElem* pArmor	= g_pPlayer->m_Inventory.GetEquip( dwParts );
										if( pArmor )
										{
											g_WndMng.PutString( prj.GetText( TID_GAME_CHECK_EQUIP ), NULL, prj.GetTextColor( TID_GAME_CHECK_EQUIP ) );
											bAble = FALSE;
										}
									}
								}

								if( bAble )
								{
									SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
									g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
									g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
									g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_WARNINGCCLS, pProp->dwID, pProp->dwID );
									bAble = FALSE;
								}
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
					//g_WndMng.PutString( "�ŷ��߿� ������ �����? �Ұ����ؿ�.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
				else if(g_WndMng.GetWndBase( APP_SUMMON_ANGEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
#ifdef __EVE_MINIGAME
				else if(g_WndMng.GetWndBase( APP_MINIGAME_DICE ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO_WIN ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_WORD ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#endif //__EVE_MINIGAME
				else if(g_WndMng.GetWndBase( APP_SMELT_EXTRACTION ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_JEWEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_PET_FOODMILL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_SAFETY ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#ifdef __QUIZ
				else if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
				{
					g_WndMng.PutString( prj.GetText( TID_SBEVE_NOTUSEITEM ), NULL, prj.GetTextColor( TID_SBEVE_NOTUSEITEM ) );
				}
#endif // __QUIZ
				else if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					//g_WndMng.PutString( "�����߿� ������ �����? �Ұ����ؿ�.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
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

void CWndInventory::SetEnchantCursor()
{
	// �κ�â�� �����ְ� ��þƮ ����̸�? Ŀ�����? ����
	if( m_bIsUpgradeMode )
	{
		SetMouseCursor( CUR_HAMMER );
	}
	else
	{
		SetMouseCursor( CUR_BASE );		
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
			//g_WndMng.PutString( "�ŷ��߿� ������ �����? �Ұ����ؿ�.", NULL, 0xffff0000 );
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
			
			return FALSE;
		}
	}
	else if( g_WndMng.GetWndBase( APP_REPAIR ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			SetForbid( TRUE );
			//g_WndMng.PutString( "�����߿� ������ �����? �Ұ����ؿ�.", NULL, 0xffff0000 );
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� ����
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndCharInfo::CWndCharInfo()
{
	m_nStrCount = 0;
	m_nStaCount = 0;
	m_nDexCount = 0;
	m_nIntCount = 0;
	m_nGpPoint = 0;

	m_nATK = 0;
	m_nDEF = 0;
	m_nCritical = 0;
	m_nATKSpeed = 0;

	m_bExpert = FALSE;
	m_pWndChangeJob = NULL;
	m_fWaitingConfirm	= FALSE;
	m_nDisplay = 1;

}


CWndCharInfo::~CWndCharInfo()
{
	SAFE_DELETE(m_pWndChangeJob);
}

void CWndCharInfo::OnDraw(C2DRender* p2DRender)
{

	CRect rect = GetClientRect();
	rect.bottom -= 30;
	int			y = 0, nNext = 15, nyAdd3 = 21;
	DWORD		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	BYTE		checkhero = g_pPlayer->GetLegendChar();
	int			xpos = 0;
	int			ypos = 0;
	CTexture*	pTexture;
	CString		strPath;
	CPoint		point;

	y = 13;
	p2DRender->TextOut( 60, y, prj.GetText((TID_APP_CHARACTER_BASE)), dwColor );
	y = 10 + nyAdd3;
	if( TRUE ) //::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}
		if(checkhero == LEGEND_CLASS_HERO)
			xpos = 103;
			p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;

		y = 81+ nyAdd3;
	}
	else
	{
		p2DRender->TextOut( 50, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 50, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}
		if(checkhero == LEGEND_CLASS_HERO)
			xpos = 73;
			p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
	
		y = 81 + nyAdd3;
	}

	point.x = xpos;
	point.y = ypos - 2;
	if(checkhero == LEGEND_CLASS_MASTER) //������ ���� ���?.
	{
		if(/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70) //Level Down�� ���? �����ؼ� �ּ�ó��
			strPath = MakePath( DIR_ICON, "Icon_MasterMark1.dds");
		else if(g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark2.dds");
		else if(g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark3.dds");
		else if(g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark4.dds");
		else if(g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark5.dds");
		else if(g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );			
		
	}
	else if(checkhero == LEGEND_CLASS_HERO) //������ ���?.
	{
		strPath = MakePath( DIR_ICON, "Icon_HeroMark.dds");
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );
	}
	//���� ����
	y = 55+ nyAdd3;
	CString strServerName;
	strServerName.Format( "%s", g_dpCertified.GetServerName(g_Option.m_nSer) );
	if( TRUE ) //::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, strServerName, dwColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( 50, y, strServerName, dwColor );
		y += nNext;
	}

	//ä�� ����
	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for( int j = 0; j < (int)( g_dpCertified.m_dwSizeofServerset ); j++ )
	{
		if(g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if(nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if(g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if(pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format( "%s",  g_dpCertified.m_aServerset[j+g_Option.m_nMSer].lpName );
				if( TRUE ) //::GetLanguage() == LANG_JAP )
					p2DRender->TextOut( 80, y, strServerName, dwColor );
				else
					p2DRender->TextOut( 50, y, strServerName, dwColor );

				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}
	/*
	y = 96;
	
	p2DRender->TextOut( 96, y, g_pPlayer->GetFlightLv(), dwColor ); y += nNext;
	{
		char szBuff[64];
		int nMaxFxp = prj.m_aFxpCharacter[ g_pPlayer->GetFlightLv() + 1 ].dwFxp;
		sprintf( szBuff, "%5.2f%%", (float)g_pPlayer->GetFxp() * 100.0f / (float)nMaxFxp );
		p2DRender->TextOut( 90, y, szBuff, dwColor ); y += nNext;
	}
	*/
	y = 10+ nyAdd3;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_01), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_APP_CHARACTER_JOB), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_02), dwColor ); y += nNext;
	//p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_03), dwColor ); y += nNext;
	//�������� -> ����/ä�� ����
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVER), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), dwColor ); y += nNext;
	/*
	y += 10;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_04), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_05), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_06), dwColor );
	*/

	p2DRender->TextOut( 60, 113, prj.GetText((TID_APP_CHARACTER_DETAIL)), D3DCOLOR_ARGB(255,0,0,0));
	/////////////////////////// detail begin //////////////////////////////////
	int nyAdd = 121;
	int x = 5, nNextX = 100;

	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	x = 50; y = 10 + nyAdd;
	nNext = 15;
	// ���ݷ�
	RenderATK( p2DRender, x, y );
	y += nNext;

	//����
	if(	m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense( FALSE ) ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		p2DRender->TextOut( x , y, GetVirtualDEF(), dwColor ); y += nNext;
	}
	else
		p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext;

	x = 140; y = 10 + nyAdd;

	//ũ��Ƽ��
	CString strMsg;
	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	if(	m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb() ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		strMsg.Format( "%d%%", GetVirtualCritical() );
	}
	else
		strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	//���ݼӵ�	
	float fAttackSpeed;
	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	if(	m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed() ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		fAttackSpeed = GetVirtualATKSpeed();
	}
	else
		fAttackSpeed = g_pPlayer->GetAttackSpeed();

	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	x =15; nNextX = 60;
	// �Ʒ����� �ɷ�ġ ���� 
	y = 52 + nyAdd;
	int StatYPos = 50;

	if( g_pPlayer->m_nStr == g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nStr < g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

	if( g_pPlayer->m_nSta == g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nSta < g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

	if( g_pPlayer->m_nDex == g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nDex < g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

	if( g_pPlayer->m_nInt == g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nInt < g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetInt(), dwColor ); y += nNext;

	if(m_nGpPoint)
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
	}

	p2DRender->TextOut( 105, y, m_nGpPoint, dwColor ); y += nNext;

	CRect rectHittest[5];

	rectHittest[0].SetRect( 10, 52+ nyAdd, 80, 65 + nyAdd);
	rectHittest[1].SetRect( 10, 67+ nyAdd, 80, 80 + nyAdd);
	rectHittest[2].SetRect( 10, 82+ nyAdd, 80, 95 + nyAdd);
	rectHittest[3].SetRect( 10, 97+ nyAdd, 80, 110 + nyAdd);
	rectHittest[4].SetRect( 10, 112+ nyAdd, 160, 125 + nyAdd);

	CRect rectTemp;
	CPoint ptTemp;
	// ���� ������ �ϱ�( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );
			DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );
			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else // GP
				strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_GPPOINT ) );
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10 + nyAdd;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 85, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 85, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;
	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;
	
	//�ɷ�ġ ���� Tooltip
	rect.SetRect( 7, 10+ nyAdd, 160, 38 + nyAdd);
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS1 ) );
		else
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS2 ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
	//Edit Tooltip
	rect.SetRect(90, 52+ nyAdd, 160, 110+ nyAdd);
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_EDIT ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	//Button Tooltip
	rect = m_wndApply.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_APPLY ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	rect = m_wndReset.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_RESET ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}

	//////////////// pvp /////////////////////////
	
	if( g_pPlayer->IsBaseJob() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}
	else if( g_pPlayer->IsExpert() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}

	//CRect rect = GetClientRect();
	//rect.bottom -= 30;
	int nyAdd2 = 284;
	y = 15 + nyAdd2, nNext = 15;
	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szBuff[32];
	int gap1 = 0;
	int gap2 = 0;
	gap1 -= 10;
	gap2 -= 10;
	
	p2DRender->TextOut( 60, 281, prj.GetText((TID_GAME_CHARACTTER_PVP0)), dwColor );

	strcpy( szBuff, g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 100+gap1, y, szBuff  , dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nFame	, dwColor ); y += nNext;
	y += 20;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetPKValue()	, dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetPKPropensity()	, dwColor ); y += nNext;
	
	y = 13 + nyAdd2;
	nNext = 19;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP1), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP2), dwColor ); y += nNext;
	y += 20;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP3), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP4), dwColor ); y += nNext;
}


void CWndCharInfo::OnInitialUpdate()
{
	
	CWndBase::OnInitialUpdate();
	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "WndNewCharacter01.tga" ) ), TRUE );

	// �Ʒ����� �ɷ�ġ ���� 
	int nyAdd = 121;
	int posY = 49 + nyAdd;
	int posX = 128;

	m_editStrCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 100 );
	m_wndStrPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 101 );
	m_wndStrMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 102 ); posY += 15;

	m_editStaCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 103 );
	m_wndStaPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 104 );
	m_wndStaMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 105 ); posY += 15;

	m_editDexCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 106 );
	m_wndDexPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 107 );
	m_wndDexMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 108 ); posY += 15;

	m_editIntCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 109 );
	m_wndIntPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 110 );
	m_wndIntMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 111 );

	m_wndStrPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStrMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndStaPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStaMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndDexPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDexMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndIntPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndIntMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );

	posY += 36;
	m_wndApply.Create  ( "Apply", 0, CRect( posX - 108, posY, posX - 58, posY + 22 ), this, 112 );
	m_wndReset.Create  ( "Reset", 0, CRect( posX - 30, posY, posX + 20, posY + 22 ), this, 113 );

	if(::GetLanguage() == LANG_FRE)
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButStateOk.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButStateCancel.tga" ) ), TRUE );
	}
	else
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharApply.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharReset.tga" ) ), TRUE );
	}

	//���? ������ ������ 1�� �����̹Ƿ� �Ʒ� ����

	m_nGpPoint = g_pPlayer->GetRemainGP();
	//���� ������ ���� �����Ƿ� Minus Button Default�� False
	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if(g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}
	int nyAdd2 = 280;
	int y = 105 + nyAdd2;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
		m_wndChangeJob.Create( ">", 0, CRect( 130, y, 135+40, y + 13 ), this, 10  ); 

	//SetTexture(m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "WndTile00.tga")), TRUE);

	RefreshStatPoint();

	//FitTextureSize();

	MakeVertexBuffer();
}


BOOL CWndCharInfo::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = m_pWndRoot->GetWindowRect();
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);

}

BOOL CWndCharInfo::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	int editnum = 0;
	
	if( nID == 100 || nID == 103 || nID == 106 || nID == 109 )
	{
		CString tempnum = m_editStrCount.GetString();
		for( int i = 0 ; i < tempnum.GetLength() ; i++ )
		{
			if( isdigit2( tempnum.GetAt(i) ) == FALSE )
				return FALSE;
		}
	}
	switch(nID) 
	{
		case 100:
			editnum = atoi( m_editStrCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
			m_nStrCount = editnum;
			break;
		case 103:
			editnum = atoi( m_editStaCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
			m_nStaCount = editnum;
			break;
		case 106:
			editnum = atoi( m_editDexCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
			m_nDexCount = editnum;
			break;
		case 109:
			editnum = atoi( m_editIntCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
			m_nIntCount = editnum;
			break;
	}

	if( message == WNM_CLICKED )		
	{
		switch(nID) 
		{
			case 101: //Str Plus
				m_nStrCount++;
				break;
			case 102: //Str Minus
				m_nStrCount--;
				break;
			case 104: //Sta Plus
				m_nStaCount++;
				break;
			case 105: //Sta Minus
				m_nStaCount--;
				break;
			case 107: //Dex Plus
				m_nDexCount++;
				break;
			case 108: //Dex Minus
				m_nDexCount--;
				break;
			case 110: //Int Plus
				m_nIntCount++;
				break;
			case 111: //Int Minus
				m_nIntCount--;
				break;
			case 112: //Apply
				if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
				{
					SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
					g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
					g_WndMng.m_pWndStateConfirm->Initialize();
				}
				break;
			case 113: //Reset
				m_nStrCount = 0;
				m_nStaCount = 0;
				m_nDexCount = 0;
				m_nIntCount = 0;
				RefreshStatPoint();
				break;

			case JOB_MERCENARY: 
			case JOB_ACROBAT: 
			case JOB_ASSIST: 
			case JOB_MAGICIAN: 
			case JOB_PUPPETEER:
				{
					if( nID != g_pPlayer->GetJob() )
					{
						//"�ڽ��� ������ �ø��� �ֽ��ϴ�"
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0037) ) );
					}
					if( m_fWaitingConfirm == FALSE ) 
					{
						g_DPlay.SendIncJobLevel( nID );
						m_fWaitingConfirm = TRUE;
					}
					break;
				}
			case 10: // ���� 
				// �������? ������ 15�̻��ΰ��� ã��
				if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL )
				{
					SAFE_DELETE(m_pWndChangeJob);
					m_pWndChangeJob = new CWndChangeJob( g_pPlayer->GetJob() ); 
					m_pWndChangeJob->Initialize( this, 1106 );
				}
				break;
		}
	}

	if(nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if(g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if(g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}

	return CWndBase::OnChildNotify( message, nID, pLResult );
}


BOOL CWndCharInfo::Process()
{
	//Control Button Plus or Minus
	if(m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if(m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if(m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if(m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if(m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);
		
		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
		
		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if(m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}


void CWndCharInfo::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}


void CWndCharInfo::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;
	
	*pnMin = 0;
	*pnMax = 0;
	
	if( g_pPlayer )
	{
		ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp( nParts );
		if( pItemProp == NULL )
			return;
		
		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;
		
		*pnMin = g_pPlayer->GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = g_pPlayer->GetParam( DST_ABILITY_MAX, *pnMax );
		
		int nATK = 0;
		switch( pItemProp->dwWeaponType )
		{
			case WT_MELEE_SWD:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_AXE:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MELEE_STAFF:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_STICK:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)) );
				break;
			case WT_MELEE_KNUCKLE:
				nATK = (int)( float( (g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MAGIC_WAND:
				nATK = (int)( ( g_pPlayer->GetInt() + m_nIntCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f );
				break;
			case WT_MELEE_YOYO:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_RANGE_BOW:
				nATK = (int)( (((g_pPlayer->GetDex() + m_nDexCount -14)*4.0f + (g_pPlayer->GetLevel()*1.3f) + ((g_pPlayer->GetStr()+m_nStrCount)*0.2f)) * 0.7f) );
				break;
		}
		
		nATK += g_pPlayer->GetPlusWeaponATK( pItemProp->dwWeaponType );	// ������ �߰� ���ݷ¸� ���Ѵ�.

		int nPlus = nATK + g_pPlayer->GetParam( DST_CHR_DMG, 0 );

		*pnMin += nPlus;
		*pnMax += nPlus;
		
		CItemElem *pWeapon = g_pPlayer->GetWeaponItem( nParts );
		if( pWeapon && pWeapon->GetProp() )
		{
			float f = g_pPlayer->GetItemMultiplier( pWeapon );
			*pnMin = (int)( *pnMin * f );
			*pnMax = (int)( *pnMax * f );
		}
		
		if( pWeapon )
		{
			int nOption = pWeapon->GetAbilityOption();
			if( nOption > 0 )
			{
				int nValue = (int)pow( (float)( nOption ), 1.5f );

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}


int CWndCharInfo::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info;
	memset( &info, 0x00, sizeof(info) );
	info.dwAtkFlags = AF_GENERIC;
	
	float fFactor = 1.0f;
	if( g_pPlayer )
	{
		JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
		assert( pProperty );
		fFactor = pProperty->fFactorDef;
	}
	nDefense	= (int)( ((((g_pPlayer->GetLevel()*2) + ((g_pPlayer->GetSta()+m_nStaCount)/2)) / 2.8f ) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor) );
	nDefense	= nDefense + (g_pPlayer->GetDefenseByItem( FALSE ) / 4);
	nDefense	= nDefense + ( g_pPlayer->GetParam( DST_ADJDEF, 0 ) );

	nDefense	= (int)( nDefense * g_pPlayer->GetDEFMultiplier( &info ) );

#ifdef __JEFF_11
	if( nDefense < 0 )
		nDefense	= 0;
#endif	// __JEFF_11
	return nDefense;
}


int CWndCharInfo::GetVirtualCritical()
{
	int nCritical;
	nCritical	= ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical	= (int)( nCritical * g_pPlayer->GetJobPropFactor( JOB_PROP_CRITICAL ) );
	nCritical	= g_pPlayer->GetParam( DST_CHR_CHANCECRITICAL, nCritical );	// ũ��Ƽ�� Ȯ���� �����ִ� ��ų���� 
#ifdef __JEFF_11
	if( nCritical < 0 )
		nCritical	= 0;
#endif	// __JEFF_11
	
	if( g_pPlayer )
	{
		if( g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL) )	// ��Ƽ && 1ȸ ũ��Ƽ�� �ߵ�?
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				nCritical += (g_Party.m_nSizeofMember / 2);	
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}

	}
	return nCritical;
}


float CWndCharInfo::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;
	
	ItemProp* pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if( pWeaponProp )
		fItem = pWeaponProp->fAttackSpeed;
	
	JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
	ASSERT( pProperty );
	
	// A = int( ĳ������ ���� + ( ������ ���� * ( 4 * ���� + ( ���� / 8 ) ) ) - 3 )
	// ���ݼӵ� = ( ( 50 / 200 - A ) / 2 ) + ����ġ 
	int A = int( pProperty->fAttackSpeed + ( fItem * ( 4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f ) ) - 3.0f );

	if( 187.5f <= A )
		A	= (int)( 187.5f );

	const int MAX_ATTACK_SPEED_PLUSVALUE = 18;
	
	int nIndex = A / 10;
	nIndex = max( nIndex, 0 );
	nIndex = min( nIndex, (MAX_ATTACK_SPEED_PLUSVALUE-1) );
	
	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};
	
	fSpeed = ( ( 50.0f / (200.f - A) ) / 2.0f ) + fPlusValue[nIndex];
	
	float fDstParam = g_pPlayer->GetParam( DST_ATTACKSPEED, 0 ) / 1000.0f;
	fSpeed += fDstParam;
	
	if( 0 < g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) )
		fSpeed = fSpeed + ( fSpeed * g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) / 100 );
	
	if( fSpeed < 0.0f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
	
	return fSpeed;
}


void CWndCharInfo::RenderATK( C2DRender* p2DRender, int x, int y )
{
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szText[32];
	int nMin, nMax;
	int nTemp1, nTemp2;

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax( &nMin, &nMax );

	if(	(m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax) ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax( &nMin, &nMax );
	
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	nATK += g_pPlayer->GetParam( DST_ATKPOWER, 0 );

#ifdef __JEFF_11
	if( nATK < 0 )
		nATK	= 0;
#endif	// __JEFF_11

	wsprintf( szText, "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor );
}

void CWndHonor::OnDraw( C2DRender* p2DRender )
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	LPWNDCTRL	pCustom = NULL;
	DWORD		dwNormal;
	int			nIndex = 0;
	pCustom = GetWndCtrl( WIDC_LISTBOX1 );
	dwNormal = D3DCOLOR_ARGB( 255, 0, 0, 0 );

	if(m_vecTitle.empty() != false)
		p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*16,prj.GetText(TID_GAME_NO_TITLE), dwNormal);
}


void CWndHonor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	// Ÿ��Ʋ�� �˻��ؼ� ����Ʈ�� �߰��ϰ�, ������ ���ٴ� �޽��� ���?
	RefreshList();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);
}

void CWndHonor::RefreshList()
{
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->ResetContent();
	pWndButton1->EnableWindow(FALSE);

	m_vecTitle.clear();
	m_vecTitle	= CTitleManager::Instance()->m_vecEarned;

	if(g_pPlayer)
		m_nSelectedId = g_pPlayer->m_nHonor;

	if(m_vecTitle.size() > 0)
	{
		pWndListBox->AddString(prj.GetText(TID_GAME_NOT_SELECTED_TITLE));
		for(auto iter = m_vecTitle.begin(); iter != m_vecTitle.end(); ++iter)
		{
			pWndListBox->AddString(iter->strTitle.GetBuffer(0));
		}
	}
}


BOOL CWndHonor::Initialize( CWndBase* pWndParent,DWORD dwWndId )
{

	return CWndNeuz::InitDialog( APP_HONOR, pWndParent, 0, CPoint( 0, 0 ) );
}


BOOL CWndHonor::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	switch(nID)
	{
	case WIDC_LISTBOX1: // view ctrl
		{
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			if(pWndListBox->GetCurSel() == 0)
			{
				m_nSelectedId = -1;
				pWndButton1->EnableWindow(TRUE);
			}
			else
			{
				m_nSelectedId = m_vecTitle[pWndListBox->GetCurSel() - 1].nId;
				pWndButton1->EnableWindow(TRUE);
			}
			break;
		}
	case WIDC_BUTTON1:// ���õ� Ÿ��Ʋ�� ����ϰ�? �Ѵ�
		if(g_pPlayer)
		{
			pWndButton1->EnableWindow(FALSE);
			g_DPlay.SendReqHonorTitleChange(m_nSelectedId);
			break;
		}
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ����  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndCharacter::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		int nCurSel;
		ar >> nCurSel;
		if(nCurSel > 1) nCurSel = 0;
		lpTabCtrl->SetCurSel( nCurSel );
	}
	else
	{
		ar << lpTabCtrl->GetCurSel();
	}
}
void CWndCharacter::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//CRect rectRoot = m_pWndRoot->GetLayoutRect();
	//CPoint point( rectRoot.left, rectRoot.top + 96 + 16 );
	//Move( point );
	
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	//m_wndTabCtrl.Create( WBS_NOFRAME, rect, this, 12 );
	m_wndCharInfo.Create(WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 100000);
	m_wndHonor.Create(WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, APP_HONOR);
	m_wndHonor.AddWndStyle( WBS_NOFRAME );
	m_wndHonor.AddWndStyle( WBS_NODRAWFRAME );
	m_wndCharInfo.AddWndStyle( WBS_NOFRAME );
	//m_wndCharInfo.AddWndStyle( WBS_NODRAWFRAME );
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_CHAR_INFO);//"�⺻";
	tabTabItem.pWndBase = &m_wndCharInfo;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_HONOR);//"����";
	tabTabItem.pWndBase = &m_wndHonor;
	lpTapCtrl->InsertItem( 1, &tabTabItem );
	lpTapCtrl->SetCurSel( 0 );

	//lpTapCtrl->SetButtonLength( 60 );

}
BOOL CWndCharacter::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( dwWndId, pWndParent, 0, CPoint(0, 0) );
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void CWndStateConfirm::OnSetState( UINT nId )
{
	m_nId = nId;
}
void CWndStateConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CString strMessage = prj.GetText(TID_GAME_CHARSTATUS_APPLY_Q);

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	// 1�� �������� Ȯ��
	if( g_pPlayer->IsBaseJob() == TRUE )
	{
		// �����?
		CString strAddMessage;
		strAddMessage.Format( prj.GetText( TID_DIAG_0082 ) );
		strMessage += '\n';
		strMessage += strAddMessage;
	}
	pWndText->SetString( strMessage );
	pWndText->EnableWindow( FALSE );

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndStateConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_STATE_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
  ���� ������ ���� ���? 
BOOL CWndStateConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/

BOOL CWndStateConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if( nID == WIDC_YES )
	{
		CWndCharacter* pWndBase = (CWndCharacter*)g_WndMng.GetWndBase( APP_CHARACTER3 );
		if( pWndBase )
		{
			CWndCharInfo* pInfo = &pWndBase->m_wndCharInfo;
			if( pInfo )
			{
				g_DPlay.SendModifyStatus(pInfo->m_nStrCount, pInfo->m_nStaCount, pInfo->m_nDexCount, pInfo->m_nIntCount);
				pInfo->m_nStrCount = pInfo->m_nStaCount = pInfo->m_nDexCount = pInfo->m_nIntCount = 0;
				pInfo->RefreshStatPoint();
				Destroy();
			}
		}
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CHARACTER3 );
		if( pWndBase ) 
		{
			( (CWndCharacter*)pWndBase )->m_wndCharInfo.m_fWaitingConfirm	= FALSE;
		}
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndStateConfirm::SendYes( void )
{
	g_DPlay.SendIncStatLevel( (CHAR)m_nId );
	Destroy();
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void CWndCharacterBase::OnDraw(C2DRender* p2DRender)
{
	CRect rect = GetClientRect();
	rect.bottom -= 30;
//	p2DRender->RenderFillRect( rect, 0xff00ffff );
	int y = 0, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	BYTE checkhero = g_pPlayer->GetLegendChar();
	int xpos = 0;
	int ypos = 0;
	CTexture* pTexture;
	CString strPath;
	CPoint point;

//	TCHAR szString[ 32 ];
//	p2DRender->TextOut( 5, y, _T( "Name"  ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Style" ) ); y += nNext;
//	p2DRender->TextOut( 5, y, _T( "Job"   ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Guild" ) ); y += nNext;
//	p2DRender->TextOut( 5, y, _T( "Level" ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp1." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp2." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Gold"  ) ); y += nNext;

	y = 10;

	if( ::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}
			p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;

		y = 81;
	}
	else
	{
		p2DRender->TextOut( 50, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 50, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}
			p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
		y = 81;
	}

	point.x = xpos;
	point.y = ypos - 2;
	if(checkhero == LEGEND_CLASS_MASTER) //������ ���� ���?.
	{
		if(/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70) //Level Down�� ���? �����ؼ� �ּ�ó��
			strPath = MakePath( DIR_ICON, "Icon_MasterMark1.dds");
		else if(g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark2.dds");
		else if(g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark3.dds");
		else if(g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark4.dds");
		else if(g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark5.dds");
		else if(g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );			
		
	}
	else if(checkhero == LEGEND_CLASS_HERO) //������ ���?.
	{
		strPath = MakePath( DIR_ICON, "Icon_Hero.dds");
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );
	}
	//���� ����
	y = 55;
	CString strServerName;
	strServerName.Format( "%s", g_dpCertified.GetServerName(g_Option.m_nSer) );
	if( ::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, strServerName, dwColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( 50, y, strServerName, dwColor );
		y += nNext;
	}

	//ä�� ����
	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for( int j = 0; j < (int)( g_dpCertified.m_dwSizeofServerset ); j++ )
	{
		if(g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if(nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if(g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if(pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format( "%s",  g_dpCertified.m_aServerset[j+g_Option.m_nMSer].lpName );
				if( ::GetLanguage() == LANG_JAP )
					p2DRender->TextOut( 80, y, strServerName, dwColor );
				else
					p2DRender->TextOut( 50, y, strServerName, dwColor );

				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}

	y = 96;
	
	p2DRender->TextOut( 96, y, g_pPlayer->GetFlightLv(), dwColor ); y += nNext;
	{
		char szBuff[64];
		int nMaxFxp = prj.m_aFxpCharacter[ g_pPlayer->GetFlightLv() + 1 ].dwFxp;
		sprintf( szBuff, "%5.2f%%", (float)g_pPlayer->GetFxp() * 100.0f / (float)nMaxFxp );
		p2DRender->TextOut( 90, y, szBuff, dwColor ); y += nNext;
	}
	
	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_01), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_APP_CHARACTER_JOB), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_02), dwColor ); y += nNext;
	//p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_03), dwColor ); y += nNext;
	//�������� -> ����/ä�� ����
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVER), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), dwColor ); y += nNext;
	
	y += 10;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_04), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_05), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_06), dwColor );	
}
void CWndCharacterBase::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "WndCharacter2_1.tga" ) ), TRUE );
	FitTextureSize();
	
	MakeVertexBuffer();
}

BOOL CWndCharacterBase::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect( 240, 0, 240 + 330, 255 - 135 );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CWndCharacterDetail::CWndCharacterDetail()
{
	if( !g_WndMng.m_pWndStateConfirm )
		m_fWaitingConfirm	= FALSE;

	m_nCnt = 0;
}

void CWndCharacterDetail::RenderATK( C2DRender* p2DRender, int x, int y )
{
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szText[32];
	int nMin, nMax;
	g_pPlayer->GetHitMinMax( &nMin, &nMax );
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );

//#ifdef __PET_0410
	nATK	+= g_pPlayer->GetParam( DST_ATKPOWER, 0 );
//#endif	// __PET_0410

#ifdef __JEFF_11
	if( nATK < 0 )
		nATK	= 0;
#endif	// __JEFF_11

	wsprintf( szText, "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor ); 
}

void CWndCharacterDetail::OnDraw(C2DRender* p2DRender)
{
	int y = 5, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);

	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;
	RenderATK( p2DRender, x, y );		// ���ݷ�
	y += nNext;

	p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext; //����
	x = 100; y = 5;

	x = 130; y = 10;
	CString strMsg;
	strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;		// ũ��Ƽ��
	
	float fAttackSpeed = g_pPlayer->GetAttackSpeed();
	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

#ifdef _KWCSC_UPDATE
	x = 50;
	int fHitRate = g_pPlayer->GetAdjHitRate();
	strMsg.Format( "%d%%", fHitRate);
	p2DRender->TextOut( x , y, strMsg, dwColor );
#endif
	x =15; y = 40; nNextX = 60;
	// �Ʒ����� �ɷ�ġ ���� 
#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
	if( g_pPlayer->m_nStr == g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nStr < g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

	if( g_pPlayer->m_nSta == g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nSta < g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

	if( g_pPlayer->m_nDex == g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nDex < g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

	if( g_pPlayer->m_nInt == g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nInt < g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetInt(), dwColor ); y += nNext;

	if( m_fWaitingConfirm  == FALSE )
	{
		m_wndStr.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndSta.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndDex.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndInt.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	}
	else
	{
		m_wndStr.EnableWindow( FALSE );
		m_wndSta.EnableWindow( FALSE );
		m_wndDex.EnableWindow( FALSE );
		m_wndInt.EnableWindow( FALSE );
	}
#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;

	//
	// m_nCnt���� �۷ι��� ī���ʹ� g_nRenderCnt�� g_nProcessCnt�� ���� �˴ϴ�.
	// g_nProcessCnt�� �� ���μ���(1/60)���� �ѹ��� �ö󰡴� ī��Ʈ��
	// g_nRenderCnt�� �� ����ȭ�鸶�� �ѹ��� ī���� �Ǵ� ���Դϴ�.  -xuzhu-
	// 
	if( g_pPlayer->GetRemainGP() )
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
			m_nCnt = 0;
		}
	}

	p2DRender->TextOut( 105, y, g_pPlayer->GetRemainGP(), dwColor ); y += nNext;

	CRect rectHittest[5];
#ifdef _KWCSC_UPDATE
	rectHittest[0].SetRect( 10, 67, 160, 65 );
	rectHittest[1].SetRect( 10, 82, 160, 80 );
	rectHittest[2].SetRect( 10, 97, 160, 95 );
	rectHittest[3].SetRect( 10, 112, 160, 110 );
	rectHittest[4].SetRect( 10, 127, 160, 125 );
#else
	rectHittest[0].SetRect( 10, 52, 160, 65 );
	rectHittest[1].SetRect( 10, 67, 160, 80 );
	rectHittest[2].SetRect( 10, 82, 160, 95 );
	rectHittest[3].SetRect( 10, 97, 160, 110 );
	rectHittest[4].SetRect( 10, 112, 160, 125 );
#endif
	CRect rectTemp;
	CPoint ptTemp;
	// ���� ������ �ϱ�( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );
			DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );
			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else	// GP
			{
				szString.Format( "GP  %d\n ", g_pPlayer->GetRemainGP() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_GP0 ) );
			}
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;
#ifdef _KWCSC_UPDATE
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_HITRATE), dwColor ); y += nNext;
#endif
	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;	
}
void CWndCharacterDetail::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndCharacter2.tga" ) ), TRUE );
	FitTextureSize();

	int y = m_rectClient.Height() - 30;

	// �Ʒ����� �ɷ�ġ ���� 
#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
	m_wndStr.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 100 ); y += 14;
	m_wndSta.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 101 ); y += 14;
	m_wndDex.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 102 ); y += 14;
	m_wndInt.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 103 ); y += 14;

	m_wndStr.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndSta.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDex.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndInt.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );

	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
}
BOOL CWndCharacterDetail::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndCharacterDetail::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )		
	{
		if( m_fWaitingConfirm == FALSE ) {
			m_fWaitingConfirm	= TRUE;
			SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
			g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
			g_WndMng.m_pWndStateConfirm->OnSetState( nID );
			g_WndMng.m_pWndStateConfirm->Initialize();
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}

void CWndCharacterDetail::OnMouseWndSurface( CPoint point )
{
	// ���� ������ ��ġ�� Ŀ���� �������� ������ ������ ����(�����̸��� ��Ƽ���� ��ȣ)�� �����ش�.
/*	CRect rectHittest( 10, 52, 100, 64 );	
	if( rectHittest.PtInRect( point ) )
	{
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );
		CString szEditString;
		szEditString.Format( "%d, %d", point.x, point.y );
		g_toolTip.PutToolTip( 100, szEditString, rectHittest, point2 );
		g_WndMng.PutString( szEditString );
//		g_toolTip.PutToolTip( (DWORD)this, m_strTooltip, rectHittest, point2, 0 );
	}
*/}
	
//////////////////////////////////////////////////////////////////////////
// CWndCharacterDetail2 Class
//////////////////////////////////////////////////////////////////////////

void CWndCharacterDetail2::RenderATK( C2DRender* p2DRender, int x, int y )
{
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szText[32];
	int nMin, nMax;
	int nTemp1, nTemp2;

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax( &nMin, &nMax );

	if(	(m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax) ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax( &nMin, &nMax );
	
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	nATK += g_pPlayer->GetParam( DST_ATKPOWER, 0 );

#ifdef __JEFF_11
	if( nATK < 0 )
		nATK	= 0;
#endif	// __JEFF_11

	wsprintf( szText, "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor ); 
}

void CWndCharacterDetail2::OnDraw(C2DRender* p2DRender)
{
	if(g_pPlayer == NULL)
		return;

	int y = 5, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);

	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;
	// ���ݷ�
	RenderATK( p2DRender, x, y );
	y += nNext;

	//����
	if(	m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense( FALSE ) ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		p2DRender->TextOut( x , y, GetVirtualDEF(), dwColor ); y += nNext;
	}
	else
		p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext;

	x = 130; y = 10;

	//ũ��Ƽ��
	CString strMsg;
	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	if(	m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb() ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		strMsg.Format( "%d%%", GetVirtualCritical() );
	}
	else
		strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	//���ݼӵ�	
	float fAttackSpeed;
	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	if(	m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed() ) //���� ������ ����ǰ�? ���� �ɷ�ġ�� �ٸ� ���?
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		fAttackSpeed = GetVirtualATKSpeed();
	}
	else
		fAttackSpeed = g_pPlayer->GetAttackSpeed();

	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	x =15; y = 40; nNextX = 60;
	// �Ʒ����� �ɷ�ġ ���� 
	y = 52;
	int StatYPos = 50;

	if( g_pPlayer->m_nStr == g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nStr < g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

	if( g_pPlayer->m_nSta == g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nSta < g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

	if( g_pPlayer->m_nDex == g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nDex < g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

	if( g_pPlayer->m_nInt == g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nInt < g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetInt(), dwColor ); y += nNext;

	if(m_nGpPoint)
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
	}

	p2DRender->TextOut( 105, y, m_nGpPoint, dwColor ); y += nNext;

	CRect rectHittest[5];

	rectHittest[0].SetRect( 10, 52, 80, 65 );
	rectHittest[1].SetRect( 10, 67, 80, 80 );
	rectHittest[2].SetRect( 10, 82, 80, 95 );
	rectHittest[3].SetRect( 10, 97, 80, 110 );
	rectHittest[4].SetRect( 10, 112, 160, 125 );

	CRect rectTemp;
	CPoint ptTemp;
	// ���� ������ �ϱ�( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );
			DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );
			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else // GP
				strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_GPPOINT ) );
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;
	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;
	
	//�ɷ�ġ ���� Tooltip
	CRect rect;
	rect.SetRect( 7, 10, 160, 38 );
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS1 ) );
		else
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS2 ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
	//Edit Tooltip
	rect.SetRect(90, 52, 160, 110);
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_EDIT ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	//Button Tooltip
	rect = m_wndApply.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_APPLY ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	rect = m_wndReset.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_RESET ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
}

void CWndCharacterDetail2::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "WndCharacter2_2.tga" ) ), TRUE );
	FitTextureSize();

	// �Ʒ����� �ɷ�ġ ���� 
	int posY = 49;
	int posX = 128;

	m_editStrCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 100 );
	m_wndStrPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 101 );
	m_wndStrMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 102 ); posY += 15;

	m_editStaCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 103 );
	m_wndStaPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 104 );
	m_wndStaMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 105 ); posY += 15;

	m_editDexCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 106 );
	m_wndDexPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 107 );
	m_wndDexMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 108 ); posY += 15;

	m_editIntCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 109 );
	m_wndIntPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 110 );
	m_wndIntMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 111 );

	m_wndStrPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStrMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndStaPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStaMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndDexPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDexMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndIntPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndIntMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );

	posY += 36;
	m_wndApply.Create  ( "Apply", 0, CRect( posX - 108, posY, posX - 58, posY + 22 ), this, 112 );
	m_wndReset.Create  ( "Reset", 0, CRect( posX - 30, posY, posX + 20, posY + 22 ), this, 113 );

	if(::GetLanguage() == LANG_FRE)
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButStateOk.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButStateCancel.tga" ) ), TRUE );
	}
	else
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharApply.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharReset.tga" ) ), TRUE );
	}

	//���? ������ ������ 1�� �����̹Ƿ� �Ʒ� ����

	m_nGpPoint = g_pPlayer->GetRemainGP();
	//���� ������ ���� �����Ƿ� Minus Button Default�� False
	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if(g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}

	RefreshStatPoint();
}

BOOL CWndCharacterDetail2::Process()
{
	//Control Button Plus or Minus
	if(m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if(m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if(m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if(m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if(m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);
		
		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
		
		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if(m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}

void CWndCharacterDetail2::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}

void CWndCharacterDetail2::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;
	
	*pnMin = 0;
	*pnMax = 0;
	
	if( g_pPlayer )
	{
		ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp( nParts );
		if( pItemProp == NULL )
			return;
		
		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;
		
		*pnMin = g_pPlayer->GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = g_pPlayer->GetParam( DST_ABILITY_MAX, *pnMax );
		
		int nATK = 0;
		switch( pItemProp->dwWeaponType )
		{
			case WT_MELEE_SWD:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_AXE:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MELEE_STAFF:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_STICK:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)) );
				break;
			case WT_MELEE_KNUCKLE:
				nATK = (int)( float( (g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MAGIC_WAND:
				nATK = (int)( ( g_pPlayer->GetInt() + m_nIntCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f );
				break;
			case WT_MELEE_YOYO:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_RANGE_BOW:
				nATK = (int)( (((g_pPlayer->GetDex() + m_nDexCount -14)*4.0f + (g_pPlayer->GetLevel()*1.3f) + ((g_pPlayer->GetStr()+m_nStrCount)*0.2f)) * 0.7f) );
				break;
		}
		
		nATK += g_pPlayer->GetPlusWeaponATK( pItemProp->dwWeaponType );	// ������ �߰� ���ݷ¸� ���Ѵ�.

		int nPlus = nATK + g_pPlayer->GetParam( DST_CHR_DMG, 0 );

		*pnMin += nPlus;
		*pnMax += nPlus;
		
		CItemElem *pWeapon = g_pPlayer->GetWeaponItem( nParts );
		if( pWeapon && pWeapon->GetProp() )
		{
			float f = g_pPlayer->GetItemMultiplier( pWeapon );
			*pnMin = (int)( *pnMin * f );
			*pnMax = (int)( *pnMax * f );
		}
		
		if( pWeapon )
		{
			int nOption = pWeapon->GetAbilityOption();
			if( nOption > 0 )
			{
				int nValue = (int)pow( (float)( nOption ), 1.5f );

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}

int CWndCharacterDetail2::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info;
	memset( &info, 0x00, sizeof(info) );
	info.dwAtkFlags = AF_GENERIC;
	
	float fFactor = 1.0f;
	if( g_pPlayer )
	{
		JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
		assert( pProperty );
		fFactor = pProperty->fFactorDef;
	}
	nDefense	= (int)( ((((g_pPlayer->GetLevel()*2) + ((g_pPlayer->GetSta()+m_nStaCount)/2)) / 2.8f ) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor) );
	nDefense	+= (g_pPlayer->GetDefenseByItem( FALSE ) / 4);
	nDefense	+= g_pPlayer->GetParam( DST_ADJDEF, 0 );
	nDefense	= (int)( nDefense * g_pPlayer->GetDEFMultiplier( &info ) );

#ifdef __JEFF_11
	if( nDefense < 0 )
		nDefense	= 0;
#endif	// __JEFF_11
	return nDefense;
}

int CWndCharacterDetail2::GetVirtualCritical()
{
	int nCritical;
	nCritical	= ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical	= (int)( nCritical * g_pPlayer->GetJobPropFactor( JOB_PROP_CRITICAL ) );
	nCritical	= g_pPlayer->GetParam( DST_CHR_CHANCECRITICAL, nCritical );	// ũ��Ƽ�� Ȯ���� �����ִ� ��ų���� 
#ifdef __JEFF_11
	if( nCritical < 0 )
		nCritical	= 0;
#endif	// __JEFF_11
	
	if( g_pPlayer )
	{
		if( g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL) )	// ��Ƽ && 1ȸ ũ��Ƽ�� �ߵ�?
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				nCritical += (g_Party.m_nSizeofMember / 2);	
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}

	}
	return nCritical;
}

float CWndCharacterDetail2::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;
	
	ItemProp* pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if( pWeaponProp )
		fItem = pWeaponProp->fAttackSpeed;
	
	JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
	ASSERT( pProperty );
	
	// A = int( ĳ������ ���� + ( ������ ���� * ( 4 * ���� + ( ���� / 8 ) ) ) - 3 )
	// ���ݼӵ� = ( ( 50 / 200 - A ) / 2 ) + ����ġ 
	int A = int( pProperty->fAttackSpeed + ( fItem * ( 4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f ) ) - 3.0f );

	if( 187.5f <= A )
		A	= (int)( 187.5f );

	const int MAX_ATTACK_SPEED_PLUSVALUE = 18;
	
	int nIndex = A / 10;
	nIndex = max( nIndex, 0 );
	nIndex = min( nIndex, (MAX_ATTACK_SPEED_PLUSVALUE-1) );
	
	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};
	
	fSpeed = ( ( 50.0f / (200.f - A) ) / 2.0f ) + fPlusValue[nIndex];
	
	float fDstParam = g_pPlayer->GetParam( DST_ATTACKSPEED, 0 ) / 1000.0f;
	fSpeed += fDstParam;
	
	if( 0 < g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) )
		fSpeed = fSpeed + ( fSpeed * g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) / 100 );
	
	if( fSpeed < 0.0f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
	
	return fSpeed;
}

BOOL CWndCharacterDetail2::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}

BOOL CWndCharacterDetail2::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	int editnum = 0;
	
	if( nID == 100 || nID == 103 || nID == 106 || nID == 109 )
	{
		CString tempnum = m_editStrCount.GetString();
		for( int i = 0 ; i < tempnum.GetLength() ; i++ )
		{
			if( isdigit2( tempnum.GetAt(i) ) == FALSE )
				return FALSE;
		}
	}
	switch(nID) 
	{
		case 100:
			editnum = atoi( m_editStrCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
			m_nStrCount = editnum;
			break;
		case 103:
			editnum = atoi( m_editStaCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
			m_nStaCount = editnum;
			break;
		case 106:
			editnum = atoi( m_editDexCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
			m_nDexCount = editnum;
			break;
		case 109:
			editnum = atoi( m_editIntCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
			m_nIntCount = editnum;
			break;
	}

	if( message == WNM_CLICKED )		
	{
		switch(nID) 
		{
			case 101: //Str Plus
				m_nStrCount++;
				break;
			case 102: //Str Minus
				m_nStrCount--;
				break;
			case 104: //Sta Plus
				m_nStaCount++;
				break;
			case 105: //Sta Minus
				m_nStaCount--;
				break;
			case 107: //Dex Plus
				m_nDexCount++;
				break;
			case 108: //Dex Minus
				m_nDexCount--;
				break;
			case 110: //Int Plus
				m_nIntCount++;
				break;
			case 111: //Int Minus
				m_nIntCount--;
				break;
			case 112: //Apply
				if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
				{
					SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
					g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
					g_WndMng.m_pWndStateConfirm->Initialize();
				}
				break;
			case 113: //Reset
				m_nStrCount = 0;
				m_nStaCount = 0;
				m_nDexCount = 0;
				m_nIntCount = 0;
				RefreshStatPoint();
				break;
		}
	}

	if(nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if(g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if(g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndChangeJob::CWndChangeJob( int nJob ) 
{ 
	m_nJob = nJob;
} 

void CWndChangeJob::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	for( int i  = 0 ; i < 6 ; ++i )
	{
		m_wndExpert[ i ].SetFontColor( 0xffffffff );
	}
	m_wndOk.SetFontColor( 0xffffffff );
	m_wndCancel.SetFontColor( 0xffffffff );

	if( m_nJob == 0 )	{
		m_wndExpert[ 0 ].Create( prj.m_aJob[ MAX_JOBBASE + 0 ].szName, 0, CRect( 10, 10, 230, 35 ), this, 10 );
		m_wndExpert[ 1 ].Create( prj.m_aJob[ MAX_JOBBASE + 1 ].szName, 0, CRect( 10, 40, 230, 65 ), this, 11 );
		m_wndExpert[ 2 ].Create( prj.m_aJob[ MAX_JOBBASE + 2 ].szName, 0, CRect( 10, 70, 230, 95 ), this, 12 );
		m_wndExpert[ 3 ].Create( prj.m_aJob[ MAX_JOBBASE + 3 ].szName, 0, CRect( 10, 100, 230, 125 ), this, 13 );
		m_wndExpert[ 4 ].Create( prj.m_aJob[ MAX_JOBBASE + 4 ].szName, 0, CRect( 10, 130, 230, 155 ), this, 14 );
	}
	else	{
		m_wndExpert[ 0 ].Create( prj.m_aJob[ m_nJob * 2 + 4 ].szName, 0, CRect( 10, 10, 230, 35  ), this, 10 ); 
		m_wndExpert[ 1 ].Create( prj.m_aJob[ m_nJob * 2 + 5 ].szName, 0, CRect( 10, 40, 230, 65  ), this, 11 ); 
	}
	m_wndOk.Create( _T( "OK" )  , 0, CRect( 60, 180, 110, 200), this, IDOK ); 
	m_wndCancel.Create( _T( "Cancel" ) , 0, CRect( 130, 180, 180, 200), this, IDCANCEL ); 

} 
BOOL CWndChangeJob::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	//SetTitle( _T( "����" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, dwWndId ); 
} 

BOOL CWndChangeJob::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WNM_CLICKED )
	{
		if( 10 <= nID && nID <= 14 )
		{
			if( g_pPlayer->IsBaseJob() )
			{
				if( 10 == nID || 11 == nID || 12 == nID || 13 == nID )
				{
					g_DPlay.SendChangeJob( nID - 9 );
				}
				else
				{
					g_WndMng.PutString( "Not Change Job" );
				}
			}
			else
			if( g_pPlayer->IsExpert() )
			{
				if( nID == 10 )
					g_DPlay.SendChangeJob( m_nJob * 2 + 4 );
				else
				if( nID == 11 )
					g_DPlay.SendChangeJob( m_nJob * 2 + 5 );
			}
			Destroy();
		}
		else
		{
			Destroy();
		}
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

/*
MERCENARY    
KNIGHT       
SPEARMAN     
TUMBLER      
PIERROT      
BEASTMASTER  
HEALER       
FORTUNETELLER
RINGMASTER   
JACKMETA     
JACKFLAM     
JACKEMITER   
PUPPETEER    
STRONGMAN    
STRINGMASTER 
*/



CWndSkillTreeEx::CWndSkillTreeEx()
{
	m_nCount     = 0;
//	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_bDrag = FALSE;
	m_apSkill = NULL;
	m_nJob = -1;
	m_pVBGauge = NULL;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		m_atexSkill[ i ] = NULL;

	m_aSkillLevel[ 0 ] = NULL;
	m_aSkillLevel[ 1 ] = NULL;
	m_aSkillLevel[ 2 ] = NULL;

	m_atexJobPannel[ 0 ] = NULL;
	m_atexJobPannel[ 1 ] = NULL;

	m_atexTopDown[0] = NULL;
	m_atexTopDown[1] = NULL;
	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
	m_bSlot[3] = TRUE;
	m_bLegend = FALSE;

	m_nCurrSkillPoint = 0;
}
CWndSkillTreeEx::~CWndSkillTreeEx()
{
	DeleteDeviceObjects();
}

void CWndSkillTreeEx::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		ar >> m_bSlot[0] >> m_bSlot[1] >> m_bSlot[2] >> m_bSlot[3];
		BOOL bFlag[4];

		bFlag[0] = m_bSlot[0];
		bFlag[1] = m_bSlot[1];
		bFlag[2] = m_bSlot[2];
		bFlag[3] = m_bSlot[3];
		
		m_bSlot[0] = m_bSlot[1] = m_bSlot[2] = m_bSlot[3] = TRUE;
		
		SetActiveSlot( 0, bFlag[0] );
		SetActiveSlot( 1, bFlag[1] );
		SetActiveSlot( 2, bFlag[2] );
		if(m_bLegend)
		{
			if(FULLSCREEN_HEIGHT == 600)
			{
				if(bFlag[3])
					SetActiveSlot( 2, FALSE );
			}
			else
				SetActiveSlot( 3, bFlag[3] );
		}
	}
	else
	{
		dwVersion = 0;
		ar << m_bSlot[0] << m_bSlot[1] << m_bSlot[2] << m_bSlot[3];
	}
}

void CWndSkillTreeEx::SetActiveSlot(int nSlot, BOOL bFlag)
{
	CRect rect = GetWindowRect(TRUE);

	if( nSlot == 0 )
	{
		if( m_bSlot[0] == bFlag )
			return;

		CWndStatic* lpWndStatic;
		lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );

		DWORD dwValue = 48;

		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect( 0, dwValue );
			lpWndStatic->SetWndRect( crect, TRUE );

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC4 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC8 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );	
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );	
			}
			int n = rect.Height();
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect( 0, ((-1) * (int)( dwValue )) );
			lpWndStatic->SetWndRect( crect, TRUE );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC4 );
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC8 );
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}

			int n = rect.Height();
			rect.bottom -= dwValue;

		}

		SetWndRect(rect);

		m_bSlot[0] = bFlag;
	}
	else
	if( nSlot == 1 )
	{
		if( m_bSlot[1] == bFlag )
			return;

		DWORD dwValue = 144;
		
		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );

			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );
			}

			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			lpWndStatic->SetVisible(FALSE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}
			
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[1] = bFlag;
	}
	else
	if( nSlot == 2 )
	{
		if( m_bSlot[2] == bFlag )
			return;

		DWORD dwValue = 144;
		
		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			lpWndStatic->SetVisible(TRUE);

			if(m_bLegend)
			{
				CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );
			}
			
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			lpWndStatic->SetVisible(FALSE);

			if(m_bLegend)
			{
				CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}
			
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[2] = bFlag;
	}
	else if(nSlot == 3)
	{
		if( m_bSlot[3] == bFlag )
			return;
		
		DWORD dwValue = 48;
		
		if( bFlag )
		{
			m_pWndHeroStatic[1]->SetVisible(TRUE);
			rect.bottom += dwValue;
		}
		else
		{
			m_pWndHeroStatic[1]->SetVisible(FALSE);
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[3] = bFlag;
	}

	if( rect.bottom > (int)( FULLSCREEN_HEIGHT-48 ) )
	{
		rect.OffsetRect( 0, (int)(FULLSCREEN_HEIGHT-48)-rect.bottom );
		SetWndRect(rect);
	}
		
	AdjustWndBase();
}


BOOL CWndSkillTreeEx::IsDownPoint(DWORD skill)
{
	LPSKILL pSkill = GetdwSkill( skill );
	if( pSkill == NULL )
		return FALSE;

	LPSKILL pSkillMy = g_pPlayer->GetSkill( pSkill->dwSkill );
	if( pSkillMy == NULL )
		return FALSE;

	if(  pSkill->dwLevel <= pSkillMy->dwLevel )
		return FALSE;

	return TRUE;
}

void CWndSkillTreeEx::SubSkillPointDown( LPSKILL lpSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == NULL_ID )
			continue;
		
		LPSKILL pSkill2 = &m_apSkill[i];
		
		if( pSkill2 == NULL )
			continue;
		
		if( pSkill2->GetProp()->dwReSkill1 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel1 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel1 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
		
		if( pSkill2->GetProp()->dwReSkill2 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel2 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel2 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
	}
}

LPSKILL CWndSkillTreeEx::GetdwSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == dwSkill )
			return &m_apSkill[i];
	}
	return NULL;
}

LPSKILL CWndSkillTreeEx::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];
	
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	if( pSkill->GetProp() == NULL )
	{
		Error( "GetSkill Error Not Prop : %d", i );
		return NULL;
	}
	
	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	return pSkill;
}

BOOL CWndSkillTreeEx::CheckSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];

	if( pSkill->dwSkill == NULL_ID || g_pPlayer == NULL ) 
		return FALSE;
	
	DWORD dwSkill = pSkill->dwSkill;
	
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL || pSkillProp->nLog == 1 )
		return FALSE;

	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() )
		return FALSE;
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill1 );

		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				return FALSE;
			}
		}
		else
		{
		}
	}
	
	if( pSkillProp->dwReSkill2 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				return FALSE;
			}				
		}
		else
		{
		}
	}	
	
	return TRUE;
}

HRESULT CWndSkillTreeEx::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndSkillTreeEx::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndSkillTreeEx::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );

	SAFE_DELETE( m_atexJobPannel[0] );
	SAFE_DELETE( m_atexJobPannel[1] );
	
	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		safe_delete_array( m_apSkill );
	}
	else
		m_apSkill = NULL;
	
	return S_OK;
}

void CWndSkillTreeEx::InitItem(int nJob, LPSKILL apSkill, BOOL bReset )
{
	if( g_pPlayer == NULL )
		return;

	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
		m_apSkill = new SKILL[MAX_SKILL_JOB];
		memcpy( m_apSkill, apSkill, sizeof(SKILL)*MAX_SKILL_JOB );
	}
	else
	{
		m_nCurrSkillPoint = 0;
		m_apSkill = apSkill;
	}

	m_pFocusItem = NULL;
	m_nJob = nJob;

	if( m_apSkill == NULL )
		return;


	m_nCount = 0;
	CString strTex[2];
	
	CWndStatic* lpWndStatic1 = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );
	CWndStatic* lpWndStatic2 = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
	CWndStatic* lpWndStatic3 = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );

	switch( m_nJob )
	{
	case JOB_VAGRANT:
		m_nCount  = 3;
		strTex[0] = "";//"Back_Upper.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( "" );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_KNIGHT:
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT_HERO:
		m_nCount  = 26;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "Back_Night.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		break;			
	case JOB_BLADE:
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO:
		m_nCount  = 26;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "Back_Blade.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_BLADE].szName );
		break;			
	case JOB_MERCENARY:
		m_nCount  = 18;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		m_nCount  = 28;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "Back_Bill.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		break;			
	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		m_nCount  = 28;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "Back_Ring.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_RINGMASTER].szName );
		break;			
	case JOB_ASSIST:
		m_nCount  = 20;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		m_nCount  = 39;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "Back_Ele.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		break;			
	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		m_nCount  = 28;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "Back_Psy.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_PSYCHIKEEPER].szName );
		break;			
	case JOB_MAGICIAN:
		m_nCount  = 20;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( "" );
		break;	
	case JOB_ACROBAT:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( "" );
		break;	
	case JOB_JESTER:
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Jst.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		break;	
	case JOB_RANGER:
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Rag.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		break;
	default:
		Error( "CWndSkillTreeEx::InitItem �˼����� ���� : %d", m_nJob );
		return;
	}
	
	//Master Skill�� ���ۺ��� 1Lv�̹Ƿ� ���? �̹��� ����.
	switch( m_nJob )
	{
		case JOB_KNIGHT_HERO:
			m_strHeroSkilBg = "Back_Hero_KntDrawing.tga";
			break;
		case JOB_BLADE_HERO:
			m_strHeroSkilBg = "Back_Hero_BldDefence.tga";
			break;
		case JOB_BILLPOSTER_HERO:
			m_strHeroSkilBg = "Back_Hero_BilDisEnchant.tga";
			break;
		case JOB_RINGMASTER_HERO:
			m_strHeroSkilBg = "Back_Hero_RigReturn.tga";
			break;
		case JOB_ELEMENTOR_HERO:
			m_strHeroSkilBg = "Back_Hero_EleCursemind.tga";
			break;
		case JOB_PSYCHIKEEPER_HERO:
			m_strHeroSkilBg = "Back_Hero_PsyStone.tga";
			break;
		case JOB_JESTER_HERO:
			m_strHeroSkilBg = "Back_Hero_JstSilence.tga";
			break;
		case JOB_RANGER_HERO:
			m_strHeroSkilBg = "Back_Hero_RagHawkeye.tga";
			break;
	}

	SAFE_DELETE( m_atexJobPannel[0] );
	SAFE_DELETE( m_atexJobPannel[1] );

	if( !strTex[0].IsEmpty() )
	{
		m_atexJobPannel[0] = new IMAGE;
		if( LoadImage( MakePath( DIR_THEME, strTex[0] ), m_atexJobPannel[0] ) == FALSE )
			Error( "CWndSkillTreeEx::InitItem���� %s Open1 ����", strTex[0] );

	}

	if( !strTex[1].IsEmpty() )
	{
		m_atexJobPannel[1] = new IMAGE;
		if( LoadImage( MakePath( DIR_THEME, strTex[1] ), m_atexJobPannel[1] ) == FALSE )
			Error( "CWndSkillTreeEx::InitItem���� %s Open1 ����", strTex[1] );

	}
	AdjustWndBase();
	
	// �ҽ� �������� �Է�
	LoadTextureSkillicon();

	g_nSkillCurSelect = -1;

	m_dwMouseSkill = NULL_ID;

	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];
}
BOOL CWndSkillTreeEx::GetSkillPoint(DWORD dwSkillID, CRect& rect )
{
	/*
	LPWNDCTRL lpWndCtrl;

	CPoint ptUpperAdd, ptLowerAdd;

	ptUpperAdd.x = 0;
	ptUpperAdd.y = 0;

	ptLowerAdd.x = 0;
	ptLowerAdd.y = 139;
	
	switch(dwSkillID)
	{
		case SI_VAG_ONE_CLEANHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				rect = lpWndCtrl->rect;
			}
			break;
		case SI_VAG_ONE_BRANDISH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				rect = lpWndCtrl->rect;
			}
			break;
		case SI_VAG_ONE_OVERCUTTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				rect = lpWndCtrl->rect;
			}
			break;

//�Ӽ��ʸ�
		case SI_MER_ONE_SPLMASH:		rect = CRect( 34, 109, 58, 133 ); break;
		case SI_MER_SHIELD_PROTECTION:	rect = CRect( 34, 161, 58, 185 ); break;
		case SI_MER_ONE_KEENWHEEL:		rect = CRect( 84, 97, 108, 121 ); break;
		case SI_MER_ONE_BLOODYSTRIKE:	rect = CRect( 84, 123, 108, 147 ); break;
		case SI_MER_SHIELD_PANBARRIER:	rect = CRect( 84, 149, 108, 173 ); break;
		case SI_MER_SUP_IMPOWERWEAPON:	rect = CRect( 84, 175, 108, 199 ); break;
		case SI_MER_ONE_BLINDSIDE:		rect = CRect( 134, 97, 158, 121 ); break;
		case SI_MER_ONE_REFLEXHIT:		rect = CRect( 134, 123, 158, 147 ); break;
		case SI_MER_ONE_SNEAKER:		rect = CRect( 134, 149, 158, 173 ); break;
		case SI_MER_SUP_SMITEAXE:		rect = CRect( 134, 175, 158, 199 ); break;
		case SI_MER_SUP_BLAZINGSWORD:	rect = CRect( 134, 201, 158, 225 ); break;
		case SI_MER_ONE_SPECIALHIT:		rect = CRect( 184, 97, 208, 121 ); break;
		case SI_MER_ONE_GUILOTINE:		rect = CRect( 184, 123, 208, 147 ); break;
		case SI_MER_SUP_AXEMASTER:		rect = CRect( 184, 175, 208, 199 ); break;
		case SI_MER_SUP_SWORDMASTER:	rect = CRect( 184, 201, 208, 225 ); break;
// ����Ʈ
		case SI_KNT_TWOSW_CHARGE:		rect = CRect( 34, 260, 58, 284 ); break;
		case SI_KNT_TWOAX_PAINDEALER:	rect = CRect( 34, 286, 58, 310 ); break;
		case SI_KNT_SUP_GUARD:			rect = CRect( 34, 312, 58, 336 ); break;
		case SI_KNT_TWOSW_EARTHDIVIDER:	rect = CRect( 84, 260, 108, 284 ); break;
		case SI_KNT_TWOAX_POWERSTUMP:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_KNT_SUP_RAGE:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_KNT_TWO_POWERSWING:		rect = CRect( 134, 272, 158, 296 ); break;
		case SI_KNT_SUP_PAINREFLECTION:	rect = CRect( 134, 312, 158, 336 ); break;
// �����̵�
		case SI_BLD_DOUBLESW_SILENTSTRIKE:	rect = CRect( 34, 260, 58, 284 ); break;
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	rect = CRect( 34, 286, 58, 310 ); break;
		case SI_BLD_DOUBLE_ARMORPENETRATE:	rect = CRect( 34, 312, 58, 336 ); break;
		case SI_BLD_DOUBLESW_BLADEDANCE:	rect = CRect( 84, 260, 108, 284 ); break;
		case SI_BLD_DOUBLEAX_HAWKATTACK:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_BLD_SUP_BERSERK:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_BLD_DOUBLE_CROSSSTRIKE:		rect = CRect( 134, 272, 158, 296 ); break;
		case SI_BLD_DOUBLE_SONICBLADE:		rect = CRect( 134, 312, 158, 336 ); break;
		
			
// ��ý��?	
		case SI_ASS_HEAL_HEALING:			rect = CRect( 18, 136, 42, 160 ); break;
		case SI_ASS_HEAL_PATIENCE:			rect = CRect( 64, 110, 88, 134 ); break;
		case SI_ASS_CHEER_QUICKSTEP:		rect = CRect( 64, 136, 88, 160 ); break;
		case SI_ASS_CHEER_MENTALSIGN:		rect = CRect( 64, 162, 88, 186 ); break;
		case SI_ASS_KNU_TAMPINGHOLE:		rect = CRect( 64, 188, 88, 212 ); break;
		case SI_ASS_HEAL_RESURRECTION:		rect = CRect( 108, 110, 132, 134 ); break;
		case SI_ASS_CHEER_HASTE:			rect = CRect( 110, 136, 134, 160 ); break;
		case SI_ASS_CHEER_HEAPUP:			rect = CRect( 110, 162, 134, 186 ); break;
		case SI_ASS_CHEER_STONEHAND:		rect = CRect( 110, 188, 134, 212 ); break;		
		case SI_ASS_CHEER_CIRCLEHEALING:	rect = CRect( 156, 110, 180, 134 ); break;
		case SI_ASS_CHEER_CATSREFLEX:		rect = CRect( 156, 136, 180, 160 ); break;
		case SI_ASS_CHEER_BEEFUP:			rect = CRect( 156, 162, 180, 186 ); break;
		case SI_ASS_KNU_BURSTCRACK:			rect = CRect( 156, 188, 180, 212 ); break;		
		case SI_ASS_HEAL_PREVENTION:		rect = CRect( 202, 110, 226, 134 ); break;
		case SI_ASS_CHEER_CANNONBALL:		rect = CRect( 202, 136, 226, 160 ); break;
		case SI_ASS_CHEER_ACCURACY:			rect = CRect( 202, 162, 226, 186 ); break;
		case SI_ASS_KNU_POWERFIST:			rect = CRect( 202, 188, 226, 212 ); break;		
//��������
		case SI_RIN_SUP_PROTECT:            rect = CRect( 34, 260, 58, 284 ); break;
		case SI_RIN_SUP_HOLYCROSS:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_RIN_HEAL_GVURTIALLA:		rect = CRect( 34, 312, 58, 336 ); break;
		case SI_RIN_SUP_HOLYGUARD:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_RIN_SUP_SPIRITUREFORTUNE:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_RIN_HEAL_HEALRAIN:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_RIN_SQU_GEBURAHTIPHRETH:	rect = CRect( 134, 272, 158, 296 ); break;
		case SI_RIN_SUP_MERKABAHANZELRUSHA:	rect = CRect( 134, 312, 158, 336 ); break;
//��������
		case SI_BIL_KNU_BELIALSMESHING:		rect = CRect( 34, 260, 58, 284 ); break;
		case SI_BIL_PST_ASMODEUS:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_BIL_KNU_BLOODFIST:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_BIL_PST_BARAQIJALESNA:		rect = CRect( 84, 286, 108, 310 ); break;
		case SI_BIL_KNU_PIERCINGSERPENT:	rect = CRect( 134, 260, 158, 284 ); break;
		case SI_BIL_PST_BGVURTIALBOLD:		rect = CRect( 134, 286, 158, 310 ); break;
		case SI_BIL_KNU_SONICHAND:			rect = CRect( 184, 272, 208, 296 ); break;
		case SI_BIL_PST_ASALRAALAIKUM:		rect = CRect( 184, 312, 208, 336 ); break;
			
			
// ������
		case SI_MAG_MAG_MENTALSTRIKE:			rect = CRect( 34, 96, 58, 120 );	break;
		case SI_MAG_MAG_BLINKPOOL: 				rect = CRect( 34, 148, 58, 172 );	break;
		case SI_MAG_FIRE_BOOMERANG:				rect = CRect( 84, 96, 108, 120 );   break;
		case SI_MAG_WIND_SWORDWIND:				rect = CRect( 84, 122, 108, 146 );	break;
		case SI_MAG_WATER_ICEMISSILE:			rect = CRect( 84, 148, 108, 172 );	break;			
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:	rect = CRect( 84, 174, 108, 198 );	break;
		case SI_MAG_EARTH_SPIKESTONE:			rect = CRect( 84, 200, 108, 224 );	break;	
		case SI_MAG_FIRE_HOTAIR:				rect = CRect( 134, 96, 158, 120 );		break;		
		case SI_MAG_WIND_STRONGWIND:			rect = CRect( 134, 122, 158, 146 );		break;		
		case SI_MAG_WATER_WATERBALL:			rect = CRect( 134, 148, 158, 172 );		break;		
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:	rect = CRect( 134, 174, 158, 198 );		break;		
		case SI_MAG_EARTH_ROCKCRASH:			rect = CRect( 134, 200, 158, 224 );		break;		
		case SI_MAG_FIRE_FIRESTRIKE:			rect = CRect( 184, 96, 208, 120 );		break;		
		case SI_MAG_WIND_WINDCUTTER:			rect = CRect( 184, 122, 208, 146 );		break;		
		case SI_MAG_WATER_SPRINGWATER:			rect = CRect( 184, 148, 208, 172 );		break;		
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	rect = CRect( 184, 174, 208, 198 );		break;		
		case SI_MAG_EARTH_LOOTING:				rect = CRect( 184, 200, 208, 224 );		break;
// ��������
		case SI_ELE_FIRE_FIREBIRD:				rect = CRect( 34, 234, 58, 258 ); break;
		case SI_ELE_EARTH_STONESPEAR:			rect = CRect( 34, 260, 58, 284 ); break;
		case SI_ELE_WIND_VOID:					rect = CRect( 34, 286, 58, 310 ); break;
		case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	rect = CRect( 34, 312, 58, 336 ); break;
		case SI_ELE_WATER_ICESHARK:				rect = CRect( 34, 338, 58, 362 ); break;
		case SI_ELE_FIRE_BURINGFIELD:			rect = CRect( 84, 234, 108, 258 ); break;
		case SI_ELE_EARTH_EARTHQUAKE:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_ELE_WIND_WINDFIELD:				rect = CRect( 84, 286, 108, 310 ); break;
		case SI_ELE_ELECTRICITY_ELETRICSHOCK:	rect = CRect( 84, 312, 108, 336 ); break;
		case SI_ELE_WATER_POISONCLOUD:			rect = CRect( 84, 338, 108, 362 ); break;
		case SI_ELE_MULTY_METEOSHOWER:			rect = CRect( 134, 246, 158, 270 ); break;
		case SI_ELE_MULTY_SANDSTORM:			rect = CRect( 134, 272, 158, 296 ); break;
		case SI_ELE_MULTY_LIGHTINGSTORM:		rect = CRect( 134, 298, 158, 322 ); break;
		case SI_ELE_MULTY_AVALANCHE:			rect = CRect( 134, 338, 158, 362 ); break;
		case SI_ELE_FIRE_FIREMASTER:			rect = CRect( 184, 234, 208, 258 ); break;
		case SI_ELE_EARTH_EARTHMASTER:			rect = CRect( 184, 260, 208, 284 ); break;
		case SI_ELE_WIND_WINDMASTER:			rect = CRect( 184, 286, 208, 310 ); break;
		case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	rect = CRect( 184, 312, 208, 336 ); break;
		case SI_ELE_WATER_WATERMASTER:			rect = CRect( 184, 338, 208, 362 ); break;
// ����Ű��
		case SI_PSY_NLG_DEMONOLGY:				rect = CRect( 34, 260, 58, 284 ); break;
		case SI_PSY_PSY_PSYCHICBOMB:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_PSY_NLG_CRUCIOSPELL:			rect = CRect( 34, 312, 58, 336 ); break;
		case SI_PSY_NLG_SATANOLGY:				rect = CRect( 84, 260, 108, 284 ); break;
		case SI_PSY_PSY_SPRITBOMB:				rect = CRect( 84, 286, 108, 310 ); break;
		case SI_PSY_PSY_MAXIMUMCRISIS:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_PSY_PSY_PSYCHICWALL:			rect = CRect( 134, 272, 158, 296 ); break;
		case SI_PSY_PSY_PSYCHICSQUARE:			rect = CRect( 134, 312, 158, 336 ); break;
////////////////////////////////////////////////////////////////////////////////////
			
		default:
			return FALSE;
	}

*/
	int nExpertGapX = 9;
	int nExpertGapY = 90;
	
	int nProGapX = 8;
	int nProGapY = 228;

	ItemProp* pPropSkill = prj.GetSkillProp( dwSkillID );

	if( pPropSkill )
	{
		CPoint pt;

		LPWNDCTRL lpWndCtrl;
		LPWNDCTRL lpWndCtrlUpper = GetWndCtrl( WIDC_STATIC5 );
		LPWNDCTRL lpWndCtrlLower = GetWndCtrl( WIDC_STATIC7 );

		switch( pPropSkill->dwItemKind1 )
		{
			case JTYPE_BASE  : pt = 0; break;
			case JTYPE_EXPERT: pt = lpWndCtrlUpper->rect.TopLeft(); break;
			case JTYPE_PRO   : pt = lpWndCtrlLower->rect.TopLeft(); break;
			case JTYPE_MASTER : pt = 0; break;
			case JTYPE_HERO  : pt = 0; break;
		}
		int nRectX, nRectY, nJobKind;
		nRectX = nRectY = 0;
		nJobKind = MAX_JOBBASE;
		int nLegendSkill = 0;
		switch(dwSkillID)
		{
			case SI_VAG_ONE_CLEANHIT:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SI_VAG_ONE_BRANDISH:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SI_VAG_ONE_OVERCUTTER:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
	//�Ӽ��ʸ�
			case SI_MER_ONE_SPLMASH:				nRectX =  34, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PROTECTION:			nRectX =  34, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_KEENWHEEL:				nRectX =  84, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLOODYSTRIKE:			nRectX =  84, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PANBARRIER:			nRectX =  84, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_IMPOWERWEAPON:			nRectX =  84, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLINDSIDE:				nRectX = 134, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_REFLEXHIT:				nRectX = 134, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SNEAKER:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SMITEAXE:				nRectX = 134, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_BLAZINGSWORD:			nRectX = 134, nRectY = 201, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SPECIALHIT:				nRectX = 184, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_GUILOTINE:				nRectX = 184, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_AXEMASTER:				nRectX = 184, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SWORDMASTER:			nRectX = 184, nRectY = 201, nJobKind = MAX_EXPERT; break;
	// ����Ʈ
			case SI_KNT_TWOSW_CHARGE:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_PAINDEALER:			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_GUARD:					nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOSW_EARTHDIVIDER:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_POWERSTUMP:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_RAGE:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWO_POWERSWING:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_PAINREFLECTION:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	// �����̵�
			case SI_BLD_DOUBLESW_SILENTSTRIKE:		nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_SPRINGATTACK:		nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_ARMORPENETRATE:		nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLESW_BLADEDANCE:		nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_HAWKATTACK:		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_SUP_BERSERK:				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_CROSSSTRIKE:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_SONICBLADE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				
	// ��ý��?	
			case SI_ASS_HEAL_HEALING:				nRectX =  18, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PATIENCE:				nRectX =  64, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_QUICKSTEP:			nRectX =  64, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_MENTALSIGN:			nRectX =  64, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_TAMPINGHOLE:			nRectX =  64, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_RESURRECTION:			nRectX = 110, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HASTE:				nRectX = 110, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HEAPUP:				nRectX = 110, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_STONEHAND:			nRectX = 110, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CIRCLEHEALING:		nRectX = 156, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CATSREFLEX:			nRectX = 156, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_BEEFUP:				nRectX = 156, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_BURSTCRACK:				nRectX = 156, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PREVENTION:			nRectX = 202, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CANNONBALL:			nRectX = 202, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_ACCURACY:				nRectX = 202, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_POWERFIST:				nRectX = 202, nRectY = 187, nJobKind = MAX_EXPERT; break;
	//��������
			case SI_RIN_SUP_PROTECT:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYCROSS:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_GVURTIALLA:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYGUARD:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_SPIRITUREFORTUNE:		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_HEALRAIN:				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SQU_GEBURAHTIPHRETH:		nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_MERKABAHANZELRUSHA:		nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//��������
			case SI_BIL_KNU_BELIALSMESHING:			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASMODEUS:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_BLOODFIST:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BARAQIJALESNA:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_PIERCINGSERPENT:		nRectX = 134, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BGVURTIALBOLD:			nRectX = 134, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_SONICHAND:				nRectX = 184, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASALRAALAIKUM:			nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
								
	// ������
			case SI_MAG_MAG_MENTALSTRIKE:			nRectX =  34, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_MAG_BLINKPOOL: 				nRectX =  34, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_BOOMERANG:				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_SWORDWIND:				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_ICEMISSILE:			nRectX =  84, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGBALL:	nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_SPIKESTONE:			nRectX =  84, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_HOTAIR:				nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_STRONGWIND:			nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_WATERBALL:			nRectX = 134, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGPARM:	nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_ROCKCRASH:			nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_FIRESTRIKE:			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_WINDCUTTER:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_SPRINGWATER:			nRectX = 184, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_LOOTING:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
	// ��������
			case SI_ELE_FIRE_FIREBIRD:				nRectX =  34, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_STONESPEAR:			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_VOID:					nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_ICESHARK:				nRectX =  34, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_BURINGFIELD:			nRectX =  84, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHQUAKE:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDFIELD:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_ELETRICSHOCK:	nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_POISONCLOUD:			nRectX =  84, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_METEOSHOWER:			nRectX = 134, nRectY = 246, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_SANDSTORM:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_LIGHTINGSTORM:		nRectX = 134, nRectY = 298, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_AVALANCHE:			nRectX = 134, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_FIREMASTER:			nRectX = 184, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHMASTER:			nRectX = 184, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDMASTER:			nRectX = 184, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_WATERMASTER:			nRectX = 184, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
	// ����Ű��
			case SI_PSY_NLG_DEMONOLGY:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICBOMB:			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_CRUCIOSPELL:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_SATANOLGY:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_SPRITBOMB:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_MAXIMUMCRISIS:			nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICWALL:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICSQUARE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	////////////////////////////////////////////////////////////////////////////////////
	// ��ũ�κ�
			case SI_ACR_YOYO_PULLING:				nRectX =  34, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_SLOWSTEP:				nRectX =  34, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_JUNKBOW:				nRectX =  34, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_FASTWALKER:				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_YOYOMASTER:				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_BOWMASTER: 				nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_DARKILLUSION:			nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_SNITCH:				nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_CROSSLINE:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_SILENTSHOT:				nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_AIMEDSHOT:				nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_ABSOLUTEBLOCK:			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_DEADLYSWING:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_YOYO_COUNTER:				nRectX = 184, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_AUTOSHOT:				nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_ARROWRAIN:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;		
	//������
			case SI_JST_SUP_POISON:					nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_BLEEDING:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_ESCAPE:				nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_CRITICALSWING:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_BACKSTAB:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_ABSORB:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_VATALSTAB:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_HITOFPENYA:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//������
			case SI_RAG_BOW_ICEARROW:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_FLAMEARROW:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_POISONARROW:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_FASTATTACK:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_PIRCINGARROW:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_NATURE:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_TRIPLESHOT:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_SILENTARROW:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				
			//������ ��ų
			case SI_BLD_MASTER_ONEHANDMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_KNT_MASTER_TWOHANDMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_JST_MASTER_YOYOMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_RAG_MASTER_BOWMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_ELE_MASTER_INTMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_PSY_MASTER_INTMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_BIL_MASTER_KNUCKLEMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_RIG_MASTER_BLESSING:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			//���� ��ų			
			case SI_BLD_HERO_DEFFENCE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_KNT_HERO_DRAWING:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_JST_HERO_SILENCE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_RAG_HERO_HAWKEYE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_ELE_HERO_CURSEMIND:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_PSY_HERO_STONE:					nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_BIL_HERO_DISENCHANT:			nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_RIG_HERO_RETURN:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			default:
				return FALSE;
		}

		if( nJobKind == MAX_EXPERT )
			rect = CRect(  nRectX-nExpertGapX ,  nRectY-nExpertGapY, nRectX+24-nExpertGapX, nRectY+24-nExpertGapY );
		else if( nJobKind == MAX_PROFESSIONAL )
			rect = CRect(  nRectX-nProGapX,  nRectY-nProGapY, nRectX+24-nProGapX, nRectY+24-nProGapY );
		else if( nJobKind == MAX_MASTER || nJobKind == MAX_HERO )
		{
			LPWNDCTRL lpWndCtrl;
			if(nLegendSkill == 0)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
			else if(nLegendSkill == 1)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );

			rect = lpWndCtrl->rect;
		}
		
		rect += pt;
	}
/*
	switch( m_nJob )
	{
	case JOB_KNIGHT:
		break;			
	case JOB_BLADE:
		break;			
	case JOB_MERCENARY:
		break;			
	case JOB_BILLPOSTER:
		break;			
	case JOB_RINGMASTER:
		break;			
	case JOB_ASSIST:
		break;			
	case JOB_ELEMENTOR:
		break;			
	case JOB_PSYCHIKEEPER:
		break;			
	case JOB_MAGICIAN:
		break;	
	}
*/
	return TRUE;
}

void CWndSkillTreeEx::LoadTextureSkillicon()
{
	if( m_apSkill == NULL )
		return;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}
	m_textPackNum.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "icon_IconSkillLevel.inc" ) ) );
}
void CWndSkillTreeEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	if( m_bDrag == FALSE )
		return;
	
	CPoint pt( 3, 3 );
	CRect rect;
	if( CheckSkill( g_nSkillCurSelect ) )
	{
		m_bDrag = FALSE;
		DWORD dwSkill = m_apSkill[ g_nSkillCurSelect ].dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		m_GlobalShortcut.m_pFromWnd    = this;
		m_GlobalShortcut.m_dwShortcut  = ShortcutType::Skill;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId       = g_nSkillCurSelect; // �÷�Ʈ ����Ʈ�� �� ID�� ����������.
		m_GlobalShortcut.m_pTexture = m_atexSkill[ g_nSkillCurSelect ];//L;//pItemElem->m_pTexture;
		_tcscpy( m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}

BOOL CWndSkillTreeEx::Process()
{
	if( m_apSkill == NULL || g_pPlayer == NULL)
		return FALSE;
	
	if(!m_bLegend)
	{
		if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		{
			m_bLegend = TRUE;
			//Hero SkillTree
			m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			m_pWndHeroStatic[0]->EnableWindow(TRUE);
			m_pWndHeroStatic[0]->SetVisible(TRUE);
			
			m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
			m_pWndHeroStatic[1]->EnableWindow(TRUE);
			m_pWndHeroStatic[1]->SetVisible(TRUE);

			CRect rect = GetWindowRect();
			rect.bottom += 90;
			m_bSlot[3] = TRUE;
			
			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();
			
			CPoint point( x, y );
			Move( point );
			return TRUE;
		}
	}
	else
	{
		if(g_pPlayer->GetLegendChar() < LEGEND_CLASS_MASTER)
		{
			m_bLegend = FALSE;
			//Normal SkillTree
			m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			m_pWndHeroStatic[0]->EnableWindow(FALSE);
			m_pWndHeroStatic[0]->SetVisible(FALSE);
			
			m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
			m_pWndHeroStatic[1]->EnableWindow(FALSE);
			m_pWndHeroStatic[1]->SetVisible(FALSE);
			
			CRect rect = GetWindowRect();
			rect.bottom -= 100;
			m_bSlot[3] = FALSE;
			
			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();
			
			CPoint point( x, y );
			Move( point );
			return TRUE;
		}
	}

	m_pWndButton[0]->EnableWindow( FALSE );
	m_pWndButton[1]->EnableWindow( FALSE );
	m_pWndButton[2]->EnableWindow( FALSE );
	m_pWndButton[3]->EnableWindow( FALSE );
	
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		LPSKILL lpSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
		ItemProp* pSkillProp = m_pFocusItem->GetProp();
		if( pSkillProp == NULL || lpSkillUser == NULL )
			return TRUE;
		
		int nPoint = prj.GetSkillPoint( pSkillProp );
		if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint )
			m_pWndButton[0]->EnableWindow( TRUE );	

		if( m_pFocusItem->dwLevel != lpSkillUser->dwLevel )
			m_pWndButton[1]->EnableWindow( TRUE );	

		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			m_pWndButton[2]->EnableWindow( TRUE );
			m_pWndButton[3]->EnableWindow( TRUE );
		}

		pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if(pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
		{
			m_pWndButton[0]->EnableWindow( FALSE );
			m_pWndButton[1]->EnableWindow( FALSE );
			m_pWndButton[2]->EnableWindow( FALSE );
			m_pWndButton[3]->EnableWindow( FALSE );
		}
	}
	return TRUE;
}

void CWndSkillTreeEx::OnMouseWndSurface( CPoint point )
{
	DWORD dwMouseSkill = NULL_ID;
	for( int i = 0; i < MAX_SKILL_JOB; i++ )
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL )
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}

			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}

			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}

		if( dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) )
			{
				rect.top  -= 2;
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				rect.OffsetRect( 0, m_nTopDownGap );

				if( rect.PtInRect(point) )
				{
					dwMouseSkill = dwSkill;

					ClientToScreen( &point );
					ClientToScreen( &rect );
					g_WndMng.PutToolTip_Skill( dwSkill, pSkill->dwLevel, point, &rect, CheckSkill( i ) );
					break;
				}
			}
		}
	}
	m_dwMouseSkill = dwMouseSkill;
}

int CWndSkillTreeEx::GetCalcImagePos(int nIndex)
{
	m_nTopDownGap = 0;

	if( m_bSlot[0] )
	{
		if( nIndex == JTYPE_BASE )
			m_nTopDownGap = 0;
	}
	
	if( m_bSlot[1] )
	{
		if( nIndex == JTYPE_EXPERT )
		{
			if( m_bSlot[0] )
				m_nTopDownGap = 0;
			else
				m_nTopDownGap -= 48;
		}
	}

	if( m_bSlot[2] )
	{
		if( nIndex == JTYPE_PRO )
		{
			if( m_bSlot[0] )
			{
				if( m_bSlot[1] )
					m_nTopDownGap = 0;
				else
					m_nTopDownGap -= 144;
			}
			else
			{
				if( m_bSlot[1] )
					m_nTopDownGap -= 48;
				else
					m_nTopDownGap -= (144+48);
			}
		}
	}
	if( m_bSlot[3] )
	{
		if( nIndex == JTYPE_MASTER || nIndex == JTYPE_HERO )
		{
			if( m_bSlot[0] )
			{
				if( m_bSlot[1] )
				{
					if( m_bSlot[2] )
						m_nTopDownGap = 0;
					else
						m_nTopDownGap -= 144;
				}
				else
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= 144;
					else
						m_nTopDownGap -= (144+144);
				}
			}
			else
			{
				if( m_bSlot[1] )
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= 48;
					else
						m_nTopDownGap -= (144+48);
				}
				else
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= (144+48);
					else
						m_nTopDownGap -= (144+144+48);
				}
			}
		}
	}

	return 0;
}

void CWndSkillTreeEx::OnDraw(C2DRender* p2DRender)
{
	if( m_apSkill == NULL )
		return;

	m_nAlphaCount = m_nAlpha;
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

	if(m_bLegend && m_bSlot[3])
	{
		//Master Skill�� ���ۺ��� 1Lv�̹Ƿ� ���? �̹��� ����.
/*		if( !m_strHeroSkilBg[0].IsEmpty() )
		{
			CTexture* pTexture;
			CRect rect;
			CPoint point;
			LPWNDCTRL lpWndCtrl;
			
			lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
			rect = lpWndCtrl->rect;
			GetCalcImagePos(JTYPE_MASTER);
			rect.OffsetRect( 0, m_nTopDownGap );
			point = rect.TopLeft();
			point.y -= 2;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strHeroSkilBg[0]), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
		}
*/		
		if( !m_strHeroSkilBg.IsEmpty() )
		{
			CTexture* pTexture;
			CRect rect;
			CPoint point;
			LPWNDCTRL lpWndCtrl;
			
			lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
			rect = lpWndCtrl->rect;
			GetCalcImagePos(JTYPE_HERO);
			rect.OffsetRect( 0, m_nTopDownGap );
			point = rect.TopLeft();
			point.y -= 2;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strHeroSkilBg), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
		}
	}

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;

		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}
		
		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			// ��ų ������ ���? 
			if( m_atexSkill[ i ] && CheckSkill( i ) && 0 < pSkill->dwLevel )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					rect.OffsetRect( 0, m_nTopDownGap );
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );
					int nAddNum = 0;
					LPSKILL pSkillUser = g_pPlayer->GetSkill( pSkill->dwSkill );
					if( pSkillUser && pSkill->dwLevel != pSkillUser->dwLevel )
						nAddNum = 20;
					if( pSkill->dwLevel < pSkillProp->dwExpertMax )
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), pSkill->dwLevel-1 + nAddNum );
					else
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), 19 + nAddNum );
				}
			}	
			else if( m_dwMouseSkill == dwSkill && m_atexSkill[ i ] && CheckSkill( i ) )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					rect.OffsetRect( 0, m_nTopDownGap );
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );				
				}
			}
		}
	}

	CWndStatic* lpWndStatic9 = (CWndStatic*)GetDlgItem( WIDC_STATIC9 );
	CString strSP;
	strSP.Format( "%d", m_nCurrSkillPoint );
	lpWndStatic9->SetTitle( strSP );

	// ���õ� ��ų�� ������ �� â�� ���?
	if( m_pFocusItem && g_nSkillCurSelect > -1 )
	{
		if( m_pFocusItem->dwSkill == -1 )
		{
			CString str;
			str.Format( "��ų���� ���� %d\n��ų �ε����� %d�ΰ��� ���� : ���� %d, CurSelect %d", g_pPlayer->GetJob(), m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, g_nSkillCurSelect );
			//ADDERRORMSG( str );
		}

		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_atexSkill[ g_nSkillCurSelect ]->Render( p2DRender, lpWndCtrl->rect.TopLeft() );

		ItemProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp && 0 < m_pFocusItem->dwLevel )
		{
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
			if( pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel )
				nAddNum = 20;

			if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
				m_textPackNum.Render( p2DRender, lpWndCtrl->rect.TopLeft(), m_pFocusItem->dwLevel-1 + nAddNum );
			else
				m_textPackNum.Render( p2DRender, lpWndCtrl->rect.TopLeft(), 19 + nAddNum );
		}
	}
	CRect rect;
	CWndStatic* pStatic;
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
	rect = pStatic->GetWindowRect(TRUE);

	rect.left = rect.right - 30;
	
	if( m_bSlot[0] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	rect = pStatic->GetWindowRect(TRUE);
	
	rect.left = rect.right - 30;
	
	if( m_bSlot[1] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	rect = pStatic->GetWindowRect(TRUE);
	
	rect.left = rect.right - 30;
	
	if( m_bSlot[2] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	
	///////////////////////////////////////////////////////////////////////////////////////
	
	if(m_bLegend)
	{
		pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		rect = pStatic->GetWindowRect(TRUE);
		
		rect.left = rect.right - 30;
		
		if( m_bSlot[3] )
			m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
		else
			m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	}


/*
	{
	CString str;
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM12 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM13 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM14 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	}
	/**/	
}
void CWndSkillTreeEx::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );

	m_aSkillLevel[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelup01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 2 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold02.tga" ), 0xffff00ff );
	
	InitItem( g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );


	m_atexTopDown[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "LvUp.bmp" ), 0xffff00ff );
	m_atexTopDown[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "LvDn.bmp" ), 0xffff00ff );

	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
	m_bSlot[3] = TRUE;
	
	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];

	m_pWndButton[0] = (CWndButton*) GetDlgItem( WIDC_BUTTON1 );	// + ��ư
	m_pWndButton[1] = (CWndButton*) GetDlgItem( WIDC_BUTTON2 );	// - ��ư
	m_pWndButton[2] = (CWndButton*) GetDlgItem( WIDC_BUTTON3 );	// Reset ��ư
	m_pWndButton[3] = (CWndButton*) GetDlgItem( WIDC_BUTTON4 );	// Finish ��ư
	
	if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		m_bLegend = TRUE;
	else
		m_bLegend = FALSE;

	if(m_bLegend)
	{
		m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		m_pWndHeroStatic[0]->EnableWindow(TRUE);
		m_pWndHeroStatic[0]->SetVisible(TRUE);

		m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
		m_pWndHeroStatic[1]->EnableWindow(TRUE);
		m_pWndHeroStatic[1]->SetVisible(TRUE);
	}
	else
	{
		m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		m_pWndHeroStatic[0]->EnableWindow(FALSE);
		m_pWndHeroStatic[0]->SetVisible(FALSE);
		
		m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
		m_pWndHeroStatic[1]->EnableWindow(FALSE);
		m_pWndHeroStatic[1]->SetVisible(FALSE);
		
		CRect rect = GetWindowRect();
		rect.bottom -= 100;
		m_bSlot[3] = FALSE;
		
		SetWndRect(rect);
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - rect.Height();

	CPoint point( x, y );
	Move( point );
}
BOOL CWndSkillTreeEx::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	return CWndNeuz::InitDialog( dwWndId, pWndParent, 0, CPoint( 792, 130 ) );
}
BOOL CWndSkillTreeEx::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		ItemProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp )
		{
			int nPoint = prj.GetSkillPoint( pSkillProp );
			switch( nID )
			{
			case WIDC_BUTTON1:	// + ��ư
				{
					if( nPoint <= m_nCurrSkillPoint )
					{
						if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
						{
							m_nCurrSkillPoint -= nPoint;
							++m_pFocusItem->dwLevel;
						}
						else
						{
							break;							
						}
					}					
				}
				break;
			case WIDC_BUTTON2:	// - ��ư
				{
					if( IsDownPoint(m_pFocusItem->dwSkill) )
					{
						m_nCurrSkillPoint += nPoint;
						--m_pFocusItem->dwLevel;
						SubSkillPointDown( m_pFocusItem );
					}
				}
				break;
			case WIDC_BUTTON3:	// Reset ��ư
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
						InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
					m_pFocusItem = NULL;
				}
				break;
			case WIDC_BUTTON4:	// Finish ��ư
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
					{
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
						g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					}
				}
				break;
			}
		}
	}

	if( nID == 10000 )
	{
		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase == NULL )
			{
				SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
				g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
				g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
				g_WndMng.m_pWndReSkillWarning->InitItem( TRUE );
			}
			return TRUE;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSkillTreeEx::SetJob( int nJob )
{
	m_nJob = nJob;
}
void CWndSkillTreeEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = FALSE;
}
void CWndSkillTreeEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	CPoint ptMouse = GetMousePoint();


	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
	CRect rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[0] )
		{
			SetActiveSlot(0, FALSE );
		}
		else
		{
			SetActiveSlot(0, TRUE );
		}
	}

	pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[1] )
		{
			SetActiveSlot(1, FALSE );
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(2, TRUE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
		else
		{
			SetActiveSlot(1, TRUE );
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(2, FALSE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
	}

	pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[2] )
		{
			SetActiveSlot(2, FALSE );
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(1, TRUE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
		else
		{
			SetActiveSlot(2, TRUE );
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(1, FALSE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
	}

	if(m_bLegend)
	{
		pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		rect = pStatic->GetWindowRect(TRUE);
		if( rect.PtInRect( point ) ) 
		{
			if( m_bSlot[3] )
				SetActiveSlot(3, FALSE );
			else
				SetActiveSlot(3, TRUE );
		}
	}
	
				
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;

				rect.OffsetRect( 0, m_nTopDownGap );
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];
					m_bDrag = TRUE;
					break;
				}			
			}	
		}
	}
}
void CWndSkillTreeEx::OnRButtonDblClk( UINT nFlags, CPoint point)
{

}	

void CWndSkillTreeEx::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	// ��ų�� ���? �� ��ų���� ��
	if( m_apSkill == NULL )
		return;

	// ��ųâ���� ����Ŭ���ϸ� �ڵ����� ��ų�ٿ� ��ϵȴ�?.
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
			
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				
				CPoint ptMouse = GetMousePoint();

				rect.OffsetRect( 0, m_nTopDownGap );
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];

					if( g_pPlayer->CheckSkill( m_pFocusItem->dwSkill ) == FALSE )
						return;
					CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
					if( pTaskBar->m_nExecute == 0 )		// ��ųť�� ������ �������� ��ϵ�?.
						pTaskBar->SetSkillQueue( pTaskBar->m_nCurQueueNum, 0, i, m_atexSkill[i] );
					break;
				}			
			}	
		}
	}
}

void CWndSkillTreeEx::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl;
 
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_STATIC5 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;

	pt.y += 2;
	
	GetCalcImagePos(JTYPE_EXPERT);
	pt.y += m_nTopDownGap;

	if( m_atexJobPannel[ 0 ] && m_bSlot[1] ) PaintTexture( pDest, m_atexJobPannel[ 0 ], pt, size );

	lpWndCtrl = GetWndCtrl( WIDC_STATIC7 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	GetCalcImagePos(JTYPE_PRO);
	pt.y += 2;
	pt.y += m_nTopDownGap;
	
	if( m_atexJobPannel[ 1 ] && m_bSlot[2] ) PaintTexture( pDest, m_atexJobPannel[ 1 ], pt, size );


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

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
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

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
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

CWndNavigator::CWndNavigator()
{
	m_bObjFilterPlayer  = TRUE;
	m_bObjFilterParty   = TRUE;
	m_bObjFilterNPC     = TRUE;
	m_bObjFilterMonster = TRUE;
	m_szName[ 0 ] = 0;
	m_iFrame = 0;
	m_iPastTime = 0;
	m_size.cx = 0;
	m_size.cy = 0;
	m_pDestinationPositionTexture = NULL;
}
CWndNavigator::~CWndNavigator()
{

}

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
		CObj* pObj;
		int nIndex = 0;
		int nPartyMap[ MAX_PTMEMBER_SIZE ];
		CMover* apQuest[ 100 ];
		int nQuestNum = 0;
		ZeroMemory( nPartyMap, sizeof(nPartyMap) );
		int nPartyMapCount = 0;
		float fDistMap = rect.Width() / 2 / fx;

		FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
		{
			FOR_OBJ( pLand, pObj, OT_MOVER )
			{
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
						//xu = 10, yv = 0;
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
			END_OBJ
		}
		END_LAND
		////////////////////////////////////////////////////////
		// ���� ������Ʈ�� ������ ��Ʈ���� �̸�Ƽ�� ���? 
		////////////////////////////////////////////////////////
		int nSize = pWorld->m_aStructure.GetSize();
		for( i = 0; i < nSize; i++ )
		{
			LPREGIONELEM lpRegionElem = pWorld->m_aStructure.GetAt( i );
			vPos = lpRegionElem->m_vPos;
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;
			m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpRegionElem->m_dwStructure, &pVertices );
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
/*			
			
			LPCHARACTER lpCharacter = pMover->GetCharacter();
			if( pMover->m_nQuestEmoticonIndex == 0 || pMover->m_nQuestEmoticonIndex == 2 ) 
			{
				if( lpCharacter && lpCharacter->m_nStructure == -1 )
				{
					nIndex = pMover->m_nQuestEmoticonIndex + 20;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );
				}
			}
			else
			if( pMover->m_nQuestEmoticonIndex == 1 || pMover->m_nQuestEmoticonIndex == 3 ) 
			{
				//if( lpCharacter && lpCharacter->m_nStructure != -1 )
				{
					nIndex = pMover->m_nQuestEmoticonIndex + 20;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );
				}
			}
*/
		}
/*	#ifdef __S1005_PARTY_MAP
		nPartyMap[nPartyMapCount] = g_Party.FindMember( g_pPlayer->m_idPlayer );
		if( nPartyMap[nPartyMapCount] != -1 )
			++nPartyMapCount;
		for( i = 0 ; i < g_Party.GetSizeofMember() && g_Party.GetSizeofMember() != nPartyMapCount ; ++i )
		{
			BOOL bDisplay = TRUE;
			for( int j = 0 ; j < nPartyMapCount ; ++j )
			{
				if( nPartyMap[j] == i )
				{
					bDisplay = FALSE;
					break;
				}
			}
					
			if( bDisplay )
			{
				D3DXVECTOR3 vPosBuf = g_pPlayer->GetPos() - g_Party.GetPos( i );
				float fDist = D3DXVec3Length( &vPosBuf );

				
				if( fDist > fDistMap )
				{
					RenderPartyMember( p2DRender, &pVertices, rect, g_Party.GetPos( i ), g_Party.m_aMember[i].m_uPlayerId, g_Party.m_aMember[i].m_szName );
				}
				else
				{
					vPos = g_Party.GetPos( i );
					vPos.x *= fx;
					vPos.z *= fy;
					point.x = ( rect.Width() / 2 ) + vPos.x;
					point.y = ( rect.Height() / 2 ) - vPos.z;
					point.x -= xCenter;
					point.y -= yCenter;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), 3, &pVertices );

					CRect rectHittest( point.x, point.y, point.x + 5, point.y + 5);
					CPoint ptMouse = GetMousePoint();
					if( rectHittest.PtInRect( ptMouse ) )
					{
						ClientToScreen( &ptMouse );
						ClientToScreen( &rectHittest );
						g_toolTip.PutToolTip( 10000 + g_Party.m_aMember[i].m_uPlayerId, g_Party.m_aMember[i].m_szName, rectHittest, ptMouse, 0 );
					}
				}
			}
		}
	#endif // __S1005_PARTY_MAP
*/
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
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
/*
	FLOAT fAngle	= ( g_pPlayer != NULL ? g_pPlayer->GetAngle() - 180 : 0 );
	if( fAngle < 0 ) fAngle = 360 + fAngle;
	FLOAT fTheta = D3DXToRadian( fAngle );// (2*D3DX_PI*g_pPlayer->m_nAngle)/(360);//m_nAngle
	D3DXMatrixRotationZ( &mat, fTheta );
*/
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

void CWndNavigator::RenderMark( C2DRender* p2DRender, CMover* Player )
{
	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	if ( Player->m_nvPoint.Time != 0 )
	{	
		D3DXVECTOR3 Pos = g_pPlayer->GetPos() - Player->m_nvPoint.Pos;
		Pos.x *= fx ;
		Pos.z *= -fy;
		Player->m_nvPoint.Time--;

		CPoint point;
		CRect rect = GetClientRect();
		point.x = (LONG)( ( rect.Width() / 2 ) - ( Pos.x + 2 ) );
		point.y = (LONG)( ( rect.Height() / 2 ) - ( Pos.z + 2 ) );
		
		//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
		m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

		CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
		CPoint ptMouse = GetMousePoint();
		if( rectHit.PtInRect( ptMouse ) )
		{
//			CString toolTip("Mark : ");
			CString toolTip		= prj.GetText( TID_GAME_NAV_MARK );
			toolTip += Player->GetName( TRUE );
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHit );
			g_toolTip.PutToolTip( 10000 + Player->GetId(), toolTip, rectHit, ptMouse, 0 );
		}
	}
}

//	�̰� �־� �ش����� ������ ���ؼ� NaviPoint���� ������ �����ͷ� �޴� ���·� �ٽ� �����? �־��? �Ұ��̴�.
void CWndNavigator::RenderMarkAll( C2DRender* p2DRender , CMover * Player )
{
	RenderMark( p2DRender, Player );

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	V_NaviPoint::iterator nvi = g_pPlayer->m_vOtherPoint.begin();
	for ( int i = 0 ; i < (int)( g_pPlayer->m_vOtherPoint.size() ) ; )
	{
		if ( g_pPlayer->m_vOtherPoint[i].Time != 0 )
		{
			D3DXVECTOR3 Pos = g_pPlayer->GetPos() - g_pPlayer->m_vOtherPoint[i].Pos;
			Pos.x *= fx ;
			Pos.z *= -fy;
			g_pPlayer->m_vOtherPoint[i].Time--;

			CPoint point;
			CRect rect = GetClientRect();
			point.x = (LONG)( ( rect.Width() / 2 ) - ( Pos.x + 2 ) );
			point.y = (LONG)( ( rect.Height() / 2 ) - ( Pos.z + 2 ) );
			
			//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
			m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

			CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
			CPoint ptMouse = GetMousePoint();
			if( rectHit.PtInRect( ptMouse ) )
			{
//				CString toolTip("Mark : ");
				CString toolTip		= prj.GetText( TID_GAME_NAV_MARK );
				toolTip += g_pPlayer->m_vOtherPoint[i].Name.c_str();
				ClientToScreen( &ptMouse );
				ClientToScreen( &rectHit );
				g_toolTip.PutToolTip( 10000 + g_pPlayer->m_vOtherPoint[i].objid, toolTip, rectHit, ptMouse, 0 );
			}
			nvi++;
			i++;
		}
		else
		{
			g_pPlayer->m_vOtherPoint.erase(nvi);
		}
		
	}
}

void CWndNavigator::RenderPartyMember( C2DRender* p2DRender, TEXTUREVERTEX** pVertices, CRect rect, D3DXVECTOR3 vPos, u_long uIdPlayer, LPCTSTR lpStr )
{
	float fDistx = g_pPlayer->GetPos().x - vPos.x;
	float fDistz = g_pPlayer->GetPos().z - vPos.z;
	float fRadian = atan2( fDistx, fDistz );
	float fDegree = D3DXToDegree( fRadian );
	CPoint PointBuf;
	
	if( fDegree < 0 )
	{
		if( fDegree > -45 )
			PointBuf = CPoint( (int)( rect.Width() / 2 + -fDegree ), rect.Height() - 10 );
		else if( fDegree > -135 )
			PointBuf = CPoint( rect.Width() - 10, (int)( rect.Height() / 2 + 90 + fDegree ) );
		else
			PointBuf = CPoint( (int)( rect.Width() / 2 + (180 + fDegree) ), 5 );
	}
	else
	{
		if( fDegree < 45 )
			PointBuf = CPoint( (int)( rect.Width() / 2 + -fDegree ), rect.Height() - 10 );
		else if( fDegree < 135 )
			PointBuf = CPoint( 8, (int)( rect.Height() / 2 + 90 -fDegree ) );
		else
			PointBuf = CPoint( (int)( rect.Width() / 2 + -(180 - fDegree) ), 5 );
	}

	m_texNavObjs.MakeVertex( p2DRender, PointBuf, 3, pVertices );

	CRect rectHittest( PointBuf.x, PointBuf.y, PointBuf.x + 5, PointBuf.y + 5);
	CPoint ptMouse = GetMousePoint();
	if( rectHittest.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rectHittest );
		g_toolTip.PutToolTip( 10000 + uIdPlayer, lpStr, rectHittest, ptMouse, 0 );
	}
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
	m_texNavPos.InvalidateDeviceObjects();
	m_GuildCombatTextureMask.Invalidate();
#ifdef __YDEBUG
	m_texArrow.Invalidate();
	m_texDunFog.Invalidate();
	m_texNavObjs.InvalidateDeviceObjects();
#endif //__YDEBUG
	
	m_billArrow.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndNavigator::RestoreDeviceObjects()
{
	m_texNavPos.RestoreDeviceObjects( m_pApp->m_pd3dDevice );
	m_GuildCombatTextureMask.SetInvalidate(m_pApp->m_pd3dDevice);
#ifdef __YDEBUG
	m_texArrow.SetInvalidate(m_pApp->m_pd3dDevice);
	m_texDunFog.SetInvalidate(m_pApp->m_pd3dDevice);
	m_texNavObjs.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
#endif //__YDEBUG

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
//	m_wndMover.Create  ( "U", 0, CRect( rectClient.left,  17, rectClient.left + 16,  17 + 16 ), this, 100001 );
//	m_wndMover.Create  ( "M", 0, CRect( rectClient.left,  36, rectClient.left + 16,  36 + 16 ), this, 102 );
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
	/*
	m_wndMenuPlace.AppendMenu( 0, 0 ,_T("�÷��̾�" ) );
	m_wndMenuPlace.AppendMenu( 0, 1 ,_T("�ش�"     ) ); 
	m_wndMenuPlace.AppendMenu( 0, 2 ,_T("NPC"      ) );
	m_wndMenuPlace.AppendMenu( 0, 3 ,_T("����"   ) ); 
	*/
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_PLAYER) );
	m_wndMenuPlace.AppendMenu( 0, 1 , prj.GetText(TID_GAME_PARTYTEXT) ); 
	m_wndMenuPlace.AppendMenu( 0, 2 , prj.GetText(TID_GAME_NPC) );
	m_wndMenuPlace.AppendMenu( 0, 3 , prj.GetText(TID_GAME_MONSTER) ); 
	

	m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
	m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
	m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
	m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );

/*
	m_wndMenuPlace.AppendMenu( 0, 0 ,_T("�������?" ) );
	m_wndMenuPlace.AppendMenu( 0, 1 ,_T("��ȭ��"   ) );
	m_wndMenuPlace.AppendMenu( 0, 2 ,_T("����?" ) ); 
	m_wndMenuPlace.AppendMenu( 0, 3 ,_T("����"     ) );
	m_wndMenuPlace.AppendMenu( 0, 4 ,_T("���� "    ) );
	m_wndMenuPlace.AppendMenu( 0, 5 ,_T("�������?" ) );
	m_wndMenuPlace.AppendMenu( 0, 6 ,_T("��ȭ��"   ) );
	m_wndMenuPlace.AppendMenu( 0, 7 ,_T("����?" ) ); 
	m_wndMenuPlace.AppendMenu( 0, 8 ,_T("����"     ) );
	m_wndMenuPlace.AppendMenu( 0, 9 ,_T("-- ����߰�? --" ));
*/
	m_wndMenuMover.CreateMenu( this );	
	/*
	CRect rect = GetClientRect();
	D3DXVECTOR3 vPos = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0 , 0 ) );
	// 128 : m_texture.m_size.cx = 1 : x
	FLOAT fx = (FLOAT)m_texture.m_size.cx / 256.0f * 2;
	FLOAT fy = (FLOAT)m_texture.m_size.cy / 256.0f * 2;
	vPos.x *= fx;
	vPos.z *= fy;
	CObj* pObj;
	D3DXVECTOR3 vCenter	= ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );
	*/

	//m_size = CSize( MINIMAP_SIZE, MINIMAP_SIZE );
	m_size = CSize( 256, 256) ;//MINIMAP_SIZE, MINIMAP_SIZE );
	m_nSizeCnt = 0;
	//m_wndMinimize.Create("_",0,CRect(00,0,14,14),this,10000);

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

//	return CWndNeuz::Create( WBS_MOVE|WBS_SOUND|WBS_CAPTION, rect, &g_WndMng, dwWndId );
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
	//int nImageBlock = pLand->m_texMiniMap.m_size.cx;

	CWndBase* pWndBase = (CWndBase*) pLResult;

	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
	//	g_WndMng.m_nObjectFilter = nID;
		//m_wndMenuPlace.CheckMenuRadioItem( 0, 2, nID, TRUE );
		int nState = !m_wndMenuPlace.GetMenuState( nID, 0 );
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
					CObj* pObj;
					CWndButton* pWndButton;
					int i = 0;

					FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
					{
						FOR_OBJ( pLand, pObj, OT_MOVER )
						{
							pWndButton = m_wndMenuMover.AppendMenu( i++, ((CMover*)pObj)->GetId() , ((CMover*)pObj)->GetName( TRUE ) );
							pWndButton->m_shortcut.m_dwShortcut = ShortcutType::Object;
						}
						END_OBJ
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
				if(m_size.cx > 352)
					m_size.cx = 352;
				if(m_size.cy > 352 )
					m_size.cy = 352;
				/*
				if(m_size.cx > 1024)
					m_size.cx = 1024;
				if(m_size.cy > 1024 )
					m_size.cy = 1024;
					*/
				ResizeMiniMap();
				break;
			case 100006: // zoom out 
				m_nSizeCnt = -1;
				m_size.cx -= 32;
				m_size.cy -= 32;
				if( m_size.cx < 128 )
					m_size.cx = 128;
				if( m_size.cy < 128 )
					m_size.cy = 128;
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
	CObj* pObj;
	CWndButton* pWndButton;
	int i = 0;

	CMover* pMover = NULL;
	int nTarget = 0;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			pMover = ( CMover* )pObj;
			if( !pMover->IsPlayer( ) && pMover->GetCharacter( ) )		//NPC�ΰ��? 
			{
				pWndButton = m_wndMenuMover.AppendMenu( i++, ((CMover*)pObj)->GetId() , ((CMover*)pObj)->GetName( TRUE ) );
				pWndButton->m_shortcut.m_dwShortcut = ShortcutType::Object;
				++nTarget;
			}
		}
		END_OBJ
	}
	END_LAND

	if( nTarget > 0 )
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
	
	CObj* pObj;
	CLandscape* pLand;
//	CWorld* pWorld	= g_WorldMng();
	CMover *pMover;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				pMover = (CMover *)pObj;
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
						if( CMover::GetActiveMover() != pObj )
							pWorld->SetObjFocus( pObj );
						return;
					}
				}
			}
		}
		END_OBJ
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
	CMover* pMover = CMover::GetActiveMover();

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
	CMover* pMover = CMover::GetActiveMover();
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
		EXPINTEGER	nExpResult = pMover->GetExp1() * (EXPINTEGER)10000 / pMover->GetMaxExp1();
		float fExp = (float)nExpResult / 100.0f;
		if( fExp >= 99.99f )
			nCharEXP = sprintf( cbufExp, "99.99%%" );		// sprintf�Լ� ���ο��� �ݿø��Ǿ� 100.00���� ǥ�õǴ� ���� ���� ���ؼ� 
		else
			nCharEXP = sprintf( cbufExp, "%.2f%%", fExp );

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

#ifdef __YDEBUG
	m_texGauEmptyNormal.SetInvalidate(m_pApp->m_pd3dDevice); 
	m_texGauEmptySmall.SetInvalidate(m_pApp->m_pd3dDevice);  
	m_texGauFillNormal.SetInvalidate(m_pApp->m_pd3dDevice);  
	m_texGauFillSmall.SetInvalidate(m_pApp->m_pd3dDevice);   
#endif //__YDEBUG
	
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

#ifdef __YDEBUG
	m_texGauEmptyNormal.Invalidate(); 
	m_texGauEmptySmall.Invalidate(); 
	m_texGauFillNormal.Invalidate(); 
	m_texGauFillSmall.Invalidate(); 
#endif //__YDEBUG
	
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
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

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
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

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
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

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
	
	// ������ �������� ������ �ϱ�
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
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
CWndWantedConfirm::CWndWantedConfirm() 
{ 
	memset( m_szName, 0, sizeof(m_szName) );
} 

void CWndWantedConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
BOOL CWndWantedConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( APP_WANTED_CONFIRM, nullptr, WBS_MODAL );
	
	return TRUE;
} 

void CWndWantedConfirm::SetInfo(const char szName[], const int nGold)
{
	m_nGold  = nGold;
	_tcscpy( m_szName, szName );
	
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pWndStatic->SetTitle(szName);

	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );

	CString strMsg1, strMsg2;

	strMsg1 = prj.GetText( TID_PK_POINT_SHOW );
	strMsg2.Format( prj.GetText(TID_PK_WASTE_SHOW), REQ_WANTED_GOLD );

	pWndStatic->SetTitle( strMsg1 + strMsg2 );	

	 
}
BOOL CWndWantedConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case  WIDC_BUTTON1:
			{
				if( strlen(m_szName) > 0 )
				{
					g_DPlay.SendWantedInfo(m_szName);
				}
				Destroy();
			}
			break;
		case  WIDC_BUTTON2:
			{
				Destroy();
			}
			break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

////////////////////////////////////////////////////////////////////////////////////////
// CWndWanted
////////////////////////////////////////////////////////////////////////////////////////

const int MAX_WANTED_PER_PAGE = 20;

CWndWanted::CWndWanted() 
{ 
	Init( 0 );
} 

CWndWanted::~CWndWanted() 
{ 
	SAFE_DELETE(m_pWantedConfirm);
} 

void CWndWanted::Init( time_t lTime )
{
	memset( m_aList, 0, sizeof(m_aList) );
	m_recvTime = lTime;
	m_pWantedConfirm  = NULL;
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndWanted::OnInitialUpdate() 
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
		if( m_nMax < MAX_WANTED_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_WANTED_PER_PAGE );
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
//	strTitle += tm.Format( " - %H�� %M�� %S���� ����Ÿ�Դϴ�." );
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
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndWanted::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

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

void CWndWanted::InsertWanted( const char szName[], __int64 nGold, int nTime, const char szMsg[] )
{
	if( m_nMax >= MAX_WANTED_LIST )
	{
		Error( "CWndWanted::InsertWanted - range over" );
		return;
	}

	_tcscpy( m_aList[m_nMax].szName, szName );
	m_aList[m_nMax].nGold = nGold;

	CTime cTime( (time_t)nTime );		
	SYSTEMTIME st;
	st.wYear= cTime.GetYear(); 
	st.wMonth= cTime.GetMonth(); 
	st.wDay= cTime.GetDay();  

	if( GetDateFormat( LOCALE_USER_DEFAULT, 0, &st, NULL, m_aList[m_nMax].szDate, sizeof(m_aList[m_nMax].szDate) ) == 0 )
		m_aList[m_nMax].szDate[0] = '\0';

	_tcscpy( m_aList[m_nMax].szMsg, szMsg );
	m_nMax++;
}

void CWndWanted::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szGold[16];
	CString strGold;

	sx = 8;
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
		if( i >= m_nMax )	
			break;
		
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

	// ������ �������� ������ �ϱ�
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
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

void CWndCommItemDlg::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	m_pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_COMMUSE );
	if( m_pWndEdit )
		m_pWndEdit->EnableWindow( FALSE );

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
}
void CWndCommItemDlg::SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId )
{
	m_pWndEdit->AddString( prj.GetText( dwDefindText ) );
	m_dwObjId = dwObjId;
	m_dwCtrlId = dwCtrlId;
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndCommItemDlg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_COMMITEM_DIALOG, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndCommItemDlg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK || message == EN_RETURN )
	{
		if( g_WndMng.GetWndBase( APP_SHOP_ )  ||
			g_WndMng.GetWndBase( APP_BANK )  ||
			g_WndMng.GetWndBase( APP_TRADE ) )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
		}
		else
		if( g_WndMng.GetWndBase( APP_REPAIR ) )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
		}
		else
		{
			if( m_dwObjId == II_SYS_SYS_SCR_CHACLA )
			{
				if( g_pPlayer->IsExpert() )
				{
					SAFE_DELETE( g_WndMng.m_pWndChangeClass1 );
					g_WndMng.m_pWndChangeClass1 = new CWndChangeClass1;
					g_WndMng.m_pWndChangeClass1->Initialize( &g_WndMng, APP_CHANGECLASS_1 );
				}
				else
				{
					SAFE_DELETE( g_WndMng.m_pWndChangeClass2 );
					g_WndMng.m_pWndChangeClass2 = new CWndChangeClass2;
					g_WndMng.m_pWndChangeClass2->Initialize( &g_WndMng, APP_CHANGECLASS_2 );
				}
			}
			else if( m_dwCtrlId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE )
			{
				g_DPlay.SendPetTamerMistake(m_dwObjId);
			}
			else
			{
#ifdef __AZRIA_1023
				CItemElem * pItem = g_pPlayer->GetItemId(m_dwObjId);
				if( pItem )
				{
					if( pItem->GetProp()->dwItemKind3 == IK3_TICKET )	// +
						g_DPlay.SendDoUseItemInput( MAKELONG( ITYPE_ITEM, m_dwObjId ), "0" );
					else	
						g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwObjId ), m_dwCtrlId, -1, FALSE );
				}
#else	// __AZRIA_1023
				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwObjId ), m_dwCtrlId, -1, FALSE );
#endif	// __AZRIA_1023
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

BOOL CWndChangeClassGeneric::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_BUTTON_OK || nID == WIDC_BUTTON_OK2 || message == EN_RETURN) {
		if (g_pPlayer->GetJob() == m_currentJobId) {
			g_WndMng.PutString(TID_GAME_EQUALJOB);
		} else {
			g_DPlay.SendChangeJob(m_currentJobId, FALSE);
			Destroy();
		}
	} else if (nID == WIDC_BUTTON_CANCEL || nID == WIDC_BUTTON_CANCEL2 || nID == WTBID_CLOSE) {
		Destroy();
	} else {
		for (const auto & [widgetId, jobId] : m_allJobs) {
			if (nID == widgetId) {
				m_currentJobId = jobId;
				OnModifiedJob();
				break;
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndChangeClassGeneric::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	// Set current job to player's current job
	const int currentJob = g_pPlayer->GetJob();
	m_currentJobId = currentJob;
	OnModifiedJob();

	// Disable the player's current job
	const auto itJob = std::ranges::find_if(m_allJobs,
		[currentJob](const PossibleJob & possibleJob) {
			return possibleJob.jobId == currentJob;
		});

	if (itJob != m_allJobs.end()) {
		CWndBase * const widget = GetDlgItem(itJob->widgetId);
		widget->EnableWindow(FALSE);
	}

	// Ok
	MoveParentCenter();
}

void CWndChangeClassGeneric::OnModifiedJob() {
	for (const auto & [widgetId, jobId] : m_allJobs) {
		CWndButton * const widget = GetDlgItem<CWndButton>(widgetId);
		widget->SetCheck(m_currentJobId == jobId ? TRUE : FALSE);
	}
}

CWndChangeClass1::CWndChangeClass1()
	: CWndChangeClassGeneric(
		{
			PossibleJob{ WIDC_RADIO_MER, JOB_MERCENARY },
			PossibleJob{ WIDC_RADIO_ACR, JOB_ACROBAT },
			PossibleJob{ WIDC_RADIO_ASS, JOB_ASSIST },
			PossibleJob{ WIDC_RADIO_MAG, JOB_MAGICIAN }
		}
	) {
}

BOOL CWndChangeClass1::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_CHANGECLASS_1, pWndParent, 0, CPoint(0, 0));
}

/****************************************************
WndId : APP_CHANGECLASS_2 - ��������(2��)
CtrlId : WIDC_STATIC1 - ������ ���ϴ� ������ �����Ͻʽÿ�
CtrlId : WIDC_STATIC2 - ���� ���?
CtrlId : WIDC_RADIO_KNI - ����Ʈ
CtrlId : WIDC_RADIO_BLA - �����̵�
CtrlId : WIDC_RADIO_RIN - ��������
CtrlId : WIDC_RADIO_PSY - ����Ű��
CtrlId : WIDC_RADIO_ELE - ��������
CtrlId : WIDC_RADIO_JES - ������
CtrlId : WIDC_RADIO_RAN - ������
CtrlId : WIDC_RADIO_BIL - ��������
CtrlId : WIDC_BUTTON_OK2 - Button
CtrlId : WIDC_BUTTON_CANCEL2 - Button
****************************************************/

CWndChangeClass2::CWndChangeClass2()
	: CWndChangeClassGeneric(
		{
			PossibleJob{ WIDC_RADIO_KNI, JOB_KNIGHT },
			PossibleJob{ WIDC_RADIO_BLA, JOB_BLADE },
			PossibleJob{ WIDC_RADIO_JES, JOB_JESTER },
			PossibleJob{ WIDC_RADIO_RAN, JOB_RANGER },
			PossibleJob{ WIDC_RADIO_RIN, JOB_RINGMASTER },
			PossibleJob{ WIDC_RADIO_BIL, JOB_BILLPOSTER },
			PossibleJob{ WIDC_RADIO_PSY, JOB_PSYCHIKEEPER },
			PossibleJob{ WIDC_RADIO_ELE, JOB_ELEMENTOR }
		}
	) {
}

BOOL CWndChangeClass2::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_CHANGECLASS_2, pWndParent, 0, CPoint(0, 0));
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


void CWndReSkillControl::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( prj.GetText( TID_RESKILLPOINT_CONTROL1 )  );
	m_wndTitleBar.SetVisible( FALSE );	
	pWndText->ResetString();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndReSkillControl::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_RESKILL_CONTROL1, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndReSkillControl::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
		
		if( pSkillTree )
			pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
	}
	else if( nID == WIDC_BUTTON2 )
	{
		SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
		g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
		g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
	}
	else if( nID == 10000 )
	{
		return TRUE;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


CWndReSkillWarning::~CWndReSkillWarning() 
{ 
	Destroy();	
} 
void CWndReSkillWarning::InitItem( BOOL bParentDestroy )
{
	m_bParentDestroy = bParentDestroy;
}
void CWndReSkillWarning::OnDestroy()
{
	if( m_bParentDestroy )
	{
		CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL3 );
		pWndBase1->Destroy();
	}
}
void CWndReSkillWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( _T( prj.GetText( TID_GAME_SKILLLEVEL_CHANGE ) ) );
		pWndEdit->EnableWindow( FALSE );	
	}

	m_bParentDestroy = FALSE;

	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
} 
BOOL CWndReSkillWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_QUEITMWARNING, pWndParent, WBS_MODAL, CPoint( 0, 0 ) );
} 

BOOL CWndReSkillWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
		if( pSkillTree )
			g_DPlay.SendDoUseSkillPoint( pSkillTree->GetSkill() );

		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


BOOL CWndSkillTreeEx::IsReSkillPointDone()
{
	if( m_apSkill == NULL )
		return FALSE;
	
	if( m_nCurrSkillPoint < g_pPlayer->m_nSkillPoint )
		return TRUE;

	return FALSE;	
}

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

	if(GetWndBase( APP_BAG_EX )) GetWndBase( APP_BAG_EX )->Destroy();

	m_PostTabSend.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_SEND );
	m_PostTabReceive.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_RECEIVE );

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = prj.GetText(TID_APP_POST_SEND);
	tabTabItem.pWndBase = &m_PostTabSend;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_APP_POST_RECEIVE);
	tabTabItem.pWndBase = &m_PostTabReceive;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

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

		if( pWndTabCtrl->GetCurSel() == 0 )
		{
		}
		else
		{
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

void CWndGuildCombatBoard::SetString( CHAR* szChar )
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

void CGuildCombatInfoMessageBox::SetString( CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}

void CGuildCombatInfoMessageBox::SetString( CString strMsg )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( strMsg  );
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




// ���? �Ĺ� ������ ����
CWndGuildCombatSelection::CWndGuildCombatSelection() 
{
	m_vecGuildList.clear();
	m_mapSelectPlayer.clear();
	m_vecSelectPlayer.clear();
	m_uidDefender = -1;	

	nMaxJoinMember = 0;
	nMaxWarMember  = 0;
}

void CWndGuildCombatSelection::SetDefender( u_long uiPlayer ) 
{
	m_uidDefender = uiPlayer;
}

void CWndGuildCombatSelection::AddCombatPlayer( u_long uiPlayer ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	m_vecSelectPlayer.push_back( uiPlayer );

	CGuild* pGuild = g_pPlayer->GetGuild();
	
	const auto i = pGuild->m_mapPMember.find( uiPlayer );
	CGuildMember* pMember = i->second;
				
	CString str;
	PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
	str.Format( "Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );
	pWndList->AddString( str );			
} 

void CWndGuildCombatSelection::SetMemberSize( int nMaxJoin,  int nMaxWar ) 
{ 
	nMaxJoinMember = nMaxJoin; 
	nMaxWarMember  = nMaxWar; 

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	
	CString str;
	str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_OFFER_INFO), nMaxWarMember, nMaxJoinMember );
	pWndStatic->SetTitle( str );	
}


void CWndGuildCombatSelection::AddGuildPlayer( u_long uiPlayer ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	m_vecGuildList.push_back( uiPlayer );
	
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	const auto i = pGuild->m_mapPMember.find( uiPlayer );
	CGuildMember* pMember = i->second;
				
	CString str;
	PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
	str.Format( "Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );
	pWndList->AddString( str );			
} 

void CWndGuildCombatSelection::RemoveGuildPlayer( int nIndex ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
	pWndList->DeleteString( nIndex );
	m_vecGuildList.erase( m_vecGuildList.begin() + nIndex );
} 

void CWndGuildCombatSelection::RemoveCombatPlayer( int nIndex ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	
	pWndList->DeleteString( nIndex );

	if( m_vecSelectPlayer[nIndex] == m_uidDefender )
		SetDefender( -1 );

	m_vecSelectPlayer.erase( m_vecSelectPlayer.begin() + nIndex );	
} 


void CWndGuildCombatSelection::UpDateGuildListBox() 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	if( pWndList )
	{
		pWndList->ResetContent();
		
		m_vecGuildList.clear();
		m_mapSelectPlayer.clear();

		CGuild* pGuild = g_pPlayer->GetGuild();
		if( pGuild )
		{
			// �������� ����
			CGuildMember* pMember;
			for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
				if( pPlayerData->data.uLogin > 0 )
					m_mapSelectPlayer.emplace( pPlayerData->data.nLevel, pMember);
			}

			// ����Ʈ�� �߰�			
			CString str;
			for( auto j = m_mapSelectPlayer.begin(); j != m_mapSelectPlayer.end(); ++j )
			{
				pMember		= j->second;		
				PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
				if( pPlayerData->data.uLogin > 0 )
				{
					str.Format( "Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );
					pWndList->AddString( str );	
					m_vecGuildList.push_back( pMember->m_idPlayer );
				}
			}
		}
	}
}

u_long CWndGuildCombatSelection::FindCombatPlayer(u_long uiPlayer)
{
	for( int i = 0; i < (int)( m_vecSelectPlayer.size() ); i++ )
	{
		if( m_vecSelectPlayer[i] == uiPlayer )
			return m_vecSelectPlayer[i];
	}
	
	return -1;
}

u_long CWndGuildCombatSelection::FindGuildPlayer(u_long uiPlayer)
{
	for( int i = 0; i < (int)( m_vecGuildList.size() ); i++ )
	{
		if( m_vecGuildList[i] == uiPlayer )
			return m_vecGuildList[i];
	}
	
	return -1;
}

void CWndGuildCombatSelection::OnDraw( C2DRender* p2DRender ) 
{
	CWndButton* pWndStatic = (CWndButton*)GetDlgItem( WIDC_BUTTON8 );

	if( pWndStatic )
	{
		pWndStatic->EnableWindow( FALSE );

		if( m_uidDefender <= 0 )
		{
			pWndStatic->SetVisible( FALSE );
		}
		else
		{
			pWndStatic->SetVisible( TRUE );

			int i = NULL;
			for( ; i<(int)( m_vecSelectPlayer.size() ); i++ )
			{
				if( m_uidDefender == m_vecSelectPlayer[i] )
					break;
			}

			pWndStatic->Move( 330, ( i * 17 ) + 75 );			
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

void CWndGuildCombatSelection::EnableFinish( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_FINISH );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );	
	}
}

void CWndGuildCombatSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// �ð� ���������� �Ǵ�
	if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP) ); //������ �����ۼ��� �� �� �����ϴ�.
		Destroy();
		return;
	}
	
	MoveParentCenter();

	UpDateGuildListBox();
} 

BOOL CWndGuildCombatSelection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_GUILDCOMBAT_SELECTION, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndGuildCombatSelection::Reset()
{
	m_uidDefender = 0;
	UpDateGuildListBox();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	pWndListBox->ResetContent();
	m_vecSelectPlayer.clear();	
}

BOOL CWndGuildCombatSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	// ������ ���?
	if( nID == WIDC_BUTTON1 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		if( nMaxJoinMember < (int)( m_vecSelectPlayer.size() ) )
		{
			CString str;
			str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_SELECTION_MAX), nMaxJoinMember );
			g_WndMng.OpenMessageBox( str );
			return FALSE;
		}

		CGuild *pGuild = g_pPlayer->GetGuild();

		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( m_vecGuildList[nCurSel] );

			if( pGuildMember )
			{
				if( CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer )->data.nLevel < 30 )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_LEVEL_NOTICE) ); //������ �����? ���� 30�̻��� �Ǿ��? �մϴ�.
					return FALSE;
				}
			}	
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_MEMBER) );	//�������� �����ϴ� �����? �ɹ��� �ƴմϴ�.			
				return FALSE;
			}
		}

		u_long uiPlayer;
		uiPlayer = FindCombatPlayer( m_vecGuildList[nCurSel] );

		if( uiPlayer != -1 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_ALREADY_ENTRY) ); //�̹� ��ϵǾ�? �ֽ��ϴ�. �ٽ� ������ּ���?.
			return FALSE;
		}
 
		AddCombatPlayer( m_vecGuildList[nCurSel] );		
		RemoveGuildPlayer( nCurSel );		
	}
	else
	// ������ ���?
	if( nID == WIDC_BUTTON2 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		u_long uiPlayer;
		uiPlayer = FindGuildPlayer( m_vecSelectPlayer[nCurSel] );
		
		if( uiPlayer == -1 )
		{
			// ���?��Ʈ�� ���ٸ� �߰� 
			AddGuildPlayer( m_vecSelectPlayer[nCurSel] );		
			RemoveCombatPlayer( nCurSel );		
		}
		else
		{
			RemoveCombatPlayer( nCurSel );		
		}		
	}
	else
	// ������ ���� ����
	if( nID == WIDC_BUTTON3 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 || nCurSel == 0 )
			return FALSE;
		
		CString temp1, temp2;
		pWndListBox->GetText( nCurSel-1, temp1 );
		pWndListBox->GetText( nCurSel,   temp2 );

		pWndListBox->SetString( nCurSel-1, temp2 );
		pWndListBox->SetString( nCurSel, temp1 );
		
		u_long uiTemp = 0;

		uiTemp						 = m_vecSelectPlayer[nCurSel];
		m_vecSelectPlayer[nCurSel]   = m_vecSelectPlayer[nCurSel-1];
		m_vecSelectPlayer[nCurSel-1] = uiTemp;

		m_nDefenderIndex = nCurSel-1;
	}
	else
	// ������ ���� �Ʒ�
	if( nID == WIDC_BUTTON4 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 || nCurSel == pWndListBox->GetCount()-1 )
			return FALSE;
		
		CString temp1, temp2;
		pWndListBox->GetText( nCurSel, temp1 );
		pWndListBox->GetText( nCurSel+1,   temp2 );
		
		pWndListBox->SetString( nCurSel, temp2 );
		pWndListBox->SetString( nCurSel+1, temp1 );
		
		u_long uiTemp = 0;
		
		uiTemp						 = m_vecSelectPlayer[nCurSel];
		m_vecSelectPlayer[nCurSel]   = m_vecSelectPlayer[nCurSel+1];
		m_vecSelectPlayer[nCurSel+1] = uiTemp;

		m_nDefenderIndex = nCurSel+1;
	}
	else
	if( nID == WIDC_RESET )
	{
		CGuildCombatSelectionClearMessageBox* pBox = new CGuildCombatSelectionClearMessageBox;
		g_WndMng.OpenCustomBox( "", pBox );
	}
	else
	if( nID == WIDC_FINISH )
	{
#ifndef _DEBUG
		if( m_uidDefender == -1 || m_uidDefender == 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_ASSIGN_DEFENDER) ); //�������? �������� �ʾҽ��ϴ�. ��帶���͸�? ������ 1���� �������? ������ �ּ���.
			return FALSE;
		}
#endif //_DEBUG

		// �ð� ���������� �Ǵ�
		if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP) ); //������ �����ۼ��� �� �� �����ϴ�.
			Destroy();
			return FALSE;
		}
		
		if( m_vecSelectPlayer.size() == 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_PLAYER) ); //�����ڰ� �����ϴ�. �����ڸ� �������ּ���.
			return FALSE;
		}
		
		CGuild *pGuild = g_pPlayer->GetGuild();
		CGuildMember* pGuildMemberl;
		
		if( pGuild )
		{
			BOOL bSkip = FALSE;

			// ������ �ɹ��߿� �����Ͱ� ŷ���� �ִ��� �˻縦�Ѵ�.
			// ���� �ϳ��� ������ ���� �Ұ���...
			for( int i=0; i<(int)( m_vecSelectPlayer.size() ); i++ )
			{
				pGuildMemberl = pGuild->GetMember( m_vecSelectPlayer[i] );

				if( pGuildMemberl )
				{
					if( pGuildMemberl->m_nMemberLv == GUD_MASTER || pGuildMemberl->m_nMemberLv == GUD_KINGPIN )
					{
						bSkip = TRUE;
						break;
					}
				}
			}

			if( bSkip )
			{
				g_DPlay.SendGCSelectPlayer( m_vecSelectPlayer, m_uidDefender );
				Destroy();
			}
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_MASTER) ); //������ ���ܿ� ��帶���ͳ�? ŷ���� �������� �ʽ��ϴ�.
				return FALSE;
			}
		}
	}
	else
	// �����? ����
	if( nID == WIDC_BUTTON7 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		CGuild *pGuild = g_pPlayer->GetGuild();

		if( pGuild )
		{
			if( m_vecSelectPlayer.size() > 1 &&  pGuild->IsMaster( m_vecSelectPlayer[nCurSel] ) )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_MASTER_NOT_ASSIGN_DEFENDER) ); //��帶���ʹ�? �������? �� �� �����ϴ�.
				return FALSE;
			}
		}
		
		m_nDefenderIndex = nCurSel;
		SetDefender( m_vecSelectPlayer[nCurSel] );
	}
	else
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
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
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	pWndTabCtrl->SetButtonLength( 100 );

	m_WndGuildCombatTabResultRate.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT );
	m_WndGuildCombatTabResultLog.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT_LOG );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultRate;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );


	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG2);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultLog;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();

	{
//		CRect r = GetWindowRect( TRUE );
//		m_WndGuildTabMember.m_rectParent = r;
	}
	
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
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	pWndTabCtrl->SetButtonLength( 100 );

	m_WndGuildCombatTabClass_Tot.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mer.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mag.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Acr.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Ass.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Tot;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 1 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mer;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 4 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mag;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );
	
	tabTabItem.pszText = prj.m_aJob[ 2 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Acr;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 3 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Ass;
	pWndTabCtrl->InsertItem( 4, &tabTabItem );


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

CWndGuildCombatRank_Class* CWndGuildCombatRank_Person::__GetJobKindWnd(int nJob)
{
	switch(nJob) 
	{
	case JOB_MERCENARY:
	case JOB_KNIGHT:
	case JOB_BLADE:
 	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_BLADE_HERO:
		return &m_WndGuildCombatTabClass_Mer;
		break;
		
	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
		return &m_WndGuildCombatTabClass_Acr;
		break;
		
	case JOB_ASSIST:
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_RINGMASTER_HERO:
	case JOB_BILLPOSTER_HERO:
		return &m_WndGuildCombatTabClass_Ass;
		break;
		
	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
	case JOB_ELEMENTOR_HERO:
		return &m_WndGuildCombatTabClass_Mag;
		break;
		
	default:
		break;
	}	

	return NULL;
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
	m_listRank[m_nMax].strJob     = prj.m_aJob[ nJob ].szName;	
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
//				m_pWndText->BlockInsert( "#b", "#nb" );
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_BOLD);
//				m_pWndText->BlockDelete( "#b", "#nb" );
			}
			break;
		case WIDC_CHECK2:
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);			

			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_UNDERLINE);
//				m_pWndText->BlockInsert( "#u", "#nu" );				
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_UNDERLINE);
//				m_pWndText->BlockDelete( "#u", "#nu" );				
			}
			break;
		case WIDC_CHECK3:			
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);	
			
			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_STRIKETHROUGH);
//				m_pWndText->BlockInsert( "#s", "#ns" );				
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_STRIKETHROUGH);
//				m_pWndText->BlockDelete( "#s", "#ns" );				
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
//				m_pWndText->BlockInsert( str, "#nc" );				
			}
			else
			{
				m_pWndText->BlockSetColor( 0xff000000 );
//				m_pWndText->BlockDelete( "#c", "#nc" );				
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


CWndBuffStatus::CWndBuffStatus() 
{ 
	m_BuffIconViewOpt = g_Option.m_BuffStatusMode;
} 

CWndBuffStatus::~CWndBuffStatus() 
{ 
} 

void CWndBuffStatus::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	this->DelWndStyle(WBS_CAPTION);
	
	m_wndTitleBar.SetVisible( FALSE );

	SetBuffIconInfo();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( (rectRoot.right - rectRoot.left) / 3, (rectRoot.bottom - rectRoot.top) / 3);
	Move( point );
} 
BOOL CWndBuffStatus::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_BUFF_STATUS, pWndParent, WBS_NOFOCUS, CPoint( 0, 0 ) );
} 

BOOL CWndBuffStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

BOOL CWndBuffStatus::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndBuffStatus::OnLButtonUp( UINT nFlags, CPoint point )
{
	this->m_pWndFocus = this;
}

void CWndBuffStatus::OnLButtonDown( UINT nFlags, CPoint point )
{
	if(!GetHitTestResult())
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
			this->m_pWndFocus = g_WndMng.m_pWndWorld;
		}
	}
}

void CWndBuffStatus::OnRButtonUp( UINT nFlags, CPoint point )
{
}

void CWndBuffStatus::OnRButtonDown( UINT nFlags, CPoint point )
{
	if(!GetHitTestResult())
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, p );
		}
	}
}

void CWndBuffStatus::OnMouseWndSurface( CPoint point )
{
}

void CWndBuffStatus::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(GetHitTestResult())
	{
		if(m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if(m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDblClk( nFlags, p );
		}
	}
}

void CWndBuffStatus::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	if(GetHitTestResult())
	{
		if(m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if(m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDblClk( nFlags, p );
		}
	}
}

BOOL CWndBuffStatus::GetHitTestResult()
{
#ifdef __BUFF_1107
	BOOL rtn_val = FALSE;
	RECT rectHittest;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;
	
	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for( MAPBUFF::iterator i = g_pPlayer->m_buffs.m_mapBuffs.begin(); i!= g_pPlayer->m_buffs.m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if( pBuff->GetType() == BUFF_SKILL )
		{
			buffinfo  = *it;
			SetRect( &rectHittest, buffinfo.pt.x, buffinfo.pt.y, buffinfo.pt.x+34, buffinfo.pt.y+34 );
			ClientToScreen( &rectHittest );
			if( PtInRect(&rectHittest, ptMouse ) )
				rtn_val = TRUE;
			it++;
		}
	}
	return rtn_val;
#else	// __BUFF_1107
//{{AFX
	BOOL rtn_val = FALSE;
	RECT rectHittest;
	int i;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;
	
	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for(i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		SKILLINFLUENCE* pSkill = g_pPlayer->m_SkillState.Get(i);
		if(pSkill->wID)	
		{
			if(pSkill->wType == BUFF_SKILL)
			{
				buffinfo  = *it;
				SetRect( &rectHittest, buffinfo.pt.x, buffinfo.pt.y, buffinfo.pt.x+34, buffinfo.pt.y+34 );
				ClientToScreen( &rectHittest );
				if( PtInRect(&rectHittest, ptMouse ) )
				{
					rtn_val = TRUE;
				}
				it++;
			}
		}
	}
	return rtn_val;
//}}AFX
#endif	// __BUFF_1107
}

void CWndBuffStatus::SetBuffIconInfo()
{
	BUFFICON_INFO buffinfo;
	int x = 0;
	int y = 0;
	int i;
	CRect rect;
	rect = GetWindowRect(TRUE);
	
	if(!m_pBuffIconInfo.empty())
		m_pBuffIconInfo.clear();

	if(m_BuffIconViewOpt == 0)
	{
		for(i=0; i<MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint( x, y );
			m_pBuffIconInfo.push_back(buffinfo);
			x += 34;
			if(((i+1) % 7) == 0)
			{
				x  = 0;
				y += 34;
			}
		}
		//widht 238, heigth = 68
		rect.bottom = 136 + rect.top;
		rect.right = 238 + rect.left;
	}
	else if(m_BuffIconViewOpt == 1)
	{
		for(i=0; i<MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint( x, y );
			m_pBuffIconInfo.push_back(buffinfo);
			y += 34;
			if(((i+1) % 7) == 0)
			{
				y  = 0;
				x += 34;
			}
		}
		//widht 54, heigth = 238
		rect.bottom = 238 + rect.top;
		rect.right = 136 + rect.left;
	}
	SetWndRect(rect);
	AdjustWndBase();
}

void CWndBuffStatus::PaintFrame( C2DRender* p2DRender )
{
}

void CWndBuffStatus::OnDraw( C2DRender* p2DRender )
{	
	if( g_pPlayer == NULL )
		return;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;

	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
#ifdef __BUFF_1107
	for( MAPBUFF::iterator i = g_pPlayer->m_buffs.m_mapBuffs.begin(); i != g_pPlayer->m_buffs.m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if( pBuff->GetType() == BUFF_SKILL )
		{
			buffinfo  = *it;
			RenderBuffIcon( p2DRender, pBuff, TRUE, &buffinfo, ptMouse );
			it++;
		}
	}
#else	// __BUFF_1107
//{{AFX
	int i;
	for(i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		SKILLINFLUENCE* pSkill = g_pPlayer->m_SkillState.Get(i);
		if(pSkill->wID)	
		{
			if(pSkill->wType == BUFF_SKILL)
			{
				buffinfo  = *it;
				RenderBuffIcon( p2DRender, pSkill, TRUE, &buffinfo, ptMouse );
				it++;
			}
		}
	}
//}}AFX
#endif	// __BUFF_1107
}

void CWndBuffStatus::RenderBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse )
{
	int nTexture;
	RECT rectHittest;	
	std::multimap< DWORD, BUFFSKILL >::iterator iter;
	
	if( bPlayer )
		nTexture = 0;
	else
		nTexture = 1;
	
	ItemProp* pItem = NULL;

#ifdef __BUFF_1107
	WORD wID = pBuff->GetId();
	if( pBuff->GetType() == BUFF_SKILL )
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].find( pBuff->GetId() );

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].end() )
			return;

		if( bPlayer )
			pItem = prj.GetSkillProp( pBuff->GetId() );
		else
			pItem = prj.GetPartySkill( pBuff->GetId() );
	}
	else
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].find( pBuff->GetId() );

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].end() )
			return;

		pItem = prj.GetItemProp( wID );
	}
	
	std::multimap< DWORD, BUFFSKILL >::value_type* pp = &(*iter);

	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;
	
	BOOL bFlash = FALSE;
	DWORD dwOddTime = 0;
	
	if( pBuff->GetTotal() > 0 )
	{	
		dwOddTime = pBuff->GetTotal() - ( g_tmCurrent - pBuff->GetInst() );
		bFlash = ( dwOddTime < 20 * 1000 );		// 20�� ���� �������� �����Ÿ�
		
		if( pBuff->GetTotal() < ( g_tmCurrent - pBuff->GetInst() ) ) // - �� �Ǹ� 0���� ó��
			dwOddTime = 0;
	}
	
	D3DXCOLOR color;
	
	if( bFlash )		
	{		
		if( pp->second.m_bFlsh == TRUE )
		{
			pp->second.m_nAlpha+=6;
			
			if( pp->second.m_nAlpha > 192 )
			{
				pp->second.m_nAlpha = 192;
				pp->second.m_bFlsh = FALSE;
			}
		}
		else
		{
			pp->second.m_nAlpha-=6;
			
			if( pp->second.m_nAlpha < 64 )
			{
				pp->second.m_nAlpha = 64;
				pp->second.m_bFlsh = TRUE;
			}
		}
		
		if( pItem->nEvildoing < 0 )							// ���۸�����
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 120, 255 );		// ���� �� 
		else
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );		
	}
	else
	{
		if( pItem->nEvildoing < 0 )							// ���۸�����
			color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );		// ���� �� 
		else
			color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );
	}

	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+28, pInfo->pt.y+28 );
	ClientToScreen( &rectHittest );
	
	CEditString strEdit;
	CString strLevel;
	strLevel.Format("   Lvl %d", pBuff->GetLevel() );

	if( pItem->dwItemRare == 102 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
	}
	else if( pItem->dwItemRare == 103 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
	}
	else
	{
		strEdit.AddString( pItem->szName, 0xff2fbe6d, ESSTY_BOLD );
		strEdit.AddString( strLevel, 0xff2fbe6d, ESSTY_BOLD );
	}

	CString str;

	if( pBuff->GetTotal() > 0 )
	{
		CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// �����ð��� �ʴ����� ��ȯ�ؼ� �Ѱ���
				
		if( ct.GetDays() != 0 )
		{
			str.Format( "\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//�ú��� 
		}
		else
		{
			if( ct.GetHours() >= 1 )
				str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//�ú��� 
			else
				str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );						// ����
		}
		RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 255, 255, 255 ) );
	}

	CString strTemp;
	strTemp.Format( "\n%s", pItem->szCommand );

	strEdit.AddString( strTemp );

	if( PtInRect(&rectHittest, ptMouse ) )
	{
		g_WndMng.PutDestParam( pItem->dwDestParam[0], pItem->dwDestParam[1],
			pItem->nAdjParamVal[0], pItem->nAdjParamVal[1], strEdit );

		if( pBuff->GetType() == BUFF_SKILL && 
			pBuff->GetId() != SI_RIG_MASTER_BLESSING && 
			pBuff->GetId() != SI_ASS_CHEER_STONEHAND && 
			pBuff->GetId() != SI_MAG_EARTH_LOOTING && 
			pBuff->GetId() != SI_ASS_HEAL_PREVENTION ) //������ �ູ, ���� �ڵ�, ����, �������� ����
		{
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pItem->dwSubDefine, pBuff->GetLevel() );

			if( pAddSkillProp )
			{
				g_WndMng.PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}
	}
#else	// __BUFF_1107
//{{AFX
	WORD wID = pSkill->wID;
	if( pSkill->wType == BUFF_SKILL )
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].find(pSkill->wID);

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].end() )
			return;

		if( bPlayer )
			pItem = prj.GetSkillProp( pSkill->wID );
		else
			pItem = prj.GetPartySkill( pSkill->wID );
	}
	else
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].find(pSkill->wID);

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].end() )
			return;

		pItem = prj.GetItemProp( wID );
	}
	
	multimap< DWORD, BUFFSKILL >::value_type* pp;
	pp = &(*iter);

	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;
	
	BOOL bFlash = FALSE;
	DWORD dwOddTime = 0;
	
	if( pSkill->tmCount > 0 )
	{	
		dwOddTime = pSkill->tmCount - (g_tmCurrent - pSkill->tmTime);
		bFlash = ( dwOddTime < 20 * 1000 );		// 20�� ���� �������� �����Ÿ�
		
		if(pSkill->tmCount < (g_tmCurrent - pSkill->tmTime)) // - �� �Ǹ� 0���� ó��
			dwOddTime = 0;
	}
	
	D3DXCOLOR color;
	
	if( bFlash )		
	{		
		if( pp->second.m_bFlsh == TRUE )
		{
			pp->second.m_nAlpha+=6;
			
			if( pp->second.m_nAlpha > 192 )
			{
				pp->second.m_nAlpha = 192;
				pp->second.m_bFlsh = FALSE;
			}
		}
		else
		{
			pp->second.m_nAlpha-=6;
			
			if( pp->second.m_nAlpha < 64 )
			{
				pp->second.m_nAlpha = 64;
				pp->second.m_bFlsh = TRUE;
			}
		}
		
		if( pItem->nEvildoing < 0 )							// ���۸�����
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 120, 255 );		// ���� �� 
		else
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );		
	}
	else
	{
		if( pItem->nEvildoing < 0 )							// ���۸�����
			color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );		// ���� �� 
		else
			color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );
	}

	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+28, pInfo->pt.y+28 );
	ClientToScreen( &rectHittest );
	
	CEditString strEdit;
	CString strLevel;
	strLevel.Format("   Lv %d", pSkill->dwLevel);

	if( pItem->dwItemRare == 102 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
	}
	else if( pItem->dwItemRare == 103 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
	}
	else
	{
		strEdit.AddString( pItem->szName, 0xff2fbe6d, ESSTY_BOLD );
		strEdit.AddString( strLevel, 0xff2fbe6d, ESSTY_BOLD );
	}

	CString str;

	if( pSkill->tmCount > 0 )
	{
		CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// �����ð��� �ʴ����� ��ȯ�ؼ� �Ѱ���
				
		if( ct.GetDays() != 0 )
		{
			str.Format( "\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//�ú��� 
		}
		else
		{
			if( ct.GetHours() >= 1 )
				str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//�ú��� 
			else
				str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );						// ����
		}
		RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 255, 255, 255 ) );
	}

	CString strTemp;
	strTemp.Format( "\n%s", pItem->szCommand );

	strEdit.AddString( strTemp );

	if( PtInRect(&rectHittest, ptMouse ) )
	{
		g_WndMng.PutDestParam( pItem->dwDestParam[0], pItem->dwDestParam[1],
			pItem->nAdjParamVal[0], pItem->nAdjParamVal[1], strEdit );
		
		if( pSkill->wType == BUFF_SKILL && pSkill->wID != SI_RIG_MASTER_BLESSING && 
			pSkill->wID != SI_ASS_CHEER_STONEHAND && pSkill->wID != SI_MAG_EARTH_LOOTING ) //������ �ູ, ���� �ڵ�, ���� ����
		{
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pItem->dwSubDefine, pSkill->dwLevel );

			if( pAddSkillProp )
			{
				g_WndMng.PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}
	}
//}}AFX
#endif	// __BUFF_1107

	strEdit.AddString( str );

	g_toolTip.PutToolTip( wID, strEdit, rectHittest, ptMouse, 1);

	++pInfo->nCount;
}

void CWndBuffStatus::RenderOptBuffTime(C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor )
{
	if(g_Option.m_bVisibleBuffTimeRender)
	{
		CString str;
		int seconds = (int)(ct.GetTotalSeconds());
		str.Format( "%d" , seconds );
		p2DRender->TextOut(point.x+2, point.y+22, str, dwColor, 0xFF000000);
	}
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
			
			g_DPlay.SendUltimateTransWeapon(w->m_dwObjId, o->m_dwObjId, j->m_dwObjId);
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
	m_pText = NULL;
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
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "ChangeAttribute.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

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

	CRect rect;
	ItemProp* pItemProp;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
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
			pWndStatic->SetTitle(prj.m_aJob[ pData->data.nJob ].szName);
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

				if( prj.m_aJob[ pData->data.nJob ].dwJobType == JTYPE_MASTER )
				{
					int nMasterIndex = 27;
					if(/*m_nLevel >= 60 && */pData->data.nLevel < 70) //Level Down�� ���? �����ؼ� �ּ�ó��.
						nMasterIndex = 27;
					else if(pData->data.nLevel >= 70 && pData->data.nLevel < 80)
						nMasterIndex = 28;
					else if(pData->data.nLevel >= 80 && pData->data.nLevel < 90)
						nMasterIndex = 29;
					else if(pData->data.nLevel >= 90 && pData->data.nLevel < 100)
						nMasterIndex = 30;
					else if(pData->data.nLevel >= 100 && pData->data.nLevel < 110)
						nMasterIndex = 31;
					else if(pData->data.nLevel >= 110 && pData->data.nLevel <= 120)
						nMasterIndex = 32;

					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, ptJobType,  nMasterIndex, &pVertices, 0xffffffff );
				}
				else if( prj.m_aJob[ pData->data.nJob ].dwJobType == JTYPE_HERO )
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, ptJobType,  33, &pVertices, 0xffffffff );

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
CWndCoupleTabSkill::CWndCoupleTabSkill()
{
	m_pText = NULL;
	m_pSkillBgTexture = NULL;
}

CWndCoupleTabSkill::~CWndCoupleTabSkill()
{
}

void CWndCoupleTabSkill::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "CoupleSkillInfo.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

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
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_INFO);
	tabTabItem.pWndBase = &m_wndCoupleTabInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_SKILL);
	tabTabItem.pWndBase = &m_wndCoupleTabSkill;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

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
	if( CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
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
				nMaxSmeltNumber = min(nMaxSmeltNumber, GetDefaultMaxSmeltValue());
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
