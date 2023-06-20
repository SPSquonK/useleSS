#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"

#include "Guild.h"

#include "WndGuildMerit.h"

#include "DPClient.h"

/****************************************************
  WndId : APP_GUILD_MERIT - 길드공헌창
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_STATIC1 - PXP지원 :
  CtrlId : WIDC_CHECK1 - 
  CtrlId : WIDC_STATIC2 - 지 원 금   :
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
****************************************************/

void CWndGuildMerit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
	m_wndctrlMerit.Create( WLVS_ICON | WBS_NODRAWFRAME, lpWndCtrl->rect, this, 100 );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	TCHAR szNumber[ 64 ];
	CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);
	_itot( 0, szNumber, 10 );
	pWndEdit->SetString( szNumber );
	pWndEdit->SetFocus();
} 

BOOL CWndGuildMerit::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_GUILD_MERIT1, pWndParent, 0, CPoint(0, 0));
}

void CWndGuildMerit::MeritResultMsg(CONTRIBUTION_RESULT cbResult) {
	switch (cbResult) {
		case CONTRIBUTION_OK:
			g_WndMng.PutString(TID_GAME_GUILDMERITSTUFF);
			break;
		case CONTRIBUTION_FAIL_MAXLEVEL:
			g_WndMng.PutString(TID_GAME_GUILDMERITMAXLEVEL);
			break;
		case CONTRIBUTION_FAIL_GUILD_OVERFLOW_PENYA:
			g_WndMng.PutString(TID_GAME_GUILDMAXBANKGOLD);
			break;
		case CONTRIBUTION_FAIL_INVALID_CONDITION:
			g_WndMng.PutString(TID_GAME_GUILDMERITSYSERROR);
			break;
		case CONTRIBUTION_FAIL_OVERFLOW_PENYA:
			g_WndMng.PutString(TID_GAME_GUILDMERITMAXGOLD);
			break;
	}
}

BOOL CWndGuildMerit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if (message == EN_RETURN) {
		nID = WIDC_BUTTON1;
	}

	switch( nID )
	{
	case WIDC_CHECK2:	
		{
			m_hasItemContribution = GetDlgItem<CWndButton>(WIDC_CHECK2)->GetCheck();

			if (m_hasItemContribution && g_pPlayer) {
				CItemElem* pItemElem = NULL;
					
				for (int i = 0; i < g_pPlayer->m_Inventory.GetSize(); i++) {
					CItemElem * pItemElem = g_pPlayer->m_Inventory.GetAtId(i);

					if (!pItemElem) continue;

					if (pItemElem->GetProp()->dwItemKind3 == IK3_GEM) {
						pItemElem->SetExtra(pItemElem->m_nItemNum);
						m_wndctrlMerit.AddItem(pItemElem);
					}
				}
			} else {
				m_wndctrlMerit.RestoreItem();
				m_wndctrlMerit.ClearItem();
			}
		}
		break;

	case WIDC_BUTTON1:		// OK
		{
			CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);
			m_nPenya = atoi( pWndEdit->GetString() );		// 문자열을 숫자로 변환.
			m_nPenya = std::max(0, m_nPenya);
			
			if (CGuild * pGuild = g_pPlayer->GetGuild()) {
				// 페냐가 공헌가능한지 검사.
				if( m_nPenya )
				{
					CONTRIBUTION_RESULT cbResult = pGuild->CanContribute( 0, (DWORD)m_nPenya, g_pPlayer->m_idPlayer );
					if( cbResult == CONTRIBUTION_OK )	// 페냐 공헌가능.
						g_DPlay.SendGuildContribution( 0, m_nPenya );
					MeritResultMsg( cbResult );	// 안내메시지.
				}
				
				// 공헌아이템
				if(m_hasItemContribution)
				{
					CONTRIBUTION_RESULT cbResult = pGuild->CanContribute( m_wndctrlMerit.GetTotalCount(), 0, g_pPlayer->m_idPlayer );
					if( cbResult == CONTRIBUTION_OK )	// 아이템 공헌가능
						g_DPlay.SendGuildContribution( 0, 0,  1);

					MeritResultMsg( cbResult );	// 안내메시지.
				}
			}
			Destroy();
		}
		break;
	case 10000:
	case WIDC_BUTTON2:		// NO
		m_wndctrlMerit.RestoreItem();
		Destroy();			// 걍 창 닫고 나감.
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

///////////////////////////////////////////////////////////////////////////////

CTextureMng	CWndGuildMeritCtrl::m_textureMng;
CWndGuildMeritCtrl::CWndGuildMeritCtrl()
{
	m_nWndColor		= D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor	= D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor	= D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSel	= -1;
	m_pFocusItem	= NULL;
	m_pItemContainer.clear();
	m_bDrag		= FALSE;
}

CWndGuildMeritCtrl::~CWndGuildMeritCtrl()
{
	RestoreItem();
}

void CWndGuildMeritCtrl::Create( DWORD dwListCtrlStyle, RECT & rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle	= dwListCtrlStyle;
	CWndBase::Create( m_dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}
void CWndGuildMeritCtrl::OnInitialUpdate( void )
{
	CRect rect	= GetWindowRect();
	rect.left	= rect.right - 15;

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
}
	
void CWndGuildMeritCtrl::OnDraw( C2DRender* p2DRender )
{
	if( m_pItemContainer.size() <= 0 )
		return;

	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 32;

	// 스크롤바 관련   
	CPoint pt( 0, 0 );

	nWidth = 6;
	int nPage = nHeight;
	int nRange = m_pItemContainer.size() / nWidth;// - nPage;
	if( m_pItemContainer.size() % nWidth )
		nRange++;

	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for( int i = pt.y; i < (int)( m_pItemContainer.size() ); i++ )
	{
		int x	= ( i - pt.x ) % 6;
		int y	= ( i - pt.y ) / 6;
		
		if (CItemElem * const pItemBase = m_pItemContainer[i])
		{
			pItemBase->GetTexture()->Render( p2DRender, CPoint( x * 32 + 13, y * 32 + 7 ) );
			CRect rectHittest	= CRect( x * 32 + 13, y * 32  + 7, x * 32  + 13 + 32, y * 32 + 32 + 7 );
			const CPoint point	= GetMousePoint();
			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_VENDOR );
			}
			if( i == m_nCurSel )
				p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );

			if(pItemBase->GetProp()->dwPackMax > 1 )
			{
				short nItemNum	= pItemBase->GetExtra();
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( x * 32 + 32 - size.cx + 17, y * 32 + 32 - size.cy + 10, szTemp, 0xff1010ff );
			}
		}
	}
}

void CWndGuildMeritCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_bDrag == FALSE )
		return;
	m_bDrag		= FALSE;
	CPoint pt( 3, 3 );
	CRect rect;

	CItemElem * pItemBase	= m_pFocusItem;
	if( pItemBase && pItemBase->GetExtra() > 0 )
	{
		m_GlobalShortcut.m_pFromWnd		= this;
		m_GlobalShortcut.m_dwShortcut	= ShortcutType::Item;
		m_GlobalShortcut.m_dwIndex	= m_nCurSel;
		m_GlobalShortcut.m_dwId		= pItemBase->m_dwObjId;
		m_GlobalShortcut.m_pTexture		= pItemBase->GetTexture();
		m_GlobalShortcut.m_dwData	= (DWORD)pItemBase;
		_tcscpy( m_GlobalShortcut.m_szString, pItemBase->GetProp()->szName );
	}
}

BOOL CWndGuildMeritCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}

void CWndGuildMeritCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{

}

void CWndGuildMeritCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{

}

void CWndGuildMeritCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
}

void CWndGuildMeritCtrl::OnRButtonDblClk( UINT nFlags, CPoint point )
{

}

void CWndGuildMeritCtrl::OnSize( UINT nType, int cx, int cy )
{
	CWndBase::OnSize( nType, cx, cy );
}

void CWndGuildMeritCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow	= rectWnd;
	m_rectClient	= m_rectWindow;

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndGuildMeritCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect	= GetWindowRect();

	DWORD dwColor1	= D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2	= D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3	= D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

BOOL CWndGuildMeritCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

int CWndGuildMeritCtrl::HitTest( CPoint point )
{
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 32;
	
	CPoint pt( 0, 0 );
	
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
	
	for( int i = pt.y; i < 50; i++ )
	{
		int x	= ( i - pt.y ) % 6;
		int y	= ( i - pt.y ) / 6;
		rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
		if( rect.PtInRect( point ) )
			return i;
	}
	return -1;
}

void CWndGuildMeritCtrl::AddItem(CItemElem *pItemContainer)
{
	m_pItemContainer.push_back(pItemContainer);
}

void CWndGuildMeritCtrl::ClearItem()
{
	m_pItemContainer.clear();
}

int CWndGuildMeritCtrl::GetItemSize()
{
	return m_pItemContainer.size();
}

void CWndGuildMeritCtrl::RestoreItem()
{
	CItemElem* pItemElem = NULL;

	for( int i=0; i<(int)( m_pItemContainer.size() ); i++ )
	{
		pItemElem = m_pItemContainer[i];

		if( pItemElem )
			pItemElem->SetExtra( 0 );
	}
}

int CWndGuildMeritCtrl::GetTotalCount()
{
	int nCount = 0;
	CItemElem* pItemElem = NULL;
	
	for( int i=0; i<(int)( m_pItemContainer.size() ); i++ )
	{
		pItemElem = m_pItemContainer[i];		
		if( pItemElem )
			nCount += pItemElem->GetExtra();
	}

	return nCount;
}


