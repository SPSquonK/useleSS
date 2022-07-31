#include "stdafx.h"
#include "heapmng.h"

CHeapMng::CHeapMng()
{
	CSystemInfo si;
	m_hHeap		= HeapCreate( 0, si.dwPageSize, 0 );
}

CHeapMng::~CHeapMng()
{
	HeapDestroy( m_hHeap );
}

void* CHeapMng::Malloc( size_t nSize )
{
	LPVOID lpMem	= HeapAlloc( m_hHeap, 0, nSize );
	return lpMem;
}

void CHeapMng::Free( LPVOID lpMem )
{
	HeapFree( m_hHeap, 0, lpMem );
}

void* CHeapMng::Realloc( LPVOID lpMem, size_t nSize  )
{
	LPVOID lpNewMem		= HeapReAlloc( m_hHeap, 0, lpMem, nSize );
	return lpNewMem;
}

