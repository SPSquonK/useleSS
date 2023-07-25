#ifndef __DPSRVR_H__
#define __DPSRVR_H__

#pragma once

#include "DPMng.h"
#include "MsgHdr.h"
#include "Snapshot.h"
#include <variant>
#include <sqktd/flatter_map.hpp>

class CDPSrvr;
class CUser;

// PacketHandler for the server. As it supports multiple signatures, it does
// not use the usual DPMngFeatures::PacketHandler class.
class CDPSrvrHandlers {
public:
	using GalaHandler = void (CDPSrvr:: *) (CAr &, DPID, DPID);
	using UserHandler = void (CDPSrvr:: *) (CAr &, CUser &);
	using UserPtrHandler = void (CDPSrvr:: *) (CAr &, CUser *);

	using HandlerStruct = std::variant<GalaHandler, UserHandler, UserPtrHandler>;
private:

	sqktd::flatter_map<DWORD, HandlerStruct> m_handlers;

public:
	void AddHandler(DWORD packetId, const HandlerStruct & handler) {
		m_handlers.emplace(packetId, handler);
	}

	bool Handle(CDPSrvr & self, CAr & ar, DPID dpidCache, DPID dpidUser);
};

class CDPSrvr : public CDPMng
{
	CDPSrvrHandlers m_handlers;
public:
	// Constructions
	CDPSrvr();
	virtual	~CDPSrvr();

	void OnMsg(DWORD packetId, const CDPSrvrHandlers::HandlerStruct & handler) {
		m_handlers.AddHandler(packetId, handler);
	}

	// Operations
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

private:
	// Handlers
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnAddUser( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRemoveUser( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnChat( CAr & ar, CUser * pUser );
	void  OnDoEquip(CAr & ar, CUser & pUser);
	void	OnCtrlCoolTimeCancel( CAr & ar, CUser * pUser );		
	void	OnMoveItem( CAr & ar, CUser & pUser );
	void	OnDropItem( CAr & ar, CUser & pUser );
	void	OnTrade( CAr & ar, CUser * pUser );
	void	OnConfirmTrade( CAr & ar, CUser * pUser );
	void	OnConfirmTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradePut( CAr & ar, CUser * pUser );
	void	OnTradePull( CAr & ar, CUser * pUser );
	void	OnTradePutGold( CAr & ar, CUser * pUser );
	void	OnTradeClearGold( CAr & ar, CUser * pUser );
	void	OnTradeCancel( CAr & ar, CUser * pUser );
	void	OnTradeOk( CAr & ar, CUser * pUser );
	void	OnTradelastConfrim( CAr & ar, CUser * pUser );
	void	OnPVendorOpen( CAr & ar, CUser * pUser );
	void	OnPVendorClose( CAr & ar, CUser * pUser );
	void	OnRegisterPVendorItem( CAr & ar, CUser * pUser );
	void	OnUnregisterPVendorItem( CAr & ar, CUser * pUser );
	void	OnQueryPVendorItem( CAr & ar, CUser * pUser );
	void	OnBuyPVendorItem( CAr & ar, CUser & pUser );
	void	OnEnchant( CAr & ar, CUser * pUser );
	void	OnSmeltSafety( CAr & ar, CUser * pUser );
	void	OnRemoveAttribute( CAr & ar, CUser * pUser );
	void	OnChangeAttribute(CAr & ar, CUser & pUser);
	void	OnCreateSfxObj( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnPiercing(CAr & ar, CUser & pUser);
	void	OnPiercingSize( CAr & ar, CUser * pUser );
	void	OnPiercingRemove(CAr & ar, CUser & pUser);
	void	OnExpBoxInfo( CAr & ar, CUser & pUser );
	void	OnItemTransy( CAr & ar, CUser & pUser );
	void	OnCommercialElem( CAr & ar, CUser * pUser );
	void	OnMotion( CAr & ar, CUser * pUser );
	void	OnRepairItem( CAr & ar, CUser * pUser );
	void	OnScriptDialogReq( CAr & ar, CUser * pUser );
	void	OnOpenShopWnd( CAr & ar, CUser * pUser );
	void	OnCloseShopWnd( CAr & ar, CUser * pUser );
	void	OnBuyItem( CAr & ar, CUser * pUser );
	void	OnBuyChipItem( CAr & ar, CUser * pUser );
	void	OnSellItem( CAr & ar, CUser * pUser );
	void	OnOpenBankWnd( CAr & ar, CUser * pUser );
	void	OnOpenGuildBankWnd(CAr & ar, CUser * pUser );
	void	OnCloseGuildBankWnd( CAr & ar, CUser * pUser );
	void	OnPutItemGuildBank( CAr & ar, CUser * pUser );
	void	OnGetItemGuildBank( CAr & ar, CUser * pUser );
	void	OnGuildBankMoveItem( CAr & ar, CUser * pUser );
	void	OnCloseBankWnd( CAr & ar, CUser * pUser );
	void	OnDoUseSkillPoint( CAr & ar, CUser & pUser );
	void	OnBankToBank( CAr & ar, CUser * pUser );
	void	OnPutItemBank( CAr & ar, CUser * pUser );
	void	OnGetItemBank( CAr & ar, CUser * pUser );
	void	OnPutGoldBank( CAr & ar, CUser * pUser );
	void	OnGetGoldBank( CAr & ar, CUser * pUser );
	void	OnMoveBankItem( CAr & ar, CUser * pUser );
	void	OnChangeBankPass( CAr & ar, CUser & pUser);
	void	OnConfirmBank( CAr & ar, CUser * pUser );
	void	OnRevival( CAr & ar, CUser * pUser );
	void	OnRevivalLodestar( CAr & ar, CUser * pUser );
	void	OnRevivalLodelight( CAr & ar, CUser * pUser ); 
	void	OnSetLodelight( CAr & ar, CUser * pUser );
	void	OnPlayerMoved( CAr & ar, CUser * pUser );
	void	OnPlayerBehavior( CAr & ar, CUser * pUser );
	void	OnPlayerMoved2( CAr & ar, CUser * pUser );
	void	OnPlayerBehavior2( CAr & ar, CUser * pUser );
	void	OnPlayerAngle( CAr & ar, CUser * pUser );
	void	OnPlayerCorr( CAr & ar, CUser * pUser );
	void	OnPlayerCorr2( CAr & ar, CUser * pUser );
	void	OnPlayerSetDestObj( CAr & ar, CUser * pUser );
	void	OnCorrReq( CAr & ar, CUser * pUser );
	void	OnCreateGuildCloak( CAr & ar, CUser * pUser );
	void	OnQueryGetPos(CAr & ar, CUser * pUser);
	void	OnGetPos( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryGetDestObj(CAr & ar, CUser * pUser);
	void	OnGetDestObj(CAr & ar, CUser * pUser);
	void	OnSkillTaskBar( CAr & ar, CUser & pUser );
	void OnModifyTaskBar(CAr & ar, CUser & pUser);
	void	OnPartyRequest(CAr & ar, CUser & pUser);
	void	OnAddFriendNameReqest( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPartyRequestCancle( CAr & ar, CUser & pUser);
	void	OnPartySkillUse( CAr & ar, CUser * pUser );
	void	OnAddFriendReqest( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnAddFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGetFriendName( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDuelRequest( CAr & ar, CUser * pUser );
	void	OnDuelYes( CAr & ar, CUser * pDst );
	void	OnDuelNo( CAr & ar, CUser * pUser );
	void	OnDuelPartyRequest( CAr & ar, CUser * pSrcUser );
	void	OnDuelPartyYes( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDuelPartyNo( CAr & ar, CUser * pUser );
	void	OnMoverFocus( CAr & ar, CUser * pUser );
	void	OnRemoveQuest( CAr & ar, CUser * pUser );
	void	OnQueryPlayerData( CAr & ar, CUser * pUser );
	void	OnQueryPlayerData2( CAr & ar, CUser * pUser );
	void	OnAvailPocket( CAr & ar, CUser * pUser );
	void	OnMoveItemOnPocket( CAr & ar, CUser * pUser );
#ifdef __JEFF_11
	void	OnQuePetResurrection( CAr & ar, CUser * pUser );
#endif	// __JEFF_11
#ifdef __JEFF_11_4
	void	OnArenaEnter( CAr & ar, CUser * pUser );
	void	OnArenaExit( CAr & ar, CUser * pUser );
#endif	// __JEFF_11_4
	void	OnGuildInvite( CAr & ar, CUser * pUser );
	void	OnIgnoreGuildInvite( CAr & ar, CUser * pUser );
	void	OnEndSkillQueue( CAr & ar, CUser & pUser );
	void	OnDoUseItem( CAr & ar, CUser * pUser );
	void	OnDoUseItemTarget( CAr & ar, CUser * pUser );
	void	OnAwakening( CAr & ar, CUser * pUser );
	void	OnRemoveItemLevelDown( CAr & ar, CUser * pUser );
	void	OnBlessednessCancel( CAr & ar, CUser * pUser );
	void	OnMeleeAttack( CAr & ar, CUser * pUser );
	void	OnMeleeAttack2( CAr & ar, CUser * pUser );
	void	OnMagicAttack( CAr & ar, CUser * pUser );
	void	OnRangeAttack( CAr & ar, CUser * pUser );
	void	OnUseSkill( CAr & ar, CUser & pUser );
	void	OnSfxID( CAr & ar, CUser * pUser );
	void	OnSfxHit( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSfxClear( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSnapshot( CAr & ar, CUser * pUser );
	void	OnSetTarget( CAr & ar, CUser * pUser );
//	void	OnDoCollect( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTeleSkill( CAr & ar, CUser * pUser );
	void	OnPlayerPos( CAr & ar, CUser* pUser );
	void	OnPlayerDestPos( CAr & ar, CUser* pUser );
//	void	OnPlayerDestAngle( CAr & ar, CUser* pUser );
	void	OnExpUp( CAr & ar, CUser & pUser);
	void	OnChangeJob(CAr & ar, CUser & pUser);
	void	OnSetHair( CAr & ar, CUser * pUser );
	void	OnBlock( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnError( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnShipActMsg( CAr & ar, CUser * pUser );
	void	OnLocalPosFromIA( CAr & ar, CUser * pUser );
	void	OnGuildLogo( CAr & ar, CUser * pUser );
	void	OnGuildContribution( CAr & ar, CUser * pUser );
	void	OnGuildNotice( CAr & ar, CUser * pUser );
	void	OnRequestGuildRank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnEnterChattingRoom(CAr & ar, CUser & pUser);
	void	OnChatting(CAr & ar, CUser & pUser);
	void	OnOpenChattingRoom( CAr & ar, CUser & pUser );
	void	OnCloseChattingRoom( CAr & ar, CUser & pUser );
	void	OnCommonPlace( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSetNaviPoint( CAr & ar, CUser * pUser );
	void	OnGameMasterWhisper( CAr & ar, CUser * pUser );
	void	OnNWWantedGold( CAr & ar, CUser * pUser );
	void	OnNWWantedList( CAr & ar, CUser * pUser );
	void	OnNWWantedInfo( CAr & ar, CUser * pUser );
	void	OnNWWantedName( CAr & ar, CUser * pUser );
	void	OnReqLeave(CAr & ar, CUser & pUser);
	void	OnResurrectionOK(CAr & ar, CUser & pUser);
	void	OnResurrectionCancel( CAr & ar, CUser * pUser );
	void	OnChangeMode( CAr & ar, CUser * pUser );
	void	OnStateMode( CAr & ar, CUser * pUser );
	void	OnQuerySetPlayerName( CAr & ar, CUser * pUser );
	void	OnQuerySetGuildName( CAr & ar, CUser * pUser );
	void	OnDoEscape( CAr & ar, CUser * pUser );
	void	OnCheering( CAr & ar, CUser * pUser );
	void	OnQueryEquip( CAr & ar, CUser * pUser );
	void	OnOptionEnableRenderMask( CAr & ar, CUser * pUser );
	void	OnQueryEquipSetting( CAr & ar, CUser * pUser );
	void	OnReturnScroll ( CAr & ar, CUser * pUser );
	void	OnChangeFace(CAr & ar, CUser & pUser);
	void	OnQueryPostMail( CAr & ar, CUser * pUser );
	void	OnQueryRemoveMail( CAr & ar, CUser * pUser );
	void	OnQueryGetMailItem( CAr & ar, CUser * pUser );
	void	OnQueryGetMailGold( CAr & ar, CUser * pUser );
	void	OnQueryReadMail( CAr & ar, CUser * pUser );
	void	OnQueryMailBox( CAr & ar, CUser * pUser );
	void	OnGCApp( CAr & ar, CUser * pUser );
	void	OnGCCancel( CAr & ar, CUser * pUser );
	void	OnGCRequestStatus( CAr & ar, CUser * pUser );
	void	OnGCSelectPlayer( CAr & ar, CUser * pUser );
	void	OnGCSelectMap( CAr & ar, CUser * pUser );
	void	OnGCJoin( CAr & ar, CUser * pUser );
	void	OnGCGetPenyaGuild( CAr & ar, CUser * pUser );
	void	OnGCGetPenyaPlayer( CAr & ar, CUser * pUser );
	void	OnGCTele( CAr & ar, CUser * pUser );
	void	OnGCPlayerPoint( CAr & ar, CUser * pUser );
	void	OnSummonFriend( CAr & ar, CUser * pUser );
	void	OnSummonFriendConfirm( CAr & ar, CUser * pUser );
	void	OnSummonFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSummonParty( CAr & ar, CUser * pUser );
	void	OnSummonPartyConfirm( CAr & ar, CUser * pUser );

	void	OnRemoveInvenItem( CAr & ar, CUser * pUser );
	void	OnRandomScroll( CAr & ar, CUser * pUser );
	
	void	OnCreateMonster( CAr & ar, CUser * pUser );
	
	void	OnCreateAngel(CAr & ar, CUser & pUser);
	void	OnAngleBuff(CAr & ar, CUser & pUser);
	
#ifdef __EVE_MINIGAME
	void OnKawibawiboStart( CAr & ar, CUser * pUser );
	void OnKawibawiboGetItem( CAr & ar, CUser * pUser );
	void OnReassembleStart( CAr & ar, CUser * pUser );
	void OnReassembleOpenWnd( CAr & ar, CUser * pUser );
	void OnAlphabetOpenWnd( CAr & ar, CUser * pUser );
	void OnAlphabetStart( CAr & ar, CUser * pUser );
	void OnFiveSystemOpenWnd( CAr & ar, CUser * pUser );
	void OnFiveSystemBet( CAr & ar, CUser * pUser );
	void OnFiveSystemStart( CAr & ar, CUser * pUser );
	void OnFiveSystemDestroyWnd( CAr & ar, CUser * pUser );
#endif // __EVE_MINIGAME

	void OnUltimateMakeItem(CAr & ar, CUser & pUser);
	void OnUltimateMakeGem(CAr & ar, CUser & pUser);
	void OnUltimateTransWeapon(CAr & ar, CUser & pUser);
	void OnUltimateSetGem( CAr & ar, CUser * pUser );
	void OnUltimateRemoveGem( CAr & ar, CUser * pUser );
	void OnUltimateEnchantWeapon( CAr & ar, CUser * pUser );
	
#ifdef __TRADESYS
	void OnExchange( CAr & ar, CUser * pUser );
#endif // __TRADESYS

	void	OnPetRelease( CAr & ar, CUser * pUser );
	void	OnUsePetFeed( CAr & ar, CUser * pUser );
	void	OnMakePetFeed( CAr & ar, CUser * pUser );
	void	OnPetTamerMistake( CAr & ar, CUser * pUser );
	void	OnPetTamerMiracle( CAr & ar, CUser * pUser );
	void	OnFeedPocketInactive( CAr & ar, CUser * pUser );

	void	OnQueryStartCollecting( CAr & ar, CUser * pUser );
	void	OnQueryStopCollecting( CAr & ar, CUser * pUser );

	void OnLegendSkillStart(CAr & ar, CUser & pUser);
	
	void OnModifyStatus( CAr & ar, CUser * pUser );

	void OnGC1to1TenderOpenWnd( CAr & ar, CUser * pUser );
	void OnGC1to1TenderView( CAr & ar, CUser * pUser );
	void OnGC1to1Tender( CAr & ar, CUser * pUser );
	void OnGC1to1CancelTender( CAr & ar, CUser * pUser );
	void OnGC1to1FailedTender( CAr & ar, CUser * pUser );
	void OnGC1to1MemberLineUpOpenWnd( CAr & ar, CUser * pUser );
	void OnGC1to1MemberLineUp( CAr & ar, CUser * pUser );
	void OnGC1to1TeleportToNPC( CAr & ar, CUser * pUser );
	void OnGC1to1TeleportToStage( CAr & ar, CUser * pUser );

	void OnQueryGuildBankLogList( CAr & ar, CUser * pUser );
	void OnSealCharReq(CAr & ar, CUser & pUser);
	void OnSealCharConmReq( CAr & ar, CUser * pUser );
	void OnSealCharGetReq(CAr & ar, CUser & pUser);
	void OnHonorListReq( CAr & ar, CUser * pUser );
	void OnHonorChangeReq( CAr & ar, CUser * pUser );
#ifdef __NPC_BUFF
	void OnNPCBuff( CAr & ar, CUser * pUser );
#endif // __NPC_BUFF

	void OnSecretRoomTenderOpenWnd( CAr & ar, CUser * pUser );
	void OnSecretRoomLineUpOpenWnd( CAr & ar, CUser * pUser );
	void OnSecretRoomTender( CAr & ar, CUser * pUser );
	void OnSecretRoomTenderCancelReturn( CAr & ar, CUser * pUser );
	void OnSecretRoomLineUpMember( CAr & ar, CUser * pUser );
	void OnSecretRoomEntrance( CAr & ar, CUser * pUser );
	void OnSecretRoomTeleportToNPC( CAr & ar, CUser * pUser );
	void OnSecretRoomTenderView( CAr & ar, CUser * pUser );
	void OnTeleportSecretRoomDungeon( CAr & ar, CUser * pUser );

	BOOL IsInviteAbleGuild( CUser* pUser );	//길드원 변경가능한 길드인가?(초대 및 탈퇴)

	void OnSetTaxRate( CAr & ar, CUser * pUser );

	void OnTeleportToHeavenTower( CAr & ar, CUser * pUser );

public:
	void	QueryDestroyPlayer( DPID dpidCache, DPID dpidUser, DWORD dwAuthKey, u_long idPlayer );
	void	PutCreateItemLog( CUser* pUser, CItemElem* pItemElem, const char* szAction, const char* recv ) ;
	void	OnLogItem( LogItemInfo & info, CItemElem* pItemElem = NULL, int nItemCount = 0 );
	void	InviteParty(u_long uLeaderid, u_long uMemberid);
	void	InviteCompany( CUser* pUser, OBJID objid );
	void	UpdateGuildBank(CGuild* p_GuildBank, int p_Mode, BYTE cbUpdate = 0, u_long idPlayer = 0, CItemElem* pItemElem = NULL, DWORD dwPenya = 0, short nItemCount = 0 );
	BOOL	ClosePVendor( CUser* pUser, OBJID objidVendor );
	BOOL	IsPiercingSize( CUser* pUser, CItemElem* pItemElem0, CItemElem* pItemElem1, CItemElem* pItemElem2, int& nError );

	BOOL	DoUseItemTarget_GenRandomOption( CUser* pUser, CItemElem* pTarget, int nKind,
		int nOk, int nMismatchKind, int nHasOption,
		const char* szOperation, const char* szReceive );
	BOOL	DoUseItemTarget_InitializeRandomOption( CUser* pUser, CItemElem* pTarget, int nKind,
		int nOk, int nError,
		const char* szOperation, const char* szRecv );
	BOOL	DoUseItemTarget_ItemLevelDown( CUser* pUser, CItemElem* pMaterial, CItemElem* pTarget );

public:
	void	PutItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nNum = 0 );
	void	PutPenyaLog( CUser* pUser, const char* szAction, const char* szContext, int nCost );
private:
	void	OnElectionAddDeposit( CAr & ar, CUser * pUser );
	void	OnElectionSetPledge( CAr & ar, CUser * pUser );
	void	OnElectionIncVote( CAr & ar, CUser * pUser );
	void	OnLEventCreate( CAr & ar, CUser * pUser );
	void	OnLordSkillUse( CAr & ar, CUser * pUser );
	void	OnTransformItem( CAr & ar, CUser & pUser );		// 알변환 핸들러
	void	OnPickupPetAwakeningCancel( CAr & ar, CUser * pUser );		// 픽업펫 각성 취소

	void	OnTutorialState( CAr & ar, CUser * pUser );

#ifdef __AZRIA_1023
	void	OnDoUseItemInput( CAr & ar, CUser * pUser );
#endif	// __AZRIA_1023
	void	OnClearPetName( CAr & ar, CUser * pUser );

	void	OnRainbowRacePrevRankingOpenWnd( CAr & ar, CUser * pUser );
	void	OnRainbowRaceApplicationOpenWnd( CAr & ar, CUser * pUser );
	void	OnRainbowRaceApplication( CAr & ar, CUser * pUser );
	void	OnRainbowRaceMiniGamePacket( CAr & ar, CUser * pUser );
	void	OnRainbowRaceReqFinish( CAr & ar, CUser * pUser );

	void	OnHousingSetupFurniture( CAr & ar, CUser * pUser );
	void	OnHousingSetVisitAllow( CAr & ar, CUser * pUser );
	void	OnHousingVisitRoom( CAr & ar, CUser * pUser );
	void	OnHousingVisitableList( CAr & ar, CUser * pUser );
	void	OnHousingGoOut( CAr & ar, CUser * pUser );

	void	OnReqQuestNPCPos( CAr & ar, CUser * pUser );

	void	OnPropose( CAr & ar, CUser * pUser );
	void	OnRefuse( CAr & ar, CUser * pUser );
	void	OnCouple( CAr & ar, CUser * pUser );
	void	OnDecouple( CAr & ar, CUser * pUser );
#ifdef __MAP_SECURITY
	void	OnMapKey( CAr & ar, CUser * pUser );
#endif // __MAP_SECURITY
#ifdef __QUIZ
	void	OnQuizEventEntrance( CAr & ar, CUser * pUser );
	void	OnQuizEventTeleport( CAr & ar, CUser * pUser );
#endif // __QUIZ
	void	OnRemoveVis( CAr & ar, CUser * pUser );
	void	OnSwapVis( CAr & ar, CUser * pUser );
	void	OnBuyGuildHouse( CAr & ar, CUser * pUser );
	void	OnGuildHousePacket( CAr & ar, CUser * pUser );
	void	OnGuildHouseEnter( CAr & ar, CUser * pUser );
	void	OnGuildHouseGoOut( CAr & ar, CUser * pUser );
	void	OnTeleporterReq( CAr & ar, CUser * pUser );
	void	OnCheckedQuest( CAr & ar, CUser & pUser );
	void	OnInviteCampusMember( CAr & ar, CUser * pRequest );
	void	OnAcceptCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRefuseCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRemoveCampusMember( CAr & ar, CUser * pRequest );


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	void	OnItemSelectAwakeningValue( CAr & ar, CUser * pRequest );
	//	mulcom	END100405	각성 보호의 두루마리

#ifdef __GUILD_HOUSE_MIDDLE
	void	OnGuildHouseTenderMainWnd( CAr & ar, CUser * pUser );
	void	OnGuildHouseTenderInfoWnd( CAr & ar, CUser * pUser );
	void	OnGuildHouseTenderJoin( CAr & ar, CUser * pUser );
#endif // __GUILD_HOUSE_MIDDLE

	void OnSquonKArbitraryPacket(CAr & ar, CUser & pUser);
private:
	DPID	m_dpidCache;		// 캐쉬서버 DPID
};

extern CDPSrvr g_DPSrvr;

#endif // __DPSRVR_H__