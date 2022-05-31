#include "stdafx.h"
#include "AppDefine.h"
#include "WndHelp.h"
#include <format>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 도움말
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndHelp::OnDraw(C2DRender* p2DRender) {
	const DWORD dwLeft = m_rectClient.Width() * 50 / 100;
	const CRect rect(dwLeft + 5, 5, m_rectClient.Width() - 5, 23);
	p2DRender->TextOut(rect.left + 10, rect.top + 8, m_strKeyword, 0xff000000);
}

void CWndHelp::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndTreeCtrl * pWndTreeCtrl = (CWndTreeCtrl *)GetDlgItem(WIDC_TREE1);
	pWndTreeCtrl->LoadTreeScript(MakePath(DIR_CLIENT, _T("treeHelp.inc")));
	
	CWndText * pWndText = (CWndText *)GetDlgItem(WIDC_TEXT1);
	pWndText->AddWndStyle(WBS_VSCROLL);

	MoveParentCenter();
}

BOOL CWndHelp::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_HELPER_HELP, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndHelp::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if (nID == WIDC_TREE1) {
		TREEELEM * lpTreeElem = reinterpret_cast<TREEELEM *>(pLResult);
		if (lpTreeElem) ChangeDisplayedHelp(*lpTreeElem);
	}

	return CWndNeuz::OnChildNotify(message,nID,pLResult);
}

void CWndHelp::ChangeDisplayedHelp(TREEELEM & treeElem) {
	m_strKeyword = treeElem.m_strKeyword;

	const CString string = prj.GetHelp(m_strKeyword);

	CWndText * pWndText = (CWndText *)GetDlgItem(WIDC_TEXT1);
	pWndText->SetString(string);
	pWndText->UpdateScrollBar();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 도움말
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndHelpFAQ::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	LoadFAQ(MakePath(DIR_CLIENT, _T("Faq.inc")));
	MoveParentCenter();
}

BOOL CWndHelpFAQ::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_HELPER_FAQ, 0, CPoint(0, 0), pWndParent);
}

bool CWndHelpFAQ::LoadFAQ(LPCTSTR lpszFileName) {
	CScript s;
	if (!s.Load(lpszFileName)) return FALSE;

	s.GetToken();	// keyword
	while (s.tok != FINISHED) {
		const std::string question = std::format("Q.{}", s.token);
		s.GetToken();	// {
		s.GetToken();	//
		const std::string answer = std::format("    {}", s.token);
		m_mapFAQ.insert_or_assign(question, answer);
		s.GetToken();	// }
		s.GetToken();
	}

	CWndListBox * pWndListBox = GetDlgItem<CWndListBox>(WIDC_LIST1);
	pWndListBox->ResetContent();
	for (const std::string & question : m_mapFAQ | std::views::keys) {
		pWndListBox->AddString(question.c_str());
	}

	return true;
}

BOOL CWndHelpFAQ::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_LIST1) {
		CString question;
		CWndListBox * pWndListBox = GetDlgItem<CWndListBox>(WIDC_LIST1);
		pWndListBox->GetText(pWndListBox->GetCurSel(), question); // TODO: obsolete CWndListBox::GetText

		OnChangedSelection(question.GetString());
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndHelpFAQ::OnChangedSelection(const std::string & question) {
	std::string answer;
	
	const auto it = m_mapFAQ.find(question);
	if (it != m_mapFAQ.end()) {
		answer = it->second;
	}

	CWndText * const pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	const CRect rect = pWndText->GetClientRect();
	pWndText->m_string.Init(m_pFont, &rect);
	pWndText->m_string.SetString("");
	pWndText->m_string.AddString(question.c_str(), 0xff8080ff, ESSTY_BOLD);
	pWndText->m_string.AddString("\n\n");
	pWndText->m_string.AddString(answer.c_str());
	pWndText->UpdateScrollBar();
}

CWndHelpTip::CWndHelpTip() 
{ 
} 
CWndHelpTip::~CWndHelpTip() 
{ 
} 
void CWndHelpTip::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndHelpTip::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	pWndText->AddWndStyle( WBS_NOFRAME );
	pWndText->AddWndStyle( WBS_NODRAWFRAME );
	CScript s;
	if( s.Load( MakePath( DIR_CLIENT, _T("tip.inc" ) ) ) == FALSE )
		return;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		CString string = "    " + s.Token;
		m_aString.Add( string );
		s.GetToken();
	}
	m_nPosString = 0;
	pWndText->SetString( m_aString.GetAt( 0 ), 0xff000000 );
} 
BOOL CWndHelpTip::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_HELPER_TIP );
	MoveParentCenter();
	return TRUE;
} 
BOOL CWndHelpTip::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	switch( nID )
	{
	case WIDC_NEXT:
		{
			m_nPosString++;
			if( m_nPosString == m_aString.GetSize() )
				m_nPosString = 0;
			CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
			pWndText->SetString( m_aString.GetAt( m_nPosString ), 0xff000000 );
		}
		break;
	case WIDC_CLOSE:
		Destroy();
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndHelpTip::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndHelpTip::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndHelpTip::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndHelpTip::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_CHECK1:
		g_Option.m_bTip = !g_Option.m_bTip;
		Destroy();
		break;
	//case WIDC_CLOSE:
	//	Destroy();
	//	return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
/****************************************************
  WndId : APP_HELP_INSTANT - Instant Help
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_BUTTON1 - Button
****************************************************/

CWndHelpInstant::CWndHelpInstant() 
{ 
} 
CWndHelpInstant::~CWndHelpInstant() 
{ 
} 
void CWndHelpInstant::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndHelpInstant::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString( prj.GetHelp( m_strHelpKey ) );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHelpInstant::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HELP_INSTANT, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndHelpInstant::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndHelpInstant::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndHelpInstant::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndHelpInstant::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndHelpInstant::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndHelpInstant::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
#ifdef __HELP_BUG_FIX
		Destroy();
#else //__HELP_BUG_FIX
		// 헬프 이중생성 방지
		vector<CString>::iterator where = find(g_vecHelpInsKey.begin(), g_vecHelpInsKey.end(), m_strHelpKey );

		if(where != g_vecHelpInsKey.end())
			g_vecHelpInsKey.erase(where);

		Destroy( TRUE );		
#endif //__HELP_BUG_FIX
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

