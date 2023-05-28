#pragma once

#include <charconv>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
/// DBSerialize API: to extract text from FlyFF DB strings with loops
/// SquonK, useless 2023-05
/// https://github.com/SPSquonK/useleSS
namespace DBDeserialize {
	/// A sentinel for the Slasher iterator
	struct Sentinel {};

	struct WordSplitter;


	/// Splits a string with the / symbol. The substrings are expected to be
	/// resplitted depending on commas.
	/// 
	/// It is expected to use a Slasher through the SplitBySlash function
	/// as Slasher is an iterator.
	class Slasher {
	private:
		const char * current;
	
	public:
		explicit Slasher(const char * text) : current(text) {}

		Slasher & operator++() {
			// Find end of sequence
			while (*current != '\0' && *current != '/' && *current != '$') {
				++current;
			}

			// Pass /
			if (*current == '/') {
				++current;
			}

			return *this;
		}

		// Using a sentinel as the end because computing the real end would be a
		// waste of time.
		[[nodiscard]] bool operator!=(Sentinel) const {
			return *current != '\0' && *current != '$';
		}

		// Not providing the usual unequality operator as even if we checked if a $
		// is at the end, it does not really cost must to use the Sentinel end.

		[[nodiscard]] WordSplitter operator*();
	};

	/// Splits a string with the , symbol. Stops when a / is encountered and
	/// then always return 0. Intended to be generated from a Slasher.
	class WordSplitter {
	private:
		// Pointer to the current position of the slasher to help it find the
		// end sooner.
		const char ** pCurrent;

	public:
		explicit WordSplitter(const char ** pCurrent) : pCurrent(pCurrent) {}

		auto NextInt() { return NextIntegral<int>(); }
		auto NextDWORD() { return NextIntegral<DWORD>(); }
		auto NextBool() { return NextIntegral<int>() != 0; }
		auto NextUShort() { return NextIntegral<unsigned short>(); }

		// Skip the data stored at the current position
		void Skip() { Next(FindUntil()); }


		template<typename EnumType>
		requires (std::is_enum_v<EnumType>)
		// TODO: and underlying type is writtable in int
		auto NextEnum() {
			const int rawValue = NextIntegral<int>();
			// TODO: check if in the range of the enum
			return static_cast<EnumType>(rawValue);
		}

		// Write in the given buffer the string written at the given position.
		// The string is considered DB encoded = the string is stored in the
		// form of the hexadecimal representation of each of its
		// ASCII/CP1252 characters.
		void NextStringInBuffer(std::span<char> string) {
			static constexpr auto ToInt = [](char c) {
				if (c >= '0' && c <= '9') return c - '0';
				if (c >= 'a' && c <= 'f') return c - 'a' + 10;
				if (c >= 'A' && c <= 'F') return c - 'A' + 10;
				return 0;
			};

			const char * until = FindUntil();

			size_t pos = 0;

			const char * current = *pCurrent;
			while (current != until && pos + 1 < string.size()) {
				char tens = *current;
				++current;
				if (current == until) break;
				char units = *current;
				++current;

				string[pos] = ToInt(tens) * 0x10 + ToInt(units);
				++pos;
			}

			string[pos] = '\0';

			Next(until);
		}

	private:
		template<std::integral T>
		T NextIntegral() {
			// yay templates!
			const char * until = FindUntil();
			if (*pCurrent == until) return 0;

			T result = T(0);
			std::from_chars(*pCurrent, until, result);
			// TODO: error checking

			Next(until);
			return result;
		}

		// Find the next string end, /, $ or ,
		const char * FindUntil() const {
			const char * current = *pCurrent;

			while (*current != '\0' && *current != '/' && *current != '$' && *current != ',') {
				++current;
			}

			return current;
		}

		// Sets the cursor at end. If end is on a comma, set the cursor at the
		// position just after.
		void Next(const char * end) {
			if (*end == ',') {
				++end;
			}

			*pCurrent = end;
		}
	};

	inline WordSplitter Slasher::operator*() {
		return WordSplitter(&current);
	}

	inline auto SplitBySlash(const char * text) {
		struct Range {
			const char * baseText;

			auto begin() { return Slasher(baseText); }
			auto end() { return Sentinel{}; }
		};

		return Range{ text };
	}
}

///////////////////////////////////////////////////////////////////////////////
// V15 string decoding API

// GetXXFromStr -> read a value
// "1/2/3" GetIntFromStr x3 -> 1 2 3
// GetXXPaFromStr -> If a / is encountered, it is not passed
// "1/2/3" GetIntPaFromStr x3 -> 1 0 0 (location is blocked on the first /)


// ',', '/', '\0' 을 만날때까지 값을 리턴하고 커서는 다음...
inline void GetStrFromStr(const char *pBuf, char *strReturn, int *pLocation)
{
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!='/' && pBuf[*pLocation]!=0) {
		strReturn[count]=pBuf[*pLocation];
		count++; (*pLocation)++;
	}
	strReturn[count]='\0';(*pLocation)++;
}

inline int GetIntFromStr(const char * pBuf, int * pLocation) {
	char strTemp[50];
	GetStrFromStr(pBuf, strTemp, pLocation);
	return atoi(strTemp);
}


inline void GetStrPaFromStr(const char * pBuf, char * strReturn, int * pLocation) {
	int count = 0;
	while (pBuf[*pLocation] != ',' && pBuf[*pLocation] != 0) {
		if (pBuf[*pLocation] != '/') {
			strReturn[count] = pBuf[*pLocation];
			count++; (*pLocation)++;
		} else {
			if (count == 0) {
				strReturn[0] = '\0';
			} else {
				strReturn[count] = pBuf[*pLocation];
			}
			return;
		}
	}
	strReturn[count] = 0; (*pLocation)++;
}

inline	__int64 GetInt64PaFromStr(char * pBuf, int * pLocation) {
	char strTemp[50];
	GetStrPaFromStr(pBuf, strTemp, pLocation);
	return _atoi64(strTemp);
}

// ',', '\0' 을 만날때까지 값을 리턴하고 커서는 다음...
// '/' 를 만나면 값을 리턴하고 커서는 현재('/')...
inline int GetIntPaFromStr(const char *pBuf, int *pLocation )
{
	char strTemp[50];
	GetStrPaFromStr(pBuf, strTemp, pLocation);
	return atoi(strTemp);
}

inline SERIALNUMBER	GetSerialNumberPaFromStr(char *pBuf, int *pLocation )
{
	char strTemp[50];
	GetStrPaFromStr(pBuf, strTemp, pLocation);
	return atoi(strTemp);
}

