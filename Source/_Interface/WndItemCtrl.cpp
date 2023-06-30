// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DefineText.h"
#include "DPClient.h"
#include "WndManager.h"

#include "WndText.h"
#include "MsgHdr.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndItemCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndItemCtrl::CWndItemCtrl() 
{
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_pItemContainer = NULL;
	m_bDrag = FALSE;
	m_nOnSelect = -1;

	memset( m_pArrayItemElem, 0, sizeof(CItemElem*)*100 );
	m_nArrayCount = 0;
	m_bVisibleCount = TRUE;
	m_useDieFlag = FALSE;
}
CWndItemCtrl::~CWndItemCtrl()
{
//	ResetContent();
}
void CWndItemCtrl::Create( DWORD dwListCtrlStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD | dwListCtrlStyle, rect, pParentWnd, nID );
}
void CWndItemCtrl::InitItem( CItemContainer* pItemContainer, DWORD SendToId )
{
	m_pItemContainer = pItemContainer;
	m_pFocusItem	= NULL;
	m_dwFrame = SendToId;
}

void CWndItemCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
}

void CWndItemCtrl::UpdateTooltip( void )
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	CPoint point= GetMousePoint();
//	ClientToScreen( &point );

	// 스크롤바 관련   
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
	{
		if( i < 0 )
			continue;

		int x = ( i - pt.y ) % nWidth;
		int y = ( i - pt.y ) / nWidth;

		CItemElem* pItemElem = GetItem( i );
		if( pItemElem == NULL )
			continue;

		CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			
			if( m_dwFrame == APP_SHOP_ )		// 상점
				g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
			else if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK )	// 나의 인벤토리
			{
				if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
				}
				else
				{
					g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
				}
			}
		}
	}
}


void CWndItemCtrl::OnMouseWndSurface( CPoint point )
{
	if( !m_pItemContainer )
		return;

	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	// 스크롤바 관련   
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	if( nParent == APP_INVENTORY )
	{
		for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemElem* pItemElem = GetItem( i );
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				
				if( m_dwFrame == APP_SHOP_ )		// 상점
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
				else 
				if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK )	// 나의 인벤토리
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
					}
					else
					{
						g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
					}
				}

				m_nOnSelect = i;
			}
		}
	}
	if( nParent == APP_BAG_EX )
	{
		for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemElem* pItemElem = GetItem( i );
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
					
				g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
				m_nOnSelect = i;
			}
		}
	}
	else
	{
		for( int i = 0; i < m_nArrayCount; i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemElem* pItemElem = m_pArrayItemElem[i];
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				
				if( m_dwFrame == APP_SHOP_ )		// 상점
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
				else 
				if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK
				 || m_dwFrame == APP_GUILD_BANK
				 || m_dwFrame == APP_COMMON_BANK
					)	// 나의 인벤토리
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
					}
					else
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
					}
				}

				m_nOnSelect = i;
			}
		}
	}



}

void CWndItemCtrl::RanderIcon( C2DRender* p2DRender, CItemElem* pItemElem, int x, int y, int nParent, int nalpha ) 
{
	int bRander = 0;

	if( nParent == APP_SHOP_ )
	{
		ItemProp* pItemProp		= pItemElem->GetProp();
		if( pItemProp == NULL ) return;

		switch( pItemProp->dwItemKind2 )
		{
			case IK2_WEAPON_DIRECT:
//			case IK2_WEAPON_RANGE:
			case IK2_WEAPON_MAGIC:
//			case IK2_WEAPON_DOLL:
//			case IK2_WEAPON_GUN:
			case IK2_ARMOR:
			case IK2_ARMORETC:
			case IK2_CLOTH:
			case IK2_CLOTHETC:
			{
					
				// 무기 : 직업, 능력치 스텟, 가격
				if( pItemProp->dwItemSex != 0xffffffff && pItemProp->dwItemSex != g_pPlayer->GetSex() )
				{
					bRander = 1;
					break;
				}

				if( JOB_VAGRANT != pItemProp->dwItemJob )
				{
					if( pItemProp->dwItemJob != 0xffffffff && JOB_VAGRANT < pItemProp->dwItemJob && !(g_pPlayer->IsInteriorityJob( pItemProp->dwItemJob ) ) )
					{
						bRander = 1;
						break;
					}
				}
				if( pItemProp->dwLimitLevel1 != 0xffffffff && pItemElem->IsLimitLevel( g_pPlayer ) )
				{
					bRander = 1;
					break;
				}
				break;
			}
			case IK2_RIDING:
//			case IK2_BOARDING:
//			case IK2_HATCH:
			{
				// 비행 : 비행레벨
				if( g_pPlayer->GetFlightLv() < (int)( pItemProp->dwFlightLimit ) )
				{
					bRander = 1;
					break;
				}
				break;
			}
		}
 
		// 모든아이템 가격 비교
		if( nParent == APP_SHOP_ )
		{
			CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase( APP_SHOP_ );
			LPCHARACTER lpCharacter = pWndBase->m_pMover->GetCharacter();
			if(lpCharacter && lpCharacter->m_vendor.m_type == CVendor::Type::RedChip)
			{
				if( bRander == 0 && g_pPlayer->m_Inventory.GetAtItemNum( II_CHP_RED ) < (int)pItemElem->GetChipCost() )
					bRander = 1;
			}
		}
		else
		{
			if( bRander == 0 && g_pPlayer->GetGold() < (int)pItemProp->dwCost )
				bRander = 1;
		}
	}

	if( pItemElem->IsFlag( CItemElem::expired ) )
	{
		bRander = 1;
	}


	if( !g_eLocal.GetState(EVE_SCHOOL) )
	{
		if( bRander == 0 )
		{
			pItemElem->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
		}
		else
		if( bRander == 1 )	// 사용할수 없는것
		{
			pItemElem->GetTexture()->Render2( p2DRender, CPoint( x, y ), D3DCOLOR_ARGB( nalpha, 255, 100, 100 ) );
		}
	}
	else
		pItemElem->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
}

void CWndItemCtrl::OnDraw(C2DRender* p2DRender) 
{
#ifdef __SYS_TICKET
	CMover* pPlayer		= g_pPlayer;
	if( !pPlayer )
		return;
	CWorld* pWorld	= pPlayer->GetWorld();
	if( !pWorld )
		return;
#endif	// __SYS_TICKET


	if( !m_pItemContainer )
		return;

	DWORD dwCur = GetTickCount();
	CPoint pt( 3, 3 );

	CWndBase* pWndBase = this->GetParentWnd();

	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;

		// 스크롤바 관련   
		int nPage = nHeight;
		int nRange = m_pItemContainer->m_dwIndexNum / nWidth;// - nPage;
		if( m_pItemContainer->m_dwIndexNum % nWidth )
			nRange++;
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );

		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
		{
			for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
			{
				if( i < 0 ) continue;
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				CItemElem* pItemElem = GetItem( i );
				
				if( pItemElem == NULL ) continue;

				if( pItemElem->GetTexture() )
				{
					if( 
						( pItemElem->m_nItemNum == pItemElem->GetExtra() || !m_useDieFlag ) 
							&& ( IsUsingItem(pItemElem) || pItemElem->m_bRepair 
								|| ( nParent == APP_INVENTORY && g_pPlayer->GetPetId() == pItemElem->m_dwObjId )
#ifdef __SYS_TICKET
								|| pPlayer->GetTicket() == pItemElem
#endif	// __SYS_TICKET
							) 
					)
						RanderIcon( p2DRender, pItemElem, x * 32, y * 32, nParent, 60 );
					else if( m_nOnSelect == i )
					{
						pItemElem->GetTexture()->m_size.cx += 4;
						pItemElem->GetTexture()->m_size.cy += 4;
						if( ( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime <= 0 )
#ifdef __SYS_TICKET
							|| pPlayer->GetTicket() == pItemElem
#endif	// __SYS_TICKET
							)
							RanderIcon( p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent, 100 );
						else
							RanderIcon( p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent );
						pItemElem->GetTexture()->m_size.cx -= 4;
						pItemElem->GetTexture()->m_size.cy -= 4;
					}
					else
					{
						if(pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime <= 0) //먹이 주머니 아이템이 사용안된 상태인가?
							RanderIcon( p2DRender, pItemElem, x * 32, y * 32, nParent, 100 );
						else						
							RanderIcon( p2DRender, pItemElem, x * 32, y * 32, nParent );
#ifdef __DEV
						if( pItemElem->GetAbilityOption() > 0 )
						{
							TCHAR szTemp[ 32 ];
							_stprintf( szTemp, "%d", pItemElem->GetAbilityOption() );
							CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
							m_p2DRender->TextOut( x * 32 + 32 - size.cx, y * 32 + 3, szTemp, D3DCOLOR_ARGB( 255, 255, 0, 0 ) );
							m_p2DRender->TextOut( x * 32 + 31 - size.cx, y * 32 + 2, szTemp, D3DCOLOR_ARGB( 255, 255, 176, 176 ) );
						}
#endif
					}

					if( pItemElem->GetProp()->dwPackMax > 1 )
					{
						short m_nItemNum	= pItemElem->m_nItemNum;
						if( pItemElem->GetExtra() > 0 )
							m_nItemNum	-= (short)pItemElem->GetExtra();			
						TCHAR szTemp[ 32 ];
						_stprintf( szTemp, "%d", m_nItemNum );
						CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
						m_p2DRender->TextOut( x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff0000ff );
						m_p2DRender->TextOut( x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xffb0b0f0 );
					}

					if (const auto cooldown = g_pPlayer->m_cooltimeMgr.GetElapsedTime(*pItemElem->GetProp())) {
						const CPoint pt(x * 32, y * 32);
						RenderRadar(p2DRender, pt, cooldown->elapsedTime, cooldown->totalWait);
					}

					CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
					if( pWndInventory )
					{
						if( pWndInventory->m_pUpgradeItem && pItemElem )
						{
							if( pWndInventory->m_pUpgradeItem == pItemElem )
							{
								if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff )
								{
									CPoint pt( x * 32, y * 32 );
									RenderEnchant( p2DRender, pt );
								}
							}
						}
					}
				}
				CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
			}
		}
		else
		{
			m_nArrayCount = 0;

			if( nParent == APP_SHOP_ )
			{
				CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase( APP_SHOP_ );
				for( int i = 0; i < m_pItemContainer->GetSize(); i++ ) 
				{
					CItemElem* pItemElem = GetItem( i );
					if( pItemElem == NULL ) 
						continue;

					ItemProp* pItemProp		= pItemElem->GetProp();
					if( pItemProp == NULL ) 
						continue;

					if( pWndBase )
					{
						bool emplaceItem = true;

						if (pWndBase->m_bSexSort && pItemProp->dwItemSex != 0xffffffff) {
							if (pItemProp->dwItemSex != g_pPlayer->GetSex()) {
								emplaceItem = false;
							}
						}

						if (pWndBase->m_bLevelSort && pItemProp->dwLimitLevel1 != 0xffffffff) {
							if (pItemElem->IsLimitLevel(g_pPlayer)) {
								emplaceItem = false;
							}
						}
						
						if (emplaceItem) {
							m_pArrayItemElem[m_nArrayCount++] = pItemElem;
						}

					}
				}
			}
			else
			{
				for( int i = 0; i < m_pItemContainer->GetSize(); i++ ) 
				{
					CItemElem* pItemElem = GetItem( i );	
					if( pItemElem == NULL ) 
						continue;
					
					ItemProp* pItemProp		= pItemElem->GetProp();					
					if( pItemProp == NULL ) 
						continue;
					
					m_pArrayItemElem[m_nArrayCount++] = pItemElem;
				}
			}

			for( int i = 0; i < m_nArrayCount; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				CItemElem* pItemElem = m_pArrayItemElem[i];
				if( pItemElem == NULL ) 
					continue;

				if( pItemElem->m_pTexture )
				{
					if( IsUsingItem( pItemElem ) || pItemElem->m_bRepair )
						RanderIcon( p2DRender, pItemElem, x * 32, y * 32, nParent, 60 );
					else 
					if( m_nOnSelect == i )
					{
						pItemElem->GetTexture()->m_size.cx += 4;
						pItemElem->GetTexture()->m_size.cy += 4;
						RanderIcon( p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent );
						pItemElem->GetTexture()->m_size.cx -= 4;
						pItemElem->GetTexture()->m_size.cy -= 4;
					}
					else
						RanderIcon( p2DRender, pItemElem, x * 32, y * 32, nParent );

					if( pItemElem->GetProp()->dwPackMax > 1 )
					{
						short m_nItemNum	= pItemElem->m_nItemNum;
						if( pItemElem->GetExtra() > 0 )
							m_nItemNum	-= pItemElem->GetExtra();			
						if( m_bVisibleCount )  
						{
							TCHAR szTemp[ 32 ];
							_stprintf( szTemp, "%d", m_nItemNum );
							CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
							m_p2DRender->TextOut( x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff0000ff );
							m_p2DRender->TextOut( x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xffb0b0f0 );
						}
					}
				}
			}
		}
	
}
void CWndItemCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( !g_pPlayer )
		return;

	if( !m_pItemContainer )
		return;

	CPoint pt( 3, 3 );
	CRect rect;
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;
	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;
	//SetCapture();
	int nItem = HitTest( point );

	if( nItem == -1 )	return;

	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemElem* pItemElem = NULL;

	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
	{
		if(!(nParent == APP_BAG_EX)) 

		if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff || GetWndBase(APP_SMELT_SAFETY_CONFIRM) != NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );			
			m_pFocusItem = NULL;
			return;
		}

		if( GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01) );
			m_pFocusItem = NULL;
			return;
		}

		if( GetWndBase(APP_COMMITEM_DIALOG) != NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR) );
			m_pFocusItem = NULL;
			return;
		}

		pItemElem = GetItem( nItem );
	}
	else
	{
		pItemElem = m_pArrayItemElem[nItem];
	}
	
	if( pItemElem )
	{
		if( (pItemElem->m_nItemNum != pItemElem->GetExtra() && m_useDieFlag) || 
			( !IsUsingItem( pItemElem ) && pItemElem->m_bRepair == FALSE) )
		{
			if( m_useDieFlag || IsSelectedItem( nItem ) == FALSE )
			{
				m_dwSelectAry.RemoveAll();
				m_dwSelectAry.Add( nItem );
				m_nCurSelect = nItem;
				m_pFocusItem = pItemElem;
			}
			m_bDrag = TRUE;
		}

		// 인첸트할 아이템을 선택하였다면 실행한다.
		// 클라에서 4초정도 이펙트 출력후 패킷을 날린다..
		if( nParent == APP_INVENTORY && pWndInventory )
		{
			CPoint pt( 3, 3 );

			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;

			// 인첸, 피어싱관련 이펙트 출력위치   
			int nPage = nHeight;
			int nRange = m_pItemContainer->m_dwIndexNum / nWidth;
			if( m_pItemContainer->m_dwIndexNum % nWidth )
				nRange++;
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

			int x = ( nItem - pt.y ) % nWidth;
			int y = ( nItem - pt.y ) / nWidth;

			if(pWndInventory->m_bIsUpgradeMode && pWndInventory->m_pUpgradeMaterialItem != NULL && m_pFocusItem != NULL)
			{
				if(!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT) && 
					( pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM01 || pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM01_1 ) && 
					m_pFocusItem->GetAbilityOption() >= 3 && CItemElem::IsDiceRefineryAble(m_pFocusItem->GetProp()))
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR1);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if(!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3) && pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM02 &&
						m_pFocusItem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR2);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if(!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4) && m_pFocusItem->GetAbilityOption() >= 3 && 
						(pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE || pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE_1) &&
						m_pFocusItem->GetProp()->dwItemKind2 == IK2_JEWELRY)
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR3);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if( g_pPlayer->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT ) == FALSE && 
						 m_pFocusItem->GetResistAbilityOption() >= 3 && 
						 CItemElem::IsElementalCard( pWndInventory->m_pUpgradeMaterialItem->m_dwItemId ) == TRUE && 
						 CItemElem::IsEleRefineryAble( m_pFocusItem->GetProp() ) == TRUE )
				{
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
						SAFE_DELETE( g_WndMng.m_pWndSmeltSafetyConfirm )

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm( CWndSmeltSafetyConfirm::WND_ERROR1 );
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode( m_pFocusItem );
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize( NULL );
					}
				}
				else
					pWndInventory->RunUpgrade(m_pFocusItem);
			}
		}
	}
	else
	{
		m_dwSelectAry.RemoveAll();
		m_nCurSelect	= nItem;
		m_pFocusItem	= NULL;
//		m_bDrag		= FALSE;
	}
	//SetCapture();
}

void CWndItemCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( !m_pItemContainer )
		return;

	//ReleaseCapture();
	m_bDrag = FALSE;
	//if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
	//	return;
	int nItem = HitTest( point );
	if( nItem == -1 ) return;

	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemElem* pItemElem = NULL;

	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
		pItemElem = GetItem( nItem );
	else
		pItemElem = m_pArrayItemElem[nItem];
	
	if( pItemElem )
	{
		if( (GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) )
		{
			int j = NULL;
			for( ; j < m_dwSelectAry.GetSize(); j++ )
			{
				if( m_dwSelectAry.GetAt( j ) == nItem )
					break;
			}
			if( j == m_dwSelectAry.GetSize() )
				m_dwSelectAry.Add( nItem );
			else
			if( m_nCurSelect != nItem )
				m_dwSelectAry.RemoveAt( j );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		//	m_rect = rect;
		}
		else
		if( IsSelectedItem( nItem ) )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}
	}
}
void CWndItemCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !m_pItemContainer )
		return;

	if( m_bDrag == FALSE )
		return;
	//TRACE( " hello \n" );
	m_bDrag = FALSE;
	CPoint pt( 3, 3 );
	CRect rect;
	//if( m_rect.PtInRect( point ) )
	//{
	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemElem* pItemElem = NULL;

	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
		pItemElem = GetItem( m_nCurSelect ); //
	else
		pItemElem = m_pArrayItemElem[m_nCurSelect];
	
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(!pWndInventory && nParent == APP_BAG_EX && pItemElem != NULL)
	{
		if(IsUsableItem( pItemElem ))
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = ShortcutType::Item;
			m_GlobalShortcut.m_dwIndex    = m_nCurSelect;// pItemElem->m_dwItemId; //m_nCurSelect;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwId       = pItemElem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_pTexture   = pItemElem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) pItemElem;
			
			if(pItemElem->GetProp() == NULL)
			{
				CString strErr;
				strErr.Format( "CWndItemCtrl::OnMouseMove에서 GetProp가 NULL\nItemType : %d, ItemId : %d ", 0, pItemElem->m_dwItemId );
				//ADDERRORMSG(strErr);
			}
			_tcscpy( m_GlobalShortcut.m_szString, pItemElem->GetName() );
		}
		return;
	}
	if( ((pItemElem != NULL && m_useDieFlag) || IsUsableItem( pItemElem )) && !pWndInventory->m_bRemoveJewel)
	{
		m_GlobalShortcut.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwShortcut = ShortcutType::Item;
		m_GlobalShortcut.m_dwIndex    = m_nCurSelect;// pItemElem->m_dwItemId; //m_nCurSelect;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
		m_GlobalShortcut.m_dwId       = pItemElem->m_dwObjId;//(DWORD)pItemElem;
		m_GlobalShortcut.m_pTexture   = pItemElem->GetTexture();
		m_GlobalShortcut.m_dwData     = (DWORD) pItemElem;
		
		if(pItemElem->GetProp() == NULL)
		{
			CString strErr;
			strErr.Format( "CWndItemCtrl::OnMouseMove에서 GetProp가 NULL\nItemType : %d, ItemId : %d ", 0, pItemElem->m_dwItemId );
			//ADDERRORMSG(strErr);
		}
		_tcscpy( m_GlobalShortcut.m_szString, pItemElem->GetName() );
	}
}
CItemElem* CWndItemCtrl::GetItem( DWORD dwIndex )
{
	return m_pItemContainer->GetAt( dwIndex );
}

CItemElem* CWndItemCtrl::GetItemFromArr( DWORD dwIndex )
{
	ASSERT(dwIndex < 100);
	return m_pArrayItemElem[dwIndex];
}

int CWndItemCtrl::HitTest( CPoint point )
{
	if( !m_pItemContainer )
		return -1;

	int nDstId = -1;

		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		CPoint pt( 3, 3 );

		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

		CWndBase* pWndBase = this->GetParentWnd();
		
		int nParent;
		if( pWndBase )
		{
			CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
			if( pWndBaseTemp )
				nParent = pWndBaseTemp->GetWndId();
			else
				return nDstId;
		}
		else
		{
			return nDstId;
		}

		if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
		{
			for( int i = pt.y; i < (int)( m_pItemContainer->m_dwIndexNum ); i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
		else
		{
			for( int i = pt.y; i < m_nArrayCount; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
	
	return nDstId;
}
BOOL CWndItemCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( !m_pItemContainer )
		return FALSE;

	if( pShortcut->m_dwShortcut == ShortcutType::Item)
	{
		int nDstIndex = HitTest( point );//
		if( nDstIndex == -1 ) nDstIndex = m_pItemContainer->m_dwIndexNum;
//			return FALSE;

		if( pShortcut->m_pFromWnd == this )
		{

			CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
			if( pWndFrame->GetWndId() == APP_BANK ) // 42 // Bank
			{
				g_DPlay.SendPacket<PACKETTYPE_MOVEBANKITEM, BYTE, BYTE>(
					static_cast<BYTE>(pShortcut->m_dwIndex),
					static_cast<BYTE>(static_cast<DWORD>(nDstIndex))
					);
			}
			else if( pWndFrame->GetWndId() == APP_INVENTORY )	// inventory -> inventory
			{
				CItemElem* pItemElem	= m_pItemContainer->GetAt( pShortcut->m_dwIndex );
				if (!IsUsingItem(pItemElem)) {
					g_DPlay.SendPacket<PACKETTYPE_MOVEITEM, BYTE, BYTE>(
						static_cast<BYTE>(pShortcut->m_dwIndex),
						static_cast<BYTE>(static_cast<DWORD>(nDstIndex))
						);
				}
			}
		}
		else
		{
			CWndBase* pParent = (CWndBase*)GetParentWnd();
			pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
		}
		return TRUE;
	}
	return FALSE;
}
/*
void CWndItemCtrl::OnDropIcon( DWORD dwShorCutType, DWORD dwShortCutIndex, CPoint point )
{
	if( dwShorCutType == SHORTCUT_ITEM )
	{
		if( m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
	CPoint pt( 3, 3 );

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
			for( int i = pt.y; i < m_pItemContainer->m_dwIndexNum; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				CItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ i ] ] ;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					for( int j = m_pItemContainer->m_dwIndexNum; j > i; j-- )
						m_pItemContainer->m_apIndex[ j ] = m_pItemContainer->m_apIndex[ j - 1 ];
					m_pItemContainer->m_apIndex[ j ] = dwItemOld;
					(m_pItemContainer->m_dwIndexNum)++;
					return;
				}
			}
			m_pItemContainer->m_apIndex[ m_pItemContainer->m_dwIndexNum ] = dwItemOld;
			(m_pItemContainer->m_dwIndexNum)++;
		}



		_pItemContainer->Swap( CWndBase::m_GlobalShortcut.m_dwData, 
		
		CItemElem* pItemOld = &m_apItem[ CWndBase::m_GlobalShortcut.m_dwData ];
		for( int i = 0; m_pItemContainer->m_dwIndexNum; i++ )
		{
			CItemElem* pItemElem = &m_apItem[ i ];
			if( pItemElem->m_dwItemId )
			{
				*pItemElem = *pItemOld;
				pItemOld->m_dwItemId = 0;
			}
		}
		
	}
}*/
BOOL CWndItemCtrl::IsSelectedItem( int iItem )
{
	for( int i = 0; i < m_dwSelectAry.GetSize(); i++ )
		if( m_dwSelectAry.GetAt( i ) == iItem )
			return TRUE;
	return FALSE;
}
int CWndItemCtrl::GetSelectedItem( int i )
{
	return m_dwSelectAry.GetAt( i );
}
UINT CWndItemCtrl::GetSelectedCount()
{
	return m_dwSelectAry.GetSize();
}

void CWndItemCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}
void CWndItemCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );

	if( pWndInventory && pWndInventory->m_dwEnchantWaitTime == 0xffffffff )
	{
		pWndInventory->BaseMouseCursor();
		if(g_WndMng.m_pWndSmeltSafetyConfirm != NULL)
			SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)
	}
	
	if( !m_pItemContainer )
		return;

	CRect rectCtrl = GetClientRect();

	int nWidth = rectCtrl.Width() / 32;
	int nHeight = rectCtrl.Height() / 32;

	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for( int i=pt.y ; i < m_pItemContainer->GetSize() ; i++ )
	{
		if( i < 0 ) continue;
		int x = ( i - pt.y ) % nWidth;
		int y = ( i - pt.y ) / nWidth;

		CItemElem* pItemElem = GetItem( i );
		if( pItemElem == NULL ) continue;
		
		CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			const ItemProp* pItemProp = pItemElem->GetProp();

			if( pItemProp->dwItemKind2 == IK2_TEXT ) 
			{
				CString strText = pItemProp->szTextFileName;
				strText.MakeLower();

				if (strText.Find("book_") != -1) {
					g_WndMng.OpenItemInfo(this, CWndMgr::ItemInfoType::Book, pItemElem);
				} else if (strText.Find("scroll_") != -1) {
					g_WndMng.OpenItemInfo(this, CWndMgr::ItemInfoType::Scroll, pItemElem);
				} else if (strText.Find("letter_") != -1) {
					g_WndMng.OpenItemInfo(this, CWndMgr::ItemInfoType::Letter, pItemElem);
				}

			} else if (pItemProp->dwItemKind3 == IK3_QUEST) {
				g_WndMng.OpenItemInfo(this, CWndMgr::ItemInfoType::QuestItem, pItemElem);
			}
		}
	}
}
BOOL CWndItemCtrl::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );	
	if( pWndInventory )
		pWndInventory->SetEnchantCursor();
	
	return TRUE;
}

void CWndItemCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( !g_pPlayer )
		return;

	if( !m_pItemContainer )
		return;

	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;
	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;

	int nItem = HitTest( point );

	if( nItem == -1 )	return;

	CItemElem* pItemElem = NULL;
	pItemElem = GetItem( nItem );

	if( IsUsableItem( pItemElem ) || (m_useDieFlag && pItemElem != NULL))
	{
		if( IsSelectedItem( nItem ) == FALSE )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}

		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WIN_DBLCLK,m_nIdWnd,(LRESULT*)m_pFocusItem); 
	}
}
void CWndItemCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndItemCtrl::OnSize( UINT nType, int cx, int cy )
{
//	m_string.Reset( g_2DRender.m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndItemCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 0 );

	if( m_pItemContainer ) 
	{
			m_rectClient.right -= 15; // 스크롤 바가 보이면 
	}
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndItemCtrl::PaintFrame( C2DRender* p2DRender )
{
}

BOOL CWndItemCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

BOOL CWndItemCtrl::OnMouseWheel(UINT, short zDelta, CPoint) {
	m_wndScrollBar.MouseWheel(zDelta);
	return TRUE;
}

void CWndItemCtrl::SetDieFlag(BOOL flag)
{
	this->m_useDieFlag = flag;
}
