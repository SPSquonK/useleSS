#include "stdafx.h"
#include "AppDefine.h"
#include "WndMotion.h"
#include "WndManager.h"
#include "defineText.h"

void CWndMotion::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	CWndTabCtrl * lpTapCtrl = (CWndTabCtrl *)GetDlgItem(WIDC_TABCTRL1);
	m_wndMotion1.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, lpTapCtrl, 100000);
	m_wndEmoticon.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, lpTapCtrl, 1000001);

	lpTapCtrl->InsertItem(&m_wndMotion1, prj.GetText(TID_GAME_TOOLTIP_MOTION));
	lpTapCtrl->InsertItem(&m_wndEmoticon, prj.GetText(TID_GAME_TOOLTIP_EMOTICON));
	lpTapCtrl->SetCurSel(0);

	lpTapCtrl->SetButtonLength(130);

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	Move(rectRoot.TopLeft());
}

BOOL CWndMotion::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_MOTION, pWndParent, 0, CPoint(0, 0));
}

///////////////////////////////////////////////////////////////////////////////

CWndMotion1::CWndMotion1()
{
	m_pSelectMotion = NULL;
	m_nSelect = 0;	
}

void CWndMotion1::OnDraw(C2DRender* p2DRender)
{
	int j = 0;
	
	for (const MotionProp * pMotionProp : m_motionArray) {
		const int ny = j / 8;
		p2DRender->RenderTexture( CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) ), pMotionProp->pTexture );
		j++;
	}
}
void CWndMotion1::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	for (MotionProp & pMotionProp : prj.m_aPropMotion) {
		m_motionArray.emplace_back(&pMotionProp);
		pMotionProp.pTexture = m_textureMng.AddTexture( MakePath( DIR_ICON, pMotionProp.szIconName ), 0xffff00ff );
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
}
BOOL CWndMotion1::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
void CWndMotion1::OnSize(UINT nType, int cx, int cy)
{
}
void CWndMotion1::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_pSelectMotion )
		g_WndMng.ObjectExecutor(ShortcutType::Motion, m_pSelectMotion->dwID );
			
}
void CWndMotion1::OnLButtonDown(UINT nFlags, CPoint point)
{
	int j = 0;

	m_pSelectMotion = NULL;
	m_nSelect = 0;

	for (MotionProp * pMotionProp : m_motionArray) {

			const int ny = j / 8;
			CPoint pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
			CRect rect( pt, CSize(32, 32) );
			
			if( PtInRect(&rect, point) )
			{
				m_pSelectMotion = pMotionProp;
				m_nSelect = j;
				break;
			}
			
			j++;
		
	}
}

BOOL CWndMotion1::Process( void )
{
	if( !IsFocusChild() )
		return FALSE;
	
	CPoint point = GetMousePoint();
	CPoint pt;
	MotionProp* pMotionProp = NULL;
	int j = 0;
	int ny;

	for (MotionProp * pMotionProp : m_motionArray) {

			if( pMotionProp->dwID != MOT_BASE_CHEER )
			{
				j++;
				continue;
			}

			ny = j / 8;
			pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
			
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );
			
			TCHAR szDesc [ 1024 ] = { 0 };
			_tcscpy( szDesc, pMotionProp->szDesc );
			
			if( g_pPlayer )
			{
				TCHAR szChar[ 256 ] = { 0 };
				_stprintf( szChar, prj.GetText(TID_CHEER_MESSAGE1), g_pPlayer->m_nCheerPoint );
				_tcscat( szDesc, "\n" );
				_tcscat( szDesc, szChar );	
				
				if( g_pPlayer->m_nCheerPoint < MAX_CHEERPOINT )
				{
					CTimeSpan ct( ((g_pPlayer->m_dwTickCheer - GetTickCount()) / 1000) );		// 남은시간을 초단위로 변환해서 넘겨줌
					
					TCHAR szChar2[ 256 ] = { 0 };
					if( ct.GetMinutes() <= 0 )
					{
						_stprintf( szChar2, prj.GetText(TID_PK_LIMIT_SECOND), ct.GetSeconds() );
					}
					else
					{
						_stprintf( szChar2, prj.GetText(TID_PK_LIMIT_MINUTE), ct.GetMinutes() );
					}
					
					_tcscat( szDesc, "\n" );
					_tcscat( szDesc, prj.GetText(TID_CHEER_MESSAGE2) );	
					_tcscat( szDesc, szChar2 );	
				}
				
				g_toolTip.PutToolTip( pt.x + 10000, szDesc, rect, pt2, 0 );
				
				break;
			}
			j++;
		
	}

	return TRUE;
}

void CWndMotion1::OnMouseWndSurface( CPoint point )
{
	int j = 0;

	for (MotionProp * pMotionProp : m_motionArray) {

			const int ny = j / 8;
			CPoint pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
			CRect rect( pt, CSize(32, 32) );
	
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );
			
			if( pMotionProp->dwID != MOT_BASE_CHEER )		
				g_toolTip.PutToolTip( pt.x + 10000, pMotionProp->szDesc, rect, pt2, 0 );
			
			j++;
		
	}
}	
void CWndMotion1::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_pSelectMotion && IsPush())
	{
		m_GlobalShortcut.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwShortcut = ShortcutType::Motion;
		m_GlobalShortcut.m_dwIndex    = m_nSelect;
		m_GlobalShortcut.m_dwId       = m_pSelectMotion->dwID;
		m_GlobalShortcut.m_pTexture   = m_pSelectMotion->pTexture;
		m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectMotion;
	}
}

///////////////////////////////////////////////////////////////////////////////

void CWndEmoticon::OnDraw(C2DRender* p2DRender)
{
	int j = 0;
	
	for( int i=0; i < (int)( g_DialogMsg.m_texEmoticonUser.GetNumber() ); i++ )
	{
		CTexture *pTexture = g_DialogMsg.m_texEmoticonUser.GetAt(i);
		if( pTexture )
		{
			const int ny = j / 8;
			p2DRender->RenderTexture( CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) ), pTexture );
			j++;
		}
	}
}
void CWndEmoticon::OnInitialUpdate()
{
	m_nSelect = -1;

	CWndBase::OnInitialUpdate();
}
BOOL CWndEmoticon::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}

void CWndEmoticon::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_pSelectTexture )
		g_WndMng.ObjectExecutor(ShortcutType::Emoticon, m_nSelect );
}
void CWndEmoticon::OnLButtonDown(UINT nFlags, CPoint point)
{
	int j = 0;
	m_nSelect = -1;

	for( int i=0; i < (int)( g_DialogMsg.m_texEmoticonUser.GetNumber() ); i++ )
	{
		CTexture *pTexture = g_DialogMsg.m_texEmoticonUser.GetAt(i);
		if( pTexture )
		{
			const int ny = j / 8;
			const CPoint pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
			
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );

			if( PtInRect(&rect, pt2))
			{
				m_pSelectTexture  = pTexture;
				m_nSelect = i;
				break;
			}
			
			j++;
		}
	}
}
void CWndEmoticon::OnMouseWndSurface( CPoint point )
{
	int j = 0;
	for( int i = 0; i < (int)( g_DialogMsg.m_texEmoticonUser.GetNumber() ) ; i++ )
	{
		CTexture *pTexture = g_DialogMsg.m_texEmoticonUser.GetAt(i);
		if( pTexture )
		{
			const int ny = j / 8;
			const CPoint pt = CPoint( ( j - ( ny * 8 ) ) * 32 + 3, 7 + ( ny * 32 ) );
			CRect rect( pt.x, pt.y, pt.x+32, pt.y+32 );
			
			CPoint pt2 = point;
			
			ClientToScreen( &pt2 );
			ClientToScreen( &rect );
			
			if( PtInRect( &rect, pt2 ) )
			{
				TCHAR szCmd[1024] = { 0 };

				_tcscat( szCmd, "/" );
				_tcscat( szCmd, g_DialogMsg.m_EmiticonCmd[i].m_szCommand );

				g_toolTip.PutToolTip( pt.x + 10000, szCmd, rect, pt2, 0 );
			}
			
			j++;
		}
	}
	
	
}	
void CWndEmoticon::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_pSelectTexture && IsPush())
	{
		m_GlobalShortcut.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwShortcut = ShortcutType::Emoticon;
		m_GlobalShortcut.m_dwIndex    = m_nSelect;
		m_GlobalShortcut.m_dwId       = m_nSelect;
		m_GlobalShortcut.m_pTexture   = m_pSelectTexture;
		m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectTexture;

		TCHAR szCmd[1024] = { 0 };
		
		_tcscat( szCmd, "/" );
		_tcscat( szCmd, g_DialogMsg.m_EmiticonCmd[m_nSelect].m_szCommand );
		
		_tcscpy( m_GlobalShortcut.m_szString, szCmd  );
	}
}

