#pragma once

class CWndRoomList : public CWndNeuz {
public:
	struct Item {
		CString text;
		u_long playerId;
	};

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
	void Refresh();
};

class CWndQuitRoom final : public CWndNeuz {
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};
