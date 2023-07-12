#pragma once

#include <optional>
#include <span>
#include "WndTListBox.hpp"

struct PlayerLineup {
	u_long playerId;
	CString cachedText;

	struct SimpleDisplayer {
		void Render(
			C2DRender * const p2DRender, CRect rect,
			PlayerLineup & displayed,
			DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	struct NumberedDisplayer {
		void Render(
			C2DRender * const p2DRender, CRect rect,
			PlayerLineup & displayed,
			DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	enum class SelectReturn {
		NoSelection, 
		FullLineup, NoGuild, NotAMember, TooLowLevel,
		AlreadyInLineup, Ok
	};

	struct RuleSet {
		std::optional<size_t> maxSelect;
		std::optional<unsigned int> minimumLevel;
	};

	template<typename PoolDisplayer, typename LineupDisplayer>
	struct ListManager {
		using CWndPoolList   = CWndTListBox<PlayerLineup, PoolDisplayer>;
		using CWndLineupList = CWndTListBox<PlayerLineup, LineupDisplayer>;

		CWndPoolList * pWndPool;
		CWndLineupList * pWndLineup;

		ListManager(CWndPoolList * pWndPool, CWndLineupList * pWndLineup)
			: pWndPool(pWndPool), pWndLineup(pWndLineup) {}

		void Reset(std::span<const u_long> lineup = {});
		std::optional<u_long> ToGuild();
		SelectReturn ToSelect(const RuleSet & ruleSet);
		void MoveUp();
		void MoveDown();
	};

	using DoubleListManager = ListManager<SimpleDisplayer, NumberedDisplayer>;
	using DoubleGCListManager = ListManager<SimpleDisplayer, SimpleDisplayer>;
};
