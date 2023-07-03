#pragma once

class CWndMotion1 : public CWndBase
{
	MotionProp* m_pSelectMotion;
	int m_nSelect;
	
	std::vector<MotionProp *> m_motionArray;
	
public:
	CWndMotion1();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL Process();
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
};

class CWndEmoticon : public CWndBase
{
	CTexture*   m_pSelectTexture;

	int m_nSelect;
		
public:
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	// message
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
};

class CWndMotion final : public CWndNeuz
{ 
public: 
	CWndMotion1		m_wndMotion1;
	CWndEmoticon    m_wndEmoticon;

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	void OnInitialUpdate() override;
}; 
