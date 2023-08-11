#include "StdAfx.h"
#include "data.h"
#include "sqktd/util.hpp"

DWORD SAI79::GetAttackStone(ePropType element) {
	switch (element) {
		case ePropType::FIRE:        return II_CHR_SYS_SCR_FIREASTONE;
		case ePropType::WATER:       return II_CHR_SYS_SCR_WATEILSTONE;
		case ePropType::WIND:        return II_CHR_SYS_SCR_WINDYOSTONE;
		case ePropType::ELECTRICITY: return II_CHR_SYS_SCR_LIGHTINESTONE;
		case ePropType::EARTH:       return II_CHR_SYS_SCR_EARTHYSTONE;
		default:                     return 0;
	}
}

DWORD SAI79::GetDefenseStone(ePropType element) {
	switch (element) {
		case ePropType::FIRE:        return II_CHR_SYS_SCR_DEFIREASTONE;
		case ePropType::WATER:       return II_CHR_SYS_SCR_DEWATEILSTONE;
		case ePropType::WIND:        return II_CHR_SYS_SCR_DEWINDYOSTONE;
		case ePropType::ELECTRICITY: return II_CHR_SYS_SCR_DELIGHTINESTONE;
		case ePropType::EARTH:       return II_CHR_SYS_SCR_DEEARTHYSTONE;
		default:                     return 0;
	}
}

std::optional<std::pair<SAI79::ePropType, SAI79::StoneType>> SAI79::StoneIdToElement(DWORD stoneID) {
	using P = std::pair<SAI79::ePropType, SAI79::StoneType>;
	switch (stoneID) {
		case II_CHR_SYS_SCR_FIREASTONE:      return P{ ePropType::FIRE       , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEFIREASTONE:    return P{ ePropType::FIRE       , StoneType::Defense };
		case II_CHR_SYS_SCR_WATEILSTONE:     return P{ ePropType::WATER      , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEWATEILSTONE:   return P{ ePropType::WATER      , StoneType::Defense };
		case II_CHR_SYS_SCR_WINDYOSTONE:     return P{ ePropType::WIND       , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEWINDYOSTONE:   return P{ ePropType::WIND       , StoneType::Defense };
		case II_CHR_SYS_SCR_LIGHTINESTONE:   return P{ ePropType::ELECTRICITY, StoneType::Attack  };
		case II_CHR_SYS_SCR_DELIGHTINESTONE: return P{ ePropType::ELECTRICITY, StoneType::Defense };
		case II_CHR_SYS_SCR_EARTHYSTONE:     return P{ ePropType::EARTH      , StoneType::Attack  };
		case II_CHR_SYS_SCR_DEEARTHYSTONE:   return P{ ePropType::EARTH      , StoneType::Defense };
		default:                             return std::nullopt;
	}
}

DWORD SAI79::GetResistDST(const ePropType element) {
	switch (element) {
		case ePropType::FIRE:
		default:
			return DST_RESIST_FIRE;
		case ePropType::WATER:
			return DST_RESIST_WATER;
		case ePropType::WIND:
			return DST_RESIST_WIND;
		case ePropType::ELECTRICITY:
			return DST_RESIST_ELECTRICITY;
		case ePropType::EARTH:
			return DST_RESIST_EARTH;
	}
}

DWORD SAI79::GetEleCard(const ePropType element) {
	// 속성 제련 용 카드의 종류가 
	// 속성 당 하나로 통합됨
	switch (element) {
		case ePropType::FIRE:        return II_GEN_MAT_ELE_FLAME;
		case ePropType::WATER:       return II_GEN_MAT_ELE_RIVER;
		case ePropType::ELECTRICITY: return II_GEN_MAT_ELE_GENERATOR;
		case ePropType::WIND:        return II_GEN_MAT_ELE_CYCLON;
		case ePropType::EARTH:       return II_GEN_MAT_ELE_DESERT;
		default:                     return 0;
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
