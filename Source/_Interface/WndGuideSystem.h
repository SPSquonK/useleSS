// GuideSystem.h: interface for the CGuideSystem class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __WNDGUIDESYSTEM__H
#define __WNDGUIDESYSTEM__H

#include "wndTutorial.h"

typedef struct GUIDE_STRUCT
{
	CString m_str;
	int     m_nkey;
	int     m_nEventMsg;
	BOOL    m_bBeginner;
	BOOL    m_bFlag;
	BOOL    m_bIsClear;
	int		m_nShowLevel;
	int		m_nSequence;
	int		m_nVicCondition;
	int		m_nLevel;
	UINT	m_nInput;
	CString m_strInput;
	
	GUIDE_STRUCT() 
	{
		init();
	};

	void init()
	{
		m_nLevel = m_nInput = m_nkey = m_nEventMsg = m_bBeginner = m_bFlag = m_nShowLevel = m_nSequence = m_nVicCondition = 0;
		m_strInput.Empty();
		m_str.Empty();
	};
} GUIDE_STRUCT;

class CWndGuideTextMgr final : public CWndNeuz 
{ 

public: 
	CString		m_strHelpKey;
	CTexture*	m_pTextureBG;
	CRect       m_Rect[4];
	int						  m_nCurrentVector;

	std::vector<GUIDE_STRUCT>	  m_VecGuideText;

	GUIDE_STRUCT GetGuideText()
	{
		return m_VecGuideText[m_nCurrentVector];
	}


	void AddGuideText( GUIDE_STRUCT guide );
	void _SetGuideText( GUIDE_STRUCT guide, bool bIsNext);
	CWndGuideTextMgr(); 
	~CWndGuideTextMgr(); 
	
	void UpDate();
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	BOOL OnEraseBkgnd( C2DRender* p2DRender );
}; 

class CWndGuideSelection final : public CWndNeuz 
{
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
};

	typedef std::map<int, GUIDE_STRUCT>::value_type mgValType;
	typedef std::map<int, GUIDE_STRUCT>::iterator mgMapItor;

#define NO_CONDITION		0
#define CAMERA_ROTATION		1
#define CAMERA_ZOOMED		2
#define INPUT_KEY			3
#define INPUT_STRING		4
#define MOVE_ON_MOUSE		5
#define MOVE_ON_KEY			6
#define	OPEN_WINDOW			7

struct CONDITION
{
	bool		bIsCamMove;
	bool		bIsCamZoomed;
	UINT		nInputKey;
	bool		bIsClickOnLand;
	bool		bIsKeyMove;
	int			nOpenedWindowID;
	CString		strInput;

	void Init() 
	{
		bIsKeyMove = bIsCamMove = bIsCamZoomed = bIsClickOnLand = false;
		nOpenedWindowID = nInputKey = 0;
		strInput.Empty();
	};
};

class CWndInfoPang final : public CWndNeuz
{

public:
	virtual ~CWndInfoPang(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};


class CWndGuideSystem final : public CWndNeuz 
{
public:
	BOOL				 m_bIsLoad;
	enum { ANI_INTRO = 0, ANI_IDLE, ANI_BYTE };
	enum { KEY = 0, STR };
	DWORD				 m_dwGuideLevel;
	DWORD				 m_dwTime;
	BYTE				 m_bAniState;
	std::vector<GUIDE_STRUCT>	m_vecEventGuide;
	std::map<int, GUIDE_STRUCT>  m_mapGuide;
	mgMapItor				m_CurrentIter;
	mgMapItor				m_EmptyIter;
	GUIDE_STRUCT			m_CurrentGuide;
	CONDITION				m_Condition;
	BOOL CheckCompletion(GUIDE_STRUCT Guide);
	BOOL PassToNext();
	CWndTutorial*		m_pWndTutorialView;
	bool				m_bIsViewVisible;
	CModelObject*		 m_pModel;
	CWndGuideTextMgr*    m_wndGuideText;
	CWndGuideSelection*  m_wndGuideSelection;
	
	CWndMenu			m_wndMenuPlace;
	
	
public:
	void SetAni(int nJob, int nAniKind);
	void ChangeModel( int nJob );
	void CreateGuideSelection();
	void GuideStart(BOOL ischart = FALSE);
	void PushBack( GUIDE_STRUCT guide );
	BOOL	m_bIsGuideChart[2];

	CWndGuideSystem();
	virtual ~CWndGuideSystem();

	void		 SendGuideMessage( int nMsg );
	BOOL		 LoadGuide( LPCTSTR lpszFileName );
		
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL Process();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
};












#endif // __WNDGUIDESYSTEM__H
