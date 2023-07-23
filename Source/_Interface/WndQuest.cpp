#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndQuest.h"
#include "party.h"
#include "dpclient.h"
#include "definequest.h"
#include "WorldMap.h"
#include "sqktd/util.hpp"
#ifdef __IMPROVE_MAP_SYSTEM
#include "WndMapEx.h"
#endif // __IMPROVE_MAP_SYSTEM

CTreeInformationManager g_QuestTreeInfoManager;

BOOL CWndRemoveQuest::Initialize(CWndBase * pWndParent) {
	CRect rect = g_WndMng.MakeCenterRect(250, 130);
	Create("", MB_OKCANCEL, rect, APP_MESSAGEBOX);

	m_wndText.SetString(prj.GetText(TID_GAME_QUEST_DELCONFIRM));
	m_wndText.ResetString();
	if (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1) {	// FPS모드일때
		g_WndMng.m_pWndWorld->SetMouseMode(0);
	}
	return TRUE;
}

BOOL CWndRemoveQuest::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (message == WNM_CLICKED) {
		switch (nID) {
			case IDCANCEL:
				Destroy();
				break;
			case IDOK:
				if (g_pPlayer->FindQuest(m_nRemoveQuestId)) {
					g_DPlay.SendRemoveQuest(m_nRemoveQuestId);
				}
				Destroy();
				break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


CWndQuest::CWndQuest() 
{ 
	m_idSelQuest = -1;
}     
CWndQuest::~CWndQuest() 
{ 
	if( g_WndMng.m_pWndQuestDetail )
		SAFE_DELETE( g_WndMng.m_pWndQuestDetail );
} 
void CWndQuest::OnDraw( C2DRender* p2DRender ) 
{ 
	//CWndText* pText4 = (CWndText*)GetDlgItem( WIDC_TEXT4 );
	//CRect rect = pText4->GetWindowRect( TRUE );
	//p2DRender->RenderTexture( rect.TopLeft(), &m_texChar, 255, 0.3, 0.3 );
} 
void CWndQuest::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		{
			ar >> m_idSelQuest;

			std::uint32_t nNum; ar >> nNum;
			for (std::uint32_t i = 0; i < nNum; ++i) {
				ar >> m_aOpenTree.emplace_back();
			}

			TreeOpen();
			RemoveQuest();
		}
	}
	else
	{
		dwVersion = 1;
		ar << m_idSelQuest;
		ar << static_cast<std::uint32_t>(m_aOpenTree.size());
		for (const QuestId questId : m_aOpenTree) {
			ar << questId;
		}
	}
}
void CWndQuest::TreeOpen() 
{
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );
	CWndQuestTreeCtrl* pTreeCtrl = ( CWndQuestTreeCtrl* )pWndTabCtrl->GetSelectedTab();
	assert( pTreeCtrl );
	OpenTreeArray( pTreeCtrl->GetRootElem()->m_ptrArray );
}
void CWndQuest::Update( int nNewQuestId ) 
{
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );

	for (size_t i = 0; i != pWndTabCtrl->GetSize(); ++i) {
		CWndQuestTreeCtrl * pTreeCtrl = static_cast<CWndQuestTreeCtrl *>(pWndTabCtrl->GetTabItem(i));
		pTreeCtrl->DeleteAllItems();
	}


	std::set<QuestId> aOldHeadData;
	if( CTreeInformationManager::m_eQuestListGroup != CTreeInformationManager::COMPLETE_QUEST_LIST )
	{
		if (g_pPlayer->m_quests) {
			for (const QUEST & quest : g_pPlayer->m_quests->current) {
				InsertQuestItem(quest.m_wId, aOldHeadData, FALSE);
			}
		}
	}
	if( CTreeInformationManager::m_eQuestListGroup != CTreeInformationManager::CURRENT_QUEST_LIST )
	{
		if (g_pPlayer->m_quests) {
			for (const QuestId wQuest : g_pPlayer->m_quests->completed) {
				InsertQuestItem(wQuest, aOldHeadData, TRUE, nNewQuestId);
			}
		}
	}
	if( nNewQuestId != -1 )
		m_idSelQuest = nNewQuestId;
	TreeOpen();
	RemoveQuest();
}

CString CWndQuest::MakeQuestString( CString& string, BOOL bCond ) 
{
	CString strResult;
	if( bCond )
		strResult = "#s#cffa0a0a0" + string + "#ns#nc\n";
	else
		strResult = string + "\n";
	return strResult;
}

void CWndQuest::RemoveQuest( void )
{
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );

	CWndQuestTreeCtrl* pTreeCtrl = ( CWndQuestTreeCtrl* )pWndTabCtrl->GetSelectedTab();
	assert( pTreeCtrl );
	CWndButton* pWndRemove = (CWndButton*)GetDlgItem( WIDC_REMOVE );
	assert( pWndRemove );
	LPQUEST lpQuest = NULL;
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	QuestId nQuest = QuestIdNone;
	if( lpTreeElem )
	{
		nQuest = QuestId::From(lpTreeElem->m_dwData);
		lpQuest = g_pPlayer->FindQuest(nQuest);
	}
	BOOL bComplete = nQuest != QuestIdNone && g_pPlayer->IsCompleteQuest(nQuest);
	if( lpQuest || bComplete )
	{
		CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG_EX );
		const QuestProp * pQuestProp = nQuest.GetProp();
		// 운영자는 언제나 퀘스트를 지울 수 있다
		if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) && g_Option.m_bOperator )
			pWndRemove->EnableWindow( TRUE );
		else if( bComplete || lpQuest->m_nState == QS_END || pWndDialog )
			pWndRemove->EnableWindow( FALSE );
		else
		{
			if( pQuestProp->m_bNoRemove )
				pWndRemove->EnableWindow( FALSE );
			else
				pWndRemove->EnableWindow( TRUE );
		}
	}
	else
		pWndRemove->EnableWindow( FALSE );
}

void CWndQuest::ControlOpenTree(const LPTREEELEM lpTreeElem) {
	const QuestId questId = QuestId::From(lpTreeElem->m_dwData);
	if (lpTreeElem->m_bOpen == TRUE) {
		m_aOpenTree.emplace_back(questId);
	} else {
		const auto it = std::find(m_aOpenTree.begin(), m_aOpenTree.end(), questId);
		if (it != m_aOpenTree.end()) m_aOpenTree.erase(it);
	}
}

void CWndQuest::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndTabCtrl* pWndTabCtrl = ( CWndTabCtrl* )GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );

	pWndTabCtrl->SetTabTitleAlign( CWndTabCtrl::ALIGN_CENTER );

	m_WndScenario.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 11 );
	m_WndScenario.SetMemberCheckingMode( TRUE );
	m_WndScenario.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	m_WndNormal.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 12 );
	m_WndNormal.SetMemberCheckingMode( TRUE );
	m_WndNormal.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	m_WndRequest.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 13 );
	m_WndRequest.SetMemberCheckingMode( TRUE );
	m_WndRequest.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	m_WndEvent.Create( WBS_CHILD | WBS_NODRAWFRAME | WBS_VSCROLL, GetClientRect(), pWndTabCtrl, 14 );
	m_WndEvent.SetMemberCheckingMode( TRUE );
	m_WndEvent.SetMaxCheckNumber( MAX_CHECK_NUMBER );

	QuestProp* pQuestProp = NULL;
	
	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_SCENARIO );
	pWndTabCtrl->InsertItem(&m_WndScenario, pQuestProp ? pQuestProp->m_szTitle : _T(""));

	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_NORMAL );
	pWndTabCtrl->InsertItem(&m_WndNormal, pQuestProp ? pQuestProp->m_szTitle : _T(""));

	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_REQUEST );
	pWndTabCtrl->InsertItem(&m_WndRequest, pQuestProp ? pQuestProp->m_szTitle : _T(""));

	pQuestProp = prj.m_aPropQuest.GetAt( QUEST_KIND_EVENT );
	pWndTabCtrl->InsertItem(&m_WndEvent, pQuestProp ? pQuestProp->m_szTitle : _T(""));

	pWndTabCtrl->SetCurSel( CTreeInformationManager::m_nSelectedTabNumber );

	CWndComboBox* pWndComboBox = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_QUEST_LIST );
	if( pWndComboBox )
	{
		pWndComboBox->AddWndStyle( EBS_READONLY );
		pWndComboBox->AddString( prj.GetText( TID_GAME_QUEST_ALL_LIST ) );
		pWndComboBox->AddString( prj.GetText( TID_GAME_QUEST_CURRENT_LIST ) );
		pWndComboBox->AddString( prj.GetText( TID_GAME_QUEST_COMPLETE_LIST ) );
		pWndComboBox->SetCurSel( CTreeInformationManager::m_eQuestListGroup );
	}

	Update();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndQuest::Initialize( CWndBase* pWndParent )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_QUEST_EX_LIST, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndQuest::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );

	CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
	if( pWndQuestDetail )
	{
		CRect rtCurrentWndQuestDetail = pWndQuestDetail->GetWndRect();
		CRect rtChangedWndQuestDetail;
		int nRevisedLeft = rectWnd.right - 2;
		pWndQuestDetail->Move( nRevisedLeft, rectWnd.top );
	}
}
BOOL CWndQuest::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message != WNM_SELCHANGE && message != WNM_DBLCLK && message != WNM_SELCANCEL && message != WNM_CLICKED )
		return FALSE;

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_QUEST_LIST_TABCTRL );
	assert( pWndTabCtrl );
	CTreeInformationManager::m_nSelectedTabNumber = pWndTabCtrl->GetCurSel();

	CWndQuestTreeCtrl* pTreeCtrl = ( CWndQuestTreeCtrl* )pWndTabCtrl->GetSelectedTab();
	assert( pTreeCtrl );
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	QuestId nQuestID = QuestIdNone;
	if( lpTreeElem )
	{
		m_idSelQuest = lpTreeElem->m_dwData;
		if( lpTreeElem->m_ptrArray.empty() )
			nQuestID = QuestId::From(lpTreeElem->m_dwData);
	}

	switch( message )
	{
	case WNM_SELCHANGE:
		{
			switch( nID )
			{
			case WIDC_COMBOBOX_QUEST_LIST:
			case WIDC_QUEST_LIST_TABCTRL:
				{
					if( g_WndMng.m_pWndQuestDetail )
						SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
						CWndComboBox* pWndComboBox = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_QUEST_LIST );
					if( pWndComboBox )
					{
						CTreeInformationManager::m_eQuestListGroup = static_cast< CTreeInformationManager::QuestListGroup >( pWndComboBox->GetCurSel() );
						Update();
					}
					break;
				}
			}
			break;
		}
	case WNM_DBLCLK:
		{
			if( lpTreeElem && lpTreeElem->m_ptrArray.empty() )
				ControlOpenTree( lpTreeElem );
			break;
		}
	case WNM_SELCANCEL:
	case WNM_CLICKED:
		{
			switch( nID )
			{
			case WIDC_REMOVE:
				{
					if( lpTreeElem && lpTreeElem->m_ptrArray.empty() )
					{
						if( g_WndMng.m_pWndQuestDetail )
							SAFE_DELETE( g_WndMng.m_pWndQuestDetail )

						g_WndMng.OpenCustomBox(new CWndRemoveQuest(nQuestID));
					}
					break;
				}
			case WIDC_CLOSE:
				{
					nID = WTBID_CLOSE;
					break;
				}
			case WIDC_BUTTON_CLOSE:
				{
					Destroy();
					break;
				}
			default:
				{
					if( lpTreeElem && lpTreeElem->m_ptrArray.empty() )
					{
						if( g_WndMng.m_pWndQuestDetail && nQuestID == g_WndMng.m_pWndQuestDetail->GetQuestID() )
							SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
						else
						{
							SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
								g_WndMng.m_pWndQuestDetail = new CWndQuestDetail( nQuestID );
							CWndQuestDetail* pWndQuestDetail = g_WndMng.m_pWndQuestDetail;
							pWndQuestDetail->Initialize();
							if( pWndQuestDetail )
								pWndQuestDetail->UpdateQuestText( TRUE );
							CWndQuestQuickInfo* pWndQuestQuickInfo = g_WndMng.m_pWndQuestQuickInfo;
							if( pWndQuestQuickInfo )
								pWndQuestQuickInfo->SetUpdateTextSwitch( TRUE );
						}
					}
					RemoveQuest();
				}
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
} 

CWndQuestTreeCtrl* CWndQuest::GetQuestTreeSelf( QuestId dwQuestID )
{
	DWORD dwRootHeadQuestID = GetRootHeadQuest(dwQuestID).get();
	switch( dwRootHeadQuestID )
	{
	case QUEST_KIND_SCENARIO: // 시나리오 퀘스트
		{
			return &m_WndScenario;
		}
	case QUEST_KIND_NORMAL: // 일반 퀘스트
		{
			return &m_WndNormal;
		}
	case QUEST_KIND_REQUEST: // 의뢰소 퀘스트
		{
			return &m_WndRequest;
		}
	case QUEST_KIND_EVENT: // 이벤트 퀘스트
		{
			return &m_WndEvent;
		}
	default:
		{
			return &m_WndNormal;
		}
	}
}

void CWndQuest::OpenTreeArray(TreeElems & rPtrArray, BOOL bOpen) {
	for (TREEELEM & lpTreeElem : rPtrArray) {
		if (lpTreeElem.m_ptrArray.empty()) continue;
			
		const QuestId myQuestId = QuestId::From(lpTreeElem.m_dwData);
		if (std::ranges::contains(m_aOpenTree, myQuestId)) {
			lpTreeElem.m_bOpen = bOpen;
		}

		OpenTreeArray(lpTreeElem.m_ptrArray, bOpen);
	}
}

void CWndQuest::InsertQuestItem( const QuestId dwQuestID, std::set<QuestId> & raOldHeadQuestID, const BOOL bCompleteQuest, const int nNewQuestId )
{
	static CWndQuestTreeCtrl* pQuestTreeCtrl = NULL;

	const QuestProp * pQuestProp = dwQuestID.GetProp();
	if (!pQuestProp) return;
		
	if (pQuestProp->m_nHeadQuest == QuestIdNone) {
		pQuestTreeCtrl = GetQuestTreeSelf(dwQuestID);
		raOldHeadQuestID.emplace(dwQuestID);
		return;
	}

	QuestId dwNowHeadQuest = pQuestProp->m_nHeadQuest;
		
	if (!raOldHeadQuestID.contains(dwNowHeadQuest)) {
		InsertQuestItem(dwNowHeadQuest, raOldHeadQuestID, bCompleteQuest);
	} else {
		pQuestTreeCtrl = GetQuestTreeSelf(dwNowHeadQuest);
	}

	assert( pQuestTreeCtrl );

	TREEELEM * lpTreeElem = nullptr;

	if (!sqktd::is_among(dwNowHeadQuest.get(),
		QUEST_KIND_SCENARIO, QUEST_KIND_NORMAL, QUEST_KIND_REQUEST, QUEST_KIND_EVENT
	)) {
		lpTreeElem = pQuestTreeCtrl->FindTreeElem(dwNowHeadQuest.get());
	}


	CString strFullQuestTitle = _T( "" );

	CString strQuestTitle = pQuestProp->m_szTitle;

	const auto questState = g_pPlayer->GetQuestState(dwQuestID);
	if(questState.has_value()) {
		CString strState = _T( "" );
		if( g_Option.m_bOperator || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
			strState.Format( "(%d, %ul)", static_cast<int>(questState.value()), dwQuestID.get());

		strFullQuestTitle = strQuestTitle + strState;
				
		if (bCompleteQuest) {
			TREEELEM * elem = pQuestTreeCtrl->InsertItem(lpTreeElem, strFullQuestTitle, dwQuestID.get(), TRUE);
			elem->m_dwColor = 0xffc0c0c0;
		} else {
			if (g_QuestTreeInfoManager.GetTreeInformation(dwQuestID.get()) == NULL)
				g_QuestTreeInfoManager.InsertTreeInformation(dwQuestID.get(), TRUE);
			pQuestTreeCtrl->InsertItem(lpTreeElem, strFullQuestTitle, dwQuestID.get(), FALSE, IsCheckedQuestID(dwQuestID));
		}
	}
	else
	{
		strFullQuestTitle = strQuestTitle;
		LPTREEELEM pFolderTreeElem = pQuestTreeCtrl->InsertItem( lpTreeElem, strFullQuestTitle, dwQuestID.get());
		if( nNewQuestId != -1 && prj.m_aPropQuest.GetAt( nNewQuestId )->m_nHeadQuest == dwQuestID )
		{
			pFolderTreeElem->m_bOpen = TRUE;
			m_aOpenTree.emplace_back(dwQuestID);
		}
	}
	raOldHeadQuestID.emplace( dwQuestID );
}

bool CWndQuest::IsCheckedQuestID(QuestId dwQuestID) const {
	if (!g_pPlayer->m_quests) return false;
	return std::ranges::contains(g_pPlayer->m_quests->checked, dwQuestID);
}


BOOL CWndQuestTreeCtrl::OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_CHECK:
		{
		for (const TREEITEM & pTreeItem : GetTreeItemArray()) {

				if( GetMemberCheckingMode() == TRUE )
				{
					TREEELEM * pTreeElem = pTreeItem.m_lpTreeElem;
					CWndButton* pWndCheckBox = pTreeElem->m_pWndCheckBox;
					QuestId dwQuestID = QuestId::From(pTreeElem->m_dwData);
					if( pWndCheckBox && ( CWndButton* )pLResult == pWndCheckBox )
					{
						if( pWndCheckBox->GetCheck() == TRUE )
						{
							if( static_cast<int>(g_pPlayer->m_quests->checked.size()) >= GetMaxCheckNumber())
							{
								pWndCheckBox->SetCheck( FALSE );
								g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_DONT_CHECK ), NULL, prj.GetTextColor( TID_GAME_ERROR_DONT_CHECK ) );
								break;
							}

							if( g_QuestTreeInfoManager.GetTreeInformation( dwQuestID.get() ) == NULL )
								g_QuestTreeInfoManager.InsertTreeInformation( dwQuestID.get(), TRUE);
							g_DPlay.SendCheckedQuestId( dwQuestID, TRUE );
						}
						else
						{
							g_QuestTreeInfoManager.DeleteTreeInformation( dwQuestID.get() );
							g_DPlay.SendCheckedQuestId( dwQuestID, FALSE );
						}
					}
				}
			}
			break;
		}
	}
	return TRUE;
}

void CWndQuestTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{

	for (const TREEITEM & pTreeItem : GetTreeItemArray()) {

		if( GetMemberCheckingMode() == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem.m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem.m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened( pParentTreeElem ) == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}
		CRect rect = pTreeItem.m_rect;
		SIZE size = m_pTexButtOpen->m_size;
		rect.left -= GetCategoryTextSpace();
		rect.SetRect( rect.left, rect.top, rect.left + size.cx, rect.top + size.cy );
		if( rect.PtInRect( point ) )
		{
			TREEELEM * lpTreeElem = pTreeItem.m_lpTreeElem;
			lpTreeElem->m_bOpen = !lpTreeElem->m_bOpen;
			SetFocusElem( lpTreeElem );

			CWndBase* pWndParent = m_pParentWnd;
			while( pWndParent->GetStyle() & WBS_CHILD )
				pWndParent = pWndParent->GetParentWnd();
			CWndQuest* pWndQuest = ( CWndQuest* )pWndParent;
			pWndQuest->ControlOpenTree( lpTreeElem );
		}
	}
}
//-----------------------------------------------------------------------------

void CWndQuestDetail::CWndQConditionTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{

	for (const TREEITEM & pTreeItem : GetTreeItemArray()) {

		if( GetMemberCheckingMode() == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem.m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem.m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && pParentTreeElem->m_bOpen == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}

		LPTREEELEM pTreeElem = pTreeItem.m_lpTreeElem;
		if( pTreeItem.m_rect.PtInRect( point ) )
		{
			DWORD dwGoalTextID = SetQuestDestinationInformation( ( ( CWndQuestDetail* )m_pParentWnd )->GetQuestID(), pTreeElem->m_dwData );
			ProcessQuestDestinationWorldMap( dwGoalTextID );
		}
	}
}
//-----------------------------------------------------------------------------
CWndQuestDetail::CWndQuestDetail( QuestId dwQuestID ) : 
m_dwQuestID( dwQuestID )
{
}

//-----------------------------------------------------------------------------
BOOL CWndQuestDetail::Initialize( CWndBase* pWndParent )
{
	return CWndNeuz::InitDialog( APP_QUEST_EX_DETAIL, pWndParent, 0, CPoint( 0, 0 ) );
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM_CONDITION );
	lpWndCtrl->dwWndStyle |= WBS_VSCROLL;

	m_WndQConditionTreeCtrl.Create( lpWndCtrl->dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
	m_WndQConditionTreeCtrl.SetCategoryTextSpace( 0 );

	CWndQuest* pWndQuest = ( CWndQuest* )g_WndMng.GetDlgItem( APP_QUEST_EX_LIST );
	if( pWndQuest )
	{
		CRect rtWndQuest = pWndQuest->GetWndRect();
		int nRevisedLeft = rtWndQuest.right - 2;
		Move( nRevisedLeft, rtWndQuest.top );
	}
	else
		MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndQuestDetail::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndQuest* pWndQuest = ( CWndQuest* )g_WndMng.GetDlgItem( APP_QUEST_EX_LIST );
	if( pWndQuest )
		DelWndStyle( WBS_MOVE );
	else
		AddWndStyle( WBS_MOVE );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	if( m_pTexture )
	{
		RenderWnd();
		if( IsWndStyle( WBS_CAPTION ) )
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
			p2DRender->TextOut( 10, 4, m_strTitle, m_dwColor );
			p2DRender->SetFont( pOldFont );
		}
	}
	else if( m_strTexture.IsEmpty() )
	{
		m_Theme.RenderWndBaseFrame( p2DRender, &rect );
		if( IsWndStyle( WBS_CAPTION ) )
		{
			rect.bottom = 21;
			m_Theme.RenderWndBaseTitleBar( p2DRender, &rect, m_strTitle, m_dwColor );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::UpdateQuestText( BOOL bClick )
{
	if( m_dwQuestID == QuestIdNone)
		return;

	CWndText* pTextDesc = ( CWndText* )GetDlgItem( WIDC_TEXT_CONTENTS );
	CWndQConditionTreeCtrl* pWndQuestConditionTree = ( CWndQConditionTreeCtrl* )GetDlgItem( WIDC_CUSTOM_CONDITION );
	pWndQuestConditionTree->DeleteAllItems();
	CWndText* pTextReward = (CWndText*)GetDlgItem( WIDC_TEXT_REWARD );

	LPQUEST lpQuest = g_pPlayer->FindQuest( m_dwQuestID );
	BOOL bComplete = g_pPlayer->IsCompleteQuest( m_dwQuestID );

	if( lpQuest || bComplete )
		UpdateQuestDetailText( m_dwQuestID, lpQuest, bComplete, bClick );
	else
	{
		pTextDesc->SetString( _T( "" ) );
		pWndQuestConditionTree->DeleteAllItems();
		pTextReward->SetString( _T( "" ) );
	}
	pTextDesc->ResetString();
	pTextReward->ResetString();
}
//-----------------------------------------------------------------------------
void CWndQuestDetail::UpdateQuestDetailText( QuestId dwQuestID, LPQUEST lpQuest, BOOL bComplete, BOOL bClick )
{
	CWndText* pTextDesc = ( CWndText* )GetDlgItem( WIDC_TEXT_CONTENTS );
	CWndQConditionTreeCtrl* pWndQuestConditionTree = ( CWndQConditionTreeCtrl* )GetDlgItem( WIDC_CUSTOM_CONDITION );
	pWndQuestConditionTree->DeleteAllItems();
	CWndText* pTextReward = ( CWndText* )GetDlgItem( WIDC_TEXT_REWARD );

	const QuestProp * pQuestProp = dwQuestID.GetProp();

	// 디스크립션만 출력
	if( bComplete )
	{
		for( int i = QS_END; i >= 0; --i )
		{
			QuestState* pQuestState = pQuestProp->m_questState[ i ];
			if( pQuestState )
			{
				pTextDesc->SetString( pQuestState->m_szDesc, 0xff000000 );
				break;
			}
		}
	}
	else
	{
		QuestState* pQuestState = pQuestProp->m_questState[ lpQuest->m_nState ];
		pTextDesc->SetString( pQuestState->m_szDesc, 0xff000000 );
	}

	//////////////////////////////////////////////////////////////////////////////////
	// 퀘스트 종료 조건
	//////////////////////////////////////////////////////////////////////////////////
	CString strTemp = _T( "" );
#ifdef __BS_PUTNAME_QUESTARROW
	CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
	if( pWndWorld )
		pWndWorld->m_strDestName = _T( "" );
#endif // __BS_PUTNAME_QUESTARROW
	if( bComplete )
		pWndQuestConditionTree->InsertItem( NULL, GETTEXT( TID_QUEST_COMPLETED ), 0 );
	else
		MakeQuestConditionItems( dwQuestID, pWndQuestConditionTree, bClick );

	//////////////////////////////////////////////////////////////////////////////////
	// 보상 아이템 목록
	//////////////////////////////////////////////////////////////////////////////////
	CString strReward = _T( "" );
	for( int i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
	{
		QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ];
		if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == g_pPlayer->GetSex() )
		{
			if( pEndRewardItem->m_nType == 0 )
			{
				if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == g_pPlayer->GetJob() )
				{
					if( pEndRewardItem->m_nItemIdx )
					{
						ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
						if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
							strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
						else
							strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
						strReward += strTemp + "\n";
					}
				}
			}
			else if( pEndRewardItem->m_nType == 1 )
			{
				if( pEndRewardItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndRewardItem->m_nJobOrItem ) ) 
				{
					if( pEndRewardItem->m_nItemIdx )
					{
						ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
						if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
							strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
						else
							strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
						strReward += strTemp + "\n";
					}
				}
			}
		}
	}
	if( pQuestProp->m_nEndRewardGoldMin )
	{
		if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
		{
			if( ( pQuestProp->m_nEndRewardGoldMax - pQuestProp->m_nEndRewardGoldMin ) == 0 )
				strTemp.Format( GETTEXT( TID_QUEST_GOLD1 ), pQuestProp->m_nEndRewardGoldMin );
			else
				strTemp.Format( GETTEXT( TID_QUEST_GOLD2 ), pQuestProp->m_nEndRewardGoldMin, pQuestProp->m_nEndRewardGoldMax );
		}
		else
			strTemp.Format( GETTEXT( TID_QUEST_GOLD3 ) );
		strReward += strTemp + "\n";
	}
	if( pQuestProp->m_nEndRewardExpMin )
	{
		strTemp.Format( GETTEXT( TID_QUEST_EXP1 ) );
		strReward += strTemp + "\n";
	}
	if( pQuestProp->m_nEndRewardSkillPoint )
	{
		strTemp.Format( GETTEXT( TID_QUEST_SKILLPOINT ) );
		strReward += strTemp + "\n";
	}
	if( pQuestProp->m_nEndRewardPKValueMin )
	{
		strTemp.Format( GETTEXT( TID_QUEST_PK_REWARD_PKVALUE ) );
		strReward += strTemp + "\n";
	}
	pTextReward->SetString( strReward );
}
//-----------------------------------------------------------------------------
void MakeQuestConditionItems( const QuestId dwQuestID, CWndTreeCtrl* pWndTreeCtrl, BOOL bClick, LPTREEELEM lpTreeElem, DWORD dwStartColor, DWORD dwEndColor, DWORD dwSelectColor )
{
	CString strTemp = _T( "" );
	LPQUEST lpQuest = g_pPlayer->FindQuest( dwQuestID );
	const QuestProp * pQuestProp = dwQuestID.GetProp();

	int i = NULL;
	for( ; i < 14; i++ )
	{
		if( pQuestProp->m_questState[ i ] == NULL )
			break;
	}
	if( lpQuest->m_nState == ( i - 1 ) || lpQuest->m_nState == 14 )
	{
		int nNum;
		if( pQuestProp->m_nEndCondLimitTime )
		{
			if( lpQuest->m_wTime )
			{
				strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), lpQuest->m_wTime & 0x7fff );
				pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, 0, TRUE, FALSE, dwEndColor, dwSelectColor );
			}
			else
			{
				strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIMEOUT ) );
				pWndTreeCtrl->InsertItem( lpTreeElem, "#cffff0000" + strTemp + "#ns#nc", 0, TRUE, FALSE, dwStartColor, dwSelectColor );
			}
		}
		for( int i = 0 ; i < 2; i++ )
		{
			if( pQuestProp->m_nEndCondKillNPCIdx[ i ] )
			{
				MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondKillNPCIdx[ i ] );
				nNum = lpQuest->m_nKillNPCNum[ i ];
				if( nNum > pQuestProp->m_nEndCondKillNPCNum[ i ] )
					nNum = pQuestProp->m_nEndCondKillNPCNum[ i ];
				strTemp.Format( GETTEXT( TID_QUEST_KILL_NPC ), pMoverProp->szName, lpQuest->m_nKillNPCNum[ i ], pQuestProp->m_nEndCondKillNPCNum[ i ] );

				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, lpQuest->m_nKillNPCNum[ i ], pQuestProp->m_nEndCondKillNPCNum[ i ] );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, nNum == pQuestProp->m_nEndCondKillNPCNum[ i ] ), pQuestProp->m_KillNPCGoalData[ i ].m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		for( int i = 0; i < pQuestProp->m_nEndCondItemNum; i++ )
		{
			QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[ i ];

			if( pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == g_pPlayer->GetSex() )
			{
				if( pEndCondItem->m_nType == 0 )
				{
					if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{
						if( pEndCondItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

							DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, nNum, pEndCondItem->m_nItemNum );
							pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, nNum >= pEndCondItem->m_nItemNum ), pEndCondItem->m_ItemGoalData.m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
						}
					}
				}
				else if( pEndCondItem->m_nType == 1 )
				{
					if( pEndCondItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondItem->m_nJobOrItem ) )
					{
						if( pEndCondItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

							DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, nNum, pEndCondItem->m_nItemNum );
							pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, nNum >= pEndCondItem->m_nItemNum ), pEndCondItem->m_ItemGoalData.m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
						}
					}
				}
			}
		}
		if( pQuestProp->m_dwEndCondPatrolWorld )
		{
			strTemp.Format( GETTEXT( TID_QUEST_PATROL ), prj.GetPatrolDestination( pQuestProp->m_dwPatrolDestinationID ) );
			DWORD dwCompleteColor = 0;
			if( lpQuest->m_bPatrol )
			{
				strTemp += _T( " " );
				strTemp += prj.GetText( TID_GAME_QUEST_COMPLETE_TEXT );
				dwCompleteColor = dwEndColor;
			}
			else
				dwCompleteColor = dwStartColor;
			pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, lpQuest->m_bPatrol ), pQuestProp->m_PatrolWorldGoalData.m_dwGoalIndex, TRUE, FALSE, dwCompleteColor, dwSelectColor );
		}
		if( pQuestProp->m_nEndCondDisguiseMoverIndex )
		{
			if( pQuestProp->m_nEndCondDisguiseMoverIndex == -1 )
			{
				strTemp.Format( GETTEXT( TID_QUEST_NODISQUISE ) );
				DWORD dwCompleteColor = 0;
				if( g_pPlayer->IsDisguise() == FALSE )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->IsDisguise() == FALSE ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else
			{
				MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondDisguiseMoverIndex );
				strTemp.Format( GETTEXT( TID_QUEST_DISQUISE ), pMoverProp->szName );
				DWORD dwCompleteColor = 0;
				if( g_pPlayer->IsDisguise() )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, g_pPlayer->IsDisguise() ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		if( pQuestProp->m_nEndCondSkillIdx )
		{
			ItemProp* pSkillProp = prj.GetSkillProp( pQuestProp->m_nEndCondSkillIdx );
			LPSKILL lpSkill = g_pPlayer->GetSkill( pQuestProp->m_nEndCondSkillIdx );
			if( g_pPlayer->CheckSkill( pQuestProp->m_nEndCondSkillIdx ) && lpSkill )
			{
				strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, lpSkill->dwLevel, pQuestProp->m_nEndCondSkillLvl );
				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, lpSkill->dwLevel, pQuestProp->m_nEndCondSkillLvl );
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, (int)( lpSkill->dwLevel ) >= pQuestProp->m_nEndCondSkillLvl ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else
			{
				strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, 0, pQuestProp->m_nEndCondSkillLvl );
				DWORD dwCompleteColor = MakeTextColor( dwStartColor, dwEndColor, 0, pQuestProp->m_nEndCondSkillLvl );
				pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		// 파티 체크
		if( pQuestProp->m_nEndCondParty != 0 )
		{
			BOOL bLeader = FALSE;
			BOOL bParty  = g_Party.IsMember( g_pPlayer->m_idPlayer );
			int nSize = g_Party.GetSizeofMember();

			if( pQuestProp->m_nEndCondParty == 1 ) // 싱글이어야 함
			{
				strTemp.Format( GETTEXT( TID_QUEST_PARTY_SOLO ) );
				DWORD dwCompleteColor = 0;
				if( !bParty )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, !bParty ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else if( pQuestProp->m_nEndCondParty == 2 ) // 파티여야 함
			{
				int nTextId = 0;
				BOOL bResult = FALSE;

				if( pQuestProp->m_nEndCondPartyNum == 0 )
				{
					if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY;
					else if( pQuestProp->m_nEndCondPartyLeader == 1 ) // 파티, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_MEMBER;
					else if( pQuestProp->m_nEndCondPartyLeader == 2 ) // 파티, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_LEADER;

					strTemp.Format( GETTEXT( nTextId ) );

					if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 
						bResult = TRUE;
					else if( ( pQuestProp->m_nEndCondPartyLeader - 1 ) == bLeader ) // 파티, 맴버 여부
						bResult = TRUE;
				}
				else
				{
					if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_NUM;
					else if( pQuestProp->m_nEndCondPartyLeader == 1 ) // 파티, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_MEMBER_NUM;
					else if( pQuestProp->m_nEndCondPartyLeader == 2 ) // 파티, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_PARTY_LEADER_NUM;

					if( pQuestProp->m_nEndCondPartyNumComp == 0 ) 
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondPartyNum );
						if( nSize == pQuestProp->m_nEndCondPartyNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondPartyNumComp == -1 )
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondPartyNum );
						if( nSize <= pQuestProp->m_nEndCondPartyNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondPartyNumComp == 1)
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondPartyNum );
						if( nSize >= pQuestProp->m_nEndCondPartyNum )
							bResult = TRUE;
					}
				}
				DWORD dwCompleteColor = 0;
				if( bResult && bParty )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, bResult && bParty ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		// 길드 체크
		if( pQuestProp->m_nEndCondGuild != 0 ) 
		{
			BOOL bLeader = FALSE;
			BOOL bGuild  = g_Party.IsMember( g_pPlayer->m_idPlayer );
			int nSize = g_Party.GetSizeofMember();

			if( pQuestProp->m_nEndCondGuild == 1 ) // 길드가 아니어야 됨
			{
				strTemp.Format( GETTEXT( TID_QUEST_GUILD_SOLO ) );
				DWORD dwCompleteColor = 0;
				if( !bGuild )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, !bGuild ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
			else if( pQuestProp->m_nEndCondGuild == 2 ) // 길드여야 됨
			{
				int nTextId = 0;
				BOOL bResult = FALSE;

				if( pQuestProp->m_nEndCondGuildNum == 0 ) 
				{
					if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 파티 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD;
					else if( pQuestProp->m_nEndCondGuildLeader == 1 ) // 파티, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_MEMBER;
					else if( pQuestProp->m_nEndCondGuildLeader == 2 ) // 파티, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_LEADER;

					strTemp.Format( GETTEXT( nTextId ) );

					if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부
						bResult = TRUE;
					else if( ( pQuestProp->m_nEndCondGuildLeader - 1 ) == bLeader ) // 길드, 리더 여부
						bResult = TRUE;
				}
				else
				{
					if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_NUM;
					else if( pQuestProp->m_nEndCondGuildLeader == 1 ) // 길드, 맴버 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_MEMBER_NUM;
					else if( pQuestProp->m_nEndCondGuildLeader == 2 ) // 길드, 리더 여부 (0 == 0 )
						nTextId = TID_QUEST_GUILD_LEADER_NUM;

					if( pQuestProp->m_nEndCondGuildNumComp == 0 )
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondGuildNum );
						if( nSize == pQuestProp->m_nEndCondGuildNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondGuildNumComp == -1 )
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondGuildNum );
						if( nSize <= pQuestProp->m_nEndCondGuildNum )
							bResult = TRUE;
					}
					else if( pQuestProp->m_nEndCondGuildNumComp == 1)
					{
						strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondGuildNum );
						if( nSize >= pQuestProp->m_nEndCondGuildNum )
							bResult = TRUE;
					}
				}
				DWORD dwCompleteColor = 0;
				if( bResult && bGuild )
					dwCompleteColor = dwEndColor;
				else
					dwCompleteColor = dwStartColor;
				pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, bResult && bGuild ), 0, TRUE, FALSE, dwCompleteColor, dwSelectColor );
			}
		}
		//////////////////////////////////////////////////////
		CString strEndCondOneItem;
		CString strEndCondOneItemComplete;
		for( int i = 0; i < pQuestProp->m_nEndCondOneItemNum; i++ )
		{
			QuestPropItem* pEndCondOneItem = &pQuestProp->m_paEndCondOneItem[ i ];

			if( pEndCondOneItem->m_nSex == -1 || pEndCondOneItem->m_nSex == g_pPlayer->GetSex() )
			{
				if( pEndCondOneItem->m_nType == 0 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || pEndCondOneItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							if( nNum >= pEndCondOneItem->m_nItemNum )
							{
								strEndCondOneItemComplete = "#s#cffa0a0a0";
								break;
							}
						}
					}
				}
				else if( pEndCondOneItem->m_nType == 1 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondOneItem->m_nJobOrItem ) )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							if( nNum >= pEndCondOneItem->m_nItemNum )
							{
								strEndCondOneItemComplete = "#s#cffa0a0a0";
								break;
							}
						}
					}
				}
			}
		}
		for( int i = 0; i < pQuestProp->m_nEndCondOneItemNum; i++ )
		{
			QuestPropItem* pEndCondOneItem = &pQuestProp->m_paEndCondOneItem[ i ];

			if( pEndCondOneItem->m_nSex == -1 || pEndCondOneItem->m_nSex == g_pPlayer->GetSex() )
			{
				if( pEndCondOneItem->m_nType == 0 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || pEndCondOneItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondOneItem->m_nItemNum );

							strEndCondOneItem += strEndCondOneItemComplete + "-" + strTemp + "#ns#nc";
						}
					}
				}
				else if( pEndCondOneItem->m_nType == 1 )
				{
					if( pEndCondOneItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondOneItem->m_nJobOrItem ) )
					{
						if( pEndCondOneItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndCondOneItem->m_nItemIdx );
							nNum = g_pPlayer->GetItemNum( pEndCondOneItem->m_nItemIdx );
							strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondOneItem->m_nItemNum );

							strEndCondOneItem += strEndCondOneItemComplete + "-" + strTemp + "#ns#nc";
						}
					}
				}
			}
		}
		if( 0 < strEndCondOneItem.GetLength() )
		{
			strTemp.Format( GETTEXT( TID_QUEST_COND_ITEM ) );
			pWndTreeCtrl->InsertItem( lpTreeElem, strEndCondOneItemComplete + strTemp + "#ns#nc" + strEndCondOneItem, 0, TRUE, FALSE, dwEndColor, dwSelectColor );
		}
		if( pQuestProp->m_szEndCondDlgCharKey[ 0 ] )
		{
			LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondDlgCharKey );
			strTemp.Format( GETTEXT( TID_QUEST_DIALOG ), lpCharacter->m_strName );
			pWndTreeCtrl->InsertItem( lpTreeElem, MakeString( strTemp, lpQuest->m_bDialog ), pQuestProp->m_DialogCharacterGoalData.m_dwGoalIndex, TRUE, FALSE, dwEndColor, dwSelectColor );
		}
		if( pQuestProp->m_szEndCondCharacter[ 0 ] )
		{
			LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondCharacter );
			strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
			pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, pQuestProp->m_MeetCharacterGoalData.m_dwGoalIndex, TRUE, FALSE, dwEndColor, dwSelectColor );
			if( bClick ) // 클릭해서 업데이트되는 경우에만 좌표를 요청한다
			{
				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if( pWndWorld )
				{
					CWorld* pWorld = g_WorldMng();
					if( pWorld )
						pWorld->SetObjFocus(NULL);

					pWndWorld->m_bSetQuestNPCDest = FALSE;
					pWndWorld->m_pNextTargetObj = NULL;
#ifdef __BS_PUTNAME_QUESTARROW
					pWndWorld->m_strDestName = lpCharacter->m_strName;
#endif // __BS_PUTNAME_QUESTARROW
				}
			}
		}
		else if( pQuestProp->m_lpszEndCondMultiCharacter )
		{
			for( int i = 0; i < 10; i++ )
			{
				CHAR* lpszChar = &pQuestProp->m_lpszEndCondMultiCharacter[ i * 64 ];
				if( lpszChar[ 0 ] )
				{
					LPCHARACTER lpCharacter = prj.GetCharacter( lpszChar );
					if( lpCharacter )
					{
						for (const auto & [_, itemId] : lpCharacter->m_dstQuests) {
							if( g_pPlayer->GetItemNum( itemId ) )
							{
								strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
								i = 10;
								pWndTreeCtrl->InsertItem( lpTreeElem, strTemp, 0, TRUE, FALSE, dwEndColor, dwSelectColor );
								break;
							}
						}
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
const CString MakeString( const CString& string, BOOL bCond )
{
	return CString( ( bCond ) ? "#cffa0a0a0" + string + "#nc" : string );
}
//-----------------------------------------------------------------------------
DWORD MakeTextColor( DWORD dwStartColor, DWORD dwEndColor, int nCurrentNumber, int nCompleteNumber )
{
	if( nCompleteNumber == 0 )
		return 0;

	int nStartColorRed = ( dwStartColor & 0x00ff0000 ) >> 16;
	int nStartColorGreen = ( dwStartColor & 0x0000ff00 ) >> 8;
	int nStartColorBlue = ( dwStartColor & 0x000000ff );
	int nEndColorRed = ( dwEndColor & 0x00ff0000 ) >> 16;
	int nEndColorGreen = ( dwEndColor & 0x0000ff00 ) >> 8;
	int nEndColorBlue = ( dwEndColor & 0x000000ff );

	int nDifferenceRed = abs( nEndColorRed - nStartColorRed );
	int nDifferenceGreen = abs( nEndColorGreen - nStartColorGreen );
	int nDifferenceBlue = abs( nEndColorBlue - nStartColorBlue );

	FLOAT fItemGettingRate = static_cast<FLOAT>( nCurrentNumber ) / static_cast<FLOAT>( nCompleteNumber );
	DWORD dwCompleteRedRate = static_cast<DWORD>( fItemGettingRate * static_cast<FLOAT>( nDifferenceRed ) );
	if( (int)( dwCompleteRedRate ) > nDifferenceRed )
		dwCompleteRedRate = nDifferenceRed;
	if( dwCompleteRedRate < 0 )
		dwCompleteRedRate = 0;
	DWORD dwCompleteGreenRate = static_cast<DWORD>( fItemGettingRate * static_cast<FLOAT>( nDifferenceGreen ) );
	if( (int)( dwCompleteGreenRate ) > nDifferenceGreen )
		dwCompleteGreenRate = nDifferenceGreen;
	if( dwCompleteGreenRate < 0 )
		dwCompleteGreenRate = 0;
	DWORD dwCompleteBlueRate = static_cast<DWORD>( fItemGettingRate * static_cast<FLOAT>( nDifferenceBlue ) );
	if( (int)( dwCompleteBlueRate ) > nDifferenceBlue )
		dwCompleteBlueRate = nDifferenceBlue;
	if( dwCompleteBlueRate < 0 )
		dwCompleteBlueRate = 0;

	DWORD dwCompleteRed = 0;
	if( nEndColorRed - nStartColorRed > 0 )
		dwCompleteRed = nStartColorRed + dwCompleteRedRate;
	else if( nEndColorRed - nStartColorRed < 0 )
		dwCompleteRed = nStartColorRed - dwCompleteRedRate;
	else
		dwCompleteRed = nStartColorRed;

	DWORD dwCompleteGreen = 0;
	if( nEndColorGreen - nStartColorGreen > 0 )
		dwCompleteGreen = nStartColorGreen + dwCompleteGreenRate;
	else if( nEndColorGreen - nStartColorGreen < 0 )
		dwCompleteGreen = nStartColorGreen - dwCompleteGreenRate;
	else
		dwCompleteGreen = nStartColorGreen;
	
	DWORD dwCompleteBlue = 0;
	if( nEndColorBlue - nStartColorBlue > 0 )
		dwCompleteBlue = nStartColorBlue + dwCompleteBlueRate;
	else if( nEndColorBlue - nStartColorBlue < 0 )
		dwCompleteBlue = nStartColorBlue - dwCompleteBlueRate;
	else
		dwCompleteBlue = nStartColorBlue;
	
	return D3DCOLOR_ARGB( 255, dwCompleteRed, dwCompleteGreen, dwCompleteBlue );
}
//-----------------------------------------------------------------------------
QuestId GetRootHeadQuest(const QuestId dwHeadQuest) {
	const QuestProp * const pHeadQuestProp = dwHeadQuest.GetProp();
	if (!pHeadQuestProp || pHeadQuestProp->m_nHeadQuest == QuestIdNone) {
		return dwHeadQuest;
	} else {
		return GetRootHeadQuest(pHeadQuestProp->m_nHeadQuest);
	}
}
//-----------------------------------------------------------------------------
DWORD SetQuestDestinationInformation( QuestId dwQuestID, DWORD dwGoalIndex )
{
	D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
	rDestinationArrow = D3DXVECTOR3( -1.0F, 0.0F, -1.0F );

	if( dwGoalIndex == 0 )
		return 0;

	const QuestProp * pQuestProp = dwQuestID.GetProp();
	if( pQuestProp == NULL )
		return 0;

	if( g_WndMng.m_pWndWorld == NULL )
		return 0;

	static const FLOAT CHARACTER_HEIGHT = 0.0F;
	int i = 0;
	
	int nEndCondItemNum = pQuestProp->m_nEndCondItemNum;
	for( i = 0; i < nEndCondItemNum; ++i )
	{
		QuestGoalData& roQuestGoalData = pQuestProp->m_paEndCondItem[ i ].m_ItemGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			continue;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	for( i = 0 ; i < 2; ++i )
	{
		const QuestGoalData& roQuestGoalData = pQuestProp->m_KillNPCGoalData[ i ];
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			continue;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	{
		const QuestGoalData& roQuestGoalData = pQuestProp->m_MeetCharacterGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			return 0;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	{
		const QuestGoalData& roQuestGoalData = pQuestProp->m_PatrolWorldGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			return 0;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}

	{
		const QuestGoalData& roQuestGoalData = pQuestProp->m_DialogCharacterGoalData;
		if( roQuestGoalData.m_dwGoalIndex == 0 )
			return 0;
		else if( roQuestGoalData.m_dwGoalIndex == dwGoalIndex )
		{
			rDestinationArrow = D3DXVECTOR3( roQuestGoalData.m_fGoalPositionX, CHARACTER_HEIGHT, roQuestGoalData.m_fGoalPositionZ );
			return roQuestGoalData.m_dwGoalTextID;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
void ProcessQuestDestinationWorldMap( DWORD dwGoalTextID )
{
	const D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
	if( rDestinationArrow != D3DXVECTOR3( -1.0F, 0.0F, -1.0F ) )
	{
#ifdef __IMPROVE_MAP_SYSTEM
		CWndMapEx* pWndMapEx = ( CWndMapEx* )g_WndMng.CreateApplet( APP_MAP_EX );
		if( pWndMapEx == NULL )
		{
			return;
		}

		pWndMapEx->SetConstructionMode( CWndMapEx::DESTINATION );
#else // __IMPROVE_MAP_SYSTEM
		CWorldMap::GetInstance()->SetDestinationMode( TRUE );
		CWorldMap::GetInstance()->LoadWorldMap();
		CWorldMap::GetInstance()->UpdateDestinationPosition( rDestinationArrow );
#endif // __IMPROVE_MAP_SYSTEM
	}
	else
	{
		if( dwGoalTextID == 0 )
			return;

		g_WndMng.OpenMessageBoxWithTitle( prj.GetQuestDestination( dwGoalTextID ), prj.GetText( TID_GAME_DESTINATION_BOX_TITLE ) );
	}
}
//-----------------------------------------------------------------------------
