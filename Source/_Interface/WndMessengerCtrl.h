#pragma once

#include "rtmessenger.h"

class C2DRender;

struct __MESSENGER_PLAYER {
	int		m_nChannel        = 0;
	FriendStatus m_dwStatus = FriendStatus::ONLINE;
	int		m_nLevel          = 0;
	int		m_nJob            = 0;
	DWORD	m_dwPlayerId      = 0;
	TCHAR	m_szName[ 64 ]    = "";
	BOOL	m_bBlock          = FALSE;
	BOOL	m_bVisitAllowed   = FALSE;
};

int GetVertexIconIndex(FriendStatus status);

struct MessengerHelper {
	struct Sorter {
		enum class By { Channel, Status, Level, Job, Name };

		explicit Sorter(By defaultSort = By::Channel)
			: lastSort(defaultSort) {
		}

		bool channelAsc = true;
		bool statusAsc  = true;
		bool levelAsc   = true;
		bool jobAsc     = true;
		bool nameAsc    = true;

		By lastSort;

		void ChangeSort(By criterion, std::span<__MESSENGER_PLAYER> list);
		void ReApply(std::span<__MESSENGER_PLAYER> list) const;
	};
};

class CWndFriendCtrlEx : public CWndBase
{
public:
	int m_nCurSelect  = -1;
	int m_nFontHeight = 20;
	int m_nDrawCount  = 0;
	CWndScrollBar m_wndScrollBar;
	CWndMenu m_menu;
	std::vector < __MESSENGER_PLAYER > m_vPlayerList;

private:
	MessengerHelper::Sorter m_sortStrategy{ MessengerHelper::Sorter::By::Status };

public:
	void Create( const RECT& rect, CWndBase* pParentWnd, UINT nID );

	// Overridables
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	// Sort Func.
	void ChangeSort(MessengerHelper::Sorter::By by);
	void UpdatePlayerList();

	// UI Func.
	void SetScrollBar();
	int		GetSelect( CPoint point, u_long & idPlayer, Friend** ppFriend );
	void	GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend );
	u_long GetSelectId( int SelectCount );
	void ScrollBarPos( int nPos );
	int GetDrawCount( void );
};

class CWndGuildCtrlEx : public CWndBase
{
public:
	int m_nCurSelect  = -1;
	int m_nFontHeight = 20;
	int m_nDrawCount  = 0;
	CWndScrollBar m_wndScrollBar;
	std::vector < __MESSENGER_PLAYER > m_vPlayerList;

private:
	MessengerHelper::Sorter m_sortStrategy{ MessengerHelper::Sorter::By::Channel };

public:
	void Create( const RECT& rect, CWndBase* pParentWnd, UINT nID );

	// Overridables
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	// Sort Func.
	void ChangeSort(MessengerHelper::Sorter::By by);
	void UpdatePlayerList();

	// UI Func.
	void SetScrollBar();
	int		GetSelect( CPoint point, u_long & idPlayer, CGuildMember** lppGuildMember );
//	void	GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend );
	u_long GetSelectId( int SelectCount );
	void ScrollBarPos( int nPos );
	int GetDrawCount( void );
};

class CWndCampus : public CWndNeuz
{
public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
 	virtual	void PaintFrame( C2DRender* p2DRender );
 	virtual void OnDraw( C2DRender* p2DRender );
 	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
 	virtual void OnMouseMove( UINT nFlags, CPoint point );

public:
	void UpdatePlayerList( void );
	int GetDiscipleDrawCount( void ) const;
	__MESSENGER_PLAYER* GetSelectedDiscipleID( int nSelectedNumber );
	u_long GetSelectedMasterID( CPoint point );
	u_long GetSelectedDiscipleID( CPoint point );
	void ChangeSort(MessengerHelper::Sorter::By by);

private:
	enum { MASTER_RENDERING_POSITION = 43, DISCIPLE_RENDERING_POSITION = 122 };
	MessengerHelper::Sorter m_sortStrategy{ MessengerHelper::Sorter::By::Level };

	BOOL m_bCurSelectedMaster  = FALSE;
	int m_nCurSelectedDisciple = -1;
	int m_nFontHeight          = 20;
	CWndMenu m_Menu;
	__MESSENGER_PLAYER m_MasterPlayer;
	std::vector < __MESSENGER_PLAYER > m_vDisciplePlayer;
};
