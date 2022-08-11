#pragma once

#include <memory>

class CWndGuildWarPeace final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndGuildWarGiveUp final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndGuildWarDecl final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndGuildTabWar final : public CWndNeuz {
	std::unique_ptr<CWndGuildWarDecl>   m_pWndGuildWarDecl;
	std::unique_ptr<CWndGuildWarGiveUp> m_pWndGuildWarGiveUp;
	std::unique_ptr<CWndGuildWarPeace>  m_pWndGuildWarPeace;

public:
	void UpdateData();

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
};


class CWndGuildWarRequest final : public CWndNeuz {
	DWORD m_idEnemyGuild;
	char m_szMaster[MAX_PLAYER];
public:

	void Set(DWORD idEnemyGuild, LPCTSTR szMaster) {
		m_idEnemyGuild = idEnemyGuild;
		strcpy(m_szMaster, szMaster);
	}

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndGuildWarPeaceConfirm final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};
