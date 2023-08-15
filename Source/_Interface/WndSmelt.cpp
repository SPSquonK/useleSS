#include "StdAfx.h"
#include "WndSmelt.h"

// Libraries
#include "sqktd/util.hpp"

// Defines
#include "defineText.h"
#include "AppDefine.h"

// Network
#include "MsgHdr.h"
#include "DPClient.h"


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
BOOL CWndRandomScrollConfirm::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_RANDOMSCROLL_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndRandomScrollConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		g_DPlay.SendRandomScroll( objid, objid1 );
	}
	Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewelConfirm Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndMixJewelConfirm::OnDestroy() {
	if (CWndMixJewel * pWndMixJewel = GetWndBase<CWndMixJewel>(APP_SMELT_MIXJEWEL)) {
		pWndMixJewel->SetConfirmInit();
	}
}

void CWndMixJewelConfirm::OnDraw(C2DRender * p2DRender) {
	const ItemProp * pItemProp = prj.GetItemProp(II_GEN_MAT_ORICHALCUM02);
	if (!pItemProp) return;

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	CTexture * pTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
	if (pTexture) {
		pTexture->Render(p2DRender, wndCtrl->rect.TopLeft());
	}
}

void CWndMixJewelConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndText * pText = GetDlgItem<CWndText>(WIDC_TEXT1);
	pText->SetString(prj.GetText(TID_GAME_MIXJEWEL_SUCCESS));

	MoveParentCenter();
}

BOOL CWndMixJewelConfirm::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_SMELT_MIXJEWEL_CONFIRM, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndMixJewelConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_OK)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
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
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	m_pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(m_pWndInventory != NULL)
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
	
	MoveParentCenter();
}

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndMixJewel::Initialize( CWndBase* pWndParent )
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
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("SmeltExtraction.inc"));

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndExtraction::Initialize( CWndBase* pWndParent )
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
	
	m_nStatus = Status::Base;
	m_nCount = 0;
	m_nDelay = 25;
	m_nAlpha = 0;
	m_nEyeYPos = 0.0f;
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
	if(m_pItemElem)
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

	CWndButton* pButton = GetDlgItem<CWndButton>(WIDC_START);
	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("SmeltJewel.inc"));

  m_disableTextures.blue  = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, "WndDisableBlue.bmp" ), 0xffff00ff);
  m_disableTextures.black = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, "WndDisableBlack.bmp"), 0xffff00ff);
  m_disableTextures.red   = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, "WndDisableRed.bmp"  ), 0xffff00ff);

	MoveParentCenter();
} 

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndSmeltJewel::Initialize( CWndBase* pWndParent )
{ 
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_JEWEL, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSmeltJewel::Process()
{
  if (m_pItemElem) {
    CWndBase * pButton = GetDlgItem(WIDC_START);

    const bool canStart =
      m_nStatus == Status::Base
      && (m_nUsableSlot >= 0 && m_nUsableSlot < 5)
      && m_dwJewel[m_nUsableSlot] != -1;

    pButton->EnableWindow(canStart ? TRUE : FALSE);
  }

		if(m_nStatus == Status::Smelting) //Start��ư ���� ���? ��ǻ���� ������ ȸ���ϵ��� ��.
		{
			if(m_nCount > m_nDelay)
			{
				m_fAddRot += 4.0f;
				
				if(m_nDelay < 10)
				{
					m_nEyeYPos -= 0.4f;
					m_nAlpha += 18;
          m_nAlpha = std::min(m_nAlpha, 230);
				}

				m_nDelay -= 1;				
				if(m_nDelay < 0)
				{
					m_nStatus = Status::Base;
					m_nDelay = 25;
					m_fRotate = 0.0f;
					m_fAddRot = 2.0f;
					m_nAlpha = 0;
					m_nEyeYPos = 0.0f;
					
					if(m_pItemElem && m_objJewelId != -1)
						g_DPlay.SendUltimateSetGem(m_pItemElem->m_dwObjId, m_objJewelId);
				}
				
				m_nCount = 0;
			}
			m_nCount++;
		}

	return TRUE;
}

void CWndSmeltJewel::OnDraw( C2DRender* p2DRender ) {
  if (!m_pItemElem) return;

	WndFieldUtil::ResetRenderState();

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255,255,255,255 ) );
	
	// �ʱ�ȭ
	
	const D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	// ���⿡ ���� Camera Angle����
  auto [vEyePt, vLookatPt] = WeaponCameraAngle(m_pItemElem->GetProp());

	if(m_nStatus == Status::Smelting) //Start�� �̹��� ȸ�� �� ������ ���� ����.
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
  D3DXMATRIXA16 matRot; D3DXMatrixRotationX(&matRot, D3DXToRadian( m_fRotate ) );
	
	// Scaling
  D3DXMATRIXA16 matScale; D3DXMatrixScaling(&matScale, 1.5f, 1.5f, 1.5f);

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	// ������ ��ġ ����

  // ����Ʈ ���� 
  D3DVIEWPORT9 viewport = WndFieldUtil::BuildViewport(p2DRender, lpFace);


	pd3dDevice->SetViewport(&viewport);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 );

	// Matrix Multiply
  const D3DXMATRIXA16 matWorld = matScale * matRot;
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
	
	m_pMainItem->Render( &matWorld );

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

		for(int i=0; i<5; i++)
		{
			LPWNDCTRL pWndCtrl = GetWndCtrl( m_nJewelSlot[i] );
			if( i < m_pItemElem->GetUltimatePiercingSize() ) //�ո� ����
			{
				if(m_dwJewel[i] != -1) //���� ����
				{
					if(i != m_nUsableSlot) //���� ���� ���������� ��ĥ���� ����.
					{
            if (m_disableTextures.blue) {
              m_disableTextures.blue->Render(p2DRender, pWndCtrl->rect.TopLeft());
            }
					}

					const ItemProp * pItemProp = prj.GetItemProp( m_dwJewel[i] );
					if(pItemProp != NULL)
					{
            if (CTexture * pTexture = pItemProp->GetTexture()) {
              pTexture->Render(p2DRender, pWndCtrl->rect.TopLeft());
            }
					}
				}
				else
				{
					if(i != m_nUsableSlot) //�ո� ���� �� ������ ���� �� �ִ� ù��° ���Ը� ���� �������� ȸ������.
					{
            if (m_disableTextures.black) {
              m_disableTextures.black->Render(p2DRender, pWndCtrl->rect.TopLeft());
            }
					}
				}
			}
			else //�� �ո� ����
			{
        if (m_disableTextures.red) {
          m_disableTextures.red->Render(p2DRender, pWndCtrl->rect.TopLeft());
        }
			}
		}
	

  if (m_nStatus == Status::Smelting) {
    const DWORD color = D3DCOLOR_ARGB(m_nAlpha, 240, 255, 255);
    const CRect rect = GetWndCtrl(WIDC_CUSTOM1)->rect;
    p2DRender->RenderFillRect(rect, color);
  }
}

CWndSmeltJewel::WeaponCameraAngle::WeaponCameraAngle(const ItemProp * pProp) {
  vEyePt = D3DXVECTOR3(0.f, 0.f, 0.f);
  vLookatPt = D3DXVECTOR3(0.f, 0.f, 0.f);
  if (!pProp) return;

  if (pProp->dwItemKind3 == IK3_AXE || pProp->dwItemKind3 == IK3_SWD) {
    if (pProp->dwHanded == HD_ONE) {
      vEyePt    = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
      vLookatPt = D3DXVECTOR3(0.6f, -0.2f, 0.0f);
    } else if (pProp->dwHanded == HD_TWO) {
      vEyePt    = D3DXVECTOR3(1.0f, 5.0f, 0.0f);
      vLookatPt = D3DXVECTOR3(1.2f, -0.2f, 0.0f);
    }

  } else if (pProp->dwItemKind3 == IK3_YOYO ||
    pProp->dwItemKind3 == IK3_KNUCKLEHAMMER ||
    pProp->dwItemKind3 == IK3_BOW) {
    vEyePt    = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
    vLookatPt = D3DXVECTOR3(0.01f, -0.2f, 0.0f);

  } else if (pProp->dwItemKind3 == IK3_WAND) {
    vEyePt    = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
    vLookatPt = D3DXVECTOR3(0.4f, -0.2f, 0.0f);

  } else if (pProp->dwItemKind3 == IK3_CHEERSTICK || pProp->dwItemKind3 == IK3_STAFF) {
    vEyePt    = D3DXVECTOR3(0.0f, 4.0f, 0.0f);
    vLookatPt = D3DXVECTOR3(0.01f, -0.2f, 0.0f);
  }
}

BOOL CWndSmeltJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if(m_nStatus != Status::Base)
		return FALSE;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CUSTOM1 );		

  CItemElem * pItemElem = g_pPlayer->GetItemId(pShortcut->m_dwId);
  if (!pItemElem) return TRUE;

	//Set Weapon

	if((pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC) &&
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
			
			m_pMainItem = (CModelObject*)prj.m_modelMng.LoadModel( OT_ITEM, m_pItemElem->m_dwItemId );
			m_pMainItem->InitDeviceObjects( );
		}
	} 
	
	//SetJewel
	if(m_pItemElem && m_nUsableSlot != -1)
	{
		if(IsJewel(pItemElem->m_dwItemId))
		{
      const bool inRect = GetWndCtrl(m_nJewelSlot[m_nUsableSlot])->rect.PtInRect(point);;

			if(inRect && m_dwJewel[m_nUsableSlot] == -1)
			{
				m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				m_pJewelElem = pItemElem;
			}
		}
	}
	
	return TRUE;
}

bool CWndSmeltJewel::IsJewel(DWORD itemId) {
  return sqktd::is_among(itemId,
    II_GEN_MAT_RUBY, II_GEN_MAT_DIAMOND, II_GEN_MAT_EMERALD,
    II_GEN_MAT_SAPPHIRE, II_GEN_MAT_TOPAZ
  );
}

void CWndSmeltJewel::SetJewel(CItemElem * pItemElem) {
  if (!pItemElem) return;
  if (m_nStatus != Status::Base) return;
  if (!m_pItemElem) return;

  if (!IsJewel(pItemElem->m_dwItemId)) return;

  if (m_nUsableSlot == -1) return;
  if (m_dwJewel[m_nUsableSlot] != -1) return;

  m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
  m_objJewelId = pItemElem->m_dwObjId;
  pItemElem->SetExtra(pItemElem->GetExtra() + 1);
  m_pJewelElem = pItemElem;
}

void CWndSmeltJewel::OnLButtonDblClk(UINT nFlags, CPoint point) {
  if (m_nStatus != Status::Base) return;
  if (m_nUsableSlot == -1) return;

  LPWNDCTRL wndCtrl = GetWndCtrl(m_nJewelSlot[m_nUsableSlot]);
  if (wndCtrl && wndCtrl->rect.PtInRect(point)) {
    if (m_dwJewel[m_nUsableSlot] != -1) {
      if (m_pJewelElem)
        m_pJewelElem->SetExtra(0);
      m_dwJewel[m_nUsableSlot] = -1;
    }
  }
}

BOOL CWndSmeltJewel::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
  if (message == WNM_CLICKED) {
    if (nID == WIDC_START) {
      GetDlgItem<CWndButton>(WIDC_START)->EnableWindow(FALSE);
      m_nStatus = Status::Smelting;
      PLAYSND("PcSkillD-Counter01.wav");
    }
  }

  return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSmeltJewel::ReceiveResult(int result) {
  //���? ���� ���� ó��
  //1. ���� : �ʱ�ȭ
  //2. ���� : �ʱ�ȭ
  //3. ���? : ��Ÿ.

  switch (result) {
    case CUltimateWeapon::ULTIMATE_SUCCESS:
      g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SMELTJEWEL_SUCCESS));
      PLAYSND("InfUpgradeSuccess.wav");
      InitializeJewel(m_pItemElem);
      Destroy();
      break;
    case CUltimateWeapon::ULTIMATE_FAILED:
      g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SMELTJEWEL_FAIL));
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
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

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
BOOL CWndChangeWeapon::Initialize( CWndBase* pWndParent )
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

CWndRemoveJewelConfirm::CWndRemoveJewelConfirm() {
  m_pInventory = NULL;
  m_pUpgradeItem = NULL;
}

void CWndRemoveJewelConfirm::OnDestroy() {
  if (m_pInventory) {
    m_pInventory->m_pWndRemoveJewelConfirm = NULL;
    m_pInventory->m_bRemoveJewel = FALSE;
  }
}

void CWndRemoveJewelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// ���⿡ �ڵ��ϼ���
	CWndText* pText = GetDlgItem<CWndText>( WIDC_TEXT1 );
	pText->m_string.AddParsingString( prj.GetText( TID_GAME_REMOVEJEWEL ) );
	pText->ResetString();
	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndRemoveJewelConfirm::Initialize( CWndBase* pWndParent )
{
	m_pInventory = (CWndInventory*)pWndParent;
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_REMOVE_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndRemoveJewelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(m_pInventory)
		{
			// ��þƮ�� �Ǵ� ������ - ���? ���?
			m_pInventory->m_pUpgradeItem = m_pUpgradeItem;
			m_pInventory->m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
		}
		Destroy();
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		if(m_pInventory) m_pInventory->BaseMouseCursor();
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
BOOL CWndHeroSkillUp::Initialize( CWndBase* pWndParent )
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
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("RemoveAttribute.inc"));

	const WNDCTRL * const wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	m_receiver.Create(0, wndCtrl->rect, this, WIDC_PIC_SLOT + 1);

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndRemoveAttribute::Initialize( CWndBase* pWndParent ) 
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

BOOL CWndRemoveAttribute::CWndConfirm::Initialize(CWndBase * pWndParent) {
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
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	
	
	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("SmeltRemovePiercing.inc"));

	m_receiver.Create(0, GetWndCtrl(WIDC_PIC_SLOT)->rect, this, WIDC_Receiver);
	m_receiver.SetTooltipId(TID_GAME_TOOLTIP_PIERCINGITEM);

	MoveParentCenter();
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndRemovePiercing::Initialize( CWndBase* pWndParent )
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
// CWndRemoveJewel ClassCWndRemoveJewel
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
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

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

BOOL CWndRemoveJewel::Initialize( CWndBase* pWndParent )
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

BOOL CWndChangeAttribute::Initialize( CWndBase* pWndParent )
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
			m_pTexture->Render( p2DRender, GetWndCtrl( WIDC_STATIC1 )->rect.TopLeft() );
	}

	// Draw Attribute & Draw Rect Round Lamp
	for(int i=0; i<5; i++)
	{
		const CRect rect = GetWndCtrl( m_nAttributeStaticID[i] )->rect;
		g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, rect.TopLeft(), i, 255, 1.5f, 1.5f);
		
		if(i == m_nAttributeNum)
			FillRect( p2DRender, rect, 0xffbb00ff, 0x00bb00ff );
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

void CWndChangeAttribute::OnSetCursor()
{
	bool bOnTitle = false;
	const CPoint point = GetMousePoint();

	for(int i=0; i<5; i++) 
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( m_nAttributeStaticID[i] );
		CRect rect = lpWndCtrl->rect;
		if (rect.PtInRect(point)) {
			bOnTitle = true;
		}
	}

	if (bOnTitle) {
		SetMouseCursor(CUR_SELECT);
	} else {
		SetMouseCursor(CUR_BASE);
		CWndBase::OnSetCursor();
	}
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
				m_pTexture = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
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

CWndSmeltSafety::~CWndSmeltSafety() {
	for (GenLine & genLine : m_genLines) {
		genLine.OnDestruction(m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT);
	}

	if (m_pItemElem != NULL) {
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}

	if (CWndInventory * pWndInventory = GetWndBase<CWndInventory>(APP_INVENTORY)) {
		pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
	}
}

void CWndSmeltSafety::GENMATDIEINFO::OnDestruction() {
	if (!pItemElem) return;
	if (g_pPlayer->m_vtInfo.IsTrading(pItemElem)) return;
	pItemElem->SetExtra(0);
	pItemElem = nullptr;
}

void CWndSmeltSafety::GenLine::OnDestruction(bool destroyScroll2) {
	material.OnDestruction();
	scroll1.OnDestruction();
	if (destroyScroll2) scroll2.OnDestruction();
}

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndSmeltSafety::Initialize( CWndBase* pWndParent )
{
	// Daisy���� ������ ���ҽ��� ������ ����.
	return CWndNeuz::InitDialog( APP_SMELT_SAFETY, pWndParent, 0, CPoint( 0, 0 ) );
}

void CWndSmeltSafety::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	// ���⿡ �ڵ��ϼ���

	constexpr int StaticMaterialID[SMELT_MAX] = {WIDC_STATIC11, WIDC_STATIC12, WIDC_STATIC13, WIDC_STATIC14, WIDC_STATIC15, WIDC_STATIC16,
								WIDC_STATIC17, WIDC_STATIC18, WIDC_STATIC19, WIDC_STATIC20};
	constexpr int StaticScrollID1[SMELT_MAX] = {WIDC_STATIC41, WIDC_STATIC42, WIDC_STATIC43, WIDC_STATIC44, WIDC_STATIC45, WIDC_STATIC46,
								WIDC_STATIC47, WIDC_STATIC48, WIDC_STATIC49, WIDC_STATIC50};
	constexpr int StaticScrollID2[SMELT_MAX] = {WIDC_STATIC61, WIDC_STATIC62, WIDC_STATIC63, WIDC_STATIC64, WIDC_STATIC65, WIDC_STATIC66,
								WIDC_STATIC67, WIDC_STATIC68, WIDC_STATIC69, WIDC_STATIC70};
	constexpr int ResultStaticID[SMELT_MAX] = { WIDC_STATIC31, WIDC_STATIC32, WIDC_STATIC33, WIDC_STATIC34, WIDC_STATIC35, WIDC_STATIC36, WIDC_STATIC37, WIDC_STATIC38, WIDC_STATIC39, WIDC_STATIC40 };


	for (size_t i = 0; i != SMELT_MAX; ++i) {
		GenLine & genLine = m_genLines[i];

		genLine.material.OnInitialUpdate(GetWndCtrl(StaticMaterialID[i]), StaticMaterialID[i]);
		genLine.scroll1.OnInitialUpdate(GetWndCtrl(StaticScrollID1[i]), StaticScrollID1[i]);
		genLine.scroll2.OnInitialUpdate(GetWndCtrl(StaticScrollID2[i]), StaticScrollID2[i]);
		genLine.resultStaticId = ResultStaticID[i];
		genLine.resultStatic = false;
	}

	

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
		for (GenLine & genLine : m_genLines) {
			CWndStatic* pWndStatic = GetDlgItem<CWndStatic>(genLine.resultStaticId);
			assert(pWndStatic != NULL);
			rect = pWndStatic->GetWndRect();
			pWndStatic->Move(rect.left + EXTENSION_PIXEL, rect.top);
		}
    
    static constexpr std::initializer_list<int> widgetsToMove = {
      WIDC_STATIC1, WIDC_TITLE_NOW_GRADE, WIDC_TITLE_MAX_GRADE,
      WIDC_NOW_GRADE, WIDC_EDIT_MAX_GRADE, WIDC_BUTTON_PLUS,
      WIDC_BUTTON_MINUS, WIDC_BUTTON_START, WIDC_BUTTON_STOP,
      WIDC_BUTTON_RESET
    };

    for (const int widgetID : widgetsToMove) {
      CWndBase * pWndBase = GetDlgItem(widgetID);
      assert(pWndBase);
      const CRect rect = pWndBase->GetWndRect();
      pWndBase->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);
    }

		CRect wndrect = GetWndRect();
		wndrect.right = wndrect.right + EXTENSION_PIXEL;
		SetWndRect(wndrect);
	}
	else
	{
		for (GenLine & genLine : m_genLines) {
			CWndBase * pWndStatic = GetDlgItem(genLine.scroll2.staticNum);
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

	m_pNowGaugeTexture = CWndBase::m_textureMng.AddTexture( MakePath(DIR_THEME, "SafetyGauge.bmp"), 0xffff00ff);
	m_pSuccessTexture = CWndBase::m_textureMng.AddTexture( MakePath(DIR_THEME, "SafetySuccess.bmp"), 0xffff00ff);
	m_pFailureTexture = CWndBase::m_textureMng.AddTexture( MakePath(DIR_THEME, "SafetyFailure.bmp"), 0xffff00ff);

	MoveParentCenter();
}

void CWndSmeltSafety::GENMATDIEINFO::OnInitialUpdate(LPWNDCTRL wndCtrl, int staticNum) {
	this->wndCtrl = wndCtrl;
	this->staticNum = staticNum;
	this->isUse = FALSE;
	this->pItemElem = nullptr;
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
			m_genLines[m_nCurrentSmeltNumber].SendUpgradeRequestToServer(m_pItemElem);

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

void CWndSmeltSafety::GenLine::SendUpgradeRequestToServer(CItemElem * upgradedItem) {
	if (material.isUse && scroll1.isUse) {
		g_DPlay.SendSmeltSafety(
			upgradedItem->m_dwObjId,
			material.pItemElem->m_dwObjId,
			scroll1.pItemElem->m_dwObjId,
			scroll2.isUse ? scroll2.pItemElem->m_dwObjId : NULL_ID
		);

		material.RemoveItem();
		scroll1.RemoveItem();
		scroll2.RemoveItem();
	}
}

void CWndSmeltSafety::GENMATDIEINFO::RemoveItem() {
	isUse = FALSE;
	if (pItemElem) {
		pItemElem->SetExtra(pItemElem->GetExtra() - 1);
		pItemElem = nullptr;
	}
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
    ClientToScreen(&pointMouse);
    ClientToScreen(&rectSmeltItem);
		if(m_pItemElem != NULL)
		{
			g_WndMng.PutToolTip_Item(m_pItemElem, pointMouse, &rectSmeltItem);
		}
		else
		{
			CString strEmptyTooltip = prj.GetText(TID_GAME_TOOLTIP_SMELT_SAFETY_ITEM);
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectSmeltItem, pointMouse);
		}
	}

	DrawListItem(p2DRender);

	const int nExtensionPixel((m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT) ? EXTENSION_PIXEL : 0);
	for (GenLine & genLine : GenLinesUntilCurrentSmelt()) {
		LPWNDCTRL lpStatic = GetWndCtrl(genLine.resultStaticId);

		CRect rectStaticTemp = lpStatic->rect + CPoint(nExtensionPixel, 0);
		CTexture * pTexture = genLine.resultStatic ? m_pSuccessTexture : m_pFailureTexture;
		m_Theme.RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferSuccessImage, pTexture);
	}

	if(m_bStart != NULL && m_bResultSwitch != false)
	{
		const int nExtensionPixel( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) ? EXTENSION_PIXEL : 0 );
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(m_genLines[m_nCurrentSmeltNumber].resultStaticId);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth + nExtensionPixel;
		assert(m_pVertexBufferGauge != NULL);
		m_Theme.RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferGauge, m_pNowGaugeTexture);
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
				if(m_eWndMode == WND_PIERCING && g_pPlayer)
				{
					if(g_pPlayer->GetGold() < 100000)
					{
						g_WndMng.PutString(TID_GAME_LACKMONEY);
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
					m_genLines[m_nMaterialCount - 1].material.SubtractListItem();
					--m_nMaterialCount;

					while(m_nScroll1Count > m_nMaterialCount)
					{
						m_genLines[m_nScroll1Count - 1].scroll1.SubtractListItem();
						--m_nScroll1Count;
					}

					if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT )
					{
						while(m_nScroll2Count > m_nMaterialCount)
						{
							m_genLines[m_nScroll2Count - 1].scroll2.SubtractListItem();
							--m_nScroll2Count;
						}
					}
				}
			}
			else
			{
				m_genLines[m_nMaterialCount - 1].material.SubtractListItem();
				--m_nMaterialCount;

				if(m_nScroll1Count > m_nMaterialCount)
				{
					m_genLines[m_nScroll1Count - 1].scroll1.SubtractListItem();
					--m_nScroll1Count;
				}

				if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && m_nScroll2Count > m_nMaterialCount )
				{
					m_genLines[m_nScroll2Count - 1].scroll2.SubtractListItem();
					--m_nScroll2Count;
				}
			}
			RefreshValidSmeltCounter();
			if( m_nMaterialCount == m_nCurrentSmeltNumber )
				m_pSelectedElementalCardItemProp = NULL;
		}
		else if(IsDropScroll1Zone(point) && m_nScroll1Count > m_nCurrentSmeltNumber)
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nScroll1Count > m_nCurrentSmeltNumber)
				{
					m_genLines[m_nScroll1Count - 1].scroll1.SubtractListItem();
					--m_nScroll1Count;
				}
			}
			else
			{
				m_genLines[m_nScroll1Count - 1].scroll1.SubtractListItem();
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
					m_genLines[m_nScroll2Count - 1].scroll2.SubtractListItem();
					--m_nScroll2Count;
				}
			}
			else
			{
				m_genLines[m_nScroll2Count - 1].scroll2.SubtractListItem();
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
		m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferGauge, 
												 NULL);
	assert(m_pVertexBufferGauge != NULL);

	if( m_pVertexBufferSuccessImage == NULL )
		m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferSuccessImage, 
												 NULL);
	assert(m_pVertexBufferSuccessImage != NULL);

	if( m_pVertexBufferFailureImage == NULL )
		m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
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
					g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR01);
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
					g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR01);
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
					g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR02);
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
					g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR01);
				}
				break;
			}
		}

		if(bAcceptableItem && m_pItemElem == NULL && pItemElem != NULL)
		{
			m_pItemElem = pItemElem;
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);

			assert(pItemProp != NULL);
			m_pItemTexture = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

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
		if(IsAcceptableMaterial(pItemProp))
		{
			if(m_nMaterialCount < SMELT_MAX)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					while(m_nMaterialCount < SMELT_MAX && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						m_genLines[m_nMaterialCount].material.AddListItem(pItemElem);
						++m_nMaterialCount;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						m_genLines[m_nMaterialCount].material.AddListItem(pItemElem);
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
						m_genLines[m_nScroll1Count].scroll1.AddListItem(pItemElem);
						++m_nScroll1Count;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						m_genLines[m_nScroll1Count].scroll1.AddListItem(pItemElem);
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
							g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR03);
						}
						else
						{
							// ���� ������ ����Į���� ����ؾ�? �մϴ�.
							g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR04);
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// ���� �������� ����ؾ�? �մϴ�.
						g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR05);
						break;
					}
				case WND_ELEMENT:
					{
						// ���� �Ӽ� ī�带 ����ؾ�? �մϴ�.
						g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR17);
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
						m_genLines[m_nScroll2Count].scroll2.AddListItem(pItemElem);
						++m_nScroll2Count;
					}
				}
				else
				{
					m_nScroll2Count = (m_nCurrentSmeltNumber > m_nScroll2Count) ? m_nCurrentSmeltNumber : m_nScroll2Count;
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						m_genLines[m_nScroll2Count].scroll2.AddListItem(pItemElem);
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
							g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR03);
						}
						else
						{
							// ���� ������ ����Į���� ����ؾ�? �մϴ�.
							g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR04);
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// ���� �������� ����ؾ�? �մϴ�.
						g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR05);
						break;
					}
				case WND_ELEMENT:
					{
						// ���� �Ӽ� ī�带 ����ؾ�? �մϴ�.
						g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR17);
						break;
					}
				}
			}
		}
		else
		{
			if( m_eWndMode == WND_ELEMENT && SAI79::IsElementalCard( pItemProp->dwID ) )
			{
				if( m_pItemElem->GetItemResist() != SAI79::NO_PROP )
				{
					// �̹� �����ۿ� �Ӽ� ������ �Ǿ� �ֽ��ϴ�. �� �����ۿ� �� �Ӽ��� ���? �� �����ϴ�.
					g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR18);
				}
				else if( pItemProp != m_pSelectedElementalCardItemProp )
				{
					// �̹� �ٸ� ������ �Ӽ� ī�尡 ��ϵǾ�? �ֽ��ϴ�.
					g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR19);
				}
			}
			else
			{
				// ���� �����ۿ� �´� ���? �η縶���� �ƴմϴ�.
				g_WndMng.PutString(TID_GAME_SMELT_SAFETY_ERROR06);
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
		for (GenLine & genLine : m_genLines) {
			genLine.scroll2.RemoveItem();
		}
	}
}

void CWndSmeltSafety::ResetData(void)
{
	m_nMaterialCount = 0;
	m_nScroll1Count = 0;
	m_nScroll2Count = 0;
	m_nCurrentSmeltNumber = 0;

	for (GenLine & line : m_genLines) {
		line.material.RemoveItem();
		line.scroll1.RemoveItem();
		line.scroll2.RemoveItem();

		CWndBase * pWndStatic = GetDlgItem(line.resultStaticId);
		assert(pWndStatic);
		pWndStatic->SetTitle("");
	}
	m_pSelectedElementalCardItemProp = NULL;
}

void CWndSmeltSafety::GENMATDIEINFO::AddListItem(CItemElem* pItemElem)
{
	assert(this->isUse == FALSE && this->pItemElem == NULL);
	this->isUse = TRUE;
	this->pItemElem = pItemElem;
	this->pItemElem->SetExtra(pItemElem->GetExtra() + 1);
}
void CWndSmeltSafety::GENMATDIEINFO::SubtractListItem()
{
	assert(this->isUse != FALSE && this->pItemElem != NULL);
	this->isUse = FALSE;
	this->pItemElem->SetExtra(this->pItemElem->GetExtra() - 1);
	this->pItemElem = NULL;
}

void CWndSmeltSafety::DrawListItem(C2DRender* p2DRender)
{
	if(m_eWndMode == WND_NORMAL && (m_pItemElem == NULL || m_pItemTexture == NULL))
		return;

	const ItemProp* pItemProp = NULL;
	CTexture* pTexture = NULL;

	for (GenLine & genLine : GenLinesSinceCurrentSmelt()) {
		
		assert(genLine.material.wndCtrl != NULL);
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
					const auto card = SAI79::GetEleCard(static_cast<SAI79::ePropType>(m_pItemElem->GetItemResist()));
					if (card != 0) {
						pItemProp = prj.GetItemProp(card);
					}
				}
				break;
			}
		}
		genLine.material.Render(p2DRender, pItemProp);

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
		genLine.scroll1.Render(p2DRender, pItemProp);

		if( m_eWndMode == WND_NORMAL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE && m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL )
		{
			genLine.scroll2.Render(p2DRender, prj.GetItemProp(II_SYS_SYS_SCR_SMELTING));
		}
		if( m_eWndMode == WND_ELEMENT && m_pItemElem && m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
		{
			genLine.scroll2.Render(p2DRender, prj.GetItemProp(II_SYS_SYS_SCR_SMELTING2));
		}
	}
}

void CWndSmeltSafety::GENMATDIEINFO::Render(C2DRender * p2DRender, const ItemProp * pItemProp) {
	assert(wndCtrl);
	if (!pItemProp) return;

	static constexpr int NORMAL_ALPHA = 255;
	static constexpr int TRANSLUCENT_ALPHA = 75;

	CTexture * pTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
	assert(pTexture != NULL);
	const int nAlphaBlend = isUse ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
	pTexture->Render(p2DRender, wndCtrl->rect.TopLeft(), nAlphaBlend);
}


bool CWndSmeltSafety::IsDropMaterialZone(CPoint point) const {
	for (size_t i = 0; i < SMELT_MAX; i++) {
		if (m_genLines[i].material.wndCtrl->rect.PtInRect(point)) {
			return true;
		}
	}
	return false;
}

bool CWndSmeltSafety::IsDropScroll1Zone(CPoint point) const {
	for (size_t i = 0; i < SMELT_MAX; i++) {
		if (m_genLines[i].scroll1.wndCtrl->rect.PtInRect(point)) {
			return true;
		}
	}
	return false;
}

bool CWndSmeltSafety::IsDropScroll2Zone(CPoint point) const {
	for (size_t i = 0; i < SMELT_MAX; i++) {
		if (m_genLines[i].scroll2.wndCtrl->rect.PtInRect(point)) {
			return true;
		}
	}
	return false;
}

bool CWndSmeltSafety::IsAcceptableMaterial(const ItemProp* pItemProp)
{
	assert(m_pItemElem != NULL);

	switch(m_eWndMode)
	{
	case WND_NORMAL:
		if (m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE) {
			return ItemProps::IsOrichalcum(*pItemProp);
		} else {
			return pItemProp->dwID == II_GEN_MAT_ORICHALCUM02;
		}
	case WND_ACCESSARY:
	case WND_PIERCING:
		return ItemProps::IsMoonstone(*pItemProp);
	case WND_ELEMENT:
		{
			const auto itemResist = m_pItemElem->GetItemResist();

			if (itemResist != SAI79::NO_PROP) {
				const DWORD wantedCard = SAI79::GetEleCard(static_cast<SAI79::ePropType>(itemResist));
				return pItemProp->dwID == wantedCard;
			} else if (m_nMaterialCount == m_nCurrentSmeltNumber) {
				if (SAI79::IsElementalCard(pItemProp->dwID)) {
					m_pSelectedElementalCardItemProp = pItemProp;
					return TRUE;
				}
			} else {
				return pItemProp == m_pSelectedElementalCardItemProp;
			}
		}
		break;
	}
	return false;
}

bool CWndSmeltSafety::IsAcceptableScroll1(const ItemProp* pItemProp) const
{
	assert(m_pItemElem != NULL);

	switch (m_eWndMode) {
	case WND_NORMAL:
	{
		if (m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE) {
			return pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT;
		} else {
			return pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT3;
		}
		break;
	}
	case WND_ACCESSARY:
		return pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT4;
	case WND_PIERCING:
		return pItemProp->dwID == II_SYS_SYS_SCR_PIEPROT;
	case WND_ELEMENT:
		return pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT;
	}

	return false;
}

bool CWndSmeltSafety::IsAcceptableScroll2(const ItemProp* pItemProp) const
{
	assert(m_pItemElem != NULL);

	switch( m_eWndMode )
	{
	case WND_NORMAL:
		{
			if( m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
			{
				return pItemProp->dwID == II_SYS_SYS_SCR_SMELTING;
			}
			break;
		}
	case WND_ELEMENT:
		{
			if( m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
			{
				return pItemProp->dwID == II_SYS_SYS_SCR_SMELTING2 ;
			}
			break;
		}
	}
	return false;
}

int CWndSmeltSafety::GetNowSmeltValue() const
{
	if (m_pItemElem) {
		if (m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY) {
			return m_pItemElem->GetAbilityOption();
		} else if (m_eWndMode == WND_PIERCING) {
			return m_pItemElem->GetPiercingSize();
		} else if (m_eWndMode == WND_ELEMENT) {
			return m_pItemElem->GetResistAbilityOption();
		}
	}

	return 0;
}

int CWndSmeltSafety::GetDefaultMaxSmeltValue() const {
	assert(m_pItemElem != NULL);

	switch (m_eWndMode) {
		case WND_NORMAL:
			return 10;
		case WND_ACCESSARY:
			return 20;
		case WND_PIERCING:
			if (m_pItemElem->GetProp()->dwItemKind3 == IK3_SUIT) {
				return 4;
			} else {
				return 10;
			}
		case WND_ELEMENT:
			return 20;
		default:
			return 0;
	}
}


///////////////////////////////////////////////////////////////////////////////

BOOL CWndSmeltSafetyConfirm::Initialize(CWndBase * pWndParent) {
  return CWndNeuz::InitDialog(APP_SMELT_SAFETY_CONFIRM, pWndParent, 0, CPoint(0, 0));
}

DWORD CWndSmeltSafetyConfirm::GetText(MissingScroll eErrorMode) {
  switch (eErrorMode) {
    case MissingScroll::SProtect:
      return TID_GAME_SMELT_SAFETY_CONFIRM_NORMAL;
    case MissingScroll::UProtect:
      return TID_GAME_SMELT_SAFETY_CONFIRM_HIGHEST;
    case MissingScroll::AProtect:
      return TID_GAME_SMELT_SAFETY_CONFIRM_ACCESSARY;
    default:
      return 0;
  }
}

void CWndSmeltSafetyConfirm::OnInitialUpdate() {
  CWndNeuz::OnInitialUpdate();

  CWndText * pWndText = GetDlgItem<CWndText>(WIDC_CONFIRM_TEXT);
  assert(pWndText);

  const DWORD tid = GetText(m_eErrorMode);
  if (tid != 0) {
    pWndText->AddString(prj.GetText(tid));
  }

  MoveParentCenter();
}

BOOL CWndSmeltSafetyConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
  if (CWndInventory * pWndInventory = GetWndBase<CWndInventory>(APP_INVENTORY)) {
    if (nID == WIDC_SMELT_YES) {
      if (m_pItemElem)
        pWndInventory->RunUpgrade(m_pItemElem);

      Destroy();
    } else if (nID == WIDC_SMELT_NO) {
      pWndInventory->BaseMouseCursor();
      Destroy();
    }
  }

  return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


///////////////////////////////////////////////////////////////////////////////

void CWndSmeltSafetyConfirm::OnDestroy() {
  if (CWndInventory * pWndInventory = GetWndBase<CWndInventory>(APP_INVENTORY)) {
    pWndInventory->BaseMouseCursor();
  }
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

BOOL CWndEquipBindConfirm::Initialize( CWndBase* pWndParent )
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
	pWndButton->SetDefault();

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
