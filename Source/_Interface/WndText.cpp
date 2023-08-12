#include "stdafx.h"
#include "WndText.h"
#include "DPClient.h"


///////////////////////////////////////////////////////////////////////////////

BOOL CWndTextQuest::Initialize(CWndBase * pWndParent) {
	return CWndMessageBox::Initialize("퀘스트를 진행하시겠습니까?", pWndParent, MB_OKCANCEL);
}

BOOL CWndTextQuest::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case IDOK:
			g_DPlay.SendDoUseItem(MAKELONG(0, m_pItemBase->m_dwObjId), NULL_ID);
			Destroy();
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


///////////////////////////////////////////////////////////////////////////////


void CWndTextFromItem::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요

	const ItemProp * const pItemProp = m_pItemBase->GetProp();

	if (pItemProp->dwQuestId == QuestIdNone || m_pItemBase->m_bItemResist != SAI79::NO_PROP) {
		CWndButton * pWndAccept = GetDlgItem<CWndButton>(WIDC_ACCEPT);
		pWndAccept->EnableWindow(FALSE);
	}

	CFileIO file;
	if (file.Open(MakePath(DIR_TEXT, pItemProp->szTextFileName), "rb")) {
		std::unique_ptr<CHAR[]> pText = std::make_unique<CHAR[]>(file.GetLength() + 1);
		file.Read(pText.get(), file.GetLength());
		pText[file.GetLength()] = 0;

		CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
		pWndText->SetString(pText.get());
	}

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
}

BOOL CWndTextFromItem::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_ACCEPT:
		{
			CWndTextQuest * pWndTextQuest = new CWndTextQuest;
			pWndTextQuest->m_pItemBase = m_pItemBase;
			g_WndMng.OpenCustomBox(pWndTextQuest);
			break;
		}
		case WIDC_CLOSE:
			Destroy();
			break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
