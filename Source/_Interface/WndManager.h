#pragma once

#include <boost/container/flat_map.hpp>

#include "rtmessenger.h"
#include "WndWorld.h"
#include "WndBank.h"
#include "WndRepairItem.h"	 // 수리창
#include "WndUpgradebase.h"

#include "WndPiercing.h"

#include "WndInvenRemoveItem.h"
#include "WndCollecting.h"

#include "WndAwakening.h"
#include "WndLvReqDown.h"
#include "WndBlessingCancel.h"

#include "WndLord.h"

#include "WndPetRes.h"

#include "WndSummonAngel.h"

#ifdef __EVE_MINIGAME
#include "WndMiniGame.h"
#endif //__EVE_MINIGAME

#include "WndPetSys.h"

#include "WndGuildCombat1to1.h"

#include "WndSecretRoom.h"
#ifdef __AZRIA_1023
#include "WndSelectCh.h"
#endif
#ifdef __HELP_BUG_FIX
#include "WndHelp.h"
#endif //__HELP_BUG_FIX

#include "WndRainbowRace.h"
#include "WndRoom.h"
#ifdef __QUIZ
#include "WndQuizEvent.h"
#endif // __QUIZ
#include "WndQuestQuickInfo.h"
#include "WndQuest.h"

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#include "WndChattingBlockingList.h"
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
#include "WndHousing.h"
#endif //__GUILD_HOUSE_MIDDLE
typedef CMap<DWORD, DWORD, void *, void *> CMapDWordToPtr;


class CWndMessage;
class CWndInstantMsg;
class CWndCommand;
class CWndMap;
class CWndPartyChangeName;
class CWndPartyConfirm;
class CWndPartyLeaveConfirm;
class CWndFriendConFirm;
class CWndDuelConfirm;
class CWndDuelResult;
class CWndAddFriend;
class CWndCloseExistingConnection;
class CWndMessageNote;
class CWndMessengerNote;
class CWndTextFromItem;
class CWndSelectVillage;
class CWndGHMainMenu;
class CWndGHUpkeep;
class CWndMap;
class CWndStateConfirm;

class CWndSealChar; class CWndSealCharSelect; class CWndSealCharSend; class CWndSealCharSet;

class CWndGuildConfirm; class CWndGuildWarPeaceConfirm; class CWndGuildWarRequest;
class CWndGuildBank;
class CWndGuildNickName;
class CWndCommercialElem; class CWndRemoveElem;
class CWndReSkillWarning;
class CWndPost;
class CWndTradeGoldwithFunction;

#define REG_VERSION 1

#include "WndRankCmn.h"
#include "WndGuildBank.h"

#include "WndChangeName.h"


#define MAX_SKILL		  16

// chat style
#define CHATSTY_GENERAL   0x0001
#define CHATSTY_WHISPER   0x0002
#define CHATSTY_SHOUT     0x0004
#define CHATSTY_PARTY     0x0008
#define CHATSTY_GUILD     0x0010

#define CHATSTY_HELP          0x0060
#define CHATSTY_HELP_CLIENT   0x0020
#define CHATSTY_HELP_CHAT     0x0040
#define CHATSTY_SYSTEM        0x0180
#define CHATSTY_SYSTEM_CLIENT 0x0080
#define CHATSTY_SYSTEM_CHAT   0x0100
#define CHATSTY_GAME          0x0600
#define CHATSTY_GAME_CLIENT   0x0200
#define CHATSTY_GAME_CHAT     0x0400

#define PS_USE_MACRO     0x00000001
#define PS_NOT_MACRO     0x00000002

struct AppletFunc
{
	CWndNeuz* (*m_pFunc)();
	DWORD m_dwIdApplet;
	LPCTSTR m_pAppletName;
	LPCTSTR m_pAppletDesc;
	LPCTSTR m_pszIconName;
	CHAR m_cHotkey;
};

struct ToolTipItemTextColor
{
	DWORD dwName0;				// 이름 0단계
	DWORD dwName1;				// 이름 1단계
	DWORD dwName2;				// 이름 2단계
	DWORD dwName3;				// 이름 3단계
	DWORD dwName4;				// 이름 4단계
	DWORD dwName5;				// 이름 5단계
	DWORD dwGeneral;			// 일반
	DWORD dwPiercing;			// 피어싱
	DWORD dwPlusOption;			// 플러스 옵션
	DWORD dwResist;				// 속성 일반
	DWORD dwResistFire;			// 속성 불
	DWORD dwResistWater;		// 속성 물
	DWORD dwResistEarth;		// 속성 땅
	DWORD dwResistElectricity;	// 속성 전기
	DWORD dwResistWind;			// 속성 바람
	DWORD dwResistSM;			// 속성 상용화 무기
	DWORD dwResistSM1;			// 속성 상용화 방어구
	DWORD dwTime;				// 시간 및 효과
	DWORD dwEffective0;			// 아이템 효능
	DWORD dwEffective1;			// 아이템 효능1
	DWORD dwEffective2;			// 아이템 효능2
	DWORD dwEffective3;			// 아이템 효능3
	DWORD dwRandomOption;		// 랜덤 옵션
	DWORD dwEnchantOption;		// 인첸트 옵션
	DWORD dwSetName;			// 세트 이름
	DWORD dwSetItem0;			// 세트 목록(비착용시)
	DWORD dwSetItem1;			// 세트 목록(착용시)
	DWORD dwSetEffect;			// 세트 효과
	DWORD dwGold;				// 가격
	DWORD dwCommand;			// 설명
	DWORD dwNotUse;				// 사용못함
	std::array<DWORD, 9> dwAddedOpt; // Jewel bonus color
	DWORD dwAwakening;	// 아이템 각성
	DWORD dwBlessing;	// 여신의 축복
	DWORD dwBlessingWarning;	// 여신의 축복 경고
};

enum 
{
	FIRST_TC,
	SECOND_TC,
	THIRD_TC,
	MAX_TC
};


/// Subclasses of CWndMgr
///
/// Because a Mixin is better than a single class with tons of methods
namespace WndMgr {
	/// List of all children CWndBase whose memory is managed by a CWndMgr class.
	///
	/// /!\ This class does not "behave like an int"
	/// 
	/// How to use:
	/// - Add the line `CWndMyWindow * m_myWindow = nullptr;`
	/// - That's it. The memory will be correctly managed thanks to the reflection
	/// used in SafeDeleteAll and DeleteChild implementations.
	struct COwnedChildren {
		void SafeDeleteAll();
		bool DeleteChild(CWndBase * window);

		CWndBeautyShop * m_pWndBeautyShop = nullptr;
		CWndFaceShop * m_pWndFaceShop = nullptr;

#ifdef __NEWYEARDAY_EVENT_COUPON
		CWndUseCouponConfirm * m_pWndUseCouponConfirm = nullptr;
#endif //__NEWYEARDAY_EVENT_COUPON

		CWndSummonAngel * m_pWndSummonAngel = nullptr;
		CWndDialog * m_pWndDialog = nullptr;
		CWndShop * m_pWndShop = nullptr;
		CWndTrade * m_pWndTrade = nullptr;
		CWndTradeGoldwithFunction * m_pWndTradeGoldFunc = nullptr;
		CWndDropItem * m_pWndDropItem = nullptr;
		CWndConfirmTrade * m_pWndConfirmTrade = nullptr;
		CWndCommand * m_pWndCommand = nullptr;
#ifdef __JEFF_11
		CWndPetRes * m_pPetRes = nullptr;
#endif

#ifdef __EVE_MINIGAME
		CWndKawiBawiBoGame * m_pWndKawiBawiBoGame = nullptr;
		CWndKawiBawiBoGameConfirm * m_pWndKawiBawiBoGameConfirm = nullptr;
		CWndFindWordGame * m_pWndFindWordGame = nullptr;
		CWndDiceGame * m_pWndDiceGame = nullptr;
		CWndPuzzleGame * m_pWndPuzzleGame = nullptr;
#endif //__EVE_MINIGAME


		CWndMixJewel * m_pWndSmeltMixJewel = nullptr;
		CWndSmeltJewel * m_pWndSmeltJewel = nullptr;
		CWndExtraction * m_pWndExtraction = nullptr;
		CWndChangeWeapon * m_pWndChangeWeapon = nullptr;
		CWndPetStatus * m_pWndPetStatus = nullptr;

		CWndFoodConfirm * m_pWndFoodConfirm = nullptr;
		CWndPetMiracle * m_pWndPetMiracle = nullptr;
		CWndPetFoodMill * m_pWndPetFoodMill = nullptr;
		CWndPetLifeConfirm * m_pWndPetLifeConfirm = nullptr;

		CWndHeroSkillUp * m_pWndHeroSkillUp = nullptr;
		CWndRemovePiercing * m_pWndRemovePiercing = nullptr;
		CWndRemoveJewel * m_pWndRemoveJewel = nullptr;
		CWndPetTransEggs * m_pWndPetTransEggs = nullptr;

#ifdef __TRADESYS
		CWndDialogEvent * m_pWndDialogEvent = nullptr;
#endif //__TRADESYS

		CWndHeavenTower * m_pWndHeavenTower = nullptr;
		CWndRemoveAttribute * m_pWndRemoveAttribute = nullptr;
		CWndPost * m_pWndPost = nullptr;
		CWndQueryEquip * m_pWndQueryEquip = nullptr;
		CWndChatLog * m_pWndChatLog = nullptr;
		CWndQuestItemWarning * m_pWndQuestItemWarning = nullptr;
		CWndChangeName * m_pWndChangeName = nullptr;
		CWndDropConfirm * m_pWndDropConfirm = nullptr;
		CWndUpgradeBase * m_pWndUpgradeBase = nullptr;
		CWndPiercing * m_pWndPiercing = nullptr;

		CWndSealChar * m_pWndSealChar = nullptr;
		CWndSealCharSelect * m_pWndSealCharSelect = nullptr;
		CWndSealCharSend * m_pWndSealCharSend = nullptr;
		CWndSealCharSet * m_pWndSealCharSet = nullptr;

		CWndInvenRemoveItem * m_pWndInvenRemoveItem = nullptr;
		CWndCommercialElem * m_pWndCommerialElem = nullptr;
		CWndRemoveElem * m_pRemoveElem = nullptr;
		CWndConfirmBank * m_pWndConfirmBank = nullptr;
		CWndBankPassword * m_pWndBankPassword = nullptr;

		CWndRankGuild * m_pWndRankGuild = nullptr;
		CWndRankInfo * m_pWndRankInfo = nullptr;
		CWndRankWar * m_pWndRankWar = nullptr;

#ifdef __FUNNY_COIN
		CWndFunnyCoinConfirm * m_pFunnyCoinConfirm = nullptr;
#endif //__FUNNY_COIN

		CWndGuildBankLog * m_pWndGuildBankLog = nullptr;
		CWndGuildConfirm * m_pWndGuildConfirm = nullptr;
		CWndGuildWarPeaceConfirm * m_pWndGuildWarPeaceConfirm = nullptr;
		CWndGuildWarRequest * m_pWndGuildWarRequest = nullptr;
		CWndGuildCombatRank_Person * m_pWndGuildCombatRanking = nullptr;
		CWndGuildCombatState * m_pWndGuildWarState = nullptr;

		CWndGuildCombat1to1Offer * m_pWndGuildCombatOffer = nullptr;
		CWndGuildCombatSelection * m_pWndGuildCombatSelection = nullptr;
		CWndGuildCombatJoinSelection * m_pWndGuildCombatJoinSelection = nullptr;
		CWndGuildCombatBoard * m_pWndGuildCombatBoard = nullptr;
		CGuildCombatInfoMessageBox2 * m_pWndGuildCombatInfoMessageBox2 = nullptr;
		CGuildCombatInfoMessageBox * m_pWndGuildCombatInfoMessageBox = nullptr;
		CWndGuildCombatResult * n_pWndGuildCombatResult = nullptr;
		CWndGuildCombat1to1Selection * m_pWndGuildCombat1to1Selection = nullptr;
		CWndGuildCombat1to1Offer * m_pWndGuildCombat1to1Offer = nullptr;

		CCollectingWnd * m_pWndCollecting = nullptr;
		CWndLvReqDown * m_pWndLvReqDown = nullptr;
		CWndBlessingCancel * m_pWndBlessingCancel = nullptr;
		CWndAwakening * m_pWndAwakening = nullptr;

		CWndLordTender * m_pWndLordTender = nullptr;
		CWndLordVote * m_pWndLordVote = nullptr;
		CWndLordState * m_pWndLordState = nullptr;
		CWndLordEvent * m_pWndLordEvent = nullptr;
		CWndLordSkillConfirm * m_pWndLordSkillConfirm = nullptr;
		CWndLordInfo * m_pWndLordInfo = nullptr;
		CWndLordRPInfo * m_pWndLordRPInfo = nullptr;

		CWndSecretRoomInfoMsgBox * m_pWndSecretRoomMsg = nullptr;
		CWndSecretRoomSelection * m_pWndSecretRoomSelection = nullptr;
		CWndSecretRoomOffer * m_pWndSecretRoomOffer = nullptr;
		CWndSecretRoomOfferState * m_pWndSecretRoomOfferState = nullptr;
		CWndSecretRoomChangeTaxRate * m_pWndSecretRoomChangeTaxRate = nullptr;
		CWndSecretRoomCheckTaxRate * m_pWndSecretRoomCheckTaxRate = nullptr;
		CWndSecretRoomBoard * m_pWndSecretRoomBoard = nullptr;
		CWndSecretRoomQuick * m_pWndSecretRoomQuick = nullptr;

#ifdef __AZRIA_1023
		CWndSelectCh * m_pWndSelectCh = nullptr;
#endif

		CWndPetAwakCancel * m_pWndPetAwakCancel = nullptr;

		CWndRainbowRaceOffer * m_pWndRainbowRaceOffer = nullptr;
		CWndRainbowRaceInfo * m_pWndRainbowRaceInfo = nullptr;
		CWndRainbowRaceRule * m_pWndRainbowRaceRule = nullptr;
		CWndRainbowRaceRanking * m_pWndRainbowRaceRanking = nullptr;
		CWndRainbowRacePrize * m_pWndRainbowRacePrize = nullptr;
		CWndRainbowRaceMiniGameButton * m_pWndRainbowRaceMiniGameButton = nullptr;
		CWndRainbowRaceMiniGame * m_pWndRainbowRaceMiniGame = nullptr;
		CWndRainbowRaceMiniGameEnd * m_pWndRainbowRaceMiniGameEnd = nullptr;
		CWndRRMiniGameKawiBawiBo * m_pWndRRMiniGameKawiBawiBo = nullptr;
		CWndRRMiniGameDice * m_pWndRRMiniGameDice = nullptr;
		CWndRRMiniGameArithmetic * m_pWndRRMiniGameArithmetic = nullptr;
		CWndRRMiniGameStopWatch * m_pWndRRMiniGameStopWatch = nullptr;
		CWndRRMiniGameTyping * m_pWndRRMiniGameTyping = nullptr;
		CWndRRMiniGameCard * m_pWndRRMiniGameCard = nullptr;
		CWndRRMiniGameLadder * m_pWndRRMiniGameLadder = nullptr;

		CWndCoupleMessage * m_pWndCoupleMessage = nullptr;
		CWndChangeAttribute * m_pWndChangeAttribute = nullptr;

		CWndReSkillWarning * m_pWndReSkillWarning = nullptr;

		CWndPartyChangeName * m_pWndPartyChangeName = nullptr;
		CWndPartyConfirm * m_pWndPartyConfirm = nullptr;
		CWndPartyLeaveConfirm * m_pWndPartyLeaveConfirm = nullptr;

		CWndMessageNote * m_pWndMessageNote = nullptr;
		CWndMessengerNote * m_pWndMessengerNote = nullptr;

		CWndFontEdit * m_pWndFontEdit = nullptr;
		CWndFriendConFirm * m_pWndFriendConfirm = nullptr;
		CWndDuelConfirm * m_pWndDuelConfirm = nullptr;
		CWndDuelResult * m_pWndDuelResult = nullptr;
		CWndAddFriend * m_pWndAddFriend = nullptr;

		CWndStateConfirm * m_pWndStateConfirm = nullptr;
		CWndRandomScrollConfirm * m_pWndRandomScrollConfirm = nullptr;

#ifdef __HELP_BUG_FIX
		CWndHelpInstant * m_pWndHelpInstant = nullptr;
#endif //__HELP_BUG_FIX

		CWndRoomList * m_pWndRoomList = nullptr;
		CWndQuitRoom * m_pWndQuitRoom = nullptr;

		CWndChangePetName * m_pWndChangePetName = nullptr;
		CWndSmeltSafety * m_pWndSmeltSafety = nullptr;
		CWndSmeltSafetyConfirm * m_pWndSmeltSafetyConfirm = nullptr;
		CWndEquipBindConfirm * m_pWndEquipBindConfirm = nullptr;
		CWndRestateConfirm * m_pWndRestateConfirm = nullptr;

#ifdef __QUIZ
		CWndQuizEventConfirm * m_pWndQuizEventConfirm = nullptr;
		CWndQuizEventQuestionOX * m_pWndQuizEventQuestionOX = nullptr;
		CWndQuizEventQuestion4C * m_pWndQuizEventQuestion4C = nullptr;
		CWndQuizEventButton * m_pWndQuizEventButton = nullptr;
#endif // __QUIZ

		CWndQuestQuickInfo * m_pWndQuestQuickInfo = nullptr;
		CWndQuestDetail * m_pWndQuestDetail = nullptr;

		CWndCampusInvitationConfirm * m_pWndCampusInvitationConfirm = nullptr;
		CWndCampusSeveranceConfirm * m_pWndCampusSeveranceConfirm = nullptr;

		CWndBuffPetStatus * m_pWndBuffPetStatus = nullptr;
		CWndConfirmVis * m_pWndConfirmVis = nullptr;

		CWndGHUpkeep * m_pWndUpkeep = nullptr;

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
		CWndChattingBlockingList * m_pWndChattingBlockingList = nullptr;
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
		CWndGuildHouseBid * m_pWndGHBid = nullptr;
#endif //__GUILD_HOUSE_MIDDLE

		CWndMessageBox * m_pWndMessageBox = nullptr;
		CWndMessageBoxUpper * m_pWndMessageBoxUpper = nullptr;

		CWndRevival * m_pWndRevival = nullptr;
		CWndResurrectionConfirm * m_pWndResurrectionConfirm = nullptr;

		CWndReWanted * m_pReWanted = nullptr;
		CWndWanted * m_pWanted = nullptr;

		CWndLogOutWaitting * m_pLogOutWaitting = nullptr;
		CWndCommItemDlg * m_pWndCommItemDlg = nullptr;
		CWndChangeClass1 * m_pWndChangeClass1 = nullptr;
	};

	/// List of all functions formely in CWndMgr that are used to update the
	/// current tooltip and decide the text to display in tooltips
	class CTooltipBuilder {
	public:
		std::array<ToolTipItemTextColor, MAX_TC> dwItemColor;

	public:
		void PutToolTip_Troupe(DWORD dwSkill, CPoint point, CRect pRect) const;
		void PutToolTip_Skill(DWORD dwSkill, DWORD dwLevel, CPoint point, CRect pRect, BOOL bActive = TRUE) const;
		void PutToolTip_Item(DWORD dwType, DWORD dwId, CPoint point, const CRect * pRect, int flag = 0) const;
		void PutToolTip_Item(CItemElem * pItemBase, CPoint point, const CRect * pRect, int flag = 0) const;
		void PutToolTipItemAndSubToolTip(CItemElem * pItemBase, CPoint point, const CRect * pRect, int nFlag = 0) const;
		void MakeToolTipText(CItemElem * pItemBase, CEditString & strEdit, int flag = 0) const;
		static void PutToolTip_Character(int SelectCharacter, CPoint point, CRect pRect);

	private:
		void PutToolTipParts(CItemElem * pPartsItemBase, CPoint point, CRect pRect, int nFlag, int nSubToolTipFlag) const;
		void MakeToolTipText_(CMover & pMover, CItemElem & pItemElem, const ItemProp & itemProp, CEditString & strEdit, int fromApp) const;
		
	public:
		static void PutDestParam(DWORD dwDst1, DWORD dwDst2, DWORD dwAdj1, DWORD dwAdj2, CEditString & str);
		static void PutDestParam(SINGLE_DST dst, CEditString & str);

		[[nodiscard]] DWORD PutItemName(const CItemElem & pItemElem, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutItemAbilityPiercing(const CItemElem & pItemElem, CEditString & pEdit, DWORD dwColorBuf) const;
		void PutItemResist(const CItemElem & pItemElem, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutItemSpeed(const ItemProp & itemProp, CEditString & pEdit) const;
		void PutItemMinMax(const CMover & pMover, const CItemElem & pItemElem, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutRandomOpt(const CItemElem & pItemElem, CEditString & pEdit) const;
		void PutAwakeningBlessing(const CItemElem & pItemElem, CEditString & pEdit) const;
		void PutPiercingOpt(const CItemElem & pItemElem, CEditString & pEdit) const;
		void PutEnchantOpt(const CMover & pMover, const CItemElem & pItemElem, CEditString & pEdit, int fromApp) const;
		void PutSetItemOpt(const CMover & pMover, DWORD itemId, CEditString & pEdit) const;
		void PutBaseItemOpt(const CItemElem & pItemElem, const ItemProp & pItemProp, CEditString & pEdit) const;
		void PutMedicine(const ItemProp & pItemProp, CEditString & pEdit) const;
		void PutBaseResist(const ItemProp & pItemProp, CEditString & pEdit) const;
		void PutItemGold(const CItemElem & pItemElem, CEditString & pEdit, int fromApp) const;
		void PutCoolTime(const CMover & pMover, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutKeepTime(CItemElem & pItemElem, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutCommand(const CItemElem & pItemElem, CEditString & pEdit) const;
		void PutSex(const CMover & pMover, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutJob(const CMover & pMover, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutLevel(const CMover & pMover, const CItemElem & pItemElem, CEditString & pEdit) const;
		void PutWeapon(const ItemProp & pItemProp, CEditString & pEdit) const;
		void PutAddedOpt(const CItemElem & pItemElem, CEditString & pEdit) const;

		void PutPetInfo(const CItemElem & pItemElem, CEditString & pEdit) const;
		void PutPetFeedPocket(const CItemElem & pItemElem, const ItemProp & itemProp, CEditString & pEdit) const;
		void PutNeededVis(const ItemProp & pItemPropVis, CEditString & pEdit) const;
		void PutVisPetInfo(const CItemElem & pItemElem, CEditString & pEdit) const;
		void PutPetKind(const CItemElem & pItemElem, CEditString & pEdit) const;

		void PutSealChar(const CItemElem & pItemElem, CEditString & pEdit) const;

		void PutEquipItemText(CEditString & pEdit) const;

	private:
		static CString TimeAsStrWithOneNumberPrecision(time_t t);

		[[nodiscard]] DWORD GetOkOrErrorColor(bool isOk) const;
	};


	class CBanningSystem {
	protected:
		int m_nWarningCounter = 0;
		int m_nWarning2Counter = 0;
		CTimer m_timerWarning;
		CTimer m_timerWarning2;
		CTimer m_timerShortcutWarning;
		CTimer m_timerBanning;
		BOOL m_bShortcutCommand = FALSE;

	public:
		enum { BANNING_POINT = 5, BANNING_2_POINT = 2 };
		enum { WARNING_MILLISECOND = 700, WARNING_2_MILLISECOND = 1000, SHORTCUT_WARNING_MILLISECOND = 3000, BANNING_MILLISECOND = 180000 };

	public:
		// Getters and setters
		void SetWarningCounter(int nWarningCounter) { m_nWarningCounter = nWarningCounter; }
		void SetWarning2Counter(int nWarning2Counter) { m_nWarning2Counter = nWarning2Counter; }
		[[nodiscard]] int GetWarningCounter() const { return m_nWarningCounter; }
		[[nodiscard]] int GetWarning2Counter() const { return m_nWarning2Counter; }
		CTimer & GetWarningTimer() { return m_timerWarning; }
		CTimer & GetWarning2Timer() { return m_timerWarning2; };
		CTimer & GetShortcutWarningTimer() { return m_timerShortcutWarning; }
		CTimer & GetBanningTimer() { return m_timerBanning; }
		[[nodiscard]] BOOL IsShortcutCommand() const { return m_bShortcutCommand; };

	public:
		// Actual OOP methods

		void InitializeTimers();

	};
}

/// Utility classes intended to be used by CWndMgr
namespace WndMgr {
	class RegInfo {
	private:
		DWORD dwWndId;
		CRect rect;
		BOOL  bOpen;
		DWORD dwVersion;
		DWORD dwWndSize;
		std::vector<BYTE> lpArchive;

	public:
		// CWndNeuz
		RegInfo(CWndNeuz & pWndNeuz, BOOL bOpen);
		void RestoreParameters(CWndNeuz & pWndBase) const;

		// CFileIO
		explicit RegInfo(CFileIO & file);
		void StoreIn(CFileIO & file) const;

		// Everybody likes getters
		[[nodiscard]] DWORD GetWndId() const noexcept { return dwWndId; }
	};

	struct StoredChatMessage {
		CString message;
		DWORD color;
		DWORD style;
	};
}

class CWndMgr :
	public CWndBase,
	public WndMgr::COwnedChildren,
	public WndMgr::CTooltipBuilder
#ifdef __BAN_CHATTING_SYSTEM
	, public WndMgr::CBanningSystem
#endif
{
private:

	CString m_strChatBackup;
	CTimer m_timerDobe;

public:
	std::vector<WndMgr::StoredChatMessage> m_aChat;

	CRTMessenger	m_RTMessenger;
	boost::container::flat_map<std::string, std::unique_ptr<CWndMap>>        m_mapMap;
	boost::container::flat_map<CString, std::unique_ptr<CWndInstantMsg>> m_mapInstantMsg;
	boost::container::flat_map<CString, std::unique_ptr<CWndMessage>>    m_mapMessage;

	BOOL m_bAutoRun;
	
	DWORD m_dwSavePlayerTime;
	
	TCHAR m_szTimerChat[ 128 ];
	CTimer m_timerChat;
	
	BOOL   m_bConnect;

	BOOL	m_bTitle        ;
	boost::container::flat_map<DWORD, AppletFunc *> m_mapAppletFunc;
	std::vector<int> m_tempWndId;
	BOOL m_clearFlag;

	AppletFunc*		GetAppletFunc( DWORD dwIdApplet );
	void	AddAllApplet();
	BOOL ScreenCapture();
	BOOL SaveBitmap( LPCTSTR lpszName );
	BOOL SaveJPG( LPCTSTR lpszName );
	void InitSetItemTextColor();
	void Free();

public:
	DWORD		   m_dwSkillTime[MAX_SKILL];
	
	CWndBase*      m_pWndActiveDesktop; // Dead field
	CWndTradeConfirm * m_pWndTradeConfirm; // Memory leak? (Original pos: just after CWndConfirmTrade)

	CWndBank*	   m_pWndBank; // also destroys tradegold
	CWndWorld*     m_pWndWorld;

	CWndSelectVillage*		m_pWndSelectVillage; // Native memleak
	CWndRepairItem* m_pWndRepairItem; // Native memleak

	CWndGuildBank*	m_pWndGuildBank;

	BOOL	m_bWaitRequestMail;

	CWndGuildNickName* m_pWndGuildNickName;



	bool						m_bIsOpenLordSkill;



	CWndCloseExistingConnection*	m_pWndCloseExistingConnection; // CWndBase def is not included by default
	

#ifdef __S_SERVER_UNIFY
	BOOL							m_bAllAction;
#endif // __S_SERVER_UNIFY

	CWndGHMainMenu* m_pWndGHMain;

#ifdef __PROTECT_AWAKE
	CWndSelectAwakeCase* m_pWndSelectAwakeCase;
#endif 
	CWndTaskBar * m_pWndTaskBar;
	CWndTaskMenu * m_pWndMenu;
	CWndQuestItemInfo * m_pQuestItemInfo;




	// 인터페이스 텍스춰
	CTexturePack  m_texture;
	CTexturePack  m_texCommand;
	CTexturePack  m_texIcon;
	CTexturePack  m_texWnd;

	// 메지시 윈도

	//	퀘스트 아이템 정보

	void OpenQuestItemInfo(CWndBase* pWndParent, CItemElem * pItemBase);
	void ChangeQuestItemInfo(CItemElem * pItemBase);

	CWndTextFromItem * m_pWndTextBook;
	CWndTextFromItem * m_pWndTextScroll;
	CWndTextFromItem * m_pWndTextLetter;
	void OpenTextBook    (CWndBase* pWndParent, CItemElem * pItemBase);
	void OpenTextScroll  (CWndBase* pWndParent, CItemElem * pItemBase);
	void OpenTextLetter  (CWndBase* pWndParent, CItemElem * pItemBase);
	void ChangeTextBook  (CItemElem * pItemBase);
	void ChangeTextScroll(CItemElem * pItemBase);
	void ChangeTextLetter(CItemElem * pItemBase);
	// Field

	std::map<DWORD, CWndNeuz *> m_mapWndApplet;
	std::map<DWORD, WndMgr::RegInfo>  m_mapWndRegInfo;

	void PutDefinedString( DWORD dwText, ... );
	void PutString( LPCTSTR lpszString, CObj* pObj = NULL, DWORD dwColor = 0xffffffff, DWORD dwChatStyle = CHATSTY_GAME, DWORD dwPStyle = 0x00000001 );
	void PutString(DWORD textId);
	template<typename ... Ts>
	void PutString(DWORD textId, const Ts & ... ts) requires (sizeof...(Ts) > 0) {
		const char * textBase = prj.GetText(textId);
		CString text;
		text.Format(textBase, ts...);
		PutString(text.GetString(), nullptr, prj.GetTextColor(textId));
	}
	void ParsingChat( CString string );
	void WordChange( CString& rString );

	BOOL	IsTitle()	{	return m_bTitle;	}

	void	OpenTitle( BOOL bFirstTime = FALSE );
	void	OpenField();

	BOOL	OpenTaskBar();
	BOOL	OpenMenu();

	void	ObjectExecutor(ShortcutType dwShortcut, DWORD dwId );
	void	ObjectExecutor( LPSHORTCUT lpShortcut );
	void OpenApplet(DWORD dwId) { return ObjectExecutor(ShortcutType::Applet, dwId); }
	void	UseSkillShortCut( DWORD dwSkillIdx );
		
	// map
	CWndMap* OpenMap( LPCTSTR lpszMapFileName );
	CCollectingWnd* OpenCollecting();
	BOOL CloseCollecting();

	// massenger
	CWndMessage* GetMessage( LPCTSTR lpszFrom );
	CWndMessage* OpenMessage( LPCTSTR lpszFrom );
	BOOL	UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew );
	CWndInstantMsg* GetInstantMsg( LPCTSTR lpszFrom );
	CWndInstantMsg* OpenInstantMsg( LPCTSTR lpszFrom );

	// MessageBox
	BOOL	OpenCustomBox( LPCTSTR strMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent = NULL );
	BOOL	OpenMessageBox( LPCTSTR strMessage, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
	BOOL	OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
	BOOL    OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType = MB_OK, BOOL bPostLogoutMsg = FALSE );
		
	void	CloseMessageBox();

	// tooltip
	enum { TOOL_TIP_SWITCH_MAIN = 0, TOOL_TIP_SWITCH_SUB1 = 1, TOOL_TIP_SWITCH_SUB2 = 2 };


	BOOL CheckConfirm(CItemElem * pItem );

	CWndMgr();
	virtual	~CWndMgr();

	void	DestroyApplet();
	void	AlighWindow( CRect rcOld, CRect rcNew );

	//virtual CItem* GetFocusItem();
	virtual	BOOL    Process();
	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL);
	virtual	void	OnInitialUpdate();
	virtual	void	OnDestroyChildWnd(CWndBase* pWndChild);
	virtual	void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void	OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL	OnCommand(UINT nID,DWORD dwMessage);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual BOOL    OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0) { return TRUE; }
	virtual	BOOL	OnEraseBkgnd(C2DRender* p2DRender);
	virtual	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) { return TRUE; }

	DWORD	GetAppletId( TCHAR* lpszAppletName );
	CWndBase* GetApplet( DWORD dwIdApplet );
	CWndBase* CreateApplet( DWORD dwIdApplet );
	void	AddAppletFunc(CWndNeuz * (*pAppletFunc)(), DWORD dwIdApplet, LPCTSTR lpszKeyword, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc = NULL, CHAR cHotkey = 0 );

	HRESULT	RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	
//	Initialize player
	void SetPlayer( CMover* pMover );

	BOOL LoadRegInfo( LPCTSTR lpszFileName );
	BOOL SaveRegInfo( LPCTSTR lpszFileName );
	void PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen );


	CWndBase* GetWndVendorBase( void );

	void ClearAllWnd();
	void RestoreWnd();

	[[nodiscard]] static DWORD GetDSTStringId(int nDstParam);
	BOOL IsDstRate(int nDstParam);

#ifdef __WINDOW_INTERFACE_BUG
	void CloseBoundWindow(void); // 아이템이 걸려 있거나, 아이템을 조작할 가능성이 있는 모든 창을 강제로 닫는다.
#endif // __WINDOW_INTERFACE_BUG

protected:
	void __HotKeyChange( DWORD dwId, char* pch );
	void	FormalizeChatString( CString & str );
};

extern CPtrArray      m_wndOrder;
extern CWndMgr          g_WndMng; // 윈도 매니저 클래스 

extern void RenderEnchant( C2DRender* p2DRender, CPoint pt );
extern void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor );
#define WID_MESSAGEBOX        10

