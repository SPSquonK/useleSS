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

	struct DoubleListManager {
		using CWndPoolList   = CWndTListBox<PlayerLineup, SimpleDisplayer>;
		using CWndLineupList = CWndTListBox<PlayerLineup, NumberedDisplayer>;

		CWndPoolList * pWndPool;
		CWndLineupList * pWndLineup;

		DoubleListManager(CWndPoolList * pWndPool, CWndLineupList * pWndLineup)
			: pWndPool(pWndPool), pWndLineup(pWndLineup) {}

		void Reset(std::span<const u_long> lineup = {});
		void ToGuild();
		SelectReturn ToSelect(const RuleSet & ruleSet);
		void MoveUp();
		void MoveDown();
	};
};
