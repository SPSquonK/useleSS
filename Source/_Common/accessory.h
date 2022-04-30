#pragma once

#include <array>
#include <vector>
#include "SingleDst.h"

#define	MAX_AAO		20


class CAccessoryProperty final {
public:
	using ItemId = DWORD;

	bool LoadScript(LPCTSTR szFile);
	void ChangeProbabilities(const std::vector<DWORD> & probabilities);

	[[nodiscard]] bool IsAccessory(ItemId dwItemId) const noexcept {
		return m_mapAccessory.contains(dwItemId);
	}
	[[nodiscard]] DWORD GetProbability(const int nAbilityOption) const noexcept {
		if (nAbilityOption < 0 || nAbilityOption >= MAX_AAO) return 0;
		return m_adwProbability[nAbilityOption];
	}

	[[nodiscard]] const std::vector<SINGLE_DST> * GetDst(ItemId dwItemId, int nAbilityOption) const noexcept {
		const auto it = m_mapAccessory.find(dwItemId);
		if (it == m_mapAccessory.end()) return nullptr;
		if (nAbilityOption < 0) return nullptr;
		if (it->second.size() < nAbilityOption) return nullptr;
		return &(it->second[nAbilityOption]);
	}

private:
	std::array<DWORD, MAX_AAO> m_adwProbability = { 0 };
	std::map<ItemId, std::array<std::vector<SINGLE_DST>, MAX_AAO + 1>>	m_mapAccessory;
};

extern CAccessoryProperty g_AccessoryProperty;
