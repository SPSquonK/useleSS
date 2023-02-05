#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

#include "Resource.h"

extern	CProject			prj;
 
#if defined(__ITEM_REMOVE_LIST)
BOOL CALLBACK VerifyPlayerDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
		case WM_LBUTTONDOWN:
			SendMessage( hDlg, WM_NCLBUTTONDOWN, HTCAPTION, lParam ); 
			break;
		case WM_LBUTTONDBLCLK:
			EndDialog( hDlg, 0 );
			break;
	}
	return FALSE;
}
#endif
