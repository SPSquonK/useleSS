#pragma once

#include <vector>
#include <memory>

class C2DRender;

// Notification code
#define WNM_ERRSPACE            (-2)
#define WNM_SELCHANGE           1
#define WNM_DBLCLK              2
#define WNM_SELCANCEL           3
#define WNM_SETFOCUS            4 
#define WNM_KILLFOCUS           5
#define WNM_CLICKED             6   
///////
#define WNM_ITEMCHANGING        10
#define WNM_ITEMCHANGED         11
#define WNM_INSERTITEM          12
#define WNM_DELETEITEM          13
#define WNM_DELETEALLITEMS      14
#define WNM_BEGINLABELEDITA     15
#define WNM_BEGINLABELEDITW     16
#define WNM_ENDLABELEDITA       17
#define WNM_ENDLABELEDITW       18
#define WNM_COLUMNCLICK         19
#define WNM_BEGINDRAG           20
#define WNM_BEGINRDRAG          21
///////
//#define WNM_ITEMCHANGING        30
//#define WNM_ITEMCHANGED         31 
#define WNM_ITEMCLICK           32
#define WNM_ITEMDBLCLICK        33
#define WNM_DIVIDERDBLCLICK     34
#define WNM_BEGINTRACK          35
#define WNM_ENDTRACK            36
#define WNM_TRACK               37

//  WndItemCtrl Norification Message
#define WIN_DBLCLK              40
#define WIN_ITEMDROP            41   

// ItemReceiver
#define WNM_ItemReceiver_Changed 42

//NM_CLICK
//WNM_BEGINRDRAG
//HDN_TRACK

/**
 윈도 매시지 박스에서 사용하는 버튼 종류 
 MB_OK                 
 MB_OKCANCEL           
 MB_ABORTRETRYIGNORE   
 MB_YESNOCANCEL        
 MB_YESNO              
 MB_RETRYCANCEL        
**/
// 이것은 추가된 것
#define MB_CANCEL           0x00000006L

//////////////////////////////////////////////////////////////////////////////
// CWndButton 
//////////////////////////////////////////////////////////////////////////////


class CWndMenu;

class CWndStatic : public CWndBase {
public:
	CWndStatic();
	~CWndStatic() override = default;
	
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT & rect, CWndBase * pParentWnd, UINT nID);
	
	void OnDraw(C2DRender * p2DRender) override;
	void SetWndRect(CRect rectWnd, BOOL bOnSize) override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;
	void PaintFrame(C2DRender * p2DRender) override;

	virtual BOOL IsPickupSpace(CPoint point);
};

class CWndCustom : public CWndBase
{
public:
	CWndCustom() { m_byWndType = WTYPE_CUSTOM; }
	~CWndCustom() override = default;

	void OnDraw( C2DRender* p2DRender );
	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID )	{
		if( lpszCaption && strlen( lpszCaption ) ) m_strTitle = lpszCaption;
		return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect,  pParentWnd, nID );
	}
};
class CWndButton : public CWndBase
{
protected:
	DWORD     m_nFontColor;
	DWORD     m_nPushColor;
	DWORD     m_nHighlightColor;
	DWORD     m_nDisableColor;
	BOOL      m_bCheck;
	int       m_nTimePush;
	BOOL      m_bHighLight;
	CPoint    m_ptPush;

public:
	CHAR      m_cHotkey;

	CWndMenu*  m_pWndMenu;
	SHORTCUT   m_shortcut;

	CWndButton();
	~CWndButton() override = default;

	BOOL Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);// { return Create(lpszCaption,dwStyle,rect,pParentWnd,nID,NULL,0); }
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void PaintFrame( C2DRender* p2DRender );
	virtual	BOOL Process();

	void SetMenu( CWndMenu* pWndMenu );

	void FitTextureSize();

	// Attributes
	void SetPushTime(int nTime); // 버튼을 누르고 nTime이후에 계속 OnCommand를 패어런트에게 보낸다.
	UINT GetState() const; // Retrieves the check state, highlight state, and focus state of a button control.
	void SetState(BOOL bHighlight); // Sets the highlighting state of a button control
	int  GetCheck() const; // Retrieves the check state of a button control.
	void SetCheck(int nCheck); // Sets the check state of a button control.

	void SetString( CString strSndEffect ); 
	BOOL IsHighLight() { return m_bHighLight; }
	void SetFontColor     (DWORD dwColor) { m_nFontColor      = (DWORD)dwColor; }
	void SetPushColor     (DWORD dwColor) { m_nPushColor      = (DWORD)dwColor; }
	void SetDisableColor  (DWORD dwColor) { m_nDisableColor   = (DWORD)dwColor; }
	void SetHighLightColor(DWORD dwColor) { m_nHighlightColor = (DWORD)dwColor; }
	[[nodiscard]] DWORD GetFontColor     () const noexcept { return m_nFontColor     ; }
	[[nodiscard]] DWORD GetPushColor     () const noexcept { return m_nPushColor     ; }
	[[nodiscard]] DWORD GetDisableColor  () const noexcept { return m_nDisableColor  ; }
	[[nodiscard]] DWORD GetHighLightColor() const noexcept { return m_nHighlightColor; }
	void SetPushPoint(int x,int y) { m_ptPush = CPoint(x,y); }
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual LRESULT DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
//////////////////////////////////////////////////////////////////////////////

class CWndScrollBar : public CWndBase
{
protected:
	int        m_nScrollPos;
	int        m_nScrollMinPos;
	int        m_nScrollMaxPos;
	int        m_nScrollBarIdx;
	int        m_nScrollPage;
	int        m_nPos;
	BOOL       m_bPushPad; 
	CRect      m_rectStick;
	CWndButton m_wndArrow1;
	CWndButton m_wndArrow2;

public:
	CTexture* m_pTexButtVScrUp   ;
	CTexture* m_pTexButtVScrDown ;
	CTexture* m_pTexButtVScrBar  ;
	CTexture* m_pTexButtVScrPUp  ;
	CTexture* m_pTexButtVScrPDown;
	CTexture* m_pTexButtVScrPBar ;

//static CTexturePack m_texturePack;
	
	CWndScrollBar();
	~CWndScrollBar();

	BOOL Create(DWORD dwScrollStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);//,CSprPack* pSprPack,int nSprIdx);

	int  GetMinScrollPos();
	int  GetMaxScrollPos();

	void SetMinScrollPos();
	void SetMaxScrollPos();
	int  GetScrollPos() const; // Retrieves the current position of a scroll box. 
	int  SetScrollPos(int nPos, BOOL bRedraw = TRUE); // Sets the current position of a scroll box. 
	void GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const; // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar. 
	int  GetScrollRange() const;
	void SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE); // Sets minimum and maximum position values for the given scroll bar. 
	void SetScrollPage(int nPage);
	int  GetScrollPage() const;
	int  GetScrollAbiliableRange();
	int  GetScrollAbiliablePos();
	int  GetScrollAbiliablePage(); 
	void ShowScrollBar(BOOL bShow = TRUE); // Shows or hides a scroll bar. 
	BOOL EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH); // Enables or disables one or both arrows of a scroll bar. 
	BOOL SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE); // Sets information about the scroll bar. 
	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL); // Retrieves information about the scroll bar. 
	int  GetScrollLimit(); // Retrieves the limit of the scroll bar
	void SetScrollDown();

	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);

	void MouseWheel(short zDelta);
};

//////////////////////////////////////////////////////////////////////////////
// CWndMenu

class CWndMenu : public CWndBase
{
	BOOL IsOnMenu(CPoint pt);
	int m_nLargeWidth;
protected:
	std::vector<std::unique_ptr<CWndButton>> m_wndMenuItems;
public:
	
	// Constructors
	CWndMenu();
	
	void SetVisibleSub( BOOL bVisible );
	
	BOOL CreateMenu(CWndBase* pWndParent);

	void OnKillFocus(CWndBase* pNewWnd) override;
	void PaintFrame(C2DRender* p2DRender) override;
	BOOL OnEraseBkgnd(C2DRender* p2DRender) override;
	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE ) override;
	// Attributes
	
	// CMenu Operations
	void DeleteAllMenu();

	// CMenuItem Operations
	CWndButton * AddButton(UINT nID, LPCTSTR text);

	bool CheckMenuItem(UINT nIDCheckItem, int nCheck);
	[[nodiscard]] int GetMenuState(UINT nID) const;

	void SetLargeWidth( int nLargeWidth ) { m_nLargeWidth = nLargeWidth; }
	
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );

private:
	void SetVisibleAllMenu(BOOL bVisible);


};
//////////////////////////////////////////////////////////////////////////////
// CWndText
//////////////////////////////////////////////////////////////////////////////
#define EBS_LEFT             0x00000000L
#define EBS_CENTER           0x00000001L
#define EBS_RIGHT            0x00000002L
#define EBS_MULTILINE        0x00000004L
#define EBS_UPPERCASE        0x00000008L
#define EBS_LOWERCASE        0x00000010L
#define EBS_PASSWORD         0x00000020L
#define EBS_AUTOVSCROLL      0x00000040L
#define EBS_AUTOHSCROLL      0x00000080L
#define EBS_NOHIDESEL        0x00000100L
#define EBS_OEMCONVERT       0x00000400L
#define EBS_READONLY         0x00000800L
#define EBS_WANTRETURN       0x00001000L
#define EBS_NUMBER           0x00002000L

class CWndText : public CWndBase
{
protected:
	CTimer m_timerCaret;
	BOOL   m_bCaretVisible;
	CPoint m_ptCaret;
	BOOL   m_bLButtonDown;

	DWORD m_dwBlockBegin;
	DWORD m_dwBlockEnd;
	DWORD m_dwOffset;
	
	BOOL	m_bScr;
	int		m_nLineRefresh;

public:
	void Delete( int nIndex, int nLen );
	DWORD GetBlockBegine() { return m_dwBlockBegin; }
	DWORD GetBlockEnd() { return m_dwBlockEnd; }
	
	void BlockDelete( LPCTSTR pstr, LPCTSTR pstr2 );
	void BlockInsert( LPCTSTR pstr, LPCTSTR pstr2 );
	void Insert( int nIndex, LPCTSTR pstr );
	void BlockSetStyle( DWORD dwStyle );
	void BlockClearStyle( DWORD dwStyle );
	void BlockSetColor( DWORD dwColor );
	CWndMenu m_wndMenu;
	
	TCHAR m_TextComp[3];
	TCHAR m_szCaret[ 3 ];
	BOOL m_bEnableClipboard;
	//CTexture* m_apTexture[ 9 ];
	int m_nLineSpace; 
	CWndScrollBar m_wndScrollBar;
	CEditString m_string;

	CWndText();
	~CWndText();
	BOOL Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	CPoint m_ptDeflateRect;

	virtual	void OnInitialUpdate();
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnCommand ( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual BOOL OnChildNotify ( UINT nCode, UINT nID, LRESULT* pLResult );
	virtual void OnMouseMove( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnSetFocus( CWndBase* pOldWnd );
	virtual void OnKillFocus( CWndBase* pNewWnd );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnChar( UINT nChar );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );

	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }

	LONG GetOffset( CPoint point );
	void UpdateScrollBar();
	BOOL IsEmptyBlock() { return m_dwBlockBegin == m_dwBlockEnd; }
	CPoint OffsetToPoint( DWORD dwOffset, TCHAR* pszStr );
	CPoint GetCaretPos() { return m_ptCaret; }
virtual void DrawCaret( C2DRender* p2DRender );
	void SetCaretPos( CPoint ptCaret ) { m_ptCaret = ptCaret; m_timerCaret.Reset(); }
	void HideCaret() { m_bCaretVisible = FALSE; }
	void ShowCaret() { m_bCaretVisible = TRUE; }

	void SetString( LPCTSTR pszString, DWORD dwColor = 0xff000000 );
	void AddString( LPCTSTR pszString, DWORD dwColor = 0xff000000, DWORD dwPStyle = 0x00000001 );
	void ResetString();

	static void SetupDescription(CWndText * self, LPCTSTR filename);
};
//////////////////////////////////////////////////////////////////////////////
// CWndStatic
//////////////////////////////////////////////////////////////////////////////
/*
class CWndStatic : public CWndBase
{
public:

	CWndStatic();
	~CWndStatic();
	BOOL Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);

	virtual	void OnInitialUpdate();
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void OnSize(UINT nType, int cx, int cy);

	LONG GetOffset(CPoint point);
	void UpdateScrollBar();

	CPoint OffsetToPoint(DWORD dwOffset);
	CPoint GetCaretPos() { return m_ptCaret; }
	void DrawCaret(C2DRender* p2DRender);
	void SetCaretPos(CPoint ptCaret) { m_ptCaret = ptCaret; }
	void HideCaret() { m_bCaretVisible = FALSE; }
	void ShowCaret() { m_bCaretVisible = TRUE; }

	void SetString(LPCTSTR pszString);
	void AddString(LPCTSTR pszString);
	void ResetString();
};
*/
//////////////////////////////////////////////////////////////////////////////
// CWndViewCtrl
//////////////////////////////////////////////////////////////////////////////

typedef struct tagScriptElem
{
	tagScriptElem( void );
	tagScriptElem( const CRect* pRect );
	tagScriptElem* m_lpParent;
	DWORD m_dwColor;
	DWORD m_dwSelectColor;
	CEditString m_strKeyword;
	DWORD m_dwData;
	CPtrArray m_ptrArray;
	BOOL m_bOpen;
	CWndButton* m_pWndCheckBox;
} 
TREEELEM,* LPTREEELEM;

class CWndTreeCtrl : public CWndBase
{
public:							//sun!!
	typedef struct tagITEM
	{
		CRect      m_rect;
		BOOL       m_bButton;
		LPTREEELEM m_lpTreeElem;
	} TREEITEM,* LPTREEITEM;

private:
	CPtrArray m_treeItemArray;
	void InterpriteScript( CScript& script, CPtrArray& ptrArray ); 
	virtual void PaintTree( C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray );
	LPTREEELEM m_pFocusElem;
	int  m_nFontHeight ;
	DWORD m_nWndColor   ;
	TREEELEM m_treeElem;
	CWndScrollBar m_wndScrollBar;
	void FreeTree( CPtrArray& ptrArray );
public:
	enum { CHECK_BOX_SIZE_XY = 13 };
private:
	BOOL m_bMemberCheckingMode;
	int m_nMaxCheckNumber;
	int m_nTreeTabWidth;
	int m_nCategoryTextSpace;
	int m_nTreeItemsMaxWidth;
public:
	int   m_nLineSpace  ;
	CTexture* m_pTexButtOpen;
	CTexture* m_pTexButtClose;
	DWORD m_nFontColor  ; 
	DWORD m_nSelectColor;

	CWndTreeCtrl();
	~CWndTreeCtrl();
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
	BOOL DeleteAllItems();
	void DeleteItemArray( void );
	LPTREEELEM GetCurSel();
	LPTREEELEM GetRootElem();
	LPTREEELEM GetNextElem( LPTREEELEM pElem, int& nPos );
	LPTREEELEM FindTreeElem( DWORD dwData );
	LPTREEELEM FindTreeElem( CPtrArray& ptrArray, DWORD dwData );
	LPTREEELEM SetCurSel( DWORD dwData );
	LPTREEELEM SetCurSel( LPCTSTR lpszKeyword );
	LPTREEELEM FindTreeElem( LPCTSTR lpszKeyword );
	LPTREEELEM FindTreeElem( CPtrArray& ptrArray, LPCTSTR lpszKeyword );
	LPTREEELEM InsertItem( LPTREEELEM lpParent, 
						   LPCTSTR lpString, 
						   DWORD dwData, 
						   BOOL bForbidChecking = TRUE, 
						   BOOL bCheck = FALSE, 
						   DWORD dwColor = D3DCOLOR_ARGB( 255, 64, 64, 64 ), 
						   DWORD dwSelectColor = D3DCOLOR_ARGB( 255, 0, 0, 255 ) );
	void LoadTreeScript( LPCTSTR lpFileName ); 
	BOOL Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);
	BOOL CheckParentTreeBeOpened( LPTREEELEM lpTreeElem );
	CPtrArray* GetTreeItemArray( void );
	void SetFocusElem( const LPTREEELEM pFocusElem );
	const LPTREEELEM GetFocusElem( void ) const;
	void SetLineSpace( int nLineSpace );
	int GetLineSpace( void ) const;
	void SetMemberCheckingMode( BOOL bMemberCheckingMode );
	BOOL GetMemberCheckingMode( void ) const;
	void SetMaxCheckNumber( int nMaxCheckNumber );
	int GetMaxCheckNumber( void ) const;
	void SetTreeTabWidth( int nTreeTabWidth );
	int GetTreeTabWidth( void ) const;
	void SetCategoryTextSpace( int nCategoryTextSpace );
	int GetCategoryTextSpace( void ) const;
	int GetTreeItemsNumber( void ) const;
	void SetTreeItemsMaxWidth( int nTreeItemsMaxWidth );
	int GetTreeItemsMaxWidth( void ) const;
	int GetTreeItemsMaxHeight( void );
	void SetTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor );

private:
	void CalculateTreeItemsNumber( int& nSumTreeItemsNumber, const CPtrArray& rPtrArray ) const;
	void CalculateTreeItemsMaxHeight( int& nSumHeight, const CPtrArray& rPtrArray );
	void CalculateTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor, const CPtrArray& rPtrArray );

public:
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual BOOL OnEraseBkgnd( C2DRender* p2DRender );

	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	
};
//////////////////////////////////////////////////////////////////////////////
// CWndSliderCtrl
//////////////////////////////////////////////////////////////////////////////

class CWndSliderCtrl : public CWndBase
{
public:
};
//////////////////////////////////////////////////////////////////////////////
// CWndListBox
//////////////////////////////////////////////////////////////////////////////

/*
 * Listbox Styles
 */
#define WLBS_NOTIFY            0x0001L
#define WLBS_SORT              0x0002L
#define WLBS_NOREDRAW          0x0004L
#define WLBS_MULTIPLESEL       0x0008L
#define WLBS_OWNERDRAWFIXED    0x0010L
#define WLBS_OWNERDRAWVARIABLE 0x0020L
#define WLBS_HASSTRINGS        0x0040L
#define WLBS_USETABSTOPS       0x0080L
#define WLBS_NOINTEGRALHEIGHT  0x0100L
#define WLBS_MULTICOLUMN       0x0200L
#define WLBS_WANTKEYBOARDINPUT 0x0400L
#define WLBS_EXTENDEDSEL       0x0800L
#define WLBS_DISABLENOSCROLL   0x1000L
#define WLBS_NODATA            0x2000L
#define WLBS_NOSEL             0x4000L
#define WLBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

#include <boost/container/stable_vector.hpp>

class CWndListBox : public CWndBase
{
	void PaintListBox(C2DRender * p2DRender, CPoint & pt);

public:
	struct LISTITEM {
		CRect       m_rect       = CRect(0, 0, 0, 0);
		CEditString m_strWord    = _T("");
		BOOL        m_bIsValid   = TRUE;
		DWORD       m_dwData     = 0;
		CString     m_strKey     = _T("");
	};

protected:
	boost::container::stable_vector<LISTITEM> m_listItemArray;
	LISTITEM * m_pFocusItem;
	int           m_nCurSelect  ;
	DWORD         m_nWndColor   ;
	CWndScrollBar m_wndScrollBar;
public:
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;
private:
	DWORD         m_dwOnMouseColor;
	DWORD         m_dwInvalidColor;
	DWORD         m_nLeftMargin;
public:
	int           m_nLineSpace  ;
	int           m_nFontHeight ;

	CWndListBox();
	~CWndListBox() override;

	void Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
	int   GetCount() const;
	DWORD GetItemData(int nIndex) const;
	void* GetItemDataPtr(int nIndex) const;
	int   SetItemData(int nIndex,DWORD dwItemData);
	int   SetItemDataPtr(int nIndex,void* pData);
	BOOL GetItemValidity( int nIndex );
	const CRect& GetItemRect( int nIndex ) const;
	void  GetText(int nIndex,CString& rString) const;
	int   GetTextLen(int nIndex) const;
	
	int   GetCurSel() const;
	int   SetCurSel(int nSelect);
	int   GetScrollPos() {return m_wndScrollBar.GetScrollPos();};
	void  SetScrollPos( int nPos, BOOL bRedraw = TRUE ) { m_wndScrollBar.SetScrollPos( nPos, bRedraw ); }	//gmpbigsun: added
	LISTITEM & AddString(LPCTSTR lpszItem);
	int   DeleteString(UINT nIndex);
	void	SetString( int nIndex, LPCTSTR lpszItem );
	const CString& GetString( int nIndex ) const;
	void SetKeyString( int nIndex, LPCTSTR lpszItem );
	const CString& GetKeyString( int nIndex ) const;


	void SetLeftMargin( int nLeftMargin );

	int GetItemIndex( const CString& strItem ) const;
	int GetItemIndex( DWORD dwItem ) const;

	void  ResetContent();
	int   FindString(int nStartAfter,LPCTSTR lpszItem) const;
	int   SelectString(int nStartAfter,LPCTSTR lpszItem);
	void  SortListBox();

	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSetFocus( CWndBase* pOldWnd );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
};
// class CListCtrl  CButton


#include "WndTListBox.hpp"

/////////////////////////////////////////////////////////////////////////////
// CListCtrl
// begin_r_commctrl
// List view Control

#define WLVS_ICON                0x0000
#define WLVS_REPORT              0x0001
#define WLVS_SMALLICON           0x0002
#define WLVS_LIST                0x0003
#define WLVS_TYPEMASK            0x0003
#define WLVS_SINGLESEL           0x0004
#define WLVS_SHOWSELALWAYS       0x0008
#define WLVS_SORTASCENDING       0x0010
#define WLVS_SORTDESCENDING      0x0020
#define WLVS_SHAREIMAGELISTS     0x0040
#define WLVS_NOLABELWRAP         0x0080
#define WLVS_AUTOARRANGE         0x0100
#define WLVS_EDITLABELS          0x0200
#define WLVS_NOSCROLL            0x2000
#define WLVS_TYPESTYLEMASK       0xfc00
#define WLVS_ALIGNTOP            0x0000
#define WLVS_ALIGNLEFT           0x0800
#define WLVS_ALIGNMASK           0x0c00
#define WLVS_OWNERDRAWFIXED      0x0400
#define WLVS_NOCOLUMNHEADER      0x4000
#define WLVS_NOSORTHEADER        0x8000

// end_r_commctrl

class CWndEdit;

class CWndListCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	LVITEM*       m_pFocusItem;
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	DWORD         m_nWndColor   ;
	CWndScrollBar m_wndScrollBar;
	CPtrArray     m_aItems;
	CPtrArray     m_aColumns;

// Constructors
public:
	int           m_nLineSpace  ;
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;

	CWndListCtrl();
	~CWndListCtrl();

	void Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
// Attributes
	int   GetCurSel() const;
	int   SetCurSel(int nSelect);

	int GetItemCount() const;
	BOOL GetItem(LVITEM* pItem) const;
	BOOL SetItem(const LVITEM* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
	UINT GetCallbackMask() const;
	BOOL SetCallbackMask(UINT nMask);
	int GetNextItem(int nItem, int nFlags) const;
	POSITION GetFirstSelectedItemPosition() const;
	int GetNextSelectedItem(POSITION& pos) const;
	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const;
	BOOL SetItemPosition(int nItem, POINT pt);
	BOOL GetItemPosition(int nItem, LPPOINT lpPoint) const;
	int GetStringWidth(LPCTSTR lpsz) const;
	CWndEdit* GetEditControl() const;
	BOOL GetColumn(int nCol, LVCOLUMN* pColumn) const;
	BOOL SetColumn(int nCol, const LVCOLUMN* pColumn);
	int GetColumnWidth(int nCol) const;
	BOOL SetColumnWidth(int nCol, int cx);
	BOOL GetViewRect(LPRECT lpRect) const;
	COLORREF GetTextColor() const;
	BOOL SetTextColor(COLORREF cr);
	COLORREF GetTextBkColor() const;
	BOOL SetTextBkColor(COLORREF cr);
	int GetTopIndex() const;
	int GetCountPerPage() const;
	BOOL GetOrigin(LPPOINT lpPoint) const;
	BOOL SetItemState(int nItem, LVITEM* pItem);
	BOOL SetItemState(int nItem, UINT nState, UINT nMask);
	UINT GetItemState(int nItem, UINT nMask) const;
	CString GetItemText(int nItem, int nSubItem) const;
	int GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const;
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	void SetItemCount(int nItems);
	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem) const;
	UINT GetSelectedCount() const;
	BOOL SetColumnOrderArray(int iCount, LPINT piArray);
	BOOL GetColumnOrderArray(LPINT piArray, int iCount = -1);
	CSize SetIconSpacing(CSize size);
	CSize SetIconSpacing(int cx, int cy);

	BOOL GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& ref);
	int GetHotItem();
	int SetHotItem(int iIndex);
	int GetSelectionMark();
	int SetSelectionMark(int iIndex);
	DWORD GetExtendedStyle();
	DWORD SetExtendedStyle(DWORD dwNewStyle);
	int SubItemHitTest(LPLVHITTESTINFO pInfo);
	void SetWorkAreas(int nWorkAreas, LPRECT lpRect);
	BOOL SetItemCountEx(int iCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL);
	CSize ApproximateViewRect(CSize sz = CSize(-1, -1), int iCount = -1) const;
	BOOL GetBkImage(LVBKIMAGE* plvbkImage) const;
	DWORD GetHoverTime() const;
	void GetWorkAreas(int nWorkAreas, LPRECT prc) const;
	BOOL SetBkImage(HBITMAP hbm, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0);
	BOOL SetBkImage(LPTSTR pszUrl, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0);
	BOOL SetBkImage(LVBKIMAGE* plvbkImage);
	DWORD SetHoverTime(DWORD dwHoverTime = (DWORD)-1);
	UINT GetNumberOfWorkAreas() const;
	BOOL GetCheck(int nItem) const;
	BOOL SetCheck(int nItem, BOOL fCheck = TRUE);

// Operations
	int InsertItem(const LVITEM* pItem);
	int InsertItem(int nItem, LPCTSTR lpszItem);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);

	int HitTest(CPoint pt, UINT* pFlags = NULL) const;
	BOOL EnsureVisible(int nItem, BOOL bPartialOK);
	BOOL Scroll(CSize size);
	CEdit* EditLabel(int nItem);

// Overridables
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void PaintFrame(C2DRender* p2DRender);
// Implementation
public:
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
		UINT nStateMask, int nImage, LPARAM lParam);

};


//////////////////////////////////////////////////////////////////////////////
// CWndGroupBox
//////////////////////////////////////////////////////////////////////////////

class CWndGroupBox : public CWndBase
{
protected:

public:
	CWndGroupBox();
	~CWndGroupBox();
	BOOL Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );

	virtual	void OnInitialUpdate();
	//virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	//virtual void OnLButtonDown(UINT nFlags, CPoint point);
	//virtual void OnLButtonUp(UINT nFlags, CPoint point);
	//virtual void OnMouseMove(UINT nFlags, CPoint point);
	//virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	//virtual void OnSetFocus(CWndBase* pOldWnd);
	//virtual void OnKillFocus(CWndBase* pNewWnd);aa
	//virtual void OnSize(UINT nType, int cx, int cy);
};
/////////////////////////////////////////////////////////////////////////////
// CTabCtrl

class CWndTabCtrl : public CWndBase {
	struct WTCITEM {
		LPCTSTR pszText;
		CWndBase * pWndBase;
	};

	std::vector<WTCITEM> m_aTab;

// Constructors
	size_t m_nCurSelect;
	CTexture m_aTexture[ 6 ];
	int m_nTabButtonLength;
public:
	enum TabTitleAlign { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER };

private:
	TabTitleAlign m_eTabTitleAlign;
public:
	CWndTabCtrl();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID);

	void SetButtonLength(const int nLength) { m_nTabButtonLength = nLength; }
	[[nodiscard]] size_t GetSize() const { return m_aTab.size(); }
	
	void PaintFrame(C2DRender * p2DRender) override;
	void OnDraw(C2DRender * p2DRender) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnSize(UINT nType, int cx, int cy) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;
	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE) override;
	void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 ) override;
	void OnInitialUpdate() override;
	
// Attributes
	[[nodiscard]] CWndBase * GetTabItem(size_t nItemNumber) const;
	[[nodiscard]] CWndBase * GetSelectedTab() const;
	void SetTabTitleAlign( const TabTitleAlign eTabTitleAlign );
	const TabTitleAlign GetTabTitleAlign( void ) const;

	[[nodiscard]] size_t GetCurSel() const { return m_nCurSelect; }
	void SetCurSel(size_t nItem);

// Operations
	void InsertItem(CWndBase * window, LPCTSTR tabText);

	friend CAr & operator<<(CAr & ar, const CWndTabCtrl & tab);
	friend CAr & operator>>(CAr & ar, CWndTabCtrl & tab);
};

//////////////////////////////////////////////////////////////////////////////
// CWndComboBox
//////////////////////////////////////////////////////////////////////////////

#define WCBS_SIMPLE            0x0001L
#define WCBS_DROPDOWN          0x0002L
#define WCBS_DROPDOWNLIST      0x0003L
#define WCBS_OWNERDRAWFIXED    0x0010L
#define WCBS_OWNERDRAWVARIABLE 0x0020L
#define WCBS_AUTOHSCROLL       0x0040L
#define WCBS_OEMCONVERT        0x0080L
#define WCBS_SORT              0x0100L
#define WCBS_HASSTRINGS        0x0200L
#define WCBS_NOINTEGRALHEIGHT  0x0400L
#define WCBS_DISABLENOSCROLL   0x0800L
#define WCBS_UPPERCASE         0x2000L
#define WCBS_LOWERCASE         0x4000L

#include "WndEditCtrl.h"

class CWndComboListBox : public CWndListBox
{
public:
#ifdef __IMPROVE_MAP_SYSTEM
	virtual	void PaintFrame( C2DRender* p2DRender );
#endif // __IMPROVE_MAP_SYSTEM
	virtual void OnKillFocus( CWndBase* pNewWnd );
};

class CWndComboBox : public CWndEdit
{
public: 
//	DWORD m_dwComboBoxStyle; 
	CWndComboListBox m_wndListBox;
	CWndButton  m_wndButton;

	CWndComboBox();
	~CWndComboBox();

// Attributes
	// for entire combo box
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	LCID GetLocale() const;
	LCID SetLocale(LCID nNewLocale);
// Win4
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	int InitStorage(int nItems, UINT nBytes);
	void SetHorizontalExtent(UINT nExtent);
	UINT GetHorizontalExtent() const;
	int SetDroppedWidth(UINT nWidth);
	int GetDroppedWidth() const;

	// for edit control
	DWORD GetEditSel() const;
	BOOL LimitText(int nMaxChars);
	BOOL SetEditSel(int nStartChar, int nEndChar);

	// for combobox item
	DWORD GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD dwItemData);
	void* GetItemDataPtr(int nIndex) const;
	int SetItemDataPtr(int nIndex, void* pData);

	DWORD GetSelectedItemData( void ) const;
	void GetListBoxText( int nIndex, CString& strString ) const;
	int GetListBoxTextLength( int nIndex ) const;
	int GetListBoxSize( void ) const;

	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int SetExtendedUI(BOOL bExtended = TRUE);
	void GetDroppedControlRect(LPRECT lprect) const;
	BOOL GetDroppedState() const;

// Operations
	// for drop-down combo boxes
	void ShowDropDown(BOOL bShowIt = TRUE);

	// manipulating listbox items
	CWndListBox::LISTITEM & AddString(LPCTSTR lpszString);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszString);
	void ResetContent();
	int Dir(UINT attr, LPCTSTR lpszWildCard);

	// selection helpers
	int FindString(int nStartAfter, LPCTSTR lpszString) const;
	int SelectString(int nStartAfter, LPCTSTR lpszString);

	// Clipboard operations
	void Clear();
	void Copy();
	void Cut();
	void Paste();

#ifdef __IMPROVE_MAP_SYSTEM
	void SelectItem( const CString& strItem );
	void SelectItem( DWORD dwItem );

private:
	void SetSelectedItemInformation( void );

public:
#endif // __IMPROVE_MAP_SYSTEM

	void Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	virtual	void OnInitialUpdate();
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
private:
	void OpenListBox( void );
	BOOL m_bOpen;
};
