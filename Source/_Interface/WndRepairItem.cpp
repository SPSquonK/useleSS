#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndRepairItem.h"
#include "dpclient.h"
#include "WndManager.h"

/****************************************************
  WndId : APP_REPAIR - 수리창
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_STATIC1 - 총수리가격
  CtrlId : WIDC_EDIT2 - 
  CtrlId : WIDC_OK - 수리확인
  CtrlId : WIDC_CANCEL - 수리취소
  CtrlId : WIDC_STATIC2 - 수리목록
****************************************************/

CWndRepairItem::CWndRepairItem() 
{
	SetPutRegInfo( FALSE );
}

CWndRepairItem::~CWndRepairItem() 
{
	OnInit();
}

void CWndRepairItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	memset( m_adwIdRepair, 0xff, sizeof(m_adwIdRepair) );
	m_dwCost = 0;

	CWndEdit* pWndEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	pWndEdit->EnableWindow( FALSE );

	pWndStaticCost = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );

	m_wndItemCtrl.Create( WLVS_ICON | WBS_NODRAWFRAME, CRect( 6, 24, 174, 192 ), this, 11 );
	m_wndItemCtrl.InitItem( m_adwIdRepair );

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		ptMove = ptInventory - CPoint( rectInventory.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	Move( ptMove );
} 

BOOL CWndRepairItem::Initialize( CWndBase* pWndParent, DWORD ) 
{ 
	return CWndNeuz::InitDialog( APP_REPAIR, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndRepairItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		if( nID == 11 ) // item
		{
			if( pWndFrame->GetWndId() == APP_INVENTORY )
			{
				CItemElem * pItemBase = g_pPlayer->GetItemId(lpShortcut->m_dwId);
				OnItemElemDrop(pItemBase, lpShortcut->m_dwData - 100);
			}
		}
	}
	else
	if( message == WNM_CLICKED )
	{
		if( nID == WIDC_OK )
		{
			g_DPlay.SendRepairItem( m_adwIdRepair );
		}
		else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
		{
			OnInit();
			nID = WTBID_CLOSE;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRepairItem::OnItemElemDrop(CItemElem * pItemBase, DWORD targetSlot) {
	if (!pItemBase) return;
	
	if (g_pPlayer->m_Inventory.IsEquip(pItemBase->m_dwObjId)) {
		g_WndMng.PutString(TID_GAME_REPAIR_EQUIP);
		return;
	}

	// 아이템인가?
	// 방어구 혹은 무기인가?
	// 수리할 필요가 있는가?
	const ItemProp * pItemProp = pItemBase->GetProp();
	if (!pItemProp) {
		return;
	}

	const bool repairable = pItemProp->dwItemKind2 >= IK2_WEAPON_HAND && pItemProp->dwItemKind2 <= IK2_ARMORETC && pItemBase->m_nRepair < pItemProp->nMaxRepair;
	if (!repairable) {
		// 수리할수 없는 아이템
		g_WndMng.PutString(TID_GAME_REPAIR_DONOT);
		return;
	}
	
	if(pItemBase->m_nHitPoint < 0 )
		pItemBase->m_nHitPoint = 0;

	if (pItemBase->m_nHitPoint >= (int)(pItemProp->dwEndurance)) {
		// 수리할 필요가 없는 아이템
		g_WndMng.PutString(TID_GAME_REPAIR_FULLENDURANCE);
	}

	const int nRepair = 100 - ((pItemBase->m_nHitPoint * 100) / pItemProp->dwEndurance);
	const DWORD dwSumCost = m_dwCost + nRepair * (pItemProp->dwCost / 1000 + 1);

	if (g_pPlayer->GetGold() < (int)(dwSumCost)) {
		// 페냐가 부족하여 수리를 할수 없습니다.
		g_WndMng.PutString(TID_GAME_REPAIR_NOTENOUGHPENYA);
		return;
	}

	m_dwCost = dwSumCost;
	char szCost[MAX_PATH] = {0,};
	sprintf( szCost, "%-d", m_dwCost );
	pWndStaticCost->SetTitle( szCost );
	m_adwIdRepair[targetSlot]	= pItemBase->m_dwObjId;
	pItemBase->m_bRepair	= TRUE;
}

void CWndRepairItem::OnDestroy( void )
{
	OnInit();
}

void CWndRepairItem::OnInit( void )
{
	for( int i = 0; i < MAX_REPAIRINGITEM; i++ )
	{
		if( m_adwIdRepair[i] != NULL_ID )
		{
			CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAtId( m_adwIdRepair[i] );
			if( pItemElem )
			{
				pItemElem->m_bRepair	= FALSE;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////

CWndRepairItemCtrl::CWndRepairItemCtrl()
{
	m_nCurSel	= -1;
	m_pFocusItem	= NULL;

	m_pdwIdRepair	= NULL;
}

void CWndRepairItemCtrl::Create( DWORD dwListCtrlStyle, const RECT & rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create(dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}


void CWndRepairItemCtrl::InitItem( DWORD* pdwIdRepair )
{

	m_pdwIdRepair	= pdwIdRepair;
	m_pFocusItem	= NULL;
}

void CWndRepairItemCtrl::OnInitialUpdate( void )
{

}
	
void CWndRepairItemCtrl::OnDraw( C2DRender* p2DRender )
{
	if( NULL == m_pdwIdRepair )
		return;

	if (!g_pPlayer)
		return;

	for( int i = 0; i < MAX_REPAIRINGITEM; i++ )
	{
		int x	= i % 5;
		int y	= i / 5;

		if( m_pdwIdRepair[i] != NULL_ID )
		{

			CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAtId( m_pdwIdRepair[i] );
			if( pItemElem )
			{
				pItemElem->GetTexture()->Render( p2DRender, CPoint( x * 32 + 6, y * 32 + 10 ) );
				CRect rectHittest	= CRect( x * 32, y * 32 + 3, x * 32 + 32, y * 32 + 32 + 3 );
				CPoint point	= GetMousePoint();
				if( rectHittest.PtInRect( point ) )
				{
					CPoint point2 = point;
					ClientToScreen( &point2 );
					ClientToScreen( &rectHittest );
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, 276 ); // APP_REPAIR
				}
				if( i == m_nCurSel )
					p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );
			}
		}
	}
}

BOOL CWndRepairItemCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( pShortcut->m_dwData != 0 ) {
		CItemElem * pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pItemBase )
		{
			const int i = HitTest(point);

			if (i != -1 && m_pdwIdRepair[i] == NULL_ID )
			{
				pShortcut->m_dwData		= i + 100;
				CWndBase* pParent	= GetParentWnd();
				pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut );
			}
			
		}
	}
	return TRUE;
}

void CWndRepairItemCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;

	if( CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;

	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemElem*	pItemElem	= g_pPlayer->m_Inventory.GetAtId( m_pdwIdRepair[nItem] );
	if( pItemElem )
	{
		m_pFocusItem	= pItemElem;
		m_nCurSel	= nItem;
	}
}

void CWndRepairItemCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow	= rectWnd;
	m_rectClient	= m_rectWindow;

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndRepairItemCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect	= GetWindowRect();

	static constexpr DWORD dwColor1	= D3DCOLOR_ARGB( 100, 0, 0,  0 );
	static constexpr DWORD dwColor2	= D3DCOLOR_ARGB( 255, 240, 240,  240 );
	static constexpr DWORD dwColor3	= D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

BOOL CWndRepairItemCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

int CWndRepairItemCtrl::HitTest( CPoint point )
{
	for (int i = 0; i < MAX_VENDITEM; i++) {
		const int x = i % 5;
		const int y = i / 5;

		const CRect hitRect{ CPoint(x * 32, y * 32), CSize(32, 32) };

		if (hitRect.PtInRect(point)) {
			return i;
		}
	}

	return -1;
}

