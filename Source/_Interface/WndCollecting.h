#pragma once

class CCollectingWnd final : public CWndNeuz
{ 
public: 
	CCollectingWnd(); 

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
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual void OnDestroy();
	virtual	void OnInitialUpdate(); 
	virtual void OnMouseWndSurface( CPoint point );
	void	Update();
	void SetButtonCaption(bool bIsStart);


}; 

