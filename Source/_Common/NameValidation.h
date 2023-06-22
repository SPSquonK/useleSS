#pragma once

#include <array>
#include <expected>
#include <string>
#include <vector>  // find the computer room


class CNameValider {
	// Note: CNameValider logic is incompatible with wide char encoding.
	
	// std::array<bool, 256> is faster than std::bitset<256>
	// https://stackoverflow.com/a/58476584

private:
	std::vector<std::string> m_invalidNames;
	std::array<bool, 256> m_allowedLetters{ false, };
	std::array<bool, 256> m_allowedLettersForVendor{ false, };
	static_assert(sizeof(char) == 1, "char should be on only one byte so its value goes from 0 included to 256 excluded");

public:
	bool Load();

	static void Formalize(LPSTR szName);
	[[nodiscard]] bool IsNotAllowedName(LPCSTR name) const;
	[[nodiscard]] bool IsNotAllowedVendorName(LPCSTR name) const;
	[[nodiscard]] bool IsAllowedLetter(LPCSTR name) const { return AllLettersAreIn(name, m_allowedLetters); }

private:
	[[nodiscard]] bool IsInvalidName(LPCSTR name) const;
	[[nodiscard]] static bool AllLettersAreIn(LPCSTR name, const std::array<bool, 256> & allowed);
};

DWORD IsValidPlayerName(CString & strName);


enum class GuildNameError {
	TooShort, TooLong, DigitLead,
	BadEUCKRSymbol, BadSymbol,
	UnallowedName
};

std::expected<CString, GuildNameError> CheckGuildName(LPCTSTR str);

DWORD CheckGuildNickName(CString & strNickName);

DWORD CheckPartyChangeName(CString & PartyName);
