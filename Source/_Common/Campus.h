// Campus.h: interface for the CCampus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMPUS_H__D0177E19_6285_41A9_9AB5_6A145BBD08BC__INCLUDED_)
#define AFX_CAMPUS_H__D0177E19_6285_41A9_9AB5_6A145BBD08BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ar.h"

enum class CampusRole { Invalid = 0, Master = 1, Pupil = 2 };

#define		MAX_PUPIL_NUM			3
#define		MIN_LV2_POINT			41
#define		MIN_LV3_POINT			101

struct CCampusMember {
	static constexpr bool Archivable = true;
	u_long     idPlayer  = 0;
	CampusRole nMemberLv = CampusRole::Invalid;
};

class CCampus
{
public:
	CCampus();
	~CCampus();

	void	Clear();
	void	Serialize( CAr & ar );
	
	u_long	GetCampusId()	{	return m_idCampus;	}
	void	SetCampusId( u_long idCampus )	{	m_idCampus = idCampus;	}
	u_long	GetMaster()		{	return m_idMaster;	}
	void	SetMaster( u_long idMaster )	{	m_idMaster = idMaster;	}
	BOOL	IsMaster( u_long idPlayer )		{	return ( idPlayer == m_idMaster );	}
	int		GetMemberSize()	{	return m_mapCM.size();	}

	BOOL	IsPupil( u_long idPlayer );
	std::vector<u_long>	GetPupilPlayerId();
	int		GetPupilNum();
	
	std::vector<u_long>	GetAllMemberPlayerId();
	CampusRole		GetMemberLv( u_long idPlayer );
	BOOL	IsMember( u_long idPlayer );
	BOOL	AddMember( CCampusMember* pCM );
	BOOL	RemoveMember( u_long idPlayer );
	CCampusMember*	GetMember( u_long idPlayer );
	

private:
	u_long	m_idCampus;
	u_long	m_idMaster;
	std::map<u_long, CCampusMember *>	m_mapCM;

#ifdef __WORLDSERVER
public:
	BOOL	IsChangeBuffLevel( u_long idPlayer );
	int		GetBuffLevel( u_long idPlayer );

private:
	int		m_nPreBuffLevel;
#endif // __WORLDSERVER
};

class CCampusMng
{
public:
	CCampusMng();
	~CCampusMng();

	BOOL	IsEmpty()	{	return m_mapCampus.empty();	}
	void	Clear();
	void	Serialize( CAr & ar );

	u_long	AddCampus( CCampus* pCampus );
	BOOL	RemoveCampus( u_long idCampus );

	CCampus*	GetCampus( u_long idCampus );

	bool	AddPlayerId2CampusId( u_long idPlayer, u_long idCampus );
	void	RemovePlayerId2CampusId( u_long idPlayer )	{	m_mapPid2Cid.erase( idPlayer );	}
	u_long	GetCampusIdByPlayerId( u_long idPlayer );

private:
	u_long	m_idCampus;
	std::map<u_long, CCampus *>	m_mapCampus;
	std::map<u_long, u_long>	m_mapPid2Cid;
};
#endif // !defined(AFX_CAMPUS_H__D0177E19_6285_41A9_9AB5_6A145BBD08BC__INCLUDED_)
