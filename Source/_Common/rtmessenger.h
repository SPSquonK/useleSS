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

#define MAX_FRIEND		200 // 맥스값 친구 등록 횟수

typedef struct	_Friend
{
	BOOL	bBlock;
	DWORD	dwState;
	_Friend()
	{
		bBlock	= FALSE;
		dwState		= 0;
	}
}	Friend;

class CRTMessenger	: public	std::map<u_long, Friend>
{
public:
	CRTMessenger();
	virtual	~CRTMessenger();
//
//	void	SetFriend( u_long idFriend, BOOL bBlock, DWORD dwState );
	void	SetFriend( u_long idFriend, Friend * pFriend );
	void	RemoveFriend( u_long idFriend )		{	erase( idFriend );	}
	Friend*	GetFriend( u_long idFriend );

	void	SetBlock( u_long idFriend, BOOL bBlock );
	BOOL	IsBlock( u_long idFriend );

	DWORD	GetState( void )	{	return m_dwState;	}
	void	SetState( DWORD dwState )	{	m_dwState	= dwState;	}

//	void	Serialize( CAr & ar );
	int	Serialize( CAr & ar );

	CRTMessenger &	operator =( CRTMessenger & rRTMessenger );
private:
	DWORD	m_dwState;
};

