#ifndef __DPCORECLIENT_H__
#define __DPCORECLIENT_H__

#include "DPMng.h"
#include "MsgHdr.h"
#include "misc.h"

#include "guild.h"

#include "InstanceDungeonBase.h"

class CDPCoreClient : public CDPMng,
	public DPMngFeatures::SendPacketDual<CDPCoreClient>,
	public DPMngFeatures::PacketHandler<CDPCoreClient>
{
private:
	WSAEVENT	m_hWait;
	u_long		m_uRecharge;
	BOOL		m_bAlive;
private:
	void	MyRegister( u_long uKey );
	void	Recharge( u_long uBlockSize );
	bool Contribute(const CUser & pUser, DWORD dwPxpCount, DWORD dwPenya);

public:

public:
//	Constructions
	CDPCoreClient();
	virtual	~CDPCoreClient();
//	Override
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	BOOL	Run( LPSTR lpszAddr, USHORT uPort, u_long uKey );
	BOOL	CheckIdStack( void );

	[[nodiscard]] bool IsAlive() const { return m_bAlive; }

	void	SendJoin( u_long idPlayer, const char* szPlayer, BOOL bOperator );
	void	SendSystemMessage( LPCTSTR lpsz );
//	Operator commands
	void	SendWhisper( u_long idFrom, u_long idTo, const CHAR* lpString );

	void	SendSay( u_long idFrom, u_long idTo, const CHAR* lpString );
	void	SendModifyMode( DWORD dwMode, bool fAdd, u_long idFrom, u_long idTo );
	void	SendShout( CUser* pUser, const CHAR* lpString );
	void	SendPartyChat( CUser* pUser, const CHAR* lpString );

	void	SendUserPartySkill( u_long uidPlayer, int nMode, DWORD dwSkillTime, int nRemovePoint , int nCachMode );

	void	SendGMSay( u_long idPlayer, DWORD dwWorldID, const CHAR* lpString );
	void	SendPlayMusic( DWORD dwWorldID, u_long idmusic );
	void	SendPlaySound( DWORD dwWorldID, u_long idsound );
	void	SendKillPlayer( u_long idOperator, u_long idPlayer );
	void	SendGetPlayerAddr( u_long idOperator, u_long idPlayer );
	void	SendGetPlayerCount( u_long idOperator );
	void	SendGetCorePlayer( u_long idOperator );
	void	SendSystem( const CHAR* lpString );
	void	SendToServer( LPBYTE lpBuffer, u_long uBufSize, DPID dpidCache, DPID dpidUser );
	
	void	SendCaption( const CHAR* lpString, DWORD dwWorldId = 0, BOOL bSmall = FALSE );
		
	void	SendEventRealItem( u_long uIdPlayer, int nRealItemIndex, int nRealItemCount );
	void	SendPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp );
	void	SendPartyMemberFlightLevel( CUser* pUser );
	void	SendAddPartyExp( u_long uPartyId, int nMonLv, BOOL bSuperLeader , BOOL bLeaderSMExpUp );

	void	SendRemovePartyPoint( u_long uPartyId, int nRemovePoint );
	void	SendGameRate( FLOAT fRate, BYTE nFlag );
	void	SendLoadConstant();
	void	SendSetMonsterRespawn( u_long uidPlayer, DWORD dwMonsterID, DWORD dwRespawnNum, DWORD dwAttackNum, DWORD dwRect, DWORD dwRespawnTime, BOOL bFlying );
	void	SendFallSnow();
	void	SendFallRain();
	void	SendStopSnow();
	void	SendStopRain();
//	void	SendSetPointParam( CMover* pMover, int nParam, int nVal );
	void	SendGuildCombatState( int nState );
	void	SendGCRemoveParty( u_long uidPartyid, u_long uidPlayer );
	void	SendGCAddParty( u_long idLeader, LONG nLeaderLevel, LONG nLeaderJob, DWORD dwLSex, 
							u_long idMember, LONG nMemberLevel, LONG nMemberJob, DWORD dwMSex );
	void	OnCWWantedList( CAr & ar );
	void	OnCWWantedReward( CAr & ar );
	void	SendSetPartyDuel( u_long idParty1, u_long idParty2, BOOL bDuel );
	void	SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, const char* szGuild );
	void	SendGuildChat( CUser* pUser, const char* sChat );
	void SendGuildStatLogo(  CUser* pUser, DWORD data );
	bool SendGuildStatPenya(CUser * pUser, DWORD data);
	bool SendGuildStatPxp(CUser * pUser, DWORD data);
	void SendGuildStatNotice(CUser * pUser, const char * notice);
	void	SendGuildGetPay( u_long uGuildId, DWORD nGoldGuild );
	void	SendGuildMsgControl_Bank_Penya( CUser* pUser, DWORD p_Penya, BYTE p_Mode, BYTE cbCloak );
	void	SendWarDead( u_long idPlayer );
	void	SendWarMasterAbsent(WarId idWar, BOOL bDecl );
	void	SendWarTimeout(WarId idWar );
	void	OnWarDead( CAr & ar );
	void	OnWarEnd( CAr & ar );
	void	SendAddFriendNameReqest( u_long uLeaderid, LONG nLeaderJob, BYTE nLeaderSex, u_long uMember, const char * szLeaderName, const char * szMemberName );
	void	SendBlock( BYTE nGu, u_long uidPlayerTo, char *szNameTo, u_long uidPlayerFrom );
	void	SendWCWantedGold( LPCTSTR szPlayer, u_long idPlayer, int nGold, LPCTSTR szMsg );
#ifdef __LAYER_1015
	void	SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos, int nLayer );
#else	// __LAYER_1015
	void	SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos );
#endif	// __LAYER_1015
	void	SendQuerySetGuildName( u_long idPlayer, u_long idGuild, const char* lpszGuild, BYTE nId );
	void	SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease  );
	void	OnSetSnoop( CAr & ar );
	void	SendSetSnoopGuild( u_long idGuild, BOOL bRelease );
	void	SendChat( u_long idPlayer1, u_long idPlayer2, const char* lpszChat );
	void	SendPing( void );
	void	OnDestroyPlayer( CAr & ar );
private:
	void	OnInstanceDungeonAllInfo( CAr & ar );
	void	OnInstanceDungeonCreate( CAr & ar );
	void	OnInstanceDungeonDestroy( CAr & ar );
	void	OnInstanceDungeonSetCoolTimeInfo( CAr & ar );
	void	OnInstanceDungeonDeleteCoolTimeInfo( CAr & ar );
public:
	void	SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info );
	void	SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info );
	void	SendInstanceDungeonSetCoolTimeInfo( int nType, DWORD dwPlayerId, const COOLTIME_INFO & CT_Info );
#ifdef __QUIZ
public:
	void	SendQuizSystemMessage( int nDefinedTextId, BOOL bAll = FALSE, int nChannel = 0, int nTime = 0 );
#endif // __QUIZ

	// Handlers
private:
	void	OnLoadWorld( CAr & ar );
	void	OnQueryTickCount( CAr & ar );
	void	OnRecharge( CAr & ar );

	void	OnSetPartyExp( CAr & ar );
	void	OnRemovePartyPoint( CAr & ar );
	void	OnPartyChangeTroup( CAr & ar );
	void	OnPartyChangeName( CAr & ar );
	void	OnAddFriend( CAr & ar );
	void	OnRemovefriend( CAr & ar );
	void	OnShout( CAr & ar );
	void	OnPlayMusic( CAr & ar );
	void	OnPlaySound( CAr & ar );
	void	OnErrorParty( CAr & ar );
	void	OnAddPartyMember( CAr & ar );
	void	OnRemovePartyMember( CAr & ar );
	void	OnAddPlayerParty( CAr & ar );
	void	OnRemovePlayerParty( CAr & ar );
	void	OnGuildMemberLv( CAr & ar );
	void	OnSetPartyMode( CAr & ar );
	void	OnPartyChangeItemMode( CAr & ar );
	void	OnPartyChangeExpMode( CAr & ar );

	void	OnEnvironmentEffect( CAr & ar );

	void	OnPartyChat( CAr & ar  );
	void	OnCreateGuild( CAr & ar );
	void	OnDestroyGuild( CAr & ar );
	void	OnAddGuildMember( CAr & ar );
	void	OnRemoveGuildMember( CAr & ar );
	void	OnGuildClass( CAr & ar );
	void	OnGuildNickName( CAr & ar );
	void	OnChgMaster( CAr & ar );
	void	OnGuildMemberLogOut( CAr & ar );
	void	OnGuildLogoACK( CAr & ar );
	void	OnGuildContributionACK( CAr & ar );
	void	OnGuildNoticeACk( CAr & ar );
	void	OnAddVoteResultACk( CAr & ar );
	void	OnModifyVote( CAr & ar );

	void	OnGuildAuthority( CAr & ar );
	void	OnGuildPenya( CAr & ar );
	void	OnGuildRealPenya( CAr & ar );
	void	OnGuildSetName( CAr & ar );
	void	OnGuildMsgControl( CAr & ar );
	void	OnAcptWar( CAr & ar );
	void	OnSurrender( CAr & ar );

	void	OnSetPointParam( CAr & ar );
	void	OnFriendInterceptState( CAr & ar );
	void	OnSetFriendState( CAr & ar );
	void	OnPartyChangeLeader( CAr & ar );

	void	OnLoadConstant( CAr & ar );
	void	OnGameRate( CAr & ar );
	void	OnSetMonsterRespawn( CAr & ar );
	void	OnSetPlayerName( CAr & ar );
#ifdef __EVENT0913
	void	OnEvent0913( CAr & ar );
#endif	// __EVENT0913
#ifdef __EVENT1206
	void	OnEvent1206( CAr & ar );
#endif	// __EVENT1206
	void	OnEvent( CAr & ar );
	void	OnGuildCombatState( CAr & ar );
	void	OnRemoveUserFromCORE( CAr & ar );
	void	OnPing( CAr & ar );


	void OnBuyingInfo(CAr & ar);
	void OnModifyMode(CAr & ar);

	void PASS(CAr & ar);
};

extern CDPCoreClient g_DPCoreClient;

#endif	// __DPCORECLIENT_H__