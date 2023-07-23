#ifndef __SCRIPT_HELPER_H__
#define __SCRIPT_HELPER_H__

class CNpcDialogInfo;
void InitPredefineKey(); 
BOOL RunPredefineKey( LPCTSTR szKey, CNpcDialogInfo& info );

CWorld* __GetWorld( int nPcId, int nSrcId, int nCaller );
int __AddKey( int nPcId, LPCTSTR lpszWord, LPCTSTR lpszKey = "", DWORD dwParam = 0);
int __AddAnswer( int nPcId, LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam1, QuestId nQuest );
int __RemoveQuest( int nPcId, QuestId nQuest );
int __RemoveAllKey( int nPcId );
int __SayQuest( int nPcId, QuestId nQuestId, int nIdx );
int __RunQuest( int nPcId, int nNpcId, QuestId nQuestId );
int __EndQuest( int nPcId, QuestId nQuestId, BOOL IsEndQuestCondition = TRUE );
int __AddQuestKey( int nPcId, QuestId nQuestId, LPCTSTR lpKey, bool bNew);
void __QuestBegin( int nPcId, int nNpcId, QuestId nQuestId );
void __QuestEnd( int nPcId, int nNpcId, int& nGlobal, QuestId nQuestId = QuestIdNone, BOOL bButtOK = FALSE );
void __QuestBeginYes( int nPcId, int nNpcId, QuestId nQuestId );
void __QuestBeginNo( int nPcId, int nNpcId, QuestId nQuestId );
void __QuestEndComplete( int nPcId, int nNpcId, int& nGlobal, int nVal, QuestId nQuestId );

int __SetQuestState( DWORD dwIdMover, QuestId nQuest, int nState );
void __SetQuest( DWORD dwIdMover, QuestId nQuest );
#ifdef __JEFF_11
void __CreateItem( DWORD dwIdMover, int v1, int v2, int v3 = 0, BYTE v4 = 0 );
#else	// __JEFF_11
void __CreateItem( DWORD dwIdMover, int v1, int v2 );
#endif	// __JEFF_11
void __AddGold( DWORD dwIdMover, int nGold );
void __RemoveGold( DWORD dwIdMover, int nGold );
void __QuestSort(std::vector<QuestId> & vecQuestId );

#endif
