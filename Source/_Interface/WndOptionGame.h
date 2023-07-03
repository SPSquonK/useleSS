#ifndef __WNDOPTIONGAME__H
#define __WNDOPTIONGAME__H

class CWndOptionGame : public CWndNeuz 
{ 
public: 
	CWndOptionGame(); 
	~CWndOptionGame(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	CTexture         m_Texture;
	CTexture         m_TexturePt;
	BOOL			 m_bLButtonClick;
	BOOL			 m_bLButtonClick2;
	int				 m_nStep[2];

	virtual void OnMouseMove(UINT nFlags, CPoint point);

	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point );
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point );
	CPoint GetStepPos( int nStep, int nWidth, int nDivision );
}; 
#endif
