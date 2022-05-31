// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDNEUZ_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDNEUZ_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////////////
// 윈도의 타이틀 바 
// 타이틀 출력, 윈도 관리 버튼 
//
#define WTBID_CLOSE     10000
#define WTBID_MAX       10001
#define WTBID_MIN       10002
#define WTBID_HELP      10003
#define WTBID_REPORT    10004
#define WTBID_EXTENSION 10005
#define WTBID_PIN       10006

#define WBS_MINIMIZEBOX 0x00000001L // 최소화 버튼 
#define WBS_MAXIMIZEBOX 0x00000002L // 최대화 버튼 
#define WBS_HELP        0x00000004L // 핼프 버튼 
#define WBS_VIEW        0x00000008L // 뷰 전환 버튼 
#define WBS_PIN         0x00000010L // 핀 버튼 
#define WBS_EXTENSION   0x00000020L // 확장 버튼 
#define WBS_THICKFRAME  0x00000040L // 리사이즈 가능 윈도

class CWndTitleBar : public CWndBase
{
	CWndButton m_awndButton[ 7 ];

public:
	int m_nButtonMax;

	/*
	CWndButton m_wndClose;
	CWndButton m_wndMaximize;
	CWndButton m_wndMinimize;
	CWndButton m_wndHelp;
	CWndButton m_wndReport;
	*/

	CWndButton* GetTitleBarButton( int nID ) { return &m_awndButton[ nID - 10000 ]; }

	CWndTitleBar();
	virtual ~CWndTitleBar();
	void Replace();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwStyle = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
};
//////////////////////////////////////////////////////////////////////////////////////
// 작업 윈도 
// 메뉴 버튼, 명령 아이콘, 단축 아이콘, 활성화 기능 등등
//
class CWndNeuz : public CWndBase  
{
	CWndButton m_wndButton;
	BOOL m_bFullMax;
	DWORD m_dwWID;
	BOOL m_bPutRegInfo;
	
protected:
	CWndTitleBar m_wndTitleBar;
	std::vector< CWndBase* > m_aryDlgItem;
	CRect m_rectBackup;
	CPtrArray m_wndArrayTemp;

public:
	CPoint m_ptMouseCenter;
	int m_nWndType; // nomal wnd, tool wnd, 

	// 윈도 정보 저장 관련 
	void SetPutRegInfo( BOOL bPutRegInfo ) { m_bPutRegInfo = bPutRegInfo; }
	BOOL IsPutRegInfo() { return m_bPutRegInfo; }
#ifdef __CLIENT
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
#endif
	// 윈도 정보 저장 관련 끝 

	BOOL InitDialog( HWND hWnd, DWORD dwWID, DWORD dwStyle = 0, CPoint ptLeftTop = 0, CWndBase* pWndParent = NULL );
	CWndBase* CreateControl( HWND hWnd, LPWNDCTRL lpWndCtrl );
	//CWndBase* GetDlgItem( UINT nID )
protected:
	CWndNeuz();
public:
	~CWndNeuz() override;

	D3DFORMAT m_d3dFormat;
	void AdjustWndBase( D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	void PaintTexture( LPVOID pDestData, LPIMAGE pImage, CPoint pt, CSize sizeSurface ) { ::PaintTexture( pDestData, pImage, pt, sizeSurface, m_d3dFormat ); }
	void SetFullMax( BOOL bFullMax ) { m_bFullMax = bFullMax; }
	BOOL IsFullMax() { return m_bFullMax; }
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwStyle = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual BOOL OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnNonClientLButtonDblClk( UINT nFlags, CPoint point );
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );

	void SetSizeMax();
	void SetSizeWnd();
};

#endif // !defined(AFX_WNDNEUZ_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
