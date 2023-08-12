#pragma once

#include <concepts>
#include <optional>

namespace SAI79
{
	enum class ePropType {
		NO_PROP = 0,
		FIRE,
		WATER,
		ELECTRICITY,
		WIND,
		EARTH
	};

	static constexpr auto NO_PROP = ePropType::NO_PROP;
	static constexpr size_t ePropTypeSize = 1 + 5;

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

	[[nodiscard]] constexpr std::optional<ePropType> From(const std::integral auto value) {
		if constexpr (std::signed_integral<decltype(value)>) {
			if (value < 0) return std::nullopt;
		}

		if (std::cmp_greater_equal(value, ePropTypeSize)) return std::nullopt;
		return static_cast<ePropType>(value);
	}

	[[nodiscard]] constexpr bool IsValid(const ePropType element) {
		const auto fromFrom = From(std::to_underlying(element));
		return fromFrom && *fromFrom == element;
	}
}

