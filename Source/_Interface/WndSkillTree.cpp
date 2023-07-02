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

std::unique_ptr<IMAGE> CWndSkillTreeCommon::GetBackgroundImage(const int nJob) {
	const char * background = GetBackgroundFilename(nJob);
	if (!background) return nullptr;

	std::unique_ptr<IMAGE> image = std::make_unique<IMAGE>();

	if (LoadImage(MakePath(DIR_THEME, background), image.get()) == FALSE) {
		Error("Could not load skill background %s (job %d)", background, nJob);
		return nullptr;
	}

	return image;
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

std::optional<CWndSkillTreeCommon::JobSkillPositionInfo> CWndSkillTreeCommon::GetSkillIconInfo(const DWORD dwSkillID) {
	using enum TabType;

	switch (dwSkillID) {
		// Vagrant
		case SI_VAG_ONE_CLEANHIT:   return TabPosition(Vagrant, 94, 149);
		case SI_VAG_ONE_BRANDISH:   return TabPosition(Vagrant, 144, 149);
		case SI_VAG_ONE_OVERCUTTER: return TabPosition(Vagrant, 194, 149);
		// Mercenary
		case SI_MER_ONE_SPLMASH:				return TabPosition(Expert, 34, 109);
		case SI_MER_SHIELD_PROTECTION:	return TabPosition(Expert, 34, 161);
		case SI_MER_ONE_KEENWHEEL:			return TabPosition(Expert, 84, 97);
		case SI_MER_ONE_BLOODYSTRIKE:		return TabPosition(Expert, 84, 123);
		case SI_MER_SUP_IMPOWERWEAPON:	return TabPosition(Expert, 84, 175);
		case SI_MER_SHIELD_PANBARRIER:	return TabPosition(Expert, 84, 149);
		case SI_MER_ONE_BLINDSIDE:			return TabPosition(Expert, 134, 97);
		case SI_MER_ONE_REFLEXHIT:			return TabPosition(Expert, 134, 123);
		case SI_MER_ONE_SNEAKER:				return TabPosition(Expert, 134, 149);
		case SI_MER_SUP_SMITEAXE:				return TabPosition(Expert, 134, 175);
		case SI_MER_SUP_BLAZINGSWORD:		return TabPosition(Expert, 134, 201);
		case SI_MER_ONE_SPECIALHIT:			return TabPosition(Expert, 184, 97);
		case SI_MER_ONE_GUILOTINE:			return TabPosition(Expert, 184, 123);
		case SI_MER_SUP_AXEMASTER:			return TabPosition(Expert, 184, 175);
		case SI_MER_SUP_SWORDMASTER:		return TabPosition(Expert, 184, 201);

		// Knight
		case SI_KNT_TWOSW_CHARGE:				return TabPosition(Pro, 34, 260 - 138);
		case SI_KNT_TWOAX_PAINDEALER:		return TabPosition(Pro, 34, 286 - 138);
		case SI_KNT_SUP_GUARD:					return TabPosition(Pro, 34, 312 - 138);
		case SI_KNT_TWOSW_EARTHDIVIDER:	return TabPosition(Pro, 84, 260 - 138);
		case SI_KNT_TWOAX_POWERSTUMP:		return TabPosition(Pro, 84, 286 - 138);
		case SI_KNT_SUP_RAGE:						return TabPosition(Pro, 84, 312 - 138);
		case SI_KNT_TWO_POWERSWING:			return TabPosition(Pro, 134, 272 - 138);
		case SI_KNT_SUP_PAINREFLECTION:	return TabPosition(Pro, 134, 312 - 138);

		// Blade
		case SI_BLD_DOUBLESW_SILENTSTRIKE:	return TabPosition(Pro, 34, 260 - 138);
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	return TabPosition(Pro, 34, 286 - 138);
		case SI_BLD_DOUBLE_ARMORPENETRATE:	return TabPosition(Pro, 34, 312 - 138);
		case SI_BLD_DOUBLESW_BLADEDANCE:		return TabPosition(Pro, 84, 260 - 138);
		case SI_BLD_DOUBLEAX_HAWKATTACK:		return TabPosition(Pro, 84, 286 - 138);
		case SI_BLD_SUP_BERSERK:						return TabPosition(Pro, 84, 312 - 138);
		case SI_BLD_DOUBLE_CROSSSTRIKE:			return TabPosition(Pro, 134, 272 - 138);
		case SI_BLD_DOUBLE_SONICBLADE:			return TabPosition(Pro, 134, 312 - 138);

		// Assist
		case SI_ASS_HEAL_HEALING:					return TabPosition(Expert, 18, 135);
		case SI_ASS_HEAL_PATIENCE:				return TabPosition(Expert, 64, 109);
		case SI_ASS_CHEER_QUICKSTEP:			return TabPosition(Expert, 64, 135);
		case SI_ASS_CHEER_MENTALSIGN:			return TabPosition(Expert, 64, 161);
		case SI_ASS_KNU_TAMPINGHOLE:			return TabPosition(Expert, 64, 187);
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
		case SI_RIN_SUP_PROTECT:						return TabPosition(Pro, 34, 260 - 138);
		case SI_RIN_SUP_HOLYCROSS:					return TabPosition(Pro, 34, 286 - 138);
		case SI_RIN_HEAL_GVURTIALLA:				return TabPosition(Pro, 34, 312 - 138);
		case SI_RIN_SUP_HOLYGUARD:					return TabPosition(Pro, 84, 260 - 138);
		case SI_RIN_SUP_SPIRITUREFORTUNE:		return TabPosition(Pro, 84, 286 - 138);
		case SI_RIN_HEAL_HEALRAIN:					return TabPosition(Pro, 84, 312 - 138);
		case SI_RIN_SQU_GEBURAHTIPHRETH:		return TabPosition(Pro, 134, 272 - 138);
		case SI_RIN_SUP_MERKABAHANZELRUSHA:	return TabPosition(Pro, 134, 312 - 138);

		// Billposter
		case SI_BIL_KNU_BELIALSMESHING:		return TabPosition(Pro, 34, 260 - 138);
		case SI_BIL_PST_ASMODEUS:					return TabPosition(Pro, 34, 286 - 138);
		case SI_BIL_KNU_BLOODFIST:				return TabPosition(Pro, 84, 260 - 138);
		case SI_BIL_PST_BARAQIJALESNA:		return TabPosition(Pro, 84, 286 - 138);
		case SI_BIL_KNU_PIERCINGSERPENT:	return TabPosition(Pro, 134, 260 - 138);
		case SI_BIL_PST_BGVURTIALBOLD:		return TabPosition(Pro, 134, 286 - 138);
		case SI_BIL_KNU_SONICHAND:				return TabPosition(Pro, 184, 272 - 138);
		case SI_BIL_PST_ASALRAALAIKUM:		return TabPosition(Pro, 184, 312 - 138);

		// Magician
		case SI_MAG_MAG_MENTALSTRIKE:						return TabPosition(Expert, 34, 96);
		case SI_MAG_MAG_BLINKPOOL: 							return TabPosition(Expert, 34, 148);
		case SI_MAG_FIRE_BOOMERANG:							return TabPosition(Expert, 84, 96);
		case SI_MAG_WIND_SWORDWIND:							return TabPosition(Expert, 84, 122);
		case SI_MAG_WATER_ICEMISSILE:						return TabPosition(Expert, 84, 148);
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:		return TabPosition(Expert, 84, 174);
		case SI_MAG_EARTH_SPIKESTONE:						return TabPosition(Expert, 84, 200);
		case SI_MAG_FIRE_HOTAIR:								return TabPosition(Expert, 134, 96);
		case SI_MAG_WIND_STRONGWIND:						return TabPosition(Expert, 134, 122);
		case SI_MAG_WATER_WATERBALL:						return TabPosition(Expert, 134, 148);
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:		return TabPosition(Expert, 134, 174);
		case SI_MAG_EARTH_ROCKCRASH:						return TabPosition(Expert, 134, 200);
		case SI_MAG_FIRE_FIRESTRIKE:						return TabPosition(Expert, 184, 96);
		case SI_MAG_WIND_WINDCUTTER:						return TabPosition(Expert, 184, 122);
		case SI_MAG_WATER_SPRINGWATER:					return TabPosition(Expert, 184, 148);
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	return TabPosition(Expert, 184, 174);
		case SI_MAG_EARTH_LOOTING:							return TabPosition(Expert, 184, 200);

		// Elementor
		case SI_ELE_FIRE_FIREBIRD:							return TabPosition(Pro, 34, 234 - 138);
		case SI_ELE_EARTH_STONESPEAR:						return TabPosition(Pro, 34, 260 - 138);
		case SI_ELE_WIND_VOID:									return TabPosition(Pro, 34, 286 - 138);
		case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	return TabPosition(Pro, 34, 312 - 138);
		case SI_ELE_WATER_ICESHARK:							return TabPosition(Pro, 34, 338 - 138);
		case SI_ELE_FIRE_BURINGFIELD:						return TabPosition(Pro, 84, 234 - 138);
		case SI_ELE_EARTH_EARTHQUAKE:						return TabPosition(Pro, 84, 260 - 138);
		case SI_ELE_WIND_WINDFIELD:							return TabPosition(Pro, 84, 286 - 138);
		case SI_ELE_ELECTRICITY_ELETRICSHOCK:		return TabPosition(Pro, 84, 312 - 138);
		case SI_ELE_WATER_POISONCLOUD:					return TabPosition(Pro, 84, 338 - 138);
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
		case SI_PSY_NLG_DEMONOLGY:			return TabPosition(Pro, 34, 260 - 138);
		case SI_PSY_PSY_PSYCHICBOMB:		return TabPosition(Pro, 34, 286 - 138);
		case SI_PSY_NLG_CRUCIOSPELL:		return TabPosition(Pro, 34, 312 - 138);
		case SI_PSY_NLG_SATANOLGY:			return TabPosition(Pro, 84, 260 - 138);
		case SI_PSY_PSY_SPRITBOMB:			return TabPosition(Pro, 84, 286 - 138);
		case SI_PSY_PSY_MAXIMUMCRISIS:	return TabPosition(Pro, 84, 312 - 138);
		case SI_PSY_PSY_PSYCHICWALL:		return TabPosition(Pro, 134, 272 - 138);
		case SI_PSY_PSY_PSYCHICSQUARE:	return TabPosition(Pro, 134, 312 - 138);

		// Acrobat
		case SI_ACR_YOYO_PULLING:				return TabPosition(Expert, 34, 97);
		case SI_ACR_SUP_SLOWSTEP:				return TabPosition(Expert, 34, 123);
		case SI_ACR_BOW_JUNKBOW:				return TabPosition(Expert, 34, 175);
		case SI_ACR_SUP_FASTWALKER:			return TabPosition(Expert, 84, 96);
		case SI_ACR_SUP_YOYOMASTER:			return TabPosition(Expert, 84, 122);
		case SI_ACR_SUP_BOWMASTER: 			return TabPosition(Expert, 84, 174);
		case SI_ACR_SUP_DARKILLUSION:		return TabPosition(Expert, 134, 96);
		case SI_ACR_YOYO_SNITCH:				return TabPosition(Expert, 134, 122);
		case SI_ACR_YOYO_CROSSLINE:			return TabPosition(Expert, 134, 149);
		case SI_ACR_BOW_SILENTSHOT:			return TabPosition(Expert, 134, 174);
		case SI_ACR_BOW_AIMEDSHOT:			return TabPosition(Expert, 134, 200);
		case SI_ACR_YOYO_ABSOLUTEBLOCK:	return TabPosition(Expert, 184, 96);
		case SI_ACR_YOYO_DEADLYSWING:		return TabPosition(Expert, 184, 122);
		case SI_ACR_YOYO_COUNTER:				return TabPosition(Expert, 184, 149);
		case SI_ACR_BOW_AUTOSHOT:				return TabPosition(Expert, 184, 174);
		case SI_ACR_BOW_ARROWRAIN:			return TabPosition(Expert, 184, 200);

		// Jester
		case SI_JST_SUP_POISON:					return TabPosition(Pro, 34, 260 - 138);
		case SI_JST_SUP_BLEEDING:				return TabPosition(Pro, 34, 286 - 138);
		case SI_JST_YOYO_ESCAPE:				return TabPosition(Pro, 34, 312 - 138);
		case SI_JST_YOYO_CRITICALSWING:	return TabPosition(Pro, 84, 260 - 138);
		case SI_JST_YOYO_BACKSTAB:			return TabPosition(Pro, 84, 286 - 138);
		case SI_JST_SUP_ABSORB:					return TabPosition(Pro, 84, 312 - 138);
		case SI_JST_YOYO_VATALSTAB:			return TabPosition(Pro, 134, 272 - 138);
		case SI_JST_YOYO_HITOFPENYA:		return TabPosition(Pro, 134, 312 - 138);

		// Ranger
		case SI_RAG_BOW_ICEARROW:				return TabPosition(Pro, 34, 260 - 138);
		case SI_RAG_BOW_FLAMEARROW:			return TabPosition(Pro, 34, 286 - 138);
		case SI_RAG_BOW_POISONARROW:		return TabPosition(Pro, 34, 312 - 138);
		case SI_RAG_SUP_FASTATTACK:			return TabPosition(Pro, 84, 260 - 138);
		case SI_RAG_BOW_PIRCINGARROW:		return TabPosition(Pro, 84, 286 - 138);
		case SI_RAG_SUP_NATURE:					return TabPosition(Pro, 84, 312 - 138);
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
		case SI_LOD_SUP_PULLING:			return TabPosition(LegendHero, 34, 122);
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
		case SI_CRA_SUP_POWERINCREASE:			return TabPosition(LegendHero, 34, 122);
		case SI_CRA_CRBOW_RANGESTRIKE:			return TabPosition(LegendHero, 34, 148);
		case SI_CRA_SUP_CONTROLINCREASE:		return TabPosition(LegendHero, 84, 122);
		case SI_CRA_SUP_HAWKEYE:						return TabPosition(LegendHero, 134, 122);

		// Florist
		case SI_FLO_HEAL_DOT:						return TabPosition(LegendHero, 34, 122 + 13);
		case SI_FLO_SQU_BLESSSTEP:			return TabPosition(LegendHero, 84 - 4, 96 + 13);
		case SI_FLO_SQU_BLESSBODY:			return TabPosition(LegendHero, 84 - 4, 122 + 13);
		case SI_FLO_SQU_BLESSARMOR:			return TabPosition(LegendHero, 84 - 4, 148 + 13);
		case SI_FLO_CHEER_PATTERS:			return TabPosition(LegendHero, 84 - 4, 174 + 13);
		case SI_FLO_SUP_ABSOLUTE:				return TabPosition(LegendHero, 134 - 8, 174 + 13);

		// Forcemaster
		case SI_FOR_SQU_FORCETENACITY:	return TabPosition(LegendHero, 34, 122);
		case SI_FOR_SQU_FORCEANGER:			return TabPosition(LegendHero, 84, 122);
		case SI_FOR_SQU_FORCESPEED:			return TabPosition(LegendHero, 34, 148);
		case SI_FOR_SQU_FORCEMAD:				return TabPosition(LegendHero, 84, 148);

		// Mentalist
		case SI_MEN_WAN_ATKDECREASE:		return TabPosition(LegendHero, 84 - 4, 96 + 13);
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

		CTexture * texture = m_textureMng.AddTexture(MakePath(DIR_ICON, pSkillProp->szIcon), 0xffff00ff);
		if (!texture) continue;

		m_pTexSkill.emplace(skill.dwSkill, texture);
	}

	m_kTexLevel.DeleteDeviceObjects();
	m_kTexLevel.LoadScript(MakePath(DIR_ICON, _T("icon_IconSkillLevel.inc")));
}

void CWndSkillTreeCommon::OnSkillPointUp() {
	if (!m_pFocusItem) return;

	const ItemProp * prop = m_pFocusItem->GetProp();
	if (!prop) return;

	const int nPoint = prj.GetSkillPoint(prop);
	if (nPoint == 0) return;

	const bool loop = g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->m_bShiftPushed;

	do {
		if (nPoint <= m_nCurrSkillPoint && m_pFocusItem->dwLevel < prop->dwExpertMax) {
			m_nCurrSkillPoint -= nPoint;
			++m_pFocusItem->dwLevel;
		} else {
			break;
		}
	} while (loop);
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


///////////////////////////////////////

CTexture * CWndSkillTreeCommon::GetTextureOf(const SKILL & skill) const {
	return sqktd::find_in_map(m_pTexSkill, skill.dwSkill, nullptr);
}

void CWndSkillTreeCommon::RenderLevel(
	C2DRender * p2DRender, CPoint point,
	DWORD curLevel, DWORD maxLevel, bool isLevelDiff
) {
	if (curLevel == 0) return;

	const int offset = isLevelDiff ? 20 : 0;
	if (curLevel < maxLevel)
		m_kTexLevel.Render(p2DRender, point, curLevel - 1 + offset);
	else
		m_kTexLevel.Render(p2DRender, point, 19 + offset);
}

///////////////////////////////////////////////////////////////////////////////

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
		return g_Neuz.m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL);
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

	m_aTexJobPannelExpert = nullptr;
	m_aTexJobPannelPro = nullptr;

	return S_OK;
}

void CWndSkillTreeEx::InitItem() {
	if (!g_pPlayer) return;

	ResetSkills();
	m_dwMouseSkill = NULL_ID;
	m_focusedSkill = nullptr;

	CWndStatic * lpWndStatic1 = GetDlgItem<CWndStatic>(WIDC_STATIC2);
	lpWndStatic1->SetTitle(prj.jobs.info[JOB_VAGRANT].szName);

	const auto jobTabs = JobToTabJobs(g_pPlayer->m_nJob);

	CWndStatic * lpWndStatic2 = GetDlgItem<CWndStatic>(WIDC_STATIC3);
	if (jobTabs.size() < 2) {
		m_aTexJobPannelExpert = nullptr;
		lpWndStatic2->SetTitle("");
	} else {
		m_aTexJobPannelExpert = GetBackgroundImage(jobTabs[1]);
		lpWndStatic2->SetTitle(prj.jobs.info[jobTabs[1]].szName);
	}

	CWndStatic * lpWndStatic3 = (CWndStatic *)GetDlgItem(WIDC_STATIC6);
	if (jobTabs.size() < 3) {
		m_aTexJobPannelPro = nullptr;
		lpWndStatic3->SetTitle("");
	} else {
		m_aTexJobPannelPro = GetBackgroundImage(jobTabs[2]);
		lpWndStatic3->SetTitle(prj.jobs.info[jobTabs[2]].szName);
	}

	m_strHeroSkilBg = GetHeroBackground(g_pPlayer->m_nJob);

	AdjustWndBase();
}

std::optional<CRect> CWndSkillTreeEx::GetSkillPoint(DWORD dwSkillID) {
	// Vagrant skills are positioned relatively to a custom

	if (dwSkillID == SI_VAG_ONE_CLEANHIT) {
		if (!m_bSlot[0]) return std::nullopt;

		CRect rect = GetWndCtrl(WIDC_CUSTOM2)->rect;
		rect.top += 1;
		return rect;
	} else if (dwSkillID == SI_VAG_ONE_BRANDISH) {
		if (!m_bSlot[0]) return std::nullopt;

		CRect rect = GetWndCtrl(WIDC_CUSTOM3)->rect;
		rect.top += 1;
		return rect;
	} else if (dwSkillID == SI_VAG_ONE_OVERCUTTER) {
		if (!m_bSlot[0]) return std::nullopt;

		CRect rect = GetWndCtrl(WIDC_CUSTOM4)->rect;
		rect.top += 1;
		return rect;
	}

	// It's not a vagrant skill. Let's discover where it is.

	const auto where = GetSkillIconInfo(dwSkillID);
	if (!where) return std::nullopt;

	if (std::holds_alternative<MasterPosition>(*where)) {
		if (!m_bSlot[3]) return std::nullopt;
		return GetWndCtrl(WIDC_CUSTOM5)->rect;
	} else if (std::holds_alternative<HeroPosition>(*where)) {
		if (!m_bSlot[3]) return std::nullopt;
		return GetWndCtrl(WIDC_CUSTOM6)->rect;
	} else {
		const TabPosition * tabPos = std::get_if<TabPosition>(&*where);
		if (!tabPos) return std::nullopt; // ??

		if (tabPos->tab == TabType::LegendHero) return std::nullopt; // Not supported by this window

		CPoint topLeftOfCtrl;
		if (tabPos->tab == TabType::Expert) {
			if (!m_bSlot[1]) return std::nullopt;
			topLeftOfCtrl = GetWndCtrl(WIDC_STATIC5)->rect.TopLeft();
		} else if (tabPos->tab == TabType::Pro) {
			if (!m_bSlot[2]) return std::nullopt;
			topLeftOfCtrl = GetWndCtrl(WIDC_STATIC7)->rect.TopLeft();
		} else {
			// Should not happen but ok
			return std::nullopt;
		}

		return CRect(
			CPoint(tabPos->point.x - 9, tabPos->point.y - 90),
			CSize(24, 24)
		) + topLeftOfCtrl;
	}
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

		if (!pSkillProp) continue;
		GetCalcImagePos(pSkillProp->dwItemKind1);

		if (std::optional<CRect> rect = GetSkillPoint(pSkillProp->dwID)) {
			rect->top -= 2;
			rect->right = rect->left + 27;
			rect->bottom = rect->top + 27;
			rect->OffsetRect(0, m_nTopDownGap);

			if (rect->PtInRect(point)) {
				dwMouseSkill = skill.dwSkill;

				ClientToScreen(&point);
				ClientToScreen(&*rect);
				g_WndMng.PutToolTip_Skill(skill.dwSkill, skill.dwLevel, point, &*rect, 
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
			CTexture * pTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_THEME, m_strHeroSkilBg), 0xffff00ff);
			if (pTexture)
				pTexture->Render(p2DRender, point, CPoint(27, 27));
		}
	}

	for (const SKILL & pSkill : m_apSkills) {
		const DWORD dwSkill = pSkill.dwSkill;

		const ItemProp * pSkillProp = pSkill.GetProp();
		if (!pSkillProp) continue;


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
			if (std::optional<CRect> rect = GetSkillPoint(pSkillProp->dwID)) {
				rect->top -= 2;
				rect->OffsetRect(0, m_nTopDownGap);
				skillTexture->Render(p2DRender, rect->TopLeft(), CPoint(27, 27));

				const SKILL * pSkillUser = g_pPlayer->GetSkill(pSkill.dwSkill);

				RenderLevel(
					p2DRender, rect->TopLeft() - CPoint(2, 2),
					pSkill.dwLevel, pSkillProp->dwExpertMax,
					pSkillUser && pSkill.dwLevel != pSkillUser->dwLevel
				);
			}
		} else if (m_dwMouseSkill == dwSkill) {
			if (std::optional<CRect> rect = GetSkillPoint(pSkillProp->dwID)) {
				rect->top -= 2;
				rect->OffsetRect(0, m_nTopDownGap);
				skillTexture->Render(p2DRender, rect->TopLeft(), CPoint(27, 27));
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
			const SKILL * pSkillUser = g_pPlayer->GetSkill(m_focusedSkill->dwSkill);

			RenderLevel(
				p2DRender, lpWndCtrl->rect.TopLeft(),
				m_focusedSkill->dwLevel, pSkillProp->dwExpertMax,
				pSkillUser && m_focusedSkill->dwLevel != pSkillUser->dwLevel
			);
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

	m_texGauEmptyNormal.LoadTexture(MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(MakePath(DIR_THEME, "GauFillNormal.bmp"), 0xffff00ff, TRUE);

	m_aSkillLevel[0] = m_textureMng.AddTexture(MakePath(DIR_THEME, "ButtSkillLevelHold01.tga"), 0xffff00ff);
	m_aSkillLevel[1] = m_textureMng.AddTexture(MakePath(DIR_THEME, "ButtSkillLevelup01.tga"), 0xffff00ff);
	m_aSkillLevel[2] = m_textureMng.AddTexture(MakePath(DIR_THEME, "ButtSkillLevelHold02.tga"), 0xffff00ff);

	InitItem();

	m_atexTopDown[0] = m_textureMng.AddTexture(MakePath(DIR_THEME, "LvUp.bmp"), 0xffff00ff);
	m_atexTopDown[1] = m_textureMng.AddTexture(MakePath(DIR_THEME, "LvDn.bmp"), 0xffff00ff);

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
					OnSkillPointUp();
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


		GetCalcImagePos(pSkillProp->dwItemKind1);


		if (pSkillProp->nLog != 1) {
			std::optional<CRect> rect = GetSkillPoint(pSkillProp->dwID);
			if (!rect) continue;

			if (GetSkillStatus(pSkill) != SkillStatus::No) {
				rect->top -= 2;

				rect->right = rect->left + 27;
				rect->bottom = rect->top + 27;

				rect->OffsetRect(0, m_nTopDownGap);

				if (rect->PtInRect(ptMouse)) {
					m_focusedSkill = &pSkill;
					m_bDrag = TRUE;
					break;
				}
			}
		}
	}
}

void CWndSkillTreeEx::OnLButtonDblClk(UINT nFlags, CPoint point) {
	// ��ųâ���� ����Ŭ���ϸ� �ڵ����� ��ų�ٿ� ��ϵȴ�?.

	CPoint ptMouse = GetMousePoint();

	for (SKILL & pSkill : m_apSkills) {
		DWORD dwSkill = pSkill.dwSkill;

		const ItemProp * pSkillProp = pSkill.GetProp();
		if (!pSkillProp) continue;

		GetCalcImagePos(pSkillProp->dwItemKind1);
		

		if (pSkillProp->nLog != 1) {
			std::optional<CRect> rect = GetSkillPoint(pSkillProp->dwID);
			if (!rect) continue;

			if (GetSkillStatus(pSkill) != SkillStatus::Learnable) {
				rect->top -= 2;

				rect->right = rect->left + 27;
				rect->bottom = rect->top + 27;


				rect->OffsetRect(0, m_nTopDownGap);

				if (rect->PtInRect(ptMouse)) {
					m_focusedSkill = &pSkill;

					CWndTaskBar * pTaskBar = g_WndMng.m_pWndTaskBar;
					if (pTaskBar->m_nExecute == 0)
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

	if (m_aTexJobPannelExpert && m_bSlot[1]) {
		PaintTexture(pDest, m_aTexJobPannelExpert.get(), pt, size);
	}

	lpWndCtrl = GetWndCtrl(WIDC_STATIC7);
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	GetCalcImagePos(JTYPE_PRO);
	pt.y += 2;
	pt.y += m_nTopDownGap;

	if (m_aTexJobPannelPro && m_bSlot[2]) {
		PaintTexture(pDest, m_aTexJobPannelPro.get(), pt, size);
	}
}
