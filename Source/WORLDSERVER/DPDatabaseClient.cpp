#include "StdAfx.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "User.h"
#include "WorldMng.h"
#include "misc.h"
#include "sqktd/util.hpp"
#include "playerdata.h"

#include "eveschool.h"
#include "definetext.h"
#include "eveschool.h"
#include "party.h"
#include "guild.h"

#include "post.h"
extern	BOOL CanAdd( DWORD dwGold, int nPlus );
#include "spevent.h"

#include "slord.h"

#include "Tax.h"

extern char		g_szDBAddr[16];

#include "couplehelper.h"

#ifdef __FUNNY_COIN
#include "FunnyCoin.h"
#endif // __FUNNY_COIN

#include "PCBang.h"

#include "InstanceDungeonBase.h"

#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ

#include "GuildHouse.h"

#include "CampusHelper.h"

CDPDatabaseClient	g_dpDBClient;

CDPDatabaseClient::CDPDatabaseClient()
{
	ON_MSG( PACKETTYPE_JOIN_DbWorld, &CDPDatabaseClient::OnJoin );
	ON_MSG( PACKETTYPE_ALL_PLAYER_DATA, &CDPDatabaseClient::OnAllPlayerData );
	ON_MSG( PACKETTYPE_ADD_PLAYER_DATA, &CDPDatabaseClient::OnAddPlayerData );
	ON_MSG( PACKETTYPE_DELETE_PLAYER_DATA, &CDPDatabaseClient::OnDeletePlayerData );
	ON_MSG( PACKETTYPE_UPDATE_PLAYER_DATA, &CDPDatabaseClient::OnUpdatePlayerData );
	ON_MSG( PACKETTYPE_GUILD_BANK, &CDPDatabaseClient::OnGuildBank );
	ON_MSG( PACKETTYPE_UPDATE_GUILD_RANKING_END, &CDPDatabaseClient::OnUpdateGuildRankingFinish );
	ON_MSG( PACKETTYPE_QUERYGUILDQUEST, &CDPDatabaseClient::OnQueryGuildQuest );
	ON_MSG( PACKETTYPE_BASEGAMESETTING, &CDPDatabaseClient::OnBaseGameSetting );
	ON_MSG( PACKETTYPE_MONSTERRESPAWNSETTING, &CDPDatabaseClient::OnMonsterRespawnSetting );
	ON_MSG( PACKETTYPE_MONSTERPROPGAMESETTING, &CDPDatabaseClient::OnMonsterPropSetting );
	ON_MSG( PACKETTYPE_GAMEMASTER_CHATTING, &CDPDatabaseClient::OnGMChat );
	ON_MSG( PACKETTYPE_PING, &CDPDatabaseClient::OnPing );
	ON_MSG( PACKETTYPE_ADD_GUILDCOMBAT, &CDPDatabaseClient::OnAllGuildCombat );
	ON_MSG( PACKETTYPE_IN_GUILDCOMBAT, &CDPDatabaseClient::OnInGuildCombat );
	ON_MSG( PACKETTYPE_OUT_GUILDCOMBAT, &CDPDatabaseClient::OnOutGuildCombat );
	ON_MSG( PACKETTYPE_RESULT_GUILDCOMBAT, &CDPDatabaseClient::OnResultGuildCombat );
	ON_MSG( PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, &CDPDatabaseClient::OnPlayerPointGuildCombat );
	ON_MSG( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, &CDPDatabaseClient::OnGetPenyaGuildGC );
	ON_MSG( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, &CDPDatabaseClient::OnGetPenyaPlayerGC );
	ON_MSG( PACKETTYPE_CONTINUE_GUILDCOMBAT, &CDPDatabaseClient::OnContinueGC );
	ON_MSG( PACKETTYPE_QUERYPOSTMAIL, &CDPDatabaseClient::OnPostMail );
	ON_MSG( PACKETTYPE_QUERYREMOVEMAIL,	&CDPDatabaseClient::OnRemoveMail );
	ON_MSG( PACKETTYPE_QUERYGETMAILITEM,	&CDPDatabaseClient::OnGetMailItem );
	ON_MSG( PACKETTYPE_QUERYGETMAILGOLD,	&CDPDatabaseClient::OnGetMailGold );
	ON_MSG( PACKETTYPE_READMAIL,	&CDPDatabaseClient::OnReadMail );
	ON_MSG( PACKETTYPE_ALLMAIL, &CDPDatabaseClient::OnAllMail );
	
	

	//////////////////////////////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_QUERYMAILBOX    , &CDPDatabaseClient::OnMailBox );
	ON_MSG( PACKETTYPE_QUERYMAILBOX_REQ, &CDPDatabaseClient::OnMailBoxReq );
	//////////////////////////////////////////////////////////////////////////


	ON_MSG( PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, &CDPDatabaseClient::OnQueryRemoveGuildBankTbl );
	ON_MSG( PACKETTYPE_EVENT_GENERIC, &CDPDatabaseClient::OnEventGeneric );
	ON_MSG( PACKETTYPE_EVENT_FLAG, &CDPDatabaseClient::OnEventFlag );

	ON_MSG( PACKETTYPE_EVENTLUA_STATE, &CDPDatabaseClient::OnEventStateLua );
	ON_MSG( PACKETTYPE_EVENTLUA_CHANGED, &CDPDatabaseClient::OnEventLuaChanged );

	ON_MSG( PACKETTYPE_GC1TO1_OPEN, &CDPDatabaseClient::OnGC1to1Open );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERTOSRVR, &CDPDatabaseClient::OnGC1to1TenderGuildFromDB );
	ON_MSG( PACKETTYPE_GUILDLOG_VIEW, &CDPDatabaseClient::OnGuildBankLogViewFromDB );
	ON_MSG( PACKETTYPE_SEALCHAR_REQ, &CDPDatabaseClient::OnSealCharFromDB );
	ON_MSG( PACKETTYPE_SEALCHARGET_REQ, &CDPDatabaseClient::OnSealCharGetFromDB );

	ON_MSG( PACKETTYPE_ELECTION_ADD_DEPOSIT, &CDPDatabaseClient::OnElectionAddDeposit );
	ON_MSG( PACKETTYPE_ELECTION_SET_PLEDGE, &CDPDatabaseClient::OnElectionSetPledge );
	ON_MSG( PACKETTYPE_ELECTION_INC_VOTE, &CDPDatabaseClient::OnElectionIncVote );
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_CANDIDACY, &CDPDatabaseClient::OnElectionBeginCandidacy );
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_VOTE, &CDPDatabaseClient::OnElectionBeginVote );
	ON_MSG( PACKETTYPE_ELECTION_END_VOTE, &CDPDatabaseClient::OnElectionEndVote );
	ON_MSG( PACKETTYPE_LORD, &CDPDatabaseClient::OnLord );
	ON_MSG( PACKETTYPE_L_EVENT_CREATE, &CDPDatabaseClient::OnLEventCreate );
	ON_MSG( PACKETTYPE_L_EVENT_INITIALIZE, &CDPDatabaseClient::OnLEventInitialize );
	ON_MSG( PACKETTYPE_LORD_SKILL_USE, &CDPDatabaseClient::OnLordSkillUse );
	ON_MSG( PACKETTYPE_LORD_SKILL_TICK, &CDPDatabaseClient::OnLordSkillTick );
	ON_MSG( PACKETTYPE_L_EVENT_TICK, &CDPDatabaseClient::OnLEventTick );

	ON_MSG( PACKETTYPE_TAX_ALLINFO, &CDPDatabaseClient::OnTaxInfo );

	ON_MSG( PACKETTYPE_SECRETROOM_INFO_CLEAR, &CDPDatabaseClient::OnSecretRoomInfoClear );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDERINFO_TO_WSERVER, &CDPDatabaseClient::OnSecretRoomTenderInfo );

	ON_MSG( PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, &CDPDatabaseClient::OnRainbowRaceInfo );

	ON_MSG( PACKETTYPE_HOUSING_LOADINFO, &CDPDatabaseClient::OnHousingLoadInfo );
	ON_MSG( PACKETTYPE_HOUSING_FURNITURELIST, &CDPDatabaseClient::OnHousingSetFunitureList );
	ON_MSG( PACKETTYPE_HOUSING_SETUPFURNITURE, &CDPDatabaseClient::OnHousingSetupFuniture );
	ON_MSG( PACKETTYPE_HOUSING_SETVISITALLOW, &CDPDatabaseClient::OnHousingSetVisitAllow );
	ON_MSG( PACKETTYPE_HOUSING_DBFAILED, &CDPDatabaseClient::OnHousingDBFailed );
	m_bAlive	= TRUE;
	m_cbPing	= 0;
	ON_MSG( PACKETTYPE_PROPOSE, &CDPDatabaseClient::OnProposeResult );
	ON_MSG( PACKETTYPE_COUPLE, &CDPDatabaseClient::OnCoupleResult );
	ON_MSG( PACKETTYPE_DECOUPLE, &CDPDatabaseClient::OnDecoupleResult );
	ON_MSG( PACKETTYPE_ALL_COUPLES, &CDPDatabaseClient::OnCouple );
	ON_MSG( PACKETTYPE_ADD_COUPLE_EXPERIENCE, &CDPDatabaseClient::OnAddCoupleExperience );
#ifdef __FUNNY_COIN
	ON_MSG( PACKETTYPE_FUNNYCOIN_ACK_USE, &CDPDatabaseClient::OnFunnyCoinAckUse );
#endif // __FUNNY_COIN
	ON_MSG( PACKETTYPE_PCBANG_SETAPPLY, &CDPDatabaseClient::OnPCBangToggle );
#ifdef __VTN_TIMELIMIT
	ON_MSG( PACKETTYPE_TIMELIMIT_INFO, &CDPDatabaseClient::OnTimeLimitInfoAck );
	ON_MSG( PACKETTYPE_TIMELIMIT_RESET, &CDPDatabaseClient::OnTimeLimitReset );
#endif // __VTN_TIMELIMIT
#ifdef __QUIZ
	ON_MSG( PACKETTYPE_QUIZ_OPEN, &CDPDatabaseClient::OnQuizEventOpen );
	ON_MSG( PACKETTYPE_QUIZ_LOADQUIZ, &CDPDatabaseClient::OnQuizList );
	ON_MSG( PACKETTYPE_QUIZ_NOTICE, &CDPDatabaseClient::OnQuizEventNotice );
	ON_MSG( PACKETTYPE_QUIZ_CHANGED, &CDPDatabaseClient::OnQuizEventChanged );
#endif // __QUIZ

	ON_MSG( PACKETTYPE_GUILDHOUSE_LOAD, &CDPDatabaseClient::OnLoadGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_BUY, &CDPDatabaseClient::OnBuyGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_REMOVE, &CDPDatabaseClient::OnRemoveGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_PACKET, &CDPDatabaseClient::OnGuildHousePacket );
#ifdef __GUILD_HOUSE_MIDDLE
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_INFO, &CDPDatabaseClient::OnGuildHouseTenderInfo );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, &CDPDatabaseClient::OnGuildHouseTenderJoin );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_STATE, &CDPDatabaseClient::OnGuildHouseTenderState );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_RESULT, &CDPDatabaseClient::OnGuildHouseTenderResult );
	ON_MSG( PACKETTYPE_GUILDHOUSE_GRADE_UPDATE, &CDPDatabaseClient::OnGuildHouseGradeUpdate );
	ON_MSG( PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, &CDPDatabaseClient::OnGuildHouseLevelUpdate );
	ON_MSG( PACKETTYPE_GUILDHOUSE_EXPIRED, &CDPDatabaseClient::OnGuildHouseExpired );
#endif // __GUILD_HOUSE_MIDDLE

	ON_MSG( PACKETTYPE_CAMPUS_ALL, &CDPDatabaseClient::OnAllCampus );
	ON_MSG( PACKETTYPE_CAMPUS_ADD_MEMBER, &CDPDatabaseClient::OnAddCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REMOVE_MEMBER, &CDPDatabaseClient::OnRemoveCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_UPDATE_POINT, &CDPDatabaseClient::OnUpdateCampusPoint );
}

CDPDatabaseClient::~CDPDatabaseClient()
{
}

void CDPDatabaseClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			break;
	}
}

void CDPDatabaseClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg + sizeof(DPID) + sizeof(DPID), dwMsgSize - ( sizeof(DPID) + sizeof(DPID) ) );
	DWORD dw; ar >> dw;

	static std::map<DWORD, CString> mapstrProfile;
	auto it = mapstrProfile.find( dw );
	if( it == mapstrProfile.end() )
	{
		CString strTemp;
		strTemp.Format("CDPDatabaseClient::UserMessageHandler(0x%08x)", dw );
		it = mapstrProfile.emplace( dw, strTemp ).first;
	}
	_PROFILE( it->second );
		
	if (Handle(ar, dw, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)((LPBYTE)lpMsg + sizeof(DPID)))) {
		if (ar.IsOverflow()) Error("World-Database: Packet %08x overflowed", dw);
	} else {
		Error("Handler not found(%08x)\n", dw);
	}
}

#ifdef __LAYER_1015
void CDPDatabaseClient::SavePlayer( CUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, int nLayer, BOOL bLogout )
#else	// __LAYER_1015
void CDPDatabaseClient::SavePlayer( CUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, BOOL bLogout )
#endif	//__LAYER_1015
{
	BEFORESENDDUAL( ar, PACKETTYPE_SAVE_PLAYER, DPID_UNKNOWN, DPID_UNKNOWN );  

	ar << bLogout;

	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << ((CMover*)pUser)->m_nSlot;

	pUser->m_dwTimeout4Save	= SEC_SAVEPLAYER;


	ar << dwWorldId;
	ar << vPos;
#ifdef __LAYER_1015
	ar << nLayer;
#endif	// __LAYER_1015

	CObj::SetMethod( METHOD_NONE );
	// raiders.2006.11.28  저장될 돈 = 인벤돈 + 내 거래창 돈
	int nTrade = pUser->m_vtInfo.TradeGetGold();
	pUser->AddGold( nTrade, FALSE );
	pUser->Serialize( ar );
	pUser->AddGold( -nTrade, FALSE );
	//
	ar << pUser->m_playTaskBar;
	ar << pUser->m_RTMessenger.GetState();

#ifndef __JEFF_9_20
	if( pUser->m_pActMover->IsFly() )
	{
		pUser->SetFlyTimeOff();
		ar << pUser->GetFlyTime();
		pUser->m_dwSaveFlyTime += pUser->m_dwSendFlyTime;
		pUser->m_dwSendFlyTime = 0;
		pUser->SetFlyTimeOn();
	}
	else
	{
		ar << pUser->GetFlyTime();
		pUser->m_dwSaveFlyTime += pUser->m_dwSendFlyTime;
		pUser->m_dwSendFlyTime = 0;
	}
#endif	// __JEFF_9_20

	ar << pUser->m_dwReturnWorldID;
	ar << pUser->m_vReturnPos;

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	ar << pUser->m_nEventFlag;
#else // __EVENT_1101_2
	ar << pUser->m_dwEventFlag;
#endif // __EVENT_1101_2
	ar << pUser->m_dwEventTime;
	ar << pUser->m_dwEventElapsed;
#endif	// __EVENT_1101
	
#ifdef __EXP_ANGELEXP_LOG
	ar << pUser->m_nExpLog;
	ar << pUser->m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	ar << pUser->m_nCoupon;
#endif // __EVENTLUA_COUPON

	ar << pUser->GetRestPoint();

	SEND( ar, this, DPID_SERVERPLAYER );
#ifdef __VTN_TIMELIMIT
	//	mulcom	BEGIN100315	베트남 시간 제한
	if( ::GetLanguage() == LANG_VTN )
	{
		SendTimeLimitUpdateReq( pUser->m_playAccount.lpszAccount, pUser->m_nAccountPlayTime );
	}
	//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT
}

#ifdef __S_RECOMMEND_EVE
void CUser::GiveRecommendEveItems( int nValue )
{
	if (!g_eLocal.GetState(EVE_RECOMMEND)) return;

	struct Item { DWORD id; short quantity = 1; BYTE flag = 0; };

	boost::container::small_vector<Item, 4> items;

	if (nValue == 0) {
		const int nLevel = GetLevel();

		if (nLevel == 5 || nLevel == 15) {
			items.emplace_back(Item{ II_CHR_SYS_SCR_UPCUTSTONE, 1, 2 });
		} else if (nLevel == 10 || nLevel == 1) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESA, 3, 2 });
		} else if (nLevel == 20) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESB, 3, 2 });
			items.emplace_back(Item{ II_CHR_SYS_SCR_UPCUTSTONE, 2, 2 });
		} else if (nLevel == 23 || nLevel == 29 || nLevel == 35 || nLevel == 43 || nLevel == 47 || nLevel == 51) {
			items.emplace_back(Item{ II_CHR_SYS_SCR_UPCUTSTONE, 2, 2 });
		} else if (nLevel == 26 || nLevel == 32 || nLevel == 38) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESB, 3, 2 });
		} else if (nLevel == 40 || nLevel == 50) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESC, 2, 2 });
			items.emplace_back(Item{ II_CHR_REF_REF_HOLD, 3, 2 });
			items.emplace_back(Item{ II_CHR_POT_DRI_VITALX, 3, 2 });
		} else if (nLevel == 45) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESC, 2, 2 });
			items.emplace_back(Item{ II_CHR_REF_REF_HOLD, 1, 2 });
			items.emplace_back(Item{ II_CHR_POT_DRI_VITALX, 1, 2 });
		} else if (nLevel == 55) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESC, 2, 2 });
			items.emplace_back(Item{ II_CHR_SYS_SCR_UPCUTSTONE, 2, 2 });
		} else if (nLevel == 58) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESC, 2, 2 });
			items.emplace_back(Item{ II_CHR_REF_REF_HOLD, 3, 2 });
			items.emplace_back(Item{ II_CHR_POT_DRI_VITALX, 3, 2 });
			items.emplace_back(Item{ II_CHR_SYS_SCR_UPCUTSTONE, 2, 2 });
		} else if (nLevel == 60) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_AMPESS, 10, 2 });
		}
	} else if (sqktd::is_among(nValue, JOB_MERCENARY, JOB_MAGICIAN, JOB_ACROBAT, JOB_ASSIST)) {
		items.emplace_back(Item{ II_SYS_SYS_SCR_BXCOSTUME01 });
	} else if (sqktd::is_among(nValue, JOB_KNIGHT, JOB_BLADE)) {
		if (GetSex() == SEX_MALE) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXMMER60SET });
		} else {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXFMER60SET });
		}

		items.emplace_back(Item{ II_SYS_SYS_SCR_BXSUHO01 });

	} else if (sqktd::is_among(nValue, JOB_RANGER, JOB_JESTER)) {
		if (GetSex() == SEX_MALE) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXMACR60SET });
		} else {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXFACR60SET });
		}

		items.emplace_back(Item{ II_SYS_SYS_SCR_BXSUHO01 });
	} else if (sqktd::is_among(nValue, JOB_RINGMASTER, JOB_BILLPOSTER)) {
		if (GetSex() == SEX_MALE) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXMASS60SET });
		} else {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXFASS60SET });
		}

		items.emplace_back(Item{ II_SYS_SYS_SCR_BXSUHO01 });
	} else if (sqktd::is_among(nValue, JOB_PSYCHIKEEPER, JOB_ELEMENTOR)) {
		if (GetSex() == SEX_MALE) {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXMMAG60SET });
		} else {
			items.emplace_back(Item{ II_SYS_SYS_SCR_BXFMAG60SET });
		}

		items.emplace_back(Item{ II_SYS_SYS_SCR_BXSUHO01 });
	}

	for (const Item & item : items) {
		CItemElem itemElem;
		itemElem.m_dwItemId = item.id;
		itemElem.m_nItemNum = item.quantity;
		itemElem.m_byFlag = item.flag;

		CreateOrSendItem(itemElem, TID_GAME_LEVELUP_CAPTION);
	}
}
#endif // __S_RECOMMAED_EVE

void CDPDatabaseClient::SendPreventLogin( LPCTSTR szAccount, DWORD dwPreventTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PREVENT_LOGIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szAccount );
	ar << dwPreventTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogConnect( CUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PLAY_CONNECT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->GetWorld()->GetID();	// world id
	ar.Write( &pUser->m_playAccount.m_stLogin, sizeof(SYSTEMTIME) );
	ar.WriteString( pUser->m_playAccount.lpAddr );	// ip
	DWORD dwSeed = pUser->GetGold() + pUser->m_dwGoldBank[pUser->m_nSlot];
	ar << dwSeed;	// seed
	ar << pUser->m_idPlayer;
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << pUser->GetLevel();
	ar << pUser->GetJob();
	ar << CPCBang::GetInstance()->GetPCBangClass( pUser->m_idPlayer );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendInGuildCombat(u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya) {
	SendPacket<PACKETTYPE_IN_GUILDCOMBAT, u_long, DWORD, DWORD>(idGuild, dwPenya, dwExistingPenya);
}

void CDPDatabaseClient::SendOutGuildCombat( u_long idGuild ) {
	SendPacket<PACKETTYPE_OUT_GUILDCOMBAT, u_long>(idGuild);
}

void CDPDatabaseClient::SendGuildCombatStart() {
	SendPacket<PACKETTYPE_START_GUILDCOMBAT>();
}

void CDPDatabaseClient::SendGuildCombatResult( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RESULT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );

	float fRequstCanclePercent = g_GuildCombatMng.m_nRequestCanclePercent / 100.0f;
	float fNotRequestPercent = g_GuildCombatMng.m_nNotRequestPercent / 100.0f;
	ar << g_GuildCombatMng.m_uWinGuildId;
	ar << g_GuildCombatMng.m_nWinGuildCount;
	ar << g_GuildCombatMng.m_uBestPlayer;
	
	// 길드 저장
	ar << (u_long)g_GuildCombatMng.m_vecGuildCombatMem.size();

	const auto contenders = g_GuildCombatMng.GetContenders();
	for (const CGuildCombat::__GuildCombatMember * pGCMember : g_GuildCombatMng.m_vecGuildCombatMem) {
		// 출전한 길드인가
		const bool bSelectGuild = std::find(contenders.begin(), contenders.end(), pGCMember) != contenders.end();

		ar << pGCMember->uGuildId;
		ar << pGCMember->nGuildPoint;
		if( bSelectGuild )	// 출전 길드
		{
			if( g_GuildCombatMng.m_uWinGuildId == pGCMember->uGuildId )	// 승리한 길드
			{
				ar << g_GuildCombatMng.GetPrizePenya( 0 );
				ar << (DWORD)0;
				ar << g_GuildCombatMng.m_nWinGuildCount;
			}
			else
			{
				ar << (__int64)0;
				ar << (DWORD)0;
				ar << (int)0;
			}
		}
		else
		{
			if( pGCMember->bRequest )	// 신청은 했는데 출전이 안된 길드 수수료 2%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fNotRequestPercent );
				ar << (int)0;

				// 바로 우편함으로 지급
				CGuild* pGuild = g_GuildMng.GetGuild( pGCMember->uGuildId );
				if( pGuild )
				{
					CItemElem pItemElem;
					LPCTSTR szMsg  = prj.GetText(TID_GAME_GC_NOTREQUEST);
					LPCTSTR szMsg1 = prj.GetText(TID_GAME_GC_NOTREQUEST1);
					SendQueryPostMail( pGuild->m_idMaster, 0, pItemElem, (int)( pGCMember->dwPenya * fNotRequestPercent ), szMsg, szMsg1 );
				}
			}
			else						// 취소한 길드 수수료 20%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fRequstCanclePercent );
				ar << (int)0;
			}
		}

		// 참가자 포인트
		ar << (u_long)pGCMember->vecGCSelectMember.size();
		for (const CGuildCombat::__JOINPLAYER * pJoinPlayer : pGCMember->vecGCSelectMember) {
			ar << pJoinPlayer->uidPlayer;
			ar << pJoinPlayer->nPoint;
			if( pJoinPlayer->uidPlayer == g_GuildCombatMng.m_uBestPlayer )
			{
				ar << g_GuildCombatMng.GetPrizePenya( 1 );
			}
			else 
			{
				ar << (__int64)0;
			}
		}
	}
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildcombatContinue(int nGuildCombatID, u_long uidGuild, int nWinCount) {
	SendPacket<PACKETTYPE_CONTINUE_GUILDCOMBAT, int, u_long, int>(nGuildCombatID, uidGuild, nWinCount);
}

void CDPDatabaseClient::SendGCGetPenyaGuild(u_long uidPlayer, int nGuildCombatID, u_long uidGuild) {
	SendPacket<PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, u_long, int, u_long>(uidPlayer, nGuildCombatID, uidGuild);
}

void CDPDatabaseClient::SendGCGetPenyaPlayer(u_long uidPlayer, int nGuildCombatID, u_long uidGuild) {
	SendPacket<PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, u_long, int, u_long>(uidPlayer, nGuildCombatID, uidGuild);
}

void CDPDatabaseClient::OnJoin( CAr & ar, DPID dpidCache, DPID dpidUser )
{
	DWORD dwWorldId, dwIndex;
	DWORD dwSavePlayerTime = 0;
	DWORD dwSaveFlyTime = 0;
	int nMaximumLevel = 0;

	char lpOutputString[128]	= { 0, };
	sprintf( lpOutputString, "WORLDSERVER.EXE\t// PACKETTYPE_JOIN_DbWorld\t// %d", g_uKey );
	OutputDebugString( lpOutputString );

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( pUser )
	{

		DWORD dwAuthKey;
		ar >> dwAuthKey;
		if( pUser->m_dwAuthKey != dwAuthKey )
		{
			WriteLog( "OnJoin(): Different dwAuthKey" );
			return;		//
		}

		char lpAddrtmp[16];

#ifdef __ON_ERROR
		int nOnError	= 0;
		try
		{
#endif	// __ON_ERROR

		memcpy( lpAddrtmp, pUser->m_playAccount.lpAddr, 16 );
		ar >> pUser->m_playAccount;
		memcpy( pUser->m_playAccount.lpAddr, lpAddrtmp, 16 );

		ar >> dwWorldId;
#ifdef __LAYER_1015
		int nLayer;
		ar >> nLayer;
#endif // __LAYER_1015

		ar >> dwIndex;

#ifdef __ON_ERROR
		nOnError	= 1;
#endif	// __ON_ERROR

		pUser->SetIndex( dwIndex, FALSE );
		CObj::SetMethod( METHOD_NONE );
		pUser->Serialize( ar );
		pUser->InitMotion( pUser->m_dwMotion );
		pUser->m_nCheerPoint	= 0;
		pUser->m_dwTickCheer	= GetTickCount() + TICK_CHEERPOINT;
		
		pUser->SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );	// 셑팅 빼기

		if( pUser->GetLevel() > MAX_3RD_LEGEND_LEVEL )
		{
			WriteError( "MAX_LEVEL//%s//%d", pUser->GetName(), pUser->GetLevel() );
			pUser->m_nLevel	= MAX_3RD_LEGEND_LEVEL;
		}

		pUser->RemoveBuff( BUFF_ITEM, II_CHEERUP );

		if( !pUser->GetPet() )
			pUser->SetPetId( NULL_ID );

		pUser->m_pActMover->RemoveStateFlag( OBJSTAF_ACC );			// 비행은 멈춤상태  		
		pUser->SetAngle( 0.0f );	// 각도 초기화, 임시 코드이므로 원인을 분석하여 수정하도록 하자.

		// 이상타 땜방 dwWorldId가 200이면서 못 들어가는 유저들이 생김 
		CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
		if( pWorld && pWorld->VecInWorld( pUser->GetPos() ) == FALSE )
		{
			dwWorldId = 1;
			pUser->SetPos( D3DXVECTOR3( 6971.984f, 100.0f, 3336.884f ) );
		}

		pUser->AdjustGuildQuest( dwWorldId );
		pUser->m_nDuel	= pUser->m_idDuelParty = 0;

#ifdef __ON_ERROR
		nOnError	= 2;
#endif	// __ON_ERROR

		ar >> pUser->m_playTaskBar;

#ifdef __ON_ERROR
		nOnError	= 3; 
#endif	// __ON_ERROR

		pUser->m_RTMessenger.Serialize( ar );

#ifdef __ON_ERROR
		nOnError	= 4;
#endif	// __ON_ERROR
		ar.ReadString( pUser->m_szPartyName, 32 );
		ar.ReadString( pUser->m_szBankPass, 5 );
		ar >> dwSavePlayerTime;
		pUser->SetPlayerTime( dwSavePlayerTime );

#ifndef __JEFF_9_20
		ar >> dwSaveFlyTime;
		pUser->SetSaveFlyTime( dwSaveFlyTime );
#endif	// __JEFF_9_20

		ar >> nMaximumLevel;

		// 쪽지를 읽어들인다.
		short nTagCount;
		ar >> nTagCount;

		TAG_ENTRY tags[MAX_TAGS];
		for( short i=0; i<nTagCount; ++i )
		{
			ar >> tags[i].idFrom;					// 누구에게서 
			ar >> tags[i].dwDate;					// 언제 
			ar.ReadString( tags[i].szString, 256 );		// 쪽지 내용 
		}

		char GuildMemberTime[15] = {0,};
		ar.ReadString( GuildMemberTime, 15 );

		char cYear[5] = {0,};
		char cMonth[3] = {0,};
		char cDay[3] = {0,};
		char cHour[3] = {0,};
		char cMin[3] = {0,};
		
		strncpy(cYear, GuildMemberTime, 4);
		strncpy(cMonth, GuildMemberTime + 4, 2 );
		strncpy(cDay, GuildMemberTime + 6, 2 );
		strncpy(cHour, GuildMemberTime + 8, 2 );
		strncpy(cMin, GuildMemberTime + 10, 2 );
		
		CTime tGuildMember( atoi(cYear), atoi(cMonth), atoi(cDay), atoi(cHour), atoi(cMin), 0 );
		pUser->m_tGuildMember = tGuildMember;

		ar >> pUser->m_dwReturnWorldID;
		ar >> pUser->m_vReturnPos;

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
		ar >> pUser->m_nEventFlag;
#else // __EVENT_1101_2
		ar >> pUser->m_dwEventFlag;
#endif // __EVENT_1101_2
		ar >> pUser->m_dwEventTime;
		ar >> pUser->m_dwEventElapsed;
#endif	// __EVENT_1101

#ifdef __EXP_ANGELEXP_LOG
		ar >> pUser->m_nExpLog;
		ar >> pUser->m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
		ar >> pUser->m_nCoupon;
#endif // __EVENTLUA_COUPON
		DWORD dwPCBangClass;
		ar >> dwPCBangClass;

		int nRestPoint; time_t tLogOut;
		ar >> nRestPoint >> tLogOut;
		if( pUser->m_idGuild != 0 )
			pUser->SetRestPoint( nRestPoint );

#ifdef __ON_ERROR
		nOnError	= 5;
#endif	// __ON_ERROR

		pUser->m_pActMover->ClearState();
		pUser->RedoEquip( FALSE );
		
		if( pUser->m_nHitPoint == 0 )
			pUser->m_nHitPoint	= pUser->GetMaxHitPoint();

		pUser->SetHitPoint( pUser->GetHitPoint() );			// max넘은 경우를 대비한다.
		pUser->SetManaPoint( pUser->GetManaPoint() );		// max넘은 경우를 대비한다.
		pUser->SetFatiguePoint( pUser->GetFatiguePoint() ); // max넘은 경우를 대비한다.

		pUser->SetId( NULL_ID );
		pUser->CheckHonorStat();

		if( prj.GetUserByID( pUser->m_idPlayer ) )
		{
			WriteLog( "OnJoin(): an ocular spectrum %d", pUser->m_idPlayer );
			pUser->m_idPlayer	= (u_long)0xffffffff;
/*
			1. 이 경우는 현재 발생하지 않는다. 그러나 만약을 대비하여 추적 로그를 남기도록 한다.
*/
			return;
		}

#ifdef __ON_ERROR
		nOnError	= 6;
#endif	// __ON_ERROR

		pUser->ValidateItem();
		pUser->m_nGuildCombatState	= 0;

#ifdef __ON_ERROR
		nOnError	= 700;
#endif	// __ON_ERROR

#ifdef __INTERNALSERVER
		pUser->m_dwAuthorization = AUTH_ADMINISTRATOR;
#endif // __INTERNALSERVER

		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON );

#ifdef __ON_ERROR
		nOnError	= 701;
#endif	// __ON_ERROR

		if( !g_eLocal.GetState( EVE_PK ) || pUser->GetPKValue() == 0 )
			pUser->RemoveIk3Buffs( IK3_ANGEL_BUFF );

#ifdef __ON_ERROR
		nOnError	= 702;
#endif	// __ON_ERROR

		pUser->AdjustMailboxState();

#ifdef __ON_ERROR
		nOnError	= 703;
#endif	// __ON_ERROR

#ifdef __LAYER_1015
		if( 
			GuildHouseMng->IsLoginAble( pUser, dwWorldId, nLayer ) &&
			pWorld && pWorld->m_linkMap.GetLinkMap( nLayer )							// 해당 레이어가 존재하고
			&& !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId )	// 인스턴스 던전이 아닌 경우...
#ifdef __QUIZ
			&& dwWorldId != WI_WORLD_QUIZ												// 퀴즈이벤트 지역이 아닌 경우
#endif // __QUIZ
			)
		{
			g_UserMng.AddPlayer( pUser, dwWorldId, nLayer );
		}
		else	// 레이어가 존재하지 않으면 무조건 마을로 이동한다.
		{
			pUser->m_vPos = D3DXVECTOR3( 6971.984f, 100.0f, 3336.884f );
			g_UserMng.AddPlayer( pUser, WI_WORLD_MADRIGAL, nDefaultLayer );
			pUser->Replace( WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984f, 99.8f, 3336.884f ), REPLACE_NORMAL, nDefaultLayer );
		}
#else	// __LAYER_1015
		g_UserMng.AddPlayer( pUser, dwWorldId );
#endif	// __LAYER_1015

		if( !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
		{
			// 무적, 투명, 초필, 반무적 
			if( pUser->m_dwMode & ( MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE ) )
			{
				Error( "Abnormal User!! - %s(%07d), m_dwMode:%08x", pUser->GetName(), pUser->m_idPlayer, pUser->m_dwMode );
				pUser->SetNotMode( MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE );
			}
		}

#ifdef __ON_ERROR
		nOnError	= 704;
#endif	// __ON_ERROR

		pUser->AddEventLuaDesc();

#ifdef __ON_ERROR
		nOnError	= 705;
#endif	// __ON_ERROR

		pUser->AdjustPartyQuest( dwWorldId );

#ifdef __ON_ERROR
		nOnError	= 706;
#endif	// __ON_ERROR

		pUser->AddTag( nTagCount, tags );	// 클라이언트에게 쪽지를 보낸다.

#ifdef __ON_ERROR
		nOnError	= 8;
#endif	// __ON_ERROR

		g_DPCoreClient.SendJoin( pUser->m_idPlayer, pUser->GetName(), ( pUser->m_dwAuthorization >= AUTH_GAMEMASTER ) );
		
		if( pUser->m_idGuild != 0 )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( !pGuild || !pGuild->IsMember( pUser->m_idPlayer ) )
				pUser->m_idGuild	= 0;
		}

#ifdef __ON_ERROR
		nOnError	= 9;
#endif	// __ON_ERROR

		CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
		if( pParty && pParty->IsMember( pUser->m_idPlayer ) )
		{
			const char* pszLeader	= CPlayerDataCenter::GetInstance()->GetPlayerString( pParty->GetPlayerId( 0 ) );
			pUser->AddPartyMember( pParty, 0, ( pszLeader? pszLeader: "" ), "" );
			if( pParty->m_idDuelParty )		// 듀얼중인 파티였다면.
			{
				pUser->m_nDuel = 2;			// 듀얼상태를 세팅
				pUser->m_idDuelParty = pParty->m_idDuelParty;
			}
		}
		else
		{
			pUser->m_idparty	= 0;
		}

#ifdef __ON_ERROR
		nOnError	= 10;
#endif	// __ON_ERROR

		pUser->AddSMModeAll();

		pUser->AddGameSetting();
		pUser->AddMonsterProp();
		
#ifdef __S_SERVER_UNIFY
		// 서버 통합 작업
		pUser->SetAllAction();
#endif // __S_SERVER_UNIFY

#ifdef __ON_ERROR
		nOnError	= 11;
#endif	// __ON_ERROR

		pUser->AddGCIsRequest( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, TRUE ) );
		pUser->AddGuildCombatState();
		if( g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam == CGuildCombat::MAINTENANCE_STATE )
			pUser->AddGuildCombatEnterTime( g_GuildCombatMng.m_dwTime - GetTickCount() );
		else
			pUser->AddGuildCombatNextTime( g_GuildCombatMng.m_dwTime - GetTickCount(), g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam );
		pUser->AddGCWinGuild();
		pUser->AddGCBestPlayer();
		//pUser->AddGCIsRequest( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, TRUE ) );
		if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
		{
			g_UserMng.AddGCGuildStatus( pUser->m_idGuild, pUser );
			g_UserMng.AddGCWarPlayerlist( pUser->m_idGuild, pUser );
		}
#ifdef __ON_ERROR
		nOnError	= 12;
#endif	// __ON_ERROR

#ifdef __INVALID_LOGIN_0320
		CalluspXXXMultiServer( g_uIdofMulti, pUser );
#else	// __INVALID_LOGIN_0320
		CalluspXXXMultiServer( g_uIdofMulti, pUser->m_idPlayer );
#endif	// __INVALID_LOGIN_0320

#ifdef __ON_ERROR
		nOnError	= 13;
#endif	// __ON_ERROR

		pWorld = pUser->GetWorld();
		g_GuildCombat1to1Mng.SendNowState( pUser );
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		{
			if( pWorld  && pWorld->GetID() >= WI_WORLD_GUILDWAR1TO1_0
						&& pWorld->GetID() <= WI_WORLD_GUILDWAR1TO1_L )
			{
				pUser->Replace( WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_NORMAL, nDefaultLayer );
			}
		}

// 		if( !pUser->GetWorld() )
// 			pUser->Replace( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ) );

	if( pUser->IsCollecting() )
		pUser->StopCollecting();

#ifdef __ON_ERROR
		nOnError	= 14;
#endif	// __ON_ERROR

		CSecretRoomMng::GetInstance()->GetAllInfo( pUser );
		if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( pUser ) )
			pUser->Replace( WI_WORLD_MADRIGAL, CContinent::GetInstance()->GetRevivalPos( CONT_FLARIS ), REPLACE_NORMAL, nDefaultLayer );

		if( pWorld->GetID() == WI_DUNGEON_SECRET_0 )
			pUser->Replace( WI_WORLD_MADRIGAL, CContinent::GetInstance()->GetRevivalPos( CONT_DARKON12 ), REPLACE_NORMAL, nDefaultLayer );

#ifdef __LAYER_1015
#endif	// __LAYER_1015

#ifdef __ON_ERROR
		nOnError	= 15;
#endif	// __ON_ERROR

		pUser->AddTaxInfo();
		CTax::GetInstance()->SendNoSetTaxRateOpenWnd( pUser );

#ifdef __ON_ERROR
		nOnError	= 16;
#endif	// __ON_ERROR

		{
			// 게임 진입 시 시스템 펫 각성 효과 적용
			CItemElem* pItem	= pUser->GetPetItem();
			if( pItem )
				pUser->SetDestParamRandomOptExtension( pItem );
		}


#ifdef __ON_ERROR
		nOnError	= 17;
#endif	// __ON_ERROR

		// 완주하지 않고 접속을 끊은 놈은 탈락이므로 완주한 놈만 정보를 받게된다. chipi_090317
		if( CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_PROGRESS )
		{
			CRainbowRace* pRainbowRace = CRainbowRaceMng::GetInstance()->GetRainbowRacerPtr( pUser->m_idPlayer );
			if( pRainbowRace )
				pUser->AddRainbowRaceState( pRainbowRace );
		}

		CHousingMng::GetInstance()->CreateRoomLayer( pUser->m_idPlayer );
		CHousingMng::GetInstance()->ReqLoadHousingInfo( pUser->m_idPlayer );

		CPCBang::GetInstance()->SetPCBangPlayer( pUser, dwPCBangClass );

#ifdef __VTN_TIMELIMIT
		//	mulcom	BEGIN100315	베트남 시간 제한
 		if( ::GetLanguage() == LANG_VTN )
		{
			SendTimeLimitInfoReq( pUser->m_idPlayer, pUser->m_playAccount.lpszAccount );
		}
		//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT

		if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId ) )
		{
			// 인던 안에서 접속 종료 후 재접속 시 플라리스로 가지 않고 인던 출구로 강제 이동
			CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		}
		pUser->AddGuildHouseAllInfo( GuildHouseMng->GetGuildHouse( pUser->m_idGuild ) );
		GuildHouseMng->SetApplyDST( pUser );
		if( pWorld && GuildHouseMng->IsGuildHouse( pWorld->GetID() ) && pUser->GetLayer() == pUser->m_idGuild )	// 접속지가 길드하우스 이면..
			pUser->SetIncRestPoint( tLogOut / REST_POINT_TICK * REST_POINT_LOGOUT_INC );	// 로그아웃 휴식 포인트를 증가 시켜준다.

		u_long idCampus = pUser->GetCampusId();
		if( idCampus )
		{
			CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( idCampus );
			if( pCampus && pCampus->IsMember( pUser->m_idPlayer ) )
				pUser->AddUpdateCampus( pCampus );
			else
				pUser->SetCampusId( 0 );
		}

#ifdef __ON_ERROR
		}
		catch( ... )
		{
			WriteLog( "OnJoin(): %s, %d", pUser->GetName(), nOnError );
		}
#endif	// __ON_ERROR
	}
}

void CDPDatabaseClient::OnGCResultValue( CAr & ar )
{
	u_long uSize;
	g_GuildCombatMng.m_GCResultValueGuild.clear();
	ar >> uSize;
	
	int	i = 0;
	
	for( i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCRESULTVALUEGUILD ResultValueGuild;
		ar >> ResultValueGuild.nCombatID;
		ar >> ResultValueGuild.uidGuild;
		ar >> ResultValueGuild.nReturnCombatFee;
		ar >> ResultValueGuild.nReward;
		g_GuildCombatMng.m_GCResultValueGuild.push_back( ResultValueGuild );
	}
	
	// 베스트 플레이어 관련 정보
	g_GuildCombatMng.m_GCResultValuePlayer.clear();
	ar >> uSize;
	for( i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuePlayer;
		ar >> ResultValuePlayer.nCombatID;
		ar >> ResultValuePlayer.uidGuild;
		ar >> ResultValuePlayer.uidPlayer;
		ar >> ResultValuePlayer.nReward;
		g_GuildCombatMng.m_GCResultValuePlayer.push_back( ResultValuePlayer );
	}
}
void CDPDatabaseClient::OnGCPlayerPoint( CAr & ar )
{
	g_GuildCombatMng.m_vecGCPlayerPoint.clear();
	// 길드대전 개인 포인트
	u_long uSize;
	ar >> uSize;
	for( int i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCPLAYERPOINT GCPlayerPoint;
		ar >> GCPlayerPoint;
		g_GuildCombatMng.m_vecGCPlayerPoint.push_back( GCPlayerPoint );
	}
}
void CDPDatabaseClient::OnAllGuildCombat( CAr & ar, DPID, DPID )
{
	g_GuildCombatMng.GuildCombatClear(1);
	
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;

	// 참가한 인원수
	u_long uSize; ar >> uSize;
	for (u_long i = 0; i < uSize; ++i) {
		const auto [idGuild, dwPenya, bRequest] = ar.Extract<u_long, DWORD, BOOL>();

		if (g_GuildMng.GetGuild(idGuild)) {
			g_GuildCombatMng.JoinGuildCombat(idGuild, dwPenya, bRequest);
		}
	}

	// 길드 보상 관련 정보
	OnGCResultValue( ar );

	// 길드대전 플레이어 포인트
	OnGCPlayerPoint( ar );
	
	g_GuildCombatMng.SetNpc();

}
// 참가 신청
void CDPDatabaseClient::OnInGuildCombat( CAr & ar, DPID, DPID )
{
	u_long idGuild;
	DWORD dwPenya, dwExistingPenya;
	ar >> idGuild;
	ar >> dwPenya;
	ar >> dwExistingPenya;
	
	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		g_GuildCombatMng.JoinGuildCombat( idGuild, dwPenya, TRUE );
		CUser* pMaster = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
		if( IsValidObj( pMaster ) )
		{
			CString str;

			// 전체 페냐가 0일경우
			if( dwExistingPenya == 0 )
			{
				str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), 0, dwPenya );
			}
			else
			{
				// 전체 금액이 있으면 추가되는 페냐를 빼서 계산..
				str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), dwExistingPenya, dwPenya-dwExistingPenya  );
			}

			pMaster->AddDiagText( str );
			g_UserMng.AddGCIsRequest( idGuild, g_GuildCombatMng.IsRequestWarGuild( idGuild, TRUE ) );
		}

		CString strMsg;
		// 길드대전 신청이 완료되었습니다.
		strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_GUILDCOMBAT ) );	// "수정해야함 : 길드대전에 신청하였습니다. 매주 토요일 17시에 길드대전이 시작됩니다 로 수정할까?? ㅎㅎ " );
		g_UserMng.AddGuildMsg( pGuild, strMsg );
		strMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP) );
		g_UserMng.AddGuildMsg( pGuild, strMsg );
	}
}

void CDPDatabaseClient::OnOutGuildCombat( CAr & ar, DPID, DPID )
{
	u_long idGuild;
	ar >> idGuild;

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		BOOL bJoin = TRUE;
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember( idGuild );
		if( pGCMember == NULL )
		{
			bJoin = FALSE;
		}
		else
		{
			if( pGCMember->bRequest == FALSE )
				bJoin = FALSE;
		}
		if( bJoin )
		{
			g_GuildCombatMng.OutGuildCombat( idGuild );
			CUser* pMaster = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pMaster ) )
			{
				CString strMsg;
				strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_EXPENSE_RETURN ) );	// "수정해야함 : 대전 참가비는 대전이 끝나고 반환됩니다" );
				pMaster->AddText( strMsg );
			}
			
			CString strMsg;
			//신청하신 길드대전을 취소했습니다.
			strMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL) );
			g_UserMng.AddGuildMsg( pGuild, strMsg );
			g_UserMng.AddGCIsRequest( idGuild, g_GuildCombatMng.IsRequestWarGuild( idGuild, TRUE ) );
		}
	}
}

void CDPDatabaseClient::OnResultGuildCombat( CAr & ar, DPID, DPID )
{
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;

	OnGCResultValue( ar );
	g_GuildCombatMng.GuildCombatGameClear();

	g_UserMng.AddGCWinGuild();
	g_UserMng.AddGCBestPlayer();
}
void CDPDatabaseClient::OnPlayerPointGuildCombat( CAr & ar, DPID, DPID )
{
	OnGCPlayerPoint( ar );
}
void CDPDatabaseClient::OnGetPenyaGuildGC( CAr & ar, DPID, DPID )
{
	u_long uidPlayer, uidGuild;
	int nGuildCombatId;
	ar >> uidPlayer;
	ar >> nGuildCombatId;
	ar >> uidGuild;
	CMover* pMover = prj.GetUserByID( uidPlayer );
	if( IsValidObj( pMover ) )
	{
		if( uidGuild == pMover->m_idGuild )
		{
			int nGetResult = 0;
			__int64 nGetPenya = 0;

			BOOL bFind = FALSE;
			int nFindVeci = 0;
			CGuildCombat::__GCRESULTVALUEGUILD ResultValuetmp;
			for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValueGuild.size() ) ; ++veci )
			{
				ResultValuetmp = g_GuildCombatMng.m_GCResultValueGuild[veci];
				if( nGuildCombatId == ResultValuetmp.nCombatID && uidGuild == ResultValuetmp.uidGuild )
				{
					bFind = TRUE;
					nFindVeci = veci;
					if( ResultValuetmp.nReturnCombatFee )
						nGetResult = 1;
					else
						nGetResult = 0;
					nGetPenya = ResultValuetmp.nReturnCombatFee + ResultValuetmp.nReward;
					break;
				}
			}

			if( bFind && nGetResult )
			{				
				__int64 nTotal = (__int64)pMover->GetGold() + nGetPenya;
				if( nGetPenya > INT_MAX || nTotal > INT_MAX )
				{
					((CUser*)pMover)->AddGCGetPenyaGuild( 3, nGetPenya );
					bFind = FALSE;
				}

				pMover->AddGold( (int)( nGetPenya ) );
				((CUser*)pMover)->AddGCGetPenyaGuild( nGetResult, nGetPenya );

				LogItemInfo aLogItem;
				aLogItem.Action		= "9";
				aLogItem.SendName	= pMover->GetName();
				aLogItem.RecvName	= "GUILDCOMBAT_VG";
				aLogItem.WorldId	= pMover->GetWorld()->GetID();
				aLogItem.Gold		= (DWORD)( pMover->GetGold() - nGetPenya );
				aLogItem.Gold2		= pMover->GetGold();
				//aLogItem.ItemName	= "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber	= (DWORD)( nGetPenya );
				g_DPSrvr.OnLogItem( aLogItem );

				g_GuildCombatMng.m_GCResultValueGuild.erase( g_GuildCombatMng.m_GCResultValueGuild.begin() + nFindVeci );		
			}
		}
	}
}
void CDPDatabaseClient::OnGetPenyaPlayerGC( CAr & ar, DPID, DPID )
{
	u_long uidPlayer, uidGuild;
	int nGuildCombatId;
	ar >> uidPlayer;
	ar >> nGuildCombatId;
	ar >> uidGuild;
	CMover* pMover = prj.GetUserByID( uidPlayer );
	if( IsValidObj( pMover ) )
	{
		int nGetResult = 0;
		__int64 nGetPenya = 0;
		
		BOOL bFind = FALSE;
		int nFindVeci = 0;
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuetmp;
		for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValuePlayer.size() ) ; ++veci )
		{
			ResultValuetmp = g_GuildCombatMng.m_GCResultValuePlayer[veci];
			if( nGuildCombatId == ResultValuetmp.nCombatID && uidPlayer == ResultValuetmp.uidPlayer && uidGuild == ResultValuetmp.uidGuild )
			{
				bFind = TRUE;
				nFindVeci = veci;
				nGetPenya = ResultValuetmp.nReward;
				break;
			}
		}
		
		if( bFind )
		{				
			__int64 nTotal = (__int64)pMover->GetGold() + nGetPenya;
			if( nGetPenya > INT_MAX || nTotal > INT_MAX )
			{
				((CUser*)pMover)->AddGCGetPenyaPlayer( 3, nGetPenya );
				bFind = FALSE;
			}
			
			if( bFind )
			{
				pMover->AddGold( (int)( nGetPenya ) );
				((CUser*)pMover)->AddGCGetPenyaPlayer( nGetResult, nGetPenya );

				LogItemInfo aLogItem;
				aLogItem.Action		= "9";
				aLogItem.SendName	= pMover->GetName();
				aLogItem.RecvName	= "GUILDCOMBAT_VP";
				aLogItem.WorldId	= pMover->GetWorld()->GetID();
				aLogItem.Gold		= (DWORD)( pMover->GetGold() - nGetPenya );
				aLogItem.Gold2		= pMover->GetGold();
				//aLogItem.ItemName	= "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber	= (DWORD)( nGetPenya );
				g_DPSrvr.OnLogItem( aLogItem );
			}				

			g_GuildCombatMng.m_GCResultValuePlayer.erase( g_GuildCombatMng.m_GCResultValuePlayer.begin() + nFindVeci );		
		}
	}
}

void CDPDatabaseClient::OnContinueGC( CAr & ar, DPID, DPID )
{
	g_GuildCombatMng.m_uWinGuildId = 0;
	g_GuildCombatMng.m_nWinGuildCount = 0;
	
	g_UserMng.AddGCWinGuild();
}

void CDPDatabaseClient::OnAllPlayerData( CAr & ar , DPID, DPID )
{
	CPlayerDataCenter::GetInstance()->Serialize( ar );
}

void CDPDatabaseClient::OnAddPlayerData( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	PlayerData pd;
	ar >> idPlayer;
	ar.ReadString( pd.szPlayer, MAX_PLAYER );
	ar.Read( &pd.data, sizeof(sPlayerData) );
	CPlayerDataCenter::GetInstance()->AddPlayerData( idPlayer, pd );
}

void CDPDatabaseClient::OnDeletePlayerData( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	ar >> idPlayer;
	CPlayerDataCenter::GetInstance()->DeletePlayerData( idPlayer );
}

void CDPDatabaseClient::SendUpdatePlayerData( CUser* pUser )
{
	sPlayerData data;
	data.nJob	= pUser->GetJob();
	data.nLevel	= pUser->GetLevel();
	data.nSex	= pUser->GetSex();

	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << pUser->m_idPlayer;
	ar.Write( &data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_SERVERPLAYER );	
}

void CDPDatabaseClient::OnUpdatePlayerData( CAr & ar , DPID, DPID )
{
	u_long idPlayer;
	sPlayerData data;
	ar >> idPlayer;
	ar.Read( &data, sizeof(sPlayerData) );
	
	PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if( pPlayerData )
	{
		memcpy( &pPlayerData->data, &data, sizeof(sPlayerData) );
		CCoupleHelper::Instance()->OnUpdatePlayerData( idPlayer, pPlayerData );
		CCampusHelper::GetInstance()->OnUpdatePlayerData( idPlayer, pPlayerData );
	}
}

void CDPDatabaseClient::OnBaseGameSetting( CAr & ar, DPID, DPID )
{
	BOOL bBaseGameSetting = FALSE;
	BOOL bFirst = FALSE;
	ar >> bFirst;
	ar >> bBaseGameSetting;
	if( bBaseGameSetting )
	{
		ar >> prj.m_fMonsterExpRate	>> prj.m_fGoldDropRate >> prj.m_fItemDropRate >> prj.m_fMonsterHitRate;
		ar >> prj.m_fMonsterRebirthRate >> prj.m_fMonsterHitpointRate >> prj.m_fMonsterAggressiveRate >> prj.m_fMonsterRespawnRate;
	}

	g_UserMng.AddGameRate( prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP );
	g_UserMng.AddGameRate( prj.m_fGoldDropRate, GAME_RATE_GOLDDROP );
	g_UserMng.AddGameRate( prj.m_fItemDropRate, GAME_RATE_ITEMDROP );	
	g_UserMng.AddGameRate( prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT );
	g_UserMng.AddGameRate( prj.m_fMonsterRebirthRate, GAME_RATE_REBIRTH );
	g_UserMng.AddGameRate( prj.m_fMonsterHitpointRate, GAME_RATE_HITPOINT );
	g_UserMng.AddGameRate( prj.m_fMonsterAggressiveRate, GAME_RATE_AGGRESSIVE );
	g_UserMng.AddGameRate( prj.m_fMonsterRespawnRate, GAME_RATE_RESPAWN );	

	if( bFirst )
	{
		for (auto & pWorld : g_WorldMng.m_worlds) {
			pWorld->LoadRegion();
#ifdef __EVENT_0117
			CRespawner* pRespawner	= pWorld->m_respawner.Proto();

			for (CRespawnInfo & rRespawnInfo : pRespawner->m_vRespawnInfoRegion) {

				CRespawnInfo* pRespawnInfo	= &rRespawnInfo;
		
				if( pRespawnInfo->m_dwType == OT_MOVER )	// 몬스터 리스폰
				{
					const MoverProp* pMoverProp	= prj.GetMoverProp( pRespawnInfo->m_dwIndex );
					if( pMoverProp && pMoverProp->dwFlying == 0 && pMoverProp->dwLevel > 0 && pMoverProp->dwLevel <= MAX_MONSTER_LEVEL )
					{
						CEventGeneric::GetInstance()->AddRegionGeneric( pMoverProp->dwLevel, pWorld->GetID(), pRespawnInfo );
#ifdef __EVENTLUA_SPAWN
						if( pWorld->GetID() == WI_WORLD_MADRIGAL )	// 현재까지는 마드리갈 대륙에만 출현한다.
							prj.m_EventLua.SetAddSpawnRegion( pWorld->GetID(), pRespawnInfo->m_rect );		
#endif // __EVENTLUA_SPAWN
					}	// if
				}	// if
			}	// for
#endif	// __EVENT_0117
		}	// while
#ifdef __EVENT_0117
		CEventGeneric::GetInstance()->SortRegionGeneric();
#endif	// __EVENT_0117
	}	// if
}

void CDPDatabaseClient::OnMonsterRespawnSetting( CAr & ar, DPID, DPID )
{
	CWorld * pWorld = g_WorldMng.GetWorld(WI_WORLD_MADRIGAL);
	if (!pWorld) return;

	char szMonsterName[32];
	D3DXVECTOR3	vPos;
	int		nAddIndex;
	int		nQuantity;
	int		nAggressive;
	int		nRange;
	int		nTime;

	int nMonsterRespawnSize;
	ar >> nMonsterRespawnSize;
	for( int i = 0 ; i < nMonsterRespawnSize ; ++i )
	{
		ar >> nAddIndex;
		ar.ReadString( szMonsterName, 32 );
		ar >> vPos;
		ar >> nQuantity;
		ar >> nAggressive;
		ar >> nRange;
		ar >> nTime;

		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		
		if( pMoverProp )
		{			
			CRespawnInfo ri;
			ri.m_dwType = OT_MOVER;
			ri.m_dwIndex = pMoverProp->dwID;
			ri.m_nMaxcb = nQuantity;
			ri.m_nMaxAttackNum = nAggressive;
			if( pMoverProp->dwFlying != 0 )
				ri.m_vPos = vPos;
			ri.m_rect.left		= (LONG)( vPos.x - nRange );
			ri.m_rect.right		= (LONG)( vPos.x + nRange );
			ri.m_rect.top		= (LONG)( vPos.z - nRange );
			ri.m_rect.bottom	= (LONG)( vPos.z + nRange );
			ri.m_uTime = nTime;
			ri.m_nGMIndex = nAddIndex;
			
			pWorld->m_respawner.AddScriptSpawn(ri);
		}
	} 

	int nRemoveSize;
	int nRemoveIndex;
	ar >> nRemoveSize;
	for( int i = 0 ; i < nRemoveSize ; ++ i )
	{
		ar >> nRemoveIndex;
		pWorld->m_respawner.RemoveScriptSpawn( nRemoveIndex );
	}
}

void CDPDatabaseClient::OnMonsterPropSetting( CAr & ar, DPID, DPID )
{
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;

	int		nMonsterPropSize;

	prj.m_nAddMonsterPropSize = 0;
	prj.m_nRemoveMonsterPropSize = 0;

	ar >> nMonsterPropSize;
	for( int i = 0 ; i < nMonsterPropSize ; ++i )
	{
		ar.ReadString( szMonsterName, 32 );
		ar >> nHitPoint;
		ar >> nAttackPower;
		ar >> nDefence;
		ar >> nExp;
		ar >> nItemDrop;
		ar >> nPenya;
		
		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		if( pMoverProp )
		{
			pMoverProp->m_fHitPoint_Rate = (float)nHitPoint / 100.0f;
			pMoverProp->m_fAttackPower_Rate = (float)nAttackPower / 100.0f;
			pMoverProp->m_fDefence_Rate = (float)nDefence / 100.0f;
			pMoverProp->m_fExp_Rate = (float)nExp / 100.0f;
			pMoverProp->m_fItemDrop_Rate = (float)nItemDrop / 100.0f;
			pMoverProp->m_fPenya_Rate = (float)nPenya / 100.0f;
			pMoverProp->m_bRate = TRUE;

			MONSTER_PROP MonsterProp;
			strcpy( MonsterProp.szMonsterName, szMonsterName );
			MonsterProp.nHitPoint = nHitPoint;
			MonsterProp.nAttackPower = nAttackPower;
			MonsterProp.nDefence = nDefence;
			MonsterProp.nExp = nExp;
			MonsterProp.nItemDrop = nItemDrop;
			MonsterProp.nPenya = nPenya;
			prj.AddMonsterProp( MonsterProp );
		}
	} 

	int	nRemovePropSize;
	ar >> nRemovePropSize;
	for( int i = 0 ; i < nRemovePropSize ; ++i )
	{
		ar.ReadString( szMonsterName, 32 );
		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		if( pMoverProp )
		{
			pMoverProp->m_fHitPoint_Rate = 1.0f;
			pMoverProp->m_fAttackPower_Rate = 1.0f;
			pMoverProp->m_fDefence_Rate = 1.0f;
			pMoverProp->m_fExp_Rate = 1.0f;
			pMoverProp->m_fItemDrop_Rate = 1.0f;
			pMoverProp->m_fPenya_Rate = 1.0f;
			pMoverProp->m_bRate = FALSE;
			prj.RemoveMonsterProp( szMonsterName );
		}
	}
	
	g_UserMng.AddMonsterProp();
}

void CDPDatabaseClient::OnGMChat( CAr & ar, DPID, DPID )
{
	int nSize;
	ar >> nSize;
	for( int i = 0 ; i < nSize ; ++i )
	{
		ar.ReadString( prj.m_chGMChat[i], 256 );
	}
	if( 0 < nSize )
		g_UserMng.AddGMChat( nSize );
}

void CDPDatabaseClient::SendLogPlayDeath(CMover* pMover, CMover* pSender)
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PLAY_DEATH, DPID_UNKNOWN, DPID_UNKNOWN );

	ar.WriteString( pMover->GetName() );
	ar << pMover->GetWorld()->GetID();
	ar << pMover->GetPos();
	ar << pMover->GetLevel();
	ar << (int)0;	// MaxHitPower
	ar << pMover->GetMaxHitPoint();
	if( pSender )
		ar.WriteString( pSender->GetName() );
	else
		ar.WriteString( "" );
	ar << pMover->m_idPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogSkillPoint( int nAction, int nPoint, CMover* pMover, const SKILL * pSkill )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SKILLPOINTLOG, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << nAction;
	ar << pMover->m_idPlayer;
	ar << pMover->m_nSkillLevel;
	ar << pMover->m_nSkillPoint;
	if (pSkill) {
		ar << pSkill->dwSkill;
		ar << pSkill->dwLevel;
	} else {
		ar << static_cast<DWORD>(0);
		ar << static_cast<DWORD>(0);
	}
	ar << nPoint;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogExpBox(u_long idPlayer, OBJID objid, EXPINTEGER iExp, BOOL bGet) {
	SendPacket<
		PACKETTYPE_LOG_EXPBOX, u_long, OBJID, EXPINTEGER, BOOL
	>(
		idPlayer, objid, iExp, bGet
	);
}

void CDPDatabaseClient::SendLogLevelUp( CMover* pSender, int Action )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_LEVELUP, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << pSender->GetExp1();
	ar << pSender->GetFlightLv();
	ar << pSender->GetLevel();
	ar << pSender->GetJob();
	ar << (DWORD)1;				// joblevel
	ar << pSender->m_nStr;
	ar << pSender->m_nDex;
	ar << pSender->m_nInt;
	ar << pSender->m_nSta;
	ar << pSender->m_nRemainGP;
	ar << (LONG)0;					// LP사용하지 않는다.
	ar << pSender->GetGold();
	ar << ( (CUser*)pSender )->GetPlayerTime();
	ar << Action;
	ar << pSender->m_idPlayer;
	
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogGamemaChat( CUser* pUser, LPCTSTR lpszString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_GAMEMASTER_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	
	ar << pUser->m_idPlayer;
	ar.WriteString( lpszString );
	
	SEND( ar, this, DPID_SERVERPLAYER );

}

void CDPDatabaseClient::SendLogItem( CUser* pUser, CMover* pSender )
{
}

void CDPDatabaseClient::SendLogSMItemUse( const char *Action, CUser* pUser, CItemElem* pItemElem, ItemProp* pItemProp, const char* szName1 )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( Action );
	ar.WriteString( pUser->GetName() );
	if( strlen( szName1 ) > 0 )
		ar.WriteString( szName1 );
	else 
		ar.WriteString( pUser->GetName() );
	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. // Begin
	// ar << pUser->GetWorld()->GetID();
	DWORD dwWorldId = pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	ar << dwWorldId;
	// chipi_090623 // End
	ar << pUser->GetGold();
	ar << pUser->GetGold();
	

	if( pItemElem == NULL )
	{
		ar << (SERIALNUMBER)0;
		ar << (int)0;
		ar << (int)0;
		//ar.WriteString( pItemProp->szName );
		char szItemId[32] = {0, };
		_stprintf( szItemId, "%d", pItemProp->dwID );
		ar.WriteString( szItemId );
		ar << (DWORD)1;
		ar << (DWORD)0;
		ar << pUser->m_dwGoldBank[pUser->m_nSlot];
		ar << (int)0;
		ar << (int)0;
		ar << (int)0;
		ar << (int)0;
		ar << (DWORD)0;
		ar << (int)0;
		ar << (int)0;
		ar << (int)0;
	}
	else
	{
		ar << pItemElem->GetSerialNumber();
		ar << pItemElem->m_nHitPoint;
		ar << pItemElem->m_nRepair;
		//ar.WriteString( (char*)pItemElem->GetProp()->szName );
		char szItemId[32] = {0, };
		_stprintf( szItemId, "%d", pItemElem->GetProp()->dwID );
		ar.WriteString( szItemId );
		ar << (DWORD)pItemElem->m_nItemNum;
		ar << pItemElem->GetAbilityOption();
		ar << pUser->m_dwGoldBank[pUser->m_nSlot];
		ar << (int)0;
		ar << (int)0;
		ar << (int)pItemElem->m_bItemResist;
		ar << (int)pItemElem->m_nResistAbilityOption;
		ar << pItemElem->m_dwKeepTime;
		ar << pItemElem->GetPiercingSize();
		for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
			ar << pItemElem->GetPiercingItem( i );
		ar << pItemElem->GetUltimatePiercingSize();
		for( int i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
			ar << pItemElem->GetUltimatePiercingItem( i );
		ar << pItemElem->GetRandomOptItemId();
		// mirchang_100514 TransformVisPet_Log
		if( pItemElem->IsTransformVisPet() == TRUE )
		{
			ar << (BYTE)100;
		}
		// mirchang_100514 TransformVisPet_Log
	}
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendChangeBankPass( const char* szName, const char *szNewPass, u_long uidPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANGEBANKPASS, DPID_UNKNOWN, DPID_UNKNOWN );
	
	ar.WriteString( szName );
	ar.WriteString( szNewPass );
	ar << uidPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPDatabaseClient::SendLogGetHonorTime(CMover * pMover, int nGetHonor) {
	SendPacket<PACKETTYPE_LOG_GETHONORTIME, int, u_long>(nGetHonor, pMover->m_idPlayer);
}

void CDPDatabaseClient::SendLogUniqueItem(CMover* pMover, CItem* pItem, int nOption )
{
	SendLogUniqueItem2( pMover, pItem->GetProp(), nOption );
}

void CDPDatabaseClient::SendLogUniqueItem2( CMover* pMover, ItemProp* pItemProp, int nOption )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_UNIQUEITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( pMover->GetName() );
	ar << pMover->GetWorld()->GetID();
	ar << pMover->GetPos();
//	if( rItemElem.m_szItemText[0] == '\0' )
		//ar.WriteString( pItemProp->szName );
	char szItemId[32] = {0, };
	::memset( szItemId, 0, sizeof( szItemId ) );
	_stprintf( szItemId, "%d", pItemProp->dwID );
	ar.WriteString( szItemId );
//	else
//		ar.WriteString( rItemElem.m_szItemText );
	ar << nOption;
	ar << pMover->m_idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSchoolReport( PSCHOOL_ENTRY pSchool )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SCHOOL_REPORT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( pSchool, sizeof(SCHOOL_ENTRY) * MAX_SCHOOL );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber )
{
	BEFORESENDDUAL( ar, PACKETTYPE_BUYING_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( (void*)pbi2, sizeof(BUYING_INFO2) );
	ar << iSerialNumber;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogPkPvp( CMover* pUser, CMover* pLose, int nPoint, char chState )
{
	if( !g_eLocal.GetState( EVE_PK ) )
		return;

	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PK_PVP, DPID_UNKNOWN, DPID_UNKNOWN );

	int nSendBuf = 0;

	if( pUser == NULL || pLose == NULL )
	{
		if( pUser == NULL )
			nSendBuf = 1;
		else
			nSendBuf = 2;
	}
	else
	{
		nSendBuf = 3;
	}
	
	ar << nSendBuf;
	ar << nPoint;
	ar << chState;

	if( nSendBuf == 1 || nSendBuf == 3 )
	{
		ar << pLose->m_idPlayer;
		ar << pLose->GetWorld()->GetID();
		ar << pLose->GetLevel();
		ar << pLose->GetPos();
		ar << pLose->m_nFame;
		ar << pLose->m_nPKValue;
		ar << pLose->m_dwPKPropensity;
	}

	if( nSendBuf == 2 || nSendBuf == 3 )
	{
		ar << pUser->m_idPlayer;
		ar << pUser->GetWorld()->GetID();
		ar << pUser->GetLevel();
		ar << pUser->GetPos();
		ar << pUser->m_nFame;
		ar << pUser->m_nPKValue;
		ar << pUser->m_dwPKPropensity;
	}

	SEND( ar, this, DPID_SERVERPLAYER );		
}

void CDPDatabaseClient::SendLogSchool( u_long idPlayer, LPCTSTR szName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_SCHOOL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szName );
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPDatabaseClient::SendGuildGetPay(u_long idGuild, DWORD nGoldGuild, DWORD dwPay) {
	SendPacket<PACKETTYPE_GUILD_DB_REALPENYA, u_long, DWORD, DWORD>(idGuild, nGoldGuild, dwPay);
}

void CDPDatabaseClient::SendGuildContribution( CONTRIBUTION_CHANGED_INFO & info, BYTE nLevelUp, LONG nMemberLevel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WD_GUILD_CONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << info; 
	ar << nLevelUp;	// 레벨업 1 : 0( 했으면 1 )
	ar << nMemberLevel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGuildBank() {
	SendPacket<PACKETTYPE_GUILD_BANK_QUERY>();
}

void CDPDatabaseClient::UpdateGuildRanking() {
	// 월드 서버에서 요청
	SendPacket<PACKETTYPE_UPDATE_GUILD_RANKING, int>(1);
}

void CDPDatabaseClient::UpdateGuildRankingUpdate() {
	SendPacket<PACKETTYPE_UPDATE_GUILD_RANKING_DB>();
}

void CDPDatabaseClient::OnGuildBank( CAr & ar, DPID, DPID )
{
	int nCount;

	// 1. 몇개의 길드 뱅크 정보가 들어왔는지 Serialize한다.
	ar >> nCount;

	// 2. 루프를 돌며 길드창고 정보를 Serialize한다.
	for (int i=0; i<nCount; ++i)
	{
		int nGuildId = 0, nGoldGuild = 0;

		// 2-1 길드 핸들값을 받는다.
		ar >> nGuildId;
		ar >> nGoldGuild;

		// 2-2 핸들값으로 길드매니져에서 길드 정보를 포인트하여 창고정보를 Serialize한다.
		CGuild* pGuild = g_GuildMng.GetGuild( nGuildId );
		if( pGuild )
		{
			pGuild->m_nGoldGuild = nGoldGuild;
			ar >> pGuild->m_GuildBank;
		}
		else
		{
			// 임시로 템프변수 만들어서 받기 banktmp
			CGuild tempGuild;
			ar >> tempGuild.m_GuildBank;
		}
	}
}

void CDPDatabaseClient::OnUpdateGuildRankingFinish(CAr & ar, DPID, DPID) {
	ar >> CGuildRank::Instance;

	// TODO: send the GuildRank to all users
}


void CDPDatabaseClient::SendQueryGuildQuest() {
	SendPacket<PACKETTYPE_QUERYGUILDQUEST>();
}

void CDPDatabaseClient::SendInsertGuildQuest(u_long idGuild, int nId) {
	SendPacket<PACKETTYPE_INSERTGUILDQUEST, u_long, int>(idGuild, nId);
}

void CDPDatabaseClient::SendUpdateGuildQuest(u_long idGuild, int nId, int nState) {
	SendPacket<PACKETTYPE_UPDATEGUILDQUEST, u_long, int, int>(idGuild, nId, nState);
}

void CDPDatabaseClient::OnQueryGuildQuest( CAr & ar, DPID, DPID ) {
	size_t nCount; ar >> nCount;

	for( size_t i = 0; i < nCount; i++ )
	{
		u_long idGuild; ar >> idGuild;

		boost::container::flat_map<int, int> quests;
		size_t s; ar >> s;
		for (size_t i = 0; i != s; ++i) {
			const auto [a, b] = ar.Extract<int, int>();
			quests.emplace(a, b);
		}

		if (CGuild * pGuild = g_GuildMng.GetGuild(idGuild)) {
			pGuild->m_quests = std::move(quests);
		}
	}
}

void CDPDatabaseClient::SendQuerySetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << dwData;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendPing( void )
{
	if( FALSE == m_bAlive )
	{
#ifndef _DEBUG
		Error( "TRANS : is not alive" );
		if( m_cbPing <= 1 )
		{
			ExitProcess( -1 );
			return;
		}
#endif	// _DEBUG
	}

	m_cbPing++;
	m_bAlive	= FALSE;
	SendPacket<PACKETTYPE_PING, time_t>(time_null());
}

void CDPDatabaseClient::OnPing( CAr & ar, DPID, DPID )
{
	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

	time_t d	= time_null() - tSend;
	CTime tmTrans( tTrans );
#if !defined(_DEBUG)
	if( d >  10 )
		Error( "ping TRANS - %d, trans:%s", d, tmTrans.Format( "%Y/%m/%d %H:%M:%S" ) );
#endif 
	
	m_bAlive	= TRUE;
}

void CDPDatabaseClient::SendQueryMailBox(u_long idReceiver) {
	// Load the mailbox through guild's completion queue, suppose
	// that the box already exists in WorldServer
	SendPacket<PACKETTYPE_QUERYMAILBOX, u_long>(idReceiver);
}

void CDPDatabaseClient::SendQueryMailBoxReq(u_long idReceiver) {
	// Load the mail box and send it through Req (does not exist in WorldServer)
	SendPacket<PACKETTYPE_QUERYMAILBOX_REQ, u_long>(idReceiver);
}

void CDPDatabaseClient::SendQueryMailBoxCount(u_long idReceiver) {
	// Load the mail box and send it through normal (already exists in WorldServer)
	SendPacket<PACKETTYPE_QUERYMAILBOX_COUNT, u_long>(idReceiver);
}

void CDPDatabaseClient::OnMailBox( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	ar >> idReceiver;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	if (!pMailBox) {
		Error("GetMailBox - pMailBox is NULL. idReceiver : %d", idReceiver);
		return;
	}

	pMailBox->ReadMailContent( ar );
	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		pUser->AddMailBox( pMailBox );
		pUser->mailBoxRequest.ResetCheckClientReq();
	}
}

void CDPDatabaseClient::OnMailBoxReq( CAr & ar, DPID, DPID ) {
	const auto [idReceiver, bHaveMailBox] = ar.Extract<u_long, bool>();

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		pUser->mailBoxRequest.CheckTransMailBox();
		pUser->SendCheckMailBoxReq( bHaveMailBox );
	}
	
	if (!bHaveMailBox) {
		if (IsValidObj(pUser)) {
			pUser->mailBoxRequest.ResetCheckClientReq();
		}

		return;
	}
	
	if (CPost::GetInstance()->GetMailBox(idReceiver)) {
		Error("CDPDatabaseClient::OnMailBoxReq - pMailBox is NOT NULL. idReceiver : %d", idReceiver);
		return;
	}
	
	CMailBox* pNewMailBox = new CMailBox( idReceiver );

	const bool added = CPost::GetInstance()->AddMailBox(pNewMailBox);

	if (!added) {
		Error("CDPDatabaseClient::OnMailBoxReq - AddMailBox Failed. idReceiver : %d", idReceiver);
		return;
	}

	pNewMailBox->ReadMailContent( ar );	// pMailBox->m_nStatus	= CMailBox::data;

	if( IsValidObj( pUser ) )
	{
		pUser->AddMailBox( pNewMailBox );
		pUser->mailBoxRequest.ResetCheckClientReq();
	}
}

void CDPDatabaseClient::SendQueryPostMail( u_long idReceiver, u_long idSender, CItemElem& itemElem, int nGold, const char* lpszTitle, const char* lpszText )
{
// 	//	BEGINTEST
// 	Error( "SendQueryPostMail  Receiver[%d] Sender[%d]", idReceiver, idSender );

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << idSender;
	if( FALSE == itemElem.IsEmpty() )
	{
		ar << (BYTE)1;
		ar << itemElem;
	}
	else
	{
		ar << (BYTE)0;
	}
	ar << nGold;
	ar.WriteString( lpszTitle );
	ar.WriteString( lpszText );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryRemoveMail(u_long idReceiver, u_long nMail) {
	SendPacket<PACKETTYPE_QUERYREMOVEMAIL, u_long, u_long>(idReceiver, nMail);
}

void CDPDatabaseClient::SendQueryGetMailItem(u_long idReceiver, u_long nMail) {
	SendPacket<PACKETTYPE_QUERYGETMAILITEM, u_long, u_long, u_long>(idReceiver, nMail, g_uIdofMulti);
}

void CDPDatabaseClient::SendQueryGetMailGold(u_long idReceiver, u_long nMail) {
	SendPacket<PACKETTYPE_QUERYGETMAILGOLD, u_long, u_long, u_long>(idReceiver, nMail, g_uIdofMulti);
}

void CDPDatabaseClient::SendQueryReadMail(u_long idReceiver, u_long nMail) {
	SendPacket<PACKETTYPE_READMAIL, u_long, u_long>(idReceiver, nMail);
}

void CDPDatabaseClient::OnPostMail( CAr & ar, DPID, DPID )
{
	const auto [bResult, idReceiver] = ar.Extract<BOOL, u_long>();
	CMail * pMail = new CMail; ar >> *pMail;

	if( bResult )
	{
		if( CPost::GetInstance()->AddMail( idReceiver, pMail ) <= 0 )
		{
			Error( "OnPostMail - pMail->m_nMail : %d", pMail->m_nMail );
		}

		CUser * pReceiver = prj.GetUserByID(idReceiver);
		if (IsValidObj(pReceiver)) {
			if (pReceiver->IsPosting()) {
				pReceiver->SendSnapshotNoTarget<SNAPSHOTTYPE_POSTMAIL, CMail>(*pMail);
			}

			pReceiver->SetMode(MODE_MAILBOX);
			g_UserMng.AddModifyMode(pReceiver);
		}

		CUser * pSender = prj.GetUserByID(pMail->m_idSender);
		if (IsValidObj(pSender)) {
			pSender->AddDiagText(prj.GetText(TID_MAIL_SEND_OK));
		}
	}
	else
	{
		Error( "OnPostMail - Send Mail Failed. idSender : %d, idReceiver : %d", pMail->m_idSender, idReceiver );
		CUser* pUser	= prj.GetUserByID( pMail->m_idSender );
		if( IsValidObj( pUser ) )
		{
			if( pMail->m_pItemElem )
				pUser->CreateItem( pMail->m_pItemElem );
			
			if( ::GetLanguage() == LANG_KOR )
				pUser->AddGold( 100 + pMail->m_nGold, TRUE );
			else
				pUser->AddGold( 500 + pMail->m_nGold, TRUE );
			
			pUser->AddDiagText( prj.GetText(TID_GAME_POST_ERROR) );
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
		}

		SAFE_DELETE( pMail );
	}
}

void CDPDatabaseClient::OnRemoveMail( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );

	if( pMailBox )
	{
		pMailBox->RemoveMail( nMail );

		CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			pUser->AddRemoveMail( nMail, CMail::mail );
			if( !pMailBox->IsStampedMailExists() && pUser->IsMode( MODE_MAILBOX ) )
			{
				pUser->SetNotMode( MODE_MAILBOX );
				g_UserMng.AddModifyMode( pUser );
			}
		}
	}
}

void CDPDatabaseClient::OnGetMailItem( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	CItemElem	itemRestore;

	if( pMailBox )
	{
		CMail* pMail	= pMailBox->GetMail( nMail );
		if( pMail && pMail->m_pItemElem )
		{
			itemRestore		= *( pMail->m_pItemElem );
			CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
			if( IsValidObj( pUser ) )
			{
				if( g_uIdofMulti== uQuery )		// 편지 받기 요청 주체면 아이템 생성, 우편 아이템 제거
				{
					if( pUser->CreateItem( pMail->m_pItemElem ) )
					{
						pUser->AddRemoveMail( nMail, CMail::item );
						itemRestore.Empty();
					}
				}
				else	// 우편 아이템 받기 요청 주체가 아니면, 우편 아이템 제거
				{
					pUser->AddRemoveMail( nMail, CMail::item );
					itemRestore.Empty();
				}
			}
		}
		pMailBox->RemoveMailItem( nMail );
	}
	if( !itemRestore.IsEmpty() && g_uIdofMulti== uQuery )
		g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemRestore, 0, "", "" );
}

void CDPDatabaseClient::OnGetMailGold( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	int nRestore	= 0;

	if( pMailBox )
	{
		CMail* pMail	= pMailBox->GetMail( nMail );
		if( pMail && pMail->m_nGold > 0 )
		{
			nRestore	= pMail->m_nGold;
			CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
			if( IsValidObj( pUser ) )
			{
				if( g_uIdofMulti == uQuery )
				{
					if( CanAdd( pUser->GetGold(), pMail->m_nGold ) )
					{
						pUser->AddGold( pMail->m_nGold );
						pUser->AddRemoveMail( nMail, CMail::gold );
						nRestore	= 0;
					}
				}
				else
				{
					pUser->AddRemoveMail( nMail, CMail::gold );
					nRestore	= 0;
				}
			}
		}
		pMailBox->RemoveMailGold( nMail );
	}
	if( nRestore > 0 && g_uIdofMulti == uQuery )
	{
		CItemElem itemElem;		// empty item
		g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemElem, nRestore, "", "" );
	}
}

void CDPDatabaseClient::OnReadMail( CAr & ar, DPID, DPID ) {
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	if (!pMailBox) return;

	pMailBox->ReadMail( nMail );

	CUser * pUser = prj.GetUserByID(idReceiver);

	if (IsValidObj(pUser)) {
		pUser->mailBoxRequest.ResetCheckClientReq();
		pUser->AddRemoveMail(nMail, CMail::read);

		if (!pMailBox->IsStampedMailExists() && pUser->IsMode(MODE_MAILBOX)) {
			pUser->SetNotMode(MODE_MAILBOX);
			g_UserMng.AddModifyMode(pUser);
		}
	}

}

void CDPDatabaseClient::OnAllMail(CAr & ar, DPID, DPID) {
	ar >> CPost::GetInstance()->AsStructure();
}

void CDPDatabaseClient::OnQueryRemoveGuildBankTbl( CAr & ar, DPID, DPID )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == 0 )
		return;

	int nNo;
	u_long idGuild;
	DWORD dwItemId;
	SERIALNUMBER iSerialNumber;
	DWORD dwItemNum;
	ar >> nNo >> idGuild >> dwItemId >> iSerialNumber >> dwItemNum;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	DWORD dwRemoved	= 0;
	if( pGuild )
	{
		if( dwItemId == 0 )		// penya
		{
			dwRemoved	= dwItemNum;
			if( pGuild->m_nGoldGuild - dwRemoved > pGuild->m_nGoldGuild )	// overflow
				dwRemoved	= pGuild->m_nGoldGuild;
			pGuild->m_nGoldGuild	-= dwRemoved;
			if( dwRemoved > 0 )
			{
				g_DPSrvr.UpdateGuildBank( pGuild, GUILD_QUERY_REMOVE_GUILD_BANK );
				CUser* pUsertmp;
				CGuildMember* pMember;
				for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
				{
					pMember		= i->second;
					pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
					if( IsValidObj( pUsertmp ) && pUsertmp->m_bGuildBank )
						pUsertmp->AddRemoveGuildBankItem( idGuild,  NULL_ID, dwRemoved );
				}
			}
		}
		else	// item
		{
			CItemElem* pItemElem	= pGuild->GetItem( dwItemId, iSerialNumber );
			if( pItemElem )
			{
				DWORD dwId	= pItemElem->m_dwObjId;
				if( static_cast<short>( dwItemNum ) < 0 )
					dwItemNum	= 0;

				dwRemoved	= (DWORD)pGuild->RemoveItem( dwId, static_cast<short>( dwItemNum ) );
				if( dwRemoved > 0 )
				{
					g_DPSrvr.UpdateGuildBank( pGuild, GUILD_QUERY_REMOVE_GUILD_BANK );
#ifdef _DEBUG
					char lpOutputString[200];
					sprintf( lpOutputString, "WORLD: OnQueryRemoveGuildBankTbl: dwId = %d, nRemoved=%d", dwId, dwRemoved );
					OutputDebugString( lpOutputString );
#endif	// _DEBUG
					CUser* pUsertmp;
					CGuildMember* pMember;
					for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
					{
						pMember		= i->second;
						pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
						if( IsValidObj( pUsertmp ) && pUsertmp->m_bGuildBank )
							pUsertmp->AddRemoveGuildBankItem( idGuild,  dwId, dwRemoved );
					}
				}
			}
		}
	}
	SendQueryRemoveGuildBankTbl( nNo, dwRemoved );
}

void CDPDatabaseClient::SendQueryRemoveGuildBankTbl(int nNo, DWORD dwRemoved) {
	SendPacket<PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, int, DWORD>(nNo, dwRemoved);
}

void CDPDatabaseClient::OnEventGeneric( CAr & ar, DPID, DPID )
{
	DWORD dwFlag;
	ar >> *CEventGeneric::GetInstance();
	ar >> dwFlag;

	for (const EVENT_GENERIC & pEvent : CEventGeneric::GetInstance()->GetEventList()) {
		char lpOutputString[512]	= { 0, };
		sprintf( lpOutputString, "dwFlag=0x%08x, nId=%d, nFlag=%d", dwFlag, pEvent.nId, pEvent.nFlag );
		OutputDebugString( lpOutputString );

		if( dwFlag & pEvent.nFlag )
			g_eLocal.SetState( pEvent.nId, 1 );
	}
}

void CDPDatabaseClient::OnEventFlag( CAr & ar, DPID, DPID )
{
	DWORD dwFlag;
	ar >> dwFlag;

	// 康
	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "OnEventFlag: dwFlag=0x%08x", dwFlag );
	OutputDebugString( lpOutputString );

	for (const EVENT_GENERIC & pEvent : CEventGeneric::GetInstance()->GetEventList()) {
		char lpOutputString[100]	= { 0, };
		sprintf( lpOutputString, "OnEventFlag: nId=%d, nFlag=%d", pEvent.nId, pEvent.nFlag );
		OutputDebugString( lpOutputString );

		if( dwFlag & pEvent.nFlag )
		{
			if( g_eLocal.GetState( pEvent.nId ) == 0 )
			{
				if( g_eLocal.SetState( pEvent.nId, 1 ) )
				{
					g_UserMng.AddSetLocalEvent( pEvent.nId, 1 );
					Error( "event: %d: 1", pEvent.nId );
				}
			}
		}
		else
		{
			if( g_eLocal.GetState( pEvent.nId ) == 1 )
			{
				if( g_eLocal.ClearEvent( pEvent.nId ) )
				{
					g_UserMng.AddSetLocalEvent( pEvent.nId, 0 );
					Error( "OnEvent: %d: 0", pEvent.nId );
				}
			}
		}
	}
}

void CDPDatabaseClient::OnEventStateLua( CAr & ar, DPID, DPID )
{
	BOOL bTextOut = FALSE;
	int nMapSize = 0;
	ar >> bTextOut;
	ar >> nMapSize;
	
	BYTE nId;
	BOOL bState;
	for( int i=0; i<nMapSize; i++ )
	{
		ar >> nId;
		ar >> bState;
		prj.m_EventLua.SetState( nId, bState );
		if( bTextOut )
			g_UserMng.AddEventLua( nId, bState );
	}
	prj.m_EventLua.PrepareProxy();
}

void CDPDatabaseClient::OnEventLuaChanged( CAr & ar, DPID, DPID )
{
	prj.m_EventLua.LoadScript();
	if( prj.m_EventLua.m_bRun )
		Error( "Event.lua 재실행 완료!!!" );
}

void CDPDatabaseClient::OnGC1to1Open( CAr & ar, DPID, DPID )
{
	g_GuildCombat1to1Mng.GuildCombat1to1Open();
}

void CDPDatabaseClient::OnGC1to1TenderGuildFromDB( CAr & ar, DPID, DPID )
{
	int nVecGuildSize = 0, nVecMemberSize = 0;
	u_long uGuildId = 0;
	int nPenya = 0;
	u_long uPlayerId = 0;

	CGuildCombat1to1Mng::__GC1TO1TENDER gc1to1Tender;
	
	g_GuildCombat1to1Mng.m_vecTenderGuild.clear();
	ar >> nVecGuildSize;
	for( int i=0; i<nVecGuildSize; i++ )
	{
		ar >> uGuildId >> nPenya;
		gc1to1Tender.ulGuildId = uGuildId;
		gc1to1Tender.nPenya = nPenya;
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
			g_GuildCombat1to1Mng.m_vecTenderGuild.push_back( gc1to1Tender );
		ar >> nVecMemberSize;
		for( int j=0; j<nVecMemberSize; j++ )
		{
			ar >> uPlayerId;
			if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
				g_GuildCombat1to1Mng.m_vecTenderGuild[i].vecMemberId.push_back( uPlayerId );
		}
	}

	g_GuildCombat1to1Mng.m_vecTenderFailGuild.clear();
	ar >> nVecGuildSize;
	for( int i=0; i<nVecGuildSize; i++ )
	{
		ar >> uGuildId >> nPenya;
		gc1to1Tender.ulGuildId = uGuildId;
		gc1to1Tender.nPenya = nPenya;
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
			g_GuildCombat1to1Mng.m_vecTenderFailGuild.push_back( gc1to1Tender );
	}
}

void CDPDatabaseClient::CalluspLoggingQuest( u_long idPlayer, QuestId nQuest, int nState, const char* pszComment )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_USPLOGGINGQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << static_cast<int>(nQuest.get()) << nState;
	ar.WriteString( pszComment );
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __INVALID_LOGIN_0320
void CDPDatabaseClient::CalluspXXXMultiServer( u_long uKey, CUser* pUser )
#else	// __INVALID_LOGIN_0320
void CDPDatabaseClient::CalluspXXXMultiServer( u_long uKey, u_long idPlayer )
#endif	// __INVALID_LOGIN_0320
{
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_XXX_MULTI_SERVER, DPID_UNKNOWN, DPID_UNKNOWN );
#ifdef __INVALID_LOGIN_0320
	if( pUser )
	{
		ar << uKey << pUser->m_idPlayer;
		ar.WriteString( pUser->m_playAccount.lpszAccount );
	}
	else
	{
		ar << uKey << 0;
		ar.WriteString( "" );	// not used
	}
#else	// __INVALID_LOGIN_0320
	ar << uKey << idPlayer;
#endif	// __INVALID_LOGIN_0320
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::CalluspPetLog(u_long idPlayer, SERIALNUMBER iSerial, DWORD dwData, int nType, CPet * pPet) {
	if (!pPet) return;

	SendPacket<PACKETTYPE_CALL_USP_PET_LOG, u_long, SERIALNUMBER, DWORD, int, CPet>(
		idPlayer, iSerial, dwData, nType, *pPet
	);
}

void CDPDatabaseClient::SendEventLuaChanged() {
	SendPacket<PACKETTYPE_EVENTLUA_CHANGED>();
}

void CDPDatabaseClient::SendGC1to1StateToDBSrvr() {
	SendPacket<PACKETTYPE_GC1TO1_STATETODB>(g_GuildCombat1to1Mng.m_nState);
}

void CDPDatabaseClient::SendGC1to1Tender(char cGU, u_long uGuildId, int nPenya, char cState) {
	SendPacket<PACKETTYPE_GC1TO1_TENDERTODB, char, u_long, int, char>(
		cGU, uGuildId, nPenya, cState
	);
}

void CDPDatabaseClient::SendGC1to1LineUp( u_long uGuildId, std::vector<u_long>& vecMemberId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_LINEUPTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId;
	ar << vecMemberId.size();
	for( int i=0; i<(int)( vecMemberId.size() ); i++ )
		ar << vecMemberId[i];
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1WarPerson(u_long uGuildId, u_long uIdPlayer, char cState) {
	SendPacket<PACKETTYPE_GC1TO1_WARPERSONTODB, u_long, u_long, char>(uGuildId, uIdPlayer, cState);
}

void CDPDatabaseClient::SendGC1to1WarGuild(DWORD dwWorldId, u_long uGuildId_0, u_long uGuildId_1, char cState) {
	SendPacket<PACKETTYPE_GC1TO1_WARGUILDTODB, DWORD, u_long, u_long, char>(
		dwWorldId, uGuildId_0, uGuildId_1, cState
	);
}



void CDPDatabaseClient::SendQueryGetGuildBankLogList(u_long idReceiver, DWORD idGuild, BYTE byListType) {
	SendPacket<PACKETTYPE_GUILDLOG_VIEW, u_long, u_long, BYTE, u_long>(
		idReceiver, idGuild, byListType, g_uIdofMulti
	);
}

void CDPDatabaseClient::OnGuildBankLogViewFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	BYTE	byListType = 0;
	ar >> nCount;
	ar >> byListType;	
	ar >> idReceiver;	

	if( nCount > GUILDBANKLOGVIEW_MAX )
		return;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__GUILDBANKLOG_ENTRY	logs[GUILDBANKLOGVIEW_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar.ReadString( logs[i].szPlayer, 42 );		// 
			ar >> logs[i].nDate;					// 
			ar >> logs[i].nItemID;					// 
			ar >> logs[i].nItemAbilityOption;					// 
			ar >> logs[i].nItemCount;					// 
		}
		pUser->AddGuildBankLogView( byListType, (short)nCount, logs);
	}
}


void CDPDatabaseClient::SendQueryGetSealChar( u_long idReceiver ,const char* szAccount)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHAR_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGetSealCharConm(u_long idReceiver) {
	SendPacket<PACKETTYPE_SEALCHARCONM_REQ, u_long, u_long>(idReceiver, g_uIdofMulti);
}

void CDPDatabaseClient::OnSealCharFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	ar >> nCount;
	ar >> idReceiver;	

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__SEALCHAR_ENTRY	seals[SEALCHAR_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar >> seals[i].nPlayerSlot;					// 
			ar >> seals[i].idPlayer;					// 
			ar.ReadString( seals[i].szPlayer, 42 );		// 
		}

		if(nCount < 2 )
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
		else
			pUser->AddSealChar( (short)nCount, seals);
	}
}

void CDPDatabaseClient::SendQueryGetSealCharGet( u_long idReceiver ,const char* szAccount,DWORD dwGetId)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARGET_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	ar << dwGetId;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendQueryGetSealCharSet( u_long idReceiver ,const char* szAccount,LONG lSetPlayerSlot,DWORD dwSetPlayerId)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARSET_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	ar << lSetPlayerSlot;
	ar << dwSetPlayerId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnSealCharGetFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	LONG	lSetSlot = -1;
	DWORD dwGetID;
	ar >> nCount;
	ar >> idReceiver;
	ar >> dwGetID;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__SEALCHAR_ENTRY	seals[SEALCHAR_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar >> seals[i].nPlayerSlot;					// 
			ar >> seals[i].idPlayer;					// 
			ar.ReadString( seals[i].szPlayer, 42 );		// 
		}

		if( nCount > 2 || nCount < 1)
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
			g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
		}
		else
		{
			if( pUser->IsUsableState( dwGetID ) == FALSE )
				return;

			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwGetID );
			if( IsUsableItem( pItemElem ) )
			{
				if(pItemElem->m_dwItemId != II_SYS_SYS_SCR_SEALCHARACTER )
					return;
				if( nCount == 1 )
				{
					if(seals[0].nPlayerSlot == 0)
						lSetSlot = 1;
					else if(seals[0].nPlayerSlot == 1 || seals[0].nPlayerSlot == 2 )
						lSetSlot = 0;
				}
				else if(nCount == 2)
				{
					if(seals[0].nPlayerSlot == 0)
					{
						if(seals[1].nPlayerSlot == 1)
							lSetSlot = 2;
						else if(seals[1].nPlayerSlot == 2)
							lSetSlot = 1;
					}
					else if(seals[0].nPlayerSlot == 1 )
					{
						if(seals[1].nPlayerSlot == 0)
							lSetSlot = 2;
						else if(seals[1].nPlayerSlot == 2)
							lSetSlot = 0;
					}
					else if(seals[0].nPlayerSlot == 2 )
					{
						if(seals[1].nPlayerSlot == 0)
							lSetSlot = 1;
						else if(seals[1].nPlayerSlot == 1)
							lSetSlot = 0;
					}
				}
				g_dpDBClient.SendQueryGetSealCharSet( pUser->m_idPlayer,pUser->m_playAccount.lpszAccount,lSetSlot,pItemElem->m_nHitPoint);
				pUser->AddSealCharSet();

				ItemProp* pItemProp  = pItemElem->GetProp();
		
				g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemProp );		
				OBJID       dwTmpObjId = pItemElem->m_dwObjId;
				pUser->RemoveItem( (BYTE)( dwTmpObjId ), 1 );
				g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
			}
		}
	}
}

void CDPDatabaseClient::OnElectionAddDeposit( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	__int64 iDeposit;
	time_t tCreate;
	BOOL bRet;
	ar >> idPlayer >> iDeposit >> tCreate >> bRet;

	CUser* pUser	= static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bRet )
	{
		IElection* pElection	= CSLord::Instance()->GetElection();
		pElection->AddDeposit( idPlayer, iDeposit, tCreate );
	}
	else
	{
		// *Restore
		if( IsValidObj( pUser ) )
			pUser->AddGold( static_cast<int>( iDeposit ) );
	}
}

void CDPDatabaseClient::OnElectionSetPledge( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	char szPledge[CCandidate::nMaxPledgeLen]	= {0, };
	BOOL bRet;
	ar >> idPlayer;
	ar.ReadString( szPledge, CCandidate::nMaxPledgeLen );
	ar >> bRet;

	CUser* pUser	= static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	IElection* pElection	= CSLord::Instance()->GetElection();
	if( bRet )
	{
		pElection->SetPledge( idPlayer, szPledge );
	}
	else
	{
		// *Restore
		if( pElection->HasPledge( idPlayer ) )
		{
			if( IsValidObj( pUser ) )
				pUser->AddGold( static_cast<int>( CCandidate::nSetPledgeCost ) );
		}
	}
}

void CDPDatabaseClient::OnElectionIncVote( CAr & ar, DPID, DPID )
{
	u_long idPlayer, idElector;
	BOOL bRet;
	ar >> idPlayer >> idElector >> bRet;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idElector );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bRet )
	{
		IElection* pElection	= CSLord::Instance()->GetElection();
		pElection->IncVote( idPlayer, idElector );
		if( IsValidObj( pUser ) )	//
		{
			pUser->SetElection( pElection->GetId() );
			// 투표 참여 버프
			ItemProp* pProp		= prj.GetItemProp( II_SYS_SYS_VOTE_THANKS );
			if( pProp )
				pUser->DoApplySkill( pUser, pProp, NULL );
		}
	}
	else
	{
		// *print
		if( IsValidObj( pUser ) )
			pUser->AddDefinedText( TID_GAME_ELECTION_INC_VOTE_E004 );	// 이미 투표에 참여하셨습니다.
	}
}

void CDPDatabaseClient::OnElectionBeginCandidacy( CAr & ar, DPID, DPID )
{
	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->BeginCandidacy();
}

void CDPDatabaseClient::OnElectionBeginVote( CAr & ar, DPID, DPID )
{
	int nRequirement;
	ar >> nRequirement;

	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->BeginVote( nRequirement );
}

void CDPDatabaseClient::OnElectionEndVote( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	ar >> idPlayer;

	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->EndVote( idPlayer );
}

void CDPDatabaseClient::OnLord( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLord" );
	ar >> *CSLord::Instance();
}

void CDPDatabaseClient::OnLEventCreate( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLEventCreate" );

	const auto [pComponent, bResult] = ar.Extract<CLEComponent, BOOL>();

	CUser* pUser	= g_UserMng.GetUserByPlayerID( pComponent.GetIdPlayer() );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if (bResult) {
		ILordEvent * pEvent = CSLord::Instance()->GetEvent();
		pEvent->AddComponent(pComponent);
	}
}

void CDPDatabaseClient::OnLEventInitialize( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLEventInitialize" );
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	pEvent->Initialize();
}

void CDPDatabaseClient::OnLordSkillUse( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLordSkillUse" );
	u_long idPlayer, idTarget;
	int nSkill, nRet;
	ar >> idPlayer >> idTarget >> nSkill >> nRet;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( !nRet )
	{
		CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
		CLordSkillComponentExecutable* pComponent	= pSkills->GetSkill( nSkill );
		pComponent->Execute( idPlayer, idTarget, NULL );
		if( IsValidObj( pUser ) )
			g_UserMng.AddLordSkillUse( pUser, idTarget, nSkill );
	}
	else
	{
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( IsValidObj( pUser ) )
			pUser->AddDefinedText( nRet );
	}
}

void CDPDatabaseClient::OnLordSkillTick( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLordSkillTick" );

	CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
	pSkills->ReadTick( ar );
	if( CSLord::Instance()->Get() == NULL_ID )
		return;
	CUser* pLord	= g_UserMng.GetUserByPlayerID( CSLord::Instance()->Get() );
	if( IsValidObj( pLord ) )
		pLord->AddLordSkillTick( *pSkills );
}

void CDPDatabaseClient::OnLEventTick( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLEventTick" );
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	pEvent->ReadTick(ar);
	g_UserMng.AddLEventTick( *pEvent );
	pEvent->EraseExpiredComponents();
}

void CDPDatabaseClient::SendElectionAddDeposit(u_long idPlayer, __int64 iDeposit) {
	SendPacket<PACKETTYPE_ELECTION_ADD_DEPOSIT, u_long, __int64>(idPlayer, iDeposit);
}

void CDPDatabaseClient::SendElectionSetPledge( u_long idPlayer, const char* szPledge )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPledge );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendElectionIncVote( u_long idPlayer, u_long idElector )
{
	SendPacket<PACKETTYPE_ELECTION_INC_VOTE, u_long, u_long>(idPlayer, idElector);
}

void CDPDatabaseClient::SendLEventCreate(u_long idPlayer, int iEEvent, int iIEvent) {
	SendPacket<PACKETTYPE_L_EVENT_CREATE, u_long, int, int>(
		idPlayer, iEEvent, iIEvent
	);
}

void CDPDatabaseClient::SendLordSkillUse(u_long idPlayer, u_long idTarget, int nSkill) {
	SendPacket<PACKETTYPE_LORD_SKILL_USE, u_long, u_long, int>(
		idPlayer, idTarget, nSkill
	);
}

// operator commands
void CDPDatabaseClient::SendLEventInitialize() {
	SendPacket<PACKETTYPE_L_EVENT_INITIALIZE>();
}

void CDPDatabaseClient::SendElectionProcess(BOOL bRun) {
	SendPacket<PACKETTYPE_ELECTION_PROCESS, BOOL>(bRun);
}

void CDPDatabaseClient::SendElectionBeginCandidacy() {
	SendPacket<PACKETTYPE_ELECTION_BEGIN_CANDIDACY>();
}

void CDPDatabaseClient::SendElectionBeginVote() {
	SendPacket<PACKETTYPE_ELECTION_BEGIN_VOTE>();
}

void CDPDatabaseClient::SendElectionBeginEndVote() {
	SendPacket<PACKETTYPE_ELECTION_END_VOTE>();
}

void CDPDatabaseClient::SendSecretRoomWinGuild(BYTE nCont, DWORD dwGuildId) {
	SendPacket<PACKETTYPE_TAX_SET_SECRETROOM_WINNER, BYTE, DWORD>(nCont, dwGuildId);
}

void CDPDatabaseClient::SendLord(DWORD dwIdPlayer) {
	SendPacket<PACKETTYPE_TAX_SET_LORD, DWORD>(dwIdPlayer);
}

void CDPDatabaseClient::SendTaxRate(BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate) {
	SendPacket<PACKETTYPE_TAX_SET_TAXRATE, BYTE, int, int>(
		nCont, nSalesTaxRate, nPurchaseTaxRate
	);
}

void CDPDatabaseClient::SendApplyTaxRateNow() {
	SendPacket<PACKETTYPE_TAX_APPLY_TAXRATE_NOW>();
}

void CDPDatabaseClient::OnTaxInfo( CAr & ar, DPID, DPID )
{
	ar >> *CTax::GetInstance();
	
	BOOL bConnect, bToAllClient;
	ar >> bConnect;
	if( bConnect )
	{
		ar >> CTax::GetInstance()->m_nMinTaxRate;
		ar >> CTax::GetInstance()->m_nMaxTaxRate;
#ifdef __OCCUPATION_SHOPITEM
		size_t nSize;
		ar >> nSize;
		for( size_t i=0; i<nSize; i++ )
		{
			DWORD dwItemId;
			ar >> dwItemId;
			CTax::GetInstance()->m_vecdwOccupationShopItem.push_back( dwItemId );
		}
#endif // __OCCUPATION_SHOPITEM
	}
	ar >> bToAllClient;
	if( bToAllClient )	// 세율 변경 시점
	{
		g_UserMng.AddTaxInfo();
		if( g_eLocal.GetState( EVE_SECRETROOM ) )
			g_DPCoreClient.SendCaption( GETTEXT(TID_GAME_TAX_CHANGENEXT) );
		// 사냥터용 비밀의 방에 있는 놈들을 쫓아낸다.
		CWorld* pWorld = g_WorldMng.GetWorld( WI_DUNGEON_SECRET_0 );
		if( !pWorld )
			return;


		for (CObj * pObj : pWorld->m_Objs.ValidObjs()) {
			if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() )
				((CUser*)pObj)->Replace( ((CUser*)pObj)->m_idMarkingWorld, ((CUser*)pObj)->m_vMarkingPos, REPLACE_NORMAL, nTempLayer );
		}
	}
}

void CDPDatabaseClient::SendAddTax(BYTE nCont, int nTax, BYTE nTaxKind) {
	SendPacket<PACKETTYPE_TAX_ADDTAX, BYTE, int, BYTE>(nCont, nTax, nTaxKind);
}

void CDPDatabaseClient::SendSecretRoomInsertToDB(BYTE nContinent, __SECRETROOM_TENDER & srTender) {
	SendPacket<PACKETTYPE_SECRETROOM_TENDER_INSERTTODB, BYTE, DWORD, int>(nContinent, srTender.dwGuildId, srTender.nPenya);
}

void CDPDatabaseClient::SendSecretRoomUpdateToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_TENDER_UPDATETODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId << srTender.nPenya << chState;
	if( chState == 'W' || chState == 'L' )
		ar << srTender.dwWorldId << srTender.nWarState << srTender.nKillCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomInsertLineUpToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_LINEUP_INSERTTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId;
	ar << srTender.vecLineUpMember.size();
	for( int i=0; i<(int)( srTender.vecLineUpMember.size() ); i++ )
		ar << srTender.vecLineUpMember[i];
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomClosed() {
	SendPacket<PACKETTYPE_SECRETROOM_CLOSED>();
}

void CDPDatabaseClient::OnSecretRoomInfoClear(CAr & ar, DPID, DPID) {
	CSecretRoomMng * pSRMng = CSecretRoomMng::GetInstance();
	for (auto & [_, pSrCont] : pSRMng->m_mapSecretRoomContinent) {
		pSrCont->m_vecSecretRoomTender.clear();
	}
}

void CDPDatabaseClient::OnSecretRoomTenderInfo( CAr & ar, DPID, DPID )
{
	BYTE nContinent; DWORD dwGuildId, dwMemberId; int nPenya, nSize;
	std::vector<DWORD> vecMemberId;
	ar >> nContinent >> dwGuildId >> nPenya;
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwMemberId;
		vecMemberId.push_back( dwMemberId );
	}

	if( !g_eLocal.GetState( EVE_SECRETROOM ) )
		return;

	CSecretRoomMng* pSRMng = CSecretRoomMng::GetInstance();
	auto it = pSRMng->m_mapSecretRoomContinent.find( nContinent );
	if( it!=pSRMng->m_mapSecretRoomContinent.end() )
	{
		CSecretRoomContinent* pSRCont = it->second;
		__SECRETROOM_TENDER srTender;
		srTender.dwGuildId = dwGuildId;
		srTender.nPenya = nPenya;
		srTender.vecLineUpMember = vecMemberId;
		pSRCont->m_vecSecretRoomTender.push_back( srTender );
	}
}

void CDPDatabaseClient::OnRainbowRaceInfo( CAr & ar, DPID, DPID )
{
	CRainbowRaceMng::GetInstance()->Clear();
	
	int nSize;
	DWORD dwPlayerId;
	
	// 신청자 목록을 받아서 등록한다.
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwPlayerId;
		CRainbowRaceMng::GetInstance()->SetApplication( dwPlayerId );
	}

	// 이전 대회 랭킹 정보를 받아온다.
	std::vector<DWORD> vec_dwPrevRanking;
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwPlayerId;
		vec_dwPrevRanking.push_back( dwPlayerId );
	}
	CRainbowRaceMng::GetInstance()->SetPrevRanking( vec_dwPrevRanking );
}

void CDPDatabaseClient::SendRainbowRaceReqLoad() {
	SendPacket<PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD>();
}

void CDPDatabaseClient::SendRainbowRaceApplication(DWORD dwPlayerId) {
	SendPacket<PACKETTYPE_RAINBOWRACE_APPTODB, DWORD>(dwPlayerId);
}

void CDPDatabaseClient::SendRainbowRaceFailedUser(DWORD dwPlayerId) {
	SendPacket<PACKETTYPE_RAINBOWRACE_FAILEDTODB, DWORD>(dwPlayerId);
}

void CDPDatabaseClient::SendRainbowRaceRanking(DWORD dwPlayerId, int nRanking) {
	SendPacket<PACKETTYPE_RAINBOWRACE_RANKINGTODB, DWORD, int>(dwPlayerId, nRanking);
}

void CDPDatabaseClient::OnHousingLoadInfo( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	ar >> dwPlayerId;

	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
	if( !pHousing )
		pHousing = CHousingMng::GetInstance()->CreateHousing( dwPlayerId );
	else
		Error( "CDPDatabaseClient::OnHousingLoadInfo - %d", dwPlayerId );

	if( pHousing )
	{
		ar >> *pHousing;
		CUser * pUser = prj.GetUserByID(dwPlayerId);
		if( IsValidObj( pUser ) )
			pUser->AddHousingAllInfo( pHousing );
		pHousing->AddAllFurnitureControl();
	}
}

void CDPDatabaseClient::OnHousingSetFunitureList(CAr & ar, DPID, DPID) {
	auto [dwPlayerId, housingInfo, bAdd] = ar.Extract<DWORD, HOUSINGINFO, BOOL>();
	CHousingMng::GetInstance()->SetFurnitureList(dwPlayerId, housingInfo, bAdd);
}

void CDPDatabaseClient::OnHousingSetupFuniture(CAr & ar, DPID, DPID) {
	const auto [dwPlayerId, housingInfo] = ar.Extract<DWORD, HOUSINGINFO>();
	CHousingMng::GetInstance()->SetupFurniture(dwPlayerId, housingInfo);
}

void CDPDatabaseClient::OnHousingSetVisitAllow( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId, dwTargetId;
	BOOL bAllow;
	ar >> dwPlayerId >> dwTargetId >> bAllow;

	CHousingMng::GetInstance()->SetVisitAllow( dwPlayerId, dwTargetId, bAllow );
}

void CDPDatabaseClient::OnHousingDBFailed( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId, dwItemId;
	ar >> dwPlayerId >> dwItemId;
	
	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
	if( pHousing )	pHousing->Setting( FALSE );

	if( dwItemId != NULL_ID )
	{
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum = 1;
			if( !pUser->CreateItem( &itemElem ) )
				Error( "CDPDatabaseClient::OnHousingDBFailed -> CreateItem Failed! - %d, %s, %d", dwPlayerId, pUser->GetName(), dwItemId );
		}
		else
			Error( "CDPDatabaseClient::OnHousingDBFailed -> Invalid User - %d, %d", dwPlayerId, dwItemId );
	}
}

void CDPDatabaseClient::SendPropose(u_long idProposer, u_long idTarget) {
	SendPacket<PACKETTYPE_PROPOSE, u_long, u_long>(idProposer, idTarget);
}

void CDPDatabaseClient::SendCouple(u_long idProposer, u_long idTarget) {
	SendPacket<PACKETTYPE_COUPLE, u_long, u_long>(idProposer, idTarget);
}

void CDPDatabaseClient::SendDecouple(u_long idPlayer) {
	SendPacket<PACKETTYPE_DECOUPLE, u_long>(idPlayer);
}

void CDPDatabaseClient::SendClearPropose() {
	SendPacket<PACKETTYPE_CLEAR_PROPOSE>();
}

void CDPDatabaseClient::OnProposeResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnProposeResult( ar );
}

void CDPDatabaseClient::OnCoupleResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnCoupleResult( ar );
}

void CDPDatabaseClient::OnDecoupleResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnDecoupleResult( ar );
}

void CDPDatabaseClient::OnCouple(CAr & ar, DPID, DPID) {
	ar >> *CCoupleHelper::Instance();
}

void CDPDatabaseClient::SendQueryAddCoupleExperience(u_long idPlayer, int nExperience) {
	SendPacket<PACKETTYPE_ADD_COUPLE_EXPERIENCE, u_long, int>(idPlayer, nExperience);
}

void CDPDatabaseClient::OnAddCoupleExperience( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnAddCoupleExperience( ar );
}

#ifdef __FUNNY_COIN
void CDPDatabaseClient::SendFunnyCoinReqUse(DWORD dwPlayerId, CItemElem * pItemElem)	// 퍼니코인을 사용했음을 TransServer에 알린다.
{
	SendPacket<PACKETTYPE_FUNNYCOIN_REQ_USE, DWORD, DWORD, SERIALNUMBER>(
		dwPlayerId,
		pItemElem->m_dwItemId, pItemElem->GetSerialNumber()
	);
}

void CDPDatabaseClient::OnFunnyCoinAckUse( CAr & ar, DPID, DPID )	// 퍼니코인 사용에 대한 응답을 TranServer로 부터 받았다.
{
	CFunnyCoin::GetInstance()->OnFunnyCoinAckUse( ar );
}
#endif // __FUNNY_COIN
void CDPDatabaseClient::OnPCBangToggle( CAr & ar, DPID, DPID )
{
	BOOL bApply;
	ar >> bApply;
	CPCBang::GetInstance()->SetApply( bApply );
}

#ifdef __VTN_TIMELIMIT
void CDPDatabaseClient::SendTimeLimitInfoReq( DWORD dwPlayerId, char* szAccount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	ar.WriteString( szAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnTimeLimitInfoAck( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	int nPlayTime;
	ar >> dwPlayerId >> nPlayTime;

	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
	if( IsValidObj( pUser ) )
	{
		pUser->m_nAccountPlayTime = nPlayTime;
		pUser->AddAccountPlayTime();

		//	mulcom	BEGIN100315	베트남 시간 제한
		if( ::GetLanguage() == LANG_VTN )
		{
			if( pUser->m_nAccountPlayTime != -1 )
			{
				pUser->AddBuff( BUFF_EQUIP, II_VIETNAM_BUFF01, 1, 999999999 );
			}
		}
		//	mulcom	END100315	베트남 시간 제한
	}
}

void CDPDatabaseClient::SendTimeLimitUpdateReq( char* szAccount, int nPlayTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szAccount );
	ar << nPlayTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnTimeLimitReset( CAr & ar, DPID, DPID )
{
	g_UserMng.ResetAccountPlayTime();
}

#endif // __VTN_TIMELIMIT

void CDPDatabaseClient::SendLogInstanceDungeon( DWORD dwDungeonId, DWORD dwWorldId, u_long uChannel, int nDungeonType, char chState )
{
	SendPacket<PACKETTYPE_INSTANCEDUNGEON_LOG>(
		dwDungeonId, dwWorldId, uChannel, nDungeonType, chState
	);
}

#ifdef __QUIZ
void CDPDatabaseClient::OnQuizEventOpen( CAr & ar, DPID, DPID )
{
	CQuiz::GetInstance()->OpenQuizEvent();
}

void CDPDatabaseClient::OnQuizList( CAr & ar, DPID, DPID )
{
	CQuiz::QUIZLIST QL;
	ar >> QL;
	CQuiz::GetInstance()->MakeQuizList( QL );
	ar >> CQuiz::GetInstance()->m_nQuizSize;
}

void CDPDatabaseClient::OnQuizEventNotice( CAr & ar, DPID, DPID )
{
	CQuiz::GetInstance()->NoticeMessage();
}

void CDPDatabaseClient::OnQuizEventChanged( CAr & ar, DPID, DPID )
{
	CQuiz::GetInstance()->LoadScript();
	if( !CQuiz::GetInstance()->IsRun() )
		Error( "QuizEvent.lua 재실행 완료!!!" );
}

void CDPDatabaseClient::SendQuizEventOpen(int nType) {
	SendPacket<PACKETTYPE_QUIZ_OPEN, int>(nType);
}

void CDPDatabaseClient::SendQuizEventState( int nState, int nChannel, int nWinnerCount, int nQuizCount )
{
	SendPacket<PACKETTYPE_QUIZ_STATE, int, int, int, int>(
		nState, nChannel, nWinnerCount, nQuizCount
	);
}

void CDPDatabaseClient::SendQuizEventEntrance(u_long idPlayer, int nChannel) {
	SendPacket<PACKETTYPE_QUIZ_ENTRANCE, u_long, int>(idPlayer, nChannel);
}

void CDPDatabaseClient::SendQuizEventSelect( u_long idPlayer, int nChannel, int nQuizId, int nSelect, int nAnswer )
{
	SendPacket<PACKETTYPE_QUIZ_SELECT>(idPlayer, nChannel, nQuizId, nSelect, nAnswer);
}

void CDPDatabaseClient::SendPostPrizeItem(u_long idPlayer, DWORD dwItemId, int nItemNum) {
	SendPacket<PACKETTYPE_QUIZ_PRIZEITEM, u_long, DWORD, int>(idPlayer, dwItemId, nItemNum);
}
#endif // __QUIZ

#ifdef __ERROR_LOG_TO_DB
void CDPDatabaseClient::SendErrorLogToDB( CUser* pUser, char chType, LPCTSTR szError )
{
	if( !IsValidObj( pUser ) )
		return;
	BEFORESENDDUAL( ar, PACKETTYPE_ERROR_LOG_TO_DB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer;
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << g_uIdofMulti << chType;
	ar.WriteString( szError );
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __ERROR_LOG_TO_DB

void CDPDatabaseClient::OnLoadGuildHouse( CAr & ar, DPID, DPID )
{
	ar >> *GuildHouseMng;
}
void CDPDatabaseClient::OnBuyGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnBuyGuildHouse( ar );
}
void CDPDatabaseClient::OnRemoveGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnRemoveGuildHouse( ar );
}

void CDPDatabaseClient::OnGuildHousePacket( CAr & ar, DPID, DPID )
{
	BOOL bResult; DWORD dwGuildId;
	int nPacketType, nIndex;
	GH_Fntr_Info gfi;
	
	ar >> bResult >> dwGuildId;
	ar >> nPacketType >> nIndex;
	ar >> gfi;
	
	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse )
	{
		if( bResult )
		{
			if( pGuildHouse->OnGuildHousePacket( nPacketType, gfi, nIndex ) )
				pGuildHouse->SendWorldToClient( nPacketType, gfi, nIndex );
		}
		else	// 테스트용 코드
		{
			pGuildHouse->AfterFailedGuildHousePacket( nPacketType, gfi, nIndex );
		}
		pGuildHouse->SetWaitDBAckPlayerId( NULL_ID );
	}
}

void CDPDatabaseClient::SendLogGuildFurniture(DWORD dwGuildId, GH_Fntr_Info & gfi, char chState) {
	SendPacket<PACKETTYPE_GUILDFURNITURE_LOG>(
		dwGuildId, gfi, chState
	);
}

#ifdef __GUILD_HOUSE_MIDDLE
void CDPDatabaseClient::OnGuildHouseTenderInfo( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderInfo( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderJoin( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderJoin( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderState( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderState( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderResult( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderResult( ar );
}

void CDPDatabaseClient::OnGuildHouseGradeUpdate( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseGradeUpdate( ar );
}

void CDPDatabaseClient::OnGuildHouseLevelUpdate( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseLevelUpdate( ar );
}

void CDPDatabaseClient::OnGuildHouseExpired( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseExpired( ar );
}
#endif // __GUILD_HOUSE_MIDDLE

void CDPDatabaseClient::OnAllCampus(CAr & ar, DPID, DPID) {
	ar >> *CCampusHelper::GetInstance();
}

void CDPDatabaseClient::OnAddCampusMember( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnAddCampusMember( ar );
}

void CDPDatabaseClient::OnRemoveCampusMember( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnRemoveCampusMember( ar );
}

void CDPDatabaseClient::OnUpdateCampusPoint( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnUpdateCampusPoint( ar );
}

void CDPDatabaseClient::SendAddCampusMember( u_long idMaster, int nMasterPoint, u_long idPupil, int nPupilPoint )
{
	SendPacket<PACKETTYPE_CAMPUS_ADD_MEMBER, u_long, int, u_long, int>(
		idMaster, nMasterPoint, idPupil, nPupilPoint
	);
}

void CDPDatabaseClient::SendRemoveCampusMember(u_long idCampus, u_long idPlayer) {
	SendPacket<PACKETTYPE_CAMPUS_REMOVE_MEMBER, u_long, u_long>(idCampus, idPlayer);
}

void CDPDatabaseClient::SendUpdateCampusPoint(u_long idPlayer, int nCampusPoint, BOOL bAdd, char chState) {
	SendPacket<PACKETTYPE_CAMPUS_UPDATE_POINT, u_long, int, BOOL, char>(
		idPlayer, nCampusPoint, bAdd, chState
	);
}

