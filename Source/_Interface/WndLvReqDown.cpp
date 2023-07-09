#include "stdafx.h"
#include "resData.h"
#include "WndLvReqDown.h"
#include "DPClient.h"


// APP_LVREQDOWN : WIDC_TEXT1, WIDC_BUTTON1, WIDC_STATIC1

void CWndLvReqDown::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();

	m_receiver.Create(0, GetWndCtrl(WIDC_STATIC1)->rect, this, WIDC_Receiver);

	CWndButton* pButton = GetDlgItem<CWndButton>(WIDC_BUTTON1);
	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);
	pButton->EnableWindow(FALSE);
	
	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("ReqLvDown.inc"));

	MoveParentCenter();
} 

BOOL CWndLvReqDown::Initialize(CWndBase * pWndParent) {
	return CWndNeuz::InitDialog(APP_LVREQDOWN, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndLvReqDown::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if (nID == WIDC_BUTTON1) {
		if(CItemElem * pItemElem = m_receiver.GetItem()) {
			g_DPlay.SendRemoveItemLevelDown(pItemElem->m_dwObjId);
			Destroy();
		}
	} else if (nID == WIDC_Receiver && message == WNM_ItemReceiver_Changed) {
		CWndButton * pButton = GetDlgItem<CWndButton>(WIDC_BUTTON1);
		pButton->EnableWindow(m_receiver.GetItem() ? TRUE : FALSE);
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

bool CWndLvReqDown::LeveledDownItemReceiver::CanReceiveItem(const CItemElem & itemElem, bool) {
	return itemElem.GetLevelDown() < 0;
}

BOOL CWndLvReqDown::OnDropIcon(LPSHORTCUT pShortcut, CPoint point) {
	return m_receiver.OnDropIcon(pShortcut, CPoint(16, 16));
}
