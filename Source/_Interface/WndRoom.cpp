#include "stdafx.h"
#include "WndRoom.h"

#include <boost/range/adaptor/indexed.hpp>

#include "MsgHdr.h"
#include "resData.h"
#include "defineText.h"
#include "WndTListBox.hpp"
#include "playerdata.h"
#include "DPClient.h"

/****************************************************
  WndId : APP_MINIROOM_LIST - 미니룸 목록
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
  CtrlId : WIDC_BUTTON3 - Button
  CtrlId : WIDC_LISTBOX1 - Listbox
  CtrlId : WIDC_TEXT1 - 현재 입장이 가능한 미니룸의 캐릭터 목록입니다.
****************************************************/

void CWndRoomList::Refresh() {
	// 리스트를 새로 갱신
	CWndBasicTListBox<Item> * pWndListBox = GetDlgItem<CWndBasicTListBox<Item>>(WIDC_LISTBOX1);

	pWndListBox->ResetContent();

	auto visitable = CHousing::GetInstance()->m_vecVisitable
		| boost::adaptors::indexed(1);

	CString strName;
	for (const auto [nIndex, playerId] : visitable) {
		const PlayerData * pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(playerId);
		if (pPlayerData) {
			strName.Format("%d. %s", nIndex, pPlayerData->szPlayer);
			pWndListBox->Add(Item{ .text = strName, .playerId = playerId });
		}
	}
}

void CWndRoomList::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	auto & pWndListBox = ReplaceListBox<Item, WndTListBox::BasicDisplayer<Item>>(WIDC_LISTBOX1);

	// 여기에 코딩하세요
	CHousing::GetInstance()->m_vecVisitable.clear();
	g_DPlay.SendHousingReqVisitableList();

	pWndListBox.ChangeSelectColor(D3DCOLOR_ARGB(255, 255, 0, 0));

	MoveParentCenter();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRoomList::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	return CWndNeuz::InitDialog(APP_MINIROOM_LIST, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndRoomList::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_BUTTON1:// ok 버튼
		{
			const CWndBasicTListBox<Item> * pWndListBox = GetDlgItem<CWndBasicTListBox<Item>>(WIDC_LISTBOX1);

			if (const Item * item = pWndListBox->GetCurSelItem()) {
				if (const u_long dwID = item->playerId) {
					g_DPlay.SendHousingVisitRoom(dwID);
				}

				Destroy();
			}
			break;
		}

		case WIDC_BUTTON2:// refresh 버튼
			g_DPlay.SendHousingReqVisitableList();
			break;

		case WIDC_BUTTON3:// cancel 버튼
			Destroy();
			break;
	};

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


/****************************************************
	WndId : APP_QUIT_ROOM - 미니룸
	CtrlId : WIDC_STATIC1 - 미니룸에서 퇴장 하시겠습니까?
	CtrlId : WIDC_BUTTON1 - Button
	CtrlId : WIDC_BUTTON2 - Button
****************************************************/

void CWndQuitRoom::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndQuitRoom::Initialize(CWndBase * pWndParent, DWORD /*dwWndId*/) {
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog(APP_QUIT_ROOM, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndQuitRoom::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_BUTTON1:// ok 버튼
			g_DPlay.SendPacket<PACKETTYPE_HOUSING_GOOUT>();
			Destroy();
			break;

		case WIDC_BUTTON2:// cancel 버튼
			Destroy();
			break;
	};
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
