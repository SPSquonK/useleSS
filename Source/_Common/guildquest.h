#pragma once

#include "projectcmn.h"
#include "groupquest.h"

struct GUILDQUESTPROP : GroupQuest::QuestProp {
	GroupQuest::WORMON wormon;
};

#ifdef __WORLDSERVER
class CGuildQuestProcessor final : public GroupQuest::CQuestProcessor {
public:
	static CGuildQuestProcessor * GetInstance();

	void SetQuest(int nQuestId, int nState, int ns, int nf, u_long idParty, OBJID objidWormon) {
		_SetQuest(nQuestId, nState, ns, nf, idParty, objidWormon);
	}

	void Process();
};
#endif
