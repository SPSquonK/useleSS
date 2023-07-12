#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _USE_32BIT_TIME_T
#define POINTER_64 __ptr64
#define __DBSERVER
#define _HAS_STD_BYTE 0
#define NOMINMAX

#include <afx.h>
#include <afxwin.h>
#include <afxext.h>         
#include <afxdisp.h>       
#include <afxdtctl.h>		
#include <afxcmn.h>			

#include "VersionCommon.h"
#include "CommonHeader.h"

#include "memtrace.h"

#include "DefineCommon.h"
#include "CmnHdr.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <assert.h>


#include "vutil.h"
#include "DXUtil.h"
#include <d3dx9math.h>
#include <mmsystem.h>
#include "xutil.h"
#include "path.h"
#include "Define.h"
#include "DefineNeuz.h"
#include "DefineJob.h"
#include "DefineWorld.h"
#include "resdata.h"
#include "Data.h"
#include "file.h"
#include "authorization.h"
#include "Scanner.h"
#include "Script.h"
#include "Project.h"
#include "Timer.h"
#include "Debug.h"
#include "d3dfont.h"
#include "Query.h"
#include "DbManager.h"
#include "defineitemkind.h"

#include "buyinginfo.h"
#include "DefineConMode.h"

#include "memtrace.h"

extern	HWND	hMainWnd;
extern	HINSTANCE	hInst;

struct APP_INFO {
	DWORD	dwSys;
	DWORD	dwId;		// server id
};

extern APP_INFO g_appInfo;

extern	char	DSN_NAME_CHARACTER01[260];
extern	char	DB_ADMIN_ID_CHARACTER01[260];
extern	char	DSN_NAME_LOG[260];
extern	char	DB_ADMIN_ID_LOG[260];

extern	char	DSN_NAME_BACKSYSTEM[260];
extern	char	DB_ADMIN_ID_BACKSYSTEM[260];
