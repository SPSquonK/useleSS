#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndCommItem.h"
#include "DPClient.h"


/****************************************************
  WndId : APP_COMM_ITEM - 상용 ITEM 지속일자
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

void CWndCommItem::OnInitialUpdate() 
{ 
#ifdef __TAIWAN__
	Destroy();
#endif // __TAIWAN__

	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndTabCtrl * pWndTabCtrl = GetDlgItem<CWndTabCtrl>(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	m_wndCommItemCtrl.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COMM_ITEM );

	pWndTabCtrl->InsertItem(&m_wndCommItemCtrl, prj.GetText(TID_TOOLTIP_ITEMTIME));

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCommItem::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_COMM_ITEM, pWndParent, 0, CPoint( 0, 0 ) );
} 





///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndPartyCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndCommItemCtrl::CWndCommItemCtrl() 
{
	m_pVBGauge = NULL;
	pWndWorld = NULL;
}
CWndCommItemCtrl::~CWndCommItemCtrl()
{
	DeleteDeviceObjects();
}

void CWndCommItemCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

HRESULT CWndCommItemCtrl::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndCommItemCtrl::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndCommItemCtrl::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
void CWndCommItemCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	m_nFontHeight = 30;

	pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( MakePath( DIR_THEME, "GauFillSmall.bmp" ), 0xffff00ff, TRUE );
}
void CWndCommItemCtrl::OnMouseWndSurface( CPoint point )
{
	CPoint pt( 3, 3 );
	
	if( NULL == pWndWorld )
	{
		pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		return;
	}
	
	int nCountinueCount = 0;

	for (const DWORD itemId : m_dwDraw) {
		int x = 0, nWidth = m_rectClient.Width();// - 1;
		CRect rectHittest = CRect( x + 3, pt.y, x + 3 + nWidth, pt.y + 32 );// pt.x, pt.y+ 12, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight+ 12 );

		if( rectHittest.PtInRect( point ) )
		{
			ClientToScreen( &point );
			ClientToScreen( &rectHittest );
			ItemProp* pItem = prj.GetItemProp(itemId);
			if( pItem == NULL )
				continue;

			CEditString str;
			CString strTemp;
			CString strEnter = '\n';
			str.AddString( pItem->szName, 0xff0000ff, ESSTY_BOLD );
			str += strEnter;
			if( pItem->dwID == II_SYS_SYS_SCR_BLESSING )
			{
				strTemp.Format( prj.GetText( TID_GAME_COND_USE ) );	// 지속시간 : 
				str += strTemp;
			}
			else if( pItem->dwID == II_SYS_SYS_SCR_SMELPROT || pItem->dwID == II_SYS_SYS_SCR_SMELTING
#ifdef __SM_ITEM_2ND_EX
				|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT2
#endif	// __SM_ITEM_2ND_EX
				|| pItem->dwID == II_SYS_SYS_SCR_SUPERSMELTING
				|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT3
				|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT4
				|| pItem->dwID == II_SYS_SYS_SCR_SMELTING2
			)
			{
				strTemp.Format( "%s", prj.GetText( TID_GAME_DEMOL_USE ) );
				str += strTemp;
			}
			else if( pItem->dwItemKind3 == IK3_ANGEL_BUFF )
			{
				strTemp.Format( "%s", prj.GetText( TID_GAME_TIP_ANGEL_PLAIN ) );
				str += strTemp;
			}
#ifdef __SYS_TICKET
			else if( pItem->dwItemKind3 == IK3_TICKET )
			{
				CItemElem* pTicket	= g_pPlayer->GetTicket();
				CString str1, str2;
				if( pTicket )
				{
					time_t t	= pTicket->m_dwKeepTime - time_null();
					if( t > 0 )
					{
						CTimeSpan time( t );
						str1.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds() );
					}

					str2	= str1 + prj.GetText( TID_TOOLTIP_PERIOD );
					str		+= str2;
				}
				else
				{
					str		= "";
				}
			}
#endif	// __SYS_TICKET

			else if( pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET )
			{
				//검색해서 활성화 된 먹이 주머니를 찾는다.
				CItemElem* ptr;
				CItemElem* pItemElem = NULL;

				int nMax = g_pPlayer->m_Inventory.GetMax();
				for( int i = 0 ; i < nMax; i++ )
				{
					ptr	= g_pPlayer->m_Inventory.GetAtId( i );
					if( IsUsableItem( ptr ) && ptr->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET &&
						ptr->m_dwKeepTime > 0 && !ptr->IsFlag( CItemElem::expired ) )	// 활성화한 먹이 주머니일 경우
					{						
						pItemElem = ptr;
						i = nMax;
					}
				}

				if(pItemElem != NULL)
				{
					CString strTemp;
					//사용 제한 시한
					time_t t = pItemElem->m_dwKeepTime - time_null();
//					if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( CItemElem::expired ) )
					{
						if( t > 0 )
						{
							CTimeSpan ct( t );
							strTemp.Format( prj.GetText( TID_TOOLTIP_ITEMTIME ) );	// 지속시간 : 
							str += strTemp;
							strTemp.Format( " : " );
							str += strTemp;
							strTemp.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// 지속시간 : 
							str += strTemp;
						}
					}
				}
			}
			else if( pItem->dwItemKind2 == IK2_BUFF2 )
			{
				IBuff* pBuff	= g_pPlayer->m_buffs.GetBuff( BUFF_ITEM2, (WORD)( pItem->dwID ) );
				time_t				t = 0;

				if( !pBuff )
				{
					t = 0;
				}
				else
				{
					t = (time_t)pBuff->GetLevel() - time_null();
					if( t < 0 )  t = 0;
				}
				
				CTimeSpan ts( t );

				strTemp.Format( prj.GetText( TID_TOOLTIP_ITEMTIME ) );	// 지속시간 : 
				str += strTemp;
				strTemp.Format( " : " );
				str += strTemp;
				strTemp.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ts.GetDays()), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds() );	// 지속시간 : 
				str += strTemp;
			}
			else
			{
				//TODO_미송님
				long lData;
				if( pItem->dwCircleTime == (DWORD)-1 )
					lData = (long)(pItem->dwSkillTime / 1000.0f);
				else
					lData = pItem->dwCircleTime;

				CTimeSpan ct( lData );
				strTemp.Format( prj.GetText( TID_TOOLTIP_ITEMTIME ) );	// 지속시간 : 
				str += strTemp;
				strTemp.Format( " : " );
				str += strTemp;

#ifdef __BS_ITEM_UNLIMITEDTIME
				//gmpbigsun : 지속시간이 999999999 이상이고, 다른 유지시간이 없을경우 무제한처리
				if( pItem->dwSkillTime >= 0x3b9ac9ff && pItem->dwCircleTime == (DWORD)-1 && pItem->dwAbilityMin == (DWORD)-1 )		
				{
					strTemp = prj.GetText( TID_GAME_TOOLTIP_PERMANENTTIME_1 ); //"UNLIMITED";
					str += strTemp;
				}
				else
#endif //__BS_ITEM_UNLIMITEDTIME
				{
					strTemp.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// 지속시간 : 
					str += strTemp;
				}
			}
				
			str += strEnter;
			strTemp.Format( prj.GetText( TID_TOOLTIP_USE ), pItem->szCommand );	// 용도 :
			str += strTemp;
			g_toolTip.PutToolTip(itemId, str, rectHittest, point, 0);
		}
		pt.y += m_nFontHeight;
	}
}

// 출력해야할 MAX값얻어오기
int CWndCommItemCtrl::GetMaxBuff() const {
	int nMaxCount = 0;
	for (int i = 0; i < SM_MAX; ++i) {
		if (g_pPlayer->m_dwSMTime[i] > 0) {
			++nMaxCount;
		}
	}

	nMaxCount	+= g_pPlayer->m_buffs.GetCommercialCount();
	return nMaxCount;
}

void CWndCommItemCtrl::DrawSM( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll )
{
	for( int i = 0 ; i < SM_MAX; ++i )
	{
		if( g_pPlayer->m_dwSMTime[i] <= 0 )
			continue;
		
		++nScroll;
		if(m_wndScrollBar.GetScrollPos() >= nScroll)
			continue;

		CTexture * pTexture = pWndWorld->GetSMBuffTexture(*g_pPlayer, i);
		
		const ItemProp * pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[i]);

		if (pItem && pTexture) {
			p2DRender->RenderTexture(CPoint(2, pPoint->y), pTexture, 192);
		}

		m_dwDraw.emplace_back(g_AddSMMode.dwSMItemID[i]);
			
		CString string;
		string.Format( "#cff0000ff%s#nc", pItem ? pItem->szName : "???");
		CEditString strEdit;
		strEdit.SetParsingString( string );
		p2DRender->TextOut_EditString( x + 40, pPoint->y + 3, strEdit, 0, 0, 2 );
	
		CTimeSpan ct( g_pPlayer->m_dwSMTime[i] );
		if( pItem && pItem->dwID == II_SYS_SYS_SCR_BLESSING )
			string = prj.GetText( TID_GAME_COND_USE );
		else
			string.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
		p2DRender->TextOut( x + 40, pPoint->y + 18, string, 0xff000000 );
		pPoint->y += m_nFontHeight;// + 3;
	}
}

void CWndCommItemCtrl::DrawSkill( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll )
{
	ItemProp* pItem = NULL;
	std::multimap<DWORD, BUFFSKILL>::value_type* pp = NULL;
	BOOL bExpRander[6];
	BOOL IsOverlap;

	for( int iRander = 0 ; iRander < 6 ; ++iRander )
		bExpRander[iRander]		= TRUE;
	for( auto it1 = g_pPlayer->m_buffs.m_mapBuffs.begin(); it1 != g_pPlayer->m_buffs.m_mapBuffs.end(); ++it1 )
	{
		IBuff* ptr1	= it1->second;
		if( ptr1->GetType() == BUFF_SKILL || ptr1->GetType() == BUFF_EQUIP )
			continue;
		DWORD dwSkillID		= ptr1->GetId();
		int nExpCount	= 0;
		DWORD dwExpTime[3];
		ZeroMemory( dwExpTime, sizeof(dwExpTime) );
		if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESA1 || dwSkillID == II_SYS_SYS_SCR_AMPESA2 )	
		{
			dwSkillID	= II_SYS_SYS_SCR_AMPESA;
			BOOL IsOverlap	= TRUE;
			if( bExpRander[0] == FALSE )
				continue;
		}
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESB || dwSkillID == II_SYS_SYS_SCR_AMPESB1 || dwSkillID == II_SYS_SYS_SCR_AMPESB2 )	
		{
			dwSkillID	= II_SYS_SYS_SCR_AMPESB;
			BOOL IsOverlap	= TRUE;
			if( bExpRander[1] == FALSE )
				continue;
		}
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESC1 || dwSkillID == II_SYS_SYS_SCR_AMPESC2 )	
		{
			dwSkillID	= II_SYS_SYS_SCR_AMPESC;
			BOOL IsOverlap	= TRUE;
			if( bExpRander[2] == FALSE )
				continue;
		}
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESD || dwSkillID == II_SYS_SYS_SCR_AMPESD1 || dwSkillID == II_SYS_SYS_SCR_AMPESD2 )	
		{
			dwSkillID	= II_SYS_SYS_SCR_AMPESD;
			BOOL IsOverlap	= TRUE;
			if( bExpRander[3] == FALSE )
				continue;
		}
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESE )
		{
			dwSkillID	= II_SYS_SYS_SCR_AMPESE;
			IsOverlap	= FALSE;
			if( bExpRander[4] == FALSE )
				continue;
		}
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESS || dwSkillID == II_SYS_SYS_SCR_AMPESS1 || dwSkillID == II_SYS_SYS_SCR_AMPESS2 )	
		{
			dwSkillID	= II_SYS_SYS_SCR_AMPESS;
			BOOL IsOverlap	= TRUE;
			if( bExpRander[5] == FALSE )
				continue;
		}

		if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESB
			|| dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESD  
			|| dwSkillID == II_SYS_SYS_SCR_AMPESE
			|| dwSkillID == II_SYS_SYS_SCR_AMPESS
			)
		{
			for (const auto & [_, ptr2] : g_pPlayer->m_buffs.m_mapBuffs) {
				if( dwSkillID == II_SYS_SYS_SCR_AMPESE )	// 중복되지 않는 ES증폭의 두루마리 defineitem에서 처리 잘못되어 따로 분기
				{
					if( ptr2->GetId() == dwSkillID )
					{
						dwExpTime[0]	= ptr2->GetTotal() - ( g_tmCurrent - ptr2->GetInst() );
						nExpCount	= 1;
						break;
					}
				}
				else
				{
					if( ptr2->GetId() == dwSkillID || ptr2->GetId() == dwSkillID+1 || ptr2->GetId() == dwSkillID+2 )	
					{
						if(IsOverlap)
						{
							dwExpTime[nExpCount] = ptr2->GetTotal()  - ( g_tmCurrent - ptr2->GetInst() );
							++nExpCount;				
						}
						else
						{
							dwExpTime[0] = ptr2->GetTotal() - ( g_tmCurrent - ptr2->GetInst() );
							nExpCount = 1;
							break;
						}
					}
				}
			}
		}				
		pItem = prj.GetItemProp( dwSkillID );
		if( pItem->bCharged == FALSE )
			continue;
		if( pItem->dwItemKind3 == IK3_EGG )
			continue;
		if( pItem->dwItemKind1 == IK1_HOUSING )
			continue;

		if(pWndWorld->m_pBuffTexture[2].find(dwSkillID) != pWndWorld->m_pBuffTexture[2].end())
			pp	= &( *( pWndWorld->m_pBuffTexture[2].find( dwSkillID ) ) );
		if(pp == NULL)
			continue;
		if( pp->second.m_pTexture == NULL )
			continue;

		++nScroll;
		if( ( m_wndScrollBar.GetScrollPos() >= nScroll ) )
			continue;

		p2DRender->RenderTexture( CPoint( 2, pPoint->y ), pp->second.m_pTexture, 192 );

		DWORD dwColor = 0xff000000;
		CString string;
		string.Format( "#cff0000ff%s#nc", pItem->szName );
		CEditString strEdit;
		strEdit.SetParsingString( string );
		p2DRender->TextOut_EditString( x + 40, pPoint->y + 3, strEdit, 0, 0, 2 );

		if( pItem->dwID == II_SYS_SYS_SCR_SMELPROT || pItem->dwID == II_SYS_SYS_SCR_SMELTING
#ifdef __SM_ITEM_2ND_EX
			|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT2
#endif	// __SM_ITEM_2ND_EX
			|| pItem->dwID == II_SYS_SYS_SCR_SUPERSMELTING
			|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT3
			|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT4
			|| pItem->dwID == II_SYS_SYS_SCR_SMELTING2
		)
		{
			string.Format( "%s", prj.GetText( TID_GAME_DEMOL_USE ) );
		}
		else if( pItem->dwID == II_SYS_SYS_SCR_RETURN )
		{
			string.Format( "%s", prj.GetText( TID_TOOLTIP_RETURN_USE ) );
		}
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESB
			|| dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESD
			|| dwSkillID == II_SYS_SYS_SCR_AMPESE
			|| dwSkillID == II_SYS_SYS_SCR_AMPESS
			)
		{
			string = "";
			for( int k = 0 ; k < nExpCount ; ++k )
			{
				DWORD dwOddTime = dwExpTime[k];
				CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
				CString strTemp;
				strTemp.Format( "%.1d:%.2d:%.2d ", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
				string += strTemp;
				if( k + 1 != nExpCount )
					string += "/ ";
			}
		}
		else
		{
			if( pItem->dwItemKind3 == IK3_ANGEL_BUFF )
			{
				string.Format( prj.GetText( TID_GAME_TIP_ANGEL_PLAIN ) );	
			}
			else if( pItem->dwItemKind2 == IK2_BUFF2 )
			{
				time_t	t = (time_t)ptr1->GetLevel() - time_null();
				if( t < 0 )
					t	= 0;
				CTimeSpan ts( t );

				CString strTemp, str;
				strTemp.Format( prj.GetText( TID_TOOLTIP_ITEMTIME ) );	// 지속시간 : 
				str += strTemp;
				strTemp.Format( " : " );
				str += strTemp;
				strTemp.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ts.GetDays()), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds() );	// 지속시간 : 
				str += strTemp;
				string	= str;	
			}
			else if( pItem->dwItemKind3 == IK3_TICKET )
			{
				CItemElem* pTicket	= g_pPlayer->GetTicket();
				CString str;
				if( pTicket )
				{
					time_t t	= pTicket->m_dwKeepTime - time_null();
					if( t > 0 )
					{
						CTimeSpan time( t );
						str.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds() );
					}
					string	= str + prj.GetText( TID_TOOLTIP_PERIOD );
				}
				else
				{
					string	= "";
				}
			}
			else if( pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET )
			{
				//검색해서 활성화 된 먹이 주머니를 찾는다.
				CItemElem* ptr;
				CItemElem* pItemElem = NULL;

				int nMax = g_pPlayer->m_Inventory.GetMax();
				for( int i = 0 ; i < nMax; i++ )
				{
					ptr	= g_pPlayer->m_Inventory.GetAtId( i );
					if( IsUsableItem( ptr ) && ptr->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET &&
						ptr->m_dwKeepTime > 0 && !ptr->IsFlag( CItemElem::expired ) )	// 활성화한 먹이 주머니일 경우
					{						
						pItemElem = ptr;
						i = nMax;
					}
				}

				if(pItemElem != NULL)
				{
					CString strTemp;
					//사용 제한 시한
					time_t t = pItemElem->m_dwKeepTime - time_null();
//					if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( CItemElem::expired ) )
					{
						if( t > 0 )
						{
							CTimeSpan ct( t );
							string.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
						}
					}
				}
			}
			else
			{

#ifdef __BS_ITEM_UNLIMITEDTIME
				if( pItem->dwSkillTime >= 999999999)		//gmpbigsun : (999999999) 시간제한 없음 
				{
					string = prj.GetText( TID_GAME_TOOLTIP_PERMANENTTIME_1 ); // 시간 무제한
				}
				else
#endif //__BS_ITEM_UNLIMITEDTIME
				{
					DWORD dwOddTime = ptr1->GetTotal() - ( g_tmCurrent - ptr1->GetInst() );
					CTimeSpan ct( (dwOddTime / 1000 ) );		// 남은시간을 초단위로 변환해서 넘겨줌
					string	= prj.GetText( TID_TOOLTIP_DATE );
					string.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>( ct.GetDays() ), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
				}
			}
		}

		p2DRender->TextOut( x + 40, pPoint->y + 18, string, dwColor );
		
		pPoint->y += m_nFontHeight;// + 3;
		m_dwDraw.emplace_back(pItem->dwID);

		if( dwSkillID == II_SYS_SYS_SCR_AMPESA )
			bExpRander[0] = FALSE;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESB )
			bExpRander[1] = FALSE;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESC )
			bExpRander[2] = FALSE;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESD )
			bExpRander[3] = FALSE;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESE )
			bExpRander[4] = FALSE;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESS )
			bExpRander[5] = FALSE;
	}
}

void CWndCommItemCtrl::OnDraw( C2DRender* p2DRender ) {
	if (!pWndWorld) {
		pWndWorld = (CWndWorld *)g_WndMng.GetWndBase(APP_WORLD);
		return;
	}

	if (!g_pPlayer) return;
	
	m_dwDraw.clear();

	CPoint pt(3, 3);
	int x = 0;
	int nScroll = 0;
	DrawSM( p2DRender, &pt, x, nScroll );
	DrawSkill( p2DRender, &pt, x, nScroll );
}

void CWndCommItemCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	const int nPage = GetClientRect().Height() / m_nFontHeight;
	const int nRange = GetMaxBuff();
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	
	CWndBase::OnSize( nType, cx, cy);
}
void CWndCommItemCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndCommItemCtrl::PaintFrame( C2DRender* p2DRender )
{
	const int nPage = GetClientRect().Height() / ( m_nFontHeight );
	const int nRange = GetMaxBuff();

	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}
BOOL CWndCommItemCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

/****************************************************
  WndId : APP_COMMERCIAL_ELEM - 상용 속성 아이템 사용
  CtrlId : WIDC_STATIC1 - 속성 능력
  CtrlId : WIDC_STATIC3 - 아이템
  CtrlId : WIDC_STATIC4 - 속성 아이템
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_EDIT2 - 
  CtrlId : WIDC_CUSTOM1 - Custom
  CtrlId : WIDC_CUSTOM2 - Custom
  CtrlId : WIDC_ELEM_OK - Button
  CtrlId : WIDC_ELEM_CANCEL - Button
****************************************************/

CWndCommercialElem::CWndCommercialElem() 
{ 
	SetPutRegInfo( FALSE );
} 
CWndCommercialElem::~CWndCommercialElem() { m_slots.Clear(); }

void CWndCommercialElem::OnDraw( C2DRender* p2DRender )  { 
	m_slots.Draw(p2DRender, this);
} 

void CWndCommercialElem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_slots.Clear();
	m_slots.SetRects({
		GetWndCtrl(WIDC_CUSTOM1)->rect,
		GetWndCtrl(WIDC_CUSTOM2)->rect
		});

	// 윈도를 중앙으로 옮기는 부분.
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
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCommercialElem::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_COMMERCIAL_ELEM, pWndParent, 0, CPoint( 0, 0 ) );
} 


void CWndCommercialElem::OnRButtonUp(UINT nFlags, CPoint point) {
	if (m_slots[0].IsIn(point)) {
		m_slots.Clear();
	} else if (m_slots[1].IsIn(point)) {
		m_slots[1].Clear();
	}
}

BOOL CWndCommercialElem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_ELEM_OK:
		{
			if(m_slots[0] && m_slots[1]) {
				g_DPlay.SendCommercialElem(m_slots[0]->m_dwObjId, m_slots[1]->m_dwObjId );
				m_slots.Clear();
				Destroy();
			}
		}
		break;
	case WIDC_ELEM_CANCEL:
		m_slots.Clear();
		Destroy();
		break;
	case WTBID_CLOSE:
		m_slots.Clear();
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


BOOL CWndCommercialElem::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	
	// 아이템이 인벤토리에서 왔는가?
	if( pShortcut->m_dwShortcut == ShortcutType::Item && pWndFrame->GetWndId() == APP_INVENTORY )
	{
		BOOL bbid = FALSE;
		CItemElem * pItemElem = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if (pItemElem && IsRestrictionItem(pItemElem)) {
			if (!m_slots[0] && m_slots[0].IsIn(point) && IsUpgradeItem(pItemElem)) {
				m_slots[1].Clear();
				m_slots[0].Set(pItemElem);
			} else if (m_slots[0] && !m_slots[1] && m_slots[1].IsIn(point) && IsSMItem(pItemElem)) {
				m_slots[1].Set(pItemElem);
			} else {
				bbid = TRUE;
			}
		} else {
			bbid = TRUE;
		}

		if( bbid )
			SetForbid( TRUE );
	}
	else
		SetForbid( FALSE );

	return FALSE;
}

bool CWndCommercialElem::IsRestrictionItem(CItemElem * pItemElem) {
	// 장착되어 있는지? 유니크 아이템인지 검사
	if (g_pPlayer->m_Inventory.IsEquip(pItemElem->m_dwObjId)) {
		g_WndMng.PutString(TID_GAME_EQUIPPUT);
		return false;
	}
	
	if (pItemElem->GetProp()->nLog >= 2) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_UPGRADE_ERROR_NOUNICK), MB_OK);
		return false;
	}

	return true;
}

bool CWndCommercialElem::IsUpgradeItem(CItemElem * pItemElem) {
	const ItemProp * prop = pItemElem->GetProp();
	const auto itemKind2 = prop->dwItemKind2;
	const auto itemKind3 = prop->dwItemKind3;

	// 방어구(슈트만), 무기류가 아니면 제련불가능
	if( !( (itemKind2 == IK2_WEAPON_MAGIC ||
		itemKind2 == IK2_WEAPON_DIRECT ) ||
		( (itemKind2 == IK2_ARMOR || itemKind2 == IK2_ARMORETC )
		&& itemKind3 == IK3_SUIT )
		) )
	{
		CString str;
		str.Format( prj.GetText(TID_GAME_NOTEQUALELEM), prop->szName );
		g_WndMng.PutString( str, NULL, prj.GetTextColor(TID_GAME_NOTEQUALELEM) );
		return FALSE;
	}

	if (pItemElem->m_bItemResist <= SAI79::NO_PROP || SAI79::END_PROP <= pItemElem->m_bItemResist) {
		g_WndMng.PutString(TID_GAME_NOTELEMENT);
		return FALSE;
	}

	if (pItemElem->m_nResistSMItemId != 0) { // 이미적용한 아이템이면 불가능
		g_WndMng.PutString(TID_GAME_ALREADYELEM);
		return FALSE;
	}

	return TRUE;
}

bool CWndCommercialElem::IsSMItem(CItemElem * pItemElem) {
	if (pItemElem->GetProp()->dwItemKind2 != IK2_SYSTEM) {
		return FALSE;
	}

	static_assert(std::is_same_v<bool, std::underlying_type_t<SAI79::StoneType>>);

	const auto asElementStone = SAI79::StoneIdToElement(pItemElem->m_dwItemId);

		
		// 속성 공격력 추가
		// 속성 방어력 추가
		if(asElementStone)

		{
			if( m_slots[0]->m_bItemResist == SAI79::NO_PROP )
			{
				g_WndMng.PutString(TID_GAME_NOTELEMENT);
				return FALSE;
			}
			
			if (asElementStone->second == SAI79::StoneType::Attack) {
				if (m_slots[0].m_item->GetProp()->dwItemKind2 == IK2_ARMOR ||
					m_slots[0].m_item->GetProp()->dwItemKind2 == IK2_ARMORETC) {
					g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
					return FALSE;
				}
			} else {
				assert(asElementStone->second == SAI79::StoneType::Defense);

				if (m_slots[0].m_item->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
					m_slots[0].m_item->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT) {
					g_WndMng.PutString(TID_GAME_NOTEQUALITEM);
					return FALSE;
				}

			}

			if(m_slots[0]->m_bItemResist != asElementStone->first ) {
				CString str;
				str.Format( prj.GetText(TID_GAME_NOTEQUALELEM), pItemElem->GetProp()->szName );
				g_WndMng.PutString( str, NULL, prj.GetTextColor(TID_GAME_NOTEQUALELEM) );
				return FALSE;
			}
		}
		else // 속성 제거
		if( pItemElem->m_dwItemId == II_CHR_SYS_SCR_TINEINEDSTONE )
		{
			if( m_slots[0]->m_nResistAbilityOption <= 0 )
				return FALSE;
		}
		else
			return FALSE;


	return TRUE;
}

/****************************************************
  WndId : APP_REMOVE_ELEM - 속성 적용
  CtrlId : WIDC_STATIC_MESSAGE - 
  CtrlId : WIDC_BTN_ELEMOK - 
  CtrlId : WIDC_BTN_ELEMCANCEL - Button
****************************************************/

void CWndRemoveElem::OnSetItem( BYTE nType, DWORD dwItemId, OBJID objid, int nParts, CItemElem* pItemElem )
{
	m_nType = nType;
	m_nParts = nParts;
	m_dwItemId = dwItemId;
	m_objid = objid;
	m_pItemElem = pItemElem;
	m_bSetting = TRUE;
}

void CWndRemoveElem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	m_bSetting = FALSE;
	// 여기에 코딩하세요
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT_MESSAGE );
	if( pWndText )
	{
		pWndText->m_string = prj.GetText(TID_GAME_REMOVEELEM);
	}

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveElem::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_REMOVE_ELEM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndRemoveElem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BTN_ELEMOK:
		{
			if( m_bSetting )
			{
				if( m_nType == 1 )
				{
					g_DPlay.SendDoUseItem( m_dwItemId, m_objid, m_nParts, FALSE );
				}
				else
				if( m_nType == 2 )
				{
					g_DPlay.SendDoEquip( m_pItemElem, m_nParts, FALSE );
				}
				m_bSetting = FALSE;
			}
			Destroy();
		}
		break;
	case WIDC_BTN_ELEMCANCEL:
	case WTBID_CLOSE:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
