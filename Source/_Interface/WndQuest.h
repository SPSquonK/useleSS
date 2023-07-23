#ifndef __WNDQUEST__H
#define __WNDQUEST__H

#include "TreeInformation.h"

extern CTreeInformationManager g_QuestTreeInfoManager;

class CWndQuestTreeCtrl : public CWndTreeCtrl {
public:
	BOOL OnChildNotify(UINT nCode, UINT nID, LRESULT * pLResult) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;
};

class CWndRemoveQuest : public CWndCustomMessageBox {
	QuestId m_nRemoveQuestId;
public:
	CWndRemoveQuest(const QuestId nRemoveQuestId) : m_nRemoveQuestId(nRemoveQuestId) {}
	
	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
};

class CWndQuest final : public CWndNeuz
{ 
public: 
	using TreeElems = boost::container::stable_vector<TREEELEM>;

	CWndQuest(); 
	~CWndQuest();

	enum { MAX_CHECK_NUMBER = 5 };

	int m_idSelQuest;
	std::vector<QuestId> m_aOpenTree;
	void Update( int nNewQuestId = -1 );
	void RemoveQuest( void );
	void ControlOpenTree( const LPTREEELEM lpTreeElem );

	void TreeOpen();
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	CString MakeQuestString( CString& string, BOOL bCond ); 
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );

private:
	CWndQuestTreeCtrl* GetQuestTreeSelf( QuestId dwQuestID );
	void OpenTreeArray( TreeElems & rPtrArray, BOOL bOpen = TRUE );
	void InsertQuestItem(QuestId dwQuestID, std::set<QuestId> & raOldHeadQuestID, BOOL bCompleteQuest, int nNewQuestId = -1 );
	[[nodiscard]] bool IsCheckedQuestID(QuestId dwQuestID) const;

private:
	CWndQuestTreeCtrl m_WndScenario;
	CWndQuestTreeCtrl m_WndNormal;
	CWndQuestTreeCtrl m_WndRequest;
	CWndQuestTreeCtrl m_WndEvent;
}; 

class CWndQuestDetail final : public CWndNeuz
{
public:
	class CWndQConditionTreeCtrl : public CWndTreeCtrl {
	public:
		void OnLButtonDown(UINT nFlags, CPoint point) override;
	};

	CWndQuestDetail( QuestId dwQuestID = QuestIdNone );

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );

public:
	void UpdateQuestText( BOOL bClick = FALSE );
	void UpdateQuestDetailText( QuestId dwQuestID, LPQUEST lpQuest, BOOL bComplete, BOOL bClick = FALSE );
	void SetQuestID(QuestId dwQuestID) { m_dwQuestID = dwQuestID; }
	[[nodiscard]] QuestId GetQuestID() const { return m_dwQuestID; }

private:
	QuestId m_dwQuestID = QuestIdNone;
	CWndQConditionTreeCtrl m_WndQConditionTreeCtrl;
};

void MakeQuestConditionItems( QuestId dwQuestID, 
						      CWndTreeCtrl* pWndTreeCtrl, 
							  BOOL bClick = FALSE, 
							  LPTREEELEM lpTreeElem = NULL, 
							  DWORD dwStartColor = D3DCOLOR_ARGB( 255, 0, 0, 0 ), 
							  DWORD dwEndColor = D3DCOLOR_ARGB( 255, 0, 0, 0 ), 
							  DWORD dwSelectColor = D3DCOLOR_ARGB( 255, 0, 0, 255 ) );
const CString MakeString( const CString& string, BOOL bCond );
DWORD MakeTextColor( DWORD dwStartColor = D3DCOLOR_ARGB( 255, 0, 0, 0 ), DWORD dwEndColor = D3DCOLOR_ARGB( 255, 0, 0, 0 ), int nCurrentNumber = 0, int nCompleteNumber = 0 );
QuestId GetRootHeadQuest( QuestId dwHeadQuest );
DWORD SetQuestDestinationInformation( QuestId dwQuestID, DWORD dwGoalIndex );
void ProcessQuestDestinationWorldMap( DWORD dwGoalTextID );

#endif
