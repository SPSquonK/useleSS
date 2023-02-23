#include "stdafx.h"
#include "WantedListSnapshot.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// CWantedListSnapshot
//////////////////////////////////////////////////////////////////////

CWantedListSnapshot & CWantedListSnapshot::GetInstance() {
	static CWantedListSnapshot wantedList;
	return wantedList;
}

CAr & operator>>(CAr & ar, CWantedListSnapshot & self) {
	self.m_wantedList.clear();

	ar >> self.m_lRecvTime;

	int nCount;
	ar >> nCount;						
	for (int i = 0; i < nCount; i++) {
		WANTED_ENTRY entry;
		ar.ReadString(entry.szPlayer);
		ar >> entry.nGold;
		ar >> entry.nEnd;
		ar.ReadString(entry.szMsg);

		self.m_wantedList.push_back(entry);
	}

	return ar;
}

CAr & operator<<(CAr & ar, const CWantedListSnapshot & self) {
	ar << self.m_lRecvTime;

	ar << static_cast<int>(self.m_wantedList.size());
	for (const WANTED_ENTRY & entry : self.m_wantedList) {
		ar.WriteString(entry.szPlayer);
		ar << entry.nGold;
		ar << entry.nEnd;
		ar.WriteString(entry.szMsg);
	}

	return ar;
}

bool CWantedListSnapshot::IsWantedPlayer(LPCTSTR lpszPlayer) const {
	return std::any_of(m_wantedList.begin(), m_wantedList.end(), 
		[lpszPlayer](const WANTED_ENTRY & wanted) {
			return strcmp(lpszPlayer, wanted.szPlayer) == 0;
		}
	);
}
