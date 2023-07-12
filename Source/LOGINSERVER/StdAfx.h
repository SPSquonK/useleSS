#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _USE_32BIT_TIME_T
#define POINTER_64 __ptr64
#define __MASSIVE
#define _HAS_STD_BYTE 0
#define NOMINMAX

#include <afxwin.h>

#include <mmsystem.h>

#pragma warning(disable:4786)
#include <map>
#include <list>
#include <vector>


#include "VersionCommon.h"
#include "CommonHeader.h"

#include "memtrace.h"

#include "DefineCommon.h"
#include "CmnHdr.h"

#include <afxdisp.h>      

#include "DXUtil.h"
#include "Data.h"
#include "file.h"
#include "Scanner.h"
#include "Timer.h"
#include "d3dfont.h"
#include "vutil.h"

#include "debug.h"
#include "xutil.h"
#include "tickcount.h"

struct ServerAddrs {
	char db[16];
	char core[16];
	char cache[16];
};

extern	CTickCount	g_TickCount;
extern ServerAddrs g_lpAddr;
extern  BOOL	g_bNProtectAuth;
extern	char	g_szMSG_VER[256];