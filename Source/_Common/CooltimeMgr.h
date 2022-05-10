#pragma once

#include <array>
#include <optional>

struct ItemProp;

// Class that manages cooldown information
class CCooltimeMgr {
public:
	static constexpr size_t MAX_COOLTIME_TYPE = 3;
	
	struct Cooldown {
		DWORD time = 0;	// End of cooldown
		DWORD base = 0; // Event occurrence time (time the item was eaten)
	};

	struct RemainingCooldown {
		DWORD elapsedTime;
		DWORD totalWait;
	};

public:
	enum class CooldownType { NoCooldownAtAll, OnCooldown, Available };

	[[nodiscard]] CooldownType CanUse(const ItemProp & itemProp) const {
		const auto group = GetGroup(itemProp);
		if (group == 0) return CooldownType::NoCooldownAtAll;
		if (!CanUse(group)) return CooldownType::OnCooldown;
		return CooldownType::Available;
	}

	void StartCooldown(const ItemProp & itemProp);

	[[nodiscard]] std::optional<RemainingCooldown>
		GetElapsedTime(const ItemProp & itemProp) const;

	[[nodiscard]] DWORD GetRemainingTime(const ItemProp & itemProp) const;

private:
	static DWORD GetGroup(const ItemProp & pItemProp);

	[[nodiscard]] bool CanUse(DWORD dwGroup) const;

	std::array<Cooldown, MAX_COOLTIME_TYPE> m_cds = {};
};
