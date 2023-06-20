#pragma once

#include "PlayerLineup.h"

//////////////////////////////////////////////////////////////////////////
// 1:1 GuildCombat Class
//////////////////////////////////////////////////////////////////////////


class CWndGuildCombat1to1Selection final : public CWndNeuz
{
public:
	using CWndListGuild  = CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer>;
	using CWndListSelect = CWndTListBox<PlayerLineup, PlayerLineup::NumberedDisplayer>;

public: 
	void SetSelection(std::span<const u_long> playerIds);
	void ResetSelection();

	BOOL	Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL	OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void	OnInitialUpdate() override;

private:
	void OnClickFinish();
}; 

class CWndGuildCombat1to1Offer : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold = 0;
	DWORD			m_dwMinGold = 0;
	DWORD			m_dwBackupGold = 0;
	
public:
	enum class CombatType { GC1to1, GCGuild };
	CombatType m_nCombatType;

public: 
	explicit CWndGuildCombat1to1Offer(CombatType nCombatType)
		: m_nCombatType(nCombatType) {}
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
	virtual void	PaintFrame( C2DRender* p2DRender );
	void			SetGold( DWORD nCost );
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
