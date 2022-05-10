#include "StdAfx.h"
#include "User.h"
#include "defineText.h"
#include "DPSrvr.h"
#include "FunnyCoin.h"
#include "ScriptHelper.h"
#include "DPDatabaseClient.h"
#include "InstanceDungeonBase.h"
#include "FuncTextCmd.h"

static ItemProp * GetDevirtualizedItemPropOf(const CItemElem & pItemBase) {
	ItemProp * pItemProp = pItemBase.GetProp();
	if (!pItemProp) return nullptr;

	if (pItemProp->dwItemKind3 != IK3_LINK) return pItemProp;
	return prj.GetItemProp(pItemProp->dwActiveSkill);
}

bool CUser::DoUseItem(DWORD dwData, DWORD dwFocusId, int nPart) {
	if (IsDie()) return false;

#ifdef __S_SERVER_UNIFY
	if (m_bAllAction == FALSE) return false;
#endif // __S_SERVER_UNIFY

	const WORD dwId = HIWORD(dwData);

	CItemElem * pItemBase = GetItemId(dwId);
	if (!IsUsableItem(pItemBase)) return false;

	ItemProp * const pItemProp = GetDevirtualizedItemPropOf(*pItemBase);
	if (!pItemProp) return false;

	const DWORD dwItemKind2 = pItemProp->dwItemKind2;
	const DWORD dwItemKind3 = pItemProp->dwItemKind3;

	// 길드대전맵에서 선수만 사용
	CWorld * pWorld = GetWorld();
	if (pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR) {
		if (m_nGuildCombatState == 0) {
			AddText(prj.GetText(TID_GAME_GUILDCOMBAT_STANDS_NOTUSEITEM));	//"수정해야함 : 관중석에서는 아이템을 사용할수 없습니다" );
			return false;
		}
	}

	if (GetAdjParam(DST_CHRSTATE) & CHS_SETSTONE) return false;
	
	if (pItemProp->dwParts != NULL_ID) {
		// armor, weapon
		if (m_pActMover->IsActAttack() == FALSE)	// 공격중엔 장비 못바꿈.
		{
			DoUseEquipmentItem(pItemBase, dwId, nPart);
		}
		// 장착 아이템은 탈착 가능해야하므로 기간 만료에 의한 사용 제한 없음.
	
		return true;
	}
	
	
	// 일반적인 아이템 사용 
	if (!IsItemRedyTime(pItemProp, pItemBase->m_dwObjId, TRUE)) return false;

	const auto cooldownType = m_cooltimeMgr.CanUse(*pItemProp);
	if (cooldownType == CCooltimeMgr::CooldownType::OnCooldown) return false;

	// 비 상용화 아이템
	switch (dwItemKind2) {
		case IK2_GUILDHOUSE_FURNITURE:
		case IK2_GUILDHOUSE_NPC:
		case IK2_GUILDHOUSE_PAPERING:
		{
			if (GuildHouseMng->SendWorldToDatabase(this, GUILDHOUSE_PCKTTYPE_LISTUP, GH_Fntr_Info(pItemProp->dwID)))
				g_DPSrvr.PutItemLog(this, "f", "GUILDHOUSE_LISTUP", pItemBase, 1);
			else
				return false;
			break;
		}
		case IK2_GUILDHOUES_COMEBACK:
		{
			if (!GuildHouseMng->EnteranceGuildHouse(this, pItemProp->dwID))
				return false;
			break;
		}
		case IK2_FURNITURE:
		case IK2_PAPERING:
		{
			if (CHousingMng::GetInstance()->ReqSetFurnitureList(this, pItemBase->m_dwItemId)) {
				// 하우징 리스트 추가 로그
				LogItemInfo aLogItem;
				aLogItem.SendName = GetName();
				aLogItem.RecvName = "HOUSING_USE";
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = GetGold();
				aLogItem.Gold2 = GetGold();
				aLogItem.Action = "f";
				g_DPSrvr.OnLogItem(aLogItem, pItemBase);
			} else
				return false;
			break;
		}
#ifdef __FUNNY_COIN
		case IK2_TOCASH:
		{
			if (!CFunnyCoin::GetInstance()->DoUseFunnyCoin(this, pItemBase))
				return false;
		}
		break;
#endif // __FUNNY_COIN
		case IK2_WARP:
			return DoUseItemWarp(pItemProp, pItemBase);
		case IK2_BUFF2:
		{
			if (IsDoUseBuff(pItemProp) != 0)
				return FALSE;
			CTime tm = CTime::GetCurrentTime() + CTimeSpan(0, 0, pItemProp->dwAbilityMin, 0);
			time_t t = (time_t)(tm.GetTime());
			// wID: dwItemId
			// dwLevel
			AddBuff(BUFF_ITEM2, (WORD)(pItemBase->m_dwItemId), t, 0);
			break;
		}
#ifdef __BUFF_TOGIFT
		case IK2_BUFF_TOGIFT:
#endif // __BUFF_TOGIFT
		case IK2_BUFF:
		{
			if (IsDoUseBuff(pItemProp) != 0)
				return FALSE;
			DoApplySkill(this, pItemProp, NULL);		// 대상에게 아이템효과를 적용함. 
			if (pItemProp->dwItemKind3 == IK3_ANGEL_BUFF) {
#ifdef __EXP_ANGELEXP_LOG
				m_nAngelExpLog = 0;
#endif // __EXP_ANGELEXP_LOG
#ifdef __ANGEL_LOG
				LogItemInfo aLogItem;
				aLogItem.Action = "&";
				aLogItem.SendName = GetName();
				aLogItem.RecvName = "ANGEL_SUMMON";
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				//aLogItem.ItemName = pItemProp->szName;
				_stprintf(aLogItem.szItemName, "%d", pItemProp->dwID);
				g_DPSrvr.OnLogItem(aLogItem);
#endif // __ANGEL_LOG
				m_nAngelExp = 0;
				m_nAngelLevel = GetLevel() + 1;
				AddAngelInfo();
			}
		}
		break;
		case IK2_TEXT: //텍스트 문서 처리 
			// 퀘스트가 있으면 퀘스트 시작 
			if (pItemProp->dwQuestId && pItemBase->m_bItemResist == FALSE) {
				::__SetQuest(GetId(), pItemProp->dwQuestId);
				pItemBase->m_bItemResist = TRUE;
				UpdateItem((BYTE)(pItemBase->m_dwObjId), UI_IR, pItemBase->m_bItemResist);
			}
			break;
		case IK2_SYSTEM:
		{
#ifdef __JEFF_9_20
#ifdef __WORLDSERVER
			if (pItemProp->dwLimitLevel1 != 0xFFFFFFFF) {
				if (GetLevel() < (int)(pItemProp->dwLimitLevel1)) {
					AddDefinedText(TID_GAME_ITEM_LEVEL, "\"%d\"", pItemProp->dwLimitLevel1);
					return FALSE;
				}
			}
#endif	// __WORLDSERVER
#endif	// __JEFF_9_20
			int nResult = DoUseItemSystem(pItemProp, pItemBase, nPart);
			{
				if (0 < nResult) {
					if (nResult == 2) {
						AddDefinedText(TID_GAME_LIMITED_USE, ""); // 사용중이어서 사용할수 없음
					} else if (nResult == 3) {
						AddDefinedText(TID_GAME_NOTOVERLAP_USE, ""); // 이 아이템과는 중복하여 사용할 수 없습니다
					}
					return FALSE;
				} else {
					if (pItemBase->m_dwItemId == II_SYS_SYS_SCR_RECCURENCE_LINK) {
						g_dpDBClient.SendLogSMItemUse("1", this, pItemBase, pItemProp);
					}

					//////////////////////////////////////////////////////////////////////////
					//	mulcom	BEGIN100125	이벤트용 리스킬 및 이벤트용 리스테트 사용 내역에 대한 로그 추가
					//						( e-mail : [유럽] 아이템 로그 추가 ( 2010-01-25 17:33 ) 참고 )
					else if (pItemBase->m_dwItemId == II_SYS_SYS_SCR_RECCURENCE && pItemBase->m_bCharged != TRUE) {
						g_DPSrvr.PutItemLog(this, "w", "USE_RECCURENCE_ITEM", pItemBase, 1);
					} else if (pItemBase->m_dwItemId == II_CHR_SYS_SCR_RESTATE && pItemBase->m_bCharged != TRUE) {
						g_DPSrvr.PutItemLog(this, "w", "USE_RESTATE_ITEM", pItemBase, 1);
					}
					//	mulcom	END100125	이벤트용 리스킬 및 이벤트용 리스테트 사용 내역에 대한 로그 추가
					//////////////////////////////////////////////////////////////////////////
				}
			}
		}
		break;

		case IK2_BLINKWING:
			return DoUseItemBlinkWing(pItemProp, pItemBase);

		case IK2_REFRESHER:
		{
			if (pItemProp->dwCircleTime != (DWORD)-1) {
				if (IsSMMode(SM_MAINTAIN_MP) == FALSE && SetSMMode(SM_MAINTAIN_MP, pItemProp->dwCircleTime)) {
					SetPointParam(DST_MP, GetMaxManaPoint());
				} else {
					AddDefinedText(TID_GAME_LIMITED_USE, ""); //   
					return false;
				}
				g_dpDBClient.SendLogSMItemUse("1", this, pItemBase, pItemProp);
			} else {
				if (DoUseItemFood(pItemProp, pItemBase) == FALSE)
					return false;
			}
			break;
		}
		case IK2_POTION:
			if (pItemProp->dwCircleTime != (DWORD)-1)	// 상용아이템은 dwCircleTime을 썼더라. 나중에 dwSkillTime으로 통합예정.
			{
				if (IsSMMode(SM_MAINTAIN_FP) == FALSE && SetSMMode(SM_MAINTAIN_FP, pItemProp->dwCircleTime)) {
					SetPointParam(DST_FP, GetMaxFatiguePoint());
				} else {
					AddDefinedText(TID_GAME_LIMITED_USE, ""); //   
					return FALSE;
				}
				g_dpDBClient.SendLogSMItemUse("1", this, pItemBase, pItemProp);
			} else {
				if (DoUseItemFood(pItemProp, pItemBase) == FALSE)
					return FALSE;
			}
			break;
		case IK2_FOOD:
			if (DoUseItemFood(pItemProp, pItemBase) == FALSE)
				return FALSE;
			break;
		case IK2_AIRFUEL:	// 비행연료류
		{
			ItemProp * pRideProp = prj.GetItemProp(m_dwRideItemIdx);	// 빗자루 프로퍼티
			if (pRideProp && m_pActMover->IsFly()) {
				if (pItemProp->dwItemKind3 == IK3_ACCEL)	// 가속 연료
				{
					m_tmAccFuel = pItemProp->dwAFuelReMax * 1000;;	// 몇초간 가속할수 있냐.
				} else {	// 비행연료
					if (pRideProp->dwItemKind3 == IK3_BOARD && pItemProp->dwItemKind3 == IK3_CFLIGHT)	// 타고있는건 보드 / 사용하려는건 빗자루용
					{
						AddDefinedText(TID_GAME_COMBFUEL, "");	// 이건 빗자루용이여~
						return FALSE;
					} else
						if (pRideProp->dwItemKind3 == IK3_STICK && pItemProp->dwItemKind3 == IK3_BFLIGHT)	// 타고있는건 빗자루 / 사용하려는건 보드용
						{
							AddDefinedText(TID_GAME_BOARDFUEL, "");	// 이건 보드용이여~
							return FALSE;
						}

					m_nFuel += pItemProp->dwFuelRe;		// 충전량.
					if ((DWORD)(m_nFuel) >= pRideProp->dwFFuelReMax)	// 빗자루 연료통보다 크면 끝.
						m_nFuel = pRideProp->dwFFuelReMax;
				}
				AddSetFuel(m_nFuel, m_tmAccFuel);		// 변화된 량을 전송. - SetPointParam에다 합쳐도 괜찮을듯?
			} else
				return FALSE;
		}
		break;

		case IK2_GMTEXT:
		{
			BOOL bGMTime = TRUE;
			if (pItemProp->dwSkillTime != NULL_ID)		// 지속시간이 있는 아이템
			{
				if (pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE)	// 변신 아이템은 비행중에는 사용할수 없음.
				{
					if (IsFly()) {
						AddDefinedText(TID_PK_FLIGHT_NOUSE, ""); // 비행 중에는 사용할 수 없습니다.
						bGMTime = FALSE;
					} else if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(GetWorld()->GetID())) {
						AddDefinedText(TID_GAME_INSTANCE_DISGUISE01);
						bGMTime = FALSE;
					}
				}
				if (bGMTime) {
					if (!HasBuffByIk3(pItemProp->dwItemKind3)) {
						DoApplySkill(this, pItemProp, NULL);		// 대상에게 아이템효과를 적용함. 
					} else {
						AddDefinedText(TID_GAME_LIMITED_USE, ""); // 사용중이어서 사용할수 없음
						bGMTime = FALSE;
					}
				}
			}

			if (pItemProp->dwID == II_SYS_SYS_GM_NODISGUISE) {
				if (HasBuffByIk3(IK3_TEXT_DISGUISE))
					RemoveIk3Buffs(IK3_TEXT_DISGUISE);
				else
					bGMTime = FALSE;
			}

			if (bGMTime) {
				if (pItemProp->dwID != II_SYS_SYS_GM_NODISGUISE) {
					char szGMCommand[64] = { 0, };
					CString szGMText = pItemProp->szTextFileName;
					szGMText.Replace('(', '\"');
					szGMText.Replace(')', '\"');
					int nGMCount = szGMText.Find("/", 1);
					if (nGMCount != -1)
						strncpy(szGMCommand, szGMText, nGMCount);
					else
						strcpy(szGMCommand, szGMText);

					g_textCmdFuncs.ParseCommand(szGMCommand, this, TRUE);
				}
			} else
				return FALSE;
		}
		break;
		case IK2_SKILL:
		{
			BOOL bUseItem = FALSE;
			DWORD dwActiveSkill = pItemProp->dwActiveSkill;
			if (dwActiveSkill != NULL_ID)		// 추가 발동 스킬이 있다.
			{
				if (pItemProp->dwActiveSkillRate == NULL_ID ||
					xRandom(100) < pItemProp->dwActiveSkillRate)	// 발동확률에 걸렸는가.
				{
					ItemProp * pSkillProp;
					pSkillProp = (ItemProp *)prj.GetSkillProp(dwActiveSkill);
					if (pSkillProp) {
						if (pSkillProp->dwUseChance == WUI_TARGETINGOBJ)	// 타겟에게 사용하는 방식.
						{
							if (IsPlayer()) {
								DWORD idTarget = m_idSetTarget;
								CMover * pTarget = prj.GetMover(idTarget);
								if (IsValidObj(pTarget)) {
									DoActiveSkill(dwActiveSkill, pItemProp->dwActiveSkillLv, pTarget);	// dwActiveSkill 발동.
								}
							}
						} else
							if (pSkillProp->dwUseChance == WUI_NOW)		// 자신에게만 사용하는 방식.
							{
								DoActiveSkill(dwActiveSkill, pItemProp->dwActiveSkillLv, this);	// 발동
							}
					} else
						Error("DoUseItem, IK2_SKILL, item %s -> %d skill not prop", pItemProp->szName, dwActiveSkill);
				}
			}
		}
		break;
		default:
		{
			if (pItemProp->dwSkillTime != NULL_ID)		// 지속시간이 있는 아이템
			{
				DoApplySkill(this, pItemProp, NULL);		// 대상에게 아이템효과를 적용함. 
			}
		}
	} // switch ik2

	switch (dwItemKind3) {
		case IK3_EGG:
			DoUseSystemPet(pItemBase);
			break;
		case IK3_PET:
			DoUseEatPet(pItemBase);
			break;
	}

	OnAfterUseItem(pItemProp);	// raiders 06.04.20
	pItemBase->UseItem();			// --m_nItemNum;

	if (pItemBase->m_bCharged)		// 상용화 아이템 로그
		g_dpDBClient.SendLogSMItemUse("1", this, pItemBase, pItemProp);

	CHAR cUIParam = UI_NUM;
	if (cooldownType == CCooltimeMgr::CooldownType::Available)	// 쿨타임 아이템이면 사용시각을 기록한다.
	{
		m_cooltimeMgr.StartCooldown(*pItemProp);
		cUIParam = UI_COOLTIME;
	}

	UpdateItem((BYTE)(dwId), cUIParam, pItemBase->m_nItemNum);	// 갯수가 0이면  아이템 삭제 , 전송 
	
	return true;
}

