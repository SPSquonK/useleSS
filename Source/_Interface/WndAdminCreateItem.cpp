#include "stdafx.h"
#include "AppDefine.h"
#include "WndAdminCreateItem.h"
#include "FuncTextCmd.h"


CWndAdminCreateItem::Item::Item(const ItemProp * itemProp) {
	this->name.Format("%s (Lv%d)", itemProp->szName, itemProp->dwLimitLevel1);
	this->itemProp = itemProp;
}

void CWndAdminCreateItem::Displayer::Render(
	C2DRender * const p2DRender, const CRect rect,
	const Item & item, const DWORD color, const WndTListBox::DisplayArgs &
) const {
	p2DRender->TextOut(rect.left, rect.top, item.name.GetString(), color);
}

void CWndAdminCreateItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	ReplaceListBox<Item, Displayer>(WIDC_CONTROL1);

	UpdateItems(NULL_ID, SEX_SEXLESS, -1, 0);

	CWndComboBox* pWndItemKind = (CWndComboBox*)GetDlgItem( WIDC_ITEM_KIND );
	CWndComboBox* pWndItemSex = (CWndComboBox*)GetDlgItem( WIDC_ITEM_SEX );
	CWndComboBox* pWndItemJob = (CWndComboBox*)GetDlgItem( WIDC_ITEM_JOB );
	CWndEdit* pWndLevel = (CWndEdit*)GetDlgItem( WIDC_LEVEL );
	CStringArray strArray;
	CScript::GetFindIdToArray( "IK2_", &strArray );
	CWndListBox::LISTITEM & allKinds = pWndItemKind->AddString( "ALL" );
	allKinds.m_dwData = NULL_ID;
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		CWndListBox::LISTITEM & kind = pWndItemKind->AddString( strArray.GetAt( i ) );
		DWORD dwNum = CScript::GetDefineNum( strArray.GetAt( i ) );
		kind.m_dwData = dwNum;
	}
	pWndItemKind->m_wndListBox.SortListBox();
	int nIndex = pWndItemKind->m_wndListBox.FindString( 0, "ALL" );
	pWndItemKind->SetCurSel( nIndex );
	
	strArray.RemoveAll();
	CScript::GetFindIdToArray( "SEX_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		CWndListBox::LISTITEM & sex = pWndItemSex->AddString( strArray.GetAt( i ) );
		DWORD dwNum = CScript::GetDefineNum( strArray.GetAt( i ) );
		sex.m_dwData = dwNum;
	}
	pWndItemSex->SetCurSel( 2 );

	strArray.RemoveAll();
	CScript::GetFindIdToArray( "JOB_", &strArray );
	CWndListBox::LISTITEM & allJobs = pWndItemKind->AddString("ALL");
	allJobs.m_dwData = -1;
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		auto & itemJob = pWndItemJob->AddString( strArray.GetAt( i ) );
		DWORD dwNum = CScript::GetDefineNum( strArray.GetAt( i ) );
		itemJob.m_dwData = dwNum;
	}
	pWndItemJob->m_wndListBox.SortListBox();
	nIndex = pWndItemJob->m_wndListBox.FindString( 0, "ALL" );
	pWndItemJob->SetCurSel( nIndex );
	// 좌표 이동 

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAdminCreateItem::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_ADMIN_CREATEITEM, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndAdminCreateItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ITEM_KIND || nID == WIDC_ITEM_SEX || nID == WIDC_ITEM_JOB || nID == WIDC_LEVEL )
	{
		if( message == WNM_SELCHANGE || message == EN_CHANGE )
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_LEVEL );
			CString string = pWndEdit->GetString();
			const DWORD dwLevel = atoi( string );

			CWndComboBox* pWndItemKind = (CWndComboBox*)GetDlgItem( WIDC_ITEM_KIND );
			const DWORD dwKind = pWndItemKind->GetItemData(pWndItemKind->GetCurSel());

			CWndComboBox* pWndItemSex = (CWndComboBox*)GetDlgItem( WIDC_ITEM_SEX );
			const DWORD dwSex = pWndItemSex->GetItemData(pWndItemSex->GetCurSel());

			CWndComboBox* pWndItemJob = (CWndComboBox*)GetDlgItem( WIDC_ITEM_JOB );
			const DWORD dwJob = pWndItemJob->GetItemData(pWndItemJob->GetCurSel());

			UpdateItems(dwKind, dwSex, dwJob, dwLevel);
		}
	}
	else
	if( nID == WIDC_OK || ( nID == WIDC_CONTROL1 && message == WNM_DBLCLK ) )
	{
		CString string;
		
		CWndText* pWndItemNum = GetDlgItem<CWndText>( WIDC_ITEM_NUM );
		DWORD dwNum;
		if (pWndItemNum->m_string.GetLength()) {
			dwNum = _ttoi(pWndItemNum->m_string);
		} else {
			dwNum = 1;
		}
		
		dwNum = ( dwNum == 0? 1:dwNum );
		
		CWndText * pWndItemName = GetDlgItem<CWndText>(WIDC_ITEM_NAME);
		if (pWndItemName->m_string.GetLength()) {
			string.Format( "/ci %s %d", pWndItemName->m_string.GetString(), dwNum);
			g_textCmdFuncs.ParseCommand(string.GetString(), g_pPlayer);
		}
		else 
		{
			const ItemPropListBox * pListBox = GetDlgItem<ItemPropListBox>(WIDC_CONTROL1);
			const Item * item = pListBox->GetCurSelItem();

			if (item && item->itemProp) {
				string.Format( "/ci \"%s\" %lu", item->itemProp->szName, dwNum);
				g_textCmdFuncs.ParseCommand(string.GetString(), g_pPlayer);
			}
		}
	} else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE) {
		nID = WTBID_CLOSE;
		Destroy(TRUE);
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


void CWndAdminCreateItem::UpdateItems(
	DWORD kind, DWORD sex, DWORD job, DWORD level
) {
	ItemPropListBox * pListBox = GetDlgItem<ItemPropListBox>(WIDC_CONTROL1);

	pListBox->ResetContent();
	for (const ItemProp & pItemProp : prj.m_aPropItem) {

		if ((pItemProp.dwItemKind2 == kind || kind == NULL_ID) &&
			(pItemProp.dwItemSex == sex || sex == SEX_SEXLESS)) {
			if (pItemProp.dwLimitLevel1 >= level && (pItemProp.dwItemJob == job || job == -1)) {
				if (GetLanguage() != LANG_KOR && pItemProp.nVer >= 7 && pItemProp.bCharged == TRUE)
					continue;

				pListBox->Add(Item(&pItemProp));
			}
		}
	}
}