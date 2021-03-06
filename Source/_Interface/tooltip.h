#ifndef _TOOLTIP_H_
#define _TOOLTIP_H_

enum 
{
	FIRST_TT,
//	SECOND_TT,
//	THIRD_TT,
	MAX_TT
};

class CToolTip
{
	BOOL        m_bEnable       ;
	BOOL        m_bPutToolTip   ;
	BOOL        m_bReadyToopTip ;
	DWORD       m_dwToolTipId   ;
	CEditString m_strToolTip    ;
	CTimer      m_timerToopTip  ;
	CPoint      m_ptToolTip     ;
	CPoint      m_ptOldToolTip  ;
	CRect       m_rect          ;
	CRect       m_rectRender    ;
	int         m_nPosition     ;
	BOOL        m_bToolTip      ;
	int         m_nAlpha        ;
	CTexture	m_apTextureToolTip[ MAX_TT * 9 ];
	int			m_nAdded;
	int			m_nAddedJewel[5];
	int			m_nSlot;
	CItemElem *	m_pUltimateItemBase;
	CTexture*	m_pUltimateTexture;
	CTexture*	m_pJewelBgTexture;

#ifndef __IMPROVE_MAP_SYSTEM
	int			m_nMonInfoCnt;
	DWORD		m_pDwMonId[5];
#endif // __IMPROVE_MAP_SYSTEM
	int			m_nSubToolTipFlag;
	CRect		m_nRevisedRect;
	int			m_nSubToolTipNumber;
#ifdef __IMPROVE_MAP_SYSTEM
	vector< DWORD > m_vecMapMonsterID;
#endif // __IMPROVE_MAP_SYSTEM
public:
	CToolTip();
	~CToolTip();
	void SetPosition( int nPos ) { m_nPosition = nPos; }
	void SetEnable( BOOL bEnable ) { m_bEnable = bEnable; }
	BOOL GetEnable() { return m_bEnable; }
	void CancelToolTip();

	void PutToolTip( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void PutToolTip( DWORD dwToolTipId, CString& string, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void PutToolTip( DWORD dwToolTipId, LPCTSTR lpszString, CRect rect, CPoint pt, int nToolTipPos = 0 );	
	void PutToolTipEx( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos = 0, int nSubToolTipFlag = 0 );
	void Process( CPoint pt, C2DRender* p2DRender );
	void Paint( C2DRender* p2DRender );
	void Delete();
	void InitTexture();
	void SetUltimateToolTip(CItemElem * pItemBase);
#ifndef __IMPROVE_MAP_SYSTEM
	void SetWorldMapMonsterInfo(int nMonCnt, DWORD* pDwMonId);
#endif // __IMPROVE_MAP_SYSTEM
#ifdef __IMPROVE_MAP_SYSTEM
	void ResizeMapMonsterToolTip( void );
	void InsertMonsterID( DWORD dwMonsterID );
#endif // __IMPROVE_MAP_SYSTEM
	const CPoint& GetPointToolTip( void ) const;
	const CRect& GetRect( void ) const;
	void SetRenderRect( const CRect& rectRender );
	const CRect& GetRenderRect( void ) const;
	const CRect& GetRevisedRect( void ) const;
	void SetSubToolTipNumber( int nSubToolTipNumber );
	int GetSubToolTipNumber( void ) const;
	BOOL GetReadyToolTipSwitch( void ) const;
};
/*
#define TOOLTIP_COMBATMODE   1  // "????/????\n??????[C]"
#define TOOLTIP_INVENTORY    2  // "??????\n??????[I]"
#define TOOLTIP_OPTION       3  // "????\n??????[ALT-X]"
#define TOOLTIP_STATUS       4  // "??????????\n??????[S]"

#define TOOLTIP_EQUIPMENT    5  // "????????\n??????[E]"
#define TOOLTIP_KNOWLEDGE    6  // "????????\n??????[K]"
#define TOOLTIP_LOOK         7  // "??????????\n??????[L]"

#define TOOLTIP_SLOT         8  // "?????? ????"  
#define TOOLTIP_SLOT_F1      9  // "1?? ???? ????\n??????[F1]"
#define TOOLTIP_SLOT_F2      10 // "2?? ???? ????\n??????[F2]"
#define TOOLTIP_SLOT_F3      11 // "3?? ???? ????\n??????[F3]"

#define TOOLTIP_CHAT         12 // "???? ??????\n??????[ENTER]"
#define TOOLTIP_CHAT1        14 // "???? ????"
#define TOOLTIP_CHAT2        15 // "??????"
#define TOOLTIP_CHAT3        16 // 

#define TOOLTIP_MESSAGE      17 // "?????? ??????"
*/
#endif