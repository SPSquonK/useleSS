#pragma once

#include "Campus.h"
#include "playerdata.h"

#define		COMPLETE_PUPIL_LEVEL	75
#define		MIN_MASTER_LEVEL		91
#define		REMOVE_CAMPUS_POINT		5

class CCampusHelper  
{
public:
	struct REWARD {
		int nMasterReward = 0;
		int nPupilReward  = 0;
	};

	static CCampusHelper* GetInstance();

	friend CAr & operator>>(CAr & ar, CCampusHelper & self) { return ar >> self.m_pCampusMng; }
	u_long	AddCampus( CCampus* pCampus )	{	return m_pCampusMng.AddCampus( pCampus );	}
	BOOL	RemoveCampus( u_long idCampus )	{	return m_pCampusMng.RemoveCampus( idCampus );	}
	
	CCampus*	GetCampus( u_long idCampus )	{	return m_pCampusMng.GetCampus( idCampus );	}

	bool	AddPlayerId2CampusId( u_long idPlayer, u_long idCampus )	{	return m_pCampusMng.AddPlayerId2CampusId( idPlayer, idCampus );	}
	void	RemovePlayerId2CampusId( u_long idPlayer )	{	m_pCampusMng.RemovePlayerId2CampusId( idPlayer );	}
	u_long	GetCampusIdByPlayerId( u_long idPlayer )	{	return m_pCampusMng.GetCampusIdByPlayerId( idPlayer );	}

	void	OnAddCampusMember( CAr & ar );
	void	OnRemoveCampusMember( CAr & ar );
	void	OnUpdateCampusPoint( CAr & ar );
	void	OnUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData );

	void	LoadScript();
	BOOL	IsInviteAble( CUser* pRequest, CUser* pTarget );
	BOOL	IsMasterLevel( CUser* pUser );
	BOOL	IsPupilLevel( CUser* pUser );
	[[nodiscard]] static size_t GetMaxPupilNum(const CUser * pUser);
	[[nodiscard]] bool	IsCompleteCampusQuest(const CUser * pUser) const;
	void	AddAllMemberUpdateCampus( CCampus* pCampus );
	void	AddAllMemberRemoveCampus( CCampus* pCampus );
	DWORD	GetCampusBuff( int nLevel );
	void	RecoveryCampusPoint( CUser* pUser, DWORD dwTick );
	void	SetLevelUpReward( CUser* pUser );
	REWARD* GetReward( int nLevel );

	void	OnInviteCampusMember( CUser* pRequest, CUser* pTarget );
	void	OnAcceptCampusMember( CUser* pRequest, CUser* pTarget );
	void	OnRemoveCampusMember( CUser* pRequest, u_long idTarget );

private:
	CCampusMng m_pCampusMng;
	DWORD		m_dwRecoveryTime;
	int			m_nRecoveryPoint;
	std::vector<QuestId>	m_vecCQuest;
	std::map<int, DWORD>	m_mapCBuff;
	std::map<int, REWARD>	m_mapCReward;
};
