#pragma once

#include <concepts>
#include <memory>

#include "guild.h"

class CWndGuildPayConfirm : public CWndNeuz {
public:
	DWORD   m_dwAppellation = -1;

	virtual BOOL Initialize(CWndBase * pWndParent);
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndGuildTabApp : public CWndNeuz {
public:
	void UpdateData();
	void EnableButton(BOOL bEnable);
	CWndGuildTabApp();

	DWORD	m_adwPower[MAX_GM_LEVEL];
	std::unique_ptr<CWndGuildPayConfirm> m_pWndGuildPayConfirm;
	CWndStatic * m_pWndPenya[MAX_GM_LEVEL];

	void SetData(DWORD dwPower[]);
	void SetPenya(void);

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;

private:
	void ForEachPower(std::invocable<UINT, int, DWORD> auto func);
	void ForEachPower(std::invocable<CWndButton &, int, DWORD> auto func);
};

