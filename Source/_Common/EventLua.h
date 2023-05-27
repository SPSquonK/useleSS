#pragma once

#include "LuaBase.h"

#ifdef __WORLDSERVER
class CEventLua;
class CEventLuaProxy final {
public:
	CEventLuaProxy() = default;
public:
	std::vector<BYTE>	GetEventList()	{	return m_vEventLists;	}
	float	GetExpFactor()	{	return m_fExpFactor;	}
	float	GetItemDropRate()	{	return m_fItemDropRate;	}
	float	GetPieceItemDropRate()	{	return m_fPieceItemDropRate;	}
	float	GetGoldDropFactor()	{	return m_fGoldDropFactor;	}
	int		GetAttackPower()	{	return m_nAttackPower;	}
	int		GetDefensePower()	{	return m_fDefensePower;		}
	DWORD	GetCouponEvent()	{	return m_dwCouponEvent;	}
#ifdef __EVENTLUA_CHEEREXP
	float	GetCheerExpFactor() { return m_fCheerExpFactor; }
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
	DWORD	GetKeepConnectTime()	{	return m_dwKeepConnect;	}
#endif // __EVENTLUA_KEEPCONNECT

	float	GetWeatherEventExpFactor()	{ return m_fWeatherEventExpFactor; }

#ifdef __SHOP_COST_RATE
	float	GetShopBuyFactor()	{	return m_fShopBuyFactor;	}
	float	GetShopSellFactor()	{	return m_fShopSellFactor;	}
#endif // __SHOP_COST_RATE
	void	Initialize( CEventLua* pEventLua );
private:
	std::vector<BYTE>	m_vEventLists;
	float	m_fExpFactor = 1.0f;
	float	m_fItemDropRate = 1.0f;
	float	m_fPieceItemDropRate = 1.0f;
	float	m_fGoldDropFactor = 1.0f;
	int		m_nAttackPower = 0;
	int		m_fDefensePower = 0;
	DWORD	m_dwCouponEvent = 0;
#ifdef __EVENTLUA_CHEEREXP
	float	m_fCheerExpFactor = 1.0f;
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
	DWORD	m_dwKeepConnect = 0;
#endif // __EVENTLUA_KEEPCONNECT


	float	m_fWeatherEventExpFactor = 1.0f;


#ifdef __SHOP_COST_RATE
	float	m_fShopBuyFactor = 1.0f;
	float	m_fShopSellFactor = 1.0f;
#endif // __SHOP_COST_RATE
};
#endif // __WORLDSERVER

class CEventLua final {
public:
#ifdef __DBSERVER
#ifdef __AUTO_NOTICE
	BOOL			IsNoticeTime();
	std::vector<std::string>	GetNoticeMessage();
#endif // __AUTO_NOTICE
	BOOL			CheckEventState();
	std::map<BYTE, BOOL>	m_mapState;
	CMclCritSec		m_Access;
#ifdef __VTN_TIMELIMIT
private:
	BOOL			m_bTimeLimit = TRUE;
public:
	BOOL			IsTimeLimit()	{ return m_bTimeLimit; }
#endif // __VTN_TIMELIMIT
#endif // __DBSERVER

#if defined(__WORLDSERVER) || defined(__DBSERVER)
	std::vector<BYTE>	GetEventList( BOOL bProxy = TRUE );
	void			LoadScript();
	BOOL			IsPossible() { return m_bRun; }
	BOOL			m_bRun = FALSE;
	CLuaBase m_Lua;
#endif // __WORLDSERVER, __DBSERVER

#ifdef __WORLDSERVER
	void			SetState( BYTE nId, BOOL bState );
	float			GetExpFactor( BOOL bProxy = TRUE );
	float			GetItemDropRate( BOOL bProxy = TRUE );
	float			GetPieceItemDropRate( BOOL bProxy = TRUE );
	float			GetGoldDropFactor( BOOL bProxy = TRUE );
	int				GetAttackPower( BOOL bProxy = TRUE );
	int				GetDefensePower( BOOL bProxy = TRUE );
#ifdef __EVENTLUA_COUPON
	void			SetCoupon( CUser* pUser, DWORD dwTick );
#endif // __EVENTLUA_COUPON
	std::string			GetDesc( BYTE nId );	// x
#ifdef __EVENTLUA_GIFT
	void			SetLevelUpGift( CUser* pUser, int nLevel );		// x
#endif // __EVENTLUA_GIFT
	std::map<DWORD, int>	GetItem( DWORD dwLevel );	// x
	void GetAllEventList( CUser* pUser );	// x
	void GetEventInfo( CUser* pUser, int nId );	// x
#ifdef __EVENTLUA_CHEEREXP
	float GetCheerExpFactor( BOOL bProxy = TRUE );
#endif // __EVENTLUA_CHEEREXP


	float			GetWeatherEventExpFactor( BOOL bProxy = TRUE );
	std::string			GetWeatherEventTitle();


#ifdef __SHOP_COST_RATE
	float			GetShopBuyFactor( BOOL bProxy = TRUE );
	float			GetShopSellFactor( BOOL bProxy = TRUE );
#endif // __SHOP_COST_RATE
#endif // __WORLDSERVER
	
	DWORD	GetCouponEvent( BOOL bProxy = TRUE );

#ifdef __WORLDSERVER
	void	PrepareProxy()	{	m_proxy.Initialize( this );	}
private:
	CEventLuaProxy	m_proxy;

#ifdef __EVENTLUA_SPAWN
private:
	struct __SPAWNINFO
	{
		int		nType;
		DWORD	dwId, dwInterval, dwTimeOut;
		__SPAWNINFO( int nT, DWORD dwI, DWORD dwIT ) : nType( nT ), dwId( dwI ), dwInterval( dwIT ), dwTimeOut( 0 ) {}
		BOOL	IsTimeOut()
		{
			DWORD dwCurrentTime = timeGetTime();
			if( dwTimeOut > dwCurrentTime )
				return FALSE;
			dwTimeOut = dwCurrentTime + dwInterval;
			return TRUE;
		}
	};
	typedef std::vector<__SPAWNINFO> VECSPAWNINFO;

	struct __SPAWNREGION
	{
		DWORD	dwWorldId;		// 월드 번호
		CRect	rect;	// 참조 영역
		__SPAWNREGION( DWORD dwWI, CRect & r ) : dwWorldId( dwWI ), rect( r ) {}
		D3DXVECTOR3 GetPos() { return D3DXVECTOR3( (float)( xRandom( rect.left, rect.right ) ), 0.0f, (float)( xRandom( rect.top, rect.bottom ) ) ); }
	};

	std::vector<__SPAWNREGION>	m_vecSpawnRegion;		// spawn 이벤트 진행시 아이템 및 몬스터의 생성위치(월드 및 좌표) 정보

	std::map<BYTE, VECSPAWNINFO>	m_mapSpawnList;			// spawn 이벤트 진행시 생성될 아이템 및 몬스터 정보
	std::map<DWORD, BYTE>		m_mapMonsterId;			// spawn 이벤트 진행시 생성된 몬스터가 이벤트용 인지 검사하기 위해 필요
	std::map<OBJID, BYTE>		m_mapSpawnedMonster;	// spawn 이벤트 진행시 실제로 생성된 몬스터들의 목록 저장(이벤트 종료시 삭제하기 위해 필요)
	std::vector<BYTE>			m_vecEndSpawnEvent;			// 종료된 이벤트 ID 목록( 남아있는 spawn 몬스터가 있으면 다 삭제한 후 ID를 제거한다

	void	SetSpawnEvent( BYTE nId, BOOL bState );
	__SPAWNREGION GetSpawnRegion() { return m_vecSpawnRegion[xRandom( m_vecSpawnRegion.size() )]; }
public:
	void	SetAddSpawnRegion( DWORD dwWorldId, CRect & rect ) { m_vecSpawnRegion.push_back( __SPAWNREGION( dwWorldId, rect ) ); }
	BOOL	IsEventSpawnMonster( DWORD dwMonsterId ) { return m_mapMonsterId.find( dwMonsterId ) != m_mapMonsterId.end(); }
	void	EventSpawnProcess();
	void	DeleteEndEventProcess();
	void	RemoveSpawnedMonster( OBJID objId ) { m_mapSpawnedMonster.erase( objId ); }
#endif // __EVENTLUA_SPAWN

#ifdef __EVENTLUA_KEEPCONNECT
private:
	std::vector<u_long>	m_vecKeepConnectUser;
	std::map<DWORD, int>	m_mapItemList;
public:
	void			SetKeepConnectEvent( CUser* pUser, DWORD dwTick );
	DWORD			GetKeepConnectTime( BOOL bProxy = TRUE );
	void			GetKeepConnectItem();
	void			KeepConnectEventProcess();
#endif // __EVENTLUA_KEEPCONNECT

#endif // __WORLDSERVER
};

