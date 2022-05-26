#pragma once

#include "projectcmn.h"
#include "groupquest.h"
#include <boost/container/flat_map.hpp>

#define	MAX_PARTY_QUEST_TITLE_LEN		32
#define	MAX_PARTY_QUEST_STATE		16
#define	MAX_PARTY_QUEST		256
#define	MAX_PARTY_QUEST_DESC_LEN	260


typedef struct WORMON
{
	DWORD	dwWormon;
	D3DXVECTOR3		vPos;	
} WORMON, *PWORMON;



struct	PARTYQUESTPROP
{
	char	szTitle[MAX_PARTY_QUEST_TITLE_LEN];
	int		nLevel;
	char	szDesc[MAX_PARTY_QUEST_STATE][MAX_PARTY_QUEST_DESC_LEN];
//	DWORD	dwWormon;
	DWORD	dwWorldId;
	char    szWorldKey[32];
//	D3DXVECTOR3		vPos;
	std::vector< WORMON > vecWormon;	
	int	x1;
	int y1;
	int x2;
	int y2;
//	DWORD	dwTime;
	
	PARTYQUESTPROP()
		{
			ZeroMemory( szTitle, sizeof(szTitle) );
			nLevel	= 0;
//			dwTime	= 0;
			ZeroMemory( szDesc, sizeof(szDesc) );
//			dwWormon	= 0;
			dwWorldId	= 0;
//			vPos	= D3DXVECTOR3( 0, 0, 0 );
			vecWormon.clear();
			x1	= y1	= x2	= y2	= 0;
			ZeroMemory( szWorldKey, sizeof(szWorldKey) );		
		};
};

#ifdef __WORLDSERVER

class CPartyQuestProcessor final : public GroupQuest::CQuestProcessor {
public:
	static CPartyQuestProcessor * GetInstance();
	
	void Process();

	void SetQuest(int nQuestId, int nState, int ns, int nf, u_long idParty, OBJID objidWormon) {
		_SetQuest(nQuestId, nState, ns, nf, idParty, objidWormon);
		SetQuestParty_Log(nQuestId, idParty, objidWormon);
	}

	void SendQuestLimitTime(GroupQuest::ProcessState nState, DWORD dwTime, u_long idParty);

private:
	void SetQuestParty_Log(int nQuestId, u_long idParty, OBJID objidWormon);
	void RemoveAllDynamicObj(DWORD dwWorldID, D3DXVECTOR3 vPos, int nRange);

	void Boom(int nQuestId, CRect rect, DWORD dwWorldId);
};

#endif