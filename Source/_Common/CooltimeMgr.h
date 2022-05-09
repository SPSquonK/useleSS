#pragma once

#include <array>

struct ItemProp;

// Class that manages cooldown information
class CCooltimeMgr {
public:
	static constexpr size_t MAX_COOLTIME_TYPE = 3;
	
	struct Cooldown {
		DWORD time = 0;	// End of cooldown
		DWORD base = 0; // Event occurrence time (time the item was eaten)
	};

public:
	static DWORD GetGroup(const ItemProp * pItemProp);
	
	[[nodiscard]] bool CanUse(const ItemProp & itemProp, DWORD * groupPtr = nullptr) const {
		const auto group = GetGroup(&itemProp);
		if (groupPtr) *groupPtr = group;
		return group == 0 || CanUse(group);
	}

	void SetTime(DWORD dwGroup, DWORD dwCoolTime);
	
	[[nodiscard]] DWORD GetTime(const DWORD dwGroup) const {
		ASSERT(dwGroup > 0);
		return m_cds[dwGroup - 1].time;
	}

	[[nodiscard]] DWORD GetBase(const DWORD dwGroup) const {
		ASSERT(dwGroup > 0);
		return m_cds[dwGroup - 1].base;
	}

private:
	[[nodiscard]] bool CanUse(DWORD dwGroup) const;

	std::array<Cooldown, MAX_COOLTIME_TYPE> m_cds = {};
};
