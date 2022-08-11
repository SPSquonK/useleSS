#pragma once

#include "WndGuildTabApp.h"
#include "WndGuildTabInfo.h"
#include "WndGuildTabMember.h"
#include "WndGuildTabWar.h"
#include "WndGuildTabPower.h"


class CWndGuild final : public CWndNeuz {
public:
	CWndGuildTabApp     m_WndGuildTabApp;
	CWndGuildTabInfo    m_WndGuildTabInfo;
	CWndGuildTabMember  m_WndGuildTabMember;
	CWndGuildTabWar			m_WndGuildTabWar;
	CWndGuildTabPower		m_WndGuildTabPower;

	void SetCurTab(int index);
	void UpdateDataAll();

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	void OnInitialUpdate() override;
}; 

class CWndGuildConfirm final : public CWndNeuz { 
public:
	static void OpenWindow(u_long idMaster, const CGuild & guild);

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

private:
	explicit CWndGuildConfirm(u_long idMaster) : m_idMaster(idMaster) {}
	void SetGuildName(const char * pGuildName);

	u_long m_idMaster;
};




