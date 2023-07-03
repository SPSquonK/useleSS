#pragma once

//////////////////////////////////////////////////////////////////////////
// Secret Room 
//////////////////////////////////////////////////////////////////////////

class CWndSecretRoomSelection final : public CWndNeuz {
public: 
	BOOL Initialize(CWndBase * pWndParent = nullptr);
	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void	OnInitialUpdate() override;

	void ResetLineup(std::span<const u_long> lineup = {});
}; 

class CWndSecretRoomOffer final : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold;
	DWORD			m_dwMinGold;
	DWORD			m_dwBackupGold;
	
public: 
	CWndSecretRoomOffer();
	virtual ~CWndSecretRoomOffer();
	
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			SetGold( DWORD nCost );
	void			SetMinGold( DWORD dwMinGold )		{ m_dwMinGold    = dwMinGold; }
	void			SetReqGold( DWORD dwReqGold )		{ m_dwReqGold    = dwReqGold; }
	void			SetBackupGold( DWORD dwBackupGold ) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept( BOOL bFlag );	
}; 

class CWndSecretRoomOfferState final : public CWndNeuz
{ 
private: 
	CTimeSpan		m_ct;
	time_t    		m_tEndTime;		// timegettime+
	time_t    		m_tCurrentTime; // timegettime+
	
public:
	CWndSecretRoomOfferState();
	virtual ~CWndSecretRoomOfferState(); 
	
	void		 InsertTitle( const char szTitle[] );
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
	virtual BOOL Process ();
	void		 SetGold( int nGold );
	void		 SetRate( int nRate );
	void	 	 SetTime( time_t tTime ) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
};

class CWndSecretRoomBoard final : public CWndNeuz
{
public: 
	CWndSecretRoomBoard();
	virtual ~CWndSecretRoomBoard();
	
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	void			SetString();
};

#define MAX_SECRETROOM_MEMBER 11

class CWndSecretRoomQuick final : public CWndNeuz
{ 
public:
	std::array<int, MAX_SECRETROOM_MEMBER> m_StaticID;
	std::array<CWndStatic *, MAX_SECRETROOM_MEMBER> m_pWndMemberStatic;
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge = nullptr;

	std::vector<u_long> m_vecGuildMemberId;
	u_long m_FocusMemberid = 0;
	int m_MemberCount      = 0;
	int m_nWndHeight       = 0;
	BOOL m_bMini           = FALSE;
public: 

	CWndSecretRoomQuick(); 
	~CWndSecretRoomQuick(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();	

	void SetActiveMember(int MemberNum);
	void SetGuildMemCount(int nGuildMemCount) {m_MemberCount = nGuildMemCount;};
	void SetGuildMember(u_long uGuildMemberId);
	void SortMemberList();
};

class CWndSecretRoomChangeTaxRate final : public CWndNeuz
{
public:
	int m_nDefaultSalesTax;
	int m_nDefaultPurchaseTax;
	int m_nChangeSalesTax;
	int m_nChangePurchaseTax;
	int m_nMinTax;
	int m_nMaxTax;

	BYTE m_nCont;
	
public:
	CWndSecretRoomChangeTaxRate();
	virtual ~CWndSecretRoomChangeTaxRate(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();

	void SetDefaultTax(int nMinTax, int nMaxTax, BYTE nCont);
}; 

class CWndSecretRoomCheckTaxRate final : public CWndNeuz
{ 
public:
	int m_nSalesTax;
	int m_nPurchaseTax;
	DWORD m_dwGuildId;

public:
	CWndSecretRoomCheckTaxRate();
	virtual ~CWndSecretRoomCheckTaxRate(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();
}; 

//////////////////////////////////////////////////////////////////////////
// Message Box Class
//////////////////////////////////////////////////////////////////////////

class CWndSecretRoomSelectionResetConfirm : public CWndCustomMessageBox
{ 
public: 
	CString m_strMsg;
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndSecretRoomOfferMessageBox : public CWndCustomMessageBox
{ 
public: 
	DWORD m_nCost;
	void	SetValue( CString str, DWORD nCost );
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndSecretRoomInfoMsgBox final : public CWndNeuz
{
public:
	CWndSecretRoomInfoMsgBox();

	virtual ~CWndSecretRoomInfoMsgBox();
	void	SetString( const CHAR* szChar );
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
};

class CWndSecretRoomChangeTaxRateMsgBox : public CWndCustomMessageBox
{ 
public: 
	int m_nSalesTax;
	int m_nPurchaseTax;
	BYTE m_nCont;

public:
	void	SetValue( CString str, int nSalesTax, int nPurchaseTax, BYTE nCont );
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};

class CWndSecretRoomCancelConfirm : public CWndNeuz
{
public:
	CWndSecretRoomCancelConfirm();
	virtual ~CWndSecretRoomCancelConfirm();
	
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
};
