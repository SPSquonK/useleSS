#pragma once

#include <stack>
#include <list>

#define INVALID_KEY	(DWORD)0xFFFFFFFF



#ifdef	__VERIFY_LOOP041010
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP(n)				int __loop = (n);
	#define VERIFY_LOOP(file, line)		if( --__loop == 0 ) { FILEOUT( "..\\mymap.txt", "%s, %d", file, line ); }	
#else
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP(n)				((void)0);
	#define VERIFY_LOOP(file, line)		((void)0);
#endif

/*--------------------------------------------------------------------------------*/
