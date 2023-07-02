#pragma once

class CWndCharInfo final : public CWndBase {
public:
	enum class TooltipBox { Str, Sta, Dex, Int, Gp, Status, Edit, Apply, Reset };
	enum class LoadedIcon { None, Master60, Master70, Master80, Master90, Master100, Master110, Hero };

	struct Hoverable {
		CRect rect;
		TooltipBox box;
	};

	struct StatChange {
		CWndButton plus;
		CWndButton minus;
		CWndEdit edit;
		int count = 0;

		void Setup(bool hasGp);
		void Update(int gp);
		bool Handle(UINT nID, UINT messagen, CWndCharInfo & parent);
	};

	static constexpr DWORD TitleColor        = D3DCOLOR_ARGB(255, 0, 0, 0);
	static constexpr DWORD LabelColor        = D3DCOLOR_ARGB(255, 0, 0, 180);
	static constexpr DWORD RegularValueColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	static constexpr DWORD ChangeValueColor  = D3DCOLOR_ARGB(255, 255, 0, 0);

public:
	CWndButton		m_wndChangeJob;
	BOOL			m_fWaitingConfirm = FALSE;
	int				m_nDisplay = 1;

	StatChange m_str;
	StatChange m_sta;
	StatChange m_dex;
	StatChange m_int;

	CWndButton m_wndApply, m_wndReset;

	int m_nGpPoint = 0;

	CTexture * m_masterIcon = nullptr;
	LoadedIcon m_currentMasterIcon = LoadedIcon::None;

public:

	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	BOOL Process() override;
	std::pair<int, int> GetVirtualATK() const;
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();

	void ResetCount();

private:
	void RenderATK(C2DRender * p2DRender, int x, int y);

	static DWORD StatColor(int rawStat, int totalStat);

	void DrawCharacterBase(C2DRender * p2DRender);
	void DrawStats(C2DRender * p2DRender);
	void DrawPvp(C2DRender * p2DRender);

	void CheckAndDrawTooltip();

	[[nodiscard]] int GetAttributedTotal() const;
	CTexture * UpdateAndGetMasterIcon();
};

class CWndHonor final : public CWndNeuz {
private:
	int m_nSelectedId = 0;
	std::vector<int> m_vecTitle;
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

