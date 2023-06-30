#pragma once

#include <algorithm>
#include <boost/container/flat_map.hpp>
#include "DPMng.h"
#include "msghdr.h"
#include "ServerDesc.h"
#include "ObjMap.h"
#include "guild.h"
#include "party.h"
#include "Player.h"

#include "InstanceDungeonBase.h"
#include "rtmessenger.h"
	
class CDPCoreSrvr : public CDPMng,
	public DPMngFeatures::BroadcastPacketDual<CDPCoreSrvr>
{
public:
	int					m_nGCState;
public:
	CServerDescArray	m_apSleepServer;
	CServerDescArray	m_apServer;	// active
	boost::container::flat_map<u_long, DPID> m_multiIdToDpid; // [1]
	CMclCritSec		m_AccessLock;
	CObjMap		m_objmap;
	DPMngFeatures::PacketHandler<CDPCoreSrvr, DPID, DPID, DPID, u_long> m_handlers;

	// [1] By design, m_multiIdToDpid will never change size / invalidate its
	// iterators / etc after reading the ini file. This make it a structure that
	// can be read and values can be modified without locking. We consider this
	// design good enough as worlds should not connect / disconnect all the time,
	// but we still want some guarantees about the core not crashing if a world
	// crashes.
	// (when a world crashes, the DPID is not removed but set to DPID_UNKNOWN)
public:
	// Constructions
	CDPCoreSrvr();
	virtual	~CDPCoreSrvr();
	
	// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendRecharge( u_long uBlockSize, DPID dpid );
	void	SendQueryTickCount( DWORD dwTime, DPID dpid, double dCurrentTime );
//	Operator commands
	void	SendShout( u_long idPlayer, const CHAR* lpString, DPID dpid );
	void	SendPlayMusic( u_long idmusic, DWORD dwWorldID, DPID dpid );
	void	SendPlaySound( u_long idsound, DWORD dwWorldID, DPID dpid );
	
	void	SendSetPartyMode( u_long idParty, int nMode, BOOL bOnOff, LONG nPoint = 0 , DWORD dwSkillTime = 0 );
	void	SendRemoveParty( u_long idParty, u_long idLeader, u_long idMember );
	void	SendPartyChangeTroup( u_long idParty, const char * szPartyName );
	void	SendPartyChangeName( u_long idParty, const char * szPartyName );
	void	SendPartyChangeItemMode( u_long idParty, CParty::ShareItemMode nItemMode );
	void	SendPartyChangeExpMode( u_long idParty, CParty::ShareExpMode nExpMode );

	void	SendAddPartyMember(u_long uPartyId, u_long idLeader, u_long idMember);
	void	SendRemoveFriend( u_long uidSender, u_long uidFriend );
	void	SendEnvironmentEffect();
	void	SendPartyChat( u_long idParty, const CHAR* lpName, const CHAR* lpString, OBJID objid );
	void	SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData, BOOL f );

	void	SendDestroyPlayer( u_long idPlayer, DPID dpid );

#ifdef __EVENT0913
	void	SendEvent0913( BOOL f );
#endif	// __EVENT0913
#ifdef __EVENT1206
	void	SendEvent1206( BOOL f );
#endif	// __EVENT1206
	void	SendEvent( DWORD dwEvent );
	void	SendGuildCombatState( void );
	void	SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease );

	// Handlers
private:
	void	OnAddConnection( CAr & ar, DPID dpid, DPID, DPID, u_long );
	void	OnRemoveConnection( DPID dpid );
	void	OnRecharge( CAr & ar, DPID dpid, DPID, DPID, u_long );
	void	OnJoin( CAr & ar, DPID dpid, DPID, DPID, u_long );		// idPlayer, szPlayer, bOperator

//	Operator commands
	void	OnWhisper( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long );
	void	OnSay( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long );
	void	OnModifyMode( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long );
	void	OnShout( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGMSay( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPlayMusic( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPlaySound( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnKillPlayer( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGetPlayerAddr( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGetPlayerCount( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGetCorePlayer( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSystem( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnCaption( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnAddPartyExp( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnRemovePartyPoint( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnFallSnow( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnFallRain( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnStopSnow( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnStopRain( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPartySkillUse( CAr & ar, DPID, DPID, DPID, u_long uBufSize );
	void	OnPartyChat( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPartyLevel( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnLoadConstant( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGameRate( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSetMonsterRespawn( CAr & ar, DPID, DPID, DPID, u_long );

	void    OnGuildLogo( CAr & ar, DPID, DPID, DPID, u_long );
	void    OnGuildContribution( CAr & ar, DPID, DPID, DPID, u_long );
	void    OnGuildNotice( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGuildRealPenya( CAr & ar, DPID, DPID, DPID, u_long );

	void	OnSetSnoop( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSetSnoopGuild( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnChat( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnPing( CAr & ar, DPID dpid, DPID, DPID, u_long );

	void	OnGCState( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGCRemoveParty( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGCAddParty( CAr & ar, DPID, DPID, DPID, u_long );

private:
	DPID	GetWorldSrvrDPID( u_long uIdofMulti );

public:
	[[nodiscard]] DPID GetWorldSrvrDPIDWeak(u_long uIdofMulti) const;

	void	OnGuildMsgControl( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnCreateGuild( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnGuildChat( CAr & ar, DPID, DPID, DPID, u_long );
	void	SendCreateGuild( GUILD_MEMBER_INFO* pInfo, int nSize, u_long idGuild, const char* szGuild );
	void	SendDestroyGuild( u_long idGuild );
	void	SendAddGuildMember( const GUILD_MEMBER_INFO & info, u_long idGuild );
	void	SendRemoveGuildMember( u_long idMember, u_long idGuild );
	void	SendGuildMemberLv( u_long idPlayer, int nMemberLv, u_long idGuild );
	
	void	SendGuildClass( u_long idPlayer, int nClass, u_long idGuild );
	void	SendGuildNickName( u_long idPlayer, LPCTSTR strNickName, u_long idGuild );
	void	SendGuildMemberLogOut( u_long idGuild, u_long idPlayer );
	void	SendGuildAuthority( u_long uGuildId, const GuildPowerss & dwAuthority );
	void	SendGuildPenya( u_long uGuildId, DWORD dwType, DWORD dwPenya );
	void	SendChgMaster( u_long idPlayer, u_long idPlayer2, u_long idGuild );
	void	SendGuildSetName( u_long idGuild, const char * lpszGuild , u_long idPlayer = 0, BYTE nId = 0xff, BYTE nError = 0 );

	void	SendAddVoteResult( VOTE_INSERTED_INFO& info );
	void	SendAddRemoveVote( u_long idGuild, u_long idVote );		
	void	SendAddCloseVote( u_long idGuild, u_long idVote );
	void	SendAddCastVote( u_long idGuild, u_long idVote, BYTE cbSelection );

	void	SendAcptWar(WarId idWar, u_long idDecl, u_long idAcpt );
	void	SendWarDead(WarId idWar, BOOL bDecl );
	void	SendWarEnd(WarId idWar, int nWptDecl, int nWptAcpt, int nType );
	void	SendSurrender(WarId idWar, u_long idPlayer, BOOL bDecl );
	void	SendWarMasterAbsent(WarId idWar, BOOL bDecl );
	void	OnWarDead( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnWarMasterAbsent( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnWarTimeout( CAr & ar, DPID, DPID, DPID, u_long );

public:
	void	SendSetFriendState( u_long uidPlayer, FriendStatus dwState );
	void	SendChangeLeader( u_long uPartyId, u_long uidChangeLeader );
	void	SendFriendInterceptState( u_long uIdPlayer, u_long uIdFriend );
		
	void	OnAddFriendNameReqest( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnBlock( CAr & ar, DPID, DPID, DPID, u_long );

	void	UpdateWantedList();
	void	SendCWWantedList( DPID dpid = DPID_ALLPLAYERS );
	void	OnWCWantedGold( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnWCWantedReward( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnSetPartyDuel( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnQuerySetGuildName( CAr & ar, DPID, DPID, DPID, u_long );
	void	SendRemoveUser( DWORD dwSerial );
public:
	void	SendInstanceDungeonAllInfo( int nType, CInstanceDungeonBase* pID, DPID dpId );
	void	SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info );
	void	SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, const ID_INFO & ID_Info );
	void	SendInstanceDungeonSetCoolTimeInfo( ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO CT_Info );
	void	SendInstanceDungeonDeleteCoolTimeInfo( int nType, DWORD dwPlayerId );
private:
	void	OnInstanceDungeonCreate( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnInstanceDungeonDestroy( CAr & ar, DPID, DPID, DPID, u_long );
	void	OnInstanceDungeonSetCoolTimeInfo( CAr & ar, DPID, DPID, DPID, u_long );
#ifdef __QUIZ
private:
	void	OnQuizSystemMessage( CAr & ar, DPID, DPID, DPID, u_long );
#endif // __QUIZ

public:
	template<DWORD PacketId, typename ... Ts>
	void SendPacketAbout(CPlayer & player, const Ts & ... ts) {
		const DPID dpid = GetWorldSrvrDPID(player.m_uIdofMulti);
		if (dpid == DPID_UNKNOWN) return;

		BEFORESENDDUAL(ar, PacketId, DPID_ALLPLAYERS, DPID_ALLPLAYERS);
		ar.Accumulate(player.uKey, ts...);
		SEND(ar, this, dpid);
	}
};

extern CDPCoreSrvr g_dpCoreSrvr;

inline DPID CDPCoreSrvr::GetWorldSrvrDPID(const u_long uIdofMulti) {
	if (uIdofMulti == NULL_ID) return DPID_UNKNOWN;

	const auto i = std::ranges::find_if(m_apServer,
		[&](const CServerDescArray::value_type & pair) {
			const CServerDesc * const pServer = pair.second;
			return pServer->GetIdofMulti() == uIdofMulti;
		}
	);

	return i != m_apServer.end() ? i->first : DPID_UNKNOWN;
}

inline DPID CDPCoreSrvr::GetWorldSrvrDPIDWeak(u_long uIdofMulti) const {
	const auto it = m_multiIdToDpid.find(uIdofMulti);
	return it != m_multiIdToDpid.end() ? it->second : DPID_UNKNOWN;
}
