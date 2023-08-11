#include "StdAfx.h"
#include "data.h"
#include "sqktd/util.hpp"

DWORD SAI79::GetAttackStone(ePropType element) {
	switch (element) {
		case SAI79::FIRE:        return II_CHR_SYS_SCR_FIREASTONE;
		case SAI79::WATER:       return II_CHR_SYS_SCR_WATEILSTONE;
		case SAI79::WIND:        return II_CHR_SYS_SCR_WINDYOSTONE;
		case SAI79::ELECTRICITY: return II_CHR_SYS_SCR_LIGHTINESTONE;
		case SAI79::EARTH:       return II_CHR_SYS_SCR_EARTHYSTONE;
		default:                 return 0;
	}
}

DWORD SAI79::GetDefenseStone(ePropType element) {
	switch (element) {
		case SAI79::FIRE:        return II_CHR_SYS_SCR_DEFIREASTONE;
		case SAI79::WATER:       return II_CHR_SYS_SCR_DEWATEILSTONE;
		case SAI79::WIND:        return II_CHR_SYS_SCR_DEWINDYOSTONE;
		case SAI79::ELECTRICITY: return II_CHR_SYS_SCR_DELIGHTINESTONE;
		case SAI79::EARTH:       return II_CHR_SYS_SCR_DEEARTHYSTONE;
		default:                 return 0;
	}
}

std::optional<std::pair<SAI79::ePropType, SAI79::StoneType>> SAI79::StoneIdToElement(DWORD stoneID) {
	using P = std::pair<SAI79::ePropType, SAI79::StoneType>;
	switch (stoneID) {
		case II_CHR_SYS_SCR_FIREASTONE:      return P{ SAI79::FIRE       , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEFIREASTONE:    return P{ SAI79::FIRE       , StoneType::Defense };
		case II_CHR_SYS_SCR_WATEILSTONE:     return P{ SAI79::WATER      , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEWATEILSTONE:   return P{ SAI79::WATER      , StoneType::Defense };
		case II_CHR_SYS_SCR_WINDYOSTONE:     return P{ SAI79::WIND       , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEWINDYOSTONE:   return P{ SAI79::WIND       , StoneType::Defense };
		case II_CHR_SYS_SCR_LIGHTINESTONE:   return P{ SAI79::ELECTRICITY, StoneType::Attack  };
		case II_CHR_SYS_SCR_DELIGHTINESTONE: return P{ SAI79::ELECTRICITY, StoneType::Defense };
		case II_CHR_SYS_SCR_EARTHYSTONE:     return P{ SAI79::EARTH      , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEEARTHYSTONE:   return P{ SAI79::EARTH      , StoneType::Defense };
		default:                             return std::nullopt;
	}
}

SAI79::ePropType SAI79::GetElementWeakTo(ePropType element) {
	switch (element) {
		case ePropType::FIRE:        return ePropType::WIND;
		case ePropType::WATER:       return ePropType::FIRE;
		case ePropType::WIND:        return ePropType::EARTH;
		case ePropType::ELECTRICITY: return ePropType::WATER;
		case ePropType::EARTH:       return ePropType::ELECTRICITY;
		default:                     return ePropType::NO_PROP;
	}
}

SAI79::ePropType SAI79::GetElementStrongAgainst(ePropType element) {
	switch (element) {
		case ePropType::FIRE:        return ePropType::WIND;
		case ePropType::WATER:       return ePropType::FIRE;
		case ePropType::WIND:        return ePropType::EARTH;
		case ePropType::ELECTRICITY: return ePropType::WATER;
		case ePropType::EARTH:       return ePropType::ELECTRICITY;
		default:                     return ePropType::NO_PROP;
	}
}

DWORD SAI79::GetResistDST(const ePropType element) {
	switch (element) {
		case SAI79::FIRE:
		default:
			return DST_RESIST_FIRE;
		case SAI79::WATER:
			return DST_RESIST_WATER;
		case SAI79::WIND:
			return DST_RESIST_WIND;
		case SAI79::ELECTRICITY:
			return DST_RESIST_ELECTRICITY;
		case SAI79::EARTH:
			return DST_RESIST_EARTH;
	}
}

DWORD SAI79::GetEleCard(const ePropType element) {
	// 속성 제련 용 카드의 종류가 
	// 속성 당 하나로 통합됨
	switch (element) {
		case SAI79::FIRE:        return II_GEN_MAT_ELE_FLAME;
		case SAI79::WATER:       return II_GEN_MAT_ELE_RIVER;
		case SAI79::ELECTRICITY: return II_GEN_MAT_ELE_GENERATOR;
		case SAI79::WIND:        return II_GEN_MAT_ELE_CYCLON;
		case SAI79::EARTH:       return II_GEN_MAT_ELE_DESERT;
		default:                 return 0;
	}
}

bool SAI79::IsElementalCard(DWORD dwItemId) {
	return sqktd::is_among(dwItemId,
		II_GEN_MAT_ELE_FLAME,
		II_GEN_MAT_ELE_RIVER,
		II_GEN_MAT_ELE_GENERATOR,
		II_GEN_MAT_ELE_CYCLON,
		II_GEN_MAT_ELE_DESERT
	);
}
