#pragma once

#include <unordered_map>

class CWndHelp : public CWndNeuz {
	CString m_strKeyword;

	void ChangeDisplayedHelp(TREEELEM & treeElem);

public:
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
};

class CWndHelpFAQ : public CWndNeuz {
	std::unordered_map<std::string, std::string> m_mapFAQ;

	bool LoadFAQ(LPCTSTR lpszFileName);
	void OnChangedSelection(const std::string & question);

public:
	void OnInitialUpdate() override;
	BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0) override;
	BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult) override;
};

class CWndHelpTip : public CWndNeuz {
	std::vector<CString> m_aString;
	size_t m_current = 0;

	bool LoadTips();

public: 
	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override;
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnInitialUpdate() override;
}; 

class CWndHelpInstant : public CWndNeuz {
	CString m_strHelpKey;

public:
	CWndHelpInstant(CString helpKey) : m_strHelpKey(std::move(helpKey)) {}

	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override;
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnInitialUpdate() override;
}; 
