#include "stdafx.h"
#include "clientMsg.h"
#include "timer.h"


CClientMsg g_ClientMsg;

/////////////////

// CClientMsg::m_textArray is a
// std::vector<std::unique_ptr<CLIENTMSG>>
// because CLIENTMSG contains a CEditString
// and CEditString copy / etc is a bit messy

void CClientMsg::ClearAllMessage() {
	m_textArray.clear();
}

void CClientMsg::Render(const CPoint point, C2DRender* p2DRender ) {
	if (m_textArray.empty()) return;

	static constexpr int m_nLineSpace = -3;
	
	int nLine = 0; 
	const int nSize = static_cast<int>(m_textArray.size());
	for (auto it = m_textArray.rbegin(); it != m_textArray.rend(); ++it) {
		const auto & lpClientMsg = *it;

		LPCTSTR lpStr = lpClientMsg->m_string;
		CSize size;
		p2DRender->m_pFont->GetTextExtent(lpStr,&size);
		nLine -= size.cy + m_nLineSpace;

		CPoint pt = point;
		pt.x -= size.cx / 2;
		pt.y += ( ( nSize * ( size.cy + m_nLineSpace ) ) / 2 ) + nLine;
		lpClientMsg->m_string.SetColor( ( lpClientMsg->m_nAlpha << 24 ) | ( lpClientMsg->m_dwRGB & 0x00ffffff ) );

		p2DRender->TextOut_EditString( pt.x, pt.y, lpClientMsg->m_string );
	}

	/* if (!m_textArray.empty()) */ {
		const auto & frontClientMsg = m_textArray.front();

		if (frontClientMsg->m_nAlpha != 255) {
			frontClientMsg->m_nAlpha -= 10;

			if (frontClientMsg->m_nAlpha < 0) {
				m_textArray.erase(m_textArray.begin());
			}
		}
	}

	if (!m_textArray.empty() && m_timer.Over()) {
		m_timer.Reset();

		const auto & frontClientMsg = m_textArray.front();

		if (frontClientMsg->m_nAlpha == 255) {
			frontClientMsg->m_nAlpha = 254;
		}
	}
}

void CClientMsg::AddMessage(LPCTSTR lpszMessage, DWORD RGB, BOOL bEffect) {
	auto & lpClientMsg = m_textArray.emplace_back(std::make_unique<CLIENTMSG>());
	lpClientMsg->m_dwRGB = RGB;
	lpClientMsg->m_string.SetParsingString(lpszMessage, 0xffffffff);
	lpClientMsg->m_nAlpha = 255;

	static constexpr size_t nMax = 6;
	if (m_textArray.size() > nMax) {
		m_textArray.erase(m_textArray.begin());
	}

	m_timer.Set(3000);
}
