#include "StdAfx.h"
#include "AppDefine.h"
#include "WndSkillTree.h"
#include "defineSkill.h"
#include "sqktd/algorithm.h"

///////////////////////////////////////////////////////////////////////////////

[[nodiscard]] boost::container::static_vector<DWORD, 4> CWndSkill_16::JobToTabJobs(const int nJob) {
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

	m_bLegend = g_pPlayer->GetJobType() >= JTYPE_MASTER;

	m_pTexSeletionBack = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndPosMark.tga"), 0xffff00ff);
	assert(m_pTexSeletionBack);

	MoveParentCenter();
}

BOOL CWndSkill_16::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_BUTTON_C1:
			m_selectedTab = SelectedTab::Vagrant;
			InitItem();
			break;
		case WIDC_BUTTON_C2:
			m_selectedTab = SelectedTab::Expert;
			InitItem();
			break;
		case WIDC_BUTTON_C3:
			m_selectedTab = SelectedTab::Pro;
			InitItem();
			break;
		case WIDC_BUTTON_C4:
			m_selectedTab = SelectedTab::LegendHero;
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

						// SubSkillPointDown(curSel);
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
		m_selectedTab = SelectedTab::Vagrant;
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

	InitItem_LoadTextureSkillIcon();
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
		case SelectedTab::Vagrant:
			lpWndStatic1->SetTitle(lpWndStatic_C1->GetTitle());
			break;
		case SelectedTab::Expert:
			lpWndStatic1->SetTitle(lpWndStatic_C2->GetTitle());
			break;
		case SelectedTab::Pro:
			lpWndStatic1->SetTitle(lpWndStatic_C3->GetTitle());
			break;
		case SelectedTab::LegendHero:
			lpWndStatic1->SetTitle(lpWndStatic_C4->GetTitle());
			break;
	}
}

void CWndSkill_16::InitItem_LoadTextureSkillIcon() {
	m_kTexLevel.DeleteDeviceObjects();
	m_kTexLevel.LoadScript(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("icon_IconSkillLevel.inc")));
}

std::optional<CWndSkill_16::JobSkillPositionInfo> CWndSkill_16::GetSkillIconInfo(const DWORD dwSkillID) {
	using enum SelectedTab;
	
	switch (dwSkillID) {
		// Vagrant
		case SI_VAG_ONE_CLEANHIT:   return TabPosition(Vagrant,  94, 149);
		case SI_VAG_ONE_BRANDISH:   return TabPosition(Vagrant, 144, 149);
		case SI_VAG_ONE_OVERCUTTER: return TabPosition(Vagrant, 194, 149);
		// Mercenary
		case SI_MER_ONE_SPLMASH:				return TabPosition(Expert,  34, 109);
		case SI_MER_SHIELD_PROTECTION:	return TabPosition(Expert,  34, 161);
		case SI_MER_ONE_KEENWHEEL:			return TabPosition(Expert,  84,  97);
		case SI_MER_ONE_BLOODYSTRIKE:		return TabPosition(Expert,  84, 123);
		case SI_MER_SUP_IMPOWERWEAPON:	return TabPosition(Expert,  84, 175);
		case SI_MER_SHIELD_PANBARRIER:	return TabPosition(Expert,  84, 149);
		case SI_MER_ONE_BLINDSIDE:			return TabPosition(Expert, 134,  97);
		case SI_MER_ONE_REFLEXHIT:			return TabPosition(Expert, 134, 123);
		case SI_MER_ONE_SNEAKER:				return TabPosition(Expert, 134, 149);
		case SI_MER_SUP_SMITEAXE:				return TabPosition(Expert, 134, 175);
		case SI_MER_SUP_BLAZINGSWORD:		return TabPosition(Expert, 134, 201);
		case SI_MER_ONE_SPECIALHIT:			return TabPosition(Expert, 184,  97);
		case SI_MER_ONE_GUILOTINE:			return TabPosition(Expert, 184, 123);
		case SI_MER_SUP_AXEMASTER:			return TabPosition(Expert, 184, 175);
		case SI_MER_SUP_SWORDMASTER:		return TabPosition(Expert, 184, 201);

		// Knight
		case SI_KNT_TWOSW_CHARGE:				return TabPosition(Pro,  34, 260 - 138);
		case SI_KNT_TWOAX_PAINDEALER:		return TabPosition(Pro,  34, 286 - 138);
		case SI_KNT_SUP_GUARD:					return TabPosition(Pro,  34, 312 - 138);
		case SI_KNT_TWOSW_EARTHDIVIDER:	return TabPosition(Pro,  84, 260 - 138);
		case SI_KNT_TWOAX_POWERSTUMP:		return TabPosition(Pro,  84, 286 - 138);
		case SI_KNT_SUP_RAGE:						return TabPosition(Pro,  84, 312 - 138);
		case SI_KNT_TWO_POWERSWING:			return TabPosition(Pro, 134, 272 - 138);
		case SI_KNT_SUP_PAINREFLECTION:	return TabPosition(Pro, 134, 312 - 138);

		// Blade
		case SI_BLD_DOUBLESW_SILENTSTRIKE:	return TabPosition(Pro,  34, 260 - 138);
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	return TabPosition(Pro,  34, 286 - 138);
		case SI_BLD_DOUBLE_ARMORPENETRATE:	return TabPosition(Pro,  34, 312 - 138);
		case SI_BLD_DOUBLESW_BLADEDANCE:		return TabPosition(Pro,  84, 260 - 138);
		case SI_BLD_DOUBLEAX_HAWKATTACK:		return TabPosition(Pro,  84, 286 - 138);
		case SI_BLD_SUP_BERSERK:						return TabPosition(Pro,  84, 312 - 138);
		case SI_BLD_DOUBLE_CROSSSTRIKE:			return TabPosition(Pro, 134, 272 - 138);
		case SI_BLD_DOUBLE_SONICBLADE:			return TabPosition(Pro, 134, 312 - 138);

		// Assist
		case SI_ASS_HEAL_HEALING:					return TabPosition(Expert,  18, 135);
		case SI_ASS_HEAL_PATIENCE:				return TabPosition(Expert,  64, 109);
		case SI_ASS_CHEER_QUICKSTEP:			return TabPosition(Expert,  64, 135);
		case SI_ASS_CHEER_MENTALSIGN:			return TabPosition(Expert,  64, 161);
		case SI_ASS_KNU_TAMPINGHOLE:			return TabPosition(Expert,  64, 187);
		case SI_ASS_HEAL_RESURRECTION:		return TabPosition(Expert, 110, 109);
		case SI_ASS_CHEER_HASTE:					return TabPosition(Expert, 110, 135);
		case SI_ASS_CHEER_HEAPUP:					return TabPosition(Expert, 110, 161);
		case SI_ASS_CHEER_STONEHAND:			return TabPosition(Expert, 110, 187);
		case SI_ASS_CHEER_CIRCLEHEALING:	return TabPosition(Expert, 156, 109);
		case SI_ASS_CHEER_CATSREFLEX:			return TabPosition(Expert, 156, 135);
		case SI_ASS_CHEER_BEEFUP:					return TabPosition(Expert, 156, 161);
		case SI_ASS_KNU_BURSTCRACK:				return TabPosition(Expert, 156, 187);
		case SI_ASS_HEAL_PREVENTION:			return TabPosition(Expert, 202, 109);
		case SI_ASS_CHEER_CANNONBALL:			return TabPosition(Expert, 202, 135);
		case SI_ASS_CHEER_ACCURACY:				return TabPosition(Expert, 202, 161);
		case SI_ASS_KNU_POWERFIST:				return TabPosition(Expert, 202, 187);

		// Ringmaster
		case SI_RIN_SUP_PROTECT:						return TabPosition(Pro,  34, 260 - 138);
		case SI_RIN_SUP_HOLYCROSS:					return TabPosition(Pro,  34, 286 - 138);
		case SI_RIN_HEAL_GVURTIALLA:				return TabPosition(Pro,  34, 312 - 138);
		case SI_RIN_SUP_HOLYGUARD:					return TabPosition(Pro,  84, 260 - 138);
		case SI_RIN_SUP_SPIRITUREFORTUNE:		return TabPosition(Pro,  84, 286 - 138);
		case SI_RIN_HEAL_HEALRAIN:					return TabPosition(Pro,  84, 312 - 138);
		case SI_RIN_SQU_GEBURAHTIPHRETH:		return TabPosition(Pro, 134, 272 - 138);
		case SI_RIN_SUP_MERKABAHANZELRUSHA:	return TabPosition(Pro, 134, 312 - 138);

		// Billposter
		case SI_BIL_KNU_BELIALSMESHING:		return TabPosition(Pro,  34, 260 - 138);
		case SI_BIL_PST_ASMODEUS:					return TabPosition(Pro,  34, 286 - 138);
		case SI_BIL_KNU_BLOODFIST:				return TabPosition(Pro,  84, 260 - 138);
		case SI_BIL_PST_BARAQIJALESNA:		return TabPosition(Pro,  84, 286 - 138);
		case SI_BIL_KNU_PIERCINGSERPENT:	return TabPosition(Pro, 134, 260 - 138);
		case SI_BIL_PST_BGVURTIALBOLD:		return TabPosition(Pro, 134, 286 - 138);
		case SI_BIL_KNU_SONICHAND:				return TabPosition(Pro, 184, 272 - 138);
		case SI_BIL_PST_ASALRAALAIKUM:		return TabPosition(Pro, 184, 312 - 138);

		// Magician
		case SI_MAG_MAG_MENTALSTRIKE:						return TabPosition(Expert,  34,  96);
		case SI_MAG_MAG_BLINKPOOL: 							return TabPosition(Expert,  34, 148);
		case SI_MAG_FIRE_BOOMERANG:							return TabPosition(Expert,  84,  96);
		case SI_MAG_WIND_SWORDWIND:							return TabPosition(Expert,  84, 122);
		case SI_MAG_WATER_ICEMISSILE:						return TabPosition(Expert,  84, 148);
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:		return TabPosition(Expert,  84, 174);
		case SI_MAG_EARTH_SPIKESTONE:						return TabPosition(Expert,  84, 200);
		case SI_MAG_FIRE_HOTAIR:								return TabPosition(Expert, 134,  96);
		case SI_MAG_WIND_STRONGWIND:						return TabPosition(Expert, 134, 122);
		case SI_MAG_WATER_WATERBALL:						return TabPosition(Expert, 134, 148);
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:		return TabPosition(Expert, 134, 174);
		case SI_MAG_EARTH_ROCKCRASH:						return TabPosition(Expert, 134, 200);
		case SI_MAG_FIRE_FIRESTRIKE:						return TabPosition(Expert, 184,  96);
		case SI_MAG_WIND_WINDCUTTER:						return TabPosition(Expert, 184, 122);
		case SI_MAG_WATER_SPRINGWATER:					return TabPosition(Expert, 184, 148);
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	return TabPosition(Expert, 184, 174);
		case SI_MAG_EARTH_LOOTING:							return TabPosition(Expert, 184, 200);

		// Elementor
		case SI_ELE_FIRE_FIREBIRD:							return TabPosition(Pro,  34, 234 - 138);
		case SI_ELE_EARTH_STONESPEAR:						return TabPosition(Pro,  34, 260 - 138);
		case SI_ELE_WIND_VOID:									return TabPosition(Pro,  34, 286 - 138);
		case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	return TabPosition(Pro,  34, 312 - 138);
		case SI_ELE_WATER_ICESHARK:							return TabPosition(Pro,  34, 338 - 138);
		case SI_ELE_FIRE_BURINGFIELD:						return TabPosition(Pro,  84, 234 - 138);
		case SI_ELE_EARTH_EARTHQUAKE:						return TabPosition(Pro,  84, 260 - 138);
		case SI_ELE_WIND_WINDFIELD:							return TabPosition(Pro,  84, 286 - 138);
		case SI_ELE_ELECTRICITY_ELETRICSHOCK:		return TabPosition(Pro,  84, 312 - 138);
		case SI_ELE_WATER_POISONCLOUD:					return TabPosition(Pro,  84, 338 - 138);
		case SI_ELE_MULTY_METEOSHOWER:					return TabPosition(Pro, 134, 246 - 138);
		case SI_ELE_MULTY_SANDSTORM:						return TabPosition(Pro, 134, 272 - 138);
		case SI_ELE_MULTY_LIGHTINGSTORM:				return TabPosition(Pro, 134, 298 - 138);
		case SI_ELE_MULTY_AVALANCHE:						return TabPosition(Pro, 134, 338 - 138);
		case SI_ELE_FIRE_FIREMASTER:						return TabPosition(Pro, 184, 234 - 138);
		case SI_ELE_EARTH_EARTHMASTER:					return TabPosition(Pro, 184, 260 - 138);
		case SI_ELE_WIND_WINDMASTER:						return TabPosition(Pro, 184, 286 - 138);
		case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	return TabPosition(Pro, 184, 312 - 138);
		case SI_ELE_WATER_WATERMASTER:					return TabPosition(Pro, 184, 338 - 138);
		
		// Psych... Sorcerer :>
		case SI_PSY_NLG_DEMONOLGY:			return TabPosition(Pro,  34, 260 - 138);
		case SI_PSY_PSY_PSYCHICBOMB:		return TabPosition(Pro,  34, 286 - 138);
		case SI_PSY_NLG_CRUCIOSPELL:		return TabPosition(Pro,  34, 312 - 138);
		case SI_PSY_NLG_SATANOLGY:			return TabPosition(Pro,  84, 260 - 138);
		case SI_PSY_PSY_SPRITBOMB:			return TabPosition(Pro,  84, 286 - 138);
		case SI_PSY_PSY_MAXIMUMCRISIS:	return TabPosition(Pro,  84, 312 - 138);
		case SI_PSY_PSY_PSYCHICWALL:		return TabPosition(Pro, 134, 272 - 138);
		case SI_PSY_PSY_PSYCHICSQUARE:	return TabPosition(Pro, 134, 312 - 138);

		// Acrobat
		case SI_ACR_YOYO_PULLING:				return TabPosition(Expert,  34,  97);
		case SI_ACR_SUP_SLOWSTEP:				return TabPosition(Expert,  34, 123);
		case SI_ACR_BOW_JUNKBOW:				return TabPosition(Expert,  34, 175);
		case SI_ACR_SUP_FASTWALKER:			return TabPosition(Expert,  84,  96);
		case SI_ACR_SUP_YOYOMASTER:			return TabPosition(Expert,  84, 122);
		case SI_ACR_SUP_BOWMASTER: 			return TabPosition(Expert,  84, 174);
		case SI_ACR_SUP_DARKILLUSION:		return TabPosition(Expert, 134,  96);
		case SI_ACR_YOYO_SNITCH:				return TabPosition(Expert, 134, 122);
		case SI_ACR_YOYO_CROSSLINE:			return TabPosition(Expert, 134, 149);
		case SI_ACR_BOW_SILENTSHOT:			return TabPosition(Expert, 134, 174);
		case SI_ACR_BOW_AIMEDSHOT:			return TabPosition(Expert, 134, 200);
		case SI_ACR_YOYO_ABSOLUTEBLOCK:	return TabPosition(Expert, 184,  96);
		case SI_ACR_YOYO_DEADLYSWING:		return TabPosition(Expert, 184, 122);
		case SI_ACR_YOYO_COUNTER:				return TabPosition(Expert, 184, 149);
		case SI_ACR_BOW_AUTOSHOT:				return TabPosition(Expert, 184, 174);
		case SI_ACR_BOW_ARROWRAIN:			return TabPosition(Expert, 184, 200);

		// Jester
		case SI_JST_SUP_POISON:					return TabPosition(Pro,  34, 260 - 138);
		case SI_JST_SUP_BLEEDING:				return TabPosition(Pro,  34, 286 - 138);
		case SI_JST_YOYO_ESCAPE:				return TabPosition(Pro,  34, 312 - 138);
		case SI_JST_YOYO_CRITICALSWING:	return TabPosition(Pro,  84, 260 - 138);
		case SI_JST_YOYO_BACKSTAB:			return TabPosition(Pro,  84, 286 - 138);
		case SI_JST_SUP_ABSORB:					return TabPosition(Pro,  84, 312 - 138);
		case SI_JST_YOYO_VATALSTAB:			return TabPosition(Pro, 134, 272 - 138);
		case SI_JST_YOYO_HITOFPENYA:		return TabPosition(Pro, 134, 312 - 138);

		// Ranger
		case SI_RAG_BOW_ICEARROW:				return TabPosition(Pro,  34, 260 - 138);
		case SI_RAG_BOW_FLAMEARROW:			return TabPosition(Pro,  34, 286 - 138);
		case SI_RAG_BOW_POISONARROW:		return TabPosition(Pro,  34, 312 - 138);
		case SI_RAG_SUP_FASTATTACK:			return TabPosition(Pro,  84, 260 - 138);
		case SI_RAG_BOW_PIRCINGARROW:		return TabPosition(Pro,  84, 286 - 138);
		case SI_RAG_SUP_NATURE:					return TabPosition(Pro,  84, 312 - 138);
		case SI_RAG_BOW_TRIPLESHOT:			return TabPosition(Pro, 134, 272 - 138);
		case SI_RAG_BOW_SILENTARROW:		return TabPosition(Pro, 134, 312 - 138);

		// Master
		case SI_BLD_MASTER_ONEHANDMASTER:	return MasterPosition{};
		case SI_KNT_MASTER_TWOHANDMASTER:	return MasterPosition{};
		case SI_JST_MASTER_YOYOMASTER:		return MasterPosition{};
		case SI_RAG_MASTER_BOWMASTER:			return MasterPosition{};
		case SI_ELE_MASTER_INTMASTER:			return MasterPosition{};
		case SI_PSY_MASTER_INTMASTER:			return MasterPosition{};
		case SI_BIL_MASTER_KNUCKLEMASTER:	return MasterPosition{};
		case SI_RIG_MASTER_BLESSING:			return MasterPosition{};

		// Hero
		case SI_BLD_HERO_DEFFENCE:		return HeroPosition{};
		case SI_KNT_HERO_DRAWING:			return HeroPosition{};
		case SI_JST_HERO_SILENCE:			return HeroPosition{};
		case SI_RAG_HERO_HAWKEYE:			return HeroPosition{};
		case SI_ELE_HERO_CURSEMIND:		return HeroPosition{};
		case SI_PSY_HERO_STONE:				return HeroPosition{};
		case SI_BIL_HERO_DISENCHANT:	return HeroPosition{};
		case SI_RIG_HERO_RETURN:			return HeroPosition{};

		// Templar
		case SI_LOD_SUP_PULLING:			return TabPosition(LegendHero,  34, 122);
		case SI_LOD_ONE_GRANDRAGE:		return TabPosition(LegendHero, 34, 148);
		case SI_LOD_SUP_HOLYARMOR:		return TabPosition(LegendHero, 34, 174);
		case SI_LOD_SUP_SCOPESTRIKE:	return TabPosition(LegendHero, 84, 148);
		case SI_LOD_SUP_ANGER:				return TabPosition(LegendHero, 84, 174);
		case SI_LOD_ONE_SHILDSTRIKE:	return TabPosition(LegendHero, 134, 148);

		// Stormblade
		case SI_STORM_DOUBLE_CROSSBLOOD:		return TabPosition(LegendHero, 34, 122);
		case SI_STORM_SUP_POWERINCREASE:		return TabPosition(LegendHero, 34, 148);
		case SI_STORM_DOUBLE_STORMBLAST:		return TabPosition(LegendHero, 84, 122);
		case SI_STORM_DOUBLE_HOLDINGSTORM:	return TabPosition(LegendHero, 84, 148);

		// Windlurker
		case SI_WIN_SUP_EVASIONINCREASE:		return TabPosition(LegendHero, 34, 122);
		case SI_WIN_YOYO_MADHURRICANE:			return TabPosition(LegendHero, 34, 148);
		case SI_WIN_SUP_CONTROLINCREASE:		return TabPosition(LegendHero, 84, 122);
		case SI_WIN_YOYO_BACKSTEP:					return TabPosition(LegendHero, 84, 148);

		// Crackshooter
		case SI_CRA_SUP_POWERINCREASE:			return TabPosition(LegendHero,  34, 122);
		case SI_CRA_CRBOW_RANGESTRIKE:			return TabPosition(LegendHero,  34, 148);
		case SI_CRA_SUP_CONTROLINCREASE:		return TabPosition(LegendHero,  84, 122);
		case SI_CRA_SUP_HAWKEYE:						return TabPosition(LegendHero, 134, 122);

		// Florist
		case SI_FLO_HEAL_DOT:						return TabPosition(LegendHero,  34    , 122 + 13);
		case SI_FLO_SQU_BLESSSTEP:			return TabPosition(LegendHero,  84 - 4,  96 + 13);
		case SI_FLO_SQU_BLESSBODY:			return TabPosition(LegendHero,  84 - 4, 122 + 13);
		case SI_FLO_SQU_BLESSARMOR:			return TabPosition(LegendHero,  84 - 4, 148 + 13);
		case SI_FLO_CHEER_PATTERS:			return TabPosition(LegendHero,  84 - 4, 174 + 13);
		case SI_FLO_SUP_ABSOLUTE:				return TabPosition(LegendHero, 134 - 8, 174 + 13);

		// Forcemaster
		case SI_FOR_SQU_FORCETENACITY:	return TabPosition(LegendHero, 34, 122);
		case SI_FOR_SQU_FORCEANGER:			return TabPosition(LegendHero, 84, 122);
		case SI_FOR_SQU_FORCESPEED:			return TabPosition(LegendHero, 34, 148);
		case SI_FOR_SQU_FORCEMAD:				return TabPosition(LegendHero, 84, 148);

		// Mentalist
		case SI_MEN_WAN_ATKDECREASE:		return TabPosition(LegendHero, 84 - 4,  96 + 13);
		case SI_MEN_WAN_FEARSCREAM:			return TabPosition(LegendHero, 34 - 0, 122 + 13);
		case SI_MEN_WAN_DEFDECREASE:		return TabPosition(LegendHero, 84 - 4, 122 + 13);
		case SI_MEN_WAN_SPEDECREASE:		return TabPosition(LegendHero, 84 - 4, 148 + 13);
		case SI_MEN_WAN_DARKNESSLAKE:		return TabPosition(LegendHero, 84 - 4, 174 + 13);

		// Elementor but better
		case SI_ELE_STF_FINALSPEAR:			return TabPosition(LegendHero, 34, 122);
		case SI_ELE_STF_COSMICELEMENT:	return TabPosition(LegendHero, 34, 148);
		case SI_ELE_STF_THUNDERBOLT:		return TabPosition(LegendHero, 84, 122 + 13);
		case SI_ELE_STF_SLIPPING:				return TabPosition(LegendHero, 84, 148 + 26);

		// Bad skill
		default:												return std::nullopt;
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
	if (m_bLegend && m_strHeroSkilBg) {
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

			int nAddNum = 0;
			if (pSkillUser && pSkill.dwLevel != pSkillUser->dwLevel)
				nAddNum = 20;

			if (pSkill.dwLevel != 0) {
				if (pSkill.dwLevel < pSkillProp->dwExpertMax)
					m_kTexLevel.Render(p2DRender, skillAnchor - CPoint(2, 2), pSkill.dwLevel - 1 + nAddNum);
				else
					m_kTexLevel.Render(p2DRender, skillAnchor - CPoint(2, 2), 19 + nAddNum);
			}
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

		if (pSkillProp && 0 < m_pFocusItem->dwLevel) {
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill(m_pFocusItem->dwSkill);
			if (pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel)
				nAddNum = 20;

			//스킬당 레벨 표시
			if (m_pFocusItem->dwLevel < pSkillProp->dwExpertMax)
				m_kTexLevel.Render(p2DRender, lpWndCtrl->rect.TopLeft(), m_pFocusItem->dwLevel - 1 + nAddNum);
			else
				m_kTexLevel.Render(p2DRender, lpWndCtrl->rect.TopLeft(), 19 + nAddNum);
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

void CWndSkill_16::SubSkillPointDown(SKILL * lpSkill) {
	// TODO: inherit from a common class with CWndSkillTreeEx
	for (SKILL & skill : m_apSkills) {
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

void CWndSkill_16::AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat) {
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC_PANNEL);

	CPoint pt = lpWndCtrl->rect.TopLeft();
	pt.y += 26;

	if (m_pTexJobPannel) PaintTexture(pDest, m_pTexJobPannel.get(), pt, size);
}

int CWndSkill_16::GetJobByJobLevel(const SelectedTab tab, const int realJob) {
	if (tab == SelectedTab::Vagrant) {
		return JOB_VAGRANT;
	}

	const auto line = prj.jobs.GetAllJobsOfLine(realJob);
	// 0 = Vagrant, 1 = Pro, 2 = Expert, 3 = Master, 4 = Hero, 5 = LegendHero

	if (tab == SelectedTab::Expert) {
		if (line.size() > 1) return line[1];
		else return -1;
	} else if (tab == SelectedTab::Pro) {
		if (line.size() > 2) return line[2];
		return -1;
	} else if (tab == SelectedTab::LegendHero) {
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
		case SelectedTab::Vagrant:		buttons[0]->SetCheck(2); break;
		case SelectedTab::Expert:			buttons[1]->SetCheck(2); break;
		case SelectedTab::Pro:				buttons[2]->SetCheck(2); break;
		case SelectedTab::LegendHero: buttons[3]->SetCheck(2); break;
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

const char * CWndSkill_16::GetBackgroundFilename(const int nJob) {
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

const char * CWndSkill_16::GetHeroBackground(const int nJob) {
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

std::optional<CWndSkill_16::SelectedTab> CWndSkill_16::GetMaxTabFromJob(const int nJob) {
	if (nJob == JOB_VAGRANT)		return SelectedTab::Vagrant;
	if (nJob < MAX_EXPERT)			return SelectedTab::Expert;
	if (nJob < MAX_HERO)				return SelectedTab::Pro;
	if (nJob < MAX_LEGEND_HERO) return SelectedTab::LegendHero;

	Error(__FUNCTION__"(): Invalid job %d", nJob);
	return std::nullopt;
}

HRESULT CWndSkill_16::DeleteDeviceObjects() {
	CWndBase::DeleteDeviceObjects();

	m_pTexJobPannel.reset();
	m_apSkills.clear();

	return S_OK;
}
