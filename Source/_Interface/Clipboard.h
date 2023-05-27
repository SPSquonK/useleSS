#pragma once
#include <string>

// CClipboard Class
// ----------------
// Written by David Terracino <davet@lycosemail.com>
//
// This code is released into the public domain, because
// it's silly easy. If you want to use and expand it, go
// ahead; fine by me. If you do expand this class's
// functionality, please consider sending it back up to
// the MFC Programmer's Sourcebook at www.codeguru.com.
//
// And as always, please give credit where credit is
// due. Don't remove my name from the source.

class CClipboard
{
public:
	static std::string GetText();
	static bool SetText(LPCTSTR lpszBuffer);
};
