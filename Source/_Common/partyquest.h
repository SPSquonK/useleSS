#ifndef __PARTYQUEST_H__
#define	__PARTYQUEST_H__

#pragma once

#include "projectcmn.h"
#include "groupquest.h"

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

typedef	struct	_PARTYQUEST
{
	int		nId;
	int		nState;
	u_long	idParty;

	_PARTYQUEST()
		{
			nId		= -1;
			nState	= 0;
			idParty		= 0;
		};
}
PARTYQUEST,	*PPARTYQUEST;

typedef struct	_PARTYQUESTELEM : public _PARTYQUEST
{
	DWORD	dwEndTime;
	GroupQuest::ProcessState	nProcess;
	BYTE	ns;
	BYTE	nf;
	OBJID	objidWormon;
	short	nCount;

	_PARTYQUESTELEM() : _PARTYQUEST()
	{
		dwEndTime	= 0;
		nProcess	= GroupQuest::ProcessState::Ready;
		ns	= nf	= 0;
		objidWormon		= NULL_ID;
		nCount	= 0;
	};
}
PARTYQUESTELEM, *PPARTYQUESTELEM;

typedef struct	__PARTYQUESTRECT
{
	int nId;
	DWORD	dwWorldId;
	CRect rect;
}
PARTYQUESTRECT, *PPARTYQUESTRECT;

//class CGuild;
#include <boost/container/flat_map.hpp>

class CPartyQuestProcessor final
{
private:
	PARTYQUESTELEM	m_pElem[MAX_PARTY_QUEST];
	PARTYQUESTRECT	m_pRect[MAX_PARTY_QUEST];
	int		m_nRect = 0;
public:
//	Operations
	void	Process();

	void	SetPartyQuest( int nQuestId, int nState, int ns, int nf, u_long idParty, OBJID objidWormon );
	PPARTYQUESTELEM GetPartyQuest( int nQuestId );
	PPARTYQUESTRECT	GetPartyQuestRect( int nQuestId );
	void	RemovePartyQuest( int nQuestId );
	BOOL	IsQuesting( int nQuestId );
	static	CPartyQuestProcessor* GetInstance( void );
	void	RemoveAllDynamicObj( DWORD dwWorldID, D3DXVECTOR3 vPos, int nRange );
	void	SendQuestLimitTime(GroupQuest::ProcessState nState, DWORD dwTime, u_long idParty );
	void	AddQuestRect( int nId, DWORD dwWorldId, int x1, int y1, int x2, int y2 );
	int		PtInQuestRect( DWORD dwWorldId, const D3DXVECTOR3 & vPos );
};


#endif	// __PARTYQUEST_H__