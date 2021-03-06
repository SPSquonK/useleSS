#include "stdafx.h"
#include "MsgHdr.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndManager.h"
#include "DPClient.h"
#include "party.h"




/****************************************************
  WndId : APP_CHANGETROUP - 극단 변경
  CtrlId : WIDC_STATIC1 - 순회극단으로 변경하시겠습니까?
  CtrlId : WIDC_STATIC2 - 극단명 : 
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

void CWndPartyChangeName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	const char * partyName = g_pPlayer->m_szPartyName;
	if (std::strlen(partyName) == 0) {
		partyName = prj.GetText(TID_GAME_PARTY_NAME);
	}
	
	strcpy(m_sParty, partyName);

	CWndEdit * pEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);
	pEdit->SetString(m_sParty);
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartyChangeName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_PARTYCHANGENAME, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndPartyChangeName::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_OK:
			OnSendName();
			return FALSE;
		case WIDC_CANCEL:
			Destroy();
			return FALSE;
		default:
			return CWndNeuz::OnChildNotify(message, nID, pLResult);
	}
}

void CWndPartyChangeName::OnSendName() {
	CWndEdit * pEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);
	CString PartyName = pEdit->GetString();

	PartyName.TrimLeft();
	PartyName.TrimRight();
	LPCTSTR lpszString = PartyName;

	if (PartyName.IsEmpty()) {
		// "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오."
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_RULE_0)));
		return;
	}

	if (PartyName.GetLength() < 4 || PartyName.GetLength() > 16) {
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_RULE_0)));
		return;
	}

	CHAR c = PartyName[0];

	if (IsDBCSLeadByte(c) == FALSE && isdigit2(c)) {
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0012)));
		return;
	}

	for (int i = 0; i < PartyName.GetLength(); i++) {
		CHAR c = PartyName[i];
		// 숫자나 알파벳이 아닐 경우는 의심하자.
		if (IsDBCSLeadByte(c) == TRUE) {
			CHAR c2 = PartyName[++i];
			WORD word = ((c << 8) & 0xff00) | (c2 & 0x00ff);
			if (::GetLanguage() == LANG_KOR) {
				if (IsHangul(word) == FALSE) {
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0014)));
					return;
				}
			}
		} else if (::GetLanguage() != LANG_THA) {
			if (!IsCyrillic(c) && (isalnum(c) == FALSE || iscntrl(c))) {
				// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0013)));
				return;
			}
		}
	}

	if (prj.nameValider.IsNotAllowedName(PartyName)) {
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0020)));
		return;
	}

	strcpy(m_sParty, PartyName);
	if (0 == strcmp(m_sParty, prj.GetText(TID_GAME_PARTY_NAME))) {
		return;
	}

	g_DPlay.SendChangeTroup(TRUE, m_sParty);
	Destroy();
}

//--------------------------------------------------------------------------------------------------------------------

CWndPartyChangeTroup::~CWndPartyChangeTroup() 
{ 
#ifdef __FIX_WND_1109
	SAFE_DELETE( m_WndPartyChangeName );
#endif	// __FIX_WND_1109
} 

void CWndPartyChangeTroup::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartyChangeTroup::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_CHANGETROUP, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndPartyChangeTroup::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) { 
	if (nID == WIDC_OK) {
		SAFE_DELETE(m_WndPartyChangeName);
		m_WndPartyChangeName = new CWndPartyChangeName;
		m_WndPartyChangeName->Initialize();
		Destroy();
	} else if (nID == WIDC_CANCEL) {
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

//------------------------------------------------------------------------------------------------
// 극단 탈퇴시 다시 확인 창
//------------------------------------------------------------------------------------------------
/****************************************************
WndId : APP_PARTYLEAVE_CONFIRM - 파티탈퇴확인
CtrlId : WIDC_EDIT1 - 
CtrlId : WIDC_YES - Yes
CtrlId : WIDC_NO - Button
****************************************************/

void CWndPartyLeaveConfirm::SetLeaveId(u_long uidPlayer) {
	// Precondition: Initialize must have been called
	uLeaveId = uidPlayer;

	CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_EDIT1);
	if (g_pPlayer->m_idPlayer == uLeaveId) {
		pWndEdit->SetString(prj.GetText(TID_DIAG_0084));
	} else {
		pWndEdit->SetString(prj.GetText(TID_DIAG_0085));
	}
	pWndEdit->EnableWindow(FALSE);
}

void CWndPartyLeaveConfirm::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartyLeaveConfirm::Initialize(CWndBase * pWndParent, DWORD) {
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(APP_PARTYLEAVE_CONFIRM, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndPartyLeaveConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if (nID == WIDC_NO || nID == WTBID_CLOSE) {
		Destroy();
	} else if (nID == WIDC_YES) {
		g_DPlay.SendPacket<PACKETTYPE_REMOVEPARTYMEMBER_NeuzCore, u_long>(uLeaveId);
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
