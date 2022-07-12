#include "StdAfx.h"
#include "WndField.h"
#include "AppDefine.h"
#include "defineSkill.h"
#include "defineText.h"

/////////////

CWndReSkillWarning::~CWndReSkillWarning() {
	Destroy();
}

void CWndReSkillWarning::InitItem(BOOL bParentDestroy) {
	m_bParentDestroy = bParentDestroy;
}

void CWndReSkillWarning::OnDestroy() {
	if (m_bParentDestroy) {
		g_WndMng.GetWndBase(APP_SKILL3)->Destroy();
	}
}

void CWndReSkillWarning::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	if (CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_CONTEXT)) {
		pWndEdit->SetString(_T(prj.GetText(TID_GAME_SKILLLEVEL_CHANGE)));
		pWndEdit->EnableWindow(FALSE);
	}

	m_bParentDestroy = FALSE;

	MoveParentCenter();
}

BOOL CWndReSkillWarning::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_QUEITMWARNING, pWndParent, WBS_MODAL, CPoint(0, 0));
}


//////////////

CWndSkillTreeEx::~CWndSkillTreeEx() {
	DeleteDeviceObjects();
}

void CWndSkillTreeEx::SerializeRegInfo(CAr & ar, DWORD & dwVersion) {
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	if (ar.IsLoading()) {
		ar >> m_bSlot[0] >> m_bSlot[1] >> m_bSlot[2] >> m_bSlot[3];
		BOOL bFlag[4];

		bFlag[0] = m_bSlot[0];
		bFlag[1] = m_bSlot[1];
		bFlag[2] = m_bSlot[2];
		bFlag[3] = m_bSlot[3];

		m_bSlot[0] = m_bSlot[1] = m_bSlot[2] = m_bSlot[3] = TRUE;

		SetActiveSlot(0, bFlag[0]);
		SetActiveSlot(1, bFlag[1]);
		SetActiveSlot(2, bFlag[2]);
		if (m_bLegend) {
			if (FULLSCREEN_HEIGHT == 600) {
				if (bFlag[3])
					SetActiveSlot(2, FALSE);
			} else
				SetActiveSlot(3, bFlag[3]);
		}
	} else {
		dwVersion = 0;
		ar << m_bSlot[0] << m_bSlot[1] << m_bSlot[2] << m_bSlot[3];
	}
}

void CWndSkillTreeEx::SetActiveSlot(int nSlot, BOOL bFlag) {
	CRect rect = GetWindowRect(TRUE);

	if (nSlot == 0) {
		if (m_bSlot[0] == bFlag)
			return;

		CWndStatic * lpWndStatic;
		lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC3);

		DWORD dwValue = 48;

		if (bFlag) {
			CWndStatic * lpWndStatic;
			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC3);
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect(0, dwValue);
			lpWndStatic->SetWndRect(crect, TRUE);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC4);
			lpWndStatic->SetVisible(TRUE);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC5);
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move(crect.left, crect.top + dwValue);
			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC7);
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move(crect.left, crect.top + dwValue);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC8);
			lpWndStatic->SetVisible(TRUE);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move(crect.left, crect.top + dwValue);
			if (m_bLegend) {
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move(crect.left, crect.top + dwValue);

				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move(crect.left, crect.top + dwValue);
			}
			int n = rect.Height();
			rect.bottom += dwValue;
		} else {
			CWndStatic * lpWndStatic;
			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC3);
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect(0, ((-1) * (int)(dwValue)));
			lpWndStatic->SetWndRect(crect, TRUE);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC4);
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC5);
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move(crect.left, crect.top - dwValue);
			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC7);
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move(crect.left, crect.top - dwValue);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC8);
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move(crect.left, crect.top - dwValue);

			if (m_bLegend) {
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move(crect.left, crect.top - dwValue);

				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move(crect.left, crect.top - dwValue);
			}

			int n = rect.Height();
			rect.bottom -= dwValue;

		}

		SetWndRect(rect);

		m_bSlot[0] = bFlag;
	} else
		if (nSlot == 1) {
			if (m_bSlot[1] == bFlag)
				return;

			DWORD dwValue = 144;

			if (bFlag) {
				CWndStatic * lpWndStatic;
				lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC5);
				lpWndStatic->SetVisible(TRUE);

				lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
				CRect crect = lpWndStatic->GetWindowRect(TRUE);
				lpWndStatic->Move(crect.left, crect.top + dwValue);

				lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC7);
				crect = lpWndStatic->GetWindowRect(TRUE);
				lpWndStatic->Move(crect.left, crect.top + dwValue);

				if (m_bLegend) {
					crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
					m_pWndHeroStatic[0]->Move(crect.left, crect.top + dwValue);

					crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
					m_pWndHeroStatic[1]->Move(crect.left, crect.top + dwValue);
				}

				rect.bottom += dwValue;
			} else {
				CWndStatic * lpWndStatic;
				lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC5);
				lpWndStatic->SetVisible(FALSE);

				lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
				CRect crect = lpWndStatic->GetWindowRect(TRUE);
				lpWndStatic->Move(crect.left, crect.top - dwValue);


				lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC7);
				crect = lpWndStatic->GetWindowRect(TRUE);
				lpWndStatic->Move(crect.left, crect.top - dwValue);

				if (m_bLegend) {
					crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
					m_pWndHeroStatic[0]->Move(crect.left, crect.top - dwValue);

					crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
					m_pWndHeroStatic[1]->Move(crect.left, crect.top - dwValue);
				}

				rect.bottom -= dwValue;
			}

			SetWndRect(rect);

			m_bSlot[1] = bFlag;
		} else
			if (nSlot == 2) {
				if (m_bSlot[2] == bFlag)
					return;

				DWORD dwValue = 144;

				if (bFlag) {
					CWndStatic * lpWndStatic;
					lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC7);
					lpWndStatic->SetVisible(TRUE);

					if (m_bLegend) {
						CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
						m_pWndHeroStatic[0]->Move(crect.left, crect.top + dwValue);

						crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
						m_pWndHeroStatic[1]->Move(crect.left, crect.top + dwValue);
					}

					rect.bottom += dwValue;
				} else {
					CWndStatic * lpWndStatic;
					lpWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC7);
					lpWndStatic->SetVisible(FALSE);

					if (m_bLegend) {
						CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
						m_pWndHeroStatic[0]->Move(crect.left, crect.top - dwValue);

						crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
						m_pWndHeroStatic[1]->Move(crect.left, crect.top - dwValue);
					}

					rect.bottom -= dwValue;
				}

				SetWndRect(rect);

				m_bSlot[2] = bFlag;
			} else if (nSlot == 3) {
				if (m_bSlot[3] == bFlag)
					return;

				DWORD dwValue = 48;

				if (bFlag) {
					m_pWndHeroStatic[1]->SetVisible(TRUE);
					rect.bottom += dwValue;
				} else {
					m_pWndHeroStatic[1]->SetVisible(FALSE);
					rect.bottom -= dwValue;
				}

				SetWndRect(rect);

				m_bSlot[3] = bFlag;
			}

			if (rect.bottom > (int)(FULLSCREEN_HEIGHT - 48)) {
				rect.OffsetRect(0, (int)(FULLSCREEN_HEIGHT - 48) - rect.bottom);
				SetWndRect(rect);
			}

			AdjustWndBase();
}

bool CWndSkillTreeEx::IsDownPoint(const SKILL & skill) const {
	SKILL * pSkillMy = g_pPlayer->GetSkill(skill.dwSkill);
	if (!pSkillMy) return false;

	return skill.dwLevel > pSkillMy->dwLevel;
}

void CWndSkillTreeEx::SubSkillPointDown(SKILL * lpSkill) {
	for (SKILL & skill : m_skills) {
		SKILL * pSkill2 = &skill;

		if (pSkill2->GetProp()->dwReSkill1 == lpSkill->dwSkill) {
			if (pSkill2->GetProp()->dwReSkillLevel1) {
				if (lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel1) {
					if (pSkill2->dwLevel > 0) {
						m_nCurrSkillPoint += (prj.GetSkillPoint(pSkill2->GetProp()) * pSkill2->dwLevel);
						pSkill2->dwLevel = 0;
						SubSkillPointDown(pSkill2);
					}
				}
			}
		}

		if (pSkill2->GetProp()->dwReSkill2 == lpSkill->dwSkill) {
			if (pSkill2->GetProp()->dwReSkillLevel2) {
				if (lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel2) {
					if (pSkill2->dwLevel > 0) {
						m_nCurrSkillPoint += (prj.GetSkillPoint(pSkill2->GetProp()) * pSkill2->dwLevel);
						pSkill2->dwLevel = 0;
						SubSkillPointDown(pSkill2);
					}
				}
			}
		}
	}
}

SKILL * CWndSkillTreeEx::GetSkill(int i) {
	if (i < 0 || std::cmp_greater_equal(i, m_skills.size())) {
		return nullptr;
	}

	return &m_skills[i];
}

bool CWndSkillTreeEx::CheckSkill(int i) {
	const auto GetdwSkill = [&](DWORD dwSkill) {
		const auto it = std::ranges::find_if(m_skills,
			[dwSkill](const SKILL & skill) { return skill.dwSkill == dwSkill; }
		);

		return it != m_skills.end() ? &*it : nullptr;
	};

	SKILL * pSkill = GetSkill(i);

	if (!pSkill || !g_pPlayer) return FALSE;

	const DWORD dwSkill = pSkill->dwSkill;
	const ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);

	if (pSkillProp == NULL || pSkillProp->nLog == 1)
		return FALSE;

	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero())
		return FALSE;

	if (pSkillProp->dwReSkill1 != 0xffffffff) {
		LPSKILL pSkillBuf = GetdwSkill(pSkillProp->dwReSkill1);

		if (pSkillBuf) {
			if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1) {
				return FALSE;
			}
		} else {
		}
	}

	if (pSkillProp->dwReSkill2 != 0xffffffff) {
		LPSKILL pSkillBuf = GetdwSkill(pSkillProp->dwReSkill2);
		if (pSkillBuf) {
			if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2) {
				return FALSE;
			}
		} else {
		}
	}

	return TRUE;
}

HRESULT CWndSkillTreeEx::RestoreDeviceObjects() {
	CWndBase::RestoreDeviceObjects();
	if (m_pVBGauge == NULL)
		return m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL);
	return S_OK;
}
HRESULT CWndSkillTreeEx::InvalidateDeviceObjects() {
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}
HRESULT CWndSkillTreeEx::DeleteDeviceObjects() {
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);

	SAFE_DELETE(m_atexJobPannel[0]);
	SAFE_DELETE(m_atexJobPannel[1]);

	m_skills.clear();
	m_skillsTexture.clear();

	return S_OK;
}

void CWndSkillTreeEx::InitItem() {
	if (!g_pPlayer) return;

	if (g_pPlayer->m_nSkillPoint > 0) {
		m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
	} else {
		m_nCurrSkillPoint = 0;
	}

	m_skills = g_pPlayer->m_jobSkills;
	m_skillsTexture.clear();
	m_skillsTexture.resize(m_skills.size(), nullptr);

	m_nCurSelect = -1;
	m_nJob = g_pPlayer->m_nJob;

	m_nCount = 0;
	CString strTex[2];

	CWndStatic * lpWndStatic1 = (CWndStatic *)GetDlgItem(WIDC_STATIC2);
	CWndStatic * lpWndStatic2 = (CWndStatic *)GetDlgItem(WIDC_STATIC3);
	CWndStatic * lpWndStatic3 = (CWndStatic *)GetDlgItem(WIDC_STATIC6);

	switch (m_nJob) {
		case JOB_VAGRANT:
			m_nCount = 3;
			strTex[0] = "";//"Back_Upper.TGA";
			strTex[1] = "";//"Back_Lower.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle("");
			lpWndStatic3->SetTitle("");
			break;
		case JOB_KNIGHT:
		case JOB_KNIGHT_MASTER:
		case JOB_KNIGHT_HERO:
			m_nCount = 26;
			strTex[0] = "Back_Me.TGA";
			strTex[1] = "Back_Night.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_KNIGHT].szName);
			break;
		case JOB_BLADE:
		case JOB_BLADE_MASTER:
		case JOB_BLADE_HERO:
			m_nCount = 26;
			strTex[0] = "Back_Me.TGA";
			strTex[1] = "Back_Blade.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_BLADE].szName);
			break;
		case JOB_MERCENARY:
			m_nCount = 18;
			strTex[0] = "Back_Me.TGA";
			strTex[1] = "";//"Back_Lower.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
			lpWndStatic3->SetTitle("");
			break;
		case JOB_BILLPOSTER:
		case JOB_BILLPOSTER_MASTER:
		case JOB_BILLPOSTER_HERO:
			m_nCount = 28;
			strTex[0] = "Back_As.TGA";
			strTex[1] = "Back_Bill.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_BILLPOSTER].szName);
			break;
		case JOB_RINGMASTER:
		case JOB_RINGMASTER_MASTER:
		case JOB_RINGMASTER_HERO:
			m_nCount = 28;
			strTex[0] = "Back_As.TGA";
			strTex[1] = "Back_Ring.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_RINGMASTER].szName);
			break;
		case JOB_ASSIST:
			m_nCount = 20;
			strTex[0] = "Back_As.TGA";
			strTex[1] = "";//"Back_Lower.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
			lpWndStatic3->SetTitle("");
			break;
		case JOB_ELEMENTOR:
		case JOB_ELEMENTOR_MASTER:
		case JOB_ELEMENTOR_HERO:
			m_nCount = 39;
			strTex[0] = "Back_Ma.TGA";
			strTex[1] = "Back_Ele.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_ELEMENTOR].szName);
			break;
		case JOB_PSYCHIKEEPER:
		case JOB_PSYCHIKEEPER_MASTER:
		case JOB_PSYCHIKEEPER_HERO:
			m_nCount = 28;
			strTex[0] = "Back_Ma.TGA";
			strTex[1] = "Back_Psy.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_PSYCHIKEEPER].szName);
			break;
		case JOB_MAGICIAN:
			m_nCount = 20;
			strTex[0] = "Back_Ma.TGA";
			strTex[1] = "";//"Back_Lower.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
			lpWndStatic3->SetTitle("");
			break;
		case JOB_ACROBAT:
			m_nCount = 20;
			strTex[0] = "Back_Acr.tga";
			strTex[1] = "";//"Back_Lower.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
			lpWndStatic3->SetTitle("");
			break;
		case JOB_JESTER:
		case JOB_JESTER_MASTER:
		case JOB_JESTER_HERO:
			m_nCount = 20;
			strTex[0] = "Back_Acr.tga";
			strTex[1] = "Back_Jst.TGA";//"Back_Lower.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_JESTER].szName);
			break;
		case JOB_RANGER:
		case JOB_RANGER_MASTER:
		case JOB_RANGER_HERO:
			m_nCount = 20;
			strTex[0] = "Back_Acr.tga";
			strTex[1] = "Back_Rag.TGA";//"Back_Lower.TGA";
			lpWndStatic1->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
			lpWndStatic2->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
			lpWndStatic3->SetTitle(prj.m_aJob[JOB_RANGER].szName);
			break;
		default:
			Error("CWndSkillTreeEx::InitItem �˼����� ���� : %d", m_nJob);
			return;
	}

	//Master Skill�� ���ۺ��� 1Lv�̹Ƿ� ���? �̹��� ����.
	switch (m_nJob) {
		case JOB_KNIGHT_HERO:
			m_strHeroSkilBg = "Back_Hero_KntDrawing.tga";
			break;
		case JOB_BLADE_HERO:
			m_strHeroSkilBg = "Back_Hero_BldDefence.tga";
			break;
		case JOB_BILLPOSTER_HERO:
			m_strHeroSkilBg = "Back_Hero_BilDisEnchant.tga";
			break;
		case JOB_RINGMASTER_HERO:
			m_strHeroSkilBg = "Back_Hero_RigReturn.tga";
			break;
		case JOB_ELEMENTOR_HERO:
			m_strHeroSkilBg = "Back_Hero_EleCursemind.tga";
			break;
		case JOB_PSYCHIKEEPER_HERO:
			m_strHeroSkilBg = "Back_Hero_PsyStone.tga";
			break;
		case JOB_JESTER_HERO:
			m_strHeroSkilBg = "Back_Hero_JstSilence.tga";
			break;
		case JOB_RANGER_HERO:
			m_strHeroSkilBg = "Back_Hero_RagHawkeye.tga";
			break;
	}

	SAFE_DELETE(m_atexJobPannel[0]);
	SAFE_DELETE(m_atexJobPannel[1]);

	if (!strTex[0].IsEmpty()) {
		m_atexJobPannel[0] = new IMAGE;
		if (LoadImage(MakePath(DIR_THEME, strTex[0]), m_atexJobPannel[0]) == FALSE)
			Error("CWndSkillTreeEx::InitItem���� %s Open1 ����", strTex[0]);

	}

	if (!strTex[1].IsEmpty()) {
		m_atexJobPannel[1] = new IMAGE;
		if (LoadImage(MakePath(DIR_THEME, strTex[1]), m_atexJobPannel[1]) == FALSE)
			Error("CWndSkillTreeEx::InitItem���� %s Open1 ����", strTex[1]);

	}
	AdjustWndBase();

	// �ҽ� �������� �Է�
	LoadTextureSkillicon();

	m_dwMouseSkill = NULL_ID;
}
BOOL CWndSkillTreeEx::GetSkillPoint(DWORD dwSkillID, CRect & rect) {
	int nExpertGapX = 9;
	int nExpertGapY = 90;

	int nProGapX = 8;
	int nProGapY = 228;

	ItemProp * pPropSkill = prj.GetSkillProp(dwSkillID);

	if (pPropSkill) {
		CPoint pt;

		LPWNDCTRL lpWndCtrl;
		LPWNDCTRL lpWndCtrlUpper = GetWndCtrl(WIDC_STATIC5);
		LPWNDCTRL lpWndCtrlLower = GetWndCtrl(WIDC_STATIC7);

		switch (pPropSkill->dwItemKind1) {
			case JTYPE_BASE: pt = 0; break;
			case JTYPE_EXPERT: pt = lpWndCtrlUpper->rect.TopLeft(); break;
			case JTYPE_PRO: pt = lpWndCtrlLower->rect.TopLeft(); break;
			case JTYPE_MASTER: pt = 0; break;
			case JTYPE_HERO: pt = 0; break;
		}
		int nRectX, nRectY, nJobKind;
		nRectX = nRectY = 0;
		nJobKind = MAX_JOBBASE;
		int nLegendSkill = 0;
		switch (dwSkillID) {
			case SI_VAG_ONE_CLEANHIT:
			{
				lpWndCtrl = GetWndCtrl(WIDC_CUSTOM2);
				rect = lpWndCtrl->rect;
				rect.top += 1;
			}
			break;
			case SI_VAG_ONE_BRANDISH:
			{
				lpWndCtrl = GetWndCtrl(WIDC_CUSTOM3);
				rect = lpWndCtrl->rect;
				rect.top += 1;
			}
			break;
			case SI_VAG_ONE_OVERCUTTER:
			{
				lpWndCtrl = GetWndCtrl(WIDC_CUSTOM4);
				rect = lpWndCtrl->rect;
				rect.top += 1;
			}
			break;
			//�Ӽ��ʸ�
			case SI_MER_ONE_SPLMASH:				nRectX = 34, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PROTECTION:			nRectX = 34, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_KEENWHEEL:				nRectX = 84, nRectY = 97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLOODYSTRIKE:			nRectX = 84, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PANBARRIER:			nRectX = 84, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_IMPOWERWEAPON:			nRectX = 84, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLINDSIDE:				nRectX = 134, nRectY = 97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_REFLEXHIT:				nRectX = 134, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SNEAKER:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SMITEAXE:				nRectX = 134, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_BLAZINGSWORD:			nRectX = 134, nRectY = 201, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SPECIALHIT:				nRectX = 184, nRectY = 97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_GUILOTINE:				nRectX = 184, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_AXEMASTER:				nRectX = 184, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SWORDMASTER:			nRectX = 184, nRectY = 201, nJobKind = MAX_EXPERT; break;
				// ����Ʈ
			case SI_KNT_TWOSW_CHARGE:				nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_PAINDEALER:			nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_GUARD:					nRectX = 34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOSW_EARTHDIVIDER:			nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_POWERSTUMP:			nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_RAGE:					nRectX = 84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWO_POWERSWING:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_PAINREFLECTION:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				// �����̵�
			case SI_BLD_DOUBLESW_SILENTSTRIKE:		nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_SPRINGATTACK:		nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_ARMORPENETRATE:		nRectX = 34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLESW_BLADEDANCE:		nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_HAWKATTACK:		nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_SUP_BERSERK:				nRectX = 84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_CROSSSTRIKE:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_SONICBLADE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;

				// ��ý��?	
			case SI_ASS_HEAL_HEALING:				nRectX = 18, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PATIENCE:				nRectX = 64, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_QUICKSTEP:			nRectX = 64, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_MENTALSIGN:			nRectX = 64, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_TAMPINGHOLE:			nRectX = 64, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_RESURRECTION:			nRectX = 110, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HASTE:				nRectX = 110, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HEAPUP:				nRectX = 110, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_STONEHAND:			nRectX = 110, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CIRCLEHEALING:		nRectX = 156, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CATSREFLEX:			nRectX = 156, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_BEEFUP:				nRectX = 156, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_BURSTCRACK:				nRectX = 156, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PREVENTION:			nRectX = 202, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CANNONBALL:			nRectX = 202, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_ACCURACY:				nRectX = 202, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_POWERFIST:				nRectX = 202, nRectY = 187, nJobKind = MAX_EXPERT; break;
				//��������
			case SI_RIN_SUP_PROTECT:				nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYCROSS:				nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_GVURTIALLA:			nRectX = 34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYGUARD:				nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_SPIRITUREFORTUNE:		nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_HEALRAIN:				nRectX = 84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SQU_GEBURAHTIPHRETH:		nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_MERKABAHANZELRUSHA:		nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				//��������
			case SI_BIL_KNU_BELIALSMESHING:			nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASMODEUS:				nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_BLOODFIST:				nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BARAQIJALESNA:			nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_PIERCINGSERPENT:		nRectX = 134, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BGVURTIALBOLD:			nRectX = 134, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_SONICHAND:				nRectX = 184, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASALRAALAIKUM:			nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;

				// ������
			case SI_MAG_MAG_MENTALSTRIKE:			nRectX = 34, nRectY = 96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_MAG_BLINKPOOL: 				nRectX = 34, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_BOOMERANG:				nRectX = 84, nRectY = 96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_SWORDWIND:				nRectX = 84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_ICEMISSILE:			nRectX = 84, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGBALL:	nRectX = 84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_SPIKESTONE:			nRectX = 84, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_HOTAIR:				nRectX = 134, nRectY = 96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_STRONGWIND:			nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_WATERBALL:			nRectX = 134, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGPARM:	nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_ROCKCRASH:			nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_FIRESTRIKE:			nRectX = 184, nRectY = 96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_WINDCUTTER:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_SPRINGWATER:			nRectX = 184, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_LOOTING:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
				// ��������
			case SI_ELE_FIRE_FIREBIRD:				nRectX = 34, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_STONESPEAR:			nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_VOID:					nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	nRectX = 34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_ICESHARK:				nRectX = 34, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_BURINGFIELD:			nRectX = 84, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHQUAKE:			nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDFIELD:				nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_ELETRICSHOCK:	nRectX = 84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_POISONCLOUD:			nRectX = 84, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_METEOSHOWER:			nRectX = 134, nRectY = 246, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_SANDSTORM:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_LIGHTINGSTORM:		nRectX = 134, nRectY = 298, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_AVALANCHE:			nRectX = 134, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_FIREMASTER:			nRectX = 184, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHMASTER:			nRectX = 184, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDMASTER:			nRectX = 184, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_WATERMASTER:			nRectX = 184, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
				// ����Ű��
			case SI_PSY_NLG_DEMONOLGY:				nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICBOMB:			nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_CRUCIOSPELL:			nRectX = 34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_SATANOLGY:				nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_SPRITBOMB:				nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_MAXIMUMCRISIS:			nRectX = 84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICWALL:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICSQUARE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				////////////////////////////////////////////////////////////////////////////////////
				// ��ũ�κ�
			case SI_ACR_YOYO_PULLING:				nRectX = 34, nRectY = 97, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_SLOWSTEP:				nRectX = 34, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_JUNKBOW:				nRectX = 34, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_FASTWALKER:				nRectX = 84, nRectY = 96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_YOYOMASTER:				nRectX = 84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_BOWMASTER: 				nRectX = 84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_DARKILLUSION:			nRectX = 134, nRectY = 96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_SNITCH:				nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_CROSSLINE:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_SILENTSHOT:				nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_AIMEDSHOT:				nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_ABSOLUTEBLOCK:			nRectX = 184, nRectY = 96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_DEADLYSWING:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_COUNTER:				nRectX = 184, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_AUTOSHOT:				nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_ARROWRAIN:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
				//������
			case SI_JST_SUP_POISON:					nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_BLEEDING:				nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_ESCAPE:				nRectX = 34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_CRITICALSWING:			nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_BACKSTAB:				nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_ABSORB:					nRectX = 84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_VATALSTAB:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_HITOFPENYA:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				//������
			case SI_RAG_BOW_ICEARROW:				nRectX = 34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_FLAMEARROW:				nRectX = 34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_POISONARROW:			nRectX = 34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_FASTATTACK:				nRectX = 84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_PIRCINGARROW:			nRectX = 84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_NATURE:					nRectX = 84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_TRIPLESHOT:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_SILENTARROW:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;

				//������ ��ų
			case SI_BLD_MASTER_ONEHANDMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_KNT_MASTER_TWOHANDMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_JST_MASTER_YOYOMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_RAG_MASTER_BOWMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_ELE_MASTER_INTMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_PSY_MASTER_INTMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_BIL_MASTER_KNUCKLEMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_RIG_MASTER_BLESSING:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
				//���� ��ų			
			case SI_BLD_HERO_DEFFENCE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_KNT_HERO_DRAWING:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_JST_HERO_SILENCE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_RAG_HERO_HAWKEYE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_ELE_HERO_CURSEMIND:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_PSY_HERO_STONE:					nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_BIL_HERO_DISENCHANT:			nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_RIG_HERO_RETURN:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			default:
				return FALSE;
		}

		if (nJobKind == MAX_EXPERT)
			rect = CRect(nRectX - nExpertGapX, nRectY - nExpertGapY, nRectX + 24 - nExpertGapX, nRectY + 24 - nExpertGapY);
		else if (nJobKind == MAX_PROFESSIONAL)
			rect = CRect(nRectX - nProGapX, nRectY - nProGapY, nRectX + 24 - nProGapX, nRectY + 24 - nProGapY);
		else if (nJobKind == MAX_MASTER || nJobKind == MAX_HERO) {
			LPWNDCTRL lpWndCtrl;
			if (nLegendSkill == 0)
				lpWndCtrl = GetWndCtrl(WIDC_CUSTOM5);
			else if (nLegendSkill == 1)
				lpWndCtrl = GetWndCtrl(WIDC_CUSTOM6);

			rect = lpWndCtrl->rect;
		}

		rect += pt;
	}
	/*
		switch( m_nJob )
		{
		case JOB_KNIGHT:
			break;
		case JOB_BLADE:
			break;
		case JOB_MERCENARY:
			break;
		case JOB_BILLPOSTER:
			break;
		case JOB_RINGMASTER:
			break;
		case JOB_ASSIST:
			break;
		case JOB_ELEMENTOR:
			break;
		case JOB_PSYCHIKEEPER:
			break;
		case JOB_MAGICIAN:
			break;
		}
	*/
	return TRUE;
}

void CWndSkillTreeEx::LoadTextureSkillicon() {
	m_skillsTexture.resize(m_skills.size(), nullptr);

	for (size_t i = 0; i != m_skillsTexture.size(); ++i) {
		const ItemProp * pSkillProp = prj.m_aPropSkill.GetAt(m_skills[i].dwSkill);
		if (pSkillProp) {
			m_skillsTexture[i] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pSkillProp->szIcon), 0xffff00ff);
		}
	}


	m_textPackNum.LoadScript(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("icon_IconSkillLevel.inc")));
}
void CWndSkillTreeEx::OnMouseMove(UINT nFlags, CPoint point) {
	if (m_bDrag == FALSE)
		return;

	if (CheckSkill(m_nCurSelect)) {
		m_bDrag = FALSE;
		const DWORD dwSkill = m_skills[m_nCurSelect].dwSkill;
		const ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);

		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = ShortcutType::Skill;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId = dwSkill;
		m_GlobalShortcut.m_pTexture = m_skillsTexture[m_nCurSelect];
		_tcscpy(m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}

BOOL CWndSkillTreeEx::Process() {
	if (g_pPlayer == NULL)
		return FALSE;

	if (!m_bLegend) {
		if (g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER) {
			m_bLegend = TRUE;
			//Hero SkillTree
			m_pWndHeroStatic[0] = (CWndStatic *)GetDlgItem(WIDC_STATIC1);
			m_pWndHeroStatic[0]->EnableWindow(TRUE);
			m_pWndHeroStatic[0]->SetVisible(TRUE);

			m_pWndHeroStatic[1] = (CWndStatic *)GetDlgItem(WIDC_STATIC11);
			m_pWndHeroStatic[1]->EnableWindow(TRUE);
			m_pWndHeroStatic[1]->SetVisible(TRUE);

			CRect rect = GetWindowRect();
			rect.bottom += 90;
			m_bSlot[3] = TRUE;

			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();

			CPoint point(x, y);
			Move(point);
			return TRUE;
		}
	} else {
		if (g_pPlayer->GetLegendChar() < LEGEND_CLASS_MASTER) {
			m_bLegend = FALSE;
			//Normal SkillTree
			m_pWndHeroStatic[0] = (CWndStatic *)GetDlgItem(WIDC_STATIC1);
			m_pWndHeroStatic[0]->EnableWindow(FALSE);
			m_pWndHeroStatic[0]->SetVisible(FALSE);

			m_pWndHeroStatic[1] = (CWndStatic *)GetDlgItem(WIDC_STATIC11);
			m_pWndHeroStatic[1]->EnableWindow(FALSE);
			m_pWndHeroStatic[1]->SetVisible(FALSE);

			CRect rect = GetWindowRect();
			rect.bottom -= 100;
			m_bSlot[3] = FALSE;

			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();

			CPoint point(x, y);
			Move(point);
			return TRUE;
		}
	}

	m_pWndButton[0]->EnableWindow(FALSE);
	m_pWndButton[1]->EnableWindow(FALSE);
	m_pWndButton[2]->EnableWindow(FALSE);
	m_pWndButton[3]->EnableWindow(FALSE);

	SKILL * m_pFocusItem = GetFocusedItem();
	if (m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint) {
		LPSKILL lpSkillUser = g_pPlayer->GetSkill(m_pFocusItem->dwSkill);
		const ItemProp * pSkillProp = m_pFocusItem->GetProp();
		if (pSkillProp == NULL || lpSkillUser == NULL)
			return TRUE;

		int nPoint = prj.GetSkillPoint(pSkillProp);
		if (m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint)
			m_pWndButton[0]->EnableWindow(TRUE);

		if (m_pFocusItem->dwLevel != lpSkillUser->dwLevel)
			m_pWndButton[1]->EnableWindow(TRUE);

		if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint) {
			m_pWndButton[2]->EnableWindow(TRUE);
			m_pWndButton[3]->EnableWindow(TRUE);
		}

		pSkillProp = prj.GetSkillProp(m_pFocusItem->dwSkill);
		if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO) {
			m_pWndButton[0]->EnableWindow(FALSE);
			m_pWndButton[1]->EnableWindow(FALSE);
			m_pWndButton[2]->EnableWindow(FALSE);
			m_pWndButton[3]->EnableWindow(FALSE);
		}
	}
	return TRUE;
}

void CWndSkillTreeEx::OnMouseWndSurface(CPoint point) {
	DWORD dwMouseSkill = NULL_ID;

	for (int i = 0; std::cmp_less(i, m_skills.size()); i++) {
		LPSKILL pSkill = GetSkill(i);
		if (pSkill == NULL)
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);

		if (pSkillProp) {
			if (!m_bSlot[0]) {
				if (pSkillProp->dwItemKind1 == JTYPE_BASE)
					continue;
			}

			if (!m_bSlot[1]) {
				if (pSkillProp->dwItemKind1 == JTYPE_EXPERT)
					continue;
			}

			if (!m_bSlot[2]) {
				if (pSkillProp->dwItemKind1 == JTYPE_PRO)
					continue;
			}
			if (!m_bSlot[3]) {
				if (pSkillProp->dwItemKind1 == JTYPE_HERO)
					continue;
			}
			GetCalcImagePos(pSkillProp->dwItemKind1);
		}

		if (dwSkill != NULL_ID) {
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect)) {
				rect.top -= 2;
				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;
				rect.OffsetRect(0, m_nTopDownGap);

				if (rect.PtInRect(point)) {
					dwMouseSkill = dwSkill;

					ClientToScreen(&point);
					ClientToScreen(&rect);
					g_WndMng.PutToolTip_Skill(dwSkill, pSkill->dwLevel, point, &rect, CheckSkill(i));
					break;
				}
			}
		}
	}
	m_dwMouseSkill = dwMouseSkill;
}

int CWndSkillTreeEx::GetCalcImagePos(int nIndex) {
	m_nTopDownGap = 0;

	if (m_bSlot[0]) {
		if (nIndex == JTYPE_BASE)
			m_nTopDownGap = 0;
	}

	if (m_bSlot[1]) {
		if (nIndex == JTYPE_EXPERT) {
			if (m_bSlot[0])
				m_nTopDownGap = 0;
			else
				m_nTopDownGap -= 48;
		}
	}

	if (m_bSlot[2]) {
		if (nIndex == JTYPE_PRO) {
			if (m_bSlot[0]) {
				if (m_bSlot[1])
					m_nTopDownGap = 0;
				else
					m_nTopDownGap -= 144;
			} else {
				if (m_bSlot[1])
					m_nTopDownGap -= 48;
				else
					m_nTopDownGap -= (144 + 48);
			}
		}
	}
	if (m_bSlot[3]) {
		if (nIndex == JTYPE_MASTER || nIndex == JTYPE_HERO) {
			if (m_bSlot[0]) {
				if (m_bSlot[1]) {
					if (m_bSlot[2])
						m_nTopDownGap = 0;
					else
						m_nTopDownGap -= 144;
				} else {
					if (m_bSlot[2])
						m_nTopDownGap -= 144;
					else
						m_nTopDownGap -= (144 + 144);
				}
			} else {
				if (m_bSlot[1]) {
					if (m_bSlot[2])
						m_nTopDownGap -= 48;
					else
						m_nTopDownGap -= (144 + 48);
				} else {
					if (m_bSlot[2])
						m_nTopDownGap -= (144 + 48);
					else
						m_nTopDownGap -= (144 + 144 + 48);
				}
			}
		}
	}

	return 0;
}

void CWndSkillTreeEx::OnDraw(C2DRender * p2DRender) {
	m_nAlphaCount = m_nAlpha;
	CPoint pt;

	if (m_bLegend && m_bSlot[3]) {
		//Master Skill�� ���ۺ��� 1Lv�̹Ƿ� ���? �̹��� ����.

		if (!m_strHeroSkilBg.IsEmpty()) {
			CPoint point;

			LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM6);
			CRect rect = lpWndCtrl->rect;
			GetCalcImagePos(JTYPE_HERO);
			rect.OffsetRect(0, m_nTopDownGap);
			point = rect.TopLeft();
			point.y -= 2;
			CTexture * pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, m_strHeroSkilBg), 0xffff00ff);
			if (pTexture != NULL)
				pTexture->Render(p2DRender, point, CPoint(27, 27));
		}
	}

	for (int i = 0; std::cmp_less(i, m_skills.size()); i++) {
		SKILL * pSkill = &m_skills[i];
		const DWORD dwSkill = pSkill->dwSkill;

		const ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);
		if (!pSkillProp) continue;

		if (!m_bSlot[0]) {
			if (pSkillProp->dwItemKind1 == JTYPE_BASE)
				continue;
		}

		if (!m_bSlot[1]) {
			if (pSkillProp->dwItemKind1 == JTYPE_EXPERT)
				continue;
		}

		if (!m_bSlot[2]) {
			if (pSkillProp->dwItemKind1 == JTYPE_PRO)
				continue;
		}

		if (!m_bSlot[3]) {
			if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
				continue;
		}

		GetCalcImagePos(pSkillProp->dwItemKind1);

		if (pSkillProp->nLog != 1 && dwSkill != NULL_ID) {
			// ��ų ������ ���? 
			if (m_skillsTexture[i] && CheckSkill(i) && 0 < pSkill->dwLevel) {
				CRect rect;
				if (GetSkillPoint(pSkillProp->dwID, rect)) {
					rect.top -= 2;
					rect.OffsetRect(0, m_nTopDownGap);
					m_skillsTexture[i]->Render(p2DRender, rect.TopLeft(), CPoint(27, 27));
					int nAddNum = 0;
					LPSKILL pSkillUser = g_pPlayer->GetSkill(pSkill->dwSkill);
					if (pSkillUser && pSkill->dwLevel != pSkillUser->dwLevel)
						nAddNum = 20;
					if (pSkill->dwLevel < pSkillProp->dwExpertMax)
						m_textPackNum.Render(p2DRender, rect.TopLeft() - CPoint(2, 2), pSkill->dwLevel - 1 + nAddNum);
					else
						m_textPackNum.Render(p2DRender, rect.TopLeft() - CPoint(2, 2), 19 + nAddNum);
				}
			} else if (m_dwMouseSkill == dwSkill && m_skillsTexture[i] && CheckSkill(i)) {
				CRect rect;
				if (GetSkillPoint(pSkillProp->dwID, rect)) {
					rect.top -= 2;
					rect.OffsetRect(0, m_nTopDownGap);
					m_skillsTexture[i]->Render(p2DRender, rect.TopLeft(), CPoint(27, 27));
				}
			}
		}
	}

	CWndStatic * lpWndStatic9 = (CWndStatic *)GetDlgItem(WIDC_STATIC9);
	CString strSP;
	strSP.Format("%d", m_nCurrSkillPoint);
	lpWndStatic9->SetTitle(strSP);

	// ���õ� ��ų�� ������ �� â�� ���?
	SKILL * pFocusItem = GetFocusedItem();
	if (pFocusItem) {
		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
		m_skillsTexture[m_nCurSelect]->Render(p2DRender, lpWndCtrl->rect.TopLeft());

		ItemProp * pSkillProp = prj.GetSkillProp(pFocusItem->dwSkill);
		if (pSkillProp && 0 < pFocusItem->dwLevel) {
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill(pFocusItem->dwSkill);
			if (pSkillUser && pFocusItem->dwLevel != pSkillUser->dwLevel)
				nAddNum = 20;

			if (pFocusItem->dwLevel < pSkillProp->dwExpertMax)
				m_textPackNum.Render(p2DRender, lpWndCtrl->rect.TopLeft(), pFocusItem->dwLevel - 1 + nAddNum);
			else
				m_textPackNum.Render(p2DRender, lpWndCtrl->rect.TopLeft(), 19 + nAddNum);
		}
	}
	CRect rect;
	CWndStatic * pStatic;
	pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC2);
	rect = pStatic->GetWindowRect(TRUE);

	rect.left = rect.right - 30;

	if (m_bSlot[0])
		m_atexTopDown[0]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));
	else
		m_atexTopDown[1]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC3);
	rect = pStatic->GetWindowRect(TRUE);

	rect.left = rect.right - 30;

	if (m_bSlot[1])
		m_atexTopDown[0]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));
	else
		m_atexTopDown[1]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
	rect = pStatic->GetWindowRect(TRUE);

	rect.left = rect.right - 30;

	if (m_bSlot[2])
		m_atexTopDown[0]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));
	else
		m_atexTopDown[1]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));

	///////////////////////////////////////////////////////////////////////////////////////

	if (m_bLegend) {
		pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC1);
		rect = pStatic->GetWindowRect(TRUE);

		rect.left = rect.right - 30;

		if (m_bSlot[3])
			m_atexTopDown[0]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));
		else
			m_atexTopDown[1]->Render(p2DRender, rect.TopLeft(), CPoint(20, 20));
	}
}
void CWndSkillTreeEx::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();

	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauFillNormal.bmp"), 0xffff00ff, TRUE);

	m_aSkillLevel[0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "ButtSkillLevelHold01.tga"), 0xffff00ff);
	m_aSkillLevel[1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "ButtSkillLevelup01.tga"), 0xffff00ff);
	m_aSkillLevel[2] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "ButtSkillLevelHold02.tga"), 0xffff00ff);

	InitItem();

	m_atexTopDown[0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "LvUp.bmp"), 0xffff00ff);
	m_atexTopDown[1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "LvDn.bmp"), 0xffff00ff);

	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
	m_bSlot[3] = TRUE;

	m_nCurSelect = -1;

	m_pWndButton[0] = (CWndButton *)GetDlgItem(WIDC_BUTTON1);	// + ��ư
	m_pWndButton[1] = (CWndButton *)GetDlgItem(WIDC_BUTTON2);	// - ��ư
	m_pWndButton[2] = (CWndButton *)GetDlgItem(WIDC_BUTTON3);	// Reset ��ư
	m_pWndButton[3] = (CWndButton *)GetDlgItem(WIDC_BUTTON4);	// Finish ��ư

	if (g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		m_bLegend = TRUE;
	else
		m_bLegend = FALSE;

	if (m_bLegend) {
		m_pWndHeroStatic[0] = (CWndStatic *)GetDlgItem(WIDC_STATIC1);
		m_pWndHeroStatic[0]->EnableWindow(TRUE);
		m_pWndHeroStatic[0]->SetVisible(TRUE);

		m_pWndHeroStatic[1] = (CWndStatic *)GetDlgItem(WIDC_STATIC11);
		m_pWndHeroStatic[1]->EnableWindow(TRUE);
		m_pWndHeroStatic[1]->SetVisible(TRUE);
	} else {
		m_pWndHeroStatic[0] = (CWndStatic *)GetDlgItem(WIDC_STATIC1);
		m_pWndHeroStatic[0]->EnableWindow(FALSE);
		m_pWndHeroStatic[0]->SetVisible(FALSE);

		m_pWndHeroStatic[1] = (CWndStatic *)GetDlgItem(WIDC_STATIC11);
		m_pWndHeroStatic[1]->EnableWindow(FALSE);
		m_pWndHeroStatic[1]->SetVisible(FALSE);

		CRect rect = GetWindowRect();
		rect.bottom -= 100;
		m_bSlot[3] = FALSE;

		SetWndRect(rect);
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - rect.Height();

	CPoint point(x, y);
	Move(point);
}
BOOL CWndSkillTreeEx::Initialize(CWndBase * pWndParent, DWORD dwWndId) {
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	return CWndNeuz::InitDialog(dwWndId, pWndParent, 0, CPoint(792, 130));
}
BOOL CWndSkillTreeEx::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	SKILL * curSel = GetFocusedItem();
	if (curSel && g_pPlayer->m_nSkillPoint > 0) {
		const ItemProp * pSkillProp = curSel->GetProp();
		if (pSkillProp) {
			const int nPoint = prj.GetSkillPoint(pSkillProp);
			switch (nID) {
				case WIDC_BUTTON1:	// + ��ư
					if (nPoint <= m_nCurrSkillPoint) {
						if (curSel->dwLevel < pSkillProp->dwExpertMax) {
							m_nCurrSkillPoint -= nPoint;
							++curSel->dwLevel;
						}
					}
					break;
				case WIDC_BUTTON2:	// - ��ư
					if (curSel && IsDownPoint(*curSel)) {
						m_nCurrSkillPoint += nPoint;
						--curSel->dwLevel;
						SubSkillPointDown(curSel);
					}
					break;
				case WIDC_BUTTON3:	// Reset ��ư
					if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint)
						InitItem();

					m_nCurSelect = -1;
					break;
				case WIDC_BUTTON4:	// Finish ��ư
				{
					if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint) {
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
						g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					}
				}
				break;
			}
		}
	}

	if (nID == 10000) {
		if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint) {
			CWndBase * pWndBase = (CWndBase *)g_WndMng.GetWndBase(APP_QUEITMWARNING);
			if (pWndBase == NULL) {
				SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
				g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
				g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
				g_WndMng.m_pWndReSkillWarning->InitItem(TRUE);
			}
			return TRUE;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSkillTreeEx::SetJob(int nJob) {
	m_nJob = nJob;
}
void CWndSkillTreeEx::OnLButtonUp(UINT nFlags, CPoint point) {
	m_bDrag = FALSE;
}
void CWndSkillTreeEx::OnLButtonDown(UINT nFlags, CPoint point) {
	CPoint ptMouse = GetMousePoint();

	CWndStatic * pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC2);
	CRect rect = pStatic->GetWindowRect(TRUE);
	if (rect.PtInRect(point)) {
		if (m_bSlot[0]) {
			SetActiveSlot(0, FALSE);
		} else {
			SetActiveSlot(0, TRUE);
		}
	}

	pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC3);
	rect = pStatic->GetWindowRect(TRUE);
	if (rect.PtInRect(point)) {
		if (m_bSlot[1]) {
			SetActiveSlot(1, FALSE);
			if (FULLSCREEN_HEIGHT == 600 && m_bLegend) {
				SetActiveSlot(2, TRUE);
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();

				CPoint point(x, y);
				Move(point);
			}
		} else {
			SetActiveSlot(1, TRUE);
			if (FULLSCREEN_HEIGHT == 600 && m_bLegend) {
				SetActiveSlot(2, FALSE);
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();

				CPoint point(x, y);
				Move(point);
			}
		}
	}

	pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
	rect = pStatic->GetWindowRect(TRUE);
	if (rect.PtInRect(point)) {
		if (m_bSlot[2]) {
			SetActiveSlot(2, FALSE);
			if (FULLSCREEN_HEIGHT == 600 && m_bLegend) {
				SetActiveSlot(1, TRUE);
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();

				CPoint point(x, y);
				Move(point);
			}
		} else {
			SetActiveSlot(2, TRUE);
			if (FULLSCREEN_HEIGHT == 600 && m_bLegend) {
				SetActiveSlot(1, FALSE);
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();

				CPoint point(x, y);
				Move(point);
			}
		}
	}

	if (m_bLegend) {
		pStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC1);
		rect = pStatic->GetWindowRect(TRUE);
		if (rect.PtInRect(point)) {
			if (m_bSlot[3])
				SetActiveSlot(3, FALSE);
			else
				SetActiveSlot(3, TRUE);
		}
	}


	for (int i = 0; std::cmp_less(i, m_skills.size()); ++i) {
		SKILL * pSkill = &m_skills[i];
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);

		if (pSkillProp) {
			if (!m_bSlot[0]) {
				if (pSkillProp->dwItemKind1 == JTYPE_BASE)
					continue;
			}

			if (!m_bSlot[1]) {
				if (pSkillProp->dwItemKind1 == JTYPE_EXPERT)
					continue;
			}

			if (!m_bSlot[2]) {
				if (pSkillProp->dwItemKind1 == JTYPE_PRO)
					continue;
			}

			if (!m_bSlot[3]) {
				if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
					continue;
			}
			GetCalcImagePos(pSkillProp->dwItemKind1);
		}

		if (pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID) {
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect) && CheckSkill(i)) {
				rect.top -= 2;

				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;

				rect.OffsetRect(0, m_nTopDownGap);

				if (rect.PtInRect(ptMouse)) {
					m_nCurSelect = i;
					m_bDrag = TRUE;
					break;
				}
			}
		}
	}
}
void CWndSkillTreeEx::OnRButtonDblClk(UINT nFlags, CPoint point) {

}

void CWndSkillTreeEx::OnLButtonDblClk(UINT nFlags, CPoint point) {
	// ��ųâ���� ����Ŭ���ϸ� �ڵ����� ��ų�ٿ� ��ϵȴ�?.

	for (int i = 0; std::cmp_less(i, m_skills.size()); i++) {
		LPSKILL pSkill = GetSkill(i);
		if (pSkill == NULL)
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);

		if (pSkillProp) {
			if (!m_bSlot[0]) {
				if (pSkillProp->dwItemKind1 == JTYPE_BASE)
					continue;
			}

			if (!m_bSlot[1]) {
				if (pSkillProp->dwItemKind1 == JTYPE_EXPERT)
					continue;
			}

			if (!m_bSlot[2]) {
				if (pSkillProp->dwItemKind1 == JTYPE_PRO)
					continue;
			}
			if (!m_bSlot[3]) {
				if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
					continue;
			}

			GetCalcImagePos(pSkillProp->dwItemKind1);
		}

		if (pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID) {
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect) && CheckSkill(i)) {
				rect.top -= 2;

				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;

				CPoint ptMouse = GetMousePoint();

				rect.OffsetRect(0, m_nTopDownGap);

				if (rect.PtInRect(ptMouse)) {
					m_nCurSelect = i;

					if (g_pPlayer->CheckSkill(dwSkill) == FALSE)
						return;
					CWndTaskBar * pTaskBar = g_WndMng.m_pWndTaskBar;
					if (pTaskBar->m_nExecute == 0)		// ��ųť�� ������ �������� ��ϵ�?.
						pTaskBar->SetSkillQueue(pTaskBar->m_nCurQueueNum, dwSkill, m_skillsTexture[i]);
					break;
				}
			}
		}
	}
}

void CWndSkillTreeEx::AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat) {
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC5);
	CPoint pt = lpWndCtrl->rect.TopLeft() + pt2;

	pt.y += 2;

	GetCalcImagePos(JTYPE_EXPERT);
	pt.y += m_nTopDownGap;

	if (m_atexJobPannel[0] && m_bSlot[1]) PaintTexture(pDest, m_atexJobPannel[0], pt, size);

	lpWndCtrl = GetWndCtrl(WIDC_STATIC7);
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	GetCalcImagePos(JTYPE_PRO);
	pt.y += 2;
	pt.y += m_nTopDownGap;

	if (m_atexJobPannel[1] && m_bSlot[2]) PaintTexture(pDest, m_atexJobPannel[1], pt, size);


}
