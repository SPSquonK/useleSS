#pragma once

#include <array>

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
	[[nodiscard]] BOOL IsCloseNpc(int nMenu, const CObj * obj) const;
	[[nodiscard]] BOOL IsCloseNpc(const CObj * obj) const;

private:
	std::array<std::set<OBJID>, MAX_MOVER_MENU> m_perMenu;
	std::set<OBJID> m_all;

	static bool HasNear(const std::set<OBJID> & objIds, const CObj * pObj, double maxDistance);
	static void RemoveFrom(std::set<OBJID> & set, OBJID id);
};
