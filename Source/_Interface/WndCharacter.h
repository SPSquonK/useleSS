#pragma once


class CWndCharInfo final : public CWndBase {
public:
	CWndButton		m_wndChangeJob;
	BOOL			m_fWaitingConfirm = FALSE;
	int				m_nDisplay = 1;

	CWndButton m_wndStrPlus, m_wndStrMinus;
	CWndButton m_wndStaPlus, m_wndStaMinus;
	CWndButton m_wndDexPlus, m_wndDexMinus;
	CWndButton m_wndIntPlus, m_wndIntMinus;

	CWndButton m_wndApply, m_wndReset;

	CWndEdit m_editStrCount;
	CWndEdit m_editStaCount;
	CWndEdit m_editDexCount;
	CWndEdit m_editIntCount;

	int m_nStrCount = 0;
	int m_nStaCount = 0;
	int m_nDexCount = 0;
	int m_nIntCount = 0;

	int m_nGpPoint = 0;

	int m_nATK = 0;
	int m_nDEF = 0;
	int m_nCritical = 0;
	int m_nATKSpeed = 0;

public:

	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	// BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0) override;
	// message
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	BOOL Process() override;
	void RefreshStatPoint();
	std::pair<int, int> GetVirtualATK() const;
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();

private:
	void RenderATK(C2DRender * p2DRender, int x, int y);

	static DWORD StatColor(int rawStat, int totalStat);

};

class CWndHonor final : public CWndNeuz {
private:
	int						m_nSelectedId = 0;
	std::vector<EarnedTitle>		m_vecTitle;
public:
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0) override;
	
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void RefreshList();
};

class CWndCharacter final : public CWndNeuz {
public:
	CWndCharInfo		m_wndCharInfo;
	CWndHonor			m_wndHonor;
public:
	void SerializeRegInfo(CAr & ar, DWORD & dwVersion) override;
	void OnInitialUpdate() override;
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0) override;
};

class CWndStateConfirm final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

	void SendYes();
	void CloseTheWindow();
};

