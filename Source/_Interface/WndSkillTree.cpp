#include "StdAfx.h"
#include "WndSkillTree.h"
#include "WndField.h"
#include "AppDefine.h"
#include "defineSkill.h"
#include "defineText.h"
#include "DPClient.h"
#include "sqktd/algorithm.h"

/////////////

CWndReSkillWarning::~CWndReSkillWarning() {
	Destroy();
}

void CWndReSkillWarning::OnDestroy() {
	if (m_bParentDestroy) {
		g_WndMng.GetWndBase(APP_SKILL_)->Destroy();
	}
}

void CWndReSkillWarning::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	if (CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_CONTEXT)) {
		pWndEdit->SetString(_T(prj.GetText(TID_GAME_SKILLLEVEL_CHANGE)));
		pWndEdit->EnableWindow(FALSE);
	}

	MoveParentCenter();
}

BOOL CWndReSkillWarning::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_QUEITMWARNING, pWndParent, WBS_MODAL, CPoint(0, 0));
}

BOOL CWndReSkillWarning::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_BTN_YES || message == EN_RETURN) {
		if (auto * pSkillTree = g_WndMng.GetWndBase<CWndSkillTreeCommon>(APP_SKILL_)) {
			g_DPlay.SendDoUseSkillPoint(pSkillTree->GetSkills());
		}

		Destroy();
	} else if (nID == WIDC_BTN_NO) {
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////////////////////////////////////////////////////////

void CWndSkillTreeCommon::ReInitIfOpen() {
	if (APP_SKILL_ == APP_SKILL3) {
		CWndSkillTreeEx * wnd = g_WndMng.GetWndBase<CWndSkillTreeEx>(APP_SKILL3);
		if (wnd) wnd->InitItem();
	} else if (APP_SKILL_ == APP_SKILL_V16) {
		CWndSkill_16 * wnd = g_WndMng.GetWndBase<CWndSkill_16>(APP_SKILL_V16);
		if (wnd) wnd->InitItem();
	}
}

///////////////////////////////////////

const char * CWndSkillTreeCommon::GetBackgroundFilename(const int nJob) {
	switch (nJob) {
		case JOB_VAGRANT:							return "ImgSkillVagrant.tga";
		case JOB_MERCENARY:						return "Back_Me.TGA";
		case JOB_ACROBAT:							return "Back_Acr.tga";
		case JOB_ASSIST:							return "Back_As.TGA";
		case JOB_MAGICIAN:						return "Back_Ma.TGA";
		case JOB_KNIGHT:							return "Back_Night.TGA";
		case JOB_BLADE:								return "Back_Blade.TGA";
		case JOB_BILLPOSTER:					return "Back_Bill.TGA";
		case JOB_RINGMASTER:					return "Back_Ring.TGA";
		case JOB_ELEMENTOR:						return "Back_Ele.TGA";
		case JOB_PSYCHIKEEPER:				return "Back_Psy.TGA";
		case JOB_JESTER:							return "Back_Jst.TGA";
		case JOB_RANGER:							return "Back_Rag.TGA";
		case JOB_LORDTEMPLER_HERO:		return "SkillTreeLord.tga";
		case JOB_STORMBLADE_HERO:			return "SkillTreeStormb.tga";
		case JOB_WINDLURKER_HERO:			return "SkillTreeWindI.tga";
		case JOB_CRACKSHOOTER_HERO:		return "SkillTreeCracks.tga";
		case JOB_FLORIST_HERO:				return "SkillTreeFlor.tga";
		case JOB_FORCEMASTER_HERO:		return "SkillTreeForcm.tga";
		case JOB_MENTALIST_HERO:			return "SkillTreeMent.tga";
		case JOB_ELEMENTORLORD_HERO:	return "SkillTreeElel.tga";
		default:
			Error(__FUNCTION__"(): Invalid job %d", nJob);
			return nullptr;
	}
}

const char * CWndSkillTreeCommon::GetHeroBackground(const int nJob) {
	// Master Skill is 1 Lv from the start, so the background image is excluded from skill icon image.
	switch (nJob) {
		case JOB_KNIGHT_HERO:
		case JOB_LORDTEMPLER_HERO:
			return "Back_Hero_KntDrawing.tga";
		case JOB_BLADE_HERO:
		case JOB_STORMBLADE_HERO:
			return "Back_Hero_BldDefence.tga";
		case JOB_BILLPOSTER_HERO:
		case JOB_FORCEMASTER_HERO:
			return "Back_Hero_BilDisEnchant.tga";
		case JOB_RINGMASTER_HERO:
		case JOB_FLORIST_HERO:
			return "Back_Hero_RigReturn.tga";
		case JOB_ELEMENTOR_HERO:
		case JOB_ELEMENTORLORD_HERO:
			return "Back_Hero_EleCursemind.tga";
		case JOB_PSYCHIKEEPER_HERO:
		case JOB_MENTALIST_HERO:
			return "Back_Hero_PsyStone.tga";
		case JOB_JESTER_HERO:
		case JOB_WINDLURKER_HERO:
			return "Back_Hero_JstSilence.tga";
		case JOB_RANGER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			return "Back_Hero_RagHawkeye.tga";
		default:
			return nullptr;
	}
}


///////////////////////////////////////

[[nodiscard]] boost::container::static_vector<DWORD, 4> CWndSkillTreeCommon::JobToTabJobs(const int nJob) {
	boost::container::static_vector<DWORD, 4> result;

	const auto jobLine = prj.jobs.GetAllJobsOfLine(nJob);

	// 0 = Vagrant
	result.emplace_back(JOB_VAGRANT);

	// 1 = Expert
	if (jobLine.size() <= 1) return result;
	result.emplace_back(jobLine[1]);

	// 2 = Pascal Praud
	if (jobLine.size() <= 2) return result;
	result.emplace_back(jobLine[2]);

	// 3 = Master
	// 4 = Hero

	// 5 = Legend Hero
	if (jobLine.size() <= 5) return result;
	result.emplace_back(jobLine[5]);

	return result;
}

bool CWndSkillTreeCommon::IsSkillHigherThanReal(const SKILL & windowSkill) {
	const SKILL * realSkill = g_pPlayer->GetSkill(windowSkill.dwSkill);
	return realSkill && realSkill->dwLevel < windowSkill.dwLevel;
}

CWndSkillTreeCommon::SkillStatus CWndSkillTreeCommon::GetSkillStatus(const SKILL & pSkill) const {
	// Is it usable?
	if (!g_pPlayer) return SkillStatus::No;

	const SKILL * playerSkill = g_pPlayer->GetSkill(pSkill.dwSkill);
	if (!playerSkill) return SkillStatus::No;

	if (playerSkill->dwLevel > 0) return SkillStatus::Usable;

	// Is it learnable?
	const ItemProp * pSkillProp = pSkill.GetProp();
	if (!pSkillProp) return SkillStatus::No;

	if (!g_pPlayer->HasLevelForSkill(*pSkillProp))
		return SkillStatus::No;

	if (pSkillProp->dwReSkill1 != NULL_ID) {
		const SKILL * reqSkill = m_apSkills.FindBySkillId(pSkillProp->dwReSkill1);
		if (!reqSkill || reqSkill->dwLevel < pSkillProp->dwReSkillLevel1) {
			return SkillStatus::No;
		}
	}

	if (pSkillProp->dwReSkill2 != NULL_ID) {
		const SKILL * reqSkill = m_apSkills.FindBySkillId(pSkillProp->dwReSkill2);
		if (!reqSkill || reqSkill->dwLevel < pSkillProp->dwReSkillLevel2) {
			return SkillStatus::No;
		}
	}

	return SkillStatus::Learnable;
}

void CWndSkillTreeCommon::ResetSkills() {
	m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
	m_apSkills = g_pPlayer->m_jobSkills;

	for (const SKILL & skill : m_apSkills) {
		const ItemProp * pSkillProp = skill.GetProp();
		if (!pSkillProp) continue;

		CTexture * texture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pSkillProp->szIcon), 0xffff00ff);
		if (!texture) continue;

		m_pTexSkill.emplace(skill.dwSkill, texture);
	}
}

void CWndSkillTreeCommon::OnSkillPointDown(const SKILL & reducedSkill) {
	for (SKILL & inPlaceSkill : m_apSkills) {
		const ItemProp * prop = inPlaceSkill.GetProp();
		if (!prop) continue;

		if (prop->dwReSkill1 == reducedSkill.dwSkill) {
			if (prop->dwReSkillLevel1) {
				if (reducedSkill.dwLevel < prop->dwReSkillLevel1) {
					if (inPlaceSkill.dwLevel > 0) {
						m_nCurrSkillPoint += (prj.GetSkillPoint(prop) * inPlaceSkill.dwLevel);
						inPlaceSkill.dwLevel = 0;
						OnSkillPointDown(inPlaceSkill);
					}
				}
			}
		}

		if (prop->dwReSkill2 == reducedSkill.dwSkill) {
			if (prop->dwReSkillLevel2) {
				if (reducedSkill.dwLevel < prop->dwReSkillLevel2) {
					if (inPlaceSkill.dwLevel > 0) {
						m_nCurrSkillPoint += (prj.GetSkillPoint(prop) * inPlaceSkill.dwLevel);
						inPlaceSkill.dwLevel = 0;
						OnSkillPointDown(inPlaceSkill);
					}
				}
			}
		}
	}
}

CTexture * CWndSkillTreeCommon::GetTextureOf(const SKILL & skill) const {
	return sqktd::find_in_map(m_pTexSkill, skill.dwSkill, nullptr);
}

//////////////

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

	return S_OK;
}

void CWndSkillTreeEx::InitItem() {
	if (!g_pPlayer) return;

	ResetSkills();
	m_dwMouseSkill = NULL_ID;
	m_focusedSkill = nullptr;

	m_nJob = g_pPlayer->m_nJob;

	CString strTex[2];

	CWndStatic * lpWndStatic1 = GetDlgItem<CWndStatic>(WIDC_STATIC2);
	lpWndStatic1->SetTitle(prj.jobs.info[JOB_VAGRANT].szName);


	const auto jobTabs = JobToTabJobs(m_nJob);

	CWndStatic * lpWndStatic2 = GetDlgItem<CWndStatic>(WIDC_STATIC3);
	if (jobTabs.size() < 2) {
		strTex[0] = "";
		lpWndStatic2->SetTitle("");
	} else {
		const char * bg = GetBackgroundFilename(jobTabs[1]);
		strTex[0] = bg ? bg : "";
		lpWndStatic2->SetTitle(prj.jobs.info[jobTabs[1]].szName);
	}

	CWndStatic * lpWndStatic3 = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
	if (jobTabs.size() < 3) {
		strTex[1] = "";
		lpWndStatic3->SetTitle("");
	} else {
		const char * bg = GetBackgroundFilename(jobTabs[2]);
		strTex[1] = bg ? bg : "";
		lpWndStatic3->SetTitle(prj.jobs.info[jobTabs[2]].szName);
	}

	m_strHeroSkilBg = GetHeroBackground(m_nJob);

	
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
	return TRUE;
}

void CWndSkillTreeEx::LoadTextureSkillicon() {
	m_textPackNum.LoadScript(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("icon_IconSkillLevel.inc")));
}

void CWndSkillTreeEx::OnMouseMove(UINT nFlags, CPoint point) {
	if (m_bDrag == FALSE)
		return;

	if (!m_focusedSkill) return;

	if (GetSkillStatus(*m_focusedSkill) == SkillStatus::Usable) {
		m_bDrag = FALSE;
		const ItemProp * pSkillProp = m_focusedSkill->GetProp();

		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = ShortcutType::Skill;
		m_GlobalShortcut.m_dwIndex = m_focusedSkill->dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId = m_focusedSkill->dwSkill;
		m_GlobalShortcut.m_pTexture = GetTextureOf(*m_focusedSkill);
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

	if (m_focusedSkill && 0 < g_pPlayer->m_nSkillPoint) {
		LPSKILL lpSkillUser = g_pPlayer->GetSkill(m_focusedSkill->dwSkill);
		const ItemProp * pSkillProp = m_focusedSkill->GetProp();
		if (pSkillProp == NULL || lpSkillUser == NULL)
			return TRUE;

		int nPoint = prj.GetSkillPoint(pSkillProp);
		if (m_focusedSkill->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint)
			m_pWndButton[0]->EnableWindow(TRUE);

		if (m_focusedSkill->dwLevel != lpSkillUser->dwLevel)
			m_pWndButton[1]->EnableWindow(TRUE);

		if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint) {
			m_pWndButton[2]->EnableWindow(TRUE);
			m_pWndButton[3]->EnableWindow(TRUE);
		}

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

	for (SKILL & skill : m_apSkills) {
		const ItemProp * pSkillProp = skill.GetProp();

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

			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect)) {
				rect.top -= 2;
				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;
				rect.OffsetRect(0, m_nTopDownGap);

				if (rect.PtInRect(point)) {
					dwMouseSkill = skill.dwSkill;

					ClientToScreen(&point);
					ClientToScreen(&rect);
					g_WndMng.PutToolTip_Skill(skill.dwSkill, skill.dwLevel, point, &rect, 
						(GetSkillStatus(skill) != SkillStatus::No) ? TRUE : FALSE
					);
					break;
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

		if (m_strHeroSkilBg) {
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

	for (const SKILL & pSkill : m_apSkills) {
		const DWORD dwSkill = pSkill.dwSkill;

		const ItemProp * pSkillProp = pSkill.GetProp();
		if (!pSkillProp) continue;

		if (!m_bSlot[0] && pSkillProp->dwItemKind1 == JTYPE_BASE)
			continue;

		if (!m_bSlot[1] && pSkillProp->dwItemKind1 == JTYPE_EXPERT)
			continue;

		if (!m_bSlot[2] && pSkillProp->dwItemKind1 == JTYPE_PRO)
			continue;

		if (!m_bSlot[3]) {
			if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
				continue;
		}

		GetCalcImagePos(pSkillProp->dwItemKind1);

		if (pSkillProp->nLog == 1) {
			continue;
		}


		CTexture * skillTexture = GetTextureOf(pSkill);
		if (!skillTexture) continue;

		const SkillStatus status = GetSkillStatus(pSkill);
		if (status == SkillStatus::No) continue;

		// ��ų ������ ���? 
		if (pSkill.dwLevel > 0) {
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect)) {
				rect.top -= 2;
				rect.OffsetRect(0, m_nTopDownGap);
				skillTexture->Render(p2DRender, rect.TopLeft(), CPoint(27, 27));
				int nAddNum = 0;
				LPSKILL pSkillUser = g_pPlayer->GetSkill(pSkill.dwSkill);
				if (pSkillUser && pSkill.dwLevel != pSkillUser->dwLevel)
					nAddNum = 20;
				if (pSkill.dwLevel < pSkillProp->dwExpertMax)
					m_textPackNum.Render(p2DRender, rect.TopLeft() - CPoint(2, 2), pSkill.dwLevel - 1 + nAddNum);
				else
					m_textPackNum.Render(p2DRender, rect.TopLeft() - CPoint(2, 2), 19 + nAddNum);
			}
		} else if (m_dwMouseSkill == dwSkill) {
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect)) {
				rect.top -= 2;
				rect.OffsetRect(0, m_nTopDownGap);
				skillTexture->Render(p2DRender, rect.TopLeft(), CPoint(27, 27));
			}
		}
		
	}

	CWndStatic * lpWndStatic9 = (CWndStatic *)GetDlgItem(WIDC_STATIC9);
	CString strSP;
	strSP.Format("%d", m_nCurrSkillPoint);
	lpWndStatic9->SetTitle(strSP);

	// ���õ� ��ų�� ������ �� â�� ���?
	if (m_focusedSkill) {
		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
		GetTextureOf(*m_focusedSkill)->Render(p2DRender, lpWndCtrl->rect.TopLeft());

		const ItemProp * pSkillProp = m_focusedSkill->GetProp();
		if (pSkillProp && m_focusedSkill->dwLevel > 0) {
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill(m_focusedSkill->dwSkill);
			if (pSkillUser && m_focusedSkill->dwLevel != pSkillUser->dwLevel)
				nAddNum = 20;

			if (m_focusedSkill->dwLevel < pSkillProp->dwExpertMax)
				m_textPackNum.Render(p2DRender, lpWndCtrl->rect.TopLeft(), m_focusedSkill->dwLevel - 1 + nAddNum);
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

	m_focusedSkill = nullptr;

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
	if (m_focusedSkill && g_pPlayer->m_nSkillPoint > 0) {
		const ItemProp * pSkillProp = m_focusedSkill->GetProp();
		if (pSkillProp) {
			const int nPoint = prj.GetSkillPoint(pSkillProp);
			switch (nID) {
				case WIDC_BUTTON1:	// + ��ư
					if (nPoint <= m_nCurrSkillPoint) {
						if (m_focusedSkill->dwLevel < pSkillProp->dwExpertMax) {
							m_nCurrSkillPoint -= nPoint;
							++m_focusedSkill->dwLevel;
						}
					}
					break;
				case WIDC_BUTTON2:	// - ��ư
					if (IsSkillHigherThanReal(*m_focusedSkill)) {
						m_nCurrSkillPoint += nPoint;
						--m_focusedSkill->dwLevel;
						OnSkillPointDown(*m_focusedSkill);
					}
					break;
				case WIDC_BUTTON3:	// Reset ��ư
					if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint)
						InitItem();

					m_focusedSkill = nullptr;
					break;
				case WIDC_BUTTON4:	// Finish ��ư
				{
					if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint) {
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning(false);
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
				g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning(true);
				g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
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


	for (SKILL & pSkill : m_apSkills) {
		const DWORD dwSkill = pSkill.dwSkill;
		const ItemProp * pSkillProp = pSkill.GetProp();

		if (!pSkillProp) continue;

		if (!m_bSlot[0] && pSkillProp->dwItemKind1 == JTYPE_BASE) {
			continue;
		}

		if (!m_bSlot[1] && pSkillProp->dwItemKind1 == JTYPE_EXPERT) {
			continue;
		}

		if (!m_bSlot[2] && pSkillProp->dwItemKind1 == JTYPE_PRO) {
			continue;
		}

		if (!m_bSlot[3]) {
			if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
				continue;
		}
		GetCalcImagePos(pSkillProp->dwItemKind1);


		if (pSkillProp->nLog != 1 && dwSkill != NULL_ID) {
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect) && GetSkillStatus(pSkill) != SkillStatus::No) {
				rect.top -= 2;

				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;

				rect.OffsetRect(0, m_nTopDownGap);

				if (rect.PtInRect(ptMouse)) {
					m_focusedSkill = &pSkill;
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

	for (SKILL & pSkill : m_apSkills) {
		DWORD dwSkill = pSkill.dwSkill;

		const ItemProp * pSkillProp = pSkill.GetProp();
		if (!pSkillProp) continue;

		if (!m_bSlot[0] && pSkillProp->dwItemKind1 == JTYPE_BASE) {
			continue;
		}

		if (!m_bSlot[1] && pSkillProp->dwItemKind1 == JTYPE_EXPERT) {
			continue;
		}

		if (!m_bSlot[2] && pSkillProp->dwItemKind1 == JTYPE_PRO) {
			continue;
		}

		if (!m_bSlot[3]) {
			if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
				continue;
		}

		GetCalcImagePos(pSkillProp->dwItemKind1);
		

		if (pSkillProp->nLog != 1 && dwSkill != NULL_ID) {
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect) && GetSkillStatus(pSkill) != SkillStatus::Learnable) {
				rect.top -= 2;

				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;

				CPoint ptMouse = GetMousePoint();

				rect.OffsetRect(0, m_nTopDownGap);

				if (rect.PtInRect(ptMouse)) {
					m_focusedSkill = &pSkill;

					CWndTaskBar * pTaskBar = g_WndMng.m_pWndTaskBar;
					if (pTaskBar->m_nExecute == 0)		// ��ųť�� ������ �������� ��ϵ�?.
						pTaskBar->SetSkillQueue(pTaskBar->m_nCurQueueNum, dwSkill, GetTextureOf(pSkill));
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
