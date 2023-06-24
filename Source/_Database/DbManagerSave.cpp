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
	ItemContainerStruct	icsInventory;
	SaveItemContainer( pMover->m_Inventory, icsInventory );

	///////// Equipment
	char Equipment[500] = {0,};
	SaveEquipment( pMover, Equipment );
		
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
	const char * characterStrSaveFormat =
		"{call CHARACTER_STR_SAVEPLAYER('%07d','%02d',"
	// 1 2 3 4 5 6 7 8 9 0 1 2
		"?,?,?,?,?,?,?,?,?,?,?,?,"  // 1: @im_szName -> @im_dwSex
		"?,?,?,?,?,?,?,?,?,?,"      // 2: @im_vScale_x -> @im_pActMover
		"?,?,?,?,?,?,?,"            // 3: @im_nStr -> @im_aJobLv
		"?,?,?,?,?,?,?,?,?,"        // 4: @im_idMarkingWorld -> @im_lpQuestCntArray
		"?,?,?,?,?,?,?,?,"          // 5: @im_dwMode -> @im_nMessengerState
		"?,?,?,?,?,?,?,?,?,?,"      // 6: @im_nPKValue -> @im_adwEquipment
		"?,?,?,?,?,?,?,?,?,"        // 7: @im_aSlotApplet -> @im_aCompleteQuest
		"?,%7.0f,%7.0f,%7.0f,"      // 8: @im_dwReturnWorldID -> @im_vReturnPos_z
		"?,?,?,?,?,?,?,?,?,?,?,"    // 9: @im_SkillPoint -> @im_nCoupon
		"?,?,?,?,?"                 // 10: @im_nHonor -> @im_idCampus
		")}";

	sprintf( szQuery, characterStrSaveFormat,
		pMover->m_idPlayer, g_appInfo.dwSys,
		pMover->m_vReturnPos.x, pMover->m_vReturnPos.y, pMover->m_vReturnPos.z
	);		// +3

	int nSex = pMover->GetSex();
	DWORD dwGold = pMover->GetGold();
	int i=0;
	int j=-1;

//////////////////////////////////////////////////////////////////////////////////////
	
	std::array<bool, 256> bOK;
	bOK.fill(false);

//	qry->StartLogBindedParameters();
	// 1
	bOK[++j] = qry->BindParameter( ++i, pMover->m_szName, 32 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwWorldID );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwIndex );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vPos.x );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vPos.y );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vPos.z );
	bOK[++j] = qry->BindParameter( ++i, pMover->m_szCharacterKey, 32 );
	DWORD skinSet = pMover->m_skin.skinSet;
	bOK[++j] = qry->BindParameter( ++i, &skinSet);
	DWORD hairMesh = pMover->m_skin.hairMesh;
	bOK[++j] = qry->BindParameter( ++i, &hairMesh);
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwHairColor );
	DWORD headMesh = pMover->m_skin.headMesh;
	bOK[++j] = qry->BindParameter( ++i, &headMesh);
	bOK[++j] = qry->BindParameter( ++i, &nSex );
	// 2
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vScale.x );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwMotion );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_fAngle );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nHitPoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nManaPoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFatiguePoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwRideItemIdx );
	bOK[++j] = qry->BindParameter( ++i, &dwGold );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nJob );
	bOK[++j] = qry->BindParameter( ++i, cActMover, 50 );
	// 3
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nStr );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nSta );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nDex );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nInt );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nLevel );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nExp1 );
	bOK[++j] = qry->BindParameter( ++i, JobLv, 500 );
	// 4
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_idMarkingWorld );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vMarkingPos.x );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vMarkingPos.y );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_vMarkingPos.z );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nRemainGP );

	int nTutorialState	= pMover->GetTutorialState();
	bOK[++j] = qry->BindParameter( ++i, &nTutorialState );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFxp );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFxp );
	bOK[++j] = qry->BindParameter( ++i, QuestCnt, 3072 );
	// 5
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwMode );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_idparty );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_idMurderer );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFame );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nDeathExp );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nDeathLevel );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwMute );

	long dwState	= static_cast<long>(pMover->m_RTMessenger.GetState());
	bOK[++j] = qry->BindParameter( ++i, &dwState );
	// 6
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nPKValue );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwPKPropensity );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwPKExp );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szItem, 6940 );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szIndex, 345  );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szObjIndex, 345  );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szExt, 2000 );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szPiercing, 7800 );
	bOK[++j] = qry->BindParameter( ++i, icsInventory.szPet, 2000 );
	bOK[++j] = qry->BindParameter( ++i, Equipment, 135  );
	// 7
	bOK[++j] = qry->BindParameter( ++i, AppletTaskBar, 3100 );
	bOK[++j] = qry->BindParameter( ++i, ItemTaskBar, 6885 );
	bOK[++j] = qry->BindParameter( ++i, SkillTaskBar, 225 );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_UserTaskBar.m_nActionPoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nFuel );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_tmAccFuel );
	bOK[++j] = qry->BindParameter( ++i, szSMTime, 2560);
	bOK[++j] = qry->BindParameter( ++i, szSkillInfluence, 7500);
	bOK[++j] = qry->BindParameter( ++i, m_aCompleteQuest, 1024 );
	// 8
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwReturnWorldID );
	// Return position x, y, and z directly in sprintf
	// 9
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nSkillPoint );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nSkillLevel );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nEventFlag );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwEventTime );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_dwEventElapsed );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nAngelExp );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nAngelLevel );
	DWORD dwPetId	= pMover->GetPetId();
	bOK[++j] = qry->BindParameter( ++i, &dwPetId );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nExpLog );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nAngelExpLog );
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nCoupon );

	// 10
	bOK[++j] = qry->BindParameter( ++i, &pMover->m_nHonor );

	int nLayer	= pMover->GetLayer();
	bOK[++j]	= qry->BindParameter( ++i, &nLayer );

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

			return;
		}
	}

	if( qry->Exec( szQuery ) == FALSE )
	{
		WriteLog( "SavePlayer(%s) - Exec RETURN FALSE, ThreadID : %d", pMover->m_szName, ::GetCurrentThreadId() );
		return;
	}

	if( qry->Fetch() )
	{
		int nError = qry->GetInt( "fError" );
		if( nError != 1 )
		{
			WriteLog( "SavePlayer(%s) - fError:%d", pMover->m_szName, nError );
			return;
		}
	}


	SaveHonor( qry, pMover->m_idPlayer, pMover->m_aHonorTitle, szQuery );

#ifndef __S_NEW_SKILL_2
	SaveSkill( qry, pMover->m_idPlayer, pMover->m_jobSkills, szQuery );
#endif // __S_NEW_SKILL_2

	// 다른 캐릭터의 bank 저장

	
	ItemContainerStruct icsBank[3];

	for ( i = 0 ; i < 3 ; ++i ) {
		const u_long playerBank = i == pMover->m_nSlot ? pMover->m_idPlayer : pMover->m_idPlayerBank[i];
		if (playerBank == 0) continue;

		SaveItemContainer(pMover->m_Bank[i], icsBank[i]);
		DBQryAddBankSave( szQuery, playerBank);
		
		{
			qry->BindParameter( 1, icsBank[i].szItem, 4290 );
			qry->BindParameter( 2, icsBank[i].szIndex, 215 );
			qry->BindParameter( 3, icsBank[i].szObjIndex, 215 );
			qry->BindParameter( 4, &pMover->m_dwGoldBank[i]);
			qry->BindParameter( 5, icsBank[i].szExt, 2000 );
			qry->BindParameter( 6, icsBank[i].szPiercing, 7800 );
			qry->BindParameter( 7, icsBank[i].szPet, 2689 );
		}

		if ( FALSE == qry->Exec( szQuery ) ) {
			return;
		}
	}

	PocketStruct	aPocket[3];
	SavePocket( pMover, aPocket );
	for( i = 0; i < MAX_POCKET; i++ )
	{
		MakeQueryPocket( szQuery, pMover->m_idPlayer );

		qry->BindParameter( 1, &i );
		qry->BindParameter( 2, aPocket[i].szItem    , 4290);
		qry->BindParameter( 3, aPocket[i].szIndex   , 215);
		qry->BindParameter( 4, aPocket[i].szObjIndex, 215);
		qry->BindParameter( 5, aPocket[i].szExt     , 2000);
		qry->BindParameter( 6, aPocket[i].szPiercing, 7800);
		qry->BindParameter( 7, aPocket[i].szPet     , 2689);
		qry->BindParameter( 8, &aPocket[i].bExpired );
		qry->BindParameter( 9, &aPocket[i].tExpirationDate );

		if( !qry->Exec( szQuery ) )
		{
			return;
		}
	}

}

void	CDbManager::SaveHonor( CQuery *qry, u_long uidPlayer, std::span<const int> aHonor, char* szQuery )
{
	std::string str;

	char buffer[128];

	for (size_t i = 0; i != aHonor.size(); ++i) {
		const int value = aHonor[i];
		
		if (value != 0) {
			std::sprintf(buffer, "%lu,%d/", i, value);
			str += buffer;
		}
	}

	str += '$';

	sprintf(szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@progress=?",
		g_appInfo.dwSys, uidPlayer
		);

	qry->BindParameter(1, str);

	if (!qry->Exec(szQuery)) {
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
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
				quest.m_wId.get(), quest.m_nState, quest.m_wTime,
				quest.m_nKillNPCNum[0], quest.m_nKillNPCNum[1],
				quest.m_bPatrol  , quest.m_bReserve2, quest.m_bReserve3, quest.m_bReserve4,
				quest.m_bReserve5, quest.m_bReserve6, quest.m_bReserve7, quest.m_bReserve8
			);
			strncat(szQuestCnt, buffer, sizeof(buffer));
		}
		strcat(szQuestCnt, NullStr);

		szm_aCompleteQuest[0] = '\0';
		for (const auto questId : quests.completed) {
			sprintf(buffer, "%d/", questId.get());
			strncat(szm_aCompleteQuest, buffer, sizeof(buffer));
		}
		strcat(szm_aCompleteQuest, NullStr);

		szCheckedQuest[0] = '\0';
		for (const auto questId : quests.checked) {
			sprintf(buffer, "%d/", questId.get());
			strncat(szCheckedQuest, buffer, sizeof(buffer));
		}
		strcat(szCheckedQuest, NullStr);
	};

	SaveQuest_(*pMover->m_quests, szQuestCnt, szm_aCompleteQuest, szCheckedQuest);
}

void	CDbManager::SavePocket( CMover* pMover, std::span<PocketStruct, MAX_POCKET> pPocketStruct )
{
	for( int nPocket = 0; nPocket < MAX_POCKET; nPocket++ )
	{
		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
		if( pPocket )
		{
			SaveItemContainer(*pPocket, pPocketStruct[nPocket]);

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

void ItemContainerStruct::Accumulate(const ItemStruct & is, const DWORD apIndex, const DWORD objIndex) {
	std::strncat(szItem, is.szItem, sizeof(is.szItem));
	std::strncat(szExt, is.szExt, sizeof(is.szExt));
	std::strncat(szPiercing, is.szPiercing, sizeof(is.szPiercing));
	std::strncat(szPet, is.szPet, sizeof(is.szPet));

	char Onem_apIndex[16]; std::sprintf(Onem_apIndex, "%d/", apIndex);
	std::strncat(szIndex, Onem_apIndex, sizeof(Onem_apIndex));

	std::sprintf(Onem_apIndex, "%d/", objIndex);
	std::strcat(szObjIndex, Onem_apIndex);
}

void CDbManager::SaveItemContainer(CItemContainer & itemContainer, ItemContainerStruct & stringified) {
	stringified.Clear();

	ItemStruct is;

	bool hasExt = false, hasPiercing = false, hasPet = false;

	for (DWORD ch = 0; ch < itemContainer.m_dwItemMax; ch++) {
		SaveOneItem(&itemContainer.m_apItem[ch], &is);
		stringified.Accumulate(is, itemContainer.m_apIndex[ch], itemContainer.m_apItem[ch].m_dwObjIndex);

		hasExt |= is.hasExt;
		hasPiercing |= is.hasPiercing;
		hasPet |= is.hasPet;
	}

	// Finalize
	strcat(stringified.szItem,     NullStr);
	strcat(stringified.szIndex,    NullStr);
	strcat(stringified.szObjIndex, NullStr);

	if (!hasExt)      stringified.szExt[0] = '\0';
	if (!hasPiercing) stringified.szPiercing[0] = '\0';
	if (!hasPet)      stringified.szPet[0] = '\0';

	strcat(stringified.szExt,      NullStr);
	strcat(stringified.szPiercing, NullStr);
	strcat(stringified.szPet,      NullStr);
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

void CDbManager::SaveTaskBar(CMover * pMover, char * szAppletTaskBar, char * szItemTaskBar, char * szSkillTaskBar) {
	static constexpr auto AppendNumber = [](char * destination, int value) {
		char buffer[32] = "";
		std::sprintf(buffer, "%d,", value);
		std::strcat(destination, buffer);
	};
	
	static constexpr auto BufferShortcut = [](char * destination, const SHORTCUT & shortcut) {
		char buffer[MAX_TASKBAR] = "";

		sprintf(buffer, "%d,%d,0,%d,0,%d",
			static_cast<int>(shortcut.m_dwShortcut), shortcut.m_dwId,
			shortcut.m_dwIndex, shortcut.m_dwData);

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

void CDbManager::DBQryAddBankSave( char* szSql, u_long idPlayer )
{
		sprintf( szSql, "{call ADD_BANK_STR('U1','%07d','%02d', ?, ?, ?, ?, ?, ?, ?)}", idPlayer, g_appInfo.dwSys );
}

void CDbManager::MakeQueryPocket( char* szQuery, u_long idPlayer)
{
	sprintf( szQuery, "{call uspSavePocket( '%02d', '%07d', ?, ?, ?, ?, ?, ?, ?, ?, ?) }", g_appInfo.dwSys, idPlayer);
}

void CDbManager::SaveOneItem( CItemElem* pItemElem, ItemStruct * pItemStruct )
{
	if (pItemElem->IsEmpty()) {
		std::strcpy(pItemStruct->szItem, "");
		pItemStruct->hasExt = false;
		std::strcpy(pItemStruct->szExt, "0,0,0/");
		pItemStruct->hasPiercing = false;
		std::strcpy(pItemStruct->szPiercing, "0/");
		pItemStruct->hasPet = false;
		std::strcpy(pItemStruct->szPet, "0/");
		return;
	}

	char szPiercing[32]		= {0,};

	const char * itemText = pItemElem->m_szItemText;
	if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER) {
		itemText = "";
	}

	sprintf( pItemStruct->szItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
			pItemElem->m_dwObjId, pItemElem->m_dwItemId,
			0, 0, itemText,
			pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
			pItemElem->m_nHitPoint, pItemElem->m_nRepair,
			0, pItemElem->m_byFlag,
			pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
  			pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
  			pItemElem->m_idGuild,
			pItemElem->m_nResistSMItemId
		);


	// ext
	sprintf( pItemStruct->szExt, "0,%lu,%I64d,%d/",
		pItemElem->m_dwKeepTime, pItemElem->GetRandomOptItemId()
		, static_cast<int>( pItemElem->m_bTranformVisPet )
		);
	pItemStruct->hasExt = pItemElem->m_dwKeepTime > 0 || pItemElem->GetRandomOptItemId() != 0;


	// piercing
	pItemStruct->hasPiercing = pItemElem->IsPiercedItem();

	const ItemProp* itemProp = prj.GetItemProp( pItemElem->m_dwItemId );
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
	pItemStruct->hasPet = pItemElem->m_pPet;

	if (!pItemElem->m_pPet) {
		sprintf(pItemStruct->szPet, "0/");
	} else {
		sprintf(pItemStruct->szPet, "1,%d,%d,%d,%d,%d",
			pItemElem->m_pPet->GetKind(), pItemElem->m_pPet->GetLevel(),
			pItemElem->m_pPet->GetExp(), pItemElem->m_pPet->GetEnergy(),
			pItemElem->m_pPet->GetLife());
		for (int i = PL_D; i <= pItemElem->m_pPet->GetLevel(); i++) {
			char szAvailLevel[16] = { 0, };
			sprintf(szAvailLevel, ",%d", pItemElem->m_pPet->GetAvailLevel(i));
			strcat(pItemStruct->szPet, szAvailLevel);
		}

		char szFmt[MAX_PET_NAME_FMT] = { 0, };
		SetDBFormatStr(szFmt, MAX_PET_NAME_FMT, pItemElem->m_pPet->GetName());

		strcat(pItemStruct->szPet, ",");
		strcat(pItemStruct->szPet, szFmt);
		strcat(pItemStruct->szPet, "/");
	}

}