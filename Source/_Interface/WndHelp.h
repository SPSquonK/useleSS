#pragma once

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////////////////
// µµ¿ò¸»
//
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

class CWndHelpTip : public CWndNeuz 
{ 
	CStringArray m_aString;
	int m_nPosString;
public: 
	CWndHelpTip(); 
	~CWndHelpTip(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


class CWndHelpInstant : public CWndNeuz 
{ 
public: 
	CString m_strHelpKey;
	CWndHelpInstant(); 
	~CWndHelpInstant(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
