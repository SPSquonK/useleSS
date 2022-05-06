#include "stdafx.h"
#include "Chatting.h"

#ifdef __CLIENT
CChatting g_Chatting;
#endif

#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CChatting>*	CChatting::m_pPool		= new MemPooler<CChatting>( 512, "CChatting" );
	#else	// __VM_0819
	MemPooler<CChatting>*	CChatting::m_pPool		= new MemPooler<CChatting>( 512 );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

bool CChatting::AddChattingMember(const ChatMember & newMember) {
	if (IsChattingMember(newMember.m_playerId)) return false;
	
	m_members.emplace_back(newMember);
	return true;
}

bool CChatting::RemoveChattingMember(u_long uidPlayer) {
	const auto it = std::ranges::find_if(m_members,
		[uidPlayer](const ChatMember & member) {
			return member.m_playerId == uidPlayer;
		});

	if (it == m_members.end()) return false;

	m_members.erase(it);
	return true;
}

bool CChatting::IsChattingMember(const u_long uidPlayer) const noexcept {
	const auto it = std::ranges::find_if(m_members,
		[uidPlayer](const ChatMember & member) {
			return member.m_playerId == uidPlayer;
		});
	
	return it != m_members.end();
}

#ifdef __CLIENT
const CChatting::ChatMember * CChatting::GetMember(u_long uIdPlayer) const noexcept {
	const auto it = std::ranges::find_if(m_members,
		[uIdPlayer](const ChatMember & member) {
			return member.m_playerId == uIdPlayer;
		});

	if (it == m_members.end()) return nullptr;
	return &*it;
}
#endif

void CChatting::ClearMember() {
	m_sChatting[0] = '\0';
	m_members.clear();
}














//////////////////////////////////////////////////////////////////////////////////////////////////
//		ChattingMng
//////////////////////////////////////////////////////////////////////////////////////////////////
CChattingMng::CChattingMng()
{
}

CChattingMng::~CChattingMng()
{
	Clear();
}

void CChattingMng::Clear( void )
{
	for( C2CharttingPtr::iterator i	= m_2ChatPtr.begin(); i != m_2ChatPtr.end(); ++i )
		safe_delete( i->second );
	
	m_2ChatPtr.clear();
}

u_long CChattingMng::NewChattingRoom( u_long uChattingId )
{
	CChatting* pChatting = GetChttingRoom( uChattingId );
	if( pChatting )
	{
		pChatting->ClearMember();		
	}
	else
	{
		CChatting* pChatting = new CChatting;
		m_2ChatPtr.insert( C2CharttingPtr::value_type( uChattingId, pChatting ) );
	}
	return uChattingId;
}

BOOL CChattingMng::DeleteChattingRoom( u_long uChattingId )
{
	CChatting* pChatting = GetChttingRoom( uChattingId );
	if( pChatting )
	{
		safe_delete( pChatting );
		m_2ChatPtr.erase( uChattingId );
		return TRUE;
	}
	return FALSE;
}

CChatting* CChattingMng::GetChttingRoom( u_long uidChtting )
{
	C2CharttingPtr::iterator i = m_2ChatPtr.find( uidChtting );
	if( i != m_2ChatPtr.end() )
		return i->second;
	return NULL;
}

CChattingMng	g_ChattingMng;

