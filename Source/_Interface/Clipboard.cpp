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

#include "stdafx.h"
#include "Clipboard.h"

// Disable "nullptr dereferenced"
#pragma warning(disable:6011)

std::string CClipboard::GetText()
{
	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed.
	HWND hWnd = nullptr;
	OpenClipboard(hWnd);

	// Request a pointer to the text on the clipboard.
	HGLOBAL hGlobal = GetClipboardData(CF_TEXT);

	// If there was no text on the clipboard, we have
	// been returned a NULL handle.	
	if (hGlobal == NULL) {
		CloseClipboard();
		return "";
	}

	// Now we have a global memory handle to the text
	// stored on the clipboard. We have to lock this global
	// handle so that we have access to it.
	LPSTR lpszData = (LPSTR)GlobalLock(hGlobal);
	// Now get the size of the text on the clipboard.
	SIZE_T nSize = GlobalSize(hGlobal);

	// Copy data in string
	std::string result;

	if (lpszData) {
		result.reserve(nSize + 1);
		for (SIZE_T i = 0; i != nSize; ++i) {
			result.push_back(lpszData[i]);
		}
	}

	// Now, simply unlock the global memory pointer
	// and close the clipboard.
	GlobalUnlock(hGlobal);
	CloseClipboard();

	return result;
}

bool CClipboard::SetText (LPCTSTR lpszBuffer) {
	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed. After opening, empty the clipboard so we
	// can put our text on it.
	HWND hWnd = nullptr;
	OpenClipboard(hWnd);
	EmptyClipboard();

	// Get the size of the string in the buffer that was
	// passed into the function, so we know how much global
	// memory to allocate for the string.
	unsigned long nSize = lstrlen(lpszBuffer);

	// Allocate the memory for the string.
	HGLOBAL hGlobal = GlobalAlloc(GMEM_ZEROINIT, nSize+1);
	
	// If we got any error during the memory allocation,
	// we have been returned a NULL handle.
	if (hGlobal == NULL) return false;

	// Now we have a global memory handle to the text
	// stored on the clipboard. We have to lock this global
	// handle so that we have access to it.
	LPSTR lpszData = (LPSTR)GlobalLock(hGlobal);

	// Now, copy the text from the buffer into the allocated
	// global memory pointer.
	for (UINT i = 0; i < nSize + 1; ++i)
		*(lpszData + i) = *(lpszBuffer + i);

	// Now, simply unlock the global memory pointer,
	// set the clipboard data type and pointer,
	// and close the clipboard.
	GlobalUnlock(hGlobal);
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();

	return true;
}
