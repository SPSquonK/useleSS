#include "StdAfx.h"
#include "DisplayedInfo.h"

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
