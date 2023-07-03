#ifndef __WNDLORD__H
#define __WNDLORD__H
#include "clord.h"


class CWndLordConfirm final : public CWndNeuz
{ 
private:
	CString		m_pPledge;
public: 
	CWndLordConfirm(CString strStr) {m_pPledge = strStr;}; 
	~CWndLordConfirm() {}; 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndLordPledge final : public CWndNeuz
{ 
private:
	SPC					m_pRanker;
	bool				m_bIsFirst;
	CWndLordConfirm*	m_pWndConfirm;

public: 
	CWndLordPledge(); 
	CWndLordPledge(int nRank); 
	~CWndLordPledge(); 
	
	void SetPledge(LPCTSTR strPledge);
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	BOOL SetRanker(int nRank);
}; 

class CWndLordState final : public CWndNeuz
{ 
private:
	DWORD				m_tmRefresh;
	CString				strName[10];
	CString				strClass[10];
	int					m_nSelect;
public: 
	CWndLordPledge*		m_pWndPledge;

	CWndLordState(); 
	~CWndLordState(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL Process();
}; 



class CWndLordTender final : public CWndNeuz
{ 
public: 
	CWndLordTender(); 
	~CWndLordTender(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	void	RefreshDeposit();
}; 

class CWndLordVote final : public CWndNeuz
{ 
private:
	DWORD				m_tmRefresh;

public: 
	CWndLordVote(); 
	~CWndLordVote(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL Process();
}; 

class CWndLordEvent final : public CWndNeuz
{ 
private:
	int m_nEEvent;	// 사용자가 선택한 경험치 이벤트 인덱스, 초기값 0
	int m_nDEvent;	// 사용자가 선택한 드롭률 이벤트 인덱스, 초기값 0
public: 
	CWndLordEvent(); 
	~CWndLordEvent(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndLordSkill final : public CWndNeuz
{ 
private:
	BOOL          m_bDrag;
	CTexture*     m_aTexSkill[9];
	int			  m_nCurSelect;
	LPWNDCTRL	  m_aWndCtrl[9];
public: 
	CWndLordSkill(); 
	~CWndLordSkill(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
}; 

#define LORDSKILL_RECALL	6
#define LORDSKILL_TELEPORT	7

class CWndLordSkillConfirm final : public CWndNeuz
{ 
private:
	int m_nType;

public: 
	CWndLordSkillConfirm() {m_nType = 0;}; 
	CWndLordSkillConfirm(int nType) {m_nType = nType;};

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
};

class CWndLordInfo final : public CWndNeuz
{ 

public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
};

class CWndLordRPInfo final : public CWndNeuz
{ 

public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
};

#endif
