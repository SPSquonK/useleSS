#include "StdAfx.h"
#include "ItemMorph.h"
#include <format>

namespace ItemMorph {

	template<typename Implem> std::string GenericBuildListOfExistingMorphs();

	//////////////////////////////////////////////////////////////////////////////
	// Vanilla Morph

  const ItemProp * VanillaMorph::GetTransyItem(const ItemProp & toMorph) {
		if (!_IsPotentiallyMorphable(toMorph)) {
			return nullptr;
		}

		// Note: we iterate from end to start to be able to return early without
		// modifying the default behaviour (which is mapping to the farthest possible
		// item)

		for (const ItemProp & otherItem : prj.m_aPropItem) {
			if (!_IsPotentiallyMorphable(otherItem)) continue;

			if (IsMorphableTo(toMorph, otherItem)) {
				return &otherItem;
			}
		}

		return nullptr;
  }

	bool VanillaMorph::_IsPotentiallyMorphable(const ItemProp & pItemProp) {
		return pItemProp.dwItemKind2 == IK2_ARMOR || pItemProp.dwItemKind2 == IK2_ARMORETC
			&& (pItemProp.dwItemSex == SEX_MALE || pItemProp.dwItemSex == SEX_FEMALE);
	}

	bool VanillaMorph::IsMorphableTo(const ItemProp & lhs, const ItemProp & rhs) {
		if (!_IsPotentiallyMorphable(lhs)) return false;
		if (!_IsPotentiallyMorphable(rhs)) return false;
		
		// Opposite gender
		// This also ensures that the two items are different
		const bool maleFemale = lhs.dwItemSex == SEX_MALE && rhs.dwItemSex == SEX_FEMALE;
		const bool femaleMale = lhs.dwItemSex == SEX_FEMALE && rhs.dwItemSex == SEX_MALE;
		if (!maleFemale && !femaleMale) return false;

		// Same item charactistics
		const bool hasSimilarCharacteristics =
			lhs.dwItemKind1 == rhs.dwItemKind1
			&& lhs.dwItemKind2 == rhs.dwItemKind2
			&& lhs.dwItemKind3 == rhs.dwItemKind3

			&& lhs.dwItemJob == rhs.dwItemJob
			&& lhs.dwHanded == rhs.dwHanded
			&& lhs.dwParts == rhs.dwParts

			&& lhs.dwItemJob == rhs.dwItemJob
			&& lhs.dwHanded == rhs.dwHanded
			&& lhs.dwParts == rhs.dwParts

			&& lhs.dwItemLV == rhs.dwItemLV
			&& lhs.dwAbilityMin == rhs.dwAbilityMin
			&& lhs.dwAbilityMax == rhs.dwAbilityMax
			&& lhs.fAttackSpeed == rhs.fAttackSpeed;

		if (!hasSimilarCharacteristics) return false;

		// Set checking
		// TODO: check if the two sets have the same bonuses
		const bool lhsHasSet = g_SetItemFinder.GetSetItemByItemId(lhs.dwID) != nullptr;
		const bool rhsHasSet = g_SetItemFinder.GetSetItemByItemId(rhs.dwID) != nullptr;
		
		return lhsHasSet == rhsHasSet;
	}

	std::string VanillaMorph::BuildListOfExistingMorphs() {
		return GenericBuildListOfExistingMorphs<VanillaMorph>();
	}
	
	//////////////////////////////////////////////////////////////////////////////
	// Reflexive Morph


	const ItemProp * ReflexiveMorph::GetTransyItem(const ItemProp & toMorph) {
		return nullptr;
	}

	bool ReflexiveMorph::_IsPotentiallyMorphable(const ItemProp & pItemProp) {
		return true;
	}

	bool ReflexiveMorph::IsMorphableTo(const ItemProp & lhs, const ItemProp & rhs) {

		return false;
	}




	std::string ReflexiveMorph::BuildListOfExistingMorphs() {
		return GenericBuildListOfExistingMorphs<VanillaMorph>();
	}

	/////////////////////////////////////////////////////////////////////////////

	// Generic implementation of BuildListOfExistingMorphs that uses the other
	// methods to generate the list of morphs.
	template<typename Implem>
	std::string GenericBuildListOfExistingMorphs() {
		std::string output;

		std::vector<const char *> canBeMorphedTo;
		canBeMorphedTo.reserve(4);

		for (const ItemProp & self : prj.m_aPropItem) {
			canBeMorphedTo.clear();

			for (const ItemProp & other : prj.m_aPropItem) {
				if (Implem::IsMorphableTo(self, other)) {
					canBeMorphedTo.push_back(other.szName);
				}
			}

			if (canBeMorphedTo.empty()) {
				canBeMorphedTo.emplace_back("NULL");
			}

			if (canBeMorphedTo.size() == 1) {
				output += std::format("{} -> {}\n", self.szName, canBeMorphedTo[0]);
			} else {
				for (size_t index = 0; index != canBeMorphedTo.size(); ++index) {
					output += std::format("{} -> {} ({})\n", self.szName, canBeMorphedTo[index], index + 1);
				}
			}
		}

		return output;
	}

}

namespace UI {
	std::optional<ChangeItemId> ChangeItemId::MorphItem(const CItemElem & itemElem) {
		const ItemProp * prop = itemElem.GetProp();
		if (!prop) return std::nullopt;
		const ItemProp * towards = ItemMorph::GetTransyItem(*prop);
		if (!towards) return std::nullopt;

		return ChangeItemId(towards->dwID);
	}
}
