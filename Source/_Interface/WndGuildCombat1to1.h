#pragma once

//////////////////////////////////////////////////////////////////////////
// 1:1 GuildCombat Class
//////////////////////////////////////////////////////////////////////////

class CWndGuildCombat1to1Selection final : public CWndNeuz
{
public:
	struct Player {
		u_long playerId;
		CString str;
	};

	struct UnselectedPlayerDisplayer {
		void Render(
			C2DRender * const p2DRender, CRect rect,
			Player & displayed,
			DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	struct SelectedPlayerDisplayer {
		void Render(
			C2DRender * const p2DRender, CRect rect,
			Player & displayed,
			DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	using CWndListGuild  = CWndTListBox<Player, UnselectedPlayerDisplayer>;
	using CWndListSelect = CWndTListBox<Player, SelectedPlayerDisplayer>;

public: 
	void SetSelection(std::span<const u_long> playerIds);
	void ResetSelection();

	BOOL	Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void	OnDraw(C2DRender * p2DRender) override;
	void	OnInitialUpdate() override;

private:
	void OnClickToGuild();
	void OnClickToSelect();
	void OnClickMoveUp();
	void OnClickMoveDown();
	void OnClickFinish();
}; 

class CWndGuildCombat1to1Offer : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold;
	DWORD			m_dwMinGold;
	DWORD			m_dwBackupGold;
	
public:
	int				m_nCombatType; // 0 : 길드대전 , 1 : 1:1길드대전

public: 
	CWndGuildCombat1to1Offer(int nCombatType);
	virtual ~CWndGuildCombat1to1Offer();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
	virtual void	PaintFrame( C2DRender* p2DRender );
	void			SetGold( DWORD nCost );
//	void			SetTotalGold( __int64 nCost );
	void			SetMinGold( DWORD dwMinGold )		{ m_dwMinGold    = dwMinGold; }
	void			SetReqGold( DWORD dwReqGold )		{ m_dwReqGold    = dwReqGold; }
	void			SetBackupGold( DWORD dwBackupGold ) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept( BOOL bFlag );	
}; 

//////////////////////////////////////////////////////////////////////////
// Message Box Class
//////////////////////////////////////////////////////////////////////////

class CGuildCombat1to1SelectionResetConfirm : public CWndMessageBox
{ 
public: 
	CString m_strMsg;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndGuildCombat1to1OfferMessageBox : public CWndMessageBox
{ 
public: 
	DWORD m_nCost;
	void	SetValue( CString str, DWORD nCost );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 
