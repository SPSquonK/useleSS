#include "StdAfx.h"
#include "AppDefine.h"
#include "WndSkillTree.h"
#include "defineSkill.h"
#include "sqktd/algorithm.h"


///////////////////////////////////////////////////////////////////////////////

BOOL CWndSkill_16::Initialize(CWndBase * pWndParent, DWORD nType) {
	return CWndNeuz::InitDialog(APP_SKILL_V16, pWndParent, WBS_KEY, 0);
}

void CWndSkill_16::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();

	if (!g_pPlayer) {
		Error(__FUNCTION__"(): g_pPlayer is NULL");
		return;
	}

	InitItem();

	m_buttonPlus  = GetDlgItem<CWndButton>(WIDC_BUTTON_PLUS);
	m_buttonMinus = GetDlgItem<CWndButton>(WIDC_BUTTON_MINUS);
	m_buttonOk    = GetDlgItem<CWndButton>(WIDC_BUTTON_OK);
	m_buttonReset = GetDlgItem<CWndButton>(WIDC_BUTTON_BACK);

	m_pTexSeletionBack = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndPosMark.tga"), 0xffff00ff);
	assert(m_pTexSeletionBack);

	MoveParentCenter();
}

BOOL CWndSkill_16::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_BUTTON_C1:
			m_selectedTab = TabType::Vagrant;
			InitItem();
			break;
		case WIDC_BUTTON_C2:
			m_selectedTab = TabType::Expert;
			InitItem();
			break;
		case WIDC_BUTTON_C3:
			m_selectedTab = TabType::Pro;
			InitItem();
			break;
		case WIDC_BUTTON_C4:
			m_selectedTab = TabType::LegendHero;
			InitItem();
			break;
	}

	if (m_pFocusItem && g_pPlayer->m_nSkillPoint > 0) {
		if (const ItemProp * pSkillProp = m_pFocusItem->GetProp()) {
			const int nPoint = prj.GetSkillPoint(pSkillProp);

			switch (nID) {
				case WIDC_BUTTON_PLUS:
					if (m_nCurrSkillPoint >= nPoint && m_pFocusItem->dwLevel < pSkillProp->dwExpertMax) {
						m_nCurrSkillPoint -= nPoint;
						++m_pFocusItem->dwLevel;
					}
					break;

				case WIDC_BUTTON_MINUS:
					if (IsSkillHigherThanReal(*m_pFocusItem)) {
						m_nCurrSkillPoint += nPoint;
						--m_pFocusItem->dwLevel;

						OnSkillPointDown(*m_pFocusItem);
					}
					break;

				case WIDC_BUTTON_BACK:
					if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint)
						InitItem();
					m_pFocusItem = nullptr;
					break;

				case WIDC_BUTTON_OK:
					if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint) {
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning(false);
						g_WndMng.m_pWndReSkillWarning->Initialize(nullptr);
					}
					break;
			}

		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSkill_16::InitItem() {
	if (!g_pPlayer) return;

	// Reset skills and points
	ResetSkills();
	m_pFocusItem = nullptr;
	m_dwMouseSkill = NULL_ID;

	// 
	const auto tabs = JobToTabJobs(g_pPlayer->GetJob());

	if (static_cast<size_t>(m_selectedTab) >= tabs.size()) {
		m_selectedTab = TabType::Vagrant;
	}

	const int selectedJob = static_cast<int>(tabs[static_cast<size_t>(m_selectedTab)]);

	InitItem_FillJobNames();

	m_strHeroSkilBg = GetHeroBackground(selectedJob);

	const char * background = GetBackgroundFilename(selectedJob);
	if (background != nullptr) {
		m_pTexJobPannel = std::make_unique<IMAGE>();
		const bool ok = LoadImage(MakePath(DIR_THEME, background), m_pTexJobPannel.get());
		if (!ok) {
			Error(__FUNCTION__"(): Could not load panel background %s", background);
			m_pTexJobPannel = nullptr;
		}
	} else {
		m_pTexJobPannel = nullptr;
	}

	AdjustWndBase();

	InitItem_AutoControlClassBtn();

}

void CWndSkill_16::InitItem_FillJobNames() {
	CWndStatic * const lpWndStatic1   = GetDlgItem<CWndStatic>(WIDC_STATIC_CLASSNAME);
	CWndStatic * const lpWndStatic_C1 = GetDlgItem<CWndStatic>(WIDC_STATIC_C1);
	CWndStatic * const lpWndStatic_C2 = GetDlgItem<CWndStatic>(WIDC_STATIC_C2);
	CWndStatic * const lpWndStatic_C3 = GetDlgItem<CWndStatic>(WIDC_STATIC_C3);
	CWndStatic * const lpWndStatic_C4 = GetDlgItem<CWndStatic>(WIDC_STATIC_C4);

	const auto jobLine = prj.jobs.GetAllJobsOfLine(g_pPlayer->GetJob());
	lpWndStatic_C1->SetTitle(prj.jobs.info[JOB_VAGRANT].szName);

	if (jobLine.size() > 1) {
		lpWndStatic_C2->SetTitle(prj.jobs.info[jobLine[1]].szName);
	} else {
		lpWndStatic_C2->SetTitle("CLOSED");
	}

	if (jobLine.size() > 2) {
		lpWndStatic_C3->SetTitle(prj.jobs.info[jobLine[2]].szName);
	} else {
		lpWndStatic_C3->SetTitle("CLOSED");
	}

	// 3 = Master
	// 4 = Hero

	if (jobLine.size() > 5) {
		lpWndStatic_C4->SetTitle(prj.jobs.info[jobLine[5]].szName);
	} else {
		lpWndStatic_C4->SetTitle("CLOSED");
	}

	switch (m_selectedTab) {
		case TabType::Vagrant:
			lpWndStatic1->SetTitle(lpWndStatic_C1->GetTitle());
			break;
		case TabType::Expert:
			lpWndStatic1->SetTitle(lpWndStatic_C2->GetTitle());
			break;
		case TabType::Pro:
			lpWndStatic1->SetTitle(lpWndStatic_C3->GetTitle());
			break;
		case TabType::LegendHero:
			lpWndStatic1->SetTitle(lpWndStatic_C4->GetTitle());
			break;
	}
}

std::optional<CRect> CWndSkill_16::GetSkillIconRect(DWORD dwSkillID) {
	const auto skillIconInfo = GetSkillIconInfo(dwSkillID);
	if (!skillIconInfo) return std::nullopt;

	if (std::holds_alternative<MasterPosition>(*skillIconInfo)) {
		return GetWndCtrl(WIDC_CUSTOM2)->rect;
	} else if (std::holds_alternative<HeroPosition>(*skillIconInfo)) {
		return GetWndCtrl(WIDC_CUSTOM3)->rect;
	} else if (const TabPosition * tabPos = std::get_if<TabPosition>(&*skillIconInfo)) {
		if (tabPos->tab == m_selectedTab) {
			LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC_PANNEL);

			CPoint point = lpWndCtrl->rect.TopLeft();
			point.Offset(-13, -88);

			return CRect(point, CSize(24, 24));

		} else {
			return std::nullopt;
		}
	} else {
		return std::nullopt;
	}
}

//-----------------------------------------------------------------------------
void CWndSkill_16::OnDraw(C2DRender * p2DRender) {
	if (m_strHeroSkilBg) {
		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM3);

		CPoint point = lpWndCtrl->rect.TopLeft();
		point.y -= 2;

		CTexture * pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, m_strHeroSkilBg), 0xffff00ff);
		if (pTexture) {
			pTexture->Render(p2DRender, point, CPoint(27, 27));
		}
	}

	for (const SKILL & pSkill : m_apSkills) {
		const ItemProp * pSkillProp = prj.GetSkillProp(pSkill.dwSkill);
		if (!pSkillProp) continue;
		
		auto pSkillRect = GetSkillIconRect(pSkill.dwSkill);
		if (!pSkillRect) continue;

		const SkillStatus skillStatus = GetSkillStatus(pSkill);
		if (skillStatus == SkillStatus::No) continue;

		const CPoint skillAnchor = pSkillRect->TopLeft() - CPoint(0, 2);

		if (&pSkill == m_pFocusItem) {
			m_pTexSeletionBack->Render(p2DRender, pSkillRect->TopLeft() - CPoint(4, 4), CPoint(31, 31));
		} else {
			if (skillStatus == SkillStatus::Learnable && pSkill.dwLevel == 0 && m_dwMouseSkill != pSkill.dwSkill) {
				continue;
			}
		}
		CTexture * texture = GetTextureOf(pSkill);
		if (!texture) continue;

		texture->Render(p2DRender, skillAnchor, CPoint(27, 27));

		if (pSkill.dwLevel > 0) {
			const SKILL * pSkillUser = g_pPlayer->GetSkill(pSkill.dwSkill);

			RenderLevel(
				p2DRender, skillAnchor - CPoint(2, 2),
				pSkill.dwLevel, pSkillProp->dwExpertMax,
				pSkillUser && pSkill.dwLevel != pSkillUser->dwLevel
				);
		}
	}

	//남은 스킬 포인트
	CWndStatic * lpWndStaticSP = GetDlgItem<CWndStatic>(WIDC_STATIC_SP);
	CString strTemp;
	strTemp.Format("%d", m_nCurrSkillPoint);
	lpWndStaticSP->SetTitle(strTemp);

	// 선택된 스킬이 있을시 위 창에 출력
	if (m_pFocusItem) {
		//선택한 스킬 표현슬롯
		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
		GetTextureOf(*m_pFocusItem)->Render(p2DRender, lpWndCtrl->rect.TopLeft() - CPoint(0, 2), CPoint(36, 36));

		const ItemProp * pSkillProp = m_pFocusItem->GetProp();
		if (!pSkillProp) return;

		if (pSkillProp && m_pFocusItem->dwLevel > 0) {
			const SKILL * pSkillUser = g_pPlayer->GetSkill(m_pFocusItem->dwSkill);

			RenderLevel(
				p2DRender, lpWndCtrl->rect.TopLeft(),
				m_pFocusItem->dwLevel, pSkillProp->dwExpertMax,
				pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel
			);
		}

		//선택한 스킬의 필요sp 출력
		CWndStatic * lpWndStaticSP = GetDlgItem<CWndStatic>(WIDC_STATIC_NEEDSP);
		DWORD dwNeedSp = prj.GetSkillPoint(pSkillProp);
		strTemp.Format("%d", dwNeedSp);
		lpWndStaticSP->SetTitle(strTemp);

	}
}

BOOL CWndSkill_16::Process() {
	if (g_pPlayer == NULL) return FALSE;

	m_buttonPlus ->EnableWindow(FALSE);			//plus
	m_buttonMinus->EnableWindow(FALSE);			//minus
	m_buttonOk   ->EnableWindow(FALSE);			//ok
	m_buttonReset->EnableWindow(FALSE);			//cancel


	if (m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint) {
		const SKILL * lpSkillUser = g_pPlayer->GetSkill(m_pFocusItem->dwSkill);
		const ItemProp * pSkillProp = m_pFocusItem->GetProp();
		if (!pSkillProp || !lpSkillUser)
			return TRUE;

		int nPoint = prj.GetSkillPoint(pSkillProp);
		if (m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint)
			m_buttonPlus->EnableWindow(TRUE);

		if (m_pFocusItem->dwLevel != lpSkillUser->dwLevel)
			m_buttonMinus->EnableWindow(TRUE);

		if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint) {
			m_buttonOk->EnableWindow(TRUE);
			m_buttonReset->EnableWindow(TRUE);
		}

		if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO) {
			m_buttonPlus->EnableWindow(FALSE);
			m_buttonMinus->EnableWindow(FALSE);
		}
	}

	return CWndBase::Process();
}
//-----------------------------------------------------------------------------
void CWndSkill_16::OnMouseWndSurface(CPoint point) {
	DWORD dwMouseSkill = NULL_ID;

	//포커스 슬롯 처리
	if (m_pFocusItem) {
		LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM1);  //포커스 슬롯
		if (lpWndCtrl1) {
			CRect rectFocus = lpWndCtrl1->rect;
			if (rectFocus.PtInRect(point)) {
				dwMouseSkill = m_pFocusItem->dwSkill;

				ClientToScreen(&point);
				ClientToScreen(&rectFocus);
				g_WndMng.PutToolTip_Skill(m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, point, &rectFocus, TRUE);

				m_dwMouseSkill = dwMouseSkill;
				return; //포커스 슬롯에 마우스가 온경우는 해당 스킬의 도움말을 보여주고 끝임. 트리쪽 검사 불필요
			}
		}
	}

	//스킬 트리 처리
	for (const SKILL & pSkill : m_apSkills) {
		const ItemProp * pSkillProp = pSkill.GetProp();

		const auto maybeRect = GetSkillIconRect(pSkill.dwSkill);

		if (maybeRect) {
			CRect rect = maybeRect.value();
			rect.top -= 2;
			rect.right = rect.left + 27;
			rect.bottom = rect.top + 27;

			if (rect.PtInRect(point)) {
				dwMouseSkill = pSkill.dwSkill;

				ClientToScreen(&point);
				ClientToScreen(&rect);
				g_WndMng.PutToolTip_Skill(
					pSkill.dwSkill, pSkill.dwLevel,
					point, &rect,
					(GetSkillStatus(pSkill) != SkillStatus::No) ? TRUE : FALSE
				);
				break;
			}
		}

	}

	m_dwMouseSkill = dwMouseSkill;
}

void CWndSkill_16::OnLButtonDown(UINT nFlags, CPoint point) {
	CPoint ptMouse = GetMousePoint();

	for (SKILL & pSkill : m_apSkills) {
		const auto rect = GetSkillIconRect(pSkill.dwSkill);
		if (!rect) continue;

		const SkillStatus status = GetSkillStatus(pSkill);
		if (status == SkillStatus::No) continue;

		CRect hitRect = rect.value();
		hitRect.top -= 2;

		hitRect.right = hitRect.left + 27;
		hitRect.bottom = hitRect.top + 27;

		if (hitRect.PtInRect(ptMouse)) {
			m_pFocusItem = &pSkill;
			m_bDrag = true;
			break;
		}
	}
}

void CWndSkill_16::OnLButtonUp(UINT nFlags, CPoint point) {
	m_bDrag = false;
}

void CWndSkill_16::OnMouseMove(UINT nFlags, CPoint point) {
	if (!m_bDrag) return;
	if (!m_pFocusItem) return;

	CPoint pt(3, 3);

	const SKILL * playerSkill = g_pPlayer->GetSkill(m_pFocusItem->dwSkill);
	if (playerSkill && playerSkill->dwLevel > 0) {
		m_bDrag = false;
		const DWORD dwSkill = m_pFocusItem->dwSkill;
		const ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);

		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = ShortcutType::Skill;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId = dwSkill;
		m_GlobalShortcut.m_pTexture = GetTextureOf(*m_pFocusItem);
		_tcscpy(m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}

void CWndSkill_16::AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat) {
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC_PANNEL);

	CPoint pt = lpWndCtrl->rect.TopLeft();
	pt.y += 26;

	if (m_pTexJobPannel) PaintTexture(pDest, m_pTexJobPannel.get(), pt, size);
}

int CWndSkill_16::GetJobByJobLevel(const TabType tab, const int realJob) {
	if (tab == TabType::Vagrant) {
		return JOB_VAGRANT;
	}

	const auto line = prj.jobs.GetAllJobsOfLine(realJob);
	// 0 = Vagrant, 1 = Pro, 2 = Expert, 3 = Master, 4 = Hero, 5 = LegendHero

	if (tab == TabType::Expert) {
		if (line.size() > 1) return line[1];
		else return -1;
	} else if (tab == TabType::Pro) {
		if (line.size() > 2) return line[2];
		return -1;
	} else if (tab == TabType::LegendHero) {
		if (line.size() > 5) return line[5];
		return -1;
	} else {
		return -1;
	}
}

void CWndSkill_16::InitItem_AutoControlClassBtn() {
	const auto tabs = JobToTabJobs(g_pPlayer->m_nJob);

	std::array<CWndButton *, 4> buttons{
		GetDlgItem<CWndButton>(WIDC_BUTTON_C1),
		GetDlgItem<CWndButton>(WIDC_BUTTON_C2),
		GetDlgItem<CWndButton>(WIDC_BUTTON_C3),
		GetDlgItem<CWndButton>(WIDC_BUTTON_C4)
	};

	for (size_t i = 0; i != 4; ++i) {
		CWndButton * jobButton = buttons[i];
		
		const char * buttonName;
		if (i < tabs.size()) buttonName = nullptr;
		else buttonName = GetFileNameClassBtn(static_cast<int>(tabs[i]));

		if (buttonName) {
			jobButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, buttonName));
			jobButton->EnableWindow(TRUE);
		} else {
			jobButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "SlotVan.bmp"));
			jobButton->EnableWindow(FALSE);
		}

		jobButton->SetCheck(0);
	}

	switch (m_selectedTab) {
		case TabType::Vagrant:		buttons[0]->SetCheck(2); break;
		case TabType::Expert:			buttons[1]->SetCheck(2); break;
		case TabType::Pro:				buttons[2]->SetCheck(2); break;
		case TabType::LegendHero: buttons[3]->SetCheck(2); break;
	}
}

const char * CWndSkill_16::GetFileNameClassBtn(const int nJob) {
	switch (nJob) {
		case JOB_VAGRANT:							return "SlotVan.bmp";

		case JOB_MERCENARY:						return "SlotMer.bmp";
		case JOB_ACROBAT:							return "SlotArc.bmp";
		case JOB_ASSIST:							return "SlotAs.bmp";
		case JOB_MAGICIAN:						return "SlotMag.bmp";

		case JOB_KNIGHT:							return "SlotKnight.bmp";
		case JOB_BLADE:								return "SlotBlad.bmp";
		case JOB_JESTER:							return "SlotJes.bmp";
		case JOB_RANGER:							return "SlotRan.bmp";
		case JOB_RINGMASTER:					return "SlotRingm.bmp";
		case JOB_BILLPOSTER:					return "SlotBillfor.bmp";
		case JOB_PSYCHIKEEPER:				return "SlotPsy.bmp";
		case JOB_ELEMENTOR:						return "SlotElem.bmp";

		case JOB_LORDTEMPLER_HERO:		return "SlotLord.bmp";
		case JOB_STORMBLADE_HERO:			return "SlotStormb.bmp";
		case JOB_WINDLURKER_HERO:			return "SlotWindI.bmp";
		case JOB_CRACKSHOOTER_HERO:		return "SlotCracks.bmp";
		case JOB_FLORIST_HERO:				return "SlotFlor.bmp";
		case JOB_FORCEMASTER_HERO:		return "SlotForcem.bmp";
		case JOB_MENTALIST_HERO:			return "SlotMent.bmp";
		case JOB_ELEMENTORLORD_HERO:	return "SlotElel.bmp";

		case JOB_PUPPETEER:
		case JOB_GATEKEEPER:
		case JOB_DOPPLER:
			return nullptr;

		default:
			Error(__FUNCTION__"(): Unknown job: %d", nJob);
			return nullptr;
	}
}

std::optional<CWndSkill_16::TabType> CWndSkill_16::GetMaxTabFromJob(const int nJob) {
	if (nJob == JOB_VAGRANT)		return TabType::Vagrant;
	if (nJob < MAX_EXPERT)			return TabType::Expert;
	if (nJob < MAX_HERO)				return TabType::Pro;
	if (nJob < MAX_LEGEND_HERO) return TabType::LegendHero;

	Error(__FUNCTION__"(): Invalid job %d", nJob);
	return std::nullopt;
}

HRESULT CWndSkill_16::DeleteDeviceObjects() {
	CWndBase::DeleteDeviceObjects();

	m_pTexJobPannel.reset();
	m_apSkills.clear();

	return S_OK;
}
