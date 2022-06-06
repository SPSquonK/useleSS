#pragma once

struct GENMATDIEINFO{
	LPWNDCTRL wndCtrl;
	BOOL isUse;
	int staticNum;
	CItemElem* pItemElem;
};

#define MAX_MATDIE	20
#define ITEM_VALID			0
#define ITEM_MAX_OVERFLOW	1
#define ITEM_INVALID		2

class CWndSummonAngel : public CWndNeuz 
{ 
public:
	CWndText* m_pText = nullptr;
	CWndStatic * m_pStatic[3] = { nullptr, nullptr, nullptr };
	GENMATDIEINFO m_MatDie[MAX_MATDIE];

	int m_nitemcount  = 0;
	int m_nSelecCtrl  = -1;
	int m_nOrichalcum = 0;
	int m_nMoonstone  = 0;

	float m_WhiteAngelRate = 0.f;
	float m_RedAngelRate   = 0.f;
	float m_BlueAngelRate  = 0.f;
	float m_GreenAngelRate = 0.f;

	BOOL m_nowStarting = FALSE;
	
public: 
	
	CWndSummonAngel(); 
	~CWndSummonAngel(); 
	
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnDestroy( void );

	void SetQuestText( CHAR* szChar );
	int HitTest( CPoint point );
	void SummonRateRefresh();
	void ReFreshAll(BOOL extracheck);

	void SetDie(CItemElem* pItemElem);
}; 


