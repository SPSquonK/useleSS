#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/container/small_vector.hpp>
#include <optional>
#include <memory>
#include <variant>

class CWndSkill_16 : public CWndNeuz {
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

	enum class SkillStatus { No, Learnable, Usable };

public:
	[[nodiscard]] static bool IsSkillHigherThanReal(const SKILL & windowSkill);
	
	// Return the relative icon location in the panel
	[[nodiscard]] static std::optional<JobSkillPositionInfo> GetSkillIconInfo(DWORD dwSkillID);

	[[nodiscard]] static const char * GetFileNameClassBtn(int nJob);
	[[nodiscard]] static const char * GetBackgroundFilename(int nJob);
	[[nodiscard]] static const char * GetHeroBackground(int nJob);
	[[nodiscard]] static boost::container::static_vector<DWORD, 4> JobToTabJobs(int nJob);

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
	SKILL * GetdwSkill(DWORD dwSkill);
	
	void	SubSkillPointDown(LPSKILL lpSkill);

	int		GetCurrSkillPoint() { return m_nCurrSkillPoint; }

protected:
	// Return the absolute icon location in the window
	std::optional<CRect> GetSkillIconRect(DWORD dwSkillID);
	
	void	AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4);
	static int GetJobByJobLevel(SelectedTab jobLevel, int currentJob);
	
	static std::optional<SelectedTab> GetMaxTabFromJob(int Job);
	
	[[nodiscard]] SkillStatus GetSkillStatus(const SKILL & skill) const;

private:
	void InitItem_FillJobNames();
	void InitItem_LoadTextureSkillIcon();
	void InitItem_AutoControlClassBtn();

private:
	boost::container::flat_map<DWORD, CTexture *> m_pTexSkill;

	SelectedTab m_selectedTab = SelectedTab::Vagrant;
	
	MoverSkills m_apSkills;
	int m_nCurrSkillPoint = 0;

	// Picture for skill level number display (1, 2, ..., max)
	CTexturePack m_kTexLevel;
	
	
	SKILL * m_pFocusItem = nullptr;				//Selected item: In other words, a skill item that can be leveled up by double-clicking the skill.
	
	// Current skill tree image
	std::unique_ptr<IMAGE> m_pTexJobPannel = nullptr;			
	
	CWndButton * m_buttonPlus  = nullptr;
	CWndButton * m_buttonMinus = nullptr;
	CWndButton * m_buttonOk    = nullptr;
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

