#pragma once

#include "projectcmn.h"
#include "groupquest.h"

#define	MAX_GUILD_QUEST_TITLE_LEN		32
#define	MAX_GUILD_QUEST_STATE		16
#define	MAX_GUILD_QUEST		256
#define	MAX_GUILD_QUEST_DESC_LEN	260

struct	GUILDQUESTPROP
{
	char	szTitle[MAX_GUILD_QUEST_TITLE_LEN];
	int		nLevel;
	char	szDesc[MAX_GUILD_QUEST_STATE][MAX_GUILD_QUEST_DESC_LEN];
	DWORD	dwWormon;
	DWORD	dwWorldId;
	D3DXVECTOR3		vPos;
	int	x1;
	int y1;
	int x2;
	int y2;
//	DWORD	dwTime;
	
	GUILDQUESTPROP()
		{
			ZeroMemory( szTitle, sizeof(szTitle) );
			nLevel	= 0;
//			dwTime	= 0;
			ZeroMemory( szDesc, sizeof(szDesc) );
			dwWormon	= 0;
			dwWorldId	= 0;
			vPos	= D3DXVECTOR3( 0, 0, 0 );
			x1	= y1	= x2	= y2	= 0;
		};
};

typedef struct	__GUILDQUESTRECT
{
	int nId;
	CRect rect;
}
GUILDQUESTRECT, *PGUILDQUESTRECT;

class CGuild;


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
