#include "StdAfx.h"
#include "DisplayedInfo.h"
#include <ranges>

CDisplayedInfo g_DisplayedInfo;
extern HWND g_hMainWnd;

void CDisplayedInfo::Paint(HDC & hDC) {
	int x, y;
	x = 0;
	y = 0;

	for (int i = 0; i < LOGTYPE_MAX; ++i) {
		TextOut(hDC, x, y, g_szBuffer[i], strlen(g_szBuffer[i]));
		
		y += 20;
	}

	{
		RECT rcCli;
		GetClientRect(WindowFromDC(hDC), &rcCli);
		rcCli.top = y;
		DrawText(hDC, m_listOfMaps.c_str(), -1, &rcCli, DT_WORDBREAK);
	}

#ifdef __NEW_PROFILE
	if (CProfileInfo::GetInstance()->IsToggleProfiling()) {
		TextOut(hDC, x, y, "Profiler : ON", 13);
		y += 20;
	}
	/*
	for( i=0; i<CProfileInfo::GetInstance()->m_vecstrProfileInfo.size(); i++ )
	{
		TextOut( hDC, x, y, CProfileInfo::GetInstance()->m_vecstrProfileInfo[i].c_str(), CProfileInfo::GetInstance()->m_vecstrProfileInfo[i].length() );
		y += 20;
	}
	*/
#endif // __NEW_PROFILE
}

void CDisplayedInfo::Redraw() {
	InvalidateRect(g_hMainWnd, NULL, TRUE);
}

static std::string WorldsIdsToString(const std::vector<DWORD> & ids);

void CDisplayedInfo::SetListOfMaps(std::vector<std::pair<DWORD, std::string>> worlds) {
	std::map<DWORD, std::string> worldIdToWorldName;
	std::map<std::string, DWORD> worldNameToFirstWorldId;

	for (const auto & [worldId, worldName] : worlds) {
		worldIdToWorldName[worldId] = worldName;

		auto it = worldNameToFirstWorldId.find(worldName);
		if (it == worldNameToFirstWorldId.end()) {
			worldNameToFirstWorldId.insert_or_assign(worldName, worldId);
		} else if (it->second > worldId) {
			it->second = worldId;
		}
	}

	// This is "slow", but we only execute this function once with less than 200 values
	// so the O(n^2 * m) complexity is ok (n = number of worlds, m = longest world name)

	std::string str;

	for (const auto & [worldId, worldName] : worldIdToWorldName) { // O(n)
		const auto it = worldNameToFirstWorldId.find(worldName);
		if (it == worldNameToFirstWorldId.end()) continue; // impossible by design but ok
		if (it->second != worldId) continue; // already displayed

		std::vector<DWORD> worldsIds;
		for (const auto & [otherWorldId, otherWorldName] : worldIdToWorldName) { // O(n)
			if (worldName != otherWorldName) continue;

			worldsIds.push_back(otherWorldId);
		}

		str += WorldsIdsToString(worldsIds) + "=" + worldName + " ";
	}

	m_listOfMaps = str;
}

static bool IsContiguous(const std::vector<DWORD> & values) {
	if (values.size() == 0) return true;
	if (values.size() == 1) return true;

	auto previous = values[0];

	for (const auto value : values | std::views::drop(1)) {
		if (value - 1 != previous) return false;
		previous = value;
	}

	return true;
}

static std::string WorldsIdsToString(const std::vector<DWORD> & ids) {
	std::string res = "[";

	if (ids.size() != 0) {
		res += std::to_string(ids[0]);

		if (ids.size() > 5 && IsContiguous(ids)) {
			res += ":";
			res += std::to_string(ids.back());
		} else {
			for (const DWORD id : ids | std::views::drop(1)) {
				res += ",";
				res += std::to_string(id);
			}
		}
	}

	res += "]";

	return res;
}
