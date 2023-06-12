#include "stdafx.h"
#include "defineText.h"
#include "defineItem.h"
#include "defineSkill.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpsrvr.h"
#include "definesound.h"
#include "defineObj.h"
#include "user.h"
#include "worldmng.h"
#include "misc.h"
#include "lang.h"
#include "npchecker.h"
#include "guild.h"
#include "..\_Common\Ship.h"
#include "Party.h"
#include "Chatting.h"
#include "post.h"
#include "..\_Network\ErrorCode.h"
#include "WantedListSnapshot.h"
#include "definequest.h"

#include "pet.h"

#include "slord.h"
#include "lordskillexecutable.h"

#include "randomoption.h"

#include "playerdata.h"

	#include "CreateMonster.h"

	#include "honor.h"

#include "SecretRoom.h"

#include "Tax.h"

#include "ItemUpgrade.h"

#include "tools.h"

#include "couplehelper.h"

#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ

#include "GuildHouse.h"

#include "CampusHelper.h"
#include "FuncTextCmd.h"
#include "GroupUtils.h"

#define	MAX_RANGE_ASYNC			1024
#define	MAX_RANGE_NPC_MENU		1024

CCommonCtrl* CreateExpBox( CUser* pUser );
CDPSrvr		g_DPSrvr;

// dwGold과 nPlus을 더 할 수 있는가?
BOOL CanAdd( DWORD dwGold, int nPlus )
{
	if( nPlus <= 0 )		// 더하려는 값이 0이하이면 넌센스 
		return FALSE;

	int nGold = dwGold;
	ASSERT( nGold >= 0 );
	return ( (nGold + nPlus) > nGold );		// 더한 값이 overflow이면 막아야한다.
}

CDPSrvr::CDPSrvr()
{
	OnMsg( PACKETTYPE_JOIN, &CDPSrvr::OnAddUser );
	OnMsg( PACKETTYPE_LEAVE, &CDPSrvr::OnRemoveUser );
	OnMsg( PACKETTYPE_REPLACE, &CDPSrvr::OnReplace );
	OnMsg( PACKETTYPE_CORR_REQ, &CDPSrvr::OnCorrReq );
	OnMsg( PACKETTYPE_SCRIPTDLG, &CDPSrvr::OnScriptDialogReq );
	OnMsg( PACKETTYPE_TRADEPUT, &CDPSrvr::OnTradePut );
	OnMsg( PACKETTYPE_TRADEPULL, &CDPSrvr::OnTradePull );
	OnMsg( PACKETTYPE_TRADEPUTGOLD, &CDPSrvr::OnTradePutGold );

	OnMsg( PACKETTYPE_SFX_ID, &CDPSrvr::OnSfxID );
	OnMsg( PACKETTYPE_SFX_CLEAR, &CDPSrvr::OnSfxClear );
	OnMsg( PACKETTYPE_CREATE_GUILDCLOAK, &CDPSrvr::OnCreateGuildCloak );
	OnMsg( PACKETTYPE_CHAT, &CDPSrvr::OnChat );
	OnMsg( PACKETTYPE_CTRL_COOLTIME_CANCEL, &CDPSrvr::OnCtrlCoolTimeCancel );
	OnMsg( PACKETTYPE_DOEQUIP, &CDPSrvr::OnDoEquip );
	OnMsg( PACKETTYPE_MOVEITEM, &CDPSrvr::OnMoveItem );
	OnMsg( PACKETTYPE_SNAPSHOT, &CDPSrvr::OnSnapshot );
	OnMsg( PACKETTYPE_SEND_TO_SERVER_CHANGEJOB, &CDPSrvr::OnChangeJob );
	OnMsg( PACKETTYPE_SETLODELIGHT, &CDPSrvr::OnSetLodelight );
	OnMsg( PACKETTYPE_REVIVAL, &CDPSrvr::OnRevival );
	OnMsg( PACKETTYPE_REVIVAL_TO_LODESTAR, &CDPSrvr::OnRevivalLodestar );
	OnMsg( PACKETTYPE_REVIVAL_TO_LODELIGHT, &CDPSrvr::OnRevivalLodelight );
	OnMsg( PACKETTYPE_OPENSHOPWND, &CDPSrvr::OnOpenShopWnd );
	OnMsg( PACKETTYPE_CLOSESHOPWND, &CDPSrvr::OnCloseShopWnd );
	OnMsg( PACKETTYPE_SEND_TO_SERVER_EXP, &CDPSrvr::OnExpUp );
	OnMsg( PACKETTYPE_OPENBANKWND, &CDPSrvr::OnOpenBankWnd );
	OnMsg( PACKETTYPE_GUILD_BANK_WND, &CDPSrvr::OnOpenGuildBankWnd );
	OnMsg( PACKETTYPE_PUTITEMGUILDBANK, &CDPSrvr::OnPutItemGuildBank );
	OnMsg( PACKETTYPE_GETITEMGUILDBANK, &CDPSrvr::OnGetItemGuildBank );
	OnMsg( PACKETTYPE_GUILD_BANK_WND_CLOSE, &CDPSrvr::OnCloseGuildBankWnd );
	OnMsg( PACKETTYPE_GUILD_BANK_MOVEITEM, &CDPSrvr::OnGuildBankMoveItem );
	OnMsg( PACKETTYPE_CLOSEBANKWND, &CDPSrvr::OnCloseBankWnd );
	OnMsg( PACKETTYPE_PUTITEMBACK, &CDPSrvr::OnPutItemBank );
	OnMsg( PACKETTYPE_DOUSESKILLPOINT, &CDPSrvr::OnDoUseSkillPoint );
	OnMsg( PACKETTYPE_PUTBACKTOBANK, &CDPSrvr::OnBankToBank );
	OnMsg( PACKETTYPE_GETITEMBACK, &CDPSrvr::OnGetItemBank );
	OnMsg( PACKETTYPE_PUTGOLDBACK, &CDPSrvr::OnPutGoldBank );
	OnMsg( PACKETTYPE_GETGOLDBACK, &CDPSrvr::OnGetGoldBank );
	OnMsg( PACKETTYPE_MOVEBANKITEM, &CDPSrvr::OnMoveBankItem );
	OnMsg( PACKETTYPE_CHANGEBANKPASS, &CDPSrvr::OnChangeBankPass );
	OnMsg( PACKETTYPE_CONFIRMBANK, &CDPSrvr::OnConfirmBank );	
	OnMsg( PACKETTYPE_PLAYERMOVED, &CDPSrvr::OnPlayerMoved );
	OnMsg( PACKETTYPE_PLAYERBEHAVIOR, &CDPSrvr::OnPlayerBehavior );
	OnMsg( PACKETTYPE_PLAYERMOVED2, &CDPSrvr::OnPlayerMoved2 );
	OnMsg( PACKETTYPE_PLAYERBEHAVIOR2, &CDPSrvr::OnPlayerBehavior2 );
	OnMsg( PACKETTYPE_PLAYERANGLE, &CDPSrvr::OnPlayerAngle );
	OnMsg( PACKETTYPE_PLAYERCORR, &CDPSrvr::OnPlayerCorr );
	OnMsg( PACKETTYPE_PLAYERCORR2, &CDPSrvr::OnPlayerCorr2 );
	OnMsg( PACKETTYPE_PLAYERSETDESTOBJ, &CDPSrvr::OnPlayerSetDestObj );
	OnMsg( PACKETTYPE_TRADE, &CDPSrvr::OnTrade );
	OnMsg( PACKETTYPE_CONFIRMTRADE, &CDPSrvr::OnConfirmTrade );
	OnMsg(PACKETTYPE_CONFIRMTRADECANCEL, &CDPSrvr::OnConfirmTradeCancel);
	OnMsg( PACKETTYPE_TRADECANCEL, &CDPSrvr::OnTradeCancel );
	OnMsg( PACKETTYPE_DOUSEITEM, &CDPSrvr::OnDoUseItem );
	OnMsg( PACKETTYPE_DO_USE_ITEM_TARGET, &CDPSrvr::OnDoUseItemTarget );
	OnMsg( PACKETTYPE_REMOVE_ITEM_LEVEL_DOWN, &CDPSrvr::OnRemoveItemLevelDown );
	OnMsg( PACKETTYPE_AWAKENING, &CDPSrvr::OnAwakening );
	OnMsg( PACKETTYPE_BLESSEDNESS_CANCEL, &CDPSrvr::OnBlessednessCancel );
#ifdef __AZRIA_1023
	OnMsg( PACKETTYPE_DO_USE_ITEM_INPUT, &CDPSrvr::OnDoUseItemInput );
#endif	// __AZRIA_1023
	OnMsg( PACKETTYPE_CLEAR_PET_NAME, &CDPSrvr::OnClearPetName );
	OnMsg( PACKETTYPE_AVAIL_POCKET, &CDPSrvr::OnAvailPocket );
	OnMsg( PACKETTYPE_MOVE_ITEM_POCKET, &CDPSrvr::OnMoveItemOnPocket );

#ifdef __JEFF_11
	OnMsg( PACKETTYPE_QUE_PETRESURRECTION, &CDPSrvr::OnQuePetResurrection );
#endif	// __JEFF_11

#ifdef __JEFF_11_4
	OnMsg( PACKETTYPE_ARENA_ENTER, &CDPSrvr::OnArenaEnter );
	OnMsg( PACKETTYPE_ARENA_EXIT, &CDPSrvr::OnArenaExit );
#endif	// __JEFF_11_4

	OnMsg( PACKETTYPE_TRADECONFIRM, &CDPSrvr::OnTradelastConfrim );
	OnMsg( PACKETTYPE_MOVERFOCOUS, &CDPSrvr::OnMoverFocus );
	OnMsg( PACKETTYPE_DROPITEM, &CDPSrvr::OnDropItem );
	OnMsg( PACKETTYPE_BUYITEM, &CDPSrvr::OnBuyItem );
	OnMsg( PACKETTYPE_BUYCHIPITEM, &CDPSrvr::OnBuyChipItem );
	OnMsg( PACKETTYPE_SELLITEM, &CDPSrvr::OnSellItem );
	OnMsg( PACKETTYPE_TRADEOK, &CDPSrvr::OnTradeOk );
	OnMsg( PACKETTYPE_MELEE_ATTACK, &CDPSrvr::OnMeleeAttack );
	OnMsg( PACKETTYPE_MELEE_ATTACK2, &CDPSrvr::OnMeleeAttack2 );
	OnMsg( PACKETTYPE_MAGIC_ATTACK, &CDPSrvr::OnMagicAttack );
	OnMsg( PACKETTYPE_RANGE_ATTACK, &CDPSrvr::OnRangeAttack );
	OnMsg( PACKETTYPE_SFX_HIT, &CDPSrvr::OnSfxHit );
	OnMsg( PACKETTYPE_USESKILL, &CDPSrvr::OnUseSkill );
	OnMsg( PACKETTYPE_SETTARGET, &CDPSrvr::OnSetTarget );	// Core에 보내야 하는지 확인해 줄것.
	OnMsg( PACKETTYPE_TELESKILL, &CDPSrvr::OnTeleSkill );	
	OnMsg( PACKETTYPE_SKILLTASKBAR, &CDPSrvr::OnSkillTaskBar );
	OnMsg( PACKETTYPE_MODIFYTASKBAR, &CDPSrvr::OnModifyTaskBar);
	OnMsg( PACKETTYPE_QUERYGETPOS, &CDPSrvr::OnQueryGetPos );
	OnMsg( PACKETTYPE_GETPOS, &CDPSrvr::OnGetPos );
	OnMsg( PACKETTYPE_QUERYGETDESTOBJ, &CDPSrvr::OnQueryGetDestObj );
	OnMsg( PACKETTYPE_GETDESTOBJ, &CDPSrvr::OnGetDestObj );
	OnMsg( PACKETTYPE_MEMBERREQUEST, &CDPSrvr::OnPartyRequest );
	OnMsg( PACKETTYPE_MEMBERREQUESTCANCLE, &CDPSrvr::OnPartyRequestCancle );
	OnMsg( PACKETTYPE_PARTYSKILLUSE, &CDPSrvr::OnPartySkillUse );
	OnMsg( PACKETTYPE_ADDFRIENDREQEST, &CDPSrvr::OnAddFriendReqest );
	OnMsg( PACKETTYPE_ADDFRIENDNAMEREQEST, &CDPSrvr::OnAddFriendNameReqest );
	OnMsg( PACKETTYPE_ADDFRIENDCANCEL, &CDPSrvr::OnAddFriendCancel );
	OnMsg( PACKETTYPE_DUELREQUEST, &CDPSrvr::OnDuelRequest );
	OnMsg( PACKETTYPE_DUELYES, &CDPSrvr::OnDuelYes );
	OnMsg( PACKETTYPE_DUELNO, &CDPSrvr::OnDuelNo );
	OnMsg( PACKETTYPE_DUELPARTYREQUEST, &CDPSrvr::OnDuelPartyRequest );
	OnMsg( PACKETTYPE_DUELPARTYYES, &CDPSrvr::OnDuelPartyYes );
	OnMsg( PACKETTYPE_DUELPARTYNO, &CDPSrvr::OnDuelPartyNo );
	OnMsg( PACKETTYPE_SEND_TO_SERVER_AP, &CDPSrvr::OnActionPoint );
	OnMsg( PACKETTYPE_QUERY_PLAYER_DATA, &CDPSrvr::OnQueryPlayerData );
	OnMsg( PACKETTYPE_QUERY_PLAYER_DATA2, &CDPSrvr::OnQueryPlayerData2 );
	OnMsg( PACKETTYPE_GUILD_INVITE, &CDPSrvr::OnGuildInvite );
	OnMsg( PACKETTYPE_IGNORE_GUILD_INVITE, &CDPSrvr::OnIgnoreGuildInvite );
	OnMsg( PACKETTYPE_NW_GUILDLOGO, &CDPSrvr::OnGuildLogo );			// 로고 변경 
	OnMsg( PACKETTYPE_NW_GUILDCONTRIBUTION, &CDPSrvr::OnGuildContribution );		// 공헌도 
	OnMsg( PACKETTYPE_NW_GUILDNOTICE, &CDPSrvr::OnGuildNotice );		// 공지사항  
	OnMsg( PACKETTYPE_REQUEST_GUILD_RANKING, &CDPSrvr::OnRequestGuildRank );
	OnMsg( PACKETTYPE_PVENDOR_OPEN, &CDPSrvr::OnPVendorOpen );
	OnMsg( PACKETTYPE_PVENDOR_CLOSE, &CDPSrvr::OnPVendorClose );
	OnMsg( PACKETTYPE_REGISTER_PVENDOR_ITEM, &CDPSrvr::OnRegisterPVendorItem );
	OnMsg( PACKETTYPE_UNREGISTER_PVENDOR_ITEM, &CDPSrvr::OnUnregisterPVendorItem );
	OnMsg( PACKETTYPE_QUERY_PVENDOR_ITEM, &CDPSrvr::OnQueryPVendorItem );
	OnMsg( PACKETTYPE_BUY_PVENDOR_ITEM, &CDPSrvr::OnBuyPVendorItem );
	OnMsg( PACKETTYPE_REPAIRITEM, &CDPSrvr::OnRepairItem );
	OnMsg( PACKETTYPE_SET_HAIR, &CDPSrvr::OnSetHair );
	OnMsg( PACKETTYPE_BLOCK, &CDPSrvr::OnBlock );
	OnMsg( PACKETTYPE_MOTION, &CDPSrvr::OnMotion );
	OnMsg( PACKETTYPE_SHIP_ACTMSG, &CDPSrvr::OnShipActMsg );
	OnMsg( PACKETTYPE_LOCALPOSFROMIA, &CDPSrvr::OnLocalPosFromIA );
	OnMsg( PACKETTYPE_UPGRADEBASE, &CDPSrvr::OnUpgradeBase );
	OnMsg( PACKETTYPE_ENCHANT, &CDPSrvr::OnEnchant );
	OnMsg( PACKETTYPE_SMELT_SAFETY, &CDPSrvr::OnSmeltSafety );
	OnMsg( PACKETTYPE_REMVOE_ATTRIBUTE, &CDPSrvr::OnRemoveAttribute );
	OnMsg( PACKETTYPE_CHANGE_ATTRIBUTE, &CDPSrvr::OnChangeAttribute );
	OnMsg( PACKETTYPE_PIERCING_SIZE, &CDPSrvr::OnPiercingSize );
	OnMsg( PACKETTYPE_EXPBOXINFO, &CDPSrvr::OnExpBoxInfo );
	OnMsg( PACKETTYPE_RANDOMSCROLL, &CDPSrvr::OnRandomScroll );
	OnMsg( PACHETTYPE_ITEMTRANSY, &CDPSrvr::OnItemTransy );
	OnMsg( PACKETTYPE_PIERCING, &CDPSrvr::OnPiercing );
	OnMsg( PACKETTYPE_PIERCINGREMOVE, &CDPSrvr::OnPiercingRemove );
	OnMsg( PACKETTYPE_ENTERCHTTING, &CDPSrvr::OnEnterChattingRoom );
	OnMsg( PACKETTYPE_CHATTING, &CDPSrvr::OnChatting );
	OnMsg( PACKETTYPE_OPENCHATTINGROOM, &CDPSrvr::OnOpenChattingRoom );
	OnMsg( PACKETTYPE_CLOSECHATTINGROOM, &CDPSrvr::OnCloseChattingRoom );
	OnMsg( PACKETTYPE_COMMONPLACE, &CDPSrvr::OnCommonPlace );
	OnMsg( PACKETTYPE_SETNAVIPOINT, &CDPSrvr::OnSetNaviPoint );
	OnMsg( PACKETTYPE_DO_ESCAPE, &CDPSrvr::OnDoEscape );
	OnMsg( PACKETTYPE_LOG_GAMEMASTER_CHAT, &CDPSrvr::OnGameMasterWhisper );
	OnMsg( PACKETTYPE_RETURNSCROLL, &CDPSrvr::OnReturnScroll );
	OnMsg( PACKETTYPE_ENDSKILLQUEUE, &CDPSrvr::OnEndSkillQueue );
	OnMsg( PACKETTYPE_FOCUSOBJ, &CDPSrvr::OnFoucusObj );

	OnMsg( PACKETTYPE_REMOVEQUEST, &CDPSrvr::OnRemoveQuest );
	OnMsg( PACKETTYPE_COMMERCIALELEM, &CDPSrvr::OnCommercialElem );
//	ON_MSG( PACKETTYPE_DO_COLLECT, OnDoCollect );

	OnMsg( PACKETTYPE_NW_WANTED_GOLD, &CDPSrvr::OnNWWantedGold );
	OnMsg( PACKETTYPE_NW_WANTED_LIST, &CDPSrvr::OnNWWantedList );
	OnMsg( PACKETTYPE_NW_WANTED_INFO, &CDPSrvr::OnNWWantedInfo );
	OnMsg( PACKETTYPE_NW_WANTED_NAME, &CDPSrvr::OnNWWantedName );
	OnMsg( PACKETTYPE_REQ_LEAVE, &CDPSrvr::OnReqLeave );
	OnMsg( PACKETTYPE_RESURRECTION_OK, &CDPSrvr::OnResurrectionOK );
	OnMsg( PACKETTYPE_RESURRECTION_CANCEL, &CDPSrvr::OnResurrectionCancel );
	OnMsg( PACKETTYPE_MODE, &CDPSrvr::OnChangeMode );

	OnMsg( PACKETTYPE_STATEMODE, &CDPSrvr::OnStateMode );
	OnMsg( PACKETTYPE_QUERYSETPLAYERNAME, &CDPSrvr::OnQuerySetPlayerName );
	OnMsg( PACKETTYPE_QUERYSETGUILDNAME, &CDPSrvr::OnQuerySetGuildName );
	OnMsg( PACKETTYPE_CHEERING, &CDPSrvr::OnCheering );
	OnMsg( PACKETTYPE_QUERYEQUIP, &CDPSrvr::OnQueryEquip );
	OnMsg( PACKETTYPE_QUERYEQUIPSETTING, &CDPSrvr::OnQueryEquipSetting );
	OnMsg( PACKETTYPE_QUERYPOSTMAIL, &CDPSrvr::OnQueryPostMail );
	OnMsg( PACKETTYPE_QUERYREMOVEMAIL, &CDPSrvr::OnQueryRemoveMail );
	OnMsg( PACKETTYPE_QUERYGETMAILITEM, &CDPSrvr::OnQueryGetMailItem );
	OnMsg( PACKETTYPE_QUERYGETMAILGOLD, &CDPSrvr::OnQueryGetMailGold );
	OnMsg( PACKETTYPE_READMAIL, &CDPSrvr::OnQueryReadMail );
	OnMsg( PACKETTYPE_QUERYMAILBOX, &CDPSrvr::OnQueryMailBox );
	OnMsg( PACKETTYPE_CHANGEFACE, &CDPSrvr::OnChangeFace );

	OnMsg( PACKETTYPE_CREATEMONSTER, &CDPSrvr::OnCreateMonster );

	OnMsg( PACKETTYPE_IN_GUILDCOMBAT, &CDPSrvr::OnGCApp );
	OnMsg( PACKETTYPE_OUT_GUILDCOMBAT, &CDPSrvr::OnGCCancel );	
	OnMsg( PACKETTYPE_REQUEST_STATUS, &CDPSrvr::OnGCRequestStatus );
	OnMsg( PACKETTYPE_SELECTPLAYER_GUILDCOMBAT, &CDPSrvr::OnGCSelectPlayer );
	OnMsg( PACKETTYPE_SELECTMAP_GUILDCOMBAT, &CDPSrvr::OnGCSelectMap );
	OnMsg( PACKETTYPE_JOIN_GUILDCOMBAT, &CDPSrvr::OnGCJoin );	
	OnMsg( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, &CDPSrvr::OnGCGetPenyaGuild );
	OnMsg( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, &CDPSrvr::OnGCGetPenyaPlayer );
	OnMsg( PACKETTYPE_TELE_GUILDCOMBAT, &CDPSrvr::OnGCTele );
	OnMsg( PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, &CDPSrvr::OnGCPlayerPoint );
	OnMsg( PACKETTYPE_SUMMON_FRIEND, &CDPSrvr::OnSummonFriend );
	OnMsg( PACKETTYPE_SUMMON_FRIEND_CONFIRM, &CDPSrvr::OnSummonFriendConfirm );
	OnMsg( PACKETTYPE_SUMMON_FRIEND_CANCEL, &CDPSrvr::OnSummonFriendCancel );
	OnMsg( PACKETTYPE_SUMMON_PARTY, &CDPSrvr::OnSummonParty );
	OnMsg( PACKETTYPE_SUMMON_PARTY_CONFIRM, &CDPSrvr::OnSummonPartyConfirm );

	OnMsg( PACKETTYPE_REMOVEINVENITEM, &CDPSrvr::OnRemoveInvenItem );
	////////////////////////////////////////////////////////////////////////
	OnMsg( PACKETTYPE_CREATEANGEL, &CDPSrvr::OnCreateAngel );
	OnMsg( PACKETTYPE_ANGELBUFF, &CDPSrvr::OnAngleBuff );
	
#ifdef __EVE_MINIGAME
	OnMsg( PACKETTYPE_KAWIBAWIBO_START, &CDPSrvr::OnKawibawiboStart );
	OnMsg( PACKETTYPE_KAWIBAWIBO_GETITEM, &CDPSrvr::OnKawibawiboGetItem );
	OnMsg( PACKETTYPE_REASSEMBLE_START, &CDPSrvr::OnReassembleStart );
	OnMsg( PACKETTYPE_REASSEMBLE_OPENWND, &CDPSrvr::OnReassembleOpenWnd );
	OnMsg( PACKETTYPE_ALPHABET_OPENWND, &CDPSrvr::OnAlphabetOpenWnd );
	OnMsg( PACKETTYPE_ALPHABET_START, &CDPSrvr::OnAlphabetStart );
	OnMsg( PACKETTYPE_FIVESYSTEM_OPENWND, &CDPSrvr::OnFiveSystemOpenWnd );
	OnMsg( PACKETTYPE_FIVESYSTEM_BET, &CDPSrvr::OnFiveSystemBet );
	OnMsg( PACKETTYPE_FIVESYSTEM_START, &CDPSrvr::OnFiveSystemStart );
	OnMsg( PACKETTYPE_FIVESYSTEM_DESTROYWND, &CDPSrvr::OnFiveSystemDestroyWnd );
#endif // __EVE_MINIGAME
	
	OnMsg( PACKETTYPE_ULTIMATE_MAKEITEM, &CDPSrvr::OnUltimateMakeItem );
	OnMsg( PACKETTYPE_ULTIMATE_MAKEGEM, &CDPSrvr::OnUltimateMakeGem );
	OnMsg( PACKETTYPE_ULTIMATE_TRANSWEAPON, &CDPSrvr::OnUltimateTransWeapon );
	OnMsg( PACKETTYPE_ULTIMATE_SETGEM, &CDPSrvr::OnUltimateSetGem );
	OnMsg( PACKETTYPE_ULTIMATE_REMOVEGEM, &CDPSrvr::OnUltimateRemoveGem );
	OnMsg( PACKETTYPE_ULTIMATE_ENCHANTWEAPON, &CDPSrvr::OnUltimateEnchantWeapon );

#ifdef __TRADESYS
	OnMsg( PACKETTYPE_EXCHANGE, &CDPSrvr::OnExchange );
#endif // __TRADESYS

	OnMsg( PACKETTYPE_PET_RELEASE, &CDPSrvr::OnPetRelease );
	OnMsg( PACKETTYPE_USE_PET_FEED, &CDPSrvr::OnUsePetFeed );
	OnMsg( PACKETTYPE_MAKE_PET_FEED, &CDPSrvr::OnMakePetFeed );
	OnMsg( PACKETTYPE_PET_TAMER_MISTAKE, &CDPSrvr::OnPetTamerMistake );
	OnMsg( PACKETTYPE_PET_TAMER_MIRACLE, &CDPSrvr::OnPetTamerMiracle );
	OnMsg( PACKETTYPE_FEED_POCKET_INACTIVE, &CDPSrvr::OnFeedPocketInactive );

	OnMsg( PACKETTYPE_LEGENDSKILLUP_START, &CDPSrvr::OnLegendSkillStart );

	OnMsg( PACKETTYPE_MODIFY_STATUS, &CDPSrvr::OnModifyStatus );

	OnMsg( PACKETTYPE_GC1TO1_TENDEROPENWND, &CDPSrvr::OnGC1to1TenderOpenWnd );
	OnMsg( PACKETTYPE_GC1TO1_TENDERVIEW, &CDPSrvr::OnGC1to1TenderView );
	OnMsg( PACKETTYPE_GC1TO1_TENDER, &CDPSrvr::OnGC1to1Tender );
	OnMsg( PACKETTYPE_GC1TO1_TENDERCANCEL, &CDPSrvr::OnGC1to1CancelTender );
	OnMsg( PACKETTYPE_GC1TO1_TENDERFAILED, &CDPSrvr::OnGC1to1FailedTender );
	OnMsg( PACKETTYPE_GC1TO1_MEMBERLINEUPOPENWND, &CDPSrvr::OnGC1to1MemberLineUpOpenWnd );
	OnMsg( PACKETTYPE_GC1TO1_MEMBERLINEUP, &CDPSrvr::OnGC1to1MemberLineUp );
	OnMsg( PACKETTYPE_GC1TO1_TELEPORTTONPC, &CDPSrvr::OnGC1to1TeleportToNPC );
	OnMsg( PACKETTYPE_GC1TO1_TELEPORTTOSTAGE, &CDPSrvr::OnGC1to1TeleportToStage );	

	OnMsg( PACKETTYPE_GUILDLOG_VIEW, &CDPSrvr::OnQueryGuildBankLogList );
	OnMsg( PACKETTYPE_SEALCHAR_REQ, &CDPSrvr::OnSealCharReq );
	OnMsg( PACKETTYPE_SEALCHARCONM_REQ, &CDPSrvr::OnSealCharConmReq );
	OnMsg( PACKETTYPE_SEALCHARGET_REQ, &CDPSrvr::OnSealCharGetReq );
	OnMsg( PACKETTYPE_HONOR_LIST_REQ, &CDPSrvr::OnHonorListReq );
	OnMsg( PACKETTYPE_HONOR_CHANGE_REQ, &CDPSrvr::OnHonorChangeReq );

	OnMsg( PACKETTYPE_QUERY_START_COLLECTING, &CDPSrvr::OnQueryStartCollecting );
	OnMsg( PACKETTYPE_QUERY_STOP_COLLECTING, &CDPSrvr::OnQueryStopCollecting );

#ifdef __NPC_BUFF
	OnMsg( PACKETTYPE_NPC_BUFF, &CDPSrvr::OnNPCBuff );
#endif // __NPC_BUFF

	OnMsg( PACKETTYPE_SECRETROOM_TENDEROPENWND, &CDPSrvr::OnSecretRoomTenderOpenWnd );
	OnMsg( PACKETTYPE_SECRETROOM_TENDER, &CDPSrvr::OnSecretRoomTender );
	OnMsg( PACKETTYPE_SECRETROOM_TENDERCANCELRETURN, &CDPSrvr::OnSecretRoomTenderCancelReturn );
	OnMsg( PACKETTYPE_SECRETROOM_LINEUPOPENWND, &CDPSrvr::OnSecretRoomLineUpOpenWnd );
	OnMsg( PACKETTYPE_SECRETROOM_LINEUPMEMBER, &CDPSrvr::OnSecretRoomLineUpMember );
	OnMsg( PACKETTYPE_SECRETROOM_ENTRANCE, &CDPSrvr::OnSecretRoomEntrance );
	OnMsg( PACKETTYPE_SECRETROOM_TELEPORTTONPC, &CDPSrvr::OnSecretRoomTeleportToNPC );
	OnMsg( PACKETTYPE_SECRETROOM_TENDERVIEW, &CDPSrvr::OnSecretRoomTenderView );
	OnMsg( PACKETTYPE_SECRETROOM_TELEPORTTODUNGEON, &CDPSrvr::OnTeleportSecretRoomDungeon );

	OnMsg( PACKETTYPE_ELECTION_ADD_DEPOSIT, &CDPSrvr::OnElectionAddDeposit );
	OnMsg( PACKETTYPE_ELECTION_SET_PLEDGE, &CDPSrvr::OnElectionSetPledge );
	OnMsg( PACKETTYPE_ELECTION_INC_VOTE, &CDPSrvr::OnElectionIncVote );
	OnMsg( PACKETTYPE_L_EVENT_CREATE, &CDPSrvr::OnLEventCreate );
	OnMsg( PACKETTYPE_LORD_SKILL_USE, &CDPSrvr::OnLordSkillUse );
	// 알변환 핸들러
	OnMsg( PACKETTYPE_TRANSFORM_ITEM, &CDPSrvr::OnTransformItem );

	OnMsg( PACKETTYPE_TAX_SET_TAXRATE, &CDPSrvr::OnSetTaxRate );

	OnMsg( PACKETTYPE_HEAVENTOWER_TELEPORT, &CDPSrvr::OnTeleportToHeavenTower );

	OnMsg( PACKETTYPE_TUTORIAL_STATE, &CDPSrvr::OnTutorialState );

	OnMsg( PACKETTYPE_PICKUP_PET_AWAKENING_CANCEL, &CDPSrvr::OnPickupPetAwakeningCancel );

	OnMsg( PACKETTYPE_OPTION_ENABLE_RENDER_MASK, &CDPSrvr::OnOptionEnableRenderMask );

	OnMsg( PACKETTYPE_RAINBOWRACE_PREVRANKING_OPENWND, &CDPSrvr::OnRainbowRacePrevRankingOpenWnd );
	OnMsg( PACKETTYPE_RAINBOWRACE_APPLICATION_OPENWND, &CDPSrvr::OnRainbowRaceApplicationOpenWnd );
	OnMsg( PACKETTYPE_RAINBOWRACE_APPLICATION, &CDPSrvr::OnRainbowRaceApplication );
	OnMsg( PACKETTYPE_RAINBOWRACE_MINIGAME_PACKET, &CDPSrvr::OnRainbowRaceMiniGamePacket );
	OnMsg( PACKETTYPE_RAINBOWRACE_REQ_FINISH, &CDPSrvr::OnRainbowRaceReqFinish );

	OnMsg( PACKETTYPE_HOUSING_SETUPFURNITURE, &CDPSrvr::OnHousingSetupFurniture );
	OnMsg( PACKETTYPE_HOUSING_SETVISITALLOW, &CDPSrvr::OnHousingSetVisitAllow );
	OnMsg( PACKETTYPE_HOUSING_VISITROOM, &CDPSrvr::OnHousingVisitRoom );
	OnMsg( PACKETTYPE_HOUSING_REQVISITABLELIST, &CDPSrvr::OnHousingVisitableList );
	OnMsg( PACKETTYPE_HOUSING_GOOUT, &CDPSrvr::OnHousingGoOut );


	OnMsg( PACKETTYPE_QUESTHELPER_REQNPCPOS, &CDPSrvr::OnReqQuestNPCPos );

	OnMsg( PACKETTYPE_PROPOSE, &CDPSrvr::OnPropose );
	OnMsg( PACKETTYPE_REFUSE, &CDPSrvr::OnRefuse );
	OnMsg( PACKETTYPE_COUPLE, &CDPSrvr::OnCouple );
	OnMsg( PACKETTYPE_DECOUPLE, &CDPSrvr::OnDecouple );
#ifdef __MAP_SECURITY
	OnMsg( PACKETTYPE_MAP_KEY, &CDPSrvr::OnMapKey );
#endif // __MAP_SECURITY
#ifdef __QUIZ
	OnMsg( PACKETTYPE_QUIZ_ENTRANCE, &CDPSrvr::OnQuizEventEntrance );
	OnMsg( PACKETTYPE_QUIZ_TELEPORT, &CDPSrvr::OnQuizEventTeleport );
#endif // __QUIZ
	OnMsg( PACKETTYPE_VISPET_REMOVEVIS, &CDPSrvr::OnRemoveVis );
	OnMsg( PACKETTYPE_VISPET_SWAPVIS, &CDPSrvr::OnSwapVis );
	OnMsg( PACKETTYPE_GUILDHOUSE_BUY, &CDPSrvr::OnBuyGuildHouse );
	OnMsg( PACKETTYPE_GUILDHOUSE_PACKET, &CDPSrvr::OnGuildHousePacket );
	OnMsg( PACKETTYPE_GUILDHOUSE_ENTER, &CDPSrvr::OnGuildHouseEnter );
	OnMsg( PACKETTYPE_GUILDHOUSE_GOOUT, &CDPSrvr::OnGuildHouseGoOut );
	OnMsg( PACKETTYPE_TELEPORTER, &CDPSrvr::OnTeleporterReq );
	OnMsg( PACKETTYPE_QUEST_CHECK, &CDPSrvr::OnCheckedQuest );
	OnMsg( PACKETTYPE_CAMPUS_INVITE, &CDPSrvr::OnInviteCampusMember );
	OnMsg( PACKETTYPE_CAMPUS_ACCEPT, &CDPSrvr::OnAcceptCampusMember );
	OnMsg( PACKETTYPE_CAMPUS_REFUSE, &CDPSrvr::OnRefuseCampusMember );
	OnMsg( PACKETTYPE_CAMPUS_REMOVE_MEMBER, &CDPSrvr::OnRemoveCampusMember );


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	OnMsg( PACKETTYPE_ITEM_SELECT_AWAKENING_VALUE, &CDPSrvr::OnItemSelectAwakeningValue );
	//	mulcom	END100405	각성 보호의 두루마리

#ifdef __GUILD_HOUSE_MIDDLE
	OnMsg( PACKETTYPE_GUILDHOUSE_TENDER_MAINWND, &CDPSrvr::OnGuildHouseTenderMainWnd );
	OnMsg( PACKETTYPE_GUILDHOUSE_TENDER_INFOWND, &CDPSrvr::OnGuildHouseTenderInfoWnd );
	OnMsg( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, &CDPSrvr::OnGuildHouseTenderJoin );
#endif // __GUILD_HOUSE_MIDDLE

	OnMsg(PACKETTYPE_SQUONK_ARBITRARY_PACKET, &CDPSrvr::OnSquonKArbitraryPacket);

	// Reception is disabled on official V15, Emission is disabled on official v21. It is up to
	// the developers of the server to choose whtenever they want to look further in this method
	// on entirely remove this packet.
	// OnMsg(PACKETTYPE_ERRORCODE, &CDPSrvr::OnError);
}

bool CDPSrvrHandlers::Handle(CDPSrvr & self, CAr & ar, DPID dpidCache, DPID dpidUser) {
	DWORD packetId; ar >> packetId;

	const auto handler = m_handlers.find(packetId);

	if (handler == m_handlers.end()) return false;

	const HandlerStruct & pfn = handler->second;

	struct Visitor {
		CDPSrvr & self; CAr & ar;
		DPID dpidCache; DPID dpidUser;

		void operator()(const GalaHandler & gala) const {
			int size;
			BYTE * buffer = ar.GetBuffer(&size);
			(self.*(gala)) (ar, dpidCache, dpidUser, buffer, size);
		}

		void operator()(const UserHandler & userHandler) const {
			CUser * const user = g_UserMng.GetUser(dpidCache, dpidUser);
			if (!IsValidObj(user)) return;

			(self.*(userHandler)) (ar, *user);
		}

		void operator()(const UserPtrHandler & userHandler) const {
			CUser * const user = g_UserMng.GetUser(dpidCache, dpidUser);
			if (!IsValidObj(user)) return;

			(self.*(userHandler)) (ar, user);
		}
	};

	std::visit(Visitor{ self, ar, dpidCache, dpidUser }, pfn);

	if (ar.IsOverflow()) Error("World-Neuz: Packet %08x overflowed", packetId);
	return true;
}



CDPSrvr::~CDPSrvr()
{
	m_dpidCache = DPID_UNKNOWN;			// 캐쉬서버 DPID
}

void CDPSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	BYTE * const lpBuffer = (BYTE *)lpMsg + sizeof(DPID);
	const u_long uBufSize = dwMsgSize - sizeof(DPID);

	CAr ar(lpBuffer, uBufSize);
	m_handlers.Handle(*this, ar, idFrom, *(UNALIGNED DPID *)lpMsg);
}

void CDPSrvr::OnAddConnection( DPID dpid )
{
}

void CDPSrvr::OnRemoveConnection( DPID dpid )
{
	// 실제 캐쉬 서버가 붙을 경우도 있고, 테스트 용으로 telnet이 붙을 경우도 있다.
	// 캐쉬서버와 연결이 끊기면, 캐쉬 서버와 연계된 유저들을 끊어야 한다.
	// 위 2가지를 고려해서, 유저가 등록된 연결만을 실제 캐쉬서버로 간주하고
	// 등록하게 한다.
	if( dpid == m_dpidCache )
	{
		g_UserMng.RemoveAllUsers();		
		m_dpidCache = DPID_UNKNOWN;	
	}
}

void CDPSrvr::OnAddUser( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	m_dpidCache = dpidCache;			// 캐쉬서버의 DPID를 보관한다.

	TCHAR	lpszAccount[MAX_ACCOUNT], lpszpw[MAX_PASSWORD], lpAddr[16];
	DWORD	dwAuthKey;
	u_long	idPlayer;
	BYTE	nSlot;
	DPID	dpidSocket;

	ar >> dwAuthKey >> idPlayer >> nSlot;
	ar >> dpidSocket;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar.ReadString( lpszpw, MAX_PASSWORD );
	ar.ReadString( lpAddr, 16 );

	if( nSlot >= 3 )
		return;

	CUser* pUser = (CUser*)prj.GetUserByID( idPlayer );
	if( pUser )
	{
		//WriteLog( "CDPSrvr::OnAddUser idPlayer:%d account:%s DPID:%d", idPlayer, lpszAccount, dpidUser );
		// 캐쉬서버에는 socket번호를 보내야 한다. ( pUser->m_Snapshot.dpidUser는 소켓번호 )
		QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
		QueryDestroyPlayer( dpidCache, dpidSocket, dpidUser, idPlayer );	
		return;
	}

	pUser = g_UserMng.AddUser( dpidCache, dpidUser, dpidSocket );
	if( pUser == NULL )
		return;

	pUser->m_dwAuthKey = dwAuthKey;
	memcpy( pUser->m_playAccount.lpAddr, lpAddr, 16 );

	//	TRANS
	BEFORESENDDUAL( arJoin, PACKETTYPE_JOIN, dpidCache, dpidUser );
	arJoin << dwAuthKey;
	arJoin.WriteString( lpszAccount );
	arJoin.WriteString( lpszpw );
	arJoin << nSlot << idPlayer;
	SEND( arJoin, &g_dpDBClient, DPID_SERVERPLAYER );
}


void CDPSrvr::OnRemoveUser( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	g_UserMng.RemoveUser( (DWORD)dpidUser ); // dpidUser는 CACHE에서 사용되는 serial한 값 
}

void CDPSrvr::OnChat( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	static	TCHAR	sChat[1024];
	if( uBufSize > 1031 )	// 4 + 4 + 1024 - 1		= 1031
		return;

	ar.ReadString( sChat, 1024 );
	CString strChat	= sChat;
	strChat.Replace( "\\n", " " );
	
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->m_dwAuthorization >= AUTH_LOGCHATTING )		// 일반유저가 아니면 로그남김 모든 로그남김
		{
			g_dpDBClient.SendLogGamemaChat( pUser, strChat );
		}

		if (sChat[0] == '/' && g_textCmdFuncs.ParseCommand(strChat, pUser)) return;

		if( pUser->IsMode( TALK_MODE ) )
			return;
#ifdef __JEFF_9_20
		int nText	= pUser->GetMuteText();
		if(  nText )
		{
			pUser->AddDefinedText( nText );
			return;
		}
#endif	// __JEFF_9_20

		if (!(pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT))) {
			ParsingEffect(sChat, strlen(sChat));
		}

		strChat	= sChat;
		strChat.Replace( "\\n", " " );
	
		g_UserMng.AddChat( pUser, strChat );
	}
}

void CDPSrvr::OnCtrlCoolTimeCancel(CAr &, DPID dpidCache, DPID dpidUser, LPBYTE, u_long) {
	CUser * const pUser = g_UserMng.GetUser(dpidCache, dpidUser);

	if (!IsValidObj(pUser)) return;

	pUser->m_dwCtrlReadyTime = 0xffffffff;

	CCommonCtrl * pCtrl = (CCommonCtrl *)prj.GetCtrl(pUser->m_dwCtrlReadyId);
	if (!IsValidObj(pCtrl)) return;

	pUser->m_dwCtrlReadyId = NULL_ID;

	pCtrl->m_dwCtrlReadyTime = 0xffffffff;
	pCtrl->m_bAction         = FALSE;
}

void CDPSrvr::OnDoEquip(CAr & ar, CUser & pUser) {
	const auto [nId, nPart] = ar.Extract<DWORD, int>();
	// An extra float will be extracted later for unequiped PARTS_RIDE for flight check
	
	if (nPart >= MAX_HUMAN_PARTS) return;

	CItemElem* pItemElem = pUser.m_Inventory.GetAtId( nId );
	if (!IsUsableItem( pItemElem )) return;

	if (pUser.m_Inventory.IsEquip(nId)) {
		if (nPart > 0 && pItemElem != pUser.m_Inventory.GetEquip(nPart)) {
			return;
		}
	}

	const ItemProp * const pItemProp = pItemElem->GetProp();
	if (pItemProp && pItemProp->dwParts == PARTS_RIDE) {
		if (!pUser.m_Inventory.IsEquip(nId)) {
			FLOAT fVal;
			ar >> fVal;
			if (fVal != pItemProp->fFlightSpeed) {
				pUser.AddDefinedText(TID_GAME_MODIFY_FLIGHT_SPEED);
				return;
			}
		}
	}

	if (!pUser.IsDie()) {
		pUser.DoUseEquipmentItem(pItemElem, nId, nPart);
	}
}

void CDPSrvr::OnMoveItem(CAr & ar, CUser & pUser) {
	const auto [nSrcIndex, nDstIndex] = ar.Extract<BYTE, BYTE>();

	if (nSrcIndex == nDstIndex) return;
	if (nDstIndex >= MAX_INVENTORY || nSrcIndex >= MAX_INVENTORY)
		return;

	CItemElem* pItemSrc = pUser.m_Inventory.GetAt( nSrcIndex );
	CItemElem* pItemDst = pUser.m_Inventory.GetAt( nDstIndex );
	if( pItemDst == NULL || IsUsableItem( pItemDst ) ) // 빈 공간 or 거래중이지 않는 아이템 ?			
	{	
		if( IsUsableItem( pItemSrc ) )					// 거래중이지 않는 아이템 ?
		{
			pUser.m_Inventory.Swap( nSrcIndex, nDstIndex );
			pUser.AddMoveItem( 0, nSrcIndex, nDstIndex );
		}
	}
}

void CDPSrvr::OnDropItem(CAr & ar, CUser & pUser) {
	const auto [dwItemId, nDropNum, vPos] = ar.Extract<
		DWORD, short, D3DXVECTOR3
	>();

	if (nDropNum <= 0) return;

	if (g_eLocal.GetState(EVE_DROPITEMREMOVE)) {
		CItemElem * pItemElem = pUser.GetItemId(dwItemId);

		if (!IsUsableItem(pItemElem)) return;
		if (!pUser.IsDropable(pItemElem, FALSE)) return;
		if (nDropNum > pItemElem->m_nItemNum) return;

		pUser.RemoveItem((BYTE)dwItemId, nDropNum);
	} else {
		pUser.DropItem(dwItemId, nDropNum, vPos);
	}
}

void CDPSrvr::OnReplace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	ASSERT( 0 );
	Error("CDPSrvr::OnReplace called\n");
}

void CDPSrvr::OnScriptDialogReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	static TCHAR lpKey[256];
	OBJID objid;
	int nGlobal1, nGlobal2, nGlobal3, nGlobal4;

	ar >> objid;
 	ar.ReadString( lpKey, 256 );

	ar >> nGlobal1 >> nGlobal2 >> nGlobal3 >> nGlobal4;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __QUEST_1208
		DWORD dwTickCount	= GetTickCount();
		if( dwTickCount < pUser->m_tickScript + 400 )
			return;
		pUser->m_tickScript	= dwTickCount;
#endif	// __QUEST_1208
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) )
		{
#ifdef __HACK_1130
			char lpOutputString[512]	= { 0,};
			sprintf( lpOutputString, "npc = %s, key = %s, n1 = %d, n2 = %d, n3 = %d, n4 = %d",
				pMover->GetName(), lpKey, nGlobal1, nGlobal2, nGlobal3, nGlobal4 );
			OutputDebugString( lpOutputString );
#endif	// __HACK_1130

			D3DXVECTOR3 vOut	= pUser->GetPos() - pMover->GetPos();
			if( fabs( (double)D3DXVec3LengthSq( &vOut ) ) > MAX_LEN_MOVER_MENU )
			{
				return;
			}

			if( pMover->m_pNpcProperty &&
				pMover->m_pNpcProperty->IsDialogLoaded() )
			{
				if( lstrlen( lpKey ) == 0 )
					lstrcpy( lpKey, _T( "#init" ) );
				if( nGlobal3 == 0 )	
					nGlobal3 = (int)pMover->GetId();
				if( nGlobal4 == 0 )	
					nGlobal4 = (int)pUser->GetId();

				pMover->m_pNpcProperty->RunDialog( lpKey, NULL, nGlobal1, (int)pMover->GetId(), (int)pUser->GetId(), nGlobal2 );

				// 퀘스트 조건 대화에 맞는 키일 경우는 대화 성공 플렉을 세팅하고 퀘스트 정보를 클라이언트에 보내준다.
				if (pUser->m_quests) {
					const auto lpQuest = std::ranges::find_if(
						pUser->m_quests->current,
						[&](const QUEST & quest) {
							const QuestProp * pQuestProp = quest.GetProp();
							return pQuestProp
								&& (strcmp(pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey) == 0)
								&& (strcmp(pQuestProp->m_szEndCondDlgAddKey, lpKey) == 0);
						}
					);

					if (lpQuest != pUser->m_quests->current.end()) {
						lpQuest->m_bDialog = TRUE;
						pUser->AddSetQuest(&*lpQuest);
					}
				}
			}
		}
	}
}

void CDPSrvr::OnRevival( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) )
	{
		pUser->m_Resurrection_Data = std::nullopt;

		if( pUser->IsDie() == FALSE )
		{
			Error( "OnRevival\t// %s is not dead", pUser->GetName() );
			return;
		}
		
		BOOL bGuildCombat = FALSE;

		CItemElem* pItemElem = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION );
		if( IsUsableItem( pItemElem ) )
		{
			pUser->m_nDead = PROCESS_COUNT * 5;		// 죽은 후 5초간은 무적

			if( pUser->IsChaotic() )
			{
				pUser->SubDieDecExp( TRUE, 0, TRUE );		// 죽어서 부활하면 겸치 깎임,.
			}
			else
			{
			}
			float fRate = 0.1f;

			fRate = 0.2f;

			pUser->m_pActMover->ClearState();

			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
				fRate = 1.0f;


			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			{
				g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL );
				g_GuildCombatMng.JoinObserver( pUser );
			}
			else
			{
				g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL );

				ItemProp* pItemProp = pItemElem->GetProp();
				if( pItemProp && pItemProp->dwSfxObj3 != -1 )
					g_UserMng.AddCreateSfxObj((CMover *)pUser, pItemElem->GetProp()->dwSfxObj3 );

				// 상용화 아이템 사용 로그 삽입
				g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemProp );
				pUser->RemoveItem( (BYTE)pItemElem->m_dwObjId, 1 );
			}

			pUser->SetPointParam( DST_HP, (int)( pUser->GetMaxHitPoint() * fRate ) );
					
			int nVal	= (int)(pUser->GetMaxManaPoint() * fRate);			// mp 회복
			if( pUser->GetManaPoint() < nVal )
				pUser->SetPointParam( DST_MP, nVal );
			
			nVal	= (int)(pUser->GetMaxFatiguePoint() * fRate);			// fp 회복
			if( pUser->GetFatiguePoint() < nVal )
				pUser->SetPointParam( DST_FP, nVal );			
		}
		else
		{
			Error( "Error CMover::OnRevival Not Inventory RevivalItem" );
		}
	}
}

void CDPSrvr::OnRevivalLodestar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) )
	{
		pUser->m_Resurrection_Data = std::nullopt;

		if( pUser->IsDie() == FALSE )
		{
			// 이리로 들어오는 경우가 생긴다. 생각엔 이미 한번 여길 들어왔다가 렉 걸린동안 
			// 로드스타를 또 누르면 들어오는게 아닌가 싶다.
			return;
		}

		CCommonCtrl* pCtrl	= CreateExpBox( pUser );
		if( pCtrl )
		{
			pCtrl->AddItToGlobalId();
			pWorld->ADDOBJ( pCtrl, FALSE, pUser->GetLayer() );
			g_dpDBClient.SendLogExpBox( pUser->m_idPlayer, pCtrl->GetId(), pCtrl->m_nExpBox );
		}
		
		BOOL bGuildCombat = FALSE;
		
		g_dpDBClient.SendLogLevelUp( (CMover*)pUser, 9 );	// 로드스타로 부활 로그

		pUser->m_nDead = PROCESS_COUNT * 5;		// 죽은 후 5초간은 무적
		float fRate		= pUser->SubDieDecExp();		// 죽어서 부활하면 겸치 깎임,.
		pUser->m_pActMover->ClearState();

		if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			fRate = 1.0f;
		else if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
			fRate = 1.0f;
		else
			fRate = 0.2f;
		

		if( pWorld->GetID() == WI_WORLD_GUILDWAR )
		{
			g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL );
			g_GuildCombatMng.JoinObserver( pUser );
		}
		else
		{
			g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_REVIVAL_TO_LODESTAR );
		}		

		pUser->SetPointParam( DST_HP, (int)(pUser->GetMaxHitPoint() * fRate) );	// hp 회복
		
		int nVal	= (int)(pUser->GetMaxManaPoint() * fRate);			// mp 회복
		//if( pUser->GetManaPoint() < nVal )
			pUser->SetPointParam( DST_MP, nVal );
		
		nVal	= (int)(pUser->GetMaxFatiguePoint() * fRate);			// fp 회복
		//if( pUser->GetFatiguePoint() < nVal )
			pUser->SetPointParam( DST_FP, nVal );

		if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			return;

		if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( pUser ) )
		{
			pUser->Replace( WI_WORLD_MADRIGAL, CSecretRoomMng::GetInstance()->GetRevivalPos( pUser ), REPLACE_NORMAL, nDefaultLayer );
			return;
		}
		
		// 보스몹 맵에서 죽었다...
		// 그러면 부활은 마을에서...
		if( pWorld->GetID() == WI_DUNGEON_MUSCLE || pWorld->GetID() == WI_DUNGEON_KRRR || pWorld->GetID() == WI_DUNGEON_BEAR )
		{			
			pUser->Replace( WI_WORLD_MADRIGAL, D3DXVECTOR3( 6968.0f, 0.0f, 3328.8f ), REPLACE_NORMAL, nDefaultLayer );
			return;
		}

		const REGIONELEM * pRgnElem = g_WorldMng.GetRevival(
			*pWorld,
			pUser->GetPos(),
			pUser->IsChaotic() && !pWorld->IsArena()
		);

		if( pRgnElem ) 
			pUser->Replace( *pRgnElem, REPLACE_FORCE, nRevivalLayer );
		else 
			pUser->Replace( pWorld->GetID(), pUser->GetPos(), REPLACE_FORCE, pUser->GetLayer() );
	}
}

void CDPSrvr::OnRevivalLodelight( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
}

void CDPSrvr::OnSetLodelight( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_MARKING>(pUser) )
			return;
		pUser->SetMarkingPos();
		pUser->AddDefinedText( TID_GAME_LODELIGHT, "" );
	}
}

void CDPSrvr::OnCorrReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID idObj;
	ar >> idObj;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );		// 어느유저로부터 날아온거냐.
	if( IsValidObj( pUser ) )
	{
		CMover *pMover = prj.GetMover( idObj );	// 선택된 오브젝트의 포인터
		if( IsValidObj( pMover ) )
		{
			pUser->AddCorrReq( pMover );	// 요청한 클라에게 선택된 오브젝트의 정보를 보냄.
		}
	}
}

void CDPSrvr::OnCreateGuildCloak( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	CUser * const pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser)) return;

	CGuild * const pGuild = pUser->GetGuild();
	if (!pGuild) return;

	if (pGuild->m_dwLogo == 0) {
		//  로고가 지정되어 있지 않거나
		// 로그가 지정되지 않아서 못만듬
		pUser->AddDefinedText(TID_GAME_GUILDSETTINGLOGO, "");
		return;
	}

	if (!pGuild->IsMaster(pUser->m_idPlayer)) {
		// 마스터가 아니거나
		// 마스터가 아니므로 만들수가 없음.
		pUser->AddDefinedText(TID_GAME_GUILDONLYMASTERLOGO, "");
		return;
	}

	// 길드창고가 망토를 넣을 공간이 충분한지 체크한다. 물론 길드 망토를 길드 창고에 넣을때이다. 아니라면 주석처리 해주세용
	if (pGuild->m_GuildBank.GetEmptyCount() == 0) {
		pUser->AddDefinedText(TID_GAME_GUILDBANKFULL, "");		// 길드창고가 꽉찼시유~
		return;
	}

	// 길드창고에 돈이 충분하냐?
	if (pGuild->m_nGoldGuild < 10000) {
		pUser->AddDefinedText(TID_GAME_GUILDNEEDGOLD, "");		// 길드창고에 돈이 엄떵!
		return;
	}

	pGuild->m_nGoldGuild -= 10000;

	// 길드창고에서 돈 지불하고, 길드 망토를 길드창고에 생성시킴.

	constexpr auto GetCloakIdFromLogoId = [](const DWORD guildLogo) -> DWORD {
		if (guildLogo == 0 || guildLogo == 999) return II_ARM_S_CLO_CLO_BLANK;
		const DWORD cloakId = II_ARM_S_CLO_CLO_SYSCLOAK01 + guildLogo - 1;
		if (cloakId < II_ARM_S_CLO_CLO_SYSCLOAK01 || cloakId >= II_ARM_S_CLO_CLO_SYSCLOAK20) {
			return II_ARM_S_CLO_CLO_SYSCLOAK01;
		} else {
			return cloakId;
		}
	};

	CItemElem itemElem;
	itemElem.m_dwItemId = GetCloakIdFromLogoId(pGuild->m_dwLogo);

	itemElem.m_nItemNum		= 1;
	// 길드 아이디를 망토에 박음. 클라에선 숫자를 기반으로 커스텀 망토를 읽는다.
	// 커스텀 망토가 아닌경우는 이번호로 길드이름을 보여준다.
	itemElem.m_idGuild	= pGuild->m_idGuild;			
			

	// a. 요청한 클라에게 길드창고 페냐가 소모되었음을 알린다.
	// b. 현재 같은 서버에 있는 같은 길드원인 클라이언트에게 페냐가 소모되었음을 알린다.
	// c. 다른 멀티서버셋에 있는 같은 길드원인 클라이언트에게 페냐가 소모되었을을 알린다.
	itemElem.SetSerialNumber();
	ItemProp* pItemProp		= itemElem.GetProp();
	if( pItemProp )
		itemElem.m_nHitPoint	= pItemProp->dwEndurance;
	else
		itemElem.m_nHitPoint	= 0;

	pUser->AddPutItemGuildBank( &itemElem );
	pGuild->m_GuildBank.Add( &itemElem );
	g_UserMng.AddPutItemElem( pUser, &itemElem );
						
	// 자신의 길드원들의 루프를 돌면서 길드망토를 사서 10000페냐가 소모되었다고 알려준다.
	// 물론 루프에서 요청한 클라이언트에게도 메시지를 함께 보낸다.
	for (CUser * pUsertmp : AllMembers(*pGuild)) {
		pUsertmp->AddGetGoldGuildBank( 10000, 2, pUser->m_idPlayer, 1 );	// 2는 업데이트 해야할 클라이게
	}
	// 현 멀티셋 서버에는 위 루틴이 모두 10000페냐가 소모됨을 알렸으므로 DPCoreClient로 캐시서버에 요청하여 
	// 모든 멀티셋에 10000페냐가 소모되었다고 알린다. 물론 보내는 이 멀티셋 서버는 이 메시지를 무시해야 한다. ( 무시하게 해놨지만 잘 될런지 -_- )
	g_DPCoreClient.SendGuildMsgControl_Bank_Penya( pUser, 10000, 2, 1 ); 	// 2는 업데이트 해야할 다른 월드서버의 클라이언트
	UpdateGuildBank(pGuild, GUILD_CLOAK, 0, pUser->m_idPlayer, &itemElem, 10000, 1 ); // 0은 길드 페냐를 업데이트 한다는 것이다.(실은 모든것을 업데이트하지만 -_-)
	pUser->AddDefinedText( TID_GAME_GUILDCREATECLOAK, "" );
}

void CDPSrvr::OnQueryGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) && !pMover->IsEmptyDestObj() )
			pUser->AddGetDestObj( objid, pMover->GetDestId(), pMover->m_fArrivalRange );
	}
}

void CDPSrvr::OnGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid, objidDest;
	ar >> objid >> objidDest;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->SetDestObj( objidDest );
/*
	{
		if( NULL_ID == objid )
		{
			pUser->SetDestObj( objidDest );
		}
		else
		{
			CUser* ptr	= prj.GetUser( objid );
			if( IsValidObj( ptr ) )
				ptr->AddGetDestObj( pUser->GetId(), objidDest, ptr->m_fArrivalRange );
		}
	}
*/
}

void CDPSrvr::OnQueryGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) )
		{
			if( FALSE == pMover->IsPlayer() )
			{
				pUser->AddGetPos( objid, pMover->GetPos(), pMover->GetAngle() );
			}
			else
			{
				( (CUser*)pMover )->AddQueryGetPos( pUser->GetId() );
			}
		}
	}
}

void CDPSrvr::OnGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	D3DXVECTOR3 vPos;
	float fAngle;
	OBJID objid;
	ar >> vPos >> fAngle >> objid;
	
	if( _isnan((double)fAngle) )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		D3DXVECTOR3 vDistance	= pUser->GetPos() - vPos;
		if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
		{
//			Error( "PACKETTYPE_GETPOS" );
			return;
		}

		if( NULL_ID == objid )
		{
			pUser->SetPos( vPos );
			pUser->SetAngle( fAngle );
			
			pUser->m_fWaitQueryGetPos	= FALSE;
			
			if( FALSE == pUser->IsEmptyDestPos() )
				pUser->SetDestPos( pUser->GetDestPos(), pUser->m_bForward, FALSE );
		}
		else
		{
			CUser* ptr	= prj.GetUser( objid );
			if( IsValidObj( ptr ) )
				ptr->AddGetPos( pUser->GetId(), vPos, fAngle );
		}
	}
}

void CDPSrvr::OnPartyRequest(CAr & ar, CUser & pUser) {
	u_long uMemberid; ar >> uMemberid;

	CParty * myParty = g_PartyMng.GetParty(pUser.m_idparty);
	if (myParty) {
		if (!myParty->IsLeader(pUser.m_idPlayer)) {
			return;
		}
	}

	InviteParty(pUser.m_idPlayer, uMemberid);
}

void CDPSrvr::OnPartyRequestCancle(CAr & ar, CUser & pMember) {
	const auto [uLeaderId, nMode] = ar.Extract<u_long, int>();
	
	CUser * pLeader = g_UserMng.GetUserByPlayerID(uLeaderId);
	if (IsValidObj(pLeader)) {
		pLeader->AddPartyRequestCancel(pMember.m_idPlayer, nMode);
	}
}

void CDPSrvr::OnPartySkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid;
	int nSkill;
	ar >> uLeaderid >> nSkill;	

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsInvalidObj( pUser ) )		
		return;
	
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
		if( pParty->IsLeader( pUser->m_idPlayer ) )
			pParty->DoUsePartySkill( pUser->m_idparty, pUser->m_idPlayer, nSkill );
	}
}

void CDPSrvr::OnAddFriendReqest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid, uMemberid;
	ar >> uLeaderid >> uMemberid;
	
	CUser* pLeader	= g_UserMng.GetUserByPlayerID( uLeaderid );
	CUser* pMember	= g_UserMng.GetUserByPlayerID( uMemberid );
	if( IsValidObj( pMember ) && IsValidObj( pLeader ) )
	{

		if( 0 < pLeader->m_nDuel ||  0 < pMember->m_nDuel )
		{
			return;
		}

		// 길드대전장에는 친구추가를 할수 없습니다
		CWorld* pWorldLeader = pLeader->GetWorld();
		CWorld* pWorldMember = pMember->GetWorld();
		if( ( pWorldLeader && pWorldLeader->GetID() == WI_WORLD_GUILDWAR ) ||
			( pWorldMember && pWorldMember->GetID() == WI_WORLD_GUILDWAR ) )
		{			
			pLeader->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD) );
			return;
		}
		if( g_GuildCombat1to1Mng.IsPossibleUser( pLeader ) || g_GuildCombat1to1Mng.IsPossibleUser( pMember ) )
		{
			pLeader->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD) );
			return;
		}

		if( !pLeader->m_RTMessenger.GetFriend( uMemberid ) )
		{
			if( pMember->IsAttackMode() )
				pLeader->AddDefinedText( TID_GAME_BATTLE_NOTFRIEND, "" );
			else
				pMember->AddFriendReqest( uLeaderid, pLeader->m_nJob, (BYTE)pLeader->GetSex(), pLeader->GetName() );	// 친구 등록 여부 질의
		}
	}
}

// 다른 멀티서버에 있는 캐릭을 추가 시키려면 코어로 보내야 한다.
// 이름으로 오므로 월드에서 idPlayer로 바꿔서 보냄
void CDPSrvr::OnAddFriendNameReqest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid, uMember;
	char szMemberName[64] = {0,};

	ar >> uLeaderid;
	ar.ReadString( szMemberName, 64 );

	uMember	= CPlayerDataCenter::GetInstance()->GetPlayerId( szMemberName );
	CUser* pLeader	= g_UserMng.GetUserByPlayerID( uLeaderid );	

	if( IsValidObj( pLeader ) )
	{
		if( uMember > 0 )
		{
			if( !pLeader->m_RTMessenger.GetFriend( uMember ) )
				g_DPCoreClient.SendAddFriendNameReqest( uLeaderid, pLeader->m_nJob, (BYTE)pLeader->GetSex(), uMember, pLeader->GetName(), szMemberName );
			else
				pLeader->AddFriendError( 1, szMemberName );
		}
		else
		{
			// 이 이름을 가지고 잇는 캐릭은 없음.
			pLeader->AddFriendError( 2, szMemberName );
		}
	}
}


void CDPSrvr::OnAddFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uLeaderid, uMemberid;
	ar >> uLeaderid >> uMemberid;

	CUser* pLeader = g_UserMng.GetUserByPlayerID( uLeaderid );
	if( IsValidObj( pLeader ) )
		pLeader->AddFriendCancel();	// uMemberid
}

void CDPSrvr::OnActionPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nAP;
	ar >> nAP;

	if( nAP < 0 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		pUser->m_playTaskBar.m_nActionPoint = nAP;
	}
}


void CDPSrvr::OnRemoveQuest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	QuestId dwQuestCancelID; ar >> dwQuestCancelID;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __QUEST_1208
		DWORD dwTickCount	= GetTickCount();
		if( dwTickCount < pUser->m_tickScript + 400 )
			return;
		pUser->m_tickScript	= dwTickCount;
#endif	// __QUEST_1208
		LPQUEST lpQuest = pUser->GetQuest( dwQuestCancelID );
		if( lpQuest )
		{
			if( lpQuest->m_nState != QS_END )
			{
				const QuestProp * pQuestProp = lpQuest->GetProp();
				if( pQuestProp && pQuestProp->m_bNoRemove == FALSE )
				{
					pUser->RemoveQuest( dwQuestCancelID );
					pUser->AddCancelQuest( dwQuestCancelID );
					g_dpDBClient.CalluspLoggingQuest( pUser->m_idPlayer, dwQuestCancelID, 30 );
					// 시작시 변신을 했으면 퀘스트 삭제시 변신 해제시킨다.
					if( pQuestProp->m_nBeginSetDisguiseMoverIndex )
					{
						pUser->NoDisguise();
						g_UserMng.AddNoDisguise( pUser );
					}
				}
			}
		}
	}
}

void CDPSrvr::OnQueryPlayerData( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idPlayer;
	ar >> idPlayer;
	int nVer;
	ar >> nVer;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
		if( pPlayerData && pPlayerData->data.nVer != nVer )
			pUser->AddQueryPlayerData( idPlayer, pPlayerData );
	}
}

void CDPSrvr::OnQueryPlayerData2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nSize;
//	u_long idPlayer;
	ar >> nSize;

	if( nSize > 1024 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		for( int i = 0; i < nSize; i++ )
		{
			PDVer	pdv;
			ar.Read( &pdv, sizeof(PDVer) );
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pdv.idPlayer );
			if( pPlayerData && pPlayerData->data.nVer != pdv.nVer )
				pUser->AddQueryPlayerData( pdv.idPlayer, pPlayerData );
		}
	}
}

void CDPSrvr::OnGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	InviteCompany( pUser, objid );
}

void CDPSrvr::OnIgnoreGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idPlayer;
	ar >> idPlayer;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CUser* pPlayer	= g_UserMng.GetUserByPlayerID( idPlayer );		// kingpin
		if( IsValidObj( pPlayer ) )
		{
			pPlayer->AddDefinedText( TID_GAME_COMACCEPTDENY, "%s", pUser->GetName( TRUE ) );
		}
	}
}

// 로고 변경 
void CDPSrvr::OnGuildLogo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwLogo;
	ar >> dwLogo;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( dwLogo > CUSTOM_LOGO_MAX )
		return;

	if( dwLogo > 20 && !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
		return;

	g_DPCoreClient.SendGuildStatLogo( pUser, dwLogo );
}

// 공헌도 
void CDPSrvr::OnGuildContribution( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE cbPxpCount, cbItemFlag;
	int nGold;

	cbItemFlag = 0;
	ar >> cbPxpCount >> nGold;
	ar >> cbItemFlag;

	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE ) 
		return;

	if( nGold > 0 )
	{
		if( pUser->GetGold() >= nGold ) 
		{
			if( g_DPCoreClient.SendGuildStatPenya( pUser, nGold ) )
			{
				pUser->AddGold( -nGold );

				LogItemInfo aLogItem;
				aLogItem.Action = "W";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDBANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold() + nGold;
				aLogItem.Gold2 = pUser->GetGold();
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				OnLogItem( aLogItem );
			}
		} 
		else
		{
			pUser->AddDefinedText( TID_GAME_GUILDNOTENGGOLD, "" );	// 인벤에 돈이부족
		}
	}
	else if( cbItemFlag )
	{
		for( int i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( !IsUsableItem( pItemElem ) )
				continue;

			if( pItemElem->GetProp()->dwItemKind3 != IK3_GEM )
				continue;

			int nValue = 0;

			if( pItemElem->m_nItemNum > 0 )
			{
				// 아이템 레벨에 따라서 공헌도를 다르게 한다.
				nValue = (pItemElem->GetProp()->dwItemLV + 1) / 2;	
				nValue *= pItemElem->m_nItemNum;					
			}

			if( nValue > 0 )
			{
				if( g_DPCoreClient.SendGuildStatPxp( pUser, nValue ) )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "V";
					aLogItem.SendName = pUser->GetName();
					aLogItem.RecvName = "GUILDBANK";
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					OnLogItem( aLogItem, pItemElem, pItemElem->m_nItemNum );
					pUser->RemoveItem( (BYTE)i, pItemElem->m_nItemNum );
				}
			}
		}

	}
}

// 공지사항
void CDPSrvr::OnGuildNotice( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	const auto [szNotice] = ar.Extract<char[MAX_BYTE_NOTICE]>();

	if (std::strlen(szNotice) == 0) return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_DPCoreClient.SendGuildStatNotice( pUser, szNotice );
}

void CDPSrvr::OnDuelRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CUser* pDstUser = g_UserMng.GetUserByPlayerID( uidDst );
	if( IsValidObj( pUser ) && IsValidObj( pDstUser ) )
	{
		if( 0 < pUser->m_idparty && pUser->m_idparty == pDstUser->m_idparty )
		{
			pUser->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			return;
		}

		if( 0 < pUser->m_idparty ||  0 < pDstUser->m_idparty )
			return;
		if( pUser->m_vtInfo.GetOther() )	// 거래중 이면 듀얼 불가 
			return;
		if( pDstUser->m_vtInfo.GetOther() )	// 거래중 이면 듀얼 불가 
			return;
	

		if (pUser->GetSummonState() != CUser::SummonState::Ok_0)
			return;
		if (pDstUser->GetSummonState() != CUser::SummonState::Ok_0)
			return;


		if( pUser->IsPVPInspection( pDstUser, CUser::PVPInspection::Solo ) )
		{
			if( pDstUser->IsMode( PVPCONFIRM_MODE ) )
			{
				pUser->AddDefinedText( TID_PK_MODE_REJECT, "" );	// PVP거절 모드입니다
			}
			else
			{
#ifdef __HACK_1130
				pUser->m_tmDuelRequest	= GetTickCount();
#endif	// __HACK_1130
				pDstUser->AddDuelRequest( uidSrc, uidDst );
			}
		}
	}
}

// 듀얼승락을 받음.  두캐릭터에게 시작하라고 보내줘야 함.
void CDPSrvr::OnDuelYes( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;

	CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );
#ifdef __HACK_1130
	CUser* pDst	=	g_UserMng.GetUser( dpidCache, dpidUser );
#else	// __HACK_1130
	CUser* pDst = g_UserMng.GetUserByPlayerID( uidDst );
#endif	// __HACK_1130

	if( IsValidObj(pSrc) && IsValidObj(pDst) )
	{
		if( 0 < pSrc->m_idparty && pSrc->m_idparty == pDst->m_idparty )
		{
			pSrc->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			pDst->AddDefinedText( TID_PK_PARTY_LIMIT, "" );	// 같은파티원
			return;
		}

		if( 0 < pSrc->m_idparty ||  0 < pDst->m_idparty )
		{
			return;
		}
		//개인상점 중에는 듀얼 불가 
		if( pSrc->m_vtInfo.VendorIsVendor() || pSrc->m_vtInfo.IsVendorOpen() ||
            pDst->m_vtInfo.VendorIsVendor() || pDst->m_vtInfo.IsVendorOpen() )
		{
			return;	//
		}


		if( pSrc->IsPVPInspection( pDst, CUser::PVPInspection::Solo) )
		{
#ifdef __HACK_1130
			if( pSrc->m_tmDuelRequest + SEC( 10 ) < GetTickCount() )	// 듀얼 신청 시간을 10초 초과하면
			{
				pSrc->m_tmDuelRequest	= 0;
				return;
			}
#endif	// __HACK_1130
			pSrc->m_nDuel = 1;
			pSrc->m_nDuelState = 104;
			pSrc->m_idDuelOther = pDst->GetId();
			pDst->m_nDuel = 1;
			pDst->m_nDuelState = 104;
			pDst->m_idDuelOther = pSrc->GetId();
			pSrc->AddDuelStart( uidDst );	// 서로 상대방에 대한 아이디만 보내주면 된다.
			pDst->AddDuelStart( uidSrc );
			pSrc->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pDst->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pSrc->SetPosChanged( TRUE );
			pDst->SetPosChanged( TRUE );
		}
	}
}

// pUser가 듀얼 신청을 거부했다. pSrc에게 알려야 한다.
void CDPSrvr::OnDuelNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc; //, uidDst;
	ar >> uidSrc;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );
		if( IsValidObj(pSrc) )
			pSrc->AddDuelNo( pUser->GetId() );	// pSrc에게 pUser가 거부했다는걸 알림.
	}
}

// 파티듀얼 ----------------------------------------------------------------
// Src가 Dst에게 한판 붙자고 신청해왔다.
void CDPSrvr::OnDuelPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;
	
	CUser* pSrcUser	= g_UserMng.GetUser( dpidCache, dpidUser );	// 신청자 유저.
	CUser* pDstUser	= g_UserMng.GetUserByPlayerID( uidDst );	// 상대 유저
	if( IsValidObj( pDstUser ) && IsValidObj( pSrcUser ) )
	{
		if( pDstUser->IsMode( PVPCONFIRM_MODE ) )
		{
			pSrcUser->AddDefinedText( TID_PK_MODE_REJECT, "" );	// PVP거절 모드입니다
		}
		else
		{

			if( pSrcUser->IsPVPInspection( pDstUser, CUser::PVPInspection::Party) )
			{
				CParty* pSrcParty = g_PartyMng.GetParty( pSrcUser->m_idparty );		// 신청자의 파티꺼냄
				if( pSrcParty == NULL || pSrcParty->IsLeader( pSrcUser->m_idPlayer ) == FALSE )
				{
					pSrcUser->AddDefinedText( TID_PK_NO_IPARTYLEADER, "" );	// 파티장이 아닙니다
					return;
				}
				CParty *pDstParty = g_PartyMng.GetParty( pDstUser->m_idparty );		// 도전받는자의 파티꺼냄.
				if( pDstParty == NULL || pDstParty->IsLeader( pDstUser->m_idPlayer ) == FALSE )
				{
					pSrcUser->AddDefinedText( TID_PK_NO_UPARTYLEADER, "" );		// 상대방이 파티장이 아닙니다
					return;
				}
				pDstUser->AddDuelPartyRequest( uidSrc, uidDst );
			}
		}

	}
}

// 파티듀얼승락을 받음.  모든 양측 파티원들에게 듀얼이 시작됨을 알림.
void CDPSrvr::OnDuelPartyYes( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc, uidDst;
	ar >> uidSrc >> uidDst;
	
	CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );	
	CUser* pDst = g_UserMng.GetUserByPlayerID( uidDst );
	if( IsValidObj(pSrc) && IsValidObj(pDst) )
	{

		if( pSrc->IsPVPInspection( pDst, CUser::PVPInspection::Party) )
		{
			CParty* pSrcParty = g_PartyMng.GetParty( pSrc->m_idparty );		// 신청자의 파티꺼냄
			if( pSrcParty == NULL || pSrcParty->IsLeader( pSrc->m_idPlayer ) == FALSE )
			{
				Error( "CDPSrvr::OnDuelPartyYes : 신청자 파티 읽기 실패 %d %s", pSrc->m_idparty, pSrc->GetName() );
				return;
			}
			CParty *pDstParty = g_PartyMng.GetParty( pDst->m_idparty );		// 도전받는자의 파티꺼냄.
			//			if( pDstParty == NULL || pDstParty->IsMember( pDst->m_idPlayer ) == FALSE )
			if( pDstParty == NULL || pDstParty->IsLeader( pDst->m_idPlayer ) == FALSE )
			{
				Error( "CDPSrvr::OnDuelPartyYes : 상대 파티 읽기 실패 %d %s", pDst->m_idparty, pDst->GetName() );
				return;
			}

			pSrc->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pDst->m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
			pSrc->SetPosChanged( TRUE );	// UpdateRegionAttr
			pDst->SetPosChanged( TRUE );

			g_DPCoreClient.SendSetPartyDuel( pSrcParty->m_uPartyId, pDstParty->m_uPartyId, TRUE );

			pSrcParty->DoDuelPartyStart( pDstParty );		// 상대파티와 결투가 시작됐다는걸 세팅.
			pDstParty->DoDuelPartyStart( pSrcParty );		// 상대파티와 결투가 시작됐다는걸 세팅.
		}
	}
}

// pUser가 듀얼 신청을 거부했다. pSrc에게 알려야 한다.
void CDPSrvr::OnDuelPartyNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidSrc; //, uidDst;
	ar >> uidSrc;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		CUser* pSrc = g_UserMng.GetUserByPlayerID( uidSrc );
		if( IsValidObj(pSrc) )
			pSrc->AddDuelPartyNo( pUser->GetId() );		// pSrc에게 pUser가 거부했다는걸 알림.
	}
}

void CDPSrvr::OnMoverFocus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long uidPlayer;
	ar >> uidPlayer;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{
		CUser* pFocus = g_UserMng.GetUserByPlayerID( uidPlayer );
		if( IsValidObj(pFocus) )
			pUser->AddMoverFocus( pFocus );
	}
}

void CDPSrvr::OnSkillTaskBar(CAr & ar, CUser & pUser) {
	ar >> pUser.m_playTaskBar.m_aSlotQueue;
	// TODO: maybe sanitize a bit?
}

void CDPSrvr::OnModifyTaskBar(CAr & ar, CUser & pUser) {
	// Operation, BarName, unsigned int?, unsigned int, SHORTCUT?
	using Operation = CTaskbar::Operation;
	using BarName = CTaskbar::BarName;

	const auto [operation, onApplet] = ar.Extract<Operation, BarName>();
	
	std::span<SHORTCUT> shortcutBar;

	if (onApplet == BarName::Applet) {
		shortcutBar = std::span<SHORTCUT>(pUser.m_playTaskBar.m_aSlotApplet);
	} else {
		unsigned int barId; ar >> barId;
		if (barId >= pUser.m_playTaskBar.m_aSlotItem.size()) {
			return;
		}

		shortcutBar = std::span<SHORTCUT>(pUser.m_playTaskBar.m_aSlotItem[barId]);
	}

	unsigned int slot; ar >> slot;
	if (slot >= shortcutBar.size()) {
		return;
	}

	if (operation == Operation::Remove) {
		shortcutBar[slot].Empty();
	} else {
		// We do not really care about the validity of shortcuts. Nothing bad can
		// happen for the server. If the client sends a bad shortcut, it is their
		// problem.
		SHORTCUT replacement; ar >> replacement;
		SHORTCUT & destination = shortcutBar[slot];

		const bool canAdd = pUser.m_playTaskBar.CanAddShortcut(
			replacement.m_dwShortcut, destination
		);

		if (!canAdd) {
			pUser.AddDefinedText(TID_GAME_MAX_SHORTCUT_CHAT);
			return;
		}

		destination = replacement;
	}
}


void CDPSrvr::OnPlayerMoved( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsInvalidObj( pUser ) )
		return;

	if( pUser->GetIndex() == 0 )
	{
		WriteError( "PACKETTYPE_PLAYERMOVED" );
		return;
	}

	if( pUser->m_pActMover->IsDie() )
		return;

	D3DXVECTOR3 v, vd;
	float f;
	DWORD dwState, dwStateFlag, dwMotion	= 0;
	int nMotionEx	= 0, nLoop;
	DWORD dwMotionOption;
	__int64 nTickCount;

	ar >> v >> vd >> f;
	ar >> dwState >> dwStateFlag;
	ar >> dwMotion >> nMotionEx;
	ar >> nLoop >> dwMotionOption;
	ar >> nTickCount;

	if( pUser->m_pActMover->IsFly() )	return;		// 비행상태인데 일로 들어왔다면 취소시키자.

	D3DXVECTOR3 vDistance	= pUser->GetPos() - v;
	if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
	{
//		Error( "PACKETTYPE_PLAYERMOVED" );
		return;
	}

	int delay	= (int)( (float)g_TickCount.GetOffset( nTickCount ) / 66.6667f );

	if( delay >= MAX_CORR_SIZE_45 ) {
		pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
	}
	else 	
	{
		if( pUser->m_pActMover->IsStateFlag( OBJSTAF_FLY ) || dwStateFlag & OBJSTAF_FLY ) 
		{
			pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
		}
		else 
		{
			pUser->m_pActMover->DefaultSet();

			int nRemnant	= (int)( MAX_CORR_SIZE_45 - delay );
			D3DXVECTOR3 d	= v - pUser->GetPos();
			float fLength	= D3DXVec3Length( &d );
			float fSpeed	= ( fLength / (float)nRemnant ) / 4 + 0.020f;
			
			pUser->m_fCrrSpd	= pUser->m_pActMover->IsStateFlag( OBJSTAF_WALK )? fSpeed*4.0f: fSpeed;

			pUser->m_CorrAction.v	= v;
			pUser->m_CorrAction.vd		= vd;
			pUser->m_CorrAction.f	= f;
			pUser->m_CorrAction.dwState	= dwState;
			pUser->m_CorrAction.dwStateFlag	= dwStateFlag;
			pUser->m_CorrAction.dwMotion	= dwMotion;
			pUser->m_CorrAction.nMotionEx	= nMotionEx;
			pUser->m_CorrAction.nLoop	= nLoop;
			pUser->m_CorrAction.dwMotionOption	= dwMotionOption;
			pUser->m_CorrAction.fValid	= TRUE;

			pUser->SetDestPos( v, ( dwState & OBJSTA_MOVE_ALL ) != OBJSTA_BMOVE );
		}
	}

	g_UserMng.AddMoverMoved
		( pUser, v, vd, f, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
}

void CDPSrvr::OnPlayerBehavior( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
//	TRACE( "OnPlayerBehavior()\n" );
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsInvalidObj( pUser ) )
		return;

	if( pUser->GetIndex() == 0 )
	{
		WriteError( "PACKETTYPE_PLAYERBEHAVIOR" );
		return;
	}

	if( pUser->m_pActMover->IsDie() )
			return;
	
	if( pUser->m_pActMover->IsState( OBJSTA_ATK_CASTING1 | OBJSTA_ATK_CASTING2 | OBJSTA_ATK_MAGIC1 | OBJSTA_ATK_RANGE1 | OBJSTA_ATK_MELEESKILL | OBJSTA_ATK_RANGESKILL ) )
		return;

	D3DXVECTOR3 v, vd;
	float f;
	DWORD dwState, dwStateFlag, dwMotion	= 0;
	int nMotionEx	= 0, nLoop;
	DWORD dwMotionOption;
	__int64 nTickCount;

	ar >> v >> vd >> f;
	ar >> dwState >> dwStateFlag;
	ar >> dwMotion >> nMotionEx;

	ar >> nLoop >> dwMotionOption;
	ar >> nTickCount;

	if( pUser->m_pActMover->IsFly() )
		return;
	
	int delay	= (int)( (float)g_TickCount.GetOffset( nTickCount ) / 66.6667f );


	if( !pUser->IsEmptyDest() && pUser->m_CorrAction.fValid == FALSE ) 
		pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
	else 
		pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );

	g_UserMng.AddMoverBehavior( pUser, v, vd, f, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
}

void CDPSrvr::OnPlayerMoved2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERMOVED2" );
			return;
		}

		if( pUser->m_pActMover->IsDie() )
			return;

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		DWORD dwState, dwStateFlag, dwMotion	= 0;
		int nMotionEx	= 0, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;
		BYTE nFrame;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;

		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;
		ar >> nFrame;
		int delay	= (int)( (float)g_TickCount.GetOffset( nTickCount ) / 66.6667f );

		if( pUser->m_pActMover->IsFly() )
		{
			// 1
			// 2
			// 3
	//		pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
//			if( false/*nTickCount < 0*/ ) {
			if( delay >= nFrame ) {
				pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
			}
			else {
				pUser->m_pActMover->DefaultSet();

				pUser->m_CorrAction.v	= v;
				pUser->m_CorrAction.vd		= vd;
				pUser->m_CorrAction.f	= f;
				pUser->m_CorrAction.dwState	= dwState;
				pUser->m_CorrAction.dwStateFlag	= dwStateFlag;
				pUser->m_CorrAction.dwMotion	= dwMotion;
				pUser->m_CorrAction.nMotionEx	= nMotionEx;
				pUser->m_CorrAction.nLoop	= nLoop;
				pUser->m_CorrAction.dwMotionOption	= dwMotionOption;
				pUser->m_CorrAction.fAngleX	= fAngleX;
				pUser->m_CorrAction.fAccPower	= fAccPower;
				pUser->m_CorrAction.fTurnAngle	= fTurnAngle;
				pUser->m_CorrAction.fValid	= TRUE;

				int nRemnant	= (int)nFrame - delay;
				pUser->m_uRemnantCorrFrm	= nRemnant;
				pUser->SetDestPos( v );
			}

			g_UserMng.AddMoverMoved2
				( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount, nFrame );
		} 
	}
}

void CDPSrvr::OnPlayerBehavior2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERBEHAVIOR2" );
			return;
		}

		if( pUser->m_pActMover->IsDie() )
			return;

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		DWORD dwState, dwStateFlag, dwMotion;
		int nMotionEx, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;
		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;
#ifdef _DEBUG
		if( dwStateFlag & OBJSTAF_ACC )	
		{
			int a = 0;
		}
#endif

		if( pUser->m_pActMover->IsFly() == FALSE )	return;	// 비행상태가 아닌데 이리로 들어왔다면 취소.

		// 1
		// 2
		// 3
		pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );
		// 4
		g_UserMng.AddMoverBehavior2
			( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
	}
}

// 클라로부터 올라온 각도를 세팅.
// 보통 비행중 사용.
void CDPSrvr::OnPlayerAngle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERANGLE" );
			return;
		}

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		__int64 nTickCount;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> nTickCount;

		int delay	= (int)( (float)g_TickCount.GetOffset( nTickCount ) / 66.6667f );

		if( pUser->m_pActMover->IsFly() )
		{
			if( delay >= MAX_CORR_SIZE_150 )
			{
				if( pUser->m_CorrAction.fValid &&  !pUser->m_CorrAction.fHalf )
				{
						pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle,
						pUser->m_CorrAction.dwState, pUser->m_CorrAction.dwStateFlag, pUser->m_CorrAction.dwMotion, pUser->m_CorrAction.nMotionEx,
						pUser->m_CorrAction.nLoop, pUser->m_CorrAction.dwMotionOption );
				}
				else
				{
					pUser->HalfForceSet( v, vd, f, fAngleX, fAccPower, fTurnAngle );
				}
			}
			else
			{
				pUser->m_pActMover->DefaultSet();

				pUser->m_CorrAction.v	= v;
				pUser->m_CorrAction.vd		= vd;
				pUser->m_CorrAction.f	= f;
				pUser->m_CorrAction.fAngleX	= fAngleX;
				pUser->m_CorrAction.fAccPower	= fAccPower;
				pUser->m_CorrAction.fTurnAngle	= fTurnAngle;
				pUser->m_CorrAction.fHalf	= !pUser->m_CorrAction.fValid;
				pUser->m_CorrAction.fValid	= TRUE;

				int nRemnant	= MAX_CORR_SIZE_150 - delay;
				pUser->m_uRemnantCorrFrm	= nRemnant;
				pUser->SetDestPos( v );
			}
			g_UserMng.AddMoverAngle( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, nTickCount );
		}
	}
}

void CDPSrvr::OnPlayerSetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		OBJID objid;
		float fRange;
		ar >> objid >> fRange;

		CCtrl* pCtrl;

		pCtrl	= prj.GetCtrl( objid );
		if( IsValidObj( pCtrl ) )
		{
#ifdef __TRAFIC_1222
			if( pUser->GetDestId() == objid )
				return;
#endif	// __TRAFIC_1222
			pUser->m_pActMover->DefaultSet();
			pUser->BehaviorActionForceSet();
			pUser->SetDestObj( objid, fRange );

			g_UserMng.AddMoverSetDestObj( (CMover*)pUser, objid, fRange );
		}

	}
}


// raider_test 없는 아이템을 사용했다고 하면?
void CDPSrvr::OnDoUseItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwData;
	OBJID objid;
	int	  nPart;

	ar >> dwData >> objid >> nPart;
	if( nPart >= MAX_HUMAN_PARTS )	
		return;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	WORD nId = HIWORD( dwData );
	if( nPart > 0 )
	{
		CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nId );
		if( !IsUsableItem( pItemElem ) )
			return;
		if( pUser->m_Inventory.IsEquip( nId ) )
		{
			if( pItemElem != pUser->m_Inventory.GetEquip( nPart ) )
				return;
		}
	}
#ifdef __HACK_1023
	CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nId );
	if( IsUsableItem( pItemElem ) )
	{
		ItemProp* pItemProp	= pItemElem->GetProp();
		if( pItemProp && pItemProp->dwParts == PARTS_RIDE )
		{
			if( !pUser->m_Inventory.IsEquip( nId ) )
			{
				FLOAT fVal;
				ar >> fVal;
				if( fVal != pItemProp->fFlightSpeed )
				{
					pUser->AddDefinedText( TID_GAME_MODIFY_FLIGHT_SPEED );
					return;
				}
			}
		}
	}
#endif	// __HACK_1023

	pUser->OnDoUseItem( dwData, objid, nPart );
}

void CDPSrvr::OnPlayerCorr( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
//	TRACE( "OnPlayerCorr\n" );
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERCORR" );
			return;
		}

		D3DXVECTOR3 v, vd;
		float f;
		DWORD dwState, dwStateFlag, dwMotion;
		int nMotionEx, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;

		ar >> v >> vd >> f;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;
		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;

		if( pUser->m_pActMover->IsFly() == FALSE )
		{
			D3DXVECTOR3 vDistance	= pUser->GetPos() - v;
			if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
			{
//				Error( "PACKETTYPE_PLAYERCORR" );
				return;
			}
			// 1
			// 2
			// 3
			pUser->ClearDest();
			pUser->ClearDestAngle();
			memset( &pUser->m_CorrAction, 0, sizeof(CORR_ACTION) );

			pUser->ActionForceSet( v, vd, f, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );

			// 4
			g_UserMng.AddMoverCorr
				( pUser, v, vd, f, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
		}
	}
}

void CDPSrvr::OnPlayerCorr2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
//	TRACE( "OnPlayerCorr2()\n" );
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_PLAYERCORR2" );
			return;
		}

		D3DXVECTOR3 v, vd;
		float f, fAngleX, fAccPower, fTurnAngle;
		DWORD dwState, dwStateFlag, dwMotion;
		int nMotionEx, nLoop;
		DWORD dwMotionOption;
		__int64 nTickCount;

		ar >> v >> vd >> f >> fAngleX >> fAccPower >> fTurnAngle;
		ar >> dwState >> dwStateFlag;
		ar >> dwMotion >> nMotionEx;

		ar >> nLoop >> dwMotionOption;
		ar >> nTickCount;

		if( pUser->m_pActMover->IsFly() )
		{
			// 1
			// 2
			// 3
			pUser->ClearDest();
			pUser->ClearDestAngle();
			memset( &pUser->m_CorrAction, 0, sizeof(CORR_ACTION) );

			pUser->ActionForceSet2( v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion, nMotionEx, nLoop, dwMotionOption );

			// 4
			g_UserMng.AddMoverCorr2
				( pUser, v, vd, f, fAngleX, fAccPower, fTurnAngle, dwState, dwStateFlag, dwMotion,  nMotionEx, nLoop, dwMotionOption, nTickCount );
		}
	}
}

void CDPSrvr::OnOpenShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		OBJID objid;
		ar >> objid;
		
		CMover* pVendor	= prj.GetMover( objid );
		if( IsValidObj( pVendor ) && pUser->m_vtInfo.GetOther() == NULL )
		{
			if( pVendor->IsNPC() == FALSE )		// 대상이 NPC가 아니면?
				return;

			if( pUser->IsChaotic() )
			{
				CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
				if( !Propensity.nShop )
					return;
			}

			if( pVendor->IsVendorNPC() == FALSE )
			{
				WriteError( "VENDOR//%s", pVendor->GetName() );
				return;
			}

			if( pUser->m_bBank )
			{
				pUser->AddDefinedText( TID_GAME_TRADELIMITNPC, "" );
				return;
			}

			if( pUser->m_vtInfo.VendorIsVendor() )
				return;

#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction == FALSE )
				return;
#endif // __S_SERVER_UNIFY

			pUser->m_vtInfo.SetOther( pVendor );
			pUser->AddOpenShopWnd( pVendor );
		}
	}
}

void CDPSrvr::OnCloseShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CMover* pMover = pUser->m_vtInfo.GetOther();
		if( IsValidObj( pMover ) && pMover->IsNPC() )
			pUser->m_vtInfo.SetOther( NULL );
	}
}

void CDPSrvr::OnBuyItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	auto [cTab, nId, nNum, dwItemId] = ar.Extract<CHAR, BYTE, short, DWORD>();
	if (cTab >= MAX_VENDOR_INVENTORY_TAB || nNum < 1) return;

	CUser * pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj(pUser)) return;

	CMover * pVendor = pUser->m_vtInfo.GetOther();
	if (!pVendor) return;

	CHARACTER * lpChar = prj.GetCharacter( pVendor->m_szCharacterKey );
	if (!lpChar) return;

	if (lpChar->m_vendor.m_type != CVendor::Type::Penya)	return;
	if (!pVendor->IsNPC()) return;
	if (!CNpcChecker::GetInstance()->IsCloseNpc<MMI_TRADE>(pUser)) return;

	CItemElem * pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId(nId);
	if (!pItemElem) return;

	if (dwItemId != pItemElem->m_dwItemId) return;

	nNum = std::min(nNum, pItemElem->m_nItemNum);

#ifdef __OCCUPATION_SHOPITEM
	if (CTax::GetInstance()->IsOccupationShopItem(dwItemId) && !CTax::GetInstance()->IsOccupationGuildMember(pUser)) {
		pUser->AddDefinedText(TID_GAME_SECRETROOM_STORE_BUY);
		return;
	}
#endif // __OCCUPATION_SHOPITEM
		
	int nCost = (int)pItemElem->GetCost();
#ifdef __SHOP_COST_RATE
	nCost = (int)( prj.m_EventLua.GetShopBuyFactor() * nCost );
#endif // __SHOP_COST_RATE

	if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN) nCost = PERIN_VALUE;
	nCost = std::max(nCost, 1);
	


	std::int64_t nTax = 0;
	if (CTax::GetInstance()->IsApplyTaxRate(pUser, pItemElem))
		nTax = (int)(nCost * CTax::GetInstance()->GetPurchaseTaxRate(pUser));
	nCost += nTax;
	nTax *= nNum;
	
	if (nCost <= 0) return;

	std::int64_t totalCost = static_cast<int64_t>(nCost) * nNum;

	if (totalCost > pUser->GetTotalGold()) {
		pUser->AddDefinedText(TID_GAME_LACKMONEY, "");
		return;
	}

#ifdef __PERIN_BUY_BUG
	if( pUser->m_dwLastBuyItemTick + 500 > GetTickCount() ) // 아이템 구입시도 후 0.5초이내에 다시 구입시도한 경우
	{
		Error( "CDPSrvr::OnBuyItem : __PERIN_BUY_BUG -> [PlayerId:%07d(%s)], [LastTick:%d], [CurTick:%d], [LastTryItem:%d], [Packet:%d,%d,%d,%d]",
				pUser->m_idPlayer, pUser->GetName(), pUser->m_dwLastBuyItemTick, GetTickCount(), pUser->m_dwLastTryBuyItem, cTab, nId, nNum, dwItemId );
		//g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
		return;
	}
	pUser->m_dwLastTryBuyItem = pItemElem->m_dwItemId;
	pUser->m_dwLastBuyItemTick = GetTickCount();
#endif // __PERIN_BUY_BUG
	
	CItemElem itemElem;
	itemElem	= *pItemElem;
	itemElem.m_nItemNum	      = nNum;
	itemElem.SetSerialNumber();

	const bool created = pUser->CreateItem(&itemElem);
	
	if (!created) {
		pUser->AddDefinedText(TID_GAME_LACKSPACE, "");
		return;
	}

	constexpr int two_millions = 2000000000;

	LogItemInfo aLogItem;
	aLogItem.Action = "B";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = pVendor->GetName();
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = totalCost % two_millions;
	aLogItem.Gold_1 = totalCost / two_millions;

	pItemElem->SetSerialNumber( itemElem.GetSerialNumber() );
	OnLogItem( aLogItem, pItemElem, nNum );		// why do not pass &itemElem as argument?
	pItemElem->SetSerialNumber( 0 );

	if (pItemElem->IsPerin()) {
		// RemoveTotalGold prioritizes Perin over gold. If we are buying perins, we are
		// reversing the order
		while (pUser->GetGold() >= PERIN_VALUE && totalCost >= PERIN_VALUE) {
			pUser->AddGold(-PERIN_VALUE, TRUE);
			totalCost -= PERIN_VALUE;
		}

		pUser->RemoveTotalGold(totalCost);

	} else {
		pUser->RemoveTotalGold(totalCost);
	}
	
	
	if (nTax) {
		// Add taxes by steps of 2 millions max because ints
		while (nTax > two_millions) {
			CTax::GetInstance()->AddTax(CTax::GetInstance()->GetContinent(pUser), two_millions, TAX_PURCHASE);
			nTax -= two_millions;
		}

		CTax::GetInstance()->AddTax(CTax::GetInstance()->GetContinent(pUser), nTax, TAX_PURCHASE);
	}
}

// 칩으로 아이템 구매
void CDPSrvr::OnBuyChipItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	auto [cTab, nId, nNum, dwItemId] = ar.Extract<CHAR, BYTE, short, DWORD>();
	if( cTab >= MAX_VENDOR_INVENTORY_TAB || nNum < 1 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && pUser->m_vtInfo.GetOther() )
	{
		CMover* pVendor = pUser->m_vtInfo.GetOther();
		LPCHARACTER lpChar = prj.GetCharacter( pVendor->m_szCharacterKey );
		if (!lpChar) return;

		if(lpChar->m_vendor.m_type != CVendor::Type::RedChip)	// 1 - 칩 상인
			return;

		if( pVendor->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
			return;

		if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_TRADE>(pUser) )
			return;

		CItemElem* pItemElem = pVendor->m_ShopInventory[cTab]->GetAtId( nId );
		if( NULL == pItemElem )
			return;

		if( dwItemId != pItemElem->m_dwItemId )
			return;

#ifdef __OCCUPATION_SHOPITEM
		if( CTax::GetInstance()->IsOccupationShopItem( dwItemId ) && !CTax::GetInstance()->IsOccupationGuildMember( pUser ) )
		{
			pUser->AddDefinedText( TID_GAME_SECRETROOM_STORE_BUY );
			return;
		}
#endif // __OCCUPATION_SHOPITEM
		if( nNum > pItemElem->m_nItemNum )
			nNum = pItemElem->m_nItemNum;
		
		// 소지한 칩의 개수가 부족할 때
		if( pUser->m_Inventory.GetAtItemNum( II_CHP_RED ) < (int)( pItemElem->GetChipCost() * nNum ) )
		{
			// 칩 개수 부족 텍스트 출력
			pUser->AddDefinedText( TID_GAME_LACKCHIP );
			return;
		}

		// 인벤토리가 꽉 찼을 때 
		if( pUser->m_Inventory.IsFull( pItemElem, pItemElem->GetProp(), nNum ) )
		{
			int nChipNum = pItemElem->GetChipCost() * nNum;
			ItemProp* pChipItemProp = prj.GetItemProp( II_CHP_RED );
			if( nChipNum < (int)( pChipItemProp->dwPackMax ) )
			{
				CItemElem* pTempElem;
				int bEmpty = FALSE;
				for( int i = 0; i < pUser->m_Inventory.GetSize(); i++ )
				{
					pTempElem = pUser->m_Inventory.GetAtId( i );
					if( IsUsableItem(pTempElem) && pChipItemProp->dwID == pTempElem->m_dwItemId )
					{
						if( pTempElem->m_nItemNum <= nChipNum )
							bEmpty = TRUE;
						break;
					}
				}
				if( !bEmpty )
				{
					// 인벤이 꽉찼다는 텍스트 출력
					pUser->AddDefinedText( TID_GAME_LACKSPACE, "" );
					return;
				}
			}
		}
		
		// 구매 가격 만큼의 칩 삭제
		DWORD dwChipCost = pItemElem->GetChipCost() * nNum;
		if( dwChipCost > 0x7fff )
		{
			for( ; dwChipCost > 0x7fff; )
			{
				pUser->RemoveItemA( II_CHP_RED, 0x7fff );
				dwChipCost -= 0x7fff;
			}
		}
		pUser->RemoveItemA( II_CHP_RED, (short)dwChipCost );
				
		// 구매 아이템 생성
		CItemElem itemElem;
		itemElem	= *pItemElem;
		itemElem.m_nItemNum = nNum;
		itemElem.SetSerialNumber();
		if( pUser->CreateItem( &itemElem ) )
		{
			// 로그 남김
			LogItemInfo aLogItem;
			aLogItem.Action = "B";
			aLogItem.SendName = pUser->GetName();
			CString strTemp;
			strTemp.Format( "%s_C", pVendor->GetName() );
			aLogItem.RecvName = (LPCTSTR)strTemp;
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetItemNum( II_CHP_RED ) + itemElem.GetChipCost() * nNum;
			aLogItem.Gold2 = pUser->GetItemNum( II_CHP_RED );
			aLogItem.Gold_1 = (DWORD)( (-1) * (int)( (itemElem.GetChipCost() * nNum) ) );
			OnLogItem( aLogItem, &itemElem, nNum );
		}
		else
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "B";
			aLogItem.SendName = pUser->GetName();
			CString strTemp;
			strTemp.Format( "%s_CF", pVendor->GetName() );
			aLogItem.RecvName = (LPCTSTR)strTemp;
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetItemNum( II_CHP_RED ) + itemElem.GetChipCost() * nNum;
			aLogItem.Gold2 = pUser->GetItemNum( II_CHP_RED );
			aLogItem.Gold_1 = (DWORD)( (-1) * (int)( (itemElem.GetChipCost() * nNum) ) );
			OnLogItem( aLogItem, &itemElem, nNum );
		}
	}
}

//NPC에게 파는 경우
void CDPSrvr::OnSellItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nId;
	short nNum;

	ar >> nId >> nNum;
	if( nNum < 1 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && IsValidObj( pUser->m_vtInfo.GetOther() ) )
	{
		if( pUser->m_vtInfo.GetOther()->IsNPC() == FALSE )		// 판매할 대상이 NPC가 아니면?
			return;

		if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_TRADE>(pUser) )
			return;

		CItemElem* pItemElem = pUser->m_Inventory.GetAtId( nId );		
		if( !IsUsableItem( pItemElem ) )
			return;

		ItemProp* pProp	= pItemElem->GetProp();
		if( pProp->dwItemKind3 == IK3_EVENTMAIN )
			return;
		
		if( pItemElem->IsQuest() )
			return;

		if( pUser->m_Inventory.IsEquip( nId ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
			return;
		}
		if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
			return;
		if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN )
			return;

		if( nNum > pItemElem->m_nItemNum )
			nNum = pItemElem->m_nItemNum;
		if( nNum < 1 )
			nNum = 1;

		int nGold = ( pItemElem->GetCost() / 4 );
#ifdef __SHOP_COST_RATE
		nGold = (int)( prj.m_EventLua.GetShopSellFactor() * nGold );
#endif // __SHOP_COST_RATE
		if( nGold == 0 )
			nGold = 1;
		int nTax = 0;
		if( CTax::GetInstance()->IsApplyTaxRate( pUser, pItemElem ) )
			nTax = (int)( nGold * CTax::GetInstance()->GetSalesTaxRate( pUser ) );
		nGold -= nTax;
		nGold *= nNum;
		nTax  *= nNum;
		float fTmpGold = (float)( pUser->GetGold() + nGold );
		if( fTmpGold >= 2100000000 )
		{
			return;
		}
//		if( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == II_RID_RID_BOR_LADOLF )
//			return;
		if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
			return;

		if( pUser->IsUsing( pItemElem ) )
		{
			pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
			return;
		}

		LogItemInfo aLogItem;
		aLogItem.Action = "S";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = pUser->m_vtInfo.GetOther()->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold() + nGold;
		OnLogItem( aLogItem, pItemElem, nNum );
		int nCost	= (int)pItemElem->GetCost() / 4;


		if( nCost < 1 )	
			nCost = 1;

		if( nGold < 1 )
			nGold = 1;
		
		pUser->AddGold( nGold );
		if( nTax )
			CTax::GetInstance()->AddTax( CTax::GetInstance()->GetContinent( pUser ), nTax, TAX_SALES );
		pUser->RemoveItem( nId, nNum );
	}
}

// 패스워드 변경창을 띄울것인지 패스워드 확인창을 띄을것인지를 알려준다
void CDPSrvr::OnOpenBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD	dwId;
	ar >> dwId;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc<MMI_BANKING>(pUser) )
			return;
		if( pUser->IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
			if( !Propensity.nBank )
				return;
		}
		if( 0 == strcmp( pUser->m_szBankPass, "0000") )
		{
			pUser->SendSnapshotNoTarget<
				SNAPSHOTTYPE_BANK, Subsnapshot::Bank, OBJID
			>(Subsnapshot::Bank::InitialRequirePassword, dwId);
		} else {
			pUser->SendSnapshotNoTarget<
				SNAPSHOTTYPE_BANK, Subsnapshot::Bank, OBJID
			>(Subsnapshot::Bank::AskCurrentPassword, dwId);
		}
	}
}

void CDPSrvr::OnOpenGuildBankWnd(CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;
		if( pUser->m_bBank )				// 창고를 열고 있으면?
			return;
#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction == FALSE )
			return;
#endif // __S_SERVER_UNIFY

		pUser->AddGuildBankWindow( 0 );
		pUser->m_bGuildBank = TRUE;
	}
}

void CDPSrvr::OnCloseBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
		pUser->m_bBank = FALSE;
		pUser->m_bInstantBank	= FALSE;
	}
}

void CDPSrvr::OnDoUseSkillPoint( CAr & ar, CUser & pUser ) {
	const auto nope = [&]() {
		pUser.AddDefinedText(TID_RESKILLPOINT_ERROR);
		return;
	};

	MoverSkills wanted; ar >> wanted;

	if (wanted.size() != pUser.m_jobSkills.size()) {
		return nope();
	}

	int nChangePoint = 0;

	for (size_t i = 0; i != pUser.m_jobSkills.size(); ++i) {
		const SKILL & wantedSkill = wanted[i];
		const SKILL & currentSkill = pUser.m_jobSkills[i];

		if (wantedSkill.dwSkill != currentSkill.dwSkill) return nope();
		if (wantedSkill.dwLevel < currentSkill.dwLevel) return nope();

		const ItemProp * skillProp = wantedSkill.GetProp();
		if (!skillProp) return nope();
		if (wantedSkill.dwLevel > skillProp->dwExpertMax) return nope();

		const int costPerPoint = prj.GetSkillPoint(skillProp);
		if (costPerPoint != 0) {
			nChangePoint += costPerPoint * (wantedSkill.dwLevel - currentSkill.dwLevel);
		} else {
			if (wantedSkill.dwLevel != currentSkill.dwLevel) return nope();
		}
	}

	// TODO: check consistency of skills wrt the required previous skills


	if (nChangePoint == 0) return;
	if (pUser.m_nSkillPoint < nChangePoint) return nope();
		
	// 스킬별 재분배 하기 // 스킬 레벨 셋팅
	pUser.m_nSkillPoint -= nChangePoint;

	for (size_t i = 0; i != pUser.m_jobSkills.size(); ++i) {
		const SKILL & wantedSkill = wanted[i];
		SKILL & currentSkill = pUser.m_jobSkills[i];

		const ItemProp * skillProp = wantedSkill.GetProp();
		int nPoint = (wantedSkill.dwLevel - currentSkill.dwLevel) * prj.GetSkillPoint(skillProp);
		if( 0 < nPoint ) // 스킬 부여한것만 로그에 남기자
			g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_USE, nPoint, &pUser, &wantedSkill);

		currentSkill.dwLevel = wantedSkill.dwLevel;
	}	

	g_UserMng.AddCreateSfxObj(&pUser, XI_SYS_EXCHAN01, pUser.GetPos().x, pUser.GetPos().y, pUser.GetPos().z);
	pUser.AddDoUseSkillPoint(pUser.m_jobSkills, pUser.m_nSkillPoint);
#ifdef __S_NEW_SKILL_2
	g_dpDBClient.SaveSkill( pUser );
#endif // __S_NEW_SKILL_2
}

void CDPSrvr::OnCloseGuildBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) )
	{
		pUser->m_bGuildBank = FALSE;
	}
}

void CDPSrvr::OnBankToBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nFlag, nPutSlot, nSlot, nId;
	short nItemNum;
	DWORD dwGold;
	ar >> nFlag >> nPutSlot >> nSlot;

	if( nPutSlot >= 3 || nSlot >= 3 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_idPlayerBank[nPutSlot] != 0 && pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() )
		{
			if( nFlag == 1 )	// Item
			{
				ar >> nId >> nItemNum;
				CItemElem* pItemElem	= pUser->m_Bank[nPutSlot].GetAtId( nId );
				if( pItemElem == NULL )
					return;

				if( nItemNum > pItemElem->m_nItemNum )
					nItemNum	= pItemElem->m_nItemNum;
				if( nItemNum < 1 )
					nItemNum	= 1;

				if( MAX_BANK > pUser->m_Bank[nSlot].GetCount() )
				{
					CItemElem itemElem;
					itemElem	= *pItemElem;
					itemElem.m_nItemNum		= nItemNum;
					pUser->AddPutItemBank( nSlot, &itemElem );
					pUser->m_Bank[nSlot].Add( &itemElem );
					pUser->UpdateItemBank( nPutSlot, nId, pItemElem->m_nItemNum - nItemNum );		// 은행에 빼기및 전송

					LogItemInfo aLogItem;
					aLogItem.Action = "A";
					aLogItem.SendName = pUser->GetName();
					aLogItem.RecvName = "BANK";
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
					aLogItem.nSlot = nSlot;
					aLogItem.nSlot1 = nPutSlot;
					OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
				}
				else
				{
					// 꽉차서 넣을수 가 없음. 메세지 처리
					pUser->AddBankIsFull();
				}
			}
			else		// Gold
			{
				 ar >> dwGold;
				
				 if( dwGold > pUser->m_dwGoldBank[nPutSlot] )
					 dwGold = pUser->m_dwGoldBank[nPutSlot];
				 
				 int nGold = dwGold;
				 if( CanAdd( pUser->m_dwGoldBank[nSlot], nGold ) )
				 {
					 pUser->m_dwGoldBank[nSlot]	+= nGold;
					 pUser->m_dwGoldBank[nPutSlot] -= nGold;
					 
					 pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );
					 pUser->AddPutGoldBank( nPutSlot, pUser->GetGold(), pUser->m_dwGoldBank[nPutSlot] );

					 LogItemInfo aLogItem;
					 aLogItem.Action = "A";
					 aLogItem.SendName = pUser->GetName();
					 aLogItem.RecvName = "BANK";
					 aLogItem.WorldId = pUser->GetWorld()->GetID();
					 aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					 //aLogItem.ItemName = "SEED";
					 _stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
					 aLogItem.itemNumber = nGold;
					 aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
					 aLogItem.nSlot = nSlot;
					 aLogItem.nSlot1 = nPutSlot;
					 OnLogItem( aLogItem );
				 }	
			}
		}
	}
}
void CDPSrvr::OnPutItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nSlot, nId;
	short nItemNum;

	ar >> nSlot >> nId >> nItemNum;
	if( nSlot >= 3 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_BANKING>(pUser) )
				return;
		}

		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( nId );
			if( !IsUsableItem( pItemElem ) )
				return;

			if( nSlot != pUser->m_nSlot )
			{
				if( pItemElem->IsQuest() )
					return;
			}
				
			if( pItemElem->IsBinds() )
				return;

			ItemProp* pProp	= pItemElem->GetProp();
			if( !pProp )
				return;

			if( pUser->IsUsing( pItemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
				return;
			}
			
			if( pProp->dwItemKind3 == IK3_EVENTMAIN ||
				pProp->dwItemKind3 == IK3_LINK ||
				( pProp->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild != 0 ) )
				return;

//			if( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == II_RID_RID_BOR_LADOLF )
//				return;
			if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				return;


			if( pUser->m_Inventory.IsEquip( nId ) )
			{
				pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
				return;
			}
			
			if( nItemNum > pItemElem->m_nItemNum )
				nItemNum	= pItemElem->m_nItemNum;
			if( nItemNum < 1 )
				nItemNum	= 1;

//			if( MAX_BANK > pUser->m_Bank[nSlot].GetCount() )
			if( pUser->m_Bank[nSlot].IsFull( pItemElem, pProp, nItemNum ) == FALSE )
			{
				CItemElem itemElem;
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;
				pUser->AddPutItemBank( nSlot, &itemElem );
				pUser->m_Bank[nSlot].Add( &itemElem );

				LogItemInfo aLogItem;
				aLogItem.Action = "P";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "BANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot1 = nSlot;
				OnLogItem( aLogItem, pItemElem, nItemNum );

				pUser->RemoveItem( nId, nItemNum );
			}
			else
			{
				// 꽉차서 넣을수 가 없음. 메세지 처리
				pUser->AddBankIsFull();
			}
		}
	}
}

void CDPSrvr::OnPutItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	BYTE nId, mode;
	DWORD nItemNum;

	ar >> nId >> nItemNum >> mode;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
		if( !pUser->GetWorld() || !GuildHouseMng->IsGuildHouse( pUser->GetWorld()->GetID() ) )
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_GUILDBANKING>(pUser) )
				return;

		if( mode == 0 ) // 길드창고에는 Gold를 넣을수 없습니다.
			return;

		CItemElem* pItemElem = pUser->m_Inventory.GetAtId( nId );		
		if( !IsUsableItem( pItemElem ) )
			return;

		if( pItemElem->IsQuest() )
			return;
		
		if( pItemElem->IsBinds() )
			return;

		if( pUser->IsUsing( pItemElem ) )
		{
			pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
			return;
		}

		ItemProp* pProp	= pItemElem->GetProp();
		if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
			return;

		if( pUser->m_Inventory.IsEquip( nId ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPTRADE, "" );
			return;
		}

		nItemNum = std::clamp<short>(static_cast<short>(nItemNum), 1, pItemElem->m_nItemNum);
		
		//	GUILD_BANK_STR 'S1','000000','01' 
		//	GUILD BANK 전체 불러오기 ex ) GUILD_BANK_STR 'S1',@im_idGuild,@iserverindex GUILD_BANK_STR 'S1','000000','01'  
		//	GUILD BANK 저장하기 ex ) GUILD_BANK_STR 'U1',@im_idGuild,@iserverindex,@im_nGoldGuild,@im_apIndex,@im_dwObjIndex,@im_GuildBank GUILD_BANK_STR 'U1','000001','01',0,'$','$','$' 			

		CGuild*	pGuild = pUser->GetGuild();
		if( pGuild )
		{
			CItemElem itemElem;
			itemElem	= *pItemElem;
			itemElem.m_nItemNum	= (short)nItemNum;
			if ( pGuild->m_GuildBank.Add( &itemElem ) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "W";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDBANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				OnLogItem( aLogItem, &itemElem, nItemNum );

				pUser->RemoveItem( (BYTE)( nId ), (short)( nItemNum ) );
				UpdateGuildBank( pGuild, GUILD_PUT_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, (short)( nItemNum ) );
				pUser->AddPutItemGuildBank( &itemElem );
				g_UserMng.AddPutItemElem( pUser, &itemElem );
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_GUILDBANKFULL, "" );		// 길드창고가 꽉찼시유~
			}
		}
	}
}

void CDPSrvr::OnGetItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	BYTE nId, mode;
	DWORD dwItemNum;

	ar >> nId >> dwItemNum >> mode;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );	
	if( IsValidObj( pUser ) )
	{
		if( !pUser->GetWorld() || !GuildHouseMng->IsGuildHouse( pUser->GetWorld()->GetID() ) )
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_GUILDBANKING>(pUser) )
				return;
		
		if (mode == 0) // Gold를 길드창고에서 빼낼때
		{
			int nGold = (int)dwItemNum;
			if( nGold <= 0 || CanAdd( pUser->GetGold(), nGold ) == FALSE )
				return;

			CGuild*	pGuild = pUser->GetGuild();
			if (pGuild && pGuild->IsGetPenya(pUser->m_idPlayer))
			{
				if( (DWORD)nGold > pGuild->m_nGoldGuild )
					return;

				pUser->AddGold( nGold, FALSE );
				pGuild->m_nGoldGuild -= nGold;
				pUser->AddGetGoldGuildBank( nGold, 0, pUser->m_idPlayer, 0 );	// 0은 업데이트 시킨 클라에게 
				pGuild->DecrementMemberContribution( pUser->m_idPlayer, nGold, 0 );
				UpdateGuildBank(pGuild, GUILD_GET_PENYA, 1, pUser->m_idPlayer, NULL, nGold );

				LogItemInfo aLogItem;
				aLogItem.Action = "Y";
				aLogItem.SendName = "GUILDBANK";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold() - nGold;
				aLogItem.Gold2 = pUser->GetGold();
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				OnLogItem( aLogItem );

				for (CUser * pUsertmp : AllMembers(*pGuild)) {
					if( IsValidObj( pUsertmp ) && pUsertmp != pUser )
					{
						pUsertmp->AddGetGoldGuildBank( nGold, 2, pUser->m_idPlayer, 0 );	// 2는 업데이트 해야할 클라이게
					}
				}

				g_DPCoreClient.SendGuildMsgControl_Bank_Penya( pUser, nGold, 2, 0 ); 	// 2는 업데이트 해야할 다른 월드서버의 클라이언트
			}
			//	Core 서버에 전 서버에 업데이트 되야함을 알린다.
		}
		else if (mode == 1) // 아이템을 길드창고에서 빼낼때
		{
			CGuild*			pGuild = pUser->GetGuild();
			if (pGuild && pGuild->IsGetItem(pUser->m_idPlayer))
			{
				CItemElem* pItemElem	= pGuild->m_GuildBank.GetAtId( nId );
				
				if( NULL == pItemElem )
					return;
				
				dwItemNum = std::clamp<short>(static_cast<short>(dwItemNum), 1, pItemElem->m_nItemNum);
				
				CItemElem itemElem;
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= (short)( dwItemNum );
				itemElem.m_dwObjId		= pItemElem->m_dwObjId;

				if( pUser->m_Inventory.Add( &itemElem ) )
				{
					if (pItemElem->m_nItemNum > (int)( dwItemNum ) )
						pItemElem->m_nItemNum	= (short)( pItemElem->m_nItemNum - dwItemNum );
					else 
						pGuild->m_GuildBank.RemoveAtId( nId );
					UpdateGuildBank(pGuild, GUILD_GET_ITEM, 0, pUser->m_idPlayer, &itemElem, 0, (short)( dwItemNum ) );
					LogItemInfo aLogItem;
					aLogItem.Action = "Y";
					aLogItem.SendName = "GUILDBANK";
					aLogItem.RecvName = pUser->GetName();
					aLogItem.WorldId = pUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					OnLogItem( aLogItem, &itemElem, dwItemNum );
				
					// 클라이언트에게 아이템이 인벤토리에 추가됨을 알린다.
					pUser->AddGetItemGuildBank( &itemElem );
					// 자신을 제외한 모든 클라이언트에게 알려준다.
					g_UserMng.AddGetItemElem( pUser, &itemElem );
				}
				else
				{
					// 꽉차서 넣을수 가 없음. 메세지 처리
					pUser->AddBankIsFull();
				}
			}
		}
	}
}


void CDPSrvr::OnGuildBankMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == FALSE )		
		return;

	BYTE nSrcIndex, nDestIndex;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> nSrcIndex >> nDestIndex;
		
		// 길드 창고의 아이템을 스왑한다.
		CGuild* pGuild = pUser->GetGuild();
		if ( pGuild )
		{
			pGuild->m_GuildBank.Swap( nSrcIndex, nDestIndex );
			// 모든 클라이언트에게 길드창고에서 아이템이 이동했음을 알려준다.
			// 길드창고를 업데이트한다.
			UpdateGuildBank(pGuild, 4); // 4번은 아이템이 스왑된것임
		}
	}
}

void CDPSrvr::UpdateGuildBank(CGuild* p_GuildBank, int p_Mode, BYTE cbUpdate, u_long idPlayer, CItemElem* pItemElem, DWORD dwPenya, short nItemCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_BANK_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );

	if (p_GuildBank)
	{
		ar << p_GuildBank->m_idGuild;
		ar << p_GuildBank->m_nGoldGuild;
		ar << p_GuildBank->m_GuildBank;
		ar << cbUpdate;	// 멤버의 공헌페냐를 업뎃해야하는가? 
		ar << idPlayer;
		ar << p_Mode;
		if( pItemElem == NULL )
		{
			ar << (DWORD)0;
			ar << (int)0;
			ar << (SERIALNUMBER)0;
		}
		else
		{
			ar << pItemElem->m_dwItemId;
			ar << pItemElem->GetAbilityOption();
			ar << pItemElem->GetSerialNumber();
		}		
		ar << nItemCount;
		ar << dwPenya;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
}


void CDPSrvr::OnGetItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nSlot, nId;
	short nItemNum;

	ar >> nSlot >> nId >> nItemNum;
	if( nSlot >= 3 )
		return;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) )
	{
		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_BANKING>(pUser) )
				return;
		}
		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			CItemElem* pItemElem	= pUser->m_Bank[nSlot].GetAtId( nId );
			if( NULL == pItemElem )
				return;
	
			if( nItemNum > pItemElem->m_nItemNum )
				nItemNum = pItemElem->m_nItemNum;
			if( nItemNum < 1 )
				nItemNum	= 1;
			
			if( MAX_INVENTORY > pUser->m_Inventory.GetCount() )
			{
				CItemElem itemElem;
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;
				pUser->AddGetItemBank( &itemElem );			// 유저에게 전송
				pUser->m_Inventory.Add( &itemElem );		// 인벤에 넣기

				LogItemInfo aLogItem;
				aLogItem.Action = "G";
				aLogItem.SendName = "BANK";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot = nSlot;
				OnLogItem( aLogItem, pItemElem, nItemNum );
				pUser->UpdateItemBank( nSlot, nId, pItemElem->m_nItemNum - nItemNum );		// 은행에 빼기및 전송
			}
			else
			{
				// 꽉차서 넣을수 가 없음. 메세지 처리
				pUser->AddBankIsFull();
			}
		}		
	}
}

void CDPSrvr::OnPutGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	DWORD dwGold;
	BYTE nSlot;
	ar >> nSlot >> dwGold;
	
	int nGold = dwGold;
	if( nGold <= 0 )
		return;

	if( nSlot >= 3 )
		return;

	if( IsValidObj( pUser ) )
	{
		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_BANKING>(pUser) )
				return;
		}
		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			if( nGold > pUser->GetGold() )
				nGold = pUser->GetGold();
			
			// 은행돈이 overflow되지 않게 한다.
			if( CanAdd(pUser->m_dwGoldBank[nSlot], nGold) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "P";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "BANK";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() - nGold;
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot1 = nSlot;
				OnLogItem( aLogItem );
				
				pUser->m_dwGoldBank[nSlot] += nGold;
				pUser->AddGold( -nGold, FALSE );
				
				pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );
			}
		}		
	}
}

void CDPSrvr::OnGetGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	DWORD dwGold;
	BYTE nSlot;
	ar >> nSlot >> dwGold;

	int nGold = dwGold;		
	if( nGold <= 0 )
		return;

	if( nSlot >= 3 )
		return;

	if( IsValidObj( pUser ) )
	{
		if( !pUser->m_bInstantBank )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_BANKING>(pUser ) )
				return;
		}
		if( pUser->m_nSlot == nSlot || ( pUser->m_idPlayerBank[nSlot] != 0 && pUser->IsCommBank() ) )
		{
			if( nGold > (int)( pUser->m_dwGoldBank[nSlot] ) )
				nGold = pUser->m_dwGoldBank[nSlot];

			if( CanAdd(pUser->GetGold(), nGold) )
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "G";
				aLogItem.SendName = "BANK";
				aLogItem.RecvName = pUser->GetName();
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = pUser->GetGold();
				aLogItem.Gold2 = pUser->GetGold() + nGold;
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = nGold;
				aLogItem.Gold_1 = pUser->m_dwGoldBank[pUser->m_nSlot];
				aLogItem.nSlot = nSlot;
				OnLogItem( aLogItem );

				pUser->AddGold( nGold, FALSE );
				pUser->m_dwGoldBank[nSlot] -= nGold;
				pUser->AddPutGoldBank( nSlot, pUser->GetGold(), pUser->m_dwGoldBank[nSlot] );		
			}
		}		
	}
}

void CDPSrvr::OnMoveBankItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
}

void CDPSrvr::OnChangeBankPass( CAr & ar, CUser & pUser ) {
	const auto [szLastPass, szNewPass, dwId] = ar.Extract<char[5], char[5], OBJID>();

	constexpr auto IsValidBankPassword = [](const char * const str) {
		return std::strlen(str) == 4 && std::all_of(str, str + 4, isdigit2);
	};

	if (!IsValidBankPassword(szNewPass)) {
		pUser.SendSnapshotNoTarget<
			SNAPSHOTTYPE_BANK, Subsnapshot::Bank, OBJID
		>(Subsnapshot::Bank::InvalidNewPasswordQuery, dwId);
		return;
	}

	// 여기서 비밀번호 확인작업
	if( 0 == strcmp( szLastPass, pUser.m_szBankPass ) )
	{
		// 패스워드가 바꿨으므로 DB와 클라이언트에 게 바뀠다고 보내줌
		strcpy( pUser.m_szBankPass, szNewPass );
		g_dpDBClient.SendChangeBankPass( pUser.GetName(), szNewPass, pUser.m_idPlayer );
		
		pUser.SendSnapshotNoTarget<
			SNAPSHOTTYPE_BANK, Subsnapshot::Bank, OBJID
		>(Subsnapshot::Bank::OkForNewPassword, dwId);
	}
	else
	{
		pUser.SendSnapshotNoTarget<
			SNAPSHOTTYPE_BANK, Subsnapshot::Bank, OBJID
		>(Subsnapshot::Bank::InvalidNewPasswordQuery, dwId);
	}
}

void CDPSrvr::OnConfirmBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if (!IsValidObj(pUser)) return;
	if( pUser->m_vtInfo.GetOther() ) return;
	if( pUser->m_vtInfo.VendorIsVendor() ) return;
	if( pUser->m_bGuildBank ) return;
#ifdef __S_SERVER_UNIFY
	if( pUser->m_bAllAction == FALSE ) return;
#endif // __S_SERVER_UNIFY

	// 여기서 비밀번호 확인작업
	char szPass[5] = { 0, };
	ar >> szPass;
	OBJID dwId;
	ar >> dwId;

	if( dwId == NULL_ID && !CNpcChecker::GetInstance()->IsCloseNpc<MMI_BANKING>(pUser) )
			return;

	if (0 != strcmp(szPass, pUser->m_szBankPass))
	{
		pUser->SendSnapshotNoTarget<
			SNAPSHOTTYPE_BANK, Subsnapshot::Bank, OBJID
		>(Subsnapshot::Bank::InvalidCurrentPassword, dwId);
		return;
	}

	// 비밀번호를 확인 하였으므로 은행을 열수 잇게 해줌
	if( dwId != NULL_ID )
	{
		CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
		if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != II_SYS_SYS_SCR_CUSTODY)
		{
			return;
		}

		pUser->m_bInstantBank	= TRUE;
		pUser->RemoveItem( (BYTE)( pItemElem->m_dwObjId ), 1 );
	}
	pUser->m_bBank = TRUE;

	pUser->SendSnapshotNoTarget<SNAPSHOTTYPE_BANK, Subsnapshot::Bank>(
		Subsnapshot::Bank::ValidateBankAccess
	);
}

void CDPSrvr::OnSfxHit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int idSfxHit;
	int nMagicPower;
	DWORD dwSkill;
	OBJID idAttacker;
	int	nDmgCnt;	// 일반적으론 0, 지속데미지의 경우 첫빵이후는 1이상이 넘어온다. 이경우는 데미지의 10%만 준다.
	float fDmgAngle, fDmgPower;
	PSfxHit pSfxHit		= NULL;
	CMover* pAttacker	= NULL;

	ar >> idSfxHit >> nMagicPower >> dwSkill >> idAttacker >> nDmgCnt >> fDmgAngle >> fDmgPower;		

	// idAttacker가 NULL_ID면 어태커를 dpidUser로 한다.
	if( idAttacker == NULL_ID )
		pAttacker = g_UserMng.GetUser( dpidCache, dpidUser );	
	else
		pAttacker = prj.GetMover( idAttacker );

	if( IsValidObj( pAttacker ) == FALSE ) 
		return;

	pSfxHit	= pAttacker->m_sfxHitArray.GetSfxHit( idSfxHit );
	if( pSfxHit == NULL ) 
		return;

	CMover* pTarget	= prj.GetMover( pSfxHit->objid );

	/*
	// 康	// 06-10-23
	if( dwSkill == SI_MAG_FIRE_HOTAIR )	
	{
		if( IsValidObj( pTarget ) && pTarget->IsLive() )
		{
			SFXHIT_INFO si	=
				{ pTarget->GetId(), nMagicPower, dwSkill, nDmgCnt, fDmgAngle, fDmgPower, pSfxHit->dwAtkFlags	};
			AttackBySFX( pAttacker, si );
		}
		return;
	}
	*/

	pAttacker->RemoveSFX( pSfxHit->objid, idSfxHit, ( IsInvalidObj( pTarget ) || pTarget->IsDie() ), dwSkill );
	pAttacker->m_sfxHitArray.RemoveSfxHit( idSfxHit, TRUE );	// 무조건 제거
}

// 클라로부터 받은 idSfx를 어레이에 추가시켜둠
void CDPSrvr::OnSfxID( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID	idTarget;
	int		idSfxHit;
	DWORD	dwType,	dwSkill;
	int		nMaxDmgCnt;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> idTarget >> idSfxHit >> dwType >> dwSkill >> nMaxDmgCnt;
		pUser->m_sfxHitArray.Add( idSfxHit, idTarget, dwType, dwSkill, nMaxDmgCnt );		
	}
}

// 공격이 빗나가서 저절로 없어졌을때 삭제 명령.
void CDPSrvr::OnSfxClear( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID	idMover = NULL_ID;
	int	  idSfxHit;

	ar >> idSfxHit;
	ar >> idMover;

	CMover *pMover;

	if( idMover == NULL_ID )
		pMover = g_UserMng.GetUser( dpidCache, dpidUser );
	else
		pMover = prj.GetMover( idMover );

	if( IsValidObj( pMover ) )
	{
		PSfxHit pSfxHit	= pMover->m_sfxHitArray.GetSfxHit( idSfxHit );
		pMover->m_sfxHitArray.RemoveSfxHit( idSfxHit, TRUE );
	}
}


void CDPSrvr::OnMeleeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3;
#ifdef __HACK_1023
	FLOAT fVal;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3 >> fVal;
#else	// __HACK_1023
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3;
#endif	// __HACK_1023

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_MELEE_ATTACK" );
			return;
		}
#ifdef __HACK_1023
		const ItemProp* pHandItemProp	= pUser->GetActiveHandItemProp();
		FLOAT f	= pHandItemProp? pHandItemProp->fAttackSpeed: 0.0F;
		if( f != fVal )
		{
			pUser->AddDefinedText( TID_GAME_MODIFY_ATTACK_SPEED );
			return;
		}
#endif	// __HACK_1023

		CMover* pTargetObj	= prj.GetMover( objid );
		if( IsValidObj( pTargetObj ) )
		{
			int nRet = pUser->SendActMsg( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3 );
			
			if( nRet == 0 && pUser->IsFly() == FALSE )
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );

			if( nRet != -2 )	// -2는 명령 완전 무시.
			{
				g_UserMng.AddMeleeAttack( pUser, dwAtkMsg, objid, nParam2, nParam3 );
			}
		}
	}
}

void CDPSrvr::OnMeleeAttack2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_MELEE_ATTACK2" );
			return;
		}

		CMover* pTargetObj	= prj.GetMover( objid );
		if( IsValidObj( pTargetObj ) )
		{			
			int nRet = pUser->SendActMsg( (OBJMSG)dwAtkMsg, objid, nParam2, nParam3 );
			if( nRet == 0 )
				pUser->m_pActMover->m_qMeleeAtkMsg.AddTail( ACTMSG( dwAtkMsg, objid, nParam2, nParam3 ) );
			
			if( nRet != -2 )	// -2는 명령 완전 무시.
				g_UserMng.AddMeleeAttack2( pUser, dwAtkMsg, objid, nParam2, nParam3 );
		}
	}
}


void CDPSrvr::OnMagicAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAtkMsg;
	OBJID objid;
	int nParam2, nParam3, nMagicPower, idSfxHit;
	ar >> dwAtkMsg >> objid >> nParam2 >> nParam3 >> nMagicPower >> idSfxHit;

	nParam2 = 0;		//  m_qMagicAtkMsg에서 nParam2가 0이면 range attack으로 간주된다.

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pTargetObj	= prj.GetMover( objid );

		if( IsValidObj( pTargetObj ) ) 
		{
			int nRet = pUser->DoAttackMagic( pTargetObj, nMagicPower, idSfxHit );
			
			if( nRet == 0 && pUser->IsFly() == FALSE )
				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( MAGICATKMSG( dwAtkMsg, objid, nParam2, nParam3, nMagicPower, idSfxHit ) );
		}
	}
}

void CDPSrvr::OnRangeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	
	DWORD dwAtkMsg;
	OBJID objid;
	DWORD dwItemID;
	int idSfxHit;

	ar >> dwAtkMsg >> objid >> dwItemID >> idSfxHit;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMover* pTargetObj	= prj.GetMover( objid );

		if( IsValidObj( pTargetObj ) ) 
		{
			if( pUser->DoAttackRange( pTargetObj, dwItemID, idSfxHit ) == 0 )
				pUser->m_pActMover->m_qMagicAtkMsg.AddTail( MAGICATKMSG( dwAtkMsg, objid, 1, dwItemID, 0, idSfxHit ) );
		}
	}
}

void CDPSrvr::OnTeleSkill( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
//	OBJID objid;
	D3DXVECTOR3 vPos;
	ar >> vPos;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		const SKILL * blinkpool = pUser->GetSkill(SI_MAG_MAG_BLINKPOOL);

		if (!blinkpool || blinkpool->dwLevel == 0) return;

		// 현재 위치 - 클락워크 지역 x, 대상 위치 - 클락워크 지역 o = 불가
//		if( prj.IsGuildQuestRegion( pUser->GetPos() ) == FALSE && prj.IsGuildQuestRegion( vPos ) == TRUE )
		D3DXVECTOR3 v	= pUser->GetPos();
		if( prj.IsGuildQuestRegion( v ) == TRUE || prj.IsGuildQuestRegion( vPos ) )		// 현 좌표, 혹은 대상 좌표가 클락워크 지역이다.
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( !pWorld )
				return;
			if( fabs( pWorld->GetLandHeight( v ) - v.y ) > 1.0F || fabs( pWorld->GetLandHeight( vPos ) - vPos.y ) > 1.0F )
				return;
		}
		pUser->Replace( pUser->GetWorld()->GetID(), vPos, REPLACE_NORMAL, pUser->GetLayer() );
	}
}


void CDPSrvr::OnSetTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID idTarget;
	BYTE bClear;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> idTarget >> bClear;	// idTarget은 MOVER라고 가정하자.

		if( bClear == 2 )		// 2 : 타겟잡은놈을 기억.
			pUser->m_idSetTarget = idTarget;
		if( bClear < 2 )		// 0 / 1 : 타겟잡은놈에게 나를 기록 / 타겟잡은놈에게서 나를 지움.
		{
			CMover *pTarget = prj.GetMover( idTarget );		// 타겟의 포인터
			if( IsValidObj( pTarget ) )
			{
				if( bClear )	// 타겟이 해제됬다.
				{
					if( pTarget->m_idTargeter == pUser->GetId() )	// 자기가 잡았던 타겟만 자기가 풀수있다.
					{
						pTarget->m_idTargeter = NULL_ID;
						DWORD	dwTmpTick = GetTickCount();
						int	nTmpSkillID = pUser->m_pActMover->GetCastingSKillID();
						if( pUser->m_pActMover->GetCastingEndTick() > dwTmpTick && ( nTmpSkillID == SI_KNT_HERO_DRAWING || nTmpSkillID == SI_RIG_HERO_RETURN ) )
						{
							pUser->m_pActMover->SetCastingEndTick(0);
							pUser->m_pActMover->ClearState();				// 상태 클리어하고 다시 맞춤.
						}
					}
				}
				else
				{
					if( pTarget->m_idTargeter == NULL_ID )			// 타겟잡은 사람이 없을때만 타게터를 박을 수 있다. 0819
						pTarget->m_idTargeter = pUser->GetId();		// pUser가 타겟을 잡았다.
				}
			}
		}
	}
}


void CDPSrvr::OnSnapshot( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE c;
	WORD	wHdr;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> c;
		while( c-- )
		{
			ar >> wHdr;
			switch( wHdr )
			{
				case SNAPSHOTTYPE_DESTPOS:	OnPlayerDestPos( ar, pUser );	break;
				default:
					{
						// handler not found
						ASSERT( 0 );
						break;
					}
			}
		}
	}
}


void CDPSrvr::OnPlayerDestPos( CAr & ar, CUser* pUser )
{
	D3DXVECTOR3 vPos;
	BYTE fForward;
	ar >> vPos >> fForward;

	D3DXVECTOR3 vDistance	= pUser->GetPos() - vPos;
	if( D3DXVec3LengthSq( &vDistance ) > 1000000.0F )
	{
//		Error( "SNAPSHOTTYPE_DESTPOS" );
		return;
	}

#ifdef __IAOBJ0622
	CShip* pIAObj	= NULL;
	OBJID objidIAObj;
	ar >> objidIAObj;

	if( objidIAObj != NULL_ID )
	{
		pIAObj	= (CShip*)prj.GetCtrl( objidIAObj );
		if( IsValidObj( pIAObj ) == FALSE )
		{
			Error( "CShip object not found" );
			return;
		}
	}
#endif	// __IAOBJ0622

	bool bForward = (fForward != 0);

	pUser->m_pActMover->DefaultSet();
	pUser->BehaviorActionForceSet();	// 명령이 행동 명령이면 강제 동기하고, 무빙 명령이면 무시한다.

#ifdef __IAOBJ0622
	if( pIAObj )
		pUser->SetDestPos( pIAObj, vPos );
	else
		pUser->SetDestPos( vPos, bForward );
#else	// __IAOBJ0622
	pUser->SetDestPos( vPos, bForward );
#endif	// __IAOBJ0622

#ifdef __IAOBJ0622
	g_UserMng.AddSetDestPos( pUser, vPos, bForward, objidIAObj );
#else	// __IAOBJ0622
	g_UserMng.AddSetDestPos( pUser, vPos, bForward );
#endif	// __IAOBJ0622
}
/*
void CDPSrvr::OnPlayerDestAngle( CAr & ar, CUser* pUser )
{
	float fDestAngle;
	BYTE fLeft;
	ar >> fDestAngle >> fLeft;

	pUser->SetDestAngle( fDestAngle, (bool)(fLeft != 0) );
	g_UserMng.AddSetDestAngle( pUser, fDestAngle, fLeft );
}
*/
void CDPSrvr::OnChangeFace( CAr & ar, CUser & pUser ) {
	static constexpr int cost = CHANGE_FACE_COST;
	
	const auto [dwFaceNum, bUseCoupon] = ar.Extract<DWORD, bool>();

	if (dwFaceNum >= MAX_HEAD) return;
	if (CItemUpgrade::IsInTrade(pUser)) return;
	if (!CNpcChecker::GetInstance()->IsCloseNpc<MMI_BEAUTYSHOP_SKIN>(&pUser)) return;

	if (!bUseCoupon) {
		if (pUser.GetGold() < cost) {
			pUser.AddDefinedText(TID_GAME_LACKMONEY, "");
			return;
		}
		pUser.AddGold(-(cost));
	} else {
		CItemElem * pItemElem = pUser.m_Inventory.GetAtItemId(II_SYS_SYS_SCR_FACEOFFFREE);
		if (!IsUsableItem(pItemElem)) {
			pUser.AddDefinedText(TID_GAME_WARNNING_COUPON, "");
			return;
		}

		pUser.UpdateItem(*pItemElem, UI::Num::ConsumeOne);
	}

	pUser.SetHead(dwFaceNum);
	g_UserMng.AddChangeFace(pUser, dwFaceNum);
}

void CDPSrvr::OnExpUp(CAr & ar, CUser & pUser) {
	if (!pUser.IsAuthHigher(AUTH_GAMEMASTER)) return;

	EXPINTEGER nExp; ar >> nExp;
	pUser.EarnExperience(nExp, true, true);
}

void CDPSrvr::OnChangeJob(CAr & ar, CUser & pUser) {
	const auto [wantedJob, scroll] = ar.Extract<int, std::optional<OBJID>>();

	if (wantedJob < 0 || wantedJob >= MAX_JOB) return;
	if (wantedJob == JOB_PUPPETEER || wantedJob == JOB_DOPPLER || wantedJob == JOB_GATEKEEPER) return;

	CItemElem * itemScroll = nullptr;

	if (scroll) {
		itemScroll = pUser.GetItemId(scroll.value());
		if (!itemScroll) return;
		if (!IsUsableItem(itemScroll)) return;
		if (itemScroll->m_dwItemId != II_SYS_SYS_SCR_CHACLA) return;

		// TODO: check not in guildwar

		if (CItemUpgrade::IsInTrade(pUser)) {
			return pUser.AddDefinedText(TID_GAME_TRADE_NOTUSE);
		}

		if (pUser.IsAttackMode()) {
			return pUser.AddDefinedText(TID_GAME_BATTLE_NOTTRADE);
		}

		const auto currentJobType = prj.jobs.info[pUser.GetJob()].dwJobType;
		const auto wantedJobType = prj.jobs.info[wantedJob].dwJobType;
		if (currentJobType != wantedJobType) return;
		
		boost::container::small_vector<const ItemProp *, MAX_HUMAN_PARTS> badItems;
		for (int i = 0; i != MAX_HUMAN_PARTS; ++i) {
			const CItemElem * const equip = pUser.GetEquipItem(i);
			if (!equip) continue;

			const ItemProp * const prop = equip->GetProp();
			if (!prop) continue;

			const auto itemJob = prop->dwItemJob;
			if (!CMover::IsInteriorityJob(itemJob, wantedJob)) {
				badItems.push_back(prop);
			}
		}

		if (!badItems.empty()) {
			pUser.AddDefinedText(TID_GAME_CHECK_EQUIP);
			for (const ItemProp * badItem : badItems) {
				pUser.AddText(badItem->szName);
			}
			return;
		}
	} else {
		if (!pUser.IsAuthHigher(AUTH_GAMEMASTER)) {
			return;
		}
	}

	const auto exp = pUser.GetExp1();
	pUser.InitLevel(wantedJob, pUser.GetLevel());
	pUser.SetExperience(pUser.GetExp1(), pUser.GetLevel());
	pUser.AddSetExperience(pUser.GetExp1(), pUser.GetLevel(), pUser.m_nSkillPoint, pUser.m_nSkillLevel);

	if (itemScroll) {
		pUser.RemoveItem(itemScroll->m_dwObjId, 1);

		// TODO: log scroll usage
	}
}

void	CDPSrvr::OnLogItem( LogItemInfo & info, CItemElem* pItemElem, int nItemCount )
{
	if( pItemElem != NULL )
	{
		info.ItemNo = pItemElem->GetSerialNumber();
		//info.ItemName = pItemElem->GetProp()->szName;
		_stprintf( info.szItemName, "%d", pItemElem->GetProp()->dwID );
		info.itemNumber = nItemCount;
		info.nItemResist = pItemElem->m_bItemResist;
		info.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
		info.nAbilityOption = pItemElem->GetAbilityOption();
		info.Negudo = pItemElem->m_nHitPoint;
		info.MaxNegudo	= pItemElem->m_nRepair;
		info.m_dwKeepTime = pItemElem->m_dwKeepTime;
		info.nPiercedSize = pItemElem->GetPiercingSize();
		for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
			info.adwItemId[i] = pItemElem->GetPiercingItem( i );
		info.nUMPiercedSize = pItemElem->GetUltimatePiercingSize();
		for( int i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
			info.adwUMItemId[i] = pItemElem->GetUltimatePiercingItem( i );
		info.m_iRandomOptItemId = pItemElem->GetRandomOptItemId();
		if( pItemElem->m_pPet )
		{
			CPet* pPet = pItemElem->m_pPet;

			info.nPetKind = pPet->GetKind();
			info.nPetLevel = pPet->GetLevel();
			info.dwPetExp = pPet->GetExp();
			info.wPetEnergy = pPet->GetEnergy();
			info.wPetLife = pPet->GetLife();
			info.nPetAL_D = pPet->GetAvailLevel( PL_D );
			info.nPetAL_C = pPet->GetAvailLevel( PL_C );
			info.nPetAL_B = pPet->GetAvailLevel( PL_B );
			info.nPetAL_A = pPet->GetAvailLevel( PL_A );
			info.nPetAL_S = pPet->GetAvailLevel( PL_S );
		}
		// mirchang_100514 TransformVisPet_Log
		else if( pItemElem->IsTransformVisPet() == TRUE )
		{
			info.nPetKind = (BYTE)100;
		}
		// mirchang_100514 TransformVisPet_Log
	}

	BEFORESENDDUAL( ar, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( info.Action );
	ar.WriteString( info.SendName );
	ar.WriteString( info.RecvName );
	ar << info.WorldId;
	ar << info.Gold;
	ar << info.Gold2;
	ar << info.ItemNo; // 아이템 고유번호
	ar << info.Negudo; // 내구도 
	ar << info.MaxNegudo; // 내구도 
	if( _tcslen( info.szItemName ) == 0 )
	{
		_stprintf( info.szItemName, "%d", -1 );
	}
	ar.WriteString( info.szItemName );
	ar << info.itemNumber;
	ar << info.nAbilityOption;
	ar << info.Gold_1;
	ar << info.nSlot;
	ar << info.nSlot1;
	ar << info.nItemResist;
	ar << info.nResistAbilityOption;
	ar << info.m_dwKeepTime;
	ar << info.nPiercedSize;
	for( int i=0; i<info.nPiercedSize; i++ )
		ar << info.adwItemId[i];
	ar << info.nUMPiercedSize;
	for( int i=0; i<info.nUMPiercedSize; i++ )
		ar << info.adwUMItemId[i];
	ar << info.m_iRandomOptItemId;
	ar << info.nPetKind;
	ar << info.nPetLevel;
	ar << info.dwPetExp;
	ar << info.wPetEnergy;
	ar << info.wPetLife;
	ar << info.nPetAL_D;
	ar << info.nPetAL_C;
	ar << info.nPetAL_B;
	ar << info.nPetAL_A;
	ar << info.nPetAL_S;
	
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
}


void CDPSrvr::OnMotion( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetIndex() == 0 )
		{
			WriteError( "PACKETTYPE_MOTION" );
			return;
		}

		DWORD dwMsg;
		ar >> dwMsg;

		if( pUser->SendActMsg( (OBJMSG)dwMsg ) == 1 )
		{
//#ifdef __JEFF_VER_8
//			if( dwMsg != OBJMSG_STANDUP )
//#endif	// __JEFF_VER_8
			{
				pUser->ClearDest();
				pUser->ClearDestAngle();
				memset( &pUser->m_CorrAction, 0, sizeof(CORR_ACTION) );
			}
			g_UserMng.AddMotion( pUser, dwMsg );
		}
		else
			pUser->AddMotionError();
	}
}

void CDPSrvr::OnRepairItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BYTE c, nId;
		ar >> c;

		if( c == 0 || c > MAX_REPAIRINGITEM )
			return;

		CItemElem* apItemRepair[MAX_REPAIRINGITEM];
		int		anRepair[MAX_REPAIRINGITEM];
		memset( anRepair, 0, sizeof(int) * MAX_REPAIRINGITEM );

		CItemElem* pItemElem;
		int nCost	= 0;
		for( int i = 0; i < c; i++ )
		{
			apItemRepair[i]		= NULL;
			ar >> nId;
			pItemElem	= pUser->m_Inventory.GetAtId( nId );
			if( pItemElem )
			{
				ItemProp* pItemProp	= pItemElem->GetProp();
				if( pItemProp && pItemProp->dwItemKind2 >= IK2_WEAPON_HAND && pItemProp->dwItemKind2 <= IK2_ARMORETC && 
					pItemElem->m_nHitPoint < (int)( pItemProp->dwEndurance ) && pItemElem->m_nRepair < pItemProp->nMaxRepair )
				{
					int nRepair	= 100 - ( ( pItemElem->m_nHitPoint * 100 ) / pItemProp->dwEndurance );
					if( nRepair == 0 )
						continue;
					nCost	+= nRepair * ( pItemProp->dwCost / 1000 + 1 );
					apItemRepair[i]		= pItemElem;
					anRepair[i]		= nRepair;
				}
			}
		}

		if( pUser->GetGold() < nCost )
			return;

		for (int i = 0; i < c; i++) {
			if (apItemRepair[i]) {
				pUser->UpdateItem(*apItemRepair[i], UI::Repair::From(anRepair[i]));
			}
		}

		pUser->AddDefinedText( TID_GAME_REPAIRITEM );
	}
}

void CDPSrvr::OnSetHair( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nHair, nR, nG, nB;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __NEWYEARDAY_EVENT_COUPON
		BOOL bUseCoupon;
		ar >> nHair >> nR >> nG >> nB >> bUseCoupon;
#else //__NEWYEARDAY_EVENT_COUPON
		ar >> nHair >> nR >> nG >> nB;// >> nCost;
#endif //__NEWYEARDAY_EVENT_COUPON

		int nCost;
	
		nCost	= CMover::GetHairCost( (CMover*)pUser, nR, nG, nB, nHair );

#ifdef __NEWYEARDAY_EVENT_COUPON
		if( pUser->GetGold() < nCost  && !bUseCoupon)
#else //__NEWYEARDAY_EVENT_COUPON
		if( pUser->GetGold() < nCost )
#endif //__NEWYEARDAY_EVENT_COUPON
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
			return;
		}

		pUser->SetHair( nHair );
		float r, g, b;
		r	= (float)nR / 255.0f;
		g	= (float)nG / 255.0f;
		b	= (float)nB / 255.0f;

#ifdef __NEWYEARDAY_EVENT_COUPON
		if(!bUseCoupon)
			pUser->AddGold( -( nCost ) );
		else
		{
			CItemElem* pItemElem = NULL;
			pItemElem = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_HAIRCHANGE );
			if( !IsUsableItem( pItemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_WARNNING_COUPON, "" );
				return;
			}
			pUser->UpdateItem(*pItemElem, UI::Num::ConsumeOne);
		}
#else //__NEWYEARDAY_EVENT_COUPON
		pUser->AddGold( -( nCost ) );
#endif //__NEWYEARDAY_EVENT_COUPON

		pUser->SetHairColor( r, g, b );
		g_UserMng.AddSetHair( pUser, nHair, nR, nG, nB );
	}
}

void CDPSrvr::OnBlock( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nGu;
	u_long uidPlayerTo, uidPlayerFrom;
	char szNameTo[MAX_NAME] = {0,};
	char szNameFrom[MAX_NAME] = {0,};
	ar >> nGu;
	ar.ReadString( szNameTo, MAX_NAME );
	ar.ReadString( szNameFrom, MAX_NAME );

	uidPlayerTo	 = CPlayerDataCenter::GetInstance()->GetPlayerId( szNameTo );
	uidPlayerFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( szNameFrom );

	if( uidPlayerTo > 0 && uidPlayerFrom > 0 )
		g_DPCoreClient.SendBlock( nGu, uidPlayerTo, szNameTo, uidPlayerFrom );
}

void CDPSrvr::OnPiercingSize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId1, dwId2, dwId3;
	
	ar >> dwId1;
	ar >> dwId2;
	ar >> dwId3;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		CItemUpgrade::GetInstance()->OnPiercingSize( pUser, dwId1, dwId2, dwId3 );
}

void CDPSrvr::OnItemTransy(CAr & ar, CUser & pUser) {
	const auto [objidTarget, objidTransy, dwChangeId, bCash]
		= ar.Extract<OBJID, OBJID, DWORD, BOOL>();

	CItemUpgrade::GetInstance()->OnItemTransy(&pUser, objidTarget, objidTransy, dwChangeId, bCash);
}

void CDPSrvr::OnExpBoxInfo(CAr & ar, CUser & pUser) {
	const auto [objid] = ar.Extract<OBJID>();

	CCtrl * pCtrl = prj.GetCtrl(objid);
	if (!IsValidObj(pCtrl)) return;

	CCommonCtrl * pCCtrl = static_cast<CCommonCtrl *>(pCtrl);

	pUser.AddExpBoxInfo(
		objid,
		pCCtrl->m_CtrlElem.m_dwSet,
		pCCtrl->m_dwDelete - timeGetTime(),
		pCCtrl->m_idExpPlayer
	);
}

void CDPSrvr::OnPiercing(CAr & ar, CUser & pUser) {
	const auto [weaponPos, cardPos] = ar.Extract<DWORD, DWORD>();
	CItemUpgrade::GetInstance()->OnPiercing(&pUser, weaponPos, cardPos);
}

// 피어싱 옵션 제거(카드 제거)
void CDPSrvr::OnPiercingRemove(CAr & ar, CUser & pUser) {
	const auto [weaponPos] = ar.Extract<DWORD>();
	CItemUpgrade::OnPiercingRemove(pUser, weaponPos);
}

void CDPSrvr::OnCreateSfxObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwSfxId;
	u_long uIdPlayer;
	BOOL bFlag;
	ar >> dwSfxId >> uIdPlayer >> bFlag;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( uIdPlayer == NULL_ID )
		{
			g_UserMng.AddCreateSfxObj((CMover *)pUser, dwSfxId, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z, bFlag);
		}
		else
		{
			CUser* pUsertmp	= (CUser*)prj.GetUserByID( uIdPlayer );
			if( IsValidObj( pUsertmp ) )
				g_UserMng.AddCreateSfxObj((CMover *)pUsertmp, dwSfxId, pUsertmp->GetPos().x, pUsertmp->GetPos().y, pUsertmp->GetPos().z, bFlag);
		}		
	}
}

void CDPSrvr::OnRemoveItemLevelDown( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwId;
	ar >> dwId;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CItemElem* pItemElem	= pUser->GetItemId( dwId );
		if( !IsUsableItem( pItemElem ) )
			return;
		if( pItemElem->GetLevelDown() == 0 )
		{
			pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
			return;
		}
		PutItemLog( pUser, "v", "OnRemoveItemLevelDown", pItemElem );
		pItemElem->SetLevelDown();
		pUser->UpdateItem(*pItemElem, UI::RandomOptItem::Sync);
	}
}

void CDPSrvr::OnDoUseItemTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwMaterial, dwTarget;
	ar >> dwMaterial >> dwTarget;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CItemElem* pMaterial	= pUser->GetItemId( dwMaterial );
		CItemElem* pTarget	= pUser->GetItemId( dwTarget );
		if( !IsUsableItem( pMaterial ) || !IsUsableItem( pTarget ) )
			return;
		if( pUser->m_Inventory.IsEquip( dwTarget ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT );
			return;
		}
		BOOL	b	= FALSE;
		switch( pMaterial->m_dwItemId ) 
		{
			case II_SYS_SYS_QUE_PETRESURRECTION02_S:
			case II_SYS_SYS_QUE_PETRESURRECTION02_A:
			case II_SYS_SYS_QUE_PETRESURRECTION02_B:
				{
					CPet* pPet	= pTarget->m_pPet;
					if( !pPet || !pTarget->IsFlag( CItemElem::expired ) )
					{
						pUser->AddDefinedText( TID_GAME_PETRESURRECTION_WRONG_TARGET_01 );
					}
					else
					{
						BYTE nLevel		= pPet->GetLevel();
						if( ( nLevel == PL_B && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_B )
							|| ( nLevel == PL_A && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_A )
							|| ( nLevel == PL_S && pMaterial->m_dwItemId != II_SYS_SYS_QUE_PETRESURRECTION02_S )
							|| ( nLevel < PL_B )
							)
						{
							pUser->AddDefinedText( TID_GAME_PETRESURRECTION_WRONG_TARGET_02 );
						}
						else
						{
							PutItemLog( pUser, "r", "::PetResurrection", pTarget );

							pTarget->ResetFlag( CItemElem::expired );
							pUser->UpdateItem(*pTarget, UI::Flag::Sync);
							pPet->SetLife( 0 );
							pPet->SetEnergy( pPet->GetMaxEnergy() / 2 );
							pUser->AddPetState( pTarget->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp() );
							pUser->AddDefinedText( TID_GAME_PETRESURRECTION_SUCCESS );
							b	= TRUE;
						}
					}
					break;
				}
			case II_SYS_SYS_SCR_AWAKECANCEL:
			case II_SYS_SYS_SCR_AWAKECANCEL02:
				b	= DoUseItemTarget_InitializeRandomOption( pUser, pTarget, CRandomOptionProperty::eAwakening,
					TID_GAME_AWAKECANCEL_INFO, TID_GAME_AWAKECANCEL,
					"r", "::AwakeCancel" );
				break;
			case II_SYS_SYS_SCR_AWAKE:
				b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eAwakening, 
					0, TID_GAME_INVALID_TARGET_ITEM, TID_GAME_AWAKE_OR_BLESSEDNESS01,
					"r", "::Awake" );
				break;
			case II_SYS_SYS_SCR_BLESSEDNESS:
			case II_SYS_SYS_SCR_BLESSEDNESS02:
				b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eBlessing, 
					0, TID_GAME_USE_BLESSEDNESS_INFO, TID_GAME_BLESSEDNESS_INVALID_ITEM,
					"r", "::Blessedness" );
				break;
			case II_SYS_SYS_SCR_EATPETAWAKE:	// 먹펫 각성
				b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eEatPet,
					TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
					"r", "EATPETAWAKE" );
				break;
			case II_SYS_SYS_SCR_PETAWAKE:	// 시스템 펫 각성
				b	= DoUseItemTarget_GenRandomOption( pUser, pTarget, CRandomOptionProperty::eSystemPet, 
					TID_GAME_PETAWAKE_S00, TID_GAME_PETAWAKE_E00, TID_GAME_PETAWAKE_E00,
					"r", "PETAWAKE" );
				break;
			case II_SYS_SYS_SCR_PETAWAKECANCEL:		// 시스템 펫 각성 취소
				b	= DoUseItemTarget_InitializeRandomOption( pUser, pTarget, CRandomOptionProperty::eSystemPet,
					TID_GAME_PETAWAKECANCEL_S00, TID_GAME_PETAWAKECANCEL_E00,
					"r", "PETAWAKECANCEL" );
				break;
			case II_SYS_SYS_SCR_LEVELDOWN01:
			case II_SYS_SYS_SCR_LEVELDOWN02:
				b	= DoUseItemTarget_ItemLevelDown( pUser, pMaterial, pTarget );
				break;
			default:
				break;
		}
		if( b )
		{
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
			if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
				g_UserMng.AddCreateSfxObj( pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

			PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterial );

			pUser->UpdateItem(*pMaterial, UI::Num::ConsumeOne);
		}
	}
}

void CDPSrvr::OnSmeltSafety( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID dwItemId, dwItemMaterialId, dwItemProtScrId, dwItemSmeltScrId;

	//	pItemSmeltScrId - 일반제련시의 제련두루마리(사용안할시엔 Client에서 NULL_ID를 입력)
	ar >> dwItemId >> dwItemMaterialId >> dwItemProtScrId >> dwItemSmeltScrId;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) )
	{
#ifdef __QUIZ
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}
#endif // __QUIZ
		// 康
		if( pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor() )	// 거래중인 대상이 있으면?
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemMaterialId );
		CItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwItemProtScrId );
		CItemElem* pItemElem3	= NULL;
		if( dwItemSmeltScrId != NULL_ID )
		{
			pItemElem3	= pUser->m_Inventory.GetAtId( dwItemSmeltScrId );
			if( !IsUsableItem( pItemElem3 ) )
				return;
		}

		if( !IsUsableItem( pItemElem0 ) || !IsUsableItem( pItemElem1 ) || !IsUsableItem( pItemElem2 ) )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		// 장착되어 있는 아이템은 제련 못함
		if( pUser->m_Inventory.IsEquip( dwItemId ) )
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}

		if( pItemElem0->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
		{
			pUser->AddSmeltSafety( 0 );
			return;
		}
		
		BYTE nResult = CItemUpgrade::GetInstance()->OnSmeltSafety( pUser, pItemElem0, pItemElem1, pItemElem2, pItemElem3 );

		pUser->AddSmeltSafety( nResult );
	}
}

void CDPSrvr::OnEnchant( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwItemId, dwItemMaterialId;
	
	ar >> dwItemId;
	ar >> dwItemMaterialId;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __QUIZ
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
			return;
#endif // __QUIZ
		// 康
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;


		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemMaterialId );		

		if( !IsUsableItem( pItemElem0 ) || !IsUsableItem( pItemElem1 ) )
			return;

		if( pItemElem0->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
		{
			pUser->AddDefinedText( TID_GAME_NOTUPGRADE , "" );
			return;
		}


		CItemUpgrade::GetInstance()->OnEnchant( pUser, pItemElem0, pItemElem1 );
	}	
}

void CDPSrvr::OnRemoveAttribute( CAr & ar, CUser & pUser_) {
	int nPayPenya = 100000; //속성제련 제거시 필요한 페냐

	OBJID objItemId; ar >> objItemId;

	CUser * pUser = &pUser_;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId( objItemId );

	if( !IsUsableItem( pItemElem ) )
	{
		pUser->AddRemoveAttribute( FALSE );
		return;
	}
	// 무기나 방어구만 가능
	if( !CItemElem::IsEleRefineryAble(pItemElem->GetProp()) )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM , "" );
		return;
	}
	if( pItemElem->m_nResistSMItemId != 0 ) // 상용화 아이템 적용중이면 불가능
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_NOTUPGRADE , "" );
		return;
	}
	// 장착되어 있는지 확인을 해야 함.
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return;
	}
	// 10만 페냐 이상을 소지해야만 속성 제거 가능.
	if( pUser->GetGold() < nPayPenya )
	{
		pUser->AddRemoveAttribute( FALSE );
		pUser->AddDefinedText( TID_GAME_LACKMONEY , "" );
		return;
	}
	
	// 현재 무기에 속성이 적용되어 있어야 가능.
	if( (pItemElem->m_bItemResist != SAI79::NO_PROP) && (pItemElem->m_nResistAbilityOption > 0) )
	{
		pUser->AddGold( -nPayPenya );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
	
		pUser->UpdateItem(*pItemElem, UI::Element::None());
		pUser->AddRemoveAttribute( TRUE );
		
		// 속성제련 제거 성공 로그
		LogItemInfo aLogItem;
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "REMOVE_ATTRIBUTE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold() + nPayPenya;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = -nPayPenya;
		aLogItem.Action = "O";
		OnLogItem( aLogItem, pItemElem, 1 );
	}
	else
		pUser->AddRemoveAttribute( FALSE );
	
}

void CDPSrvr::OnChangeAttribute(CAr & ar, CUser & pUser) {
	const auto [objTargetItem, objMaterialItem, nAttribute] = ar.Extract<OBJID, OBJID, int>();
	CItemUpgrade::GetInstance()->ChangeAttribute(&pUser, objTargetItem, objMaterialItem, static_cast<SAI79::ePropType>(nAttribute));
}

void CDPSrvr::OnRandomScroll( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize )
{
	DWORD dwId1, dwId2;
	
	ar >> dwId1;
	ar >> dwId2;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwId1 );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwId2 );
		if( !IsUsableItem( pItemElem0 ) || !IsUsableItem( pItemElem1 ) )
		{
			return;
		}

		if( pUser->m_Inventory.IsEquip( dwId1 ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}			

		if( pItemElem1->GetProp()->dwItemKind3 != IK3_RANDOM_SCROLL )
		{
			return;					
		}

		if( !(pItemElem0->GetProp()->dwItemKind1 == IK1_WEAPON || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC ) )
		{
			pUser->AddDefinedText(  TID_GAME_RANDOMSCROLL_ERROR, "" );
			return;
		}

		std::uint8_t nSTR[4] = { 1,  9, 21, 37 };
		std::uint8_t nDEX[4] = { 2, 10, 22, 38 };
		std::uint8_t nINT[4] = { 3, 11, 23, 39 };
		std::uint8_t nSTA[4] = { 4, 12, 24, 40 };

		int nValue = 0;
		int nRandom = xRandom( 100 );
		if( nRandom < 64 )
			nValue = 1;
		else if( nRandom < 94 )
			nValue = 2;
		else if( nRandom < 99 )
			nValue = 3;
		else if( nRandom < 100 )
			nValue = 4;

		if( 0 < nValue )
		{
			std::uint8_t nKind = 0;
			int nToolKind = 0;
			if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMSTR )
			{
				nKind = nSTR[nValue-1];
				nToolKind = DST_STR;
			}
			else if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMDEX )
			{
				nKind = nDEX[nValue-1];
				nToolKind = DST_DEX;
			}
			else if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMINT )
			{
				nKind = nINT[nValue-1];
				nToolKind = DST_INT;
			}
			else if( pItemElem1->GetProp()->dwID == II_SYS_SYS_SCR_RANDOMSTA )
			{
				nKind = nSTA[nValue-1];
				nToolKind = DST_STA;
			}

			pUser->UpdateItem(*pItemElem0, UI::RandomOptItemFromRandomScroll(nKind));

			LogItemInfo aLogItem;
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "RANDOMSCROLL";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Action = "(";
			OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
			aLogItem.Action = ")";
			OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );

			pUser->RemoveItem( (BYTE)( dwId2 ), (short)1 );		

			// 아이템 박기 성공~
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			

			DWORD dwStringNum = 0;
			switch( nToolKind )
			{
			case DST_STR:
				dwStringNum = TID_TOOLTIP_STR;
				break;
			case DST_DEX:
				dwStringNum = TID_TOOLTIP_DEX;
				break;
			case DST_STA:
				dwStringNum = TID_TOOLTIP_STA;
				break;
			default: //case DST_INT:
				dwStringNum = TID_TOOLTIP_INT;
				break;
			}
			CString strMessage;
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_SUCCESS ), pItemElem0->GetProp()->szName, prj.GetText( dwStringNum ), nValue );
			pUser->AddText( strMessage );
		}
	}
}
	
void CDPSrvr::OnUpgradeBase( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
}


void CDPSrvr::OnCommercialElem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwItemId0, dwItemId1;
	ar >> dwItemId0 >> dwItemId1;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
		// 인벤토리에 있는지 검사
		CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId0 );
		CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwItemId1 );

		if( !IsUsableItem( pItemElem0 ) || 
			!IsUsableItem( pItemElem1 ) || 
			pItemElem0->GetProp()->nLog >=2 || 
			pItemElem1->GetProp()->nLog >=2 )
		{
			return;
		}

		if( pUser->m_Inventory.IsEquip( dwItemId0 ) || pUser->m_Inventory.IsEquip( dwItemId1 ) )
		{
			pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
			return;
		}

		// 방어구(슈트만), 무기류가 아니면 제련불가능
		if( !( ( pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
			pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT ) ||
			( ( pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR || pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC ) 
			&& pItemElem0->GetProp()->dwItemKind3 == IK3_SUIT )
			) )		
		{
			return;			
		}

		if( pItemElem0->m_nResistSMItemId != 0 ) // 이미적용한 아이템이면 불가능
		{
			return;	
		}

		BOOL bResistDelete = FALSE;

		if( pItemElem1->GetProp()->dwItemKind2 == IK2_SYSTEM )
		{
			// 속성 공격력 추가
			if( pItemElem1->m_dwItemId == II_CHR_SYS_SCR_FIREASTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_WATEILSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_WINDYOSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_LIGHTINESTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_EARTHYSTONE ) 
				
			{
				if( pItemElem0->GetProp()->dwItemKind2 == IK2_ARMOR ||
					pItemElem0->GetProp()->dwItemKind2 == IK2_ARMORETC )
				{
					return;	
				}
			}
			else // 속성 방어력 추가
			if(	pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEFIREASTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEWATEILSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEWINDYOSTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DELIGHTINESTONE ||
				pItemElem1->m_dwItemId == II_CHR_SYS_SCR_DEEARTHYSTONE )
			{
				if( pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC ||
					pItemElem0->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT )
				{
					return;	
				}
			}
			else // 속성 제거
			if( pItemElem1->m_dwItemId == II_CHR_SYS_SCR_TINEINEDSTONE )
			{
				if( pItemElem0->m_bItemResist == SAI79::NO_PROP )
				{
					return;	
				}
				pItemElem0->m_bItemResist = SAI79::NO_PROP;
				pItemElem0->m_nResistAbilityOption = 0;
				bResistDelete = TRUE;
			}
			else
			{
				return;	
			}
		}
		else 
		{
			return;	
		}

		// 성공
		
		if( bResistDelete )
		{
			pUser->AddCommercialElem( pItemElem0->m_dwObjId, 9999 );
		}
		else
		{
			pItemElem0->m_nResistSMItemId = pItemElem1->m_dwItemId;
		}
		
		// 로그
		g_dpDBClient.SendLogSMItemUse( "5", pUser, pItemElem1, pItemElem1->GetProp() );
		LogItemInfo aLogItem;
		aLogItem.Action = "5";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.ItemNo = pItemElem0->GetSerialNumber();
		//aLogItem.ItemName = pItemElem0->GetProp()->szName;
		_stprintf( aLogItem.szItemName, "%d", pItemElem0->GetProp()->dwID );
		aLogItem.itemNumber = pItemElem0->m_nItemNum;
		aLogItem.nAbilityOption = pItemElem0->GetAbilityOption();
		aLogItem.nItemResist = pItemElem0->m_bItemResist;
		aLogItem.nResistAbilityOption = pItemElem0->m_nResistAbilityOption;
		aLogItem.Negudo = int((float)pItemElem0->m_nHitPoint * 100 / (float)pItemElem0->GetProp()->dwEndurance );
		OnLogItem( aLogItem );

		pUser->RemoveItem( (BYTE)dwItemId1, (short)1 );
		
		g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_SYS_EXPAN01, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddCommercialElem( pItemElem0->m_dwObjId, pItemElem0->m_nResistSMItemId );
		
	}
}

void CDPSrvr::OnRequestGuildRank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CTime		tm			= CTime::GetCurrentTime();
	CTimeSpan	tm_update	= tm - CGuildRank::Instance.m_UpdateTime;

	DWORD	ver;
	
	//	버젼 정보를 받는다.
	ar >> ver;

	// 갱신된지 하루가 지났는지 체크
	if ( tm_update.GetHours() >= 24 )
	{
		// TRANS 서버에게 다시 랭크 정보를 갱신할 것을 요청한다.
		g_dpDBClient.UpdateGuildRankingUpdate();
	}
	else
	{
		// 랭크 정보 버젼이 다를 경우엔 랭크 정보를 보내게 된다.
		if ( CGuildRank::Instance.m_Version != ver )
		{
			// 랭킹 정보를 보낸다.
			CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
			if( IsValidObj( pUser ) )
			{
				pUser->SendGuildRank();
			}
		}
	}
}

void CDPSrvr::OnEnterChattingRoom(CAr & ar, CUser & pUser) {
	u_long uidChattingRoom; ar >> uidChattingRoom;

	if (pUser.m_idChatting != 0) return;

	CChatting * pChatting = g_ChattingMng.GetChttingRoom(uidChattingRoom);
	if (!pChatting) return;

	// > mfw no range check of the user with the owner of the chatting room
	// > mfw no face

	const bool added = pChatting->AddChattingMember(pUser.m_idPlayer);
	if (!added) return;

	pUser.m_idChatting = uidChattingRoom;


	for (const auto & member : pChatting->GetMembers()) {
		if (member.m_playerId == pUser.m_idPlayer) break;

		CUser * pUsertmp = prj.GetUserByID(member.m_playerId);
		if (IsValidObj(pUsertmp)) {
			pUsertmp->AddEnterChatting(&pUser);
		}
	}

	pUser.AddNewChatting(pChatting);
}

void CDPSrvr::OnChatting( CAr & ar, CUser & pUser ) {
	TCHAR	sChat[1024];
	ar.ReadString(sChat);

#ifdef __JEFF_9_20
	int nText	= pUser.GetMuteText();
	if (nText) {
		pUser.AddDefinedText(nText);
		return;
	}
#endif	// __JEFF_9_20

	if (!(pUser.HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
		ParsingEffect(sChat, strlen(sChat));

	RemoveCRLF(sChat);

	CChatting * pChatting = g_ChattingMng.GetChttingRoom(pUser.m_idChatting);
	if (!pChatting) {
		// 채팅방이 없음.
		pUser.m_idChatting = 0;
		pUser.AddDeleteChatting();
		return;
	}

	if (!pChatting->IsChattingMember(pUser.m_idPlayer)) {
		// 채팅멤버가 아님
		pUser.m_idChatting = 0;
		pUser.AddDeleteChatting();
		return;
	}

	for (const auto & member : pChatting->GetMembers()) {
		CUser * pUsertmp = prj.GetUserByID(member.m_playerId);
		if (IsValidObj(pUsertmp)) {
			pUsertmp->AddChatting(pUser.m_idPlayer, sChat);
		}
	}
}

static void OnUpdateChattingRoomStatus(CUser & pUser, BOOL newState) {
	CChatting * pChatting = g_ChattingMng.GetChttingRoom(pUser.m_idChatting);
	if (!pChatting) return;

	pChatting->m_bState = newState;

	for (const auto & member : pChatting->GetMembers()) {
		CUser * pUsertmp = prj.GetUserByID(member.m_playerId);
		if (IsValidObj(pUsertmp)) {
			if (member.m_playerId != pUser.m_idPlayer) {
				pUsertmp->AddChttingRoomState(newState);
			}
		}
	}
}

void CDPSrvr::OnOpenChattingRoom(CAr & ar, CUser & pUser) {
	OnUpdateChattingRoomStatus(pUser, TRUE);
}

void CDPSrvr::OnCloseChattingRoom(CAr & ar, CUser & pUser) {
	OnUpdateChattingRoomStatus(pUser, FALSE);
}

void CDPSrvr::OnCommonPlace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
//		BYTE nType;
//		ar >> nType;
}


void CDPSrvr::QueryDestroyPlayer( DPID dpidCache, DPID dpidSocket, DWORD dwSerial, u_long idPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERY_DESTROY_PLAYER, dpidSocket );
	ar << dwSerial;
	ar << idPlayer;
	SEND( ar, this, dpidCache );
}

void CDPSrvr::OnSetNaviPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	const auto [pos, objidTarget] = ar.Extract<D3DXVECTOR3, OBJID>();

	NaviPoint nv;
	nv.Pos = pos;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( objidTarget == NULL_ID )
		{
			if(CParty * pParty = g_PartyMng.GetParty(pUser->m_idparty)) {
				for (CUser * pUsertmp : AllMembers(*pParty)) {
					pUsertmp->AddSetNaviPoint(nv, pUser->GetId(), pUser->GetName(TRUE));
				}
			}
		}
		else
		{
			CUser* pUsertmp	= prj.GetUser( objidTarget );
			if( IsValidObj( pUsertmp ) )
			{
				pUser->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
				pUsertmp->AddSetNaviPoint( nv, pUser->GetId(), pUser->GetName( TRUE ) );
			}
		}
	}
}

void CDPSrvr::OnGameMasterWhisper( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHAR	sPlayerFrom[MAX_PLAYER], lpString[260], szChat[1024];
	ar.ReadString( sPlayerFrom, MAX_PLAYER );
	ar.ReadString( lpString, 260 );

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{	
		sprintf( szChat, "%s -> %s", sPlayerFrom, lpString );
		g_dpDBClient.SendLogGamemaChat( pUser, szChat );
	}
}
/*
#ifdef __S0114_RELOADPRO
void CDPSrvr::SendReloadProject( DPID dpidCache, DPID dpidUser )
{
//	BEFORESENDSOLE( ar1, PACKETTYPE_RELOAD_PROJECT, dpidUser );
//	SEND( ar1, &g_DPSrvr, dpidCache );
}
#endif // __S0114_RELOADPRO
*/
// 현상금 걸기 패킷 
void CDPSrvr::OnNWWantedGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int		nGold;
	char	szMsg[WANTED_MSG_MAX + 1];

	ar >> nGold;
	ar.ReadString( szMsg, WANTED_MSG_MAX + 1 );

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( pUser->m_idMurderer == 0 )		// 나를 죽인자가 없었으면 현상금을 걸 수 없다.
		return;

	if( szMsg[0] == '\0' )
		return;
	
	if( strlen(szMsg) > WANTED_MSG_MAX )
		return;

	if( nGold < MIN_INPUT_REWARD || nGold > MAX_INPUT_REWARD )			// 현상금은 최소 1000패냐에서 최대 2억 패냐까지 걸 수 있다. 
		return;

	int nTax = MulDiv( nGold, 10, 100 );					// 건 현상금의 10%는 수수료로 지급된다. 

	if( pUser->GetGold() >= (nGold + nTax) ) 
		pUser->AddGold( -(nGold + nTax) );
	else
		return;

	const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pUser->m_idMurderer );
	if( lpszPlayer == NULL )
		lpszPlayer = "";
	g_DPCoreClient.SendWCWantedGold( lpszPlayer, pUser->m_idMurderer, nGold, szMsg );
}


// 현상금 리스트 요청 패킷
void CDPSrvr::OnNWWantedList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	{
		BEFORESENDSOLE( out, PACKETTYPE_WN_WANTED_LIST, pUser->m_Snapshot.dpidUser );
		out << CWantedListSnapshot::GetInstance();
		SEND( out, this, dpidCache );
	}
}

void CDPSrvr::OnNWWantedName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	LPCSTR lpszPlayer = "";
	if( pUser->m_idMurderer )		
	{
		lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pUser->m_idMurderer );
		if( lpszPlayer == NULL )
			lpszPlayer	= "";
	}
	
	{
		BEFORESENDSOLE( ar, PACKETTYPE_WN_WANTED_NAME, pUser->m_Snapshot.dpidUser );
		ar.WriteString( lpszPlayer );
		SEND( ar, this, dpidCache );
	}
}


// 현상범 자세한정보 요청 패킷 
void CDPSrvr::OnNWWantedInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char szPlayer[64];
	ar.ReadString( szPlayer, 64 );

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	const bool isWanted = CWantedListSnapshot::GetInstance().IsWantedPlayer(szPlayer);
	if (!isWanted) return;

	const int nGold = REQ_WANTED_GOLD;
	if (pUser->GetGold() < nGold) {
		pUser->AddDefinedText(TID_GAME_LACKMONEY, "");	// 인벤에 돈이부족
		return;
	}

	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );		// 현상범의 위치 
	BYTE		byOnline = 0;					// 1 이면 online
	DWORD		dwWorldID = 0;
	LPCTSTR		lpszWorld = "";

	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( szPlayer );
	CUser* pTarget	= g_UserMng.GetUserByPlayerID( idPlayer );	
	if( IsValidObj(pTarget) && pTarget->GetWorld() )
	{
		vPos      = pTarget->GetPos();
		byOnline  = 1;
		dwWorldID = pTarget->GetWorld()->GetID();
		lpszWorld = pTarget->GetWorld()->m_szWorldName;

		pUser->AddGold( -nGold );
	}

	pUser->AddWantedInfo( vPos, byOnline, dwWorldID, lpszWorld );
}

void CDPSrvr::OnReqLeave(CAr & ar, CUser & pUser) {
	if (pUser.m_dwLeavePenatyTime == 0)	// 페널티 타임을 세팅한 적이 없는가?
		pUser.m_dwLeavePenatyTime = ::timeGetTime() + TIMEWAIT_CLOSE * 1000;	//  세팅 
}

void CDPSrvr::OnStateMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwStateMode;
	BYTE nFlag;
	ar >> dwStateMode;
	ar >> nFlag;


	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{	
		if( pUser->IsStateMode( dwStateMode ) )
		{
			if( nFlag == STATEMODE_BASEMOTION_CANCEL )
			{
				pUser->SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
				pUser->m_nReadyTime = 0;
				pUser->m_dwUseItemId = 0;
			}
		}
	}
}

// 부활취소
void CDPSrvr::OnResurrectionCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) && pUser->m_Resurrection_Data )
	{	
		pUser->m_Resurrection_Data = std::nullopt;
	}
}

// 사용자가 OK하면 부활을 쓰게 한다
void CDPSrvr::OnResurrectionOK(CAr & ar, CUser & pUser) {
	// 서버에서 부활모드인가??
	if (!pUser.m_Resurrection_Data) return;

	if (!pUser.IsDie()) {
		pUser.m_Resurrection_Data = std::nullopt;
		return;
	}

	// 약간의 에너지를 채워주자
	RESURRECTION_DATA pData = pUser.m_Resurrection_Data.value();
	pUser.m_Resurrection_Data = std::nullopt;

	CUser * pSrc = prj.GetUserByID(pData.dwPlayerID);
	if (!IsValidObj(pSrc)) return;

	// 부활 SFX효과
	g_UserMng.AddCreateSfxObj(&pUser, XI_SKILL_ASS_HEAL_RESURRECTION01);

	// 부활하기
	g_UserMng.AddHdr(&pUser, SNAPSHOTTYPE_RESURRECTION);
	pUser.m_pActMover->SendActMsg(OBJMSG_RESURRECTION);

	pUser.ApplyParam(pSrc, pData.pSkillProp, pData.pAddSkillProp, TRUE, 0);

	if (pData.pAddSkillProp->dwDestParam2 == DST_RECOVERY_EXP) {
		pUser.SubDieDecExp(TRUE, pData.pAddSkillProp->nAdjParamVal2);	// 부활이 되면서 겸치가 조금 깎임.			
	}
}

void CDPSrvr::OnChangeMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nFlag;
	DWORD dwMode;
	ar >> dwMode;
	ar >> nFlag;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj(pUser) )
	{	
		switch( nFlag )
		{
		case 0:	// 자동 PK ON
			pUser->SetMode( FREEPK_MODE );
			break;
		case 1:	// 자동 PK OFF
			pUser->SetNotMode( FREEPK_MODE );
			break;
		case 2:	// 자동 PVP ON
			pUser->SetMode( PVPCONFIRM_MODE );
			break;
		case 3:	// 자동 PVP OFF
			pUser->SetNotMode( PVPCONFIRM_MODE );
			break;
		}
		g_UserMng.AddModifyMode( pUser );
	}
}

void CDPSrvr::OnQuerySetPlayerName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
// II_SYS_SYS_SCR_CHANAM
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER]	= { 0, };

	ar >> dwData;
	ar.ReadString( lpszPlayer, MAX_PLAYER );

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
#ifdef __QUIZ
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
			return;
#endif // __QUIZ
#ifdef __RULE_0615
		if (prj.nameValider.IsNotAllowedName(lpszPlayer)) {
			pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
			return;
		}
		prj.nameValider.Formalize( lpszPlayer );
#endif	// __RULE_0615

#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
		{
			WORD wId	= LOWORD( dwData );
			if( (short)wId >= 0 )
			{
				CItemElem* pItemElem = pUser->GetItemId( wId );
				if( IsUsableItem( pItemElem ) && pItemElem->m_dwItemId == II_SYS_SYS_SCR_CHANAM && pItemElem->m_bQuery == FALSE )
				{
					pItemElem->m_bQuery	= TRUE;
					g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, lpszPlayer, dwData );
				}
			}
		}
#ifdef __S_SERVER_UNIFY
		else
		{
			g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, lpszPlayer, dwData );
		}
#endif // __S_SERVER_UNIFY
	}
}

void CDPSrvr::OnQuerySetGuildName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nId;
	char lpszGuild[MAX_G_NAME]	= { 0, };

	ar >> nId;
	ar.ReadString( lpszGuild, MAX_G_NAME );

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if (!IsValidObj(pUser)) return;

	if (prj.nameValider.IsNotAllowedName(lpszGuild)) {
		pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
		return;
	}

	if( pUser->m_bAllAction ) {
		CItemElem* pItemElem = pUser->GetItemId( nId );
		if (IsUsableItem(pItemElem) && pItemElem->m_bQuery == FALSE && pItemElem->m_dwItemId == II_SYS_SYS_SCR_GCHANAM) {
			pItemElem->m_bQuery = TRUE;
			g_DPCoreClient.SendQuerySetGuildName(pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId);
		}
	} else {
		g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, lpszGuild, nId );
	}
}

void CDPSrvr::PutCreateItemLog( CUser* pUser, CItemElem* pItemElem, const char* szAction, const char* recv )
{
	LogItemInfo logitem;
	logitem.Action		= szAction;
	logitem.SendName = pUser->GetName();
	logitem.RecvName = recv;
	logitem.WorldId = pUser->GetWorld()->GetID();
	logitem.Gold = pUser->GetGold();
	logitem.Gold2 = 0;
	logitem.ItemNo = pItemElem->GetSerialNumber();
	//logitem.ItemName = pItemElem->GetProp()->szName;
	_stprintf( logitem.szItemName, "%d", pItemElem->GetProp()->dwID );
	logitem.itemNumber = pItemElem->m_nItemNum;
	logitem.nAbilityOption = 0;
	logitem.Gold_1 = 0;
	logitem.nItemResist = pItemElem->m_bItemResist;
	logitem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
	logitem.Negudo		= 100;
	OnLogItem( logitem );
}

// 클라로부터 탈출요청이 들어옴
void CDPSrvr::OnDoEscape( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ItemProp* pItemProp = prj.GetItemProp( II_CHR_SYS_SCR_ESCAPEBLINKWING );
		if( pItemProp )
		{
			if( !pUser->IsSMMode( SM_ESCAPE ) )
				pUser->DoUseItemVirtual( II_CHR_SYS_SCR_ESCAPEBLINKWING, FALSE );
			else
				pUser->AddDefinedText( TID_GAME_STILLNOTUSE );		
		}
	}
}

void CDPSrvr::OnCheering( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetId() == objid )
			return;

		CMover* pTarget		= prj.GetMover( objid );
		if( IsValidObj( pTarget ) && pTarget->GetType() == OT_MOVER && pTarget->IsPlayer() )
		{
			DWORD dwTickCount	= GetTickCount();
			if( pUser->m_nCheerPoint <= 0 )
			{
				pUser->AddDefinedText( TID_CHEER_NO1, "%d", (pUser->m_dwTickCheer - dwTickCount) / 60000 );
				return;
			}

			if( pUser->m_nCheerPoint == MAX_CHEERPOINT )
				pUser->SetCheerParam( pUser->m_nCheerPoint - 1, dwTickCount, TICK_CHEERPOINT );
			else
				pUser->SetCheerParam( pUser->m_nCheerPoint - 1, dwTickCount, pUser->m_dwTickCheer - dwTickCount );

			FLOAT fAngle = GetDegree(pTarget->GetPos(), pUser->GetPos());
			pUser->SetAngle(fAngle);

			if( pTarget->GetSex() == pUser->GetSex() )
			{
				((CUser*)pTarget)->AddDefinedText( TID_CHEER_MESSAGE3, "%s", pUser->GetName() );
				pUser->SendActMsg( OBJMSG_MOTION, MTI_CHEERSAME, ANILOOP_1PLAY );
			}
			else
			{
				((CUser*)pTarget)->AddDefinedText( TID_CHEER_MESSAGE4, "%s", pUser->GetName() );
				pUser->SendActMsg( OBJMSG_MOTION, MTI_CHEEROTHER, ANILOOP_1PLAY );
			}

			g_UserMng.AddCreateSfxObj(pUser, XI_CHEERSENDEFFECT );
			g_UserMng.AddCreateSfxObj(pTarget, XI_CHEERRECEIVEEFFECT );


			CMover* pSrc = pUser ;
			g_UserMng.AddMoverBehavior( pSrc, pSrc->GetPos(), pSrc->m_pActMover->m_vDelta,
				pSrc->GetAngle(), pSrc->m_pActMover->GetState(), pSrc->m_pActMover->GetStateFlag(), 
				pSrc->m_dwMotion, pSrc->m_pActMover->m_nMotionEx, pSrc->m_pModel->m_nLoop, pSrc->m_dwMotionOption, g_TickCount.GetTickCount(), TRUE );

			ItemProp *pItemProp = prj.GetItemProp( II_CHEERUP ); // 응원 아이템
			if( pItemProp )
				pTarget->DoApplySkill( pTarget, pItemProp, NULL );
		}
		else
		{
			pUser->AddDefinedText( TID_CHEER_NO2, "" );	
		}
	}
}

void CDPSrvr::OnQueryEquip( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CUser* pUsertmp		= prj.GetUser( objid );
		if( IsValidObj( pUsertmp ) )
		{
			
			if( pUsertmp->IsMode( EQUIP_DENIAL_MODE ) && pUser->IsAuthHigher( AUTH_GAMEMASTER ) == FALSE )
			{
				pUser->AddDefinedText( TID_DIAG_0088 );
				return;
			}
			pUser->AddQueryEquip( pUsertmp );
		}
	}
}

void CDPSrvr::OnQueryEquipSetting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BOOL bAllow;
	ar >> bAllow;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( bAllow )
			pUser->SetNotMode( EQUIP_DENIAL_MODE );
		else
			pUser->SetMode( EQUIP_DENIAL_MODE );
		g_UserMng.AddModifyMode( pUser );
	}
}

void CDPSrvr::OnOptionEnableRenderMask( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		BOOL bEnable;
		ar >> bEnable;
		if( bEnable )
			pUser->SetNotMode( MODE_OPTION_DONT_RENDER_MASK );
		else
			pUser->SetMode( MODE_OPTION_DONT_RENDER_MASK );
		g_UserMng.AddModifyMode( pUser );
	}
}

void CDPSrvr::OnReturnScroll( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nSelect;
	ar >> nSelect;
	if( nSelect < -1 || nSelect > 2 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	if( nSelect >= 0 )
	{
		static const char* szPos[] = { "flaris",	"saintmorning",	"darkon" };
		pUser->m_lpszVillage = szPos[nSelect];
		pUser->AddReturnScroll();	//응답을 보내면 '귀환의 두루마리'아이템을 사용한다.	
	}
	else
	{
		// 저장된 위치로 돌아가기 
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_RETURN ) )
			pUser->DoUseItemVirtual( II_SYS_SYS_SCR_RETURN, TRUE );
	}
}

void CDPSrvr::OnEndSkillQueue(CAr & ar, CUser & pUser) {
	pUser.m_playTaskBar.OnEndSkillQueue(&pUser);
}

void CDPSrvr::OnQueryPostMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszReceiver[MAX_PLAYER]	= { 0, };
	char lpszTitle[MAX_MAILTITLE]	= { 0, };
	char lpszText[MAX_MAILTEXT]		= { 0, };
	BYTE nItem;
	short nItemNum;
	int nGold;

	ar >> nItem >> nItemNum;
	ar.ReadString( lpszReceiver, MAX_PLAYER );
	
	ar >> nGold;
	ar.ReadString( lpszTitle, MAX_MAILTITLE );
	ar.ReadString( lpszText, MAX_MAILTEXT );

	int nPostGold = 100;
	if( ::GetLanguage() != LANG_KOR )
		nPostGold = 500;

	if( nItem < 0 || nGold < 0 || ( nGold + nPostGold ) <= 0 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		//raiders.2006.11.27
		if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
			return;
		if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
			return;
		if( pUser->m_bBank )				// 창고를 열고 있으면?
			return;
#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction == FALSE )
			return;
#endif // __S_SERVER_UNIFY
		//--
		u_long idReceiver	= CPlayerDataCenter::GetInstance()->GetPlayerId( lpszReceiver );
		if( idReceiver > 0 )
		{
			if( CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() ) == idReceiver )
			{
				pUser->AddDiagText(prj.GetText(TID_GAME_MSGSELFSENDERROR));
				return;
			}

			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_POST>(pUser) )
			{
				return;
			}
			CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
			if( pMailBox && pMailBox->m_mails.size() >= CMailBox::SoftMaxMail )
			{
				pUser->AddDefinedText( TID_GAME_MAILBOX_FULL, "%s", lpszReceiver );
				return;
			}

			CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nItem );
			if( pItemElem )
			{
				if( !IsUsableItem( pItemElem ) )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pItemElem->IsQuest() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pItemElem->IsBinds() )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}
				if( pUser->IsUsing( pItemElem ) )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_DO_USINGITEM ) );
					return;
				}
				const ItemProp* pProp	= pItemElem->GetProp();
				if( pProp->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild != 0 )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}

				if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				{
					pUser->AddDiagText( prj.GetText( TID_GAME_CANNOT_POST ) );
					return;
				}

				if( nItemNum <= 0 )			// hacking
					return;
				if( pItemElem->m_nItemNum < nItemNum )
					nItemNum	= pItemElem->m_nItemNum;
			}
			if( pUser->GetGold() < nPostGold + nGold )
			{
				pUser->AddDiagText( prj.GetText( TID_GAME_LACKMONEY ) );
				return;
			}

			pUser->AddGold( - nPostGold - nGold , TRUE );	// 사용료 지급
			
			CItemElem	itemElem;
			if( pItemElem )
			{
				itemElem	= *pItemElem;
				itemElem.m_nItemNum		= nItemNum;
				pUser->RemoveItem( (BYTE)pItemElem->m_dwObjId, nItemNum );
				CWorld* pWorld	= pUser->GetWorld();
				if( pWorld )
#ifdef __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
					g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
			}

// 			//	BEGINTEST
// 			Error( "OnQueryPostMail  Receiver[%d] Sender[%d]", idReceiver, pUser->m_idPlayer );

			g_dpDBClient.SendQueryPostMail( idReceiver, pUser->m_idPlayer, itemElem, nGold, lpszTitle, lpszText );
		}
		else
		{
			// input name is wrong
			pUser->AddDiagText(prj.GetText(TID_MAIL_UNKNOW));
		}
	}
	else
	{
// 		//	BEGINTEST
// 		Error( "OnQueryPostMail  pUser == NULL [%d]", dpidUser );
	}

}

void CDPSrvr::OnQueryRemoveMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryRemoveMail( pUser->m_idPlayer, nMail );
			}
			else
			{
				Error( "CDPSrvr::OnQueryRemoveMail - Invalid nMail. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			Error( "CDPSrvr::OnQueryRemoveMail - Invalid pMailBox. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryGetMailItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_Inventory.GetEmptyCount() < 1 )
		{
			pUser->AddDiagText(  prj.GetText( TID_GAME_LACKSPACE ) );
			return;
		}
		
		CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox )
		{
			CMail* pMail = pMailBox->GetMail(nMail);
			if( pMail && pMail->m_pItemElem )
			{
				// 기본 보관일수 지났는지를 검사하여 보관료 부과한다.
				const auto [nDay, dwTime] = pMail->GetMailInfo();

				// 기본 보관일수가 지났다!!!
				if( (MAX_KEEP_MAX_DAY*24) - dwTime > (MAX_KEEP_BASIC_DAY*24) )
				{
					FLOAT fCustody = 0.0f;
					FLOAT fPay = 0.0f;
					fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );
					fPay = pMail->m_pItemElem->GetCost() * fCustody;
					if( fPay < 0.0f )
						fPay = 0.0f;

					if( (int)fPay > pUser->GetGold() )
					{
						pUser->AddDiagText( prj.GetText( TID_GAME_LACKMONEY ) );
						return;
					}
					pUser->AddGold( -((int)fPay) );
				}
			}
		}
		g_dpDBClient.SendQueryGetMailItem( pUser->m_idPlayer, nMail );
	}
}

void CDPSrvr::OnQueryGetMailGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryGetMailGold( pUser->m_idPlayer, nMail );
			}
			else
			{
				Error( "CDPSrvr::OnQueryGetMailGold - Invalid nMail. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			Error( "CDPSrvr::OnQueryGetMailGold - Invalid pMailBox. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryReadMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long nMail;
	ar >> nMail;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
		if( pMailBox != NULL )
		{
			CMail* pMail = pMailBox->GetMail( nMail );
			if( pMail != NULL )
			{
				g_dpDBClient.SendQueryReadMail( pUser->m_idPlayer, nMail );
			}
			else
			{
				Error( "CDPSrvr::OnQueryReadMail - Invalid nMail. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
			}
		}
		else
		{
			Error( "CDPSrvr::OnQueryReadMail - Invalid pMailBox. idReceiver : %07d, nMail :[%d]", pUser->m_idPlayer, nMail );
		}
	}
}

void CDPSrvr::OnQueryMailBox( CAr & ar, CUser * pUser) {
	const int nClientReqCount = pUser->mailBoxRequest.GetCountClientReq();

	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox(pUser->m_idPlayer);
	if (pMailBox) {
		if (nClientReqCount >= 2) {
			g_dpDBClient.SendQueryMailBoxCount(pUser->m_idPlayer);
		} else {
			switch (pMailBox->m_nStatus) {
				case CMailBox::BoxStatus::nodata:
					g_dpDBClient.SendQueryMailBox(pUser->m_idPlayer);
					break;
				case CMailBox::BoxStatus::read:
					// Currently waiting for an answer from DBServer, do nothing
					break;
				case CMailBox::BoxStatus::data:
					pUser->AddMailBox(pMailBox);
					break;
			}
		}

		if (pMailBox->m_nStatus == CMailBox::BoxStatus::nodata) {
			pMailBox->m_nStatus = CMailBox::BoxStatus::read;
		}

	} else if (!pUser->mailBoxRequest.GetCheckTransMailBox()) {
		// The user's mailbox does not exist in the world server. request to DB
		g_dpDBClient.SendQueryMailBoxReq(pUser->m_idPlayer);
	} else {
		// DB already said they had no mailbox for this user
		pUser->SendCheckMailBoxReq(false);
	}

}

void CDPSrvr::OnGCApp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE nState;
	ar >> nState;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CGuild *pGuild = pUser->GetGuild();

		// 길드가 없거나 길드장이 아니면 신청 불가
		if( pGuild == NULL || pGuild->IsMaster( pUser->m_idPlayer ) == FALSE )
		{
			pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER ) );
			return;
		}

		if( nState == GC_IN_WINDOW )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
			{
				// 1:1길드대전에 입찰한 길드는 입찰 불가능하다.
				int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );
				if( nIndex != NULL_ID )
				{
					pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_ISGC1TO1TENDER );
					return;
				}
				pUser->AddGCWindow( g_GuildCombatMng.GetPrizePenya( 2 ), g_GuildCombatMng.GetRequstPenya( pUser->m_idGuild ), g_GuildCombatMng.m_nJoinPanya );
			}
		}
		else if( nState == GC_IN_APP )
		{
			DWORD dwPenya;
			ar >> dwPenya;
			g_GuildCombatMng.GuildCombatRequest( pUser, dwPenya );	// 
		}
	}
}
void CDPSrvr::OnGCCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		g_GuildCombatMng.GuildCombatCancel( pUser );
}
void CDPSrvr::OnGCRequestStatus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->AddGCRequestStatus( g_GuildCombatMng.GetPrizePenya( 2 ), g_GuildCombatMng.vecRequestRanking );
}

void CDPSrvr::OnGCSelectPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BOOL bWindow;
	ar >> bWindow;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if (!IsValidObj(pUser)) return;
	
	// 캐릭터를 선택할수 있는 시간인지 검사
	if (g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE) {
		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP)); //지금은 명단작성을 할 수 없습니다.		
		return;
	}

	// 캐릭터를 선택할수 있는 길드 인지 검사
	if (g_GuildCombatMng.IsRequestWarGuild(pUser->m_idGuild, FALSE) == FALSE) {
		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP_FAIL));	//길드대전 입찰을 하지 않았거나 최종 선발 길드순위에 들지 못했습니다.		
		return;
	}

	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	// 캐릭터를 선택할수 잇는것은 마스터와 킹핀급이다.
	BOOL bMK = FALSE;
	if (!pGuild) return;

	CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
	if (pGuildMember) {
		if (pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER)
			bMK = TRUE;
	}

	if (!bMK) return;
		
		
	if( bWindow == FALSE )
	{
		u_long uidDefender;
		int nSize;
		ar >> uidDefender;
		ar >> nSize;
		if( nSize > g_GuildCombatMng.m_nMaxJoinMember )
			return;

		std::vector< u_long > vecSelectPlayer;
		bool hasDefender = false;
		for( int i = 0 ; i < nSize ; ++i )
		{
			u_long uidPlayer;
			ar >> uidPlayer;
			CUser* pUsertmp = g_UserMng.GetUserByPlayerID( uidPlayer );
			if (!IsValidObj(pUsertmp)) continue;

			CGuildMember * pGuildMember = pGuild->GetMember(uidPlayer);
			if (!pGuildMember) continue;

			if (pUsertmp->GetLevel() < 30) continue;

			vecSelectPlayer.push_back(uidPlayer);
			if (uidPlayer == uidDefender) hasDefender = true;
		}

#ifdef _DEBUG
		hasDefender = true;
#endif // _DEBUG

		if (hasDefender) {
			g_GuildCombatMng.SelectPlayerClear(pUser->m_idGuild);

			for (const u_long uidSelectPlayer : vecSelectPlayer) {
				g_GuildCombatMng.AddSelectPlayer(pUser->m_idGuild, uidSelectPlayer);
			}

			g_GuildCombatMng.SetDefender( pUser->m_idGuild, uidDefender );			

			WriteLog( "GuildCombat SelectPlayer GuildID=%d", pUser->m_idGuild );
		}
	}

	std::vector<CGuildCombat::__JOINPLAYER> vecSelectList;
	g_GuildCombatMng.GetSelectPlayer( pUser->m_idGuild, vecSelectList );
	pUser->AddGCSelectPlayerWindow( vecSelectList, g_GuildCombatMng.GetDefender(pUser->m_idGuild), bWindow, g_GuildCombatMng.IsRequestWarGuild(pUser->m_idGuild, FALSE) );

	if( bWindow == FALSE )
	{
		g_UserMng.AddGCGuildStatus( pUser->m_idGuild );
		g_UserMng.AddGCWarPlayerlist( pUser->m_idGuild );
	}
}

void CDPSrvr::OnGCSelectMap( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nMap;
	ar >> nMap;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		g_GuildCombatMng.SetSelectMap( pUser, nMap );
}
void CDPSrvr::OnGCJoin( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_GUILDWAR_JOIN>(pUser) )
			return;
		g_GuildCombatMng.GuildCombatEnter( pUser );
	}
}
void CDPSrvr::OnGCGetPenyaGuild( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CGuild *pGuild = pUser->GetGuild();		
		if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			// 신청금액 및 보상이 있는가?
			__int64 nGetPenya = 0;
			BOOL bFind = FALSE;
			CGuildCombat::__GCRESULTVALUEGUILD ResultValueGuild;
			for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValueGuild.size() ) ; ++veci )
			{
				ResultValueGuild = g_GuildCombatMng.m_GCResultValueGuild[veci];
				if( pGuild->m_idGuild == ResultValueGuild.uidGuild )
				{
					bFind = TRUE;
					nGetPenya = ResultValueGuild.nReturnCombatFee + ResultValueGuild.nReward;
					break;
				}
			}

			if( bFind )
			{
				__int64 nTotal = (__int64)pUser->GetGold() + nGetPenya;
				if( nGetPenya > INT_MAX || nTotal > INT_MAX )
					pUser->AddGCGetPenyaGuild( 3, nGetPenya );
				else
					g_dpDBClient.SendGCGetPenyaGuild( pUser->m_idPlayer, ResultValueGuild.nCombatID, ResultValueGuild.uidGuild );
			}
			else
			{
				pUser->AddGCGetPenyaGuild( 2, nGetPenya );
			}
		}
	}
}

void CDPSrvr::OnGCGetPenyaPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		// 보상이 있는가?
		__int64 nGetPenya = 0;
		BOOL bFind = FALSE;
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuePlayer;
		for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValuePlayer.size() ) ; ++veci )
		{
			ResultValuePlayer = g_GuildCombatMng.m_GCResultValuePlayer[veci];
			if( pUser->m_idPlayer == ResultValuePlayer.uidPlayer )
			{
				bFind = TRUE;
				nGetPenya = ResultValuePlayer.nReward;
				break;
			}
		}

		if( bFind )
		{
			__int64 nTotal = (__int64)pUser->GetGold() + nGetPenya;
			if( nGetPenya > INT_MAX || nTotal > INT_MAX )
				pUser->AddGCGetPenyaPlayer( 2, nGetPenya );
			else
				g_dpDBClient.SendGCGetPenyaPlayer( pUser->m_idPlayer, ResultValuePlayer.nCombatID, ResultValuePlayer.uidGuild );
		}
		else
		{
			pUser->AddGCGetPenyaPlayer( 1, nGetPenya );
		}
	}
}

void CDPSrvr::OnGCTele( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if (g_WorldMng.GetRevivalPos(WI_WORLD_MADRIGAL, "flaris")) {
			pUser->Replace(WI_WORLD_MADRIGAL, D3DXVECTOR3(6983.0f, 0.0f, 3330.0f), REPLACE_NORMAL, nDefaultLayer);
		}
	}
}
void CDPSrvr::OnGCPlayerPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->AddGCPlayerPoint();
}

void CDPSrvr::OnSummonFriend( CAr & ar, CUser * pUser )
{
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	ar >> dwData >> lpszPlayer;

	if( strlen(lpszPlayer) >= MAX_NAME )
		return;
	
	CUser::SummonState nState = pUser->GetSummonState();
	if (nState != CUser::SummonState::Ok_0) {
		const DWORD dwMsgId = CUser::GetSummonStateTIdForMyself(nState);
		pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
		return;
	}

	WORD wId	= LOWORD( dwData );
	CItemElem* pItemElem = pUser->GetItemId( wId );
	if (!IsUsableItem(pItemElem)) {
		ItemProp * pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_FRIENDSUMMON_A);
		if (pItemProp)
			pUser->AddDefinedText(TID_ERROR_SUMMONFRIEND_NOITEM, "\"%s\" \"%s\"", pItemProp->szName, lpszPlayer);
		return;
	}

	if( pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_A
		&& pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_B )
		return;

	if( pItemElem->m_bQuery )
		return;

	CUser * pUsertmp = g_UserMng.GetUserByPlayerID(CPlayerDataCenter::GetInstance()->GetPlayerId(lpszPlayer));
	if (!IsValidObj(pUsertmp)) {
		pUser->AddDefinedText(TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer);
		return;
	}

	nState = pUsertmp->GetSummonState();

	if( nState != CUser::SummonState::Ok_0)
	{
		const DWORD dwMsgId = CUser::GetSummonStateTIdForOther(nState);
		pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
	}
	else if( pUser->m_idPlayer == pUsertmp->m_idPlayer )
	{
		pUser->AddDefinedText( TID_GAME_SUMMON_FRIEND_MY_NOUSE );
	}
	else if( prj.IsGuildQuestRegion( pUser->GetPos() ) )
	{
		pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
	}
	else if( prj.IsGuildQuestRegion( pUsertmp->GetPos() ) )
	{
		pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
	}
	else if( pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID()
			|| pUser->GetLayer() != pUsertmp->GetLayer()
		)
	{
		CString strtmp;
		strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName );
		pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", strtmp );
	}
	else if( CRainbowRaceMng::GetInstance()->IsEntry( pUser->m_idPlayer )
			|| CRainbowRaceMng::GetInstance()->IsEntry( pUsertmp->m_idPlayer ) )
	{
		pUser->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
		return;
	}
#ifdef __QUIZ
	else if( pUser->GetWorld()->GetID() == WI_WORLD_QUIZ 
			|| pUsertmp->GetWorld()->GetID() == WI_WORLD_QUIZ )
		pUser->AddDefinedText( TID_GAME_QUIZ_DO_NOT_USE );
						
#endif // __QUIZ
	else
	{
		if( pUsertmp->m_RTMessenger.IsBlock( pUser->m_idPlayer ) )
		{
			pUser->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", lpszPlayer );
			return;
		}
		pItemElem->m_bQuery		= TRUE;
		pUsertmp->AddSummonFriendConfirm( pUser->GetId(), dwData, pUser->GetName(), pUser->GetWorld()->m_szWorldName );
		pUser->AddDefinedText( TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", lpszPlayer );
	}


}
void CDPSrvr::OnSummonFriendConfirm( CAr & ar, CUser * pUser )
{
	OBJID objid;
	DWORD dwData;
	ar >> objid >> dwData;

	CUser* pUsertmp = prj.GetUser( objid );
	if (!IsValidObj(pUsertmp)) {
		return;
	}

	CUser::SummonState nState = pUser->GetSummonState();
	if (nState != CUser::SummonState::Ok_0) {
		const DWORD dwMsgId = CUser::GetSummonStateTIdForMyself(nState);
		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(dwMsgId));
		pUsertmp->AddDefinedText(TID_GAME_STATE_NOTSUMMON, "\"%s\"", prj.GetText(dwMsgId + 1));
		return;
	}

	nState = pUsertmp->GetSummonState();
	if (nState != CUser::SummonState::Ok_0) {
		const DWORD dwMsgId = CUser::GetSummonStateTIdForOther(nState);

		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(dwMsgId));
		pUsertmp->AddDefinedText(TID_GAME_STATE_NOTSUMMON, "\"%s\"", prj.GetText(dwMsgId - 1));
		return;
	}

			WORD wId	= LOWORD( dwData );
			CItemElem* pItemElem = pUsertmp->GetItemId( wId );
			if( IsUsableItem( pItemElem ) )
			{
				if( ( pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_A && pItemElem->m_dwItemId != II_SYS_SYS_SCR_FRIENDSUMMON_B ) || pItemElem->m_bQuery == FALSE )
					return;

				if( prj.IsGuildQuestRegion( pUser->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
				}
				else if( prj.IsGuildQuestRegion( pUsertmp->GetPos() ) )
				{
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE1 ) );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON , "\"%s\"", prj.GetText( TID_GAME_EVENT_WORLD_NOTUSE ) );
				}
				else if( pUser->GetWorld()->GetID() != pUsertmp->GetWorld()->GetID()
						|| pUser->GetLayer() != pUsertmp->GetLayer()
					)
				{
					CString strtmp;
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUser->GetWorld()->m_szWorldName, pUsertmp->GetWorld()->m_szWorldName );
					pUser->AddDefinedText( TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp );
					strtmp.Format( prj.GetText( TID_GAME_WORLD_NOTUSE ), pUsertmp->GetWorld()->m_szWorldName, pUser->GetWorld()->m_szWorldName );
					pUsertmp->AddDefinedText( TID_GAME_STATE_NOTSUMMON, "\"%s\"", strtmp );
				}
#ifdef __QUIZ
				else if( pUser->GetWorld()->GetID() == WI_WORLD_QUIZ 
						|| pUsertmp->GetWorld()->GetID() == WI_WORLD_QUIZ )
					pUser->AddDefinedText( TID_GAME_QUIZ_DO_NOT_USE );
#endif // __QUIZ
				else 
				{
					g_dpDBClient.SendLogSMItemUse( "1", pUsertmp, pItemElem, pItemElem->GetProp(), pUser->GetName() );

					pUser->Replace(*pUsertmp, REPLACE_NORMAL);

					pItemElem->m_bQuery		= FALSE;
					pUsertmp->RemoveItem( (BYTE)( wId ), (short)1 );
					pUser->AddDefinedText( TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pUsertmp->GetName() );
					pUsertmp->AddDefinedText( TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName() );
				}
			}
			else
			{
				ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_FRIENDSUMMON_A );
				if( pItemProp )
					pUsertmp->AddDefinedText( TID_ERROR_SUMMONFRIEND_NOITEM, "\"%s\" \"%s\"", pItemProp->szName, pUser->GetName() );
			}


}

void CDPSrvr::OnSummonFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	DWORD dwData;
	ar >> objid >> dwData;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CUser* pUsertmp = prj.GetUser( objid );
		if( IsValidObj( (CObj*)pUsertmp ) )
		{
			WORD wId	= LOWORD( dwData );
			CItemElem* pItemElem = pUsertmp->GetItemId( wId );
			if( IsUsableItem( pItemElem ) && ( pItemElem->m_dwItemId == II_SYS_SYS_SCR_FRIENDSUMMON_A || pItemElem->m_dwItemId == II_SYS_SYS_SCR_FRIENDSUMMON_B ) )
				pItemElem->m_bQuery		= FALSE;
			pUsertmp->AddDefinedText( TID_GAME_SUMMON_FRIEND_CANCEL, "\"%s\"", pUser->GetName() );
		}
	}
}

void CDPSrvr::OnSummonParty( CAr & ar, CUser * pUser )
{
	DWORD dwData; ar >> dwData;

	const CUser::SummonState nState = pUser->GetSummonState();
	if( nState != CUser::SummonState::Ok_0 )
	{
		const DWORD dwMsgId = CUser::GetSummonStateTIdForMyself(nState);
		pUser->AddDefinedText( TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText( dwMsgId ) );
		return;
	}

	if (prj.IsGuildQuestRegion(pUser->GetPos())) {
		pUser->AddDefinedText(TID_GAME_STATE_NOTUSE, "\"%s\"", prj.GetText(TID_GAME_EVENT_WORLD_NOTUSE));
		return;
	}

	if (CRainbowRaceMng::GetInstance()->IsEntry(pUser->m_idPlayer)) {
		pUser->AddDefinedText(TID_GAME_RAINBOWRACE_NOTELEPORT);
		return;
	}
	
	const WORD wId	= LOWORD( dwData );
	CItemElem * const pItemElem = pUser->GetItemId( wId );

	if (!IsUsableItem(pItemElem)) {
		const ItemProp * pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_PARTYSUMMON);
		if (pItemProp)
			pUser->AddDefinedText(TID_ERROR_SUMMONPARTY_NOITEM, "\"%s\"", pItemProp->szName);
		return;
	}

	CParty * pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
	if (!pParty) return;
	if (!pParty->IsLeader(pUser->m_idPlayer)) return;

	if (pUser->HasBuff(BUFF_ITEM, (WORD)(pItemElem->m_dwItemId))) {
		pUser->AddDefinedText(TID_GAME_LIMITED_USE);
		return;
	}

	if (pItemElem->m_dwItemId != II_SYS_SYS_SCR_PARTYSUMMON) {
		return;
	}

	pParty->m_dwWorldId = pUser->GetWorld()->GetID();

	g_dpDBClient.SendLogSMItemUse("1", pUser, pItemElem, pItemElem->GetProp(), pParty->m_sParty);

	ItemProp * pItemProptmp = prj.GetItemProp(II_SYS_SYS_SCR_PARTYSUMMON);

	for (int i = 1; i < pParty->m_nSizeofMember; i++) {
		CUser * pUsertmp = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(i));
		if (IsValidObj(pUsertmp)) {
			pUser->DoApplySkill(pUsertmp, pItemProptmp, NULL);
			pUsertmp->AddSummonPartyConfirm(pUser->GetId(), dwData, pUser->GetWorld()->m_szWorldName);
			if (pItemProptmp)
				g_UserMng.AddCreateSfxObj(pUsertmp, pItemProptmp->dwSfxObj3);
			pUser->AddDefinedText(TID_GAME_SUMMONFRIEND_CONFIRM, "\"%s\"", pUsertmp->GetName());
		} else {
			pUser->AddDefinedText(TID_ERROR_SUMMONFRIEND_NOUSER, "\"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString(pParty->GetPlayerId(i)));
		}
	}

	pUser->DoApplySkill(pUser, pItemProptmp, NULL);
	pUser->RemoveItem((BYTE)(wId), 1);

	if (pItemProptmp) {
		g_UserMng.AddCreateSfxObj(pUser, pItemProptmp->dwSfxObj3);
	}
}

void CDPSrvr::OnSummonPartyConfirm( CAr & ar, CUser * pUser )
{
	OBJID objid;
	DWORD dwData;
	ar >> objid;
	ar >> dwData;

	CParty* pParty = g_PartyMng.GetParty( pUser->GetPartyId() );

	if (!pParty) {
		pUser->AddDefinedText(TID_GAME_NOPARTY);
		return;
	}

	CUser * pLeader = prj.GetUser( objid );
	if (!(IsValidObj(pLeader)
		&& pParty->IsLeader(pLeader->m_idPlayer)
		&& pLeader->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON))) {
		pUser->AddDefinedText(TID_ERROR_SUMMONPARTY_NOTTIME);
		return;
	}

	if (pUser == pLeader) {
		return;
	}

	CUser::SummonState nState = pUser->GetSummonState();
	if (nState != CUser::SummonState::Ok_0 && nState != CUser::SummonState::Fly_5) {
		const DWORD dwMsgId = CUser::GetSummonStateTIdForMyself(nState);
		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(dwMsgId));
		return;
	}
				
	nState = pLeader->GetSummonState();
	if (nState != CUser::SummonState::Ok_0) {
		const DWORD dwMsgId = CUser::GetSummonStateTIdForOther(nState);
		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(dwMsgId));
		return;
	}
				
	if (prj.IsGuildQuestRegion(pLeader->GetPos())) {
		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(TID_GAME_EVENT_WORLD_NOTUSE1));
	} else if (prj.IsGuildQuestRegion(pUser->GetPos())) {
		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(TID_GAME_EVENT_WORLD_NOTUSE));
	} else if (pUser->GetWorld()->GetID() != pParty->m_dwWorldId) {
		CWorld * pWorld = g_WorldMng.GetWorld(pParty->m_dwWorldId);
		if (pWorld) {
			CString strtmp;
			strtmp.Format(prj.GetText(TID_GAME_WORLD_NOTUSE), pUser->GetWorld()->m_szWorldName, pWorld->m_szWorldName);
			pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", strtmp);
		}
	} else if (pParty->m_dwWorldId != pLeader->GetWorld()->GetID()) {
		CWorld * pWorld = g_WorldMng.GetWorld(pParty->m_dwWorldId);
		if (pWorld)
		{
			pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(TID_GAME_WORLDLEADER_NOTUSE));
		}
	} else if (pLeader->GetLayer() != pUser->GetLayer()) {
		pUser->AddDefinedText(TID_GAME_STATE_NOTSUMMONOK, "\"%s\"", prj.GetText(TID_GAME_WORLD_NOTUSE));
	} else if (CRainbowRaceMng::GetInstance()->IsEntry(pUser->m_idPlayer)) {
		pUser->AddDefinedText(TID_GAME_RAINBOWRACE_NOTELEPORT);
	} else if (pLeader->GetWorld()->GetID() == WI_WORLD_QUIZ
		|| pUser->GetWorld()->GetID() == WI_WORLD_QUIZ) {
		pUser->AddDefinedText(TID_GAME_QUIZ_DO_NOT_USE);
	} else {
		pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON);
		pUser->Replace(*pLeader, REPLACE_FORCE);
		pUser->AddDefinedText(TID_GAME_SUMMON_SUCCESS1, "\"%s\"", pLeader->GetName());
		pLeader->AddDefinedText(TID_GAME_SUMMON_SUCCESS, "\"%s\"", pUser->GetName());
		ItemProp * pItemProptmp = prj.GetItemProp(II_SYS_SYS_SCR_PARTYSUMMON);
		if (pItemProptmp)
			g_UserMng.AddCreateSfxObj(pUser, pItemProptmp->dwSfxObj3);
	}
}

void CDPSrvr::OnRemoveInvenItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD	dwId;
	int		nNum;
	ar >> dwId;
	ar >> nNum;

	if( nNum <= 0 )
		return;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwId );
	if( !IsUsableItem( pItemElem ) )
		return;

	if( pUser->m_Inventory.IsEquip( dwId ) )
		return;
	
	if( pUser->IsUsing( pItemElem ) )
	{
		pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
		return;
	}
	
	if( pItemElem->m_nItemNum < nNum )
		return ;

	if( pItemElem->IsUndestructable() )
		return;

	CString strNum;
	strNum.Format("%d", nNum );
	pUser->AddDefinedText( TID_GAME_SUCCESS_REMOVE_ITEM, "\"%s\" \"%s\"", pItemElem->GetProp()->szName, strNum );

	LogItemInfo aLogItem;
	aLogItem.Action = "*";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "GARBAGE";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	OnLogItem( aLogItem, pItemElem, nNum );

	pUser->RemoveItem( (BYTE)( dwId ), (short)nNum );
}

void CDPSrvr::OnCreateMonster( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwItemIdRec;
	D3DXVECTOR3 vPos;
	ar >> dwItemIdRec;
	ar >> vPos;

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	DWORD dwId = HIWORD( dwItemIdRec );
	
	if( pUser->IsUsableState( dwId ) == FALSE )
		return;
	
	CCreateMonster::GetInstance()->CreateMonster( pUser, dwId, vPos );

}

void CDPSrvr::OnFoucusObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objid;
	ar >> objid;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->AddFocusObj(objid);
}

void CDPSrvr::OnTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidTrader;
	ar >> objidTrader;

	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && pUser->m_vtInfo.GetOther() == NULL )
	{
		CMover* pTrader		= prj.GetMover( objidTrader );
		if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		{
			if( pTrader->IsPlayer() )	// pc
			{

				if( 0 < pUser->m_nDuel ||  0 < pTrader->m_nDuel )
				{
					return;
				}


				//개인상점 중에는 거래 불가 
				if( pUser->m_vtInfo.VendorIsVendor() || pUser->m_vtInfo.IsVendorOpen() ||
                    pTrader->m_vtInfo.VendorIsVendor() || pTrader->m_vtInfo.IsVendorOpen() )
				{
					return;	//
				}
#ifdef __S_SERVER_UNIFY
				if( pUser->m_bAllAction == FALSE || ((CUser*)pTrader)->m_bAllAction == FALSE )
					return;
#endif // __S_SERVER_UNIFY

				if( pUser->m_bBank || pTrader->m_bBank )
				{
					pUser->AddDefinedText( TID_GAME_TRADELIMITPC, "" );
					return;	//
				}

				if( pUser->m_bGuildBank || pTrader->m_bGuildBank )
				{
					pUser->AddDefinedText( TID_GAME_TRADELIMITPC, "" );
					return;	//
				}

				pUser->m_vtInfo.SetOther( pTrader );
				pTrader->m_vtInfo.SetOther( pUser );

				pUser->AddTrade( (CUser*)pTrader, pUser->m_idPlayer );
				( (CUser*)pTrader )->AddTrade( pUser, pUser->m_idPlayer );
			}
		}
	}
}

void CDPSrvr::OnConfirmTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidTrader;
	ar >> objidTrader;

	CWorld* pWorld;
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && pUser->m_vtInfo.GetOther() == NULL )
	{
		// 대전장에서는 거래를 할수 없습니다.
		if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
		{			
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}

		CMover* pTrader		= prj.GetMover( objidTrader );
		if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		{
			if( pTrader->IsPlayer() == FALSE )
				return;

			if( 0 < pUser->m_nDuel ||  0 < pTrader->m_nDuel )
			{
				return;
			}

			if( pTrader->IsAttackMode() )
				pUser->AddDefinedText( TID_GAME_BATTLE_NOTTRADE, "" );
			else
				((CUser*)pTrader)->AddComfirmTrade( pUser->GetId() );
		}

		pUser->RemoveInvisible();		// 거래를 하면 투명은 풀린다.
	}
}

void CDPSrvr::OnConfirmTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidTrader;
	ar >> objidTrader;

	CMover* pTrader		= prj.GetMover( objidTrader );
	if( IsValidObj( pTrader ) && pTrader->GetWorld() && pTrader->m_vtInfo.GetOther() == NULL )
		( (CUser*)pTrader )->AddComfirmTradeCancel( objidTrader );
}

void CDPSrvr::OnTradePut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE i, nItemType, nId;
	short nItemNum;

	ar >> i >> nItemType >> nId >> nItemNum;
	if( i >= MAX_TRADE || nItemNum < 1 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		CItemElem* pItem	= pUser->GetItemId( nId );
		if( pItem && pItem->IsFlag( CItemElem::expired ) )
			return;

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			short nItemNumResult	= nItemNum;
			DWORD dwText	= pUser->m_vtInfo.TradeSetItem2( nId, i, nItemNumResult );
			if( dwText == 0 )
			{
				pUser->AddTradePut_Me( i, nItemType, nId, nItemNumResult );
				( (CUser*)pTrader )->AddTradePut_Them( pUser->GetId(), i, pItem, nItemNumResult );
			}
			else
			{
				pUser->AddDefinedText( dwText );
			}
		}
		else
		{
			pUser->AddTradePutError();
		}
	}
}

void CDPSrvr::OnTradePull( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE i;
	ar >> i;

	if( i >= MAX_TRADE )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{

		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			if( pUser->m_vtInfo.TradeClearItem( i ) )
			{
				pUser->AddTradePull( pUser->GetId(), i );
				( (CUser*)pUser->m_vtInfo.GetOther() )->AddTradePull( pUser->GetId(), i );
			}
		}
	}
}

void CDPSrvr::OnTradePutGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwGold;
	ar >> dwGold;
	
	int nGold = dwGold;
	if( nGold <= 0 )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			if( nGold > pUser->GetGold() )
				nGold = pUser->GetGold();
			
			pUser->m_vtInfo.TradeSetGold( nGold );
			pUser->AddGold( -nGold, FALSE );	// raiders.2006.11.28  인벤돈 = 인벤돈 - 거래창 돈 

			pUser->AddTradePutGold( pUser->GetId(), nGold );
			( (CUser*)pTrader )->AddTradePutGold( pUser->GetId(), nGold );
		}
	}
}
//raiders.2006.11.28
/*
void CDPSrvr::OnTradeClearGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM && pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			pUser->m_vtInfo.TradeSetGold( 0 );

			pUser->AddTradeClearGold( pUser->GetId() );
			( (CUser*)pTrader )->AddTradeClearGold( pUser->GetId() );
		}
	}
}
*/
void CDPSrvr::OnTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nMode;
	ar >> nMode;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) &&
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		pUser->m_vtInfo.TradeClear();
		pTrader->m_vtInfo.TradeClear();

		pUser->AddTradeCancel( pUser->GetId(), pUser->m_idPlayer, nMode );
		( (CUser*)pTrader )->AddTradeCancel( pUser->GetId(), pUser->m_idPlayer, nMode );
	}
}


// pPlayer는 지금 ok를 누른 사용자이고 pTrdaer는 먼저 ok를 누른 사용자이다.	
void CDPSrvr::OnTradelastConfrim( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );	
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->m_vtInfo.TradeGetState() != TRADE_STEP_OK )
			return;

		switch( pTrader->m_vtInfo.TradeGetState() )
		{
		case TRADE_STEP_OK:
			pUser->m_vtInfo.TradeSetState( TRADE_STEP_CONFIRM );	

			pUser->AddTradelastConfirmOk( pUser->GetId() );					// ok를 눌렀음을 표시하게 한다.
			( (CUser*)pTrader )->AddTradelastConfirmOk( pUser->GetId() );	// ok를 눌렀음을 표시하게 한다.
			break;

		case TRADE_STEP_CONFIRM:
			{
				BEFORESENDDUAL( out, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN );		// log용 ar
				TRADE_CONFIRM_TYPE type = pUser->m_vtInfo.TradeLastConfirm( out );
				switch( type )
				{
				case TRADE_CONFIRM_ERROR:	
					pUser->AddTradeCancel( NULL_ID, pUser->m_idPlayer );	
					( (CUser*)pTrader )->AddTradeCancel( NULL_ID, pUser->m_idPlayer );
					break;
				case TRADE_CONFIRM_OK:
					SEND( out, &g_dpDBClient, DPID_SERVERPLAYER );

					pUser->AddTradeConsent();	
					( (CUser*)pTrader )->AddTradeConsent();
					break;
				}					
			}
			break;	
		} 
	}
}

// 사용자가 ok 버튼을 눌렀을 때
void CDPSrvr::OnTradeOk( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	CMover* pTrader	= ( pUser ? pUser->m_vtInfo.GetOther() : NULL );
	CWorld* pWorld;

	if( IsValidObj( pUser ) && ( pWorld = pUser->GetWorld() ) && 
		IsValidObj( pTrader ) && pTrader->GetWorld() == pWorld && pTrader->m_vtInfo.GetOther() == pUser )
	{
		if( pUser->m_vtInfo.TradeGetState() == TRADE_STEP_ITEM )
		{
			pUser->m_vtInfo.TradeSetState( TRADE_STEP_OK ); 

			if( pTrader->m_vtInfo.TradeGetState() == TRADE_STEP_OK )	// 상대가 먼저 ok를 눌러서 교환이 성립되는 경우
			{
				( (CUser*)pUser )->AddTradelastConfirm();
				( (CUser*)pTrader )->AddTradelastConfirm();
			}
			else	// 내가 먼저 ok 버튼을 누른 경우
			{
				pUser->AddTradeOk( pUser->GetId() );				// 클라에 전송해 ok를 눌렀음을 표시하게 한다.
				( (CUser*)pTrader )->AddTradeOk( pUser->GetId() );	// 클라에 전송해 ok를 눌렀음을 표시하게 한다.
			}
		}
	}
}

void CDPSrvr::OnPVendorOpen( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( uBufSize > 55 )	// 4 + 4 + 48 - 1		= 55
		return;

	char szPVendor[MAX_VENDORNAME];	// 개인 상점 이름( 48 )
	ar.ReadString( szPVendor, MAX_VENDORNAME );	

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
/*		
#ifdef __Y_BEAUTY_SHOP_CHARGE
		if( ::GetLanguage() == LANG_TWN )
		{
			if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_VENDOR ) ) )
			{
				return;
			}
		}
#endif //__Y_BEAUTY_SHOP_CHARGE
*/

#ifdef __RULE_0615
		if (prj.nameValider.IsNotAllowedVendorName(szPVendor)) {
			pUser->AddDiagText(prj.GetText(TID_DIAG_0020));
			return;
		}
#endif	// __RULE_0615

		if( !( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT ) ) )
			ParsingEffect(szPVendor, strlen(szPVendor) );
		
		// 대전장에서는 개인상점을 열수 없습니다.
		CWorld* pWorld = pUser->GetWorld();
		if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
		{			
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_TRADE) ); //길드대전장 에서는 거래에 관한 모든것들을 이용 할 수 없습니다.
			return;
		}
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
			return;
		if( pUser->m_vtInfo.GetOther() )	// 거래중 이면 개인상점 불가 
			return;
		if( 0 < pUser->m_nDuel )
		{
			return;
		}

		if( pUser->IsAttackMode() )
			return;

		pUser->m_vtInfo.SetTitle( szPVendor );

		if( pUser->m_pActMover->IsFly() )
			return;

		if( pUser->IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( pUser->GetPKPropensity() );
			if( !Propensity.nVendor )
			{
				pUser->AddDefinedText( TID_GAME_CHAOTIC_NOT_VENDOR );
				return;
			}
		}

#ifdef __S_SERVER_UNIFY
		if( pUser->m_bAllAction == FALSE )
			return;
#endif // __S_SERVER_UNIFY

		if( pUser->m_vtInfo.IsVendorOpen() )
		{
			if( pUser->m_vtInfo.VendorIsVendor() )	
			{
				g_ChattingMng.NewChattingRoom( pUser->m_idPlayer );
				CChatting * pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idPlayer );
				
				g_UserMng.AddPVendorOpen( pUser );
				if( pChatting )
				{
					pChatting->m_bState		= TRUE;
					if( pChatting->AddChattingMember( pUser->m_idPlayer ) )
						pUser->m_idChatting		= pUser->m_idPlayer;
					pUser->AddNewChatting( pChatting );
				}
				pUser->m_dwHonorCheckTime = GetTickCount();
			}
		}
	}
}

void CDPSrvr::OnPVendorClose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidVendor;
	ar >> objidVendor;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ClosePVendor( pUser, objidVendor );
	}	
}

BOOL CDPSrvr::ClosePVendor( CUser* pUser, OBJID objidVendor )
{
	if( pUser->GetId() == objidVendor )
	{
		if( pUser->m_vtInfo.IsVendorOpen() == FALSE && 
			!pUser->m_vtInfo.VendorIsVendor() )
		{
			return FALSE;
		}
		
		CMover* pTrader	= pUser->m_vtInfo.GetOther();   //raiders.2006.11.27

		pUser->m_vtInfo.VendorClose();
		
		//raiders.2006.11.27
		pUser->m_vtInfo.TradeClear();
		if( pTrader )
			pTrader->m_vtInfo.TradeClear();
		//--

		g_UserMng.AddPVendorClose( pUser );

		CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
		if( pChatting )
		{
			for (const auto & m : pChatting->GetMembers()) {
				CUser * pUserBuf = prj.GetUserByID(m.m_playerId);
				if (IsValidObj(pUserBuf)) {
					// 채팅방이 없어짐
					pUserBuf->AddDeleteChatting();
					pUserBuf->m_idChatting = 0;
				}
			}
		}
		g_ChattingMng.DeleteChattingRoom( pUser->m_idPlayer );
		pUser->m_idChatting		= 0;
	}
	else
	{
		if( IsValidObj( pUser->m_vtInfo.GetOther() ) )
		{
			pUser->m_vtInfo.SetOther( NULL );
			pUser->AddPVendorClose( objidVendor );

			CChatting* pChatting	= g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
			if( pChatting )
			{
				for (const auto & m : pChatting->GetMembers()) {
					CUser * pUserBuf = prj.GetUserByID(m.m_playerId);
					if( IsValidObj( pUserBuf ) )		// 채팅에서 나감
					{							
						pUserBuf->AddRemoveChatting( pUser->m_idPlayer );
					}
				}

				pChatting->RemoveChattingMember( pUser->m_idPlayer );
				pUser->m_idChatting		= 0;
			}
		}
	}

	return TRUE;
}

void CDPSrvr::OnBuyPVendorItem(CAr & ar, CUser & pUser) {
	const auto [objidVendor, nItem, dwItemId, nNum] = ar.Extract<OBJID, BYTE, DWORD, short>();
	
	if( nItem >= MAX_VENDITEM || nNum <= 0 )
		return;

	CUser* pPVendor	= prj.GetUser( objidVendor );
	if (!IsValidObj(pPVendor)) return;

	CVTInfo::VENDOR_SELL_RESULT result = pPVendor->m_vtInfo.VendorSellItem( &pUser, nItem, dwItemId, nNum );
	if (!result.isOk) {
		if (result.nErrorCode) {
			pUser.AddDefinedText(result.nErrorCode, "");
		}

		return;
	}

	LogItemInfo info;

	info.Action = "Z";
	info.SendName = pUser.GetName();
	info.RecvName = pPVendor->GetName();
	info.WorldId = pUser.GetWorld()->GetID();
	info.Gold = pUser.GetGold() + ( result.item.m_nCost * nNum );
	info.Gold2 = pUser.GetGold();
	info.Gold_1 = pPVendor->GetGold();
	OnLogItem( info, &result.item, nNum );

	info.Action = "X";
	info.SendName = pPVendor->GetName();
	info.RecvName = pUser.GetName();
	info.WorldId = pPVendor->GetWorld()->GetID();
	info.Gold = pPVendor->GetGold() - ( result.item.m_nCost * nNum );
	info.Gold2 = pPVendor->GetGold();
	info.Gold_1 = pUser.GetGold();
	OnLogItem( info, &result.item, nNum );
}

void CDPSrvr::OnQueryPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidVendor;
	ar >> objidVendor;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_pActMover->IsFly() )
			return;

		CUser* pPVendor	= prj.GetUser( objidVendor );
		if( IsValidObj( pPVendor ) )
		{
			BOOL bChatting = TRUE;
			CChatting* pChatting = g_ChattingMng.GetChttingRoom( pPVendor->m_idChatting );
			if( pChatting )
				bChatting = pChatting->m_bState;

			if( pPVendor->m_vtInfo.IsVendorOpen() )
			{
				pUser->m_vtInfo.SetOther( pPVendor );
				pUser->AddPVendorItem( pPVendor, bChatting );
			}
		}

	}
}

void CDPSrvr::OnUnregisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE i;
	ar >> i;
	if( i >= MAX_VENDITEM )
		return;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.IsVendorOpen() )
			return;

		if( pUser->m_vtInfo.VendorClearItem( i ) )
			pUser->AddUnregisterPVendorItem( i );
	}
}

void CDPSrvr::OnRegisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	BYTE iIndex, nType, nId;
	short nNum;
	int nCost;
	ar >> iIndex >> nType >> nId >> nNum >> nCost;

	if( iIndex >= MAX_VENDOR_REVISION )
		return;
	if( nCost < 1 )	
		nCost = 1;

	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->m_vtInfo.GetOther() )
			return;
		if( pUser->m_vtInfo.IsVendorOpen() )
			return;

		if( pUser->IsRegionAttr( RA_FIGHT ) )
		{
			pUser->AddDefinedText( TID_GAME_FAIL_TO_OPEN_SHOP );
			return;
		}

		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
			return;

		if( CNpcChecker::GetInstance()->IsCloseFromAnyNpc(pUser) )
		{
			// NPC근처 개인 상점 불가(3m) - 시도 시 확인창이 생성 되도록 처리
			pUser->AddDiagText( prj.GetText( TID_GAME_NPC_RADIUS ) );
//			pUser->AddDefinedText( TID_GAME_NPC_RADIUS );
			return;
		}

#ifdef __QUIZ
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		{
			pUser->AddDefinedText( TID_GAME_FAIL_TO_OPEN_SHOP );
			return;
		}
#endif // __QUIZ

		CItemElem* pItemElem = pUser->GetItemId( nId );
		if( IsUsableItem( pItemElem ) )		
		{
			if( pItemElem->IsQuest() )
				return;

			if( pItemElem->IsBinds() )
				return;
			if( pUser->IsUsing( pItemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_CANNOT_DO_USINGITEM );
				return;
			}
			
			ItemProp* pProp	= pItemElem->GetProp();
			if( pProp->dwItemKind3 == IK3_CLOAK  && pItemElem->m_idGuild != 0 )
				return;

//			if( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == II_RID_RID_BOR_LADOLF )
//				return;
			if( pProp->dwParts == PARTS_RIDE && pProp->dwItemJob == JOB_VAGRANT )
				return;

			if( pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )
				return;

			if( pItemElem->IsFlag( CItemElem::expired ) )
				return;
			/*
			if(pProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet) //사망한 펫은 거래 불가
			{
				if(pItemElem->m_pPet->GetLife() <= 0)
					return;
			}
			*/
			
			if( nNum > pItemElem->m_nItemNum )
				nNum = pItemElem->m_nItemNum;
			if( nNum < 1 )	
				nNum = 1;

			ItemProp* pItemProp	= pItemElem->GetProp();


			pUser->m_vtInfo.VendorSetItem( nId, iIndex, nNum, nCost );
			pUser->AddRegisterPVendorItem( iIndex, 0, nId, nNum, nCost );
		}
	}
}

void CDPSrvr::OnCreateAngel( CAr & ar, CUser & pUser )
{
	// $
	if (CNpcChecker::GetInstance()->IsCloseNpc<MMI_SUMMON_ANGEL>(&pUser)) return;
	if (CItemUpgrade::IsInTrade(pUser)) return;

	static constexpr size_t MAX_SENT_ITEMS = 20;

	// ~~ Receive input
	size_t numberOfValues; ar >> numberOfValues;
	if (numberOfValues > MAX_SENT_ITEMS) return;

	boost::container::static_vector<ItemPos, MAX_SENT_ITEMS> sentItems;
	for (size_t i = 0; i != numberOfValues; ++i) {
		ItemPos pos; ar >> pos;
		sentItems.emplace_back(pos);
	}


	// ~~ What was sent?
	struct Detail { CItemElem * item; unsigned int quantity; };
	boost::container::flat_map<ItemPos, Detail> quantityPerPos;

	for (const ItemPos pos : sentItems) {
		const auto it = quantityPerPos.find(pos);
		if (it == quantityPerPos.end()) {
			CItemElem * item = CItemUpgrade::GetModifiableItem(pUser, pos); // $
			if (!item) return;

			quantityPerPos[pos] = Detail{ item, 1 };
		} else {
			++quantityPerPos[pos].quantity;
		}
	}

	unsigned int nOrichalcum = 0;
	unsigned int nMoonstone = 0;
	for (const auto & [_, details] : quantityPerPos) {
		if (ItemProps::IsOrichalcum(*details.item)) {
			nOrichalcum += details.quantity;
		} else if (ItemProps::IsMoonstone(*details.item)) {
			nMoonstone += details.quantity;
		} else {
			return; // $
		}

		if (details.item->m_nItemNum < details.quantity) { // $
			return;
		}
	}

	if (nOrichalcum + nMoonstone < 3) return;
	if (nOrichalcum > 10) return;
	if (nMoonstone > 10) return;


	// ~~ Will be able to add the angel?

	if (pUser.m_Inventory.GetEmptyCount() < 1) {
		pUser.AddDiagText(prj.GetText(TID_GAME_LACKSPACE));
		return;
	}

	// Determine kind of angels
	static constexpr std::array</* ItemId */ DWORD, 4> angelKinds = {
		II_SYS_SYS_QUE_ANGEL_WHITE,
		II_SYS_SYS_QUE_ANGEL_GREEN,
		II_SYS_SYS_QUE_ANGEL_BLUE,
		II_SYS_SYS_QUE_ANGEL_RED
	};

	std::array<DWORD, 3> rates = { // Over 1000
		nOrichalcum + nMoonstone,
		(nOrichalcum + nMoonstone) * 10,
		(nOrichalcum + nMoonstone) * 20,
		// Red angel = the rest
	};

	static_assert(rates.size() + 1 == angelKinds.size());

	DWORD roll = xRandom(1000);

	size_t i = 0;
	while (i < rates.size() && roll >= rates[i]) {
		roll -= rates[i];
	}

	const DWORD angelId = angelKinds[i];

	// ~~ Remove materials
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "&";
		aLogItem.SendName = pUser.GetName();
		aLogItem.RecvName = "ANGEL_MATERIAL";
		aLogItem.WorldId = pUser.GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser.GetGold();


		for (const auto & [position, details] : quantityPerPos) {
			details.item->SetExtra(0);
			OnLogItem(aLogItem, details.item, details.quantity);
			pUser.RemoveItem(position, details.quantity);
		}
	}

	// ~~ Add angel
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = angelId;
		itemElem.m_nItemNum = 1;
		if (pUser.CreateItem(&itemElem)) {
			LogItemInfo aLogItem;
			aLogItem.Action = "&";
			aLogItem.SendName = pUser.GetName();
			aLogItem.RecvName = "ANGEL_CREATE";
			aLogItem.WorldId = pUser.GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser.GetGold();
			OnLogItem(aLogItem, &itemElem, 1);
		}
	}
}

void CDPSrvr::OnAngleBuff(CAr & ar, CUser & pUser) {
	// 엘젤을 없애고 아이템화 인벤 자리가 없다면 메세지 처리
	IBuff * const pBuff = pUser.m_buffs.GetBuffByIk3(IK3_ANGEL_BUFF);
	if (!pBuff) return;
	if (pBuff->GetRemove()) return;
			
	const ItemProp * const pItemProp = pBuff->GetProp();
	if (!pItemProp) return;

	long nAngel = pItemProp->nAdjParamVal[0];
	if (nAngel <= 0 || nAngel > 100) nAngel = 100;


	const EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[pUser.m_nAngelLevel].nExp1 / 100 * nAngel;
	if (pUser.m_nAngelExp < nMaxAngelExp) return;

	constexpr auto Get100VersionOfAngel = [](const DWORD id) -> std::optional<DWORD> {
		switch (id) {
			case II_SYS_SYS_QUE_ANGEL_RED:   return II_SYS_SYS_QUE_ANGEL_RED100;
			case II_SYS_SYS_QUE_ANGEL_BLUE:  return II_SYS_SYS_QUE_ANGEL_BLUE100;
			case II_SYS_SYS_QUE_ANGEL_GREEN: return II_SYS_SYS_QUE_ANGEL_GREEN100;
			case II_SYS_SYS_QUE_ANGEL_WHITE: return II_SYS_SYS_QUE_ANGEL_WHITE100;
			default:                         return std::nullopt;
		}
	};
	const auto itemId = Get100VersionOfAngel(pItemProp->dwID);
	if (!itemId) return;

	CItemElem itemElem;
	itemElem.m_dwItemId = itemId.value();
	itemElem.m_nItemNum		= 1;

	if (!pUser.CreateItem(&itemElem)) {
		pUser.AddDefinedText(TID_GAME_NOT_INVEN_ANGEL, "");
		return;
	}

	pUser.RemoveIk3Buffs( IK3_ANGEL_BUFF );

	LogItemInfo aLogItem;
	aLogItem.Action = "&";
	aLogItem.SendName = pUser.GetName();
	aLogItem.RecvName = "ANGEL_CREATE_COMPLETED";
	aLogItem.WorldId = pUser.GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser.GetGold();
	OnLogItem( aLogItem, &itemElem, 1 );

	char szMessage[512] = {0,};
	sprintf(szMessage, prj.GetText(TID_EVE_REAPITEM), itemElem.GetProp()->szName);
	pUser.AddText(szMessage);
}

#ifdef __EVE_MINIGAME
void CDPSrvr::OnKawibawiboStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	pUser->m_nKawibawiboState = prj.m_MiniGame.Result_Kawibawibo( pUser );
	if( pUser->m_nKawibawiboState == CMiniGame::KAWIBAWIBO_WIN )
	{
		CMiniGame::__KAWIBAWIBO Kawibawibo = prj.m_MiniGame.FindKawibawibo( pUser->m_nKawibawiboWin );
		CMiniGame::__KAWIBAWIBO KawibawiboNext = prj.m_MiniGame.FindKawibawibo( pUser->m_nKawibawiboWin + 1 );
		pUser->AddKawibawiboResult( pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin, Kawibawibo.dwItemId, Kawibawibo.nItemCount, KawibawiboNext.dwItemId, KawibawiboNext.nItemCount );
	}
	else
		pUser->AddKawibawiboResult( pUser->m_nKawibawiboState, pUser->m_nKawibawiboWin );
}
void CDPSrvr::OnKawibawiboGetItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	prj.m_MiniGame.ResultItem_Kawibawibo( pUser );
}

void CDPSrvr::OnReassembleStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	OBJID objItemId[9];
	
	for( int i=0; i<9; ++i )
		ar >> objItemId[i];
		
	BOOL nResult = prj.m_MiniGame.Result_Reassemble( pUser, objItemId, 9 );
	if( nResult == TRUE )
		prj.m_MiniGame.ResultItem_Reassemble( pUser );
		
}

void CDPSrvr::OnReassembleOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	prj.m_MiniGame.OpenWnd_Reassemble( pUser );
}

void CDPSrvr::OnAlphabetOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	prj.m_MiniGame.OpenWnd_Alphabet( pUser );
}

void CDPSrvr::OnAlphabetStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	int nQuestionID = 0;
	OBJID objItemId[5];

	ar >> nQuestionID;
	for( int i=0; i<5; ++i )
		ar >> objItemId[i];

	int nResult = prj.m_MiniGame.Result_Alphabet( pUser, objItemId, 5, nQuestionID );
	if( nResult != CMiniGame::ALPHABET_FAILED && nResult != CMiniGame::ALPHABET_NOTENOUGH_MONEY )
		prj.m_MiniGame.ResultItem_Alphabet( pUser, nResult );
	else
		pUser->AddAlphabetResult( nResult );
//	else
}

void CDPSrvr::OnFiveSystemOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	prj.m_MiniGame.OpenWnd_FiveSystem( pUser );
}

void CDPSrvr::OnFiveSystemBet( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	int nBetNum;
	int nBetPenya;
	
	ar >> nBetNum;
	ar >> nBetPenya;

	int nResult = prj.m_MiniGame.Bet_FiveSystem( pUser, nBetNum, nBetPenya );
	if( nResult == CMiniGame::FIVESYSTEM_NOTENOUGH )
	{
		pUser->AddFiveSystemResult( nResult );
	}
	else if( nResult == CMiniGame::FIVESYSTEM_OVERMAX )
	{
		pUser->AddFiveSystemResult( nResult );
	}
	else if( nResult == CMiniGame::FIVESYSTEM_FAILED )
	{
		pUser->AddFiveSystemResult( nResult );
	}
}

void CDPSrvr::OnFiveSystemStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	int nResult = prj.m_MiniGame.Result_FiveSystem( pUser );

	if( !( prj.m_MiniGame.ResultPenya_FiveSystem( pUser, nResult ) ) )
		pUser->AddFiveSystemResult( CMiniGame::FIVESYSTEM_FAILED );
}	

void CDPSrvr::OnFiveSystemDestroyWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	prj.m_MiniGame.DestroyWnd_FiveSystem( pUser );
}
#endif // __EVE_MINIGAME

void CDPSrvr::OnUltimateMakeItem(CAr & ar, CUser & pUser) {
	std::array<OBJID, MAX_JEWEL> objItemId; ar >> objItemId;

	const CUltimateWeapon::Result nResult = prj.m_UltimateWeapon.MakeOrichalcum2(pUser, objItemId);
	pUser.SendSnapshotThisId<SNAPSHOTTYPE_ULTIMATE, BYTE, CUltimateWeapon::Result>(
		ULTIMATE_MAKEITEM, nResult
		);
}

void CDPSrvr::OnUltimateMakeGem(CAr & ar, CUser & pUser) {
	OBJID objItemId; ar >> objItemId;
	
	const CUltimateWeapon::MakeGemAnswer nResult = prj.m_UltimateWeapon.MakeGem(pUser, objItemId);

	pUser.SendSnapshotThisId<
		SNAPSHOTTYPE_ULTIMATE, BYTE, CUltimateWeapon::MakeGemAnswer
	>(ULTIMATE_MAKEGEM, nResult);
}

void CDPSrvr::OnUltimateTransWeapon(CAr & ar, CUser & pUser) {
	const auto [weapon, jewel, ori] = ar.Extract<OBJID, OBJID, OBJID>();
	const int nResult = prj.m_UltimateWeapon.TransWeapon(&pUser, weapon, jewel, ori);
	pUser.AddUltimateWeapon(ULTIMATE_TRANSWEAPON, nResult);
}

void CDPSrvr::OnUltimateSetGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	OBJID objItemWeapon;
	OBJID objItemGem;

	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.SetGem( pUser, objItemWeapon, objItemGem );
	pUser->AddUltimateWeapon( ULTIMATE_SETGEM, nResult );
}

void CDPSrvr::OnUltimateRemoveGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	OBJID objItemWeapon;
	OBJID objItemGem;
	
	ar >> objItemWeapon;
	ar >> objItemGem;
	
	int nResult = prj.m_UltimateWeapon.RemoveGem( pUser, objItemWeapon, objItemGem );

	if(nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{
		//성공 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_SUCCESS, "" );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);		
	}
	else if(nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_GAME_REMOVEGEM_FAILED, "" );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);				
	}
	pUser->AddUltimateWeapon( ULTIMATE_REMOVEGEM, nResult );
}

void CDPSrvr::OnUltimateEnchantWeapon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	OBJID objItemWeapon;
	OBJID objItemGem;
	
	ar >> objItemWeapon;
	ar >> objItemGem;

	int nResult = prj.m_UltimateWeapon.EnchantWeapon( pUser, objItemWeapon, objItemGem );

	if(nResult == CUltimateWeapon::ULTIMATE_SUCCESS)
	{
		//성공 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL, "" );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);		
	}
	else if(nResult == CUltimateWeapon::ULTIMATE_FAILED)
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_FAIL, "" );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);				
	}
	pUser->AddUltimateWeapon( ULTIMATE_ENCHANTWEAPON, nResult );
}

#ifdef __TRADESYS
void CDPSrvr::OnExchange( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
		return;

	int nMMIid;
	int nListNum;

	ar >> nMMIid;
	ar >> nListNum;

	int	nResult = prj.m_Exchange.ResultExchange( pUser, nMMIid, nListNum );

	pUser->AddExchangeResult( EXCHANGE_RESULT, nResult );
}
#endif // __TRADESYS

void CDPSrvr::InviteParty( u_long uLeaderid, u_long uMemberid ) {
	CUser * pUser = g_UserMng.GetUserByPlayerID(uMemberid);
	CUser * pLeaderUser = g_UserMng.GetUserByPlayerID(uLeaderid);
	
	if (!IsValidObj(pLeaderUser)) return;

	if (!IsValidObj(pUser)) {
		pLeaderUser->AddPartyRequestCancel(uMemberid, 4);
		return;
	}

		// 대전장에서는 파티를 할수 없습니다.
		CWorld* pWorld = pUser->GetWorld();
		if( ( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR ) || pLeaderUser->GetWorld() && pLeaderUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
		{			
			pLeaderUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY) );// "수정해야함 : 길드대전장에는 파티를 할수 없습니다" );
			return;
		}
		if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) )
		{
			pLeaderUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_PARTY) );// "수정해야함 : 길드대전장에는 파티를 할수 없습니다" );
			return;
		}

		if( 0 < pUser->m_nDuel ||  0 < pLeaderUser->m_nDuel )
		{
			return;
		}
		
		if( pLeaderUser->m_nDuel == 2 )
		{			
			pLeaderUser->AddDefinedText( TID_GAME_PPVP_ADDPARTY, "" );		// 극단 듀얼중엔 초청 못함다.
		} 
		else
		{
			if( 0 < (CMover*)pUser->GetPartyId() )	// 이미 파티가 있을때
			{
				pLeaderUser->AddPartyRequestCancel(uMemberid, 1 );
			}
			else {
				if (pUser->IsAttackMode()) {
					pLeaderUser->AddDefinedText(TID_GAME_BATTLE_NOTPARTY, "");
				} else {
					pUser->AddPartyRequest(pLeaderUser);
				}
			}
		}


}

void CDPSrvr::InviteCompany( CUser* pUser, OBJID objid )
{
	if( !IsInviteAbleGuild( pUser ) )
	{
		return;
	}
	
	if( IsValidObj( pUser ) )
	{
		CUser* pUsertmp	= prj.GetUser( objid );
		if( IsValidObj( pUsertmp ) )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( pGuild )
			{
				CGuildMember* pMember	= pGuild->GetMember( pUser->m_idPlayer );
				if( !pMember )
				{
					// is not member
					return;
				}
				if( !pGuild->IsCmdCap( pMember->m_nMemberLv, GuildPower::Invitation ) )
				{
					// have no power
					pUser->AddDefinedText( TID_GAME_GUILDINVAITNOTWARR );
					return;
				}
				CGuild* pGuildtmp	= g_GuildMng.GetGuild( pUsertmp->m_idGuild );
				if( pGuildtmp && pGuildtmp->IsMember( pUsertmp->m_idPlayer ) )
				{
					// is already guild member
					pUser->AddDefinedText( TID_GAME_COMACCEPTHAVECOM, "%s", pUsertmp->GetName( TRUE ) );
				}
				else
				{
					if( 0 < pUsertmp->m_nDuel )
					{
						return;
					}

					if( !pGuild->GetWar() )
					{
						if( pUsertmp->IsAttackMode() )
						{
							pUser->AddDefinedText( TID_GAME_BATTLE_NOTGUILD, "" );
						} 
						else
						{
							
							pUsertmp->m_idGuild	= 0;
							pUsertmp->AddGuildInvite( pGuild->m_idGuild, pUser->m_idPlayer );
							pUser->AddDefinedText( TID_GAME_COMACCEPTKINGPIN, "%s", pUsertmp->GetName( TRUE ) );
						}
					}
					else
					{
						pUser->AddDefinedText( TID_GAME_GUILDWARNOMEMBER );
					}
				}
			}
		}
	}	
}

BOOL CDPSrvr::IsInviteAbleGuild( CUser* pUser )
{
	CGuild* pGuild = pUser->GetGuild();
	if( !pGuild )
		return FALSE;

	// 길드대전
	if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
	{
		if( g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild ) &&
			g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild )->bRequest )
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_INVITATION_GUILD );
			return FALSE;
		}
	}

	// 1:1 길드대전
	if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_CLOSE )
	{
		if( g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser ) != NULL_ID )
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOT_INVITATION_GUILD );
			return FALSE;
		}
	}

	// 비밀의 방
	if( CSecretRoomMng::GetInstance()->m_nState != SRMNG_CLOSE )
	{
		auto it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for( ; it!=CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); it++ )
		{
			CSecretRoomContinent* pSRCont = it->second;
			if( pSRCont && ( pSRCont->GetTenderGuild( pGuild->GetGuildId() ) != NULL_ID ) )
			{
				pUser->AddDefinedText( TID_GAME_SECRETROOM_NOT_INVITATION_GUILD );
				return FALSE;
			}
		}
	}

	return TRUE;
}


CCommonCtrl* CreateExpBox( CUser* pUser )
{
//#if __VER >= 9 // __S_9_ADD
//	return NULL;
//#endif // __S_9_ADD
	CWorld* pWorld = pUser->GetWorld();

	if( pWorld == NULL  )
		return NULL;

	///////////////////////////////////////////////////////////////////////
	// 경험치가 깎이지 않으면 드랍을 안함!!!
	CMover* pMover = (CMover*)pUser;
	
	float fRate = 0.1f, fDecExp = 0.0f;
	BOOL  bPxpClear = FALSE, bLvDown = FALSE;
	int   nLevel	= pMover->GetLevel();	
	pMover->GetDieDecExp( nLevel, fRate, fDecExp, bPxpClear, bLvDown );
	if( fDecExp )
	{
		pMover->GetDieDecExpRate( fDecExp, 0, FALSE );
	}

	// 축복의 두루마리 사용시에는 경험치 상자 만들지 않는다...

	if( pMover->IsSMMode( SM_REVIVAL ) )
		fDecExp = 0.0f;

	if( pMover->GetExp1() == 0 )
		fDecExp = 0.0f;	

	if( pMover->m_bLastPK || pMover->m_bGuildCombat || pMover->m_bLastDuelParty )		// 무조건 경험치 안깍는다...
		fDecExp = 0.0f;			
		
	if( fDecExp == 0.0f )
		return NULL;
	
	CCommonCtrl* pCtrl	= (CCommonCtrl*)CreateObj( D3DDEVICE, OT_CTRL, 46 );
	
	if( !pCtrl )
		return NULL;
	///////////////////////////////////////////////////////////////////////

	pCtrl->m_CtrlElem.m_dwSet    = UA_PLAYER_ID;
	pCtrl->m_idExpPlayer = pUser->m_idPlayer;
	
	EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[pUser->m_nLevel+1].nExp1 * fDecExp );	// 현재레벨의 최대경험치 * 퍼센트

	if( nDecExp > pMover->GetExp1() )
		nDecExp = pMover->GetExp1();
	
	pCtrl->m_nExpBox     = (EXPINTEGER)(nDecExp * 0.3f);
	pCtrl->m_dwDelete    = timeGetTime() + MIN( 30 );
	
	pCtrl->SetPos( pUser->GetPos() );
	
	pUser->AddDefinedText(TID_GAME_EXPBOX_INFO_MSG, "" );
	pUser->AddChatText(   TID_GAME_EXPBOX_INFO_MSG, "");
	return pCtrl;
}

void CDPSrvr::OnPetRelease( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( !pUser->HasActivatedSystemPet() )
		{
			pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
		}
		else
		{
			CItemElem* pItemElem	= pUser->GetPetItem();
			if( pItemElem && pItemElem->m_pPet )
				pUser->PetRelease();
		}
	}
}

void CDPSrvr::OnUsePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CPet* pPet	= pUser->GetPet();
		if( pPet == NULL )
		{
			pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			return;
		}
		DWORD dwFeedId;		// 먹이 식별자
		short nNum;
		ar >> dwFeedId;
		CItemElem * pFeed = pUser->GetItemId(dwFeedId);
		if( !IsUsableItem( pFeed ) )
			return;
		if( !pFeed->IsFeed() )
			return;

		nNum	= pFeed->m_nItemNum;

		int nMaxNum	= 0;
		if( pPet->GetLevel() == PL_EGG )
		{
			nMaxNum	= MAX_PET_EGG_EXP - pPet->GetExp();
		}
		else
		{
			nMaxNum	= pPet->GetMaxEnergy() - pPet->GetEnergy();
			nMaxNum	/= 2;	// 먹이 1당 기력 2회복	// 0723
		}

		if( nNum > nMaxNum )
			nNum	= nMaxNum;
		if( nNum == 0 )
			return;

		if( pPet->GetLevel() == PL_EGG )
		{
			pPet->SetExp( pPet->GetExp() + nNum );
			pUser->AddPetSetExp( pPet->GetExp() );
		}
		else
		{
			pPet->SetEnergy( pPet->GetEnergy() + nNum * 2 );	// 먹이 1당 기력 2회복	// 0723
			g_UserMng.AddPetFeed( pUser, pPet->GetEnergy() );
		}
		pUser->UpdateItem(*pFeed, UI::Num::Consume(nNum));
		pUser->AddDefinedText( TID_GAME_PETFEED_S01, "%d", nNum );

		// log
		CItemElem* pPetItem	= pUser->GetPetItem();
		g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), nNum, PETLOGTYPE_FEED, pPet );
	}
}

void CDPSrvr::OnMakePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		DWORD dwMaterialId, dwToolId;
		short nNum;

		ar >> dwMaterialId >> dwToolId >> nNum;

		BOOL bTool	= FALSE;
		CItemElem* pTool	= nullptr;
		if( dwToolId != NULL_ID )
		{
			pTool	= pUser->GetItemId( dwToolId );
			if( IsUsableItem( pTool ) )
			{
				if( pTool->m_dwItemId == II_SYS_SYS_FEED_MAKER )
					bTool	= TRUE;
			}
			if( !bTool )	// error
				return;
		}

		// 펫 테이머에 의한 먹이 제조 시, 펫 테이머와 인접해 있지 않으면 무시
		if( bTool == FALSE && CNpcChecker::GetInstance()->IsCloseNpc<MMI_PET_FOODMILL>(pUser) == FALSE )
			return;

		CItemElem* pMaterial	= pUser->GetItemId( dwMaterialId );
		if( !IsUsableItem( pMaterial ) )
		{
			// error
			return;
		}
		ItemProp* pProp	= pMaterial->GetProp();
		if( pProp == NULL )
		{
			// error
			return;
		}

		if( pProp->dwItemKind3 != IK3_GEM )
		{
			pUser->AddDefinedText( TID_GAME_PET_IS_NOT_FEED );
			return;
		}

		if( nNum <= 0 )
			nNum	= 1;
		if( nNum > pMaterial->m_nItemNum )
			nNum	= pMaterial->m_nItemNum;

		if( pUser->m_Inventory.IsEquip( dwMaterialId ) )
		{
			pUser->AddDefinedText( TID_GAME_PET_FEED_EQUIPED );
			return;
		}

		CItemElem itemElem;
		itemElem.m_dwItemId		= II_SYS_SYS_FEED_01;
		int nTotalFeed	= 0;
		int nPackMax	= itemElem.GetProp()->dwPackMax;
		for( int i = 0; i < nNum; i++ )
		{
			int nFeed	= CPetProperty::GetInstance()->GetFeedEnergy( pProp->dwCost, (int)bTool );
			if( nTotalFeed + nFeed > nPackMax )
			{
				nNum	= i;
				break;
			}
			nTotalFeed	+= nFeed;
		}
		itemElem.m_nItemNum		= nTotalFeed;
		itemElem.m_nHitPoint	= -1;

		int nResult = pUser->CreateItem( &itemElem );
		if( nResult )
		{
			//먹이 만들기 로그 : pMaterial이 없어질 먹이 재료, itemElem.m_nItemNum이 새로 생성된 먹이 량
			LogItemInfo aLogItem;
			aLogItem.Action = "~";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "PET_FOOD_MATERIAL_REMOVE";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			OnLogItem( aLogItem, pMaterial, nNum );

			aLogItem.RecvName = "PET_FOOD_CREATE";
			OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );

			pUser->UpdateItem(*pMaterial, UI::Num::Consume(nNum));
			if (bTool) pUser->UpdateItem(*pTool, UI::Num::ConsumeOne);
		}
		pUser->AddPetFoodMill(nResult, itemElem.m_nItemNum);
	}
}

void CDPSrvr::OnPetTamerMistake( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwId;
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> dwId;
		CItemElem* pItemElem	= pUser->GetItemId( dwId );
		if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE ) // m_dwItemId
		{
			CPet* pPet	= pUser->GetPet();
			if( pPet )
			{
				if( pPet->GetLevel() >= PL_C && pPet->GetLevel() <= PL_S )
				{
					// 현재 레벨에서 얻은 능력치가 취소되며,
					// 전 단계 레벨, 경험치 100%로 돌아간다.
					pPet->SetAvailLevel( pPet->GetLevel(), 0 );		// 능력치 취소
					pPet->SetLevel( pPet->GetLevel() - 1 );
					pPet->SetExp( MAX_PET_EXP );
					// 기존 버프 제거
					if( pUser->HasPet() )
						pUser->RemovePet();
					pUser->AddPet( pPet, PF_PET_LEVEL_DOWN );	// 自
					g_UserMng.AddPetLevelup( pUser, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
					pUser->UpdateItem(*pItemElem, UI::Num::ConsumeOne);

					// log
					CItemElem* pPetItem		= pUser->GetPetItem();
					g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), 0, PETLOGTYPE_MISTAKE, pPet );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_PET_BETWEEN_C_TO_S );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			}
		}
		else
		{
			// error
		}
	}
}

void CDPSrvr::OnPetTamerMiracle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwId;
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> dwId;
		CItemElem* pItemElem	= pUser->GetItemId( dwId );
		if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MIRACLE )
		{
			CPet* pPet	= pUser->GetPet();
			if( pPet )
			{
				if( pPet->GetLevel() >= PL_B && pPet->GetLevel() <= PL_S )
				{
					// 현재 레벨과 그 전 레벨에서 얻은 능력치가 취소되며,
					// 다시 랜덤하게 얻게 된다.
					pPet->SetAvailLevel( pPet->GetLevel() - 1, 0 );		// 능력치 취소
					pPet->SetAvailLevel( pPet->GetLevel(), 0 );		// 능력치 취소
					// 임의 능력치 상승
					BYTE nAvailLevel	= CPetProperty::GetInstance()->GetLevelupAvailLevel( pPet->GetLevel() - 1 );
					pPet->SetAvailLevel( pPet->GetLevel() - 1, nAvailLevel );
					nAvailLevel	= CPetProperty::GetInstance()->GetLevelupAvailLevel( pPet->GetLevel() );
					pPet->SetAvailLevel( pPet->GetLevel(), nAvailLevel );

					if( pUser->HasPet() )
						pUser->RemovePet();
					pUser->AddPet( pPet, PF_PET_GET_AVAIL );	// 自	// PF_PET_GET_AVAIL 
					g_UserMng.AddPetLevelup( pUser, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
					pUser->UpdateItem(*pItemElem, UI::Num::ConsumeOne);

					// log
					CItemElem* pPetItem		= pUser->GetPetItem();
					g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pPetItem->GetSerialNumber(), 0, PETLOGTYPE_MIRACLE, pPet );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_PET_BETWEEN_B_TO_S );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_PET_NOT_FOUND );
			}
		}
		else
		{
			// error
		}
	}
}

void CDPSrvr::OnFeedPocketInactive( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET ) )
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET );
	}
}

void CDPSrvr::OnModifyStatus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	int nStrCount, nStaCount, nDexCount, nIntCount;
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	ar >> nStrCount >> nStaCount >> nDexCount >> nIntCount;
	
	if((nStrCount < 0 || nStaCount < 0 || nDexCount < 0 || nIntCount < 0) ||
		(nStrCount + nStaCount + nDexCount + nIntCount <= 0)) //양수 검사 및 합이 0이하일 경우 중단.
		return;

	if( pUser->m_nRemainGP >= (nStrCount + nStaCount + nDexCount + nIntCount) )
	{
		pUser->m_nStr += nStrCount;
		pUser->m_nSta += nStaCount;
		pUser->m_nDex += nDexCount;
		pUser->m_nInt += nIntCount;
		pUser->m_nRemainGP = pUser->m_nRemainGP - (nStrCount + nStaCount + nDexCount + nIntCount);
		pUser->AddSetState( pUser->m_nStr, pUser->m_nSta, pUser->m_nDex, pUser->m_nInt, pUser->m_nRemainGP );
		pUser->CheckHonorStat();
		pUser->AddHonorListAck();//09.02.12
		g_UserMng.AddHonorTitleChange( pUser, pUser->m_nHonor);
	}
}	

void CDPSrvr::OnLegendSkillStart( CAr & ar, CUser & pUser )
{
	const auto [objItemId] = ar.Extract<std::array<OBJID, 5>>();

	if (!pUser.IsJobTypeOrBetter(JTYPE_HERO)) return;

	const auto lpSkill = std::ranges::find_if(pUser.m_jobSkills,
		[](const SKILL & skill) {
			const ItemProp * pSkillProp = skill.GetProp();
			if (!pSkillProp) return false;
			return pSkillProp->dwItemKind1 == JTYPE_HERO;
		}
	);

	if (lpSkill == pUser.m_jobSkills.end()) {
		pUser.AddLegendSkillResult(-1);
		return;
	}

	if (lpSkill->dwLevel > 4) {
		pUser.AddLegendSkillResult(-1);
		return;
	}

	CItemElem* pItemElem[5];
	// 일치하는지 검사 (인벤토리에서 검사)
	for( int i=0; i<5; i++ )
	{
		pItemElem[i]	= pUser.m_Inventory.GetAtId( objItemId[i] );
		if( !IsUsableItem( pItemElem[i] ) )
			return ;
	}

	if( pItemElem[0]->m_dwItemId != II_GEN_MAT_DIAMOND ||
		pItemElem[1]->m_dwItemId != II_GEN_MAT_EMERALD ||
		pItemElem[2]->m_dwItemId != II_GEN_MAT_SAPPHIRE ||
		pItemElem[3]->m_dwItemId != II_GEN_MAT_RUBY ||
		pItemElem[4]->m_dwItemId != II_GEN_MAT_TOPAZ )
		return;

	// 모두 일치하면 보석 아이템 삭제
	for( int i=0; i<5 ; i++ )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "+";
		aLogItem.SendName = pUser.GetName();
		aLogItem.RecvName = "LEGENDSKILL_USE";
		aLogItem.WorldId = pUser.GetWorld()->GetID();
		OnLogItem( aLogItem, pItemElem[i], 1 );
		pUser.RemoveItem( (BYTE)( objItemId[i] ), (short)1 );
	}

	if (xRandom(1000) <= 766) {
		pUser.AddLegendSkillResult(0);
		return;
	}

	lpSkill->dwLevel++;

	g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_USE, 1, &pUser, &*lpSkill);

	g_UserMng.AddCreateSfxObj(&pUser, XI_SYS_EXCHAN01, pUser.GetPos().x, pUser.GetPos().y, pUser.GetPos().z);
	pUser.AddDoUseSkillPoint(pUser.m_jobSkills, pUser.m_nSkillPoint);
#ifdef __S_NEW_SKILL_2
	g_dpDBClient.SaveSkill( pUser );
#endif // __S_NEW_SKILL_2
	pUser.AddLegendSkillResult(1);
}

void CDPSrvr::OnGC1to1TenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SendTenderGuildOpenWnd( pUser );
}

void CDPSrvr::OnGC1to1TenderView( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SendTenderGuildView( pUser );
}

void CDPSrvr::OnGC1to1Tender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;
	
	int nPenya;
	ar >> nPenya;

	g_GuildCombat1to1Mng.SetTenderGuild( pUser, nPenya );
}

void CDPSrvr::OnGC1to1CancelTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SetCancelTenderGuild( pUser );
}

void CDPSrvr::OnGC1to1FailedTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SetFailedTenderGuild( pUser );
}


void CDPSrvr::OnGC1to1MemberLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SendMemberLineUpOpenWnd( pUser );
}

void CDPSrvr::OnGC1to1MemberLineUp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	std::vector<u_long> vecMemberId;
	u_long nTemp;
	int nSize;

	ar >> nSize;

	if( nSize > g_GuildCombat1to1Mng.m_nMaxJoinPlayer )
		return;

	for( int i=0; i<nSize; i++ )
	{
		ar >> nTemp;
		vecMemberId.push_back( nTemp );
	}
	
	g_GuildCombat1to1Mng.SetMemberLineUp( pUser, vecMemberId );
}

void CDPSrvr::OnGC1to1TeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SetTeleportToNPC( pUser );
}

void CDPSrvr::OnGC1to1TeleportToStage( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	g_GuildCombat1to1Mng.SetTeleportToStage( pUser );
}

void CDPSrvr::OnQueryStartCollecting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( pUser->IsDisguise() )
			return;
		pUser->StartCollecting();
	}
}

void CDPSrvr::OnQueryStopCollecting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		pUser->StopCollecting();
}

	
void CDPSrvr::OnQueryGuildBankLogList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	bool bMK = FALSE;
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		BYTE byListType;
		ar >> byListType;
		CGuild *pGuild = pUser->GetGuild();

		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
			if( pGuildMember )
			{
				if( pGuildMember->m_nMemberLv == GUD_KINGPIN || pGuildMember->m_nMemberLv == GUD_MASTER )
					g_dpDBClient.SendQueryGetGuildBankLogList( pUser->m_idPlayer, pGuild->m_idGuild,byListType );
			}
		}
		// 길드가 없거나 길드장이 아니면 신청 불가
	}
}
void CDPSrvr::OnHonorListReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		pUser->AddHonorListAck();
	}
}
void CDPSrvr::OnHonorChangeReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		int nChange;
		ar >> nChange;
		if( nChange > -1 && nChange < MAX_HONOR_TITLE )
		{
			int nNeed =  CTitleManager::Instance()->GetNeedCount( nChange,-1);
			if( nNeed < 0)
				return;
			if( pUser->GetHonorTitle(nChange) >= nNeed )
			{
				pUser->m_nHonor = nChange;
				g_UserMng.AddHonorTitleChange( pUser, nChange );
			}
		}
		else if(nChange == -1)
		{
			pUser->m_nHonor = nChange;
			g_UserMng.AddHonorTitleChange( pUser, nChange );
		}
	}
}

void CDPSrvr::OnSealCharReq( CAr & ar, CUser & pUser ) {
	if (pUser.GetGuild()) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANGUILD);
	}

	if (pUser.IsChaotic()) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANEQUIP);
	}

	int nSize	= 0;
	for (int i = 0; i < pUser.m_Inventory.GetMax(); ++i) {
		const CItemElem * const pItemElem = pUser.m_Inventory.GetAtId(i);
		if (IsUsableItem(pItemElem) && pItemElem->m_nItemNum > 0) {
			nSize++;
		}
	}

	if (nSize > 1) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANINVEN);
	}

	const CItemElem * const pItemElem = pUser.m_Inventory.GetAtItemId(II_SYS_SYS_SCR_SEAL);
	if (!IsUsableItem(pItemElem)) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANINVEN);
	} else if (pItemElem->m_nItemNum > 1) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANINVEN);
	}

	if (pUser.GetGold() > 0) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANEQUIP);
	}

	for (int i = 0; i < pUser.m_Bank[pUser.m_nSlot].GetMax(); ++i) {
		const CItemElem * const pItemElem = pUser.m_Bank[pUser.m_nSlot].GetAtId(i);
		if (IsUsableItem(pItemElem) && pItemElem->m_nItemNum > 0) {
			return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANBANK);
		}
	}

	if (pUser.m_dwGoldBank[pUser.m_nSlot] > 0) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANBANK);
	}

	if (!pUser.m_Pocket.IsAllClean()) {
		return pUser.AddDefinedText(TID_GAME_SEALCHAR_NO_CLEANBANK);
	}

	g_dpDBClient.SendQueryGetSealChar(pUser.m_idPlayer, pUser.m_playAccount.lpszAccount);
}

void CDPSrvr::OnSealCharConmReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OBJID objidSend;
	ar >> objidSend;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CGuild *pGuild = pUser->GetGuild();
		if( pGuild != NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANGUILD );
			return;
		}
		int nSize	= 0;
		for( int i=0; i< pUser->m_Inventory.GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( !IsUsableItem( pItemElem ) )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nSize++;
		}

		CItemElem* pItemElemTrue = NULL;
		CItemElem* pItemElemtmp = NULL;
		
		if( nSize > 1 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
			return;
		}
		else
		{
			pItemElemTrue = pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_SEAL );
			if( !IsUsableItem( pItemElemTrue ) )
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			else if(pItemElemTrue->m_nItemNum > 1)
			{
				pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANINVEN );
				return;
			}
			pItemElemtmp = pItemElemTrue;
		}

		if( pUser->GetGold() > 0 )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANEQUIP );
			return;
		}
		int nBankSize	= 0;
		for( int i=0; i< pUser->m_Bank[pUser->m_nSlot].GetMax(); ++i )
		{
			CItemElem* pItemElem = pUser->m_Bank[pUser->m_nSlot].GetAtId( i );
			if( !IsUsableItem( pItemElem ) )
				continue;
			if( pItemElem->m_nItemNum > 0 )
				nBankSize++;
		}
		if( !pUser->m_Pocket.IsAllClean() )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
		if( nBankSize > 0 || pUser->m_dwGoldBank[pUser->m_nSlot] > 0)
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CLEANBANK );
			return;
		}
	
		const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( objidSend );

		if( lpszPlayer == NULL )
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
			return;
		}


		CItemElem itemElemSend;
		itemElemSend.m_dwItemId = II_SYS_SYS_SCR_SEALCHARACTER;
		itemElemSend.m_nItemNum	= 1;
		itemElemSend.SetSerialNumber();

//		lstrcpy( itemElemSend.m_szItemText, pUser->GetName() );
		memcpy( itemElemSend.m_szItemText, pUser->GetName(),sizeof(itemElemSend.m_szItemText) );

		itemElemSend.m_nRepairNumber	= pUser->GetLevel();//nlevel
		itemElemSend.m_nHitPoint	= pUser->m_idPlayer;//m_idPlayer
		itemElemSend.m_nRepair	= pUser->GetJob();//njob

		itemElemSend.m_nResistAbilityOption	= pUser->GetRemainGP();//nPOINT

		itemElemSend.SetPiercingSize( 4 );
		itemElemSend.SetPiercingItem( 0, pUser->GetSta() );		//nSTA
		itemElemSend.SetPiercingItem( 1, pUser->GetStr() );		//nSTR
		itemElemSend.SetPiercingItem( 2, pUser->GetDex() );		//nDEX
		itemElemSend.SetPiercingItem( 3, pUser->GetInt() );		//nINT
		
		LogItemInfo aLogItem;
		aLogItem.RecvName = "SEALCHAR";
		g_dpDBClient.SendQueryPostMail( objidSend, 0, itemElemSend, 0, itemElemSend.GetProp()->szName, 	(char*)GETTEXT( TID_MMI_SEALCHARITEM ) );

		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		OnLogItem( aLogItem, &itemElemSend, 1 );


		ItemProp* pItemProp  = pItemElemtmp->GetProp();
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemtmp, pItemProp );		
//		pItemElemtmp->UseItem();
		OBJID       dwTmpObjId = pItemElemtmp->m_dwObjId;
		pUser->RemoveItem( (BYTE)( dwTmpObjId ), 1 );

		pUser->ClearAllSMMode();
		pUser->RemoveAllBuff();

		g_dpDBClient.SendQueryGetSealCharConm( pUser->m_idPlayer);

		QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
	}
}
void CDPSrvr::OnSealCharGetReq( CAr & ar, CUser & pUser )
{
	DWORD dwData; ar >> dwData ;

	const DWORD dwId = HIWORD( dwData );
	if (!pUser.IsUsableState(dwId)) return;

	const CItemElem * const pItemElem = pUser.m_Inventory.GetAtId(dwId);
	if (!IsUsableItem(pItemElem)) return;
	if (pItemElem->m_dwItemId != II_SYS_SYS_SCR_SEALCHARACTER) return;

	g_dpDBClient.SendQueryGetSealCharGet(pUser.m_idPlayer, pUser.m_playAccount.lpszAccount, dwId);
}


void	CDPSrvr::OnMoveItemOnPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nPocket1, nItem, nPocket2;
		short nNum;
		ar >> nPocket1 >> nItem >> nNum >> nPocket2;

		if( nPocket1 == nPocket2 )
			return;

		// mirchang 091214 - 착용중인 아이템인지 체크
		if( nPocket1 == -1 )	// 원본이 인벤토리인지 검사!
		{
			if( pUser->m_Inventory.IsEquip( nItem ) )
				return;
		}

		CItemElem* pItem	= pUser->GetItemId2( nPocket1, nItem );	// 여기서 휴대가방 만료검사 같이함.
		if( IsUsableItem( pItem ) )
		{
			if( nPocket1 < 0 && pUser->IsUsing( pItem ) )
				return;
			if( nNum > pItem->m_nItemNum )
				return;

			CItemElem item;
			item	= *pItem;
			item.m_nItemNum		= nNum;
			if( pUser->CreateItem2( &item, nPocket2 ) )
			{
				pUser->RemoveItem2( nItem, nNum, nPocket1 );
				// log
				LogItemInfo	log;
				log.Action	= "m";
				log.SendName	= pUser->GetName();
				log.RecvName	= "OnMoveItemOnPocket";
				log.WorldId		= pUser->GetWorld()->GetID();
				log.Gold	= nPocket1;
				log.Gold2	= nPocket2;
				OnLogItem( log, &item, nNum );
			}
		}
	}
}

void	CDPSrvr::OnAvailPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		int nPocket, nItem;
		ar >> nPocket >> nItem;
		if( nPocket <= 0 && nPocket >= MAX_POCKET )
			return;
		CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nItem );
		if( IsUsableItem( pItemElem ) )
		{
			ItemProp* pItemProp		= pItemElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_POCKET )
			{
				if( !pUser->m_Pocket.IsAvailable( nPocket ) )
				{
					pUser->m_Pocket.SetAttribute( CPocketController::avail, nPocket, pItemProp->dwSkillTime );
					// log
					LogItemInfo	log;
					log.Action	= "u";
					log.SendName	= pUser->GetName();
					log.RecvName	= "OnAvailPocket";
					log.WorldId		= pUser->GetWorld()->GetID();
					log.Gold	= pUser->GetGold();
					log.Gold2	= pUser->GetGold();
					log.Gold_1	= nPocket;
					OnLogItem( log, pItemElem, 1 );
					pUser->UpdateItem(*pItemElem, UI::Num::ConsumeOne);
#ifdef __INTERNALSERVER
					pUser->AddPocketView();
#endif	// __INTERNALSERVER
				}
			}
		}
	}
}

void	CDPSrvr::OnBlessednessCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_BLESSING_CANCEL>(pUser) )
			return;

		int nItem;
		ar >> nItem;
		CItemElem* pItem	= pUser->GetItemId( nItem );
		if( IsUsableItem( pItem ) )
		{
			if( g_xRandomOptionProperty.GetRandomOptionKind( pItem ) == CRandomOptionProperty::eBlessing
				&& g_xRandomOptionProperty.GetRandomOptionSize( pItem->GetRandomOptItemId() ) > 0 )
			{
				g_xRandomOptionProperty.InitializeRandomOption( pItem->GetRandomOptItemIdPtr() );
				pUser->UpdateItem(*pItem, UI::RandomOptItem::Sync);
				pUser->AddDiagText( prj.GetText( TID_GAME_BLESSEDNESS_CANCEL_INFO ) );
				// log
				LogItemInfo	log;
				log.Action	= "r";
				log.SendName	= pUser->GetName();
				log.RecvName	= "OnBlessednessCancel";
				log.WorldId		= pUser->GetWorld()->GetID();
				log.Gold	= pUser->GetGold();
				log.Gold2	= pUser->GetGold();
				OnLogItem( log, pItem, 1 );
			}
			else
			{
				pUser->AddDefinedText( TID_GAME_BLESSEDNESS_CANCEL );	
			}
		}		
	}
}

void	CDPSrvr::OnAwakening( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_ITEM_AWAKENING>(pUser) )
			return;

		const int	nCost	= 100000;
		if( pUser->GetGold() < nCost )
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY, "" );
			return;
		}
		int	nItem;
		ar >> nItem;
		CItemElem* pItem	= pUser->m_Inventory.GetAtId( nItem );
		if( IsUsableItem( pItem ) )
		{
			int nRandomOptionKind	= g_xRandomOptionProperty.GetRandomOptionKind( pItem );

			if( nRandomOptionKind != CRandomOptionProperty::eAwakening )
			{
				pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
				return;
			}
			if( g_xRandomOptionProperty.GetRandomOptionSize( pItem->GetRandomOptItemId() ) > 0 )
			{
				pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
				return;
			}
			g_xRandomOptionProperty.InitializeRandomOption( pItem->GetRandomOptItemIdPtr() );
			g_xRandomOptionProperty.GenRandomOption( pItem->GetRandomOptItemIdPtr(), nRandomOptionKind, pItem->GetProp()->dwParts );
			pUser->UpdateItem(*pItem, UI::RandomOptItem::Sync);
			pUser->AddGold( -nCost );
			pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );
			// log
			LogItemInfo	log;
			log.Action	= "r";
			log.SendName	= pUser->GetName();
			log.RecvName	= "OnAwakening";
			log.WorldId		= pUser->GetWorld()->GetID();
			log.Gold	= pUser->GetGold() + nCost;
			log.Gold2	= pUser->GetGold();
			log.Gold_1	= -nCost;
			OnLogItem( log, pItem, 1 );
		}
	}
}

#ifdef __NPC_BUFF
void	CDPSrvr::OnNPCBuff( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CHAR	m_szKey[64];
		ar.ReadString(m_szKey, 64);
		LPCHARACTER lpChar = prj.GetCharacter( m_szKey );
		
		if( lpChar )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_NPC_BUFF>(pUser) )
				return;

			std::vector<NPC_BUFF_SKILL> vecNPCBuff = lpChar->m_vecNPCBuffSkill;
			for( int i=0; i<(int)( vecNPCBuff.size() ); i++ )
			{
				if( pUser->GetLevel() >= vecNPCBuff[i].nMinPlayerLV && pUser->GetLevel() <= vecNPCBuff[i].nMaxPlayerLV )
				{
					ItemProp* pSkillProp = prj.GetSkillProp( vecNPCBuff[i].dwSkillID );
					if( pSkillProp )
					{
						if( vecNPCBuff[i].dwSkillLV < pSkillProp->dwExpertLV || vecNPCBuff[i].dwSkillLV > pSkillProp->dwExpertMax )
							continue;
						
						if(	( pSkillProp->dwID == SI_GEN_EVE_QUICKSTEP && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_QUICKSTEP) )
							|| ( pSkillProp->dwID == SI_GEN_EVE_HASTE && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_HASTE) )
							|| ( pSkillProp->dwID == SI_GEN_EVE_HEAPUP && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_HEAPUP) )
	  						|| ( pSkillProp->dwID == SI_GEN_EVE_ACCURACY && pUser->HasBuff(BUFF_SKILL, SI_ASS_CHEER_ACCURACY) ) )
						{
							pUser->AddDefinedText( TID_GAME_NPCBUFF_FAILED, "\"%s\"", pSkillProp->szName );
							continue;
						}
						
						AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine + vecNPCBuff[i].dwSkillLV - 1 );
						if( pAddSkillProp )
						{
							// skill property를 수정하기 때문에 백업해두었다가
							// 스킬 시전후 restore시킨다.
							DWORD dwReferTarget1Backup = pSkillProp->dwReferTarget1;
							pSkillProp->dwReferTarget1 = NULL_ID;
							DWORD dwReferTarget2Backup = pSkillProp->dwReferTarget2;
							pSkillProp->dwReferTarget2 = NULL_ID;
							DWORD dwSkillTimeBackup = pAddSkillProp->dwSkillTime;
							pAddSkillProp->dwSkillTime = vecNPCBuff[i].dwSkillTime;

							pUser->DoApplySkill( pUser, pSkillProp, pAddSkillProp );
							g_UserMng.AddDoApplySkill( pUser, pUser->GetId(), vecNPCBuff[i].dwSkillID, vecNPCBuff[i].dwSkillLV );

							pSkillProp->dwReferTarget1 = dwReferTarget1Backup;
							pSkillProp->dwReferTarget2 = dwReferTarget2Backup;
							pAddSkillProp->dwSkillTime = dwSkillTimeBackup;
						}
					}
				}
				else
				{
					ItemProp* pSkillProp = prj.GetSkillProp( vecNPCBuff[i].dwSkillID );
					if( pSkillProp )
						pUser->AddDefinedText( TID_GAME_NPCBUFF_LEVELLIMIT, "%d %d \"%s\"", vecNPCBuff[i].nMinPlayerLV, vecNPCBuff[i].nMaxPlayerLV, pSkillProp->szName );
				}
			}
		}
	}
}
#endif // __NPC_BUFF

#ifdef __JEFF_11_4
void	CDPSrvr::OnArenaEnter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
// 아레나 지역 입장
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( pUser->IsBaseJob() )	// 1차 전직을 완료한 유저만 가능
			return;
		pUser->SetMarkingPos();
		pUser->Replace( WI_WORLD_ARENA, D3DXVECTOR3( 540.0F, 140.0F, 485.0F ), REPLACE_NORMAL, nDefaultLayer );
	}
}

void	CDPSrvr::OnArenaExit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
		pUser->Replace( pUser->m_idMarkingWorld, pUser->m_vMarkingPos, REPLACE_NORMAL, nTempLayer );
}
#endif	// __JEFF_11_4

#ifdef __JEFF_11
// 펫을 조각으로 교환
void	CDPSrvr::OnQuePetResurrection( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nItem;
		ar >> nItem;
		CItemElem*	pItem	= pUser->GetItemId( nItem );
		if( !IsUsableItem( pItem ) )
			return;

		CPet* pPet	= pItem->m_pPet;
		if( !pPet || pUser->GetPetId() == pItem->m_dwObjId || pItem->IsFlag( CItemElem::expired ) )
		{
			pUser->AddQuePetResurrectionResult( FALSE );
			return;
		}

		BYTE nLevel		= pPet->GetLevel();
		if( nLevel < PL_B || nLevel > PL_S )
		{
			pUser->AddQuePetResurrectionResult( FALSE );
			return;
		}

		CItemElem itemElem;
		itemElem.m_nItemNum	= 1;
		switch( nLevel )
		{
			case PL_B:	itemElem.m_dwItemId		= II_SYS_SYS_QUE_PETRESURRECTION01_B;	break;
			case PL_A:	itemElem.m_dwItemId		= II_SYS_SYS_QUE_PETRESURRECTION01_A;	break;
			case PL_S:	itemElem.m_dwItemId		= II_SYS_SYS_QUE_PETRESURRECTION01_S;	break;
		}
		// log
		LogItemInfo	log;
		log.Action	= "x";
		log.SendName	= pUser->GetName();
		log.RecvName	= "OnQuePetResurrection";
		OnLogItem( log, pItem, 1 );

		pUser->RemoveItem( nItem, 1 );
		pUser->CreateItem( &itemElem );
		pUser->AddQuePetResurrectionResult( TRUE );
	}
}
#endif	// __JEFF_11

void	CDPSrvr::OnSecretRoomTenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTenderOpenWnd( pUser );
}

void	CDPSrvr::OnSecretRoomTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	int nPenya;
	ar >> nPenya;

	if( nPenya <= 0 )
		return;

	CSecretRoomMng::GetInstance()->SetTender( pUser, nPenya );
}

void	CDPSrvr::OnSecretRoomTenderCancelReturn( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTenderCancelReturn( pUser );
}

void	CDPSrvr::OnSecretRoomLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetLineUpOpenWnd( pUser );
}

void	CDPSrvr::OnSecretRoomLineUpMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	
	if( IsValidObj( pUser ) == FALSE )
		return;

	std::set<DWORD> checker;
	std::vector<DWORD> vecLineUpMember;
	int nSize;
	ar >> nSize;

	if( nSize > CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum )
		return;

	for( int i=0; i<nSize; i++ )
	{
		DWORD dwIdPlayer;
		ar >> dwIdPlayer;

		// 유효한 Player ID 인가?
		PlayerData* pData	= CPlayerDataCenter::GetInstance()->GetPlayerData( dwIdPlayer );
		if( !pData )
			return;

		// ID 중복체크		
		if( !checker.insert( dwIdPlayer ).second )
			return;

		vecLineUpMember.push_back( dwIdPlayer );
	}

	CSecretRoomMng::GetInstance()->SetLineUp( pUser, vecLineUpMember );
}

void	CDPSrvr::OnSecretRoomEntrance( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->SetTeleportSecretRoom( pUser );
}

void	CDPSrvr::OnSecretRoomTeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;
	pUser->Replace( WI_WORLD_MADRIGAL, CSecretRoomMng::GetInstance()->GetRevivalPos( pUser ), REPLACE_NORMAL, nDefaultLayer );
}

void CDPSrvr::OnSecretRoomTenderView( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;

	CSecretRoomMng::GetInstance()->GetTenderView( pUser );
}

void CDPSrvr::OnTeleportSecretRoomDungeon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pUser ) == FALSE )
		return;
	if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_SECRET_ENTRANCE_1>(pUser) )
		return;

	__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( CTax::GetInstance()->GetContinent( pUser ) );
	if( pTaxInfo && pUser->GetGuild() && (pTaxInfo->dwId == pUser->GetGuild()->GetGuildId()) )
	{
		pUser->SetMarkingPos();
		pUser->SetAngle( 180.0f );
		int nRandx = xRandom(4) - 2;
		int nRandz = xRandom(4) - 2;
		pUser->Replace( WI_DUNGEON_SECRET_0, D3DXVECTOR3( (float)( 295 + nRandx ), 102.0f, (float)( 530 + nRandz ) ), REPLACE_NORMAL, nDefaultLayer );
	}
	else
		pUser->AddDefinedText( TID_GAME_SECRETROOM_NOENTRANCE_1 );
}

void CDPSrvr::OnElectionAddDeposit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		__int64 iTotal;
		ar >> iTotal;

		__int64 iDeposit;
		int nRet	= election::AddDepositRequirements( pUser, iTotal, iDeposit );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		pUser->AddGold( -static_cast<int>( iDeposit ) );
		g_dpDBClient.SendElectionAddDeposit( pUser->m_idPlayer, iDeposit );

		PutPenyaLog( pUser, "d", "DEPOSIT", static_cast<int>( iDeposit ) );
	}
}

void CDPSrvr::PutItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nNum )
{	// 아이템이 제거된 이후에 호출되지 않도록 주의해야 함
	LogItemInfo	log;
	log.Action	=  szAction;
	log.SendName	= pUser->GetName();
	log.RecvName	= szContext;
	log.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. 
	log.Gold	= pUser->GetGold();
	log.Gold2	= pUser->GetGold();
	if( nNum == 0 )
		nNum	= pItem->m_nItemNum;
	OnLogItem( log, pItem, nNum );
}

void CDPSrvr::PutPenyaLog( CUser* pUser, const char* szAction, const char* szContext,  int nCost )
{	// 모든 결과 처리 후 호출되어야 함
	LogItemInfo	log;
	log.Action	= szAction;
	log.SendName	= pUser->GetName();
	log.RecvName	= szContext;
	log.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. 
	//log.ItemName	= "SEED";
	_stprintf( log.szItemName, "%d", II_GOLD_SEED1 );
	log.Gold	= pUser->GetGold() + nCost;
	log.Gold2	= pUser->GetGold();
	log.Gold_1	= -nCost;
	OnLogItem( log );
}

void CDPSrvr::OnElectionSetPledge( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char szPledge[CCandidate::nMaxPledgeLen]	= { 0,};
	ar.ReadString( szPledge, CCandidate::nMaxPledgeLen );
	if( strlen( szPledge ) == 0 )
		return;

	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nCost;
		int nRet	= election::SetPledgeRequirements( pUser, nCost );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		pUser->AddGold( -nCost );
		g_dpDBClient.SendElectionSetPledge( pUser->m_idPlayer, szPledge );

		PutPenyaLog( pUser, "p", "PLEDGE", nCost );
	}
}

void CDPSrvr::OnElectionIncVote( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		u_long idPlayer;
		ar >> idPlayer;

		int nRet	= election::IncVoteRequirements( pUser, idPlayer );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		g_dpDBClient.SendElectionIncVote( idPlayer, pUser->m_idPlayer );
	}
}

void CDPSrvr::OnLEventCreate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int iEEvent, iIEvent;
		ar >> iEEvent >> iIEvent;
		int nRet	= lordevent::CreateRequirements( pUser, iEEvent, iIEvent );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();

		int nPerin	= pUser->RemoveTotalGold( pEvent->GetCost( iEEvent, iIEvent ) );
		char szContext[100]		= { 0,};
		sprintf( szContext, "OnLEventCreate: cost: %d(perin), %I64d(penya)", nPerin, pEvent->GetCost( iEEvent, iIEvent ) - ( nPerin * PERIN_VALUE ) );
		PutPenyaLog( pUser, "e", szContext, 0 );

		g_dpDBClient.SendLEventCreate( pUser->m_idPlayer, iEEvent, iIEvent );
	}
}

void CDPSrvr::OnLordSkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int nSkill;
		char szTarget[MAX_PLAYER]	= { 0,};
		ar >> nSkill;
		ar.ReadString( szTarget, MAX_PLAYER );
		u_long idTarget;
		int nRet	= lordskill::UseRequirements( pUser, szTarget, nSkill, idTarget );
		if( nRet )
		{
			pUser->AddDefinedText( nRet );
			return;
		}
		g_dpDBClient.SendLordSkillUse( pUser->m_idPlayer, idTarget, nSkill );
	}
}

void CDPSrvr::OnSetTaxRate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	BYTE nCont;
	int nSalesTaxRate, nPurchaseTaxRate;
	ar >> nCont;
	ar >> nSalesTaxRate >> nPurchaseTaxRate;

	__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( nCont );
	if( pTaxInfo && pUser->m_idGuild == pTaxInfo->dwNextId && pUser->IsGuildMaster() )
		CTax::GetInstance()->SetNextTaxRate( nCont, nSalesTaxRate, nPurchaseTaxRate );
	else
		Error( "\"%\"s User Is Not Next Win Guild Master!!!", pUser->GetName() );
}

void CDPSrvr::OnTeleportToHeavenTower( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_HEAVEN_TOWER>(pUser) )
		return;
    	
	int nFloor;
	ar >> nFloor;

	int nCost = 0;
	DWORD dwWorldId = NULL_ID;
	float fAngle = 0.0f;
	D3DXVECTOR3 vPos;

	switch( nFloor )
	{
		case 1 :	// 1층
			nCost = 10000;	dwWorldId = WI_WORLD_HEAVEN01; vPos = D3DXVECTOR3( 253, 102, 78 ); fAngle = 183.0f;
			break;
		case 2 :	// 2층
			nCost = 30000;	dwWorldId = WI_WORLD_HEAVEN02; vPos = D3DXVECTOR3( 251, 102, 95 ); fAngle = 183.0f;
			break;
		case 3 :	// 3층
			nCost = 50000;	dwWorldId = WI_WORLD_HEAVEN03; vPos = D3DXVECTOR3( 264, 102, 227 ); fAngle = 183.0f;
			break;
		case 4 :	// 4층
			nCost = 70000;	dwWorldId = WI_WORLD_HEAVEN04; vPos = D3DXVECTOR3( 253, 102, 86 ); fAngle = 174.0f;
			break;
		case 5 :	// 5층
			nCost = 100000;	dwWorldId = WI_WORLD_HEAVEN05; vPos = D3DXVECTOR3( 218, 102, 101); fAngle = 176.0f;
			break;

		default :
			Error( "CDPSrvr::OnTeleportToHeavenTower() - 잘못된 층 : %d, Name = %s", nFloor, pUser->GetName() );
			return;
	}

	if( pUser->GetGold() < nCost )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return;
	}

	BYTE nCont = CTax::GetInstance()->GetContinent( pUser );
	// 해당 층으로 텔레포트 -> 실패시 그냥 리턴...
	if( pUser->Replace( dwWorldId, vPos, REPLACE_NORMAL, nDefaultLayer ) )
	{
		pUser->AddGold( -nCost );
		pUser->SetAngle( fAngle );
		__TAXINFO* pTaxInfo = CTax::GetInstance()->GetTaxInfo( nCont );
		if( pTaxInfo && pTaxInfo->dwId != NULL_ID )
			CTax::GetInstance()->AddTax( nCont, nCost, TAX_ADMISSION );
		CString strFloor;
		strFloor.Format( "HEAVEN_TOWER_%2d", nFloor );
		PutPenyaLog( pUser, "h", strFloor, nCost );
	}
	else
		return;
}

BOOL CDPSrvr::DoUseItemTarget_GenRandomOption(
											  CUser* pUser, CItemElem* pTarget, int nKind,
											  int nOk, int nMismatchKind, int nHasOption,
											  const char* szOperation, const char* szReceive )
{
	int nRandomOptionKind	= g_xRandomOptionProperty.GetRandomOptionKind( pTarget );
	if( nRandomOptionKind != nKind || pTarget->IsFlag( CItemElem::expired ) )
	{
		pUser->AddDefinedText( nMismatchKind );
		return FALSE;
	}

	if( 
		// 여신의 축복과 먹펫 각성은 각성 취소 없이 덮어 쓸 수 있게 한다
		nKind != CRandomOptionProperty::eBlessing && nKind != CRandomOptionProperty::eEatPet &&
		g_xRandomOptionProperty.GetRandomOptionSize( pTarget->GetRandomOptItemId() ) > 0
	)
	{
		pUser->AddDefinedText( nHasOption );
		return FALSE;
	}

	if( nOk > 0 )
		pUser->AddDefinedText( nOk );

	// 활성화 된 픽업펫이면  기존 효과를 제거	// 康: 2008-09-29 추가
	if( pUser->IsUsing( pTarget ) && nKind == CRandomOptionProperty::eEatPet )
		pUser->ResetDestParamRandomOptExtension( pTarget );


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	//g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
	//g_xRandomOptionProperty->GenRandomOption( pTarget->GetRandomOptItemIdPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts );
	bool	bCheckedSafeFlag	= false;
	bCheckedSafeFlag	= g_xRandomOptionProperty.IsCheckedSafeFlag( pTarget->GetRandomOptItemId() );

	if( bCheckedSafeFlag == true )
	{
		g_xRandomOptionProperty.ResetSafeFlag( pTarget->GetRandomOptItemIdPtr() );

		g_xRandomOptionProperty.InitializeRandomOption( pTarget->GetNewRandomOptionPtr() );
		g_xRandomOptionProperty.GenRandomOption( pTarget->GetNewRandomOptionPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts, true );

		//	mulcom	BEGIN100426	각성 보호의 두루마리 로그.
// 		WriteLog( "UserID [%d] : ItemSerialNumber[%d] Protect GenNewRandomOption [%I64d]", (int)( pUser->GetId() ), (int)( pTarget->GetSerialNumber() ), pTarget->GetNewRandomOption() );
 
		TCHAR	szNewOption[128];
		::memset( szNewOption, 0, sizeof(szNewOption) );
		::_stprintf( szNewOption, "%I64d", pTarget->GetNewRandomOption() );
		g_DPSrvr.PutItemLog( pUser, "z", szNewOption, pTarget, 1 );
		//	mulcom	END100426	각성 보호의 두루마리 로그.

		pUser->SendNewRandomOption( (BYTE)( pTarget->m_dwObjId ), (DWORD)( pTarget->GetSerialNumber() ), pTarget->GetNewRandomOption() );

	}
	else
	{
		g_xRandomOptionProperty.InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
		g_xRandomOptionProperty.GenRandomOption( pTarget->GetRandomOptItemIdPtr(), nRandomOptionKind, pTarget->GetProp()->dwParts );
	}
	//	mulcom	END100405	각성 보호의 두루마리

	pUser->UpdateItem(*pTarget, UI::RandomOptItem::Sync);

	// 활성화 된 픽업펫 또는 리어펫 각성 직 후 효과 적용
	if( pUser->IsUsing( pTarget ) 
		&& ( nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet ) )
		pUser->SetDestParamRandomOptExtension( pTarget );

	// log
	LogItemInfo	log;
	log.Action	= szOperation;
	log.SendName	= pUser->GetName();
	log.RecvName	= szReceive;
	log.WorldId		= pUser->GetWorld()->GetID();
	log.Gold	= pUser->GetGold();
	log.Gold2	= pUser->GetGold();
	OnLogItem( log, pTarget, 1 );
	return TRUE;
}

BOOL CDPSrvr::DoUseItemTarget_InitializeRandomOption(
											  CUser* pUser, CItemElem* pTarget, int nKind,
											  int nOk, int nError,
											  const char* szOperation, const char* szRecv )
{
	int nRandomOptionKind	= g_xRandomOptionProperty.GetRandomOptionKind( pTarget );
	if( nRandomOptionKind == nKind && g_xRandomOptionProperty.GetRandomOptionSize( pTarget->GetRandomOptItemId() ) > 0 )
	{
		// 먹펫 또는 시스템 펫 각성 취소 후 효과 제거
		if( pUser->IsUsing( pTarget ) 
			&& ( nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet ) )
			pUser->ResetDestParamRandomOptExtension( pTarget );

		//	mulcom	BEGIN100405	각성 보호의 두루마리
		//g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
#ifdef __PROTECT_AWAKE
		if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AWAKESAFE ) == TRUE && nKind == CRandomOptionProperty::eAwakening )
		{
			nOk = TID_GAME_REGARDLESS_USE04;
			g_xRandomOptionProperty.SetSafeFlag( pTarget->GetRandomOptItemIdPtr() );

			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_AWAKESAFE );

			//	mulcom	BEGIN100426	각성 보호의 두루마리 로그.
			g_DPSrvr.PutItemLog( pUser, "z", "USE_AWAKESAVE_ITEM", pTarget, 1 );
			//	mulcom	END100426	각성 보호의 두루마리 로그.

		}
		else
		{
			g_xRandomOptionProperty.InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
		}
#else	//__PROTECT_AWAKE
		g_xRandomOptionProperty->InitializeRandomOption( pTarget->GetRandomOptItemIdPtr() );
#endif	//__PROTECT_AWAKE
		//	mulcom	END100405	각성 보호의 두루마리

		pUser->UpdateItem(*pTarget, UI::RandomOptItem::Sync);
		pUser->AddDiagText( prj.GetText( nOk ) );
		// log
		LogItemInfo	log;
		log.Action	= szOperation;
		log.SendName	= pUser->GetName();
		log.RecvName	= szRecv;
		log.WorldId		= pUser->GetWorld()->GetID();
		log.Gold	= pUser->GetGold();
		log.Gold2	= pUser->GetGold();
		OnLogItem( log, pTarget, 1 );
		return TRUE;
	}
	pUser->AddDefinedText( nError );	
	return FALSE;
}

BOOL CDPSrvr::DoUseItemTarget_ItemLevelDown( CUser* pUser, CItemElem* pMaterial, CItemElem* pTarget )
{
	ItemProp* pProp		= pTarget->GetProp();
	if( pProp->dwParts == NULL_ID || pProp->dwLimitLevel1 == 0xFFFFFFFF )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return FALSE;
	}

	int nLevelDown	= pTarget->GetLevelDown();
	if( nLevelDown == 0 || ( nLevelDown == -5 && pMaterial->m_dwItemId == II_SYS_SYS_SCR_LEVELDOWN02 ) )
	{
		pTarget->SetLevelDown( pMaterial->m_dwItemId == II_SYS_SYS_SCR_LEVELDOWN01? CItemElem::e5LevelDown: CItemElem::e10LevelDown );
		pUser->UpdateItem(*pTarget, UI::RandomOptItem::Sync);
		// log
		LogItemInfo	log;
		log.Action	= "r";
		log.SendName	= pUser->GetName();
		log.RecvName	= "::ItemLevelDown";
		log.WorldId		= pUser->GetWorld()->GetID();
		log.Gold	= pUser->GetGold();
		log.Gold2	= pUser->GetGold();
		OnLogItem( log, pTarget, 1 );
		return TRUE;
	}
	else
		pUser->AddDefinedText( TID_GAME_ITEM_LEVELDOWN01 );

	return FALSE;
}

void CDPSrvr::OnTransformItem( CAr & ar, CUser & pUser )
{	// 알변환
	CTransformStuff stuff;
	ar >> stuff; // 재료를 수신

	// 변환 번호로부터 변환 함수를 결정한다.
	ITransformer* pTransformer	= ITransformer::Transformer( stuff.GetTransform() );
	if (!pTransformer) return;
	pTransformer->Transform( &pUser, stuff );	// 변환
}

void CDPSrvr::OnTutorialState( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nState;
	char szState[64]	= { 0,};
	char szOut[64]	= { 0,};
	ar >> nState;
	ar.ReadString( szState, 64 );
	MakeTutorialStateString( szOut, nState, pUser->GetName() );
	if( lstrcmp( szOut, szState ) == 0 )
	{
		pUser->SetTutorialState( nState );
		pUser->AddSetTutorialState();
	}
}

// 픽업펫 각성 취소 메뉴 선택 핸들러
void CDPSrvr::OnPickupPetAwakeningCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_PET_AWAK_CANCEL>(pUser) )
		return;

	DWORD dwItem;
	ar >> dwItem;
	CItemElem* pItem	= pUser->GetItemId( dwItem );
	if (!IsUsableItem(pItem)) return;

	if( DoUseItemTarget_InitializeRandomOption( pUser, pItem, CRandomOptionProperty::eEatPet,
					TID_GAME_PICKUP_PET_AWAKENING_CANCEL_S001, TID_GAME_PICKUP_PET_AWAKENING_CANCEL_E001,
					"k", "PPAC" ) )
	{
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
	}
}

#ifdef __AZRIA_1023
void CDPSrvr::OnDoUseItemInput( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	DWORD dwData;
	ar >> dwData;
	char szInput[MAX_INPUT_LEN]		= { 0,};
	ar.ReadString( szInput, MAX_INPUT_LEN );
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	pUser->SetInput( szInput );
	pUser->OnDoUseItem( dwData, 0, -1 );
	pUser->ResetInput();
}
#endif	// __AZRIA_1023

void CDPSrvr::OnClearPetName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		CPet* pPet	= pUser->GetPet();
		if( !pPet )
		{
			pUser->AddDefinedText( TID_GAME_NAME_PET_E00 );
			return;
		}
		pPet->SetName( "" );
		g_UserMng.AddSetPetName( pUser, pPet->GetName() );
	}
}

void CDPSrvr::OnRainbowRacePrevRankingOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	pUser->AddRainbowRacePrevRankingOpenWnd();
}

void CDPSrvr::OnRainbowRaceApplicationOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	pUser->AddRainbowRaceApplicationOpenWnd();
}
void CDPSrvr::OnRainbowRaceApplication( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	
	CRainbowRaceMng::GetInstance()->SetApplicationUser( pUser );
}
void CDPSrvr::OnRainbowRaceMiniGamePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;
	
	BOOL bExt;
	ar >> bExt;

	__MINIGAME_PACKET* pMiniGamePacket;
	if( bExt )	pMiniGamePacket = new __MINIGAME_EXT_PACKET;
	else		pMiniGamePacket = new __MINIGAME_PACKET;

	pMiniGamePacket->Serialize( ar );
	CRainbowRaceMng::GetInstance()->OnMiniGamePacket( pUser, pMiniGamePacket );
	SAFE_DELETE( pMiniGamePacket );
}

void CDPSrvr::OnRainbowRaceReqFinish( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CRainbowRaceMng::GetInstance()->SetRanking( pUser );
}

void CDPSrvr::OnHousingSetupFurniture( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	HOUSINGINFO housingInfo;
	ar >> housingInfo;

	// 플레이어가 방에 있어야 하고 자신의 레이어에 들어가 있는 경우만 가능...
	CHousingMng::GetInstance()->ReqSetupFurniture( pUser, housingInfo );
}

void CDPSrvr::OnHousingSetVisitAllow( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	DWORD dwPlayerId;
	BOOL  bAllow;

	ar >> dwPlayerId >> bAllow;

	CHousingMng::GetInstance()->ReqSetAllowVisit( pUser, dwPlayerId, bAllow );
}

void CDPSrvr::OnHousingVisitRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	DWORD dwPlayerId;
	ar >> dwPlayerId;

	CHousingMng::GetInstance()->SetVisitRoom( pUser, dwPlayerId );
}

void CDPSrvr::OnHousingVisitableList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CHousingMng::GetInstance()->OnReqVisitableList( pUser );
}

void CDPSrvr::OnHousingGoOut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	CHousingMng::GetInstance()->GoOut( pUser );
}

void CDPSrvr::OnReqQuestNPCPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	char szCharKey[64] = {0,};
	ar.ReadString( szCharKey, 64 );

	LPCHARACTER lpChar = prj.GetCharacter( szCharKey );
	if( lpChar )
	{
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() == lpChar->m_dwWorldId )
			pUser->AddNPCPos( lpChar->m_vPos );
		else
			pUser->AddDefinedText( TID_GAME_QUESTINFO_FAIL );
	}
}

void CDPSrvr::OnPropose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		char szPlayer[MAX_PLAYER]	= { 0,};
		ar.ReadString( szPlayer, MAX_PLAYER );
		CCoupleHelper::Instance()->OnPropose( pUser, szPlayer );
	}
}

void CDPSrvr::OnRefuse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnRefuse( pUser );
}

void CDPSrvr::OnCouple( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnCouple( pUser );
}

void CDPSrvr::OnDecouple( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
		CCoupleHelper::Instance()->OnDecouple( pUser );
}

#ifdef __MAP_SECURITY
void CDPSrvr::OnMapKey( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		char szFileName[33] = {0,}, szMapKey[33] = {0,};
		ar.ReadString( szFileName, 33 );
		ar.ReadString( szMapKey, 33 );
		g_WorldMng.CheckMapKey( pUser, szFileName, szMapKey );
	}
}
#endif // __MAP_SECURITY

#ifdef __QUIZ
void CDPSrvr::OnQuizEventEntrance( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( !IsValidObj( pUser ) )
		return;

	int nResult;
	nResult = CQuiz::GetInstance()->EntranceQuizEvent( pUser );
	if( nResult > 0 )
		pUser->AddDefinedText( nResult );
}
void CDPSrvr::OnQuizEventTeleport( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );

	if( !IsValidObj( pUser ) )
		return;

	int nZone;
	ar >> nZone;

	int nResult;
	nResult = CQuiz::GetInstance()->TeleportToQuizEvent( pUser, nZone );
	if( nResult > 0 )
		pUser->AddDefinedText( nResult );
}
#endif // __QUIZ

void CDPSrvr::OnRemoveVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPos;
	ar >> nPos;

	CItemUpgrade::GetInstance()->RemovePetVisItem( pUser, nPos );
}

void CDPSrvr::OnSwapVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPos1, nPos2;
	ar >> nPos1 >> nPos2;

	CItemUpgrade::GetInstance()->SwapVis( pUser, nPos1, nPos2 );
}

void CDPSrvr::OnBuyGuildHouse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->ReqBuyGuildHouse( pUser );
}

void CDPSrvr::OnGuildHousePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> nPacketType >> nIndex >> gfi;

	if( nPacketType == GUILDHOUSE_PCKTTYPE_LISTUP )
		return;

	GuildHouseMng->SendWorldToDatabase( pUser, nPacketType, gfi, nIndex );
}

void CDPSrvr::OnGuildHouseEnter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->EnteranceGuildHouse( pUser );
}

void CDPSrvr::OnGuildHouseGoOut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( !IsValidObj( pUser ) )
		return;

	GuildHouseMng->GoOutGuildHouse( pUser );
}

void CDPSrvr::OnTeleporterReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser	=	g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) ) 
	{
		CHAR	m_szKey[64];
		int		nIndex;
		ar.ReadString(m_szKey, 64);
		ar >> nIndex;
		
		LPCHARACTER lpChar = prj.GetCharacter( m_szKey );
		if( lpChar )
		{
			if( !CNpcChecker::GetInstance()->IsCloseNpc<MMI_TELEPORTER>(pUser) )
				return;

			if( (int)( lpChar->m_vecTeleportPos.size() ) <= nIndex )
				return;

			pUser->Replace( WI_WORLD_MADRIGAL, lpChar->m_vecTeleportPos[nIndex], REPLACE_NORMAL, nDefaultLayer );
		}
	}
}

void CDPSrvr::OnCheckedQuest( CAr & ar, CUser & pUser ) {
	if (!pUser.m_quests) return;

	const auto [nQuestid, bCheck] = ar.Extract<QuestId, BOOL>();

	bool stable = true;

	auto & checked = pUser.m_quests->checked;

	const auto itCurrentlyChecked = std::ranges::find(checked, nQuestid);
	if (itCurrentlyChecked != checked.end()) {
		checked.erase(itCurrentlyChecked);
		stable = false;
	}

	if (bCheck) {
		if (checked.size() < checked.max_size()) {
			checked.emplace_back(nQuestid);
			stable = false;
		}
	}

	if (!stable) pUser.AddCheckedQuest();
}

void CDPSrvr::OnInviteCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pRequest = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pRequest ) )
	{
		u_long idTarget;
		ar >> idTarget;

		CUser* pTarget = g_UserMng.GetUserByPlayerID( idTarget );
		if( IsValidObj( pTarget ) )
			CCampusHelper::GetInstance()->OnInviteCampusMember( pRequest, pTarget );
	}
}

void CDPSrvr::OnAcceptCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pTarget = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pTarget ) )
	{
		u_long idRequest;
		ar >> idRequest;

		CUser* pRequest = g_UserMng.GetUserByPlayerID( idRequest );
		if( IsValidObj( pRequest ) )
			CCampusHelper::GetInstance()->OnAcceptCampusMember( pRequest, pTarget );
	}
}

void CDPSrvr::OnRefuseCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pTarget = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pTarget ) )
	{
		u_long idRequest;
		ar >> idRequest;

		CUser* pRequest = g_UserMng.GetUserByPlayerID( idRequest );
		if( IsValidObj( pRequest ) )
			pRequest->AddDefinedText( TID_GAME_TS_REFUSAL, "\"%s\"", pTarget->GetName() );
	}
}

void CDPSrvr::OnRemoveCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pRequest = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pRequest ) )
	{
		u_long idTarget;
		ar >> idTarget;

		if( idTarget > 0 )
			CCampusHelper::GetInstance()->OnRemoveCampusMember( pRequest, idTarget );
	}
}




//	mulcom	BEGIN100405	각성 보호의 두루마리
void	CDPSrvr::OnItemSelectAwakeningValue( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pRequest = g_UserMng.GetUser( dpidCache, dpidUser );

	if( IsValidObj( pRequest ) == TRUE )
	{
		DWORD	dwItemObjID		= 0;
		DWORD	dwSerialNumber	= 0;
		BYTE	bySelectFlag	= 0;

		ar >> dwItemObjID;
		ar >> dwSerialNumber;
		ar >> bySelectFlag;

		pRequest->SelectAwakeningValue( dwItemObjID, dwSerialNumber, bySelectFlag );
	}
	else
	{
		WriteLog( "pUser is invalid in OnItemSelectAwakeningValue function." );
	}
}
//	mulcom	END100405	각성 보호의 두루마리

#ifdef __GUILD_HOUSE_MIDDLE
void CDPSrvr::OnGuildHouseTenderMainWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		DWORD dwGHType;
		OBJID objNpcId;
		ar >> dwGHType >> objNpcId;

		GuildHouseMng->ReqTenderGuildHouseList( pUser, dwGHType, objNpcId );
	}
}

void CDPSrvr::OnGuildHouseTenderInfoWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		OBJID objGHId;
		ar >> objGHId;

		GuildHouseMng->ReqTenderGuildHouseInfo( pUser, objGHId );
	}
}

void CDPSrvr::OnGuildHouseTenderJoin( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		OBJID objGHId;
		int nTenderPerin, nTenderPenya;
		ar >> objGHId >> nTenderPerin >> nTenderPenya;
		
		GuildHouseMng->OnGuildHouseTenderJoin( pUser, objGHId, nTenderPerin, nTenderPenya );
	}
}
#endif // __GUILD_HOUSE_MIDDLE
