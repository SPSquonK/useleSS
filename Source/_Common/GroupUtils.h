#pragma once

#include <ranges>

#include "party.h"
#include "guild.h"



inline auto AllMembers(const CParty & party) {
	return std::views::counted(party.m_aMember, party.GetSizeofMember())
		| std::views::transform([](const PartyMember & pm) { return prj.GetUserByID(pm.m_uPlayerId); })
		| std::views::filter(IsValidObj);
}

inline auto AllMembers(const CGuild & guild) {
	return guild.m_mapPMember
		| std::views::values
		| std::views::transform([](const CGuildMember * gm) { return prj.GetUserByID(gm->m_idPlayer); })
		| std::views::filter(IsValidObj);
}
