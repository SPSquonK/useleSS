#pragma once 

#include "guild.h"
#include <concepts>

class CWndGuildTabPower : public CWndNeuz {
	// 길드윈도우에서 추가되는 탭 윈도우 ( 길드 하우스에 관한 권한 설정 )
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	void OnInitialUpdate() override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;

	void UpdateData();
	void SetData(const GuildPowerss & dwPower);
	void EnableButton(BOOL bEnable);

private:
	GuildPowerss m_aPowers;
	BOOL m_bChanedCheckBox = FALSE;
	bool m_hasBeenChanged = false;

	void ForEachPower(std::invocable<UINT, int, GuildPower> auto func);
	void ForEachPower(std::invocable<CWndButton &, int, GuildPower> auto func);
};
