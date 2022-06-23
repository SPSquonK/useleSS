#include "stdafx.h"
#include "BillingMgr.h"
#include "BillingMgrJP.h"
#include "BillingMgrJP2.h"
#include "BillingMgrTW.h"
#include "BillingMgrTH.h"
#include "query.h"

///////////////////////////////////////////////////////////////////////
// global variable
///////////////////////////////////////////////////////////////////////

static			CBillingMgr* g_pBillingMgr = NULL;	

///////////////////////////////////////////////////////////////////////
// global function
///////////////////////////////////////////////////////////////////////

// strcpy와 같은기능 하지만, NULL이전의 포인터를 리턴한다.
char* StrCpyExcludeNull( char* dst, const char* src )
{
	char *cp = dst;

	while( *cp++ = *src++ )
		   ; 
	return( cp - 1 );
}

// pCur에서 pEnd까지 공백(0x20)을 붙인다.
void AppendSpace( char* pCur, char* pEnd )
{
	if( pEnd - pCur )
		memset( pCur, 0x20, pEnd - pCur );
}

CBillingMgr* GetBillingMgr()
{
	ASSERT( g_pBillingMgr );
	return g_pBillingMgr;
}
