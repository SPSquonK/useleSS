#pragma once

#include <array>
#include <optional>

/* Max distance to open an NPC menu */
static constexpr double MAX_LEN_MOVER_MENU = 1024.;

/* Min distance with nearest NPC to open a shop */
static constexpr double MAX_NPC_RADIUS = 9.;

class CObj;

class CNpcChecker final {
public:
	static CNpcChecker * GetInstance();
	void AddNpc(CObj * pObj);
	void RemoveNpc(CObj * pObj);

	[[nodiscard]] std::optional<bool> IsCloseNpc(int nMenu, const CObj * obj) const;
	[[nodiscard]] bool IsCloseFromAnyNpc(const CObj * obj) const;

	template<int nMenu> requires (nMenu >= 0 && nMenu < MAX_MOVER_MENU)
	[[nodiscard]] BOOL IsCloseNpc(const CObj * pObj) const {
		return HasNear(m_perMenu[nMenu], pObj, MAX_LEN_MOVER_MENU);
	}

private:
	std::array<std::set<OBJID>, MAX_MOVER_MENU> m_perMenu;
	std::set<OBJID> m_all;

	static bool HasNear(const std::set<OBJID> & objIds, const CObj * pObj, double maxDistance);
};
