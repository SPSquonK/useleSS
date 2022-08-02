#ifndef __FLYFFEVENT_H__
#define	__FLYFFEVENT_H__

#include <array>

#define	MAX_EVENT	1024

enum
{
	EVE_0401A, EVE_18, ENABLE_GUILD_INVENTORY,
	EVE_GUILDWAR,
	EVE_SCHOOL, EVE_SCHOOL_BATTLE,
	EVE_NOFLYMONSTER, // 비행 몬스터 제거 
	EVE_NODARKON, // 다콘 제거 
	EVE_NOGUILD, // 길드 제거 
	EVE_WORMON,
	EVE_RESPAWN,
	EVE_PK,
	EVE_PKCOST,
	EVE_STEAL,
	EVE_EVENT0913,
	EVE_GUILDCOMBAT,
	EVE_DROPITEMREMOVE,
	EVE_EVENT1206,
	EVE_EVENT1219,
	EVE_EVENT0127,
	EVE_EVENT0214,
#ifdef __S_RECOMMEND_EVE
	EVE_RECOMMEND,
#endif // __S_RECOMMEND_EVE

	EVE_GUILDCOMBAT1TO1,

#ifdef __JEFF_11_4
	EVE_ARENA, 
#endif // __JEFF_11_4
	EVE_SECRETROOM,	// 12차 비밀의 방
	EVE_RAINBOWRACE,
};

#define	EVE_FLAG1219	(DWORD)0x00000001
#define	EVE_FLAG0127	(DWORD)0x00000002
#define	EVE_FLAG0214	(DWORD)0x00000004

class CFlyffEvent
{
private:
	std::array<BYTE, MAX_EVENT> m_aEvent;

public:
//	Constructions
	CFlyffEvent();
//	Operations
	BOOL	SetState( int id, BYTE nState );
	[[nodiscard]] BYTE	GetState( int id ) const;
	BOOL	ClearEvent( int id )	{	return SetState( id, 0 );	}

	friend CAr & operator<<(CAr & ar, const CFlyffEvent & self);
	friend CAr & operator>>(CAr & ar, CFlyffEvent & self);
	
private:
	static bool IsOutOfRange(int id) noexcept;
};

#endif	// __FLYFFEVENT_H__