#pragma once

#include "debug.h"
#include <optional>

static constexpr DWORD NULL_ID	= 0xffffffff;

#include "Container_FixedArray.hpp"

using EXPINTEGER = __int64;
using EXPFLOAT   = double;

#if defined(__WORLDSERVER ) || defined(__CLIENT)
// A struct for the ability to mark a location on click in the navigation.
struct NaviPoint {
	// Emphasize that this location is a location in the world, not a location on the navigator.
	D3DXVECTOR3 Pos = D3DXVECTOR3(0.f, 0.f, 0.f);
	// Time remaining in points
	short		Time = 0;
	DWORD		objid = 0xffffffff;
	std::string      Name = "";
}; 

typedef std::vector<NaviPoint> V_NaviPoint;

#endif

namespace SAI79
{
	enum	ePropType {
		NO_PROP = 0,
		FIRE,
		WATER,
		ELECTRICITY,
		WIND,
		EARTH,
		END_PROP
	};

	enum class StoneType : bool { Attack, Defense };

	[[nodiscard]] DWORD GetAttackStone(ePropType element);
	[[nodiscard]] DWORD GetDefenseStone(ePropType element);
	[[nodiscard]] std::optional<std::pair<ePropType, StoneType>> StoneIdToElement(DWORD stoneID);

	// Returns the resistance of the element: GetElementWeakTo(Fire) = Wind
	[[nodiscard]] constexpr ePropType GetElementWeakTo(SAI79::ePropType element) {
		switch (element) {
			case ePropType::FIRE:        return ePropType::WIND;
			case ePropType::WATER:       return ePropType::FIRE;
			case ePropType::WIND:        return ePropType::EARTH;
			case ePropType::ELECTRICITY: return ePropType::WATER;
			case ePropType::EARTH:       return ePropType::ELECTRICITY;
			default:                     return ePropType::NO_PROP;
		}
	}

	// Returns the weakness of the element: GetElementStrongAgainst(Fire) = Water
	[[nodiscard]] constexpr ePropType GetElementStrongAgainst(SAI79::ePropType element) {
		switch (element) {
			case ePropType::FIRE:        return ePropType::WATER;
			case ePropType::WATER:       return ePropType::ELECTRICITY;
			case ePropType::WIND:        return ePropType::FIRE;
			case ePropType::ELECTRICITY: return ePropType::EARTH;
			case ePropType::EARTH:       return ePropType::WIND;
			default:                     return ePropType::NO_PROP;
		}
	}

	[[nodiscard]] DWORD GetResistDST(ePropType element);

	[[nodiscard]] DWORD GetEleCard(ePropType element);
	[[nodiscard]] bool IsElementalCard(DWORD dwItemId);
}

