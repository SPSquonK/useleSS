#include "stdafx.h"
#include "lang.h"
#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013
#include <algorithm>
#include <ranges>

#if !( defined(__DBSERVER) || defined(__VPW) ) 
	#include "project.h"
#endif	// __DBSERVER

CString GetLangFileName( int nLang, int nType )
{
	const char* file[] =
		{ "Filter", "InvalidName", "Notice", "GuildCombatTEXT_1", "GuildCombatTEXT_2", "GuildCombatTEXT_3", "GuildCombatTEXT_4", "GuildCombatTEXT_5", "GuildCombatTEXT_6" 
#ifdef __RULE_0615
			,"Letter"
#endif	// __RULE_0615
			,"GuildCombat1to1_TEXT1", "GuildCombat1to1_TEXT2", "GuildCombat1to1_TEXT3", "GuildCombat1to1_TEXT4", "GuildCombat1to1_TEXT5"
#ifdef __VENDOR_1106
			, "Letter1"
#endif	// __VENDOR_1106
		};

	CString fileName = file[nType];

#ifdef __LANG_1013
	fileName	= fileName + CLangMan::GetInstance()->GetLangData( nLang )->szFileName;
#else	// __LANG_1013
	switch( nLang )
	{
	case LANG_KOR:
		fileName = fileName+"_KOR.inc";
		break;
	case LANG_ENG:
		fileName = fileName+"_ENG.inc";
		break;
	case LANG_JAP:
		fileName = fileName+"_JAP.inc";
		break;
	case LANG_CHI:
		fileName = fileName+"_CHI.inc";
		break;
	case LANG_THA:
		fileName = fileName+"_THA.inc";
		break;
	case LANG_TWN:
	case LANG_HK:
		fileName = fileName+"_TWN.inc";
		break;
	case LANG_VTN:
		fileName = fileName+"_VTN.inc";
		break;
	case LANG_RUS:
		fileName = fileName+"_RUS.inc";
		break;
	default:
		Error( "%s File Error = %d", file[nType], nLang );
		break;
	}
#endif	// __LANG_1013

	return fileName;
}

BOOL CProject::LoadPropMover( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Mover", "프리프", MB_OK );
		//ADDERRORMSG( "프로퍼티 읽기실패 : Mover" );
#endif //__CLIENT
		return FALSE;
	}

	std::set<int> checker;

	TCHAR szLastName[256];
	int i = 0;
	memset( szLastName, 0, sizeof(szLastName) );
	int old;
	while( 1 )
	{
		old = i;
		i = scanner.GetNumber();		// id
		if( scanner.tok == FINISHED )
			break;

		if( i == 0 )
			continue;

		if( i<0 || i>=MAX_PROPMOVER )		// 범위체크 
		{
			Error( "too many mover property or invalid id:%d FILE:%s\n", i, lpszFileName );
			return FALSE;
		}

		if( !checker.insert( i ).second )	// id중복체크 
		{
			Error( "Last read:%s, ID:%d is duplicated. 컬럼이 밀려도 이런에러가 난다. FILE:%s\n", szLastName, i, lpszFileName );
			return FALSE;
		}

		MoverProp* pProperty = &m_pPropMover[i];
		pProperty->dwID	= i;
		
		scanner.GetToken();
		_tcscpy( szLastName, scanner.token );
		_tcscpy( pProperty->szName, scanner.token );

#if !( defined(__DBSERVER) || defined(__VPW) )
		m_mapMVI.emplace(pProperty->szName, pProperty->dwID);
#endif	// __DBSERVER

		pProperty->dwAI					= scanner.GetNumber();
		pProperty->dwStr				= scanner.GetNumber();
		pProperty->dwSta				= scanner.GetNumber();
		pProperty->dwDex				= scanner.GetNumber();
		pProperty->dwInt				= scanner.GetNumber();
		pProperty->dwHR					= scanner.GetNumber();
		pProperty->dwER					= scanner.GetNumber();
		pProperty->dwRace				= scanner.GetNumber();
		pProperty->dwBelligerence		= scanner.GetNumber();
		pProperty->dwGender				= scanner.GetNumber();
		pProperty->dwLevel				= scanner.GetNumber();
		pProperty->dwFlightLevel		= scanner.GetNumber();
		pProperty->dwSize				= scanner.GetNumber();
		pProperty->dwClass				= scanner.GetNumber();
		pProperty->bIfParts				= scanner.GetNumber();

		if( pProperty->bIfParts == NULL_ID )
			Error( "LoadPropMover : bIfParts엔 -1을 넣으면 안된다. %s \r\n컬럼이 밀렸거나 워킹폴더를 잘못지정했을수도있음 ", pProperty->szName );

		pProperty->nChaotic				= scanner.GetNumber();
#ifdef __S1108_BACK_END_SYSTEM
		pProperty->dwUseable			= scanner.GetNumber();
#else // __S1108_BACK_END_SYSTEM
		pProperty->dwDefExtent			= scanner.GetNumber();
#endif // __S1108_BACK_END_SYSTEM
		pProperty->dwActionRadius		= scanner.GetNumber();
		pProperty->dwAtkMin				= scanner.GetNumber();
		pProperty->dwAtkMax				= scanner.GetNumber();
		pProperty->dwAtk1				= scanner.GetNumber();
		pProperty->dwAtk2				= scanner.GetNumber();
		pProperty->dwAtk3				= scanner.GetNumber();
		pProperty->dwAtk4				= scanner.GetNumber();
		pProperty->fFrame	= scanner.GetFloat();	// -1이면 가중치 영향 없음. 1.0 기본 값
		if( abs( -1.0F  - pProperty->fFrame ) < 0.000001F )
			pProperty->fFrame	= 1.0F;
		pProperty->dwOrthograde	= scanner.GetNumber();
		pProperty->dwThrustRate			= scanner.GetNumber();

		pProperty->dwChestRate			= scanner.GetNumber();
		pProperty->dwHeadRate			= scanner.GetNumber();
		pProperty->dwArmRate			= scanner.GetNumber();
		pProperty->dwLegRate			= scanner.GetNumber();

		pProperty->dwAttackSpeed		= scanner.GetNumber();
		pProperty->dwReAttackDelay		= scanner.GetNumber();
		pProperty->dwAddHp				= scanner.GetNumber();
		pProperty->dwAddMp				= scanner.GetNumber();
		pProperty->dwNaturalArmor		= scanner.GetNumber();
		pProperty->nAbrasion			= scanner.GetNumber();
		pProperty->nHardness			= scanner.GetNumber();
		pProperty->dwAdjAtkDelay		= scanner.GetNumber();
		
		pProperty->eElementType			= static_cast<SAI79::ePropType>(scanner.GetNumber());	// 정수타입과 enum	타입이 틀려서 타입 변환을 허용함.
		pProperty->wElementAtk			= static_cast<short>(scanner.GetNumber());				// 정수타입과 short	타입이 틀려서 타입 변환을 허용함.

		pProperty->dwHideLevel			= scanner.GetNumber();
		pProperty->fSpeed				= scanner.GetFloat();
		pProperty->dwShelter			= scanner.GetNumber();
		pProperty->dwFlying				= scanner.GetNumber();
		pProperty->dwJumpIng			= scanner.GetNumber();
		pProperty->dwAirJump			= scanner.GetNumber();
		pProperty->bTaming				= scanner.GetNumber();
		pProperty->dwResisMgic			= scanner.GetNumber();

		pProperty->nResistElecricity	= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistFire			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWind			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWater			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistEarth			= (int)(scanner.GetFloat() * 100.0f);

		pProperty->dwCash				= scanner.GetNumber();
		pProperty->dwSourceMaterial		= scanner.GetNumber();
		pProperty->dwMaterialAmount		= scanner.GetNumber();
		pProperty->dwCohesion			= scanner.GetNumber();
		pProperty->dwHoldingTime		= scanner.GetNumber();
		pProperty->dwCorrectionValue	= scanner.GetNumber();
		pProperty->nExpValue			= scanner.GetExpInteger();
		pProperty->nFxpValue			= scanner.GetNumber();
		pProperty->nBodyState			= scanner.GetNumber();
		pProperty->dwAddAbility			= scanner.GetNumber();
		pProperty->bKillable			= scanner.GetNumber();
		
		pProperty->dwVirtItem[0]		= scanner.GetNumber();
		pProperty->dwVirtItem[1]		= scanner.GetNumber();
		pProperty->dwVirtItem[2]		= scanner.GetNumber();
		pProperty->bVirtType[0]			= scanner.GetNumber();
		pProperty->bVirtType[1]			= scanner.GetNumber();
		pProperty->bVirtType[2]			= scanner.GetNumber();

		pProperty->dwSndAtk1			= scanner.GetNumber();
		pProperty->dwSndAtk2 			= scanner.GetNumber();

		pProperty->dwSndDie1 			= scanner.GetNumber();
		pProperty->dwSndDie2 			= scanner.GetNumber();

		pProperty->dwSndDmg1 			= scanner.GetNumber();
		pProperty->dwSndDmg2 			= scanner.GetNumber();
		pProperty->dwSndDmg3 			= scanner.GetNumber();

		pProperty->dwSndIdle1 			= scanner.GetNumber();
		pProperty->dwSndIdle2 			= scanner.GetNumber();


		scanner.GetToken();

#ifdef __S1108_BACK_END_SYSTEM
		pProperty->m_fHitPoint_Rate = 1.0f;		
		pProperty->m_fAttackPower_Rate = 1.0f;
		pProperty->m_fDefence_Rate = 1.0f;		
		pProperty->m_fExp_Rate = 1.0f;			
		pProperty->m_fItemDrop_Rate = 1.0f;
		pProperty->m_fPenya_Rate = 1.0f;			
		pProperty->m_bRate = FALSE;
#endif // __S1108_BACK_END_SYSTEM
	}

	if( checker.size() > 0 )
		m_nMoverPropSize = *(checker.rbegin()) + 1;
	
	/*
	#ifdef __INTERNALSERVER
	{
		int nIndex = 20; // MI_AIBATT1 
		MoverProp* pProperty = &m_pPropMover[ nIndex ];
		pProperty->dwAI = AII_VER2_TYPE0; 
	}	
	#endif
	*/

	return TRUE;
}

BOOL CProject::LoadPropItem( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
	#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "LoadPropItem", "flyff", MB_OK );
		//ADDERRORMSG( "LoadPropItem file open error" );
	#endif //__CLIENT
		return FALSE;
	}

	ItemProp prop;
	ZeroMemory( &prop, sizeof(prop) );

	int nVer = scanner.GetNumber();	// version
	while( scanner.tok != FINISHED )
	{
		DWORD dwID = scanner.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			Error( "%s ID:%d last_read(ID:%d name:%s)", lpszFileName, dwID, prop.dwID, prop.szName);
			return FALSE;
		}

//#ifdef __CLIENT
		prop.nVer	= nVer;
//#endif	// __CLIENT
		prop.dwID = dwID;
		scanner.GetToken();
//		_tcscpy( prop.szName, scanner.token );
		scanner.Token.TrimRight();
		_tcscpy( prop.szName, (LPCSTR)scanner.Token );

 		prop.dwNum				= scanner.GetNumber();            
		prop.dwPackMax			= scanner.GetNumber();      
		prop.dwItemKind1		= scanner.GetNumber();      
		prop.dwItemKind2		= scanner.GetNumber();      
		prop.dwItemKind3		= scanner.GetNumber();      
		prop.dwItemJob			= scanner.GetNumber();      
		prop.bPermanence		= scanner.GetNumber();      
		prop.dwUseable			= scanner.GetNumber();      
		prop.dwItemSex			= scanner.GetNumber();      
		prop.dwCost				= scanner.GetNumber();      
		prop.dwEndurance		= scanner.GetNumber(); 
		prop.nAbrasion			= scanner.GetNumber();
		prop.nMaxRepair			= scanner.GetNumber();
		prop.dwHanded			= scanner.GetNumber();      
		prop.dwFlag				= scanner.GetNumber();      
		prop.dwParts			= scanner.GetNumber();      
		prop.dwPartsub			= scanner.GetNumber();      
		prop.bPartsFile			= scanner.GetNumber();      
		prop.dwExclusive		= scanner.GetNumber();      
		prop.dwBasePartsIgnore	= scanner.GetNumber();      
		prop.dwItemLV			= scanner.GetNumber();      
		prop.dwItemRare			= scanner.GetNumber();   
		prop.dwShopAble			= scanner.GetNumber();
		prop.nLog				= scanner.GetNumber();
		prop.bCharged			= scanner.GetNumber();
		prop.bCharged	= ( prop.bCharged != 1? 0: 1 );
		prop.dwLinkKindBullet	= scanner.GetNumber();      
		prop.dwLinkKind			= scanner.GetNumber();      
		prop.dwAbilityMin		= scanner.GetNumber();      
		prop.dwAbilityMax		= scanner.GetNumber();      
		prop.eItemType			= static_cast<SAI79::ePropType>(scanner.GetNumber());
		prop.wItemEatk			= static_cast<short>(scanner.GetNumber());
		prop.dwParry			= scanner.GetNumber();
		prop.dwblockRating		= scanner.GetNumber();
		prop.nAddSkillMin		= scanner.GetNumber();
		prop.nAddSkillMax		= scanner.GetNumber();
		prop.dwAtkStyle			= scanner.GetNumber();      
		prop.dwWeaponType		= scanner.GetNumber();      
		prop.dwItemAtkOrder1	= scanner.GetNumber();      	
		prop.dwItemAtkOrder2	= scanner.GetNumber();      	
		prop.dwItemAtkOrder3	= scanner.GetNumber();      	
		prop.dwItemAtkOrder4	= scanner.GetNumber();      	
		prop.tmContinuousPain	= scanner.GetNumber();      
		prop.nShellQuantity		= scanner.GetNumber();      
		prop.dwRecoil			= scanner.GetNumber();      
		prop.dwLoadingTime		= scanner.GetNumber();      
		prop.nAdjHitRate		= scanner.GetNumber();      
		prop.fAttackSpeed		= scanner.GetFloat();      
		prop.dwDmgShift			= scanner.GetNumber();      
		prop.dwAttackRange		= scanner.GetNumber();      
		prop.nProbability		= scanner.GetNumber();      
		prop.dwDestParam[0]		= scanner.GetNumber();      
		prop.dwDestParam[1]		= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.dwDestParam[2]		= scanner.GetNumber();      
#endif	// __PROP_0827
		prop.nAdjParamVal[0]	= scanner.GetNumber();      
		prop.nAdjParamVal[1]	= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.nAdjParamVal[2]	= scanner.GetNumber();      
#endif	// __PROP_0827
		prop.dwChgParamVal[0]	= scanner.GetNumber();      
		prop.dwChgParamVal[1]	= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.dwChgParamVal[2]	= scanner.GetNumber();      
#endif	// __PROP_0827
		prop.nDestData1[0]		= scanner.GetNumber();      
		prop.nDestData1[1]		= scanner.GetNumber();   
		prop.nDestData1[2]		= scanner.GetNumber();      
		prop.dwActiveSkill		= scanner.GetNumber();
		prop.dwActiveSkillLv	= scanner.GetNumber();
		prop.dwActiveSkillRate	= scanner.GetNumber();      
		prop.dwReqMp			= scanner.GetNumber();      
		prop.dwReqFp			= scanner.GetNumber();      
        prop.dwReqDisLV			= scanner.GetNumber();      
		prop.dwReSkill1			= scanner.GetNumber();
		prop.dwReSkillLevel1	= scanner.GetNumber();
		prop.dwReSkill2			= scanner.GetNumber();
		prop.dwReSkillLevel2	= scanner.GetNumber();
        prop.dwSkillReadyType	= scanner.GetNumber();      
		prop.dwSkillReady		= scanner.GetNumber();      
		prop._dwSkillRange		= scanner.GetNumber();      
		prop.dwSfxElemental		= scanner.GetNumber();      
		prop.dwSfxObj			= scanner.GetNumber();      
		prop.dwSfxObj2			= scanner.GetNumber();      
		prop.dwSfxObj3			= scanner.GetNumber();      
		prop.dwSfxObj4			= scanner.GetNumber();      
		prop.dwSfxObj5			= scanner.GetNumber();      
		prop.dwUseMotion		= scanner.GetNumber();      
		prop.dwCircleTime		= scanner.GetNumber();      
		prop.dwSkillTime		= scanner.GetNumber();
		prop.dwExeTarget		= scanner.GetNumber();      
		prop.dwUseChance		= scanner.GetNumber();      
		prop.dwSpellRegion		= scanner.GetNumber();   
		prop.dwSpellType		= scanner.GetNumber();
		prop.dwReferStat1		= scanner.GetNumber();      
		prop.dwReferStat2		= scanner.GetNumber();      
		prop.dwReferTarget1		= scanner.GetNumber();      
		prop.dwReferTarget2		= scanner.GetNumber();      
		prop.dwReferValue1		= scanner.GetNumber();      
		prop.dwReferValue2		= scanner.GetNumber();      
		prop.dwSkillType		= scanner.GetNumber();      
		prop.nItemResistElecricity	= (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistFire	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistWind	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistWater	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistEarth	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nEvildoing			= scanner.GetNumber();      
		prop.dwExpertLV			= scanner.GetNumber();      
		prop.dwExpertMax		= scanner.GetNumber();   
		prop.dwSubDefine		= scanner.GetNumber();
		prop.dwExp				= scanner.GetNumber();      
		prop.dwComboStyle		= scanner.GetNumber();      
		prop.fFlightSpeed		= scanner.GetFloat();      
		prop.fFlightLRAngle		= scanner.GetFloat();      
		prop.fFlightTBAngle		= scanner.GetFloat();      
		prop.dwFlightLimit		= scanner.GetNumber();      
		prop.dwFFuelReMax		= scanner.GetNumber();      
		prop.dwAFuelReMax		= scanner.GetNumber();      
		prop.dwFuelRe			= scanner.GetNumber();      
		prop.dwLimitLevel1		= scanner.GetNumber();
		prop.nReflect			= scanner.GetNumber();      
		prop.dwSndAttack1		= scanner.GetNumber();     
		prop.dwSndAttack2		= scanner.GetNumber();     
		
		// szIcon
		scanner.GetToken();
		scanner.GetToken();	
	#ifdef __CLIENT		
		_tcsncpy( prop.szIcon, scanner.token, sizeof(prop.szIcon)-1 );
		prop.szCommand[sizeof(prop.szIcon)-1] = 0;
	#endif 

#ifdef __VERIFY_0201
		_tcsncpy( prop.szIcon, scanner.token, sizeof(prop.szIcon)-1 );
		prop.szCommand[sizeof(prop.szIcon)-1] = 0;
#endif	// __VERIFY_0201

		scanner.GetToken();

		// dwQuestId
		prop.dwQuestId	= QuestId(scanner.GetNumber(  ));

		// szTextFileName 
		scanner.GetToken();
		scanner.GetToken(); 
		_tcscpy( prop.szTextFileName, scanner.token );
		scanner.GetToken();

		// szCommand
		scanner.GetToken();		
	#ifdef __CLIENT		
		_tcsncpy( prop.szCommand, scanner.token, sizeof(prop.szCommand)-1 );
		prop.szCommand[sizeof(prop.szCommand)-1] = 0;
	#endif // __CLIENT

	#ifdef __WORLDSERVER
		#ifdef __INTERNALSERVER
			prop.nLog = -1;		// 개발섭에선 이거 무시.
		#endif
		if( g_eLocal.GetState( EVE_SCHOOL ) )
		{
			prop.dwShopAble	= 1;
			if( prop.dwItemRare == (DWORD)-1 )
				prop.dwItemRare		= prop.dwItemLV;
		}
	#endif	// __WORLDSERVER

		if( ::GetLanguage() == LANG_THA )
		{
			if( prop.dwLimitLevel1 > 0 && prop.dwLimitLevel1 <= 5 )
				prop.dwLimitLevel1	= 1;
			else if( prop.dwLimitLevel1 > 5 && prop.dwLimitLevel1 <= 15 )
				prop.dwLimitLevel1	= 5;
			else if( prop.dwLimitLevel1 > 15 && prop.dwLimitLevel1 <= 30 )
				prop.dwLimitLevel1	= 15;
			else if( prop.dwLimitLevel1 > 30 && prop.dwLimitLevel1 <= 45 )
				prop.dwLimitLevel1	= 30;
			else if( prop.dwLimitLevel1 > 45 && prop.dwLimitLevel1 < 60 )
				prop.dwLimitLevel1	= 45;
		}
		
//			TRACE( "PropItem = %d, %s\n", prop.dwID, prop.szName );
		m_mapII.emplace(prop.szName, prop.dwID);
#ifdef __CLIENT
		if( prop.IsUltimate() )
		{
			CString strName	= prop.szName;
			int	nFind	= strName.Find( "@", 0 );
			if( nFind > -1 )
			{
				strName.Delete( nFind - 1, 2 );
				lstrcpy( prop.szName, (LPCTSTR)strName );
			}
		}
#endif	// __CLIENT
		apObjProp->SetAtGrow(prop.dwID, prop);


		nVer = scanner.GetNumber();	// version; 
	}
	apObjProp->Optimize();
	return TRUE;
}

BOOL CProject::LoadText( LPCTSTR lpszFileName )
{
	tagColorText colorText;

	CScript scanner;
	if( scanner.Load( lpszFileName ) == FALSE )
		return FALSE;

	CStringArray strArray;
	CDWordArray  colorArray;
	DWORD dwId = scanner.GetNumber();

	do {
		DWORD dwColor = scanner.GetNumber(); // color
		scanner.GetToken();
		if( *scanner.token  == '{' )
		{

			scanner.GetToken();
			CString str	= scanner.token;
			str.Replace( "\"", "" );
//			if( str.IsEmpty() )
//				str	= "Empty";
			#ifdef _DEBUG
			if( strArray.GetSize() > (int)( dwId ) )
				if( strArray.GetAt( dwId ).IsEmpty() == FALSE )
					Error( "CProject::LoadText : 같은 아이디 존재 %d - %s", dwId, str );						
			#endif	// _DEBUG	
			strArray.SetAtGrow( dwId, str );
			colorArray.SetAtGrow( dwId, dwColor );
			scanner.GetToken();	// }
		}
		dwId = scanner.GetNumber();	// next
	} while( scanner.tok != FINISHED );

	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		if( strArray.GetAt( i ).IsEmpty() == FALSE )
		{
			m_colorText.SetAtGrow(i, colorText);

			tagColorText * pColorText = m_colorText.GetAt( i );
			pColorText->dwColor = colorArray.GetAt( i );
			pColorText->lpszData = strdup( strArray.GetAt( i ) ) ;
		}
	}
	m_colorText.Optimize();
	return TRUE;
}

CString CProject::GetLangScript( CScript& script )
{
	CString string;
	script.GetToken();
	string	= script.Token;
	script.GetToken();	// )
	script.GetToken(); // ;
	if( string.IsEmpty() )
		string = " ";
	return string;
}

void CProject::LoadStrings() {
	static constexpr std::initializer_list<const char *> lpszFiles = {
		"character.txt.txt",
		"character-etc.txt.txt",
		"character-school.txt.txt",
		"etc.txt.txt",
		"propCtrl.txt.txt",
		"propItem.txt.txt",
		"propItemEtc.txt.txt",
		"propKarma.txt.txt",
		"propMotion.txt.txt",
		"propMover.txt.txt",
		"propQuest.txt.txt",
		"propQuest-DungeonandPK.txt.txt",
		"propQuest-RequestBox2.txt.txt",
		"propQuest-RequestBox.txt.txt",
		"propQuest-Scenario.txt.txt",
		"propSkill.txt.txt",
		"propTroupeSkill.txt.txt",
		"resData.txt.txt",
		"textClient.txt.txt",
		"textEmotion.txt.txt",
		"world.txt.txt",
		"Client\\treeHelp.txt.txt",
		"Client\\tip.txt.txt",
		"Client\\PatchClient.txt.txt",
		"Client\\InstantHelp.txt.txt",
		"Client\\help.txt.txt",
		"Client\\Guide.txt.txt",
		"Client\\tutorial.txt.txt",
//		"Client\\GameGuard.txt.txt",
		"Client\\faq.txt.txt",
		"World\\WdVolcane\\WdVolcane.txt.txt",
		"World\\WdMadrigal\\wdMadrigal.txt.txt",
		"World\\WdKebaras\\WdKebaras.txt.txt",
		"World\\WdGuildWar\\WdGuildWar.txt.txt",
		"World\\WdEvent01\\WdEvent01.txt.txt",
		"World\\DuMuscle\\DuMuscle.txt.txt",
		"World\\DuKrr\\DuKrr.txt.txt",
		"World\\DuFlMas\\DuFlMas.txt.txt",
		"World\\DuDaDk\\DuDaDk.txt.txt",
		"World\\DuBear\\DuBear.txt.txt",
		"World\\DuSaTemple\\DuSaTemple.txt.txt",
		"World\\DuSaTempleBoss\\DuSaTempleBoss.txt.txt"
		,"World\\WdVolcane\\WdVolcane.txt.txt"
		,"World\\WdVolcaneRed\\WdVolcaneRed.txt.txt"
		,"World\\WdVolcaneYellow\\WdVolcaneYellow.txt.txt"
#ifdef __JEFF_11_4
		,"World\\WdArena\\WdArena.txt.txt"
#endif	// __JEFF_11_4
		,"lordskill.txt.txt"
		,"World\\WdHeaven01\\wdheaven01.txt.txt"
		,"World\\WdHeaven02\\wdheaven02.txt.txt"
		,"World\\WdHeaven03\\wdheaven03.txt.txt"
		,"World\\WdHeaven04\\wdheaven04.txt.txt"
		,"World\\WdHeaven05\\wdheaven05.txt.txt"
		,"World\\WdHeaven06\\wdheaven06.txt.txt"
		,"World\\WdHeaven06_1\\wdheaven06_1.txt.txt"
		, "honorList.txt.txt"
#ifdef __AZRIA_1023
		, "World\\WdCisland\\WdCisland.txt.txt"
#endif // __AZRIA_1023
		, "World\\DuOminous\\duominous.txt.txt"
		, "World\\DuOminous_1\\duominous_1.txt.txt"
		, "World\\WdGuildhousesmall\\WdGuildhousesmall.txt.txt"		// 소형 길드하우스
		, "World\\WdGuildhousemiddle\\WdGuildhousemiddle.txt.txt"	// 중형 길드하우스
		, "World\\WdGuildhouselarge\\WdGuildhouselarge.txt.txt"		// 대형 길드하우스
		, "World\\DuDreadfulCave\\DuDreadfulCave.txt.txt"			// 추가 인던(드래드풀 케이브)
		, "World\\DuRustia\\DuRustia.txt.txt"						// 추가 인던(러스티아 일반)
		, "World\\DuRustia_1\\DuRustia_1.txt.txt"					// 추가 인던(러스티아 마스터)
#ifdef __IMPROVE_MAP_SYSTEM
		, "propMapComboBoxData.txt.txt"
#endif // __IMPROVE_MAP_SYSTEM
	};

	for (const char * const fileName : lpszFiles) {
		CScript s;
		if (s.Load(fileName)) {
			s.LoadString();
		}
	}

	LoadText( "textClient.inc" );
}

void CProject::LoadDefines() {
	static constexpr std::initializer_list<const char *> lpszDefines = {
		"define.h",
		"defineNeuz.h",
		"defineQuest.h",
		"defineJob.h",
		"defineItem.h",
		"defineWorld.h",
		"defineItemkind.h",
		"lang.h",
		"defineObj.h",
		"defineAttribute.h",
		"defineSkill.h",
		"defineText.h",
		"defineSound.h",
		"resdata.h",
		"WndStyle.h"
		, "definelordskill.h"
		, "defineHonor.h"
#ifdef __IMPROVE_MAP_SYSTEM
		, "ContinentDef.h"
		, "defineMapComboBoxData.h"
#endif // __IMPROVE_MAP_SYSTEM
	};
	
	for (const char * const lpszDefine : lpszDefines) {
		CScript script;
		if (script.Load(lpszDefine)) {
			script.PreScan();
		}
	}	
}

void CProject::LoadPreFiles()
{
#ifdef __SECURITY_0628
#ifdef __CLIENT
	CResFile::LoadAuthFile();
#endif	// __CLIENT
#endif	// __SECURITY_0628

	LoadDefines();
	LoadStrings();
	
#ifdef __LANG_1013
	CLangMan::GetInstance()->Load( "propLang.txt" );
#endif	// __LANG_1013
}


#ifdef __RULE_0615
#include <optional>

bool CNameValider::Load() {
	constexpr auto LoadInvalidNames = []() -> std::optional<std::set<std::string>> {
		const CString strFilter = GetLangFileName(::GetLanguage(), FILE_INVALID);

		CScanner s;
		if (!s.Load(strFilter.GetString())) return std::nullopt;

		std::set<std::string> retval;

		s.GetToken();
		while (s.tok != FINISHED) {
			CString szName = s.Token;
			szName.MakeLower();
			retval.emplace(szName.GetString());
			s.GetToken();
		}

		return retval;
	};

	constexpr auto LoadValidLetters = [](bool isVendor) -> std::optional<std::set<char>> {
		const auto fileId = isVendor ? FILE_ALLOWED_LETTER2 : FILE_ALLOWED_LETTER;
		const CString strFile = GetLangFileName(::GetLanguage(), fileId);

		CScanner s;
		if (!s.Load(strFile)) return std::nullopt;

		std::set<char> retval;

		s.GetToken();
		while (s.tok != FINISHED) {
			if (s.Token.GetLength()) {
				retval.emplace(s.Token.GetAt(0));
			}
			s.GetToken();
		}

		return retval;
	};

	constexpr auto Arrayize = [](const std::optional<std::set<char>> & letters) {
		std::array<bool, 256> retval;

		if (!letters) {
			retval.fill(true);
		} else {
			retval.fill(false);

			for (const char letter : letters.value()) {
				retval[static_cast<unsigned char>(letter)] = true;
			}
		}

		return retval;
	};

	std::optional<std::set<std::string>> invalidNames = LoadInvalidNames();
	if (!invalidNames) {
		Error(__FUNCTION__ "() failed loading InvalidNames");
		return false;
	}

	std::optional<std::set<char>> allowedLetters1 = LoadValidLetters(false);
	std::optional<std::set<char>> allowedLetters2 = LoadValidLetters(true);

	m_invalidNames = std::vector(invalidNames->begin(), invalidNames->end());
	m_allowedLetters          = Arrayize(allowedLetters1);
	m_allowedLettersForVendor = Arrayize(allowedLetters2);

	return true;
}

bool CNameValider::IsNotAllowedName(LPCSTR name) const {
	return IsInvalidName(name) || !AllLettersAreIn(name, m_allowedLetters);
}

bool CNameValider::IsNotAllowedVendorName(LPCSTR name) const {
	return IsInvalidName(name) || !AllLettersAreIn(name, m_allowedLettersForVendor);
}

bool CNameValider::IsInvalidName(LPCSTR szName) const {
	CString str = szName;
	str.MakeLower();

	return std::ranges::any_of(m_invalidNames,
		[&](const std::string & invalidName) {
			return str.Find(invalidName.c_str()) != -1;
		});
}

bool CNameValider::AllLettersAreIn(LPCSTR name, const std::array<bool, 256> & allowed) {
	const char * c = name;
	while (*c != '\0') {
		if (!allowed[static_cast<unsigned char>(*c)]) {
			return false;
		}
		++c;
	}
	return true;
}

void CNameValider::Formalize(LPSTR szName) {
	const auto language = ::GetLanguage();
	if (language != LANG_GER && language != LANG_FRE) return;

	if (szName[0] == '\0') return;

	char buffer[2] = { szName[0], '\0' }; // Copy first letter
	_strupr(buffer);                      // Upper the first letter
	_strlwr(szName + 1);                  // Lower after first letter
	szName[0] = buffer[0];                // Copy back upper cased 1st letter
}
#endif	// __RULE_0615

CAr & operator<<(CAr & ar, const SHORTCUT & self) {
	ar << self.m_dwShortcut << self.m_dwId
		<< self.m_dwIndex << self.m_dwUserId << self.m_dwData;

	if (self.m_dwShortcut == ShortcutType::Chat) {
		ar.WriteString(self.m_szString);
	}

	return ar;
}

CAr & operator>>(CAr & ar, SHORTCUT & self) {
	ar >> self.m_dwShortcut >> self.m_dwId
		>> self.m_dwIndex >> self.m_dwUserId >> self.m_dwData;

	if (self.m_dwShortcut == ShortcutType::Chat) {
		ar.ReadString(self.m_szString);
	}

	return ar;
}


namespace ItemProps {
	bool IsOrichalcum(const CItemElem & item) {
		return item.m_dwItemId == II_GEN_MAT_ORICHALCUM01 || item.m_dwItemId == II_GEN_MAT_ORICHALCUM01_1;
	}

	bool IsOrichalcum(const ItemProp & item) {
		return item.dwID == II_GEN_MAT_ORICHALCUM01 || item.dwID == II_GEN_MAT_ORICHALCUM01_1;
	}

	bool IsMoonstone(const CItemElem & item) {
		return item.m_dwItemId == II_GEN_MAT_MOONSTONE || item.m_dwItemId == II_GEN_MAT_MOONSTONE_1;
	}

	bool IsMoonstone(const ItemProp & item) {
		return item.dwID == II_GEN_MAT_MOONSTONE || item.dwID == II_GEN_MAT_MOONSTONE_1;
	}

	unsigned int PiercingType::GetNumberOfPiercings() const {
		switch (m_value) {
			case V::LetterCard:
				return MAX_PIERCING_WEAPON;
			case V::NumericCard:
				return MAX_PIERCING_SUIT;
			case V::Vis:
				return MAX_VIS;
			case V::None:
			default:
				return 0;
		}
	}

#ifdef __CLIENT
	CTexture * GetItemIconTexture(const DWORD itemId) {
		const ItemProp * itemProp = prj.GetItemProp(itemId);
		return itemProp ? itemProp->GetTexture() : nullptr;
	}
#endif
}

#ifdef __CLIENT
CTexture * ItemProp::GetTexture() const {
	return CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, szIcon), 0xffff00ff);
}
#endif
