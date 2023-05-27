#pragma once

#include "lang.h"

struct LANGFONT {
	CHAR	szFontFirst[32];
	CHAR	szFontSecond[32];
	CHAR	nDivTitle;
	CHAR	nDivCaption;
};

struct FONT_DATA
{
	int		charset;
	WORD	wCodePage;
	char	faceNT[32];
	char	face9x[32];
};

struct FONT_INFO {
	TCHAR	szFont[32];
	DWORD  dwColor;
	DWORD  dwBgColor;
	DWORD  nOutLine;
	DWORD  dwFlags;
};

struct NEUZFONT {
	LANGFONT	lfCaption;	// caption
	FONT_DATA	fdLang;		// language
	FONT_INFO	afi[6];	// "gulim9", "gulim8", "gulim13", "Arial Black9", "FontWorld", "gulim20"
};

struct LANG_DATA {
	char	szPath[MAX_PATH] = "";	// "Korean\\"
	char	szFileName[MAX_PATH] = "";	// "_TWN.inc"
	char	szTitle[100] = "";	// worldserver
	NEUZFONT	font;
	
	LANG_DATA() {
		memset(&font, 0, sizeof(NEUZFONT));
	}
};

using PLANG_DATA = LANG_DATA *;

class CLangMan
{
private:
	LANG_DATA	m_aLangData[LANG_MAX];
public:

	BOOL	Load( LPCTSTR	lpszFileName );
	PLANG_DATA	GetLangData( DWORD dwLang );

	static	CLangMan*	GetInstance( void );
};

