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

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

	BYTE GetPosOfItemCtrl(const CWndBase * pWnd, BYTE defaultValue = 2) const;
	BYTE GetPosOfGold(const CWndBase * pWnd, BYTE defaultValue = 2) const;
}; 

class CWndConfirmBank final : public CWndNeuz {
private:
	OBJID	m_dwId;
public: 
	explicit CWndConfirmBank(OBJID dwId = NULL_ID) : m_dwId(dwId) {}
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	virtual	void OnInitialUpdate() override;
}; 

class CWndBankPassword : public CWndNeuz 
{ 
private:
	bool m_needCurrentPassword;
	OBJID	m_dwId;
public: 
	CWndBankPassword(bool needCurrentPassword, OBJID dwId = NULL_ID)
		: m_needCurrentPassword(needCurrentPassword)
		, m_dwId(dwId) {
	}

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

	[[nodiscard]] static bool IsValidPassword(const char * text);
	static void CopyPassword(char * buffer, const char * text);
}; 
