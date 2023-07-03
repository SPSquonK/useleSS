// WndWebBox.h: interface for the CWndWebBox class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __WNDWEBOX__H
#define __WNDWEBOX__H


void InitWebGlobalVar();

class CWndWebBox final : public CWndNeuz 
{ 
public: 
	CRect    m_rectOldBackup;
	
	CWndWebBox(); 
	~CWndWebBox(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual HRESULT RestoreDeviceObjects();
	virtual BOOL Process();
//	virtual void OnDestroy( void );
};

class CWndWebBox2 final : public CWndNeuz 
{ 
public: 
	CRect    m_rectOldBackup;
	
	CWndWebBox2(); 
	~CWndWebBox2(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual HRESULT RestoreDeviceObjects();
//	virtual void OnDestroy( void );
};
#endif // __WNDWEBOX__H
