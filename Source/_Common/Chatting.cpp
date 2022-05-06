#include "stdafx.h"
#include "Chatting.h"

#ifdef __CLIENT
CChatting g_Chatting;
#endif

#ifdef __WORLDSERVER
CChattingMng g_ChattingMng;
#endif

#pragma region Chatting

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

#pragma endregion




#pragma region ChattingMng

#ifdef __WORLDSERVER

//////////////////////////////////////////////////////////////////////////////////////////////////
//		ChattingMng
//////////////////////////////////////////////////////////////////////////////////////////////////

CChatting * CChattingMng::NewChattingRoom(const u_long uChattingId) {
	const auto p = m_2ChatPtr.insert_or_assign(uChattingId, std::make_unique<CChatting>());
	return p.first->second.get();
}

bool CChattingMng::DeleteChattingRoom(const u_long uChattingId) {
	return m_2ChatPtr.erase(uChattingId) > 0;
}

CChatting * CChattingMng::GetChttingRoom(const u_long uidChtting) {
	const auto i = m_2ChatPtr.find(uidChtting);
	if (i == m_2ChatPtr.end()) return nullptr;
	return i->second.get();
}

#endif

#pragma endregion

