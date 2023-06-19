#ifndef __WNDCREDIT__H
#define __WNDCREDIT__H

class CWndCredit : public CWndNeuz 
{ 
public: 
	float   m_fOldMusicVolume;
	CTimer m_Starttimer;
	CSound* m_pSound;
	BOOL   m_bPlayVoice;
	CWndCredit(); 
	~CWndCredit(); 
	
	std::vector<CString> m_strArray;
	int m_nLine;
	CTimer m_timer;
	CString m_strWord;
	CPoint m_ptWord;
	int m_nStyle;
	int m_nParam1;
	int m_nParam2;
	int m_nParam3;

	int m_nTexCount;
	CTexture m_texScreenShot;
	CTexture m_aTexScreenShot[18];
	CTimer m_timerScreenShot;
	
	CD3DFont* m_pFont;

	virtual	BOOL Process();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual	void	OnDestroy( void );
	
}; 

class CWndAbout : public CWndNeuz 
{ 
public: 
	CD3DFont* m_pFont;
	CWndAbout();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnDestroy();
}; 
#endif
