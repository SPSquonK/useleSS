#pragma once

#include "Ar.h"

#define FRS_ONLINE		0	// 기본 : 게임중 
#define FRS_OFFLINE		1	// 자동 : 로그오프
#define FRS_BLOCK	2	// 설정 : 차단 
#define FRS_ABSENT	3	// 설정 : 자리비움
#define FRS_HARDPLAY	4	// 설정 : 열랩중 
#define FRS_EAT		5	// 설정 : 식사중 
#define FRS_REST	6	// 설정 : 휴식중
#define FRS_MOVE	7	// 설정 : 이동중 
#define FRS_DIE		8	// 자동 : 사망 
#define FRS_DANGER		9	// 자동 : 위험
#define FRS_OFFLINEBLOCK	10
#define	FRS_AUTOABSENT		11	// 자동 : 자동 자리비움	
#define MAX_FRIENDSTAT		12	// 맥스값

struct Friend {
	static constexpr bool Archivable = true;

	BOOL	bBlock = FALSE;
	DWORD	dwState = 0;
	Friend() = default;
};

class CRTMessenger final : public	std::map<u_long, Friend>
{
public:
	static constexpr size_t MaxFriend = 200;

	void SetFriend(u_long idFriend, const Friend & pFriend = Friend());
	void RemoveFriend(u_long idFriend) { erase(idFriend); }
	Friend*	GetFriend( u_long idFriend );

	void SetBlock(u_long idFriend, bool bBlock);
	[[nodiscard]] bool IsBlock(u_long idFriend) const;

	[[nodiscard]] DWORD GetState() const  { return m_dwState; }
	void	SetState(DWORD dwState) { m_dwState = dwState; }

	int	Serialize(CAr & ar);

private:
	DWORD	m_dwState = 0;
};

