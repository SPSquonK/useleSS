#pragma once 

#include "guild.h"
#include <concepts>

/// Tab window added to the guild window (set permissions for the guild house)
class CWndGuildTabPower final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	void OnInitialUpdate() override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;

	void UpdateData();

private:
	GuildPowerss m_aPowers;
	bool m_hasBeenChanged = false;

	void SetData(const GuildPowerss & dwPower);
	void EnableButton(BOOL bEnable);

	void ForEachPower(std::invocable<UINT, int, GuildPower> auto func);
	void ForEachPower(std::invocable<CWndButton &, int, GuildPower> auto func);
};
