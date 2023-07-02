#include "stdafx.h"
#include "AppDefine.h"
#include "WndCredit.h"
//#include "version.h" //CCLIENT_VERSION

#include "langman.h"

extern char	g_szVersion[];

#define MAX_SCREENSHOT 17
#define MAX_SCREENSHOT_JAP 13

/****************************************************
  WndId : APP_CREDIT - Credit
****************************************************/
CWndCredit::CWndCredit() 
{ 
	m_nLine = 300;
	m_strWord = "Hello";
	m_nStyle = 0;
	m_nParam1 = 0;
	m_nParam2 = 0;
	m_nParam3 = 0;
	m_ptWord = 0;
	m_nTexCount = 0;
	m_pFont = NULL;
	m_bPlayVoice = FALSE;
	m_pSound = NULL;
} 
CWndCredit::~CWndCredit() 
{ 
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
} 
BOOL CWndCredit::Process()
{
	if( ::GetLanguage() == LANG_JAP )
	{
		if( m_Starttimer.TimeOut() )
		{
			if( m_bPlayVoice == FALSE && m_pSound )
			{
				m_pSound->Play();
				m_bPlayVoice = TRUE;
			}
		}
	}

	if( m_timerScreenShot.TimeOut() )
	{
		if( ::GetLanguage() == LANG_JAP )
			m_timerScreenShot.Set( SEC( 22 ) );
		else
			m_timerScreenShot.Set( SEC( 5 ) );
		
		m_nTexCount = (m_nTexCount + 1) % MAX_SCREENSHOT;
	}

	if( ::GetLanguage() == LANG_JAP )
	{
		if( m_nLine > -1860 ) 
		{
			if( m_timer.TimeOut() )
			{
				m_nLine--;
				m_timer.Set( 105 );
			}
		}
		else
			Destroy();
	}
#ifdef __FOR_PROLOGUE_UPDATE
	else if( ::GetLanguage() != LANG_KOR )
	{
		int maxCount=0;
		switch(::GetLanguage()) 
		{
			case LANG_ENG:
			case LANG_VTN:
			case LANG_RUS:
				maxCount = 1190;
				break;
			case LANG_THA:
				maxCount = 1734;
					break;
			case LANG_TWN:
			case LANG_HK:
				maxCount = 615;
					break;
			case LANG_GER:
				maxCount = 1180;
					break;
			case LANG_SPA:
				maxCount = 1180;
					break;
			case LANG_POR:
				maxCount = 1180;
					break;
			case LANG_FRE:
				maxCount = 1180;
					break;
		}
		if( m_nLine > -maxCount ) 
		{
			if( m_timer.TimeOut() )
			{
				m_nLine--;
				m_timer.Set( 100 );
			}
		}
		else
			Destroy();
	}
#endif //__FOR_PROLOGUE_UPDATE
	else
	{
		if( m_nLine > -3400 ) 
		{
			if( m_timer.TimeOut() )
			{
				m_nLine--;
				m_timer.Set( 30 );
			}
		}
	}
	CRect rect = GetClientRect();
	if ( m_nStyle  == 0)
	{
			if( m_ptWord.y < rect.bottom )
				m_ptWord.y++;
			else
			{
				m_strWord = "asdadds";
				m_ptWord.y = 0;
				m_ptWord.x += 30;
			}
	}
	return 1;
}
HRESULT CWndCredit::InitDeviceObjects()
{
	switch( ::GetLanguage() )
	{
		case LANG_JAP:
			m_pFont = new CD3DFont( _T("MS Gothic"), 11 );//, D3DFONT_BOLD );
			break;
#ifdef __FOR_PROLOGUE_UPDATE
		case LANG_ENG:
		case LANG_GER:
		case LANG_SPA:
		case LANG_POR:
		case LANG_FRE:
		case LANG_THA:
		case LANG_TWN:
		case LANG_HK:
		case LANG_VTN:
		case LANG_RUS:
			{
				PLANG_DATA pData	= CLangMan::GetInstance()->GetLangData( GetLanguage() );
				m_pFont = new CD3DFont( pData->font.lfCaption.szFontFirst, 9 );
				break;
			}
#endif	// __FOR_PROLOGUE_UPDATE
		default:
			m_pFont = new CD3DFont( _T("Arial Black"), 11);//, D3DFONT_BOLD );
			break;
	}
	m_pFont->m_nOutLine = 2;
	m_pFont->m_dwColor = 0xffffffff;
	m_pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 0, 0, 0);
	m_pFont->InitDeviceObjects( g_Neuz.m_pd3dDevice );
	return CWndNeuz::InitDeviceObjects();
}
HRESULT CWndCredit::RestoreDeviceObjects()
{
	m_pFont->RestoreDeviceObjects();
	return CWndNeuz::RestoreDeviceObjects();
}
HRESULT CWndCredit::InvalidateDeviceObjects()
{
	m_pFont->InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndCredit::DeleteDeviceObjects()
{
	m_pFont->DeleteDeviceObjects();
	//delete m_pFont;
	SAFE_DELETE( m_pFont );

	if( ::GetLanguage() == LANG_JAP )
	{
		for( int i = 0; i < MAX_SCREENSHOT_JAP; i++ )
			m_aTexScreenShot[ i ].DeleteDeviceObjects();
	}
	else
	{
		for( int i = 0; i < MAX_SCREENSHOT; i++ )
			m_aTexScreenShot[ i ].DeleteDeviceObjects();
	}
	
	m_texScreenShot.DeleteDeviceObjects();
	
	return CWndNeuz::DeleteDeviceObjects();
}
void CWndCredit::OnDraw( C2DRender* p2DRender ) 
{ 
	p2DRender->RenderTexture( CPoint( 2,  2 ), &m_aTexScreenShot[ m_nTexCount ] );
	
	CRect rect = GetClientRect();
	CD3DFont* pOldFold = p2DRender->GetFont();
	p2DRender->SetFont( m_pFont );
	int i = 0;
	for (const CString & string : m_strArray) {
		const CSize size = m_pFont->GetTextExtent( string );
		const int nCenter = ( rect.Width() / 2 ) - ( size.cx / 2 );
		p2DRender->TextOut( nCenter + 0, m_nLine + i * 20, string, 0xffffffff );
		++i;
	}
	
	p2DRender->SetFont( pOldFold );
} 
void CWndCredit::OnDestroy( void )
{
	if(m_pSound)
	{
		m_pSound->Stop();
		m_pSound->Reset();
		m_pSound = NULL;
		SetVolume(m_fOldMusicVolume);
	}
}
void CWndCredit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 모래시계로 커서 변경
	SetMouseCursor( CUR_DELAY );
	// 여기에 코딩하세요
	{
		CResFile file; 

		CString strfile;
#ifdef __FOR_PROLOGUE_UPDATE
		if( ::GetLanguage() != LANG_KOR )
			strfile = MakePath( DIR_THEME, _T( "Openning.inc" ) );
#endif //__FOR_PROLOGUE_UPDATE
			strfile = "credit.txt";
		
		if( file.Open( strfile.GetString(), "rb" ) )
		{
			int nLength = file.GetLength();
			TCHAR* pChar = new TCHAR[ nLength + 1];
			file.Read( pChar, nLength );
			pChar[ nLength ] = 0;
			TCHAR* prog = pChar;
			m_strArray.clear();

			while( *prog != '\0' )
			{
				TCHAR szString[ 256 ];
				TCHAR* temp = szString;
				while( *prog && ( *prog > 0 && *prog <= 0x20 ) && *prog != '\r' && *prog != '\0' ) 
					prog++;
				while( *prog!='\r' && *prog!='\0' ) 
					*temp++ = *prog++;
				if( *prog == '\r')
					prog++; // skim comma
				// white space를 스킵
				if( temp != szString )
				{
					temp--;
					while( ( *temp > 0 && *temp <= 0x20 ) && *temp ) 
						temp--;
					temp++;
				}
				*temp = '\0';

				m_strArray.emplace_back( szString );

			}

			delete[] pChar;
		}
	}
	m_timerScreenShot.Set( SEC( 10 ) );

	{
		for( int i = 0; i < MAX_SCREENSHOT; i++ )
		{
			CString string;
			string.Format( "shotCredit%02d.dds", i );
			//string = "shotCredit20.dds";
			
			m_aTexScreenShot[ i ].LoadTexture( MakePath( DIR_THEME, string ), 0xffff00ff );
			
			m_aTexScreenShot[ i ].m_size = CSize( 598 + 76, 337 );
		}
	}
		
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	if( ::GetLanguage() == LANG_JAP )
	{
		CString filename = MakePath( DIR_SOUND, "VocThePrologue.WAV" );
		g_SoundMng.Play( filename.GetBuffer(0), NULL, NULL, FALSE );
		m_pSound = g_SoundMng.GetSound( filename.GetBuffer(0) );
		m_fOldMusicVolume = GetVolume();
		
		if(m_fOldMusicVolume && m_fOldMusicVolume >= 0.04f)
			SetVolume(0.04f);

		m_Starttimer.Set( SEC( 9 ) );
		m_bPlayVoice = FALSE;
	}
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCredit::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_CREDIT, pWndParent, WBS_MODAL, CPoint( 0, 0 ) );
}

BOOL CWndCredit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WTBID_CLOSE )
		Destroy( TRUE );
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndCredit::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectLayout = m_rectClient;
	if( !IsWndRoot() && !IsWndStyle( WBS_NOFRAME ) )
	{
		if( IsWndStyle( WBS_CAPTION ) )
			m_rectClient.top += 17;
		m_rectClient.DeflateRect( 2, 2, 6, 8 );
		m_rectLayout = m_rectClient;
		m_rectLayout.DeflateRect( 8, 8 );	
	}
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}

/****************************************************
  WndId : APP_ABOUT - 프리프에 관해서
****************************************************/

CWndAbout::CWndAbout() 
{ 
	m_pFont = NULL;
} 

void CWndAbout::OnDraw( C2DRender* p2DRender ) 
{ 
	CD3DFont* pOldFold = p2DRender->GetFont();
	p2DRender->SetFont( m_pFont );
	p2DRender->TextOut( 165, 143, g_szVersion );
	p2DRender->SetFont( pOldFold );	
} 

void CWndAbout::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	if(m_resMng.GetAt(GetWndId()))
	{
		const char * strName = "WndAboutFlyff.tga";
		SetTexture( MakePath( DIR_THEME, strName ), TRUE );
	}
	

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAbout::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_ABOUT, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndAbout::OnDestroy()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
	
	SAFE_DELETE(m_pFont);
}

HRESULT CWndAbout::InitDeviceObjects()
{
	m_pFont = new CD3DFont( _T("Arial Black"), 9 );//, D3DFONT_BOLD );
	m_pFont->m_nOutLine = 1;
	m_pFont->m_dwColor = 0xffffffff;
	m_pFont->m_dwBgColor = D3DCOLOR_ARGB( 255, 255, 32, 32);
	m_pFont->InitDeviceObjects( g_Neuz.m_pd3dDevice );

	return CWndNeuz::InitDeviceObjects();
}
HRESULT CWndAbout::RestoreDeviceObjects()
{
	m_pFont->RestoreDeviceObjects();
	return CWndNeuz::RestoreDeviceObjects();
}
HRESULT CWndAbout::InvalidateDeviceObjects()
{
	m_pFont->InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndAbout::DeleteDeviceObjects()
{
	m_pFont->DeleteDeviceObjects();
	return CWndNeuz::DeleteDeviceObjects();
}
