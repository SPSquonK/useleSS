#include "stdafx.h"
#include "AppDefine.h"
#include "WndCommand.h"
#include "FuncTextCmd.h"
#include <boost/range/adaptor/indexed.hpp>

void CWndCommand::OnDraw( C2DRender* p2DRender ) {
	CWndListBox::OnDraw( p2DRender );
	DrawKoreanDescription(p2DRender);
} 

void CWndCommand::DrawKoreanDescription(C2DRender * p2DRender) {
	if (::GetLanguage() != LANG_KOR) return;
	
	const CRect rect = GetWindowRect();
	const int nFontHeight = GetFontHeight() + 3;

	CPoint pt(3, 3);
	pt.y -= (nFontHeight)*m_wndScrollBar.GetScrollPos();
	
	for (const auto [i, pListItem] : m_listItemArray | boost::adaptors::indexed(0)) {
		CPoint point = GetMousePoint();
		CRect rectHittest = CRect(0, pt.y - 3, 0 + rect.Width(), pt.y + nFontHeight - 3);
		if (rectHittest.PtInRect(point)) {
			const TextCmdFunc * pTextCmdFunc = reinterpret_cast<const TextCmdFunc *>(pListItem.m_dwData);
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rectHittest);
			CString string = pTextCmdFunc->m_pszDesc;
			g_toolTip.PutToolTip(i + 1000, string, rectHittest, point2, 3);
			break;
		}
		pt.y += nFontHeight;
	}
}

void CWndCommand::OnInitialUpdate() 
{ 
	CWndListBox::OnInitialUpdate(); 

	int nCount = 0, nNum = 0;

	for (const auto & func : g_textCmdFuncs) {
		const TextCmdFunc * pTextCmdFunc = &func;
		if( ::GetLanguage() != LANG_KOR )			// 한국이외에서는 사용제한 
		{
			if( memcmp( pTextCmdFunc->m_pCommand, "open", 4 ) == 0 )
				break;
		}

		if( pTextCmdFunc->m_dwAuthorization <= g_pPlayer->m_dwAuthorization )
		{
			if( ::GetLanguage() != LANG_KOR )
				g_Option.m_nChatCommand = 2;

			CWndListBox::LISTITEM * nIndex = nullptr;

			switch( g_Option.m_nChatCommand )
			{
			default:
			case 0:
				nIndex = &AddString( &(*pTextCmdFunc->m_pKrCommand) );
				break;
			case 1:
				nIndex = &AddString( &(*pTextCmdFunc->m_pKrAbbreviation) );
				break;
			case 2:
				nIndex = &AddString( &(*pTextCmdFunc->m_pCommand) );
				break;
			case 3:
				nIndex = &AddString( &(*pTextCmdFunc->m_pAbbreviation) );
				break;
			}
			
			nIndex->m_dwData = reinterpret_cast<DWORD>(pTextCmdFunc);
			nNum++;
		}
		nCount++;
	}
	SortListBox();

	if( nNum > 20 ) nNum = 20;
	if( nNum < 5 ) nNum = 5;
	SetWndRect( CRect( 0, 0, 160, nNum * ( GetFontHeight() + 3 ) + 8 ) );

	m_pWndEdit->SetFocus();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCommand::Initialize( CWndBase* pWndParent )
{ 
	CRect rectWindow = g_WndMng.GetWindowRect();
	CRect rect( 0, 0, 150, 200 );
	SetTitle( _T( "World" ) );

	CWndBase::Create( WBS_TOPMOST | WBS_VSCROLL, rect, pWndParent, APP_COMMAND );
	AdjustWndBase();
	RestoreDeviceObjects();

	return TRUE;
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndCommand::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = g_WndMng.GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/


void CWndCommand::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	CWndListBox::OnLButtonUp( nFlags, point );
	m_pParentWnd->SetFocus();
} 

void CWndCommand::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CWndListBox::OnLButtonDblClk( nFlags, point );
	m_pParentWnd->SetFocus();
}

void CWndCommand::OnSetFocus( CWndBase* pOldWnd ) {
}

void CWndCommand::OnKillFocus(CWndBase* pNewWnd)
{
	if( pNewWnd != m_pParentWnd )
		Destroy();
}
