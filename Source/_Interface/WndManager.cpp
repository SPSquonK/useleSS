#include "stdafx.h"
#include <ranges>
#include "boost/container/small_vector.hpp"
#include <boost/container/flat_map.hpp>
#include <array>
#include "defineSkill.h"
#include "defineItem.h"
#include "defineSound.h"
#include "defineText.h"
#include "AppDefine.h"
//#include "WndActiveDesktop.h"
#include "WndMessenger.h"
#include "WndCommand.h"
#include "WndNotice.h"
#include "WndNotice.h"
#include "WndMap.h"
#include "WndPartyConfirm.h"
#include "WndPartyChangeTroup.h"
#include "WndFriendConfirm.h"
#include "WndDuelConfirm.h"
#include "WndRegVend.h"
#include "WndText.h"
#include "party.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "dpcertified.h"
#include "ClientMsg.h"
#include "yUtil.h"
#include "worldmap.h"
#include "eveschool.h"
#include "post.h"
#include "wndbagex.h"
#include "sqktd.h"

#include "couplehelper.h"
#include "FuncTextCmd.h"

#ifdef __Y_INTERFACE_VER3
#include "WorldMap.h"
#endif //__Y_INTERFACE_VER3

#include "WndSelectVillage.h"

#include "randomoption.h"

#include "playerdata.h"

#include "definelordskill.h"
#include "SecretRoom.h"
#include "webbox.h"
#include "dpclient.h"
#include "guild.h"
#include "guildwar.h"
#include "Tax.h"
#include "GuildHouse.h"
#include "WndGuildHouse.h"
#include "WndQuest.h"

#include "boost/pfr.hpp"

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#include "DPCollectClient.h"
#endif // __CERTIFIER_COLLECTING_SYSTEM

extern _ERROR_STATE g_Error_State;

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;

#define CLIENT_WIDTH FULLSCREEN_WIDTH
#define CLIENT_HEIGHT FULLSCREEN_HEIGHT

#ifdef __VS2003
CPtrArray      m_wndOrder;
#endif

CWndMgr          g_WndMng;

bool IsDst_Rate(int nDstParam);
const char * FindDstString(int nDstParam);


CString SingleDstToString(const SINGLE_DST & singleDst) {
	if (singleDst.nDst == DST_STAT_ALLUP) {
		CString str;
		str.AppendFormat("\n%s%+d", FindDstString(DST_STR), singleDst.nAdj);
		str.AppendFormat("\n%s%+d", FindDstString(DST_DEX), singleDst.nAdj);
		str.AppendFormat("\n%s%+d", FindDstString(DST_INT), singleDst.nAdj);
		str.AppendFormat("\n%s%+d", FindDstString(DST_STA), singleDst.nAdj);
		return str;
	} else {
		return "\n" + singleDst.ToString();
	}
}

CString SINGLE_DST::ToString() const {
	CString retval;

	const char * const dstName = FindDstString(nDst);

	if (IsDst_Rate(nDst)) {
		if (nDst == DST_ATTACKSPEED) {
			retval.Format("%s%+d%%", dstName, nAdj / 2 / 10);
		} else {
			retval.Format("%s%+d%%", dstName, nAdj);
		}
	} else {
		retval.Format("%s%+d", dstName, nAdj);
	}

	return retval;
}

template<MultipleDsts DstList>
static CString DstsToString(const DstList & dstList) {
	CString res;
	for (const auto & dst : dstList) {
		res += SingleDstToString(dst);
	}
	return res;
}

const char* GetATKSpeedString( float fSpeed )
{
	if( fSpeed < 0.035 )
		return prj.GetText(TID_GAME_VERYSLOW);  // "아주 느림"
	else if( fSpeed < 0.050 )
		return prj.GetText(TID_GAME_SLOW);		//"느림";
	else if( fSpeed < 0.070 )
		return prj.GetText(TID_GAME_NORMALS);	// "보통";
	else if( fSpeed < 0.080 )
		return prj.GetText(TID_GAME_FAST);		//"빠름";
	else if( fSpeed < 0.17 )
		return prj.GetText(TID_GAME_VERYFAST);	//"아주 빠름";
	else
		return prj.GetText(TID_GAME_FASTEST);	//"아주 빠름";
}

CTexture*	   g_pCoolTexArry[128];

CTexture*	   g_pEnchantTexArry[11];

void RenderEnchant( C2DRender* p2DRender, CPoint pt )
{
	static int i=0;
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );

	i++;

	if( i>=11 )
		i = 0;
	
	p2DRender->RenderTextureEx( ptBack, ptBack_size, g_pEnchantTexArry[i], 255, 1.0f, 1.0f ) ;
}

// 레이더 모양을 그린다.
void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor )
{
	pt.y += 1;
	pt.x += 15;
	
	CPoint pt_size( 15, 15 );
	const BYTE bAlpha = 200;
	
	//	float fPercent = (float)dwValue / (float)dwDivisor;
	//	int nDrawIndex = (fPercent * 127);
	int nDrawIndex = MulDiv( dwValue, 127, dwDivisor );
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );
	ptBack.x -= 15;
	p2DRender->RenderFillRect( CRect( ptBack, ptBack+ptBack_size ), D3DCOLOR_ARGB( 120, 255,255,255 ) );
	
	if( nDrawIndex >= 0 && nDrawIndex < 32 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[32], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 32 && nDrawIndex <= 63 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 64 && nDrawIndex <= 95 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 96 && nDrawIndex <= 128 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[95], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
	}	
				
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMgr::WNDREGINFO::WNDREGINFO(CFileIO & file) {
	file.Read(&dwWndId, sizeof(dwWndId));
	file.Read(&rect, sizeof(rect));
	file.Read(&dwWndSize, sizeof(dwWndSize));
	file.Read(&bOpen, sizeof(bOpen));
	file.Read(&dwVersion, sizeof(dwVersion));
	size_t dwSize;
	file.Read(&dwSize, sizeof(dwSize));

	lpArchive.resize(dwSize);

	if (dwSize != 0) {
		file.Read(lpArchive.data(), dwSize);
	}

	if (rect.left < 0) rect.left = 0;
	if (rect.top < 0) rect.top = 0;
}

void CWndMgr::WNDREGINFO::StoreIn(CFileIO & file) const {
	file.Write(&dwWndId, sizeof(dwWndId));
	file.Write(&rect, sizeof(rect));
	file.Write(&dwWndSize, sizeof(dwWndSize));
	file.Write(&bOpen, sizeof(bOpen));
	file.Write(&dwVersion, sizeof(dwVersion));
	
	const size_t dwSize = lpArchive.size();
	file.Write(&dwSize, sizeof(dwSize));
	if (dwSize != 0) {
		file.Write(lpArchive.data(), dwSize);
	}
}

CWndMgr::WNDREGINFO::WNDREGINFO(CWndNeuz & pWndNeuz, BOOL bOpen) {
	dwWndId = pWndNeuz.GetWndId();
	rect = pWndNeuz.GetWindowRect(TRUE);
	bOpen = bOpen;
	dwVersion = 0;
	dwWndSize = pWndNeuz.m_nWinSize;

	// Write
	CAr ar;
	pWndNeuz.SerializeRegInfo(ar, dwVersion);

	int nSize;
	BYTE * lpData = ar.GetBuffer(&nSize);

	lpArchive.resize(nSize);
	if (nSize != 0) {
		memcpy(lpArchive.data(), lpData, nSize);
	}
}

void CWndMgr::WNDREGINFO::RestoreParameters(CWndNeuz & pWndBase) const {
	if (!lpArchive.empty()) {
		// load
		CAr ar(const_cast<BYTE *>(lpArchive.data()), lpArchive.size());
		DWORD dwVersion = this->dwVersion;
		pWndBase.SerializeRegInfo(ar, dwVersion);
	}

	if (pWndBase.IsWndStyle(WBS_THICKFRAME)) {
		if (dwWndSize == WSIZE_WINDOW) {
			pWndBase.SetSizeWnd();
			pWndBase.SetWndRect(rect);
		}

		if (dwWndSize == WSIZE_MAX) {
			pWndBase.SetSizeMax();
		}
	} else {
		const CRect wndRect = pWndBase.GetWindowRect();
		pWndBase.SetWndRect(CRect(rect.TopLeft(), wndRect.Size()));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndMgr : 매니저 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define TASKBAR_HEIGHT 38

CWndMgr::CWndMgr()
{
	m_bTitle = FALSE;
	m_pWndActiveDesktop = NULL;
	m_pWndTaskBar = NULL;
	//m_pWndNotice = NULL;
	////m_pWndTip = NULL;
	//m_bTitle = TRUE;
	m_nAlphaCount = m_nAlpha;
	m_pWndBank = NULL;

	m_pWndSelectVillage = NULL;
	
	
#ifdef __S_SERVER_UNIFY
	m_bAllAction = TRUE;
#endif // __S_SERVER_UNIFY
	
//	m_pWndMessengerSearch	= NULL;

	m_szTimerChat[ 0 ] = 0;
	m_timerChat.Set( 0 );
	m_bConnect = FALSE;
	m_timerMessenger.Set( MIN( 5 ) );

	for( int i = 0 ; i < MAX_SKILL ; i++ )
	{
		m_dwSkillTime[i] = 0;
	}
	
	m_pWndGuildBank = NULL;

	m_bIsOpenLordSkill = false;

	m_pQuestItemInfo = NULL;
	m_pWndTextBook = NULL;
	m_pWndTextScroll = NULL;
	m_pWndTextLetter = NULL;
	m_dwSavePlayerTime = 0;
	m_bAutoRun = FALSE;

	m_clearFlag = FALSE;
	InitSetItemTextColor();

	// 텍스쳐렌더 버그 관련 수정
	memset(g_pEnchantTexArry, 0, sizeof(CTexture*) * 11);
	memset(g_pCoolTexArry, 0, sizeof(CTexture*) * 128);

	m_pWndGHMain = NULL;

#ifdef __BAN_CHATTING_SYSTEM
	m_nWarningCounter = 0;
	m_nWarning2Counter = 0;
	m_bShortcutCommand = FALSE;
#endif // __BAN_CHATTING_SYSTEM

#ifdef __PROTECT_AWAKE
	m_pWndSelectAwakeCase = NULL;
#endif

#ifdef __MAIL_REQUESTING_BOX
	m_bWaitRequestMail = FALSE;
#endif
}

void CWndMgr::InitSetItemTextColor( )
{
	
//	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 0단계
//	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(  12, 144, 231 );
	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(  46, 112, 169 );
	dwItemColor[FIRST_TC].dwName1				= D3DCOLOR_XRGB(   0,  93,   0 );			// 이름 1단계
	dwItemColor[FIRST_TC].dwName2				= D3DCOLOR_XRGB( 128,  64,   0 );			// 이름 2단계
//	dwItemColor[FIRST_TC].dwName2				= D3DCOLOR_XRGB( 182,   0, 255 );			// 이름 2단계
	dwItemColor[FIRST_TC].dwName3				= D3DCOLOR_XRGB( 127,   0, 255 );			// 이름 3단계
	dwItemColor[FIRST_TC].dwName4				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 4단계
	dwItemColor[FIRST_TC].dwName5				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 5단계
	dwItemColor[FIRST_TC].dwGeneral				= D3DCOLOR_XRGB(   0,   0,   0 );			// 일반
	dwItemColor[FIRST_TC].dwPlusOption			= D3DCOLOR_XRGB(   0,   0,   0 );			// 플러스 옵션
	dwItemColor[FIRST_TC].dwPiercing			= D3DCOLOR_XRGB( 178,   0, 255 );			// 피어싱
	dwItemColor[FIRST_TC].dwResist				= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 일반
	dwItemColor[FIRST_TC].dwResistFire			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 불
	dwItemColor[FIRST_TC].dwResistWater			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 물
	dwItemColor[FIRST_TC].dwResistEarth			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 땅
	dwItemColor[FIRST_TC].dwResistElectricity	= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 전기
	dwItemColor[FIRST_TC].dwResistWind			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 바람
	dwItemColor[FIRST_TC].dwResistSM			= D3DCOLOR_XRGB(   0, 158,   0 );			// 속성 상용화 무기
	dwItemColor[FIRST_TC].dwResistSM1			= D3DCOLOR_XRGB( 255,   0,   0 );			// 속성 상용화 방어구
	dwItemColor[FIRST_TC].dwTime				= D3DCOLOR_XRGB(   0,   200,   0 );			// 시간 및 효과
	dwItemColor[FIRST_TC].dwEffective0			= D3DCOLOR_XRGB(   0,   0,   0 );			// 아이템 효능
	dwItemColor[FIRST_TC].dwEffective1			= D3DCOLOR_XRGB(   0, 255,   0 );			// 아이템 효능1
	dwItemColor[FIRST_TC].dwEffective2			= D3DCOLOR_XRGB( 255,   0,   0 );			// 아이템 효능2
	dwItemColor[FIRST_TC].dwEffective3			= D3DCOLOR_XRGB( 128, 128, 128 );			// 아이템 효능3
	dwItemColor[FIRST_TC].dwRandomOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// 랜덤 옵션
	dwItemColor[FIRST_TC].dwEnchantOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// 인첸트 옵션
	dwItemColor[FIRST_TC].dwSetName				= D3DCOLOR_XRGB(   0,   0,   0 );			// 세트 이름
	dwItemColor[FIRST_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// 세트 목록(비착용시)
	dwItemColor[FIRST_TC].dwSetItem1			= D3DCOLOR_XRGB(   0,   0, 204 );			// 세트 목록(착용시)
	dwItemColor[FIRST_TC].dwSetEffect			= D3DCOLOR_XRGB( 255, 157,   0 );			// 세트 효과
	dwItemColor[FIRST_TC].dwGold				= D3DCOLOR_XRGB(   0,   0,   0 );			// 가격
	dwItemColor[FIRST_TC].dwCommand				= D3DCOLOR_XRGB(   0,   0,   0 );			// 설명
	dwItemColor[FIRST_TC].dwNotUse				= D3DCOLOR_XRGB( 255,  66,   0 );			// 사용못함
	dwItemColor[FIRST_TC].dwAddedOpt = {
		D3DCOLOR_XRGB( 127, 255,   0 ),			//얼터멋 웨폰 추가 옵션 "경험치 상승"
		D3DCOLOR_XRGB(   0, 255, 255 ),			//얼터멋 웨폰 추가 옵션 "데미지 반사"
		D3DCOLOR_XRGB( 140,  23,  23 ),			//얼터멋 웨폰 추가 옵션 "흡혈"
		D3DCOLOR_XRGB( 255, 127,   0 ),			//얼터멋 웨폰 추가 옵션 "사냥 데미지 증가"
		D3DCOLOR_XRGB( 255, 110, 199 ),			//얼터멋 웨폰 추가 옵션 "PvP 데미지 증가"
		D3DCOLOR_XRGB( 205, 127,  50 ),			//얼터멋 웨폰 추가 옵션 "힘 증가"
		D3DCOLOR_XRGB( 255,   0,   0 ),			//얼터멋 웨폰 추가 옵션 "체력 증가"
		D3DCOLOR_XRGB(  50,  50, 255 ),			//얼터멋 웨폰 추가 옵션 "민첩 증가"
		D3DCOLOR_XRGB( 147, 112, 219 )      //얼터멋 웨폰 추가 옵션 "지능 증가"
	};			
	dwItemColor[FIRST_TC].dwAwakening	= D3DCOLOR_XRGB( 0, 0, 255 );			// 아이템 각성
	dwItemColor[FIRST_TC].dwBlessing	= D3DCOLOR_XRGB( 0, 0, 255 );			// 여신의 축복
	dwItemColor[FIRST_TC].dwBlessingWarning	= D3DCOLOR_XRGB( 255, 0, 0 );			// 여신의 축복 경고
	
	memcpy( &dwItemColor[SECOND_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	memcpy( &dwItemColor[THIRD_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	dwItemColor[SECOND_TC].dwName0				= D3DCOLOR_XRGB(  12, 144, 231 );
	dwItemColor[SECOND_TC].dwResistFire			= D3DCOLOR_XRGB( 230,   0,   0 );			// 속성 불
	dwItemColor[SECOND_TC].dwResistWater		= D3DCOLOR_XRGB(   0,   0, 182 );			// 속성 물
	dwItemColor[SECOND_TC].dwResistEarth		= D3DCOLOR_XRGB( 185, 118,   0 );			// 속성 땅
	dwItemColor[SECOND_TC].dwResistElectricity	= D3DCOLOR_XRGB(  21,  55, 152 );			// 속성 전기
	dwItemColor[SECOND_TC].dwResistWind			= D3DCOLOR_XRGB(   0, 217, 214 );			// 속성 바람
	dwItemColor[SECOND_TC].dwTime				= D3DCOLOR_XRGB( 146,   0,  38 );			// 시간 및 효과
	dwItemColor[SECOND_TC].dwPlusOption			= D3DCOLOR_XRGB( 199, 155,   0 );			// 플러스 옵션
	dwItemColor[SECOND_TC].dwPiercing			= D3DCOLOR_XRGB(  77,  11, 145 );			// 피어싱
	dwItemColor[SECOND_TC].dwRandomOption		= D3DCOLOR_XRGB( 199, 155,   0 );			// 랜덤 옵션
	dwItemColor[SECOND_TC].dwSetName			= D3DCOLOR_XRGB(  50, 178,   0 );			// 세트 이름
	dwItemColor[SECOND_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// 세트 목록(비착용시)
	dwItemColor[SECOND_TC].dwSetItem1			= D3DCOLOR_XRGB( 114, 186,  85 );			// 세트 목록(착용시)
	dwItemColor[SECOND_TC].dwSetEffect			= D3DCOLOR_XRGB(  50, 178,   0 );			// 세트 효과
	dwItemColor[SECOND_TC].dwCommand			= D3DCOLOR_XRGB( 199, 155,   0 );			// 설명
	dwItemColor[SECOND_TC].dwAwakening	= D3DCOLOR_XRGB( 0, 0, 255 );			// 아이템 각성
	dwItemColor[SECOND_TC].dwBlessing	= D3DCOLOR_XRGB( 0, 0, 255 );			// 여신의 축복
	dwItemColor[SECOND_TC].dwBlessingWarning	= D3DCOLOR_XRGB( 255, 0, 0 );			// 여신의 축복 경고
}

CWndMgr::~CWndMgr() {
	for (AppletFunc *& pApplet : m_mapAppletFunc | std::views::values) {
		SAFE_DELETE(pApplet);
	}
	Free();

	SAFE_DELETE(m_pWndMenu);
}

void CWndMgr::AlighWindow( CRect rcOld, CRect rcNew )
{
	for(int i = 0; i < m_wndOrder.GetSize(); i++ )
	{
		CWndBase* pWndBase = (CWndBase*) m_wndOrder.GetAt( i );
		CRect rcWnd = pWndBase->GetWindowRect( TRUE );
		CPoint point = rcWnd.TopLeft();
		if( pWndBase != this && pWndBase != m_pWndTaskBar )
		{
			if( pWndBase->m_nWinSize == WSIZE_MAX )
			{
				pWndBase->SetWndRect( rcNew );
			}
			else
			{
				if( rcWnd.left   < rcNew.left   ) point.x = rcNew.left;
				if( rcWnd.top    < rcNew.top    ) point.y = rcNew.top;
				if( rcWnd.right  > rcNew.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom > rcNew.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				if( rcWnd.left   == rcOld.left   ) point.x = rcNew.left;
				if( rcWnd.top    == rcOld.top    ) point.y = rcNew.top;
				if( rcWnd.right  == rcOld.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom == rcOld.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				pWndBase->Move( point );
			}
		}
	}
}


void CWndMgr::Free()
{
	m_mapWndRegInfo.clear();

	SafeDeleteAll();
	
	SAFE_DELETE( m_pWndTaskBar );
	SAFE_DELETE( m_pWndActiveDesktop );
	
	m_pWndWorld = NULL;

	SAFE_DELETE( m_pWndBank );

	SAFE_DELETE( m_pWndSelectVillage );
	CString strTemp;
	
	m_mapMessage.clear();
	m_mapInstantMsg.clear();
	m_mapMap.clear();

	SAFE_DELETE( m_pQuestItemInfo );
	SAFE_DELETE( m_pWndTextBook );
	SAFE_DELETE( m_pWndTextScroll );
	SAFE_DELETE( m_pWndTextLetter );

	SAFE_DELETE(m_pWndGuildBank);

	SAFE_DELETE( m_pWndGHMain );

#ifdef __PROTECT_AWAKE
	SAFE_DELETE( m_pWndSelectAwakeCase );
#endif
}

template<typename Ptr>
void SafeDeleteWindowBase(Ptr *& ptr) {
	static_assert(std::derived_from<Ptr, CWndBase>);

	if (ptr) {
		delete ptr;
		ptr = nullptr;
	}
}

// If you have an error near here saying "hey this window is not a CWndBase"
// it means you have to include the .h with the window class definition.
template<size_t N>
void CWndMgr_WindowShortcuts_SafeDeleteAllFrom(CWndMgr_WindowShortcuts & self) {
	if constexpr (N != boost::pfr::tuple_size_v<CWndMgr_WindowShortcuts>) {
		SafeDeleteWindowBase(boost::pfr::get<N>(self));
		CWndMgr_WindowShortcuts_SafeDeleteAllFrom<N + 1>(self);
	}
}

void CWndMgr_WindowShortcuts::SafeDeleteAll() {
	CWndMgr_WindowShortcuts_SafeDeleteAllFrom<0>(*this);
}

template<size_t N>
bool CWndMgr_WindowShortcuts_DeleteChild(CWndMgr_WindowShortcuts & self, CWndBase * window) {
	if constexpr (N != boost::pfr::tuple_size_v<CWndMgr_WindowShortcuts>) {
		if (window == boost::pfr::get<N>(self)) {
			SafeDeleteWindowBase(boost::pfr::get<N>(self));
			return true;
		}

		return CWndMgr_WindowShortcuts_DeleteChild<N + 1>(self, window);
	}

	return false;
}

bool CWndMgr_WindowShortcuts::DeleteChild(CWndBase * window) {
	return CWndMgr_WindowShortcuts_DeleteChild<0>(*this, window);
}


void CWndMgr::DestroyApplet()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	POSITION pos = m_mapWndApplet.GetStartPosition();
	while( pos )
	{
		m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
		// 윈도가 종료될 때 WndRect정보를 저장하지 않게 해야한다.
		// 여기서 파괴되는 것은 오픈되어 있는 것으로 간주해야되는데,
		// 내부에서 파괴되면 크로즈로 판단하기 때문에 재실행시 윈도가 열리지 않게 된다.
		//pWndBase->SetPutRegInfo( FALSE );
		// 이제 여기서 임의로 오픈되어 있다고 저장하자.
		if( pWndBase->IsPutRegInfo() )
			PutRegInfo( pWndBase, TRUE );

		// 이제 다 끝났다. 파괴하자.
		safe_delete( pWndBase );
	}
	// 파괴할 때 인터페이스 정보가 저장된다. 따라서 파괴 이후 save하기.
	if( m_bTitle == FALSE )
		SaveRegInfo( "regInfo.dat" );
	m_mapWndApplet.RemoveAll();
}

BOOL CWndMgr::Initialize(CWndBase* pParentWnd)
{
	CWndBase::m_resMng.Load( "ResData.inc" );
	Create( WBS_NOFRAME | WBS_MANAGER | WBS_NODRAWFRAME, CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT/* - TASKBAR_HEIGHT*/ ), this, 100 );
	return TRUE;
}
void CWndMgr::OnInitialUpdate()
{
	m_timerDobe.Set( SEC( 2 ) );
#ifdef __BAN_CHATTING_SYSTEM
	m_timerWarning.Set( WARNING_MILLISECOND, TRUE );
	m_timerWarning2.Set( WARNING_2_MILLISECOND, TRUE );
	m_timerShortcutWarning.Set( SHORTCUT_WARNING_MILLISECOND, TRUE );
	m_timerBanning.Set( BANNING_MILLISECOND, TRUE );
#endif // __BAN_CHATTING_SYSTEM
	AddAllApplet();

	m_dwSavePlayerTime = 0;

	char filename[MAX_PATH];
	for( int i=0; i<128; i++ )
	{
		sprintf( filename, "Icon_CoolTime_%.3d.tga", i );
		g_pCoolTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), 0xffff00ff );
	}

	for( int i=0; i<11; i++ )
	{
		sprintf( filename, "Icon_ImgIncAni_%.2d.tga", i );
		g_pEnchantTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), 0xffffffff );
	}	
}

void CWndMgr::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CWndMgr::OnDestroyChildWnd( CWndBase* pWndChild )
{
	BOOL bFocus = TRUE;
	if( pWndChild && pWndChild->IsWndStyle( WBS_NOFOCUS ) )
		bFocus = FALSE;

	if (DeleteChild(pWndChild)) {
		pWndChild = nullptr;
	}

	if( m_pWndWorld == pWndChild )
	{
		SAFE_DELETE( m_pWndWorld );
		pWndChild = NULL;
	}
	else
	if( m_pWndBank == pWndChild )
	{
		SAFE_DELETE( m_pWndBank );
		pWndChild = NULL;
		if( m_pWndTradeGold )
			m_pWndTradeGold->Destroy();
	} else
	if( m_pWndGuildBank == pWndChild )
	{
		g_DPlay.SendCloseGuildBankWnd();
		SAFE_DELETE( m_pWndGuildBank );
		pWndChild = NULL;
		if( m_pWndTradeGold )
			m_pWndTradeGold->Destroy();
	}

//#endif
	//else
	if( pWndChild )
	{
		CWndNeuz* pWndBase;
#ifndef __IMPROVE_MAP_SYSTEM
		if( pWndChild->GetWndId() == APP_MAP_EX )
		{
			CWndMap * pWndMap = (CWndMap *)pWndChild;
			m_mapMap.erase(pWndMap->m_szMapFile);
			pWndChild = nullptr;
		}
		else
#endif // __IMPROVE_MAP_SYSTEM
		if( pWndChild->GetWndId() == APP_PARTY_CONFIRM )
		{
			SAFE_DELETE( pWndChild );
			//return;
		}
		else
		if( m_mapWndApplet.Lookup( pWndChild->GetWndId(), (void*&) pWndBase ) == TRUE )
		{
			m_mapWndApplet.RemoveKey( pWndChild->GetWndId() );
			// 애플렛으로 등록된 윈도만 파괴할 때 윈도의 정보를 저장한다. 
			if( pWndBase->IsPutRegInfo() )
				PutRegInfo( pWndBase, FALSE );
			SAFE_DELETE( pWndBase );
			pWndChild = NULL;
			//return;
		}
		else
		if( pWndChild->GetWndId() == APP_MESSAGE )
		{
			CWndMessage* pWndMessage = (CWndMessage*) pWndChild;
			const auto it = m_mapMessage.find(pWndMessage->m_strPlayer);
			if (it != m_mapMessage.end()) {
				if (it->second.get() != pWndMessage) { /* wtf??? */ }

				m_mapMessage.erase(it);
				pWndChild = nullptr;
			} else {
				// Maybe we should panic because it means we do not know the owner of pWndChild?
			}
		}
		else
		if( pWndChild->GetWndId() == APP_INSTANTMSG )
		{
			// yay code duplication is awesome.
			CWndInstantMsg* pWndMessage = (CWndInstantMsg*) pWndChild;
			const auto it = m_mapInstantMsg.find(pWndMessage->m_strPlayer);
			if (it != m_mapInstantMsg.end()) {
				if (it->second.get() != pWndMessage) { /* again? */ }

				m_mapInstantMsg.erase(it);
				pWndChild = nullptr;
			}
		}
	}
	if( m_bTitle == FALSE && bFocus )
		SetFocus();

	SAFE_DELETE( m_pQuestItemInfo );
	SAFE_DELETE( m_pWndTextBook );
	SAFE_DELETE( m_pWndTextScroll );
	SAFE_DELETE( m_pWndTextLetter );
}	
void CWndMgr::OpenTextBook( CWndBase* pWndParent , CItemElem * pItemBase )
{
	SAFE_DELETE( m_pWndTextBook );
	m_pWndTextBook = new CWndTextFromItem;
	m_pWndTextBook->Initialize( pWndParent, pItemBase, APP_TEXT_BOOK );
}
void CWndMgr::OpenTextScroll( CWndBase* pWndParent, CItemElem * pItemBase )
{
	SAFE_DELETE( m_pWndTextScroll );
	m_pWndTextScroll = new CWndTextFromItem;
	m_pWndTextScroll->Initialize( pWndParent, pItemBase, APP_TEXT_SCROLL);
}
void CWndMgr::OpenTextLetter( CWndBase* pWndParent, CItemElem * pItemBase )
{
	SAFE_DELETE( m_pWndTextLetter );
	m_pWndTextLetter = new CWndTextFromItem;
	m_pWndTextLetter->Initialize( pWndParent, pItemBase, APP_TEXT_LETTER );
}
void CWndMgr::ChangeTextBook(CItemElem * pItemBase )
{
	m_pWndTextBook->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextScroll(CItemElem * pItemBase )
{
	m_pWndTextScroll->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextLetter(CItemElem * pItemBase )
{
	m_pWndTextLetter->SetItemBase( pItemBase );
}
void CWndMgr::OpenQuestItemInfo(CWndBase* pWndParent, CItemElem * pItemBase)
{
	SAFE_DELETE(m_pQuestItemInfo);
	m_pQuestItemInfo = new CWndQuestItemInfo;
	m_pQuestItemInfo->Initialize(pWndParent, pItemBase);
}

void CWndMgr::ChangeQuestItemInfo(CItemElem * pItemBase) {
	m_pQuestItemInfo->SetItemBase(pItemBase);
}

void CWndMgr::OpenTitle( BOOL bFirstTime )
{
	if( m_bTitle == TRUE )
		return;

	// 처음 실행하는게 아니라면 게임 중 로그아웃한 것이다. 그렇다면 현재 열려 있는 필드를 삭제한다.
	if( bFirstTime == FALSE )
	{
		g_Option.Save( "neuz.ini" );
		DestroyApplet();
		Free();
		g_WorldMng.DestroyCurrentWorld();
		g_pPlayer = NULL;
		CMover::SetActiveObj( NULL );
		g_dpCertified.DeleteDPObject();
#ifdef __CERTIFIER_COLLECTING_SYSTEM
		DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		if( g_pBipedMesh )
		{
			g_pBipedMesh->DeleteDeviceObjects();
			SAFE_DELETE( g_pBipedMesh );
		}
		g_Party.InitParty();
		g_GuildWarMng.Clear();
		g_GuildMng.Clear();
		g_GuildCombatMng.GuildCombatClear( 99 );
		CPost::GetInstance()->Clear();
		CCoupleHelper::Instance()->Clear();
		CRainbowRace::GetInstance()->m_dwRemainTime = 0;
		CPlayerDataCenter::GetInstance()->Clear();
		GuildHouse->Clear();
		if( m_pWndQuestQuickInfo )
			SAFE_DELETE( m_pWndQuestQuickInfo );
		g_QuestTreeInfoManager.DeleteAllTreeInformation();
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
		prj.m_setBlockedUserID.clear();
#endif // __YS_CHATTING_BLOCKING_SYSTEM
	}
	// 데스크탑 사이즈 세팅 
	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT ) );
	m_bTitle = TRUE;

	OpenApplet(APP_LOGIN);

	// Delete 
	//CloseTitle();
	//CloseField();
	// Open
	//OpenLogin();
	// Flag set
	// 배경 음악  
	CWndBase::m_nAlpha = 255;
	m_bTitle = TRUE;
}
void CWndMgr::OpenField()
{
	m_bAutoRun = FALSE;
	SavedSoldItem::Clear(g_Neuz.m_savedInven);

	memset( g_bSlotSwitchAboutEquipItem, FALSE, sizeof( g_bSlotSwitchAboutEquipItem ) );
#ifdef __BAN_CHATTING_SYSTEM
	g_WndMng.SetWarningCounter( 0 );
	g_WndMng.SetWarning2Counter( 0 );
#endif // __BAN_CHATTING_SYSTEM
	if( m_bTitle == FALSE )
		return;
	StopMusic();
	for( int i = 0; i < MAX_CHARACTER_LIST; i++ )
	{
		if( g_Neuz.m_apPlayer[i] )
			g_Neuz.m_apPlayer[i]->m_pModel = NULL; 
		SAFE_DELETE( g_Neuz.m_apPlayer[i] );
	}
	g_DialogMsg.ClearAllMessage();
	// 데스크탑 사이즈 세팅 
	SetWndRect( g_Neuz.GetDeviceRect() );
	//jectExecutor( SHORTCUT_APPLET, APP_TASKBAR );
	// Delete 
	//CloseTitle();
	//CloseField();
	// Open
	OpenTaskBar();
	
	m_aChatString.RemoveAll();
	m_aChatColor.RemoveAll();
	m_aChatStyle.RemoveAll();

	LoadRegInfo( "regInfo.dat" );
	OpenApplet(APP_COMMUNICATION_CHAT);
	OpenApplet(APP_NAVIGATOR);
	OpenApplet(APP_STATUS1);
	OpenApplet(APP_WORLD);

	// 공지가 enable이라면?
	if( g_Option.m_bNotice )
		OpenApplet(APP_INFO_NOTICE );
	else
	// 공지가 열지 않음으로 되어있어도 날짜가 갱신되었다면 강제 오픈
	{
		CFileStatus fileStatus;
		
		CString strFilter;
		strFilter = GetLangFileName( ::GetLanguage(), FILE_NOTICE );
		
		if( CFile::GetStatus( "Client\\"+strFilter, fileStatus ) == TRUE )
		{
			if( g_Option.m_tNoticeTime != fileStatus.m_mtime.GetTime() )
			{
				g_Option.m_tNoticeTime = (time_t)( fileStatus.m_mtime.GetTime() );
				g_Option.m_bNotice = TRUE;
				OpenApplet(APP_INFO_NOTICE);
			}
		}
	}
	CWndBase::m_nAlpha = g_Option.m_nWindowAlpha;

	//GetWndBase( APP_COMMUNICATION_MESSAGE )->SetVisible( TRUE );
	//GetApplet( APP_WORLD )->OnCommand( 10001, 0 );
//	ObjectExecutor( SHORTCUT_APPLET, APP_DIALOG             );
	//OpenWnd1();
	//OpenCharacter();
	//OpenChat();
	// Flag set
	if( m_pWndQuestQuickInfo )
		SAFE_DELETE( m_pWndQuestQuickInfo );
	m_pWndQuestQuickInfo = new CWndQuestQuickInfo;
	m_pWndQuestQuickInfo->Initialize();
	m_pWndQuestQuickInfo->SetVisible( FALSE );
	m_bTitle = FALSE;
}
BOOL CWndMgr::OnCommand(UINT nID,DWORD dwMessage)
{
	return TRUE;
}
/*
CItem* CWndMgr::GetFocusItem()
{
	if(m_pWndEquipment) return m_pWndEquipment->GetFocusItem();
	return NULL;
}
*/
void CWndMgr::CloseMessageBox()
{
	//m_pWndMessageBox->Destroy();
	SAFE_DELETE( m_pWndMessageBox );
	SAFE_DELETE( m_pWndMessageBoxUpper );
}
BOOL CWndMgr::OpenCustomBox( LPCTSTR lpszMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = pWndMessageBox;
	pWndParent = this;

	if(pWndMessageBox->Initialize( pWndParent ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenMessageBox( LPCTSTR lpszMessage, UINT nType, CWndBase* pWndParent ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
	pWndParent = this;
	if( m_pWndMessageBox->Initialize( lpszMessage, pWndParent, nType ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType, CWndBase* pWndParent )
{
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
	pWndParent = this;
	if( m_pWndMessageBox->Initialize( lpszMessage, pWndParent, nType ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE;
	}
	m_pWndMessageBox->SetTitle( strTitle );
	return FALSE;
}

BOOL CWndMgr::OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType, BOOL bPostLogoutMsg ) 
{ 
	CWndBase* pWndParent = NULL;
	SAFE_DELETE( m_pWndMessageBoxUpper );
	m_pWndMessageBoxUpper = new CWndMessageBoxUpper;
	pWndParent = this;
	if( m_pWndMessageBoxUpper->Initialize( lpszMessage, pWndParent, nType, bPostLogoutMsg ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBoxUpper );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenTaskBar() 
{ 
	if(m_pWndTaskBar)
		return FALSE;
	m_pWndTaskBar = new CWndTaskBar;
	if(m_pWndTaskBar->Initialize(this) == FALSE) 
	{
		SAFE_DELETE( m_pWndTaskBar );
		return TRUE; 
	}
	return TRUE;
}
BOOL CWndMgr::OpenMenu() 
{
	if( m_pWndMenu )
	{
		if( m_pWndMenu->IsVisible() )
			m_pWndMenu->SetVisible( FALSE );
		else
		{
			m_pWndMenu->SetVisible( TRUE );
			m_pWndMenu->SetFocus();
		}
		return FALSE;
	}
	m_pWndMenu = new CWndTaskMenu;
	if( m_pWndMenu->Initialize( this ) == FALSE ) 
	{
		SAFE_DELETE( m_pWndMenu );
		return TRUE; 
	}
	return TRUE;
}
void CWndMgr::OnSize(UINT nType, int cx, int cy)
{
	//m_pWndMessageBox->OnSize(nType,cx,cy);

	//if(m_pWndTaskBar)
	//	m_pWndTaskBar->SetWndRect(CRect(0,CLIENT_HEIGHT-TASKBAR_HEIGHT,CLIENT_WIDTH,CLIENT_HEIGHT));

	//m_pWndMenu->OnSize(nType,cx,cy);
	//m_pWnd1->OnSize(nType,cx,cy);
	//m_pWndCharacter->OnSize(nType,cx,cy);
	//m_pWndChat->OnSize(nType,cx,cy);

	//m_pWndMessanger->OnSize(nType,cx,cy);
	//m_pWndMail->OnSize(nType,cx,cy);
	//m_pWndSound->OnSize(nType,cx,cy);

	CWndBase::OnSize(nType,cx,cy);
}

HRESULT CWndMgr::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.InvalidateDeviceObjects();
	m_texCommand.InvalidateDeviceObjects();
	m_texIcon.InvalidateDeviceObjects();
	m_texWnd.InvalidateDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndMgr::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.DeleteDeviceObjects();
	m_texCommand.DeleteDeviceObjects();
	m_texIcon.DeleteDeviceObjects();
	m_texWnd.DeleteDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndMgr::RestoreDeviceObjects()
{
	//if( g_WndMng.IsTitle() )
	//	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT ) );
	//else
	//	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT - TASKBAR_HEIGHT ) );
	CWndBase::RestoreDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texCommand.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texIcon.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texWnd.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	
	return 0;
}
AppletFunc * CWndMgr::GetAppletFunc(DWORD dwIdApplet) {
	return sqktd::find_in_map(m_mapAppletFunc, dwIdApplet);
}

DWORD CWndMgr::GetAppletId( TCHAR* lpszAppletName ) {
	for (const auto & [dwIdApplet, pApplet] : m_mapAppletFunc) {
		if (!_tcscmp(pApplet->m_pAppletName, lpszAppletName)) {
			return dwIdApplet;
		}
	}
	return 0xffffffff;
}
CWndBase* CWndMgr::GetApplet( DWORD dwIdApplet )
{
	CWndBase* pWndBase;
	if( m_mapWndApplet.Lookup( dwIdApplet, (void*&)pWndBase ) == FALSE )
		return NULL;
	return pWndBase;
}

void CWndMgr::__HotKeyChange(DWORD dwId, char *pch)
{
	if( g_Option.m_nInterface == 2 )
	{
		switch( dwId )
		{
		case APP_NAVIGATOR:
			*pch = 0;
			break;
		case APP_STATUS1:
			*pch = 0;
			break;
		case APP_VENDOR_REVISION:
			*pch = 0;
			break;
		case APP_QUEST_EX_LIST:
			*pch = 'L';
			break;
		case APP_MESSENGER_:
			*pch = 'T';
			break;
		}
	}	
}

void CWndMgr::AddAppletFunc(CWndNeuz * (*pAppletFunc)(), DWORD dwIdApplet, LPCTSTR lpszAppletName, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc, CHAR cHotkey )
{
	AppletFunc* pApplet = new AppletFunc;
	pApplet->m_pFunc       = pAppletFunc;
	pApplet->m_dwIdApplet  = dwIdApplet ;
	pApplet->m_pAppletName = lpszAppletName;
	pApplet->m_pAppletDesc = lpszAppletDesc;
	pApplet->m_pszIconName = pszIconName; 
#ifdef __Y_INTERFACE_VER3
	__HotKeyChange( dwIdApplet, &cHotkey );
#endif //__Y_INTERFACE_VER3
	pApplet->m_cHotkey = cHotkey; 
	m_mapAppletFunc.emplace(dwIdApplet, pApplet);
}
CWndBase* CWndMgr::CreateApplet(const DWORD dwIdApplet) {
	const auto it = m_mapAppletFunc.find(dwIdApplet);
	if (it == m_mapAppletFunc.end()) return nullptr;

	const AppletFunc * const pAppletFunc = it->second;

	CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	if (pWndGuide && pWndGuide->IsVisible()) {
		if (pWndGuide->m_CurrentGuide.m_nVicCondition == OPEN_WINDOW && pWndGuide->m_CurrentGuide.m_nInput == (int)dwIdApplet)
			pWndGuide->m_Condition.nOpenedWindowID = 1;
	}

	CWndNeuz * pWndBase = (CWndNeuz *)GetWndBase(pAppletFunc->m_dwIdApplet);
	if (pWndBase) {
		pWndBase->SetFocus();
		return pWndBase;
	}

	pWndBase = (*pAppletFunc->m_pFunc)();
	if (!pWndBase) return pWndBase;

	if (!pWndBase->Initialize(this, dwIdApplet)) {
		SAFE_DELETE(pWndBase);
		return pWndBase;
	}

	const auto itWndRegInfo = m_mapWndRegInfo.find(dwIdApplet);

	if (itWndRegInfo != m_mapWndRegInfo.end()) {
		itWndRegInfo->second.RestoreParameters(*pWndBase);
	}

	m_mapWndApplet.SetAt( dwIdApplet, pWndBase );

	return pWndBase;
}
void CWndMgr::ObjectExecutor(ShortcutType dwShortcut, DWORD dwId )
{
	SHORTCUT shortcut;
	ZeroMemory( &shortcut, sizeof( shortcut ) );
	shortcut.m_dwShortcut = dwShortcut;
	shortcut.m_dwId = dwId;
	ObjectExecutor( &shortcut);
}
void CWndMgr::ObjectExecutor( LPSHORTCUT pShortcut )
{
	if( pShortcut->m_dwShortcut == ShortcutType::Applet)
	{
		CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_DIALOG_EX );
		if( pWndBase )
			return;

		if( pShortcut->m_dwId == APP_SKILL3 )
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase )
				return;			// 스킬 확인창이면 스킬창을 닫을수가 없음.
			CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL3 );
			if( pWndBase1 && ((CWndSkillTreeEx*)pWndBase1)->GetCurrSkillPoint() != g_pPlayer->m_nSkillPoint )
			{
				if( pWndBase == NULL )
				{
					SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
					g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
					g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					g_WndMng.m_pWndReSkillWarning->InitItem( TRUE );
				}
				return;
			}
		}
		
		if( pShortcut->m_dwId == APP_QUIT)
		{
			OpenCustomBox( NULL, new CWndQuit );
		}
		else
		if( pShortcut->m_dwId == APP_LOGOUT)
		{
			// 공격중이면 로그아웃 불가
			if( g_pPlayer->IsAttackMode() )
			{
				g_WndMng.PutString( prj.GetText(TID_PK_COMBAT_END),NULL,0xffff0000);//, CHATSTY_SYSTEM_CHAT );
				return;
			}
			else
			{
				OpenCustomBox( NULL, new CWndLogOut );
			}
		}
		else if(pShortcut->m_dwId == APP_PARTY)
		{
			CWndBase* pWndParty = (CWndBase*)g_WndMng.GetApplet( APP_PARTY );
			CWndBase* pWndPartyQuick = (CWndBase*)g_WndMng.GetApplet( APP_PARTY_QUICK );
			if(pWndParty == NULL && pWndPartyQuick == NULL)
			{
				CreateApplet( APP_PARTY ); //Party창이 모두 없을 경우 APP_PARTY창 생성.
				if(g_Party.GetSizeofMember() >= 2) //PartyQuick창은 Party Member가 있을 경우만 생성.
				{
					CreateApplet( APP_PARTY_QUICK );
				}
			}
			else if(pWndParty != NULL && pWndPartyQuick != NULL) 
			{
				if(pWndParty && pWndPartyQuick)//Party창이 모두 있을 경우 PARTY창 전부 파괴.
				{
					pWndParty->Destroy();
					pWndPartyQuick->Destroy();
					SetFocus();
				}
			}
			else //Party창이 하나라도 켜져있다면 켜진 창만 파괴.
			{
				if(pWndParty != NULL)
				{
					pWndParty->Destroy();
					SetFocus();	
				}
				else if(pWndPartyQuick != NULL)
				{
					pWndPartyQuick->Destroy();
					SetFocus();	
				}
			}
		}
		else
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetApplet( pShortcut->m_dwId );
			if( pWndBase )
			{
				if(pShortcut->m_dwId == APP_INFOPANG && !pWndBase->IsVisible())
				{
					pWndBase->OnInitialUpdate();
					//pWndBase->Destroy();
					//CreateApplet( pShortcut->m_dwId );
					SetFocus();
				}
				else
				{
					pWndBase->Destroy();
					SetFocus();
				}

			}
			else if( pShortcut->m_dwId == APP_VENDOR_REVISION )
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString(TID_GAME_SUMMONANGELUSING);
				}
				else
					CreateApplet( pShortcut->m_dwId );
			}
			else if( CreateApplet( pShortcut->m_dwId ) == FALSE )
			{
			}
		}
	}
	else if( pShortcut->m_dwShortcut == ShortcutType::Chat )
	{
		{
		//	m_timerDobe.Reset();
			CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
#ifdef __BAN_CHATTING_SYSTEM
			m_bShortcutCommand = TRUE;
#endif // __BAN_CHATTING_SYSTEM
			g_WndMng.ParsingChat( pShortcut->m_szString );
		}
	}
	else if( pShortcut->m_dwShortcut == ShortcutType::Emoticon )
	{
		TCHAR szCmd[1024] = { 0 };
		_tcscat( szCmd, "/" );
		_tcscat( szCmd, g_DialogMsg.m_EmiticonCmd[pShortcut->m_dwId].m_szCommand );
		g_WndMng.ParsingChat( szCmd );
	}
	else if( pShortcut->m_dwShortcut == ShortcutType::Item  )
	{
		if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
			return;

		CItemElem * pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( IsUsableItem( pItemBase ) )
		{
			CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();

			BOOL bAble = TRUE;
			if( pItemBase )
			{
				if( pItemBase->GetProp()->dwItemKind1 == IK1_RIDE )	// 사용한 아이템이 탈것인가.
					if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// Ground상태인가?
						if( g_pPlayer->m_pActMover->m_bGround == 0 )		// 땅에 발을 안붙이고 있다면 못탄다.
							bAble = FALSE;	// 아이템사용 실패.
			}

			if( bAble )	// 성공판정 났을때만 보냄.
			{
				if( pItemBase->GetProp() && (pItemBase->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemBase->IsFlag( CItemElem::binds ) )
				{
					SAFE_DELETE( g_WndMng.m_pWndEquipBindConfirm )
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm( CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK );
					g_WndMng.m_pWndEquipBindConfirm->SetInformationDoubleClick(pItemBase, dwObjId );
					g_WndMng.m_pWndEquipBindConfirm->Initialize( NULL );
				}
				else
				{
					// 탈착 명령일 경우, nPart는 실제 장착되어 있는 부분과 일치해야 하므로 프로퍼티에서 꺼내지 않는다.
					int nPart = pItemBase->GetProp()->dwParts;
					BOOL bEquiped	= g_pPlayer->m_Inventory.IsEquip( pItemBase->m_dwObjId );
					if( bEquiped )
						nPart	= pItemBase->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;
					
					//gmpbigsun: 퀵슬롯에서 실행된 경우 비스관련 캐쉬인경우 재확인창으로.. 09_1222
					if( !CheckConfirm( pItemBase ) )		//gmpbigsun: 패킷보내기전 확인등의 처리담당
						g_DPlay.SendDoUseItem( MAKELONG( 0, pItemBase->m_dwObjId ), dwObjId, nPart );
				}
			}
		}
	}
	else if (pShortcut->m_dwShortcut == ShortcutType::Skill) {
	  if (CMover::GetActiveMover()->m_vtInfo.VendorIsVendor()) return;

		UseSkillShortCut(0, pShortcut->m_dwId);

	} else if ( pShortcut->m_dwShortcut == ShortcutType::PartySkill) {

			if( g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == 1 )
			{
				ItemProp* pItemProp =  prj.GetPartySkill( pShortcut->m_dwId );
				if( pItemProp )
				{
					if( g_Party.IsLeader(g_pPlayer->m_idPlayer) )
					{
	#ifndef __PARTYDEBUG
						if( int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 )
	#endif // __PARTYDEBUG
						{
	#ifndef __PARTYDEBUG
							if( (g_Party.GetPoint() - pItemProp->dwExp) >= 0 )
	#endif // __PARTYDEBUG
							{
								// 스킬사용
								ItemProp* pItemProp = prj.GetPartySkill( pShortcut->m_dwId );

								DWORD	dwSkillTimeTmp = 0;
								CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
								for( MAPBUFF::iterator i = pWndWorld->m_buffs.m_mapBuffs.begin(); i != pWndWorld->m_buffs.m_mapBuffs.end(); ++i )
								{
									IBuff* ptr	= i->second;
									if( ptr->GetId() == pShortcut->m_dwId )	
										dwSkillTimeTmp = ptr->GetTotal();
								}
								#define II_SYS_SYS_SCR_PARTYEXPUP01_01 20296
								#define II_SYS_SYS_SCR_PARTYSKILLUP01_01 20297
								if( dwSkillTimeTmp == 0 
									|| g_pPlayer->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01 )
									|| g_pPlayer->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02 )
									|| g_pPlayer->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01 ) )

								{
									g_DPlay.SendPartySkillUse( pShortcut->m_dwId );
									m_dwSkillTime[pShortcut->m_dwId] = g_tmCurrent;
								}
								else
								{
									/*
									#define ST_CALL                            1
									#define ST_BLITZ                           2
									#define ST_RETREAT                         3
									#define ST_SCOUT                           4
									#define ST_SPHERECIRCLE                    5
									#define ST_LINKATTACK                      6
									#define ST_FORTUNECIRCLE                   7
									#define ST_STRETCHING                      8
									#define ST_GIFTBOX                         9
									*/
									if( pShortcut->m_dwId != ST_CALL    &&
										pShortcut->m_dwId != ST_BLITZ   &&
										pShortcut->m_dwId != ST_RETREAT )
									{
										CString str;
										str.Format( prj.GetText( TID_GAME_NOT_TIME ), pItemProp->szName );
										g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_NOT_TIME ) );
									}
								}
							}
	#ifndef __PARTYDEBUG
							else g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0049) ) );
	#endif // __PARTYDEBUG
						}
	#ifndef __PARTYDEBUG
						else // 렙이 모자라 스킬을 사용할수 없음
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0019) ) );
	#endif // __PARTYDEBUG
					}
					else // 단장이 아니다
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0047) ) );
				}
			}


	}
	else if( pShortcut->m_dwShortcut == ShortcutType::Lord )
	{
		// 군주스킬을 실행한다
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		CCLord* pLord									= CCLord::Instance();
		CLordSkillComponentExecutable* pComponent		= pLord->GetSkills()->GetSkill(pShortcut->m_dwId);

		switch(pShortcut->m_dwId)
		{
			case LI_SUMMON:
			case LI_TELEPORT:
				if(pComponent->GetTick() == 0)
				{
					SAFE_DELETE(m_pWndLordSkillConfirm);
					m_pWndLordSkillConfirm = new CWndLordSkillConfirm(pShortcut->m_dwId);
					m_pWndLordSkillConfirm->Initialize();
				}
				else
				{
					PutString(TID_GAME_STILLNOTUSE);
				}
				break;

			default:
				if(!pCtrl)
				{
					g_DPlay.SendLordSkillUse(pShortcut->m_dwId, g_pPlayer->GetName());
				}
				else if(pCtrl && pCtrl->GetType() == OT_MOVER)
				{
					if(((CMover*)pCtrl)->IsPlayer())
						g_DPlay.SendLordSkillUse(pShortcut->m_dwId, ((CMover*)pCtrl)->GetName());
				}
				break;
		};
		
	}
	else if( pShortcut->m_dwShortcut == ShortcutType::Motion )
	{
		{
			CWndBase* pWndBase	= GetWndVendorBase();
			if( pWndBase ) // 상점창이 열린 상태에서는 모션 실행 금지
				return;
		}
		MotionProp* pMotionProp = prj.GetMotionProp( pShortcut->m_dwId );
		if( pMotionProp->szName[ 0 ] )
		{
			strcpy( m_szTimerChat, pMotionProp->szName );
			m_timerChat.Set( 1000 );
		}
		bool fBehavior	= false;
		if( pShortcut->m_dwId == MOT_BASE_PICKUP )
			g_pPlayer->DoPickupItemAround();
		else		
		if( pShortcut->m_dwId == MOT_BASE_BATTLE )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) ) {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_PEACE ) == 1 ) {
					fBehavior	= true;
				}
			}
			else {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_COMBAT ) == 1 ) {
					fBehavior	= true;
				}
			}
		}
		else
		// 앉기/서기 전환
		if( pShortcut->m_dwId == MOT_BASE_SIT )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_SIT ) ) {	// 앉아있는 상태일때
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_STANDUP );
				}
			}
			else {
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_SITDOWN );
				}
			}
		} 
		else
		if( pShortcut->m_dwId == MOT_BASE_COLLECT )
		{
//			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.
//			g_pPlayer->CMD_SetCollect( pFocus );
		}
		else
		if( pShortcut->m_dwId == MOT_BASE_ESCAPE )
		{
			g_DPlay.SendDoEscape();
		}
		else
		if( pShortcut->m_dwId == MOT_BASE_CHEER )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.

			if( pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer() )
			{
				g_DPlay.SendCheering( ((CMover*)pFocus)->GetId() );
				fBehavior	= false;
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_CHEER_NO2 ), NULL, prj.GetTextColor( TID_CHEER_NO2 ) );
			}
		}
		else
		if( pShortcut->m_dwId == MOT_BASE_AUTOATTACK )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.

			if(g_pPlayer->IsAttackAble(pFocus) != NULL)
			{
				if( pFocus && pFocus->GetType() == OT_MOVER )//&& ((CMover*)pFocus)->IsNPC() )플레이어에게도 쓸수있음 pk용
				{
					ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
					if( pItemProp && pItemProp->dwItemKind3 == IK3_WAND ) //Wand일 경우 AutoAttack을 하지 않음.
					{
						CCtrl* pFocusObj = (CCtrl*)pFocus;
						g_pPlayer->CMD_SetMagicAttack( pFocusObj->GetId(), 0 );
					}
					else
						((CWndWorld *)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTTARGET ), NULL, prj.GetTextColor( TID_GAME_NOTTARGET ) );
				}
			}
		}
		else if( pShortcut->m_dwId == MOT_BASE_COUPLE )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.

			if( pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer() )
			{
				// 프러포즈 보낼 메세지 창 출력
				if(g_WndMng.m_pWndCoupleMessage)
					SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);

				g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
				if(g_WndMng.m_pWndCoupleMessage)
				{
					CString strText;
					strText.Format(prj.GetText(TID_GAME_PROPOSETO), ((CMover*)pFocus)->GetName());
					g_WndMng.m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_SENDPROPOSE, pFocus);
					g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
				}
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTCOUPLETARGET ), NULL, prj.GetTextColor( TID_GAME_NOTCOUPLETARGET ) );
			}
		}
		else
		{
			// 기타 이모션.
			MotionProp *pProp = prj.GetMotionProp( pShortcut->m_dwId );
			if( pProp )
			{
				int nMotion = pProp->dwMotion;
				int nPlayMode = 0;
				switch( pProp->dwPlay )
				{
				case 0:	nPlayMode = ANILOOP_1PLAY;	break;
				case 1:	nPlayMode = ANILOOP_CONT;	break;
				case 2:	nPlayMode = ANILOOP_LOOP;	break;
				default: ANILOOP_1PLAY;
				}

				if( g_pPlayer->SendActMsg( OBJMSG_MOTION, nMotion, nPlayMode ) == 1 )
				{
					switch(nMotion)
					{
						case MTI_POINTWAYNORTH:
							g_pPlayer->SetAngle(180.0f);
							break;
						case MTI_POINTWAYSOUTH:
							g_pPlayer->SetAngle(0.0f);
							break;
						case MTI_POINTWAYEAST:
							g_pPlayer->SetAngle(90.0f);
							break;
						case MTI_POINTWAYWEST:
							g_pPlayer->SetAngle(270.0f);
							break;
					}
					fBehavior	= true;
				}
			}
		}
				
		if( fBehavior ) 
		{
			g_pPlayer->ClearDest();
		}
		if( fBehavior )
			g_DPlay.SendPlayerBehavior();
	}
	else
	if( pShortcut->m_dwShortcut == ShortcutType::Object )
	{
		CMover* CMover = prj.GetMover( pShortcut->m_dwId );
		if( CMover )
		{
			CWndWorld* pWndWorld = (CWndWorld*) GetWndBase( APP_WORLD );
			//if( g_WorldMng()->GetObjFocus() != CMover )
			//	pWndWorld->m_bNewSelect = TRUE;
			g_WndMng.OpenMessage( pShortcut->m_szString );

			//pWndMessage->AddMessage( lpString );

			//g_WorldMng()->SetObjFocus( CMover );
			

			/*
			if( pObj->GetType() == OT_MOVER )
			{
				CMover* pMover = (CMover*) pObj;
				// 자신이라면 상태창  
				if( pMover == g_Neuz.m_pPlayer )
				{
					CreateApplet( APP_EQUIPMENT );
				}
				elseaaaaaaaaaaaaaaa
				// NPC라면 대화 혹은 전투 
				if( pMover->IsNPC() )
				{
					g_World.SetObjFocus( pMover );
					CMover* pMover = (CMover*) pObj;
					g_WndMng.CreateApplet( APP_DIALOG ); 					
					
					//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
					//if( pWndWorld )
					//{
					//	if( pWndWorld->m_bBattle )
					//	{
						//}

				}
				else
				// 플레이어라면 쪽지 전송 
				{

				}
			}
			*/
		}
	}
	else
	if( pShortcut->m_dwShortcut == ShortcutType::SkillFun )
	{
		g_WndMng.m_pWndWorld->UseSkill();
	}

}

/*
BOOL CWndMgr::WndMessageBoxToTitle(CString strMessage) 
{ 
	if(m_pWndMessageBox) delete m_pWndMessageBox;
	m_pWndMessageBox = new CWndToTitleBox;
	if(m_pWndMessageBox->Initialize(strMessage,this) == FALSE) 
	{
		delete m_pWndMessageBox;
		m_pWndMessageBox = NULL;
		return TRUE; 
	}
	return FALSE;
}


*/
BOOL CWndMgr::OnEraseBkgnd( C2DRender* p2DRender )
{
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
#ifdef __GAME_GRADE_SYSTEM
	static DWORD dwTimeGameGradeScreen = g_tmCurrent + SEC( 3 );
	if( ::GetLanguage() == LANG_KOR && pWndWorld == NULL && g_tmCurrent <= dwTimeGameGradeScreen )
	{
		CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			pWndLogin->SetVisible( FALSE );
			pWndLogin->EnableWindow( FALSE );
		}

		m_pTheme->RenderGameGradeScreen( &g_Neuz.m_2DRender );
	}
	else if( pWndWorld == NULL && m_bTitle )
	{
		CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			pWndLogin->SetVisible( TRUE );
			pWndLogin->EnableWindow( TRUE );
		}

		if( m_pWndActiveDesktop )
		{
			m_pWndActiveDesktop->SetVisible( TRUE );
		}
		else
		{
			m_pTheme->RenderDesktop( &g_Neuz.m_2DRender );
		}
	}
#else // __GAME_GRADE_SYSTEM
	// 월드가 없거나 풀화면이 아닐 때
	if( pWndWorld == NULL && m_bTitle ) //|| pWndWorld->m_nWinSize != WSIZE_MAX )
	{
		if( m_pWndActiveDesktop )
			m_pWndActiveDesktop->SetVisible( TRUE );
		//if( m_bTitle == FALSE )
		else
			m_pTheme->RenderDesktop( &g_Neuz.m_2DRender );
	}
#endif // __GAME_GRADE_SYSTEM
	else
	if( pWndWorld )
	{
		//DWORD dwColor = CWorld::GetDiffuseColor();
		//m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;

		//if( pWndWorld->m_nWinSize != WSIZE_MAX )
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
		// 월드가 화면을 완전히 가리고 있을 때 
		if( m_pWndActiveDesktop )
			m_pWndActiveDesktop->SetVisible( FALSE );
	}
	else
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	return TRUE;
}
BOOL CWndMgr::Process()
{
	_PROFILE("CWndMgr::Process()");

#ifdef __MAIL_REQUESTING_BOX
	static DWORD dwTargetTime = g_tmCurrent + 5000;
	if( m_bWaitRequestMail && dwTargetTime < g_tmCurrent )		//메일이 있다고 받고, 메일정보를 받지 못한채 5초가 흐름.
	{
		g_DPlay.SendQueryMailBox();
		dwTargetTime = g_tmCurrent + 5000;
	}
#endif //__MAIL_REQUESTING_BOX

#ifdef __BS_CONSOLE
		_update_console( );
#endif

	CWndRegVend* pWndRegVend = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_SELL);
	CWndRegVend* pWndVendorBuy = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_BUY);
	
	if( pWndRegVend || pWndVendorBuy )
	{
		return CWndBase::Process();
	}
	
	if( g_pPlayer )
	{
		if( g_bKeyTable[ VK_ESCAPE ] && m_pLogOutWaitting == NULL )
		{
			//TRACE( "Push ESCAPE \n" );
			// 부활 매뉴 
			g_bKeyTable[ VK_ESCAPE ] = 0;

			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			CWorldMap* pWorldMap = CWorldMap::GetInstance();
			
			if( pWndWorld && pWndWorld->IsViewMap() && pWorldMap )
			{
				pWndWorld->SetViewMap(FALSE);
				pWorldMap->DeleteWorldMap();
				return CWndBase::Process();
			}
		
			if( g_pPlayer->IsDie() )
			{
				if( m_pWndRevival == NULL )
				{
					m_pWndRevival = new CWndRevival;
					m_pWndRevival->Initialize( this, 0 );
				}
			}
			else
			// 태스크 매뉴 
			if( m_pWndMenu == NULL || m_pWndMenu->IsVisible() == FALSE )
			{
				CWorld* pWorld = g_WorldMng();
				CObj* pObj = pWorld->GetObjFocus();

				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if(pWndWorld && pWndWorld->m_pNextTargetObj)
				{
					pWndWorld->m_pNextTargetObj = NULL;
					CMover* pMover = (CMover*)pObj;
					if(pMover->IsPeaceful() == FALSE)
						pWndWorld->m_pRenderTargetObj = pObj;
				}
				else if( pObj )
				{
					{
						DWORD	dwTmpTick = GetTickCount();
						int	nTmpSkillID = g_pPlayer->m_pActMover->GetCastingSKillID();
						if( g_pPlayer->m_pActMover->GetCastingEndTick() > dwTmpTick && ( nTmpSkillID == SI_KNT_HERO_DRAWING || nTmpSkillID == SI_RIG_HERO_RETURN ) )
						{
							g_pPlayer->m_pActMover->SetCastingEndTick(0);
							g_pPlayer->m_pActMover->ClearState();				// 상태 클리어하고 다시 맞춤.
						}

						pWorld->SetObjFocus( NULL );
						if(pWndWorld)
							pWndWorld->m_pRenderTargetObj = NULL;
						pWorld->m_idObjFocusOld = NULL_ID;
						g_pPlayer->ClearDest();
						if(pWndWorld)
							pWndWorld->m_bAutoAttack = FALSE;
						if( g_pPlayer->m_dwFlag & MVRF_TRACKING )	// 이미 실행중이면 해제.
						{
							g_pPlayer->m_dwFlag &= (~MVRF_TRACKING);		// 추적모드해제.
							g_pPlayer->m_idTracking = NULL_ID;
						}
					}
				}
				else if(pWndWorld->m_bSetQuestNPCDest)
					pWndWorld->m_bSetQuestNPCDest = FALSE;
				else
				{
					if( g_Neuz.m_pCreateMonItem )
					{
						g_Neuz.m_pCreateMonItem = NULL;
					}
					else
					{
					if( CWndBase::m_GlobalShortcut.IsEmpty() )
					{
						CWndBase* pWndBase = g_WndMng.GetWndBase();
						
						if( pWndBase )
						{
							LPWNDAPPLET lpWndApplet = pWndBase->GetWndApplet();
							if(pWndBase == &g_WndMng.m_pWndWorld->m_wndMenuMover || pWndBase->GetWndId() == 10)
								g_WndMng.m_pWndWorld->SetFocus();
							else if(lpWndApplet != NULL)
#ifdef __EVE_MINIGAME
							{
								if(lpWndApplet->dwWndId != APP_MINIGAME_KAWIBAWIBO && lpWndApplet->dwWndId != APP_MINIGAME_KAWIBAWIBO_WIN
									&& lpWndApplet->dwWndId != APP_MINIGAME_DICE && lpWndApplet->dwWndId != APP_COLLECTING
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_KAWIBAWIBO && lpWndApplet->dwWndId != APP_RR_MINIGAME_DICE
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_ARITHMETIC && lpWndApplet->dwWndId != APP_RR_MINIGAME_STOPWATCH
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_TYPING && lpWndApplet->dwWndId != APP_RR_MINIGAME_CARD
									&& lpWndApplet->dwWndId != APP_RR_MINIGAME_LADDER)

								if( pWndBase->GetWndApplet()->dwWndId != APP_GH_MAIN )		//sun!! : escape key에 반응안하는 길드메인윈도 
									pWndBase->Destroy();
							}
#else
								pWndBase->Destroy();
#endif
						}
						else
						{
							g_WndMng.OpenMenu();
							CRect rect = g_Neuz.GetDeviceRect();
							if( g_WndMng.m_pWndMenu )
								g_WndMng.m_pWndMenu->Move( CPoint( 0, rect.bottom - 48 - g_WndMng.m_pWndMenu->GetWndRect().Height() ) ); //48->TaskBar Height
						}
					}
					}
				}
			}

			if (!m_wndRemove.empty()) {
				CWndBase * pWndBase = m_wndRemove.front();

				if(pWndBase && pWndBase->GetWndApplet()->dwWndId == APP_SECRETROOM_GUILDMEMBER)
					pWndBase->Destroy(TRUE);
			}
		}
		if( m_szTimerChat[ 0 ] && m_timerChat.IsTimeOut() )
		{
			g_DPlay.SendChat( m_szTimerChat );
			m_szTimerChat[ 0 ] = 0;
			m_timerChat.Set( 0 );
		}

		if( m_bConnect && m_timerMessenger.IsTimeOut() )
			m_timerMessenger.Set( MIN( 5 ) );
		if( g_pPlayer && CSecretRoomMng::GetInstance()->m_nState >= SRCONT_WAR )
		{
			if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( g_pPlayer ) )
			{
				if(m_pWndSecretRoomQuick == NULL)
				{
					//Create GuildMemberMng Window
					int nGuildIndex = CSecretRoomMng::GetInstance()->m_pSRCont->GetTenderGuild(g_pPlayer->m_idGuild);
					if(nGuildIndex != NULL_ID)
					{
						int nGuildMemberCount = CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember.size();
						SAFE_DELETE(m_pWndSecretRoomQuick);
						m_pWndSecretRoomQuick = new CWndSecretRoomQuick;
						m_pWndSecretRoomQuick->SetGuildMemCount(nGuildMemberCount);

						for(int k=0; k<nGuildMemberCount; k++)
							m_pWndSecretRoomQuick->SetGuildMember(CSecretRoomMng::GetInstance()->m_pSRCont->m_vecSecretRoomTender[nGuildIndex].vecLineUpMember[k]);

						m_pWndSecretRoomQuick->Initialize(NULL);
					}
				}
			}
		}
		
		if( !CSecretRoomMng::GetInstance()->IsInTheSecretRoom( g_pPlayer ) )
		{
			if(m_pWndSecretRoomQuick)
				SAFE_DELETE(m_pWndSecretRoomQuick);
		}
	}


	if( m_pLogOutWaitting ) // 종료진행중이면 테스크바 슬롯체인지 불가능
		return CWndBase::Process();


	if( m_pWndTaskBar )
	{
//		if( g_bKeyTable[ VK_BACK ] )	// 지훈씨가 빼라고 했음.
//		{
//			g_bKeyTable[ VK_BACK ] = 0;
//			g_Option.m_nInterface = !g_Option.m_nInterface;
//		}
#ifdef __Y_INTERFACE_VER3
		if( g_Option.m_nInterface == 2 )
		{
			if( g_bKeyTable[ VK_F1 ] )
			{
				g_bKeyTable[ VK_F1 ] = 0;
				m_pWndTaskBar->SetItemSlot( 0 );
			}
			if( g_bKeyTable[ VK_F2 ] )
			{
				g_bKeyTable[ VK_F2 ] = 0;
				m_pWndTaskBar->SetItemSlot( 1 );
			}
			if( g_bKeyTable[ VK_F3 ] )
			{
				g_bKeyTable[ VK_F3 ] = 0;
				m_pWndTaskBar->SetItemSlot( 2 );
			}
			if( g_bKeyTable[ VK_F4 ] )
			{
				g_bKeyTable[ VK_F4 ] = 0;
				m_pWndTaskBar->SetItemSlot( 3 );
			}
			if( g_bKeyTable[ VK_F1 ] )
			{
				g_bKeyTable[ VK_F1 ] = 0;
				ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_HELP );
			}
			if( g_bKeyTable[ '0' ] )
			{
				g_bKeyTable[ '0' ] = 0;
				ScreenCapture();
			}
			if( g_bKeyTable[ VK_F9 ] )
			{
				g_bKeyTable[ VK_F9 ] = 0;
				FILEOUT( "Postion.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
				PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
			}
		}
		else
#else //__Y_INTERFACE_VER3
		{
			if( g_bKeyTable[ '1' ] )
			{
				g_bKeyTable[ '1' ] = 0;
				m_pWndTaskBar->SetItemSlot( 0 );
			}
			if( g_bKeyTable[ '2' ] )
			{
				g_bKeyTable[ '2' ] = 0;
				m_pWndTaskBar->SetItemSlot( 1 );
			}
			if( g_bKeyTable[ '3' ] )
			{
				g_bKeyTable[ '3' ] = 0;
				m_pWndTaskBar->SetItemSlot( 2 );
			}
			if( g_bKeyTable[ '4' ] )
			{
				g_bKeyTable[ '4' ] = 0;
				m_pWndTaskBar->SetItemSlot( 3 );
			}
			if( g_bKeyTable[ '5' ] )
			{
				g_bKeyTable[ '5' ] = 0;
				m_pWndTaskBar->SetItemSlot( 4 );
			}
			if( g_bKeyTable[ '6' ] )
			{
				g_bKeyTable[ '6' ] = 0;
				m_pWndTaskBar->SetItemSlot( 5 );
			}
			if( g_bKeyTable[ '7' ] )
			{
				g_bKeyTable[ '7' ] = 0;
				m_pWndTaskBar->SetItemSlot( 6 );
			}
			if( g_bKeyTable[ '8' ] )
			{
				g_bKeyTable[ '8' ] = 0;
				m_pWndTaskBar->SetItemSlot( 7 );
			}
			if( g_bKeyTable[ '1' ] )
			{
				g_bKeyTable[ '1' ] = 0;
				OpenApplet(APP_HELPER_HELP);
			}
			if( g_bKeyTable[ '0' ] )
			{
				g_bKeyTable[ '0' ] = 0;
				ScreenCapture();
			}
			if( g_bKeyTable[ '9' ] )
			{
				g_bKeyTable[ '9' ] = 0;
				FILEOUT( "Postion.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
				PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
			}
		}
#endif //__Y_INTERFACE_VER3

#ifdef __NEW_WEB_BOX
		if( GetWndBase( APP_WEBBOX ) )
#else // __NEW_WEB_BOX
		if( GetWndBase(APP_WEBBOX) || GetWndBase(APP_WEBBOX2) )
#endif // __NEW_WEB_BOX
		{
			if( !CWebBox::GetInstance()->Get_Start() )
				CWebBox::GetInstance()->Set_Start();
		}
	}
	CWndBase* pWndPartyQuick = (CWndBase*)g_WndMng.GetApplet( APP_PARTY_QUICK );
	if(pWndPartyQuick)
	{
		if(g_Party.GetSizeofMember() < 2)
		{
			pWndPartyQuick->Destroy();
			SetFocus();
		}
	}		
	return CWndBase::Process();
}
void CWndMgr::SetPlayer( CMover* pMover )
{
	CWndBase* pWndBase;

	if( pMover != NULL )
	{
		if( pWndBase = GetApplet( APP_GAUGE ) )
		{
			pWndBase->SetTitle( pMover->GetName() );
			//( (CWndStatus*)pWndBase )->m_pModel	= prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, pMover->m_dwType, pMover->m_dwIndex, TRUE );
			//prj.m_modelMng.LoadMotion( ( (CWndStatus*)pWndBase )->m_pModel, pMover->m_dwType, pMover->m_dwIndex, 0 );
		}

		if( pWndBase = GetApplet( APP_SKILL3 ) )
			( (CWndSkillTreeEx*)pWndBase )->InitItem( pMover->GetJob(), pMover->m_aJobSkill );
		
		if( pWndBase = GetApplet( APP_INVENTORY ) )
		{
			( (CWndInventory*)pWndBase )->m_dwEnchantWaitTime = 0xffffffff;	// 제련하는도중 replace하는 경우 neuz가 크래쉬 되는 현상 수정
			( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( &pMover->m_Inventory, APP_INVENTORY );
		}

		if( pWndBase = GetApplet( APP_BAG_EX ) )
			( (CWndBagEx*)pWndBase )->InitItem();

		g_Neuz.m_camera.SetPos( pMover->GetPos() );		//

		if( CMover::GetActiveMover() == NULL )
		{
			LPCTSTR szErr = Error( "SetPlayer : ActiveMover 없음" ); 
			//ADDERRORMSG( szErr );
		}
		if( CMover::GetActiveMover()->m_pActMover == NULL )
		{
			LPCTSTR szErr = Error( "SetPlayer : ActionMover 없음" ); 
			//ADDERRORMSG( szErr );
		}

		/*
		m_bLock = TRUE;
		CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
		if( !(pMoverTarget && CMover::GetActiveMover()->m_pActMover->IsFly()) ) {
			FLOAT fAngle = CMover::GetActiveMover()->GetAngle(); 
			m_fRotx=-fAngle;
		}
		*/
//		Error( "WorldName-%s, Targer-%s, ActMover-%s, ActionMover-%s\n"  )

		if( pMover->m_pActMover->IsFly() )
			g_Neuz.m_camera.Unlock();
		else
			g_Neuz.m_camera.Lock();

		// 월드가 가동하기 전에 프로젝션과 뷰 매트릭스를 만들어주어야 
		// 문제가 없다. CWndWorld는 매시지 처리기이기 때문에 미처 행렬들이
		// 세팅되기도 전에 우선된 매시지에서 행렬을 사용할 우려가 있다.(PickObject 따위들)
		//CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
		//pWndWorld->Projection( D3DDEVICE );
		g_Neuz.m_camera.Reset();
		g_Neuz.m_camera.Process( D3DDEVICE );
		g_Neuz.m_camera.Transform( D3DDEVICE, g_WorldMng.Get() );

		//gmpbigsun: g_pPlayer가 이제 막 세팅됨, 길드하우스 
		// 클라 LandScape는 실제로 ReadWorld가 실행될때 값이 채워진다. ( 서버로 패킷수신시 업데이트 됨 ), 그전에 보이는것들에 대해 문제가 발생하므로
		// g_pPlayer가 생성될때 한번 업데이트 .
		CWorld* pWorld = g_WorldMng.Get();
		if( pWorld )
		{
			pWorld->ReadWorld( g_pPlayer->GetPos() );
			GuildHouse->ApplyEFTexture( ); //! 5년묶은 때를 벗기자 
		}

	}
	else 
	{
		if( pWndBase = GetApplet( APP_SKILL3 ) )
			( (CWndSkillTreeEx*)pWndBase )->InitItem( 0, NULL );

		if( pWndBase = GetApplet( APP_GAUGE ) )
			SAFE_DELETE( ( (CWndStatus*)pWndBase )->m_pModel );

		if( pWndBase = GetApplet( APP_INVENTORY ) )
			( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( NULL, APP_INVENTORY );
	}
}
void CWndMgr::WordChange( CString& rString )
{
	CString strResult;

	CScanner s;
	s.SetProg( (LPVOID)(LPCSTR)rString );

	s.GetToken();
	BOOL bFirst	= ( *s.m_pProg == ' ' );

	while( s.tok != FINISHED )
	{
		CString str		= s.Token;
//		str.MakeLower();

		CScanner s1;
		s1.SetProg( (LPVOID)(LPCSTR)str );
		s1.GetToken();
		BOOL bSecond	= ( *s1.m_pProg == ' ' );
		BOOL bTerminate	= ( *s1.m_pProg == '\0' );

		while( s1.tok != FINISHED )
		{
			CString str1	= s1.Token;
			str1.MakeLower();

			auto it	= prj.m_mapAlphaFilter.find( (LPSTR)(LPCSTR)str1 );
			if( it != prj.m_mapAlphaFilter.end() )
			{
//				if( g_Option.m_nSlangWord == 1 ) 
				{
					TCHAR szWord[256]	= { 0,};
					memset( szWord, '*', lstrlen( str1 ) );
					strResult	+= szWord;
				}
//				else
//				{
//					strResult	+= it->second.data();
//				}
			}
			else
			{
				strResult	+= s1.Token;
			}
			if( ( bTerminate && bFirst ) ||
				( !bTerminate && bSecond ) )
				strResult	+= ' ';
//			strResult	+= ' ';
			s1.GetToken();
			bSecond	= ( *s1.m_pProg == ' ' );
			bTerminate	= ( *s1.m_pProg == '\0' );
		}
		s.GetToken();
		bFirst	= ( *s.m_pProg == ' ' );
	}

	for( auto it = prj.m_mapNonalphaFilter.begin(); it != prj.m_mapNonalphaFilter.end(); ++it )
	{
		CString str		= it->first.data();
		if( g_Option.m_nSlangWord == 1 ) 
		{
			TCHAR	szWord[256]	= { 0,};
			memset( szWord, '*', str.GetLength() );
			strResult.Replace( str, szWord );
		}
		else
		{
			strResult.Replace( str, it->second.data() );
		}
	}
	rString		= strResult;
}
// 챗에서 또는 매크로로 입력된 텍스트가 이 함수를 용도에 맞게 분류된다.
void CWndMgr::ParsingChat( CString string )
{
	CWndGuideSystem* pWndGuide = NULL;
	pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.strInput.Format("%s", string); 
	string.TrimLeft( ' ' );
	if( string.IsEmpty() == TRUE )
		return;
	// 문장길이가 너무 길면 64바이트로 줄임.
	SetStrNull( string, 120 );

	if( string.Find("#l") != -1 )
	{
		int mCut = 0;
		mCut = string.Find('/');

		if( mCut != -1 )
		{
			string = string.Right( string.GetLength() - mCut );
		}
	}

	FormalizeChatString( string );

	if( string.GetLength() > 0 )
	{
		if( string[0] == '/' )
		{
			g_textCmdFuncs.ParseCommand(string.GetString(), g_pPlayer);
		}
		else 
		{
#ifdef __BAN_CHATTING_SYSTEM
			if( m_timerBanning.IsTimeOut() == FALSE )
			{
				CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
				if( pWndChat )
				{
					int nOriginalSecond = static_cast< int >( BANNING_MILLISECOND - static_cast< int >( m_timerBanning.GetLeftTime() ) ) / 1000;
					int nMinute = static_cast< int >( nOriginalSecond / 60 );
					int nSecond = static_cast< int >( nOriginalSecond % 60 );
					CString strMessage = _T( "" );
					// 현재 채팅 금지 페널티를 받고 있습니다. (남은 시간: %d분 %d초)
					strMessage.Format( prj.GetText( TID_GAME_ERROR_CHATTING_3 ), nMinute, nSecond );
					pWndChat->PutString( strMessage, 0xffff0000 );
					return;
				}
			}
			else
			{
				if( string != m_strChatBackup || m_timerDobe.IsTimeOut() || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				{
					m_timerDobe.Reset();
					m_strChatBackup = string;
					WordChange( string );
					SetStrNull( string, 120 );
					if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
						g_DPlay.SendChat( ( LPCSTR )string );
					else
					{
						if( m_bShortcutCommand == TRUE )
						{
							if( m_timerShortcutWarning.IsTimeOut() == FALSE )
							{
								++m_nWarningCounter;
								CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
								if( pWndChat )
								{
									if( m_nWarningCounter >= BANNING_POINT )
									{
										// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
										CString strChattingError1 = _T( "" );
										strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
										pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
										m_nWarningCounter = 0;
										m_timerBanning.Reset();
									}
									else
									{
										// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
										pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
									}
								}
							}
							else
								g_DPlay.SendChat( ( LPCSTR )string );
							m_timerShortcutWarning.Reset();
						}
						else
						{
							if( m_timerWarning.IsTimeOut() == FALSE )
							{
								++m_nWarningCounter;
								CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
								if( pWndChat )
								{
									if( m_nWarningCounter >= BANNING_POINT )
									{
										// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
										CString strChattingError1 = _T( "" );
										strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
										pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
										m_nWarningCounter = 0;
										m_timerBanning.Reset();
									}
									else
									{
										// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
										pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
									}
								}
							}
							else
							{
								if( m_timerWarning2.IsTimeOut() == FALSE )
								{
									++m_nWarning2Counter;
									CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
									if( pWndChat )
									{
										if( m_nWarning2Counter >= BANNING_2_POINT )
										{
											// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
											CString strChattingError1 = _T( "" );
											strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), BANNING_MILLISECOND / 1000 / 60 );
											pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
											m_nWarning2Counter = 0;
											m_timerBanning.Reset();
										}
										else
											g_DPlay.SendChat( ( LPCSTR )string );
									}
								}
								else
									g_DPlay.SendChat( ( LPCSTR )string );
							}
							m_timerWarning.Reset();
							m_timerWarning2.Reset();
						}
					}
				}
				else
				{
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						// 연속으로 같은 내용을 입력할 수 없습니다.
						pWndChat->PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
					}
				}
			}
#else // __BAN_CHATTING_SYSTEM
			// 문자중에 /가 포함되어 있으면 영자 명령일 수 있다.
			// 영자 명령의 누수를 막기 위해 /가 포함되어 있으면 전송하지 않는다.
			if( string != m_strChatBackup || m_timerDobe.IsTimeOut() || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ))
			{
				m_timerDobe.Reset();
				m_strChatBackup = string;
				WordChange( string );
				// 문장길이가 너무 길면 64바이트로 줄임.
				SetStrNull( string, 120 );

				g_DPlay.SendChat( (LPCSTR)string );
			}
			else
			{
				CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
				if( pWndChat )
					pWndChat->PutString( prj.GetText(TID_GAME_CHATSAMETEXT), prj.GetTextColor(TID_GAME_CHATSAMETEXT) );
					//pWndChat->PutString( "연속으로 같은 내용을 입력할 수 없습니다.", 0xffffffa0 );
			}
#endif // __BAN_CHATTING_SYSTEM
#ifdef __BAN_CHATTING_SYSTEM
			m_bShortcutCommand = FALSE;
#endif // __BAN_CHATTING_SYSTEM
		}
	}
}
void CWndMgr::PutDefinedString( DWORD dwText, ... )
{
	TCHAR szBuffer[ 512 ];
	TCHAR szFormat[ 512 ];
	strcpy( szFormat, prj.GetText( dwText ) );
	va_list args;
	va_start( args, szFormat );
	int nBuf = _vsntprintf( szBuffer, 512, szFormat, args );
	va_end( args );

	DWORD dwColor	= prj.GetTextColor( dwText );
	g_WndMng.PutString( (LPCTSTR)szBuffer, NULL, dwColor );
}
/*
// Msg
m_bMsgSys
m_bMsgHelp
m_bMsg
// Chat
m_bChatGeneral
m_bChatSay
m_bChatParty
m_bChatGuild
*/
//DWORD g_Option.m_dwChatFilter = CHATSTY_GENERAL | CHATSTY_HELP | 
//	CHATSTY_SYSTEM | CHATSTY_GAME | CHATSTY_CLIENTMSG;
void CWndMgr::PutString( LPCTSTR lpszString, CObj* pObj, DWORD dwColor, DWORD dwChatStyle, DWORD dwPStyle )
{
	CMover* pMover = (CMover*)pObj;
	CString str = lpszString;
	DWORD dwStyle = 0;
//	BOOL bChat = FALSE;

	dwPStyle = PS_USE_MACRO;
	
	// Text Message Style 
	#define TMS_CLIENT   0x00000001
	#define TMS_DIALOG   0x00000002
	#define TMS_CAPTION  0x00000004
	#define TMS_CHAT     0x00000008

	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
	}
	BOOL  bChatLog = TRUE;
	DWORD dwChatFilter = g_Option.m_dwChatFilter[ nChannel ];
	if( ( dwChatFilter & CHATSTY_GENERAL       ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_WHISPER       ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_SHOUT         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_PARTY         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 
	if( ( dwChatFilter & CHATSTY_GUILD         ) == dwChatStyle ) 
		{ dwStyle |= TMS_CHAT; bChatLog = FALSE; } 

	DWORD dwChatFilterEtc = g_Option.m_dwChatFilterEtc;
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CLIENT ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CHAT   ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_HELP_CLIENT   ) && ( dwChatStyle == CHATSTY_HELP   ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_HELP_CHAT  ; }   
	if( ( dwChatFilterEtc & CHATSTY_HELP_CHAT     ) && ( dwChatStyle == CHATSTY_HELP   ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_HELP_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CLIENT   ) && ( dwChatStyle == CHATSTY_GAME   ) ) 
	{ dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_GAME_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CHAT     ) && ( dwChatStyle == CHATSTY_GAME   ) ) 
	{ dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_GAME_CHAT  ; } 
	BOOL bRecord = TRUE; // 무조건 기록. 나중에 이 변수 없애자.

	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
	CEditString* pChatString = &m_ChatString;

	//
	//  예외 상황 처리(가능상황에서 정해진것이 여기서 무효가 될 수도 있다.)
	//
	if( pObj == NULL ) // 오브젝트가 널이면 말풍선 금지 
		dwStyle &= ~TMS_DIALOG; 
	else
	if( pObj->GetType() == OT_MOVER )
	{
		if( dwChatStyle & CHATSTY_GENERAL )
			dwStyle |= TMS_DIALOG;
		if( ( (CMover*)pObj )->IsPlayer() == FALSE ) // 플래이어가 아니라면 챗 금지 
		{
			dwStyle &= ~TMS_CHAT;
			bRecord = FALSE;
		}
		if( ( (CMover*)pObj )->m_vtInfo.IsVendorOpen() ) // 개인 상점이면 말풍선 금지 
			dwStyle &= ~TMS_DIALOG;

		// chipi_080311 - 운영자인 경우 말풍선 금지
		if( ((CMover*)pObj)->m_dwAuthorization >= AUTH_GAMEMASTER && ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) )
			dwStyle &= ~TMS_DIALOG;

#ifdef __QUIZ
		if( ( (CMover*)pObj )->GetWorld() && ( (CMover*)pObj )->GetWorld()->GetID() == WI_WORLD_QUIZ && ( ( (CMover*)pObj )->m_dwMode & QUIZ_RENDER_SKIP_MODE ) )
			dwStyle &= ~TMS_DIALOG;
#endif // __QUIZ
	}		
	/*******************************************
	    출력 
	*/
	// 클라이언트 매시지 
	if( dwStyle & TMS_CLIENT )
		g_ClientMsg.AddMessage( lpszString, dwColor );

	CString strChat	= lpszString;
	strChat.Replace( "\n", " " );
	strChat.Replace( '\n', ' ' );

	// 채팅 매시지, bRecord가 TRUE이면 채팅 창에 출력할 매시지라는 의미 (반드시 현재 채널에 출력되는 것은 아님)
	if( bRecord )
	{
		// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
		if( m_aChatString.GetSize() > 500 )
		{
			m_aChatString.RemoveAt( 0, 250 );
			m_aChatColor.RemoveAt( 0, 250 );
			m_aChatStyle.RemoveAt( 0, 250 );
		}
		
		m_aChatString.Add( strChat );
//		m_aChatString.Add( lpszString );
		m_aChatColor.Add( dwColor );
		m_aChatStyle.Add( dwChatStyle );
	}
	if( dwStyle & TMS_CHAT )
	{
		if( pWndChat )
			pWndChat->PutString( strChat, dwColor, dwPStyle );
		//else
		//{
		//	m_ChatString.AddString( lpszString, dwColor );
		//	m_ChatString.AddString( "\n", dwColor );// += '\n';
		//}
	}		
	// 캡션 매시지 
	if( dwStyle & TMS_CAPTION )
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		if( ::GetLanguage() != LANG_JAP )
			g_Caption1.AddCaption( lpszString, pWndWorld->m_pFontAPITitle );
		else
			g_Caption1.AddCaption( lpszString, NULL );		
	}

	// 다이얼로그 매시지 
	if( dwStyle & TMS_DIALOG )
	{
		if( pObj->GetType() == OT_MOVER )// && ((CMover*)pObj)->m_nQuestEmoticonIndex == -1 )
		{
			TCHAR* lpszChat	= (TCHAR*)_tcschr( lpszString, _T( ':' ) );
			lpszChat += sizeof(TCHAR) * 2;	// Skip ": "
			g_DialogMsg.AddMessage( pObj, lpszChat, 0xffffffff, 0, dwPStyle );
		}
	}

	if( m_pWndChatLog )
	{
		if( bChatLog && !(dwStyle&TMS_DIALOG) && dwChatStyle != CHATSTY_SHOUT ) //외치기, NPC말풍선 내용 System Chat창에 들어오지 않게 막자.
			m_pWndChatLog->PutString( lpszString, dwColor, dwPStyle );
	}
}

void CWndMgr::PutString(const DWORD textId) {
	PutString(prj.GetText(textId), nullptr, prj.GetTextColor(textId));
}

void CWndMgr::PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive )
{
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
	{
		Error( "CWndMgr::PutToolTip_Skill : 스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	DWORD dwSkillLevel	= dwLevel;
#ifdef __SKILL0517
	LPSKILL pSkill		= g_pPlayer->GetSkill( dwSkill );
	if( pSkill )
		dwSkillLevel	= g_pPlayer->GetSkillLevel( pSkill );
#endif	// __SKILL0517

	AddSkillProp* pAddSkillProp		= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwSkillLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "CWndMgr::PutToolTip_Skill : add스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	DWORD dwaf = D3DCOLOR_XRGB( 199, 155,   0 );
	CString str;
	CString strstr;
	CString string;
	if( bActive )
		string.Format( "#b#cff2fbe6d%s   Lv %d#nb#nc", pSkillProp->szName, dwSkillLevel );
	else
		string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );
	char statebuf[30] = {0,};
	if( pAddSkillProp->nReqMp != 0xffffffff )	
	{
		sprintf( statebuf, "\nMP : %d", g_pPlayer->GetReqMp( pAddSkillProp->nReqMp ) );
		string += statebuf;
	}
	if( pAddSkillProp->nReqFp != 0xffffffff )	
	{
		sprintf( statebuf, "\nFP : %d", g_pPlayer->GetReqFp( pAddSkillProp->nReqFp ) );
		string += statebuf;
	}
	
	
//	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
//	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
//		str.Format( "\n#cffff0000%s#nc", strstr );
//	else
//		str.Format( "\n%s", strstr );
//	string += str;

	if( pSkillProp->dwReSkill1 != -1 )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill1);

		if( pskill )
		{
			strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1 );
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel1 )
				str.Format( "\n#cffff0000%s#nc", strstr );
			else
				str.Format( "\n%s#nc", strstr );
			string += str;
		}
	}
	if( pSkillProp->dwReSkill2 != -1 )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill2);
		
		if( pskill )
		{
			strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2 );
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel2 )
				str.Format( "\n#cffff0000%s#nc", strstr );
			else
				str.Format( "\n%s#nc", strstr );
			string += str;
		}
	}

	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );

	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() )
		str.Format( "\n#cffff0000%s#nc", strstr );
	else
		str.Format( "\n%s", strstr );
	string += str;

	if( strcmp( pSkillProp->szCommand, "=" ) )
	{
		string += "\n";
		string += pSkillProp->szCommand;
	}
	
	int nSkillIdx = g_pPlayer->GetSkillIdx( dwSkill );

	if( nSkillIdx != -1 )
	{
		DWORD dwDelay = g_pPlayer->GetReuseDelay( nSkillIdx );
		if( dwDelay > 0 )
		{
			CTimeSpan ct( (long)(dwDelay / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
			str.Format( prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
			string += "\n";
			string += str;
		}	
	}
	
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
void CWndMgr::PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect )
{
	ItemProp* pSkillProp = prj.GetPartySkill( dwSkill );
	
	if( pSkillProp == NULL )
		return;
	
	CString string;
	string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );

	//string = pSkillProp->szName;
	char statebuf[30] = {0,};
	if( pSkillProp->dwReqMp != 0xffffffff )	
	{
		sprintf( statebuf, "\nMP : %d", pSkillProp->dwReqMp );
		string += statebuf;
	}
	if( pSkillProp->dwReqFp != 0xffffffff )	
	{
		sprintf( statebuf, "\nFP : %d", pSkillProp->dwReqFp );
		string += statebuf;
	}
	if( strcmp( pSkillProp->szCommand, "=" ) )
	{
		CString str;
		string += "\n";
		string += pSkillProp->szCommand;
	}
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
void CWndMgr::PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag )
{
	CItemElem * pItemBase = g_pPlayer->GetItemId( dwId );
	PutToolTip_Item( pItemBase, point, pRect, flag );
}

static boost::container::flat_map<DWORD, DWORD> g_DstString = {
	{ DST_STR                     , TID_TOOLTIP_STR },
	{ DST_DEX                     , TID_TOOLTIP_DEX },
	{ DST_INT                     , TID_TOOLTIP_INT },
	{ DST_STA                     , TID_TOOLTIP_STA },
	{ DST_SPEED                   , TID_TOOLTIP_SPEED },
	{ DST_ABILITY_MIN             , TID_TOOLTIP_ABILITYMIN },
	{ DST_ABILITY_MAX             , TID_TOOLTIP_ABILITYMAX },
	{ DST_ATTACKSPEED             , TID_TOOLTIP_ATKSPEED },
	{ DST_ADJDEF                  , TID_TOOLTIP_DEFENCE },
	{ DST_RESIST_MAGIC            , TID_TOOLTIP_DEFMAGIC },
	{ DST_RESIST_ELECTRICITY      , TID_TOOLTIP_DEFELECTRICITY },
	{ DST_RESIST_ALL              , TID_TOOLTIP_DEFALLELEMETAL },
	{ DST_RESIST_FIRE             , TID_TOOLTIP_DEFFIFE },
	{ DST_RESIST_WIND             , TID_TOOLTIP_DEFWIND },
	{ DST_RESIST_WATER            , TID_TOOLTIP_DEFWATER } ,
	{ DST_RESIST_EARTH            , TID_TOOLTIP_DEFEARTH },
	{ DST_HP_MAX                  , TID_TOOLTIP_MAXHP },
	{ DST_MP_MAX                  , TID_TOOLTIP_MAXMP },
	{ DST_FP_MAX                  , TID_TOOLTIP_MAXFP },
	{ DST_HP                      , TID_TOOLTIP_HP },
	{ DST_MP                      , TID_TOOLTIP_MP },
	{ DST_FP                      , TID_TOOLTIP_FP },
	{ DST_HP_RECOVERY             , TID_TOOLTIP_HPRECOVERY },
	{ DST_MP_RECOVERY             , TID_TOOLTIP_MPRECOVERY },
	{ DST_FP_RECOVERY             , TID_TOOLTIP_FPRECOVERY },
	{ DST_HP_RECOVERY_RATE        , TID_TOOLTIP_HPRECOVERYRATE },
	{ DST_MP_RECOVERY_RATE        , TID_TOOLTIP_MPRECOVERYRATE },
	{ DST_FP_RECOVERY_RATE        , TID_TOOLTIP_FPRECOVERYRATE },
	{ DST_ALL_RECOVERY			      , TID_TOOLTIP_ALL_RECOVERY	 }	,
	{ DST_ALL_RECOVERY_RATE		    , TID_TOOLTIP_ALL_RECOVERY_RATE },
	{ DST_KILL_HP					        , TID_TOOLTIP_KILL_HP	 },
	{ DST_KILL_MP					        , TID_TOOLTIP_KILL_MP				 },
	{ DST_KILL_FP					        , TID_TOOLTIP_KILL_FP				 },
	{ DST_KILL_ALL				        , TID_TOOLTIP_KILL_ALL			 },
	{ DST_KILL_HP_RATE			      , TID_TOOLTIP_KILL_HP_RATE },
	{ DST_KILL_MP_RATE			      , TID_TOOLTIP_KILL_MP_RATE		 }	,
	{ DST_KILL_FP_RATE			      , TID_TOOLTIP_KILL_FP_RATE		 }	,
	{ DST_KILL_ALL_RATE			      , TID_TOOLTIP_KILL_ALL_RATE		 }	,
	{ DST_ALL_DEC_RATE			      , TID_TOOLTIP_ALL_DEC_RATE },
	{ DST_ADJ_HITRATE             , TID_TOOLTIP_HITRATE },
	{ DST_CHR_DMG                 , TID_TOOLTIP_CHRDMG },
	{ DST_CHRSTATE                , TID_TOOLTIP_CHRSTATE },
	{ DST_PARRY                   , TID_TOOLTIP_PARRY },
	{ DST_ATKPOWER_RATE           , TID_TOOLTIP_ATKPOWER },
	{ DST_JUMPING                 , TID_TOOLTIP_JUMPING },
	{ DST_BLOCK_MELEE				, TID_GAME_TOOLTIP_BLOCK },
	{ DST_BLOCK_RANGE				, TID_GAME_TOOLTIP_BLOCKRANGE },
	{ DST_STAT_ALLUP				, TID_GAME_TOOLTIPALLSTAT },
	{ DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE },
	{ DST_ADDMAGIC				, TID_GAME_TOOLTIPCONDITIONRATE },
	{ DST_ADJDEF_RATE				, TID_TOOLTIP_DST_ADJDEF_RATE },
	{ DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE },
	{ DST_MP_MAX_RATE				, TID_TOOLTIP_DST_MP_MAX_RATE } ,
	{ DST_FP_MAX_RATE				, TID_TOOLTIP_DST_FP_RATE },
	{ DST_CHR_CHANCECRITICAL		, TID_TOOLTIP_DST_CRITICAL_RATE },
	{ DST_CHR_WEAEATKCHANGE		, TID_TOOLTIP_DST_CHR_WEAEATKCHANGE },
	{ DST_MASTRY_EARTH			, TID_TOOLTIP_DST_MASTRY_EARTH },
	{ DST_MASTRY_FIRE				, TID_TOOLTIP_DST_MASTRY_FIRE },
	{ DST_MASTRY_WATER			, TID_TOOLTIP_DST_MASTRY_WATER },
	{ DST_MASTRY_ELECTRICITY		, TID_TOOLTIP_DST_MASTRY_ELECTRICITY },
	{ DST_MASTRY_WIND				, TID_TOOLTIP_DST_MASTRY_WIND },
	{ DST_REFLECT_DAMAGE			, TID_TOOLTIP_DST_REFLECT_DAMAGE },
	{ DST_MP_DEC_RATE				, TID_TOOLTIP_DST_MP_DEC_RATE },
	{ DST_FP_DEC_RATE				, TID_TOOLTIP_DST_FP_DEC_RATE },
	{ DST_SPELL_RATE				, TID_TOOLTIP_DST_SPELL_RATE },
	{ DST_CAST_CRITICAL_RATE		, TID_TOOLTIP_DST_CAST_CRITICAL_RATE },
	{ DST_CRITICAL_BONUS			, TID_TOOLTIP_DST_CRITICAL_BONUS },
	{ DST_YOY_DMG,				  TID_TOOLTIP_DST_YOY_DMG },
	{ DST_BOW_DMG,				  TID_TOOLTIP_DST_BOW_DMG },
	{ DST_KNUCKLE_DMG,			  TID_TOOLTIP_DST_KNUCKLE_DMG },
	{ DST_SWD_DMG,				  TID_TOOLTIP_DST_SWD_DMG },
	{ DST_AXE_DMG,				  TID_TOOLTIP_DST_AXE_DMG },
#ifdef __NEWWPN1024
	{ DST_MASTRY_ALL,				  TID_TOOLTIP_DST_MASTRY_ALL },
#endif	// __NEWWPN1024
	{ DST_ATTACKSPEED_RATE,		  TID_TOOLTIP_ATTACKSPEED_RATE },
	{ DST_CHR_STEALHP,			  TID_TOOLTIP_DST_DMG_GET },
	{ DST_PVP_DMG_RATE,				TID_TOOLTIP_DST_DMG_GET },			//gmpbigsun:js씨가 바꿔주면 고칠것
	{ DST_EXPERIENCE,					TID_TOOLTIP_DST_EXPERIENCE },
	{ DST_MELEE_STEALHP,				TID_TOOLTIP_DST_MELEE_STEALHP },
	{ DST_MONSTER_DMG,				TID_TOOLTIP_DST_MONSTER_DMG },
	{ DST_PVP_DMG,					TID_TOOLTIP_DST_PVP_DMG },
	{ DST_HEAL,	TID_TOOLTIP_DST_HEAL },
	{ DST_ATKPOWER,	TID_TOOLTIP_ATKPOWER_VALUE },
	{ DST_ONEHANDMASTER_DMG,			TID_TOOLTIP_DST_ONEHANDMASTER_DMG },
	{ DST_TWOHANDMASTER_DMG,			TID_TOOLTIP_DST_TWOHANDMASTER_DMG },
	{ DST_YOYOMASTER_DMG,				TID_TOOLTIP_DST_YOYOMASTER_DMG },
	{ DST_BOWMASTER_DMG,				TID_TOOLTIP_DST_BOWMASTER_DMG },
	{ DST_KNUCKLEMASTER_DMG,			TID_TOOLTIP_DST_KNUCKLEMASTER_DMG },
	{ DST_HAWKEYE_RATE,				TID_TOOLTIP_DST_HAWKEYE },
	{ DST_RESIST_MAGIC_RATE,			TID_TOOLTIP_DEFMAGIC_RATE },
#ifdef __DST_GIFTBOX
	{ DST_GIFTBOX,					TID_TOOLTIP_DST_GIFTBOX },
#endif // __DST_GIFTBOX
	{ DST_HPDMG_UP,					TID_TOOLTIP_DST_HPDMG_UP },
	{ DST_DEFHITRATE_DOWN,			TID_TOOLTIP_DST_DEFHITRATE_DOWN },
	{ DST_RESTPOINT_RATE,				TID_TOOLTIP_RESTPOINT_RATE },
	{ DST_CHR_RANGE,					TID_GAME_TOOLTIP_ATTACKRANGE4 },
	{ DST_STOP_MOVEMENT,				TID_GAME_TOOLTIP_MOVEMENT1 },
	{ DST_IMMUNITY,					TID_GAME_TOOLTIP_IMMUNITY1 },
};

bool IsDst_Rate(int nDstParam) {
	static constexpr std::initializer_list<int> nDstRate = {
			DST_ADJ_HITRATE,
			DST_ATKPOWER_RATE,
			DST_ADJDEF_RATE,
			DST_DEFHITRATE_DOWN,
			DST_HP_MAX_RATE,
			DST_MP_MAX_RATE,		
			DST_FP_MAX_RATE,		
			DST_HP_RECOVERY_RATE,
			DST_MP_RECOVERY_RATE,
			DST_FP_RECOVERY_RATE,
			DST_CHR_CHANCECRITICAL,
			DST_MASTRY_EARTH,
			DST_MASTRY_FIRE,
			DST_MASTRY_WATER,
			DST_MASTRY_ELECTRICITY,
			DST_MASTRY_WIND,
			DST_ATTACKSPEED,
			DST_MP_DEC_RATE,
			DST_FP_DEC_RATE,
			DST_SPELL_RATE,
			DST_CAST_CRITICAL_RATE,
			DST_CRITICAL_BONUS,
#ifdef __NEWWPN1024
			DST_MASTRY_ALL,
#endif	

			DST_ALL_RECOVERY_RATE,
			DST_KILL_HP_RATE,	
			DST_KILL_MP_RATE,	
			DST_KILL_FP_RATE,
			DST_KILL_ALL_RATE,
			DST_ALL_DEC_RATE,
			DST_BLOCK_MELEE,
			DST_BLOCK_RANGE,		
			DST_ATTACKSPEED_RATE,
			DST_CHR_STEALHP,
			DST_EXPERIENCE,
			DST_HAWKEYE_RATE,
			DST_RESIST_MAGIC_RATE,
			DST_SPEED,
			DST_REFLECT_DAMAGE,
			DST_RESTPOINT_RATE,
			DST_MONSTER_DMG,
			DST_PVP_DMG
	};
	
	return std::ranges::find(nDstRate, nDstParam) != nDstRate.end();
}

// dst 파라메터이름을 스트링으로 리턴.
// ex) nDstparam = DST_STR -> "힘"
const char * FindDstString(int nDstParam) {
	const DWORD dstTId = CWndMgr::GetDSTStringId(nDstParam);
	if (dstTId == 0) return "";
	return prj.GetText(dstTId);
}

DWORD CWndMgr::GetDSTStringId(int nDstParam) {
	return sqktd::find_in_map(g_DstString, nDstParam, 0);
}

// 아이템 툴립 출력할것
// 랜덤 옵션 이름, 아이템 이름( 길드 망토면 길드 이름 ), (세트)
// 예 ) 곰의 코튼 슈트(세트)
DWORD CWndMgr::PutItemName( CItemElem * pItemElem, const ItemProp & itemProp, CEditString* pEdit )
{
	CString strTemp;
	if (itemProp.IsUltimate()) //Ultimate 아이콘이 들어가기 위한 여백
		strTemp = "             ";

	CString str;

	static constexpr auto GetColorFromRefer1 = [&](DWORD refer1, const ToolTipItemTextColor & colorSet) {
		switch (refer1) {
			case WEAPON_GENERAL:  return colorSet.dwName0;
			case WEAPON_UNIQUE:   return colorSet.dwName1;
			case WEAPON_ULTIMATE: return colorSet.dwName3;
			case ARMOR_SET:       return colorSet.dwName1;
			default:              return colorSet.dwName0;
		}
	};

	const DWORD dwColorbuf = GetColorFromRefer1(itemProp.dwReferStat1, dwItemColor[g_Option.m_nToolTipText]);

	if( pItemElem->IsFlag( CItemElem::binds ) 
		&& (pItemElem->m_dwItemId != II_GEN_MAT_ORICHALCUM01_1 && pItemElem->m_dwItemId != II_GEN_MAT_MOONSTONE_1)
		&& itemProp.dwFlag != IP_FLAG_EQUIP_BIND
		)
		strTemp.Format( "%s ", prj.GetText( TID_GAME_TOOLTIP_ITEM_BINDS ) );

	// 랜덤 옵션
	if(itemProp.dwParts != (DWORD)-1 )
	{
		if (const auto * const pRandomOptItem = g_RandomOptItemGen.GetRandomOptItem(pItemElem->GetRandomOpt())) {
			strTemp += pRandomOptItem->pszString;
			strTemp += " ";
		}
	}

	const bool bGuildCombatCloak = (
		pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON1 || pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON2 ||
		pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON3 || pItemElem->m_dwItemId == II_ARM_S_CLO_CLO_DRAGON4
		);

	if(itemProp.dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild )	// 길드번호가 박혀있으면.
	{
		CGuild *pGuild = g_GuildMng.GetGuild( pItemElem->m_idGuild );
		if (pGuild && bGuildCombatCloak == false)
			str.Format(prj.GetText(TID_GUILD_CLOAK), pGuild->m_szGuild);
		else
			str += pItemElem->GetProp()->szName;
	}
	else
	{
		str	= pItemElem->GetName();
	}
	strTemp += str;

	pEdit->AddString(strTemp, dwColorbuf, ESSTY_BOLD );
	
	return dwColorbuf;
}

void CWndMgr::PutItemAbilityPiercing( CItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf )
{
	CString strTemp;
	if( pItemElem->GetAbilityOption() )
	{
		strTemp.Format( " %+d", pItemElem->GetAbilityOption() );
		pEdit->AddString( strTemp, dwColorBuf, ESSTY_BOLD );
	}
	
	int nCount = 0;
	for( int j = 0; j < pItemElem->GetPiercingSize(); j++ )
	{
		if( pItemElem->GetPiercingItem( j ) != 0 )
			nCount++;
	}
	if( pItemElem->GetPiercingSize() > 0 && pItemElem->m_dwItemId != II_SYS_SYS_SCR_SEALCHARACTER )
	{
		strTemp.Format( "    (%d/%d)", nCount, pItemElem->GetPiercingSize() );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
}

void CWndMgr::PutItemResist( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
	DWORD dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResist;
	str = "";
	switch( pItemElem->m_bItemResist )
	{
	case SAI79::FIRE:
		{
			str = prj.GetText(TID_UPGRADE_FIRE);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistFire;
		}					
		break;
	case SAI79::WATER:
		{
			str = prj.GetText(TID_UPGRADE_WATER);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWater;
		}					
		break;
	case SAI79::EARTH:
		{
			str = prj.GetText(TID_UPGRADE_EARTH);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistEarth;
		}
		break;
	case SAI79::ELECTRICITY:
		{
			str = prj.GetText(TID_UPGRADE_ELECTRICITY);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistElectricity;
		}					
		break;
	case SAI79::WIND:
		{
			str = prj.GetText(TID_UPGRADE_WIND);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWind;
		}					
		break;
	}
	
	if( pItemElem->GetProp()->dwItemKind1 == IK1_WEAPON  || pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( !str.IsEmpty() )
		{
			BOOL bBold = FALSE;
			strTemp.Format( "\n%s%+d", str, pItemElem->m_nResistAbilityOption );
			if( pItemElem->m_nResistSMItemId != 0 )
				bBold = TRUE;

			if( bBold )
				pEdit->AddString( strTemp, dwResistColor, ESSTY_BOLD );
			else
				pEdit->AddString( strTemp, dwResistColor );
		}
	}
	else
	{
		strTemp.Format( "\n%s Lv%d", str,  pItemElem->GetProp()->dwItemLV );
		pEdit->AddString( strTemp, dwResistColor );
	}
}

void CWndMgr::PutItemSpeed( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
	{	// 무기
		if( pItemElem->GetProp()->fAttackSpeed != 0xffffffff ) // 공격속도
		{
			pEdit->AddString( "\n" );
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKSPEED), GetATKSpeedString( pItemElem->GetProp()->fAttackSpeed ) );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
		}
	}
}

void CWndMgr::PutAwakeningBlessing( CItemElem* pItemElem, CEditString* pEdit )
{
	int nKind	= g_xRandomOptionProperty.GetRandomOptionKind( pItemElem );
	if( nKind < 0 )		// 아이템 각성, 여신의 축복 대상이 아니면,
		return;
	int nSize	= g_xRandomOptionProperty.GetRandomOptionSize( pItemElem->GetRandomOptItemId() );

#ifdef __PROTECT_AWAKE
	//각성 보호 취소된 아이템인가.. 그렇다면 줄그어진 옵션을 보여줘야 한다.
	bool bSafe = g_xRandomOptionProperty.IsCheckedSafeFlag( pItemElem->GetRandomOptItemId() );
	if( bSafe )
		nSize = g_xRandomOptionProperty.GetViewRandomOptionSize( pItemElem->GetRandomOptItemId() );

	assert( nSize >= 0 && nSize < 4 );
#endif //__PROTECT_AWAKE

	CString	str;

	BOOL bBlessing	= FALSE;
	// title
	if( nKind == CRandomOptionProperty::eAwakening )
	{
		if( nSize == 0 )
		{
			str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENING ) );	// "각성할 수 있는 아이템"
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
		}

	}
	else if( nKind == CRandomOptionProperty::eBlessing )
	{
		if( nSize > 0 )
		{
			str.Format( "\n%s", prj.GetText( TID_GAME_BLESSING_CAPTION ) );	// 축복받은 옵션
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessing );
			bBlessing	= TRUE;
		}
	}
	else if( nKind == CRandomOptionProperty::eSystemPet || nKind == CRandomOptionProperty::eEatPet )
	{	// 시스템 펫과 먹펫의 툴팁에 각성과 관련된 내용을 추가한다
		if( nSize == 0 )
		{
			str.Format( "\n\"%s\"", prj.GetText( TID_GAME_AWAKENNIG_PET_00 ) );	// "각성할 수 있는 아이템"
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening );
		}
	}

	// option
	for (const auto & dst : g_xRandomOptionProperty.GetParams(*pItemElem)) {
		str = SingleDstToString(dst);

		if( nKind == CRandomOptionProperty::eAwakening )
		{
			DWORD dwStyle = 0;
#ifdef __PROTECT_AWAKE
			dwStyle = ( bSafe ? ESSTY_STRIKETHROUGH : 0 );		//줄 그어버릴까?
#endif //__PROTECT_AWAKE
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwAwakening, dwStyle );
		}
		else
			pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessing );
	}
	if( bBlessing )
	{
		str.Format( "\n%s", prj.GetText( TID_GAME_BLESSING_WARNING ) );
		pEdit->AddString( str, dwItemColor[g_Option.m_nToolTipText].dwBlessingWarning );
	}
}

void CWndMgr::PutRandomOpt( CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwParts == (DWORD)-1 )
		return;

	const auto * const pRandomOptItem	= g_RandomOptItemGen.GetRandomOptItem(pItemElem->GetRandomOpt());
	if( pRandomOptItem ) // 2. 랜덤 옵션의 내용을 출력한다.
	{
		const CString strTemp = DstsToString(pRandomOptItem->ia);
		pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwRandomOption);
	}
}

void CWndMgr::PutPiercingOpt(const CItemElem * const pItemElem, CEditString * pEdit) {
	const auto multipleDsts = pItemElem->GetPiercingAvail();
	const CString strTemp = DstsToString(multipleDsts);
	pEdit->AddString(strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing);
}

void CWndMgr::PutEnchantOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag )
{
	CString str;
	CString strTemp;
	BOOL bPSetItem = FALSE;
	int nAbilityOption	= pMover->GetSetItemClient();
	if( pMover->IsActiveMover() )
	{
		if( flag == APP_INVENTORY && pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId) && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts )
			&& nAbilityOption > 0 )
			bPSetItem = TRUE;
	}
	else
	{
		if( flag == APP_QUERYEQUIP && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts ) && nAbilityOption > 0 )
			bPSetItem = TRUE;
	}
	if( bPSetItem ) // 출력
	{	
		PSETITEMAVAIL psa = prj.GetSetItemAvail( nAbilityOption );
		if( psa ) // DST_MAX_HITRATE
		{					
			if( psa->nHitRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_ADJ_HITRATE ), (int)psa->nHitRate );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nBlock > 0 )
			{
				str = prj.GetText( TID_GAME_TOOLTIPBLOCKRATE );
				strTemp.Format( "\n%s+%d%%", str, (int)psa->nBlock );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nMaxHitPointRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_HP_MAX_RATE ), (int)psa->nMaxHitPointRate );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nAddMagic > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_ADDMAGIC ), (int)psa->nAddMagic );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nAdded > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_STAT_ALLUP ), (int)psa->nAdded );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
		}
	}			
}

void CWndMgr::PutSetItemOpt(CMover* pMover, CItemElem* pItemElem, CEditString* pEdit) {
	// 2. 세트 아이템의 구성 요소 인가?
	const CSetItem * const pSetItem = g_SetItemFinder.GetSetItemByItemId( pItemElem->m_dwItemId );
	if (!pSetItem) return;

	// 해당 세트 아이템의 구성 요소를 모두 출력 하되 장착 된 것과 안된 것을 색으로 구별하여 출력한다.
	// 3. 세트아이템 타이틀 출력

	const int nEquiped = pMover->GetEquipedSetItemNumber(*pSetItem);

	CString strTemp;
	strTemp.Format( "\n\n%s (%d/%lu)", pSetItem->GetString(), nEquiped, pSetItem->m_components.size());
	pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetName );
		
	for (const CSetItem::PartItem & partItem : pSetItem->m_components) {
		const ItemProp * pItemProp = prj.GetItemProp(partItem.itemId);
		if (!pItemProp) continue;

		strTemp.Format("\n   %s", pItemProp->szName);

		// 장착되어있는 세트 아이템
		const DWORD color = pMover->IsEquipedPartItem(partItem)
			? dwItemColor[g_Option.m_nToolTipText].dwSetItem1
			: dwItemColor[g_Option.m_nToolTipText].dwSetItem0;

		pEdit->AddString(strTemp, color);
	}

	// 4. 추가 능력치 출력
	const ITEMAVAIL itemAvail = pSetItem->GetItemAvail(nEquiped, true);
	const CString strDsts = DstsToString(itemAvail);
	pEdit->AddString(strDsts, dwItemColor[g_Option.m_nToolTipText].dwSetEffect);
}

void CWndMgr::PutItemMinMax( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	pEdit->AddString( "\n" );
	CString strTemp;
	if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff && pItemElem->GetProp()->dwAbilityMax != 0xffffffff 
		&& pItemElem->GetProp()->dwEndurance != 0xffffffff )
	{	
		DWORD dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective3;	// 회색
		float f = pMover->GetItemMultiplier( pItemElem );
		
		int nMin	= (int)( pMover->GetItemAbilityMin( pItemElem->GetProp()->dwID ) * f );
		int nMax	= (int)( pMover->GetItemAbilityMax( pItemElem->GetProp()->dwID ) * f );
		if( 1.0f <= f )
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective0;		// 검정
		else if( 0.8f <= f)
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective1;		// 노랑
		else if( 0.6f <= f )
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective2;		// 적색
		
		if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
		{	// 공격력
			int nOpt = 0;
			if( pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( (float)( pItemElem->GetAbilityOption() ), 1.5f );

				nMin += nAdd;
				nMax += nAdd;
			}
			if( nOpt )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE1) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE2) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
		else
		{	// 방어력
			int nOpt = 0;			
			if( pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( (float)( pItemElem->GetAbilityOption() ), 1.5f );

				nMin += nAdd;
				nMax += nAdd;
			}
			if( nOpt )
			{	
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_A) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_B) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
		pEdit->AddString( strTemp, dwColorMinMax );
	}
}

void CWndMgr::PutMedicine( CItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit )
{
	CString strTemp;
	if( dwParam != 0xffffffff )
	{	//  치료량
		if( nParamVal != 0xffffffff )
		{
			if( DST_MP == dwParam )
			{ // MP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVMP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
			else if( DST_HP == dwParam )
			{ // HP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVHP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
			else if( DST_FP == dwParam )
			{ // FP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVFP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
		}

		if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff )
		{	// 최대회복량
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MAXRECOVER), pItemElem->GetProp()->dwAbilityMin );
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		}
	}
}

void CWndMgr::PutBaseItemOpt(CItemElem * pItemElem, CEditString * pEdit) {
	if (const ItemProp * itemProp = pItemElem->GetProp()) {
		boost::container::small_vector<SINGLE_DST, 3> itemParams;

		for (int i = 0; i != 3; ++i) {
			if (itemProp->dwDestParam[i] != 0xffffffff) {
				const int nDst = static_cast<int>(itemProp->dwDestParam[i]);
				const int nAdj = itemProp->nAdjParamVal[i];

				itemParams.push_back(SINGLE_DST{ nDst, nAdj });
			}
		}

		const CString str = DstsToString(itemParams);
		pEdit->AddString(str, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
	}
	
	if( pItemElem && pItemElem->IsAccessory() )		// 액세서리
	{
		const std::vector<SINGLE_DST>* pDst	= g_AccessoryProperty.GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
		const CString str = DstsToString(*pDst);
		pEdit->AddString(str, dwItemColor[g_Option.m_nToolTipText].dwGeneral);
	}
}

void CWndMgr::PutBaseResist( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->nItemResistElecricity )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEC_RES), pItemElem->GetProp()->nItemResistElecricity );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistElectricity );
	}
	if( pItemElem->GetProp()->nItemResistFire )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_FIRE_RES), pItemElem->GetProp()->nItemResistFire );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistFire );
	}
	if( pItemElem->GetProp()->nItemResistWater )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WATER_RES), pItemElem->GetProp()->nItemResistWater );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWater );
	}
	if( pItemElem->GetProp()->nItemResistWind )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WIND_RES), pItemElem->GetProp()->nItemResistWind );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWind );
	}
	if( pItemElem->GetProp()->nItemResistEarth )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_EARTH_RES), pItemElem->GetProp()->nItemResistEarth );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistEarth );
	}
}

void CWndMgr::PutItemGold( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag )
{
	CString str;
	CString strTemp;
	if( flag == APP_SHOP_ ) // 가격은 나는 안나오게 하고 상점 인터페이스에서는 나오게함 42은 상점의 Invantory 개수
	{	// 상점
		if( pItemElem->GetCost() != 0xffffffff ) 
		{	// 가격
			CWndShop* pWndShop	= (CWndShop*)GetWndBase( APP_SHOP_ );
			LPCHARACTER lpCharacter = pWndShop->m_pMover->GetCharacter();
			int nCost = 0;
			int nBeforeTax = 0;
			if(lpCharacter)
			{
				if(lpCharacter->m_vendor.m_type == CVendor::Type::Penya)
				{
					nCost = pItemElem->GetCost();
					nCost = (int)( prj.m_fShopBuyRate * nCost );
					nBeforeTax = nCost;
					if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem ) )
						nCost += ( static_cast<int>(nCost * CTax::GetInstance()->GetPurchaseTaxRate( g_pPlayer )) );
				}
				else if(lpCharacter->m_vendor.m_type == CVendor::Type::RedChip)
					nCost = pItemElem->GetChipCost();
			}
			nCost = (int)(nCost * prj.m_fShopCost );
			if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN )
				nCost = PERIN_VALUE;
			
			if( nCost <= 0 )
			{
				nCost = 1;
				nBeforeTax = 1;
			}
			
			char szbuffer[1024];
			_itoa( nCost, szbuffer, 10 );
			str = GetNumberFormatEx(szbuffer);
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_COST2), str );
			if(lpCharacter && lpCharacter->m_vendor.m_type == CVendor::Type::RedChip)
			{
				strTemp += " ";
				strTemp += prj.GetText(TID_GAME_REDCHIP);
			}
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
			if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem )
				&& lpCharacter->m_vendor.m_type == CVendor::Type::Penya
				)
			{
				_itoa( nBeforeTax, szbuffer, 10 );
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );
			}
		}
	}
	else if( flag == APP_INVENTORY )	// 인벤토리 / 뱅크 툴팁?
	{
		CWndShop* pWndShop	= (CWndShop*)GetWndBase( APP_SHOP_ );
		if( pWndShop )	// 상점을 열엇을때 파는가격도 나와야함
		{
			if( pItemElem->GetCost() != 0xffffffff )
			{	//	파는가격
				char szbuffer[1024];
				DWORD dwCostTem = 0;
				dwCostTem = pItemElem->GetCost() / 4;
#ifdef __SHOP_COST_RATE
				dwCostTem = (DWORD)( prj.m_fShopSellRate * dwCostTem );
#endif // __SHOP_COST_RATE
				DWORD dwBeforeTax = dwCostTem;
				if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem ) )
					dwCostTem -= ( static_cast<DWORD>(dwCostTem * CTax::GetInstance()->GetSalesTaxRate( g_pPlayer )) );
				
				if( dwCostTem < 1 )
				{				
					dwCostTem = 1;
					dwBeforeTax = 1;
				}

				_itoa( dwCostTem, szbuffer, 10 );
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
				if( CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem ) )
				{
					_itoa( dwBeforeTax, szbuffer, 10 );
					str = GetNumberFormatEx(szbuffer);
					strTemp.Format( prj.GetText( TID_GAME_TAX_RATE ), str );
					pEdit->AddString( "\n" );
					pEdit->AddString( strTemp, prj.GetTextColor( TID_GAME_TAX_RATE ) );	
				}
			}
		}
	}
	else if( flag == APP_VENDOR )
	{
		if( pItemElem->m_nCost != 0xffffffff )
		{	// 파는가격
			char szbuffer[1024];
			DWORD dwCostTem = 0;
			dwCostTem = pItemElem->m_nCost;
			_itoa( dwCostTem, szbuffer, 10 );
			str = GetNumberFormatEx(szbuffer);
			
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
		}
	}
}

void CWndMgr::PutToolTip_Item(CItemElem * pItemBase, CPoint point, CRect* pRect, int flag )
{
	ItemProp* pItemProp = pItemBase->GetProp();
	if( !pItemProp )		//gmpbigsun
		return;
	CEditString strItem = _T( "" );
	MakeToolTipText( pItemBase, strItem, flag );
	g_toolTip.PutToolTip( pItemBase->m_dwItemId, strItem, *pRect, point, 0 );
	if( pItemProp->IsUltimate() )
		g_toolTip.SetUltimateToolTip( pItemBase );
}

void CWndMgr::PutToolTipItemAndSubToolTip( CItemElem * pItemBase, CPoint point, CRect* pRect, int nFlag )
{
	// 다중 툴팁 시스템이 적용되면서 툴팁 출력 전에 툴팁 스위치를 미리 세팅해서 위치를 조정
	g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_MAIN );
	PutToolTipParts( pItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_MAIN );
	ItemProp* pItemProp = pItemBase->GetProp();
	switch( pItemProp->dwItemKind2 )
	{
	case IK2_WEAPON_DIRECT:
	case IK2_WEAPON_MAGIC:
	case IK2_ARMOR:
	case IK2_ARMORETC:
	case IK2_CLOTH:
	case IK2_CLOTHETC:
	case IK2_JEWELRY:
	case IK2_RIDING:
	case IK2_CLOTHWIG:
		{
			CItemElem * pEquipItemBase = g_pPlayer->GetEquipItem( pItemProp->dwParts );
			if( pEquipItemBase )
			{
				g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
				PutToolTipParts( pEquipItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
			}
			if( pItemProp->dwParts == PARTS_RWEAPON )
			{
				CItemElem * pRWeaponItemBase = g_pPlayer->GetEquipItem( PARTS_RWEAPON );
				CItemElem * pLWeaponItemBase = g_pPlayer->GetEquipItem( PARTS_LWEAPON );
				if( pRWeaponItemBase && pLWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pLWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pRWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pLWeaponItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pLWeaponItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			else if( pItemProp->dwParts == PARTS_RING1 )
			{
				CItemElem * pRing1ItemBase = g_pPlayer->GetEquipItem( PARTS_RING1 );
				CItemElem * pRing2ItemBase = g_pPlayer->GetEquipItem( PARTS_RING2 );
				if( pRing1ItemBase && pRing2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pRing2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pRing1ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRing1ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pRing2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pRing2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			else if( pItemProp->dwParts == PARTS_EARRING1 )
			{
				CItemElem * pEarring1ItemBase = g_pPlayer->GetEquipItem( PARTS_EARRING1 );
				CItemElem * pEarring2ItemBase = g_pPlayer->GetEquipItem( PARTS_EARRING2 );
				if( pEarring1ItemBase && pEarring2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB2 );
					PutToolTipParts( pEarring2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB2 );
				}
				else if( pEarring1ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pEarring1ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
				else if( pEarring2ItemBase )
				{
					g_toolTip.SetSubToolTipNumber( TOOL_TIP_SWITCH_SUB1 );
					PutToolTipParts( pEarring2ItemBase, point, pRect, nFlag, TOOL_TIP_SWITCH_SUB1 );
				}
			}
			break;
		}
	}
}

void CWndMgr::PutToolTipParts(CItemElem * pPartsItemBase, CPoint point, CRect* pRect, const int nFlag, const int nSubToolTipFlag )
{
	if (!pPartsItemBase) return;

	ItemProp* pPartsItemProp = pPartsItemBase->GetProp();
	CEditString strEquipItem = _T( "" );
	if (nSubToolTipFlag != 0)
		PutEquipItemText(strEquipItem);
	MakeToolTipText( pPartsItemBase, strEquipItem, nFlag );
	switch( nSubToolTipFlag )
	{
	case 0:
		{
			g_toolTip.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() )
				g_toolTip.SetUltimateToolTip( pPartsItemBase );
			break;
		}
	case 1:
		{
			g_toolTipSub1.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() )
				g_toolTipSub1.SetUltimateToolTip( pPartsItemBase );
			break;
		}
	case 2:
		{
			g_toolTipSub2.PutToolTipEx( pPartsItemBase->m_dwItemId, strEquipItem, *pRect, point, 0, nSubToolTipFlag );
			if( pPartsItemProp->IsUltimate() )
				g_toolTipSub2.SetUltimateToolTip( pPartsItemBase );
			break;
		}
	}
}

void CWndMgr::MakeToolTipText(CItemElem * pItemElem, CEditString& strEdit, int flag )
{
	if (!pItemElem) return;

	ItemProp * pItemProp = pItemElem->GetProp();
	if (!pItemProp) {
		Error("PutToolTip_Item : Item id %lu has no props", pItemElem->m_dwItemId);
		assert(0);
		return;
	}
	
	CMover* pMover = g_pPlayer;
	if( flag == APP_QUERYEQUIP )
	{
		CWndQueryEquip * pWndQueryEquip = (CWndQueryEquip *)g_WndMng.GetWndBase( APP_QUERYEQUIP );
		if( pWndQueryEquip )
			pMover = pWndQueryEquip->GetMover();
	}

	if( pMover == NULL )
		return;

	CString str = _T( "" );
	CString strTemp = _T( "" );
	CString strEnter = '\n';

	DWORD dwColorBuf = PutItemName( pItemElem, *pItemProp, &strEdit );
	PutItemAbilityPiercing( pItemElem, &strEdit, dwColorBuf );
	PutPetKind( *pItemElem, strEdit );		//gmpbigsun : 아이템 명 다음줄에 펫 종류 ( 리어, 픽업, 버프 ) 삽입 
	if( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND )
	{
		strEdit.AddString( "\n" );
		if(pItemElem->IsFlag( CItemElem::binds ) )
			strEdit.AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_AFTER ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_AFTER ) );
		else
			strEdit.AddString( GETTEXT( TID_TOOLTIP_EQUIPBIND_BEFORE ), GETTEXTCOLOR( TID_TOOLTIP_EQUIPBIND_BEFORE ) );
	}
	PutWeapon( *pItemProp, strEdit );
	PutSex( *pMover, *pItemProp, strEdit );
	
	switch( pItemProp->dwItemKind2 )
	{
		case IK2_WEAPON_DIRECT:
		case IK2_WEAPON_MAGIC:
		case IK2_ARMORETC:
		case IK2_CLOTHETC:
		case IK2_ARMOR:
		case IK2_CLOTH:
		case IK2_BLINKWING:
		{
			PutItemMinMax( pMover, pItemElem, &strEdit );
			PutItemSpeed( pItemElem, &strEdit );
			if( pItemProp->dwItemKind3 == IK3_ELECARD )
				PutItemResist( pItemElem, &strEdit );
			else if( pItemElem->m_nResistAbilityOption && ( pItemProp->dwItemKind1 == IK1_WEAPON  || pItemProp->dwItemKind1 == IK1_ARMOR ) )
				PutItemResist( pItemElem, &strEdit );

			PutBaseResist( pItemElem, &strEdit );	// 속성 저항력
			
			PutBaseItemOpt( pItemElem, &strEdit );
			PutRandomOpt( pItemElem, &strEdit );			
			PutEnchantOpt( pMover, pItemElem, &strEdit, flag );
			break;
		}
	case IK2_REFRESHER:
	case IK2_FOOD:
	case IK2_POTION:
		{
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam1,  pItemProp->nAdjParamVal1, &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam2,  pItemProp->nAdjParamVal2, &strEdit );
			break;
		}
	case IK2_JEWELRY:
		{
			PutBaseItemOpt( pItemElem, &strEdit );
			break;
		}
	case IK2_SYSTEM:
		{
			if( pItemProp->dwItemKind3 == IK3_VIS )
			{
				PutNeededVis( *pItemProp, strEdit );
				PutBaseItemOpt( pItemElem, &strEdit );
			}
			
			if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
			{
				PutSealChar(*pItemElem, strEdit);
			}
			break;
		}

	case IK2_GMTEXT:
		{
			if( pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE )
			{
				PutDestParam( pItemProp->dwDestParam[0], pItemProp->dwDestParam[1],
					pItemProp->nAdjParamVal[0], pItemProp->nAdjParamVal[1], strEdit );

				if( pItemProp->dwActiveSkill != NULL_ID )
				{
					ItemProp *pSkillProp;
					AddSkillProp* pAddSkillProp;
					pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
					
					if( pAddSkillProp )
					{
						PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
							pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
					}
				}
				
				
			}
			break;
		}
	default:
		{
			break;
		}
	}	// switch( pItemProp->dwItemKind2 )

	if( pItemElem->IsEatPet() )
	{
		if( pItemProp->dwActiveSkill != NULL_ID )
		{
			ItemProp *pSkillProp;
			AddSkillProp* pAddSkillProp;
			pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pItemProp->dwActiveSkill, 1, "CWndMgr::PutToolTip_Item" );
			
			if( pAddSkillProp )
			{
				PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}
	}

	
	if( pItemElem->IsVisPet( ) )
	{
		PutVisPetInfo( *pItemElem, strEdit );
	}

	PutCoolTime( *pMover, *pItemProp, strEdit );			// 쿨타임
	PutKeepTime( *pItemElem, *pItemProp, strEdit );					// 사용할수 있는 시간
	PutJob( *pMover, *pItemProp, strEdit );
	PutLevel( *pMover, *pItemElem, strEdit );	
	PutCommand( *pItemElem, strEdit );					// 용도 
	PutItemGold( pMover, pItemElem, &strEdit, flag );	// 가격
	PutSetItemOpt( pMover, pItemElem, &strEdit );
	if( pItemProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet )//&& pItemElem->m_pPet->GetLevel() != PL_EGG )
		PutPetInfo( *pItemElem, strEdit );
	if( pItemProp->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET ) //먹이 주머니 툴팁
		PutPetFeedPocket( *pItemElem, *pItemProp, strEdit );
	PutPiercingOpt( pItemElem, &strEdit );
	PutAwakeningBlessing( pItemElem, &strEdit );
	if( (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) && 
		pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
		PutAddedOpt( *pItemElem, strEdit );
}

void CWndMgr::PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect )
{
	CString string;
	char statebuf[30] = {0,};
	// 블록 상태( 블록상태면 빨간색 )
	if( g_Neuz.m_nCharacterBlock[SelectCharacter] )
//		string.Format( "#b#cff2e9200%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
		string.Format( "#b#cff0000ff%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	else
		string.Format( "#b#cffff0000%s\nConnet Cut#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	// 성별
	if(	g_Neuz.m_apPlayer[SelectCharacter]->GetSex() )
		sprintf( statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE) );
	else
		sprintf( statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXMALE) );
	string += '\n'; string += statebuf;

	// 레벨
	if(g_Neuz.m_apPlayer[SelectCharacter]->IsMaster())
	{
		sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel );
		string += '\n'; string += statebuf;
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER );		
	}
	else if(g_Neuz.m_apPlayer[SelectCharacter]->IsHero())
	{
		sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel );
		string += '\n'; string += statebuf;
		string += prj.GetText( TID_GAME_TOOLTIP_MARK_HERO );
	}
	else
	{
		sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
		string += '\n'; string += statebuf;
	}

	// Str
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_STR), g_Neuz.m_apPlayer[SelectCharacter]->m_nStr);
	string += '\n'; string += statebuf;
	
	// Sta 
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_STA), g_Neuz.m_apPlayer[SelectCharacter]->m_nSta);
	string += '\n'; string += statebuf;
	
	// Dex
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_DEX), g_Neuz.m_apPlayer[SelectCharacter]->m_nDex);
	string += '\n'; string += statebuf;
	
	// Int
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_INT), g_Neuz.m_apPlayer[SelectCharacter]->m_nInt);
	string += '\n'; string += statebuf;
				
	// 직업
	sprintf( statebuf, "%s : %s", prj.GetText(TID_APP_CHARACTER_JOB), prj.m_aJob[ g_Neuz.m_apPlayer[SelectCharacter]->m_nJob ].szName );
	string += '\n'; string += statebuf;

	CEditString strEdit;
	strEdit.SetParsingString( string );
	g_toolTip.PutToolTip( SelectCharacter, strEdit, *pRect, point, 3 );
}

// 숏컷으로 스킬 실행하기.
void CWndMgr::UseSkillShortCut( DWORD, DWORD dwSkillIdx )
{
	LPSKILL lpSkill = g_pPlayer->GetSkill( 0, dwSkillIdx );
	if( lpSkill )
	{
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		if( pCtrl && pCtrl->GetType() != OT_OBJ )
			dwObjId = pCtrl->GetId();

		{
			// 스킬바 사용중일때는 핫키로 스킬사용 안됨. / 타겟이 무버여야만함.
			if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute == 0 && (pCtrl == NULL || (pCtrl && pCtrl->GetType() == OT_MOVER)) )	
			{
//				if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute )	// 단축키로 스킬을 실행했을때 액션슬롯으로 실행상태였다면 그것을 취소시킴.
//					((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->OnCancelSkill();
				int nResult = 0;
				
				if( g_pPlayer->IsBullet( lpSkill->GetProp() ) == FALSE )
				{
					return;
				}

				nResult = g_pPlayer->CMD_SetUseSkill( dwObjId, dwSkillIdx );

				if( nResult == 0 )
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				}

				if( nResult )		// 실행할 명령을 셋팅. 이동 + 스킬사용이 합쳐진 명령.
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				} else
				// 공격중이라 스킬 사용이 씹혔으면
				if( g_pPlayer->m_pActMover->IsActAttack() )
				{
					// 다음 턴때 이 스킬을 예약시켜둠.
					g_WndMng.m_pWndWorld->SetNextSkill( dwSkillIdx );
				}				
			}
				
		}
	}
}

BOOL CWndMgr::ScreenCapture()
{
	static long	bmpcnt = 0;
	char	filename[ _MAX_PATH ] = "";
	FILE	*fp;

	{
		CreateDirectory( "Capture", NULL );
		while(1)
		{
			sprintf( filename, "Capture\\flyff%05d.jpg", bmpcnt );
			if( (fp = fopen(filename,"r")) == NULL ) break; else fclose(fp);
			bmpcnt ++;
		}
	}
	sprintf( filename, "Capture\\flyff%05d.jpg", bmpcnt );
	SaveJPG( filename );
	CString string;
	string.Format( prj.GetText(TID_ADMIN_CAPTUREJPG), bmpcnt++ );
	PutString( string, NULL, prj.GetTextColor(TID_ADMIN_CAPTUREJPG) );
	
				
	return TRUE;
}

BOOL CWndMgr::SaveJPG( LPCTSTR lpszName )
{
	IDirect3DSurface9* pDestSurface;

	CTime ctTime = CTime::GetCurrentTime();
	char lpString[260]	= { 0, };
	sprintf( lpString, prj.GetText( TID_GAME_SCREENSHOT_TIME ), ctTime.GetYear(), ctTime.GetMonth(), ctTime.GetDay(), ctTime.GetHour(), ctTime.GetMinute(), ctTime.GetSecond() );
	CRect rect = g_Neuz.GetDeviceRect();
	CSize size = m_p2DRender->m_pFont->GetTextExtent( lpString );
	g_Neuz.m_2DRender.TextOut(rect.right - size.cx - 20, rect.bottom - 70, lpString);

	m_pApp->m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
	BOOL bResult = D3DXSaveSurfaceToFile(lpszName, D3DXIFF_JPG, pDestSurface, NULL, NULL );
	pDestSurface->Release();

	return bResult;
}

BOOL CWndMgr::SaveBitmap( LPCTSTR lpszName )
{
	LPBYTE  lpData = new BYTE[ g_Option.m_nResWidth * g_Option.m_nResHeight * 4 ];
	LPDWORD lpdwData = (LPDWORD) lpData;
	memset( lpData, 255, g_Option.m_nResWidth * g_Option.m_nResHeight * 4 );
	
	IDirect3DSurface9* pDestSurface;
	m_pApp->m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );

	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_pApp->m_d3dsdBackBuffer.Width, m_pApp->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
	HRESULT hr = pDestSurface->LockRect( &lockedRect, NULL, 0 ) ;
	if( hr == D3D_OK )// Lock
	{
		LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 
		LPWORD pSrcWord = (LPWORD)lockedRect.pBits; // 16bit
		//LPDWORD pSrcDWord = (LPDWORD)lockedRect.pBits; // 32bit

		DWORD dwOffset = 0;
		DWORD dwBit = 0;
		switch( g_Neuz.m_d3dpp.BackBufferFormat )
		{
			case D3DFMT_R8G8B8: dwBit = 24; break; // 20 24-bit RGB pixel format with 8 bits per channel. 
			case D3DFMT_A8R8G8B8: dwBit = 32; break; //  21 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8R8G8B8: dwBit = 32; break; //  22 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_R5G6B5: dwBit = 16; break; //  23 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
			case D3DFMT_X1R5G5B5: dwBit = 16; break; //  24 16-bit pixel format where 5 bits are reserved for each color. 
			case D3DFMT_A1R5G5B5: dwBit = 16; break; //  25 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
			case D3DFMT_A4R4G4B4: dwBit = 16; break; //  26 16-bit ARGB pixel format with 4 bits for each channel. 
			case D3DFMT_R3G3B2: dwBit = 8; break; //  27 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
			case D3DFMT_A8: dwBit = 8; break; //  28 8-bit alpha only. 
			case D3DFMT_A8R3G3B2: dwBit = 16; break; //  29 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
			case D3DFMT_X4R4G4B4: dwBit = 16; break; //  30 16-bit RGB pixel format using 4 bits for each color. 
			case D3DFMT_A2B10G10R10: dwBit = 32; break; //  31 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
			case D3DFMT_A8B8G8R8: dwBit = 32; break; //  32 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8B8G8R8: dwBit = 32; break; //  33 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_G16R16: dwBit = 32; break; //  34 32-bit pixel format using 16 bits each for green and red. 
			case D3DFMT_A2R10G10B10: dwBit = 32; break; //  35 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
			case D3DFMT_A16B16G16R16: dwBit = 64; break; //  36 64-bit pixel format using 16 bits for each component. 
			case D3DFMT_A8P8: dwBit = 8; break; //  40 8-bit color indexed with 8 bits of alpha. 
			case D3DFMT_P8: dwBit = 8; break; //  41 8-bit color indexed. 
			case D3DFMT_L8: dwBit = 8; break; //  50 8-bit luminance only. 
			case D3DFMT_L16: dwBit = 16; break; //  81 16-bit luminance only. 
			case D3DFMT_A8L8: dwBit = 16; break; //  51 16-bit using 8 bits each for alpha and luminance. 
			case D3DFMT_A4L4: dwBit = 8; break; //  8-bit using 4 bits each for alpha and luminance.
		}	
		if( dwBit == 16 || dwBit == 32 )
		{
			int nPitch = lockedRect.Pitch;
			if( dwBit == 32 )
			{
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						// 32 bit color code
						BYTE byData3 = pSrcByte[ y * nPitch + ( x * 4 ) + 0 ];
						BYTE byData2 = pSrcByte[ y * nPitch + ( x * 4 ) + 1 ];
						BYTE byData1 = pSrcByte[ y * nPitch + ( x * 4 ) + 2 ];
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
			else
			{
				nPitch /= 2;
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						WORD wData = pSrcWord[ y * nPitch + x ];
							
						BYTE byData1 = ( wData & 0xf800 ) >> 8;
						BYTE byData2 = ( wData & 0x07e0 ) >> 3;
						BYTE byData3 = ( wData & 0x001f ) << 3 ;
						
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
		}
		pDestSurface->UnlockRect();
	}
	
	pDestSurface->Release();
	BOOL bResult = SaveBMP( lpszName, lpData,CSize( g_Option.m_nResWidth, g_Option.m_nResHeight ) );
	SAFE_DELETE_ARRAY( lpData );
	return bResult;
}
CWndMap* CWndMgr::OpenMap(LPCTSTR lpszMapFileName) {
	const auto it = m_mapMap.find(lpszMapFileName);
	if (it != m_mapMap.end()) return it->second.get();

	CWndMap * pWndMap = new CWndMap;
	strcpy( pWndMap->m_szMapFile, lpszMapFileName );
	pWndMap->Initialize();
	m_mapMap.emplace(lpszMapFileName, pWndMap);

	return pWndMap;
}

CCollectingWnd* CWndMgr::OpenCollecting()
{

	if(!m_pWndCollecting)
	{
		m_pWndCollecting = new CCollectingWnd;
		m_pWndCollecting->Initialize();
	}

	return m_pWndCollecting;
}


BOOL CWndMgr::CloseCollecting()
{

	if(m_pWndCollecting)
	{
		m_pWndCollecting->Destroy();
		SAFE_DELETE(m_pWndCollecting);
		m_pWndCollecting = NULL;
		return TRUE;
	}

	return FALSE;
}


CWndMessage* CWndMgr::GetMessage( LPCTSTR lpszFrom ) {
	return sqktd::find_in_unique_map(m_mapMessage, lpszFrom);
}
CWndMessage* CWndMgr::OpenMessage( LPCTSTR lpszFrom ) {
	CWndMessage * pWndMessage = GetMessage(lpszFrom);
	if (pWndMessage) {
		pWndMessage->InitSize();
		return pWndMessage;
	}

	pWndMessage = new CWndMessage;
	pWndMessage->Initialize();
	m_mapMessage.emplace( lpszFrom, pWndMessage );
	const CString string = pWndMessage->GetTitle() + " - " + lpszFrom;
	pWndMessage->SetTitle( string );
	pWndMessage->m_strPlayer = lpszFrom;

	return pWndMessage;
}

BOOL CWndMgr::UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew ) {
	const auto it = m_mapMessage.find(pszOld);
	if (it == m_mapMessage.end()) return FALSE;

	CWndMessage * pWndMessage = it->second.release(); // pWndMessage owns the pointer [1]
	m_mapMessage.erase(it); // m_mapMessage does not own the pointer so the window is not deleted
	m_mapMessage.emplace(pszNew, pWndMessage); // m_mapMessage owns the pointer again

	// [1] Because we did not use std::unique_ptr<CWndMessage> pWndMessage = std::move(it->second)
	// at this line code, pWndMessage is still an observing pointer to the window

	CString string	= pWndMessage->GetTitle();
	string.Replace( pszOld, pszNew );
	pWndMessage->SetTitle( string );
	pWndMessage->m_strPlayer	= pszNew;
	return TRUE;
}

CWndInstantMsg* CWndMgr::GetInstantMsg( LPCTSTR lpszFrom ) {
	return sqktd::find_in_unique_map(m_mapInstantMsg, lpszFrom);
}

CWndInstantMsg* CWndMgr::OpenInstantMsg( LPCTSTR lpszFrom )
{
	CWndInstantMsg * alreadyHere = GetInstantMsg(lpszFrom);
	if (alreadyHere) return alreadyHere;

	CWndBase* pWndFocus = GetFocusWnd();
	CWndInstantMsg * pWndMessage = new CWndInstantMsg;
	pWndMessage->Initialize();
	pWndFocus->SetFocus();
	pWndMessage->m_strPlayer = lpszFrom;
	const int nNumber = static_cast<int>(m_mapInstantMsg.size());
	const CRect rcMsg = pWndMessage->GetWindowRect();
	const CRect rcWnd = GetLayoutRect();

	const int nNumHeight = rcWnd.Height() / rcMsg.Height();

	const int nCount = ( nNumber / nNumHeight ) + 1; // +1안해주면 화면을 벗어난 곳에 열림 
	const int nCount2 = ( nNumber % nNumHeight ) + 1;

	const CPoint pt( rcWnd.right - ( rcMsg.Width() * nCount ) , rcWnd.bottom - ( rcMsg.Height() * nCount2 ) );
 	pWndMessage->Move( pt );
	m_mapInstantMsg.emplace( lpszFrom, pWndMessage );

	return pWndMessage;
}

// 컬런트 윈도가 닫힐 때 호출 됨 
void CWndMgr::PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen ) {
	m_mapWndRegInfo.insert_or_assign(pWndNeuz->GetWndId(), WNDREGINFO(*pWndNeuz, bOpen));
}

BOOL CWndMgr::SaveRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	if( file.Open( lpszFileName, "wb" ) == FALSE )
		return FALSE;

	file.PutDW( REG_VERSION );
	file.PutDW( m_mapWndRegInfo.size() );

	for (const auto & pWndRegInfo : m_mapWndRegInfo | std::views::values) {
		pWndRegInfo.StoreIn(file);
	}
	file.Close();
	// resolution 파일 만들기. 현재 사이즈로 저장
	if( file.Open( "resolution", "wb" ) == FALSE )
		return FALSE;
	file.PutDW( g_Option.m_nResWidth );
	file.PutDW( g_Option.m_nResHeight );
	file.Close();
	return TRUE;
}
BOOL CWndMgr::LoadRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	BOOL bResult = FALSE;
	if( file.Open( "resolution", "rb" ) )
	{
		int nWidth = file.GetDW();
		int nHeight = file.GetDW();
		file.Close();
		// 해상도가 같으면 변경이 없는 상태다. 레지스트 파일 읽기 
		if( g_Option.m_nResWidth == nWidth && g_Option.m_nResHeight == nHeight )
		{
			bResult = TRUE;
			// 사이즈 저장 파일 로드 
			if( file.Open( lpszFileName, "rb" ) == FALSE )
				return FALSE;
			DWORD dwRegVersion = file.GetDW();
			if( dwRegVersion != REG_VERSION )
				return FALSE;

			int nNum = file.GetDW();
			for (int i = 0; i < nNum; i++) {
				WNDREGINFO wndRegInfo(file);
				m_mapWndRegInfo.insert_or_assign(wndRegInfo.GetWndId(), std::move(wndRegInfo));
			}

			file.Close();
		}
	}
	return bResult;
}

void CWndMgr::SetMessengerAutoState()
{
#ifdef __RT_1025
	if( m_bConnect && m_RTMessenger.GetState() == FRS_AUTOABSENT )
#else	// __RT_1025
	if( m_bConnect && m_Messenger.GetState() == FRS_AUTOABSENT )
#endif	// __RT_1025
	{
		g_DPlay.SendSetState( FRS_ONLINE );	
		m_timerMessenger.Set( MIN( 5 ) );
	}
}

void CWndMgr::PutDestParam( DWORD dwDst1, DWORD dwDst2, DWORD dwAdj1, DWORD dwAdj2, CEditString &str )
{
	if( dwDst1 != NULL_ID && dwDst1 != 0 && dwDst1 != DST_CHRSTATE )
	{
		int nDst = dwDst1;
		int nAdj = dwAdj1;

		const CString strTemp = SingleDstToString(SINGLE_DST{ nDst, nAdj });
		str.AddString(strTemp, D3DCOLOR_XRGB( 0, 0, 255 ) );
	}

	if( dwDst2 != NULL_ID && dwDst2 != 0 && dwDst2 != DST_CHRSTATE )
	{
		int nDst = dwDst2;
		int nAdj = dwAdj2;

		const CString strTemp = SingleDstToString(SINGLE_DST{ nDst, nAdj });
		str.AddString(strTemp, D3DCOLOR_XRGB(0, 0, 255));
	}	
}

BOOL IsEquipedVis( CItemElem* pPetItem, DWORD visIndex );

BOOL CWndMgr::CheckConfirm(CItemElem * pItem )
{
	ItemProp* pItemProp = pItem->GetProp();
	if( !pItemProp )
		return FALSE;

	if( g_pPlayer->HasActivatedVisPet( ) )
	{	
		// 다음은 버프펫이 활성화 된 상태에서만 불려야 한다.
		if( pItemProp->IsVis( ) )
		{
			// gmpbigsun : 비스를 장착하기전 확인창 
			if( !g_WndMng.m_pWndBuffPetStatus )
			{
				g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
				g_WndMng.m_pWndBuffPetStatus->Initialize();	
			}
				
			g_WndMng.m_pWndBuffPetStatus->DoModal_ConfirmQuestion( pItem->m_dwObjId, g_pPlayer->GetId(), pItemProp->dwID );
			return TRUE;
		}

		if( pItemProp->IsVisKey( ) )
		{
			if( !g_WndMng.m_pWndBuffPetStatus )
			{
				g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
				g_WndMng.m_pWndBuffPetStatus->Initialize();	
			}
				
			g_WndMng.m_pWndBuffPetStatus->DoModal_ConfirmQuestion( pItem->m_dwObjId, g_pPlayer->GetId(), pItemProp->dwID, 0, CWndConfirmVis::CVS_EQUIP_VISKEY );
			return TRUE;
		}
	}

	if( pItemProp->IsPickupToBuff( ) )
	{
		// 버프펫으로 변환시키는 아이템 처리 
		// gmpbigsun : 픽업펫은 클라에서 정보를 가지고 있지 않다. 고로 사용시 확인창만 띄워주고 나머진 서버에서..
		SAFE_DELETE( g_WndMng.m_pWndConfirmVis );

		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;

		pWnd->m_dwItemId = pItem->m_dwObjId;
		pWnd->m_objid = g_pPlayer->GetId();
		pWnd->m_dwItemIndex = pItemProp->dwID;
		
		pWnd->m_eSection = CWndConfirmVis::CVS_PICKUP_TO_BUFF;		
		pWnd->Initialize();
	
		return TRUE;
	}

	//gmpbigsun : 
	//1.각성보호 확인창 으로 넘김 
	//2.확인창에서 OK시 패킷쏴줌

#ifdef __PROTECT_AWAKE
	if( II_SYS_SYS_SCR_AWAKESAFE == pItem->m_dwItemId )
	{
		//각성보호 두루마리
		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;

		pWnd->m_dwItemId = pItem->m_dwObjId;
		pWnd->m_objid = g_pPlayer->GetId();
		pWnd->m_dwItemIndex = pItemProp->dwID;
		
		pWnd->m_eSection = CWndConfirmVis::ETC_PROTECT_AWAKE;		
		pWnd->Initialize();

		return TRUE;
	}
#endif //__PROTECT_AWAKE

	return FALSE;
}


void CWndMgr::ClearAllWnd()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	if(!m_clearFlag)
	{
		POSITION pos = m_mapWndApplet.GetStartPosition();
		while( pos )
		{
			m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
			if( pWndBase && pWndBase->GetWndId() != APP_WORLD)
			{
				m_tempWndId.push_back(pWndBase->GetWndId());
				pWndBase->SetVisible(FALSE);
			}
		}
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
		if(pWndChat != NULL && pWndChat->m_bChatLog)
			m_pWndChatLog->SetVisible(FALSE);

		CWndPetStatus* pWndPetStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
		if(pWndPetStatus != NULL)
			pWndPetStatus->SetVisible(FALSE);

		CWndBuffPetStatus* pWndBuffPetStatus = (CWndBuffPetStatus*)g_WndMng.GetWndBase( APP_BUFFPET_STATUS );
		if(pWndBuffPetStatus != NULL)
			pWndBuffPetStatus->SetVisible(FALSE);

		CWndGHMainMenu* pWndGHMain = ( CWndGHMainMenu* )g_WndMng.GetWndBase( APP_GH_MAIN );
		if( pWndGHMain )
			pWndGHMain->SetVisible( FALSE );

		CWndQuestQuickInfo* pWndQuestQuickInfo = ( CWndQuestQuickInfo* )g_WndMng.GetWndBase( APP_QUEST_QUICK_INFO );
		if( pWndQuestQuickInfo )
			pWndQuestQuickInfo->SetVisible( FALSE );

		CWndSecretRoomQuick* pWndSecretRoomQuick = (CWndSecretRoomQuick*)g_WndMng.GetWndBase( APP_SECRETROOM_QUICK );
		if(pWndSecretRoomQuick != NULL)
			pWndSecretRoomQuick->SetVisible(FALSE);
		SetFocus();
	}
	m_clearFlag = TRUE;
}

void CWndMgr::RestoreWnd() {
	if (!m_clearFlag) return;

	for (const int wndId : m_tempWndId) {
		POSITION pos = m_mapWndApplet.GetStartPosition();
		while( pos )
		{
			DWORD dwIdApplet;
			CWndBase * pWndBase;
			m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );

			if(wndId == pWndBase->GetWndId())
			{
				pWndBase->SetVisible(TRUE);
				break;
			}
		}
	}
	m_tempWndId.clear();

	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
	if(pWndChat != NULL && pWndChat->m_bChatLog)
		m_pWndChatLog->SetVisible(TRUE);

	static constexpr std::initializer_list<UINT> windowsToShow = {
		APP_PET_STATUS, APP_BUFFPET_STATUS,
		APP_SECRETROOM_QUICK, APP_GH_MAIN, APP_QUEST_QUICK_INFO
	};

	for (const auto appId : windowsToShow) {
		CWndBase * window = GetWndBase(appId);
		if (window) window->SetVisible(TRUE);
	}

	m_clearFlag = FALSE;
}

CWndBase* CWndMgr::GetWndVendorBase( void )
{
	return GetWndBase( APP_VENDOR_REVISION );
}

BOOL CWndMgr::IsDstRate(int nDstParam)
{
	return IsDst_Rate(nDstParam);
}

void CWndMgr::FormalizeChatString( CString & str )
{	// 康: 2008-07-18
	// "을 이용하여 클라이언트 명령어 파싱을 무시하는 행위를 막기 위한 것
	str.TrimLeft();
	str.TrimRight();

	while( str.GetLength() > 0 && str.GetAt( 0 ) == '\"' )
	{
		str.Delete( 0 );

		if(str.GetLength() > 1)
		{
			if( str.GetAt( str.GetLength() - 1 ) == '\"' )
				str.Delete( str.GetLength() - 1 );
			str.TrimLeft();
			str.TrimRight();
		}
	} 
}

#ifdef __WINDOW_INTERFACE_BUG
void CWndMgr::CloseBoundWindow(void) // 아이템이 걸려 있거나, 아이템을 조작할 가능성이 있는 모든 창을 강제로 닫는다.
{
	SAFE_DELETE( m_pWndPiercing ); // 피어싱
	SAFE_DELETE( m_pWndChangeWeapon ); // 유니크 웨폰, 얼터멋 웨폰 변환 창
	SAFE_DELETE( m_pWndRemoveAttribute ); // 속성제련 제거
	SAFE_DELETE( m_pWndRemovePiercing ); // 피어싱 옵션 제거
	SAFE_DELETE( m_pWndRemoveJewel ); // 얼터멋 웨폰 보석 제거
	SAFE_DELETE( m_pWndSmeltMixJewel ); // 오리칼쿰, 문스톤 합성창
	SAFE_DELETE( m_pWndSmeltJewel ); // 보석 합성 창
	SAFE_DELETE( m_pWndExtraction ); // 보석 추출창
	SAFE_DELETE( m_pWndLvReqDown ); // 착용레벨 하락 해지창
	SAFE_DELETE( m_pWndBlessingCancel ); // 여신의 축복 취소
	SAFE_DELETE( m_pWndBank ); // 은행
	SAFE_DELETE( m_pWndGuildBank ); // 길드창고
	SAFE_DELETE( m_pWndPost ); // 우편
	SAFE_DELETE( m_pWndUpgradeBase ); // 제련
	SAFE_DELETE( m_pWndShop );
	SAFE_DELETE( m_pWndSmeltSafety ); // 안전 제련
	SAFE_DELETE( m_pWndSmeltSafetyConfirm ); // 제련 확인
	SAFE_DELETE( m_pWndEquipBindConfirm ); // 귀속 확인
}
#endif // __WINDOW_INTERFACE_BUG

#ifdef __BAN_CHATTING_SYSTEM
//-----------------------------------------------------------------------------
void CWndMgr::SetWarningCounter( int nWarningCounter )
{
	m_nWarningCounter = nWarningCounter;
}
//-----------------------------------------------------------------------------
void CWndMgr::SetWarning2Counter( int nWarning2Counter )
{
	m_nWarning2Counter = nWarning2Counter;
}
//-----------------------------------------------------------------------------
int CWndMgr::GetWarningCounter( void ) const
{
	return m_nWarningCounter;
}
//-----------------------------------------------------------------------------
int CWndMgr::GetWarning2Counter( void ) const
{
	return m_nWarning2Counter;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetWarningTimer( void )
{
	return m_timerWarning;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetWarning2Timer( void )
{
	return m_timerWarning2;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetShortcutWarningTimer( void )
{
	return m_timerShortcutWarning;
}
//-----------------------------------------------------------------------------
CTimer& CWndMgr::GetBanningTimer( void )
{
	return m_timerBanning;
}
//-----------------------------------------------------------------------------
BOOL CWndMgr::IsShortcutCommand( void ) const
{
	return m_bShortcutCommand;
}
//-----------------------------------------------------------------------------
#endif // __BAN_CHATTING_SYSTEM