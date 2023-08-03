#pragma once

#include <boost/container/small_vector.hpp>

#include "party.h"
#include "guild.h"

struct AllMembersImplem {
	friend auto operator|(const CParty & party, AllMembersImplem) {
		boost::container::small_vector<CUser *, MAX_PTMEMBER_SIZE> out;

		for (const PartyMember & pm : std::span(party.m_aMember, party.GetSizeofMember())) {
			CUser * pUser = prj.GetUserByID(pm.m_uPlayerId);
			if (IsValidObj(pUser)) {
				out.emplace_back(pUser);
			}
		}

		return out;
	}

	friend auto operator|(const CGuild & guild, AllMembersImplem) {
		boost::container::small_vector<CUser *, 40> out;

		for (const auto & [_, guildMember] : guild.m_mapPMember) {
			CUser * pUser = prj.GetUserByID(guildMember->m_idPlayer);
			if (IsValidObj(pUser)) {
				out.emplace_back(pUser);
			}
		}

		return out;
	}

	friend auto operator|(const CParty * pParty, AllMembersImplem) {
		return *pParty | AllMembersImplem{};
	}

	friend auto operator|(const CGuild * pGuild, AllMembersImplem) {
		return *pGuild | AllMembersImplem{};
	}
};

static constexpr auto AllMembers = AllMembersImplem{};
