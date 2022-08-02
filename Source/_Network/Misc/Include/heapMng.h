#ifndef __HEAPMNG_H__
#define __HEAPMNG_H__

#pragma once

class CHeapMng
{
public:
	virtual	~CHeapMng();
	void*	Malloc( size_t nSize );
	CHeapMng ();
	void	Free( LPVOID lpMem );
	void*	Realloc( LPVOID lpMem, size_t nSize );
private:
	HANDLE	m_hHeap;
};
#endif //__HEAPMNG_H__