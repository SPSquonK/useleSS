#ifndef __NPC_PROPERTY__
#define __NPC_PROPERTY__

#include "Project.h"

class CNpcProperty
{
public:
	CNpcProperty();
	virtual ~CNpcProperty();

public:
	BOOL			LoadDialog( LPCHARACTER lpCharacter );
	BOOL			IsDialogLoaded();
	BOOL			IsTimeOut();
	void			ResetTimer();
	BOOL			RunDialog( LPCTSTR pKey, int* nResult, int nValue, int nDstId, int nSrcId, QuestId nQuestId );

protected:

	char			m_szName[64];
	int				m_nGlobal;

	DWORD			m_dwTick;
};

#endif
