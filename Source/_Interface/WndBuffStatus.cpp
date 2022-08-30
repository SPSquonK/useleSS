#include "StdAfx.h"
#include "WndBuffStatus.h"
#include <format>
#include "AppDefine.h"
#include "defineSkill.h"


CWndBuffStatus::CWndBuffStatus() {
	m_verticalDisplay = g_Option.m_BuffStatusMode == 1;
}

void CWndBuffStatus::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	DelWndStyle(WBS_CAPTION);

	m_wndTitleBar.SetVisible(FALSE);
	SetBuffIconInfo(true);

	const CRect rectRoot = m_pWndRoot->GetLayoutRect();
	const CPoint point(rectRoot.Width() / 3, rectRoot.Height() / 3);
	Move(point);
}

BOOL CWndBuffStatus::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_BUFF_STATUS, pWndParent, WBS_NOFOCUS, CPoint(0, 0));
}

void CWndBuffStatus::OnLButtonUp(UINT nFlags, CPoint point) {
	m_pWndFocus = this;
}

void CWndBuffStatus::OnLButtonDown(UINT nFlags, CPoint point) {
	if (!GetHitTestResult()) {
		if (g_WndMng.m_pWndWorld) {
			const CRect rect = GetWindowRect(TRUE);
			const CPoint p = point + rect.TopLeft();
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown(nFlags, p);
			this->m_pWndFocus = g_WndMng.m_pWndWorld;
		}
	}
}

void CWndBuffStatus::OnRButtonDown(UINT nFlags, CPoint point) {
	if (!GetHitTestResult()) {
		if (g_WndMng.m_pWndWorld) {
			const CRect rect = GetWindowRect(TRUE);
			const CPoint p = point + rect.TopLeft();
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown(nFlags, p);
		}
	}
}

void CWndBuffStatus::OnLButtonDblClk(UINT nFlags, CPoint point) {
	if (GetHitTestResult()) {
		m_verticalDisplay = !m_verticalDisplay;
		g_Option.m_BuffStatusMode = m_verticalDisplay ? 1 : 0;
		SetBuffIconInfo(true);
	} else {
		if (g_WndMng.m_pWndWorld) {
			const CRect rect = GetWindowRect(TRUE);
			const CPoint p = point + rect.TopLeft();
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDblClk(nFlags, p);
		}
	}
}

void CWndBuffStatus::OnRButtonDblClk(UINT nFlags, CPoint point) {
	if (GetHitTestResult()) {
		m_verticalDisplay = !m_verticalDisplay;
		g_Option.m_BuffStatusMode = m_verticalDisplay ? 1 : 0;
		SetBuffIconInfo(true);
	} else {
		if (g_WndMng.m_pWndWorld) {
			const CRect rect = GetWindowRect(TRUE);
			const CPoint p = point + rect.TopLeft();
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDblClk(nFlags, p);
		}
	}
}

bool CWndBuffStatus::GetHitTestResult() {
	CPoint ptMouse = GetMousePoint();
	ClientToScreen(&ptMouse);

	CPointGenerator generator = CPointGenerator(m_verticalDisplay);

	for (IBuff * pBuff : g_pPlayer->m_buffs.m_mapBuffs | std::views::values) {
		if (pBuff->GetType() == BUFF_SKILL) {
			CRect rectHittest(generator.Next(), CSize(34, 34));
			ClientToScreen(&rectHittest);
			if (PtInRect(&rectHittest, ptMouse)) {
				return true;
			}
		}
	}

	return false;
}

void CWndBuffStatus::SetBuffIconInfo(bool force) {
	if (!force && m_lastSeenSize == g_pPlayer->m_buffs.m_mapBuffs.size()) return;

	m_lastSeenSize = g_pPlayer->m_buffs.m_mapBuffs.size();

	int numberOfRows = static_cast<int>(g_pPlayer->m_buffs.m_mapBuffs.size() / 7);
	if (g_pPlayer->m_buffs.m_mapBuffs.size() % 7 != 0) {
		numberOfRows += 1;
	}

	CRect rect = GetWindowRect(TRUE);

	if (m_verticalDisplay) {
		rect.bottom = (7 * 34) + rect.top;
		rect.right = (numberOfRows * 34) + rect.left;
	} else {
		rect.right = (7 * 34) + rect.left;
		rect.bottom = (numberOfRows * 34) + rect.top;
	}

	SetWndRect(rect);
	AdjustWndBase();
}

void CWndBuffStatus::PaintFrame(C2DRender * p2DRender) {
	/* Override CWndBase Paint Frame that actually does things */
}

void CWndBuffStatus::OnDraw(C2DRender * p2DRender) {
	if (g_pPlayer == NULL) return;

	CPoint ptMouse = GetMousePoint();
	ClientToScreen(&ptMouse);

	SetBuffIconInfo(false);

	CPointGenerator generator = CPointGenerator(m_verticalDisplay);

	for (IBuff * pBuff : g_pPlayer->m_buffs.m_mapBuffs | std::views::values) {
		if (pBuff->GetType() == BUFF_SKILL) {
			RenderBuffIcon(p2DRender, pBuff, generator.Next(), ptMouse);
		}
	}
}

void CWndBuffStatus::RenderBuffIcon(C2DRender * p2DRender, IBuff * pBuff, const CPoint buffPosition, const CPoint ptMouse) {
	const ItemProp * pItem;
	std::multimap< DWORD, BUFFSKILL >::iterator pp;

	WORD wID = pBuff->GetId();
	if (pBuff->GetType() == BUFF_SKILL) {
		pp = g_WndMng.m_pWndWorld->m_pBuffTexture[0].find(pBuff->GetId());

		if (pp == g_WndMng.m_pWndWorld->m_pBuffTexture[0].end())
			return;

		pItem = prj.GetSkillProp(pBuff->GetId());
	} else {
		pp = g_WndMng.m_pWndWorld->m_pBuffTexture[2].find(pBuff->GetId());

		if (pp == g_WndMng.m_pWndWorld->m_pBuffTexture[2].end())
			return;

		pItem = prj.GetItemProp(wID);
	}

	ASSERT(pItem);
	if (pp->second.m_pTexture == NULL)
		return;

	// Check time for flashing
	BOOL bFlash = FALSE;
	DWORD dwOddTime = 0;

	if (pBuff->GetTotal() > 0) {
		dwOddTime = pBuff->GetTotal() - (g_tmCurrent - pBuff->GetInst());
		bFlash = (dwOddTime < 20 * 1000);		// Blinks when less than 20 seconds remain

		if (pBuff->GetTotal() < (g_tmCurrent - pBuff->GetInst())) {
			dwOddTime = 0;
		}
	}

	// Draw icon
	int alpha = 192;
	if (bFlash) {
		alpha = pp->second.m_flasher.Increment();
	}

	D3DXCOLOR color;
	if (pItem->nEvildoing < 0)
		color = D3DCOLOR_ARGB(alpha, 255, 120, 255);
	else
		color = D3DCOLOR_ARGB(alpha, 255, 255, 255);

	p2DRender->RenderTexture2(buffPosition, pp->second.m_pTexture, 1.2f, 1.2f, color);

	// Time
	if (dwOddTime != 0) {
		RenderOptBuffTime(p2DRender, buffPosition, dwOddTime, D3DCOLOR_XRGB(255, 255, 255));
	}

	// Tooltip
	CRect rectHittest(buffPosition, CSize(28, 28));
	ClientToScreen(&rectHittest);

	if (!rectHittest.PtInRect(ptMouse)) return;

	CEditString strEdit;
	CString strLevel;
	strLevel.Format("   Lvl %d", pBuff->GetLevel());

	if (pItem->dwItemRare == 102) {
		strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
		strEdit.AddString(strLevel, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
	} else if (pItem->dwItemRare == 103) {
		strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
		strEdit.AddString(strLevel, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
	} else {
		strEdit.AddString(pItem->szName, 0xff2fbe6d, ESSTY_BOLD);
		strEdit.AddString(strLevel, 0xff2fbe6d, ESSTY_BOLD);
	}

	CString str;

	if (pBuff->GetTotal() > 0) {
		CTimeSpan ct((long)(dwOddTime / 1000.0f));		// Converts remaining time to seconds
		str = TimeSpanToString::DHMmSs(ct);
	}

	CString strTemp;
	strTemp.Format("\n%s", pItem->szCommand);

	strEdit.AddString(strTemp);

	CWndMgr::CTooltipBuilder::PutDestParam(pItem->dwDestParam[0], pItem->dwDestParam[1],
		pItem->nAdjParamVal[0], pItem->nAdjParamVal[1], strEdit);

	bool show = true;

	if (pBuff->GetType() == BUFF_SKILL) {

		switch (pBuff->GetId()) {
			case SI_RIG_MASTER_BLESSING:
			case SI_ASS_CHEER_STONEHAND:
			case SI_MAG_EARTH_LOOTING:
			case SI_ASS_HEAL_PREVENTION:
				show = false;
				break;
			case SI_FLO_SUP_ABSOLUTE:
				CWndMgr::CTooltipBuilder::PutDestParam(
					SINGLE_DST{
						.nDst = DST_IGNORE_DMG_PVP,
						.nAdj = g_pPlayer->GetParam(DST_IGNORE_DMG_PVP, 0)
					},
					strEdit
				);
				show = false;
				break;
		}
	}

	if (show) {
		const AddSkillProp * pAddSkillProp = prj.GetAddSkillProp(pItem->dwSubDefine, pBuff->GetLevel());

		if (pAddSkillProp) {
			CWndMgr::CTooltipBuilder::PutDestParam(pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
				pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit);
		}
	}

	strEdit.AddString(str);

	g_toolTip.PutToolTip(wID, strEdit, rectHittest, ptMouse, 1);
}

void CWndBuffStatus::RenderOptBuffTime(C2DRender * p2DRender, const CPoint & point, const DWORD timeLeft, DWORD dwColor) {
	if (g_Option.m_bVisibleBuffTimeRender) {
		char buffer[65];
		const auto r = std::format_to_n(buffer, std::size(buffer) - 1, "{}", timeLeft / 1000);
		*r.out = '\0';
		p2DRender->TextOut(point.x + 2, point.y + 22, buffer, dwColor, 0xFF000000);
	}
}

CPoint CWndBuffStatus::CPointGenerator::Next() {
	CPoint retval = m_point;

	m_point.x += 34;
	if (m_point.x >= 34 * 7) {
		m_point.x -= 34 * 7;
		m_point.y += 34;
	}

	if (m_verticalAlign) {
		return CPoint(retval.y, retval.x);
	} else {
		return retval;
	}
}
