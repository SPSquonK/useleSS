#pragma once

struct BUYING_INFO {
	static constexpr bool Archivable = true;

	DWORD	dwServerIndex = 0;
	DWORD	dwPlayerId    = 0;
	DWORD	dwSenderId    = 0;
	DWORD	dwItemId      = 0;
	DWORD	dwItemNum     = 0;
	char  szBxaid[21]   = ""; // bxaid
	DWORD	dwRetVal      = 0;
};

struct BUYING_INFO2 : public BUYING_INFO {
	static constexpr bool Archivable = true;

	DWORD	dpid = 0xFFFFFFFF;
	DWORD	dwKey = 0;
};

using PBUYING_INFO2 = BUYING_INFO2 *;

struct BUYING_INFO3 : public BUYING_INFO2 {
	static constexpr bool Archivable = true;

	DWORD	dwTickCount;
};

using PBUYING_INFO3 = BUYING_INFO3 *;
