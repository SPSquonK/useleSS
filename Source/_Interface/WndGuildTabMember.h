#ifndef __WNDGUILD_TAB_MEMBER__H
#define __WNDGUILD_TAB_MEMBER__H

#include <vector>

struct MEMBERLIST
{
	DWORD	idPlayer;	// 플레이어 아이디.
	int nMemberLv;		// 호칭
	int nJob;			// 직업 
	int nSex;			// 상
	int	nLevel;			// 캐릭레벨
	BOOL bIsOnLine;     // 온라인, 오프라인
	int		nGiveGold;					// 길드에 기부한 페냐
	DWORD	dwGivePxpCount;				// 길드에 기부한 PXP횟수( 스킬 경험치 )
	short	nWin;							// 무엇을 이겼지?
	short	nLose;						// 무엇을 졌을까나?
	char szName[ MAX_NAME ];	// 이름.
	char szAlias[ MAX_GM_ALIAS ];	// 별칭
	int nClass;				// 등급
};

class CWndGuildTabMember : public CWndNeuz 
{ 
	int		m_nCurrentList;			// 출력될 멤버리스트의 시작 인덱스.
	int		m_nSelect;

	int		m_nMxOld, m_nMyOld;			// 과거 좌표.
	CWndMenu	m_wndMenu;			// 팝업 메뉴.
	CWndScrollBar m_wndScrollBar;

	BOOL m_bSortbyMemberLevel;
	BOOL m_bSortbyLevel;
	BOOL m_bSortbyJob;
	BOOL m_bSortbyName;
	BOOL m_bSortbyAlias;

public: 
	void UpdateData();
	std::vector<MEMBERLIST> m_list;

	CWndGuildTabMember(); 

	CRect	m_rectParent;			// 길드창의 좌상귀 좌표.
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );

	void SortbyMemberLevel();
	void SortbyJob();
	void SortbyLevel();
	void SortbyName();
	void SortbyAlias();

	[[nodiscard]] static int GetMemberLevelIcon(int nbClass);
}; 

#endif