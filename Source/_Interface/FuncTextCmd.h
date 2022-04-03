#ifndef __FUNCTEXTCMD_H
#define __FUNCTEXTCMD_H

struct TextCmdFunc
{
	BOOL (*m_pFunc)( CScanner& scanner );
	const TCHAR* m_pCommand;  
	const TCHAR* m_pAbbreviation;  
	const TCHAR* m_pKrCommand;  
	const TCHAR* m_pKrAbbreviation;  
	DWORD  m_nServer; // 0 = client, 1 = server, 2 = ตัดู 
	DWORD m_dwAuthorization;
	const TCHAR* m_pszDesc;
};
extern TextCmdFunc m_textCmdFunc[];

#endif
