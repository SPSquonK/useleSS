#pragma once

#include <array>
#include <utility>

class CWndChatFilter final : public CWndNeuz {
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

private:
	void SetButtonStatus();

	static std::array<std::pair<UINT, DWORD>, 5> WIDToChatsty;
};
