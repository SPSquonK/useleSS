#include "StdAfx.h"
#include "WndCharacter.h"
#include <algorithm>
#include <concepts>
#include <numeric>
#include "defineText.h"
#include "DPCertified.h"
#include "DPClient.h"
#include "party.h"
#include "AppDefine.h"


float GetAttackSpeedPlusValue(int n); // MoverAttack.cpp

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� ����
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////



struct ByLineDrawer {
	int y;
	int step;

	ByLineDrawer(int y, int step) : y(y), step(step) {}

	void DrawLine(const char *, std::invocable<int> auto func) {
		func(y);
		y += step;
	}
};

static DWORD DecideStatColor(auto displayedValue, auto statCheck, auto newValueGet) {
	if (!((g_nRenderCnt / 8) & 1)) return CWndCharInfo::RegularValueColor;
	if (!statCheck()) return CWndCharInfo::RegularValueColor;

	const auto newValue = newValueGet();
	if (displayedValue == newValue) return CWndCharInfo::RegularValueColor;
	return CWndCharInfo::ChangeValueColor;
};

void CWndCharInfo::OnDraw(C2DRender * p2DRender) {
	DrawCharacterBase(p2DRender);
	DrawStats(p2DRender);
	CheckAndDrawTooltip();
	DrawPvp(p2DRender);
}

void CWndCharInfo::DrawCharacterBase(C2DRender * p2DRender) {
	// Title
	p2DRender->TextOut(60, 13, prj.GetText((TID_APP_CHARACTER_BASE)), TitleColor);

	// Content
	ByLineDrawer characterBase(31, 15);

	characterBase.DrawLine("Name", [&](int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_01), LabelColor);
		p2DRender->TextOut(80, y, g_pPlayer->GetName(), RegularValueColor);
		});

	characterBase.DrawLine("Job", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_APP_CHARACTER_JOB), LabelColor);
		p2DRender->TextOut(80, y, g_pPlayer->GetJobString(), RegularValueColor);
		});

	characterBase.DrawLine("Level", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_02), LabelColor);
		p2DRender->TextOut(80, y, g_pPlayer->GetLevel(), RegularValueColor);

		const BYTE checkhero = g_pPlayer->GetLegendChar();
		if (checkhero == LEGEND_CLASS_NORMAL) return;

		const int xpos = g_pPlayer->GetLevel() < 100 ? 97 : 103;

		CPoint iconPoint(xpos, y - 2);
		CString strPath;

		if (checkhero == LEGEND_CLASS_MASTER) {
			if (g_pPlayer->m_nLevel < 70)
				strPath = MakePath(DIR_ICON, "Icon_MasterMark1.dds");
			else if (g_pPlayer->m_nLevel < 80)
				strPath = MakePath(DIR_ICON, "Icon_MasterMark2.dds");
			else if (g_pPlayer->m_nLevel < 90)
				strPath = MakePath(DIR_ICON, "Icon_MasterMark3.dds");
			else if (g_pPlayer->m_nLevel < 100)
				strPath = MakePath(DIR_ICON, "Icon_MasterMark4.dds");
			else if (g_pPlayer->m_nLevel < 110)
				strPath = MakePath(DIR_ICON, "Icon_MasterMark5.dds");
			else
				strPath = MakePath(DIR_ICON, "Icon_MasterMark6.dds");
		} else {
			strPath = MakePath(DIR_ICON, "Icon_HeroMark.dds");
		}

		CTexture * pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		if (pTexture) {
			pTexture->Render(p2DRender, iconPoint);
		}
		});

	characterBase.DrawLine("Server", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHAR_SERVER), LabelColor);
		p2DRender->TextOut(80, y, g_dpCertified.GetServerName(g_Option.m_nSer), RegularValueColor);
		});

	characterBase.DrawLine("Channel", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), LabelColor);
		if (CListedServers::Channel * channel = g_dpCertified.m_servers.GetChannelFromPos(g_Option.m_nSer, g_Option.m_nMSer)) {
			p2DRender->TextOut(80, y, channel->lpName, RegularValueColor);
		}
		});
}

void CWndCharInfo::DrawStats(C2DRender * p2DRender) {
	static constexpr int nyAdd = 121;

	// Title
	p2DRender->TextOut(60, 113, prj.GetText((TID_APP_CHARACTER_DETAIL)), D3DCOLOR_ARGB(255, 0, 0, 0));

	// Computed stats display
	ByLineDrawer computed(10 + nyAdd, 15);

	computed.DrawLine("ATK and Crit", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_13), LabelColor);
		RenderATK(p2DRender, 50, y);

		p2DRender->TextOut(85, y, prj.GetText(TID_GAME_CHARACTER_14), LabelColor);
		const int crit = GetVirtualCritical();
		CString strMsg;
		strMsg.Format("%d%%", crit);
		const DWORD color = DecideStatColor(crit,
			[&]() { return m_nDexCount != 0; },
			[&]() { return g_pPlayer->GetCriticalProb(); }
		);
		p2DRender->TextOut(140, y, strMsg, color);
		});

	computed.DrawLine("Def and AtkSpeed", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEFENCE), LabelColor);
		const auto virtualDef = GetVirtualDEF();
		const DWORD defColor = DecideStatColor(virtualDef,
			[&]() { return m_nStaCount != 0; },
			[&]() { return g_pPlayer->GetShowDefense(FALSE); }
		);
		p2DRender->TextOut(50, y, virtualDef, defColor);

		p2DRender->TextOut(85, y, prj.GetText(TID_GAME_CHARACTER_15), LabelColor);
		const float attackSpeed = GetVirtualATKSpeed();
		const DWORD asColor = DecideStatColor(attackSpeed,
			[&]() { return m_nDexCount != 0; },
			[&]() { return g_pPlayer->GetAttackSpeed(); }
		);
		CString strMsg;
		strMsg.Format("%d%%", int(attackSpeed * 100.0f) / 2);
		p2DRender->TextOut(140, y, strMsg, asColor);
		});

	// Raw stats
	ByLineDrawer rawStats(52 + nyAdd, 15);

	rawStats.DrawLine("Str", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STR), LabelColor);
		const DWORD dwColor = StatColor(g_pPlayer->m_nStr, g_pPlayer->GetStr());
		p2DRender->TextOut(50, y, g_pPlayer->GetStr(), dwColor);
		});

	rawStats.DrawLine("Sta", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STA), LabelColor);
		const DWORD dwColor = StatColor(g_pPlayer->m_nSta, g_pPlayer->GetSta());
		p2DRender->TextOut(50, y, g_pPlayer->GetSta(), dwColor);
		});

	rawStats.DrawLine("Int", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEX), LabelColor);
		const DWORD dwColor = StatColor(g_pPlayer->m_nDex, g_pPlayer->GetDex());
		p2DRender->TextOut(50, y, g_pPlayer->GetDex(), dwColor);
		});

	rawStats.DrawLine("Int", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_INT), LabelColor);
		const DWORD dwColor = StatColor(g_pPlayer->m_nInt, g_pPlayer->GetInt());
		p2DRender->TextOut(50, y, g_pPlayer->GetInt(), dwColor);
		});

	rawStats.DrawLine("GP", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_07), LabelColor);
		DWORD dwColor = RegularValueColor;
		if (m_nGpPoint != 0 && ((g_nRenderCnt % 8) == 1)) {
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		p2DRender->TextOut(105, y, m_nGpPoint, dwColor);
		});
}

void CWndCharInfo::DrawPvp(C2DRender * p2DRender) {
	// Title
	p2DRender->TextOut(60, 281, prj.GetText((TID_GAME_CHARACTTER_PVP0)), TitleColor);

	// Content
	ByLineDrawer pvp(15 + 284, 19);

	pvp.DrawLine("Fame", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTTER_PVP1), LabelColor);

		const char * fameName = g_pPlayer->GetFameName();
		if (fameName[0] == '\0') fameName = "-";
		p2DRender->TextOut(90, y, fameName, RegularValueColor);
		});

	pvp.DrawLine("PVP Points", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTTER_PVP2), LabelColor);
		p2DRender->TextOut(90, y, g_pPlayer->m_nFame, RegularValueColor);
		});

	pvp.y += 16;

	pvp.DrawLine("PK", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTTER_PVP3), LabelColor);
		p2DRender->TextOut(90, y, g_pPlayer->GetPKValue(), RegularValueColor);
		});

	pvp.DrawLine("Propency", [&](const int y) {
		p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTTER_PVP4), LabelColor);
		p2DRender->TextOut(90, y, g_pPlayer->GetPKPropensity(), RegularValueColor);
		});
}

void CWndCharInfo::CheckAndDrawTooltip() {
	static constexpr int nyAdd = 121;
	boost::container::small_vector<Hoverable, 10> hoverables;
	hoverables.emplace_back(CRect(10, 52 + nyAdd, 80, 65 + nyAdd), TooltipBox::Str);
	hoverables.emplace_back(CRect(10, 67 + nyAdd, 80, 80 + nyAdd), TooltipBox::Sta);
	hoverables.emplace_back(CRect(10, 82 + nyAdd, 80, 95 + nyAdd), TooltipBox::Dex);
	hoverables.emplace_back(CRect(10, 97 + nyAdd, 80, 110 + nyAdd), TooltipBox::Int);
	hoverables.emplace_back(CRect(10, 112 + nyAdd, 160, 125 + nyAdd), TooltipBox::Gp);
	hoverables.emplace_back(CRect(7, 10 + nyAdd, 160, 38 + nyAdd), TooltipBox::Status);
	hoverables.emplace_back(CRect(90, 52 + nyAdd, 160, 110 + nyAdd), TooltipBox::Edit);
	hoverables.emplace_back(m_wndApply.m_rectWindow, TooltipBox::Apply);
	hoverables.emplace_back(m_wndReset.m_rectWindow, TooltipBox::Reset);

	CPoint ptMouse = GetMousePoint();

	const auto it = std::ranges::find_if(hoverables,
		[ptMouse](const CWndCharInfo::Hoverable & hoverable) {
			return hoverable.rect.PtInRect(ptMouse);
		}
	);

	if (it == hoverables.end()) return;

	const TooltipBox tooltipType = it->box;

	ClientToScreen(&ptMouse);
	CRect rect = it->rect;
	ClientToScreen(&rect);


	static constexpr DWORD dwColorName = D3DCOLOR_XRGB(0, 93, 0);
	static constexpr DWORD dwColorCommand = D3DCOLOR_XRGB(180, 0, 0);
	CEditString strEdit;
	CString szString;

	switch (tooltipType) {
		case TooltipBox::Str: {
			szString.Format("%s", prj.GetText(TID_TOOLTIP_STR));
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			szString.Format("  %d\n ", g_pPlayer->GetStr());
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STR0));
			strEdit.AddString("(");
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STR1), dwColorCommand);
			strEdit.AddString(")");
			break;
		}
		case TooltipBox::Sta: {
			szString.Format("%s", prj.GetText(TID_TOOLTIP_STA));
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			szString.Format("  %d\n ", g_pPlayer->GetSta());
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STA0));
			strEdit.AddString("\n ");
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STA1));
			break;
		}
		case TooltipBox::Dex: {
			szString.Format("%s", prj.GetText(TID_TOOLTIP_DEX));
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			szString.Format("  %d\n ", g_pPlayer->GetDex());
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX0));
			strEdit.AddString("\n ");
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX1));
			strEdit.AddString("\n ");
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX2));
			strEdit.AddString("\n ");
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX3), dwColorCommand);
			break;
		}
		case TooltipBox::Int: {
			szString.Format("%s", prj.GetText(TID_TOOLTIP_INT));
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			szString.Format("  %d\n ", g_pPlayer->GetInt());
			strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_INT0));
			strEdit.AddString("\n ");
			strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_INT1));
			break;
		}
		case TooltipBox::Gp: {
			strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_GPPOINT));
			break;
		}
		case TooltipBox::Status: {
			if (m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
				strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_STATUS1));
			else
				strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_STATUS2));
			break;
		}
		case TooltipBox::Edit: {
			strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_EDIT));
			break;
		}
		case TooltipBox::Apply: {
			strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_APPLY));
			break;
		}
		case TooltipBox::Reset: {
			strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_RESET));
			break;
		}
	}

	g_toolTip.PutToolTip(100, strEdit, rect, ptMouse, 3);
}

DWORD CWndCharInfo::StatColor(const int rawStat, const int totalStat) {
	if (rawStat == totalStat) {
		return D3DCOLOR_ARGB(255, 0, 0, 0);
	} else if (rawStat < totalStat) {
		return D3DCOLOR_ARGB(255, 0, 0, 255);
	} else {
		return D3DCOLOR_ARGB(255, 255, 0, 0);
	}
}


void CWndCharInfo::OnInitialUpdate() {

	CWndBase::OnInitialUpdate();
	SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("WndNewCharacter01.tga")), TRUE);

	// �Ʒ����� �ɷ�ġ ���� 
	int nyAdd = 121;
	int posY = 49 + nyAdd;
	int posX = 128;

	m_editStrCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 100);
	m_wndStrPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 101);
	m_wndStrMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 102); posY += 15;

	m_editStaCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 103);
	m_wndStaPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 104);
	m_wndStaMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 105); posY += 15;

	m_editDexCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 106);
	m_wndDexPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 107);
	m_wndDexMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 108); posY += 15;

	m_editIntCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 109);
	m_wndIntPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 110);
	m_wndIntMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 111);

	m_wndStrPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndStrMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndStaPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndStaMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndDexPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndDexMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndIntPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndIntMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);

	posY += 36;
	m_wndApply.Create("Apply", 0, CRect(posX - 108, posY, posX - 58, posY + 22), this, 112);
	m_wndReset.Create("Reset", 0, CRect(posX - 30, posY, posX + 20, posY + 22), this, 113);

	if (::GetLanguage() == LANG_FRE) {
		m_wndApply.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButStateOk.tga")), TRUE);
		m_wndReset.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButStateCancel.tga")), TRUE);
	} else {
		m_wndApply.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharApply.tga")), TRUE);
		m_wndReset.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharReset.tga")), TRUE);
	}

	//���? ������ ������ 1�� �����̹Ƿ� �Ʒ� ����

	m_nGpPoint = g_pPlayer->GetRemainGP();
	//���� ������ ���� �����Ƿ� Minus Button Default�� False
	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if (g_pPlayer->GetRemainGP() <= 0) {
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}
	int nyAdd2 = 280;
	int y = 105 + nyAdd2;
	if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER)) {
		static constexpr int yJob = 10 + 21 + 15;
		m_wndChangeJob.Create("E", 0, CRect(175 - 13, yJob + 1, 175, yJob + 1 + 13), this, 10);
		m_wndChangeJob.EnableWindow(TRUE);
	}

	RefreshStatPoint();

	MakeVertexBuffer();
}

BOOL CWndCharInfo::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	int editnum = 0;

	if (nID == 100 || nID == 103 || nID == 106 || nID == 109) {
		CString tempnum = m_editStrCount.GetString();
		for (int i = 0; i < tempnum.GetLength(); i++) {
			if (isdigit2(tempnum.GetAt(i)) == FALSE)
				return FALSE;
		}
	}
	switch (nID) {
		case 100:
			editnum = atoi(m_editStrCount.GetString());
			if (editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
			m_nStrCount = editnum;
			break;
		case 103:
			editnum = atoi(m_editStaCount.GetString());
			if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
			m_nStaCount = editnum;
			break;
		case 106:
			editnum = atoi(m_editDexCount.GetString());
			if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
			m_nDexCount = editnum;
			break;
		case 109:
			editnum = atoi(m_editIntCount.GetString());
			if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
			m_nIntCount = editnum;
			break;
	}

	if (message == WNM_CLICKED) {
		switch (nID) {
			case 101: //Str Plus
				m_nStrCount++;
				break;
			case 102: //Str Minus
				m_nStrCount--;
				break;
			case 104: //Sta Plus
				m_nStaCount++;
				break;
			case 105: //Sta Minus
				m_nStaCount--;
				break;
			case 107: //Dex Plus
				m_nDexCount++;
				break;
			case 108: //Dex Minus
				m_nDexCount--;
				break;
			case 110: //Int Plus
				m_nIntCount++;
				break;
			case 111: //Int Minus
				m_nIntCount--;
				break;
			case 112: //Apply
				if (m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) {
					SAFE_DELETE(g_WndMng.m_pWndStateConfirm);
					g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
					g_WndMng.m_pWndStateConfirm->Initialize();
				}
				break;
			case 113: //Reset
				m_nStrCount = 0;
				m_nStaCount = 0;
				m_nDexCount = 0;
				m_nIntCount = 0;
				RefreshStatPoint();
				break;

			case JOB_MERCENARY:
			case JOB_ACROBAT:
			case JOB_ASSIST:
			case JOB_MAGICIAN:
			case JOB_PUPPETEER:
			{
				if (nID != g_pPlayer->GetJob()) {
					//"�ڽ��� ������ �ø��� �ֽ��ϴ�"
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0037)));
				}
				if (m_fWaitingConfirm == FALSE) {
					g_DPlay.SendIncJobLevel(nID);
					m_fWaitingConfirm = TRUE;
				}
				break;
			}
			case 10: // ���� 
				// �������? ������ 15�̻��ΰ��� ã��
				CWndChangeClass1::OpenWindow(std::nullopt);
				break;
		}
	}

	if (nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if (g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount)) {
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		} else if (g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount)) {
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}

	return CWndBase::OnChildNotify(message, nID, pLResult);
}


BOOL CWndCharInfo::Process() {
	//Control Button Plus or Minus
	if (m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if (m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if (m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if (m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if (m_nGpPoint > 0) {
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);

		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	} else {
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);

		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if (m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0) {
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	} else {
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}


void CWndCharInfo::RefreshStatPoint() {
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}


std::pair<int, int> CWndCharInfo::GetVirtualATK() const {
	if (!g_pPlayer) return { 0, 0 };

	const ItemProp * const pItemProp = g_pPlayer->GetActiveHandItemProp(PARTS_RWEAPON);
	if (!pItemProp) return { 0, 0 };

	int min = pItemProp->dwAbilityMin * 2;
	int max = pItemProp->dwAbilityMax * 2;

	min = g_pPlayer->GetParam(DST_ABILITY_MIN, min);
	max = g_pPlayer->GetParam(DST_ABILITY_MAX, max);

	int nATK = 0;
	switch (pItemProp->dwWeaponType) {
		case WT_MELEE_SWD:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_MELEE_AXE:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)));
			break;
		case WT_MELEE_STAFF:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_MELEE_STICK:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)));
			break;
		case WT_MELEE_KNUCKLE:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)));
			break;
		case WT_MAGIC_WAND:
			nATK = (int)((g_pPlayer->GetInt() + m_nIntCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f);
			break;
		case WT_MELEE_YOYO:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_RANGE_BOW:
			nATK = (int)((((g_pPlayer->GetDex() + m_nDexCount - 14) * 4.0f + (g_pPlayer->GetLevel() * 1.3f) + ((g_pPlayer->GetStr() + m_nStrCount) * 0.2f)) * 0.7f));
			break;
	}

	nATK += g_pPlayer->GetPlusWeaponATK(pItemProp->dwWeaponType);

	const int nPlus = nATK + g_pPlayer->GetParam(DST_CHR_DMG, 0);

	min += nPlus;
	max += nPlus;

	CItemElem * pWeapon = g_pPlayer->GetWeaponItem(PARTS_RWEAPON);
	if (pWeapon) {

		if (pWeapon->GetProp()) {
			float f = g_pPlayer->GetItemMultiplier(pWeapon);
			min = static_cast<int>(min * f);
			max = static_cast<int>(max * f);
		}

		const int nOption = pWeapon->GetAbilityOption();
		if (nOption > 0) {
			const int nValue = (int)pow((float)(nOption), 1.5f);
			min += nValue;
			max += nValue;
		}
	}
	
	return { min, max };
}


int CWndCharInfo::GetVirtualDEF() {
	int nDefense = 0;

	ATTACK_INFO info;
	memset(&info, 0x00, sizeof(info));
	info.dwAtkFlags = AF_GENERIC;

	float fFactor = 1.0f;
	if (g_pPlayer) {
		fFactor = prj.jobs.GetJobProp(g_pPlayer->GetJob())->fFactorDef;
	}
	nDefense = (int)(((((g_pPlayer->GetLevel() * 2) + ((g_pPlayer->GetSta() + m_nStaCount) / 2)) / 2.8f) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor));
	nDefense = nDefense + (g_pPlayer->GetDefenseByItem(FALSE) / 4);
	nDefense = nDefense + (g_pPlayer->GetParam(DST_ADJDEF, 0));

	nDefense = (int)(nDefense * g_pPlayer->GetDEFMultiplier(&info));

#ifdef __JEFF_11
	if (nDefense < 0)
		nDefense = 0;
#endif	// __JEFF_11
	return nDefense;
}


int CWndCharInfo::GetVirtualCritical() {
	int nCritical;
	nCritical = ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical = (int)(nCritical * g_pPlayer->GetJobPropFactor(JOB_PROP_CRITICAL));
	nCritical = g_pPlayer->GetParam(DST_CHR_CHANCECRITICAL, nCritical);	// ũ��Ƽ�� Ȯ���� �����ִ� ��ų���� 
#ifdef __JEFF_11
	if (nCritical < 0)
		nCritical = 0;
#endif	// __JEFF_11

	return nCritical;
}


float CWndCharInfo::GetVirtualATKSpeed() {
	float fSpeed = 1.0f;
	float fItem = 1.0f;

	const ItemProp * pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if (pWeaponProp)
		fItem = pWeaponProp->fAttackSpeed;

	const JobProp * pProperty = prj.jobs.GetJobProp(g_pPlayer->GetJob());
	ASSERT(pProperty);

	// A = int( ĳ������ ���� + ( ������ ���� * ( 4 * ���� + ( ���� / 8 ) ) ) - 3 )
	// ���ݼӵ� = ( ( 50 / 200 - A ) / 2 ) + ����ġ 
	int A = int(pProperty->fAttackSpeed + (fItem * (4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f)) - 3.0f);

	if (187.5f <= A)
		A = (int)(187.5f);

	const float fSpeedPlus = GetAttackSpeedPlusValue(A);

	fSpeed = ((50.0f / (200.f - A)) / 2.0f) + fSpeedPlus;

	float fDstParam = g_pPlayer->GetParam(DST_ATTACKSPEED, 0) / 1000.0f;
	fSpeed += fDstParam;

	if (0 < g_pPlayer->GetParam(DST_ATTACKSPEED_RATE, 0))
		fSpeed = fSpeed + (fSpeed * g_pPlayer->GetParam(DST_ATTACKSPEED_RATE, 0) / 100);

	if (fSpeed < 0.0f)
		fSpeed = 0.1f;
	if (fSpeed > 2.0f)
		fSpeed = 2.0f;

	return fSpeed;
}


void CWndCharInfo::RenderATK(C2DRender * p2DRender, const int x, const int y) {
	const auto windowMinMax = GetVirtualATK();

	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	if ((g_nRenderCnt / 8) & 1) {
		if (m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) {
			const auto realMinMax = g_pPlayer->GetHitMinMax();
			if (windowMinMax != realMinMax) {
				dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
			}
		}
	}
	
	int nATK = std::midpoint(windowMinMax.first, windowMinMax.second);

	if (g_pPlayer->IsSMMode(SM_ATTACK_UP1) || g_pPlayer->IsSMMode(SM_ATTACK_UP))
		nATK = (int)(nATK * 1.2f);

	if (0 < g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0))
		nATK = nATK + (nATK * g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0) / 100);

	nATK += g_pPlayer->GetParam(DST_ATKPOWER, 0);

	nATK = std::max(0, nATK);

	p2DRender->TextOut(x, y, nATK, dwColor);
}

void CWndHonor::OnDraw(C2DRender * p2DRender) {
	if (m_vecTitle.empty()) {
		const int nIndex = 0;
		LPWNDCTRL	pCustom = GetWndCtrl(WIDC_LISTBOX1);
		const DWORD dwNormal = D3DCOLOR_ARGB(255, 0, 0, 0);

		p2DRender->TextOut(pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex) * 16, prj.GetText(TID_GAME_NO_TITLE), dwNormal);
	}
}


void CWndHonor::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	RefreshList();
	CWndListBox * pWndListBox = GetDlgItem<CWndListBox>(WIDC_LISTBOX1);
	pWndListBox->m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);
}

void CWndHonor::RefreshList() {
	GetDlgItem<CWndButton>(WIDC_BUTTON1)->EnableWindow(FALSE);

	CWndListBox * pWndListBox = GetDlgItem<CWndListBox>(WIDC_LISTBOX1);
	pWndListBox->ResetContent();

	m_vecTitle = CTitleManager::Instance()->m_vecEarned;

	if (g_pPlayer) m_nSelectedId = g_pPlayer->m_nHonor;

	if (!m_vecTitle.empty()) {
		pWndListBox->AddString(prj.GetText(TID_GAME_NOT_SELECTED_TITLE));
		for (const EarnedTitle & title : m_vecTitle) {
			pWndListBox->AddString(title.strTitle.GetString());
		}
	}
}


BOOL CWndHonor::Initialize(CWndBase * pWndParent, DWORD) {
	// TODO: This function is never actually called, delete it
	return CWndNeuz::InitDialog(APP_HONOR, pWndParent, 0, CPoint(0, 0));
}


BOOL CWndHonor::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_LISTBOX1: {
			CWndListBox * pWndListBox = GetDlgItem<CWndListBox>(WIDC_LISTBOX1);
			if (pWndListBox->GetCurSel() == 0) {
				m_nSelectedId = -1;
			} else {
				m_nSelectedId = m_vecTitle[pWndListBox->GetCurSel() - 1].nId;
			}
			GetDlgItem<CWndButton>(WIDC_BUTTON1)->EnableWindow(TRUE);
			break;
		}
		case WIDC_BUTTON1: {
			if (g_pPlayer) {
				GetDlgItem<CWndButton>(WIDC_BUTTON1)->EnableWindow(FALSE);
				g_DPlay.SendReqHonorTitleChange(m_nSelectedId);
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ����  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndCharacter::SerializeRegInfo(CAr & ar, DWORD & dwVersion) {
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	CWndTabCtrl * lpTabCtrl = (CWndTabCtrl *)GetDlgItem(WIDC_TABCTRL1);
	if (ar.IsLoading()) {
		int nCurSel;
		ar >> nCurSel;
		if (nCurSel > 1) nCurSel = 0;
		lpTabCtrl->SetCurSel(nCurSel);
	} else {
		ar << lpTabCtrl->GetCurSel();
	}
}
void CWndCharacter::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	//CRect rectRoot = m_pWndRoot->GetLayoutRect();
	//CPoint point( rectRoot.left, rectRoot.top + 96 + 16 );
	//Move( point );

	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	CWndTabCtrl * lpTapCtrl = (CWndTabCtrl *)GetDlgItem(WIDC_TABCTRL1);
	//m_wndTabCtrl.Create( WBS_NOFRAME, rect, this, 12 );
	m_wndCharInfo.Create(WBS_CHILD | WBS_NOFRAME, rect, lpTapCtrl, 100000);
	m_wndHonor.Create(WBS_CHILD | WBS_NOFRAME, rect, lpTapCtrl, APP_HONOR);
	m_wndHonor.AddWndStyle(WBS_NOFRAME);
	m_wndHonor.AddWndStyle(WBS_NODRAWFRAME);
	m_wndCharInfo.AddWndStyle(WBS_NOFRAME);
	//m_wndCharInfo.AddWndStyle( WBS_NODRAWFRAME );
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_CHAR_INFO);//"�⺻";
	tabTabItem.pWndBase = &m_wndCharInfo;
	lpTapCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_HONOR);//"����";
	tabTabItem.pWndBase = &m_wndHonor;
	lpTapCtrl->InsertItem(1, &tabTabItem);
	lpTapCtrl->SetCurSel(0);

	//lpTapCtrl->SetButtonLength( 60 );

}
BOOL CWndCharacter::Initialize(CWndBase * pWndParent, DWORD dwWndId) {
	return CWndNeuz::InitDialog(dwWndId, pWndParent, 0, CPoint(0, 0));
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void CWndStateConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CString strMessage = prj.GetText(TID_GAME_CHARSTATUS_APPLY_Q);

	if (g_pPlayer->IsBaseJob()) {
		strMessage += '\n';
		strMessage += prj.GetText(TID_DIAG_0082);
	}

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	pWndText->SetString(strMessage);
	pWndText->EnableWindow(FALSE);

	MoveParentCenter();
}

BOOL CWndStateConfirm::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_STATE_CONFIRM, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndStateConfirm::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_YES) {
		SendYes();
	} else if (nID == WIDC_NO || nID == WTBID_CLOSE) {
		CloseTheWindow();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndStateConfirm::SendYes() {
	if (CWndCharacter * pWndBase = g_WndMng.GetWndBase<CWndCharacter>(APP_CHARACTER3)) {
		CWndCharInfo * pInfo = &pWndBase->m_wndCharInfo;
		g_DPlay.SendModifyStatus(pInfo->m_nStrCount, pInfo->m_nStaCount, pInfo->m_nDexCount, pInfo->m_nIntCount);
		pInfo->m_nStrCount = pInfo->m_nStaCount = pInfo->m_nDexCount = pInfo->m_nIntCount = 0;
		pInfo->RefreshStatPoint();
	}
	Destroy();
}

void CWndStateConfirm::CloseTheWindow() {
	if (CWndCharacter * pWndBase = g_WndMng.GetWndBase<CWndCharacter>(APP_CHARACTER3)) {
		pWndBase->m_wndCharInfo.m_fWaitingConfirm = FALSE;
	}
	Destroy();
}



