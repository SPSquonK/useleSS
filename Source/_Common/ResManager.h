#ifndef __RESMANAGER_H
#define __RESMANAGER_H

#include "data.h"
#include "lang.h"

#define DEF_CTRL_CHECK "ButtCheck.bmp"
#define DEF_CTRL_RADIO "ButtRadio.bmp"
#define DEF_CTRL_GROUP "WndEditTile200.tga"
#define DEF_CTRL_TAB1  "WndTabTile00.bmp"
#define DEF_CTRL_TAB2  "WndTabTile10.bmp"
#define DEF_WNDBASE    "WndTile00.tga"
#define DEF_CTRL_TEXT  "WndEditTile00.tga"

typedef struct tagWndCtrl
{
	DWORD dwWndId;
	DWORD dwWndType;
	DWORD dwWndStyle;
	BOOL bTile;

	CString strDefine;
	CString strTitle;
	CString strToolTip;	
	CString strTexture;
	CRect rect;
	BOOL m_bVisible;
	BOOL m_bDisabled;
//	BOOL m_bTabStop; // Never used
	BOOL m_bGroup;
} WNDCTRL,* LPWNDCTRL; 

typedef struct tagWndApplet
{
	DWORD dwWndId;
	DWORD dwWndStyle;
	BOOL bTile;
	
	CString strTitle;
	CString strToolTip;	
	D3DFORMAT d3dFormat;
	CString strDefine;
	CString strTexture;
	std::vector<WNDCTRL *> ptrCtrlArray;
	CSize size;

	LPWNDCTRL GetAt( DWORD dwWndId );

} WNDAPPLET,* LPWNDAPPLET;

class CResManager final {
public:
	~CResManager();
		
private:
	std::map<DWORD, WNDAPPLET *> m_mapWndApplet;

public:
	BOOL Load( LPCTSTR lpszName );
	LPWNDAPPLET GetAt( DWORD dwAppletId );
	LPWNDCTRL GetAtControl( DWORD dwAppletId, DWORD dwCtrlId );

private:
	CString GetLangApplet( CScript& scanner );
	CString GetLangCtrl( CScript& scanner );
};


#endif