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
CWndUpgradeBase::CWndUpgradeBase()
{
	SetPutRegInfo( FALSE );

	m_nCost = 0;
	m_dwReqItem[0] = 0;
	m_dwReqItem[1] = 0;

	m_nCount[0] = 0;
	m_nCount[1] = 0;

	m_nMaxCount = 0;
}

CWndUpgradeBase::~CWndUpgradeBase() { m_slots.Clear(); }

void CWndUpgradeBase::OnDraw( C2DRender* p2DRender ) {
	m_slots.Draw(p2DRender, this);
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

	m_slots.SetRects({
		GetWndCtrl(WIDC_CUSTOM3)->rect,
		GetWndCtrl(WIDC_CUSTOM4)->rect,
		GetWndCtrl(WIDC_CUSTOM1)->rect,
		GetWndCtrl(WIDC_CUSTOM2)->rect,
		GetWndCtrl(WIDC_CUSTOM5)->rect,
		GetWndCtrl(WIDC_CUSTOM6)->rect
		});

	m_nCount[0] = 0;
	m_nCount[1] = 0;

	m_nMaxCount = 0;
} 

BOOL CWndUpgradeBase::Initialize( CWndBase* pWndParent ) 
{
	return CWndNeuz::InitDialog( APP_TEST, pWndParent, 0, 0 );
}

void CWndUpgradeBase::OnLButtonUp( UINT nFlags, CPoint point ) 
{

}

void CWndUpgradeBase::OnLButtonDown( UINT nFlags, CPoint point ) 
{

}
BOOL CWndUpgradeBase::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( g_pPlayer == NULL )
		return FALSE;

	int nNeedRes[3][10] = {
		{ 10, 20, 40,  70, 110, 160, 220, 290, 370, 460 },
		{ 20, 40, 80, 140, 220, 320, 440, 580, 740, 920 },
		{ 2000, 4000, 6000, 8000, 10000, 12000, 14000, 16000, 18000, 20000 }
	};

	float fSucessPersent[5][10] = {
		{ 80.0f, 50.0f, 20.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f, 0.0f },
		{  0.0f, 70.0f, 50.0f, 20.0f, 10.0f,  0.0f,  0.0f,  0.0f,  0.0f, 0.0f },
		{  0.0f,  0.0f,  0.0f, 60.0f, 40.0f, 20.0f, 10.0f,  0.0f,  0.0f, 0.0f },
		{  0.0f,  0.0f,  0.0f,  0.0f,  0.0f, 60.0f, 40.0f, 20.0f, 10.0f, 0.0f },
		{  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f, 50.0f, 25.0f, 5.0f }
	};

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

		static const int *  pAbilityOption = NULL;

		CItemElem* pItemElem = g_pPlayer->GetItemId( pShortcut->m_dwId );

		if(pItemElem == NULL)
			return FALSE;
		
		// 아이템( 방어구, 무기구 )
		if (m_slots[0].IsIn(point)) {
			const bool r = DropMainItem(pItemElem);
			if (!r) return FALSE;
		}
		else
		// 아이템( 카드, 주사위 )
		if (m_slots[1].IsIn(point)) {
			if(m_slots[1] )
				return FALSE;

			// 재련할 아이템이 없는경우 리턴
			if(m_slots[0] == NULL )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_ITEMFIRST), MB_OK, this );
				return FALSE;
			}
			
			// 카드, 주사위 아이템이 아니면 리턴
			if( pItemElem->GetProp()->dwItemKind3 != IK3_ELECARD && pItemElem->GetProp()->dwItemKind3 != IK3_ENCHANT )
				return FALSE;

			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD  )
			{
				const ItemProp * zeroProp = m_slots[0].m_item->GetProp();
				if( (zeroProp->dwItemKind3 != IK3_SUIT &&
					zeroProp->dwItemKind2 != IK2_WEAPON_MAGIC &&
					zeroProp->dwItemKind2 != IK2_WEAPON_DIRECT )
					)
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOELEUPGRADE), MB_OK, this );
					return FALSE;
				}
			}
				
	
			// 카드이고, 제련할 아이템의 속성이 카드의 속성과 다를경우 리턴
			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD
				&& m_slots[0]->m_bItemResist != SAI79::NO_PROP
				&& m_slots[0]->m_bItemResist != pItemElem->GetProp()->eItemType )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_TWOELEMENT), MB_OK, this );
				return FALSE;
			}

			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD )
			{
				pAbilityOption = &(m_slots[0]->m_nResistAbilityOption);
			}

			if( pItemElem->GetProp()->dwItemKind3 == IK3_ENCHANT )
			{
				//*
				// 속성레벨이 10 이상이면 제련 불가능
				if(m_slots[0]->GetAbilityOption() >= 10 )
				{
					CString str;
					str.Format( prj.GetText(TID_UPGRADE_ERROR_MAXLEVEL), 10 );
					g_WndMng.OpenMessageBox( str, MB_OK, this );
					return FALSE;
				}
				/**/
				

				pAbilityOption = m_slots[0]->GetAbilityOptionPtr();
			}
			
			
			if( fSucessPersent[pItemElem->GetProp()->dwItemLV-1][*pAbilityOption] <= 0.0f )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGUPLEVEL), MB_OK, this );
				return FALSE;
			}

			// 제련시 필요 보조석 설정(카드인경우와 주사위인경우)
			m_dwReqItem[0] = 0;
			m_dwReqItem[1] = 0;
			
			if( pItemElem->GetProp()->dwItemKind3 == IK3_ELECARD )
			{
				switch( pItemElem->GetProp()->eItemType )
				{
					case SAI79::FIRE:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_ERONS;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_GURU;
						break;
					case SAI79::WATER:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_KRASEC;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_GURU;
						break;
					case SAI79::EARTH:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_MINERAL;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_GURU;
						break;
					case SAI79::ELECTRICITY:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_MINERAL;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_KRASEC;
						break;
					case SAI79::WIND:
						m_dwReqItem[0]  = II_GEN_MAT_SUP_ERONS;
						m_dwReqItem[1]  = II_GEN_MAT_SUP_KRASEC;
						break;
				}
			}
			else
			if( pItemElem->GetProp()->dwItemKind3 == IK3_DICE )
			{
				m_dwReqItem[0]  = II_GEN_MAT_SUP_MINERAL;
				m_dwReqItem[1]  = II_GEN_MAT_SUP_ERONS;
			}
			
			m_slots[1].Set(pItemElem);
			
			//필요한 보조석 갯수 표시
			CString str;
			CWndStatic* pWndStatic;
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			str.Format( prj.GetText(TID_UPGRADE_SUPPORTM), nNeedRes[0][*pAbilityOption] );
			pWndStatic->SetTitle(str);
			
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			str.Format( prj.GetText(TID_UPGRADE_SUPPORTM), nNeedRes[0][*pAbilityOption] );
			pWndStatic->SetTitle(str);

			//필요한 보조석 이름 표시
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			str.Format( "%s", prj.GetItemProp(m_dwReqItem[0])->szName );
			pWndStatic->SetTitle(str);

			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			str.Format( "%s", prj.GetItemProp(m_dwReqItem[1])->szName );
			pWndStatic->SetTitle(str);
			


			// 필요 금액 설정
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			str.Format( prj.GetText(TID_UPGRADE_COST), nNeedRes[2][*pAbilityOption] );
			pWndStatic->SetTitle(str);

			m_nCount[0] = nNeedRes[0][*pAbilityOption];
			m_nCount[1] = nNeedRes[0][*pAbilityOption];

			m_nMaxCount = nNeedRes[0][*pAbilityOption];
			
			// 성공확률 표시
			/*
			float fResult = fSucessPersent[m_pItemElem[1]->GetProp()->dwItemLV-1][m_pItemElem[0]->m_nAbilityOption];
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
			str.Format( prj.GetText(TID_UPGRADE_SUCCESSRATE), fResult );
			pWndStatic->SetTitle(str);
			*/
		}
		else
		// 보조석
		if( m_slots[2].IsIn(point) )
		{
			if (!m_slots[1]) return FALSE;

			if(m_slots[2] || !pAbilityOption )
				return FALSE;

			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[0] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					return FALSE;
				}

				if(m_slots[4] )
				{
					if(pItemElem->m_nItemNum < m_nCount[0])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}
				
				if( pItemElem->m_nItemNum < m_nCount[0] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[0] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[0] );	
					m_nCount[0] = 0;
				}

				m_slots[2].m_item = pItemElem;
			}
		}
		else
		if( m_slots[4].IsIn(point))
		{
			if(m_slots[1] == NULL )
				return FALSE;

			if(m_slots[4] || !pAbilityOption )
				return FALSE;
			
			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[0] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					//g_WndMng.OpenMessageBox( "필요한 보조석이 아닙니다.", MB_OK, this );
					return FALSE;
				}

				if(m_slots[2] )
				{
					if(pItemElem->m_nItemNum < m_nCount[0])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}

				if( pItemElem->m_nItemNum < m_nCount[0] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[0] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[0] );	
					m_nCount[0] = 0;
				}
				
				m_slots[4].m_item	= pItemElem;
			}
		}
		else
		if(m_slots[3].IsIn(point) )
		{
			if(m_slots[1] == NULL )
				return FALSE;

			if(m_slots[3] || !pAbilityOption )
				return FALSE;

			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[1] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					return FALSE;
				}

				if(m_slots[5] )
				{
					if(pItemElem->m_nItemNum < m_nCount[1])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}

				if( pItemElem->m_nItemNum < m_nCount[1] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[1] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[1] );	
					m_nCount[1] = 0;
				}
				
				m_slots[3].m_item	= pItemElem;
			}
		}
		else
		if(m_slots[5].IsIn(point) )
		{
			if(m_slots[1] == NULL )
				return FALSE;

			if(m_slots[5] || !pAbilityOption  )
				return FALSE;
			
			if( pItemElem->GetProp()->dwItemKind3 == IK3_SUPSTONE )
			{
				if( m_dwReqItem[1] != pItemElem->m_dwItemId )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM), MB_OK, this );
					return FALSE;
				}

				if(m_slots[3] )
				{
					if(pItemElem->m_nItemNum < m_nCount[1])
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
						return FALSE;
					}
				}

				if( pItemElem->m_nItemNum < m_nCount[1] )
				{
					pItemElem->SetExtra( pItemElem->m_nItemNum );	
					m_nCount[1] -= pItemElem->m_nItemNum;
				}
				else
				{
					pItemElem->SetExtra( m_nCount[1] );	
					m_nCount[1] = 0;
				}
				
				m_slots[5].m_item	= pItemElem;
			}
		}
	}			
	else
		SetForbid( FALSE );

	return FALSE;
}

bool CWndUpgradeBase::DropMainItem(CItemElem * pItemElem) {
	if (m_slots[0]) return false;

	if (pItemElem->m_nResistSMItemId != 0) // 상용화 아이템 적용중이면 불가능
	{
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_NOTUPGRADE), MB_OK, this);
		return false;
	}

	// 방어구나 무기류가 아니면 제련불가능
	if (pItemElem->GetProp()->dwItemKind2 == IK2_ARMOR ||
		pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT ||
		pItemElem->GetProp()->dwItemKind2 == IK2_ARMORETC) {
		m_slots[0].Set(pItemElem);
	}

	return true;
}

BOOL CWndUpgradeBase::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_OK:
			{
				DWORD dwCount[2];
				DWORD dwobjId[6];
				DWORD ItemCount[6];

				dwCount[0] = 0;
				dwCount[1] = 0;

				if (!m_slots[0] || !m_slots[1])
					break;

				dwobjId[0] = m_slots[0]->m_dwObjId;
				dwobjId[1] = m_slots[1]->m_dwObjId;
				
				const auto ConsiderSlot = [&](const size_t pos) {
					if (m_slots[pos] == NULL) {
						ItemCount[pos] = 0;
						dwobjId  [pos] = NULL_ID;
					} else {
						ItemCount[pos]      = m_slots[pos]->GetExtra();
						dwobjId  [pos]      = m_slots[pos]->m_dwObjId;
						dwCount  [pos % 2] += m_slots[pos]->GetExtra();
					}
				};

				ConsiderSlot(2);
				ConsiderSlot(4);
				ConsiderSlot(3);
				ConsiderSlot(5);

				if( (int)( dwCount[0] ) < m_nMaxCount || (int)( dwCount[1] ) < m_nMaxCount )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_UPGRADE_ERROR_NOSUPSTON), MB_OK, this );
					return FALSE;
				}

				g_DPlay.SendUpgradeBase( dwobjId[0], 
									 	dwobjId[1], 
										dwobjId[2], ItemCount[2],
										dwobjId[3], ItemCount[3],
										dwobjId[4], ItemCount[4],
										dwobjId[5], ItemCount[5] );
				Destroy();
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
	m_slots.Clear();
}


void CWndUpgradeBase::OnRButtonUp( UINT nFlags, CPoint point ) {
	const auto ResetAllTitles = [&]() {
		m_nCount[0] = m_nCount[1] = 0;
		m_dwReqItem[0] = 0;
		m_dwReqItem[1] = 0;

		GetDlgItem(WIDC_STATIC6)->SetTitle("");
		GetDlgItem(WIDC_STATIC4)->SetTitle("");
		GetDlgItem(WIDC_STATIC10)->SetTitle("");
		GetDlgItem(WIDC_STATIC11)->SetTitle("");
		GetDlgItem(WIDC_STATIC5)->SetTitle("");
	};

	if (m_slots[0].IsIn(point)) {
		m_slots.Clear();
		ResetAllTitles();
	} else if (m_slots[1].IsIn(point)) {

		for (auto it = m_slots.values.begin() + 1; it != m_slots.values.end(); ++it) {
			it->Clear();
		}

		ResetAllTitles();
	} else if (m_slots[2] && m_slots[2].IsIn(point)) {
		m_nCount[0] += m_slots[2].m_item->GetExtra();
		m_slots[2].Clear();
	} else if (m_slots[3] && m_slots[3].IsIn(point)) {
		m_nCount[1] += m_slots[3].m_item->GetExtra();
		m_slots[3].Clear();
	} else if (m_slots[4] && m_slots[4].IsIn(point)) {
		m_nCount[0] += m_slots[4].m_item->GetExtra();
		m_slots[4].Clear();
	} else if (m_slots[5] && m_slots[5].IsIn(point)) {
		m_nCount[1] += m_slots[5].m_item->GetExtra();
		m_slots[5].Clear();
	}
}



