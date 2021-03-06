#pragma once

#ifdef __WORLDSERVER
#include "User.h"
#include <ranges>
#endif // __WORLDSERVER

class CPCBangInfo
{
#ifdef __WORLDSERVER
public:
	CPCBangInfo( DWORD dwPCBangClass, DWORD dwConnectTime );
	~CPCBangInfo();

	void	SetDisconnect( DWORD dwTime )	{ m_dwDisconnectTime = dwTime; }
	BOOL	ProcessRemove( DWORD dwPlayerId );
	void	UpdateInfo();
private:
	DWORD	m_dwUnitTime;
	DWORD	m_dwDisconnectTime;
#endif // __WORLDSERVER
public:
	DWORD	GetPCBangClass()			{ return m_dwPCBangClass; }
	float	GetExpFactor()				{ return m_fExpFactor; }
	float	GetPieceItemDropFactor()	{ return m_fPieceItemDropFactor; }
	DWORD	GetUnitTime()				{ return (g_tmCurrent - m_dwConnectTime)/MIN(60); }
	void	Serialize( CAr & ar );
#ifdef __CLIENT
	CPCBangInfo();
	~CPCBangInfo();
	static CPCBangInfo* GetInstance();
	DWORD	GetConnectTime()			{ return (g_tmCurrent - m_dwConnectTime)/SEC(1); }
#endif // __CLIENT
	
private:
	DWORD	m_dwPCBangClass;
	DWORD	m_dwConnectTime;
	float	m_fExpFactor;
	float	m_fPieceItemDropFactor;
};


#ifdef __WORLDSERVER
typedef std::map<DWORD, CPCBangInfo> MAPPBI;

class CPCBang  
{
public:
	CPCBang();
	~CPCBang();
	static CPCBang*	GetInstance();
	
	BOOL	LoadScript();		

	void	SetPCBangPlayer( CUser* pUser, DWORD dwPCBangClass );	// PC방 유저를 목록에 추가한다.
	void	DestroyPCBangPlayer( DWORD dwPlayerId );	// 접속해제 시간을 저장한다.
	DWORD	GetPCBangClass( DWORD dwPlayerId );
	void	ProcessPCBang();							// 접속 해제 후 10분이 경과된 유저를 초기화 한다.
	float	GetExpInfo( DWORD dwHour );
	float	GetExpFactor( CUser* pUser );				// 증가될 경험치를 얻어온다.
	
	float GetPartyExpFactor(std::ranges::range auto range) {
		float fExpFactor = 0.0f;
		int nMemberSize = 0;
		for (CUser * pUser : range) {
			fExpFactor += GetExpFactor(pUser) - 1.0f;
			++nMemberSize;
		}
		if (nMemberSize == 0) nMemberSize = 1;

		fExpFactor /= nMemberSize;
		fExpFactor += 1.0f;

		return fExpFactor;
	}
	
	float	GetPartyExpFactor( CUser* apUser[], int nMemberSize );
	float	GetPieceItemDropInfo( DWORD dwHour );
	float	GetPieceItemDropFactor( CUser* pUser );		// 증가될 아이템 드롭률을 가져온다.
	void	SetApply( BOOL bApply );
	BOOL	IsApply()				{ return m_bApply; }
	
private:
	CPCBangInfo*	GetPCBangInfo( DWORD dwPlayerId );			// PC방 유저인가?
	int		GetPlayTime( DWORD dwConnectTime ) { return ( g_tmCurrent - dwConnectTime ) / MIN(60); }	// 접속이후 총 플레이 시간
	
	MAPPBI m_mapPCBang;	// PC방 사용자 정보
	std::vector<float>	m_vecfExp;			// 시간대별 경험치
	std::vector<float>	m_vecfDropRate;		// 시간대별 아이템 드롭률
	BOOL	m_bApply;
};
#endif // __WORLDSERVER
