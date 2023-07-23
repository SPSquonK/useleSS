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
	void	OnChat( CAr & ar, DPID dpidCache, DPID dpidUser );
	void  OnDoEquip(CAr & ar, CUser & pUser);
	void	OnCtrlCoolTimeCancel( CAr & ar, CUser * pUser );		
	void	OnMoveItem( CAr & ar, CUser & pUser );
	void	OnDropItem( CAr & ar, CUser & pUser );
	void	OnTrade( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnConfirmTrade( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnConfirmTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradePut( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradePull( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradePutGold( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradeClearGold( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradeOk( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTradelastConfrim( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPVendorOpen( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPVendorClose( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRegisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnUnregisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnBuyPVendorItem( CAr & ar, CUser & pUser );
	void	OnEnchant( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnSmeltSafety( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnRemoveAttribute( CAr & ar, CUser & pUser_);
	void	OnChangeAttribute(CAr & ar, CUser & pUser);
	void	OnCreateSfxObj( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnPiercing(CAr & ar, CUser & pUser);
	void	OnPiercingSize( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnPiercingRemove(CAr & ar, CUser & pUser);
	void	OnExpBoxInfo( CAr & ar, CUser & pUser );
	void	OnItemTransy( CAr & ar, CUser & pUser );
	void	OnUpgradeBase( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnCommercialElem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMotion( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRepairItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnScriptDialogReq( CAr & ar, CUser * pUser );
	void	OnOpenShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnCloseShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnBuyItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnBuyChipItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSellItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnOpenBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnOpenGuildBankWnd(CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnCloseGuildBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPutItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGetItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildBankMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnCloseBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDoUseSkillPoint( CAr & ar, CUser & pUser );
	void	OnBankToBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPutItemBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGetItemBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPutGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGetGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMoveBankItem( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnChangeBankPass( CAr & ar, CUser & pUser);
	void	OnConfirmBank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRevival( CAr & ar, CUser * pUser );
	void	OnRevivalLodestar( CAr & ar, CUser * pUser );
	void	OnRevivalLodelight( CAr & ar, CUser * pUser ); 
	void	OnSetLodelight( CAr & ar, CUser * pUser );
	void	OnPlayerMoved( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerBehavior( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerMoved2( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerBehavior2( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerAngle( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerCorr( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerCorr2( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerSetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser );
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
	void	OnDuelRequest( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDuelYes( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDuelNo( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDuelPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDuelPartyYes( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDuelPartyNo( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMoverFocus( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRemoveQuest( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryPlayerData( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryPlayerData2( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnAvailPocket( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMoveItemOnPocket( CAr & ar, DPID dpidCache, DPID dpidUser );
#ifdef __JEFF_11
	void	OnQuePetResurrection( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif	// __JEFF_11
#ifdef __JEFF_11_4
	void	OnArenaEnter( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnArenaExit( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif	// __JEFF_11_4
	void	OnGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnIgnoreGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnEndSkillQueue( CAr & ar, CUser & pUser );
	void	OnDoUseItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDoUseItemTarget( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnAwakening( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRemoveItemLevelDown( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnBlessednessCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMeleeAttack( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMeleeAttack2( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMagicAttack( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRangeAttack( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnUseSkill( CAr & ar, CUser & pUser);
	void	OnSfxID( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSfxHit( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSfxClear( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSnapshot( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSetTarget( CAr & ar, DPID dpidCache, DPID dpidUser );
//	void	OnDoCollect( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTeleSkill( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPlayerPos( CAr & ar, CUser* pUser );
	void	OnPlayerDestPos( CAr & ar, CUser* pUser );
//	void	OnPlayerDestAngle( CAr & ar, CUser* pUser );
	void	OnExpUp( CAr & ar, CUser & pUser);
	void	OnChangeJob(CAr & ar, CUser & pUser);
	void	OnSetHair( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnBlock( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnError( CAr & ar, DPID dpidCache, DPID dpidUser);
	void	OnShipActMsg( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnLocalPosFromIA( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildLogo( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildContribution( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildNotice( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRequestGuildRank( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnEnterChattingRoom(CAr & ar, CUser & pUser);
	void	OnChatting(CAr & ar, CUser & pUser);
	void	OnOpenChattingRoom( CAr & ar, CUser & pUser );
	void	OnCloseChattingRoom( CAr & ar, CUser & pUser );
	void	OnCommonPlace( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSetNaviPoint( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGameMasterWhisper( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnNWWantedGold( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnNWWantedList( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnNWWantedInfo( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnNWWantedName( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnReqLeave(CAr & ar, CUser & pUser);
	void	OnResurrectionOK(CAr & ar, CUser & pUser);
	void	OnResurrectionCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnChangeMode( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnStateMode( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQuerySetPlayerName( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQuerySetGuildName( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDoEscape( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnCheering( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryEquip( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnOptionEnableRenderMask( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryEquipSetting( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnFoucusObj( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnReturnScroll ( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnChangeFace(CAr & ar, CUser & pUser);
	void	OnQueryPostMail( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryRemoveMail( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryGetMailItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryGetMailGold( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryReadMail( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryMailBox( CAr & ar, CUser * pUser );
	void	OnGCApp( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCRequestStatus( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCSelectPlayer( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCSelectMap( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCJoin( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCGetPenyaGuild( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCGetPenyaPlayer( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCTele( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGCPlayerPoint( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSummonFriend( CAr & ar, CUser * pUser );
	void	OnSummonFriendConfirm( CAr & ar, CUser * pUser );
	void	OnSummonFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSummonParty( CAr & ar, CUser * pUser );
	void	OnSummonPartyConfirm( CAr & ar, CUser * pUser );

	void	OnRemoveInvenItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRandomScroll( CAr & ar, DPID dpidCache, DPID dpidUser );
	
	void	OnCreateMonster( CAr & ar, DPID dpidCache, DPID dpidUser );
	
	void	OnCreateAngel(CAr & ar, CUser & pUser);
	void	OnAngleBuff(CAr & ar, CUser & pUser);
	
#ifdef __EVE_MINIGAME
	void OnKawibawiboStart( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnKawibawiboGetItem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnReassembleStart( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnReassembleOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnAlphabetOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnAlphabetStart( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnFiveSystemOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnFiveSystemBet( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnFiveSystemStart( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnFiveSystemDestroyWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif // __EVE_MINIGAME

	void OnUltimateMakeItem(CAr & ar, CUser & pUser);
	void OnUltimateMakeGem(CAr & ar, CUser & pUser);
	void OnUltimateTransWeapon(CAr & ar, CUser & pUser);
	void OnUltimateSetGem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnUltimateRemoveGem( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnUltimateEnchantWeapon( CAr & ar, DPID dpidCache, DPID dpidUser );
	
#ifdef __TRADESYS
	void OnExchange( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif // __TRADESYS

	void	OnPetRelease( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnUsePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnMakePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPetTamerMistake( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnPetTamerMiracle( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnFeedPocketInactive( CAr & ar, DPID dpidCache, DPID dpidUser );

	void	OnQueryStartCollecting( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQueryStopCollecting( CAr & ar, DPID dpidCache, DPID dpidUser );

	void OnLegendSkillStart(CAr & ar, CUser & pUser);
	
	void OnModifyStatus( CAr & ar, DPID dpidCache, DPID dpidUser );

	void OnGC1to1TenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1TenderView( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1Tender( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1CancelTender( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1FailedTender( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1MemberLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1MemberLineUp( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1TeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnGC1to1TeleportToStage( CAr & ar, DPID dpidCache, DPID dpidUser );

	void OnQueryGuildBankLogList( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSealCharReq(CAr & ar, CUser & pUser);
	void OnSealCharConmReq( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSealCharGetReq(CAr & ar, CUser & pUser);
	void OnHonorListReq( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnHonorChangeReq( CAr & ar, DPID dpidCache, DPID dpidUser );
#ifdef __NPC_BUFF
	void OnNPCBuff( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif // __NPC_BUFF

	void OnSecretRoomTenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSecretRoomLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSecretRoomTender( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSecretRoomTenderCancelReturn( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSecretRoomLineUpMember( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSecretRoomEntrance( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSecretRoomTeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnSecretRoomTenderView( CAr & ar, DPID dpidCache, DPID dpidUser );
	void OnTeleportSecretRoomDungeon( CAr & ar, DPID dpidCache, DPID dpidUser );

	BOOL IsInviteAbleGuild( CUser* pUser );	//길드원 변경가능한 길드인가?(초대 및 탈퇴)

	void OnSetTaxRate( CAr & ar, DPID dpidCache, DPID dpidUser );

	void OnTeleportToHeavenTower( CAr & ar, DPID dpidCache, DPID dpidUser );

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
	void	OnElectionAddDeposit( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnElectionSetPledge( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnElectionIncVote( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnLEventCreate( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnLordSkillUse( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTransformItem( CAr & ar, CUser & pUser );		// 알변환 핸들러
	void	OnPickupPetAwakeningCancel( CAr & ar, DPID dpidCache, DPID dpidUser );		// 픽업펫 각성 취소

	void	OnTutorialState( CAr & ar, DPID dpidCache, DPID dpidUser );

#ifdef __AZRIA_1023
	void	OnDoUseItemInput( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif	// __AZRIA_1023
	void	OnClearPetName( CAr & ar, DPID dpidCache, DPID dpidUser );

	void	OnRainbowRacePrevRankingOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRainbowRaceApplicationOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRainbowRaceApplication( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRainbowRaceMiniGamePacket( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRainbowRaceReqFinish( CAr & ar, DPID dpidCache, DPID dpidUser );

	void	OnHousingSetupFurniture( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnHousingSetVisitAllow( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnHousingVisitRoom( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnHousingVisitableList( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnHousingGoOut( CAr & ar, DPID dpidCache, DPID dpidUser );

	void	OnReqQuestNPCPos( CAr & ar, DPID dpidCache, DPID dpidUser );

	void	OnPropose( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRefuse( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnCouple( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnDecouple( CAr & ar, DPID dpidCache, DPID dpidUser );
#ifdef __MAP_SECURITY
	void	OnMapKey( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif // __MAP_SECURITY
#ifdef __QUIZ
	void	OnQuizEventEntrance( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnQuizEventTeleport( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif // __QUIZ
	void	OnRemoveVis( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnSwapVis( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnBuyGuildHouse( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildHousePacket( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildHouseEnter( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildHouseGoOut( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnTeleporterReq( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnCheckedQuest( CAr & ar, CUser & pUser );
	void	OnInviteCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnAcceptCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRefuseCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnRemoveCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser );


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	void	OnItemSelectAwakeningValue( CAr & ar, DPID dpidCache, DPID dpidUser );
	//	mulcom	END100405	각성 보호의 두루마리

#ifdef __GUILD_HOUSE_MIDDLE
	void	OnGuildHouseTenderMainWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildHouseTenderInfoWnd( CAr & ar, DPID dpidCache, DPID dpidUser );
	void	OnGuildHouseTenderJoin( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif // __GUILD_HOUSE_MIDDLE

	void OnSquonKArbitraryPacket(CAr & ar, CUser & pUser);
private:
	DPID	m_dpidCache;		// 캐쉬서버 DPID
};

extern CDPSrvr g_DPSrvr;

#endif // __DPSRVR_H__