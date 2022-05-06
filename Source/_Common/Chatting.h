#pragma once

#include <map>
#include <memory>
#include <boost/container/small_vector.hpp>


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
	static constexpr size_t MAX_CHATTINGMEMBER = 32;
	boost::container::small_vector<ChatMember, MAX_CHATTINGMEMBER> m_members;

	BOOL	m_bState;
	//	Constructions

	bool AddChattingMember(const ChatMember & newMember);
	bool RemoveChattingMember(u_long uidPlayer);
	[[nodiscard]] bool IsChattingMember(u_long uidPlayer) const noexcept;

	void ClearMember( void );
	const auto & GetMembers() const { return m_members; }

#ifdef __CLIENT
	const ChatMember * GetMember(u_long uIdPlayer) const noexcept;
#endif
};

#ifdef __WORLDSERVER

class CChattingMng final {
	std::map<u_long, std::unique_ptr<CChatting>> m_2ChatPtr;
public:
	void Clear() { m_2ChatPtr.clear(); }
	CChatting * NewChattingRoom(u_long uChattingId);
	bool DeleteChattingRoom(u_long uChattingId);
	CChatting* GetChttingRoom( u_long uidChtting );
};

extern CChattingMng g_ChattingMng;

#endif
