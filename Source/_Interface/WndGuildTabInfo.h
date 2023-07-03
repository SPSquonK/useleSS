#pragma once

#include <memory>
#include "WndGuildName.h"

// TODO: some of these Initialize functions are not overriding anything.
// check which version is actually used in code

class CWndGuildDisMiss : public CWndNeuz {
public:
	virtual BOOL Initialize(CWndBase * pWndParent);
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};


class CWndGuildNotice : public CWndNeuz {
public:
	virtual BOOL Initialize(CWndBase * pWndParent);
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndGuildSetLogo : public CWndNeuz 
{ 
public: 
	CRect    m_rect[CUSTOM_LOGO_MAX];
	int      m_nSelectLogo;
	
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
}; 

class CWndGuildTabInfo : public CWndNeuz 
{ 
public: 
	std::unique_ptr<CWndGuildNotice>  m_pwndGuildNotice;
	std::unique_ptr<CWndGuildSetLogo> m_pwndGuildSetLogo;
	std::unique_ptr<CWndGuildDisMiss> m_pwndGuildDisMiss;
	
	void UpdateData();
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
}; 

