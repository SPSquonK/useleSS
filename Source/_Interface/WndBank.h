#pragma once

class CWndBank : public CWndNeuz 
{ 
public: 
	struct BankSlot {
		CWndTabCtrl * pTabCtrl = nullptr;
		CWndStatic * pCost = nullptr;
		CWndGold wndGold;
		CWndItemCtrl wndItemCtrl;
		CRect rectItemCtrl;
		CRect rectGold;
		bool bUse = false;
	};

	std::array<BankSlot, 3> m_slots;

	CTexture * m_pTexture;

	~CWndBank() override;
	void ReSetBank( void );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

	BYTE GetPosOfItemCtrl(const CWndBase * pWnd, BYTE defaultValue = 2) const;
	BYTE GetPosOfGold(const CWndBase * pWnd, BYTE defaultValue = 2) const;
}; 

class CWndConfirmBank : public CWndNeuz 
{ 
public:
	DWORD	m_dwId;
	DWORD	m_dwItemId;
	void	SetItem( DWORD dwId, DWORD dwItemId )	{	m_dwId	= dwId;	m_dwItemId	= dwItemId;		}
public: 
	CWndConfirmBank(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndBankPassword : public CWndNeuz 
{ 
public:
	int m_nFlags;
	DWORD	m_dwId;
	DWORD	m_dwItemId;
	void	SetItem( DWORD dwId, DWORD dwItemId )	{	m_dwId	= dwId;		m_dwItemId	= dwItemId;		}
public: 
	CWndBankPassword(); 
	
	void SetBankPassword( int nFlags );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

	[[nodiscard]] static bool IsValidPassword(const char * text);
	static void CopyPassword(char * buffer, const char * text);
}; 
