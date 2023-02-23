#pragma once

#include "misc.h"		// WANTED_ENTRY

// List of wanted players (Not updated in real time)
class CWantedListSnapshot final {
private:
	long                      m_lRecvTime = 0; // Time the list was received
	std::vector<WANTED_ENTRY> m_wantedList;

public:
	static CWantedListSnapshot & GetInstance();
	friend CAr & operator<<(CAr & ar, const CWantedListSnapshot & self);
	friend CAr & operator>>(CAr & ar,       CWantedListSnapshot & self);
	
	[[nodiscard]] bool IsWantedPlayer(LPCTSTR lpszPlayer) const;
};
