#pragma once

#include <vector>
#include <boost/container/flat_map.hpp>
#include <boost/container/small_vector.hpp>
#include <optional>
#include <memory>
#include <variant>

class CWndSkillTreeCommon : public CWndNeuz {
public:
	[[nodiscard]] static bool IsSkillHigherThanReal(const SKILL & windowSkill);
	[[nodiscard]] static const char * GetBackgroundFilename(int nJob);
	[[nodiscard]] static const char * GetHeroBackground(int nJob);
	[[nodiscard]] static boost::container::static_vector<DWORD, 4> JobToTabJobs(int nJob);

public:
	static void ReInitIfOpen();

	enum class SkillStatus { No, Learnable, Usable };
	[[nodiscard]] SkillStatus GetSkillStatus(const SKILL & skill) const;
	[[nodiscard]] CTexture * GetTextureOf(const SKILL & skill) const;

	[[nodiscard]] const MoverSkills & GetSkills() const { return m_apSkills; };
	[[nodiscard]] int GetCurrSkillPoint() const noexcept { return m_nCurrSkillPoint; }
	[[nodiscard]] const SKILL * GetFocusedItem() const { return m_pFocusItem; }

	void ResetSkills();

	void OnSkillPointDown(const SKILL & reducedSkill);

public:
	MoverSkills m_apSkills;
	int m_nCurrSkillPoint = 0;

protected:
	SKILL * m_pFocusItem = nullptr;

private:
	boost::container::flat_map<DWORD, CTexture *> m_pTexSkill;
};


class CWndSkillTreeEx : public CWndSkillTreeCommon {
protected:
	DWORD			m_dwMouseSkill = 0;			//마우스에 위치한 스킬
	SKILL * m_focusedSkill = nullptr;


	BOOL m_bDrag = FALSE;					//마우스로 클릭했는데 스킬아이콘 영역 안 인 경우 TRUE
	CTexturePack	m_textPackNum;			//스킬 레벨숫자 표시용 그림 ( 1, 2,..... max )
	CWndButton * m_pWndButton[4];		//+, -, reset, finish

	CTexture * m_aSkillLevel[3] = { nullptr, nullptr, nullptr };
	IMAGE * m_atexJobPannel[2] = { nullptr, nullptr };
	const char * m_strHeroSkilBg = nullptr;			//히어로 이미지 파일 이름
	int m_nJob = -1;			//class 번호

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge = nullptr;

	CTexture * m_atexTopDown[2] = { nullptr, nullptr };
	int 	m_nTopDownGap = 0;
	BOOL m_bSlot[4] = { TRUE, TRUE, TRUE, TRUE };

	BOOL m_bLegend = FALSE;							//전승
	CWndStatic * m_pWndHeroStatic[2] = { nullptr, nullptr };

public:
	

	int		GetCalcImagePos(int nIndex);

	BOOL	GetSkillPoint(DWORD dwSkillID, CRect & rect);
	void LoadTextureSkillicon();
	void InitItem();


	[[nodiscard]] const SKILL * GetFocusedSkill() const { return m_focusedSkill; }

	void AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4);
	void SetJob(int nJob);
	virtual BOOL Process();
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	// message
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
	void SetActiveSlot(int nSlot, BOOL bFlag);
	virtual void SerializeRegInfo(CAr & ar, DWORD & dwVersion);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
};


class CWndReSkillWarning : public CWndNeuz {
	bool	m_bParentDestroy;
public:
	CWndReSkillWarning(bool parentDestroy) : m_bParentDestroy(parentDestroy) {}
	virtual ~CWndReSkillWarning();

	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnDestroy();
};


class CWndSkill_16 : public CWndSkillTreeCommon {
public:
	// Utility classes and enums

	enum class SelectedTab : size_t { Vagrant, Expert, Pro, LegendHero };

	struct TabPosition {
		SelectedTab tab; CPoint point;
		TabPosition(SelectedTab tab, int x, int y) : tab(tab), point(x, y) {}
	};
	struct HeroPosition {};
	struct MasterPosition {};

	using JobSkillPositionInfo = std::variant<TabPosition, MasterPosition, HeroPosition>;


public:
	// Return the relative icon location in the panel
	[[nodiscard]] static std::optional<JobSkillPositionInfo> GetSkillIconInfo(DWORD dwSkillID);

	[[nodiscard]] static const char * GetFileNameClassBtn(int nJob);

public:
	CWndSkill_16() = default;
	~CWndSkill_16() override = default;

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	void OnInitialUpdate() override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	BOOL Process() override;
	void OnMouseWndSurface(CPoint point) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnMouseMove(UINT nFlags, CPoint point) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;

	HRESULT DeleteDeviceObjects() override;

//exposed
	void	InitItem();

protected:
	// Return the absolute icon location in the window
	std::optional<CRect> GetSkillIconRect(DWORD dwSkillID);

	void	AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4);
	static int GetJobByJobLevel(SelectedTab jobLevel, int currentJob);

	static std::optional<SelectedTab> GetMaxTabFromJob(int Job);

	

private:
	void InitItem_FillJobNames();
	void InitItem_LoadTextureSkillIcon();
	void InitItem_AutoControlClassBtn();

private:

	SelectedTab m_selectedTab = SelectedTab::Vagrant;


	// Picture for skill level number display (1, 2, ..., max)
	CTexturePack m_kTexLevel;

	// Current skill tree image
	std::unique_ptr<IMAGE> m_pTexJobPannel = nullptr;

	CWndButton * m_buttonPlus = nullptr;
	CWndButton * m_buttonMinus = nullptr;
	CWndButton * m_buttonOk = nullptr;
	CWndButton * m_buttonReset = nullptr;

	DWORD			m_dwMouseSkill = NULL_ID; // Skill hovered by mouse

	// Hero skill image filename
	const char * m_strHeroSkilBg = "";

	// True if player is >= master
	bool m_bLegend = false;

	// TRUE if clicked with the mouse is not in the skill icon area
	bool m_bDrag = false;

	// Background of the selected skill
	CTexture * m_pTexSeletionBack = nullptr;
};

using CWndSkillTree = CWndSkillTreeEx;
