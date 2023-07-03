#ifndef __WNDMESSENGER__H
#define __WNDMESSENGER__H

#include "WndMessengerCtrl.h"
#include "sqktd/flasher.h"

class CWndMessengerEx : public CWndNeuz 
{ 
public: 
	CWndMessengerEx(); 
	~CWndMessengerEx(); 

	CWndMenu			m_menuState;
	CWndFriendCtrlEx	m_wndFriend;
	CWndGuildCtrlEx		m_wndGuild;
	CWndCampus			m_WndCampus;
	CTexture			m_TexMail;
private:
	sqktd::_::Flasher<DWORD, 50, 255, 8> m_mailIconFlash;

public:

	virtual	BOOL Process();
//	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
//	virtual void OnSize( UINT nType, int cx, int cy ); 

	enum class UpdateListType { Friend, Guild, Campus, Any };
	static void TryUpdateList(UpdateListType type);
};

class CWndInstantMsg : public CWndNeuz 
{ 
public: 
	CTimer m_timer;
	CString m_strMessage;
	
	CString m_strPlayer;
	
	void AddMessageJoin( LPCTSTR lpszJoinName );
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage );
	void AddPostMessage( LPCTSTR lpszSendName );
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
class CWndMessage : public CWndNeuz 
{ 
public: 

	CString m_strPlayer;
	
	void InitSize( void );
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage );

	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
}; 

class CWndMessageNote : public CWndNeuz 
{ 
public:
	TCHAR m_szName[ 64 ];
	DWORD m_dwUserId;

	CWndText* m_pWndText;
	CWndEdit* m_pEdit;
public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndMessengerNote : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
	virtual	void OnInitialUpdate();
};

#endif
