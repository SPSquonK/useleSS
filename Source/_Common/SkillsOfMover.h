#pragma once

struct SKILL {
	DWORD dwSkill;
	DWORD dwLevel;
	const ItemProp * GetProp() const;
};

using LPSKILL = SKILL *;

