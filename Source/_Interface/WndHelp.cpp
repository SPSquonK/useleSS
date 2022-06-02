#include "stdafx.h"
#include "AppDefine.h"
#include "WndHelp.h"
#include <format>

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndHelp::OnDraw(C2DRender * const p2DRender) {
	const DWORD dwLeft = m_rectClient.Width() * 50 / 100;
	const CRect rect(dwLeft + 5, 5, m_rectClient.Width() - 5, 23);
	p2DRender->TextOut(rect.left + 10, rect.top + 8, m_strKeyword, 0xff000000);
}

void CWndHelp::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndTreeCtrl * pWndTreeCtrl = (CWndTreeCtrl *)GetDlgItem(WIDC_TREE1);
	pWndTreeCtrl->LoadTreeScript(MakePath(DIR_CLIENT, _T("treeHelp.inc")));
	
	CWndText * pWndText = (CWndText *)GetDlgItem(WIDC_TEXT1);
	pWndText->AddWndStyle(WBS_VSCROLL);

	MoveParentCenter();
}

BOOL CWndHelp::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog( APP_HELPER_HELP, pWndParent, 0, CPoint(0, 0) );
}

BOOL CWndHelp::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult) {
	if (nID == WIDC_TREE1) {
		TREEELEM * lpTreeElem = reinterpret_cast<TREEELEM *>(pLResult);
		if (lpTreeElem) ChangeDisplayedHelp(*lpTreeElem);
	}

	return CWndNeuz::OnChildNotify(message,nID,pLResult);
}

void CWndHelp::ChangeDisplayedHelp(TREEELEM & treeElem) {
	m_strKeyword = treeElem.m_strKeyword;

	const CString string = prj.GetHelp(m_strKeyword);

	CWndText * pWndText = (CWndText *)GetDlgItem(WIDC_TEXT1);
	pWndText->SetString(string);
	pWndText->UpdateScrollBar();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndHelpFAQ::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	LoadFAQ(MakePath(DIR_CLIENT, _T("Faq.inc")));
	MoveParentCenter();
}

BOOL CWndHelpFAQ::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog( APP_HELPER_FAQ, pWndParent, 0, CPoint(0, 0) );
}

bool CWndHelpFAQ::LoadFAQ(LPCTSTR lpszFileName) {
	CScript s;
	if (!s.Load(lpszFileName)) return FALSE;

	s.GetToken();	// keyword
	while (s.tok != FINISHED) {
		const std::string question = std::format("Q.{}", s.token);
		s.GetToken();	// {
		s.GetToken();	//
		const std::string answer = std::format("    {}", s.token);
		m_mapFAQ.insert_or_assign(question, answer);
		s.GetToken();	// }
		s.GetToken();
	}

	CWndListBox * pWndListBox = GetDlgItem<CWndListBox>(WIDC_LIST1);
	pWndListBox->ResetContent();
	for (const std::string & question : m_mapFAQ | std::views::keys) {
		pWndListBox->AddString(question.c_str());
	}

	return true;
}

BOOL CWndHelpFAQ::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_LIST1) {
		CString question;
		CWndListBox * pWndListBox = GetDlgItem<CWndListBox>(WIDC_LIST1);
		pWndListBox->GetText(pWndListBox->GetCurSel(), question); // TODO: obsolete CWndListBox::GetText

		OnChangedSelection(question.GetString());
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndHelpFAQ::OnChangedSelection(const std::string & question) {
	std::string answer;
	
	const auto it = m_mapFAQ.find(question);
	if (it != m_mapFAQ.end()) {
		answer = it->second;
	}

	CWndText * const pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	const CRect rect = pWndText->GetClientRect();
	pWndText->m_string.Init(m_pFont, &rect);
	pWndText->m_string.SetString("");
	pWndText->m_string.AddString(question.c_str(), 0xff8080ff, ESSTY_BOLD);
	pWndText->m_string.AddString("\n\n");
	pWndText->m_string.AddString(answer.c_str());
	pWndText->UpdateScrollBar();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndHelpTip::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	LoadTips();

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT);
	pWndText->AddWndStyle(WBS_NOFRAME);
	pWndText->AddWndStyle(WBS_NODRAWFRAME);
	pWndText->SetString(m_aString[0].GetString(), 0xff000000);

	MoveParentCenter();
}

bool CWndHelpTip::LoadTips() {
	m_aString.clear();
	m_current = 0;

	CScript s;
	if (!s.Load(MakePath(DIR_CLIENT, _T("tip.inc")))) {
		m_aString.emplace_back("");
		return false;
	}

	s.GetToken();
	while (s.tok != FINISHED) {
		m_aString.emplace_back("    " + s.Token);
		s.GetToken();
	}

	if (m_aString.empty()) {
		m_aString.emplace_back("");
	}

	return true;
}

BOOL CWndHelpTip::Initialize(CWndBase * pWndParent, DWORD) {
	return InitDialog( APP_HELPER_TIP );
}

BOOL CWndHelpTip::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	switch (nID) {
		case WIDC_CHECK1:
			g_Option.m_bTip = !g_Option.m_bTip;
			Destroy();
			break;
		case WIDC_NEXT:
			m_current = (m_current + 1) % m_aString.size();
			GetDlgItem<CWndText>(WIDC_TEXT)->SetString(m_aString[m_current], 0xff000000);
			break;
		case WIDC_CLOSE:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndHelpInstant::OnInitialUpdate()  {
	CWndNeuz::OnInitialUpdate(); 

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	pWndText->SetString(prj.GetHelp(m_strHelpKey));
	
	MoveParentCenter();
} 

BOOL CWndHelpInstant::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog( APP_HELP_INSTANT, pWndParent, 0, CPoint(0, 0) );
}

BOOL CWndHelpInstant::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_CLOSE) {
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
