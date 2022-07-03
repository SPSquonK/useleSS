#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"

#include "DPClient.h"


/****************************************************
  WndId : APP_PARTY_CONFIRM - party confirm
  CtrlId : WIDC_YES - yes
  CtrlId : WIDC_NO - No
  CtrlId : WIDC_STATIC1 - Static
****************************************************/

CWndPartyConfirm::CWndPartyConfirm(u_long uLeader, const TCHAR * szLeaderName)
	: m_uLeader(uLeader) {
	std::strcpy(m_szLeaderName, szLeaderName);
}

void CWndPartyConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CString strTemp;
	strTemp.Format(_T(prj.GetText(TID_DIAG_0072)),m_szLeaderName); // 메시지 바꾸려면 이걸 바꾸시오
	pWndText->SetString( strTemp );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartyConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_PARTY_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndPartyConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES ) 
	{
		// 여기다가 승락하는 처리 추가하시오
		g_DPlay.SendAddPartyMember(m_uLeader);
		Destroy();	// 수동파괴로 바꿈 -XUZHU- 09/16
	}
	else if(nID==WIDC_NO || nID==WTBID_CLOSE)
	{
		// 여기다가 거부하는 처리 추가하시오
		g_DPlay.SendPartyMemberCancle(m_uLeader, 0);
		Destroy();	// 수동파괴로 바꿈 -XUZHU- 09/16
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

