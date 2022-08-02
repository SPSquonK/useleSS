#include "stdafx.h"
#include "flyffevent.h"

CFlyffEvent::CFlyffEvent() {
	m_aEvent.fill(0);
}

bool CFlyffEvent::IsOutOfRange(const int id) noexcept {
	return id < 0 || id >= MAX_EVENT;
}

BOOL CFlyffEvent::SetState(const int id, const BYTE nState) {
	if (IsOutOfRange(id)) return FALSE;
	m_aEvent[id] = nState;
	return TRUE;
}

BYTE CFlyffEvent::GetState(const int id) const {
	if (IsOutOfRange(id)) return static_cast<BYTE>(0);
	return m_aEvent[id];
}

CAr & operator<<(CAr & ar, const CFlyffEvent & self) {
	return ar << self.m_aEvent;
}

CAr & operator>>(CAr & ar, CFlyffEvent & self) {
	return ar >> self.m_aEvent;
}
