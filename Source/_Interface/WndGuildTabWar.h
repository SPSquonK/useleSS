#ifndef __WNDGUILDTABWAR__H
#define __WNDGUILDTABWAR__H

class CWndGuildWarRequest : public CWndNeuz {
	DWORD m_idEnemyGuild;
	char m_szMaster[MAX_PLAYER];
public:
	CWndGuildWarRequest();
	~CWndGuildWarRequest();

	// Initialize하기전에 적길드아이디랑 적길마 이름을 받아둠.
	void	Set(DWORD idEnemyGuild, LPCTSTR szMaster) {
		m_idEnemyGuild = idEnemyGuild;
		strcpy(m_szMaster, szMaster);
	}

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndGuildWarPeaceConfirm : public CWndNeuz {
public:
	CWndGuildWarPeaceConfirm();
	~CWndGuildWarPeaceConfirm();

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndGuildWarPeace : public CWndNeuz {
public:
	CWndGuildWarPeace();
	~CWndGuildWarPeace();

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndGuildWarGiveUp : public CWndNeuz {
public:
	CWndGuildWarGiveUp();
	~CWndGuildWarGiveUp();

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndGuildWarDecl : public CWndNeuz {
public:
	CWndGuildWarDecl();
	~CWndGuildWarDecl();

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase * pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndGuildTabWar : public CWndNeuz 
{ 
	CWndGuildWarDecl*	m_pWndGuildWarDecl;
	CWndGuildWarGiveUp*	m_pWndGuildWarGiveUp;
	CWndGuildWarPeace*	m_pWndGuildWarPeace;
	
public: 
	

	CWndGuildTabWar(); 
	~CWndGuildTabWar(); 

	void UpdateData( void );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif
