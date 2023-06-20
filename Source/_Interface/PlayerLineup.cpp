#include "StdAfx.h"
#include "PlayerLineup.h"
#include "playerdata.h"

void PlayerLineup::SimpleDisplayer::Render(
	C2DRender * const p2DRender, CRect rect,
	PlayerLineup & displayed,
	DWORD color, const WndTListBox::DisplayArgs & misc
) const {
	if (displayed.cachedText.IsEmpty()) {
		PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(displayed.playerId);
		displayed.cachedText.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.jobs.info[pPlayerData->data.nJob].szName);
	}

	p2DRender->TextOut(rect.left, rect.top, displayed.cachedText.GetString(), color);
}

void PlayerLineup::NumberedDisplayer::Render(
	C2DRender * const p2DRender, CRect rect,
	PlayerLineup & displayed,
	DWORD color, const WndTListBox::DisplayArgs & misc
) const {
	if (displayed.cachedText.IsEmpty()) {
		PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(displayed.playerId);
		displayed.cachedText.Format("No.%d  Lv%.2d	%.16s %.10s",
			misc.i + 1,
			pPlayerData->data.nLevel, pPlayerData->szPlayer,
			prj.jobs.info[pPlayerData->data.nJob].szName
		);
	}

	p2DRender->TextOut(rect.left, rect.top, displayed.cachedText.GetString(), color);
}


template<typename P, typename L>
void PlayerLineup::ListManager<P, L>::Reset(std::span<const u_long> lineup) {
	pWndPool->ResetContent();
	pWndLineup->ResetContent();

	const CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return;

	for (const CGuildMember * pMember : pGuild->m_mapPMember | std::views::values) {
		PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
		if (pPlayerData->data.uLogin == 0) continue;

		if (!std::ranges::contains(lineup, pMember->m_idPlayer)) {
			pWndPool->Add(PlayerLineup{ pMember->m_idPlayer });
		}
	}

	for (const u_long playerId : lineup) {
		pWndLineup->Add(PlayerLineup{ playerId });
	}
}

template<typename P, typename L>
PlayerLineup::SelectReturn PlayerLineup::ListManager<P, L>::ToSelect(const RuleSet & ruleSet) {
	const auto [idSel, selPlayer] = pWndPool->GetSelection();
	if (idSel == -1) return SelectReturn::NoSelection;

	if (pWndLineup->GetSize() >= ruleSet.maxSelect.value_or(0)) {
		return SelectReturn::FullLineup;
	}

	CGuild * pGuild = g_pPlayer->GetGuild();
	if (!pGuild) return SelectReturn::NoGuild;

	CGuildMember * pGuildMember = pGuild->GetMember(selPlayer->playerId);
	if (!pGuildMember) {
		return SelectReturn::NotAMember;
	}

	const PlayerData * pd = CPlayerDataCenter::GetInstance()->GetPlayerData(pGuildMember->m_idPlayer);
	if (pd->data.nLevel < ruleSet.minimumLevel.value_or(0)) {
		return SelectReturn::TooLowLevel;
	}

	const u_long toAdd = selPlayer->playerId;
	pWndPool->Erase(idSel);

	int inSelection = pWndLineup->Find([toAdd](const PlayerLineup & p) { return p.playerId == toAdd; });

	if (inSelection != -1) {
		return SelectReturn::AlreadyInLineup;
	}

	pWndLineup->Add(PlayerLineup{ toAdd });
	return SelectReturn::Ok;
}

template<typename P, typename L>
std::optional<u_long> PlayerLineup::ListManager<P, L>::ToGuild() {
	const auto [idSel, selPlayer] = pWndLineup->GetSelection();
	if (idSel == -1) return std::nullopt;

	const u_long playerId = selPlayer->playerId;

	pWndLineup->Erase(idSel);

	int inGuild = pWndPool->Find([playerId](const PlayerLineup & p) { return p.playerId == playerId; });
	if (inGuild == -1) {
		pWndPool->Add(PlayerLineup{ playerId });
	}

	return playerId;
}

template<typename P, typename L>
void PlayerLineup::ListManager<P, L>::MoveUp() {
	const int sel = pWndLineup->GetCurSel();
	if (sel == -1 || sel == 0) return;

	const auto & [a, b] = pWndLineup->Swap(sel - 1, sel);
	if constexpr (!std::same_as<L, SimpleDisplayer>) {
		a->cachedText = "";
		b->cachedText = "";
	}
}

template<typename P, typename L>
void PlayerLineup::ListManager<P, L>::MoveDown() {
	const int sel = pWndLineup->GetCurSel();
	if (sel == -1 || sel + 1 == static_cast<int>(pWndLineup->GetSize())) return;

	const auto & [a, b] = pWndLineup->Swap(sel, sel + 1);
	if constexpr (!std::same_as<L, SimpleDisplayer>) {
		a->cachedText = "";
		b->cachedText = "";
	}
}

template struct PlayerLineup::DoubleListManager;
template struct PlayerLineup::DoubleGCListManager;
