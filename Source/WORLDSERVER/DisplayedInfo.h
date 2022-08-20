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
	struct ConnectedTo {
		bool database = false;
		bool core = false;

		[[nodiscard]] std::string_view GetText() const;
	};

	template<typename ... Ts>
	void SetLogInfo(LOGTYPE type, LPCTSTR lpszFormat, Ts && ... ts) {
		g_szBuffer[type].Format(lpszFormat, std::forward<Ts>(ts)...);
	}

	void SetListOfMaps(
		std::vector<std::pair<WorldId, std::string>> worlds,
		std::vector<WorldId> invalidWorlds
	);

	void UpdateConnectionState(ConnectedTo connectedTo) { m_connectedTo = connectedTo; }

  void Paint(HDC & hDC);
	void Redraw();

private:
	[[nodiscard]] static std::string ExistingWorldsToString(const std::vector<std::pair<WorldId, std::string>> & worlds);
	[[nodiscard]] static std::string InvalidWorldsToString(const std::vector<WorldId> & invalidWorlds);

private:
	std::array<StaticString<256>, LOGTYPE_MAX> g_szBuffer;

	ConnectedTo m_connectedTo;
	std::string m_listOfMaps;
	std::string m_invalidWorlds;
};

extern CDisplayedInfo g_DisplayedInfo;
