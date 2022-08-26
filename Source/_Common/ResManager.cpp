#include "stdafx.h"
#include "sqktd/algorithm.h"
#include <ranges>
#include <memory>
#include "ResManager.h"

LPWNDCTRL WNDAPPLET::GetAt( DWORD dwWndId ) {
	const auto it = std::ranges::find_if(ptrCtrlArray,
		[dwWndId](const WNDCTRL * pWndCtrl) { return pWndCtrl->dwWndId == dwWndId; }
	);
	return it != ptrCtrlArray.end() ? *it : nullptr;
}

CResManager::~CResManager() {
	for (WNDAPPLET *& pWndApplet : m_mapWndApplet | std::views::values) {
		for (WNDCTRL *& pWndCtrl : pWndApplet->ptrCtrlArray) {
			safe_delete(pWndCtrl);
		}
		SAFE_DELETE(pWndApplet);
	}
}

LPWNDAPPLET CResManager::GetAt( DWORD dwAppletId ) {
	return sqktd::find_in_map(m_mapWndApplet, dwAppletId, nullptr);
}

LPWNDCTRL CResManager::GetAtControl( DWORD dwAppletId, DWORD dwCtrlId )
{
	LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
	if( lpWndApplet )
		return lpWndApplet->GetAt ( dwCtrlId );
	return NULL;
}

CString CResManager::GetLangApplet( CScript& scanner )
{
	CString string;
	scanner.GetToken();	// {
	scanner.GetToken();
	string	= scanner.token;
	scanner.GetToken();	// }

	return string;
}
CString CResManager::GetLangCtrl( CScript& scanner )
{

	CString string;
	scanner.GetToken();	// {
	scanner.GetToken();
	string	= scanner.token;
	scanner.GetToken();	// }

	return string;
}

BOOL CResManager::Load( LPCTSTR lpszName )
{
	CScript scanner;
	if( scanner.Load( lpszName, FALSE ) == FALSE )
		return FALSE;

	DWORD dwWndType;
	scanner.GetToken_NoDef();

	while( scanner.tok != FINISHED )
	{
		LPWNDAPPLET pWndApplet = new WNDAPPLET;
		pWndApplet->strDefine = scanner.token;
		pWndApplet->dwWndId = CScript::GetDefineNum( scanner.token );

		scanner.GetToken();
		pWndApplet->strTexture = scanner.token;
		pWndApplet->bTile = scanner.GetNumber();
		CSize size;
		pWndApplet->size.cx = scanner.GetNumber();
		pWndApplet->size.cy = scanner.GetNumber();
		pWndApplet->dwWndStyle = scanner.GetNumber();
		pWndApplet->d3dFormat = (D3DFORMAT)scanner.GetNumber();

		// 타이틀 
		pWndApplet->strTitle = GetLangApplet( scanner );
		// 핼프 키 
		pWndApplet->strToolTip = GetLangApplet( scanner );

		
		// HelpKey
		m_mapWndApplet.emplace( pWndApplet->dwWndId, pWndApplet );
		scanner.GetToken(); // skip {
		dwWndType = scanner.GetNumber(); 
		while( *scanner.token != '}' )
		{
			LPWNDCTRL pWndCtrl = new WNDCTRL;
			pWndCtrl->dwWndType = dwWndType;
			scanner.GetToken_NoDef();
			pWndCtrl->strDefine = scanner.token;///Char;
			
			const auto it = std::ranges::find_if(
				pWndApplet->ptrCtrlArray,
				[&](WNDCTRL * existingCtrl) { return existingCtrl->strDefine == pWndCtrl->strDefine; }
			);

			if (it != pWndApplet->ptrCtrlArray.end()) {
				CString string;
				string.Format( "%s에서 ID 충돌 %s ", pWndApplet->strDefine, pWndCtrl->strDefine );
				AfxMessageBox( string );
			}

			pWndCtrl->dwWndId = CScript::GetDefineNum( scanner.token );
			scanner.GetToken();
			pWndCtrl->strTexture = scanner.token;//token;
			pWndCtrl->bTile = scanner.GetNumber();
			pWndCtrl->rect.left = scanner.GetNumber();
			pWndCtrl->rect.top = scanner.GetNumber();
			pWndCtrl->rect.right = scanner.GetNumber();
			pWndCtrl->rect.bottom = scanner.GetNumber();
			pWndCtrl->dwWndStyle = scanner.GetNumber();

			// visible, Group, Disabled, TabStop
			pWndCtrl->m_bVisible = scanner.GetNumber();
			pWndCtrl->m_bGroup = scanner.GetNumber();
			pWndCtrl->m_bDisabled = scanner.GetNumber();
			pWndCtrl->m_bTabStop = scanner.GetNumber();
			
			// 타이틀 
			pWndCtrl->strTitle = GetLangCtrl( scanner );
			// 핼프 키 
			pWndCtrl->strToolTip = GetLangCtrl( scanner );
			
			pWndApplet->ptrCtrlArray.emplace_back( pWndCtrl );
			dwWndType = scanner.GetNumber(); 
		} 
		scanner.GetToken_NoDef();
	} 

	return TRUE;
}
