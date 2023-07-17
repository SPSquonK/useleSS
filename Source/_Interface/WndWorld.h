// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDWORLD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDWORLD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __HELP_BUG_FIX
// 헬프 이중생성 방지
extern vector<CString> g_vecHelpInsKey;
#endif //__HELP_BUG_FIX

#include <boost/container/stable_vector.hpp>
#include "buff.h"


class CCaption final
{
private:
	struct CAPTION {
		TCHAR m_szCaption[512];
		CD3DFontAPI * m_pFont;

		CTexture m_texture;
		CSize m_size;
		FLOAT m_fScale;
		FLOAT m_fAddScale;
	};

	bool m_bEnd = false;
	CTimer m_timer;
	int m_nAlpha = 255;
	boost::container::stable_vector<CAPTION> m_aCaption;

public:
	void RemoveAll();
	void Process();

	void Render( CPoint ptBegin, C2DRender* p2DRender );
	void AddCaption( LPCTSTR lpszCaption, CD3DFontAPI* pFont, BOOL bChatLog = TRUE, DWORD dwColor = D3DCOLOR_ARGB(  255, 255, 255, 255 ) );

    // Initializing and destroying device-dependent objects
    HRESULT DeleteDeviceObjects();
    HRESULT InvalidateDeviceObjects();
};

class CCapTime 
{
public:
	CD3DFontAPI* m_pFont;
	CTexture m_texture;
	CSize m_size;

	FLOAT m_fXScale;
	FLOAT m_fYScale;
	FLOAT m_fAddScale;
	int m_nAlpha;
	int m_nTime;
	int m_nStep;
	BOOL m_bRender;
	CCapTime();
	~CCapTime();

	void Process();
	void Render( CPoint ptBegin, C2DRender* p2DRender );
	void SetTime( int nTime, CD3DFontAPI* pFont );
    HRESULT DeleteDeviceObjects();
    HRESULT InvalidateDeviceObjects();
};

extern CCapTime g_CapTime;
extern CCaption g_Caption1;
//////////////////////////////////////////////////////////////////////////////////////
// 월드 
//
#define NEXTSKILL_NONE				0xffffffff	// 다음스킬 사용하지 않음
#define NEXTSKILL_ACTIONSLOT		0x7fffffff	// 다음스킬 액션슬롯으로 사용
// 그외는 스킬인덱스.

#include "sqktd/flasher.h"

struct BUFFSKILL {
	sqktd::Flasher<64, 192, 6> m_flasher;
	CTexture * m_pTexture = nullptr;
};

struct BUFFICON_INFO
{
	CPoint	pt;			// render위치 
	int		nCount;		// 그려진 갯수 
	int		nDelta;		// 이동할 좌표 
};

struct BUFFICONRECT_INFO
{
	RECT    rc;
	DWORD	dwID;
};

// 버튼형식의 도움말( 클릭시 지정된 창 실행 )

class CAdvMgr final {
public:
	struct BUTTON_INFO {
		std::unique_ptr<CWndButton> m_pwndButton;
		DWORD			m_dwRunWindow;
	};

private:
	static constexpr size_t MAX_ADVBUTTON = 10;

	int        m_nIndex;
	CWndBase * m_pParentWnd;
	std::vector<BUTTON_INFO> m_vecButton;
	
public:
	CAdvMgr();

	void Init( CWndBase* pParentWnd );
	void AddAdvButton( DWORD dwid );	
	bool RunButton( DWORD dwID );
	void MoveButtons();
};

namespace WndWorld {
	struct GuildCombatInfo {
		struct GUILDRATE {
			u_long m_uidPlayer;
			int    nLife;
			bool   bJoinReady;
		};

		std::vector<GUILDRATE> GuildStatus;
		std::set<u_long> m_gc_defenders;
		boost::container::flat_map<u_long, int> m_gc_warstates;

		void OnGuildStatus(CAr & ar);
		void ClearGuildStatus();
		void OnPlayerList(CAr & ar);
		void ClearPlayerList();
		bool IsGCStatusDefender(u_long uidDefender) const;
		int  IsGCStatusPlayerWar(u_long uidPlayer) const;

		void RenderMyGuildStatus(C2DRender * p2DRender);
	};

	struct GuildCombatPrecedence {
		std::map<u_long, std::string> idToGuildName;
		std::multimap<int, u_long> guilds;
		std::multimap<int, u_long> players;

		void Clear();
		void OnGuildPrecedence(CAr & ar);
		void OnPlayerPrecedence(CAr & ar);

		void Render(C2DRender * p2DRender, CRect clientRect);

		[[nodiscard]] LPCTSTR GetGuildName(u_long guildId) const;
	};

	class FlyTargets {
	private:
		size_t current = 0;
		std::vector<OBJID> targets;

	public:
		void Clear() { targets.clear(); }
		void Add(OBJID idMover) { targets.emplace_back(idMover); }

		[[nodiscard]] std::optional<OBJID> GetNext();
		[[nodiscard]] std::span<const OBJID> GetAll() const { return targets; }
	};

}

struct __PGUEST_TIME_TEXT {
	BOOL bFlag;
	GroupQuest::ProcessState  nState;
	DWORD dwQuestTime;
};

typedef struct __SRGUILDINFO
{
	DWORD dwGuildId;
	int nWarState;
	int nKillCount;
} __SRGUILDINFO;

typedef struct __KILLCOUNTCIPHERS
{
	BOOL bDrawMyGuildKillCount;
	char szMyGuildKillCount;
	CPoint ptPos;
	float fScaleX;
	float fScaleY;
	int nAlpha;
} __KILLCOUNTCIPHERS;

#define MAX_KILLCOUNT_CIPHERS 3

namespace TimeSpanToString {
	[[nodiscard]] CString DHMmSs(CTimeSpan timeSpan);
}

class CWndWorld final : public CWndNeuz
{
	BOOL m_bBGM;
	FLOAT m_fHigh;
	FLOAT m_fLow;
	CPoint m_ptMouseOld;
	CPoint m_ptMouseSpot;
	BOOL m_bFreeMove;
	BOOL m_bFlyMove;
	BOOL m_bSelectTarget;
#ifdef __VRCAMERA
	BOOL m_bCameraMode;
#endif
	BOOL		m_bViewMap;

public:
	CAdvMgr		m_AdvMgr;
	CObj*		m_pSelectRenderObj;

	CAdvMgr*	GetAdvMgr() { return &m_AdvMgr; }
	
	BOOL	m_bCtrlInfo;
	BOOL	m_bCtrlPushed;
	BOOL	m_bRenderFPS;

	BOOL	m_bShiftPushed; //다이스 옮길 때 한번에 넣기.
	BOOL	m_bAngelFinish; //Angel 경험치 완료 유무.
	CObj*	m_pNextTargetObj;
	CObj*	m_pRenderTargetObj;
	DWORD m_dwIdBgmMusic;
//#if __VER >= 9
//	DWORD	m_dwIdBgmMusicOld;
//#endif	//
	CTexturePack m_texTarget;		// 지상에서의 4귀퉁이 타겟그림		//sun!
	CTexturePack m_texTargetFly;	// 비행중에서의 4귀퉁이 타겟그림.
	CTexturePack m_texTargetArrow;	// 타겟이 화면을 벗어났을때 화살표방향표시.
	CTexturePack m_texGauFlight;	// 비행 게이지 인터페이스.

	BOOL s_bUped, s_bDowned, s_bLefted, s_bRighted;
	BOOL s_bCombatKeyed, s_bFlyKeyed, s_bAccKeyed, s_bSitKeyed;
	BOOL s_bTempKeyed, m_bTemp2ed, m_bTemp3ed;
	BOOL s_bBoarded;

	CTimer m_timerFocusAttack;
	CTimer m_timerLButtonDown;
	CTimer m_timerRButtonDown;
	//CTimer m_timerSpell;
	DWORD  m_dwPowerTick;				// 마우스를 누른 상태로 힘을 모아 공격하는 패턴에 사용하는 시작시각 
	CTimer m_timerAutoRun;
	CTimer m_timerAutoRunPush;
	CTimer m_timerAutoRunBlock;
	BOOL   m_nDubleUp;
//	CTexture m_texFlaris;

	//CObj* m_pCaptureObj;

	CWndMenu m_wndMenuMover;

	BOOL m_bSetQuestNPCDest;
	D3DXVECTOR3 m_vQuestNPCDest;
	D3DXVECTOR3 m_vDestinationArrow;

#ifdef __BS_PUTNAME_QUESTARROW
	CString m_strDestName;
	BOOL m_bStartAniArrow;
#endif

	//ect m_rectBound;

	CTexture m_texLvUp;
	CTexture m_texLvDn;
	CTexture m_texLvUp2;
	CTexture m_texLvDn2;
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal ;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	int		m_nMouseMode;		// 비행중일때. 0:우버튼+드래그 빗자루회전   1:걍드래그 빗자루회전.
	
	int		ControlPlayer( DWORD dwMessage, CPoint point );
	int		ControlGround( DWORD dwMessage, CPoint point );
	int		ControlFlying( DWORD dwMessage, CPoint point );
	int		ControlShip( DWORD dwMessage, CPoint point );
	
	//CTexture m_texTargetGauge;
	CModelObject m_meshArrow; // 타겟의 방향을 알려주는 화살표 오브젝트이다.
	
	CModelObject	m_meshArrowWanted; // 타겟의 방향을 알려주는 화살표 오브젝트이다.
	BOOL			m_bRenderArrowWanted;
	DWORD			m_dwRenderArrowTime;
	D3DXVECTOR3		m_v3Dest;
	void			SetRenderArrowWanted( BOOL b, D3DXVECTOR3 vDest ) { m_v3Dest = vDest; m_bRenderArrowWanted = b; m_dwRenderArrowTime = g_tmCurrent; }
	
	void RenderArrow();
	BOOL UseSkillToFocusObj( CCtrl* pFocusObj );
	void GetBoundRect( CObj* pObj, CRect* pRect );

	void RenderArrow_Text( const D3DXVECTOR3& vDest, const D3DXMATRIX& mat );	//gmpbigsun : refactoring
	
	WndWorld::FlyTargets m_flyTarget;

	DWORD	m_dwNextSkill;		// 치고있는중에 스킬치기 예약.
	std::vector <BUFFICONRECT_INFO> m_rcCheck;

	CTexture		m_TexGuildWinner;
	CTexture		m_TexGuildBest;
	WndWorld::GuildCombatPrecedence m_GCprecedence;
	WndWorld::GuildCombatInfo       m_infoGC;

	CWndBase* m_pWndBuffStatus;

public:
	void InitEyeFlash();
	void SetViewMap(BOOL bViewMap){ m_bViewMap = bViewMap; }
	BOOL IsViewMap()			  { return m_bViewMap; }
	BOOL GetBuffIconRect( DWORD dwID, const CPoint& point );
	int  GetBuffTimeGap();
	void RenderOptBuffTime( C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor );
	void RenderWantedArrow();
	void RenderMoverBuff( CMover* pMover, C2DRender *p2DRender );

	CD3DFontAPI* m_pFontAPICaption;
	CD3DFontAPI* m_pFontAPITitle;
	CD3DFontAPI* m_pFontAPITime;

	CTexturePack	m_texMsgIcon;
	CTexturePack	m_texAttrIcon;
	CTexturePack	m_texPlayerDataIcon;

	CTexture			m_pTextureLogo[CUSTOM_LOGO_MAX];
	DWORD				m_dwOneSecCount;
	ADDSMMODE			m_AddSMMode;
	CTexture*			m_dwSMItemTexture[SM_MAX ];
	CTexture*			m_dwSMResistItemTexture[10];
	BOOL				m_bSMFlsh[SM_MAX];
	int					m_nSMAlpha[SM_MAX];
	CWndGuideSystem*    m_pWndGuideSystem;	

	void UseSkill();
	DWORD m_dwDropTime;
	CTexturePack m_texFontDigital;	// 디지탈 모양의 폰트(비행속도에 사용)
	
	static D3DXVECTOR3 m_vTerrainPoint;
	
	CRect m_rectSelectBegine;
	BOOL m_bNewSelect;
	//CObj* m_pSelectObj;
	BOOL m_bAutoAttack;
	OBJID	m_idLastTarget;
	BOOL	IsLastTarget( OBJID objid )	{	return( m_idLastTarget == objid );	}
	void	SetLastTarget( OBJID objid )	{	m_idLastTarget	= objid;	}
	DWORD	GetNextSkill( void ) { return m_dwNextSkill; }
	void	SetNextSkill( DWORD dwNextSkill )
	{
		m_dwNextSkill = dwNextSkill;
	}

	int		GetGMLogoIndex();

	float	m_fRollAng;		// 롤링 앵글.
	
#ifdef __YAUTOATTACK
	BOOL	m_bAttackDbk;
#endif //__YAUTOATTACK

	CMover * n_nMoverSelectCount;
	DWORD	m_dwGuildCombatTime;
	
	__PGUEST_TIME_TEXT m_QuestTime;
	
	char	m_szGuildCombatStr[64];
	struct __GuildCombatJoin
	{
		u_long uidGuild;
		char szJoinGuildName[MAX_NAME];
		int nJoinSize;
		int nOutSize;
	};
	std::vector< __GuildCombatJoin > m_vecGuildCombatJoin;
	std::vector< __GuildCombatJoin > m_vGuildCombatSort;

	CWndButton m_wndMenu;
	D3DXVECTOR3		m_vTelePos;
	void SetMouseMode( int nMode ) 
	{
		m_nMouseMode = nMode;
		if( nMode == 0 )
			ClipCursor( NULL ); // 커서가 다시 윈도우를 벗어나게 한다.
	}
	int	GetMouseMode( void ) { return m_nMouseMode; }

	void Projection( );

	CObj* PickObj( POINT point, BOOL bOnlyNPC = FALSE );
	CObj* SelectObj( POINT point );
	CObj* HighlightObj( POINT point );
	
	OBJID	m_objidTracking;

	CWndWorld();
	virtual ~CWndWorld();
	void RenderAltimeter();
	void RenderFocusObj( CObj* pObj, CRect rect, DWORD dwColor1, DWORD dwColor2 );
	void SetNextTarget();
	void RenderFocusArrow( CPoint pt );
	void RenderGauFlight( C2DRender* p2DRender );		// 비행모드시 게이지 인터페이스 Draw

	BOOL						m_bFirstFlying;
	int                         m_nLimitBuffCount;
	CBuffMgr	m_buffs;
	std::vector< std::multimap<DWORD, BUFFSKILL> >	m_pBuffTexture;

	DWORD	GetSystemPetTextureKey( IBuff* pBuff );
	void RenderBuff( C2DRender* p2DRender );
	void RenderSelectObj( C2DRender* pRender, CObj* pObj );
	void RenderExpBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BUFFICON_INFO* pInfo, CPoint ptMouse, DWORD dwItemID );
	void RenderBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse );
	void RenderSMBuff( C2DRender *p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse );
	void RenderCasting(C2DRender *p2DRender);
	void	RenderEventIcon( C2DRender* p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse );

	void PutPetTooltipInfo( CItemElem* pItemElem, CEditString* pEdit );
	void DrawGuildCombat1to1PlayerInfo(C2DRender *p2DRender);
	void DrawGuildCombat1ot1GuildInfo(C2DRender *p2DRender);
	BOOL m_bGuildCombat1to1Wait;

	char m_szSecretRoomStr[256];
	std::vector<__SRGUILDINFO> m_vecGuildList;
	__KILLCOUNTCIPHERS m_stKillCountCiphers[MAX_KILLCOUNT_CIPHERS];
	BOOL m_bFlashBackground;

	void DrawSecretRoomGuildInfo(C2DRender *p2DRender, BOOL bIsMyGuild, int nRank, __SRGUILDINFO stGuildInfo, CPoint ptRank, CPoint ptLogo, 
		CPoint ptGName, CPoint ptHypoon, CPoint ptState, float fLogoScaleX, float fLogoScaleY, CRect rectBg);
	void DrawSecretRoomInfo(C2DRender *p2DRender);
	void DrawMyGuildKillCount(C2DRender *p2DRender, __SRGUILDINFO stGuildInfo, CPoint ptState, int nMax);
	void DrawOutLineLamp(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend, int nState, int nRank, BOOL bIsMyGuild);
	void DrawOutLineFlash(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend);

	void InviteParty( u_long uidPlayer );
	void InviteCompany( OBJID objId );

	virtual void OnDraw(C2DRender* p2DRender); 
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	void OnSetCursor() override;
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMButtonUp(UINT nFlags, CPoint point);
	virtual void OnMButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL Process();
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnMouseWndSurface( CPoint point );
	
	BOOL UseFocusObj( CCtrl* pFocusObj );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

private:
	void	ShowMoverMenu( CMover* pTarget );
	int		GetGaugePower( int* pnValue );

	void	ShowCCtrlMenu( CCtrl* pCCtrl );		
	BOOL MenuException( CPoint point );	// 메뉴를 띄우기 전에 처리해 줘야 할 예외 사항들
};
#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
