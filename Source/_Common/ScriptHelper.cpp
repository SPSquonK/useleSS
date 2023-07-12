#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "party.h"
#include "User.h"
#include "guildquest.h"
#include "defineQuest.h"
#include "worldmng.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpSrvr.h"
#include "guild.h"
#include "ScriptHelper.h"
#include "WorldDialog.h"

#if defined(__REMOVE_SCIRPT_060712)

enum QUEST_ENUM
{
	QE_QUEST_END,
	QE_QUEST_BEGIN,
	QE_QUEST_BEGIN_YES,
	QE_QUEST_BEGIN_NO,
	QE_QUEST_END_COMPLETE
	, QE_QUEST_NEXT_LEVEL
	, QE_QUEST_END_FAIL
};

static std::map< std::string, QUEST_ENUM > g_mapKey;

// 미리 정의된 키들을 위한 table을 만든다.
void InitPredefineKey()
{
	g_mapKey[ "QUEST_END" ]          = QE_QUEST_END;
	g_mapKey[ "QUEST_BEGIN" ]        = QE_QUEST_BEGIN;
	g_mapKey[ "QUEST_BEGIN_YES" ]    = QE_QUEST_BEGIN_YES;
	g_mapKey[ "QUEST_BEGIN_NO" ]     = QE_QUEST_BEGIN_NO;
	g_mapKey[ "QUEST_END_COMPLETE" ] = QE_QUEST_END_COMPLETE;
	g_mapKey[ "QUEST_NEXT_LEVEL" ]   = QE_QUEST_NEXT_LEVEL;
	g_mapKey[ "QUEST_END_FAIL" ]     = QE_QUEST_END_FAIL;
}

BOOL RunPredefineKey( LPCTSTR szKey, CNpcDialogInfo& info )
{
	int& nGlobal = *( info.GetGlobal() );

	const auto it = g_mapKey.find( szKey );
	if( it == g_mapKey.end() )
		return FALSE;
	
	QUEST_ENUM type = it->second;
	switch( type )
	{
	case QE_QUEST_END:
		__QuestEnd( info.GetPcId(), info.GetNpcId(), nGlobal, info.GetQuest(), FALSE );
		break;
	case QE_QUEST_BEGIN:
		__QuestBegin( info.GetPcId(), info.GetNpcId(), info.GetQuest() );
		break;
	case QE_QUEST_BEGIN_YES:
		__QuestBeginYes( info.GetPcId(), info.GetNpcId(), info.GetQuest() );
		break;
	case QE_QUEST_BEGIN_NO:
		__QuestBeginNo( info.GetPcId(), info.GetNpcId(), info.GetQuest() );
		break;
	case QE_QUEST_END_COMPLETE:
		__QuestEndComplete( info.GetPcId(), info.GetNpcId(), nGlobal, info.GetReturn(), info.GetQuest() );
		break;
	case QE_QUEST_END_FAIL:
		__QuestEnd( info.GetPcId(), info.GetNpcId(), nGlobal, info.GetQuest(), TRUE );
		break;
	default:
		return FALSE;
	};

	return TRUE;
}
#endif // #if defined(__REMOVE_SCIRPT_060712)


	
CWorld* __GetWorld( int nPcId, int nSrcId, int nCaller )
{
	CWorld* pWorld = NULL;
	if( nCaller == WES_DIALOG )
	{
		CUser* pUser = prj.GetUser( nPcId );
		pWorld = pUser->GetWorld();
	}
	else
	if( nCaller == WES_WORLD )
		pWorld = g_WorldMng.GetWorld( nSrcId );

	return pWorld;
}

int __AddKey( int nPcId, LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam )
{
	CHAR szKey[ 128 ], szWord[ 128 ];

	strcpy( szWord, lpszWord );

	if( lpszKey[0] == '\0' ) 
		strcpy( szKey, szWord );
	else
		strcpy( szKey, lpszKey );

	CUser* pUser	= prj.GetUser( nPcId );
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_ADDKEY;
	lstrcpy( rsf.lpszVal1, szWord );
	lstrcpy( rsf.lpszVal2, szKey );
	rsf.dwVal1	= dwParam;
	rsf.dwVal2 = 0;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int __AddAnswer( int nPcId, LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam1, int nQuest )
{
	CHAR szKey[ 128 ], szWord[ 128 ];

	strcpy( szWord, lpszWord );

	if( lpszKey[0] == '\0' ) 
		strcpy( szKey, szWord );
	else
		strcpy( szKey, lpszKey );

	CUser* pUser = prj.GetUser( nPcId );
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_ADDANSWER;
	lstrcpy( rsf.lpszVal1, szWord );
	lstrcpy( rsf.lpszVal2, szKey );
	rsf.dwVal1 = (DWORD)dwParam1;
	rsf.dwVal2 = nQuest;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int __RemoveQuest( int nPcId, int nQuest_ )      
{
	QuestId nQuest = QuestId(nQuest_);
	const QuestProp * pQuestProp = nQuest.GetProp();
	if( pQuestProp )
	{
		CUser* pUser	= prj.GetUser( nPcId );
		LPQUEST pQuest	= pUser->GetQuest( nQuest );

		if( pQuest && pQuest->m_nState < QS_END ) 
			pUser->AddDefinedText( TID_EVE_CANCELQUEST, "\"%s\"", pQuestProp->m_szTitle );
		pUser->RemoveQuest( nQuest );
		pUser->AddRemoveQuest( nQuest );
		// 시작시 변신을 했으면 퀘스트 삭제시 변신 해제시킨다.
		if( pQuest && pQuestProp->m_nBeginSetDisguiseMoverIndex )
		{
			CUser* pUser = prj.GetUser( nPcId );
			pUser->NoDisguise( );
			g_UserMng.AddNoDisguise( pUser );
		}
	}
	return 1;
}

int __RemoveAllKey( int nPcId )
{
	CUser* pUser	= prj.GetUser( nPcId );
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_REMOVEALLKEY;
	pUser->AddRunScriptFunc( rsf );

	return 1;
}

int __SayQuest( int nPcId,int nQuestId, int nIdx )
{
	CString strToken;

	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp && pQuestProp->m_apQuestDialog[ nIdx ] )
		strToken = pQuestProp->m_apQuestDialog[ nIdx ];
	if( strToken.IsEmpty() == TRUE )
		return FALSE;

	CUser* pUser = prj.GetUser( nPcId );
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_SAY;//QUEST;
	lstrcpy( rsf.lpszVal1, strToken );
	rsf.dwVal2 = nQuestId;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
int __RunQuest( int nPcId, int nNpcId, int nQuestId )
{
	CMover* pMover = prj.GetMover( nNpcId );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
		for( int  i = 0; i < 4; i++ )
		{
			if( pQuestProp->m_nBeginSetAddItemIdx[ i ] )
			{
				__CreateItem( nPcId, pQuestProp->m_nBeginSetAddItemIdx[ i ], pQuestProp->m_nBeginSetAddItemNum[ i ] );
			}
		}
		if( pQuestProp->m_nBeginSetAddGold )
		{
			__AddGold( nPcId, pQuestProp->m_nBeginSetAddGold );
		}

		// 퀘스트 시작시 변신함 
		if( pQuestProp->m_nBeginSetDisguiseMoverIndex )
		{
			CUser* pUser = prj.GetUser( nPcId );
			pUser->Disguise( pQuestProp->m_nBeginSetDisguiseMoverIndex );
			g_UserMng.AddDisguise( pUser, pQuestProp->m_nBeginSetDisguiseMoverIndex );
		}
		__SetQuest( nPcId, nQuestId );
	}
	return 1;
}

//int __EndQuest( int nPcId, int nQuestId, BOOL IsEndQuestCondition = TRUE )
int __EndQuest( int nPcId, int nQuestId, BOOL IsEndQuestCondition )
{
	int nNum;
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
		CUser* pUser = prj.GetUser( nPcId );
		if( IsEndQuestCondition )
		{
			if( __IsEndQuestCondition( pUser, nQuestId ) == 0 )
				return FALSE;
		}
		// 보상 아이템 갯수 만큼, 인벤토리 여유가 있는가?
		int nItemNum = 0;
		for( int i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
		{
			QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ];
			if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == pUser->GetSex() )
			{
				if( pEndRewardItem->m_nType == 0 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == pUser->GetJob() )
					{
						if( pEndRewardItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp(  pEndRewardItem->m_nItemIdx );
							int nDiv = pEndRewardItem->m_nItemNum / pItemProp->dwPackMax;
							int nSur = pEndRewardItem->m_nItemNum % pItemProp->dwPackMax;
							if( nSur ) nDiv++;
							nItemNum += nDiv;
						}
					}
				}
				else
				if( pEndRewardItem->m_nType == 1 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pUser->GetItemNum( pEndRewardItem->m_nJobOrItem ) )
					{
						if( pEndRewardItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp(  pEndRewardItem->m_nItemIdx );
							int nDiv = pEndRewardItem->m_nItemNum / pItemProp->dwPackMax;
							int nSur = pEndRewardItem->m_nItemNum % pItemProp->dwPackMax;
							if( nSur ) nDiv++;
							nItemNum += nDiv;
						}
					}
				}
			}
		}
		if( nItemNum > pUser->m_Inventory.GetEmptyCount() ) 
		{
			pUser->AddDefinedText( TID_QUEST_NOINVENTORYSPACE ); // 인벤토리 공간이 없어서 퀘스트를 완료할 수 없습니다.
			RunScriptFunc rsf;
			rsf.wFuncType		= FUNCTYPE_EXIT;
			pUser->AddRunScriptFunc( rsf );
			return FALSE;
		}
		////////////////////////////
		// 보상 지급 
		////////////////////////////
		for( int i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
		{
			QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ];
			if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == pUser->GetSex() )
			{
				if( pEndRewardItem->m_nType == 0 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == pUser->GetJob() )
					{
						if( pEndRewardItem->m_nItemIdx )
						{
							int nItemNum = pEndRewardItem->m_nItemNum;
#ifdef __JEFF_11
							__CreateItem( nPcId, pEndRewardItem->m_nItemIdx, nItemNum, pEndRewardItem->m_nAbilityOption, pEndRewardItem->m_byFlag );
#else	// __JEFF_11
							__CreateItem( nPcId, pEndRewardItem->m_nItemIdx, nItemNum );
#endif	// __JEFF_11
						}
					}
				}
				else
				if( pEndRewardItem->m_nType == 1 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pUser->GetItemNum( pEndRewardItem->m_nJobOrItem ) )
					{
						if( pEndRewardItem->m_nItemIdx )
						{
							int nItemNum = pEndRewardItem->m_nItemNum;
#ifdef __JEFF_11
							__CreateItem( nPcId, pEndRewardItem->m_nItemIdx, nItemNum, pEndRewardItem->m_nAbilityOption, pEndRewardItem->m_byFlag );
#else	// __JEFF_11
							__CreateItem( nPcId, pEndRewardItem->m_nItemIdx, nItemNum );
#endif	// __JEFF_11
						}
					}
				}
			}
		}
		if( pQuestProp->m_bEndRewardPetLevelup )
			pUser->PetLevelup();
		if(	pQuestProp->m_nEndRewardGoldMin )
		{
			nNum = pQuestProp->m_nEndRewardGoldMax - pQuestProp->m_nEndRewardGoldMin + 1; 
			int nGold = pQuestProp->m_nEndRewardGoldMin + xRandom( nNum ); 
			pUser->AddGold( nGold );
			pUser->AddGoldText( nGold );
		}
		if(	pQuestProp->m_nEndRewardExpMin )
		{
			nNum = pQuestProp->m_nEndRewardExpMax - pQuestProp->m_nEndRewardExpMin + 1; 
			int nExp = pQuestProp->m_nEndRewardExpMin + xRandom( nNum ); 
			pUser->EarnExperience(nExp, false, false);
			pUser->AddDefinedText( TID_GAME_REAPEXP );
		}
		if( pQuestProp->m_nEndRewardSkillPoint )
		{
			pUser->AddSkillPoint( pQuestProp->m_nEndRewardSkillPoint );
			pUser->AddSetExperience( pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel );
			g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_GET_QUEST, pQuestProp->m_nEndRewardSkillPoint, (CMover*)pUser, NULL );
		}

		if( pQuestProp->m_nEndRewardPKValueMin || pQuestProp->m_nEndRewardPKValueMax )
		{
			if( pQuestProp->m_nEndRewardPKValueMin <= pQuestProp->m_nEndRewardPKValueMax )
			{
				int nPKValue = pUser->GetPKValue() - xRandom( pQuestProp->m_nEndRewardPKValueMin, pQuestProp->m_nEndRewardPKValueMax + 1 );
				if( nPKValue < 0 )
					nPKValue = 0;
				pUser->SetPKValue( nPKValue );
				pUser->AddPKValue();
				g_dpDBClient.SendLogPkPvp( (CMover*)pUser, NULL, 0, 'P' );
				pUser->CheckHonorStat();
				g_UserMng.AddHonorTitleChange( pUser, pUser->m_nHonor);
			}
		}
		if( pQuestProp->m_nEndRewardTeleport != 0 )
			pUser->Replace( pQuestProp->m_nEndRewardTeleport, pQuestProp->m_nEndRewardTeleportPos, REPLACE_NORMAL, nTempLayer );
		// 시작시 변신을 했으면 종료시 변신 해제시킨다.
		if( pQuestProp->m_nBeginSetDisguiseMoverIndex )
		{
			CUser* pUser = prj.GetUser( nPcId );
			pUser->NoDisguise( );
			g_UserMng.AddNoDisguise( pUser );
		}
		if( pQuestProp->m_nEndRewardTSP )
			g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, pQuestProp->m_nEndRewardTSP, TRUE, 'Q' );

		if( pQuestProp->m_nEndRemoveTSP )
			g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, pQuestProp->m_nEndRemoveTSP, FALSE, 'Q' );
		////////////////////////////
		// 아이템 제거 
		////////////////////////////
		for( int i = 0; i < 8; i++ )
		{
			if( pQuestProp->m_nEndRemoveItemIdx[ i ] )
			{
				int nItemNum = pQuestProp->m_nEndRemoveItemNum[ i ];
				if( nItemNum > 0x7fff )		// 삭제될 아이템 개수가 short의 MAX를 넘으면..
				{
					for( ; nItemNum > 0x7fff; )
					{
						pUser->RemoveItemA( pQuestProp->m_nEndRemoveItemIdx[ i ], 0x7fff );
						nItemNum -= 0x7fff;
					}
				}
				pUser->RemoveItemA( pQuestProp->m_nEndRemoveItemIdx[ i ], nItemNum );
			}
		}
		if( pQuestProp->m_nEndRemoveGold )
			__RemoveGold( nPcId, pQuestProp->m_nEndRemoveGold );
		////////////////////////////
		// 퀘스트 제거 (완료,진행 모두 제거함)
		////////////////////////////
		for( int i = 0; i < MAX_QUESTREMOVE; i++ )
		{
			if( pQuestProp->m_anEndRemoveQuest[ i ] )
			{
				__RemoveQuest( nPcId, pQuestProp->m_anEndRemoveQuest[ i ] );
			}
		}
		// 퀘스트 종료 
		__SetQuestState( nPcId, nQuestId, QS_END );
	}
	return 1;
}

static void AddQuestKeys(
	int nPcId,
	std::vector<QuestId> & vecNewQuest,
	std::vector<QuestId> & vecNextQuest,
	std::vector<QuestId> & vecEndQuest,
	std::vector<QuestId> & vecCurrQuest
) {
	// sort
	__QuestSort(vecNewQuest);
	__QuestSort(vecNextQuest);
	__QuestSort(vecEndQuest);
	__QuestSort(vecCurrQuest);

	// send
	for (const QuestId questId : vecNewQuest)
		__AddQuestKey(nPcId, questId, "QUEST_BEGIN", true);
		
	for (const QuestId questId : vecNextQuest)
		__AddQuestKey(nPcId, questId, "QUEST_NEXT_LEVEL", true);

	for (const QuestId questId : vecEndQuest)
		__AddQuestKey(nPcId, questId, "QUEST_END", false);

	for (const QuestId questId : vecCurrQuest)
		__AddQuestKey(nPcId, questId, "QUEST_END", false);
}

//int __AddQuestKey( int nPcId, int nQuestId, LPCTSTR lpKey, int nParam = 0 )
int __AddQuestKey( int nPcId, QuestId nQuestId, LPCTSTR lpKey, bool bNew )
{
	CHAR szWord[ 128 ], szKey[ 128 ];
	const QuestProp * pQuestProp = nQuestId.GetProp();
	if( pQuestProp )
		strcpy( szWord, pQuestProp->m_szTitle );
	if( lpKey[0] == '\0' ) 
		strcpy( szKey, szWord );
	else
		strcpy( szKey, lpKey );

	CUser* pUser	= prj.GetUser( nPcId );
	RunScriptFunc rsf;
	if( bNew )
		rsf.wFuncType		= FUNCTYPE_NEWQUEST;
	else
		rsf.wFuncType		= FUNCTYPE_CURRQUEST;
	lstrcpy( rsf.lpszVal1, szWord );
	lstrcpy( rsf.lpszVal2, szKey );
	rsf.dwVal1	= 0;
	rsf.dwVal2 = nQuestId.get();
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
void __QuestBegin( int nPcId, int nNpcId, int nQuestId )
{
	__SayQuest( nPcId, nQuestId, QSAY_BEGIN1 );
	__SayQuest( nPcId, nQuestId, QSAY_BEGIN2 );
	__SayQuest( nPcId, nQuestId, QSAY_BEGIN3 );
	__SayQuest( nPcId, nQuestId, QSAY_BEGIN4 );
	__SayQuest( nPcId, nQuestId, QSAY_BEGIN5 );
	__AddAnswer( nPcId, "__YES__", "QUEST_BEGIN_YES", 0, nQuestId );
	__AddAnswer( nPcId, "__NO__", "QUEST_BEGIN_NO", 0, nQuestId  );
	// 기타 대화를 위한 키를 추가한다.
	CMover* pMover = prj.GetMover( nNpcId );
	pMover->m_pNpcProperty->RunDialog( "#questBegin", NULL, 0, nNpcId, nPcId, nQuestId );
}
void __QuestEnd( int nPcId, int nNpcId, int& nGlobal, int nQuestId, BOOL bButtOK )
{
	// 현재 진행 중인 퀘스트 중에서 현재 NPC가 처리해야될 퀘스트를 실행(자신일 수도 있음)
	CMover* pMover = prj.GetMover( nNpcId );
	CUser* pUser = prj.GetUser( nPcId );

	BOOL bNewQuest = FALSE;

	__RemoveAllKey( nPcId );
	pMover->m_pNpcProperty->RunDialog( "#addKey", NULL, 0, nNpcId, nPcId, 0 );
	
	// 퀘스트 리스트 send
	std::vector<QuestId> vecNewQuest;
	std::vector<QuestId> vecNextQuest;
	std::vector<QuestId> vecEndQuest;
	std::vector<QuestId> vecCurrQuest;
	LPCHARACTER lpChar = prj.GetCharacter( pMover->m_szCharacterKey );
	if( lpChar )
	{
		for (const auto & [nQuest, _ ] : lpChar->m_srcQuests) {
			QUEST * lpQuest = pUser->GetQuest( nQuest );

			// new quest
			if( lpQuest == NULL && !pUser->IsCompleteQuest( nQuest ) )
			{
				// now
				if( __IsBeginQuestCondition( pUser, nQuest.get() ) )
				{
					bNewQuest = TRUE;
					vecNewQuest.push_back( nQuest );
				}
				// next
				else if( __IsNextLevelQuest( pUser, nQuest.get() ) )
					vecNextQuest.push_back( nQuest );
			}
			// current quest
			else if( lpQuest && !pUser->IsCompleteQuest( nQuest ) && lpQuest->m_nState != QS_END )
			{
				// complete
				if( __IsEndQuestCondition( pUser, nQuest.get() ) )
					vecEndQuest.push_back( nQuest );
				// running
				else
					vecCurrQuest.push_back( nQuest );
			}
		}

		AddQuestKeys(nPcId, vecNewQuest, vecNextQuest, vecEndQuest, vecCurrQuest);
	}
	
	BOOL bDialogText = TRUE;
	BOOL bCompleteCheck = TRUE;
	
	if( nQuestId )
	{
		QUEST * lpQuest = pUser->GetQuest( QuestId(nQuestId) );
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
		if( lpQuest && lpQuest->m_nState != QS_END && pQuestProp )	// 진행중인 퀘스트 선택 시
		{
			bCompleteCheck = FALSE;
			if( !bButtOK ) // 퀘스트 목록을 선택하고 들어온 경우
			{
				if( strcmpi( pQuestProp->m_szEndCondCharacter, pMover->m_szCharacterKey ) == 0 && __IsEndQuestCondition( pUser, nQuestId ) )
				{
					__SayQuest( nPcId, nQuestId, QSAY_END_COMPLETE1 );
					__SayQuest( nPcId, nQuestId, QSAY_END_COMPLETE2 );
					__SayQuest( nPcId, nQuestId, QSAY_END_COMPLETE3 );
					__AddAnswer( nPcId,"__OK__", "QUEST_END_COMPLETE", 0, nQuestId );
				}
				else
				{
					__SayQuest( nPcId, nQuestId, QSAY_END_FAILURE1 );
					__SayQuest( nPcId, nQuestId, QSAY_END_FAILURE2 );
					__SayQuest( nPcId, nQuestId, QSAY_END_FAILURE3 );
					__AddAnswer( nPcId,"__OK__", "QUEST_END_FAIL", 0, nQuestId );
				}
				bDialogText = FALSE;
			}
		}
	}
	
	// 완료 가능한 퀘스트가 있는지 검사
	if( bCompleteCheck )
	{
		if (pUser->m_quests) {
			for (QUEST & quest : pUser->m_quests->current) {
				const QuestProp * pQuestProp = quest.GetProp();
				if (!pQuestProp) {
					WriteError("__QuestEnd quest(%d) property null", quest.m_wId);
					continue;
				}

				if (strcmpi(pQuestProp->m_szEndCondCharacter, pMover->m_szCharacterKey) == 0 && quest.m_nState != QS_END) {
					if (__IsEndQuestCondition(pUser, quest.m_wId.get())) {
						__SayQuest(nPcId, quest.m_wId.get(), QSAY_END_COMPLETE1);
						__SayQuest(nPcId, quest.m_wId.get(), QSAY_END_COMPLETE2);
						__SayQuest(nPcId, quest.m_wId.get(), QSAY_END_COMPLETE3);
						__AddAnswer(nPcId, "__OK__", "QUEST_END_COMPLETE", 0, quest.m_wId.get());
						bDialogText = FALSE;
						break;
					}
				}
			}
		}
	}

	if( bDialogText )
	{
		if( bNewQuest )
		{
			if( vecNewQuest.size() == 1 && bCompleteCheck )	// 진행가능한 퀘스트가 하나면 바로 퀘스트 수락창 표시
				__QuestBegin( nPcId, nNpcId, vecNewQuest[ 0 ].get() );
			else
				pMover->m_pNpcProperty->RunDialog( "#yesQuest", NULL, 0, nNpcId, nPcId, 0 ); // 준비된 퀘스트가 있을 때의 인사말
		}
		else
			pMover->m_pNpcProperty->RunDialog( "#noQuest", NULL, 0, nNpcId, nPcId, 0 );	// 준비된 퀘스트가 없을 때의 인사말
	}
}
void __QuestBeginYes( int nPcId, int nNpcId, int nQuestId )
{
	CUser* pUser	= prj.GetUser( nPcId );

#ifdef __VTN_TIMELIMIT
	//	mulcom	BEGIN100315	베트남 시간 제한
	if( ::GetLanguage() == LANG_VTN )
	{
		if( pUser->IsPlayer() == TRUE )
		{
			if( pUser->m_nAccountPlayTime > MIN( 180 ) )
			{
				pUser->AddDefinedText( TID_GAME_ERROR_QUEST_1 );

				return;
			}
		}
	}
	//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT

	LPQUEST lpQuest		= pUser->GetQuest( QuestId(nQuestId) );
	if( __IsBeginQuestCondition( pUser, nQuestId )
		&& lpQuest == NULL
		&& !pUser->IsCompleteQuest( QuestId(nQuestId) ) )
	{
		__SayQuest( nPcId, nQuestId, QSAY_BEGIN_YES );
		__RunQuest( nPcId, nNpcId, nQuestId );
		CMover* pMover = prj.GetMover( nNpcId );
		__RemoveAllKey( nPcId );
		pMover->m_pNpcProperty->RunDialog( "#addKey", NULL, 0, nNpcId, nPcId, 0 );
		LPQUEST lpQuestList;
		// 퀘스트 리스트 send
		std::vector<QuestId> vecNewQuest;
		std::vector<QuestId> vecNextQuest;
		std::vector<QuestId> vecEndQuest;
		std::vector<QuestId> vecCurrQuest;
		LPCHARACTER lpChar = prj.GetCharacter( pMover->m_szCharacterKey );
		if( lpChar )
		{
			for (const auto & [nQuest, _] : lpChar->m_srcQuests) {
				lpQuestList = pUser->GetQuest( nQuest );
				
				// new quest
				if( lpQuestList == NULL && !pUser->IsCompleteQuest( nQuest ) )
				{
					// now
					if( __IsBeginQuestCondition( pUser, nQuest.get() ) )
						vecNewQuest.push_back( nQuest );
					// next
					else if( __IsNextLevelQuest( pUser, nQuest.get() ) )
						vecNextQuest.push_back( nQuest );
				}
				// current quest
				else if( lpQuestList && !pUser->IsCompleteQuest( nQuest ) )
				{
					// complete
					if( __IsEndQuestCondition( pUser, nQuest.get() ) )
						vecEndQuest.push_back( nQuest );
					// running
					else
						vecCurrQuest.push_back( nQuest );
				}
			}
			
			

			AddQuestKeys(nPcId, vecNewQuest, vecNextQuest, vecEndQuest, vecCurrQuest);
		}
		pMover->m_pNpcProperty->RunDialog( "#questBeginYes", NULL, 0, nNpcId, nPcId, nQuestId );
	}

}
void __QuestBeginNo( int nPcId, int nNpcId, int nQuestId )
{
	__SayQuest( nPcId, nQuestId, QSAY_BEGIN_NO );
	CMover* pMover = prj.GetMover( nNpcId );
	pMover->m_pNpcProperty->RunDialog( "#questBeginNo", NULL, 0, nNpcId, nPcId, nQuestId );
}
void __QuestEndComplete( int nPcId, int nNpcId, int& nGlobal, int nVal, int nQuestId )
{
	CUser* pUser	= prj.GetUser( nPcId );
	LPQUEST lpQuest		= pUser->GetQuest( QuestId(nQuestId) );

#ifdef __VTN_TIMELIMIT
	//	mulcom	BEGIN100315	베트남 시간 제한
	if( ::GetLanguage() == LANG_VTN )
	{
		if( pUser->IsPlayer() == TRUE )
		{
			if( pUser->m_nAccountPlayTime > MIN( 180 ) )
			{
				pUser->AddDefinedText( TID_GAME_ERROR_QUEST_1 );
				return;
			}
		}
	}
	//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT

	// 퀘스트가 진행 중인 경우 
	if( lpQuest && pUser->IsCompleteQuest( QuestId(nQuestId) ) == FALSE
		&& __IsEndQuestCondition( pUser, nQuestId ) )
	{
		CMover* pMover = prj.GetMover( nNpcId );
		if( __EndQuest( nPcId, nQuestId ) == TRUE )
			pMover->m_pNpcProperty->RunDialog( "#questEndComplete", NULL, 0, nNpcId, nPcId, nQuestId );
		__QuestEnd( nPcId, nNpcId, nGlobal, nQuestId, TRUE );
	}	
}
int __SetQuestState( DWORD dwIdMover, int nQuest_, int nState )
{
	const QuestId nQuest = QuestId(nQuest_);
	CUser* pUser	= prj.GetUser( dwIdMover );
	if( nState == QS_END )
	{
		const QuestProp * pQuestProp = nQuest.GetProp();
		if( pQuestProp )
		{
			if( nQuest !=  QuestId(QUEST_CREGUILD) )
				pUser->AddDefinedText( TID_EVE_ENDQUEST, "\"%s\"", pQuestProp->m_szTitle );
		}
		g_dpDBClient.CalluspLoggingQuest( pUser->m_idPlayer, nQuest, 20 );
	}
	else
	if( nState == QS_BEGIN )
	{
		g_dpDBClient.CalluspLoggingQuest( pUser->m_idPlayer, nQuest, 10 );
	}
	QUEST quest;
	if( pUser->SetQuest( nQuest, nState, &quest ) )
		pUser->AddSetQuest( &quest );
	return 1;
}

void __SetQuest( DWORD dwIdMover, int nQuest_ )
{
	QuestId nQuest = QuestId(nQuest_);
	const QuestProp * pQuestProp = nQuest.GetProp();
	CUser* pUser	= prj.GetUser( dwIdMover );
	if( pQuestProp )
		pUser->AddDefinedText( TID_EVE_STARTQUEST, "\"%s\"", pQuestProp->m_szTitle );
	QUEST quest;
	if( pUser->SetQuest( nQuest, 0, &quest ) )
		pUser->AddSetQuest( &quest );
	g_dpDBClient.CalluspLoggingQuest( pUser->m_idPlayer, nQuest, 10 );
}

#ifdef __JEFF_11
void __CreateItem( DWORD dwIdMover, int v1, int v2, int v3, BYTE v4 )
#else	// __JEFF_11
void __CreateItem( DWORD dwIdMover, int v1, int v2 )	
#endif	// __JEFF_11
{
	CUser* pUser	= prj.GetUser( dwIdMover );
	ItemProp* pItemProp		= prj.GetItemProp( v1 );
	if( pItemProp )
		pUser->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", pItemProp->szName );
	CItemElem itemElem;
	itemElem.m_dwItemId		= v1;
	itemElem.m_nItemNum		= v2;
#ifdef __JEFF_11
	itemElem.SetAbilityOption( v3 );
#endif	// __JEFF_11
	itemElem.SetFlag( v4 );
	if( pUser->CreateItem( &itemElem ) )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "Q";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "QUEST";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
	}
}
void __AddGold( DWORD dwIdMover, int nGold )     
{
	CUser* pUser = prj.GetUser( dwIdMover );
	pUser->AddGold( nGold );
	pUser->AddGoldText( nGold );
}

void __RemoveGold( DWORD dwIdMover, int nGold )
{
	CUser* pUser	= prj.GetUser( dwIdMover );
	if( pUser->AddGold( -nGold ) == FALSE )
		pUser->SetGold( 0 );
}

void __QuestSort(std::vector<QuestId> & vecQuestId) {
	std::ranges::sort(vecQuestId,
		[](const QuestId lhs, const QuestId rhs) {
			const QuestProp * const pPrevQuestProp = lhs.GetProp();
			const QuestProp * const pNextQuestProp = rhs.GetProp();

			if (pPrevQuestProp->m_nBeginCondLevelMin < pNextQuestProp->m_nBeginCondLevelMin) return false;
			if (pPrevQuestProp->m_nBeginCondLevelMin > pNextQuestProp->m_nBeginCondLevelMin) return true;

			if (pPrevQuestProp->m_nBeginCondLevelMax < pNextQuestProp->m_nBeginCondLevelMax) return false;
			if (pPrevQuestProp->m_nBeginCondLevelMax > pNextQuestProp->m_nBeginCondLevelMax) return true;

			return lhs > rhs;
		}
	);
}
