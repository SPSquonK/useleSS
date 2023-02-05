#include "stdafx.h"
#include "defineObj.h"
#include "project.h"
//#include "company.h"
#include "lang.h"

#include "couplehelper.h"

#include "guild.h"
#include "Tax.h"

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#include "tlord.h"

#include "honor.h"

#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ
#ifdef __GUILD_HOUSE_MIDDLE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE_MIDDLE

CProject::CProject()
{
	memset( m_apszWorld, 0, sizeof(TCHAR) * 64 * MAX_WORLD );
//	for( int i = 0; i < MAX_WORLD; i++ )
//		m_apszWorld[i][0]	= '\0';
//	m_mapII.SetSize( 128, 128, 128 );
	m_nMoverPropSize	= 0;
	m_pPropMover	= new MoverProp[MAX_PROPMOVER];
//	m_pPropMover	= (MoverProp*)::VirtualAlloc( NULL, sizeof(MoverProp) * MAX_PROPMOVER, MEM_COMMIT, PAGE_READWRITE );

	m_fItemDropRate = 1.0f;
	m_fGoldDropRate = 1.0f;
	m_fMonsterExpRate = 1.0f;
	m_fMonsterHitRate = 1.0f;
#ifdef __S1108_BACK_END_SYSTEM
	m_fMonsterRebirthRate = 1.0f;	
	m_fMonsterHitpointRate = 1.0f;	
	m_fMonsterAggressiveRate = 0.2f;
	m_fMonsterRespawnRate = 1.0f;
	m_bBaseGameSetting = FALSE;
	m_nMonsterRespawnSize = 0;
	m_nMonsterPropSize = 0;
	m_nAddMonsterRespawnSize = 0;
	m_nAddMonsterPropSize = 0;
	m_nRemoveMonsterPropSize = 0;
	m_bBackEndSystem = FALSE;
	ZeroMemory( m_chBackEndSystemTime, sizeof( m_chBackEndSystemTime ) );
	ZeroMemory( m_chBackEndSystemChatTime, sizeof( m_chBackEndSystemChatTime ) );
	ZeroMemory( m_chGMChat, sizeof( m_chGMChat ) );
#endif // __S1108_BACK_END_SYSTEM

	m_bItemUpdate = FALSE;
#ifdef __ITEM_REMOVE_LIST
	m_dwConvMode = 0;
#endif // __ITEM_REMOVE_LIST
}

CProject::~CProject()
{
	SAFE_DELETE_ARRAY( m_pPropMover );

	for (tagColorText & lpText : m_colorText) {
		if (lpText.lpszData) {
			free(lpText.lpszData);
		}
	}
}

BOOL CProject::OpenProject( LPCTSTR lpszFileName )
{
	CScanner s;
	if( s.Load( lpszFileName )	== FALSE )
		return FALSE;

	LoadPreFiles();

	do {
		s.GetToken();

		if( s.Token == _T( "propItem" ) )
		{
			s.GetToken();
			LoadPropItem( s.token, &m_aPropItem );
		}
		else if( s.Token == _T( "propMover" ) )
		{
			s.GetToken();
			LoadPropMover( s.token );
		}
		else if( s.Token == _T( "propSkill" ) )
		{
			s.GetToken();
			LoadPropItem( s.token, &m_aPropSkill, LoadPropItemStyle::V22SkillsSkip );
			jobs.LoadSkills(m_aPropSkill);
		}
		else if( s.Token == _T( "world" ) )
		{
			s.GetToken();
			LoadDefOfWorld( s.token );
		}
		else if( s.Token == _T( "expTable" ) )
		{
			s.GetToken();
			LoadExpTable( s.token );
		}
	}	while( s.tok != FINISHED );

	LoadBeginPos();
	jobs.LoadJobItem(_T("jobItem.inc"));

	m_EventLua.LoadScript();
	m_EventLua.CheckEventState();

	m_GuildCombat1to1.LoadScript();

#ifdef __RULE_0615
	if (!nameValider.Load()) return FALSE;
#endif	// __RULE_0615
	
	CPetProperty::GetInstance()->LoadScript( "pet.inc" );

	CTax::GetInstance()->LoadScript();

	CTitleManager::Instance()->LoadTitle("honorList.txt");
//CTitleManager::Instance()->AddValue(MI_AIBATT1, HI_HUNT_MONSTER, 1);

	CCoupleHelper::Instance()->Initialize();

#ifdef __QUIZ
	CQuiz::GetInstance()->LoadScript();
#endif // __QUIZ
#ifdef __GUILD_HOUSE_MIDDLE
	GuildHouseMng->LoadScript();
#endif // __GUILD_HOUSE_MIDDLE

	return TRUE;
}

void CProject::LoadBeginPos() {
	WIN32_FIND_DATA ffdFoundData; 
	
	BOOL bDoneWithHandle	= FALSE;
	HANDLE hFound	= FindFirstFile( MakePath( DIR_WORLD, "*.*" ), (LPWIN32_FIND_DATA)&ffdFoundData );
	
	if( (HANDLE)( -1 ) == hFound )
		bDoneWithHandle	= TRUE;

	D3DXVECTOR3 vPos;
	DWORD dwIndex;

	while( !bDoneWithHandle )
	{
		if( ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) && ( 0 != strcmp( ".", ffdFoundData.cFileName ) ) && ( 0 != strcmp( "..", ffdFoundData.cFileName ) ) )
		{
			CHAR lpFileName[MAX_PATH];
			sprintf( lpFileName, "%s%s\\%s.rgn", DIR_WORLD, ffdFoundData.cFileName, ffdFoundData.cFileName );
			CScanner s;
			if( s.Load( lpFileName ) == TRUE )
			{
				s.GetToken();
				while( s.tok != FINISHED )
				{
					if( s.Token == _T( "region" ) ||  s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
					{
						BOOL bNewFormat = FALSE;
						BOOL bNewFormat3 = FALSE;
						if( s.Token == _T( "region2" ) )
							bNewFormat = TRUE;
						if( s.Token == _T( "region3" ) )
						{
							bNewFormat3 = TRUE;
							bNewFormat = TRUE;
						}

						s.GetNumber();	// type
						dwIndex		= s.GetNumber();
						vPos.x	= s.GetFloat();
						vPos.y	= s.GetFloat();
						vPos.z	= s.GetFloat();
						s.GetNumber();	s.GetNumber(); s.GetToken();	s.GetToken();	s.GetToken();	// attribute, id, script, music, sound
						s.GetToken();	s.GetFloat();	s.GetFloat();	s.GetFloat();	// teleport
						s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();	// rect
						s.GetToken();	s.GetNumber();	// key, target
						if( bNewFormat3 )
						{
							s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber();
							s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber();
							s.GetNumber();
						}

						if( bNewFormat == FALSE )
						{
							char cbDesc	= s.GetNumber();	// size
							for( int i = 0; i < cbDesc; i++ )
								s.GetToken();	// desc
						}
						else
						{
							s.GetToken(); // get "title"
							BOOL bDesc = s.GetNumber();
							if( bDesc )
							{
								CString string;
								s.GetToken(); // {
								do
									s.GetToken(); 
								while( *s.token != '}' );
							}
							s.GetToken(); // get "desc"
							bDesc = s.GetNumber();
							if( bDesc )
							{
								CString string;
								s.GetToken(); // {
								do
									s.GetToken(); 
								while( *s.token != '}' );
							}
						}
						if( RI_BEGIN == dwIndex ) 
						{
							strlwr( ffdFoundData.cFileName );
							const char * worldName = ffdFoundData.cFileName;
							
							// 맵이름으로 배열 찾기, 맵이름 없으면, 생성해서 넣은뒤, 첫번째 포스 배열에 좌표 넣기,
							m_mapBeginPos[worldName].emplace_back(vPos);
						}
					}
					else if( s.Token == _T( "respawn" ) )
					{
						// type, index, x, y, z, cb, time, left, top, right, bottom
						s.GetNumber();	s.GetNumber();	s.GetFloat();	s.GetFloat();	s.GetFloat();	s.GetNumber();	s.GetNumber();
						s.GetNumber();	// m_nActiveAttackNum
						s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();
					}
					s.GetToken();
				}
			}
		}
		bDoneWithHandle
			= !FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );
	}
	FindClose( hFound );
}

std::optional<D3DXVECTOR3> CProject::GetRandomBeginPos(const DWORD dwWorldID) const {
	const TCHAR * worldName = m_apszWorld[dwWorldID];

	const auto it = m_mapBeginPos.find(worldName);
	if (it == m_mapBeginPos.end()) return std::nullopt;
	if (it->second.empty()) return std::nullopt;

	return it->second[xRandom(it->second.size())];
}

BOOL CProject::LoadDefOfWorld( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return( FALSE );

	int nBrace	= 1;
	s.SetMark();
	int i	= s.GetNumber();	// folder or id

	while( nBrace )
	{
		if( MAX_WORLD <= i )
			Error( "Error LoadDefOfWorld i = %d Max값보다 많음", i );

		if( *s.token == '}' || s.tok == FINISHED )
		{
			nBrace--;
			if( nBrace > 0 )
			{
				s.SetMark();
				i	= s.GetNumber();	// folder or id
				continue;
			}
			if( nBrace == 0 )
				continue;
		}
		s.GetToken();		// { or filename
		if( s.Token == "SetTitle" )
		{
			s.GetToken(); // (
			strcpy(	m_apszWorldName[i], GetLangScript( s ) );
		}
		else
		{
			s.GoMark();
			i	= s.GetNumber(); // id
			
			s.GetToken();		// filename
			_tcscpy( m_apszWorld[i], s.token );
			_tcslwr( m_apszWorld[i] );
		}
		s.SetMark();
		i	= s.GetNumber();	// texture fileName
	}

	return( TRUE );
}

BOOL CProject::LoadExpTable( LPCTSTR lpszFileName )
{
#ifdef __DBSERVER
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	int i	= 0;
	
	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		i	= 0;
		if( s.Token == _T( "expCompanyTest" ) )
		{
			CGuildTable::GetInstance().ReadBlock( s );
		}
		else if( s.Token == _T( "expParty" ) )
		{
			ZeroMemory( m_aExpParty, sizeof( m_aExpParty ) );
			s.GetToken();	// {
			DWORD dwVal	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_aExpParty[i].Exp	= dwVal;
				m_aExpParty[i++].Point	= s.GetNumber();
				dwVal	= s.GetNumber();
			}
		}

		s.GetToken();	// type name or }
	}
#endif	// __DBSERVER
	return TRUE;
}

void CProject::SetConvMode( DWORD dwMode )
{
	m_dwConvMode |= dwMode;
}
void CProject::ResetConvMode( DWORD dwMode )
{
	m_dwConvMode &= (~dwMode);
}
BOOL CProject::IsConvMode( DWORD dwMode )
{
	return m_dwConvMode & dwMode;
}

CProject	prj;