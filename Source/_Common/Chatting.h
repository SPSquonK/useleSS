#ifndef __CHATTING_H__
#define	__CHATTING_H__

#include "mempooler.h"
#include <map>

#define MAX_CHATTINGMEMBER	512

class CChatting final {
public:
	struct ChatMember {
		u_long m_playerId;
#ifdef __CLIENT
		TCHAR m_szName[MAX_NAME];

		ChatMember(u_long playerId, const TCHAR * name) : m_playerId(playerId) {
			std::strcpy(m_szName, name);
		}
#endif
#ifdef __WORLDSERVER
		explicit(false) ChatMember(u_long playerId) : m_playerId(playerId) {}
#endif
	};

private:

	TCHAR	m_sChatting[33] = "";					// ChattingRoom Name
	u_long	m_idChatting  = 0;
public:
	boost::container::small_vector<ChatMember, MAX_CHATTINGMEMBER> m_members;

	BOOL	m_bState;
	//	Constructions

	bool AddChattingMember(const ChatMember & newMember);
	bool RemoveChattingMember(u_long uidPlayer);
	[[nodiscard]] bool IsChattingMember(u_long uidPlayer) const noexcept;

	void	ClearMember( void );
	const auto & GetMembers() const { return m_members; }

#ifdef __CLIENT
	const ChatMember * GetMember(u_long uIdPlayer) const noexcept;
#endif
	
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CChatting>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CChatting::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CChatting::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CChatting::m_pPool->Free( (CChatting*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CChatting::m_pPool->Free( (CChatting*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

typedef	std::map< u_long, CChatting*>	C2CharttingPtr;

class CChattingMng
{
	C2CharttingPtr	m_2ChatPtr;
public:
	CChattingMng();
	~CChattingMng();

	void	Clear( void );
	u_long	NewChattingRoom( u_long uChattingId );
	BOOL	DeleteChattingRoom( u_long uChattingId );
	CChatting* GetChttingRoom( u_long uidChtting );
};

#endif	//	_CHATTING_H
