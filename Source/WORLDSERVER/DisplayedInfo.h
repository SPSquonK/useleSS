#pragma once

#include <array>
#include "StaticString.h"

enum LOGTYPE {
	LOGTYPE_TIME,
	LOGTYPE_CCU,
	LOGTYPE_PERFOMANCE,
	LOGTYPE_RESPAWN,
	LOGTYPE_WARN1,
	LOGTYPE_WARN2,
	LOGTYPE_MAX
};

class CDisplayedInfo {
public:
	template<typename ... Ts>
	void SetLogInfo(LOGTYPE type, LPCTSTR lpszFormat, Ts && ... ts) {
		g_szBuffer[type].Format(lpszFormat, std::forward<Ts>(ts)...);
	}

	void SetListOfMaps(std::string s) { m_listOfMaps = std::move(s); }

  void Paint(HDC & hDC);
	void Redraw();

private:
	std::array<StaticString<256>, LOGTYPE_MAX> g_szBuffer;
	std::string m_listOfMaps;
};

extern CDisplayedInfo g_DisplayedInfo;
