#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"

#include "party.h"
#include "guild.h"
#include "guildwar.h"

extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];

	#include "honor.h"

#include "eveschool.h"

extern	CProject			prj;

void CDbManager::SavePlayer( CQuery *qry, CQuery* pQueryLog, CMover* pMover, char* szQuery )
{
	szQuery[0] = '\0';

	///////// actMover
	char cActMover[50] = {0,};
	sprintf(cActMover, "%d,%d", pMover->m_pActMover->m_dwState, pMover->m_pActMover->m_dwStateFlag);

	///////// Skill
	char JobSkill[1024] = {0,};
	char LicenseSkill[1024] = {0,};			
	
	///////// JobLv
	char JobLv[500] = {0,};
	SaveJobLv( pMover, JobLv );
	
	///////// Quest
	char QuestCnt[3072]	= { 0, };
	char m_aCompleteQuest[1024] = { 0, };
	char CheckedQuest[100] = {0, };
	SaveQuest( pMover, QuestCnt, m_aCompleteQuest, CheckedQuest );

	///////// Inventory
	ItemContainerStruct	icsInventory, icsBank[3];
	SaveInventory( pMover, &icsInventory );
	for( int k = 0 ; k < 3 ; ++k )
		SaveBank( pMover, &pMover->m_Bank[k], &icsBank[k] );

	///////// Equipment
	char Equipment[500] = {0,};
	SaveEquipment( pMover, Equipment );

	///////// CardCube	
	char Card[2]	= "$";
	char sCardIndex[2]	= "$";
	char sCardObjIndex[2]	= "$";
	char Cube[2]	= "$";
	char sCubeIndex[2]	= "$";
	char sCubeObjIndex[2]	= "$";
	
	///////// TaskBar
	char AppletTaskBar[MAX_APPLETTASKBAR] = { 0, }; 
	char ItemTaskBar[MAX_ITEMTASKBAR] = { 0, };  
	char SkillTaskBar[1024] = { 0, };
	SaveTaskBar( pMover, AppletTaskBar, ItemTaskBar, SkillTaskBar );

	///////// SM 상용화 아이템
	char szSMTime[3072] = { 0, };
	SaveSMCode( pMover, szSMTime );

	char szSkillInfluence[7500] = { 0, };
	SaveSkillInfluence( pMover, szSkillInfluence );

	PutExpLog( pQueryLog, pMover, ( pMover->m_dwPeriodicTick == 0 ? 'O': 'P' ) );

	//	mulcom	100218	유럽 페냐 관련 CHARACTER_STR U1 변경 ( U1 -> C1 )
	sprintf( szQuery, "{call CHARACTER_STR('U1','%07d','%02d','',"
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 1
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 2
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 3
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 4
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 5
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 6
					  "?,?,?,?,?,?,?,?,?,?,?,"		// 7
//					  "?,?,?,?,?,?,?,?,?,?,?,"		// 8
					  "?,?,?,?,?,?,?,?,?,%7.0f,%7.0f,"		// 8
	#ifdef __EVENT_1101
//					  "?,?,?,?,?,?"
						"%7.0f,?,?,?,?,?"
	#else	// __EVENT_1101
					  "?,?,?"
	#endif	// __EVENT_1101
					  ",?"
					  ",?,?"
					  ",?,?,?"
#ifdef __EXP_ANGELEXP_LOG
					  ",?,?"
#endif // __EXP_ANGELEXP_LOG
#ifdef __EVENTLUA_COUPON
					  ",?"
#endif // __EVENTLUA_COUPON
					  ",?"
#ifdef __LAYER_1015
					  ",?"
#endif	// __LAYER_1015
					  ",?"
					  ",?,?"

//					  ")}", pMover->m_idPlayer, g_appInfo.dwSys );		// +3
					  ")}", pMover->m_idPlayer, g_appInfo.dwSys, pMover->m_vReturnPos.x, pMover->m_vReturnPos.y, pMover->m_vReturnPos.z );		// +3

	char szAuthority[2] = "F";
	int nSlot = 0;
	int nSex = pMover->GetSex();
	int nExpertLv = 0;
	int nRemainLP = 0;
	int nTotalPlayTime = 0;
	DWORD dwGold = pMover->GetGold();
	int i=0;
	int j=-1;
	__int64 nExp2 = 0;
	__int64 nSkillExp = 0;
	DWORD dwSkillPoint = 0;

//////////////////////////////////////////////////////////////////////////////////////
int MAX_SAVEPARAM = 88;
	MAX_SAVEPARAM += 3;
#ifdef __EVENT_1101
		MAX_SAVEPARAM += 3;
#endif // __EVENT_1101
	MAX_SAVEPARAM += 1;
	MAX_SAVEPARAM += 2;

	MAX_SAVEPARAM += 3;

#ifdef __EXP_ANGELEXP_LOG
	MAX_SAVEPARAM += 2;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	MAX_SAVEPARAM += 1;
#endif // __EVENTLUA_COUPON

	MAX_SAVEPARAM += 1;
#ifdef __LAYER_1015
	MAX_SAVEPARAM	+= 1;	// m_nLayer
#endif	// __LAYER_1015
	MAX_SAVEPARAM	+= 1;	// m_aCheckedQuest
	MAX_SAVEPARAM	+= 2;	// m_nCampusPoint, m_idCampus

//////////////////////////////////////////////////////////////////////////////////////
	
	int nNumSkill = 0;
	int nSlaughter = 0;
	BOOL* bOK = new BOOL[MAX_SAVEPARAM];
//	BOOL bOK[MAX_SAVEPARAM];
//	qry->StartLogBindedParameters();
	bOK[++j] = qry->BindParameter( ++i, pMover->m_szName, 32 );
	bOK[++j] = qry->BindParameter( ++i, &nSlot );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwWorldID );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwIndex );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vPos.x );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vPos.y );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vPos.z );
	bOK[++j] = qry->BindParameter( ++i, pMover->m_szCharacterKey, 32 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwSkinSet );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwHairMesh );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwHairColor );
	// 1
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwHeadMesh );
	bOK[++j] = qry->BindParameter( ++i, &nSex );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vScale.x );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwMotion );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_fAngle );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nHitPoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nManaPoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFatiguePoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwRideItemIdx );
	bOK[++j] = qry->BindParameter( ++i, &dwGold );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nJob );
	// 2
	bOK[++j] = qry->BindParameter( ++i, cActMover, 50 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nStr );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nSta );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nDex );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nInt );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nLevel );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nExp1 );
	bOK[++j] = qry->BindParameter( ++i, &nExp2 );
	bOK[++j] = qry->BindParameter( ++i, JobSkill, 500 );
	bOK[++j] = qry->BindParameter( ++i, LicenseSkill, 500 );
	bOK[++j] = qry->BindParameter( ++i, JobLv, 500 );
	// 3
	bOK[++j] = qry->BindParameter( ++i, &nExpertLv );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_idMarkingWorld );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vMarkingPos.x );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vMarkingPos.y );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vMarkingPos.z );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nRemainGP );
	bOK[++j] = qry->BindParameter( ++i, &nRemainLP );
	int nTutorialState	= pMover->GetTutorialState();
	bOK[++j] = qry->BindParameter( ++i, &nTutorialState );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFxp );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFxp );
	bOK[++j] = qry->BindParameter( ++i, QuestCnt, 3072 );
	// 4
	bOK[++j] = qry->BindParameter<SQL_CHAR>( ++i, szAuthority, 1 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwMode );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_idparty );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_idMurderer );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFame );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nDeathExp );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nDeathLevel );
#ifdef __JEFF_9_20
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwMute );
#else	// __JEFF_9_20
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwFlyTime );
#endif	// __JEFF_9_20
	long dwState	= static_cast<long>(pMover->m_RTMessenger.GetState());
	bOK[++j] = qry->BindParameter( ++i, &dwState );
	bOK[++j] = qry->BindParameter( ++i, &nTotalPlayTime );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nPKValue );
	// 5
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwPKPropensity );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwPKExp );
	bOK[++j] = qry->BindParameter( ++i, Card, 1980 );
	bOK[++j] = qry->BindParameter( ++i, sCardIndex, 215 );
	bOK[++j] = qry->BindParameter( ++i, sCardObjIndex, 215 );
	bOK[++j] = qry->BindParameter( ++i, Cube, 1980 );
	bOK[++j] = qry->BindParameter( ++i, sCubeIndex, 215 );
	bOK[++j] = qry->BindParameter( ++i, sCubeObjIndex, 215 );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szItem, 6940 );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szIndex, 345  );
	bOK[++j] = qry->BindParameter( ++i, Equipment, 135  );
	// 6
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szObjIndex, 345  );
	bOK[++j] = qry->BindParameter( ++i, AppletTaskBar, 3100 );
	bOK[++j] = qry->BindParameter( ++i, ItemTaskBar, 6885 );
	bOK[++j] = qry->BindParameter( ++i, SkillTaskBar, 225 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_UserTaskBar.m_nActionPoint );
	bOK[++j] = qry->BindParameter( ++i, icsBank[pMover->m_nSlot].szItem, 4290 );
	bOK[++j] = qry->BindParameter( ++i, icsBank[pMover->m_nSlot].szIndex, 215 );
	bOK[++j] = qry->BindParameter( ++i, icsBank[pMover->m_nSlot].szObjIndex, 215 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwGoldBank[pMover->m_nSlot] );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFuel );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_tmAccFuel );
	// 7
	bOK[++j] = qry->BindParameter( ++i, szSMTime, 2560);
	bOK[++j] = qry->BindParameter( ++i, szSkillInfluence, 7500);
	bOK[++j] = qry->BindParameter( ++i, &dwSkillPoint );
	bOK[++j] = qry->BindParameter( ++i, m_aCompleteQuest, 1024 );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szExt, 2000 );
	bOK[++j] = qry->BindParameter( ++i, icsBank[pMover->m_nSlot].szExt, 2000 );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szPiercing, 7800 );
	bOK[++j] = qry->BindParameter( ++i, icsBank[pMover->m_nSlot].szPiercing, 7800 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwReturnWorldID );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nSkillPoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nSkillLevel );
	bOK[++j] = qry->BindParameter( ++i, &nSkillExp );
#ifdef __EVENT_1101
	#ifdef __EVENT_1101_2
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nEventFlag );
	#else // __EVENT_1101_2
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwEventFlag );
	#endif // __EVENT_1101_2
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwEventTime );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwEventElapsed );
#endif	// __EVENT_1101
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nAngelExp );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nAngelLevel );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szPet, 2000 );
	bOK[++j] = qry->BindParameter( ++i, icsBank[pMover->m_nSlot].szPet, 2000 );
	DWORD dwPetId	= pMover->GetPetId();
	bOK[++j] = qry->BindParameter( ++i, &dwPetId );
#ifdef __EXP_ANGELEXP_LOG
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nExpLog );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nAngelExpLog );
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nCoupon );
#endif // __EVENTLUA_COUPON
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nHonor );
#ifdef __LAYER_1015
	int nLayer	= pMover->GetLayer();
	bOK[++j]	= qry->BindParameter( ++i, &nLayer );
#endif	// __LAYER_1015
	bOK[++j] = qry->BindParameter( ++i, CheckedQuest, 100 );
	int nCampusPoint	= pMover->GetCampusPoint();
	bOK[++j]	= qry->BindParameter( ++i, &nCampusPoint );
	u_long idCampus		= pMover->GetCampusId();
	bOK[++j]	= qry->BindParameter( ++i, &idCampus );
	for( i=0; i<j; ++i )
	{
		if( !bOK[i] )
		{
			char szBuffer[128];
			sprintf( szBuffer, "SavePlayer(%s) - %dth bindparam error - %d", pMover->m_szName, i+1, ::GetCurrentThreadId() );
		#ifdef _DEBUG
			AfxMessageBox( szBuffer );
		#endif
			WriteLog( szBuffer );

			// chipi_바인딩 실패시 DB Reconnect
			WriteLog( "try DB Reconnect... - ThreadId : %d", ::GetCurrentThreadId() );
			qry->DisConnect();
			if( qry->Connect( 3, qry->DBCredentials.Name, qry->DBCredentials.Id, qry->DBCredentials.Pass ) )
				WriteLog( "DB Reconnect Success! - ThreadId : %d", ::GetCurrentThreadId() );
			else
				WriteLog( "DB Reconnect Failed! - ThreadId : %d", ::GetCurrentThreadId() );
			qry->Clear();
			// chipi_바인딩 실패시 DB Reconnect

			SAFE_DELETE_ARRAY( bOK );
			return;
		}
	}

	if( qry->Exec( szQuery ) == FALSE )
	{
		WriteLog( "SavePlayer(%s) - Exec RETURN FALSE, ThreadID : %d", pMover->m_szName, ::GetCurrentThreadId() );
		SAFE_DELETE_ARRAY( bOK );
		return;
	}

	if( qry->Fetch() )
	{
		int nError = qry->GetInt( "fError" );
		if( nError != 1 )
		{
			WriteLog( "SavePlayer(%s) - fError:%d", pMover->m_szName, nError );
			SAFE_DELETE_ARRAY( bOK );
			return;
		}
	}


	SaveHonor( qry, pMover->m_idPlayer, pMover->m_aHonorTitle, szQuery );

#ifndef __S_NEW_SKILL_2
	SaveSkill( qry, pMover->m_idPlayer, pMover->m_jobSkills, szQuery );
#endif // __S_NEW_SKILL_2

	// 다른 캐릭터의 bank 저장
	for( i = 0 ; i < 3 ; ++i )
	{
		if( pMover->m_idPlayerBank[i] != 0 && i != pMover->m_nSlot )
		{
			ADDBANK_QUERYINFO info( "U1" );
			info.idPlayer			= pMover->m_idPlayerBank[i];
			info.dwGoldBank			= pMover->m_dwGoldBank[i];
			info.pszBank			= icsBank[i].szItem;
			info.pszBankIndex		= icsBank[i].szIndex;
			info.pszObjIndexBank	= icsBank[i].szObjIndex;
			info.pszExtBank			= icsBank[i].szExt;
			info.pszPirecingBank	= icsBank[i].szPiercing;
			info.pszBankPet		= icsBank[i].szPet;

			DBQryAddBankSave( szQuery, info );
			
			{
				qry->BindParameter( 1, (char*)info.pszBank, 4290 );
				qry->BindParameter( 2, (char*)info.pszBankIndex, 215 );
				qry->BindParameter( 3, (char*)info.pszObjIndexBank, 215 );
				qry->BindParameter( 4, &info.dwGoldBank );
				qry->BindParameter( 5, (char*)info.pszExtBank, 2000 );
				qry->BindParameter( 6, (char*)info.pszPirecingBank, 7800 );
				qry->BindParameter( 7, (char*)info.pszBankPet, 2689 );
			}

			if( FALSE == qry->Exec( szQuery ) )
			{
				SAFE_DELETE_ARRAY( bOK );
				return;
			}
		}
	}
	PocketStruct	aPocket[3];
	SavePocket( pMover, aPocket );
	for( i = 0; i < MAX_POCKET; i++ )
	{
		PocketParam	p;
		p.idPlayer	= pMover->m_idPlayer;
		p.pszItem	= aPocket[i].szItem;
		p.pszIndex	= aPocket[i].szIndex;
		p.pszObjIndex	= aPocket[i].szObjIndex;
		p.pszExt	= aPocket[i].szExt;
		p.pszPiercing	= aPocket[i].szPiercing;
		p.pszPet	= aPocket[i].szPet;
		p.bExpired	= aPocket[i].bExpired;
		p.tExpirationDate	= aPocket[i].tExpirationDate;

		MakeQueryPocket( szQuery, p );

		qry->BindParameter( 1, &i );
		qry->BindParameter( 2, (char*)p.pszItem    , 4290);
		qry->BindParameter( 3, (char*)p.pszIndex   , 215);
		qry->BindParameter( 4, (char*)p.pszObjIndex, 215);
		qry->BindParameter( 5, (char*)p.pszExt     , 2000);
		qry->BindParameter( 6, (char*)p.pszPiercing, 7800);
		qry->BindParameter( 7, (char*)p.pszPet     , 2689);
		qry->BindParameter( 8, &p.bExpired );
		qry->BindParameter( 9, &p.tExpirationDate );

		if( !qry->Exec( szQuery ) )
		{
			SAFE_DELETE_ARRAY( bOK );
			return;
		}
	}

	SAFE_DELETE_ARRAY( bOK );
}

void	CDbManager::SaveHonor( CQuery *qry, u_long uidPlayer, int * aHonor, char* szQuery )
{
	int	aHonorEtc[50] = {0,};
	int	aHonorItem[50] = {0,};
	int	aHonorMonster[50] = {0,};

    int nEtc = 0,nItem = 0,nMonster = 0;
	int nType = 0;
	int nCurrentTitleCount =  CTitleManager::Instance()->m_nCurrentTitleCount;
	for(int i=0;i<nCurrentTitleCount;i++)
	{
		nType = CTitleManager::Instance()->GetIdxType(i);
		if( nType == HI_HUNT_MONSTER)
		{
			aHonorMonster[nMonster] = aHonor[i];
			nMonster++;
		}
		else if( nType == HI_USE_ITEM)
		{
			aHonorItem[nItem] = aHonor[i];
			nItem++;
		}
		else
		{
			aHonorEtc[nEtc] = aHonor[i];
			nEtc++;
		}
	}

	sprintf( szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 1,
		aHonorEtc[0],aHonorEtc[1],aHonorEtc[2],aHonorEtc[3],aHonorEtc[4],aHonorEtc[5],aHonorEtc[6],aHonorEtc[7],aHonorEtc[8],aHonorEtc[9],
		aHonorEtc[10],aHonorEtc[11],aHonorEtc[12],aHonorEtc[13],aHonorEtc[14],aHonorEtc[15],aHonorEtc[16],aHonorEtc[17],aHonorEtc[18],aHonorEtc[19],
		aHonorEtc[20],aHonorEtc[21],aHonorEtc[22],aHonorEtc[23],aHonorEtc[24],aHonorEtc[25],aHonorEtc[26],aHonorEtc[27],aHonorEtc[28],aHonorEtc[29],
		aHonorEtc[30],aHonorEtc[31],aHonorEtc[32],aHonorEtc[33],aHonorEtc[34],aHonorEtc[35],aHonorEtc[36],aHonorEtc[37],aHonorEtc[38],aHonorEtc[39],
		aHonorEtc[40],aHonorEtc[41],aHonorEtc[42],aHonorEtc[43],aHonorEtc[44],aHonorEtc[45],aHonorEtc[46],aHonorEtc[47],aHonorEtc[48],aHonorEtc[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}

	sprintf( szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 2,
		aHonorItem[0],aHonorItem[1],aHonorItem[2],aHonorItem[3],aHonorItem[4],aHonorItem[5],aHonorItem[6],aHonorItem[7],aHonorItem[8],aHonorItem[9],
		aHonorItem[10],aHonorItem[11],aHonorItem[12],aHonorItem[13],aHonorItem[14],aHonorItem[15],aHonorItem[16],aHonorItem[17],aHonorItem[18],aHonorItem[19],
		aHonorItem[20],aHonorItem[21],aHonorItem[22],aHonorItem[23],aHonorItem[24],aHonorItem[25],aHonorItem[26],aHonorItem[27],aHonorItem[28],aHonorItem[29],
		aHonorItem[30],aHonorItem[31],aHonorItem[32],aHonorItem[33],aHonorItem[34],aHonorItem[35],aHonorItem[36],aHonorItem[37],aHonorItem[38],aHonorItem[39],
		aHonorItem[40],aHonorItem[41],aHonorItem[42],aHonorItem[43],aHonorItem[44],aHonorItem[45],aHonorItem[46],aHonorItem[47],aHonorItem[48],aHonorItem[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}

	sprintf( szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 3,
		aHonorMonster[0],aHonorMonster[1],aHonorMonster[2],aHonorMonster[3],aHonorMonster[4],aHonorMonster[5],aHonorMonster[6],aHonorMonster[7],aHonorMonster[8],aHonorMonster[9],
		aHonorMonster[10],aHonorMonster[11],aHonorMonster[12],aHonorMonster[13],aHonorMonster[14],aHonorMonster[15],aHonorMonster[16],aHonorMonster[17],aHonorMonster[18],aHonorMonster[19],
		aHonorMonster[20],aHonorMonster[21],aHonorMonster[22],aHonorMonster[23],aHonorMonster[24],aHonorMonster[25],aHonorMonster[26],aHonorMonster[27],aHonorMonster[28],aHonorMonster[29],
		aHonorMonster[30],aHonorMonster[31],aHonorMonster[32],aHonorMonster[33],aHonorMonster[34],aHonorMonster[35],aHonorMonster[36],aHonorMonster[37],aHonorMonster[38],aHonorMonster[39],
		aHonorMonster[40],aHonorMonster[41],aHonorMonster[42],aHonorMonster[43],aHonorMonster[44],aHonorMonster[45],aHonorMonster[46],aHonorMonster[47],aHonorMonster[48],aHonorMonster[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}


}


void CDbManager::SaveSkill(CQuery * qry, u_long uidPlayer, const MoverSkills & skills, char * szQuery) {
	sprintf(szQuery, "uspLearnSkill @serverindex='%02d',@pPlayerID='%07d',@pSkill='",
		g_appInfo.dwSys, uidPlayer
	);

	for (const SKILL & skill : skills) {
		char buffer[128] = "";
		std::sprintf(buffer, "%lu,%lu/", skill.dwSkill, skill.dwLevel);
		std::strcat(szQuery, buffer);
	}
	std::strcat(szQuery, "$'");

	if (!qry->Exec(szQuery)) {
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}
}

// raiders_todo 의미 없음 
void CDbManager::SaveJobLv( CMover* pMover, char* szJobLv )
{
	char OneJobLv[16] = {0,};
	for( int i = 0 ; i < MAX_JOB ; i++)
	{
		sprintf(OneJobLv, "%d/", 0);
		strncat(szJobLv, OneJobLv, sizeof(OneJobLv));
	}
	strncat(szJobLv, NullStr, sizeof(NullStr));
}

void CDbManager::SaveQuest( CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest, char* szCheckedQuest )
{
	if (!pMover->m_quests) {
		std::strcpy(szQuestCnt, NullStr);
		std::strcpy(szm_aCompleteQuest, NullStr);
		std::strcpy(szCheckedQuest, NullStr);
		return;
	}

	constexpr auto SaveQuest_ = [](
		const MoverSub::Quests & quests,
		char * szQuestCnt, char * szm_aCompleteQuest, char * szCheckedQuest
		) {

		char buffer[128];

		szQuestCnt[0] = '\0';
		for (const QUEST & quest : quests.current) {
			sprintf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
				quest.m_wId, quest.m_nState, quest.m_wTime,
				quest.m_nKillNPCNum[0], quest.m_nKillNPCNum[1],
				quest.m_bPatrol  , quest.m_bReserve2, quest.m_bReserve3, quest.m_bReserve4,
				quest.m_bReserve5, quest.m_bReserve6, quest.m_bReserve7, quest.m_bReserve8
			);
			strncat(szQuestCnt, buffer, sizeof(buffer));
		}
		strcat(szQuestCnt, NullStr);

		szm_aCompleteQuest[0] = '\0';
		for (const auto questId : quests.completed) {
			sprintf(buffer, "%d/", questId);
			strncat(szm_aCompleteQuest, buffer, sizeof(buffer));
		}
		strcat(szm_aCompleteQuest, NullStr);

		szCheckedQuest[0] = '\0';
		for (const auto questId : quests.checked) {
			sprintf(buffer, "%d/", questId);
			strncat(szCheckedQuest, buffer, sizeof(buffer));
		}
		strcat(szCheckedQuest, NullStr);
	};

	SaveQuest_(*pMover->m_quests, szQuestCnt, szm_aCompleteQuest, szCheckedQuest);
}

void	CDbManager::SavePocket( CMover* pMover, PPocketStruct pPocketStruct )
{
	for( int nPocket = 0; nPocket < MAX_POCKET; nPocket++ )
	{
		ItemStruct is;
		BOOL	bExt	= FALSE;
		BOOL	bPiercing	= FALSE;
		BOOL	bPet	= FALSE;
		char sIndex[10]		= { 0,};
		char sObjIndex[16]		= { 0,};
		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
		if( pPocket )
		{
			for( DWORD i = 0; i < pPocket->m_dwItemMax; i++ )
			{
				SaveOneItem( &pPocket->m_apItem[i], &is );
				strncat( pPocketStruct[nPocket].szItem, is.szItem, sizeof(is.szItem) );
				strncat( pPocketStruct[nPocket].szExt, is.szExt, sizeof(is.szExt) );
				strncat( pPocketStruct[nPocket].szPiercing, is.szPiercing, sizeof(is.szPiercing) );
				sprintf( sIndex, "%d/", pPocket->m_apIndex[i] );
				strncat( pPocketStruct[nPocket].szIndex, sIndex, sizeof(sIndex) );
				sprintf( sObjIndex, "%d/", pPocket->m_apItem[i].m_dwObjIndex );
				strcat( pPocketStruct[nPocket].szObjIndex, sObjIndex );
				strncat( pPocketStruct[nPocket].szPet, is.szPet, sizeof(is.szPet) );
				if( 0 < pPocket->m_apItem[i].m_dwKeepTime || 0 != pPocket->m_apItem[i].GetRandomOptItemId() )
					bExt	= TRUE;
				if( pPocket->m_apItem[i].IsPiercedItem() )
					bPiercing	= TRUE;
				if( pPocket->m_apItem[i].m_pPet )
					bPet	= TRUE;
			}
			strncat( pPocketStruct[nPocket].szItem, NullStr, sizeof(NullStr) );
			strncat( pPocketStruct[nPocket].szIndex, NullStr, sizeof(NullStr) );
			strcat( pPocketStruct[nPocket].szObjIndex, NullStr );
			if( bExt == FALSE )
				*pPocketStruct[nPocket].szExt	= '\0';
			if( bPiercing == FALSE )
				*pPocketStruct[nPocket].szPiercing	= '\0';
			if( bPet == FALSE )
				*pPocketStruct[nPocket].szPet	= '\0';
			strcat( pPocketStruct[nPocket].szExt, NullStr );
			strcat( pPocketStruct[nPocket].szPiercing, NullStr );
			strcat( pPocketStruct[nPocket].szPet, NullStr );
			pPocketStruct[nPocket].bExpired		= pPocket->IsExpired();
			pPocketStruct[nPocket].tExpirationDate	= pPocket->GetExpirationDate();
		}
		else
		{
			lstrcpy( pPocketStruct[nPocket].szItem, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szIndex, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szObjIndex, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szExt, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szPiercing, NullStr ); 
			lstrcpy( pPocketStruct[nPocket].szPet, NullStr ); 
			pPocketStruct[nPocket].bExpired		= TRUE;
			pPocketStruct[nPocket].tExpirationDate	= 0;
		}
	}
}

void CDbManager::SaveInventory( CMover* pMover, PItemContainerStruct pItemContainerStruct )
{
	ItemStruct	is;
	char Onem_apIndex[10]		= {0,};
	char sPerObjIndex[16]		= {0,};
	BOOL bExtInven = FALSE;
	BOOL bPirecingInven = FALSE;

	BOOL	bPet	= FALSE;

	for( DWORD ch = 0; ch < pMover->m_Inventory.m_dwItemMax; ch++ )	// 0-504
	{
		SaveOneItem( &pMover->m_Inventory.m_apItem[ch], &is );
		strncat( pItemContainerStruct->szItem, is.szItem, sizeof(is.szItem) );
		strncat( pItemContainerStruct->szExt, is.szExt, sizeof(is.szExt) );
		strncat( pItemContainerStruct->szPiercing, is.szPiercing, sizeof(is.szPiercing) );
		sprintf( Onem_apIndex, "%d/", pMover->m_Inventory.m_apIndex[ch] );
		strncat( pItemContainerStruct->szIndex, Onem_apIndex, sizeof(Onem_apIndex) );
		sprintf( sPerObjIndex, "%d/", pMover->m_Inventory.m_apItem[ch].m_dwObjIndex );
		strcat( pItemContainerStruct->szObjIndex, sPerObjIndex );
		strncat( pItemContainerStruct->szPet, is.szPet, sizeof(is.szPet) );

		if( 0 < pMover->m_Inventory.m_apItem[ch].m_dwKeepTime || 0 != pMover->m_Inventory.m_apItem[ch].GetRandomOptItemId() )
			bExtInven = TRUE;
		if( pMover->m_Inventory.m_apItem[ch].IsPiercedItem() )
			bPirecingInven = TRUE;
		if( pMover->m_Inventory.m_apItem[ch].m_pPet )
			bPet	= TRUE;
	}

	strncat( pItemContainerStruct->szItem, NullStr, sizeof(NullStr) );
	strncat( pItemContainerStruct->szIndex, NullStr, sizeof(NullStr) );
	strcat( pItemContainerStruct->szObjIndex, NullStr );
	if( bExtInven == FALSE )
		*pItemContainerStruct->szExt	= '\0';
	if( bPirecingInven == FALSE )
		*pItemContainerStruct->szPiercing	= '\0';
	if( bPet == FALSE )
		*pItemContainerStruct->szPet	= '\0';
	strcat( pItemContainerStruct->szExt, NullStr );
	strcat( pItemContainerStruct->szPiercing, NullStr );
	strcat( pItemContainerStruct->szPet, NullStr );
}

void CDbManager::SaveEquipment( CMover* pMover, char* szEquipment )
{
	char OneEquipment[16] = {0,};
	for( int i = 0 ; i < MAX_HUMAN_PARTS ; i++)
	{
		sprintf( OneEquipment, "%d/", pMover->m_aEquipInfo[i].byFlag );
		strncat(szEquipment, OneEquipment, sizeof(OneEquipment));
	}
	strncat(szEquipment, NullStr, sizeof(NullStr));
}

void CDbManager::SaveBank( CMover* pMover, CItemContainer* pPlayerBank, ItemContainerStruct * pItemContainerStruct )
{
	ItemStruct	is;
	char Onem_apIndex[10] = {0,};
	char sPerObjIndex[16]	= { 0, };
	BOOL bExtBank = FALSE;
	BOOL bPirecingBank = FALSE;

	BOOL	bPet	= FALSE;

	for( DWORD ch = 0; ch < pPlayerBank->m_dwItemMax; ch++ )	// 0-504
	{	
		SaveOneItem( &pPlayerBank->m_apItem[ch], &is );
		strncat( pItemContainerStruct->szItem, is.szItem, sizeof(is.szItem) );
		strncat( pItemContainerStruct->szExt, is.szExt, sizeof(is.szExt) );
		strncat( pItemContainerStruct->szPiercing, is.szPiercing, sizeof(is.szPiercing) );
		sprintf( Onem_apIndex, "%d/", pPlayerBank->m_apIndex[ch]);
		strncat( pItemContainerStruct->szIndex, Onem_apIndex, sizeof(Onem_apIndex) );
		sprintf( sPerObjIndex, "%d/", pPlayerBank->m_apItem[ch].m_dwObjIndex );
		strcat( pItemContainerStruct->szObjIndex, sPerObjIndex );
		strncat( pItemContainerStruct->szPet, is.szPet, sizeof(is.szPet) );
		if( 0 < pPlayerBank->m_apItem[ch].m_dwKeepTime || 0 != pPlayerBank->m_apItem[ch].GetRandomOptItemId() )
			bExtBank = TRUE;
		if( pPlayerBank->m_apItem[ch].IsPiercedItem() )
			bPirecingBank = TRUE;
		if( pPlayerBank->m_apItem[ch].m_pPet )
			bPet	= TRUE;
	}
	strncat( pItemContainerStruct->szItem, NullStr, sizeof(NullStr) );
	strncat( pItemContainerStruct->szIndex, NullStr, sizeof(NullStr) );
	strcat( pItemContainerStruct->szObjIndex, NullStr );
	if( bExtBank == FALSE )
		*pItemContainerStruct->szExt	= '\0';
	if( bPirecingBank == FALSE )
		*pItemContainerStruct->szPiercing	= '\0';
	if( bPet == FALSE )
		*pItemContainerStruct->szPet	= '\0';
	strcat( pItemContainerStruct->szExt, NullStr );
	strcat( pItemContainerStruct->szPiercing, NullStr );
	strcat( pItemContainerStruct->szPet, NullStr );
}

void CDbManager::SaveGuildBank( CItemContainer*  pGuildBank, PItemContainerStruct pItemContainerStruct )
{
	ItemStruct is;
	char Onem_apIndex[10] = {0,};
	char sPerObjIndex[16]	= { 0, };
	BOOL bExtBank = FALSE;
	BOOL bPirecingBank = FALSE;
	BOOL bPet	= FALSE;

	for( DWORD ch = 0; ch < pGuildBank->m_dwItemMax; ch++ )	// 0-504
	{	
		SaveOneItem( &pGuildBank->m_apItem[ch], &is );
		strncat( pItemContainerStruct->szItem, is.szItem, sizeof(is.szItem) );
		strncat( pItemContainerStruct->szExt, is.szExt, sizeof(is.szExt) );
		strncat( pItemContainerStruct->szPiercing, is.szPiercing, sizeof(is.szPiercing) );
		sprintf( Onem_apIndex, "%d/", pGuildBank->m_apIndex[ch]);
		strncat( pItemContainerStruct->szIndex, Onem_apIndex, sizeof(Onem_apIndex) );
		sprintf( sPerObjIndex, "%d/", pGuildBank->m_apItem[ch].m_dwObjIndex );
		strcat( pItemContainerStruct->szObjIndex, sPerObjIndex );
		strncat( pItemContainerStruct->szPet, is.szPet, sizeof(is.szPet) );

		if( 0 < pGuildBank->m_apItem[ch].m_dwKeepTime || 0 != pGuildBank->m_apItem[ch].GetRandomOptItemId() )
			bExtBank = TRUE;
		if( pGuildBank->m_apItem[ch].IsPiercedItem() )
			bPirecingBank = TRUE;
		if( pGuildBank->m_apItem[ch].m_pPet )
			bPet	= TRUE;
	}
	strncat( pItemContainerStruct->szItem, NullStr, sizeof(NullStr) );
	strncat( pItemContainerStruct->szIndex, NullStr, sizeof(NullStr));
	strcat( pItemContainerStruct->szObjIndex, NullStr );
	if( bExtBank == FALSE )
		*pItemContainerStruct->szExt	= '\0';
	if( bPirecingBank == FALSE )
		*pItemContainerStruct->szPiercing	= '\0';
	strcat( pItemContainerStruct->szExt, NullStr );
	strcat( pItemContainerStruct->szPiercing, NullStr );
	strcat( pItemContainerStruct->szPet, NullStr );
}

void CDbManager::SaveCardCube( CMover* pMover, char* szCard, char* szsCardIndex, char* szsCardObjIndex, char* szCube, char* szsCubeIndex, char* szsCubeObjIndex )
{
}

void CDbManager::SaveTaskBar(CMover * pMover, char * szAppletTaskBar, char * szItemTaskBar, char * szSkillTaskBar) {
	static constexpr auto AppendNumber = [](char * destination, int value) {
		char buffer[32] = "";
		std::sprintf(buffer, "%d,", value);
		std::strcat(destination, buffer);
	};
	
	static constexpr auto BufferShortcut = [](char * destination, const SHORTCUT & shortcut) {
		char buffer[MAX_TASKBAR] = "";

		sprintf(buffer, "%d,%d,%d,0,%d,%d",
			static_cast<int>(shortcut.m_dwShortcut), shortcut.m_dwId,
			shortcut.m_dwIndex, shortcut.m_dwUserId, shortcut.m_dwData);

		if (shortcut.m_dwShortcut == ShortcutType::Chat) {
			CDbManager::SetStrDBFormat(buffer, shortcut.m_szString);
		}

		strncat(destination, buffer, sizeof(buffer));
		strcat(destination, "/");
	};

	for (int ch = 0; ch < MAX_SLOT_APPLET; ch++) {
		const SHORTCUT & shortcut = pMover->m_UserTaskBar.m_aSlotApplet[ch];

		if (!shortcut.IsEmpty()) {
			AppendNumber(szAppletTaskBar, ch);
			BufferShortcut(szAppletTaskBar, shortcut);
		}
	}
	strcat( szAppletTaskBar, NullStr );
	
	for( int ch = 0; ch < MAX_SLOT_ITEM_COUNT; ch++ ) {
		for( int j = 0; j < MAX_SLOT_ITEM; j++ ) {
			const SHORTCUT & shortcut = pMover->m_UserTaskBar.m_aSlotItem[ch][j];

			if (!shortcut.IsEmpty()) {
				AppendNumber(szItemTaskBar, ch);
				AppendNumber(szItemTaskBar, j);
				BufferShortcut(szItemTaskBar, shortcut);
			}
		}
	}
	strcat( szItemTaskBar, NullStr );
	
	for (int ch = 0; ch < MAX_SLOT_QUEUE; ch++) {
		const SHORTCUT & shortcut = pMover->m_UserTaskBar.m_aSlotQueue[ch];

		if (!shortcut.IsEmpty()) {
			AppendNumber(szSkillTaskBar, ch);
			BufferShortcut(szSkillTaskBar, shortcut);
		}
	}
	strcat( szSkillTaskBar, NullStr );
}

void CDbManager::SaveSMCode( CMover* pMover, char* szszSMTime )
{
	BOOL bSMSave = FALSE;
	for( int ch = 0 ; ch < SM_MAX ; ++ch )
	{
		if( 0 < pMover->m_dwSMTime[ch] )
		{
			bSMSave = TRUE;
		}
		char OneSMTime[256] = { 0, };
		if( ch == SM_MAX_HP50 )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nPlusMaxHitPoint );
		}
		else
		if( ch == SM_RESIST_ATTACK_LEFT )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nAttackResistLeft );
		}
		else
		if( ch == SM_RESIST_ATTACK_RIGHT )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nAttackResistRight );
		}
		else
		if( ch == SM_RESIST_DEFENSE )
		{
			sprintf( OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nDefenseResist );
		}
		else
		{
			sprintf( OneSMTime, "%d,", pMover->m_dwSMTime[ch] );
		}
		strcat( szszSMTime, OneSMTime );
	}
	strcat( szszSMTime, NullStr );
	
	if( bSMSave == FALSE )
	{
		memset( szszSMTime, 0, sizeof( szszSMTime ) );
	}
}

void CDbManager::SaveSkillInfluence( CMover* pMover, char* szszSkillInfluence )
{
	pMover->m_buffs.ToString( szszSkillInfluence );
}

void CDbManager::SavePlayTime( CQuery *qry, CAr & arRead, const char * szPlayer)
{
	SAccountName szAccount;
	arRead >> szAccount;

	DWORD dwTime;
	u_long idPlayer;
	arRead >> idPlayer;
	arRead >> dwTime; // 시각
	dwTime /= 1000;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "U3", idPlayer, g_appInfo.dwSys, "", "", dwTime );
	
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s\r\n\t%s", __FILE__, __LINE__, szQuery, szPlayer );
	}
}

void CDbManager::DBQryAddBankSave( char* szSql, const ADDBANK_QUERYINFO & info )
{
		sprintf( szSql, "{call ADD_BANK_STR('U1','%07d','%02d', ?, ?, ?, ?, ?, ?, ?)}", info.idPlayer, g_appInfo.dwSys ); 
}

void CDbManager::MakeQueryPocket( char* szQuery, const PocketParam & p )
{
	sprintf( szQuery, "{call uspSavePocket( '%02d', '%07d', ?, ?, ?, ?, ?, ?, ?, ?, ?) }", g_appInfo.dwSys, p.idPlayer );
}

void CDbManager::SaveOneItem( CItemElem* pItemElem, PItemStruct pItemStruct )
{
	char szPiercing[32]		= {0,};

	if( pItemElem->IsEmpty() == FALSE )
	{
		if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
			sprintf( pItemStruct->szItem, "%d,%d,%d,%d,,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
					0, 0, 
					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
					0, pItemElem->m_byFlag,
					pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
  					pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
  					pItemElem->m_idGuild,
					pItemElem->m_nResistSMItemId
				);
		else
			sprintf( pItemStruct->szItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
					0, 0, pItemElem->m_szItemText,
					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
					0, pItemElem->m_byFlag,
					pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
  					pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
  					pItemElem->m_idGuild,
					pItemElem->m_nResistSMItemId
				);

		// ext
		sprintf( pItemStruct->szExt, "%d,%d,%I64d"
			",%d"
			"/", 0 /* charged */, pItemElem->m_dwKeepTime, pItemElem->GetRandomOptItemId()
			, static_cast<int>( pItemElem->m_bTranformVisPet )
			);

		// piercing
		ItemProp* itemProp = prj.GetItemProp( pItemElem->m_dwItemId );
		if( itemProp && itemProp->IsUltimate() )
		{
			sprintf( pItemStruct->szPiercing, "%d", pItemElem->GetUltimatePiercingSize() );
			for( int nPirecing = 0 ; nPirecing < pItemElem->GetUltimatePiercingSize(); ++nPirecing )
			{
				sprintf( szPiercing, ",%d", pItemElem->GetUltimatePiercingItem( nPirecing ) );
				strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
			}
			if( pItemElem->GetPiercingSize() > 0 )
			{
				sprintf( szPiercing, ",%d", pItemElem->GetPiercingSize() );
				strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
			}
		}
		else
			sprintf( pItemStruct->szPiercing, "%d", pItemElem->GetPiercingSize() );

		for( int nPirecing = 0 ; nPirecing < pItemElem->GetPiercingSize(); ++nPirecing )
		{
			sprintf( szPiercing, ",%d", pItemElem->GetPiercingItem( nPirecing ) );
			strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
		}
		if( pItemElem->IsVisPet() )
		{
			for( int nPirecing = 0; nPirecing < pItemElem->GetPiercingSize(); ++nPirecing )
			{
				sprintf( szPiercing, ",%d", pItemElem->GetVisKeepTime( nPirecing ) );
				strncat( pItemStruct->szPiercing, szPiercing, sizeof(szPiercing) );
			}
		}
		strcat( pItemStruct->szPiercing, "/" );

		// pet
		if( pItemElem->m_pPet )
		{
			sprintf( pItemStruct->szPet, "1,%d,%d,%d,%d,%d", 
				pItemElem->m_pPet->GetKind(), pItemElem->m_pPet->GetLevel(),
				pItemElem->m_pPet->GetExp(), pItemElem->m_pPet->GetEnergy(),
				pItemElem->m_pPet->GetLife() );
			for( int i = PL_D; i <= pItemElem->m_pPet->GetLevel(); i++ )
			{
				char szAvailLevel[16]	= { 0,};
				sprintf( szAvailLevel, ",%d", pItemElem->m_pPet->GetAvailLevel( i ) );
				strcat( pItemStruct->szPet, szAvailLevel );
			}
			char szTemp[MAX_PET_NAME_FMT+1]		= { 0,};
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			SetDBFormatStr( szFmt, MAX_PET_NAME_FMT, pItemElem->m_pPet->GetName() );
			sprintf( szTemp, ",%s", szFmt );
			strcat( pItemStruct->szPet, szTemp );
			strcat( pItemStruct->szPet, "/" );
		}
		else
		{
			sprintf( pItemStruct->szPet, "0/" );
		}
	}
	else
	{
		// item
		ZeroMemory( pItemStruct->szItem, sizeof(pItemStruct->szItem) );
		// ext
		sprintf( pItemStruct->szExt, "%d,%d,%d/", 0, 0, 0 );
		// piercing
		sprintf( pItemStruct->szPiercing, "%d/", 0 );
		// pet
		sprintf( pItemStruct->szPet, "0/" );
	}
}