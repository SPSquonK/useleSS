#include "stdafx.h"
#include "Campus.h"
#include <ranges>

#ifdef __WORLDSERVER
#include "User.h"
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// CCampus Construction/Destruction
//////////////////////////////////////////////////////////////////////

size_t CCampus::GetMaxPupilNum(int campusPoints) {
	if (campusPoints < 0) return 0;
	else if (campusPoints < CCampus::MIN_LV2_POINT) return 1;
	else if (campusPoints < CCampus::MIN_LV3_POINT) return 2;
	else return 3;
}

CAr & operator<<(CAr & ar, const CCampus & campus) {
	ar << campus.m_idCampus << campus.m_idMaster;
	ar << static_cast<std::uint32_t>(campus.m_mapCM.size());
	for (const CCampus::CCampusMember & cm : campus.m_mapCM | std::views::values) {
		ar << cm;
	}
	return ar;
}

CAr & operator>>(CAr & ar, CCampus & campus) {
	ar.IsUnsafe();
	campus.m_mapCM.clear();
	std::uint32_t nSize;
	ar >> campus.m_idCampus >> campus.m_idMaster >> nSize;
	for (std::uint32_t i = 0; i < nSize; ++i) {
		CCampus::CCampusMember pMember; ar >> pMember;
		campus.m_mapCM.emplace(pMember.idPlayer, pMember);
	}
	return ar;
}

bool CCampus::IsPupil(u_long idPlayer) const {
	return GetMemberLv(idPlayer) == CampusRole::Pupil;
}

boost::container::small_vector<u_long, CCampus::MAX_PUPIL_NUM> CCampus::GetPupilPlayerId() const {
	boost::container::small_vector<u_long, CCampus::MAX_PUPIL_NUM> vecPupil;
	for (const CCampusMember & member : m_mapCM | std::views::values) {
		if (member.nMemberLv == CampusRole::Pupil) {
			vecPupil.emplace_back(member.idPlayer);
		}
	}
	return vecPupil;
}

boost::container::small_vector<u_long, CCampus::MAX_PUPIL_NUM + 1> CCampus::GetAllMemberPlayerId() const {
	boost::container::small_vector<u_long, CCampus::MAX_PUPIL_NUM + 1> vecMembers;
	for (const CCampusMember & member : m_mapCM | std::views::values) {
		vecMembers.emplace_back(member.idPlayer);
	}
	return vecMembers;
}

CampusRole CCampus::GetMemberLv(const u_long idPlayer) const {
	const auto it = m_mapCM.find(idPlayer);
	return it != m_mapCM.end() ? it->second.nMemberLv : CampusRole::Invalid;
}

bool CCampus::IsMember(const u_long idPlayer) const {
	return m_mapCM.contains(idPlayer);
}

bool CCampus::AddMember(u_long idPlayer, CampusRole role) {
	if (m_mapCM.contains(idPlayer)) {
		Error("Pupil is already campus member - idCampus : %d, idPlayer : %d", GetCampusId(), idPlayer);
		return false;
	}

	if (GetPupilNum() >= MAX_PUPIL_NUM) {
		Error("Pupil is full - idCampus : %d", GetCampusId());
		return false;
	}

	m_mapCM.emplace(idPlayer, CCampusMember{ idPlayer, role });
	return true;
}

bool CCampus::RemoveMember(const u_long idPlayer) {
	const auto it = m_mapCM.find(idPlayer);
	if (it == m_mapCM.end()) {
		Error("Member not found - idCampus : %d, idPlayer : %d", GetCampusId(), idPlayer);
		return false;
	}

	m_mapCM.erase(it);
	return true;
}

#ifdef __WORLDSERVER
bool CCampus::IsChangeBuffLevel(u_long idPlayer) {
	if (!IsMaster(idPlayer)) return false;

	const int nLevel = GetBuffLevel(idPlayer);
	if (nLevel == m_nPreBuffLevel) return false;

	m_nPreBuffLevel = nLevel;
	return true;
}

int CCampus::GetBuffLevel(u_long idPlayer) const {
	if (IsMaster(idPlayer)) {
		int nLevel = 0;

		for (const auto & member : m_mapCM | std::views::values) {
			if (member.nMemberLv == CampusRole::Pupil) {
				CUser * pPupil = g_UserMng.GetUserByPlayerID(member.idPlayer);
				if (IsValidObj(pPupil)) {
					++nLevel;
				}
			}
		}

		return nLevel;
	} else {
		const CUser * const pMaster = g_UserMng.GetUserByPlayerID(GetMaster());
		return IsValidObj(pMaster) ? 1 : 0;
	}
}
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// CCampus Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusMng::CCampusMng()
:m_idCampus( 0 )
{

}

CCampusMng::~CCampusMng()
{
	Clear();
}

void CCampusMng::Clear()
{
	for( auto it = m_mapCampus.begin(); it != m_mapCampus.end(); ++it )
		SAFE_DELETE( it->second );
	
	m_mapCampus.clear();
	m_mapPid2Cid.clear();
}

CAr & operator<<(CAr & ar, const CCampusMng & self) {
	ar << self.m_idCampus;
	
	ar << self.m_mapCampus.size();
	for (const CCampus * campus : self.m_mapCampus | std::views::values) {
		ar << *campus;
	}

	ar << self.m_mapPid2Cid.size();
	for (const auto & [idPlayer, idCampus] : self.m_mapPid2Cid) {
		ar << idPlayer << idCampus;
	}

	return ar;
}

CAr & operator>>(CAr & ar, CCampusMng & self) {
	self.Clear();
	size_t nSize;
	ar >> self.m_idCampus >> nSize;
	for (int i = 0; i < (int)(nSize); ++i) {
		CCampus * pCampus = new CCampus;
		ar >> *pCampus;
		self.m_mapCampus.emplace(pCampus->GetCampusId(), pCampus);
	}

	ar >> nSize;
	for (int i = 0; i < (int)(nSize); ++i) {
		u_long idPlayer, idCampus;
		ar >> idPlayer >> idCampus;
		self.AddPlayerId2CampusId(idPlayer, idCampus);
	}

	return ar;
}

u_long CCampusMng::AddCampus( CCampus* pCampus )
{
	m_idCampus	= ( pCampus->GetCampusId() != 0? pCampus->GetCampusId(): m_idCampus + 1 );
	if( GetCampus( m_idCampus ) )
		return 0;
	pCampus->SetCampusId( m_idCampus );
	m_mapCampus.insert( decltype(m_mapCampus)::value_type(m_idCampus, pCampus));
	return m_idCampus;
}

BOOL CCampusMng::RemoveCampus( u_long idCampus )
{
	CCampus* pCampus = GetCampus( idCampus );
	if( pCampus )
	{
		SAFE_DELETE( pCampus );
		m_mapCampus.erase( idCampus );
		return TRUE;
	}
	return FALSE;
}

CCampus* CCampusMng::GetCampus( u_long idCampus )
{
	auto it	= m_mapCampus.find( idCampus );
	if( it != m_mapCampus.end() )
		return it->second;
	
	return NULL;
}

bool CCampusMng::AddPlayerId2CampusId( u_long idPlayer, u_long idCampus )
{
	bool bResult = m_mapPid2Cid.insert( decltype(m_mapPid2Cid)::value_type(idPlayer, idCampus)).second;
	return bResult;
}

u_long CCampusMng::GetCampusIdByPlayerId( u_long idPlayer )
{
	auto it	= m_mapPid2Cid.find( idPlayer );
	if( it != m_mapPid2Cid.end() )
		return it->second;
	
	return NULL;
}
