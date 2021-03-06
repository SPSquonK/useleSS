#include "StdAfx.h"
#include "DisplayedInfo.h"
#include <algorithm>
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

	if (!m_connectedTo.database || !m_connectedTo.core) {
		const std::string_view text = m_connectedTo.GetText();

		const auto originalColor = GetTextColor(hDC);
		SetTextColor(hDC, RGB(255, 0, 0));
		TextOutA(hDC, 200, 0, text.data(), text.size());
		SetTextColor(hDC, originalColor);
	}

	y += 10;

	if (m_invalidWorlds != "") {
		const auto originalColor = GetTextColor(hDC);
		SetTextColor(hDC, RGB(255, 0, 0));
		TextOutA(hDC, x, y, m_invalidWorlds.c_str(), m_invalidWorlds.size());
		SetTextColor(hDC, originalColor);
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

std::string_view CDisplayedInfo::ConnectedTo::GetText() const {
	if (database) {
		if (core) {
			return "Ok";
		} else {
			return "Core KO";
		}
	} else {
		if (core) {
			return "DB KO";
		} else {
			return "DB KO / Core KO";
		}
	}
}

void CDisplayedInfo::Redraw() {
	InvalidateRect(g_hMainWnd, NULL, TRUE);
}

static std::string WorldsIdsToString(const std::vector<DWORD> & ids);

void CDisplayedInfo::SetListOfMaps(
	std::vector<std::pair<DWORD, std::string>> worlds,
	std::vector<DWORD> invalidWorlds
) {
	m_listOfMaps = ExistingWorldsToString(worlds);

	std::sort(invalidWorlds.begin(), invalidWorlds.end());
	m_invalidWorlds = InvalidWorldsToString(invalidWorlds);
}

std::string CDisplayedInfo::ExistingWorldsToString(const std::vector<std::pair<DWORD, std::string>> & worlds) {
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

	return str;
}


std::string DWordsToString(const std::vector<DWORD> & values, const char * const separator) {
	if (values.size() == 0) return "";

	std::string res = std::to_string(values[0]);

	for (const DWORD id : values | std::views::drop(1)) {
		res += separator;
		res += std::to_string(id);
	}

	return res;
}

std::string CDisplayedInfo::InvalidWorldsToString(const std::vector<DWORD> & invalidWorlds) {
	if (invalidWorlds.size() == 0) return "";
	return "/!\\ Invalid worlds: " + DWordsToString(invalidWorlds, ", ");
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
		if (ids.size() > 5 && IsContiguous(ids)) {
			res += std::to_string(ids[0]) + ":" + std::to_string(ids.back());
		} else {
			res += DWordsToString(ids, ",");
		}
	}

	res += "]";

	return res;
}
