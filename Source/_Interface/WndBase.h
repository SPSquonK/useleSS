#pragma once

#include <exception>
#include <format>
#include <source_location>
#include <typeinfo>
#include <memory>
#include <functional>

/*
#ifdef __CLIENT
#define CLIENT_WIDTH  ( g_Neuz.m_d3dsdBackBuffer.Width )
#define CLIENT_HEIGHT ( g_Neuz.m_d3dsdBackBuffer.Height )
#define RECT_CLIENT   CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT )
#endif
*/

#define WSIZE_WINDOW   0
#define WSIZE_MIN      1
#define WSIZE_MAX      2

#include "WndStyle.h"

#define D3DCOLOR_TEMP(a,b,g,r) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff))) 

// 클라이언트의 좌표 얻기 
#define GET_CLIENT_POINT( hwnd, pt ) CPoint pt; ::GetCursorPos( &pt ); ::ScreenToClient( hwnd, &pt );

class C2DRender;

#include "ResManager.h"

//_CHILD   
class CWndBase;
 
struct WNDMESSAGE {
	WNDMESSAGE(CWndBase * pWndBase, UINT message, WPARAM wParam, LPARAM lParam)
	: m_pWndBase(pWndBase), m_message(message), m_wParam(wParam), m_lParam(lParam) {}

	CWndBase* m_pWndBase ;
	UINT      m_message  ;
	WPARAM    m_wParam   ;
	LPARAM    m_lParam   ;
};

struct CTileMapManager {
private:
	std::map<std::string, std::unique_ptr<IMAGE>, std::less<>> m_map;

public:
	IMAGE * GetImage(std::string_view lpszFileName);
	void Clear() { return m_map.clear(); }
};

//////////////////////////////////////////////////////////////////////////////
// CWndBase 
//////////////////////////////////////////////////////////////////////////////

class CWndBase  
{
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

static CPoint    m_pointOld;
static CRect     m_rectOld;

protected:
	// CWndBases are boring: only classes that inherit from it should be
	// instancied.
	CWndBase();
public:
	virtual ~CWndBase();
	CWndBase(const CWndBase &) = delete;
	CWndBase(CWndBase &&) = delete;
	CWndBase & operator=(const CWndBase &) = delete;
	CWndBase & operator=(CWndBase &&) = delete;

static CWndBase* m_pWndCapture;
	//BOOL      m_bCapture;
	bool      m_bPush; // 포커스 윈도가 눌렸나?
	int       m_nResizeDir;

//oid AddWnd(CWndBase* pWnd); 
	//void RemoveWnd(CWndBase* pWnd);
	void DestroyAllWnd(); // Force delete all windows ; called when exiting
	void SetChildFocus( CWndBase* pWndBase, POINT point );
	[[nodiscard]] CWndBase * GetChildFocus(POINT point);
//protected:
public:
static CResManager m_resMng;
	static CTileMapManager m_strWndTileMap;
	CD3DApplication* m_pApp;
	CTheme*   m_pTheme;
	CWndBase* m_pParentWnd;
	CPtrArray m_wndArray;
	UINT      m_nIdWnd;
	bool      m_bVisible;
	CSize     m_sizeTile;
	DWORD     m_dwStyle;
	bool      m_bLButtonUp;
	bool      m_bLButtonDown;
	bool      m_bRButtonDown;
	bool      m_bMButtonDown;
	bool	  m_bLButtonDowned;
	CString   m_strTitle;
	CString   m_strToolTip;
	C2DRender*   m_p2DRender;
	CRect     m_rectCurrentWindow;
	CRect     m_rectCurrentClient;
	CRect     m_rectWindow ; // Window ; 패어런트부터의 좌표 
	CRect     m_rectClient ; // Client ; 패어런트부터의 좌표 ( 보통 에찌 때문에 m_rectWindow 보다는 조금 안쪽 좌표를 갖는다. )
	CRect     m_rectLayout ; // Client ; 패어런트부터의 좌표 ( 보통 에찌 때문에 m_rectWindow 보다는 조금 안쪽 좌표를 갖는다. )
	CPoint    m_ptMouse;
	BOOL      m_bGroup;
	CD3DFont* m_pFont;
	bool      m_bAutoFree;
	bool      m_bEnable;
	CWndBase* m_pWndFocusChild;
	bool      m_bTabStop;
	bool      m_bDefault;
	bool      m_bActive;
	int       m_nToolTipPos;
	BYTE      m_byWndType;
	BOOL      m_bKeyButton;
	DWORD     m_dwColor;
	int       m_nAlphaCount;
	bool      m_bTile;
	CString   m_strTexture;
	BOOL      m_bNoCloseButton;

static BOOL           m_bCling          ;
static BOOL           m_bEdit           ;
static BOOL           m_bFullWnd        ;
static CWndBase*      m_pWndRoot        ;
static CWndBase*      m_pWndFocus       ; // 다이얼로그 윈도 포커스 
static CWndBase*      m_pCurFocus       ; // 다이얼로그, 차일드 중 최종 현재 포커스 

static std::vector<CWndBase *> m_wndRemove;
static std::vector<WNDMESSAGE> m_postMessage;
static CTexture*      m_pTexForbid;
static CTimer         m_timerForbid;
static BOOL           m_bForbid;
static CPoint         m_ptForbid;
static HWND           m_hWnd;
static HCURSOR        m_hDefaultCursor;
static CWndBase*      m_pWndOnMouseMove;
static CWndBase*      m_pWndOnSetCursor;
static int            m_nAlpha;
static CTheme         m_Theme;

	static BOOL SetForbidTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName );
	static void SetForbid( BOOL bForbid );
	static BOOL IsForbid() { return m_pTexForbid && m_bForbid; }

	CTexture*  m_pTexture;
//	CString m_strTexture;

	BOOL IsPush() { return m_bPush; }
	CRect GetScreenRect();

public:
static CTextureMng    m_textureMng;
static std::map<CWndBase *, std::unique_ptr<CTexture>> m_backgroundTextureMng;
static SHORTCUT       m_GlobalShortcut;
	BOOL m_bPickup; // 포커스 윈도를 들었나?

	void AddWnd(CWndBase* pWnd); 
	void RemoveWnd(CWndBase* pWnd);

//	CString m_strSndEffect;
	int       m_nWinSize; // 0 = nomal, 1 - minimaize, 2 = maximize

	void FitTextureSize();
	static void FreeTileTexture() { return m_strWndTileMap.Clear(); }
	static IMAGE * GetTileImage(std::string_view lpszFileName) {
		return m_strWndTileMap.GetImage(lpszFileName);
	}
	
	BOOL Create(DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);
	void SetTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName, BOOL bMyLoader = FALSE );

	static void RemoveDestroyWnd();
	void MoveParentCenter(); // 윈도를 부모 윈도의 중앙으로 옮긴다.
	void MoveRectCenter(CRect rect); // 윈도를 rect의 중앙으로 옮긴다.
	void MovePointCenter(CPoint pt); // 윈도를 pt의 중앙으로 옮긴다.
	void Move( CPoint pt ); // 왼도의 left,top을 pt로 
	void Move70(); // Horizontally centers the window and moves it at y=70
	void Move( int x, int y ) { Move( CPoint( x, y ) ); } // 왼도의 left,top을 pt로 
	[[nodiscard]] BOOL IsVisible() const { return m_bVisible; } // 윈도가 보이는 상태인가.
	void SetVisible(BOOL bVisible) { m_bVisible = ( bVisible != FALSE ); } // 윈도를 보이거나 감춘다.
	void SetCapture();
	void ReleaseCapture();
	CRect  MakeCenterRect( int nWidth, int nHeight );
	CPoint GetClientCenter(); // Client의 중앙 좌표를 돌려준다.
	CPoint GetStrCenter(C2DRender* p2DRender,LPCTSTR str); // 화면 중심에서 스트링의 중앙에 출력하기 위한 좌표를 돌려준다.
	CPoint GetStrRectCenter(C2DRender* p2DRender,CRect rect,LPCTSTR str); // rect의 중심에서 스트링의 중앙에 출력하기 위한 좌표를 돌려준다.
	CString GetTitle() { return m_strTitle; } 
	void SetTitle(CString strTitle) { m_strTitle = strTitle; }
	DWORD GetStyle() { return m_dwStyle; }
	CWndBase* GetParentWnd() { return m_pParentWnd; }
	CRect GetClientRect( BOOL bParent = FALSE );
	CRect GetWindowRect( BOOL bParent = FALSE );
	CRect GetLayoutRect( BOOL bParent = FALSE );
	CRect GetWndRect() { return m_rectWindow; }
	[[nodiscard]] bool IsWndRoot() const { return this == m_pWndRoot; }
	[[nodiscard]] static bool IsOpenModalWnd() { return m_pWndFocus && m_pWndFocus->IsWndStyle(WBS_MODAL); }
	CWndBase* GetFocusChild() { return m_pWndFocusChild; }
	[[nodiscard]] static CWndBase * GetFocusWnd() { return m_pWndFocus; }
	void GetLogFont(C2DRender* p2DRender,LOGFONT* pLogFont);
	int  GetFontHeight();
	
	[[nodiscard]] bool IsWndStyle(DWORD dwStyle) const { return m_dwStyle & dwStyle; }
	[[nodiscard]] int  GetWndStyle() const { return m_dwStyle; }
	void SetWndStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
	void AddWndStyle(DWORD dwStyle) { m_dwStyle |= dwStyle; }
	void DelWndStyle(DWORD dwStyle) { m_dwStyle &= ~dwStyle; }

	[[nodiscard]] int GetWndId() const { return m_nIdWnd; }

private:
	static CWndBase* GetWndBase_Sub( UINT idWnd );
	CWndBase * GetChildWnd(UINT nID);
	
	template<std::derived_from<CWndBase> CWndClass>
	static CWndClass * ConvertWndBaseToWndClass(CWndBase * pWindow, const char * caller);
public:
	template<std::derived_from<CWndBase> CWndClass = CWndBase>
	static CWndClass * GetWndBase(UINT idWnd);

	template<std::derived_from<CWndBase> CWndClass = CWndBase>
	CWndClass * GetDlgItem(const UINT nID);

	static CWndBase* GetWndBase();


	BOOL IsFocusWnd() { return m_pWndFocus == this; }
	BOOL IsFocusChild() { return m_pParentWnd ? m_pParentWnd->m_pWndFocusChild == this : FALSE; }
	[[nodiscard]] CPoint GetMousePoint() const noexcept { return m_ptMouse; } 
	void SetGroup(BOOL bGroup) { m_bGroup = bGroup; }
	BOOL IsGroup() { return m_bGroup; }
	void SetFont(CD3DFont* pFont) { m_pFont = pFont; }
	CD3DFont* GetFont() { return m_pFont; }
	void SetAutoFree(BOOL bFree) { m_bAutoFree = (bFree != FALSE ); }
	BOOL IsAutoFree() { return m_bAutoFree; }
	BOOL IsParentWnd(CWndBase* pParentWnd);
	BOOL IsWindowEnabled() const { return m_bEnable; }
	void EnableWindow(BOOL bEnable = TRUE) { m_bEnable = (bEnable != FALSE ); }
	LRESULT SendMessage(UINT message,WPARAM wParam = 0, LPARAM lParam = 0);
	void PostMessage(UINT message,WPARAM wParam = 0, LPARAM lParam = 0);
	void  SetToolTip(LPCTSTR lpszToolTip,int nPos = 0) { m_strToolTip = lpszToolTip; m_nToolTipPos = nPos; }
	void SetTabStop(BOOL bTabStop) { m_bTabStop = (bTabStop != FALSE); }
	virtual void SetFocus();
	void KillFocus( CWndBase* pWndFocus, CWndBase* pWndNew );
	void SetDefault(BOOL bDefault) { m_bDefault = (bDefault != FALSE); }
	BOOL IsDefault() { return m_bDefault; }
	BOOL IsFullWnd() { return m_bFullWnd; }
	CWndBase* FindFullWnd();
	void GradationRect( C2DRender* p2DRender, CRect rect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent = 40 );
	BOOL WindowToScreen( LPPOINT lpPoint );
	BOOL WindowToScreen( LPRECT lpRect   );
	BOOL ClientToScreen( LPPOINT lpPoint );
	BOOL ClientToScreen( LPRECT lpRect   );
	BOOL ScreenToWindow( LPPOINT lpPoint );
	BOOL ScreenToWindow( LPRECT lpRect   );
	BOOL ScreenToClient( LPPOINT lpPoint );
	BOOL ScreenToClient( LPRECT lpRect   );
	
	CWndBase* GetFrameWnd();
	BOOL AdjustMinRect( CRect* pRect, int nWidth, int nHeight );
	BOOL AdjustMaxRect( CRect* pRect, int nWidth, int nHeight );

	void SetWndType( int nWndType) { m_byWndType = (BYTE)nWndType; }
	DWORD GetWndType() { return (DWORD)m_byWndType; }
	LPWNDCTRL GetWndCtrl( DWORD dwWndId ) { return m_resMng.GetAtControl( m_nIdWnd, dwWndId ); }
	LPWNDAPPLET GetWndApplet() { return m_resMng.GetAt( m_nIdWnd ); }

static void ClipStrArray(C2DRender* p2DRender,CRect rect,int nLineSpace,
						 CStringArray* pStringArray,CStringArray* pNewStringArray);

virtual void AlighWindow( CRect rcOld, CRect rcNew );
 
	BOOL IsDestroy() const;
	void Destroy(BOOL bAutoFree = FALSE); // 윈도를 파괴한다.
	void PaintChild(C2DRender* p2DRender);
	void Paint(C2DRender* p2DRender, BOOL bPaintChild = TRUE );
	void SetWndSize( int cx, int cy );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void PaintRoot( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual int  GetResizeDir(CPoint ptClient);
	virtual BOOL IsPickupSpace(CPoint point); 
	virtual	BOOL Process();
	virtual	void OnDraw(C2DRender* p2DRender);
	virtual	BOOL OnDrawIcon(CWndBase* pWndBase,C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual	void OnUpdate();
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	// message
//Protected:
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMButtonUp(UINT nFlags, CPoint point);
	virtual void OnMButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnNonClientLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnChar(UINT nChar);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnPaint();
	virtual void OnClose();
	virtual void OnDestroy();
	virtual void OnTimer(UINT nIDEvent);
	virtual void OnMove(int x,int y);
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

public:
	virtual BOOL OnSystemNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnParentNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	///virtual LRESULT WndMsgProc( UINT message, WPARAM wParam, LPARAM lParam );

	LRESULT WindowRootProc( UINT message, WPARAM wParam, LPARAM lParam );
#ifdef __V050823_JAPAN_ATOK
	virtual LRESULT WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
#else
	LRESULT WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
#endif
	virtual LRESULT DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);

	//virtual HRESULT OneTimeSceneInit();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	//virtual HRESULT FinalCleanup() { return S_OK; }
	//virtual HRESULT Render(); 
	//virtual HRESULT FrameMove();
	virtual void AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void AdjustWndBase( D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );

	void MakeVertexBuffer();
	void RenderWnd();
	
friend class CWndButton;
};

template<std::derived_from<CWndBase> CWndClass>
static CWndClass * CWndBase::GetWndBase(const UINT idWnd) {
	CWndBase * const pWnd = GetWndBase_Sub(idWnd);
	return ConvertWndBaseToWndClass<CWndClass>(pWnd, "GetDlgItem");
}

template<std::derived_from<CWndBase> CWndClass>
inline CWndClass * CWndBase::GetDlgItem(const UINT nID) {
	CWndBase * const pWnd = GetChildWnd(nID);
	if (!pWnd) {
		Error("GetDlgItem : nID=%d not Found.", nID);
	}
	return ConvertWndBaseToWndClass<CWndClass>(pWnd, "GetDlgItem");
}

template<std::derived_from<CWndBase> CWndClass>
inline CWndClass * CWndBase::ConvertWndBaseToWndClass(CWndBase * pWindow, const char * caller) {
	static constexpr bool WeLiveDangerously = false;

	if constexpr (std::is_same<CWndClass, CWndBase>::value) {
		return pWindow;
	} else if constexpr (WeLiveDangerously) {
		return reinterpret_cast<CWndClass *>(pWindow);
	} else {
		if (!pWindow) return nullptr;

		CWndClass * const ptr = dynamic_cast<CWndClass *>(pWindow);

		if (!ptr) {
			Error("%s : nID=%d is not a %s", caller, pWindow->GetWndId(), typeid(CWndClass).name());
		}

		return ptr;
	}
}

namespace strings {
	[[nodiscard]] CString CStringMaxSize(const char * text, int length);
	void ReduceSize(CString & str, int length);
}

namespace Windows {
	template <typename ... Ts>
	bool IsOpen(UINT windowId, Ts ... ts) {
		if (CWndBase::GetWndBase(windowId)) return true;

		if constexpr (sizeof...(Ts) == 0) {
			return false;
		} else {
			return IsOpen(ts...);
		}
	}

	/// Closes all the windows with the passed ids if opened.
	/// At least one app ID must be passed.
	/// Returns true if at least one window has been closed.
	template <typename ...Ts>
	bool DestroyIfOpened(UINT firstWindowToClose, Ts... otherWindowsToClose) {
		bool closedOne = false;

		if (CWndBase * const window = CWndBase::GetWndBase(firstWindowToClose)) {
			window->Destroy();
			closedOne = true;
		}

		if constexpr (sizeof...(otherWindowsToClose) != 0) {
			closedOne |= DestroyIfOpened(otherWindowsToClose...);
		}

		return closedOne;
	}

	class CWndNeuzBadAppId : public std::exception {
		UINT m_appId;
		char m_message[512];

	public:
		explicit CWndNeuzBadAppId(
			UINT appId,
			const std::source_location location = std::source_location::current()
			) : m_appId(appId) {
			const auto r = std::format_to_n(
				m_message, std::size(m_message) - 1,
				"Bad APP ID {} used in {}",
				appId, location.function_name()
			);
			*r.out = '\0';
		}

		[[nodiscard]] const char * what() const {
			return m_message;
		}
	};

}

