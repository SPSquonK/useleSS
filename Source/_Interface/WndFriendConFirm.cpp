#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndFriendConFirm.h"

#include "DPClient.h"
#include "MsgHdr.h"
#include "WndManager.h"

/****************************************************
  WndId : APP_FRIEND_CONFIRM - Confirm
****************************************************/

void CWndFriendConFirm::SetMember(u_long uLeader, const char * szLeadName) {
	m_uLeader = uLeader;
	std::strcpy(m_szLeaderName, szLeadName);
}

void CWndFriendConFirm::OnDraw(C2DRender * p2DRender) {
	CString strTemp;
	strTemp.Format(_T(prj.GetText(TID_DIAG_0071)), m_szLeaderName); // 메시지 바꾸려면 이걸 바꾸시오

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	pWndText->SetString(strTemp);
}

void CWndFriendConFirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndFriendConFirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_FRIEND_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndFriendConFirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )  { 
	if( nID == WIDC_YES )  {
		// 여기다가 승락하는 처리 추가하시오
		g_DPlay.SendPacket<PACKETTYPE_NC_ADDFRIEND, u_long>(m_uLeader);
		Destroy();	// 수동파괴로 바꿈. -xuzhu- 09/16
	} else if (nID == WIDC_NO) {
		// 여기다가 거부하는 처리 추가하시오
		g_DPlay.SendFriendCancel(m_uLeader, g_pPlayer ? g_pPlayer->m_idPlayer : 0);
		Destroy();	// 수동파괴로 바꿈. -xuzhu- 09/16
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_ADDFRIEND - 친구 추가
  CtrlId : WIDC_STATIC1 - 추가할 이름을 입력하세요
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_STATIC2 - 이  름 :
  CtrlId : WIDC_OK - 확인
  CtrlId : WIDC_CANCEL - 취소
****************************************************/

void CWndAddFriend::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndEdit * pWndEdit = (CWndEdit *)GetDlgItem(WIDC_EDIT1);
	pWndEdit->SetFocus();

	MoveParentCenter();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAddFriend::Initialize(CWndBase * pWndParent, DWORD) {
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(APP_ADDFRIEND, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndAddFriend::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_OK) {
		OnOk();
	} else if (nID == WIDC_CANCEL) {
		// 여기다가 거부하는 처리 추가하시오
		// g_DPlay.SendFriendCancel( m_uLeader, m_uMember );
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndAddFriend::OnOk() {
	CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);
	LPCTSTR szAddName = pWndEdit->GetString();

	if (std::strlen(szAddName) >= MAX_NAME) {
		pWndEdit->SetString("");
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0057)));
		return;
	}

	if (strcmp(szAddName, g_pPlayer->GetName()) == 0) {
		pWndEdit->SetString("");
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0056)));
		return;
	}

	if (g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR) {
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD));	// "수정해야함 : 길드대전장에는 친구추가를 할수 없습니다" );
	} else {
		g_DPlay.SendAddFriendNameReqest(szAddName);
		g_WndMng.PutString(TID_GAME_MSGINVATE, szAddName);
	}
	Destroy();

	// 여기다가 승락하는 처리 추가하시오
}
