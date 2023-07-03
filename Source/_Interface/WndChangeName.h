#ifndef __WNDCHANGENAME__H
#define __WNDCHANGENAME__H


class CWndChangeName final : public CWndNeuz
{ 
private:
	DWORD	m_dwData;

public: 
	CWndChangeName(); 
	~CWndChangeName(); 

	void	SetData( WORD wId, WORD wReset );

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndChangePetName final : public CWndNeuz
{ 
private:
	DWORD	m_dwData;
	DWORD	m_dwId;

public: 
	CWndChangePetName(); 
	~CWndChangePetName(); 

	void	SetData( WORD wId, WORD wReset );
	void	SetItemId(DWORD wId) {m_dwId = wId;};
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#endif	// __WNDCHANGENAME__H

