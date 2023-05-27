#pragma once

#include <boost/container/small_vector.hpp>

#include "party.h"
#include "guild.h"

inline boost::container::small_vector<CUser *, MAX_PTMEMBER_SIZE> AllMembers(const CParty & party) {
	boost::container::small_vector<CUser *, MAX_PTMEMBER_SIZE> out;

	for (const PartyMember & pm : party.m_aMember) {
		CUser * pUser = prj.GetUserByID(pm.m_uPlayerId);
		if (IsValidObj(pUser)) {
			out.emplace_back(pUser);
		}
	}

	return out;
}

inline boost::container::small_vector<CUser *, 40> AllMembers(const CGuild & guild) {
	boost::container::small_vector<CUser *, 40> out;

	for (const auto & [_, guildMember] : guild.m_mapPMember) {
		CUser * pUser = prj.GetUserByID(guildMember->m_idPlayer);
		if (IsValidObj(pUser)) {
			out.emplace_back(pUser);
		}
	}

	return out;
}
