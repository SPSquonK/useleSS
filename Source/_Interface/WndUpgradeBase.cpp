#include "stdafx.h"
#include "defineitem.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndUpgradeBase.h"
#include "DPClient.h"



/****************************************************
  WndId : APP_UPGRADE_BASE - 
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndUpgradeBase::CWndUpgradeBase() {
	SetPutRegInfo(FALSE);
}

CWndUpgradeBase::~CWndUpgradeBase() { m_slot.Clear(); }

void CWndUpgradeBase::OnDraw( C2DRender* p2DRender ) {
	m_slot.Draw(p2DRender, this);
}

void CWndUpgradeBase::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	
	CRect rcInventory	= pWndInventory->GetWindowRect( TRUE );
	CRect rcVendor = GetWindowRect( TRUE );
	CPoint ptInventory	= rcInventory.TopLeft();
	CPoint point;
	if( ptInventory.x > g_WndMng.GetWndRect().Width() / 2 )
		point	= ptInventory - CPoint( rcVendor.Width(), 0 );
	else
		point	= ptInventory + CPoint( rcInventory.Width(), 0 );

	Move( point );

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	LPWNDCTRL pCustom = NULL;

	m_slot.m_rect = GetWndCtrl(WIDC_CUSTOM3)->rect;
} 

BOOL CWndUpgradeBase::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_TEST, pWndParent, 0, 0);
}

BOOL CWndUpgradeBase::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( g_pPlayer == NULL )
		return FALSE;

	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();

	// 아이템이 인벤토리에서 왔는가?
	if( pShortcut->m_dwShortcut == ShortcutType::Item && pWndFrame->GetWndId() == APP_INVENTORY )
	{
		if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
		{
			g_WndMng.PutString(TID_GAME_EQUIPPUT);
			SetForbid( TRUE );
			return FALSE;
		}

		CItemElem* pItemElem = g_pPlayer->GetItemId( pShortcut->m_dwId );

		if(pItemElem == NULL)
			return FALSE;
		
		// 아이템( 방어구, 무기구 )
		if (m_slot.IsIn(point)) {
			const bool r = DropMainItem(pItemElem);
			if (!r) return FALSE;
		}
	}			
	else
		SetForbid( FALSE );

	return FALSE;
}

bool CWndUpgradeBase::DropMainItem(CItemElem * pItemElem) {
	if (m_slot) return false;

	if (pItemElem->m_nResistSMItemId != 0) // 상용화 아이템 적용중이면 불가능
	{
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_NOTUPGRADE), MB_OK);
		return false;
	}

	// 방어구나 무기류가 아니면 제련불가능
	if (pItemElem->GetProp()->dwItemKind2 == IK2_ARMOR ||
		pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT ||
		pItemElem->GetProp()->dwItemKind2 == IK2_ARMORETC) {
		m_slot.Set(pItemElem);
	}

	return true;
}

BOOL CWndUpgradeBase::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_OK:
			{
			if (m_slot) {

				g_WndMng.OpenMessageBox(prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK);
				return FALSE;
			}
			}
			break;
		case WIDC_CANCEL:
			{
				Destroy();
			}
			break;
	};

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndUpgradeBase::OnDestroyChildWnd( CWndBase* pWndChild )
{
}

void CWndUpgradeBase::OnDestroy() {
	m_slot.Clear();
}

void CWndUpgradeBase::OnRButtonUp( UINT nFlags, CPoint point ) {
	if (m_slot.IsIn(point)) {
		m_slot.Clear();

		GetDlgItem(WIDC_STATIC6)->SetTitle("");
		GetDlgItem(WIDC_STATIC4)->SetTitle("");
		GetDlgItem(WIDC_STATIC10)->SetTitle("");
		GetDlgItem(WIDC_STATIC11)->SetTitle("");
		GetDlgItem(WIDC_STATIC5)->SetTitle("");
	}
}
