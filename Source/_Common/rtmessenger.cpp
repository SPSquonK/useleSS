#include "stdafx.h"
#include "rtmessenger.h"

bool IsValid(FriendStatus friendStatus) {
	switch (friendStatus) {
		case FriendStatus::ONLINE:
		case FriendStatus::ABSENT:
		case FriendStatus::HARDPLAY:
		case FriendStatus::EAT:
		case FriendStatus::REST:
		case FriendStatus::MOVE:
		case FriendStatus::OFFLINE:
			return true;
		default:
			return false;
	}
}

void CRTMessenger::SetFriend(u_long idFriend, const Friend & pFriend) {
	if (idFriend != 0) {
		(*this)[idFriend] = pFriend;
	}
}

Friend * CRTMessenger::GetFriend(u_long idFriend) {
	const auto i = find(idFriend);
	return i != end() ? &i->second : nullptr;
}

int CRTMessenger::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwState;
		ar << static_cast<int>( size() );
		for(const auto & [idFriend, friend_] : *this) {
			ar << idFriend << friend_;
		}
	}
	else
	{
		clear();
		ar >> m_dwState;
		int nSize;
		ar >> nSize;

		if( nSize > CRTMessenger::MaxFriend)
			return nSize;

		u_long idFriend;

		for( int i = 0; i < nSize; i++ )
		{
			ar >> idFriend;
			Friend f;
			ar >> f;
			SetFriend( idFriend, f );
		}
	}
	return 0;
}

void	CRTMessenger::SetBlock(u_long idFriend, bool bBlock) {
	if (Friend * pFriend = GetFriend(idFriend)) {
		pFriend->bBlock = bBlock;
	}
}

bool CRTMessenger::IsBlock(u_long idFriend) const {
	const auto it = find(idFriend);
	if (it == end()) return true;
	return it->second.bBlock;
}
