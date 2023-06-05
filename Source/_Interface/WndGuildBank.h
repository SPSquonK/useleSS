#ifndef __WNDGUILDBANK__H
#define __WNDGUILDBANK__H

#include "WndGuildMerit.h"

class CWndGuildBank : public CWndNeuz 
{ 
	CWndGuildMerit* m_pwndGuildMerit;		// ±æµå°øÇå.
public: 
	CWndItemCtrl m_wndItemCtrl;
	CWndGold     m_wndGold;

public:
	CWndGuildBank(); 
	~CWndGuildBank(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
};

class CWndGuildLogGeneric : public CWndBase
{
public:
	BYTE m_type;
	int m_xOffset;

	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	
	CWndScrollBar m_wndScrollBar;
	std::vector < CString > m_vLogList;

public:
	CWndGuildLogGeneric(BYTE type, int xOffset);
	~CWndGuildLogGeneric() final = default;

	void Create( RECT& rect, CWndBase* pParentWnd, UINT nID );
	void UpdateLogList();
	void UpdateScroll();

	// Overridables
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );

	// UI Func.
	int GetDrawCount( void );
};

class CWndGuildBankLog : public CWndNeuz 
{ 
public: 
	~CWndGuildBankLog() override = default;

	CWndGuildLogGeneric m_wndAddItemLog{ 0x01, 4 };
	CWndGuildLogGeneric	m_wndRemoveItemLog{ 0x02, 4 };
	CWndGuildLogGeneric	m_wndReceivePenyaLog{ 0x03, 10 };
	CWndGuildLogGeneric	m_wndInvestPenyaLog{ 0x04, 10 };

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

	void UpdateScroll();
};

#endif
