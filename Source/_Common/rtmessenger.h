#pragma once

#include "Ar.h"

enum class FriendStatus : DWORD {
	ONLINE = 0, // 기본 : 게임중 
	OFFLINE = 1, // 자동 : 로그오프
	ABSENT = 3, // 설정 : 자리비움
	HARDPLAY = 4, // 설정 : 열랩중 
	EAT = 5, // 설정 : 식사중 
	REST = 6, // 설정 : 휴식중
	MOVE = 7 // 설정 : 이동중 
};

bool IsValid(FriendStatus friendStatus);

struct Friend {
	static constexpr bool Archivable = true;

	BOOL	bBlock = FALSE;
	FriendStatus dwState = FriendStatus::ONLINE;
	Friend() = default;
};

class CRTMessenger final : public	std::map<u_long, Friend>
{
public:
	static constexpr size_t MaxFriend = 200;

	void SetFriend(u_long idFriend, const Friend & pFriend = Friend());
	void RemoveFriend(u_long idFriend) { erase(idFriend); }
	Friend*	GetFriend( u_long idFriend );
	const Friend * GetFriend(u_long idFriend) const;

	void SetBlock(u_long idFriend, bool bBlock);
	[[nodiscard]] bool IsBlock(u_long idFriend) const;

	[[nodiscard]] FriendStatus GetState() const  { return m_dwState; }
	void	SetState(FriendStatus dwState) { m_dwState = dwState; }

	int	Serialize(CAr & ar);

private:
	FriendStatus m_dwState = FriendStatus::ONLINE;
};

