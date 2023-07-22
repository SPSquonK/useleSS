#pragma once

#if defined(__WORLDSERVER)
class CUser;
#elif defined(__CLIENT)
class CMover;
using CUser = CMover;
#else
static_assert(false, "FuncTextCmd.h should only be included in WORLD and in CLIENT");
#endif

struct TextCmdFunc {
	union {
		BOOL(*m_withPlayer)(CScanner & scanner, CUser * player);
		BOOL(*m_withoutPlayer)(CScanner & scanner);
	};
	bool m_ignoresPlayer;
	CString m_pCommand;
	CString m_pAbbreviation;
	const TCHAR * m_pKrCommand;
	const TCHAR * m_pKrAbbreviation;
	DWORD m_nServer; // 0 = client, 1 = server, 2 = ตัดู 
	DWORD m_dwAuthorization;
	const TCHAR * m_pszDesc;

	TextCmdFunc(
		BOOL(*withPlayer)(CScanner & scanner, CUser * player),
		const TCHAR * m_pCommand,
		const TCHAR * m_pAbbreviation,
		const TCHAR * m_pKrCommand,
		const TCHAR * m_pKrAbbreviation,
		DWORD m_nServer,
		DWORD m_dwAuthorization,
		const TCHAR * m_pszDesc
	);

	TextCmdFunc(
		BOOL(*withoutPlayer)(CScanner & scanner),
		const TCHAR * m_pCommand,
		const TCHAR * m_pAbbreviation,
		const TCHAR * m_pKrCommand,
		const TCHAR * m_pKrAbbreviation,
		DWORD m_nServer,
		DWORD m_dwAuthorization,
		const TCHAR * m_pszDesc
	);

	BOOL Call(CScanner & scanner, CUser * player) const {
		if (m_ignoresPlayer) {
			return m_withoutPlayer(scanner);
		} else {
			return m_withPlayer(scanner, player);
		}
	}
};

namespace CmdFunc {
	class AllCommands {
	public:
		AllCommands();
		BOOL ParseCommand(LPCTSTR lpszString, CUser * pMover, BOOL bItem = FALSE);

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
