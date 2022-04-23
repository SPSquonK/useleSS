#pragma once

#include <ranges>

#include "party.h"



inline auto AllMembers(const CParty & party) {
	return std::views::counted(party.m_aMember, party.GetSizeofMember())
		| std::views::transform([](const PartyMember & pm) { return prj.GetUserByID(pm.m_uPlayerId); })
		| std::views::filter(IsValidObj);
}
