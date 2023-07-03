#include "stdafx.h"
#include "resData.h"

#include "defineText.h"
#include "AppDefine.h"

#include "WndSelectCh.h"
#include "DPClient.h"

/****************************************************
  WndId : APP_SELECT_CHANNEL - 채널 선택
  CtrlId : WIDC_LISTBOX1 - Listbox
  CtrlId : WIDC_STATIC1 - GroupBox
****************************************************/

CWndSelectCh::CWndSelectCh(int nItemId, unsigned int nChCount) { 
	m_nItemId  = nItemId;
	m_nChCount = nChCount + 1;	// 0일때 1개이므로 실제갯수대로 맞춰준다
} 

void CWndSelectCh::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 여기에 코딩하세요
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->ResetContent();
	CString strTitle;
	for (unsigned int i = 0; i < m_nChCount; ++i) {
		strTitle.Format("%s	%d", prj.GetText(TID_GAME_CHAR_SERVERNAME), i + 1);
		pWndListBox->AddString(strTitle);
	}

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSelectCh::Initialize( CWndBase* pWndParent ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_SELECT_CHANNEL, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSelectCh::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	int nSelect = 0;

	switch(nID)
	{
		case WIDC_LISTBOX1: // view ctrl
			{
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
				char strTemp[8];
				nSelect = pWndListBox->GetCurSel() * -1;
				_itoa(nSelect, strTemp, 10);
				g_DPlay.SendDoUseItemInput(m_nItemId, strTemp);
				Destroy();
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
