
#ifndef __WNDMESSENGERCTRL_H
#define __WNDMESSENGERCTRL_H

class C2DRender;

typedef struct __MESSENGER_PLAYER
{
	int		m_nChannel;
	DWORD	m_dwStatus;
	int		m_nLevel;
	int		m_nJob;
	DWORD	m_dwPlayerId;
	TCHAR	m_szName[ 64 ];
	BOOL	m_bBlock;
	BOOL	m_bVisitAllowed;
	__MESSENGER_PLAYER( void );
	void Initialize( void );
} __MESSENGER_PLAYER;

struct MessengerHelper {
	struct Sorter {
		enum class By { Channel, Status, Level, Job, Name };

		bool channelAsc = true;
		bool statusAsc  = false;
		bool levelAsc   = true;
		bool jobAsc     = true;
		bool nameAsc    = true;

		By lastSort = By::Channel;

		void ChangeSort(By criterion, std::span<__MESSENGER_PLAYER> list);
		void ReApply(std::span<__MESSENGER_PLAYER> list) const;
	};
};

class CWndFriendCtrlEx : public CWndBase
{
public:
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	CWndScrollBar m_wndScrollBar;
	CWndMenu m_menu;
	std::vector < __MESSENGER_PLAYER > m_vPlayerList;

private:
	MessengerHelper::Sorter m_sortStrategy;

public:
	CWndFriendCtrlEx();
	~CWndFriendCtrlEx();

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
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	CWndScrollBar m_wndScrollBar;
	std::vector < __MESSENGER_PLAYER > m_vPlayerList;

private:
	MessengerHelper::Sorter m_sortStrategy;

public:
	CWndGuildCtrlEx();
	~CWndGuildCtrlEx();

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
	CWndCampus( void );
	~CWndCampus( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
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
	MessengerHelper::Sorter m_sortStrategy;

	BOOL m_bCurSelectedMaster;
	int m_nCurSelectedDisciple;
	int m_nFontHeight;
	CWndMenu m_Menu;
	__MESSENGER_PLAYER m_MasterPlayer;
	std::vector < __MESSENGER_PLAYER > m_vDisciplePlayer;
};

#endif //__WNDMESSENGERCTRL_H

