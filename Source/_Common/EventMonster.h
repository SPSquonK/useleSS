#pragma once

#ifdef __EVENT_MONSTER

#include <boost/container/flat_map.hpp>

class CEventMonster final {
public:	
	struct Prop {
		int nLevel;
		DWORD dwLootTime;
		float fItemDropRange;
		BOOL bPet;
		BOOL bGiftBox;
	};

	static void LoadScript() { instance.LoadScript(); }

	[[nodiscard]] static bool IsEventMonster(const DWORD dwId) {
		return instance.IsEventMonster(dwId);
	}

	[[nodiscard]] static const Prop * GetEventMonster(const DWORD dwId) {
		return instance.GetEventMonster(dwId);
	}

private:
	struct Instance {
		void LoadScript();

		[[nodiscard]] bool IsEventMonster(const DWORD dwId) const {
			return m_mapEventMonster.contains(dwId);
		}

		[[nodiscard]] const Prop * GetEventMonster(DWORD dwId) const;

		boost::container::flat_map<DWORD, Prop> m_mapEventMonster;
	};

	static Instance instance;
};

#endif // __EVENT_MONSTER
