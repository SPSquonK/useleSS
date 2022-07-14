#pragma once

#include <vector>


class CWndSkillTreeEx : public CWndNeuz {
protected:
	int  m_nCount = 0;					//현재 직업에 해당하는 스킬 총 갯수

	int  m_nCurSelect = -1;			//잉 안쓰고있는데?
	BOOL m_bDrag = FALSE;					//마우스로 클릭했는데 스킬아이콘 영역 안 인 경우 TRUE
	int				m_nCurrSkillPoint = 0;		//현재 남은 스킬포인트
	CTexturePack	m_textPackNum;			//스킬 레벨숫자 표시용 그림 ( 1, 2,..... max )
	DWORD			m_dwMouseSkill = 0;			//마우스에 위치한 스킬
	CWndButton * m_pWndButton[4];		//+, -, reset, finish

	MoverSkills m_skills;				//스킬 목록
	std::vector<CTexture * > m_skillsTexture;

	CTexture * m_aSkillLevel[3] = { nullptr, nullptr, nullptr };
	IMAGE * m_atexJobPannel[2] = { nullptr, nullptr };
	CString		  m_strHeroSkilBg;			//히어로 이미지 파일 이름
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
	[[nodiscard]] int GetCurrSkillPoint() const noexcept { return m_nCurrSkillPoint; }
	void	SubSkillPointDown(SKILL * lpSkill);

	int		GetCalcImagePos(int nIndex);
	[[nodiscard]] int GetCurSelect() const { return m_nCurSelect; }
	SKILL * GetFocusedItem() { return GetSkill(m_nCurSelect); }

	BOOL	GetSkillPoint(DWORD dwSkillID, CRect & rect);
	LPSKILL GetSkill(int i);
	void LoadTextureSkillicon();
	void InitItem();

	const MoverSkills & GetSkills() const { return m_skills; };

	virtual ~CWndSkillTreeEx();

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

private:
	[[nodiscard]] bool IsDownPoint(const SKILL & skill) const; // Return true if the skill can be down leveled
	[[nodiscard]] bool CheckSkill(int i);
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

