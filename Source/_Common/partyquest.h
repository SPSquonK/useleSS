#pragma once

#include "projectcmn.h"
#include "groupquest.h"
#include <boost/container/flat_map.hpp>

#define	MAX_PARTY_QUEST_TITLE_LEN		32
#define	MAX_PARTY_QUEST_STATE		16
#define	MAX_PARTY_QUEST		256
#define	MAX_PARTY_QUEST_DESC_LEN	260


struct PARTYQUESTPROP : GroupQuest::QuestProp {
	char szWorldKey[32] = "";
	std::vector<GroupQuest::WORMON> vecWormon;	
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