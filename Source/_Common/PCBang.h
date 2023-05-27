#pragma once

#ifdef __WORLDSERVER
#include "User.h"
#include <ranges>
#endif // __WORLDSERVER

class CPCBangInfo final
{
#ifdef __CLIENT
public:
	CPCBangInfo();
	static CPCBangInfo * GetInstance();
#endif // __CLIENT

#ifdef __WORLDSERVER
public:
	CPCBangInfo( DWORD dwPCBangClass, DWORD dwConnectTime );

	void	SetDisconnect( DWORD dwTime )	{ m_dwDisconnectTime = dwTime; }
	bool	ProcessRemove( DWORD dwPlayerId );
	void	UpdateInfo();
#endif // __WORLDSERVER
public:
	[[nodiscard]] DWORD GetPCBangClass() const         { return m_dwPCBangClass; }
	[[nodiscard]] float GetExpFactor() const           { return m_fExpFactor; }
	[[nodiscard]] float GetPieceItemDropFactor() const { return m_fPieceItemDropFactor; }
	[[nodiscard]] DWORD GetUnitTime() const    { return (g_tmCurrent - m_dwConnectTime) / MIN(60); }
	[[nodiscard]] DWORD	GetConnectTime() const { return (g_tmCurrent - m_dwConnectTime) / SEC(1); }
	friend CAr & operator<<(CAr & ar, const CPCBangInfo & self);
	friend CAr & operator>>(CAr & ar, CPCBangInfo & self);
	
private:
	DWORD	m_dwPCBangClass;
	DWORD	m_dwConnectTime;
	float	m_fExpFactor;
	float	m_fPieceItemDropFactor;

#ifdef __WORLDSERVER
	DWORD	m_dwUnitTime;
	DWORD	m_dwDisconnectTime;
#endif // __WORLDSERVER
};


#ifdef __WORLDSERVER
class CPCBang final {
public:
	CPCBang();
	static CPCBang*	GetInstance();
	
	BOOL	LoadScript();		

	void	SetPCBangPlayer( CUser* pUser, DWORD dwPCBangClass );	// PC방 유저를 목록에 추가한다.
	void	DestroyPCBangPlayer( DWORD dwPlayerId );	// 접속해제 시간을 저장한다.
	DWORD	GetPCBangClass( DWORD dwPlayerId );
	void	ProcessPCBang();							// 접속 해제 후 10분이 경과된 유저를 초기화 한다.
	float	GetExpInfo( DWORD dwHour );
	[[nodiscard]] float	GetExpFactor( const CUser* pUser ) const;				// 증가될 경험치를 얻어온다.
	
	float GetPartyExpFactor(std::ranges::range auto range) const {
		float fExpFactor = 0.0f;
		int nMemberSize = 0;
		for (const CUser * pUser : range) {
			fExpFactor += GetExpFactor(pUser) - 1.0f;
			++nMemberSize;
		}
		if (nMemberSize == 0) return 1.0f;

		fExpFactor /= nMemberSize;
		fExpFactor += 1.0f;

		return fExpFactor;
	}
	
	float	GetPieceItemDropInfo( DWORD dwHour );
	float	GetPieceItemDropFactor(const CUser* pUser ) const;		// 증가될 아이템 드롭률을 가져온다.
	void	SetApply( BOOL bApply );
	[[nodiscard]] BOOL	IsApply() const				{ return m_bApply; }
	
private:
	CPCBangInfo*	GetPCBangInfo( DWORD dwPlayerId );			// PC방 유저인가?
	[[nodiscard]] const CPCBangInfo * GetPCBangInfo(DWORD dwPlayerId) const;
	int		GetPlayTime( DWORD dwConnectTime ) { return ( g_tmCurrent - dwConnectTime ) / MIN(60); }	// 접속이후 총 플레이 시간
	
	std::map<DWORD, CPCBangInfo> m_mapPCBang;	// PC방 사용자 정보
	std::vector<float>	m_vecfExp;			// 시간대별 경험치
	std::vector<float>	m_vecfDropRate;		// 시간대별 아이템 드롭률
	BOOL	m_bApply;
};
#endif // __WORLDSERVER
