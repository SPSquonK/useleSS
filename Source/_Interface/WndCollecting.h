#pragma once

class CCollectingWnd : public CWndNeuz 
{ 
public: 
	CCollectingWnd(); 
	~CCollectingWnd() override = default;

	bool		m_bIsCollecting;
	
	CRect			m_BetteryRect;
	CRect			m_LevelRect;
	CTexture		*m_pTexLevel, *m_pTexBatt;
	CTexture		*m_pTexGauEmptyNormal;
	CTexture		*m_pTexGauFillNormal;
	char			m_pbufText[24];
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	virtual	BOOL Process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual void OnDestroy();
	virtual	void OnInitialUpdate(); 
	virtual void OnMouseWndSurface( CPoint point );
	void	Update();
	void SetButtonCaption(bool bIsStart);


}; 

