#if !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory>
#include <optional>
#include <functional>

#include "post.h"
#include "guild.h"
#include "misc.h"  // WANTED_ENTRY

#include "couple.h"
#include "couplehelper.h"

const int MAX_WANTED_LIST =	100;

#include "honor.h"
#include "WndSqKComponents.h"

#include "WndMailRequestingBox.h"

namespace WndFieldUtil {
  void ResetRenderState();
  [[nodiscard]] D3DVIEWPORT9 BuildViewport(C2DRender * const render, WNDCTRL * const control);
}

///////////////////////////////////////////////////////////////////////////////

class CWndOnlyOneItemReceiver : public CWndItemReceiver {
private:
	DWORD m_itemId;

public:
	CWndOnlyOneItemReceiver(DWORD itemId, DWORD shadow, CWndItemReceiver::Features features = {})
		: CWndItemReceiver(), m_itemId(itemId) {
		ChangeShadowTexture(prj.GetItemProp(itemId), shadow);
	}

	CWndOnlyOneItemReceiver(DWORD itemId, CWndItemReceiver::Features features = {})
		: CWndItemReceiver(features), m_itemId(itemId) {
		if (features.shadow) {
			ChangeShadowTexture(prj.GetItemProp(itemId));
		}
	}

	bool CanReceiveItem(const CItemElem & itemElem, bool) override {
		return itemElem.m_dwItemId == m_itemId;
	}
};


//////////////////////////////////////////////////////////////////////////////////////
// 드롭아이템 
//
class CWndDropItem final : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem = nullptr;
	D3DXVECTOR3 m_vPos;
	CWndEdit* m_pEdit = nullptr;

	CWndDropItem() = default;
	~CWndDropItem() override = default;
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL Process( void );
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndDropConfirm final : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem = nullptr;
	D3DXVECTOR3 m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL Process( void );
	virtual	void OnInitialUpdate(); 
}; 

class CWndQuestItemWarning final : public CWndNeuz
{ 
public: 
	CItemElem* m_pItemElem = nullptr;
	D3DXVECTOR3 m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	~CWndQuestItemWarning() override = default;
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 


//////////////////////////////////////////////////////////////////////////////////////
//  퀘스트 아이템 정보 창
//
class CWndQuestItemInfo final : public CWndNeuz {
public:
	~CWndQuestItemInfo() override = default;

	CItemElem * m_pItemBase = nullptr;
	CPoint		m_Position;

	void SetItemBase(CItemElem * pItemBase) { m_pItemBase = pItemBase; }
	
	BOOL Create(CItemElem * pItemBase, UINT nID, CWndBase* pWndParent = NULL );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, CItemElem * pItemBase = NULL); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
};

//////////////////////////////////////////////////////////////////////////////////////
// 인벤토리 
//
class CWndRemoveJewelConfirm;

class CWndGold : public CWndButton
{
public:
	CTexture m_texture;
	~CWndGold() override = default;
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnInitialUpdate();
};

class CWndConfirmBuy;
class CWndInventory final : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	std::unique_ptr<CModelObject> m_pModel;
	BOOL				m_bLButtonDownRot;
	
	std::array<CRect, MAX_HUMAN_PARTS> m_InvenRect;

	CModelObject m_meshStage;
	CItemElem* m_pSelectItem;
	BOOL m_bReport;
	
	CTexture* m_TexRemoveItem;
public:
	static void InitializeInvenRect(std::array<CRect, MAX_HUMAN_PARTS> & invenRect, /* const */ CWndBase & self);

	void UpDateModel();

	void UpdateParts();
	CModelObject* GetModel() { return m_pModel.get(); }
	void BaseMouseCursor();
	void RunUpgrade( CItemElem * pItem );
	void SetEnchantCursor();
	
	CSfx*		m_pSfxUpgrade;
	CItemElem *	m_pUpgradeItem;
	CItemElem *	m_pUpgradeMaterialItem;
	BOOL		m_bIsUpgradeMode;
	DWORD		m_dwEnchantWaitTime;

	CWndConfirmBuy* m_pWndConfirmBuy;
	CWndGold     m_wndGold;
	CWndItemCtrl m_wndItemCtrl;
	BOOL		m_bRemoveJewel;
	CWndRemoveJewelConfirm* m_pWndRemoveJewelConfirm;
	CWndInventory();
	virtual ~CWndInventory();
	virtual BOOL Process ();
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void OnDestroy( void );
	void OnSetCursor() override;
	
public:
	void UpdateTooltip( void )	{	m_wndItemCtrl.UpdateTooltip();	}

private:
	void ProcessEnchant();
	[[nodiscard]] std::expected<void, DWORD> RunUpgradeDecide(CItemElem * pItem);
};

class CWndQueryEquip final : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	std::unique_ptr<CModelObject> m_pModel;
	BOOL				m_bLButtonDownRot;
	DWORD		 m_ObjID;

	std::unique_ptr<std::array<CItemElem, MAX_HUMAN_PARTS>> m_aEquipInfoAdd;
	std::array<CRect, MAX_HUMAN_PARTS> m_InvenRect;
public:

	CMover* GetMover() 
	{ 
		if( m_ObjID != NULL_ID )
			return prj.GetMover( m_ObjID ); 

		return NULL;
	}

	CWndQueryEquip(CMover & mover, std::unique_ptr<std::array<CItemElem, MAX_HUMAN_PARTS>> aEquipInfoAdd);
	static void EnsureHasTexture(CItemElem & equipInfoAdd);
	
	virtual BOOL Process ();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual void OnMouseMove (UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


//////////////////////////////////////////////////////////////////////////////////////
// 매신저  
//

//////////////////////////////////////////////////////////////////////////////////////
// Navigator
//

class CWndNavigator final : public CWndNeuz
{
	CWndButton m_wndPlace;
	CWndButton m_wndMover;
	CWndButton m_wndZoomIn;
	CWndButton m_wndZoomOut;
	CWndMenu   m_wndMenuPlace;
	CWndMenu   m_wndMenuMover;
	BILLBOARD  m_billboard;
	CTexture   m_texArrow;
	CTexture   m_texDunFog;
	CTexturePack m_texNavObjs;
	DWORD		 m_iFrame = 0;
	DWORD		 m_iPastTime = 0;
	CTexturePack m_texNavPos;
	inline void	 AccuFrame() { 
		DWORD CurTime = g_tmCurrent;
		if (CurTime - m_iPastTime > 120)
		{
			m_iFrame++; 
			m_iPastTime = CurTime;
		}
		if ( m_iFrame >= 4 )
			m_iFrame = 0;
	};
	
	/* Render Mark */ //	참조되는 변수는 플레이어와 파티플레이어를 얻을 수 있는 것이어야 하지만 
																//	현재는 자신만을 찍는 것을 하고 그 다음에 파티 플레이어들을 찾을수 있는 방법을 찾아 보자꾸나
	void RenderMarkAll( C2DRender* p2DRender , CMover* Player );
	bool RenderNaviPoint(C2DRender * p2DRender, NaviPoint & naviPoint);
	CBillboard m_billArrow;
	CSize      m_size = CSize(0, 0);
	int        m_nSizeCnt;
	TCHAR      m_szName[ 64 ] = "";
	void ResizeMiniMap();
	
	CTexture m_GuildCombatTextureMask;
	CTexture* m_pDestinationPositionTexture = nullptr;

public:
	BOOL m_bObjFilterPlayer  = FALSE;
	BOOL m_bObjFilterParty   = FALSE;
	BOOL m_bObjFilterNPC     = FALSE;
	BOOL m_bObjFilterMonster = FALSE;
	

	void SetRegionName( const TCHAR *tszName );		// 지역
		
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	// message
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
};
//////////////////////////////////////////////////////////////////////////////////////
// CWndStatus
//

class CWndStatus final : public CWndNeuz
{
	BOOL m_bHPVisible;
	BOOL m_bExpVisible;
public:
	CModel* m_pModel;
	int		m_nDisplay;
public:
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauEmptySmall  ;
	CTexture m_texGauFillNormal  ;
	CTexture m_texGauFillSmall   ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBMPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBFPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBEXPGauge;
	
	BOOL m_bVBHPGauge;
	BOOL m_bVBMPGauge;
	BOOL m_bVBFPGauge;
	BOOL m_bVBEXPGauge;

	BOOL m_bShowTip_AEXP;
	BOOL m_bVBAEXPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBAEXPGauge;
	
	int m_nHPWidth ;
	int m_nMPWidth ;
	int m_nFPWidth ;
	int m_nEXPWidth;
	int m_nPXPWidth;
	
	void MakeGaugeVertex();
	
	CWndStatus(); 
	virtual ~CWndStatus();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );

	virtual void OnMouseWndSurface( CPoint point );
};

class CWndQuit : public CWndCustomMessageBox
{
	BOOL	m_bFlag;	
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	
class CWndLogOut : public CWndCustomMessageBox
{
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	


class CWndLogOutWaitting final : public CWndNeuz
{
	BOOL  m_bIsLogOut;
	DWORD dwTime;
public:
	void	SetIsLogOut(BOOL bFlag) { m_bIsLogOut = bFlag; }
	virtual BOOL Process ();
	virtual BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate();
};	

class CWndTradeGold final : public CWndNeuz {
public:
	SHORTCUT::Sources::ItemOrMoney m_source;
	std::function<void(int)> m_onValidation;
	std::function<void()> m_onCancel;

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	BOOL Process() override;
	void OnInitialUpdate() override;

	struct Initializer {
		int initialQuantity;
		bool skipIf1;
		DWORD titleTID;
		DWORD countTID;
	};

	struct ExtraCreateParam {
		std::function<void()> onCancel;
	};

	template<typename SourceType>
	static CWndTradeGold * Create(
		SourceType source,
		std::function<void(SourceType, int)> onValidation,
		ExtraCreateParam extra = {}
	) {
		return CreateGeneric(source,
			[source, onValidation = std::move(onValidation)](int quantity) {
				onValidation(source, quantity);
			},
			std::move(extra)
		);
	}

	static CWndTradeGold * CreateGeneric(
		SHORTCUT::Sources::ItemOrMoney source,
		std::function<void(int)> m_onValidation,
		ExtraCreateParam extra = {}
	);
	static Initializer GetInitialValueOf(SHORTCUT::Sources::ItemOrMoney source);


private:
	CWndTradeGold() = default;
	void SetInitialValue(int value);
};

class CWndConfirmTrade final : public CWndNeuz 
{ 
	OBJID m_objid = 0;
public: 

	void OnSetName( const char* szName, OBJID objid );
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndTradeConfirm final : public CWndNeuz {
	bool bMsg = false;
public: 

	BOOL Initialize(CWndBase * pWndParent = nullptr);
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

	void OnTradelastConfirmOk();
};

class CWndTrade final : public CWndNeuz {
public:
	CWndTradeCtrl m_wndItemCtrlYou;
	CWndTradeCtrl m_wndItemCtrlI;
	int           m_nGoldI, m_nGoldYou;

	CWndTrade();
	~CWndTrade() override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;


	void DoCancel();

};

//////////

class CWndRevival final : public CWndNeuz
{ 
public: 
	CWndButton* m_pLodeLight = nullptr;
	CWndButton* m_pLodeStar  = nullptr;
	CWndButton* m_pRevival   = nullptr;
	CWndButton* m_pShop      = nullptr;
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	void	EnableButton( int nButton, BOOL bEnable );
}; 

class CWndReWanted final : public CWndNeuz
{ 
private:
	CString      m_strWanted;
	
public: 
	BOOL         CheckWantedInfo( int nGold, LPCTSTR szMsg );
	void		 SetWantedName( LPCTSTR szWanted );

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndResurrectionConfirm final : public CWndNeuz
{ 
public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndWantedConfirm final : public CWndNeuz {
	int		 m_nGold = 0;
	char	 m_szName[MAX_NAME] = "";
public: 
	void		 SetInfo( const char szName[], const int nGold );
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 


struct WANTEDLIST {
	explicit WANTEDLIST(const WANTED_ENTRY & entry);

	char	 szName[ MAX_NAME ];		// 이름.
	__int64	 nGold;						// 현상금
	char     szDate[32];
	char	 szMsg[WANTED_MSG_MAX + 1];	// 십자평 
};

class CWndWanted final : public CWndNeuz
{ 
private: 
	CWndWantedConfirm*  m_pWantedConfirm = nullptr;
	time_t				m_recvTime;
	CWndScrollBar		m_wndScrollBar;
	boost::container::static_vector<WANTEDLIST, MAX_WANTED_LIST> m_aList;
	int					m_nSelect = -1;

public: 
	CWndWanted(time_t lTime);
	virtual ~CWndWanted(); 
	
	int          GetSelectIndex( const CPoint& point );	
	void InsertWanted(const WANTED_ENTRY & entry);
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 


class CWndCommItemDlg final : public CWndNeuz 
{ 
public: 
	CWndEdit*		m_pWndEdit = nullptr;
	DWORD			m_dwObjId  = NULL_ID;
	DWORD			m_dwCtrlId = NULL_ID;
	void			SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId );
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndChangeClass1 final : public CWndNeuz {
private:
	std::optional<OBJID> m_usedScroll = std::nullopt;

public:
	using JobId = int;
	struct JobDisplayer {
		void Render(
			C2DRender * const p2DRender, CRect rect,
			const JobId & item, DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};
	using CWndJobList = CWndTListBox<JobId, JobDisplayer>;

	static void OpenWindow(std::optional<OBJID> scrollPos);

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

private:
	CWndChangeClass1(std::optional<OBJID> scrollPos) : m_usedScroll(scrollPos) {}
	
	void OnSendModifiedJob();
	void OnModifiedJob();
};

class CWndGuildCombatOffer final : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold = 0;
	DWORD			m_dwMinGold = 0;
	DWORD			m_dwBackupGold = 0;

public: 
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
	void			SetGold( DWORD nCost );
	void			SetTotalGold( __int64 nCost );
	void			SetMinGold( DWORD dwMinGold )		{ m_dwMinGold    = dwMinGold; }
	void			SetReqGold( DWORD dwReqGold )		{ m_dwReqGold    = dwReqGold; }
	void			SetBackupGold( DWORD dwBackupGold ) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept( BOOL bFlag );	
}; 

class CWndGuildCombatBoard final : public CWndNeuz
{
protected:
public:
	int m_nCombatType;
public: 
	CWndGuildCombatBoard(int m_nCombatType);
	
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual void	PaintFrame( C2DRender* p2DRender );
	void			SetString( const CHAR* szChar );
}; 

class CGuildCombatSelectionClearMessageBox : public CWndCustomMessageBox
{ 
public: 
	CString m_strMsg;
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CGuildCombatInfoMessageBox final : public CWndNeuz
{
public:
	int	m_nCombatType;
public:
	CGuildCombatInfoMessageBox(int nCombatType);
	void	SetString( const CHAR* szChar );
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void PaintFrame( C2DRender* p2DRender );
}; 

class CGuildCombatInfoMessageBox2 final : public CWndNeuz
{ 
public: 
	void	SetString( CHAR* szChar );
	virtual	void OnInitialUpdate();
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndGuildCombatSelection final : public CWndNeuz {
private:	
	u_long   m_uidDefender = -1;
	int      nMaxJoinMember = 0;
	int      nMaxWarMember = 0;
	CTexture m_TexDefender;
	
public: 
	BOOL	Initialize( CWndBase* pWndParent = NULL );
	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void	OnDraw( C2DRender* p2DRender ) override;
	void	OnInitialUpdate() override;

	void			SetDefender( u_long uiPlayer );

	void			SetMemberSize(int nMaxJoin, int nMaxWar);

	void ReceiveLineup(std::span<const u_long> members, u_long defenderId);

private:
	void OnConnectedToCombat();
	bool OnFinish();
	bool OnChooseDefender();
}; 

class CWndGuildCombatState final : public CWndNeuz
{ 
private: 
	CTimeSpan		m_ct;
	time_t    		m_tEndTime; // timegettime+
	time_t    		m_tCurrentTime; // timegettime+
	
public:
	int				m_nCombatType;

public:
	CWndGuildCombatState(int nCombatType);
	
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

	virtual void	PaintFrame( C2DRender* p2DRender );
	virtual BOOL Process ();
	void		 SetGold( int nGold );
	void		 SetRate( int nRate );
	void	 	 SetTime( time_t tTime ) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
}; 

class CWndGuildCombatJoinSelection final : public CWndNeuz
{
protected:
	int	   m_nMapNum = 99;
	CTimer m_timerInputTimeOut;
	DWORD  m_dwOldTime = 0xffffffff;
	
public: 
	BOOL	Initialize(CWndBase * pWndParent = nullptr);
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual BOOL	Process ();

	void			SetTimeOut( DWORD dwSec ) { m_dwOldTime = g_tmCurrent + dwSec; }
	void			SetMapNum( int nMap ) { m_nMapNum = nMap; }
}; 

class CWndGuildWarAppConfirm final : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public: 
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
}; 
class CWndGuildWarCancelConfirm final : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int			m_nCombatType;
public:
	CWndGuildWarCancelConfirm(int nCombatType);
	
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
	virtual void	PaintFrame( C2DRender* p2DRender );
}; 
class CWndGuildWarJoinConfirm final : public CWndNeuz
{
public:
	int			m_nCombatType;
protected:
	CWndText	m_wndText;
public:
	CWndGuildWarJoinConfirm(int nCombatType);
	
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
	virtual void	PaintFrame( C2DRender* p2DRender );
}; 

// 길드 결과 로그창
class CWndGuildCombatResult final : public CWndNeuz 
{ 
public: 
	// 길드 결과 로그 - 길드
	class CWndTabRate final : public CWndNeuz {
	public:
		virtual	void OnInitialUpdate();
	};

	// 길드 결과 로그 - 개인
	class CWndTabLog final : public CWndNeuz {
	public:
		virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
		virtual	void OnInitialUpdate();
	};

	void InsertLog( LPCTSTR str );
	void InsertPersonRate( LPCTSTR str );
	void InsertGuildRate( LPCTSTR str );
	
	CWndTabRate   m_WndGuildCombatTabResultRate;
	CWndTabLog    m_WndGuildCombatTabResultLog;
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual	void OnInitialUpdate(); 
}; 


struct GuildCombatRankInfo {
	GuildCombatRankInfo(int nJob, u_long uidPlayer, int nPoint);

	u_long uidPlayer;
	CString strName;
	int nJob;
	int nPoint;

	void Render(C2DRender * p2DRender, CPoint point, DWORD dwColor, std::optional<size_t> rank) const;
};

// 길드 랭킹 탭- 직업별
class CWndGuildCombatRank_Class final : public CWndNeuz 
{ 
public: 

	static constexpr size_t MAX_GUILDCOMBAT_RANK_PER_PAGE = 11;
	static constexpr size_t MAX_GUILDCOMBAT_RANK = 100;

	CWndScrollBar		m_wndScrollBar;
	std::vector<GuildCombatRankInfo> m_listRank;
	std::optional<size_t> m_nSelect;


	void InsertRank(GuildCombatRankInfo rankInfo);
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDown( UINT nFlags, CPoint point );

private:
	[[nodiscard]] std::optional<size_t> GetSelectIndex(const CPoint & point) const;
}; 

// 길드 랭킹 - 직업별
class CWndGuildCombatRank_Person final : public CWndNeuz {
private:
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Tot;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mer;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mag;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Acr;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Ass;

public:
	void SetList(std::vector<GuildCombatRankInfo> ranking);

	BOOL Initialize(CWndBase * pWndParent = nullptr);
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;

private:
	[[nodiscard]] CWndGuildCombatRank_Class * __GetJobKindWnd(int nJob);
};

class CWndFontEdit final : public CWndNeuz 
{ 
	CTexture * m_pTexture = nullptr;

	struct ManagedColor {
		LONG  xColor = 0;
		CRect rect   = CRect(0, 0, 0, 0);
		FLOAT fColor = 0.f;
	};

	std::array<ManagedColor, 3> m_managed;
	
	bool m_bLButtonClick = false;
	
public: 
	CWndText * m_pWndText = nullptr;
	CWndFontEdit(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point );

private:
	[[nodiscard]] DWORD GetSelectedColor() const;
}; 

/*******************************
	제련 시스템 관련 Window
********************************/
#include "WndSummonAngel.h"

#ifdef __TRADESYS

#define MAX_LIST_COUNT 15

class CWndDialogEvent final : public CWndNeuz
{
public:
	std::vector<int> m_nVecList;
	std::vector<int> m_nDescList;

	int m_nChoiceNum;
	int m_nListCount;
	int m_nDescCount;
	int m_nCurrentPage;
	int m_nMMI;
	int m_nGap;			//List Gap
	int m_nListOffset;	//Selected List Rect Offset
	
public:
	CWndDialogEvent();
	virtual ~CWndDialogEvent(); 
	
	virtual void OnDestroy();
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void SetDescription(CHAR* szChar);
	void SetMMI(int mmi);
	void ReceiveResult(int result);
};
#endif //__TRADESYS

#define MAX_FLOOR_COUNT 15

class CWndHeavenTower final : public CWndNeuz
{
public:
	std::vector<int> m_nVecList;
	std::vector<int> m_nDescList;
	std::vector<int> m_nMsgList;
	std::vector<int>	m_nCost;

	int m_nChoiceNum;
	int m_nListCount;
	int m_nDescCount;
	int m_nCurrentPage;
	int m_nGap;			//List Gap
	int m_nListOffset;	//Selected List Rect Offset
	
public:
	CWndHeavenTower();
	virtual ~CWndHeavenTower(); 
	
	virtual void OnDestroy();
	BOOL Initialize( CWndBase * pWndParent = nullptr );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void InitText();
	void InitWnd();
};

class CWndHeavenTowerEntranceConfirm : public CWndCustomMessageBox
{ 
public: 
	DWORD	m_nFloor;

public:
	void	SetValue( CString str, DWORD nFloor );
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 


class CWndCoupleMessage : public CWndCustomMessageBox
{
public:
	enum { CM_SENDPROPOSE = 0, CM_RECEIVEPROPOSE, CM_CANCELCOUPLE };

	CString m_strText;
	int m_nMode;
	CObj* m_pTargetObj;

public:	
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetMessageMod(CString strText, int nMode, CObj* pTargetObj = NULL);
};

class CWndCoupleTabInfo : public CWndNeuz 
{
public:
	CCouple* m_pCouple;

	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

public: 
	CWndCoupleTabInfo(); 
	virtual ~CWndCoupleTabInfo();
	
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();	
};

class CWndCoupleTabSkill : public CWndNeuz 
{
public:
	CTexture* m_pSkillBgTexture = nullptr;

public: 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
};

class CWndCoupleManager final : public CWndNeuz 
{
public:
	CWndCoupleTabInfo	m_wndCoupleTabInfo;
	CWndCoupleTabSkill	m_wndCoupleTabSkill;
public: 
	CWndCoupleManager(); 
	virtual ~CWndCoupleManager();
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
};

#ifdef __FUNNY_COIN
class CWndFunnyCoinConfirm final : public CWndNeuz 
{
public:
	DWORD m_dwItemId;
	CItemElem* m_pItemElem;

public: 
	CWndFunnyCoinConfirm(); 
	virtual ~CWndFunnyCoinConfirm(); 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	void SetInfo(DWORD dwItemId, CItemElem* pItemElem);
};
#endif //__FUNNY_COIN


class CWndRestateConfirm final : public CWndNeuz
{
public:
	CWndRestateConfirm(DWORD dwItemID);
	virtual ~CWndRestateConfirm(void);

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetInformation(DWORD dwItemObjID, OBJID m_ObjID, int nPart = -1);

private:
	DWORD m_dwItemID;
	DWORD m_dwItemObjID;
	OBJID m_ObjID;
	int m_nPart;
};

class CWndCampusInvitationConfirm final : public CWndNeuz
{
public:
	CWndCampusInvitationConfirm( u_long idSender = 0, const CString& rstrSenderName = _T( "" ) );
	virtual ~CWndCampusInvitationConfirm( void );

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	u_long m_idSender;
	CString m_strSenderName;
};

class CWndCampusSeveranceConfirm final : public CWndNeuz
{
public:
	CWndCampusSeveranceConfirm( u_long idTarget = 0, const CString& rstrTargetName = _T( "" ) );
	virtual ~CWndCampusSeveranceConfirm( void );

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	u_long m_idTarget;
	CString m_strTargetName;
};

#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)

