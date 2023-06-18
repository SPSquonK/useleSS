#pragma once

#include <array>
#include <utility>

class CWndChatFilter final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

private:
	void SetButtonStatus();

	[[nodiscard]] static int GetChannelForChatsty(int chatsty);
	static std::array<std::pair<UINT, DWORD>, 5> WIDToChatsty;
};
