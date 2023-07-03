#pragma once

class CWndSealChar final : public CWndNeuz {
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndSealCharSelect final : public CWndNeuz {
public:
	struct Target {
		char name[MAX_NAME] = "";
		OBJID id;
		LONG slot;
	};

	static void OpenOrResetWindow(std::vector<Target> targets);
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

private:
	CWndSealCharSelect(std::vector<Target> targets)
		: m_targets(std::move(targets)) {
	}
	void UpdateRadioButtons();

	std::vector<Target> m_targets;
};

class CWndSealCharSend final : public CWndNeuz {
public:
	static void OpenOrResetWindow(const CWndSealCharSelect::Target & target);

	CWndSealCharSelect::Target m_target;

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

private:
	CWndSealCharSend() = default;
	void UpdateTarget(const CWndSealCharSelect::Target & target);
};

class CWndSealCharSet final : public CWndNeuz {
public:
	static void OpenOrResetWindow(DWORD scrollPosition);
	DWORD m_scrollPos = 0;

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
private:
	CWndSealCharSet() = default;
};
