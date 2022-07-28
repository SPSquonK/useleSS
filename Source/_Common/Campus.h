#pragma once

#include "ar.h"
#include <boost/container/flat_map.hpp>
#include <boost/container/small_vector.hpp>

enum class CampusRole { Invalid = 0, Master = 1, Pupil = 2 };

class CCampus
{
private:
	struct CCampusMember {
		static constexpr bool Archivable = true;
		u_long     idPlayer  = 0;
		CampusRole nMemberLv = CampusRole::Invalid;
	};

public:
	static constexpr size_t MAX_PUPIL_NUM = 3;
	static constexpr int MIN_LV2_POINT = 41;
	static constexpr int MIN_LV3_POINT = 101;
	
	static size_t GetMaxPupilNum(int campusPoints);

	friend CAr & operator<<(CAr & ar, const CCampus & campus);
	friend CAr & operator>>(CAr & ar, CCampus & campus);
	
	[[nodiscard]] u_long GetCampusId() const { return m_idCampus; }
	void SetCampusId(u_long idCampus) { m_idCampus = idCampus; }
	void SetMaster(u_long idMaster) { m_idMaster = idMaster; }

	[[nodiscard]] u_long GetMaster() const { return m_idMaster; }
	[[nodiscard]] bool IsMaster(u_long idPlayer) const { return idPlayer == m_idMaster; }
	[[nodiscard]] size_t GetMemberSize() const { return m_mapCM.size(); }

	[[nodiscard]] bool IsPupil(u_long idPlayer) const;
	[[nodiscard]] boost::container::small_vector<u_long, MAX_PUPIL_NUM>	GetPupilPlayerId() const;
	[[nodiscard]] size_t GetPupilNum() const { return GetPupilPlayerId().size(); }
	
	[[nodiscard]] boost::container::small_vector<u_long, MAX_PUPIL_NUM + 1>	GetAllMemberPlayerId() const;
	[[nodiscard]] CampusRole GetMemberLv(u_long idPlayer) const;
	[[nodiscard]] bool IsMember(u_long idPlayer) const;
	bool AddMember(u_long idPlayer, CampusRole role);
	bool RemoveMember(u_long idPlayer);	

private:
	u_long	m_idCampus = 0;
	u_long	m_idMaster = 0;
	boost::container::flat_map<u_long, CCampusMember>	m_mapCM;

#ifdef __WORLDSERVER
public:
	bool IsChangeBuffLevel(u_long idPlayer);
	[[nodiscard]] int GetBuffLevel(u_long idPlayer) const;

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
