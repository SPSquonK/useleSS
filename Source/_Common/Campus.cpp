#include "stdafx.h"
#include "Campus.h"
#include <ranges>

#ifdef __WORLDSERVER
#include "User.h"
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// CCampus Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CCampus::Clear() {
	m_mapCM.clear();
}

void CCampus::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idCampus << m_idMaster << static_cast<std::uint32_t>(m_mapCM.size());
		for (auto it = m_mapCM.begin(); it != m_mapCM.end(); ++it)
			ar << it->second;
	}
	else
	{
		ar.IsUnsafe();
		Clear();
		std::uint32_t nSize;
		ar >> m_idCampus >> m_idMaster >> nSize;
		for (std::uint32_t i = 0; i < nSize; ++i) {
			CCampusMember pMember; ar >> pMember;
			m_mapCM.emplace(pMember.idPlayer, pMember);
		}
	}
}

bool CCampus::IsPupil(u_long idPlayer) const {
	return GetMemberLv(idPlayer) == CampusRole::Pupil;
}

boost::container::small_vector<u_long, MAX_PUPIL_NUM> CCampus::GetPupilPlayerId() const {
	boost::container::small_vector<u_long, MAX_PUPIL_NUM> vecPupil;
	for (const CCampusMember & member : m_mapCM | std::views::values) {
		if (member.nMemberLv == CampusRole::Pupil) {
			vecPupil.emplace_back(member.idPlayer);
		}
	}
	return vecPupil;
}

boost::container::small_vector<u_long, MAX_PUPIL_NUM + 1> CCampus::GetAllMemberPlayerId() const {
	boost::container::small_vector<u_long, MAX_PUPIL_NUM + 1> vecMembers;
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
BOOL CCampus::IsChangeBuffLevel( u_long idPlayer )
{
	if( IsMaster( idPlayer ) )
	{
		int nLevel = GetBuffLevel( idPlayer );
		if( m_nPreBuffLevel != nLevel )
		{
			m_nPreBuffLevel = nLevel;
			return TRUE;
		}
	}
	return FALSE;
}

int CCampus::GetBuffLevel( u_long idPlayer )
{
	int nLevel = 0;
	if( IsMaster( idPlayer ) )
	{
		for (auto member : m_mapCM | std::views::values) {
			CUser* pPupil = g_UserMng.GetUserByPlayerID(member.idPlayer );
			if( IsValidObj( pPupil ) && member.nMemberLv == CampusRole::Pupil)
				++nLevel;
		}
	}
	else
	{
		CUser* pMaster = g_UserMng.GetUserByPlayerID( GetMaster() );
		if( IsValidObj( pMaster ) )
			++nLevel;
	}
	return nLevel;
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

void CCampusMng::Serialize( CAr & ar )
{
	int	i = 0;
	
	if( ar.IsStoring() )
	{
		ar << m_idCampus << m_mapCampus.size();
		for( auto it = m_mapCampus.begin(); it != m_mapCampus.end(); ++it )
			( it->second )->Serialize( ar );

		ar << m_mapPid2Cid.size();
		for( auto it2 = m_mapPid2Cid.begin(); it2 != m_mapPid2Cid.end(); ++it2 )
			ar << it2->first << it2->second;

	}
	else
	{
		Clear();
		size_t nSize;
		ar >> m_idCampus >> nSize;
		for( i = 0; i < (int)( nSize ); ++i )
		{
			CCampus* pCampus = new CCampus;
			pCampus->Serialize( ar );
			m_mapCampus.insert( decltype(m_mapCampus)::value_type(pCampus->GetCampusId(), pCampus));
		}
		ar >> nSize;
		for( i = 0; i < (int)( nSize ); ++i )
		{
			u_long idPlayer, idCampus;
			ar >> idPlayer >> idCampus;
			AddPlayerId2CampusId( idPlayer, idCampus );
		}
	}
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
