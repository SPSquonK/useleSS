#pragma once

#include "timer.h"
#include <memory>

struct CLIENTMSG {
	CEditString m_string;
	DWORD   m_dwRGB;
	int m_nAlpha;
};

class CClientMsg {
	CTimer m_timer; 
	int    m_nBeginLine = 0;
	std::vector<std::unique_ptr<CLIENTMSG>> m_textArray;
public:
	void Render(CPoint point, C2DRender * p2DRender);
	void AddMessage(LPCTSTR lpszMessage, DWORD RGB, BOOL bEffect = FALSE);
	void ClearAllMessage();
};

extern CClientMsg g_ClientMsg;
