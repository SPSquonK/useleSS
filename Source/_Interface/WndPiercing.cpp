#include "stdafx.h"
#include "defineitem.h"
#include "defineText.h"
#include "defineObj.h"
#include "AppDefine.h"
#include "WndPiercing.h"
#include "DPClient.h"

///////////////////////////////////////////////////////////////////////////
// CPiercingMessageBox
//////////////////////////////////////////////////////////////////////////////

CPiercingMessageBox::CPiercingMessageBox(const std::array<CWndPiercing::Slot, 3> & slots) {
	m_Objid[0] = slots[0].m_item->m_dwObjId;
	m_Objid[1] = slots[1].m_item->m_dwObjId;
	m_Objid[2] = slots[2] ? slots[2].m_item->m_dwObjId : NULL_ID;
}

BOOL CPiercingMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_PIERCING_ERROR_NOTICE), 
		pWndParent, 
		MB_OKCANCEL );	
}

BOOL CPiercingMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{

	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendPiercingSize( m_Objid[0], m_Objid[1], m_Objid[2] );
			Destroy();

			if (CWndBase * pWndPiercing = g_WndMng.GetWndBase(APP_PIERCING)) {
				pWndPiercing->Destroy();
			}
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 





///////////////////////////////////////////////////////////////////////////
// CWndPiercing
//////////////////////////////////////////////////////////////////////////////

void CWndPiercing::Slot::Clear() {
	if (m_item) {
		m_item->SetExtra(0);
		m_item = nullptr;
	}
}

void CWndPiercing::Slot::Set(CItemElem * item) {
	if (m_item) m_item->SetExtra(0);
	m_item = item;
	m_item->SetExtra(1);
}

CWndPiercing::CWndPiercing() {
	SetPutRegInfo(FALSE);

	m_slots.fill(Slot{ });
}

CWndPiercing::~CWndPiercing() {
	if (m_pSfx) {
		m_pSfx->Delete();
	}
}

void CWndPiercing::OnDraw(C2DRender * p2DRender) {
	for (auto & slot : m_slots) {
		CItemElem * const pItemElem = slot.m_item;
		const CRect rect = slot.m_rect;

		if (pItemElem && pItemElem->GetTexture()) {
			pItemElem->GetTexture()->Render(p2DRender, rect.TopLeft(), 255);

			if (pItemElem->m_nItemNum > 1) {
				TCHAR szTemp[32];
				_stprintf(szTemp, "%d", pItemElem->GetExtra());
				p2DRender->TextOut(rect.right - 11, rect.bottom - 11, szTemp, 0xff1010ff);
			}

			//
			CRect hitrect = rect;
			CPoint point = GetMousePoint();
			if (rect.PtInRect(point)) {
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&hitrect);

				g_WndMng.PutToolTip_Item(pItemElem, point2, &hitrect);
			}
		}
	}

	CWndStatic * pWndFocusStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC9);
	if (m_slots[0]) {
		int nCost = 0;
		ItemProp * pItemProp = m_slots[0].m_item->GetProp();
		if (pItemProp)
			nCost = 100000;

		char buff[10] = { 0 };
		pWndFocusStatic->SetTitle(itoa(nCost, buff, 10));
	} else {
		pWndFocusStatic->SetTitle("0");
	}
}

void CWndPiercing::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	
	CRect rcInventory	= pWndInventory->GetWindowRect( TRUE );
	CRect rcVendor = GetWindowRect( TRUE );
	CPoint ptInventory	= rcInventory.TopLeft();
	CPoint point;
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		point	= ptInventory - CPoint( rcVendor.Width(), 0 );
	else
		point	= ptInventory + CPoint( rcInventory.Width(), 0 );

	Move( point );

	m_slots[0].m_rect = GetWndCtrl(WIDC_STATIC5)->rect;
	m_slots[1].m_rect = GetWndCtrl(WIDC_STATIC6)->rect;
	m_slots[2].m_rect = GetWndCtrl(WIDC_STATIC7)->rect;

	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_STATIC9 );
	pGoldNum->AddWndStyle( WSS_MONEY );
	

	if( g_pPlayer )
		m_pSfx = CreateSfx( g_Neuz.m_pd3dDevice, XI_INT_INCHANT, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
} 

BOOL CWndPiercing::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PIERCING, 0, 0, pWndParent );
}

BOOL CWndPiercing::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();

	// 아이템이 인벤토리에서 왔는가?
	if( !(pShortcut->m_dwShortcut == SHORTCUT_ITEM) && !(pWndFrame->GetWndId() == APP_INVENTORY) )
		return FALSE;
	
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString(TID_GAME_EQUIPPUT);
		SetForbid( TRUE );
		return FALSE;
	}
		
	CItemElem * pItemElem  = g_pPlayer->GetItemId( pShortcut->m_dwId );
	if (!pItemElem) return FALSE;

	// 아이템( 방어구, 무기구 )
	if (m_slots[0].IsIn(point)) {
		
		// 4개 초과로 피어싱 할수 없음
		if (!pItemElem->IsPierceAble(NULL_ID, TRUE)) {
			g_WndMng.PutString(TID_PIERCING_POSSIBLE);
			return FALSE;
		}
		
		if (pItemElem->GetProp()->dwItemRare == 0xffffffff) {
			g_WndMng.PutString(TID_PIERCING_POSSIBLE_ITEM);
			return FALSE;
		}

		m_slots[0].Set(pItemElem);
		m_slots[1].Clear();
		m_slots[2].Clear();
	} else if (m_slots[1].IsIn(point)) {
		// 파워다이스 8, 10만 사용할수 있음
		
		if (m_slots[0]) {
			if (pItemElem->GetProp()->dwID != II_GEN_MAT_MOONSTONE && pItemElem->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1) {
				g_WndMng.PutString(TID_SBEVE_NOTUSEITEM);
				return FALSE;
			}

			m_slots[1].Set(pItemElem);
		}
	} else if (m_slots[2].IsIn(point)) {
		if (m_slots[0]) {
			// 보조아이템은 상용화 아이템만 검사
			if (pItemElem->GetProp()->dwID != II_SYS_SYS_SCR_PIEPROT) {
				g_WndMng.PutString(TID_SBEVE_NOTUSEITEM);
				return FALSE;
			}

			m_slots[2].Set(pItemElem);
		}
	}		

	return TRUE;
}

BOOL CWndPiercing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_OK:
			if (!m_slots[0] || !m_slots[1]) return FALSE;

//		SAFE_DELETE( m_pPiercingMessageBox );
			m_pPiercingMessageBox = new CPiercingMessageBox(m_slots);
			g_WndMng.OpenCustomBox("", m_pPiercingMessageBox, this);				
			break;
		case WIDC_CANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndPiercing::OnDestroyChildWnd( CWndBase* pWndChild )
{
}

void CWndPiercing::OnDestroy() {
	for (auto & slot : m_slots) {
		slot.Clear();
	}
}

void CWndPiercing::OnRButtonUp(UINT nFlags, CPoint point) {
	if (m_slots[0] && m_slots[0].IsIn(point)) {
		m_slots[0].Clear();
		m_slots[1].Clear();
		m_slots[2].Clear();
	} else if (m_slots[1] && m_slots[1].IsIn(point)) {
		m_slots[1].Clear();
	} else if (m_slots[2] && m_slots[2].IsIn(point)) {
		m_slots[2].Clear();
	}
}
