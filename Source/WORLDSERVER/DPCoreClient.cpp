#include "StdAfx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "User.h"
#include "ServerDesc.h"
#include "WorldMng.h"
#include "DisplayedInfo.h"
#include "playerdata.h"
#include "eveschool.h"
#include "Party.h"
#include "GroupUtils.h"
#include "GroupedEmission.h"
#include "Environment.h"
#include "guild.h"
#include "guildwar.h"
#include "WantedListSnapshot.h"

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK


CDPCoreClient	g_DPCoreClient;


CDPCoreClient::CDPCoreClient()
{
	ON_MSG( PACKETTYPE_LOAD_WORLD, &CDPCoreClient::OnLoadWorld );
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, &CDPCoreClient::OnQueryTickCount );
	ON_MSG( PACKETTYPE_RECHARGE_IDSTACK, &CDPCoreClient::OnRecharge );
	ON_MSG( PACKETTYPE_SHOUT, &CDPCoreClient::OnShout );
	ON_MSG( PACKETTYPE_PLAYMUSIC, &CDPCoreClient::OnPlayMusic );
	ON_MSG( PACKETTYPE_PLAYSOUND, &CDPCoreClient::OnPlaySound );
	
	ON_MSG( PACKETTYPE_ERRORPARTY, &CDPCoreClient::OnErrorParty );
	ON_MSG(PACKETTYPE_ADDPARTYMEMBER_CoreWorld, &CDPCoreClient::OnAddPartyMember );
	ON_MSG(PACKETTYPE_REMOVEPARTYMEMBER_CoreWorld, &CDPCoreClient::OnRemovePartyMember );
	ON_MSG( PACKETTYPE_ADDPLAYERPARTY, &CDPCoreClient::OnAddPlayerParty );
	ON_MSG( PACKETTYPE_REMOVEPLAYERPARTY, &CDPCoreClient::OnRemovePlayerParty );
	ON_MSG( PACKETTYPE_GUILD_MEMBER_LEVEL, &CDPCoreClient::OnGuildMemberLv );
	ON_MSG( PACKETTYPE_SETPARTYEXP, &CDPCoreClient::OnSetPartyExp );
	ON_MSG( PACKETTYPE_REMOVEPARTYPOINT, &CDPCoreClient::OnRemovePartyPoint );
	ON_MSG( PACKETTYPE_CHANGETROUP, &CDPCoreClient::OnPartyChangeTroup );
	ON_MSG( PACKETTYPE_CHANPARTYNAME, &CDPCoreClient::OnPartyChangeName );
	ON_MSG( PACKETTYPE_SETPARTYMODE, &CDPCoreClient::OnSetPartyMode );
	ON_MSG( PACKETTYPE_PARTYCHANGEITEMMODE, &CDPCoreClient::OnPartyChangeItemMode );
	ON_MSG( PACKETTYPE_PARTYCHANGEEXPMODE, &CDPCoreClient::OnPartyChangeExpMode );
	ON_MSG( PACKETTYPE_CW_ADDFRIEND, &CDPCoreClient::OnAddFriend );
	ON_MSG( PACKETTYPE_REMOVEFRIEND, &CDPCoreClient::OnRemovefriend );


	ON_MSG( PACKETTYPE_ENVIRONMENTALL, &CDPCoreClient::OnEnvironmentEffect );


	ON_MSG( PACKETTYPE_PARTYCHAT, &CDPCoreClient::OnPartyChat );

	ON_MSG( PACKETTYPE_CREATE_GUILD, &CDPCoreClient::OnCreateGuild );
	ON_MSG( PACKETTYPE_DESTROY_GUILD, &CDPCoreClient::OnDestroyGuild );
	ON_MSG( PACKETTYPE_ADD_GUILD_MEMBER, &CDPCoreClient::OnAddGuildMember );
	ON_MSG( PACKETTYPE_REMOVE_GUILD_MEMBER, &CDPCoreClient::OnRemoveGuildMember );
	ON_MSG( PACKETTYPE_CW_GUILDLOGO, &CDPCoreClient::OnGuildLogoACK );
	ON_MSG( PACKETTYPE_CW_GUILDCONTRIBUTION, &CDPCoreClient::OnGuildContributionACK );
	ON_MSG( PACKETTYPE_CW_GUILDNOTICE, &CDPCoreClient::OnGuildNoticeACk );
	ON_MSG( PACKETTYPE_CW_ADDVOTERESULT, &CDPCoreClient::OnAddVoteResultACk );
	ON_MSG( PACKETTYPE_CW_MODIFYVOTE, &CDPCoreClient::OnModifyVote );
	ON_MSG( PACKETTYPE_GUILD_AUTHORITY, &CDPCoreClient::OnGuildAuthority );
	ON_MSG( PACKETTYPE_GUILD_PENYA, &CDPCoreClient::OnGuildPenya );
	ON_MSG( PACKETTYPE_GUILD_DB_REALPENYA, &CDPCoreClient::OnGuildRealPenya );
	ON_MSG( PACKETTYPE_GUILD_SETNAME, &CDPCoreClient::OnGuildSetName );
	ON_MSG( PACKETTYPE_GUILD_MSG_CONTROL, &CDPCoreClient::OnGuildMsgControl);
	ON_MSG( PACKETTYPE_GUILD_CLASS, &CDPCoreClient::OnGuildClass );
	ON_MSG( PACKETTYPE_GUILD_NICKNAME, &CDPCoreClient::OnGuildNickName );

	ON_MSG( PACKETTYPE_GUILD_MEMBERLOGOUT, &CDPCoreClient::OnGuildMemberLogOut );
	ON_MSG( PACKETTYPE_ACPT_GUILD_WAR, &CDPCoreClient::OnAcptWar );
	ON_MSG( PACKETTYPE_WAR_DEAD, &CDPCoreClient::OnWarDead );
	ON_MSG( PACKETTYPE_WAR_END, &CDPCoreClient::OnWarEnd );
	ON_MSG( PACKETTYPE_SURRENDER, &CDPCoreClient::OnSurrender );

	ON_MSG( PACKETTYPE_FRIENDINTERCEPTSTATE, &CDPCoreClient::OnFriendInterceptState );
	ON_MSG( PACKETTYPE_SETFRIENDSTATE, &CDPCoreClient::OnSetFriendState );

	ON_MSG( PACKETTYPE_GAMERATE, &CDPCoreClient::OnGameRate );
	ON_MSG( PACKETTYPE_LOADCONSTANT, &CDPCoreClient::OnLoadConstant );
	ON_MSG( PACKETTYPE_SETMONSTERRESPAWN, &CDPCoreClient::OnSetMonsterRespawn );
	ON_MSG( PACKETTYPE_PARTYCHANGELEADER, &CDPCoreClient::OnPartyChangeLeader );

	ON_MSG( PACKETTYPE_CW_WANTED_LIST, &CDPCoreClient::OnCWWantedList );
	ON_MSG( PACKETTYPE_CW_WANTED_REWARD, &CDPCoreClient::OnCWWantedReward );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, &CDPCoreClient::OnSetPlayerName );
	ON_MSG( PACKETTYPE_SETSNOOP, &CDPCoreClient::OnSetSnoop );

#ifdef __EVENT0913
	ON_MSG( PACKETTYPE_EVENT0913, &CDPCoreClient::OnEvent0913 );
#endif	// __EVENT0913

#ifdef __EVENT1206
	ON_MSG( PACKETTYPE_EVENT1206, &CDPCoreClient::OnEvent1206 );
#endif	// __EVENT1206
	ON_MSG( PACKETTYPE_EVENT, &CDPCoreClient::OnEvent );
	ON_MSG( PACKETTYPE_CHG_MASTER, &CDPCoreClient::OnChgMaster );
	ON_MSG( PACKETTYPE_GUILDCOMBAT_STATE, &CDPCoreClient::OnGuildCombatState );
	ON_MSG( PACKETTYPE_UNIFY, &CDPCoreClient::OnRemoveUserFromCORE );
	ON_MSG( PACKETTYPE_PING, &CDPCoreClient::OnPing );

	ON_MSG( PACKETTYPE_DESTROY_PLAYER, &CDPCoreClient::OnDestroyPlayer );
    //////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_ALLINFO, &CDPCoreClient::OnInstanceDungeonAllInfo );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_CREATE, &CDPCoreClient::OnInstanceDungeonCreate );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DESTROY, &CDPCoreClient::OnInstanceDungeonDestroy );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, &CDPCoreClient::OnInstanceDungeonSetCoolTimeInfo );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DELETECOOLTIME, &CDPCoreClient::OnInstanceDungeonDeleteCoolTimeInfo );
	
	ON_MSG(PACKETTYPE_BUYING_INFO, &CDPCoreClient::OnBuyingInfo);
	ON_MSG(PACKETTYPE_MODIFYMODE, &CDPCoreClient::OnModifyMode);

	m_bAlive	= TRUE;
	m_hWait		= WSACreateEvent();
	m_uRecharge		= 0;
}

CDPCoreClient::~CDPCoreClient()
{
	CLOSE_HANDLE( m_hWait );
}

void CDPCoreClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{

}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg , dwMsgSize );
	DWORD dw; ar >> dw;

#ifdef __NEW_PROFILE
	static std::map<DWORD, CString> mapstrProfile;
	auto it = mapstrProfile.find( dw );
	if( it == mapstrProfile.end() )
	{
		CString strTemp;
		strTemp.Format("CDPCoreClient::UserMessageHandler(0x%08x)", dw );
		it = mapstrProfile.emplace( dw, strTemp ).first;
	}
	_PROFILE( it->second );
#endif
	
	if( Handle(ar, dw) ) {
		if (ar.IsOverflow()) Error("World-Core: Packet %08x overflowed", dw);
	}
	else {
		Error( "Handler not found(%08x)\n",dw );
	}


}

BOOL CDPCoreClient::Run( LPSTR lpszAddr, USHORT uPort, u_long uKey )
{
	if( ConnectToServer( lpszAddr, uPort, TRUE ) )
	{
		MyRegister( uKey );
		return ( WaitForSingleObject( m_hWait, INFINITE ) != WAIT_TIMEOUT );
	}
	return FALSE;
}

void CDPCoreClient::MyRegister( u_long uKey )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MYREG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uKey;	// uKey는 g_uKey와 동일한 값 
	ar << (DWORD)timeGetTime();
	SEND( ar, this, DPID_SERVERPLAYER );
}

BOOL CDPCoreClient::CheckIdStack( void )
{
	int cbUser	= g_UserMng.GetCount();
	u_long uMax		= cbUser * 128 + 20480;
	int cbStack		= prj.m_objmap.m_idStack.GetCount();
	
	if( cbStack + m_uRecharge < uMax * 5 / 10 )	// 50%
	{
		m_uRecharge		= uMax - cbStack;
		Recharge( m_uRecharge );
		return FALSE;
	}
	return ( m_uRecharge == 0 );
}

void CDPCoreClient::Recharge( u_long uBlockSize )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RECHARGE_IDSTACK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uBlockSize;
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPCoreClient::SendToServer( LPBYTE lpBuffer, u_long uBufSize, DPID dpidCache, DPID dpidUser )
{
	CAr ar;
	int nBufSize;
	BYTE* lpBuf;

	ar << dpidCache << dpidUser;
	ar.Write( lpBuffer, uBufSize );	// overhead

	lpBuf	= ar.GetBuffer( &nBufSize );
	Send( (LPVOID)lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendEventRealItem( u_long uIdPlayer, int nRealItemIndex, int nRealItemCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RENEWEVNET, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uIdPlayer << nRealItemIndex << nRealItemCount;
	PASS( ar );
}


void CDPCoreClient::SendPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYLEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idparty << pUser->m_idPlayer << dwLevel << dwPoint << dwExp;	
	PASS( ar );
}
void CDPCoreClient::SendAddPartyExp( u_long uPartyId, int nMonLv, BOOL bSuperLeader , BOOL bLeaderSMExpUp )
{
	//극단에 속해있으면 포인트를 올려줌( core에서는 포인터만 가지고 있고 월드에서는 포인터를 이용하여~ 극단레벨을 구함)
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyId << nMonLv << bSuperLeader << bLeaderSMExpUp;
	PASS( ar );
}

void CDPCoreClient::SendRemovePartyPoint( u_long uPartyId, int nRemovePoint )
{
	if( nRemovePoint != 0 )
	{
		//극단에 속해있으면 포인트를 올려줌( core에서는 포인터만 가지고 있고 월드에서는 포인터를 이용하여~ 극단레벨을 구함)
		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYPOINT, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uPartyId << nRemovePoint;
		PASS( ar );
	}
}


void CDPCoreClient::SendGameRate( FLOAT fRate, BYTE nFlag )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GAMERATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << fRate;
	ar << nFlag;
	PASS( ar );	
}

void CDPCoreClient::SendLoadConstant()
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOADCONSTANT, DPID_UNKNOWN, DPID_UNKNOWN );
	PASS( ar );		
}

void CDPCoreClient::SendSetMonsterRespawn( u_long uidPlayer, DWORD dwMonsterID, DWORD dwRespawnNum, DWORD dwAttackNum, DWORD dwRect, DWORD dwRespawnTime, BOOL bFlying )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETMONSTERRESPAWN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << dwMonsterID << dwRespawnNum << dwAttackNum << dwRect << dwRespawnTime;
	ar << bFlying;
	PASS( ar );	
}


void CDPCoreClient::SendGuildMsgControl_Bank_Item( CUser* pUser, CItemElem* pItemElem, BYTE p_Mode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_UNKNOWN, DPID_UNKNOWN );

	GUILD_MSG_HEADER	Header;
	Header.HeadAMain	= p_Mode;
	Header.HeadASub		= (WORD)( pUser->m_idGuild );
	Header.HeadBMain	= GUILD_MSG_HEADER::GUILD_BANK;
	Header.HeadBSub		= GUILD_MSG_HEADER::ITEM;
	
	if (pUser->GetGuild())
	{
		ar.Write(&Header, sizeof(GUILD_MSG_HEADER));
		ar << *pItemElem;
	}
	
	PASS( ar );
}

void CDPCoreClient::SendGuildMsgControl_Bank_Penya( CUser* pUser, DWORD p_Penya, BYTE p_Mode, BYTE cbCloak )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_UNKNOWN, DPID_UNKNOWN );

	GUILD_MSG_HEADER	Header;
	Header.HeadAMain	= p_Mode;
	Header.HeadASub		= (WORD)( pUser->m_idGuild );
	Header.HeadBMain	= GUILD_MSG_HEADER::GUILD_BANK;
	Header.HeadBSub		= GUILD_MSG_HEADER::PENYA;
	
	if (pUser->GetGuild())
	{
		ar.Write(&Header, sizeof(GUILD_MSG_HEADER));
		ar << p_Penya;
		ar << cbCloak;
	}
	
	PASS( ar );
}

void CDPCoreClient::SendBlock( BYTE nGu, u_long uidPlayerTo, char *szNameTo, u_long uidPlayerFrom )
{
	BEFORESENDDUAL( ar, PACKETTYPE_BLOCK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nGu;
	ar << uidPlayerTo << uidPlayerFrom;
	ar.WriteString( szNameTo );
	PASS( ar );
	
}

// Handlers
void CDPCoreClient::OnLoadWorld( CAr & ar )
{
	CServerDesc desc;
	ar >> desc;
	ar >> g_PartyMng;
	g_GuildMng.Serialize( ar, FALSE );
	ar >> g_GuildWarMng;

	ar >> *CEnvironment::GetInstance();

#ifdef __EVENT0913
	BOOL bEvent0913;
	ar >> bEvent0913;
	if( bEvent0913 )
	{
		prj.m_fItemDropRate		*= 2;
		g_eLocal.SetState( EVE_EVENT0913, 1 );
	}
	Error( "OnLoadWorld-%d, %f", g_eLocal.GetState( EVE_EVENT0913 ), prj.m_fItemDropRate );
#endif	// __EVENT0913

#ifdef __EVENT1206
	BOOL bEvent1206;
	ar >> bEvent1206;
	if( bEvent1206 )
	{
		g_eLocal.SetState( EVE_EVENT1206, 1 );
	}
#endif	// __EVENT1206

	DWORD dwEvent;
	ar >> dwEvent;
	if( dwEvent & EVE_FLAG1219 )
	{
		g_eLocal.SetState( EVE_EVENT1219, 1 );
	}
	if( dwEvent & EVE_FLAG0127 )
	{
		g_eLocal.SetState( EVE_EVENT0127, 1 );
	}
	if( dwEvent & EVE_FLAG0214 )
	{
		g_eLocal.SetState( EVE_EVENT0214, 1 );
	}
	
	std::map<WorldId, std::string> knownWorlds;
	boost::container::flat_set<WorldId> badWorlds;

	for (const WorldId pJurisdiction : desc.m_lspJurisdiction) {
		const WORLD * lpWorld = g_WorldMng.GetWorldStruct(pJurisdiction);
		if (!lpWorld) {
			Error(__FUNCTION__ ": The world #%lu has no world Struct", pJurisdiction);
			badWorlds.emplace(pJurisdiction);
			continue;
		}

		knownWorlds.emplace(pJurisdiction, lpWorld->m_szFileName);

		g_WorldMng.Add(pJurisdiction);
	}

	g_DisplayedInfo.SetListOfMaps(knownWorlds, badWorlds);

	g_WorldMng.ReadObject();

	WSASetEvent( m_hWait );
}

void CDPCoreClient::OnRecharge( CAr & ar )
{
	OBJID idBase;
	u_long uBlockSize;
	
	ar >> idBase >> uBlockSize;
//	u_long uMax		= 4096 * 32 + 10240;	// MAX_USER
	u_long uMax		= 2048 * 128 + 20480;	// MAX_USER
	if( uBlockSize > uMax )
	{
		WriteLog( "OnRecharge(): BLOCK SIZE TOO BIG, %d", uBlockSize );
		uBlockSize	= uMax;
	}
	prj.m_objmap.m_idStack.PushIdBlock( idBase, uBlockSize );
	m_uRecharge	= 0;
}

void CDPCoreClient::SendJoin( u_long idPlayer, const char* szPlayer, BOOL bOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_JOIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPlayer );
	ar << bOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWhisper( u_long idFrom, u_long idTo, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WHISPER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idFrom << idTo;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSay( u_long idFrom, u_long idTo, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SAY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idFrom << idTo;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendModifyMode( DWORD dwMode, bool fAdd, u_long idFrom, u_long idTo )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MODIFYMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwMode << fAdd << idFrom << idTo;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendShout( CUser* pUser, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SHOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPartyChat( CUser* pUser, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << GETID( pUser );
	ar << pUser->m_idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendUserPartySkill( u_long uidPlayer, int nMode, DWORD dwSkillTime, int nRemovePoint ,int nCachMode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYSKILLUSE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nMode;
	ar << dwSkillTime;
	ar << nRemovePoint;
	ar << nCachMode;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendGMSay( u_long idPlayer, DWORD dwWorldID, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GMSAY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar << dwWorldID;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPlayMusic( DWORD dwWorldID, u_long idmusic )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYMUSIC, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idmusic;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPlaySound( DWORD dwWorldID, u_long idsound )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYSOUND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idsound;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendKillPlayer( u_long idOperator, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_KILLPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetPlayerAddr( u_long idOperator, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPLAYERADDR, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetPlayerCount( u_long idOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPLAYERCOUNT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetCorePlayer( u_long idOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETCOREPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSystem( const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SYSTEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendCaption( const CHAR* lpString, DWORD dwWorldId, BOOL bSmall )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAPTION, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bSmall;
	ar << dwWorldId;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendFallSnow( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FALLSNOW, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendFallRain( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FALLRAIN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendStopSnow( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_STOPSNOW, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendStopRain( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_STOPRAIN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGuildCombatState( int nState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDCOMBAT_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nState;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendGCRemoveParty( u_long uidPartyid, u_long uidPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTY_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPartyid << uidPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendGCAddParty( u_long idLeader, LONG nLeaderLevel, LONG nLeaderJob, DWORD dwLSex, 
								   u_long idMember, LONG nMemberLevel, LONG nMemberJob, DWORD dwMSex )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTY_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idLeader << nLeaderLevel << nLeaderJob << dwLSex;
	ar << idMember << nMemberLevel << nMemberJob << dwMSex;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnShout( CAr & ar )
{
	char	lpString[1024];
	u_long idPlayer;
	ar >> idPlayer;
	ar.ReadString( lpString, 1024 );

	const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );
	if( lpszPlayer )
	{
		CAr ar1;
		ar1 << NULL_ID << SNAPSHOTTYPE_SHOUT;
		ar1 << NULL_ID;	//GETID( pUser );
		ar1.WriteString( lpszPlayer );
		ar1.WriteString( lpString );
		ar1 << (DWORD)0xffff99cc;
		GETBLOCK( ar1, lpBlock, uBlockSize );
		g_UserMng.AddBlock( lpBlock, uBlockSize );
	}
}

void CDPCoreClient::OnPlayMusic( CAr & ar )
{
	DWORD dwWorldID;
	u_long idmusic;

	ar >> idmusic;
	ar >> dwWorldID;

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYMUSIC << idmusic;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize, pWorld );
}

void CDPCoreClient::OnPlaySound( CAr & ar )
{
	DWORD dwWorldID;
	u_long idsound;

	ar >> idsound;
	ar >> dwWorldID;

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYSOUND;
	arBlock << (BYTE)0;
	arBlock << idsound;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize, pWorld );
}

void CDPCoreClient::OnErrorParty( CAr & ar )
{
	DWORD dw;
	u_long uLeader;
	ar >> dw;
	ar >> uLeader;

	CUser* pUser	= (CUser*)prj.GetUserByID( uLeader );
	if( IsValidObj( (CObj*)pUser ) )
		pUser->AddSendErrorParty( dw );		
}

void CDPCoreClient::OnAddPartyMember( CAr & ar )
{
	const auto [idParty, idLeader, idMember] = ar.Extract<u_long, u_long, u_long>();

	const char * pszLeader = CPlayerDataCenter::GetInstance()->GetPlayerString( idLeader );
	if (!pszLeader) pszLeader = "";

	const char * pszMember = CPlayerDataCenter::GetInstance()->GetPlayerString( idMember );
	if (!pszMember) pszMember = "";
		
	CParty* pParty = g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		if( pParty->NewMember( idMember ) )
		{
			CUser * pMember = nullptr;

			for (CUser * const pUser : AllMembers(*pParty)) {
				pMember = pUser;
				pMember->AddPartyMember(pParty, idMember, pszLeader, pszMember);
			}

			if (pMember != nullptr) {
				pMember->m_idparty = idParty;
			}
		}
		else
		{
			// error
		}
	}
	else
	{
		if( idParty == g_PartyMng.NewParty( idLeader, idMember, idParty ) )
		{
			pParty	= g_PartyMng.GetParty( idParty );
			if( pParty )
			{
				for (CUser * const pMember : AllMembers(*pParty)) {
					pMember->m_idparty	= idParty;
					pMember->AddPartyMember( pParty, idMember, pszLeader, pszMember );
				}
			}
			else
			{
				// error
			}
		}
		else
		{
			// error
		}
	}
}

void CDPCoreClient::OnRemovePartyMember( CAr & ar )
{
	const auto [idParty, idLeader, idMember] = ar.Extract<u_long, u_long, u_long>();

	CParty* pParty = g_PartyMng.GetParty( idParty );
	if (!pParty) {
		Error("OnRemovePartyMember:: Not Party");
		return;
	}

	const char * pszLeader = CPlayerDataCenter::GetInstance()->GetPlayerString( idLeader );
	if (!pszLeader) pszLeader = "";

	const char * pszMember = CPlayerDataCenter::GetInstance()->GetPlayerString( idMember );
	if (!pszMember) pszMember = "";

	if (!pParty->DeleteMember(idMember)) {
		Error("OnRemovePartyMember:: Not DeleteMember");
		return;
	}

	if( pParty->GetSizeofMember() < 2 )
	{
		CUser* pMember	= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( 0 ) );
		if( IsValidObj( pMember ) )
		{
			pMember->AddPartyMember( NULL, 0, pszLeader, pszMember );
			pMember->m_idparty	= 0;
		}
		pMember	= g_UserMng.GetUserByPlayerID( idMember );	// 먼저 삭제됐던 넘의 포인터.
		if( IsValidObj( pMember ) && pMember->m_nDuel )
		{
			CParty* pDuelParty = g_PartyMng.GetParty( pParty->m_idDuelParty );
			if( pDuelParty )
				pDuelParty->DoDuelPartyCancel( pParty );
			else
				Error( "CDPCoreClient::OnRemovePartyMember : 파티멤버 %s의 정보이상. %d %d", pMember->GetName(), pMember->m_idDuelParty, pParty->m_idDuelParty );
		}
		g_PartyMng.DeleteParty( pParty->m_uPartyId );
	}
	else
	{
		// leave
		for (CUser * const pMember : AllMembers(*pParty)) {
			pMember->AddPartyMember(pParty, idMember, pszLeader, pszMember);
		}
	}

	CUser* pRemovd	= g_UserMng.GetUserByPlayerID( idMember ); 
	if( IsValidObj( pRemovd ) )
	{
		pRemovd->m_idparty	= 0;
		pRemovd->AddPartyMember( NULL, idMember, pszLeader, pszMember );
	}
}

void CDPCoreClient::OnAddPlayerParty(CAr & ar) {
	const auto [idParty, idPlayer] = ar.Extract<u_long, u_long>();

	CParty * const pParty = g_PartyMng.GetParty(idParty);
	if (!pParty) return;

	const int i = pParty->FindMember(idPlayer);
	if (i < 0) return;

	pParty->m_aMember[i].m_remove = false;
	pParty->SendSnapshotNoTarget<SNAPSHOTTYPE_SET_PARTY_MEMBER_PARAM, u_long, bool>(idPlayer, false);
}

void CDPCoreClient::OnRemovePlayerParty( CAr & ar )
{
	u_long idParty, idPlayer;
	ar >> idParty >> idPlayer;

	CParty* pParty;

	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int i	= pParty->FindMember( idPlayer );
		if( i < 0 )
			return;

		pParty->m_aMember[i].m_remove = true;
		pParty->SendSnapshotNoTarget<SNAPSHOTTYPE_SET_PARTY_MEMBER_PARAM, u_long, bool>(idPlayer, true);

		if( i == 0 )		// 극단장이 나갈경우
		{
			if( pParty->m_idDuelParty > 0 )
			{
				CParty* pDuelParty	= g_PartyMng.GetParty( pParty->m_idDuelParty );
				if( pDuelParty )
					pParty->DoDuelPartyCancel( pDuelParty );
			}

			bool fRemoveParty	= true;
			for( int j = 1; j < pParty->m_nSizeofMember; j++ )
			{
				if (!pParty->m_aMember[j].m_remove)
				{
					fRemoveParty	= false;
					pParty->SwapPartyMember( 0, j );
					// swap
					break;
				}
			}
			for( int k = 0 ; k < MAX_PARTYMODE ; k++ )
			{
				if( pParty->m_nModeTime[k] )
				{
					if( k == PARTY_PARSKILL_MODE)
						continue;
					pParty->m_nModeTime[k] = 0;
				}
			}

			CMover* pMover = pParty->GetLeader();
			if( pMover )
			{
				if( pMover->IsSMMode( SM_PARTYSKILL1 ) || pMover->IsSMMode( SM_PARTYSKILL15 ) || pMover->IsSMMode( SM_PARTYSKILL30 ) )
					g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
				else
					g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1 );
			}

			if (fRemoveParty) {
				for (CUser * pMember : AllMembers(*pParty)) {
					pMember->m_idparty = 0;
				}
				g_PartyMng.DeleteParty(pParty->m_uPartyId);
			}
		}
	}
}


void CDPCoreClient::OnSetPartyMode( CAr & ar )
{
	u_long uPartyId;
	int nMode;
	BOOL bOnOfff;
	LONG nPoint;
	DWORD	dwSkillTime;
	ar >> uPartyId >> nMode >> dwSkillTime >> bOnOfff;
	if( bOnOfff == TRUE )
		ar >> nPoint;

	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		if( bOnOfff == TRUE )
			pParty->m_nPoint = nPoint;
		// 090917 mirchang - 모드가 파스킬풀 이고 bOnOfff가 FALSE 일땐 극단원중 파스킬풀 아이템 사용중인지 체크하여 사용중인 극단원이 있을땐 다시 모드 설정
		if( nMode == PARTY_PARSKILL_MODE && !bOnOfff )
		{
			for( int j = 0; j < pParty->m_nSizeofMember; ++j )
			{
				if (pParty->m_aMember[j].m_remove) continue;

				CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[j].m_uPlayerId );
				if( IsValidObj( pUser ) && ( pUser->IsSMMode( SM_PARTYSKILL30 ) || pUser->IsSMMode( SM_PARTYSKILL15 ) || pUser->IsSMMode( SM_PARTYSKILL1 ) ) )
				{
					bOnOfff = TRUE;
					g_DPCoreClient.SendUserPartySkill( pUser->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
					break;
				}
			}
		}
		pParty->m_nModeTime[nMode] = bOnOfff;
		
		for (CUser * const pUser : AllMembers(*pParty)) {
			pUser->AddSetPartyMode(nMode, bOnOfff, pParty->m_nPoint, dwSkillTime);
		}
	}
}

void CDPCoreClient::OnPartyChangeItemMode(CAr & ar) {
	const auto [uPartyId, nMode] = ar.Extract<u_long, CParty::ShareItemMode>();

	CParty * const pParty = g_PartyMng.GetParty(uPartyId);
	if (!pParty) return;

	pParty->m_nTroupeShareItem = nMode;
	pParty->SendSnapshotNoTarget<SNAPSHOTTYPE_PARTYCHANGEITEMMODE, CParty::ShareItemMode>(nMode);
}

void CDPCoreClient::OnPartyChangeExpMode(CAr & ar) {
	const auto [uPartyId, nMode] = ar.Extract<u_long, CParty::ShareExpMode>();

	CParty * const pParty = g_PartyMng.GetParty(uPartyId);
	if (!pParty) return;

	pParty->m_nTroupsShareExp = nMode;
	pParty->SendSnapshotNoTarget<SNAPSHOTTYPE_PARTYCHANGEEXPMODE, CParty::ShareExpMode>(nMode);
}


void CDPCoreClient::OnSetPartyExp( CAr & ar )
{
	u_long uPartyId;
	LONG nExp, nPoint, nLevel;
	static_assert(
		   std::is_same_v<decltype(nExp)  , decltype(CParty::m_nExp)>  
		&& std::is_same_v<decltype(nPoint), decltype(CParty::m_nPoint)>
		&& std::is_same_v<decltype(nLevel), decltype(CParty::m_nLevel)>,
		"Mismatch between the field type (which is the one sent) and the "
		"received type in CDPCoreClient::OnSetPartyExp"
		);
	ar >> uPartyId >> nExp >> nPoint >> nLevel;

	CParty* pParty	= g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nExp	= nExp;
		pParty->m_nPoint	= nPoint;
		pParty->m_nLevel	= nLevel;

		for (CUser * const pUser : AllMembers(*pParty)) {
			pUser->AddPartyExpLevel(pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint());
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnRemovePartyPoint( CAr & ar )
{
	u_long uPartyId;
	LONG nPartyPoint;
	static_assert(
		std::is_same_v<decltype(nPartyPoint), decltype(CParty::m_nPoint)>,
		"Mismatch between the field type (which is the one sent) and the "
		"received type in CDPCoreClient::OnRemovePartyPoint"
		);
	ar >> uPartyId;
	ar >> nPartyPoint;
	
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nPoint = nPartyPoint;

		for (CUser * const pUser : AllMembers(*pParty)) {
			pUser->AddPartyExpLevel(pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint());
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnPartyChangeName( CAr & ar )
{
	u_long uidParty;
	TCHAR	sParty[128];

	ar >> uidParty;	
	ar.ReadString( sParty, 128 );
	
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uidParty );
	if( pParty )
	{
		strcpy( pParty->m_sParty, sParty );

		for (CUser * const pUser : AllMembers(*pParty)) {
			pUser->AddPartyChangeName(pParty->m_sParty);
		}
	}
}

void CDPCoreClient::OnPartyChangeTroup( CAr & ar )
{
	u_long uidParty;
	TCHAR	sParty[33];		
	
	ar >> uidParty;	
	ar.ReadString( sParty, 33 );
		
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uidParty );
	if( pParty )
	{
		pParty->m_nKindTroup = 1;
		strcpy( pParty->m_sParty, sParty );

		for (CUser * const pUser : AllMembers(*pParty)) {
			pUser->AddPartyChangeTroup(pParty->m_sParty);
		}
	}
}

void CDPCoreClient::OnAddFriend(CAr & ar) {
	const auto [uidSend, uidFriend] = ar.Extract<u_long, u_long>();

	CUser * pSender = g_UserMng.GetUserByPlayerID(uidSend);
	CUser * pFriend = g_UserMng.GetUserByPlayerID(uidFriend);
	
	const char * lpszFriend = CPlayerDataCenter::GetInstance()->GetPlayerString(uidFriend);
	const char * lpszSend = CPlayerDataCenter::GetInstance()->GetPlayerString(uidSend);
	if (!lpszFriend || !lpszSend) return;

	if (IsValidObj(pSender)) {
		pSender->m_RTMessenger.SetFriend(uidFriend);
		pSender->AddAddFriend(uidFriend, lpszFriend);
	}

	if (IsValidObj(pFriend)) {
		pFriend->m_RTMessenger.SetFriend(uidSend);
		pFriend->AddAddFriend(uidSend, lpszSend);
	}
}

void CDPCoreClient::OnRemovefriend( CAr & ar )
{
	u_long uidSend, uidFriend;
	ar >> uidSend >> uidFriend;
	
	CUser* pSender;

	pSender		= (CUser*)prj.GetUserByID( uidSend );
	if( FALSE == IsValidObj( (CObj*)pSender ) )
		return;

	CHousingMng::GetInstance()->ReqSetAllowVisit( pSender, uidFriend, FALSE );
	pSender->m_RTMessenger.RemoveFriend( uidFriend );
	pSender->AddRemoveFriend( uidFriend );
	CUser* pFriend	= g_UserMng.GetUserByPlayerID( uidFriend );
	if( IsValidObj( pFriend ) )
	{
		CHousingMng::GetInstance()->ReqSetAllowVisit( pFriend, uidSend, FALSE );
		pFriend->m_RTMessenger.RemoveFriend( uidSend );
		pFriend->AddRemoveFriend( uidSend );
	}
}

void CDPCoreClient::OnQueryTickCount( CAr & ar )
{
	DWORD dwTime;
	__int64 nTickCount;
	double dCurrentTime;
	ar >> dwTime >> nTickCount >> dCurrentTime;

	dwTime	= timeGetTime() - dwTime;
	nTickCount	+= ( dwTime * 5000 );	// ( dwTime / 2 ) * 10000

	g_TickCount.Start( nTickCount );
	g_GameTimer.SetCurrentTime( dCurrentTime );
}


void CDPCoreClient::OnEnvironmentEffect( CAr & ar )
{
	ar >> *CEnvironment::GetInstance();

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_ENVIRONMENT;
	arBlock << *CEnvironment::GetInstance();

	if( CEnvironment::GetInstance()->GetEnvironmentEffect() == TRUE )
	{
		arBlock.WriteString( prj.m_EventLua.GetWeatherEventTitle().c_str() );
	}

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize );
}


void CDPCoreClient::OnPartyChat( CAr & ar  )
{
	u_long idParty;
static	\
	CHAR	lpName[MAX_PLAYER], lpString[260], lpSnoopString[512];
	
	OBJID objid;
	ar >> objid;
	ar >> idParty;
	ar.ReadString( lpName, MAX_PLAYER );
	ar.ReadString( lpString, 260 );

	CParty* pParty;
	
	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		for (CUser * const pMember : AllMembers(*pParty)) {
				pMember->AddPartyChat( lpName, lpString, objid );
				if( pMember->m_idSnoop )
				{
					CUser* pSnoop		= g_UserMng.GetUserByPlayerID( pMember->m_idSnoop );
					if( IsValidObj( (CObj*)pSnoop ) )
					{
						sprintf( lpSnoopString, "[p][%s->%s] %s", lpName, pMember->GetName(), lpString );
						pSnoop->AddSnoop( lpSnoopString );
					}
				}
		}
	}
}

void CDPCoreClient::SendAddFriendNameReqest( u_long uLeaderid, LONG nLeaderJob, BYTE nLeaderSex, u_long uMember, const char * szLeaderName, const char * szMemberName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDFRIENDNAMEREQEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uLeaderid << uMember;
	ar << nLeaderJob << nLeaderSex;
	ar.WriteString( szLeaderName );
	ar.WriteString( szMemberName );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, const char* szGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CREATE_GUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nSize;
	ar.Write( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar.WriteString( szGuild );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGuildChat( CUser* pUser, const char* sChat )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->GetId();
	ar << pUser->m_idPlayer;
	ar.WriteString( sChat );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnCreateGuild( CAr & ar )
{
	int nSize;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];
	u_long idGuild;
	char szGuild[MAX_G_NAME];

	ar >> nSize;
	ar.Read( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar >> idGuild;
	ar.ReadString( szGuild, MAX_G_NAME );

	int	cb	= 0;
	CUser* apUser[MAX_PTMEMBER_SIZE];

	CGuild* pGuild	= new CGuild;
	pGuild->m_idGuild	= idGuild;
	lstrcpy( pGuild->m_szGuild, szGuild );
	pGuild->m_idMaster		= info[0].idPlayer;
	pGuild->m_aPower[GUD_MASTER].SetAll();
	idGuild	= g_GuildMng.AddGuild( pGuild );
	if( idGuild > 0 )
	{
		// master
		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= info[0].idPlayer;
		pMember->m_nMemberLv	= GUD_MASTER;
		if( pGuild->AddMember( pMember ) )
		{
			CUser* pUser	= (CUser*)prj.GetUserByID( info[0].idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idGuild	= idGuild;
				apUser[cb++]	= pUser;
			}
			// member
			for( int i = 1; i < nSize; i++ )
			{
				pMember		= new CGuildMember;
				pMember->m_idPlayer		= info[i].idPlayer;
				pMember->m_nMemberLv	= GUD_ROOKIE;
				if( pGuild->AddMember( pMember ) )
				{
					pUser	= (CUser*)prj.GetUserByID( info[i].idPlayer );
					if( IsValidObj( pUser ) )
					{
						pUser->m_idGuild	= idGuild;
						apUser[cb++]	= pUser;
					}
				}
				else
				{
					info[i].idPlayer	= 0;
					SAFE_DELETE( pMember );
				}
			}
		}
		else
		{
			SAFE_DELETE( pMember );
			g_GuildMng.RemoveGuild( idGuild );
		}
	}
	else
	{
		SAFE_DELETE( pGuild );
	}

	if( cb > 0 )
	{
		g_UserMng.AddCreateGuild( info[0].idPlayer, CPlayerDataCenter::GetInstance()->GetPlayerString( info[0].idPlayer ), idGuild, szGuild );	// g_UserMng.Lock();
	}

	for( int i = 0; i < cb; i++ )
	{
		g_UserMng.AddSetGuild( apUser[i], idGuild );
		apUser[i]->AddMyGuild();
	}
}

void CDPCoreClient::OnDestroyGuild( CAr & ar )
{
	u_long idGuild;
	ar >> idGuild;

	int nSize = 0;
	CUser* pUserIk3[ MAX_GM_SIZE ];

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CUser* pUsertmp;
		CGuildMember* pMember;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;

			pUsertmp	= prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj( pUsertmp ) )
			{
				pUsertmp->SetRestPoint( 0 );
				GuildHouseMng->ResetApplyDST( pUsertmp );
				pUsertmp->m_idGuild		= 0;
				g_UserMng.AddSetGuild( pUsertmp, 0 );
				pUserIk3[nSize] = pUsertmp;
				++nSize;				
			}
		}

		const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pGuild->m_idMaster );
		/*
		if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_WAR )
		{
			CUser* pUserMaster = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
			CSecretRoomMng::GetInstance()->SetFailGuild( pUserMaster );
		}
		*/
		
		g_GuildMng.RemoveGuild( pGuild->m_idGuild );
		g_UserMng.AddDestroyGuild( lpszPlayer, idGuild );	// g_UserMng.Lock();

		for( int cou = 0 ; cou < nSize ; ++cou )
		{
			if( IsValidObj( pUserIk3[cou] ) )
			{
				pUserIk3[cou]->RemoveItemIK3( IK3_CLOAK );
				pUserIk3[cou]->m_tGuildMember = CTime::GetCurrentTime();
				pUserIk3[cou]->m_tGuildMember += CTimeSpan( 2, 0, 0, 0 );
				////////////////
				// 康
				if( pUserIk3[cou]->GetWorld() )
				{
					////
					const REGIONELEM * pRgnElem	= NULL;
					DWORD dwWorldID	= pUserIk3[cou]->GetWorld()->GetID();
					CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
					int nId	= pProcessor->PtInQuestRect( WI_WORLD_MADRIGAL, pUserIk3[cou]->GetPos() );
					if( nId > -1 )
					{
						const GroupQuest::QuestElem * pElem	= pProcessor->GetQuest( nId );
						if( !pElem || pElem->idGroup != pUserIk3[cou]->m_idGuild )
						{
							pRgnElem = g_WorldMng.GetRevival(pUserIk3[cou]);
						}
					}
					////
					if (pRgnElem)
						pUserIk3[cou]->Replace(*pRgnElem, REPLACE_NORMAL, nRevivalLayer);
				}
				////////////////
			}
		}	
		return;
	}
}

void CDPCoreClient::OnAddGuildMember( CAr & ar )
{
	u_long idGuild;
//	ar >> idPlayer >> idGuild;
	GUILD_MEMBER_INFO info;
	ar.Read( &info, sizeof(GUILD_MEMBER_INFO) );
	ar >> idGuild;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->GetSize() >= MAX_GM_SIZE )
			return;

		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= info.idPlayer;
		pMember->m_nMemberLv	= GUD_ROOKIE;
		if( pGuild->AddMember( pMember ) )
		{
			CUser* pUser	= g_UserMng.GetUserByPlayerID( info.idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idGuild	= idGuild;
				g_UserMng.AddSetGuild( pUser, idGuild );
				pUser->AddGuildHouseAllInfo( GuildHouseMng->GetGuildHouse( idGuild ) );
				GuildHouseMng->SetApplyDST( pUser );
			}
		}
		else
		{
			SAFE_DELETE( pMember );
		}
	}
}

void CDPCoreClient::OnRemoveGuildMember( CAr & ar )
{
	u_long idPlayer, idGuild;
	ar >> idPlayer >> idGuild;

	BOOL bRemove = FALSE;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	CUser* pUser;
	if( pGuild )
	{
		if( pGuild->RemoveMember( idPlayer ) )
		{
			pUser	= (CUser*)prj.GetUserByID( idPlayer );
			if( IsValidObj(pUser) )
			{
				pUser->SetRestPoint( 0 );
				GuildHouseMng->ResetApplyDST( pUser );
				GuildHouseMng->GoOutGuildHouse( pUser );
				pUser->m_idGuild	= 0;
				g_UserMng.AddSetGuild( pUser, 0 );
				bRemove = TRUE;
			}
		}
	}

	if( bRemove )
	{
		if( IsValidObj(pUser) )
		{
			pUser->RemoveItemIK3( IK3_CLOAK );
			pUser->m_tGuildMember = CTime::GetCurrentTime();
			pUser->m_tGuildMember += CTimeSpan( 2, 0, 0, 0 );
			// 康
			if( pUser->GetWorld() )
			{
				////
				const REGIONELEM * pRgnElem	= NULL;
				DWORD dwWorldID	= pUser->GetWorld()->GetID();
				CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
				const int nId	= pProcessor->PtInQuestRect( WI_WORLD_MADRIGAL, pUser->GetPos() );
				if( nId > -1 )
				{
					GroupQuest::QuestElem * pElem	= pProcessor->GetQuest( nId );
					if( !pElem || pElem->idGroup != pUser->m_idGuild )
					{
						pRgnElem = g_WorldMng.GetRevival(pUser);
					}
				}
				////
				if (pRgnElem)
					pUser->Replace(*pRgnElem, REPLACE_NORMAL, nRevivalLayer);
			}
		}
	}
}

void CDPCoreClient::OnGuildMemberLv( CAr & ar )
{
	u_long idPlayer, idGuild;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
		{
			pMember->m_nMemberLv	= nMemberLv;
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildClass( CAr & ar )
{
	u_long idPlayer, idGuild;
	int nClass;
	ar >> idPlayer >> nClass >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			pMember->m_nClass	= nClass;
	}
}

void CDPCoreClient::OnGuildNickName( CAr & ar )
{
	u_long idPlayer, idGuild;
	char strNickName[MAX_GM_ALIAS] = {0,};
	ar >> idPlayer >> idGuild;
	ar.ReadString( strNickName, MAX_GM_ALIAS );
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			strcpy( pMember->m_szAlias, strNickName );
	}
}

void CDPCoreClient::OnChgMaster( CAr & ar )
{
	u_long idPlayer, idPlayer2, idGuild;
	ar >> idPlayer >> idPlayer2 >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->m_idMaster	= idPlayer2;
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_ROOKIE;
			pMember->m_nClass = 0;
		}
		pMember	= pGuild->GetMember( idPlayer2 );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_MASTER;
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildMemberLogOut( CAr & ar )
{
	u_long idPlayer, idGuild;
	ar >> idGuild >> idPlayer;
}

void CDPCoreClient::SendWarDead( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_DEAD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWarMasterAbsent(WarId idWar, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_MASTER_ABSENT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << bDecl;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWarTimeout(WarId idWar )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_TIMEOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnAcptWar( CAr & ar )
{
	WarId idWar;
	u_long idDecl, idAcpt;
	ar >> idWar >> idDecl >> idAcpt;

	CGuild* pDecl	= g_GuildMng.GetGuild( idDecl );
	CGuild* pAcpt	= g_GuildMng.GetGuild( idAcpt );

	if( !pDecl || !pAcpt )
		return;

	CGuildWar* pWar		= new CGuildWar;
	pWar->m_idWar	= idWar;
	pWar->m_Decl.idGuild	= pDecl->m_idGuild;
	pWar->m_Decl.nSize	= pDecl->GetSize();
	pWar->m_Acpt.idGuild	= pAcpt->m_idGuild;
	pWar->m_Acpt.nSize	= pAcpt->GetSize();
	pWar->m_nFlag	= WF_WARTIME;
	pWar->m_time	= CTime::GetCurrentTime();

	idWar	= g_GuildWarMng.AddWar( pWar );
	if( idWar != WarIdNone )
	{
		pDecl->m_idWar	= idWar;
		pDecl->m_idEnemyGuild	= pAcpt->m_idGuild;
		pAcpt->m_idWar	= idWar;
		pAcpt->m_idEnemyGuild	= pDecl->m_idGuild;

		CUser* pUser;
		for( auto i = pDecl->m_mapPMember.begin(); i != pDecl->m_mapPMember.end(); ++i )
		{
			pUser	= prj.GetUserByID( i->second->m_idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idWar	= idWar;
				g_UserMng.AddSetWar( pUser, idWar );
			}
		}
		for( auto i = pAcpt->m_mapPMember.begin(); i != pAcpt->m_mapPMember.end(); ++i )
		{
			pUser	= prj.GetUserByID( i->second->m_idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idWar	= idWar;
				g_UserMng.AddSetWar( pUser, idWar );
			}
		}
	}
	else
	{
		Error( "OnAcptWar()" );
		SAFE_DELETE( pWar );
	}
}

void CDPCoreClient::OnSurrender( CAr & ar )
{
	WarId idWar;
	u_long idPlayer;
	BOOL bDecl;
	ar >> idWar >> idPlayer >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		CGuild* pGuild;
		if( bDecl )
		{
			pWar->m_Decl.nSurrender++;
			pGuild	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
		}
		else
		{
			pWar->m_Acpt.nSurrender++;
			pGuild	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
		}
		if( pGuild )
		{
			CGuildMember* pMember	= pGuild->GetMember( idPlayer );
			if( pMember )
				pMember->m_nSurrender++;
		}
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( IsValidObj( (CObj*)pUser ) )
		{
			pUser->m_idWar	= WarIdNone;
			g_UserMng.AddSetWar( pUser, WarIdNone );
		}
	}
}

void CDPCoreClient::OnWarDead( CAr & ar )
{
	WarId idWar;
	BOOL bDecl;
	ar >> idWar >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		if( bDecl )
			pWar->m_Decl.nDead++;
		else
			pWar->m_Acpt.nDead++;
	}
}

void CDPCoreClient::OnWarEnd( CAr & ar )
{
	WarId idWar;
	int nWptDecl, nWptAcpt;
	int nType;
	ar >> idWar >> nWptDecl >> nWptAcpt >> nType;
	

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
		return;
	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;
	g_GuildWarMng.Result( pWar, pDecl, pAcpt, nType, nWptDecl, nWptAcpt );
}

void CDPCoreClient::OnGuildLogoACK( CAr & ar )
{
/*  // 시야안의 유저에게 로고가 변경됨을 알린다.
	u_long idGuild;
	DWORD dwLogo;

	ar >> idGuild >> dwLogo;
	TRACE("PACKETTYPE_CW_GUILDLOGO - GUILD:%d LOGO:%d\n", idGuild, dwLogo);

	prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );	// lock2

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->SetLogo( dwLogo );

		CUser* pUsertmp;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( pUsertmp )
				g_UserMng.AddSetLogo( pUsertmp, dwLogo );
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );	// unlock2
	prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1
*/
	// 전체유저에게 로고가 변경됨을 알린다.
	u_long idGuild;
	DWORD dwLogo;

	ar >> idGuild >> dwLogo;
	TRACE("PACKETTYPE_CW_GUILDLOGO - GUILD:%d LOGO:%d\n", idGuild, dwLogo);

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
		pGuild->SetLogo( dwLogo );
	g_UserMng.AddSetLogo( idGuild, dwLogo );	// g_UserMng와 교착 상태를 피하기 위해서 unlock된후에 한다.
}

void CDPCoreClient::OnGuildContributionACK( CAr & ar )
{
	CONTRIBUTION_CHANGED_INFO info;
	ULONG uServerID;

	ar >> uServerID;
	ar >> info;

	TRACE("PACKETTYPE_CW_GUILDCONTRIBUTION GUILD:%d idPlayer:%d dwPxpCount:%d dwPenya:%d " 
          "dwGuildPxpCount:%d dwGuildPenya:%d nGuildLevel:%d\n", 
		info.idGuild, info.idPlayer, info.dwPxpCount, info.dwPenya, 
		info.dwGuildPxpCount, info.dwGuildPenya, info.nGuildLevel);

	CGuild* pGuild	= g_GuildMng.GetGuild( info.idGuild );
	if( pGuild )
	{
		if( uServerID != ::g_uKey )		// 이 메세지의 원본서버가 아닌 경우만 update
			pGuild->SetContribution( info );	

		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			CGuildMember * pMember		= i->second;
			CUser * pUser	= prj.GetUserByID( pMember->m_idPlayer );
		
			if( IsValidObj( pUser ) )
				pUser->AddContribution( info );
		}
	}
}

void CDPCoreClient::OnModifyVote( CAr & ar )
{
	u_long idVote, idGuild;
	BYTE cbOperation;
	BYTE cbExtra;

	ar >> idGuild;
	ar >> idVote;
	ar >> cbOperation;
	ar >> cbExtra;

	TRACE("PACKETTYPE_CW_MODIFYVOTE guild:%d vote:%d case:%d extra:%d\n", idGuild, idVote, cbOperation, cbExtra );

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->ModifyVote( idVote, cbOperation, cbExtra ) )
		{
			CUser* pUser;
			CGuildMember* pMember;
			for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUser ) ) 
					pUser->AddModifyVote( idVote, cbOperation, cbExtra );
			}
		}
	}
}

void CDPCoreClient::OnAddVoteResultACk( CAr & ar )
{
	VOTE_INSERTED_INFO	info;
	ar >> info;

	TRACE("PACKETTYPE_CW_ADDVOTERESULT - GUILD:%d VOTE:%d\n",  info.idGuild, info.idVote);
	if( info.idVote == 0 )
		return;

	CGuild* pGuild	= g_GuildMng.GetGuild( info.idGuild );
	if( pGuild )
	{
		CUser* pUser;
		if( info.idVote )
		{
			pGuild->AddVote( info );

			CGuildMember* pMember;
			for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUser	= prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUser ) ) 
					pUser->AddInsertedVote( info );
			}
		}
		else
		{
			// 마스터에게 실패를 알린다.
			pUser	= prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pUser ) ) 
				pUser->AddInsertedVote( info );
		}
	}
}

void CDPCoreClient::OnGuildNoticeACk( CAr & ar )
{
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];

	ar >> idGuild;
	ar.ReadString( szNotice, MAX_BYTE_NOTICE );
	TRACE("PACKETTYPE_CW_GUILDNOTICE - GUILD:%d Notice:%s\n", idGuild, szNotice);

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->SetNotice( szNotice );

		CUser* pUser;
		CGuildMember* pMember;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
				pUser->AddSetNotice( idGuild, szNotice );
		}
	}
}

void CDPCoreClient::OnGuildAuthority(CAr & ar) {
	const auto [uGuildId, dwAuthority] = ar.Extract<u_long, GuildPowerss>();

	CGuild * const pGuild = g_GuildMng.GetGuild(uGuildId);
	if (!pGuild) return;

	pGuild->m_aPower = dwAuthority;
	pGuild->SendSnapshotNoTarget<SNAPSHOTTYPE_GUILD_AUTHORITY, GuildPowerss>(dwAuthority);
}

void CDPCoreClient::OnGuildPenya(CAr & ar) {
	const auto [uGuildId, dwType, dwPenya] = ar.Extract<u_long, DWORD, DWORD>();

	CGuild * pGuild = g_GuildMng.GetGuild(uGuildId);
	if (!pGuild) return;

	pGuild->m_aPenya[dwType] = dwPenya;

	pGuild->SendSnapshotNoTarget<SNAPSHOTTYPE_GUILD_PENYA, DWORD, DWORD>(dwType, dwPenya);
}

void CDPCoreClient::OnGuildRealPenya( CAr & ar )
{
	u_long uGuildId;
	int nGoldGuild;
	ar >> uGuildId >> nGoldGuild;

	CGuild* pGuild	= g_GuildMng.GetGuild( uGuildId );
	if( pGuild )
	{
		if( !(g_eLocal.GetState( ENABLE_GUILD_INVENTORY )) )
		{
			pGuild->m_nGoldGuild = nGoldGuild;
		}

		CUser* pUser;
		CGuildMember* pMember;
		for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
			{
				pUser->AddGuildRealPenya( nGoldGuild, pMember->m_nMemberLv );
			}				
		}
	}
}


// raiders_test 유저가 아이템을 사용하고 나가면?
void CDPCoreClient::OnGuildSetName( CAr & ar )
{
	u_long idGuild;
	char lpszGuild[MAX_G_NAME];
	
	ar >> idGuild;
	ar.ReadString( lpszGuild, MAX_G_NAME );
	u_long idPlayer;
	BYTE nId, nError;
	ar >> idPlayer >> nId >> nError;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( nError == 0 )	// no error
			g_GuildMng.SetName( pGuild, lpszGuild );
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( IsValidObj( (CObj*)pUser ) )
		{
			CItemElem* pItemElem;
#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
			{
				pItemElem	= pUser->GetItemId( nId );
				if( IsUsableItem( pItemElem ) )
					pItemElem->m_bQuery	= FALSE;
			}
			switch( nError )
			{
				case 1:
					// error message	// have not guild
					pUser->AddDefinedText( TID_GAME_COMNOHAVECOM );
					break;
				case 2:
					// error message	// is not kingpin
					pUser->AddDefinedText( TID_GAME_COMDELNOTKINGPIN );
					break;
				case 3:
					// error message	// already exists
					pUser->AddDefinedText( TID_GAME_COMOVERLAPNAME );
					break;
				case 0:
#ifdef __S_SERVER_UNIFY
					if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
					{
						if( IsUsableItem( pItemElem ) )
							pUser->UpdateItem( *pItemElem, UI::Num::ConsumeOne );
					}
					break;
			}
#ifdef __S_SERVER_UNIFY
			pUser->SetAllAction();
#endif // __S_SERVER_UNIFY
		}
	}
}

void CDPCoreClient::OnGuildMsgControl( CAr & ar )
{
	if ( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
	{
		GUILD_MSG_HEADER	Header;
		DWORD				dwPenya;
		BYTE				cbCloak;

		ar.Read( &Header, sizeof(GUILD_MSG_HEADER));
		ar >> dwPenya;
		ar >> cbCloak;		// 망토의 경우 

		CGuild* pGuild	= g_GuildMng.GetGuild( Header.HeadASub );
		if( pGuild )
		{
			CGuildMember*	pMember;
			CUser*			pUsertmp;
			for( auto i = pGuild->m_mapPMember.begin();	i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) ) 
				{
					pUsertmp->AddGetGoldGuildBank( dwPenya, 2, pMember->m_idPlayer, cbCloak );	// 2는 업데이트 해야할 클라이게
				}
			}
		}
	}
}

bool CDPCoreClient::Contribute(const CUser & pUser, const DWORD dwPxpCount, const DWORD dwPenya )
{
	const u_long idGuild = pUser.m_idGuild;
	const u_long idPlayer = pUser.m_idPlayer;

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if (!pGuild) return false;

	CGuildMember* pGuildMember = pGuild->GetMember( idPlayer );
	if (!pGuildMember) return false;
		

	const int nLastGuildLv = pGuild->m_nLevel;
	if (!pGuild->AddContribution(dwPxpCount, dwPenya, idPlayer)) return false;

	CONTRIBUTION_CHANGED_INFO info;

	info.idGuild		= idGuild;
	info.idPlayer		= idPlayer;
	info.dwPxpCount		= dwPxpCount;
	info.dwPenya		= dwPenya;
	info.dwGuildPxpCount= pGuild->m_dwContributionPxp;
	info.dwGuildPenya	= pGuild->m_nGoldGuild;
	info.nGuildLevel	= pGuild->m_nLevel;

	PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if (pPlayerData)
		g_dpDBClient.SendGuildContribution(info, (nLastGuildLv < pGuild->m_nLevel ? 1 : 0), pPlayerData->data.nLevel);

	{
		BEFORESENDDUAL( ar, PACKETTYPE_WC_GUILDCONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << ::g_uKey;
		ar << info;
		SEND( ar, this, DPID_SERVERPLAYER );
	}

	return true;
}


void CDPCoreClient::SendGuildStatLogo(CUser * pUser, DWORD data) {
	BEFORESENDDUAL(ar, PACKETTYPE_WC_GUILDLOGO, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->m_idGuild << pUser->m_idPlayer << data;
	SEND(ar, this, DPID_SERVERPLAYER);
}

bool CDPCoreClient::SendGuildStatPenya(CUser * pUser, DWORD data) {
	return Contribute(*pUser, 0, data);
}

bool CDPCoreClient::SendGuildStatPxp(CUser * pUser, DWORD data) {
	return Contribute(*pUser, data, 0);
}

void CDPCoreClient::SendGuildStatNotice(CUser * pUser, const char * notice) {
	BEFORESENDDUAL(ar, PACKETTYPE_WC_GUILDNOTICE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->m_idGuild << pUser->m_idPlayer;

	char szNotice[MAX_BYTE_NOTICE];
	std::strncpy(szNotice, notice, MAX_BYTE_NOTICE);
	szNotice[MAX_BYTE_NOTICE - 1] = '\0';

	ar.WriteString(szNotice);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendGuildGetPay( u_long uGuildId, DWORD nGoldGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_DB_REALPENYA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId << nGoldGuild;
	SEND( ar, this, DPID_SERVERPLAYER );		
}

void CDPCoreClient::OnSetFriendState(CAr & ar) {
	const auto [uidPlayer, dwState] = ar.Extract<u_long, FriendStatus>();

	CUser * pUser = prj.GetUserByID(uidPlayer);
	if (IsValidObj(pUser)) {
		pUser->m_RTMessenger.SetState(dwState);
	}
}

void CDPCoreClient::OnFriendInterceptState( CAr & ar )
{
	u_long uidPlayer;
	u_long uidFriend;
	ar >> uidPlayer >> uidFriend;
	
	CUser * pUser = prj.GetUserByID( uidPlayer );

	if (!IsValidObj(pUser)) return;

	Friend * pFriend = pUser->m_RTMessenger.GetFriend(uidFriend);
	if (!pFriend) return;
		
	if (pFriend->bBlock) {
		pFriend->bBlock = FALSE;

		CUser * pUserFriend = prj.GetUserByID(uidFriend);
		if (IsValidObj(pUserFriend))
			pFriend->dwState = pUserFriend->m_RTMessenger.GetState();
		else
			pFriend->dwState = FriendStatus::OFFLINE;
	} else {
		pFriend->bBlock = TRUE;
		pFriend->dwState = FriendStatus::ONLINE;
	}
}


void CDPCoreClient::OnPartyChangeLeader(CAr & ar) {
	const auto [uPartyId, idChangeLeader] = ar.Extract<u_long, u_long>();

	CParty * const pParty = g_PartyMng.GetParty(uPartyId);
	if (!pParty) return;

	pParty->ChangeLeader(idChangeLeader);
	pParty->SendSnapshotNoTarget<SNAPSHOTTYPE_ADDPARTYCHANGELEADER, u_long>(idChangeLeader);
}

void CDPCoreClient::OnGameRate( CAr & ar )
{
	FLOAT fRate;
	BYTE nFlag;
	ar >> fRate;
	ar >> nFlag;
	switch( nFlag )
	{
	case GAME_RATE_ITEMDROP:
		{
			prj.m_fItemDropRate = fRate;
		}
		break;
	case GAME_RATE_GOLDDROP:
		{
			prj.m_fGoldDropRate = fRate;
		}
		break;
	case GAME_RATE_MONSTEREXP:
		{
			prj.m_fMonsterExpRate = fRate;
		}
		break;
	case GAME_RATE_MONSTERHIT:
		{
			prj.m_fMonsterHitRate = fRate;
		}
		break;
	case GAME_RATE_X3:
		{
			prj.m_fItemDropRate = fRate;
			prj.m_fGoldDropRate = fRate;
			prj.m_fMonsterExpRate = fRate;
		}
		break;
	}
}

void CDPCoreClient::OnLoadConstant( CAr & ar )
{
	CProject::LoadConstant( "Constant.inc" );
	g_UserMng.AddGameSetting();
}

void CDPCoreClient::OnSetMonsterRespawn( CAr & ar )
{
	u_long uidPlayer;
	DWORD dwMonsterID;
	DWORD dwRespawnNum;
	DWORD dwAttackNum;
	DWORD dwRect;
	DWORD dwRespawnTime;
	BOOL  bFlying;
	ar >> uidPlayer;
	ar >> dwMonsterID >> dwRespawnNum >> dwAttackNum >> dwRect >> dwRespawnTime;
	ar >> bFlying;

	D3DXVECTOR3 vPos;
	CWorld* pWorld = NULL;
	
	CUser* pUser	= (CUser*)prj.GetUserByID( uidPlayer );
	if( IsValidObj( pUser ) )
	{
		vPos = pUser->GetPos();
		pWorld = pUser->GetWorld();
	}

	if( pWorld != NULL )
	{
		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = dwMonsterID;
		ri.m_cb = dwRespawnNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if( bFlying )
			ri.m_vPos = vPos;
		ri.m_rect.left		= (LONG)( vPos.x - dwRect );
		ri.m_rect.right		= (LONG)( vPos.x + dwRect );
		ri.m_rect.top		= (LONG)( vPos.z - dwRect );
		ri.m_rect.bottom	= (LONG)( vPos.z + dwRect );
		ri.m_uTime			= (u_short)( dwRespawnTime );
		ri.m_cbTime = 0;
		
		pWorld->m_respawner.AddScriptSpawn(ri);
		
		char chMessage[512] = {0,};
		sprintf( chMessage, "Add Respwan MonsterID = %d(%d/%d) Rect(%d, %d, %d, %d) Time = %d", 
			ri.m_dwIndex, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
		pUser->AddText( chMessage );
		
	}
}


// 코어서버로부터 리스트를 받을 때 
void CDPCoreClient::OnCWWantedList(CAr & ar) {
	ar >> CWantedListSnapshot::GetInstance();
}

// 코어서버로 부터 현상금을 받을 때 
void CDPCoreClient::OnCWWantedReward( CAr & ar )
{
	u_long		idPlayer, idAttacker;
	__int64		nGold;
	DWORD		dwWorldID;
	D3DXVECTOR3 vPos;

	ar >> idPlayer >> idAttacker >> nGold >> dwWorldID >> vPos;
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
	if( !pWorld )
	{
		Error( "OnCWWantedReward cannot found world.\n", dwWorldID );
		return;
	}

	int nDrop = 0;
	while( nGold > 0 )
	{
		if( nGold > MAX_GOLD_REWARD )
			nDrop = MAX_GOLD_REWARD;
		else
			nDrop = (int)( nGold );
		
		nGold -= nDrop;

		CItemElem* pItemElem	= new CItemElem;
		pItemElem->m_dwItemId	= II_GOLD_REWARD;
		pItemElem->m_nItemNum	= 0;					// 보통 돈 계열에서 사용되는 변수지만 II_GOLD_REWARD는 사용하지 않는다.
		pItemElem->m_nHitPoint  = nDrop;				// II_GOLD_REWARD는 여기서 사용한다. 

		CItem* pItem			= new CItem;
		pItem->m_pItemBase		= pItemElem;
		pItem->SetIndex( pItemElem->m_dwItemId );
		pItem->SetPos( vPos );
		pItem->SetAngle( (float)xRandom( 360 ) );
		pWorld->ADDOBJ( pItem, TRUE, nLayer );
	}
}

// 코어서버에 현상금을 누적 요청
void CDPCoreClient::SendWCWantedGold( LPCTSTR szPlayer, u_long idPlayer, int nGold, LPCTSTR szMsg )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WC_WANTED_GOLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szPlayer );
	ar << idPlayer << nGold;
	ar.WriteString( szMsg );

	SEND( ar, this, DPID_SERVERPLAYER );
}

// 코어서버에 (현상금얻기 & 현상금 clear) 요청  
#ifdef __LAYER_1015
void CDPCoreClient::SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos, int nLayer )
#else	// __LAYER_1015
void CDPCoreClient::SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos )
#endif	// __LAYER_1015
{
	BEFORESENDDUAL( ar, PACKETTYPE_WC_WANTED_REWARD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idAttacker << dwWorldID << vPos;
	ar.WriteString( szFormat );
#ifdef __LAYER_1015
	ar << nLayer;
#endif	// __LAYER_1015
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSetPartyDuel( u_long idParty1, u_long idParty2, BOOL bDuel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYDUEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty1 << idParty2 << bDuel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

// raiders_test 사용하고 나가면?
void CDPCoreClient::OnSetPlayerName( CAr& ar )
{
	u_long idPlayer;
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	DWORD dwData;
	BOOL f;

	ar >> idPlayer;
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> dwData >> f;
	WORD wId	= LOWORD( dwData );
	WORD wMode	= HIWORD( dwData );

	char lpOutputString[256]	= { 0, };
	sprintf( lpOutputString, "WORLDSERVER.EXE\t// %d\t// %s\t// %08x\t// %d", idPlayer, lpszPlayer, dwData, f );
	OutputDebugString( lpOutputString );

	CUser* pUser	= (CUser*)prj.GetUserByID( idPlayer );
	if( IsValidObj( (CObj*)pUser ) )
	{
		if( f )
		{
			if( wMode )
			{
				pUser->SetNotMode( QUERYSETPLAYERNAME_MODE );
				g_UserMng.AddModifyMode( pUser );
			}
			pUser->SetName( lpszPlayer );

#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
			{
				CItemElem* pItemElem	= pUser->GetItemId( wId );
				if( pItemElem )
				{
					ASSERT( pItemElem->m_bQuery );
					pItemElem->m_bQuery	= FALSE;
					pUser->UpdateItem(*pItemElem, UI::Num::ConsumeOne);
				}
			}
		}
		else
		{
#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
			{
				if( (int)wId >= 0 )
				{
					CItemElem* pItemElem	= pUser->GetItemId( wId );
					if( pItemElem )
					{
						ASSERT( pItemElem->m_bQuery );
						pItemElem->m_bQuery	= FALSE;
					}
				}
			}
			pUser->AddDefinedText( TID_DIAG_0034 );
		}
#ifdef __S_SERVER_UNIFY
		pUser->SetAllAction();
#endif // __S_SERVER_UNIFY
	}

	if( f )
	{
		char lpOutputString[128]	= { 0, };
		sprintf( lpOutputString, "WORLDSERVER.EXE\tPACKETTYPE_QUERYSETPLAYERNAME\t//%d:%s\t-%08x", idPlayer, lpszPlayer, dwData );
		OutputDebugString( lpOutputString );
		CPlayerDataCenter*	pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
		PlayerData* pPlayerData	= pPlayerDataCenter->GetPlayerData( idPlayer );
		if( pPlayerData )
		{
			PlayerData pd;
			memcpy( &pd, pPlayerData, sizeof(PlayerData) );
			pPlayerDataCenter->DeletePlayerData( idPlayer );
			lstrcpy( pd.szPlayer, lpszPlayer );
			pPlayerDataCenter->AddPlayerData( idPlayer, pd );
		}
	}
}

void CDPCoreClient::SendQuerySetGuildName( u_long idPlayer, u_long idGuild, const char* lpszPlayer, BYTE nId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETGUILDNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idGuild;
	ar.WriteString( lpszPlayer );
	ar << nId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idSnoop << bRelease;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendChat( u_long idPlayer1, u_long idPlayer2, const char* lpszChat )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer1 << idPlayer2;
	ar.WriteString( lpszChat );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnSetSnoop( CAr & ar )
{
//	OutputDebugString( "WORLDSERVER.EXE\t// PACKETTYPE_SETSNOOP" );
	u_long idPlayer, idSnoop;
	BOOL bRelease;
	ar >> idPlayer >> idSnoop >> bRelease;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( IsValidObj( (CObj*)pUser ) )
		pUser->m_idSnoop	= idSnoop;
}

void CDPCoreClient::SendSetSnoopGuild( u_long idGuild, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOPGUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << bRelease;
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __EVENT0913
void CDPCoreClient::OnEvent0913( CAr & ar, DPID, DPID, DPID )
{
	BOOL f;
	ar >> f;
	if( f )
	{
		prj.m_fItemDropRate		*= 2;
		if( g_eLocal.SetState( EVE_EVENT0913, 1 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT0913, 1 );
	}
	else
	{
		prj.m_fItemDropRate		/= 2;
		if( g_eLocal.ClearEvent( EVE_EVENT0913 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT0913, 0 );
	}
	Error( "OnEvent0913-%d, %f", g_eLocal.GetState( EVE_EVENT0913 ), prj.m_fItemDropRate );
}
#endif	// __EVENT0913

#ifdef __EVENT1206
void CDPCoreClient::OnEvent1206( CAr & ar, DPID, DPID, DPID )
{
	BOOL f;
	ar >> f;
	if( f )
	{
		if( g_eLocal.SetState( EVE_EVENT1206, 1 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT1206, 1 );
	}
	else
	{
		if( g_eLocal.ClearEvent( EVE_EVENT1206 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT1206, 0 );
	}
	Error( "OnEvent1206-%d", g_eLocal.GetState( EVE_EVENT1206 ) );
}
#endif	// __EVENT1206

void CDPCoreClient::OnEvent( CAr & ar )
{
	DWORD dwEvent;
	ar >> dwEvent;
	// xmas
	if( dwEvent & EVE_FLAG1219 )
	{
		if( g_eLocal.GetState( EVE_EVENT1219 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT1219, 1 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT1219, 1 );
				Error( "OnEvent-Xmas:1" );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT1219 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT1219 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT1219, 0 );
				Error( "OnEvent-Xmas:0" );
			}
		}
	}
	//
	if( dwEvent & EVE_FLAG0127 )
	{
		if( g_eLocal.GetState( EVE_EVENT0127 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT0127, 1 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0127, 1 );
				Error( "OnEvent-NewYear:1" );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT0127 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT0127 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0127, 0 );
				Error( "OnEvent-NewYear:0" );
			}
		}
	}
	//
	if( dwEvent & EVE_FLAG0214 )
	{
		if( g_eLocal.GetState( EVE_EVENT0214 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT0214, 1 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0214, 1 );
				Error( "OnEvent-ValentinesDay:1" );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT0214 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT0214 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0214, 0 );
				Error( "OnEvent-ValentinesDay:0" );
			}
		}
	}
}

void CDPCoreClient::OnGuildCombatState( CAr & ar )
{
	int nState;
	ar >> nState;
	
	g_GuildCombatMng.m_nState = nState;
	if( g_GuildCombatMng.m_nState == CGuildCombat::OPEN_STATE )
		g_GuildCombatMng.m_dwTime = 0;

	g_UserMng.AddGuildCombatState();
}

void CDPCoreClient::OnRemoveUserFromCORE( CAr & ar )
{
	DWORD dwSerial;
	ar >> dwSerial;		// CACHE에서 생성된 serial한 값 

	g_UserMng.RemoveUser( dwSerial ); 
}

void CDPCoreClient::SendPing( void )
{
	if( FALSE == m_bAlive )
	{
#ifndef _DEBUG
		Error( "CORE : is not alive" );
#endif	// _DEBUG
	}

	m_bAlive	= FALSE;
	BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << time_null();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnPing( CAr & ar )
{
	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

	time_t d	= time_null() - tSend;
	CTime tmTrans( tTrans );
#if !defined(_DEBUG)
	if( d > 10 )
		Error( "ping CORE - %d, trans:%s", d, tmTrans.Format( "%Y/%m/%d %H:%M:%S" ) );
#endif
	
	m_bAlive	= TRUE;
}

void CDPCoreClient::OnDestroyPlayer( CAr & ar )
{
	u_long idPlayer;
	ar >> idPlayer;
	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( pUser )
		g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
}

void CDPCoreClient::OnInstanceDungeonAllInfo( CAr & ar )
{
	CInstanceDungeonHelper::GetInstance()->OnInstanceDungeonAllInfo( ar );
}

void CDPCoreClient::OnInstanceDungeonCreate( CAr & ar )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );
	
	ar >> nType >> dwDungeonId >> ID_Info;
	
	CInstanceDungeonHelper::GetInstance()->OnCreateDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreClient::SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId << ID_Info;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonDestroy( CAr & ar )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );
	
	ar >> nType >> dwDungeonId >> ID_Info;
	
	CInstanceDungeonHelper::GetInstance()->OnDestroyDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreClient::SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_DESTROY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId << ID_Info;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonSetCoolTimeInfo( CAr & ar )
{
	int nType;
	DWORD dwPlayerId;
	COOLTIME_INFO CT_Info;
	ULONG uKey;
	
	ar >> uKey >> nType >> dwPlayerId >> CT_Info;
	
	CInstanceDungeonHelper::GetInstance()->OnSetDungeonCoolTimeInfo( uKey, nType, CT_Info, dwPlayerId );
}

void CDPCoreClient::SendInstanceDungeonSetCoolTimeInfo( int nType, DWORD dwPlayerId, const COOLTIME_INFO & CT_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uKey << nType << dwPlayerId << CT_Info;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonDeleteCoolTimeInfo( CAr & ar )
{
	int nType;
	DWORD dwPlayerId;
	
	ar >> nType >> dwPlayerId;
	
	CInstanceDungeonHelper::GetInstance()->OnDeleteDungeonCoolTimeInfo( nType, dwPlayerId );
}

#ifdef __QUIZ
void CDPCoreClient::SendQuizSystemMessage( int nDefinedTextId, BOOL bAll, int nChannel, int nTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_NOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nDefinedTextId << bAll << nChannel << nTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __QUIZ

void CDPCoreClient::OnBuyingInfo( CAr & ar )
{
	auto [playerId, bi2] = ar.Extract<u_long, BUYING_INFO2>();

	CWorld * pWorld;
	CUser * pUser = g_UserMng.GetUserByPlayerID(playerId);

	SERIALNUMBER iSerialNumber = 0;
	if (IsValidObj(pUser) && (pWorld = pUser->GetWorld())) {
		bi2.dwRetVal = 0;
		CItemElem itemElem;
		itemElem.m_dwItemId = bi2.dwItemId;
		itemElem.m_nItemNum = (short)bi2.dwItemNum;
		BYTE nId;
		bi2.dwRetVal = pUser->CreateItem(&itemElem, &nId);
#ifdef __LAYER_1015
		g_dpDBClient.SavePlayer(pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer());
#else	// __LAYER_1015
		g_dpDBClient.SavePlayer(pUser, pWorld->GetID(), pUser->GetPos());
#endif	// __LAYER_1015
		if (bi2.dwRetVal) {
			CItemElem * pItemElem = pUser->m_Inventory.GetAtId(nId);
			if (pItemElem) {
				iSerialNumber = pItemElem->GetSerialNumber();
				if (bi2.dwSenderId > 0) {
					// %s was a gift from %s.
				}
			}
		}
	}

	g_dpDBClient.SendBuyingInfo(&bi2, iSerialNumber);
}

void CDPCoreClient::OnModifyMode(CAr & ar) {
	const auto [idTo, dwMode, f, _idFrom] = ar.Extract<u_long, DWORD, bool, u_long>();

	CUser * pUser = g_UserMng.GetUserByPlayerID(idTo);
	if (!IsValidObj(pUser)) return;

	if (f)
		pUser->m_dwMode |= dwMode;
	else
		pUser->m_dwMode &= ~dwMode;

	g_UserMng.AddModifyMode(pUser);
}
