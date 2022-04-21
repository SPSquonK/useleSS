
#include "stdafx.h"
#include "resData.h"
#include "WndQuitRoom.h"
#include "MsgHdr.h"
#include "DPClient.h"

/****************************************************
  WndId : APP_QUIT_ROOM - 미니룸
  CtrlId : WIDC_STATIC1 - 미니룸에서 퇴장 하시겠습니까?
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
****************************************************/

void CWndQuitRoom::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndQuitRoom::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUIT_ROOM, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndQuitRoom::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndQuitRoom::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndQuitRoom::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case WIDC_BUTTON1:// ok 버튼
			g_DPlay.SendPacket<PACKETTYPE_HOUSING_GOOUT>();
			Destroy();
			break;

		case WIDC_BUTTON2:// cancel 버튼
			Destroy();
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

