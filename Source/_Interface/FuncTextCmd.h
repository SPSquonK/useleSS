#pragma once

struct TextCmdFunc {
	BOOL(*m_pFunc)(CScanner & scanner);
	const TCHAR * m_pCommand;
	const TCHAR * m_pAbbreviation;
	const TCHAR * m_pKrCommand;
	const TCHAR * m_pKrAbbreviation;
	DWORD  m_nServer; // 0 = client, 1 = server, 2 = ตัดู 
	DWORD m_dwAuthorization;
	const TCHAR * m_pszDesc;
};

namespace CmdFunc {
	class AllCommands {
	public:
		AllCommands();
		BOOL ParseCommand(LPCTSTR lpszString, CMover * pMover, BOOL bItem = FALSE);


		std::vector<TextCmdFunc>::const_iterator begin() const {
			return m_allCommands.cbegin();
		}

		std::vector<TextCmdFunc>::const_iterator end() const {
			return m_allCommands.cend();
		}

	private:
		std::vector<TextCmdFunc> m_allCommands;
	};
}


extern CmdFunc::AllCommands g_textCmdFuncs;
