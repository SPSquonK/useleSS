#pragma once

#include "ar.h"
#include <vector>

struct SKILL {
	DWORD dwSkill;
	DWORD dwLevel;
	const ItemProp * GetProp() const;
};

using LPSKILL = SKILL *;

struct MoverSkills : public std::vector<SKILL> {
	friend CAr & operator<<(CAr & ar, const MoverSkills & self);
	friend CAr & operator>>(CAr & ar,       MoverSkills & self);
	
#if defined(__CLIENT) || defined(__WORLDSERVER)
	[[nodiscard]] static MoverSkills ForJob(int job);
#endif

	SKILL * FindBySkillId(DWORD skillId);
	const SKILL * FindBySkillId(DWORD skillId) const;
};
