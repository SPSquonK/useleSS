#include "stdafx.h"
#include "sqktd.h"
#include "defineItem.h"
#include "lang.h"
#include "Ship.h"
#include "guild.h"
#include "spevent.h"

#include "couplehelper.h"
#include "couple.h"
#ifdef __WORLDSERVER
#include "dpdatabaseclient.h"
#include "dpsrvr.h"
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
#include "rangda.h"
#endif // __WORLDSERVER

#include "definequest.h"

#ifdef __AZRIA_1023
#include "ticket.h"
#endif	// __AZRIA_1023

#ifdef __WORLDSERVER
	#include "worldmng.h"
	#include "guildquest.h"
	#include "MsgHdr.h"
	#include "partyquest.h"
#ifdef __SYS_TICKET
	#ifndef __AZRIA_1023
		#include "ticket.h"
	#endif	// __AZRIA_1023
#endif	// __SYS_TICKET
#endif	// __WORLDSERVER

#include "pet.h"
#include "collecting.h"
#include "randomoption.h"

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER

#include "SecretRoom.h"

#include "Tax.h"

#include "CreateMonster.h"

/*
#ifdef __GIFTBOX0213
#include "dpaccountclient.h"
#include "msghdr.h"
#endif	// __GIFTBOX0213
*/

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013


#ifdef __WORLDSERVER
#include "slord.h"
#endif	// __WORLDSERVER
#ifdef __CLIENT
#include "clord.h"
#endif	// __CLIENT

#ifdef __WORLDSERVER
#include "RainbowRace.h"
#endif // __WORLDSERVER

#include "honor.h"

#ifdef __WORLDSERVER
#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ

#include "CampusHelper.h"

#include "GuildHouse.h"

#ifdef __ENVIRONMENT_EFFECT
#include "Environment.h"
#endif // __ENVIRONMENT_EFFECT

#endif // __WORLDSERVER

FLOAT	CProject::m_fItemDropRate = 1.0f;		// ���� ������ ��ӷ�
FLOAT	CProject::m_fGoldDropRate = 1.0f;		// ���� ��� ��ӷ�
FLOAT	CProject::m_fMonsterExpRate = 1.0f;		// ���� ����ġ��
FLOAT	CProject::m_fMonsterHitRate = 1.0f;		// ���� ���ݷ�
FLOAT	CProject::m_fShopCost = 1.0f;			// ��������
FLOAT	CProject::m_fSkillExpRate = 1.0f;		
DWORD	CProject::m_dwVagSP = 1;				// ����� ��ų�� �������� �ʿ��� SP����Ʈ
DWORD	CProject::m_dwExpertSP = 2;				// 1������ ��ų�� �������� �ʿ��� SP����Ʈ
DWORD	CProject::m_dwProSP = 3;				// 2������ ��ų�� �������� �ʿ��� SP����Ʈ


////////////////////////////////////////////////////////////////////////////////////
// CHARACTER
////////////////////////////////////////////////////////////////////////////////////
void CHARACTER::Clear()
{
	m_nStructure        = -1;
	m_nEquipNum         = 0;
	m_dwHairMesh = 0;
	m_dwHairColor = 0;
	m_dwHeadMesh = 0;
	m_dwMoverIdx = 0;
	m_dwMusicId = 0;

	ZeroMemory( m_szKey, sizeof( m_szKey ) );
	ZeroMemory( m_szChar, sizeof( m_szChar ) );
	ZeroMemory( m_szDialog, sizeof( m_szDialog ) );
	ZeroMemory( m_szDlgQuest, sizeof( m_szDlgQuest ) );
	ZeroMemory( m_abMoverMenu, sizeof( m_abMoverMenu ) );
	ZeroMemory( m_adwEquip, sizeof( m_adwEquip ) );
	m_vecTeleportPos.clear();
}

////////////////////////////////////////////////////////////////////////////////////
// CDropItemGenerator
////////////////////////////////////////////////////////////////////////////////////
DROPITEM* CDropItemGenerator::GetAt( int nIndex, BOOL bUniqueMode, float fProbability )
{
	//ASSERT( nIndex < m_dwSize );
	ASSERT( nIndex < (int)GetSize() );

	if( fProbability > 0.0f ) 
	{
		DROPITEM* lpDropItem	= &m_dropItems[ nIndex ];
		DWORD dwProbability		= lpDropItem->dwProbability;
		DWORD dwRand = xRandom( 3000000000 );
#if defined(__WORLDSERVER) // __EVENTLUA, __WORLDSERVER
		dwRand = static_cast<DWORD>(dwRand / fProbability );
#endif // __EVENTLUA && __WORLDSERVER
		/*
		if( lpDropItem->dwLevel && bUniqueMode && lpDropItem->dwProbability <= 10000000 )
		{
			dwRand /= 2;
		}
		// ����(����ũ)�� �ֳ� �˻�
		if( lpDropItem->dwLevel != 0 )
		{
			ItemProp* pItemProp	= prj.GetItemProp( lpDropItem->dwIndex );
			if( pItemProp && (int)pItemProp->dwItemLV > 0 )
			{
				int nValue	= dwProbability / pItemProp->dwItemLV;
				if( nValue < 21 )	nValue	= 21;
				dwProbability	= nValue - 20;
			}
		}
		*/
		return( dwRand < dwProbability? lpDropItem: NULL );
	}
	else 
	{
		return &m_dropItems[ nIndex ];
	}
}

void CDropItemGenerator::AddTail( CONST DROPITEM & rDropItem, const char* s )
{
	m_dropItems.push_back( rDropItem );
}

// CQuestItemGenerator
QUESTITEM* CQuestItemGenerator::GetAt( int nIndex )
{
	ASSERT( nIndex < (int)( m_uSize ) );
	return &m_pQuestItem[nIndex];
}

void CQuestItemGenerator::AddTail( const QUESTITEM & rQuestItem )
{
	ASSERT( m_uSize < MAX_QUESTITEM );
	memcpy( &m_pQuestItem[m_uSize++], &rQuestItem, sizeof(QUESTITEM) );
}

void CDropKindGenerator::AddTail( const DROPKIND & rDropKind )
{
	ASSERT( m_nSize < MAX_DROPKIND );
	memcpy( &m_aDropKind[m_nSize++], &rDropKind, sizeof(DROPKIND) );
}

LPDROPKIND CDropKindGenerator::GetAt( int nIndex )
{
	ASSERT( nIndex >= 0 );
	ASSERT( nIndex < m_nSize );
	if( nIndex < 0 || nIndex >= m_nSize )
		return NULL;
	return &m_aDropKind[nIndex];
}


CProject::CProject()
#ifdef __WORLDSERVER
:
m_nMaxSequence( 0 )
#endif	// __WORLDSERVER
{
	memset( m_minMaxIdxAry, 0xff, sizeof(SIZE) * MAX_ITEM_KIND3 * MAX_UNIQUE_SIZE );
	memset( m_aExpUpItem, 0, sizeof(m_aExpUpItem) );
	memset( m_aGuildAppell, 0, sizeof(m_aGuildAppell) );


#ifdef __S1108_BACK_END_SYSTEM
	m_fMonsterRebirthRate = 1.0f;	
	m_fMonsterHitpointRate = 1.0f;	
	m_fMonsterAggressiveRate = 0.2f;
	m_fMonsterRespawnRate = 1.0f;
	m_nAddMonsterPropSize = 0;
	m_nRemoveMonsterPropSize = 0;
	m_nMonsterPropSize = 0;
#endif // __S1108_BACK_END_SYSTEM

	memset( m_aSetItemAvail, 0, sizeof(m_aSetItemAvail) );

	m_nMoverPropSize = 0;
	m_pPropMover = new MoverProp[MAX_PROPMOVER];
//	m_pPropMover = (MoverProp*)::VirtualAlloc( NULL, sizeof(MoverProp) * MAX_PROPMOVER, MEM_COMMIT, PAGE_READWRITE );
#ifdef __CLIENT
#ifdef __SHOP_COST_RATE
	m_fShopBuyRate = 1.0f;
	m_fShopSellRate = 1.0f;
#endif // __SHOP_COST_RATE
#endif // __CLIENT

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	m_bMapTransparent = FALSE;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM
}

CProject::~CProject()
{
#ifdef __CLIENT
	m_mapHelp.RemoveAll();
	m_mapWordToolTip.RemoveAll();
#endif	// __CLIENT

	int i, j;
	POSITION pos = m_mapCharacter.GetStartPosition();
	LPCHARACTER lpCharacter;
	CString string;

	while(pos)
	{
		m_mapCharacter.GetNextAssoc(pos,string,(void*&)lpCharacter);
		safe_delete( lpCharacter );
	}

	m_mapCharacter.RemoveAll();

	for( i = 0; i < m_colorText.GetSize(); i++ )
	{
		tagColorText* lpText = m_colorText.GetAt(i);
		if( lpText && lpText->lpszData )
			free( lpText->lpszData ); 
	}
	for( i = 0; i < m_aPropQuest.GetSize(); i++ )
	{
		QuestProp* pQuestProp = (QuestProp*)m_aPropQuest.GetAt( i );
		if( pQuestProp )
		{
		#if defined( __WORLDSERVER ) 
			for( j = 0; j < 32; j++ )
//				SAFE_DELETE( pQuestProp->m_apQuestDialog[ j ] );
				SAFE_DELETE_ARRAY( pQuestProp->m_apQuestDialog[ j ] );
		#endif
			for( j = 0; j < 16; j++ )
				SAFE_DELETE( pQuestProp->m_questState[ j ] );
			SAFE_DELETE_ARRAY( pQuestProp->m_paBeginCondItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndCondItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndRewardItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_lpszEndCondMultiCharacter );
			SAFE_DELETE_ARRAY( pQuestProp->m_paBeginCondNotItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndCondOneItem );
		}
	}

	SAFE_DELETE_ARRAY( m_pPropMover );
//	::VirtualFree( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, MEM_DECOMMIT );

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#ifdef __CLIENT
	m_setBlockedUserID.clear();
#endif // __CLIENT
#endif // __YS_CHATTING_BLOCKING_SYSTEM
}
int SortJobSkill( const void *arg1, const void *arg2 )
{
	ItemProp* pItemProp1 = *(ItemProp**) arg1;
	ItemProp* pItemProp2 = *(ItemProp**) arg2;
	if( pItemProp1->dwReqDisLV < pItemProp2->dwReqDisLV )
		return -1;
	if( pItemProp1->dwReqDisLV > pItemProp2->dwReqDisLV )
		return 1;
	return 0;
}

void CProject::LoadSkill()
{
	LoadPropItem( "propSkill.txt", &m_aPropSkill );
	ZeroMemory( m_aJobSkillNum, sizeof( m_aJobSkillNum ) );
	// ������ ��ų�� �����ؼ� �迭�� ���� 
	for( int i = 1; i < m_aPropSkill.GetSize(); i++ )
	{
		ItemProp* pItemProp = (ItemProp*)m_aPropSkill.GetAt( i );
		if( pItemProp )
		{
			if( pItemProp->dwItemKind1 != JTYPE_COMMON )
			{
				ItemProp** apJobSkill = m_aJobSkill[ pItemProp->dwItemKind2 ];
				apJobSkill[ m_aJobSkillNum[ pItemProp->dwItemKind2 ] ] = pItemProp;
				m_aJobSkillNum[ pItemProp->dwItemKind2 ]++;
			}
		}
	}

	// ��Ʈ�ϱ� 
	for( int i = 0; i < MAX_JOB; i++ )
	{
		ItemProp** apJobSkill = m_aJobSkill[ i ];
		qsort( (void *)apJobSkill, (size_t) m_aJobSkillNum[ i ], sizeof( ItemProp* ), SortJobSkill );
	}
}

#ifdef __EVE_MINIGAME
BOOL CProject::LoadMiniGame()
{
	if( m_MiniGame.Load_Alphabet() == FALSE )
	{
		// �޼��� ó��
	}

	if( m_MiniGame.Load_KawiBawiBo() == FALSE )
	{
		// ...
	}

	if( m_MiniGame.Load_FiveSystem() == FALSE )
	{
		// ...
	}

	if( m_MiniGame.Load_Reassemble() == FALSE )
	{
		// ...
	}
	
	return TRUE;
	
}
#endif // __EVE_MINIGAME

BOOL CProject::LoadUltimateWeapon()
{
#ifdef __WORLDSERVER
	if( m_UltimateWeapon.Load_UltimateWeapon() == FALSE )
	{
		return FALSE;
	}
#endif // __WORLDSERVER
	
	if( m_UltimateWeapon.Load_GemAbility() == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CProject::OpenProject( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load( lpszFileName )==FALSE)
		return FALSE;

#if !defined(__CLIENT)
	LoadPreFiles();
#endif

	do 
	{
		scanner.GetToken();
		if( scanner.Token == "propTroupeSkill" )
		{	
			scanner.GetToken();	
			LoadPropItem( "propTroupeSkill.txt", &m_aPartySkill );
		}
		else if( scanner.Token == "propSkillAdd" )
		{
			scanner.GetToken();
			LoadPropAddSkill( scanner.token );
		}
		else if( scanner.Token == "propQuest" )
		{
			scanner.GetToken();
			LoadPropQuest( scanner.token, FALSE );
		}
		else if( scanner.Token == "expTable" )
		{	
			scanner.GetToken();	
			LoadExpTable( scanner.token );
		}
		else if( scanner.Token == "MotionProp" )
		{	
			scanner.GetToken();	
			LoadMotionProp( scanner.token );
		}
		else if( scanner.Token == "propJob" )
		{
			scanner.GetToken();
			LoadPropJob( "propJob.inc" );
		}
		else if( scanner.Token == "propSkill" )
		{	
			scanner.GetToken();	
			LoadSkill();
		}
		else if( scanner.Token == "model" )
		{	
			scanner.GetToken();	
			m_modelMng.LoadScript( scanner.token );
		}
		else if( scanner.Token == "billboard" )
		{	
			scanner.GetToken();	
		}
		else if( scanner.Token == "propMover" )
		{	
			scanner.GetToken();	
			if( !LoadPropMover( "propMover.txt" ) )
				return FALSE;
		}
		else if( scanner.Token == "propCtrl" )
		{
			scanner.GetToken();
			LoadPropCtrl( "propCtrl.txt", &m_aPropCtrl );
		}
		else if( scanner.Token == "propItem" )
		{	
			scanner.GetToken();	
		
			LoadPropItem( "propItem.txt", &m_aPropItem );
			OnAfterLoadPropItem();
		}
		else if( scanner.Token == "character" )
		{
			scanner.GetToken();
			LoadCharacter( scanner.token );
		}
		else if( scanner.Token == "world" )
		{	
			scanner.GetToken();	
			g_WorldMng.LoadScript( scanner.token );
		}
		else if( scanner.Token == "terrain" )
		{	
			scanner.GetToken();	
		#ifdef __CLIENT
#ifdef __FLYFF_INITPAGE_EXT
			if(!CWndBase::m_Theme.m_bLoadTerrainScript)
				m_terrainMng.LoadScript( scanner.token );
#else //__FLYFF_INITPAGE_EXT
			m_terrainMng.LoadScript( scanner.token );
#endif //__FLYFF_INITPAGE_EXT
		#endif
		}
		else if( scanner.Token == "help" )
		{	
			scanner.GetToken();	
		#ifdef __CLIENT
			LoadHelp( scanner.token );
		#endif
		}
	} while(scanner.tok!=FINISHED);

	LoadMotionProp( "propMotion.txt" );
	LoadPropMoverEx( "PropMoverEx.inc" );
	LoadJobItem( "jobItem.inc" );
	LoadEtc( "etc.inc" );

	LoadPropEnchant( "propEnchant.inc" );

#ifdef __WORLDSERVER
	LoadConstant( "Constant.inc" );
	LoadPropGuildQuest( "propGuildQuest.inc" );
	LoadPropPartyQuest( "propPartyQuest.inc" );

	LoadDropEvent( "propDropEvent.inc" );
	LoadGiftbox( "propGiftbox.inc" );
	
	g_PackItem.Load("propPackItem.inc");

		LoadScriptDiePenalty( "DiePenalty.inc" );
#endif // __WORLDSERVER
	LoadScriptPK( "PKSetting.inc" );

	LoadPiercingAvail( "propItemEtc.inc" );

#ifdef __EVENT_0117
#ifdef __WORLDSERVER
	CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" );
#endif	// __WORLDSERVER
#endif	// __EVENT_0117

#ifdef __CLIENT	// Ŭ���̾�Ʈ�� �ε�
	CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" );
	LoadFilter( GetLangFileName( ::GetLanguage(), FILE_FILTER ) );
#endif	// __CLIENT

#ifdef __RULE_0615
	if (!nameValider.Load()) return FALSE;
#endif	// __RULE_0615

	CPetProperty::GetInstance()->LoadScript( "pet.inc" );
	CCollectingProperty::GetInstance()->LoadScript( "collecting.inc" );
	g_AccessoryProperty.LoadScript( "accessory.inc" );
	g_xRandomOptionProperty.LoadScript( "randomoption.inc" );

	if (!g_ticketProperties.LoadScript()) return FALSE;

	m_aPropQuest.Optimize();

#ifdef __EVE_MINIGAME
	#ifdef __WORLDSERVER
		LoadMiniGame();
	#endif // __WORLDSERVER
#endif // __EVE_MINIGAME

	LoadUltimateWeapon();
		
#ifdef __TRADESYS
	m_Exchange.Load_Script();
#endif // __TRADESYS

#if defined(__WORLDSERVER) // __EVENTLUA && __WORLDSERVER
	prj.m_EventLua.LoadScript();
#endif // __EVENTLUA && __WORLDSERVER

	#ifdef __WORLDSERVER
	g_GuildCombat1to1Mng.LoadScript();
	#endif // __WORLDSERVER

#ifdef __WORLDSERVER
#ifdef __JEFF_11_3
	LoadServerScript( "s.txt" );
#endif	// __JEFF_11_3
#endif	// __WORLDSERVER

#ifdef __EVENT_MONSTER
	CEventMonster::GetInstance()->LoadScript();
#endif // __EVENT_MONSTER

#ifdef __WORLDSERVER
	CSecretRoomMng::GetInstance()->LoadScript();
	CMonsterSkill::GetInstance()->LoadScript();

	CCreateMonster::GetInstance()->LoadScript();

#ifdef __QUIZ
	CQuiz::GetInstance()->LoadScript();
#endif // __QUIZ
	CCampusHelper::GetInstance()->LoadScript();

#ifdef __ENVIRONMENT_EFFECT
	CEnvironment::GetInstance()->LoadScript();
#endif // __ENVIRONMENT_EFFECT

#ifdef __GUILD_HOUSE_MIDDLE
	GuildHouseMng->LoadScript();
#endif // __GUILD_HOUSE_MIDDLE
#endif // __WORLDSERVER

	ProtectPropMover();

#ifdef __PERF_0226
	CPartsItem::GetInstance()->Init( this );
#endif	// __PERF_0226

#ifdef __WORLDSERVER
	CSLord::Instance()->CreateColleagues();
#else	// __WORLDSERVER
	CCLord::Instance()->CreateColleagues();
#endif	// __WORLDSERVER
#ifdef __WORLDSERVER
	CRangdaController::Instance()->LoadScript( "randomeventmonster.inc" );
	CTransformItemProperty::Instance()->LoadScript( "transformitem.inc" );
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
	LoadGuideMaxSeq();
	CRainbowRaceMng::GetInstance()->LoadScript();
#endif	// __WORLDSERVER
//	OutputDropItem();
//	return FALSE;

//	CCoupleTestCase ctc;
//	CTestCaseMgr::Instance()->Test();

#ifdef __WORLDSERVER
	CCoupleHelper::Instance()->Initialize( &g_dpDBClient, &g_DPSrvr );
#else	// __WORLDSERVER
	CCoupleHelper::Instance()->Initialize();
#endif	// __WORLDSERVER

	CTitleManager::Instance()->LoadTitle("honorList.txt");
#if defined( __CLIENT )
	LoadQuestDestination();
	LoadPatrolDestination();
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	m_MapInformationManager.LoadMapInformationData();
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM
	return TRUE;
}

#ifdef __WORLDSERVER
void CProject::SetGlobal( UINT type, float fValue )
{
	switch( type )
	{
	case GAME_RATE_ITEMDROP:
		if( fValue > 20.0f )
			fValue = 20.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fItemDropRate = fValue;
		break;
	case GAME_RATE_MONSTEREXP:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fMonsterExpRate = fValue;
		break;
	case GAME_RATE_MONSTERHIT:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fMonsterHitRate = fValue;
		break;
	case GAME_RATE_GOLDDROP:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fGoldDropRate = fValue;
		break;
	default:
		ASSERT( 0 );
	}

#ifdef __THA_0808
	if( ::GetLanguage() == LANG_THA )
	{
		m_fItemDropRate	= 1.0F;
		m_fMonsterExpRate	= 1.0F;
		m_fMonsterHitRate	= 1.0F;
		m_fGoldDropRate	= 1.0F;
	}
#endif	// __THA_0808
}

void CProject::ReadConstant( CScript& script )
{
	do 
	{
		script.GetToken(); 
		if( script.Token == "itemDropRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_ITEMDROP, script.GetFloat() );
		}
		else if( script.Token == "monsterExpRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_MONSTEREXP, script.GetFloat() );
		}
		else if( script.Token == "monsterHitRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_MONSTERHIT, script.GetFloat() );
		}
		else if( script.Token == "goldDropRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_GOLDDROP, script.GetFloat() );
		}
		else if( script.Token == "dwVagSP" )
		{
			script.GetToken();// bypass '='
			m_dwVagSP = script.GetNumber();
		}
		else if( script.Token == "dwExpertSP" )
		{
			script.GetToken();// bypass '='
			m_dwExpertSP = script.GetNumber();
		}
		else if( script.Token == "dwProSP" )
		{
			script.GetToken();// bypass '='
			m_dwProSP = script.GetNumber();
		}
	} 
	while( script.tok != FINISHED && *script.token != '}' );
}

BOOL CProject::LoadConstant( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	do 
	{
		if( script.Token == "lang" )
		{
			int nLang = script.GetNumber();
			if( ::GetLanguage() == nLang ) 
			{
				script.GetToken(); // {
				do 
				{
					if( script.Token == "formula" )
					{
						ReadConstant( script );
					}

					script.GetToken(); // server type
				} while( script.tok != FINISHED && *script.token != '}' );
				script.GetToken(); // }
			}
		}
		script.GetToken();
	} while( script.tok != FINISHED );

	return TRUE;
}
#endif //__WORLDSERVER

AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine, DWORD dwLevel )
{
	if( dwLevel < 1 )
		dwLevel = 1;
	
	int nskillIndex = dwSubDefine + dwLevel - 1;

	if( nskillIndex < 5)
		nskillIndex += 5;

	if( nskillIndex < 0 || nskillIndex >= m_aPropAddSkill.GetSize() )	
	{
		Error( "CProject::GetAddSkillProp ����ħ��. %d", nskillIndex );
		return 0;
	}
	
	return m_aPropAddSkill.GetAt( nskillIndex );
}

AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine )
{
	return GetAddSkillProp( dwSubDefine, 1 );
}

BOOL CProject::LoadEtc( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	int id;
	do {
		script.GetToken();
		if( script.Token == _T( "job" ) )
		{	
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken();
				_tcscpy( m_aJob[ id ].szName, script.token );
				m_aJob[ id ].dwJobBase = script.GetNumber();
				m_aJob[ id ].dwJobType = script.GetNumber();
				id = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "structure" ) )
		{	
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken();
				_tcscpy( m_aStructure[ id ].szName, script.token );
				id = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "Guild" ) )
		{
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken();
				_tcscpy( m_aGuildAppell[ id ].szName, script.token );
				id = script.GetNumber();
			}
		}
	} while( script.tok != FINISHED );
	return TRUE;
}

BOOL CProject::LoadMotionProp( LPCTSTR lpszFileName )
{
	// ����  ��� �ε���  �䱸Lv  �䱸����ġ  ǥ�ôܾ� ��Ʈ�� ����ܾ� 
	CScript script;
	if( script.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	MotionProp MotionProp;
	ZeroMemory( &MotionProp, sizeof( MotionProp ) );


	int nVer = script.GetNumber();  // version		061206 ma	8���� �� ��ǰ����� ���� ���� �߰�	propMotion.txt

	while( script.tok != FINISHED )
	{
		DWORD dwID = script.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			Error( "%s ID:%d last_read(ID:%d name:%s)", lpszFileName, dwID, MotionProp.dwID, MotionProp.szName);
			return FALSE;
		}

		MotionProp.dwID = dwID;
		MotionProp.dwMotion = script.GetNumber();
		script.GetToken();  
		script.GetToken();  
		if( strlen(script.m_mszToken) >= sizeof(MotionProp.szIconName) )
			Error( "LoadMotionProp : �޸�ħ��. %s ����ũ��(%d), ��Ʈ������(%d)", script.m_mszToken, sizeof(MotionProp.szIconName), strlen(script.m_mszToken) );
		strcpy( MotionProp.szIconName, script.m_mszToken );
		script.GetToken();  
		MotionProp.dwPlay = (DWORD)script.GetNumber();

		script.GetToken();
		_tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();
		_tcscpy( MotionProp.szDesc, script.m_mszToken );
		m_aPropMotion.SetAtGrow( MotionProp.dwID, &MotionProp);
		
		nVer = script.GetNumber(); 
	}
	m_aPropMotion.Optimize();
	return TRUE;

}

#ifdef __WORLDSERVER
BOOL CProject::LoadGuideMaxSeq()
{

	CScript script;
	if( script.Load(MakePath(DIR_CLIENT, "Guide.inc")) == FALSE )
		return FALSE;

	script.tok = 0;
	m_nMaxSequence = 0;

	while( script.tok != FINISHED )
	{
		script.GetNumber();

		script.GetToken(); // {
		if( *script.token  == '{' )
		{
			script.GetToken(); // BEGINNER
			script.GetNumber();
			script.GetToken(); // SHOWLEVEL
			script.GetNumber();
			script.GetToken(); // FLAG
			script.GetNumber();
			script.GetToken(); // KEY
			script.GetNumber();
			script.GetToken();
			script.Token;
			script.GetToken();
			if( strcmpi( script.token, "SEQUENCE" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "LEVEL" ) == 0 )
			{
				int	nCurSeq = script.GetNumber();
				if(m_nMaxSequence < nCurSeq) m_nMaxSequence = nCurSeq;
				script.GetToken();
			}
			if( strcmpi( script.token, "CONDITION" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "INPUTCHAR" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "INPUTSTR" ) == 0 )
			{
				script.GetToken();
				script.GetToken();	// }
				while(*script.token  != '}')
				{
					script.GetToken();	// }
				}
			}
			
		}
		else
			script.GetToken();
	}
	
	return TRUE;
}
#endif	// __WORLDSERVER

#ifdef __CLIENT
BOOL CProject::LoadHelp( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString strKeyword;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		strKeyword	= s.token;
		s.GetToken();	// {
		s.GetToken();
		CString string = "   ";
		while( *s.token != '}' )
		{
			 string		+= s.Token;
			 string		+= "\n\n   ";
			s.GetToken();
		}
		m_mapHelp.SetAt( strKeyword, string );
		s.GetToken();
	}
	return TRUE;
}
CString CProject::GetHelp( LPCTSTR lpStr )
{
	CString string;
	m_mapHelp.Lookup( lpStr, string );
	return string;
}
CString CProject::GetWordToolTip( LPCTSTR lpStr )
{
	CString string;
	m_mapWordToolTip.Lookup( lpStr, string );
	return string;
}
BOOL CProject::LoadFilter( LPCTSTR lpszFileName )
{	
	CScanner scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	FILTER filter;
	scanner.GetToken();

	while( scanner.tok != FINISHED )
	{
#ifdef __FILTER_0705
		BOOL bAlpha		= TRUE;
		int nLen	= lstrlen( scanner.token );
		for( int i = 0; i < nLen; i++ )
		{
#ifdef __VS2003
			if( !iswalpha( scanner.token[i] ) )
#else //__VS2003
			if( !isalpha( scanner.token[i] ) )
#endif //__VS2003
			{
				bAlpha	= FALSE;
				break;
			}
		}
		if( bAlpha )
			scanner.Token.MakeLower();
#endif	// __FILTER_0705
		strcpy( filter.m_szSrc, scanner.Token );
		scanner.GetToken();
		strcpy( filter.m_szDst, scanner.Token );
#ifdef __FILTER_0705
		if( bAlpha )
			m_mapAlphaFilter.emplace( filter.m_szSrc, filter.m_szDst );
		else
			m_mapNonalphaFilter.emplace( filter.m_szSrc, filter.m_szDst );
#else	// __FILTER_0705
		m_aWordFilter.Add( &filter );
#endif	// __FILTER_0705
		scanner.GetToken(); 
	}
#ifndef __FILTER_0705
	m_aWordFilter.Optimize();
#endif	// __FILTER_0705
	return TRUE;
}
BOOL CProject::LoadWordToolTip( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load(lpszFileName)==FALSE)
		return FALSE;
	CString strWord;
	CString strToolTip;
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		strWord = scanner.Token;
		scanner.GetToken();
		m_mapWordToolTip.SetAt( strWord, scanner.token );
		scanner.GetToken(); 
	}
	return TRUE;
}
#endif

BOOL CProject::LoadPropGuildQuest( LPCTSTR lpszFilename )
{
#ifdef __WORLDSERVER
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
#endif	// __WORLDSERVER
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
	GUILDQUESTPROP prop;

	int nQuestId	= s.GetNumber();	// id

	while( s.tok != FINISHED )
	{
		ZeroMemory( &prop, sizeof(prop) );

		s.GetToken();
		while( s.Token[0] != '}' )
		{
			if( s.Token == "Title" )
			{
				s.GetToken();	// =
				s.GetToken();
				lstrcpy( prop.szTitle, s.Token );
			}
			else if( s.Token == "Level" )
			{
				s.GetToken();	// =
				prop.nLevel		= s.GetNumber();
			}
			else if( s.Token == "Wormon" )
			{
				s.GetToken();	// =
				prop.dwWormon	= s.GetNumber();
			}
			else if( s.Token == "World" )
			{
				s.GetToken();	// =
				prop.dwWorldId	= s.GetNumber();
			}
			else if( s.Token == "Position" )
			{
				s.GetToken();	// =
				prop.vPos.x		= s.GetFloat();
				s.GetToken();	// ,
				prop.vPos.y		= s.GetFloat();
				s.GetToken();	// ,
				prop.vPos.z		= s.GetFloat();
			}
			else if( s.Token == "Region" )
			{
				s.GetToken();	// =
				prop.x1	= s.GetNumber();
				s.GetToken();	// ,
				prop.y1	= s.GetNumber();
				s.GetToken();	// ,
				prop.x2	= s.GetNumber();
				s.GetToken();	// ,
				prop.y2	= s.GetNumber();
			#ifdef __WORLDSERVER
				pProcessor->AddQuestRect( nQuestId, prop.x1, prop.y1, prop.x2, prop.y2 );
			#endif	// __WORLDSERVER
			}
			else if( s.Token == "State" )
			{
				int nState	= s.GetNumber();
				s.GetToken();	// key or anything
				while( s.Token[0] != '}' )
				{
					if( s.Token == "desc" )
					{
						s.GetToken();	// =
						s.GetToken();
						lstrcpy( prop.szDesc[nState], s.Token );
					}
					s.GetToken();	// key or anything
				}
			}
			s.GetToken();
		}
		m_aPropGuildQuest.SetAtGrow( nQuestId, &prop );
		nQuestId	= s.GetNumber();	// id
	}
	m_aPropGuildQuest.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropPartyQuest( LPCTSTR lpszFilename )
{
#ifdef __WORLDSERVER
	CPartyQuestProcessor* pProcessor	= CPartyQuestProcessor::GetInstance();
#endif	// __WORLDSERVER
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
//	PARTYQUESTPROP prop;
	
	int nQuestId	= s.GetNumber();	// id
	
	while( s.tok != FINISHED )
	{
		PARTYQUESTPROP prop;
//		ZeroMemory( &prop, sizeof(prop) );
		
		s.GetToken();
		while( s.Token[0] != '}' )
		{
			if( s.Token == "Title" )
			{
				s.GetToken();	// =
				s.GetToken();
				lstrcpy( prop.szTitle, s.Token );
			}
			else if( s.Token == "Level" )
			{
				s.GetToken();	// =
				prop.nLevel		= s.GetNumber();
			}
			else if( s.Token == "Wormon" )
			{
				prop.vecWormon.clear();
				int nErrCnt = 0;
				WORMON WorMon;
				s.GetToken();
				while( s.Token[0] != '}' )
				{
					memset( &WorMon, 0, sizeof(WORMON) );
					nErrCnt++;

					WorMon.dwWormon = s.GetNumber();
					s.GetToken();	// ,
					WorMon.vPos.x   = s.GetFloat();
					s.GetToken();	// ,
					WorMon.vPos.y   = s.GetFloat();
					s.GetToken();	// ,
					WorMon.vPos.z   = s.GetFloat();
					s.GetToken();	// ,
					
					prop.vecWormon.push_back( WorMon );

					if( nErrCnt > 1000 )
					{
						Error( "propPartyQuest.inc = %s Error", prop.szTitle );
						return FALSE;
					}
				}				
			}
			else if( s.Token == "World" )
			{
				s.GetToken();	// =
				prop.dwWorldId	= s.GetNumber();
			}
			else if( s.Token == "WorldKey" )
			{
				s.GetToken();	// =
				s.GetToken();
				lstrcpy( prop.szWorldKey, s.Token );
			}	
			else if( s.Token == "Region" )
			{
				s.GetToken();	// =
				prop.x1	= s.GetNumber();
				s.GetToken();	// ,
				prop.y1	= s.GetNumber();
				s.GetToken();	// ,
				prop.x2	= s.GetNumber();
				s.GetToken();	// ,
				prop.y2	= s.GetNumber();
#ifdef __WORLDSERVER
				pProcessor->AddQuestRect( nQuestId, prop.dwWorldId, prop.x1, prop.y1, prop.x2, prop.y2 );
#endif	// __WORLDSERVER
			}
			else if( s.Token == "State" )
			{
				int nState	= s.GetNumber();
				s.GetToken();	// key or anything
				while( s.Token[0] != '}' )
				{
					if( s.Token == "desc" )
					{
						s.GetToken();	// =
						s.GetToken();
						lstrcpy( prop.szDesc[nState], s.Token );
					}
					s.GetToken();	// key or anything
				}
			}
			s.GetToken();
		}
		bool bResult = m_propPartyQuest.emplace( nQuestId, prop ).second;
		nQuestId	= s.GetNumber();	// id
	}
	return TRUE;
}


BOOL CProject::LoadPropQuest( LPCTSTR lpszFileName, BOOL bOptimize )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	QuestProp propQuest;
	TCHAR szLinkChar[ 10 ][ 64 ];
	int szLinkCharNum;
	int szEndCondCharNum;
	int nQuest = script.GetNumber();  // id
	static DWORD dwGoalIndex = 0;

	while( script.tok != FINISHED )
	{
		ZeroMemory( &propQuest, sizeof( propQuest ) );

		BOOL bEndCondCharacter = FALSE;
		propQuest.m_nBeginCondSex = -1;
		propQuest.m_wId = nQuest;

		QuestPropItem aEndRewardItem[ MAX_QUESTCONDITEM ]; 
		int nEndRewardItemNum = 0;
		ZeroMemory( aEndRewardItem, sizeof( aEndRewardItem ) );

		QuestPropItem aBeginCondItem[ MAX_QUESTCONDITEM ]; 
		int nBeginCondItemNum = 0;
		ZeroMemory( aBeginCondItem, sizeof( aBeginCondItem ) );

		QuestPropItem aBeginCondNotItem[ MAX_QUESTCONDITEM ]; 
		int nBeginCondNotItemNum = 0;
		ZeroMemory( aBeginCondNotItem, sizeof( aBeginCondNotItem ) );		

		QuestPropItem aEndCondOneItem[ MAX_QUESTCONDITEM ]; 
		int nEndCondOneItemNum = 0;
		ZeroMemory( aEndCondOneItem, sizeof( aEndCondOneItem ) );				

		QuestPropItem aEndCondItem[ MAX_QUESTCONDITEM ]; 
		int nEndCondItemNum = 0;
		ZeroMemory( aEndCondItem, sizeof( aEndCondItem ) );

		ZeroMemory( szLinkChar, sizeof( szLinkChar ) );
		szLinkCharNum = 0;
		szEndCondCharNum = 0;

		#if defined( __WORLDSERVER ) 
			ZeroMemory( propQuest.m_apQuestDialog, sizeof( propQuest.m_apQuestDialog ) );
		#endif

		propQuest.m_nBeginCondPetLevel	= -1;	// PL_EGG�� 0�̹Ƿ� �⺻���� -1
		propQuest.m_nEndCondPetLevel	= -1;	// PL_EGG�� 0�̹Ƿ� �⺻���� -1
		propQuest.m_nBeginCondTutorialState		= -1;

		script.GetToken();
		int nBlock = 1;
		while( nBlock && script.tok != FINISHED )
		{
			script.GetToken();
			if( script.Token == "{" )			
				nBlock++;
			else
			if( script.Token == "}" )			
				nBlock--;
			else
			if( script.Token == "SetTitle" )
			{
				script.GetToken(); // (
				strcpy(	propQuest.m_szTitle, GetLangScript( script ) );
			}
			else
			if( script.Token == "m_szTitle" )
			{
				script.GetToken(); // (
				script.GetToken();
				strcpy(	propQuest.m_szTitle, script.Token );
			}
			else
			if( script.Token == "SetNPCName" )
			{
				script.GetToken(); // (
				strcpy(	propQuest.m_szNpcName, GetLangScript( script ) );
			}
			else
			if( script.Token == "m_szNPC" )
			{
				script.GetToken();
				script.GetToken();
			}
			else
			if( script.Token == "SetCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // key
				char szKey[512] = {0,};
				strcpy( szKey, script.token );
				
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					int nLang = script.GetNumber();
					script.GetToken(); // ,
					int nSubLang = 0;
					if( script.Token == "," )
						nSubLang = script.GetNumber();
					if( nLang != ::GetLanguage() || nSubLang != ::GetSubLanguage() )
						continue;
				}
				szLinkCharNum = 0;
				strcpy( szLinkChar[ szLinkCharNum ], szKey ); 
				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum++ ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awSrcQuest.Add( nQuest );
					lpCharacter->m_anSrcQuestItem.Add( 0 );
				}
			}
			else
			if( script.Token == "SetMultiCharacter" )
			{
				script.GetToken(); // (
				szLinkCharNum = 0;
				do
				{
					script.GetToken(); // key
					strcpy( szLinkChar[ szLinkCharNum ], script.token ); 
					script.GetToken(); // ,
					int nItem = script.GetNumber();
					LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum ] );
					if( lpCharacter ) 
					{
						lpCharacter->m_awSrcQuest.Add( nQuest );
						lpCharacter->m_anSrcQuestItem.Add( nItem );
					}
					else
					{
						Error( "%s(%d) : %s�� �������� ����", lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
					}
					szLinkCharNum++;
					script.GetToken(); // , or )
				} while( *script.token != ')' && szLinkCharNum < 10 );
			}
			else
			// ���� - ���� 
			if( script.Token == "SetBeginCondSex" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondSex  = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondSkillLvl" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondSkillIdx = script.GetNumber(); 
				script.GetToken(); // ,
				propQuest.m_nBeginCondSkillLvl = script.GetNumber();
			}
			else
			if( script.Token == "SetBeginCondPKValue" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondPKValue = script.GetNumber();
			}
			else
			if( script.Token == "SetBeginCondNotItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetBeginCondNotItem ���� �ʰ�(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nSex = nSex;
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nType = nType;
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nJobOrItem =  nJobOrItem;
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nItemNum = script.GetNumber();
				nBeginCondNotItemNum++;
			}
			else
			if( script.Token == "SetBeginCondLevel" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondLevelMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondLevelMax = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondParty" ) 
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondParty = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondPartyNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondPartyNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nBeginCondPartyLeader = script.GetNumber(); 
			}
			else 
			if( script.Token == "SetBeginCondGuild" ) 
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondGuild = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondGuildNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondGuildNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nBeginCondGuildLeader = script.GetNumber(); 
			}
			else
			if( script.Token == "SetBeginCondJob" )
			{
				script.GetToken(); // (
				do
				{
					propQuest.m_nBeginCondJob[ propQuest.m_nBeginCondJobNum++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && propQuest.m_nBeginCondJobNum < MAX_JOB );
			}
			else
			if( script.Token == "SetBeginCondPreviousQuest" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondPreviousQuestType = script.GetNumber(); // 0 ������, 1 ���� ����Ʈ �Ϸ�
				script.GetToken(); // ,
				int nCnt = 0;
				do
				{
					propQuest.m_anBeginCondPreviousQuest[ nCnt++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && nCnt < 6 );
			}
			else
			if( script.Token == "SetBeginCondExclusiveQuest" )
			{
				script.GetToken(); // (
				int nCnt = 0;
				do
				{
					propQuest.m_anBeginCondExclusiveQuest[ nCnt++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && nCnt < 6 );
			}
			else
			if( script.Token == "SetBeginCondItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetBeginCondItem ���� �ʰ�(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
				aBeginCondItem[ nBeginCondItemNum ].m_nSex = nSex;
				aBeginCondItem[ nBeginCondItemNum ].m_nType = nType;
				aBeginCondItem[ nBeginCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aBeginCondItem[ nBeginCondItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aBeginCondItem[ nBeginCondItemNum ].m_nItemNum = script.GetNumber();
				nBeginCondItemNum++;
			}
			else
			if( script.Token == "SetBeginCondDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondDisguiseMoverIndex = script.GetNumber();
			}
			else
			//////////////////////////////////////////////////////////////////
			// ���� - �߰� �� ���� 
			//////////////////////////////////////////////////////////////////
			if( script.Token == "SetBeginSetDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginSetDisguiseMoverIndex = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginSetAddGold" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginSetAddGold = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginSetAddItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetBeginSetAddItem Idx �� ���� �ʰ�(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nBeginSetAddItemIdx[nIdx] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginSetAddItemNum[nIdx] = script.GetNumber();
			}
			else if( script.Token == "SetBeginCondPetExp" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondPetExp	= script.GetNumber();
			}
			else if( script.Token == "SetBeginCondPetLevel" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondPetLevel	= script.GetNumber();
			}
			else if( script.Token == "SetBeginCondTutorialState" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondTutorialState		= script.GetNumber();
			}
			else if( script.Token == "SetBeginCondTSP" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondTSP	= script.GetNumber();
			}
			else 
			////////////////////////// ���� - ���� 
			if( script.Token == "SetEndCondParty" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondParty = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondPartyNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondPartyNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nEndCondPartyLeader = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondGuild" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondGuild = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondGuildNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondGuildNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nEndCondGuildLeader = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondState" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondState = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondCompleteQuest" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondCompleteQuestOper = script.GetNumber();
				script.GetToken(); // ,
				int nIdx = 0;
				while( *script.token != ')' )
				{
					propQuest.m_nEndCondCompleteQuest[ nIdx++ ] = script.GetNumber();
					script.GetToken(); // ,
				}
			}
			else
			if( script.Token == "SetEndCondSkillLvl" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondSkillIdx = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondSkillLvl = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondLevel" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondLevelMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondLevelMax = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondExpPercent" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondExpPercentMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondExpPercentMax = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondGold = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondOneItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndCondOneItem ���� �ʰ�(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
				
				aEndCondOneItem[ nEndCondOneItemNum ].m_nSex = nSex;
				aEndCondOneItem[ nEndCondOneItemNum ].m_nType = nType;
				aEndCondOneItem[ nEndCondOneItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndCondOneItem[ nEndCondOneItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndCondOneItem[ nEndCondOneItemNum ].m_nItemNum = script.GetNumber();
				nEndCondOneItemNum++;
			}
			else
			if( script.Token == "SetEndCondLimitTime" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondLimitTime = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndCondItem ���� �ʰ�(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,

				aEndCondItem[ nEndCondItemNum ].m_nSex = nSex;
				aEndCondItem[ nEndCondItemNum ].m_nType = nType;
				aEndCondItem[ nEndCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndCondItem[ nEndCondItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndCondItem[ nEndCondItemNum ].m_nItemNum = script.GetNumber();
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_dwGoalIndex = ++dwGoalIndex;
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}

				nEndCondItemNum++;
			}
			else 
			if( script.Token == "SetEndCondKillNPC" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 1 )
				{
					nIdx = 0;
					Error( "%s(%d) SetEndCondKillNPC���� Idx �� ���� �ʰ�(0~1)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nEndCondKillNPCIdx[nIdx] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondKillNPCNum[nIdx] = script.GetNumber();
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					propQuest.m_KillNPCGoalData[ nIdx ].m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_KillNPCGoalData[ nIdx ].m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_KillNPCGoalData[ nIdx ].m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_KillNPCGoalData[ nIdx ].m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
			}
			else
			if( script.Token == "SetEndCondPatrolZone" )
			{
				script.GetToken(); // (
				propQuest.m_dwEndCondPatrolWorld = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.left = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.top = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.right = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.bottom = script.GetNumber();
				propQuest.m_rectEndCondPatrol.NormalizeRect();
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					script.GetToken_NoDef();
					propQuest.m_dwPatrolDestinationID = atoi( script.Token.Right( 4 ) );
					script.GetToken(); // ,
					propQuest.m_PatrolWorldGoalData.m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_PatrolWorldGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_PatrolWorldGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_PatrolWorldGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
			}
			else
			if( script.Token == "SetEndCondCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // "key"
				// ��Ʈ���� ������ szLinkChar�� ���õ� ������ ��ü�Ѵ�. �� �ڱ� �ڽ��̴�.
				if( strlen( script.token ) == 0 )
					strcpy( propQuest.m_szEndCondCharacter, szLinkChar[ 0 ] );
				else
					strcpy( propQuest.m_szEndCondCharacter, script.token );
				LPCHARACTER lpCharacter = GetCharacter( propQuest.m_szEndCondCharacter );
				if( lpCharacter ) 
				{
					lpCharacter->m_awDstQuest.Add( nQuest );
					lpCharacter->m_anDstQuestItem.Add( 0 );
				}
				bEndCondCharacter = TRUE;
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					propQuest.m_MeetCharacterGoalData.m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_MeetCharacterGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_MeetCharacterGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_MeetCharacterGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
			}
			else
			if( script.Token == "SetEndCondMultiCharacter" )
			{
				script.GetToken(); // (
				szEndCondCharNum = 0;
				propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
				ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
				do
				{
					script.GetToken(); // key
					strcpy( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ], script.token ); 

					script.GetToken(); // ,
					int nItem = script.GetNumber();

					LPCHARACTER lpCharacter = GetCharacter( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ] );
					if( lpCharacter ) 
					{
						lpCharacter->m_awDstQuest.Add( nQuest );
						lpCharacter->m_anDstQuestItem.Add( nItem );
					}
					else
					{
						Error( "%s(%d) : %s�� �������� ����", lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
					}
					szEndCondCharNum++;
					script.GetToken(); // , or )
				} while( *script.token != ')' && szEndCondCharNum < 10 );
				bEndCondCharacter = TRUE;
			}
			else
			if( script.Token == "SetEndCondDialog" )
			{
				script.GetToken(); // (
				script.GetToken(); 
				strcpy( propQuest.m_szEndCondDlgCharKey, script.token );
				script.GetToken(); // ,
				script.GetToken(); 
				strcpy( propQuest.m_szEndCondDlgAddKey, script.token );
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					propQuest.m_DialogCharacterGoalData.m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_DialogCharacterGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_DialogCharacterGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_DialogCharacterGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
			}
			else if( script.Token == "SetEndCondPetLevel" )
			{	// SetEndCondPetLevel( 0 );
				script.GetToken();	// (
				propQuest.m_nEndCondPetLevel	= script.GetNumber();
			}
			else if( script.Token == "SetEndCondPetExp" )
			{
				// SetEndCondPetExp( 100 );
				script.GetToken();	// (
				propQuest.m_nEndCondPetExp	= script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondDisguiseMoverIndex = script.GetNumber();
			}
			else
			if( script.Token == "SetParam" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetParam Idx �� ���� �ʰ�(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nParam[ nIdx ] = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondTSP" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondTSP  = script.GetNumber();
			}
			else
			////////////////////////// ��ȭ - ���� 
			if( script.Token == "SetDlgRewardItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetDlgRewardItem Idx �� ���� �ʰ�(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nDlgRewardItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nDlgRewardItemNum[ nIdx ] = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardItem" )
			{
				script.GetToken(); // ( 

				int nSex = script.GetNumber();
				script.GetToken(); // ,

				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndRewardItem �� ���� �ʰ�(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,

				aEndRewardItem[ nEndRewardItemNum ].m_nSex = nSex;
				aEndRewardItem[ nEndRewardItemNum ].m_nType = nType;
				aEndRewardItem[ nEndRewardItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndRewardItem[ nEndRewardItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndRewardItem[ nEndRewardItemNum ].m_nItemNum = script.GetNumber();
				script.GetToken();
				if( script.Token == "," )	// , (�ͼӼ���)
					aEndRewardItem[ nEndRewardItemNum ].m_byFlag = static_cast<BYTE>( script.GetNumber() );
			
				nEndRewardItemNum++;
			}
#ifdef __JEFF_11
			else if( script.Token == "SetEndRewardItemWithAbilityOption" )
			{
				script.GetToken();	// ( 
				int nSex	= script.GetNumber();
				script.GetToken();	// ,
				int nType	= script.GetNumber();
				script.GetToken();	// ,
				int nJobOrItem	= script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem	= -1;
					Error( "%s(%d) SetEndRewardItem �� ���� �ʰ�(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken();	// ,
				aEndRewardItem[nEndRewardItemNum].m_nSex	= nSex;
				aEndRewardItem[nEndRewardItemNum].m_nType	= nType;
				aEndRewardItem[nEndRewardItemNum].m_nJobOrItem	=  nJobOrItem;
				aEndRewardItem[nEndRewardItemNum].m_nItemIdx	= script.GetNumber();
				script.GetToken();	// ,
				aEndRewardItem[nEndRewardItemNum].m_nItemNum	= script.GetNumber();
				script.GetToken();	// ,
				aEndRewardItem[nEndRewardItemNum].m_nAbilityOption	= script.GetNumber();
				script.GetToken();
				if( script.Token == "," )	// , (�ͼӼ���)
					aEndRewardItem[ nEndRewardItemNum ].m_byFlag = static_cast<BYTE>( script.GetNumber() );
			
				nEndRewardItemNum++;
			}
#endif	// __JEFF_11
			else
			if( script.Token == "SetEndRewardGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardGoldMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardGoldMax = script.GetNumber();
			}
			else if( script.Token == "SetEndRewardPetLevelup" )
			{
				propQuest.m_bEndRewardPetLevelup	= TRUE;
			}
			else
			if( script.Token == "SetEndRewardExp" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardExpMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardExpMax = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardSkillPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardSkillPoint = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardPKValue" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardPKValueMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardPKValueMax = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardTeleport" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardTeleport = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardTeleportPos.x = script.GetFloat();
				script.GetToken(); // ,
				propQuest.m_nEndRewardTeleportPos.y = script.GetFloat();
				script.GetToken(); // ,
				propQuest.m_nEndRewardTeleportPos.z = script.GetFloat();
			}
			else
			if( script.Token == "SetEndRewardHide" )
			{
				script.GetToken(); // (
				propQuest.m_bEndRewardHide = ( script.GetNumber() != 0 );
			}
			else
			// ���� ����Ʈ ���� 
			if( script.Token == "SetEndRemoveQuest" )
			{
				script.GetToken(); // (
				int nRemoveQuestIdx = 0;
				do
				{
					propQuest.m_anEndRemoveQuest[ nRemoveQuestIdx++ ] = script.GetNumber();;
					script.GetToken(); // , or )
				} while( *script.token != ')' && nRemoveQuestIdx < MAX_QUESTREMOVE );
			}
			else
			// ���� ������ ���� 
			if( script.Token == "SetEndRemoveItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 7 )
				{
					nIdx = 0;
					Error( "%s(%d) SetEndRemoveItem Idx �� ���� �ʰ�(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemNum[ nIdx ] = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndRemoveGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveGold = script.GetNumber();
			}
			else 
			if( script.Token == "SetRepeat" )
			{
				script.GetToken(); // (
				propQuest.m_bRepeat = ( script.GetNumber() != 0 );
			}
			else 
			if( script.Token == "QuestItem" )
			{
				QUESTITEM qi;
				script.GetToken();	// (
				DWORD dwMoverIdx = script.GetNumber();
				qi.dwQuest	= nQuest;//script.GetNumber();
				qi.dwState	= 0;//script.GetNumber();
				script.GetToken();	// ,
				qi.dwIndex	= script.GetNumber();
				ASSERT( qi.dwIndex );
				script.GetToken();	// ,
				qi.dwProbability	= script.GetNumber();	//
				script.GetToken();	// ,
				qi.dwNumber	= script.GetNumber();	//
				script.GetToken();	// )
				MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
				pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
			}
			else
			if( script.Token == "SetEndRewardTSP" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardTSP  = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRemoveTSP" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveTSP  = script.GetNumber();
			}
			else
			//////////////////////////
			if( script.Token == "SetDialog" )
			{
				script.GetToken(); // (
				int nNum = script.GetNumber();
				script.GetToken(); // ,
				CString string = GetLangScript( script );
			#if defined( __WORLDSERVER )
				if( nNum < 32 )
				{
					propQuest.m_apQuestDialog[ nNum ] = new CHAR[ string.GetLength() + 1 ];
					strcpy(	propQuest.m_apQuestDialog[ nNum ], string );
				}
			#endif 
			}
			if( script.Token == "SetPatrolZoneName" )
			{
				script.GetToken(); // (
				//int nNum = script.GetNumber();
				//script.GetToken(); // ,
				CString string = GetLangScript( script );
				if( string.GetLength() < 64 )
					strcpy( propQuest.m_szPatrolZoneName, string );
				else
					Error( "%s(%d) SetPatrolZoneName ��Ʈ�� 64����Ʈ �ʰ�", lpszFileName, script.GetLineNum() ); 
			}
			else
			if( script.Token == "SetHeadQuest" )
			{
				script.GetToken(); // (
				propQuest.m_nHeadQuest = script.GetNumber();
				if( propQuest.m_nHeadQuest >= 1800 && propQuest.m_nHeadQuest <= 1999 )
				{
					switch( propQuest.m_nHeadQuest )
					{
					case 1992:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_EVENT;
							break;
						}
					case 1993:
					case 1994:
					case 1995:
					case 1997:
					case 1998:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_NORMAL;
							break;
						}
					case 1996:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_SCENARIO;
							break;
						}
					case 1999:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_REQUEST;
							break;
						}
					default:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_SCENARIO;
						}
					}
				}
			}
			else
			if( script.Token == "SetQuestType" )
			{
				script.GetToken(); // (
				propQuest.m_nQuestType = script.GetNumber();
			}
			else
			if( script.Token == "SetRemove" )
			{
				script.GetToken(); // (
				propQuest.m_bNoRemove = !script.GetNumber();
			}
			else
			if( script.Token == "state" )
			{
				int nState = script.GetNumber();

				QuestState* pQuestState = new QuestState;
				propQuest.m_questState[ nState ] = pQuestState;
				pQuestState->m_szDesc[ 0 ] = 0;
				script.GetToken(); // key or anything

				int temp=0;


				while( script.Token[0] != '}' )
				{
					temp++;
					
					if( script.Token == "QuestItem" )
					{
						QUESTITEM qi;
						script.GetToken();	// (
						DWORD dwMoverIdx = script.GetNumber();
						qi.dwQuest	= nQuest;//script.GetNumber();
						qi.dwState	= nState;//script.GetNumber();
						script.GetToken();	// ,
						qi.dwIndex	= script.GetNumber();
						script.GetToken();	// ,
						qi.dwProbability	= script.GetNumber();	//
						script.GetToken();	// ,
						qi.dwNumber	= script.GetNumber();	//
						script.GetToken();	// )
						MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
						pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
					}
					else
					if( script.Token == "SetDesc" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
						if( string.GetLength() < 512 ) // null ������ 255
							strcpy(	pQuestState->m_szDesc, string );
						else
							Error( "%s(%d) LoadPropQuest�� SetDesc ���� �ʰ�", lpszFileName, script.GetLineNum() ); 

					}
					else
					if( script.Token == "SetCond" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
					}
					else
					if( script.Token == "SetStatus" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
					}
					script.GetToken(); // key or anything
				}
			}
		}
		// EndCondCharacter������ �ȵƴٸ� LinkChar�� ��ü�Ѵ�.
		if( bEndCondCharacter == FALSE )
		{
			// ��Ʈ���� ������ szLinkChar�� ���õ� ������ ��ü�Ѵ�. �� �ڱ� �ڽ��̴�.
			if( szLinkCharNum == 1 )
			{
				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ 0 ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awDstQuest.Add( nQuest );
					lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( 0 ) );
				}
				strcpy( propQuest.m_szEndCondCharacter, szLinkChar[ 0 ] );
			}
			else
			if( szLinkCharNum > 1 )
			{
				propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
				ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
				for( int i = 0; i < szLinkCharNum; i++ )
				{
					strcpy( &propQuest.m_lpszEndCondMultiCharacter[ i * 64 ], szLinkChar[ 0 ] );
					LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ i ] );
					if( lpCharacter )
					{
						lpCharacter->m_awDstQuest.Add( nQuest );
						lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( i ) );
					}
				}
			}
		}
		if( nBeginCondItemNum )
		{
			propQuest.m_paBeginCondItem = new QuestPropItem[ nBeginCondItemNum ];
			memcpy( propQuest.m_paBeginCondItem, aBeginCondItem, sizeof( QuestPropItem ) * nBeginCondItemNum );
			propQuest.m_nBeginCondItemNum = nBeginCondItemNum;
		}
		if( nBeginCondNotItemNum )
		{
			propQuest.m_paBeginCondNotItem = new QuestPropItem[ nBeginCondNotItemNum ];
			memcpy( propQuest.m_paBeginCondNotItem, aBeginCondNotItem, sizeof( QuestPropItem ) * nBeginCondNotItemNum );
			propQuest.m_nBeginCondNotItemNum = nBeginCondNotItemNum;
		}
		
		if( nEndCondOneItemNum )
		{
			propQuest.m_paEndCondOneItem = new QuestPropItem[ nEndCondOneItemNum ];
			memcpy( propQuest.m_paEndCondOneItem, aEndCondOneItem, sizeof( QuestPropItem ) * nEndCondOneItemNum );
			propQuest.m_nEndCondOneItemNum = nEndCondOneItemNum;
		}		
		if( nEndCondItemNum )
		{
			propQuest.m_paEndCondItem = new QuestPropItem[ nEndCondItemNum ];
			memcpy( propQuest.m_paEndCondItem, aEndCondItem, sizeof( QuestPropItem ) * nEndCondItemNum );
			propQuest.m_nEndCondItemNum = nEndCondItemNum;
		}
		if( nEndRewardItemNum )
		{
			propQuest.m_paEndRewardItem = new QuestPropItem[ nEndRewardItemNum ];
			memcpy( propQuest.m_paEndRewardItem, aEndRewardItem, sizeof( QuestPropItem ) * nEndRewardItemNum );
			propQuest.m_nEndRewardItemNum = nEndRewardItemNum;
		}

		BOOL bAdd	= TRUE;
		// ���� ���� �� ����Ʈ�� �븸�� ����	// ����Ʈ ����
		if( //::GetLanguage() != LANG_TWN && 
			nQuest == QUEST_KAWIBAWIBO01 )
			bAdd	= FALSE;
		if( bAdd )
			m_aPropQuest.SetAtGrow( nQuest, &propQuest );

		nQuest = script.GetNumber();  // id
	}
	if( bOptimize )
		m_aPropQuest.Optimize();

	return TRUE;
}

const DWORD	dwDefault	= (DWORD)0xFFFFFFFF;
// propSkill.txt �ε� ���Ŀ� �ؾ��Ѵ�.
BOOL CProject::LoadPropAddSkill( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	AddSkillProp propAddSkill;
	int i = script.GetNumber( TRUE ); // id
	while ( script.tok != FINISHED )
	{
		if( i == 0 )			// �̰� �����ϱ� ���ϵڿ� �����ִ°͵� ������ �д���
			break;
		propAddSkill.dwID = i;
		propAddSkill.dwName = script.GetNumber( TRUE );
		propAddSkill.dwSkillLvl = script.GetNumber( TRUE );
		propAddSkill.dwAbilityMin = script.GetNumber( TRUE );
		propAddSkill.dwAbilityMax = script.GetNumber( TRUE );
		propAddSkill.dwAbilityMinPVP	= script.GetNumber( TRUE );		// ���� ȿ�� �ּ�
		propAddSkill.dwAbilityMaxPVP	= script.GetNumber( TRUE );		// ���� ȿ�� �ִ�
		propAddSkill.dwAttackSpeed = script.GetNumber( TRUE );
		propAddSkill.dwDmgShift = script.GetNumber( TRUE );
		propAddSkill.nProbability = script.GetNumber( TRUE );
		propAddSkill.nProbabilityPVP	= script.GetNumber( TRUE );
		propAddSkill.dwTaunt = script.GetNumber( TRUE );
		propAddSkill.dwDestParam1 = script.GetNumber( TRUE );
		propAddSkill.dwDestParam2 = script.GetNumber( TRUE );
		propAddSkill.nAdjParamVal1 = script.GetNumber( TRUE );
		propAddSkill.nAdjParamVal2 = script.GetNumber( TRUE );
		propAddSkill.dwChgParamVal1 = script.GetNumber( TRUE );
		propAddSkill.dwChgParamVal2 = script.GetNumber( TRUE );
		propAddSkill.nDestData1[0] = script.GetNumber( TRUE );
		propAddSkill.nDestData1[1] = script.GetNumber( TRUE );
		propAddSkill.nDestData1[2] = script.GetNumber( TRUE );
		propAddSkill.dwActiveSkill = script.GetNumber( TRUE );
		propAddSkill.dwActiveSkillRate = script.GetNumber( TRUE );
		propAddSkill.dwActiveSkillRatePVP	= script.GetNumber( TRUE );
		propAddSkill.nReqMp = script.GetNumber( TRUE );
		propAddSkill.nReqFp = script.GetNumber( TRUE );
		propAddSkill.dwCooldown		= script.GetNumber( TRUE );
		propAddSkill.dwCastingTime	= script.GetNumber( TRUE );
		propAddSkill.dwSkillRange = script.GetNumber( TRUE );
		propAddSkill.dwCircleTime = script.GetNumber( TRUE );
		propAddSkill.dwPainTime  = script.GetNumber( TRUE );
		propAddSkill.dwSkillTime = script.GetNumber( TRUE );
		propAddSkill.nSkillCount = script.GetNumber( TRUE );
		propAddSkill.dwSkillExp = script.GetNumber( TRUE );
		
		propAddSkill.dwExp = script.GetNumber( TRUE );
		propAddSkill.dwComboSkillTime = script.GetNumber( TRUE );

		if( propAddSkill.dwAbilityMinPVP == dwDefault )
			propAddSkill.dwAbilityMinPVP	= propAddSkill.dwAbilityMin;
		if( propAddSkill.dwAbilityMaxPVP == dwDefault )
			propAddSkill.dwAbilityMaxPVP	= propAddSkill.dwAbilityMax;

		ItemProp* pSkillProp	= GetSkillProp( propAddSkill.dwName );
		if( pSkillProp )
		{
			if( propAddSkill.dwCooldown == dwDefault )
			{
				propAddSkill.dwCooldown	= pSkillProp->dwSkillReady;
				if( pSkillProp->nVer < 9 && propAddSkill.dwCastingTime != dwDefault )
					propAddSkill.dwCastingTime	/= 4;
			}
		}
		
		if( propAddSkill.nProbabilityPVP == dwDefault )
			propAddSkill.nProbabilityPVP	= propAddSkill.nProbability;
		if( propAddSkill.dwActiveSkillRatePVP == dwDefault )
			propAddSkill.dwActiveSkillRatePVP	= propAddSkill.dwActiveSkillRate;

		m_aPropAddSkill.SetAtGrow( i, &propAddSkill );

		// TRACE( "PropAddSkill : %d %d %d\r\n", i, propAddSkill.dwName, propAddSkill.dwSkillLvl );
		i = script.GetNumber( TRUE ); 
	}
	m_aPropAddSkill.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropMoverEx( LPCTSTR szFileName )
{
	CScript script;
	int		nBlock = 0;
 
	if(script.Load(szFileName)==FALSE)
		return FALSE;
	CString string;
	int nVal = script.GetNumber(); // Mover Id

	do 
	{
		MoverProp* pProp = GetMoverProp( nVal );
		if(pProp == NULL)
		{
			TRACE("MoverAddProp���� �����ؾߵ� base Property %s�� ����. %s\n",script.Token);
			continue;
		}
		
		// �ʱ�ȭ
		pProp->m_nEvasionHP        = 0;
		pProp->m_nEvasionSec       = 0;
		pProp->m_nRunawayHP        = -1;
		pProp->m_nCallHP           = -1;
		pProp->m_nCallHelperMax    = 0;
		ZeroMemory( pProp->m_nCallHelperIdx  , sizeof( pProp->m_nCallHelperIdx   ) );
		ZeroMemory( pProp->m_nCallHelperNum  , sizeof( pProp->m_nCallHelperNum   ) );
		ZeroMemory( pProp->m_bCallHelperParty, sizeof( pProp->m_bCallHelperParty ) );

		int nCallNum = 0;

		pProp->m_nChangeTargetRand = 10;
		pProp->m_dwAttackMoveDelay = 0;
		pProp->m_bPartyAttack      = 0;
		pProp->m_dwRunawayDelay    = SEC( 1 );
		pProp->m_nAttackFirstRange = 10;

		script.GetToken(); // {
		script.GetToken(); 
		while(*script.token != '}')
		{
			if(script.Token == ";") 
			{ 
				script.GetToken(); 
				continue; 
			} 
			if( strcmpi( script.Token, "AI" ) == 0 )		// AI����
			{
				BOOL bRet = LoadPropMoverEx_AI( szFileName, script, nVal );
				if ( bRet == FALSE )
				{
					Error( "%s : MoverID %s(%d)�� AI{} ���� �б� ����", szFileName, pProp->szName, nVal );
					return FALSE;
				}
			}

			if(script.Token == "m_nAttackFirstRange")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackFirstRange  = script.GetNumber();

				if( pProp->m_nAttackFirstRange > 10 || pProp->m_nAttackFirstRange <= 0 )
					Error( "LoadPropMoverEx::m_nAttackFirstRange �̻�:%d\n", pProp->m_nAttackFirstRange );
			}
			else
			if(script.Token == "SetEvasion")
			{
				script.GetToken(); // skip (
				pProp->m_nEvasionHP  = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nEvasionSec = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetRunAway")
			{
				script.GetToken(); // skip (
				pProp->m_nRunawayHP = script.GetNumber();
				script.GetToken(); // skip ,
				if( script.Token == "," )
				{
					script.GetNumber();
					script.GetToken(); // skip ,
					script.GetNumber();
					script.GetToken(); // skip )
				}
			}
			else
			if(script.Token == "SetCallHelper")
			{
				script.GetToken(); // skip (
				pProp->m_nCallHP = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperIdx[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperNum[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_bCallHelperParty[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip )
				pProp->m_nCallHelperMax++;
				if( pProp->m_nCallHelperMax > 5 )
				{
					char pszText[100];
					sprintf( pszText, "ID = %d: helper too many", pProp->dwID );
					AfxMessageBox( pszText );
				}
			}
			else
			if(script.Token == "m_nAttackItemNear")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemNear  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemFar")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemFar  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem1")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem1  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem2")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem2  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem3")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem3  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem4")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem4  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemSec")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemSec = script.GetNumber();
			}
			else
			if(script.Token == "m_nMagicReflection")
			{
				script.GetToken(); // skip =
				pProp->m_nMagicReflection = script.GetNumber();
			}
			else
			if(script.Token == "m_nImmortality")
			{
				script.GetToken(); // skip =
				pProp->m_nImmortality = script.GetNumber();
			}
			else
			if(script.Token == "m_bBlow")
			{
				script.GetToken(); // skip =
				pProp->m_bBlow = script.GetNumber();
			}
			else
			if(script.Token == "m_nChangeTargetRand")
			{
				script.GetToken(); // skip =
				pProp->m_nChangeTargetRand = script.GetNumber();
			}
			else
			if(script.Token == "m_dwAttackMoveDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwAttackMoveDelay = script.GetNumber();
			}
			else
			if(script.Token == "m_dwRunawayDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwRunawayDelay = script.GetNumber();
			}
			else
			if(script.Token == "randomItem")
				InterpretRandomItem( &pProp->m_randomItem, script );
			else 
			if( script.Token == "Maxitem" )
			{
				script.GetToken();	// =
				pProp->m_DropItemGenerator.m_dwMax	= script.GetNumber();
			}
			else 
			if( script.Token == "DropItem" )
			{
				DROPITEM di;
				memset( &di, 0, sizeof(DROPITEM) );
				di.dtType = DROPTYPE_NORMAL;
				script.GetToken();	// (
				di.dwIndex	= script.GetNumber();	// specific item index
				if( di.dwIndex == 0 )
					Error( "%s : %s�� defineItem.h�� ���ǵ��� �ʾ���", szFileName, script.token );
				ASSERT( di.dwIndex != 0 );
				script.GetToken();	// ,
				di.dwProbability	= script.GetNumber();	// probability
				script.GetToken();	// ,
				di.dwLevel	= script.GetNumber();	// level
				script.GetToken();	// ,
				di.dwNumber	= script.GetNumber();	// number
				script.GetToken();	// )
			#ifdef __WORLDSERVER
				pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
			#endif
				di.dwNumber2 = 0;
			}
			else 
			if( script.Token == "DropKind" )
			{
				DROPKIND dropKind;
				script.GetToken();	// (
				dropKind.dwIK3	= script.GetNumber();	//	dwIK3
				script.GetToken();	// ,
				int a = script.GetNumber();
				script.GetToken();	// ,
				int b = script.GetNumber();
				dropKind.nMinUniq = (short)( pProp->dwLevel - 5 );		// �ڱⷹ������ 2~5���� ���� ���� ����ǵ���.
				dropKind.nMaxUniq = (short)( pProp->dwLevel - 2 );
				if( dropKind.nMinUniq < 1 )		
					dropKind.nMinUniq = 1;
				if( dropKind.nMaxUniq < 1 )
					dropKind.nMaxUniq = 1;
				script.GetToken();	// )
			#ifdef __WORLDSERVER
				pProp->m_DropKindGenerator.AddTail( dropKind );	// copy
			#endif
			}
			else
			if( script.Token == "DropGold" )
			{
				DROPITEM di;
				memset( &di, 0, sizeof(DROPITEM) );	// clear
				di.dtType = DROPTYPE_SEED;
				di.dwProbability = 0xFFFFFFFF;	// ������ ������.
				script.GetToken();	// (
				di.dwNumber = script.GetNumber();	// gold min
				script.GetToken();	// ,
				di.dwNumber2 = script.GetNumber();	// gold max
				script.GetToken();	// )
			#ifdef __WORLDSERVER
				pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
			#endif
			}
			else if( script.Token == "Transform" )
			{
				script.GetToken(); // (
				pProp->m_MonsterTransform.fHPRate = script.GetFloat();
				script.GetToken(); // ,
				pProp->m_MonsterTransform.dwMonsterId = script.GetNumber();
				script.GetToken(); // )
			}
			script.GetToken();
		} 

		nVal = script.GetNumber(); // Mover Id
	} while(script.tok != FINISHED);

	return TRUE;
}

BOOL CProject::LoadCharacter( LPCTSTR szFileName )
{
		CScript script;
 
	if(script.Load(szFileName)==FALSE)
		return FALSE;

	CString strName;
	LPCHARACTER lpCharacter;
	script.GetToken(); // Mover name
	strName = script.Token;
	while( script.tok != FINISHED)
	{
		lpCharacter = new CHARACTER;
		lpCharacter->Clear();
		// �ʿ� �߰��Ǵ� ���� lowercase��. �׷���, ������ key ��Ʈ���� lowercase�� �ʿ� �߰��Ǹ�
		// ���� ��ü�� �ջ�Ǵ� ����� �� ����Ʈ�� �̾Ƴ� ���� ��� lowercase�� �ǹǷ� ������ m_szKey��
		// �����صδ� ���̴�.
		_tcscpy( lpCharacter->m_szKey, strName );
		strName.MakeLower();

		lpCharacter->m_vendor.m_type = CVendor::Type::Penya;

#ifdef __CHIPI_DYO
		lpCharacter->bOutput = TRUE;
#endif // __CHIPI_DYO

#ifdef _DEBUG
		if( m_mapCharacter.Lookup( strName, (void*&)lpCharacter ) )
		{
			Error( "%s���� ������ %s �߰�", szFileName, strName );
		}
#endif
		m_mapCharacter.SetAt( strName, lpCharacter);
		script.GetToken(); // {

		int nBlock = 1;
		while( nBlock && script.tok != FINISHED )
		{ 
			script.GetToken();
			if( script.Token == "{" )			
				nBlock++;
			else
			if( script.Token == "}" )			
				nBlock--;
			else
			if(script.Token == "randomItem")
			{
				InterpretRandomItem(&lpCharacter->m_randomItem,script);
			}
			else
			if(script.Token == "SetEquip")
			{
				lpCharacter->m_nEquipNum = 0;
				ZeroMemory( lpCharacter->m_adwEquip, sizeof( lpCharacter->m_adwEquip ) );
				script.GetToken(); // (
				while( *script.token != ')' )
				{
					DWORD dwEquip = script.GetNumber();
					if( lpCharacter->m_nEquipNum < MAX_HUMAN_PARTS )
						lpCharacter->m_adwEquip[ lpCharacter->m_nEquipNum++ ] = dwEquip;
					script.GetToken(); // ,
				}
			}
			else
			if(script.Token == "m_szName")
			{
				script.GetToken(); // =
				script.GetToken(); // name
				lpCharacter->m_strName = script.Token; 
			}
			else
			if(script.Token == "SetName")
			{ 
				script.GetToken(); // (
				lpCharacter->m_strName = GetLangScript( script );
				if( strlen( lpCharacter->m_strName ) == 0 )
				{
					Error( _T( "Error SetName m_szName Not Data") );
				}				
			}
			else
			if( script.Token == "SetFigure" )
			{
				script.GetToken(); // (
				lpCharacter->m_dwMoverIdx = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHairMesh = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHairColor = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHeadMesh = script.GetNumber();
			}

			else
			if( script.Token == "SetMusic" )
			{
				script.GetToken(); // (
				lpCharacter->m_dwMusicId = script.GetNumber();
			}			
			else
			if(script.Token == "m_nStructure")
			{
				script.GetToken();
				lpCharacter->m_nStructure = script.GetNumber();
			}
			else
			if(script.Token == "m_szChar") // "SetImage"�� ���� ���� ���� ���� 
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				strcpy( lpCharacter->m_szChar, filename.GetBuffer(0) );
			}
			else
			if(script.Token == "m_szDialog")
			{
				script.GetToken();
				script.GetToken();
				strcpy( lpCharacter->m_szDialog, script.Token );
			}
			else
			if(script.Token == "m_szDlgQuest")
			{
				script.GetToken();
				script.GetToken();
				strcpy( lpCharacter->m_szDlgQuest, script.Token );
			}
			else
			if(script.Token == "SetImage" )
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				strcpy( lpCharacter->m_szChar, filename.GetBuffer(0) );
			}
			else if( script.Token == "AddMenuLang" )
			{
				script.GetToken(); // (
				int nLang	= script.GetNumber();
				script.GetToken(); // ,
				int nSubLang	= script.GetNumber();
				script.GetToken(); // ,
				int nMMI = script.GetNumber(); 
				script.GetToken(); // ) 
				if( ::GetLanguage() == nLang && ::GetSubLanguage() == nSubLang )
					lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
			}
			else
			if(script.Token == "AddMenu" )
			{
				script.GetToken(); // (
				int nMMI = script.GetNumber(); 
				script.GetToken(); // ) 

				if( ( GetLanguage() != LANG_TWN && GetLanguage() != LANG_HK ) || ( nMMI != MMI_BEAUTYSHOP && nMMI != MMI_BEAUTYSHOP_SKIN ) )
					lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
			}
			else
			if(script.Token == "AddVenderSlot" ) // "AddVendorSlot"�� ���� ���� ���� 
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); script.GetToken(); // 
				script.GetToken();
				lpCharacter->m_vendor.m_venderSlot[ nSlot ] = script.token;
				script.GetToken(); // 
			}
			else
			if(script.Token == "AddVendorSlot" )
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); // slot
				script.GetToken(); // ,
				lpCharacter->m_vendor.m_venderSlot[ nSlot ] = GetLangScript( script );
			}
#ifdef __RULE_0615
			// ������ �Ǹ� ������ ���
			else if( script.Token == "AddVendorSlotLang" )
			{
				script.GetToken(); // (
				int nLang	= script.GetNumber();	// lang
				script.GetToken();	//,
				int nSubLang	= script.GetNumber();	// sub
				script.GetToken();	//,
				int nSlot = script.GetNumber(); // slot
				script.GetToken(); // ,
				CString strSlot	= GetLangScript( script );
				if( nLang == ::GetLanguage() && nSubLang == ::GetSubLanguage() )
					lpCharacter->m_vendor.m_venderSlot[ nSlot ] = strSlot;
			}
			else if( script.Token == "AddVendorItemLang" )
			{
				script.GetToken();	//(
				int nLang	= script.GetNumber();
				script.GetToken();	//,
				int nSubLang	= script.GetNumber();
				script.GetToken();	//,
				int nSlot	= script.GetNumber(); script.GetToken(); // 
				int nItemKind3	= script.GetNumber(); script.GetToken(); // 
				int	nItemJob	= script.GetNumber(); script.GetToken();	//
				int nUniqueMin	= script.GetNumber(); script.GetToken(); // 
				int nUniqueMax	= script.GetNumber(); script.GetToken(); // 
				int nTotalNum	= script.GetNumber(); script.GetToken(); // 
				if( nLang == ::GetLanguage() && nSubLang == ::GetSubLanguage() )
				{
					lpCharacter->m_vendor.m_venderItemAry[ nSlot ].emplace_back(
						nItemKind3, nItemJob, nUniqueMin, nUniqueMax, nTotalNum
					);
				}
			}
#endif	// __RULE_0615
			else
			if( script.Token == "AddVenderItem" || script.Token == "AddVendorItem")
			{
				script.GetToken(); // (
				int nSlot      = script.GetNumber(); script.GetToken(); // 
				int nItemKind3  = script.GetNumber(); script.GetToken(); // 
				int	nItemJob	= script.GetNumber(); script.GetToken();	//
				int nUniqueMin = script.GetNumber(); script.GetToken(); // 
				int nUniqueMax = script.GetNumber(); script.GetToken(); // 
				int nTotalNum  = script.GetNumber(); script.GetToken(); // 

				lpCharacter->m_vendor.m_venderItemAry[ nSlot ].emplace_back(
					nItemKind3, nItemJob, nUniqueMin, nUniqueMax, nTotalNum
				);
			}
			else if( script.Token == "AddVenderItem2" || script.Token == "AddVendorItem2")
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); script.GetToken(); // 
				DWORD dwId = script.GetNumber(); script.GetToken(); //
				lpCharacter->m_vendor.m_venderItemAry2[ nSlot ].emplace_back(dwId);
			}
			else if( script.Token == "SetVenderType" )
			{
				script.GetToken();
				int nVenderType = script.GetNumber();

				if (nVenderType == 0) {
					lpCharacter->m_vendor.m_type = CVendor::Type::Penya;
				} else if (nVenderType == 1) {
					lpCharacter->m_vendor.m_type = CVendor::Type::RedChip;
				} else {
					Error(__FUNCTION__ "(): SetVenderType(%d) = Invalid vendor type", nVenderType);
					lpCharacter->m_vendor.m_type = static_cast<CVendor::Type>(nVenderType);
				}
				
				script.GetToken();
			}
#ifdef __NPC_BUFF
			else if( script.Token == "SetBuffSkill" )
			{
				NPC_BUFF_SKILL NPCBuffSkill;
				script.GetToken(); // (
				NPCBuffSkill.dwSkillID = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.dwSkillLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.nMinPlayerLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.nMaxPlayerLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.dwSkillTime = script.GetNumber();
				script.GetToken(); // )
				lpCharacter->m_vecNPCBuffSkill.push_back( NPCBuffSkill );
			}
#endif // __NPC_BUFF
#ifdef __CHIPI_DYO
			else if( script.Token == "SetLang" )
			{
				script.GetToken(); // (
				DWORD wLang = script.GetNumber();
				DWORD wSubLang = LANG_SUB_DEFAULT;
				script.GetToken(); // , or )
				if( script.Token == "," )
					wSubLang = script.GetNumber();
				else
					script.GetToken(); // )

				lpCharacter->m_vecdwLanguage.push_back( MAKELONG( wSubLang, wLang ) );
			}
			else if( script.Token == "SetOutput" )
			{
				script.GetToken(); // (
				script.GetToken();
				script.Token.MakeUpper();
				if( script.Token == "FALSE" )
					lpCharacter->bOutput = FALSE;
				script.GetToken(); // )
			}
#endif // __CHIPI_DYO
			else if( script.Token == "AddTeleport" )
			{
				script.GetToken(); // (
				D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
				vPos.x = script.GetFloat();
				script.GetToken(); // ,
				vPos.z = script.GetFloat();
				script.GetToken(); // )
				lpCharacter->m_vecTeleportPos.push_back( vPos );
			}
		}
		script.GetToken(); // Mover name
		strName = script.Token;
	} 
	return TRUE;
}	

void CProject::InterpretRandomItem(LPRANDOM_ITEM pRandomItem,CScript& script)
{
	script.GetToken(); // {
	script.GetToken(); 
	while(*script.token != '}')
	{
		if(script.Token == ";") 
		{ 
			script.GetToken(); 
			continue; 
		} 
		script.GetToken();
	}
}


BOOL CProject::LoadExpTable( LPCTSTR lpszFileName )
{	
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
		return FALSE;

	DWORD dwVal;
	int i = 0;

	script.GetToken(); // subject or FINISHED
	while( script.tok != FINISHED )
	{
		i = 0;
		if( script.Token == _T( "expCharacter" ) )
		{
			EXPINTEGER nVal;
			ZeroMemory( m_aExpCharacter, sizeof( m_aExpCharacter ) );
			script.GetToken(); // { 
			nVal	= script.GetExpInteger();
			while( *script.token != '}' )
			{
				m_aExpCharacter[i].nExp1	= nVal;
				m_aExpCharacter[i].nExp2	= script.GetExpInteger();
				m_aExpCharacter[i].dwLPPoint	= script.GetNumber();
				m_aExpCharacter[i++].nLimitExp		= script.GetExpInteger();
				nVal	= script.GetExpInteger();
			}
		}
		else
		if( script.Token == _T( "expFlight" ) )
		{
			ZeroMemory( m_aFxpCharacter, sizeof( m_aFxpCharacter ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aFxpCharacter[ i ].dwFxp = dwVal;
				m_aFxpCharacter[ i++ ].dwFxp2 = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expLPPoint" ) )
		{
			ZeroMemory( m_aExpLPPoint, sizeof( m_aExpLPPoint ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpLPPoint[ i++ ] = dwVal;
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expJobLevel" ) )
		{
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetNumber();		// skip
				script.GetNumber();		// skip
			}
		}
		else
		if( script.Token == _T( "expSkill" ) )
		{
			ZeroMemory( m_aExpSkill, sizeof( m_aExpSkill ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpSkill[ i++ ] = dwVal;
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "attackStyle" ) )
		{
//			ZeroMemory( m_aAttackStyle, sizeof( m_aAttackStyle ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetNumber();
				script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expParty" ) )
		{
			ZeroMemory( m_aExpParty, sizeof( m_aExpParty ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpParty[ i ].Exp = dwVal;
				m_aExpParty[ i++ ].Point = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expPartySkill" ) )
		{
			ZeroMemory( m_aAddExpParty, sizeof( m_aAddExpParty ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aAddExpParty[ i ].Exp = dwVal;
				m_aAddExpParty[ i++ ].Level = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "expCompanyTest" ) )
		{
			CGuildTable::GetInstance().ReadBlock( script );
		}
		else if( script.Token == _T( "expDropLuck" ) )
		{
			memset( m_adwExpDropLuck, 0, sizeof(m_adwExpDropLuck) );
			script.GetToken();	// {
			DWORD* pdw	= &m_adwExpDropLuck[0][0];
			DWORD dwValue	= (DWORD)script.GetNumber();	//
			while( *script.token != '}' )
			{
				ASSERT( pdw <= &m_adwExpDropLuck[121][10] );
				*pdw	= dwValue;
				pdw++;
				dwValue	= script.GetNumber();
			}
		}
		else if( script.Token == _T( "Setitem" ) )
		{
			script.GetToken();	// {
			int	 nAbilityOption		= 0;
			int nVal	= script.GetNumber();
			while( *script.token != '}' )
			{
				m_aSetItemAvail[nAbilityOption].nHitRate	= nVal;
				m_aSetItemAvail[nAbilityOption].nBlock	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nMaxHitPointRate	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nAddMagic	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nAdded	= script.GetNumber();
				nVal	= script.GetNumber();
				nAbilityOption++;
			}
		}
		else
		if( script.Token == _T( "expUpitem" ) )
		{
			ZeroMemory( m_aExpUpItem, sizeof( m_aExpUpItem ) );
			script.GetToken();		// {
			int i = 0, j = 0, nVal;
			while( 1 )
			{
				nVal = script.GetNumber();
				if( *script.token == '}' )	break;
				j = 0;
				m_aExpUpItem[ j++ ][ i ] = nVal;
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				
				i++;
			}

			if( j != 6 )
				Error( "CProject::LoadExpTable : expUpitem ������ ����Ÿ �������� ���� �ʴ�. %d", j );
			if( i != 11 )
				Error( "CProject::LoadExpTable : expUpitem ������ ����Ÿ �ళ���� ���� �ʴ�. %d", j );
			

		} // expUpitem
		else if( script.Token == _T( "RestExpFactorTable" ) )
		{
			int nMinLevel, nMaxLevel;
			float fFactor;
			script.GetToken(); // {
			while( 1 )
			{
				nMinLevel	= script.GetNumber();
				if( *script.token == '}' ) break;
				nMaxLevel	= script.GetNumber();
				fFactor		= script.GetFloat();
#ifdef __WORLDSERVER
				GuildHouseMng->AddRestExpFactorTable( nMinLevel, nMaxLevel, fFactor );
#endif // __WORLDSERVER
			}
		}

		script.GetToken(); // type name or }
	}
	return TRUE;
}

CCtrl* CProject::GetCtrl( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return pCtrl;
	return NULL;
}

#ifdef __CLIENT
CSfx* CProject::GetSfx( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_SFX ? (CSfx*)pCtrl : NULL );
	return NULL;
}
#endif

CItem* CProject::GetItem( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_ITEM ? (CItem*)pCtrl : NULL );
	return NULL;
}

CMover* CProject::GetMover( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_MOVER ? (CMover*)pCtrl : NULL );
	return NULL;
}

CShip* CProject::GetShip( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_SHIP ? (CShip*)pCtrl : NULL );
	return NULL;
}


#ifdef __WORLDSERVER

CUser* CProject::GetUser( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( ( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() ) ? (CUser*)pCtrl : NULL );
	return NULL;
}
#endif	

ObjProp* CProject::GetProp( int nType, int nIndex )
{
	switch( nType )
	{
	case OT_CTRL: return m_aPropCtrl.GetAt( nIndex ); 
	case OT_ITEM: return m_aPropItem.GetAt( nIndex ); 
	case OT_MOVER: return GetMoverProp( nIndex ); 
	}
	return NULL;
}

// ������Ƽ �޸𸮸� �б��������� �����Ѵ�.
void CProject::ProtectPropMover()
{
// �鿣�� �ý����� ���ؼ� ���Ƶд�.
//	DWORD dwOld;
//	::VirtualProtect( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, PAGE_READONLY, &dwOld );
}

#ifdef __S1108_BACK_END_SYSTEM
void CProject::AddMonsterProp( MONSTER_PROP MonsterProp )
{
	strcpy( m_aMonsterProp[ m_nMonsterPropSize ].szMonsterName, MonsterProp.szMonsterName );
	m_aMonsterProp[ m_nMonsterPropSize ].nHitPoint = MonsterProp.nHitPoint;
	m_aMonsterProp[ m_nMonsterPropSize ].nAttackPower = MonsterProp.nAttackPower;
	m_aMonsterProp[ m_nMonsterPropSize ].nDefence = MonsterProp.nDefence;
	m_aMonsterProp[ m_nMonsterPropSize ].nExp = MonsterProp.nExp;
	m_aMonsterProp[ m_nMonsterPropSize ].nItemDrop = MonsterProp.nItemDrop;
	m_aMonsterProp[ m_nMonsterPropSize ].nPenya = MonsterProp.nPenya;
	m_aAddProp[ m_nAddMonsterPropSize ] = m_aMonsterProp[ m_nMonsterPropSize ];
	++m_nMonsterPropSize;
	++m_nAddMonsterPropSize;
}

void CProject::RemoveMonsterProp( char* lpszMonsterName )
{
	BOOL bFind = FALSE;
	int nFindIndex = 0;
	for( int i = 0 ; i < m_nMonsterPropSize ; ++i )
	{
		if( strcmp( m_aMonsterProp[ i ].szMonsterName, lpszMonsterName ) == 0 )
		{
			nFindIndex = i;
			bFind = TRUE;
			break;
		}
	}
	
	if( bFind )
	{
		for( int i = 0 ; i < m_nMonsterPropSize - 1; ++ i )
		{
			m_aMonsterProp[i] = m_aMonsterProp[i + 1];					
		}
		strcpy( m_aRemoveProp[m_nRemoveMonsterPropSize], lpszMonsterName );
		++m_nRemoveMonsterPropSize;
		--m_nMonsterPropSize;
	}			
}
#endif // __S1108_BACK_END_SYSTEM

#ifdef __WORLDSERVER
BOOL CProject::LoadDropEvent( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	
	do {
		s.GetToken();
		if( s.Token == "DropItem" )
		{
			DROPITEM di;
			DWORD dwMinLevel, dwMaxLevel;
			memset( &di, 0, sizeof(DROPITEM) );
			di.dtType = DROPTYPE_NORMAL;
			s.GetToken();	// (
			di.dwIndex	= s.GetNumber();	// specific item index
			s.GetToken();	// ,
			di.dwProbability	= s.GetNumber();	// probability
			s.GetToken();	// ,
			di.dwLevel	= s.GetNumber();	// level
			s.GetToken();	// ,
			di.dwNumber	= s.GetNumber();	// number
			s.GetToken();	// ,
			dwMinLevel	= s.GetNumber();	// min
			s.GetToken();	// ,
			dwMaxLevel	= s.GetNumber();	// max
			s.GetToken();	// )

			const auto i2	= m_setExcept.find( di.dwIndex );
			if( i2 != m_setExcept.end() )
				continue;
			if( GetLanguage() != LANG_KOR )
			{
				if( di.dwIndex == II_GEN_SKILL_BUFFBREAKER )
					di.dwProbability	= (DWORD)( di.dwProbability	* 0.5 );
			}

			for( int i = 0; i < m_nMoverPropSize; i++ )
			{
				MoverProp* pProp	= m_pPropMover + i;
				if( pProp->dwID && pProp->dwLevel >= dwMinLevel && pProp->dwLevel <= dwMaxLevel	)
					pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
			}
		}
	} while( s.tok != FINISHED );

	return TRUE;
}

BOOL CProject::SortDropItem( void )
{
	// DROPITEM�� dwProbability ������������ Sortting 
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		MoverProp* pProp	= m_pPropMover + i;
		int nDropItemCount = pProp->m_DropItemGenerator.GetSize();

		DROPITEM *pDropItem0, *pDropItem1, *pDropItemTemp;		
		for( int i = 0 ; i < nDropItemCount - 1 ; ++i )
		{
			pDropItem0 = pProp->m_DropItemGenerator.GetAt( i, FALSE, 0.0f );
			for( int j = i + 1 ; j < nDropItemCount ; ++j )
			{
				pDropItem1 = pProp->m_DropItemGenerator.GetAt( j, FALSE, 0.0f );
				if( pDropItem0->dwProbability > pDropItem1->dwProbability )
				{
					pDropItemTemp = pDropItem0;
					pDropItem0 = pDropItem1;
					pDropItem1 = pDropItemTemp;
				}
			}
		}
	}
	return TRUE;
}
#endif	// __WORLDSERVER


#ifdef __WORLDSERVER
CGiftboxMan::CGiftboxMan()
{
#ifndef __STL_GIFTBOX_VECTOR
	m_nSize	= 0;
	memset( &m_giftbox, 0, sizeof(m_giftbox) );
#endif // __STL_GIFTBOX_VECTOR
	/*
#ifdef __GIFTBOX0213
	m_nQuery	= 0;
#endif	// __GIFTBOX0213
	*/
}

CGiftboxMan* CGiftboxMan::GetInstance( void )
{
	static CGiftboxMan sGiftboxMan;
	return &sGiftboxMan;
}

/*
#ifdef __GIFTBOX0213
BOOL CGiftboxMan::AddItem( DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag, int nTotal )
#else	// __GIFTBOX0213
*/
BOOL CGiftboxMan::AddItem( DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag, int nSpan, int nAbilityOption )
//#endif	// __GIFTBOX0213
{
	const auto i	= m_mapIdx.find( dwGiftbox );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_vGiftBox.size();
		GIFTBOX giftBox;
		memset( &giftBox, 0, sizeof(GIFTBOX) );
		m_vGiftBox.push_back( giftBox );

		m_mapIdx.emplace( dwGiftbox, nIdx1 );
	}

#ifdef __STL_GIFTBOX_VECTOR
	m_vGiftBox[nIdx1].dwGiftbox	= dwGiftbox;
	int nIdx2	= m_vGiftBox[nIdx1].nSize++;
	m_vGiftBox[nIdx1].adwItem[nIdx2]	= dwItem;
	m_vGiftBox[nIdx1].anNum[nIdx2]	= nNum;
	m_vGiftBox[nIdx1].anFlag[nIdx2]	= nFlag;
	m_vGiftBox[nIdx1].anSpan[nIdx2]	= nSpan;
	m_vGiftBox[nIdx1].anAbilityOption[nIdx2]		= nAbilityOption;
	
	m_vGiftBox[nIdx1].nSum	+= dwProbability;
	m_vGiftBox[nIdx1].adwProbability[nIdx2]	= m_vGiftBox[nIdx1].nSum;
#else // __STL_GIFTBOX_VECTOR
	if( m_nSize >= MAX_GIFTBOX )
	{
		OutputDebugString( "TOO MANY GIFTBOX\n" );
		return FALSE;
	}

	m_giftbox[nIdx1].dwGiftbox	= dwGiftbox;
	int nIdx2	= m_giftbox[nIdx1].nSize++;
	m_giftbox[nIdx1].adwItem[nIdx2]	= dwItem;
	m_giftbox[nIdx1].anNum[nIdx2]	= nNum;
	m_giftbox[nIdx1].anFlag[nIdx2]	= nFlag;
	m_giftbox[nIdx1].anSpan[nIdx2]	= nSpan;
	m_giftbox[nIdx1].anAbilityOption[nIdx2]		= nAbilityOption;
	/*
#ifdef __GIFTBOX0213
	m_giftbox[nIdx1].anTotal[nIdx2]	= nTotal;
	if( nTotal > 0 )
		m_giftbox[nIdx1].bGlobal	= TRUE;
#endif	// __GIFTBOX0213
		*/
	m_giftbox[nIdx1].nSum	+= dwProbability;
	m_giftbox[nIdx1].adwProbability[nIdx2]	= m_giftbox[nIdx1].nSum;
#endif // __STL_GIFTBOX_VECTOR
	return TRUE;
}

/*
#ifdef __GIFTBOX0213
BOOL CGiftboxMan::Open( DWORD dwGiftbox, LPDWORD pdwItem, int* pnNum, u_long idPlayer, CItemElem* pItemElem )
#else	// __GIFTBOX0213
*/
BOOL CGiftboxMan::Open( DWORD dwGiftbox, PGIFTBOXRESULT pGiftboxResult )
//#endif	// __GIFTBOX0213
{
	DWORD dwRand	= xRandom( 1000000 );
	const auto i		= m_mapIdx.find( dwGiftbox );
	if( i == m_mapIdx.end() )
		return 0;
	int nIdx	= i->second;
#ifdef __STL_GIFTBOX_VECTOR
	PGIFTBOX pBox	= &m_vGiftBox[nIdx];
#else // __STL_GIFTBOX_VECTOR
	PGIFTBOX pBox	= &m_giftbox[nIdx];
#endif // __STL_GIFTBOX_VECTOR

	int low	= 0;
	for( int j = 0; j < pBox->nSize; j++ )
	{
		if( dwRand >= (DWORD)( low ) && dwRand < pBox->adwProbability[j] )
		{
			/*
#ifdef __GIFTBOX0213
			if( pBox->anTotal[j] > 0 )
			{
				Query( CDPAccountClient::GetInstance(), dwGiftbox, pBox->adwItem[j], pBox->anNum[j], idPlayer, pItemElem );
				return FALSE;
			}
#endif	// __GIFTBOX0213
			*/
//			*pdwItem	= pBox->adwItem[j];
//			*pnNum	= pBox->anNum[j];
//			*pnFlag	= pBox->anFlag[j];
//			*pnSpan	= pBox->anSpan[j];
			pGiftboxResult->dwItem	= pBox->adwItem[j];
			pGiftboxResult->nNum	= pBox->anNum[j];
			pGiftboxResult->nFlag	= pBox->anFlag[j];
			pGiftboxResult->nSpan	= pBox->anSpan[j];
			pGiftboxResult->nAbilityOption	= pBox->anAbilityOption[j];
			return TRUE;
		}
	}
	return FALSE;
}

/*
#ifdef __GIFTBOX0213
BOOL CGiftboxMan::OpenLowest( DWORD dwGiftbox, LPDWORD pdwItem, int* pnNum )
{
	map<DWORD, int>::iterator i		= m_mapIdx.find( dwGiftbox );
	if( i == m_mapIdx.end() )
		return FALSE;
	int nIdx	= i->second;
	PGIFTBOX pBox	= &m_giftbox[nIdx];

	int nLowest	= pBox->nSize - 1;
	if( nLowest < 0 )
		return FALSE;
	*pdwItem	= pBox->adwItem[nLowest];
	*pnNum	= pBox->anNum[nLowest];
	return TRUE;
}
#endif	// __GIFTBOX0213
*/

void CGiftboxMan::Verify( void )
{
#ifdef __STL_GIFTBOX_VECTOR
	for( DWORD i = 0; i < m_vGiftBox.size(); i++ )
	{
		TRACE( "GIFTBOX : %d, %d\n", m_vGiftBox[i].dwGiftbox, m_vGiftBox[i].nSum );
		m_vGiftBox[i].adwProbability[m_vGiftBox[i].nSize-1]	+= ( 1000000 - m_vGiftBox[i].nSum );
	}	
#else // __STL_GIFTBOX_VECTOR
	for( int i = 0; i < m_nSize; i++ )
	{
		TRACE( "GIFTBOX : %d, %d\n", m_giftbox[i].dwGiftbox, m_giftbox[i].nSum );
		m_giftbox[i].adwProbability[m_giftbox[i].nSize-1]	+= ( 1000000 - m_giftbox[i].nSum );
	}
#endif // __STL_GIFTBOX_VECTOR
}

BOOL CProject::LoadGiftbox( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwGiftbox, dwItem, dwProbability;
	int	nNum;
	BYTE	nFlag;
	int	nSpan;
	int	nAbilityOption;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "GiftBox" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 100, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox2" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox3" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 100, nNum, nFlag ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox4" ) || s.Token == _T( "GiftBox5" ) )
		{
			DWORD dwPrecision	= ( s.Token == _T( "GiftBox4" )? 100: 10 );
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= s.GetNumber();
				nSpan	= s.GetNumber();	// �Ⱓ
				
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * dwPrecision, nNum, nFlag, nSpan ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox6" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= s.GetNumber();
				nSpan	= s.GetNumber();	// �Ⱓ
				nAbilityOption	= s.GetNumber();	// +
				
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 10, nNum, nFlag, nSpan, nAbilityOption ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		/*
#ifdef __GIFTBOX0213
		else if( s.Token == _T( "GlobalGiftbox" ) )
		{
			int nTotal;
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nTotal	= s.GetNumber();

				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability, nNum, 0, nTotal ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
#endif	// __GIFTBOX0213
		*/
		s.GetToken();
	}
	CGiftboxMan::GetInstance()->Verify();
	return TRUE;
}

/*
#ifdef __GIFTBOX0213
void CGiftboxMan::Upload( CDPMng* pdp )
{
	CAr ar;
	ar << PACKETTYPE_GLOBALGIFTBOX;
	u_long uOffset	= ar.GetOffset();
	int nSize	= 0;
	ar << nSize;
	for( int i = 0; i < m_nSize; i++ )
	{
		if( m_giftbox[i].bGlobal )
		{
			ar << m_giftbox[i].dwGiftbox;
			ar << m_giftbox[i].nSize;
			for( int j = 0; j < m_giftbox[i].nSize; j++ )
			{
				ar << m_giftbox[i].adwItem[j];
				ar << m_giftbox[i].anTotal[j];
			}
			nSize++;
		}
	}
	int nBufSize;
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	*(UNALIGNED int*)( lpBuf + uOffset )	= nSize;
	pdp->Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CGiftboxMan::Query( CDPMng* pdp, DWORD dwGiftbox, DWORD dwItem, int nNum, u_long idPlayer, CItemElem* pItemElem )
{
	if( pItemElem->m_nQueryGiftbox > 0 )
		return;

	CAr ar;
	ar << PACKETTYPE_QUERYGLOBALGIFTBOX;
	pItemElem->m_nQueryGiftbox	= ++m_nQuery;
	ar << idPlayer << dwGiftbox << dwItem << nNum << pItemElem->m_dwObjId << pItemElem->m_nQueryGiftbox;
	int nBufSize;
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	pdp->Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CGiftboxMan::Restore( CDPMng* pdp, DWORD dwGiftbox, DWORD dwItem )
{
	CAr ar;
	ar << PACKETTYPE_RESTOREGLOBALGIFTBOX;
	ar << dwGiftbox << dwItem;
	int nBufSize;
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	pdp->Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}
#endif	// __GIFTBOX0213
*/
#endif	// __WORLDSERVER

CPackItem g_PackItem;

void CPackItem::AddItem(DWORD dwPackItem, DWORD dwItem, int nAbilityOption, int nNum) {
	const auto i = m_mapIdx.find(dwPackItem);
	size_t nIdx1;
	if (i != m_mapIdx.end()) {
		nIdx1 = i->second;
	} else {
		nIdx1 = m_packitems.size();
		m_mapIdx.emplace(dwPackItem, nIdx1);
		m_packitems.emplace_back(dwPackItem);
	}

	m_packitems[nIdx1].aItems.emplace_back(
		CPackItem::PackedItem{ dwItem, nAbilityOption, nNum }
	);
}

const CPackItem::PACKITEMELEM * CPackItem::Open(const DWORD dwPackItem) const {
	const auto i = m_mapIdx.find( dwPackItem );
	if (i == m_mapIdx.end()) return nullptr;
	return &m_packitems[i->second];
}

CPackItem::PACKITEMELEM * CPackItem::Open_(const DWORD dwPackItem) {
	const auto i = m_mapIdx.find(dwPackItem);
	if (i == m_mapIdx.end()) return nullptr;
	return &m_packitems[i->second];
}

void CPackItem::Load(LPCTSTR lpszFileName) noexcept(false) {
	CScript s;
	if (!s.Load(lpszFileName)) {
		throw ProjectLoadError(__FUNCTION__"(): file not found", lpszFileName);
	}

	char lpOutputString[100]	= { 0, };
	OutputDebugString( "packItem\n" );
	OutputDebugString( "----------------------------------------\n" );
	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "PackItem" ) )
		{
			const DWORD dwPackItem	= s.GetNumber();
			const int nSpan	= s.GetNumber();
			s.GetToken();	// {
			DWORD dwItem	= s.GetNumber();
			while (*s.token != '}') {
				const int nAbilityOption = s.GetNumber();
				const int nNum = s.GetNumber();

				std::sprintf(lpOutputString, "%d\n", dwPackItem);
				OutputDebugString(lpOutputString);

				AddItem(dwPackItem, dwItem, nAbilityOption, nNum);
				dwItem = s.GetNumber();
			}
			
			if (PACKITEMELEM * const pPackItemElem = Open_(dwPackItem)) {
				pPackItemElem->nSpan = nSpan;
			}
		}
		s.GetToken();
	}
	OutputDebugString( "----------------------------------------\n" );
}

PSETITEMAVAIL CProject::GetSetItemAvail( int nAbilityOption )
{
	if( nAbilityOption < 0 || nAbilityOption > 10 )
		return NULL;
	return &m_aSetItemAvail[nAbilityOption-1];
}

BOOL CProject::LoadPiercingAvail( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwItemId;
	int nDstParam, nAdjParam;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Piercing" ) )
		{
			dwItemId	= s.GetNumber();
			s.GetToken();	// {
			nDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				nAdjParam	= s.GetNumber();
				if (!g_PiercingAvail.AddPiercingAvail( dwItemId, nDstParam, nAdjParam ) )
					return FALSE;
				nDstParam	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "SetItem" ) )
		{
			const int nId = s.GetNumber();

			s.GetToken();

			std::unique_ptr<CSetItem> pSetItem = std::make_unique<CSetItem>(nId, s.Token.GetString());

			s.GetToken();	// {
			s.GetToken();	// categori
			while( *s.token != '}' )
			{
				if( s.Token == _T( "Elem" ) )
				{
					s.GetToken();	// {
					DWORD dwItemId	= s.GetNumber();
					int nParts;
					while( *s.token != '}' )
					{
						nParts	= s.GetNumber();
						pSetItem->AddSetItemElem( dwItemId, nParts );
						dwItemId	= s.GetNumber();
					}
					s.GetToken();
				}
				else if( s.Token == _T( "Avail" ) )
				{
					s.GetToken();	// {
					int nDstParam	= s.GetNumber();
					while( *s.token != '}' )
					{
						int nAdjParam	= s.GetNumber();
						int nEquiped	= s.GetNumber();
						pSetItem->AddItemAvail( nDstParam, nAdjParam, nEquiped );
						nDstParam	= s.GetNumber();
					}
					s.GetToken();
				}
			}
			pSetItem->SortItemAvail();
			g_SetItemFinder.AddSetItem(std::move(pSetItem));
		}

		else if( s.Token == _T( "RandomOptItem" ) )
		{
			CRandomOptItemGen::RandomOptItem randomOptItem;
			randomOptItem.nId	= s.GetNumber();
			s.GetToken();
			randomOptItem.pszString = s.Token;
			randomOptItem.nLevel	= s.GetNumber();
			randomOptItem.dwProbability		= s.GetNumber();
			s.GetToken();	// {
			int nDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				int nAdjParam	= s.GetNumber();
				randomOptItem.ia.push_back(SINGLE_DST{ nDstParam, nAdjParam });
				nDstParam	= s.GetNumber();
			}
			g_RandomOptItemGen.AddRandomOption(randomOptItem);
		}
		s.GetToken();
	}

	g_RandomOptItemGen.Arrange();

	return TRUE;
}

CPiercingAvail g_PiercingAvail;

bool CPiercingAvail::AddPiercingAvail(const DWORD dwItemId, const int nDstParam, const int nAdjParam) {
	// Is it already there?
	auto it = m_itemIdToPosition.find(dwItemId);

	size_t pos;
	if (it != m_itemIdToPosition.end()) {
		pos = it->second;
	} else {
		// No, add the item
		if (m_pPiercingAvail.size() == m_pPiercingAvail.max_size()) {
			return false;
		}

		// Insert and setup iterator
		pos = m_pPiercingAvail.size();
		m_pPiercingAvail.emplace_back();
		m_pPiercingAvail.back().dwItemId = dwItemId;
		m_itemIdToPosition.insert_or_assign(dwItemId, pos);
	}
	
	// Add the bonus
	PIERCINGAVAIL & piercingAvail = m_pPiercingAvail[pos];

	if (piercingAvail.params.size() >= PIERCINGAVAIL::MAX_PIERCING_DSTPARAM) {
		return false;
	}

	piercingAvail.params.push_back(SINGLE_DST{ nDstParam, nAdjParam });
	return true;
}

const PIERCINGAVAIL * CPiercingAvail::GetPiercingAvail(DWORD dwItemId) const {
	auto i = m_itemIdToPosition.find(dwItemId);
	if (i == m_itemIdToPosition.end()) return nullptr;
	return &m_pPiercingAvail[i->second];
}

bool CSetItem::AddSetItemElem(const DWORD dwItemId, const int nParts) {
	if (m_components.size() == m_components.static_capacity) {
		Error(__FUNCTION__"(): The set #%lu %s has too much components", m_components.size(), GetString());
		return false;
	}

	const auto it = std::find_if(
		m_components.begin(), m_components.end(),
		[nParts](const PartItem & partItem) {
			return partItem.part == nParts;
		}
	);

	if (it != m_components.end()) {
		Error(__FUNCTION__"(): The set #%lu %s already has a component for part %d",
			m_components.size(), GetString(), nParts
		);
		return false;
	}

	m_components.emplace_back(PartItem{ nParts, dwItemId });

	return true;
}

bool CSetItem::AddItemAvail(const int nDstParam, const int nAdjParam, const int nEquiped) {
	if (m_avail.size() == m_avail.static_capacity) {
		Error(__FUNCTION__"(): The set #%lu %s already has more than %lu bonuses",
			m_components.size(), GetString(), m_avail.static_capacity
		);
	}

	m_avail.push_back(EquipedDst{ nDstParam, nAdjParam, nEquiped });
	return TRUE;
}

void CSetItem::SortItemAvail() {
	std::sort(
		m_avail.begin(), m_avail.end(),
		[](const EquipedDst & lhs, const EquipedDst & rhs) {
			if (lhs.anEquiped < rhs.anEquiped) return true;
			if (lhs.anEquiped > rhs.anEquiped) return false;

			if (lhs.nDst < rhs.nDst) return true;
			if (lhs.nDst > rhs.nDst) return false;

			return lhs.nAdj < rhs.nAdj;
		}
	);
}

ITEMAVAIL CSetItem::GetItemAvail(int nEquiped, bool bAll) const {
	ITEMAVAIL result;

	for (const auto & avail : m_avail) {
		if (avail.anEquiped > nEquiped) return result;
		if (!bAll && avail.anEquiped != nEquiped) continue;

		const auto it = std::find_if(result.begin(), result.end(),
			[&](const SINGLE_DST & sDst) { return sDst.nDst == avail.nDst; }
			);

		if (it == result.end()) {
			result.push_back(SINGLE_DST{ avail.nDst, avail.nAdj });
		} else {
			it->nAdj += avail.nAdj;
		}
	}

	return result;
}

CSetItemFinder g_SetItemFinder;

void CSetItemFinder::AddSetItem(std::unique_ptr<CSetItem> pSetItem) {
	CSetItem * const ptr = pSetItem.release();

	for (const CSetItem::PartItem & partItem : ptr->m_components) {
		const bool bResult = m_mapItemId.emplace(partItem.itemId, ptr).second;
		if (!bResult) {
			TRACE("adding setitem failed\t// 1\n");
		}
	}

	const bool bResult = m_mapSetId.emplace(ptr->m_nId, ptr).second;
	if (!bResult) {
		TRACE("adding setitem failed\t// 0\n");
	}
}

const CSetItem * CSetItemFinder::GetSetItem(const int nSetItemId) const {
	const auto i = m_mapSetId.find( nSetItemId );
	return i != m_mapSetId.end() ? i->second.get() : nullptr;
}

const CSetItem * CSetItemFinder::GetSetItemByItemId(const DWORD dwItemId) const {
	const auto i = m_mapItemId.find(dwItemId);
	return i != m_mapItemId.end() ? i->second : nullptr;
}	


void CRandomOptItemGen::AddRandomOption(const RandomOptItem & pRandomOptItem) {
	if (m_aRandomOptItem.size() == m_aRandomOptItem.static_capacity) {
		Error(__FUNCTION__ "(): More than %lu random options", m_aRandomOptItem.static_capacity);
	}

	m_aRandomOptItem.emplace_back(pRandomOptItem);
}

CRandomOptItemGen g_RandomOptItemGen;

void CRandomOptItemGen::Arrange() {
	// sort
	std::sort(
		m_aRandomOptItem.begin(), m_aRandomOptItem.end(),
		[](const RandomOptItem & lhs, const RandomOptItem & rhs) {
			return lhs.nLevel < rhs.nLevel;
		}
	);

	// make index & make lv index
	int nLevel	= 1, nPrevious	= -1;
	for (int i = 0; i != m_aRandomOptItem.size(); ++i) {
		const bool bResult	= m_mapid.emplace(m_aRandomOptItem[i].nId, i).second;
		if (!bResult) {
			TRACE("adding randomoptitem failed\t// 0\n");
		}
		
		if (m_aRandomOptItem[i].nLevel > nLevel) {
			for (int j = nLevel; j < m_aRandomOptItem[i].nLevel; j++)
				m_anIndex[j - 1] = nPrevious;
			nLevel = m_aRandomOptItem[i].nLevel;
		}

		nPrevious	= i;
	}

	for (int i = nLevel; i <= MAX_MONSTER_LEVEL; i++) {
		m_anIndex[i - 1] = nPrevious;
	}
}

int CRandomOptItemGen::GenRandomOptItem(int nLevel, FLOAT fPenalty, ItemProp * pItemProp, DWORD dwClass) const {
	if (!pItemProp) return 0;
	
	if (pItemProp->dwItemKind1 != IK1_WEAPON && pItemProp->dwItemKind1 != IK1_ARMOR) return 0;

	if (nLevel >= MAX_MONSTER_LEVEL) nLevel = MAX_MONSTER_LEVEL - 1;

	const int i = m_anIndex[nLevel];
	if (i == -1) return 0;

	const int nIndex	= xRandom( i + 1 );
	DWORD dwRandom	= xRandom( 3000000000 );
	if( dwClass == RANK_MIDBOSS )
		dwRandom	/= 5;	// �� ���� ���� ���� �ɼ� Ȯ�� 500%
	
	DWORD dwProbability	= (DWORD)(m_aRandomOptItem[nIndex].dwProbability * fPenalty);
	if (dwRandom >= dwProbability) return 0;
	
	return m_aRandomOptItem[nIndex].nId;
}

const CRandomOptItemGen::RandomOptItem * CRandomOptItemGen::GetRandomOptItem(const int nId) const {
	const auto i = m_mapid.find(nId);
	return i != m_mapid.end() ? &m_aRandomOptItem[i->second] : nullptr;
}

const char * CRandomOptItemGen::GetRandomOptItemString(int nId) const {
	const auto * pRandomOptItem = GetRandomOptItem(nId);
	return pRandomOptItem ? pRandomOptItem->pszString.GetRawStr() : nullptr;
}

BOOL CProject::IsGuildQuestRegion( const D3DXVECTOR3 & vPos )
{
	// That's broken, should require world
	CRect rect;
	POINT point;
	for( int i = 0; i < m_aPropGuildQuest.GetSize(); i++ )
	{
		GUILDQUESTPROP* pProp	= m_aPropGuildQuest.GetAt( i );
		if( pProp == NULL )
			continue;
		
		rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
		point.x = (int)vPos.x;
		point.y = (int)vPos.z;
		if( rect.PtInRect( point ) )
			return TRUE;
	}
	return FALSE;
}


//LoadPropItem�� ȣ���ϰ� ��ó���� �Ѵ�.
void CProject::OnAfterLoadPropItem()
{
	for( int i = 0; i < m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp = (ItemProp*)m_aPropItem.GetAt( i );
		if( pItemProp && pItemProp->dwFlag == NULL_ID )
			pItemProp->dwFlag = 0;

		if( pItemProp && pItemProp->dwItemKind3 != NULL_ID )
		{
			m_itemKindAry[ pItemProp->dwItemKind3 ].Add( pItemProp );

			switch( pItemProp->dwItemKind3 )
			{
			case IK3_EVENTMAIN:
			case IK3_BINDS:
				pItemProp->dwFlag |= IP_FLAG_BINDS;
				break;
			}
		}
	}


	ItemProp* ptmp;
	for( int i = 0; i < MAX_ITEM_KIND3; i++ )
	{
		// sort
		for( int j = 0; j < m_itemKindAry[i].GetSize() - 1; j++ )
		{
			for( int k = j + 1; k < m_itemKindAry[i].GetSize(); k++ )
			{
				if( ( (ItemProp*)m_itemKindAry[i].GetAt( k ) )->dwItemRare < ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
				{
					ptmp	= (ItemProp*)m_itemKindAry[i].GetAt( j );
					m_itemKindAry[i].SetAt( j, (void*)m_itemKindAry[i].GetAt( k ) );
					m_itemKindAry[i].SetAt( k, (void*)ptmp );
				}
			}
		}
		//
		DWORD dwItemRare	= (DWORD)-1;
		for( int j = 0; j < m_itemKindAry[i].GetSize(); j++ )
		{
			if( dwItemRare != ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
			{
				dwItemRare	= ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare;
				if( dwItemRare != (DWORD)-1 )
				{
					m_minMaxIdxAry[i][dwItemRare].cx	= j;
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
				}
			}
			else
			{
				if( dwItemRare != (DWORD)-1 )
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
			}
		}
	}

	LoadExcept( "except.txt" );
}


BOOL CProject::LoadExcept( LPCTSTR lpszFilename )
{
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
	
	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		int nLang	= atoi( s.token );
		int nSubLang	= s.GetNumber();	//

		s.GetToken();	// {
		s.GetToken();	// itemProp & moverProp
		while( *s.token != '}' )
		{
			if( s.Token == "ItemProp" )
			{
				s.GetToken();	// {
				s.GetToken();	// dwID
				while( *s.token != '}' )
				{
					DWORD dwID	= atoi( s.token );
					ItemProp* pItemProp	= GetItemProp( dwID );
					s.GetToken();	// {
					s.GetToken();	// attribute
					while( *s.token != '}' )
					{
						if( GetLanguage() != nLang || GetSubLanguage() != nSubLang )
						{
							s.GetToken();
							continue;
						}
						if( s.Token == "fFlightSpeed" )
						{
							s.GetToken();	// =
							pItemProp->fFlightSpeed	= s.GetFloat();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwShopAble" )
						{
							s.GetToken();	// =
							pItemProp->dwShopAble	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwCircleTime" )
						{
							s.GetToken();	// =
							pItemProp->dwCircleTime	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwFlag" )
						{
							s.GetToken();	// =
							pItemProp->dwFlag	= s.GetNumber();
							s.GetToken();	// ;
						}
#ifdef __JEFF_9_20
						else if( s.Token == "dwLimitLevel1" )
						{
							s.GetToken();	// =
							pItemProp->dwLimitLevel1	= s.GetNumber();
							s.GetToken();	// ;
						}
#endif	// __JEFF_9_20
						else if( s.Token == "dwSkillReadyType" )
						{	// �ݶ�����
							s.GetToken();	// =
							pItemProp->dwSkillReadyType		= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "worldDrop" )
						{
							s.GetToken();	// =
							BOOL b	= (BOOL)s.GetNumber();
							if( !b )
								m_setExcept.insert( dwID );	
							s.GetToken();	// ;
						}
						s.GetToken();	// attribute
					}
					s.GetToken();	// dwID
				}
			}
			s.GetToken();	// ItemProp OR moverProp
		}
		s.GetToken();	// lang
	}
	return TRUE;
}

JobProp* CProject::GetJobProp( int nIndex )
{
	if( nIndex < 0 || nIndex >= MAX_JOB )
	{
		LPCTSTR szErr = Error( "CProject::GetJobProp ����ħ��. %d", nIndex );
		//ADDERRORMSG( szErr );
		return NULL;
	}
	return &m_aPropJob[nIndex];
}

BOOL CProject::LoadPropEnchant( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "������Ƽ �б���� : Enchant", "������", MB_OK );
		//ADDERRORMSG( "������Ƽ �б���� : Enchant" );
#endif //__CLIENT
		return FALSE;
	}
	
	int nKind = 0;
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		if( scanner.Token == _T( "NORMAL" ) )
		{
			m_nEnchantLimitLevel[0] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "AL" ) )
		{
			m_nEnchantLimitLevel[1] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "NONE" ) )
		{
			m_nEnchantLimitLevel[2] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "NORMAL_SCAL" ) )
		{
			m_fEnchantLevelScal[0][0] = scanner.GetFloat();
			m_fEnchantLevelScal[0][1] = scanner.GetFloat();
			m_fEnchantLevelScal[0][2] = scanner.GetFloat();
			m_fEnchantLevelScal[0][3] = scanner.GetFloat();
			m_fEnchantLevelScal[0][4] = scanner.GetFloat();
			m_fEnchantLevelScal[0][5] = scanner.GetFloat();
			m_fEnchantLevelScal[0][6] = scanner.GetFloat();
			m_fEnchantLevelScal[0][7] = scanner.GetFloat();
			m_fEnchantLevelScal[0][8] = scanner.GetFloat();
			m_fEnchantLevelScal[0][9] = scanner.GetFloat();
		}
		else if( scanner.Token == _T( "ALL_SCAL" ) )
		{
			m_fEnchantLevelScal[1][0] = scanner.GetFloat();
			m_fEnchantLevelScal[1][1] = scanner.GetFloat();
			m_fEnchantLevelScal[1][2] = scanner.GetFloat();
			m_fEnchantLevelScal[1][3] = scanner.GetFloat();
			m_fEnchantLevelScal[1][4] = scanner.GetFloat();
			m_fEnchantLevelScal[1][5] = scanner.GetFloat();
			m_fEnchantLevelScal[1][6] = scanner.GetFloat();
			m_fEnchantLevelScal[1][7] = scanner.GetFloat();
			m_fEnchantLevelScal[1][8] = scanner.GetFloat();
			m_fEnchantLevelScal[1][9] = scanner.GetFloat();
		}

		scanner.GetToken();
	}
	
	return TRUE;
}


// propJob.inc�� �о���δ�.
BOOL CProject::LoadPropJob( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "������Ƽ �б���� : Job", "������", MB_OK );
		//ADDERRORMSG( "������Ƽ �б���� : Job" );
#endif //__CLIENT
		return FALSE;
	}
	
	while( 1 )
	{
		int nJob = scanner.GetNumber();	
		if( scanner.tok == FINISHED )
			break;

		JobProp* pProperty = &m_aPropJob[nJob];
		pProperty->fAttackSpeed      = scanner.GetFloat();		
		pProperty->fFactorMaxHP      = scanner.GetFloat();		
		pProperty->fFactorMaxMP      = scanner.GetFloat();		
		pProperty->fFactorMaxFP      = scanner.GetFloat();		
		pProperty->fFactorDef        = scanner.GetFloat();		
		pProperty->fFactorHPRecovery = scanner.GetFloat();	
		pProperty->fFactorMPRecovery = scanner.GetFloat();	
		pProperty->fFactorFPRecovery = scanner.GetFloat();	
		pProperty->fMeleeSWD		 = scanner.GetFloat();	
		pProperty->fMeleeAXE		 = scanner.GetFloat();	
		pProperty->fMeleeSTAFF		 = scanner.GetFloat();	
		pProperty->fMeleeSTICK		 = scanner.GetFloat();	
		pProperty->fMeleeKNUCKLE	 = scanner.GetFloat();	
		pProperty->fMagicWAND		 = scanner.GetFloat();	
		pProperty->fBlocking		 = scanner.GetFloat();	
		pProperty->fMeleeYOYO        = scanner.GetFloat();	
		pProperty->fCritical         = scanner.GetFloat();	
	}
	return TRUE;
}

BOOL CProject::LoadPropCtrl( LPCTSTR lpszFileName, CFixedArray<CtrlProp>*	apObjProp)
{
	CScript scanner;
	// load unicode
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "������Ƽ �б���� : Ctrl", "������", MB_OK );
		//ADDERRORMSG( "������Ƽ �б���� : Ctrl" );
#endif //__CLIENT
		return FALSE;
	}
	
	CtrlProp ctrlProp;

	char szTemp[ 128 ];

	int i	= scanner.GetNumber();	// id

	while( scanner.tok != FINISHED )
	{
		ctrlProp.dwID = i;    
		if( i <= 0 )
			Error( "%s �дٰ� ID�� %d�̳��Դ�. ���������� ������ %s", lpszFileName, i, szTemp );
		scanner.GetToken();
		TRACE( "PropCtrl = %d, %s\n", i, scanner.token );
		_tcscpy( ctrlProp.szName, scanner.token );
		m_mapCtrl.emplace(ctrlProp.szName, ctrlProp.dwID);
		ctrlProp.dwCtrlKind1 = scanner.GetNumber();
		ctrlProp.dwCtrlKind2 = scanner.GetNumber();
		ctrlProp.dwCtrlKind3 = scanner.GetNumber();
		ctrlProp.dwSfxCtrl = scanner.GetNumber();
		ctrlProp.dwSndDamage = scanner.GetNumber();

		scanner.GetToken();		// skip szCommand (������� �ʴ´�. )
		apObjProp->SetAtGrow( i, &ctrlProp);
		i = scanner.GetNumber(); 
	}
	apObjProp->Optimize();
	return TRUE;
}

BOOL CProject::LoadScriptDiePenalty( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		Error( "DiePenalty.ini not found" );
		return FALSE;
	}
	m_vecRevivalPenalty.clear();
	m_vecDecExpPenalty.clear();
	m_vecLevelDownPenalty.clear();
	
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{	
		if( scanner.Token == _T( "REVIVAL_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecRevivalPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		if( scanner.Token == _T( "DECEXP_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecDecExpPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		if( scanner.Token == _T( "LEVEL_DOWN_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecLevelDownPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		scanner.GetToken();
	}
	return TRUE;
}

BOOL CProject::LoadScriptPK( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		Error( "PKSetting.inc not found" );
		return FALSE;
	}

	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		if( scanner.Token == _T( "GENERAL_COLOR" ) )
		{
			m_PKSetting.dwGeneralColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "READY_COLOR" ) )
		{
			m_PKSetting.dwReadyColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "CHAO_COLOR" ) )
		{
			m_PKSetting.dwChaoColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "GENERAL_ATTACK_TIME" ) )
		{
			m_PKSetting.nGeneralAttack = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "READY_ATTACK_TIME" ) )
		{
			m_PKSetting.nReadyAttack = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "LIMITE_LEVEL" ) )
		{
			m_PKSetting.nLimitLevel = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "DIE_PROPENSITY_MIN" ) )
		{
			m_PKSetting.nDiePropensityMin = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "DIE_PROPENSITY_MAX" ) )
		{
			m_PKSetting.nDiePropensityMax = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "PROPENSITY_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					int nPropensity;
					CHAO_PROPENSITY bufPropensity;
					nPropensity						= scanner.GetNumber();
					bufPropensity.dwPropensityMin	= scanner.GetNumber();
					bufPropensity.dwPropensityMax	= scanner.GetNumber();
					bufPropensity.nEquipDorpMin		= scanner.GetNumber();
					bufPropensity.nEquipDorpMax		= scanner.GetNumber();
					bufPropensity.nInvenDorpMin		= scanner.GetNumber();
					bufPropensity.nInvenDorpMax		= scanner.GetNumber();
					bufPropensity.nShop				= scanner.GetNumber();
					bufPropensity.nBank				= scanner.GetNumber();
					bufPropensity.nFly				= scanner.GetNumber();
					bufPropensity.nGuardAttack		= scanner.GetNumber();
					bufPropensity.nVendor			= scanner.GetNumber();
					m_PKSetting.mapPropensityPenalty.emplace(nPropensity, bufPropensity);
				}
				scanner.GetToken();
			}
		}
		else if( scanner.Token == _T( "LEVEL_EXP" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDEXP" ) )
				{
					int nLevel = scanner.GetNumber();
					DWORD dwExp = scanner.GetNumber();
					m_PKSetting.mapLevelExp.emplace(nLevel, dwExp);
				}
				scanner.GetToken();
			}
		}
		scanner.GetToken();
	}
	
	return TRUE;
}


int	CProject::GetMinIdx( int nItemKind3, DWORD dwItemRare )
{
	if( dwItemRare >= MAX_UNIQUE_SIZE )
		return -1;
	return m_minMaxIdxAry[nItemKind3][dwItemRare].cx;
}

int CProject::GetMaxIdx( int nItemKind3, DWORD dwItemRare )
{
	if( dwItemRare >= MAX_UNIQUE_SIZE )
		return -1;
	return m_minMaxIdxAry[nItemKind3][dwItemRare].cy;
}

GUILDQUESTPROP*	CProject::GetGuildQuestProp( int nQuestId )
{
	VERIFY_RANGE( nQuestId, 0, m_aPropGuildQuest.GetSize(), "GetGuildQuestProp range_error", NULL );
	return m_aPropGuildQuest.GetAt( nQuestId );
}

PARTYQUESTPROP*	CProject::GetPartyQuestProp( int nQuestId )
{
	const auto i = m_propPartyQuest.find( nQuestId );
	if( i != m_propPartyQuest.end() )
		return &i->second;
	return NULL;
}	

DWORD CProject::GetSkillPoint( ItemProp* pSkillProp )
{
	DWORD dwPoint = 0;
	if( pSkillProp->dwItemKind1 == JTYPE_BASE )
		dwPoint = m_dwVagSP;
	else if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
		dwPoint = m_dwExpertSP;
	else
		dwPoint = m_dwProSP;
	return dwPoint;
}

LPCTSTR CProject::GetGuildAppell( int nAppell )
{
	VERIFY_RANGE( nAppell, 0, MAX_GUILDAPPELL, "GetGuildAppell range_error", "error" );
	return m_aGuildAppell[ nAppell ].szName;
}


int CProject::GetExpUpItem( DWORD dwItemKind3, int nOption ) 
{ 
	if( nOption == 0 )	
		return 0;

	if( nOption < 0 || nOption > 11 )	
	{
		Error( "CProject::GetExpUpItem range_error. %d", nOption );
		return 0;
	}

	switch( dwItemKind3 )
	{
	case IK3_SWD:
	case IK3_CHEERSTICK:
	case IK3_WAND:	
	case IK3_AXE:
	case IK3_KNUCKLEHAMMER:
	case IK3_STAFF:
	case IK3_YOYO: 
	case IK3_BOW:
	case IK3_COLLECTER:
		return m_aExpUpItem[0][ nOption ];
	case IK3_HELMET:
		return m_aExpUpItem[1][ nOption ];
	case IK3_SUIT:
		return m_aExpUpItem[2][ nOption ];
	case IK3_GAUNTLET:
		return m_aExpUpItem[3][ nOption ];
	case IK3_BOOTS:
		return m_aExpUpItem[4][ nOption ];
	case IK3_SHIELD:
		return m_aExpUpItem[5][ nOption ];
	}

	Error( "GetExpUpItem : itemkind���� %d, %d", dwItemKind3, nOption );
	return 0;
}

#ifdef __WORLDSERVER
#ifdef __JEFF_11_3
BOOL	CProject::LoadServerScript( const char* sFile )
{
	CScript s;
	if( s.Load( sFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Accessory_Probability" ) )	// ������ ����
		{
			const auto probabilities = s.GetNumbers<DWORD>('}');
			g_AccessoryProperty.ChangeProbabilities(probabilities);
		}
		else if( s.Token == _T( "Collecting_Enchant" ) )
		{
			CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
			std::vector<int>* ptr	= pProperty->GetEnchantProbabilityPtr();
			// 1 / 1000
			*ptr = s.GetNumbers('}');
		}
		else if( s.Token == _T( "Collecting_Item" ) )
		{
			CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
			std::vector<COLLECTING_ITEM>* ptr	= pProperty->GetItemPtr();
			COLLECTING_ITEM item;
			DWORD dwTotal	= 0;
			s.GetToken();	// {
			item.dwItemId	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwTotal		+= s.GetNumber();
				item.dwProb	= dwTotal;
				ptr->push_back( item );
				item.dwItemId	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Pet_LevelupAvail" ) )
		{
			CPetProperty* pProperty		= CPetProperty::GetInstance();
			LPDWORD ptr1	= pProperty->GetLevelupAvailLevelProbabilityPtr();
			LPBYTE ptr2		= pProperty->GetLevelupAvailLevelMaxPtr();
			// 	10000	0	0	0	0	0	0	0	0
			int	nLevel	= (int)PL_D;
			s.GetToken();	// {{
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
//				ptr1[nLevel][0]	= dwProbability;
				ptr1[nLevel*MAX_PET_AVAIL_LEVEL]	= dwProbability;
				for( int i = 1; i < MAX_PET_AVAIL_LEVEL; i++ )
				{
//					ptr1[nLevel][i]	= s.GetNumber();
					ptr1[nLevel*MAX_PET_AVAIL_LEVEL+i]	= s.GetNumber();
//					if( ptr1[nLevel][i] > 0 )
					if( ptr1[nLevel*MAX_PET_AVAIL_LEVEL+i] > 0 )
						ptr2[nLevel]	= i;
				}
				nLevel++;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Pet_AddLifeProbability" ) )
		{
			CPetProperty* pProperty		= CPetProperty::GetInstance();
			std::vector<WORD>* ptr	= pProperty->GetAddLifeProbabilityPtr();
			// ���� ȸ���� Ȯ��	// �߰� �� ��� ���� 100�� �ǵ��� Ȯ��
			// 	50	// +1
			*ptr = s.GetNumbers<WORD>('}');
		}
		s.GetToken();
	}
	return TRUE;
}
#endif	// __JEFF_11_3
#endif	// __WORLDSERVER

#ifdef __PERF_0226
CPartsItem::CPartsItem()
{
	m_pProject	= NULL;
}

CPartsItem::~CPartsItem()
{
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < MAX_HUMAN_PARTS; j++ )
			m_items[i][j].clear();
	}
}

void	CPartsItem::Init( CProject* pProject )
{
	m_pProject	= pProject;
	int nSize	= pProject->m_aPropItem.GetSize();
	for( int i = 0; i < nSize; i++ )
	{
		ItemProp* pProp	= pProject->GetItemProp( i );
		if( pProp && pProp->dwParts >= 0 && pProp->dwParts < MAX_HUMAN_PARTS )
		{
			int nSex	= pProp->dwItemSex;
			if( nSex < 0 )
				nSex	= SEX_SEXLESS;
			m_items[nSex][pProp->dwParts].push_back( pProp );
		}
	}
}

ItemProp*	CPartsItem::GetItemProp( int nSex, int nParts )
{
	int nIndex	= xRandom( m_items[nSex][nParts].size() );
	return	m_items[nSex][nParts][nIndex];
}

CPartsItem*	CPartsItem::GetInstance( void )
{
	static	CPartsItem	sPartsItem;
	return &sPartsItem;
}
#endif	// __PERF_0226

void CProject::OutputDropItem( void )
{
	const int DROP_COUNT	= 1;
	CString sLog[100];
	int nCount	= 0;
	int nWrite	= 0;
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		
		if( (++nCount % 10) == 0 )
		{
			FILE* fp;
			if(!(fp = fopen("drop.txt","a")))
				if(!(fp = fopen("drop.txt" ,"wt")))
					return;
			fprintf(fp,sLog[nWrite++]);
			fclose(fp);
		}
		CString s;
		MoverProp* pMoverProp	= prj.GetMoverProp( i );
		if( pMoverProp && pMoverProp->dwID > 0 && pMoverProp->dwID < MAX_PROPMOVER && pMoverProp->dwBelligerence != BELLI_PEACEFUL && pMoverProp->dwBelligerence != BELLI_ALLIANCE )
		{
			TRACE( "%d\n", i );
			s.Format( "\n%s\t%d", pMoverProp->szName, pMoverProp->m_DropItemGenerator.m_dwMax );
			// dropitem
			{
				int cbDropItem	= pMoverProp->m_DropItemGenerator.GetSize();
				int nNumber	= 0;
				DROPITEM* lpDropItem;
				for( int i = 0; i < cbDropItem; i++ )
				{
					if( lpDropItem = pMoverProp->m_DropItemGenerator.GetAt( i, FALSE, 0.0f ) )
					{
						if( lpDropItem->dtType == DROPTYPE_NORMAL )
						{
							DWORD dwProbability		= lpDropItem->dwProbability;
							ItemProp* pItemProp	= prj.GetItemProp( lpDropItem->dwIndex );
							if( lpDropItem->dwLevel != 0 )
							{
								if( pItemProp && (int)pItemProp->dwItemLV > 0 )
								{
									int nValue	= dwProbability / pItemProp->dwItemLV;
									if( nValue < 21 )	nValue	= 21;
									dwProbability	= nValue - 20;
								}
							}
							if( pItemProp )
							{
								CString str;
								str.Format( "\n \t%s\t%f%%\t%d\t%d", pItemProp->szName, (double)dwProbability / (double)30000000, lpDropItem->dwNumber, lpDropItem->dwLevel );
								s	+= str;
							}
						}
						else if( lpDropItem->dtType == DROPTYPE_SEED )
						{
							CString str;
							str.Format(" \tPENYA %d-%d", lpDropItem->dwNumber, lpDropItem->dwNumber2 ); 
							s	+= str;
						}
					}
				}
				sLog[nWrite]	+= s;
			}
			// dropkind
			{
				int nSize	= pMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
				for( int i = 0; i < nSize; i++ )
				{
					pDropKind	= pMoverProp->m_DropKindGenerator.GetAt( i );
					pItemKindAry	= prj.GetItemKindAry( pDropKind->dwIK3 );
					int nMinIdx	= -1,	nMaxIdx		= -1;
					for( int j = pDropKind->nMinUniq; j <= pDropKind->nMaxUniq; j++ )
					{
						nMinIdx		= prj.GetMinIdx( pDropKind->dwIK3, j );
						if( nMinIdx != -1 )
							break;
					}
					for( int j = pDropKind->nMaxUniq; j >= pDropKind->nMinUniq; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pDropKind->dwIK3, j );
						if( nMaxIdx != -1 )
							break;
					}
					if( nMinIdx < 0 || nMaxIdx < 0 )
						continue;

					for( int a = nMinIdx; a <= nMaxIdx; a++ )
					{
						ItemProp* pItemProp		= (ItemProp*)pItemKindAry->GetAt( a );
						if( !pItemProp )
							continue;

						CString s1;
						s1.Format( "\n \t%s", pItemProp->szName );
						for( int k = 10; k >= 0; k-- )
						{
							DWORD dwPrabability		= (DWORD)( prj.m_adwExpDropLuck[( pItemProp->dwItemLV > 120? 119: pItemProp->dwItemLV-1 )][k] * ( (float)pMoverProp->dwCorrectionValue / 100.0f ) );
							CString s2;
							s2.Format( "\t%d=%f%%", k, (double)dwPrabability / (double)30000000 );
							s1	+= s2;
						}
						sLog[nWrite]	+= s1;
					}
				}
			}
		}
	}

	FILE* fp;
	if(!(fp = fopen("drop.txt","a")))
		if(!(fp = fopen("drop.txt" ,"wt")))
			return;
	fprintf(fp,sLog[nWrite]);
	fclose(fp);
}

#if defined( __CLIENT )
BOOL CProject::LoadQuestDestination( void )
{
	CScanner s;
	if( s.Load( "QuestDestination.txt.txt" ) == FALSE )
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		int nID = atoi( s.Token.Right( 4 ) );
		s.GetLastFull();
		CString strQuestion = s.Token;
		m_mapQuestDestination.emplace(nID, strQuestion);
		s.GetToken();
	}
	return TRUE;
}

const CString& CProject::GetQuestDestination( DWORD dwKey ) const
{
	static CString strEmpty = _T( "" );
	const auto i = m_mapQuestDestination.find( dwKey );
	if( i != m_mapQuestDestination.end() )
		return i->second;
	return strEmpty;
}

BOOL CProject::LoadPatrolDestination( void )
{
	CScanner s;
	if( s.Load( "PatrolDestination.txt.txt" ) == FALSE )
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		int nID = atoi( s.Token.Right( 4 ) );
		s.GetLastFull();
		CString strQuestion = s.Token;
		m_mapPatrolDestination.emplace( nID, strQuestion);
		s.GetToken();
	}
	return TRUE;
}

const CString& CProject::GetPatrolDestination( DWORD dwKey ) const
{
	static CString strEmpty = _T( "" );
	const auto i = m_mapPatrolDestination.find( dwKey );
	if( i != m_mapPatrolDestination.end() )
		return i->second;
	return strEmpty;
}
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )