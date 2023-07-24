#include "stdafx.h"
#include "definetext.h"
#include "defineSound.h"
#include "AppDefine.h"
#include "WndDialog.h"
#include "WndQuest.h"

#ifdef __CLIENT
#include "DPClient.h"
#endif




/****************************************************
  WndId : APP_DIALOG - Dialog
****************************************************/

CWndDialog::~CWndDialog() {
	if (CWndQuest * pWndQuest = CWndBase::GetWndBase<CWndQuest>(APP_QUEST_EX_LIST)) {
		pWndQuest->Update();
	}
} 
void CWndDialog::OnSetCursor() {}

BOOL CWndDialog::Process() 
{
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) Destroy();
	return TRUE;
}
void CWndDialog::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndButton* pEnter = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	POINT pt = lpWndCtrl->rect.TopLeft();

	int i;
	if( m_bSay == TRUE )
		p2DRender->TextOut_EditString( pt.x, pt.y, m_string, 0, 0, 6 );
	else if( strcmp( m_aKeyButton[ m_nSelectKeyButton ].szWord, prj.GetText( TID_GAME_NEW_QUEST ) ) == 0 && m_newQuestListBox.IsEmpty())
		p2DRender->TextOut( pt.x + 10, pt.y + 10, prj.GetText( TID_GAME_EMPTY_NEW_QUEST ), D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	else if( strcmp( m_aKeyButton[ m_nSelectKeyButton ].szWord, prj.GetText( TID_GAME_CURRENT_QUEST ) ) == 0 && m_currentQuestListBox.IsEmpty())
		p2DRender->TextOut( pt.x + 10, pt.y + 10, prj.GetText( TID_GAME_EMPTY_CURRENT_QUEST ), D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	if( m_bWordButtonEnable )
	{
		for( i = 0; i < m_nWordButtonNum; i++ )
		{
			DWORD dwColor = 0xff505050;
			WORDBUTTON* pKeyButton = &m_aWordButton[ i ];
			if( strcmp( pKeyButton->szWord, "__YES__" ) == 0 )
				continue;
			if( strcmp( pKeyButton->szWord, "__NO__" ) == 0 )
				continue;
			if( strcmp( pKeyButton->szWord, "__CANCEL__" ) == 0 )
				continue;
			if( strcmp( pKeyButton->szWord, "__OK__" ) == 0 )
				continue;
			if( pKeyButton->bStatus )
			{
				dwColor = 0xff1010ff;
				CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
				p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
					CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
				
			}
			p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
		}
	}
	for( i = 0; i < m_nKeyButtonNum; i++ )
	{
		DWORD dwColor = ( i == m_nSelectKeyButton ) ? D3DCOLOR_ARGB( 255, 128, 0, 255 ) : 0xff505050;
		WORDBUTTON* pKeyButton = &m_aKeyButton[ i ];
		if( pKeyButton->bStatus )
		{
			dwColor = 0xff1010ff;
			CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
			p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
				CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
	
		}
		p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
		p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
	}

	for( i = 0; i < m_nContextButtonNum; i++ )
	{
		DWORD dwColor = 0xff101010;
		WORDBUTTON* pKeyButton = &m_aContextButton[ i ];

		if( pKeyButton->bStatus )
		{
			dwColor = 0xff1010ff;
			CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
			p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
				CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
			p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			if( pKeyButton->nLinkIndex >= 0 )
			{
				WORDBUTTON* pKeyButton2 = &m_aContextButton[ pKeyButton->nLinkIndex ];
				CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton2->szWord );
				p2DRender->RenderLine( CPoint( pKeyButton2->rect.left, pKeyButton2->rect.top + size.cy ),
					CPoint( pKeyButton2->rect.left + size.cx, pKeyButton2->rect.top + size.cy ), dwColor );
				p2DRender->TextOut( pKeyButton2->rect.left, pKeyButton2->rect.top,  pKeyButton2->szWord, dwColor );
				p2DRender->TextOut( pKeyButton2->rect.left + 1, pKeyButton2->rect.top,  pKeyButton2->szWord, dwColor );
			}
		}
	}
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	m_texChar.Render( p2DRender, lpWndCtrl->rect.TopLeft() );
} 
void CWndDialog::OnMouseWndSurface( CPoint point )
{
	for( int i = 0; i < m_nWordButtonNum; i++ )
		m_aWordButton[ i ].bStatus = FALSE;
	for( int i = 0; i < m_nKeyButtonNum; i++ )
		m_aKeyButton[ i ].bStatus = FALSE;
	for( int i = 0; i < m_nContextButtonNum; i++ )
		m_aContextButton[ i ].bStatus = FALSE;
	if( m_bWordButtonEnable )
	{
		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			if( m_aWordButton[ i ].rect.PtInRect( point ) )
			{
				SetMouseCursor( CUR_SELECT );
			 	m_aWordButton[ i ].bStatus = TRUE;
				return;
			}
		}
	}
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		WORDBUTTON* pKeyButton = &m_aKeyButton[ i ];
		if( pKeyButton->rect.PtInRect( point ) )
		{
			SetMouseCursor( CUR_SELECT );
			pKeyButton->bStatus = TRUE;
			CString string;
			string.Format( prj.GetText(TID_GAME_ABOUTQUESTION), pKeyButton->szWord );
			if( string.IsEmpty() == FALSE )
			{
				CRect rect = pKeyButton->rect;
				ClientToScreen( &rect );

				CPoint point2 = point;
				ClientToScreen( &point2 );
				rect.InflateRect( 2, 2);
				int nLang	= GetLanguage();
				switch( nLang )
				{
					case LANG_GER:
					case LANG_FRE:
						break;
					default:
						g_toolTip.PutToolTip( (DWORD)pKeyButton, string, &rect, point2 );
						break;
				}
			}
			return;
		}
	}

	for( int i = 0; i < m_nContextButtonNum; i++ )
	{
		WORDBUTTON* pKeyButton = &m_aContextButton[ i ];
		if( pKeyButton->rect.PtInRect( point ) )
		{
			SetMouseCursor( CUR_HELP );
			pKeyButton->bStatus = TRUE;
			return;
		}
	}
	SetMouseCursor( CUR_BASE );
}

void CWndDialog::RunScript( const char* szKey, DWORD dwParam, QuestId dwQuest )
{
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover )
		g_DPlay.SendScriptDialogReq( pMover->GetId(), szKey, dwParam, dwQuest, pMover->GetId(), g_pPlayer->GetId() );
	m_timer.Set( SEC( 1 ) );
}

void CWndDialog::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	if (CWndBase * pWndGroupBox = GetDlgItem(WIDC_GROUP_BOX_TITLE)) {
		pWndGroupBox->m_dwColor = D3DCOLOR_ARGB(255, 128, 0, 64);
		pWndGroupBox->SetTitle(prj.GetText(TID_GAME_DIALOG));
	}


	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	lpWndCtrl->dwWndStyle |= WBS_VSCROLL;

	static constexpr int QUEST_LIST_SELECT_COLOR = 0xffff0000;
	static constexpr int QUEST_LIST_LINE_SPACE = 3;

	m_newQuestListBox.Create( lpWndCtrl->dwWndStyle, lpWndCtrl->rect, this, WIDC_NewQuests);
	m_newQuestListBox.ChangeSelectColor(QUEST_LIST_SELECT_COLOR);
	m_newQuestListBox.SetLineSpace(QUEST_LIST_LINE_SPACE);
	m_newQuestListBox.SetVisible( FALSE );

	m_newQuestListBox.displayer.m_pNewQuestListIconTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, _T("QuestUiPaperGreen.tga")), 0xffffffff);
	m_newQuestListBox.displayer.m_pExpectedQuestListIconTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, _T("QuestUiPaperRed.tga")), 0xffffffff);
	m_newQuestListBox.displayer.xOffset = m_newQuestListBox.displayer.m_pNewQuestListIconTexture->m_size.cx;


	m_currentQuestListBox.Create( lpWndCtrl->dwWndStyle, lpWndCtrl->rect, this, WIDC_CurrentQuests);
	m_currentQuestListBox.ChangeSelectColor(QUEST_LIST_SELECT_COLOR);
	m_currentQuestListBox.SetLineSpace(QUEST_LIST_LINE_SPACE);
	m_currentQuestListBox.SetVisible( FALSE );

	m_currentQuestListBox.displayer.m_pCurrentQuestListIconTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, _T("QuestUiPaperGray.tga")), 0xffffffff);
	m_currentQuestListBox.displayer.m_pCompleteQuestListIconTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, _T("QuestUiPaperYellow.tga")), 0xffffffff);
	m_currentQuestListBox.displayer.xOffset = m_currentQuestListBox.displayer.m_pCurrentQuestListIconTexture->m_size.cx;


	
	Windows::DestroyIfOpened(APP_INVENTORY);
	
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) return;
	LPCHARACTER lpCharacter = pMover->GetCharacter();

	if( lpCharacter )
	{
		m_texChar.DeleteDeviceObjects();
		m_texChar.LoadTexture( MakePath( "char\\",lpCharacter->m_szChar ), 0xffff00ff, TRUE );
		if( lpCharacter->m_dwMusicId )
			PlayMusic( lpCharacter->m_dwMusicId, 1 );
	}
	m_nCurArray = 0;
	m_strArray.clear();

	MakeKeyButton();
	
	m_bSay = FALSE;

	UpdateButtonEnable();
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = g_WndMng.GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.top  + (rectRoot.Height() / 2) - (nHeight / 2);	
	CPoint point( x, 10 );
	Move( point );

	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST_EX_LIST );
	if( pWndQuest ) pWndQuest->Update();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDialog::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_DIALOG_EX, pWndParent, WBS_MODAL, CPoint( 0, 0 ) );
} 

BOOL CWndDialog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDialog::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDialog::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	if( m_timer.IsTimeOut() == FALSE )
		return;

	PLAYSND( SND_INF_CLICK );
	if( m_bWordButtonEnable )
	{
		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			if( m_aWordButton[ i ].rect.PtInRect( point ) )
			{			
				BeginText();
				RunScript( m_aWordButton[i].szKey, m_aWordButton[i].dwParam, m_aWordButton[i].dwParam2 );
				MakeKeyButton();
				UpdateButtonEnable();
			}
		}
	}
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		if( m_aKeyButton[ i ].rect.PtInRect( point ) )
		{
			m_nSelectKeyButton = i;

			CWndBase * pWndGroupBox = GetDlgItem( WIDC_GROUP_BOX_TITLE );
			if( pWndGroupBox )
				pWndGroupBox->SetTitle( m_aKeyButton[ m_nSelectKeyButton ].szWord );

			m_bSay = FALSE;
			BeginText();
 			LPWNDCTRL lpWndCustom1 = GetWndCtrl( WIDC_CUSTOM1 );
			if( strcmp( m_aKeyButton[ i ].szWord, prj.GetText( TID_GAME_NEW_QUEST ) ) == 0 && !m_newQuestListBox.IsEmpty())
			{
				m_newQuestListBox.SetVisible( TRUE );
				m_newQuestListBox.SetFocus();
				m_currentQuestListBox.SetVisible( FALSE );
			}
			else if( strcmp( m_aKeyButton[ i ].szWord, prj.GetText( TID_GAME_CURRENT_QUEST ) ) == 0 && !m_currentQuestListBox.IsEmpty())
			{
				m_currentQuestListBox.SetVisible( TRUE );
				m_currentQuestListBox.SetFocus();
				m_newQuestListBox.SetVisible( FALSE );
			}
			else
			{
				m_newQuestListBox.SetVisible( FALSE );
				m_currentQuestListBox.SetVisible( FALSE );
				RunScript( m_aKeyButton[i].szKey, m_aKeyButton[i].dwParam, m_aKeyButton[i].dwParam2 );
			}
			MakeKeyButton();
			UpdateButtonEnable();
		}
	}
	for( int i = 0; i < m_nContextButtonNum; i++ )
	{
		WORDBUTTON* pContextButton = &m_aContextButton[ i ];
		if( pContextButton->rect.PtInRect( point ) )
		{
			CString strKey = pContextButton->szKey;
			BeginText();
			RunScript( strKey, 0, QuestIdNone );
		}
	}
} 
void CWndDialog::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDialog::Say( LPCTSTR lpszString, QuestId dwQuest )
{
	const QuestProp * pQuestProp = dwQuest.GetProp();
 	CString string = "  ";
	if( dwQuest )
	{
		string = "#b#cff5050f0";
		string += pQuestProp->m_szTitle;
		string += "#nb#nc\n";
		string += "  ";
		string += lpszString;
	}
	else
		string += lpszString;

	int nLength = string.GetLength();//_tcslen( szString );
	for( int i = 0, j = 0; i < nLength; i++ )
	{
		if( string[ i ] == '\\' && string[ i + 1 ] == 'n' )
		{
			// 중간의 두개의 코드를 \n와 ' '로 대체.
			string.SetAt( i, '\n' );
			string.SetAt( i + 1, ' ' );
			// 그리고 스페이스 한개 삽입.
			string.Insert( i + 1, " " );
		}
	}
	m_dwQuest = dwQuest;
	ParsingString( string );
	EndSay();
}

void CWndDialog::ParsingString( LPCTSTR lpszString )
{
	CString strWord, strOriginal;
	int nMarkBraceIdx;
	BOOL bKey = FALSE;
	BOOL bBrace = FALSE;
	int nTemp  =0;
	BOOL bWirdChar = FALSE;

	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) return;

	size_t nPos =  m_strArray.size();
	m_aContextMark[ nPos ].RemoveAll();

	CEditString editString;
	editString.AddParsingString( lpszString );
	CEditString* pEditString = new CEditString;

	int nLength = editString.GetLength();
	
	// 왼쪽이 strWord, 오른쪽이 strOrigital
	for( int i = 0; i < nLength; i++ )
	{
		if( editString[ i ] == '['  )
		{
			nMarkBraceIdx = i;
			bBrace = TRUE;
			if( bKey == FALSE )
			{
				strOriginal.Empty();
				strWord.Empty();
				nTemp = 1;
			}
		}
		else
		if( editString[ i ] == ']' )
		{
			if( bBrace == TRUE )
			{
				bBrace = FALSE;
				if( bKey == FALSE )
				{
					bKey = TRUE;
					int nLen = strWord.GetLength();
					int nMark = 3;
					if( nLen == 1 ) 
						nMark = 4;
					if( nLen == 2 && bWirdChar )
						nMark = 4;
					m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 1, nMark );
					if( bWirdChar )
						m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 2, nMark );
				}
				else
				{
					bKey = FALSE;
					if( strOriginal.IsEmpty() )
						strOriginal = strWord;
					m_mapWordToOriginal[strWord] = strOriginal;
				}
			}
		}
		else
		{
			if( bKey == TRUE )
			{
				if( bBrace == FALSE )
				{
					LPCHARACTER lpCharacter = pMover->GetCharacter();
					CString string;
					string.Format( "%s에서 [%s]와 [] 사이에 잡문자가 들어있거나 연결이 안된다.", lpCharacter->m_szDialog, strWord );
					AfxMessageBox( string );
				}
				strOriginal += editString[ i ];
				if( IsDBCSLeadByte( editString[ i ] ) )
					strOriginal += editString[ ++i ];
			}
			else
			{
				int nMarkTemp = 0;
				DWORD dwStyle = editString.m_abyStyle.GetAt( i );
				DWORD dwColor = editString.m_adwColor.GetAt( i );
				if( bBrace )
				{
					nMarkTemp = nTemp;
					if( nTemp == 1 ) nTemp++;
					dwStyle = ESSTY_BOLD;
				}
				m_aContextMark[ nPos ].Add( nMarkTemp );
				pEditString->AddString( editString[ i ], dwColor, dwStyle );

				strWord += editString[ i ];
				bWirdChar = FALSE;
				if( IsDBCSLeadByte( editString[ i ] ) )
				{
					m_aContextMark[ nPos ].Add( nMarkTemp );
					pEditString->AddString( editString[ ++i ], 0xff000000, dwStyle );

					strWord += editString[ i ];
					bWirdChar = TRUE;
				}
			}
		}
	}
	pEditString->Align( pEditString->m_pFont, 0 );
	m_strArray.emplace_back( pEditString );
}
void CWndDialog::EndSay()
{
	if( !m_strArray.empty() )
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_string.Init( m_pFont, &lpWndCtrl->rect );
		m_string.SetEditString( *m_strArray[0] );
		m_nCurArray = 0;
	}
	MakeKeyButton();
	MakeContextButton();
	MakeAnswerButton();
	UpdateButtonEnable();

}
void CWndDialog::BeginText()
{
	m_strArray.clear();
	m_mapWordToOriginal.clear();

	m_nWordButtonNum = 0;
	for( int i = 0; i < 6; i++ )
	{
		if( m_apWndAnswer[ i ] )
			m_apWndAnswer[ i ]->Destroy( TRUE );
		[[maybe_unused]] CWndBase * willBeDeletedByDestroy = m_apWndAnswer[ i ].release();
	}
}
void CWndDialog::MakeContextButton()
{
	DWORD dwLineCount = m_string.GetLineCount();
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	m_nContextButtonNum = 0;
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	int x, y = lpWndCtrl->rect.top;
	DWORD dwCurOffset;
	TCHAR strHan[ 3 ];
	CRect rectWord(0, 0, 0, 0);
	DWORD dwMark = 0;
	CString strWord;
	BOOL bLinkWord = FALSE;
	for( int i = 0; i < (int)( dwLineCount ); i++)
	{
		CString string = m_string.GetLine( i );
		DWORD dwOffset = m_string.GetLineOffset( i );
		LPCTSTR lpszStr = string;
		int nLength = string.GetLength();
		x = lpWndCtrl->rect.left;
		strWord.Empty();
		
		for( int j = 0; j < nLength; j++ )
		{
			if( IsDBCSLeadByte( string[ j ] ) )
			{
				strHan[ 0 ] = string[ j++ ];
				strHan[ 1 ] = string[ j ];
				strHan[ 2 ] = '\0';
				dwCurOffset = dwOffset + ( j - 1 );
			}
			else
			{
				strHan[ 0 ] = string[ j ];
				strHan[ 1 ] = '\0';
				dwCurOffset = dwOffset + j;
			}
			CSize size = m_pFont->GetTextExtent( strHan );
			dwMark = m_aContextMark[ m_nCurArray ].GetAt( dwCurOffset );

			if( dwMark )
			{
				strWord += strHan;
				rectWord.right += size.cx;
			}
			if( dwMark == 1 || dwMark == 4 ) 
			{
				rectWord.SetRect( x, y, x + size.cx, y + dwMaxHeight );
				if( dwMark == 4 ) dwMark = 3;
			}
			if( dwMark == 3 ) 
			{
				m_aContextButton[ m_nContextButtonNum ].rect = rectWord;
				m_aContextButton[ m_nContextButtonNum ].bStatus = FALSE;
				m_aContextButton[ m_nContextButtonNum ].nLinkIndex = -1;
				m_aContextButton[ m_nContextButtonNum ].dwParam2 = m_dwQuest;
				CString strTemp = strWord;
				if( bLinkWord )
				{
					m_aContextButton[ m_nContextButtonNum ].nLinkIndex = m_nContextButtonNum - 1;
					strWord = m_aContextButton[ m_nContextButtonNum - 1 ].szWord;
					strWord += strTemp;
				}

				const char * strKey = GetOriginalOfWord(strWord);
				strcpy( m_aContextButton[ m_nContextButtonNum ].szWord, strTemp );
				strcpy( m_aContextButton[ m_nContextButtonNum ].szKey, strKey );
				if( bLinkWord )
					strcpy( m_aContextButton[ m_nContextButtonNum - 1 ].szKey, strKey );
				m_nContextButtonNum++;
				bLinkWord = FALSE;
				strWord.Empty();
			}
			x += size.cx;
		}	
		y += dwMaxHeight;
		
		// 중간에 끊어진 경우 
		if( dwMark == 1 || dwMark == 2 ) //bKeyButton == TRUE )
		{
			//bKeyButton = FALSE;
			bLinkWord = TRUE;
			m_aContextButton[ m_nContextButtonNum ].rect = rectWord;
			m_aContextButton[ m_nContextButtonNum ].bStatus = FALSE;
			m_aContextButton[ m_nContextButtonNum ].nLinkIndex = m_nContextButtonNum + 1;
			m_aContextButton[ m_nContextButtonNum ].dwParam2 = m_dwQuest;
			
			const char * strKey = GetOriginalOfWord(strWord);
			strcpy( m_aContextButton[ m_nContextButtonNum ].szWord, strWord );
			strcpy( m_aContextButton[ m_nContextButtonNum ].szKey, strKey );
			rectWord.SetRect( lpWndCtrl->rect.left, y, lpWndCtrl->rect.left, y + dwMaxHeight );
			m_nContextButtonNum++;
		}
	}
}

const char * CWndDialog::GetOriginalOfWord(const CString & strWord) const {
	const auto it = m_mapWordToOriginal.find(strWord);
	if (it != m_mapWordToOriginal.end()) return it->second.GetString();
	return strWord.GetString();
}

void CWndDialog::AddKeyButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, QuestId dwQuest )
{
	WORDBUTTON* lpKeyButton;// = &m_aKeyButton[ m_nKeyButtonNum ];
	// 같은 워드가 발견되면 무시한다. 키는 같아도 되지만 워드는 같으면 하나는 무시함.
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		if( strcmp( lpKeyButton->szWord, lpszWord ) == 0 )
			return; 
		
	}
	lpKeyButton = &m_aKeyButton[ m_nKeyButtonNum ];
	strcpy( lpKeyButton->szWord, lpszWord );
	strcpy( lpKeyButton->szKey, lpszKey );
	lpKeyButton->bStatus = FALSE;
	//lpWordButton->rect
	lpKeyButton->dwParam = dwParam;
	lpKeyButton->dwParam2 = dwQuest;
	m_nKeyButtonNum++;
	EndSay();
}
void CWndDialog::RemoveAllKeyButton()
{
	m_newQuestListBox.ResetContent();
	m_currentQuestListBox.ResetContent();
	m_nKeyButtonNum = 0;
	EndSay();
}
void CWndDialog::RemoveKeyButton( LPCTSTR lpszKey )
{
	WORDBUTTON* lpKeyButton;// = &m_aKeyButton[ m_nKeyButtonNum ];
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		//if( strcmp( lpKeyButton->szWord, lpszWord ) == 0 )
		if( strcmp( lpKeyButton->szKey, lpszKey ) == 0 )
		{
			for( int j = i; j < m_nKeyButtonNum - 1; j++ )
			{
				memcpy( &m_aKeyButton[ j ], &m_aKeyButton[ j + 1 ], sizeof( WORDBUTTON ) ) ;
			}
			m_nKeyButtonNum--;
			EndSay();
			return;
		}
	}
	EndSay();
}
// 하얀색 대화영역에 나오는 버튼 
void CWndDialog::AddAnswerButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, QuestId dwQuest )
{
	WORDBUTTON* lpWordButton = &m_aWordButton[ m_nWordButtonNum ];
	strcpy( lpWordButton->szWord, lpszWord );
	strcpy( lpWordButton->szKey, lpszKey );
	lpWordButton->bStatus = FALSE;
	//lpWordButton->rect
	lpWordButton->dwParam  = dwParam;
	lpWordButton->dwParam2 = dwQuest;
	m_nWordButtonNum++;
	EndSay();
}

void CWndDialog::MakeKeyButton()
{
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CRect rectClient = GetClientRect();
	int x = 0, y = 0;
	BOOL bQuestButtonLinefeedSwitch = FALSE;
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		WORDBUTTON* pWndButton = &m_aKeyButton[ i ];

		CSize size = m_pFont->GetTextExtent( pWndButton->szWord );
		size.cx += 16;
		//size.cy += 4;
		pWndButton->bStatus = FALSE;
		
		pWndButton->rect = CRect( lpWndCtrl->rect.left + x, lpWndCtrl->rect.top + y * dwMaxHeight, 
			lpWndCtrl->rect.left + x + size.cx, lpWndCtrl->rect.top + y * dwMaxHeight + size.cy );
		if( pWndButton->rect.right > lpWndCtrl->rect.right )
		{
			x = 0, y++;
			pWndButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				lpWndCtrl->rect.left + ( x * size.cx ) + size.cx, lpWndCtrl->rect.top + y * dwMaxHeight + size.cy );
		}
		else if( ( strcmp( pWndButton->szWord, prj.GetText( TID_GAME_NEW_QUEST ) ) == 0 || strcmp( pWndButton->szWord, prj.GetText( TID_GAME_CURRENT_QUEST ) ) == 0 ) && 
			bQuestButtonLinefeedSwitch == FALSE )
		{
			x = 0, ++y;
			pWndButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				lpWndCtrl->rect.left + ( x * size.cx ) + size.cx, lpWndCtrl->rect.top + y * dwMaxHeight + size.cy );
			bQuestButtonLinefeedSwitch = TRUE;
		}				
		x += size.cx;
	}
}
// 하얀색 대화영역에 나오는 버튼 
// AddAnswer로 추가 
void CWndDialog::MakeAnswerButton()
{
	m_bWordButtonEnable = FALSE;

	if (m_nCurArray + 1 != m_strArray.size())
		return;

		DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		
		int yBegin = lpWndCtrl->rect.top + m_string.GetLineCount() * dwMaxHeight + dwMaxHeight;
		int x = 0, y = 0;//
		int nButtonNum = 0;
		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			WORDBUTTON* pWordButton = &m_aWordButton[ i ];
			if( strcmp( pWordButton->szWord, "__YES__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__NO__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__CANCEL__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__OK__" ) == 0 )
			{ nButtonNum++; continue; }

			CSize size = m_pFont->GetTextExtent( pWordButton->szWord );
			size.cx += 10;

			pWordButton->rect = CRect( lpWndCtrl->rect.left + x, yBegin + y * dwMaxHeight, 
				lpWndCtrl->rect.left + x + size.cx, yBegin + y * size.cy + dwMaxHeight );
			if( pWordButton->rect.right > lpWndCtrl->rect.right )
			{
				x = 0, y++;
				//pWordButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				//	lpWndCtrl->rect.left + x * size.cx + size.cx, lpWndCtrl->rect.top + y * size.cy + dwMaxHeight );

				pWordButton->rect = CRect( lpWndCtrl->rect.left + x, yBegin + y * dwMaxHeight, 
					lpWndCtrl->rect.left + x + size.cx, yBegin + y * size.cy + dwMaxHeight );

			}				
			x += size.cx;
		}
		x = lpWndCtrl->rect.left + ( lpWndCtrl->rect.Width() / 2 ) - ( ( 90 * nButtonNum ) / 2 );
		CRect rect;
		CString strTexture;
		int nWndId = 0;
		int j = 0;

		std::ranges::generate(m_apWndAnswer, [] { return nullptr; });

		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			WORDBUTTON* pWordButton = &m_aWordButton[ i ];
			rect = CRect( x, lpWndCtrl->rect.bottom - 20, x + 70, lpWndCtrl->rect.bottom + 10 );
			if( strcmp( pWordButton->szWord, "__YES__" ) == 0 )
			{
				nWndId =  WIDC_YES;
				strTexture = "buttYes.tga";
			}
			else
			if( strcmp( pWordButton->szWord, "__NO__" ) == 0 )
			{
				nWndId =  WIDC_NO;
				strTexture = "buttNo.tga";
			}
			else
			if( strcmp( pWordButton->szWord, "__CANCEL__" ) == 0 )
			{
				nWndId =  WIDC_CANCEL;
				strTexture = "buttCancel.tga";
			}
			else
			if( strcmp( pWordButton->szWord, "__OK__" ) == 0 )
			{
				nWndId =  WIDC_OK;
				strTexture = "buttOk.tga";
			}
			if(	nWndId )
			{
				m_apWndAnswer[ j ] = std::make_unique<CWndAnswer>();
				m_apWndAnswer[ j ]->Create( "", WBS_CHILD, rect, this, nWndId );
				m_apWndAnswer[ j ]->SetTexture( MakePath( DIR_THEME, strTexture ), 1 );
				m_apWndAnswer[ j ]->FitTextureSize();
				m_apWndAnswer[ j ]->m_pWordButton = pWordButton;
				j++;
			}
			x += 90;
		}

		m_bWordButtonEnable = TRUE;

}

void CWndDialog::UpdateButtonEnable()
{
	CWndButton* pEnter = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	if( m_strArray.empty() || m_strArray.size() == m_nCurArray + 1 )
	{
		pEnter->SetVisible( FALSE );
	}
	else
	{
		pEnter->SetVisible( TRUE );
	}		
}


BOOL CWndDialog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON1:
		if( m_nCurArray + 1 < m_strArray.size() )
		{
			m_nCurArray++;
			LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
			m_string.Init( m_pFont, &lpWndCtrl->rect );
			m_string.SetEditString( *m_strArray[m_nCurArray] );
			MakeContextButton();
			MakeAnswerButton();
		}
		UpdateButtonEnable();
		break;
	case WIDC_OK:
	case WIDC_YES:
	case WIDC_NO:
	case WIDC_CANCEL:
		{
			CWndAnswer* pWndAnswer = (CWndAnswer*)pLResult;
			const WORDBUTTON* pWordButton = pWndAnswer->m_pWordButton;
			BeginText();
			RunScript( pWordButton->szKey,  pWordButton->dwParam,  pWordButton->dwParam2 );
			MakeKeyButton();
			UpdateButtonEnable();
		}
		break;
	case WIDC_NewQuests: {
		ListedQuest * quest = m_newQuestListBox.GetCurSelItem();
		if (quest) {
			if (quest->strKey != "QUEST_NEXT_LEVEL") {
				m_bSay = FALSE;
				m_newQuestListBox.SetVisible(FALSE);
				BeginText();
				RunScript(quest->strKey, 0, quest->questId);
				MakeKeyButton();
				UpdateButtonEnable();
			}
		}
		break;
	}
	case WIDC_CurrentQuests: {
		ListedQuest * quest = m_currentQuestListBox.GetCurSelItem();
		if (quest) {
			m_bSay = FALSE;
			m_currentQuestListBox.SetVisible(FALSE);
			BeginText();
			RunScript(quest->strKey, 0, quest->questId);
			MakeKeyButton();
			UpdateButtonEnable();
		}
		break;
	}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndDialog::AddQuestInList(const LPCTSTR lpszWord, const LPCTSTR lpszKey, const QuestId dwQuest, const bool isNewQuest) {
	const auto [elem, isValid] = MakeListedQuest(lpszWord, lpszKey, dwQuest);

	// TODO: we should check if the quest is not already listed

	if (isNewQuest) {
		MakeQuestKeyButton(prj.GetText(TID_GAME_NEW_QUEST));
		m_newQuestListBox.Add(elem, isValid);
	} else {
		MakeQuestKeyButton(prj.GetText(TID_GAME_CURRENT_QUEST));
		m_currentQuestListBox.Add(elem, isValid);
	}
}

void CWndDialog::MakeQuestKeyButton( const CString& rstrKeyButton )
{
	WORDBUTTON* lpKeyButton = NULL;
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		if( strcmp( lpKeyButton->szWord, rstrKeyButton ) == 0 )
			return;
	}
	lpKeyButton = &m_aKeyButton[ m_nKeyButtonNum ];
	strcpy( lpKeyButton->szWord, rstrKeyButton );
	strcpy( lpKeyButton->szKey, rstrKeyButton );
	lpKeyButton->bStatus = FALSE;
	lpKeyButton->dwParam = 0;
	lpKeyButton->dwParam2 = QuestIdNone;
	++m_nKeyButtonNum;
	EndSay();
}

void CWndDialog::NewQuestDisplayer::Render(
	C2DRender * p2DRender, CRect rect,
	ListedQuest & quest, DWORD color,
	const WndTListBox::DisplayArgs & misc
) const {
	CTexture * icon = misc.isValid ? m_pNewQuestListIconTexture : m_pExpectedQuestListIconTexture;
	p2DRender->RenderTexture(rect.TopLeft(), icon);

	quest.displayName.SetColor(color);
	p2DRender->TextOut_EditString(xOffset + rect.left, rect.top, quest.displayName);
}

void CWndDialog::CurrentQuestDisplayer::Render(
	C2DRender * p2DRender, CRect rect,
	ListedQuest & quest, DWORD color,
	const WndTListBox::DisplayArgs &
) const {
	CTexture * icon = __IsEndQuestCondition(g_pPlayer, quest.questId)
		? m_pCompleteQuestListIconTexture
		: m_pCurrentQuestListIconTexture;
	p2DRender->RenderTexture(rect.TopLeft(), icon);

	quest.displayName.SetColor(color);
	p2DRender->TextOut_EditString(xOffset + rect.left, rect.top, quest.displayName);
}


std::pair<CWndDialog::ListedQuest, bool> CWndDialog::MakeListedQuest(const LPCTSTR lpszWord, const LPCTSTR lpszKey, const QuestId dwQuest)
{
	//for( int i = 0; i < nQuestListNumber; ++i )
	//{
	//	CString strQuestList = pWndListBox.GetString( i );
	//	if( strcmp( strQuestList, lpszWord ) == 0 )
	//		return;
	//}
	CWndDialog::ListedQuest listed;

	CEditString strTitleWord = _T( "" );
	const QuestProp* pQuestProp = dwQuest.GetProp();
	if (pQuestProp) {
		strTitleWord.Format(
			_T("[#b%d#nb~#b%d#nb ] #b%s#nb"),
			pQuestProp->m_nBeginCondLevelMin,
			pQuestProp->m_nBeginCondLevelMax,
			lpszWord
		);
	} else {
		strTitleWord = lpszKey;
	}
	
	const bool isValid = lpszKey != std::string_view(_T("QUEST_NEXT_LEVEL"));
	listed.strKey = lpszKey;
	listed.questId = dwQuest;

	CRect rect = GetClientRect();
	listed.displayName.Init(m_pFont, &rect);
	listed.displayName.SetParsingString(strTitleWord.GetString(), 0xFF3C3C3C, 0x00000000, 0, 0x00000001, TRUE);

	return std::make_pair(listed, isValid);
}
