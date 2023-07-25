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
#include "WndCharacter.h"
#include "WndGuild.h"
#include "WndMessenger.h"
#include "WndCommand.h"
#include "WndNotice.h"
#include "WndSkillTree.h"
#include "WndMap.h"
#include "WndParty.h"
#include "WndFriendConfirm.h"
#include "WndDuelConfirm.h"
#include "WndRegVend.h"
#include "WndSealChar.h"
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
#include "sqktd/algorithm.h"
#include "WndGuildCombat1to1.h"
#include "WndLvReqDown.h"

#include "couplehelper.h"
#include "FuncTextCmd.h"

#ifdef __Y_INTERFACE_VER3
#include "WorldMap.h"
#endif //__Y_INTERFACE_VER3

#include "WndSelectVillage.h"
#include "WndPost.h"

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
#include "WndCommItem.h"
#include "WndQuest.h"

#include "boost/pfr.hpp"


extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;

#define CLIENT_WIDTH FULLSCREEN_WIDTH
#define CLIENT_HEIGHT FULLSCREEN_HEIGHT

std::vector<CWndBase *> m_wndOrder;

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

namespace WndMgr {
	RegInfo::RegInfo(CFileIO & file) {
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

	void RegInfo::StoreIn(CFileIO & file) const {
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

	RegInfo::RegInfo(CWndNeuz & pWndNeuz, BOOL bOpen) {
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

	void RegInfo::RestoreParameters(CWndNeuz & pWndBase) const {
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

#ifdef __PROTECT_AWAKE
	m_pWndSelectAwakeCase = NULL;
#endif

	m_bWaitRequestMail = FALSE;
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

void CWndMgr::Free()
{
	m_mapWndRegInfo.clear();

	SafeDeleteAll();
	
	SAFE_DELETE( m_pWndTaskBar );
	
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

namespace WndMgr {
	template<typename Ptr>
	static void Impl_SafeDeleteWindowBase(Ptr *& ptr) {
		static_assert(std::derived_from<Ptr, CWndBase>);

		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	
	template<size_t I>
	static void Impl_SafeDeleteAll(COwnedChildren & self) {
		if constexpr (I != boost::pfr::tuple_size_v<COwnedChildren>) {
			Impl_SafeDeleteWindowBase(boost::pfr::get<I>(self));
			Impl_SafeDeleteAll<I + 1>(self);
		}
	}

	void COwnedChildren::SafeDeleteAll() {
		Impl_SafeDeleteAll<0>(*this);
	}

	template<size_t I>
	static bool Impl_DeleteChild(COwnedChildren & self, CWndBase * window) {
		// If you have an error near here saying "hey this window is not a CWndBase"
		// it means you have to include the .h with the window class definition.
		if constexpr (I != boost::pfr::tuple_size_v<COwnedChildren>) {
			if (window == boost::pfr::get<I>(self)) {
				Impl_SafeDeleteWindowBase(boost::pfr::get<I>(self));
				return true;
			}

			return Impl_DeleteChild<I + 1>(self, window);
		}

		return false;
	}

	bool COwnedChildren::DeleteChild(CWndBase * window) {
		return Impl_DeleteChild<0>(*this, window);
	}
}

void CWndMgr::DestroyApplet()
{
	for (auto & [dwIdApplet, pWndBase] : m_mapWndApplet) {
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
	m_mapWndApplet.clear();

	// 파괴할 때 인터페이스 정보가 저장된다. 따라서 파괴 이후 save하기.
	if( m_bTitle == FALSE )
		SaveRegInfo( "regInfo.dat" );
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
	InitializeTimers();
#endif // __BAN_CHATTING_SYSTEM
	AddAllApplet();

	m_dwSavePlayerTime = 0;

	char filename[MAX_PATH];
	for( int i=0; i<128; i++ )
	{
		sprintf( filename, "Icon_CoolTime_%.3d.tga", i );
		g_pCoolTexArry[i] = m_textureMng.AddTexture(  MakePath( DIR_ICON, filename ), 0xffff00ff );
	}

	for( int i=0; i<11; i++ )
	{
		sprintf( filename, "Icon_ImgIncAni_%.2d.tga", i );
		g_pEnchantTexArry[i] = m_textureMng.AddTexture(  MakePath( DIR_ICON, filename ), 0xffffffff );
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
		if(m_pWndTradeGold)
			m_pWndTradeGold->Destroy();
	} else
	if( m_pWndGuildBank == pWndChild )
	{
		g_DPlay.SendCloseGuildBankWnd();
		SAFE_DELETE( m_pWndGuildBank );
		pWndChild = NULL;
		if(m_pWndTradeGold)
			m_pWndTradeGold->Destroy();
	}

//#endif
	//else
	if( pWndChild )
	{
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
		if( auto it = m_mapWndApplet.find( pWndChild->GetWndId() ); it != m_mapWndApplet.end())
		{
			CWndNeuz * pWndBase = it->second;
			m_mapWndApplet.erase(it);
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


void CWndMgr::OpenItemInfo(CWndBase * pWndParent, ItemInfoType type, CItemElem * pItemBase) {
	if (type == ItemInfoType::QuestItem) {
		if (m_pQuestItemInfo && m_pQuestItemInfo->IsVisible()) {
			m_pQuestItemInfo->SetItemBase(pItemBase);
		} else {
			SAFE_DELETE(m_pQuestItemInfo);
			m_pQuestItemInfo = new CWndQuestItemInfo;
			m_pQuestItemInfo->Initialize(pWndParent, pItemBase);
		}
	} else {
		const auto OpenTextFromItem = [pWndParent, pItemBase](CWndTextFromItem *& m_pWnd, DWORD appId) {
			if (m_pWnd && m_pWnd->IsVisible()) {
				m_pWnd->SetItemBase(pItemBase);
			} else {
				SAFE_DELETE(m_pWnd);
				m_pWnd = new CWndTextFromItem;
				m_pWnd->Initialize(pWndParent, pItemBase, appId);
			}
		};

		switch (type) {
			case ItemInfoType::Book:   OpenTextFromItem(m_pWndTextBook  , APP_TEXT_BOOK  ); break;
			case ItemInfoType::Scroll: OpenTextFromItem(m_pWndTextScroll, APP_TEXT_SCROLL); break;
			case ItemInfoType::Letter: OpenTextFromItem(m_pWndTextLetter, APP_TEXT_LETTER); break;
		}
	}
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
		g_dpCertified.DeleteDPObject();
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
		CMailBox::GetInstance()->Clear();
		CCoupleHelper::Instance.Clear();
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

	m_aChat.clear();

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
BOOL CWndMgr::OpenCustomBox( CWndCustomMessageBox * pWndMessageBox )
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = pWndMessageBox;

	if(pWndMessageBox->Initialize( this ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenMessageBox( LPCTSTR lpszMessage, UINT nType ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
	if( m_pWndMessageBox->Initialize( lpszMessage, this, nType ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType )
{
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
	if( m_pWndMessageBox->Initialize( lpszMessage, this, nType ) == FALSE)
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE;
	}
	m_pWndMessageBox->SetTitle( strTitle );
	return FALSE;
}

BOOL CWndMgr::OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType, BOOL bPostLogoutMsg ) 
{ 
	SAFE_DELETE( m_pWndMessageBoxUpper );
	m_pWndMessageBoxUpper = new CWndMessageBoxUpper;

	if( m_pWndMessageBoxUpper->Initialize( lpszMessage, this, nType, bPostLogoutMsg ) == FALSE)
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
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndMgr::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.DeleteDeviceObjects();
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
	m_texture.RestoreDeviceObjects();
#endif //__YDEBUG
	
	return 0;
}
AppletFunc * CWndMgr::GetAppletFunc(DWORD dwIdApplet) {
	return sqktd::find_in_map(m_mapAppletFunc, dwIdApplet, nullptr);
}

DWORD CWndMgr::GetAppletId( TCHAR* lpszAppletName ) {
	for (const auto & [dwIdApplet, pApplet] : m_mapAppletFunc) {
		if (!_tcscmp(pApplet->m_pAppletName, lpszAppletName)) {
			return dwIdApplet;
		}
	}
	return 0xffffffff;
}

CWndBase * CWndMgr::GetApplet(DWORD dwIdApplet) {
	return sqktd::find_in_map(m_mapWndApplet, dwIdApplet, nullptr);
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
	if (!pWndBase) return nullptr;

	const auto itWndRegInfo = m_mapWndRegInfo.find(dwIdApplet);

	if (itWndRegInfo != m_mapWndRegInfo.end()) {
		itWndRegInfo->second.RestoreParameters(*pWndBase);
	}

	m_mapWndApplet.insert_or_assign(dwIdApplet, pWndBase);

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

		if( pShortcut->m_dwId == APP_SKILL_ )
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase )
				return;			// 스킬 확인창이면 스킬창을 닫을수가 없음.
			CWndSkillTreeCommon * pWndBase1 = g_WndMng.GetWndBase<CWndSkillTreeCommon>( APP_SKILL_ );
			if( pWndBase1 && pWndBase1->GetCurrSkillPoint() != g_pPlayer->m_nSkillPoint )
			{
				if( pWndBase == NULL )
				{
					SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
					g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning(true);
					g_WndMng.m_pWndReSkillWarning->Initialize();
				}
				return;
			}
		}
		
		if( pShortcut->m_dwId == APP_QUIT)
		{
			OpenCustomBox( new CWndQuit );
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
				OpenCustomBox( new CWndLogOut );
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
		if(g_pPlayer->m_vtInfo.VendorIsVendor() )
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
					g_WndMng.m_pWndEquipBindConfirm->Initialize();
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
	  if (g_pPlayer->m_vtInfo.VendorIsVendor()) return;

		UseSkillShortCut(pShortcut->m_dwId);

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
								for (const auto & [_, ptr] : pWndWorld->m_buffs.m_mapBuffs) {
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
					const ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
					if( pItemProp && pItemProp->dwItemKind3 == IK3_WAND ) //Wand일 경우 AutoAttack을 하지 않음.
					{
						CCtrl* pFocusObj = (CCtrl*)pFocus;
						g_pPlayer->CMD_SetMagicAttack( pFocusObj->GetId(), 0 );
					}
					else
						g_WndMng.m_pWndWorld->m_bAutoAttack = TRUE;
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
					g_WndMng.m_pWndCoupleMessage->Initialize();
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
	D3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
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

		m_Theme.RenderGameGradeScreen( &g_Neuz.m_2DRender );
	}
	else if( pWndWorld == NULL && m_bTitle )
	{
		CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			pWndLogin->SetVisible( TRUE );
			pWndLogin->EnableWindow( TRUE );
		}

		m_Theme.RenderDesktop( &g_Neuz.m_2DRender );

	}
#else // __GAME_GRADE_SYSTEM
	// 월드가 없거나 풀화면이 아닐 때
	if( pWndWorld == NULL && m_bTitle ) //|| pWndWorld->m_nWinSize != WSIZE_MAX )
	{
		m_Theme.RenderDesktop( &g_Neuz.m_2DRender );
	}
#endif // __GAME_GRADE_SYSTEM
	else
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
	D3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	D3DDEVICE->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	return TRUE;
}
BOOL CWndMgr::Process()
{
	_PROFILE("CWndMgr::Process()");

	static DWORD dwTargetTime = g_tmCurrent + 5000;
	if( m_bWaitRequestMail && dwTargetTime < g_tmCurrent )		//메일이 있다고 받고, 메일정보를 받지 못한채 5초가 흐름.
	{
		g_DPlay.SendQueryMailBox();
		dwTargetTime = g_tmCurrent + 5000;
	}

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
					m_pWndRevival->Initialize( this );
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

						m_pWndSecretRoomQuick->Initialize();
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
		}

		CWndSkillTreeCommon::ReInitIfOpen();
		
		if( pWndBase = GetApplet( APP_INVENTORY ) )
		{
			( (CWndInventory*)pWndBase )->m_dwEnchantWaitTime = 0xffffffff;	// 제련하는도중 replace하는 경우 neuz가 크래쉬 되는 현상 수정
			( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( &pMover->m_Inventory, APP_INVENTORY );
		}

		if( pWndBase = GetApplet( APP_BAG_EX ) )
			( (CWndBagEx*)pWndBase )->InitItem();

		g_Neuz.m_camera.SetPos( pMover->GetPos() );		//

		if(!g_pPlayer)
		{
			Error( "SetPlayer : ActiveMover 없음" ); 
		}
		if(g_pPlayer->m_pActMover == NULL )
		{
			Error( "SetPlayer : ActionMover 없음" ); 
		}

		g_Neuz.SetCharacterName(pMover->GetName(FALSE));

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
		//pWndWorld->Projection( );
		g_Neuz.m_camera.Reset();
		g_Neuz.m_camera.Process( );
		g_Neuz.m_camera.Transform( g_WorldMng.Get() );

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
		CWndSkillTreeCommon::ReInitIfOpen();

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

	while (s.tok != FINISHED) {
		const bool bFirst = (*s.m_pProg == ' ');

		CString str		= s.Token;

		CScanner s1;
		s1.SetProg( (LPVOID)(LPCSTR)str );
		s1.GetToken();

		while (s1.tok != FINISHED) {
			const bool bSecond    = (*s1.m_pProg == ' ');
			const bool bTerminate = (*s1.m_pProg == '\0');

			CString str1 = s1.Token;
			str1.MakeLower();

			const auto it = prj.m_mapAlphaFilter.find(str1);
			if (it != prj.m_mapAlphaFilter.end()) {
				if (g_Option.m_nSlangWord == 1) {
					TCHAR szWord[256] = { 0, };
					memset(szWord, '*', lstrlen(str1));
					strResult += szWord;
				} else {
					strResult += it->second;
				}
			} else {
				strResult += s1.Token;
			}

			if ((bTerminate && bFirst) || (!bTerminate && bSecond)) {
				strResult += ' ';
			}

			s1.GetToken();
		}
		s.GetToken();
	}

	for (const auto & [str, replacement] : prj.m_mapNonalphaFilter) {
		if (g_Option.m_nSlangWord == 1) {
			TCHAR	szWord[256] = { 0, };
			memset(szWord, '*', str.GetLength());
			strResult.Replace(str, szWord);
		} else {
			strResult.Replace(str, replacement);
		}
	}

	rString = strResult;
}
// 챗에서 또는 매크로로 입력된 텍스트가 이 함수를 용도에 맞게 분류된다.
void CWndMgr::ParsingChat( CString string )
{
	CWndGuideSystem* pWndGuide = GetWndBase<CWndGuideSystem>( APP_GUIDE );
	if (pWndGuide && pWndGuide->IsVisible()) {
		pWndGuide->m_Condition.strInput = string;
	}
	string.TrimLeft( ' ' );
	if (string.IsEmpty()) return;

	// 문장길이가 너무 길면 64바이트로 줄임.
	SetStrNull( string, 120 );

	if( string.Find("#l") != -1 )
	{
		int mCut = string.Find('/');

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
			CString strMessage;
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
		if (m_aChat.size() >= 500) {
			m_aChat.erase(m_aChat.begin(), m_aChat.begin() + 250);
		}

		m_aChat.push_back(WndMgr::StoredChatMessage{ strChat, dwColor, dwChatStyle });
	}
	if( dwStyle & TMS_CHAT )
	{
		if( pWndChat )
			pWndChat->PutString( strChat, dwColor, dwPStyle );
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
	{ DST_IGNORE_DMG_PVP , TID_TOOLTIP_DST_DMG_GET },
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


// 숏컷으로 스킬 실행하기.
void CWndMgr::UseSkillShortCut( DWORD dwSkillIdx )
{
	LPSKILL lpSkill = g_pPlayer->GetSkill( dwSkillIdx );
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

	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
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
	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );

	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, g_Neuz.m_d3dsdBackBuffer.Width, g_Neuz.m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
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

CWndMessage * CWndMgr::GetMessage(LPCTSTR lpszFrom) {
	const auto it = m_mapMessage.find(lpszFrom);
	return it != m_mapMessage.end() ? it->second.get() : nullptr;
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

CWndInstantMsg * CWndMgr::GetInstantMsg(LPCTSTR lpszFrom) {
	const auto it = m_mapInstantMsg.find(lpszFrom);
	return it != m_mapInstantMsg.end() ? it->second.get() : nullptr;
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
	m_mapWndRegInfo.insert_or_assign(pWndNeuz->GetWndId(), WndMgr::RegInfo(*pWndNeuz, bOpen));
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
				WndMgr::RegInfo wndRegInfo(file);
				m_mapWndRegInfo.insert_or_assign(wndRegInfo.GetWndId(), std::move(wndRegInfo));
			}

			file.Close();
		}
	}
	return bResult;
}

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
	if(!m_clearFlag)
	{
		for (CWndNeuz * pWndBase : m_mapWndApplet | std::views::values) {
			if (pWndBase && pWndBase->GetWndId() != APP_WORLD) {
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
		for (const auto & [dwIdApplet, pWndBase] : m_mapWndApplet) {
			if (wndId == pWndBase->GetWndId()) {
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
	str.Trim();

	while( str.GetLength() > 0 && str.GetAt( 0 ) == '\"' )
	{
		str.Delete( 0 );

		if(str.GetLength() > 1)
		{
			if( str.GetAt( str.GetLength() - 1 ) == '\"' )
				str.Delete( str.GetLength() - 1 );
			str.Trim();
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


///////////////////////////////////////////////////////////////////////////////

namespace WndMgr {
	void CBanningSystem::InitializeTimers() {
		m_timerWarning.Set(WARNING_MILLISECOND, TRUE);
		m_timerWarning2.Set(WARNING_2_MILLISECOND, TRUE);
		m_timerShortcutWarning.Set(SHORTCUT_WARNING_MILLISECOND, TRUE);
		m_timerBanning.Set(BANNING_MILLISECOND, TRUE);
	}

}


