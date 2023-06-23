#include "StdAfx.h"
#include "NameValidation.h"
#include "defineText.h"
#include "lang.h"

extern CString GetLangFileName(int nLang, _FILEWITHTEXT nType);

bool CNameValider::Load() {
	constexpr auto LoadInvalidNames = []() -> std::optional<std::set<std::string>> {
		const CString strFilter = GetLangFileName(::GetLanguage(), FILE_INVALID);

		CScanner s;
		if (!s.Load(strFilter.GetString())) return std::nullopt;

		std::set<std::string> retval;

		s.GetToken();
		while (s.tok != FINISHED) {
			CString szName = s.Token;
			szName.MakeLower();
			retval.emplace(szName.GetString());
			s.GetToken();
		}

		return retval;
	};

	constexpr auto LoadValidLetters = [](bool isVendor) -> std::optional<std::set<char>> {
		const auto fileId = isVendor ? FILE_ALLOWED_LETTER2 : FILE_ALLOWED_LETTER;
		const CString strFile = GetLangFileName(::GetLanguage(), fileId);

		CScanner s;
		if (!s.Load(strFile)) return std::nullopt;

		std::set<char> retval;

		s.GetToken();
		while (s.tok != FINISHED) {
			if (s.Token.GetLength()) {
				retval.emplace(s.Token.GetAt(0));
			}
			s.GetToken();
		}

		return retval;
	};

	constexpr auto Arrayize = [](const std::optional<std::set<char>> & letters) {
		std::array<bool, 256> retval;

		if (!letters) {
			retval.fill(true);
		} else {
			retval.fill(false);

			for (const char letter : letters.value()) {
				retval[static_cast<unsigned char>(letter)] = true;
			}
		}

		return retval;
	};

	std::optional<std::set<std::string>> invalidNames = LoadInvalidNames();
	if (!invalidNames) {
		Error(__FUNCTION__ "() failed loading InvalidNames");
		return false;
	}

	std::optional<std::set<char>> allowedLetters1 = LoadValidLetters(false);
	std::optional<std::set<char>> allowedLetters2 = LoadValidLetters(true);

	m_invalidNames = std::vector(invalidNames->begin(), invalidNames->end());
	m_allowedLetters          = Arrayize(allowedLetters1);
	m_allowedLettersForVendor = Arrayize(allowedLetters2);

	return true;
}

bool CNameValider::IsNotAllowedName(LPCSTR name) const {
	return IsInvalidName(name) || !AllLettersAreIn(name, m_allowedLetters);
}

bool CNameValider::IsNotAllowedVendorName(LPCSTR name) const {
	return IsInvalidName(name) || !AllLettersAreIn(name, m_allowedLettersForVendor);
}

bool CNameValider::IsInvalidName(LPCSTR szName) const {
	CString str = szName;
	str.MakeLower();

	return std::ranges::any_of(m_invalidNames,
		[&](const std::string & invalidName) {
			return str.Find(invalidName.c_str()) != -1;
		});
}

bool CNameValider::AllLettersAreIn(LPCSTR name, const std::array<bool, 256> & allowed) {
	const char * c = name;
	while (*c != '\0') {
		if (!allowed[static_cast<unsigned char>(*c)]) {
			return false;
		}
		++c;
	}
	return true;
}

void CNameValider::Formalize(LPSTR szName) {
	const auto language = ::GetLanguage();
	if (language != LANG_GER && language != LANG_FRE) return;

	if (szName[0] == '\0') return;

	char buffer[2] = { szName[0], '\0' }; // Copy first letter
	_strupr(buffer);                      // Upper the first letter
	_strlwr(szName + 1);                  // Lower after first letter
	szName[0] = buffer[0];                // Copy back upper cased 1st letter
}


DWORD IsValidPlayerNameTWN(CString & string);

// return 0   : OK
//        0 > : error
DWORD IsValidPlayerName( CString& strName )
{
	strName.Trim();

	LPCTSTR lpszString = strName;
	if( strName.IsEmpty() )
		return TID_DIAG_0031;			// "이름을 입력하십시오."

	// "이름은 한글 2자 이상, 8자 이하로 입력하십시오."
	// "이름은 영문 4자 이상, 16자 이하로 입력하십시오."
	if( strName.GetLength() < 4 || strName.GetLength() > 16 )
		return TID_DIAG_RULE_0;

	char c = strName[ 0 ];

	if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszString ) )
		return TID_DIAG_0012;			// "명칭에 첫글자를 숫자로 사용할 수 없습니다."

	int j;
	switch( ::GetLanguage() )
	{
	case LANG_THA:
		for( j = 0; j < strName.GetLength(); ++j )
		{
			c = strName[ j ];
			if( IsNative( &lpszString[ j ] ) == FALSE && ( isalnum( c ) == FALSE || iscntrl( c ) ) )		
				return TID_DIAG_0013;	// 명칭에 콘트롤이나 스페이스, 특수 문자를 사용할 수 없습니다.	
		}
		break;
	case LANG_TWN:
	case LANG_HK:
		return IsValidPlayerNameTWN( strName );

	default:
		for( j = 0; j < strName.GetLength(); ++j )
		{
			c = strName[ j ];
			if( IsDBCSLeadByte(c) ) 
			{
				++j;
				if( ::GetLanguage() == LANG_KOR )
				{
					char c2 = strName[ j ];
					WORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( IsHangul( word ) == FALSE ) 
						return TID_DIAG_0014;
				}					
			}
			else if( isalnum( c ) == FALSE || iscntrl( c ) )
			{
				char szLetter[2]	= { c, '\0' };
				if( (		::GetLanguage() == LANG_GER 
						||	::GetLanguage() == LANG_RUS
					) && prj.nameValider.IsAllowedLetter( szLetter ) )
					continue;
				return TID_DIAG_0013;
			}
		}
		break;
	}

	return 0;
}


DWORD IsValidPlayerNameTWN( CString& string )
{
	const char* begin	= string;
	const char* end		= begin + string.GetLength();
	const char* iter	= begin;
	char bytes[16];
	
	while( *iter && iter < end ) 
	{ 
		const char* next = CharNext(iter);

		memcpy( bytes, iter, next-iter );
		bytes[next-iter] = 0;

		if( IsMultiByte( iter ) ) 
		{
			wchar_t ch = MAKEWORD( bytes[1], bytes[0] );
			
			if( ch >= 0xA259 && ch <= 0xA261 || ch == 0xA2CD || ch >= 0xA440 && ch <= 0xC67E || ch >= 0xC940 && ch <= 0xF9D5 )
				;
			else
				return TID_DIAG_0014;
		}
		else if( isalnum( bytes[0] ) == FALSE || iscntrl( bytes[0] ) )
			return TID_DIAG_0013;

		iter = next;
	}
	return 0;
}


std::expected<CString, GuildNameError> CheckGuildName(LPCTSTR szName) {
	CString strGuild = szName;
	strGuild.Trim();

	const int nLength = strGuild.GetLength();
	if (nLength < 3) return std::unexpected(GuildNameError::TooShort);
	if (nLength > 16) return std::unexpected(GuildNameError::TooLong);

	const CHAR c = strGuild.GetAt(0);
	if (isdigit2(c) && !IsDBCSLeadByte(strGuild.GetAt(0))) {
		return std::unexpected(GuildNameError::DigitLead);
	}

	for (int i = 0; i < strGuild.GetLength(); i++) {
		const CHAR c = strGuild.GetAt(i);
		// 숫자나 알파벳이 아닐 경우는 의심하자.
		if (IsDBCSLeadByte(c) && ::GetLanguage() == LANG_KOR) {
			const CHAR c2 = strGuild.GetAt(++i);
			const WORD word = ((c << 8) & 0xff00) | (c2 & 0x00ff);
			if (IsHangul(word) == FALSE) {
				return std::unexpected(GuildNameError::BadEUCKRSymbol);
			}
		} else if (!IsCyrillic(c) && (!isalnum(c) || iscntrl(c))) {
			// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
			return std::unexpected(GuildNameError::BadSymbol);
		}
	}

	if (prj.nameValider.IsNotAllowedName(szName)) {
		return std::unexpected(GuildNameError::UnallowedName);
	}

	return strGuild;
}


DWORD CheckGuildNickName(CString & strNickName) {

	CHAR c = strNickName[0];

	int nLength = strNickName.GetLength();

	// 한글은 별칭 3~12자 가능...
	const int nMinLen = 2;
	const int nMaxLen = 12;

	if (nLength < nMinLen || nLength > nMaxLen) {
		return TID_DIAG_0011_01;
	} else
		if (IsDBCSLeadByte(c) == FALSE && isdigit2(c)) {
			return TID_DIAG_0012;
		} else {
			for (int i = 0; i < strNickName.GetLength(); i++) {
				c = strNickName[i];
				// 숫자나 알파벳이 아닐 경우는 의심하자.
				if (IsDBCSLeadByte(c) == TRUE) {
					CHAR c2 = strNickName[++i];
					WORD word = ((c << 8) & 0xff00) | (c2 & 0x00ff);
					if (::GetLanguage() == LANG_KOR) {
						if (IsHangul(word) == FALSE) {
							return TID_DIAG_0014;
						}
					}
				} else
					if (!IsCyrillic(c) && (isalnum(c) == FALSE || iscntrl(c))) {
						// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
						return TID_DIAG_0013;
					}
			}

		}

	if (prj.nameValider.IsNotAllowedName(strNickName)) {
		return TID_DIAG_0020;
	}

	return 0;
}

DWORD CheckPartyChangeName(CString & PartyName) {

	if (PartyName.IsEmpty()) {
		return TID_DIAG_RULE_0;
	}

	if (PartyName.GetLength() < 4 || PartyName.GetLength() > 16) {
		return TID_DIAG_RULE_0;
	}

	CHAR c = PartyName[0];

	if (IsDBCSLeadByte(c) == FALSE && isdigit2(c)) {
		return TID_DIAG_0012;
	}

	for (int i = 0; i < PartyName.GetLength(); i++) {
		CHAR c = PartyName[i];
		// 숫자나 알파벳이 아닐 경우는 의심하자.
		if (IsDBCSLeadByte(c) == TRUE) {
			CHAR c2 = PartyName[++i];
			WORD word = ((c << 8) & 0xff00) | (c2 & 0x00ff);
			if (::GetLanguage() == LANG_KOR) {
				if (IsHangul(word) == FALSE) {
					return TID_DIAG_0014;
				}
			}
		} else if (::GetLanguage() != LANG_THA) {
			if (!IsCyrillic(c) && (isalnum(c) == FALSE || iscntrl(c))) {
				// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
				return TID_DIAG_0013;
			}
		}
	}

	if (prj.nameValider.IsNotAllowedName(PartyName)) {
		return TID_DIAG_0020;
	}

	return 0;
}



bool IsAcValid( const TCHAR* lpszAccount )
{
	CString strAccount	= lpszAccount;
	if( strAccount.IsEmpty() == FALSE )
	{
		CHAR c	= strAccount.GetAt( 0 );
		if( strAccount.GetLength() < 3 || strAccount.GetLength() > 16 )
		{
			return FALSE;
		}
		else
		{
			for( int i = 0; i < strAccount.GetLength(); i++ )
			{
				c	= strAccount.GetAt( i );
				if( IsDBCSLeadByte( c ) == TRUE )
				{
					CHAR c2	= strAccount.GetAt( ++i );
					WORD word	= ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( IsHangul( word ) == FALSE ) 
					{
						return FALSE;
					}
					else if( isalnum( c ) == FALSE || iscntrl( c ) )
					{
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}

bool IsPwdValid( const TCHAR* lpszPassword )
{
	CString strPassword	= lpszPassword;
	if( strPassword.IsEmpty() == FALSE )
	{
		CHAR c	= strPassword.GetAt( 0 );
		if( strPassword.GetLength() < 3 || strPassword.GetLength() >= 16 )
		{
			return FALSE;
		}
		else
		{
			for( int i = 0; i < strPassword.GetLength(); i++ )
			{
				c	= strPassword.GetAt( i );
				if( IsDBCSLeadByte( c ) == TRUE )
				{
					CHAR c2	= strPassword.GetAt( ++i );
					WORD word	= ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( IsHangul( word ) == FALSE ) 
					{
						return FALSE;
					}
					else if( isalnum( c ) == FALSE || iscntrl( c ) )
					{
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}
