#if !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "post.h"
#include "guild.h"

#include "couple.h"
#include "couplehelper.h"

const int MAX_WANTED_LIST =	100;

#include "honor.h"
#include "WndSqKComponents.h"

#ifdef __MAIL_REQUESTING_BOX
#include "WndMailRequestingBox.h"
#endif // __MAIL_REQUESTING_BOX

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
class CWndDropItem : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem = nullptr;
	D3DXVECTOR3 m_vPos;
	CWndEdit* m_pEdit = nullptr;

	CWndDropItem() = default;
	~CWndDropItem() override = default;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL Process( void );
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndDropConfirm : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem = nullptr;
	D3DXVECTOR3 m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	
	CWndDropConfirm() = default;
	~CWndDropConfirm() override = default;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL Process( void );
	virtual	void OnInitialUpdate(); 
}; 

class CWndRandomScrollConfirm : public CWndNeuz 
{ 
public:
	BOOL bFlag = FALSE;
	OBJID objid = NULL_ID, objid1 = NULL_ID;
	void SetItem( OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf = FALSE );
public: 
	~CWndRandomScrollConfirm() override = default;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndQuestItemWarning : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem = nullptr;
	D3DXVECTOR3 m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	~CWndQuestItemWarning() override = default;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
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
class CWndInventory : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject*		m_pModel;
	BOOL				m_bLButtonDownRot;
	
	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	CModelObject m_meshStage;
	CItemElem* m_pSelectItem;
	BOOL m_bReport;
	
	CTexture* m_TexRemoveItem;
public:
	void UpDateModel();

	void UpdateParts();
	CModelObject* GetModel() { return m_pModel; }
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
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
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
	virtual BOOL OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message );
	
public:
	void UpdateTooltip( void )	{	m_wndItemCtrl.UpdateTooltip();	}

};

class CWndQueryEquip : public CWndNeuz
{
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject*		m_pModel;
	BOOL				m_bLButtonDownRot;
	DWORD		 m_ObjID;
	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	EQUIP_INFO_ADD	m_aEquipInfoAdd[MAX_HUMAN_PARTS];
public:

	CMover* GetMover() 
	{ 
		if( m_ObjID != NULL_ID )
			return prj.GetMover( m_ObjID ); 

		return NULL;
	}
	void	SetMover( DWORD		 ObjID );
	void	SetEquipInfoAdd( EQUIP_INFO_ADD* aEquipInfoAdd );
	
	CWndQueryEquip();
	virtual ~CWndQueryEquip();
	virtual BOOL Process ();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual void OnMouseMove (UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};


//////////////////////////////////////////////////////////////////////////////////////
// 상태 
//
class CWndStateConfirm : public CWndNeuz 
{ 
public:
	UINT m_nId = 0;
	void OnSetState( UINT nId );
	void SendYes( void );
public: 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 
class CWndCharacterBase : public CWndBase
{
	CTexture m_txInfo;

public:
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
};

class CWndCharacterDetail : public CWndBase
{
	CWndButton m_wndStr;
	CWndButton m_wndSta;
	CWndButton m_wndDex;
	CWndButton m_wndInt;
	
	int			m_nCnt;

public:
	BOOL	m_fWaitingConfirm;
	CWndCharacterDetail();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnMouseWndSurface( CPoint point );

protected:
	void RenderATK( C2DRender* p2DRender, int x, int y );
};

class CWndCharacterDetail2 : public CWndBase
{
public:
	CWndButton m_wndStrPlus, m_wndStrMinus;
	CWndButton m_wndStaPlus, m_wndStaMinus;
	CWndButton m_wndDexPlus, m_wndDexMinus;
	CWndButton m_wndIntPlus, m_wndIntMinus;
	
	CWndButton m_wndApply, m_wndReset;
	
	CWndEdit m_editStrCount;
	CWndEdit m_editStaCount;
	CWndEdit m_editDexCount;
	CWndEdit m_editIntCount;

	int m_nStrCount = 0;
	int m_nStaCount = 0;
	int m_nDexCount = 0;
	int m_nIntCount = 0;

	int m_nGpPoint = 0;

	int m_nATK = 0;
	int m_nDEF = 0;
	int m_nCritical = 0;
	int m_nATKSpeed = 0;
	
public:
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL Process ();
	
	void RefreshStatPoint();
	void GetVirtualATK(int* pnMin, int* pnMax);
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();

protected:
	void RenderATK( C2DRender* p2DRender, int x, int y );
};

class CWndChangeJob;

class CWndPvpBase : public CWndBase
{
	CTexture m_txInfo;
	
public:
	CWndButton m_wndChangeJob;
	CWndChangeJob* m_pWndChangeJob;
	BOOL	m_fWaitingConfirm;
	BOOL m_bExpert;
	int m_nDisplay;			// 직업창 화면 옵션

	CWndPvpBase();
	virtual ~CWndPvpBase();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
};

class CWndChangeJob : public CWndNeuz
{
public:
	int m_nJob;
	CWndButton m_wndExpert[ 6 ];
	CWndButton m_wndOk;
	CWndButton m_wndCancel;

	CWndChangeJob( int nJob );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
};

class CWndCharInfo : public CWndBase
{

public:
	CWndButton		m_wndChangeJob;
	CWndChangeJob*	m_pWndChangeJob;
	BOOL			m_fWaitingConfirm;
	BOOL			m_bExpert;
	int				m_nDisplay;

	CWndButton m_wndStrPlus, m_wndStrMinus;
	CWndButton m_wndStaPlus, m_wndStaMinus;
	CWndButton m_wndDexPlus, m_wndDexMinus;
	CWndButton m_wndIntPlus, m_wndIntMinus;
	
	CWndButton m_wndApply, m_wndReset;
	
	CWndEdit m_editStrCount;
	CWndEdit m_editStaCount;
	CWndEdit m_editDexCount;
	CWndEdit m_editIntCount;

	int m_nStrCount;
	int m_nStaCount;
	int m_nDexCount;
	int m_nIntCount;

	int m_nGpPoint;

	int m_nATK;
	int m_nDEF;
	int m_nCritical;
	int m_nATKSpeed;

public:

	CWndCharInfo();
	virtual ~CWndCharInfo();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL Process ();
	void RefreshStatPoint();
	void GetVirtualATK(int* pnMin, int* pnMax);
	int GetVirtualDEF();
	int GetVirtualCritical();
	float GetVirtualATKSpeed();

protected:
	void RenderATK( C2DRender* p2DRender, int x, int y );
};

class CWndHonor : public CWndNeuz
{
private:
	int						m_nSelectedId = 0;
	int						m_nCount = 0;
	std::vector<EarnedTitle>		m_vecTitle;
public:
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	void RefreshList();
};

class CWndCharacter : public CWndNeuz
{
public:
	CWndCharInfo		m_wndCharInfo;
	CWndHonor			m_wndHonor;
public:
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
};



//////////////////////////////////////////////////////////////////////////////////////
// 스킬 EX 
//

#include "WndSkillTree.h"
#include <optional>

//////////////////////////////////////////////////////////////////////////////////////
// 매신저  
//

//////////////////////////////////////////////////////////////////////////////////////
// Navigator
//

class CWndNavigator : public CWndNeuz
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
	DWORD		 m_iFrame;
	DWORD		 m_iPastTime;
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
	void RenderMark( C2DRender* p2DRender, CMover* Player );	//	참조되는 변수는 플레이어와 파티플레이어를 얻을 수 있는 것이어야 하지만 
																//	현재는 자신만을 찍는 것을 하고 그 다음에 파티 플레이어들을 찾을수 있는 방법을 찾아 보자꾸나
	void RenderMarkAll( C2DRender* p2DRender , CMover* Player );
	CBillboard m_billArrow;
	CSize      m_size;
	int        m_nSizeCnt;
	TCHAR      m_szName[ 64 ];
	void ResizeMiniMap();
	
	CTexture m_GuildCombatTextureMask;
	CTexture* m_pDestinationPositionTexture;

public:
	BOOL m_bObjFilterPlayer ;
	BOOL m_bObjFilterParty  ;
	BOOL m_bObjFilterNPC    ;
	BOOL m_bObjFilterMonster;
	

	void RenderPartyMember( C2DRender* p2DRender, TEXTUREVERTEX** pVertices, CRect rect, D3DXVECTOR3 vPos, u_long uIdPlayer, LPCTSTR lpStr );
	void SetRegionName( const TCHAR *tszName );		// 지역
		
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	
	CWndNavigator(); 
	virtual ~CWndNavigator();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
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

class CWndStatus : public CWndNeuz
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
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
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

class CWndQuit : public CWndMessageBox
{
	BOOL	m_bFlag;	
public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	
class CWndLogOut : public CWndMessageBox
{
public:
	CWndLogOut() { }
	virtual ~CWndLogOut() { }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	


class CWndLogOutWaitting : public CWndNeuz
{
	BOOL  m_bIsLogOut;
	DWORD dwTime;
public:
	void	SetIsLogOut(BOOL bFlag) { m_bIsLogOut = bFlag; }
	virtual BOOL Process ();
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual	void OnInitialUpdate();
};	

class CWndTradeGold : public CWndNeuz 
{ 
public: 
	UINT m_nIdWndTo;
	SHORTCUT m_Shortcut;
	D3DXVECTOR3 m_vPos;
	DWORD m_dwGold = 0;
	CWndBase* m_pWndBase = nullptr;
	CWndStatic* pStatic;
	CWndStatic* pStaticCount;
	char m_nSlot = 0;
	char m_nPutSlot = 100;
	CWndTradeGold(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL Process( void );
	virtual	void OnInitialUpdate(); 
};

class CWndConfirmTrade : public CWndNeuz 
{ 
	OBJID m_objid = 0;
public: 

	void OnSetName( const char* szName, OBJID objid );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndTradeConfirm : public CWndNeuz 
{ 
public: 
	BOOL bMsg;
	CWndTradeConfirm(); 
	virtual ~CWndTradeConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndTrade : public CWndNeuz {
public:
	CWndTradeCtrl m_wndItemCtrlYou;
	CWndTradeCtrl m_wndItemCtrlI;
	int           m_nGoldI, m_nGoldYou;

	CWndTrade();
	~CWndTrade() override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;


	void DoCancel();

};

//////////

class CWndRevival : public CWndNeuz 
{ 
public: 
	CWndButton* m_pLodeLight = nullptr;
	CWndButton* m_pLodeStar  = nullptr;
	CWndButton* m_pRevival   = nullptr;
	CWndButton* m_pShop      = nullptr;
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	void	EnableButton( int nButton, BOOL bEnable );
}; 

class CWndReWanted : public CWndNeuz 
{ 
private:
	CString      m_strWanted;
	
public: 
	BOOL         CheckWantedInfo( int nGold, LPCTSTR szMsg );
	void		 SetWantedName( LPCTSTR szWanted );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndResurrectionConfirm : public CWndNeuz 
{ 
public: 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndWantedConfirm : public CWndNeuz 
{ 
	int		 m_nGold = 0;
	char	 m_szName[ MAX_NAME ];
public: 
	CWndWantedConfirm(); 
	
	void		 SetInfo( const char szName[], const int nGold );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 


struct WANTEDLIST
{
	char	 szName[ MAX_NAME ];		// 이름.
	__int64	 nGold;						// 현상금
	char     szDate[32];
	char	 szMsg[WANTED_MSG_MAX + 1];	// 십자평 
};

class CWndWanted : public CWndNeuz 
{ 
private: 
	CWndWantedConfirm*  m_pWantedConfirm;
	time_t				m_recvTime;
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	WANTEDLIST			m_aList[MAX_WANTED_LIST];
	int					m_nSelect;

public: 
	CWndWanted(); 
	virtual ~CWndWanted(); 
	
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertWanted( const char szName[], __int64 nGold,  int nTime, const char szMsg[] );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 


class CWndCommItemDlg : public CWndNeuz 
{ 
public: 
	CWndEdit*		m_pWndEdit = nullptr;
	DWORD			m_dwObjId  = NULL_ID;
	DWORD			m_dwCtrlId = NULL_ID;
	void			SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndChangeClassGeneric : public CWndNeuz {
public:
	struct PossibleJob {
		UINT widgetId;
		int jobId;
	};

private:
	int m_currentJobId;
	std::vector<PossibleJob> m_allJobs;

	void OnModifiedJob();
	
protected:
	CWndChangeClassGeneric(std::vector<PossibleJob> allJobs)
		: m_currentJobId(0), m_allJobs(allJobs) {
	}

public:
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
};

class CWndChangeClass1 : public CWndChangeClassGeneric {
public:
	CWndChangeClass1();
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
};

class CWndChangeClass2 : public CWndChangeClassGeneric {
public:
	CWndChangeClass2();
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
};

class CWndPostSendConfirm;

class CWndPostSend : public CWndNeuz
{
	int		m_nCost = 0;
	int		m_nCount = 0;
	BYTE  m_nItem = 0xFF;

public:	
	void ClearData();
	virtual ~CWndPostSend(); 
	
	
	BYTE GetItemId() { return m_nItem; }
	void SetItemId( BYTE nId );
	void SetCost( int nCost ) { m_nCost = nCost; }
	void SetCount( int nCount ) { m_nCount = nCount; }
	void SetReceive( const char* pchar );
	void SetTitle( const char* pchar );	
	void SetText( const char* pchar );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
};

class CWndPostItemWarning : public CWndNeuz 
{ 
public: 
	int	   m_nMailIndex = -1;
	
	void	SetIndex( int nIndex ) { m_nMailIndex = nIndex; }
	void	SetString( char* string );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndPostDeleteConfirm;
class CWndPostRead : public CWndNeuz
{
	int					m_nMailIndex;
	CWndGold			m_wndGold;
	BOOL				m_bDrag;
	CWndPostDeleteConfirm* m_pDeleteConfirm;
	
public:	
	CWndPostItemWarning*		m_pWndPostItemWarning;
	void MailReceiveItem();
	void MailReceiveGold();
	void ClearData();
	CWndPostRead(); 
	virtual ~CWndPostRead(); 
	
	void SetValue( int nMailIndex );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );	
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};


class CWndPostReceive : public CWndNeuz
{
	CTexture			m_Texture[3];
	CWndScrollBar		m_wndScrollBar;
	int					m_nSelect = -1;
	int					m_nMax = 0;
	CWndPostRead*		m_wndPostRead = nullptr;
public:	
	void UpdateScroll();
	
	virtual ~CWndPostReceive(); 

	int          GetSelectIndex( const CPoint& point );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnMouseWndSurface( CPoint point );
};
class CWndPost : public CWndNeuz
{
public:
	CWndPostSend		m_PostTabSend;
	CWndPostReceive		m_PostTabReceive;
	
	virtual ~CWndPost(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

#ifdef __MAIL_REQUESTING_BOX
public:
	void LoadLastMailBox( void );
	void SaveLastMailBox( void );
	void CloseMailRequestingBox( void );

private:
	CWndMailRequestingBox* m_pWndMailRequestingBox = nullptr;
#endif // __MAIL_REQUESTING_BOX
};

class CWndPostDeleteConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int		m_nIndex = 0;
public: 
	
	void			SetValue( int nIndex ) { m_nIndex = nIndex; }
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
}; 

struct GUILDLIST
{
	char	 szGuild[ MAX_NAME ];		// 이름.
	char	 szName[ MAX_NAME ];		// 이름.
	int		 nNum;
};

class CWndGuildCombatOffer : public CWndNeuz
{
protected:
	DWORD			m_dwReqGold = 0;
	DWORD			m_dwMinGold = 0;
	DWORD			m_dwBackupGold = 0;

public: 
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
	void			SetGold( DWORD nCost );
	void			SetTotalGold( __int64 nCost );
	void			SetMinGold( DWORD dwMinGold )		{ m_dwMinGold    = dwMinGold; }
	void			SetReqGold( DWORD dwReqGold )		{ m_dwReqGold    = dwReqGold; }
	void			SetBackupGold( DWORD dwBackupGold ) { m_dwBackupGold = dwBackupGold; }
	void			EnableAccept( BOOL bFlag );	
}; 

class CWndGuildCombatBoard : public CWndNeuz
{
protected:
public:
	int m_nCombatType;
public: 
	CWndGuildCombatBoard(int m_nCombatType);
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual void	PaintFrame( C2DRender* p2DRender );
	void			SetString( CHAR* szChar );
}; 

class CGuildCombatSelectionClearMessageBox : public CWndMessageBox
{ 
public: 
	CString m_strMsg;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CGuildCombatInfoMessageBox : public CWndNeuz
{
public:
	int	m_nCombatType;
public:
	CGuildCombatInfoMessageBox(int nCombatType);
	void	SetString( CHAR* szChar );
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void PaintFrame( C2DRender* p2DRender );
	void	SetString( CString strMsg );
}; 

class CGuildCombatInfoMessageBox2 : public CWndNeuz
{ 
public: 
	void	SetString( CHAR* szChar );
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

struct GuildCombatPlayer {
	CString display;
	u_long playerId;

	explicit GuildCombatPlayer(u_long playerId);

	void Render(
		C2DRender * p2DRender, CRect rect,
		DWORD color, const WndTListBox::DisplayArgs & misc
	) const;

	struct ById {
		u_long playerId;

		explicit ById(u_long playerId) : playerId(playerId) {}

		[[nodiscard]] bool operator()(const GuildCombatPlayer & gcp) const {
			return gcp.playerId == playerId;
		}
	};
};


class CWndGuildCombatSelection : public CWndNeuz {
private:	
	u_long   m_uidDefender = -1;
	int      nMaxJoinMember = 0;
	int      nMaxWarMember = 0;
	CTexture m_TexDefender;
	
public: 
	CWndGuildCombatSelection();
	
	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override;
	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void	OnDraw( C2DRender* p2DRender ) override;
	void	OnInitialUpdate() override;
	void			EnableFinish( BOOL bFlag );		

	void Reset();
	void			SetDefender( u_long uiPlayer );
	void			UpDateGuildListBox();

	void			SetMemberSize(int nMaxJoin, int nMaxWar);

	void ReceiveLineup(const std::vector<u_long> & members, u_long defenderId);

private:
	CWndTListBox<GuildCombatPlayer> & SelectablePlayers();
	CWndTListBox<GuildCombatPlayer> & CombatPlayers();

	bool OnConnectedToCombat();
	bool OnCombatToConnected();
	bool OnMoveUp();
	bool OnMoveDown();
	bool OnFinish();
	bool OnChooseDefender();
}; 

class CWndGuildCombatState : public CWndNeuz 
{ 
private: 
	CTimeSpan		m_ct;
	time_t    		m_tEndTime; // timegettime+
	time_t    		m_tCurrentTime; // timegettime+
	
public:
	int				m_nCombatType;

public:
	CWndGuildCombatState(int nCombatType);
	virtual ~CWndGuildCombatState(); 
	
	void		 InsertTitle( const char szTitle[] );
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
	virtual void	PaintFrame( C2DRender* p2DRender );
	virtual BOOL Process ();
	void		 SetGold( int nGold );
	void		 SetRate( int nRate );
	void	 	 SetTime( time_t tTime ) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
}; 

class CWndGuildCombatJoinSelection : public CWndNeuz
{
protected:
	int	   m_nMapNum = 99;
	CTimer m_timerInputTimeOut;
	DWORD  m_dwOldTime = 0xffffffff;
	
public: 
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual BOOL	Process ();

	void			SetTimeOut( DWORD dwSec ) { m_dwOldTime = g_tmCurrent + dwSec; }
	void			SetMapNum( int nMap ) { m_nMapNum = nMap; }
}; 

class CWndGuildWarAppConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public: 
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
}; 
class CWndGuildWarCancelConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int			m_nCombatType;
public:
	CWndGuildWarCancelConfirm(int nCombatType);
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
	virtual void	PaintFrame( C2DRender* p2DRender );
}; 
class CWndGuildWarJoinConfirm : public CWndNeuz
{
public:
	int			m_nCombatType;
protected:
	CWndText	m_wndText;
public:
	CWndGuildWarJoinConfirm(int nCombatType);
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnInitialUpdate();
	virtual void	PaintFrame( C2DRender* p2DRender );
}; 

class CWndGuildWarState : public CWndNeuz 
{ 
private: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	GUILDLIST			m_aList[MAX_WANTED_LIST];
	int					m_nSelect;
	
public: 
	CWndGuildWarState(); 
	virtual ~CWndGuildWarState(); 
	
	void		 InsertTitle( const char szTitle[] );
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 


struct GUILDNAME
{
	char	 szGuild[ MAX_NAME ];		// 이름.
};

class CWndGuildCombatRanking : public CWndNeuz 
{ 
private: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	std::multimap< int, GUILDNAME > m_multimapRanking;
	int					m_nSelect;
	
public: 
	void SortRanking();
	CWndGuildCombatRanking(); 
	virtual ~CWndGuildCombatRanking(); 
	
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], int nWinCount );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 

// 길드 결과 로그 - 길드
class CWndGuildCombatTabResultRate : public CWndNeuz 
{ 
public: 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate(); 
}; 
// 길드 결과 로그 - 개인
class CWndGuildCombatTabResultLog : public CWndNeuz 
{ 
public: 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

// 길드 결과 로그창
class CWndGuildCombatResult : public CWndNeuz 
{ 
public: 
	void InsertLog( CString str );
	void InsertPersonRate( CString str );
	void InsertGuildRate( CString str );
	CWndGuildCombatResult(); 
	virtual ~CWndGuildCombatResult(); 
	
	CWndGuildCombatTabResultRate   m_WndGuildCombatTabResultRate;
	CWndGuildCombatTabResultLog    m_WndGuildCombatTabResultLog;
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );
}; 



typedef struct __GUILDCOMBAT_RANK_INFO
{
	u_long	uidPlayer;
	int		nJob;
} __GUILDCOMBAT_RANK_INFO;

typedef struct __GUILDCOMBAT_RANK_INFO2
{
	CString strName;
	CString strJob;
	u_long	uidPlayer;
	int		nPoint;	
} __GUILDCOMBAT_RANK_INFO2;

#define MAX_GUILDCOMBAT_RANK_PER_PAGE 11
#define MAX_GUILDCOMBAT_RANK		  100


// 길드 랭킹 탭- 직업별
class CWndGuildCombatRank_Class : public CWndNeuz 
{ 
public: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	int					m_nSelect;

	int		m_nRate;
	int		m_nOldRate;

	__GUILDCOMBAT_RANK_INFO2		m_listRank[MAX_GUILDCOMBAT_RANK];

	CWndGuildCombatRank_Class(); 
	virtual ~CWndGuildCombatRank_Class(); 

	void		 InsertRank( int nJob, u_long uidPlayer, int nPoint );
	int          GetSelectIndex( const CPoint& point );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDown( UINT nFlags, CPoint point );
}; 

// 길드 랭킹 - 직업별
class CWndGuildCombatRank_Person : public CWndNeuz 
{ 
public: 
	std::multimap< int, __GUILDCOMBAT_RANK_INFO > m_mTotalRanking;
			
	CWndGuildCombatRank_Person(); 
	virtual ~CWndGuildCombatRank_Person(); 
	
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Tot;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mer;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Mag;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Acr;
	CWndGuildCombatRank_Class    m_WndGuildCombatTabClass_Ass;
	
	CWndGuildCombatRank_Class*  __GetJobKindWnd( int nJob );
	void						InsertRank( int nJob, u_long	uidPlayer, int nPoint );
	void						UpdateList();
	void						DivisionList();
	void						UpdatePlayer( int nJob, u_long idPlayer );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );

}; 

class CWndFontEdit : public CWndNeuz 
{ 
	CTexture*        m_pTexture;
	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];
	
	CRect			 m_ColorRect[3];
	FLOAT			 m_fColor[3];
	
	BOOL			 m_bLButtonClick;
	
public: 
	CWndText* m_pWndText;
	CWndFontEdit(); 
	virtual ~CWndFontEdit(); 
	
	void	ReSetBar( FLOAT r, FLOAT g, FLOAT b );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point );
}; 


#include "WndWorld.h"

class CWndBuffStatus : public CWndNeuz 
{
public:
	std::vector< std::multimap<DWORD, BUFFSKILL> > m_pBuffTexture;
	std::list<BUFFICON_INFO> m_pBuffIconInfo;

	int m_BuffIconViewOpt;
public:
	CWndBuffStatus(); 
	virtual ~CWndBuffStatus(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnRButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );

#ifdef __BUFF_1107
	void RenderBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse );
#else	// __BUFF_1107
	void RenderBuffIcon( C2DRender *p2DRender, SKILLINFLUENCE* pSkill, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse );
#endif	// __BUFF_1107
	void RenderOptBuffTime(C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor );
	void SetBuffIconInfo();
	BOOL GetHitTestResult();
};

/*******************************
	제련 시스템 관련 Window
********************************/
#include "WndSummonAngel.h"

class CWndMixJewelConfirm : public CWndNeuz
{
public: 
	CWndMixJewelConfirm(); 
	virtual ~CWndMixJewelConfirm(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

#define ITEM_VALID			0
#define ITEM_MAX_OVERFLOW	1
#define ITEM_INVALID		2

struct GENMATDIEINFO {
	LPWNDCTRL wndCtrl;
	BOOL isUse;
	int staticNum;
	CItemElem * pItemElem;
};


class CWndMixJewel : public CWndNeuz 
{ 
public:
	static constexpr unsigned int MaxSlotPerItem = 5;
	static constexpr UINT StartOffsetWidcSlots = 1500;

	class CWndOrichalcumReceiver : public CWndItemReceiver {
	public:
		CWndOrichalcumReceiver();

		bool CanReceiveItem(const CItemElem & itemElem, bool) override {
			return ItemProps::IsOrichalcum(itemElem);
		}
	};

	class CWndMoonstoneReceiver : public CWndItemReceiver {
	public:
		CWndMoonstoneReceiver();

		bool CanReceiveItem(const CItemElem & itemElem, bool) override {
			return ItemProps::IsMoonstone(itemElem);
		}
	};

	CWndText* m_pText = nullptr;
	std::array<CWndOrichalcumReceiver, MaxSlotPerItem> m_oriReceivers;
	std::array<CWndMoonstoneReceiver, MaxSlotPerItem> m_moonReceivers;
	

	BOOL m_bStart = FALSE;
	
	CWndInventory* m_pWndInventory = nullptr;
	std::unique_ptr<CWndMixJewelConfirm> m_pConfirm = nullptr;
public: 

	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override;
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnInitialUpdate() override;
	void OnDestroy() override;
	void OnDestroyChildWnd( CWndBase * pWndChild ) override;	// cr : uw :
	
	void ReceiveResult(CUltimateWeapon::Result nResult);
	void SetJewel(CItemElem* pItemElem);
	void SetConfirmInit();

private:

	template <typename F> void ForEachReceiver(F f) {
		for (auto & receiver : m_oriReceivers) { f(receiver); }
		for (auto & receiver : m_moonReceivers) { f(receiver); }
	}

	[[nodiscard]] std::optional<std::array<OBJID, MAX_JEWEL>> GetAllObjidIfFilled() const;
	
	void UpdateStartButton();

};

#include "UltimateWeapon.h"

class CWndExtraction : public CWndNeuz {
public:
	class Receiver : public CWndItemReceiver {
		bool CanReceiveItem(const CItemElem & itemElem, bool) override;
	};

private:
	static constexpr UINT WIDC_Receiver = 1500;

	Receiver m_receiver;

public: 
	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
	
	void ReceiveResult(CUltimateWeapon::MakeGemAnswer result);
	void SetWeapon(CItemElem & pItemElem);
};

class CWndSmeltJewel : public CWndNeuz
{
public:
	CModelObject* m_pMainItem;
	CItemElem* m_pItemElem;
	CItemElem* m_pJewelElem;
	CWndText* m_pText;
	
	DWORD m_dwJewel[5];
	int m_nJewelSlot[5];
	int m_nJewelCount;
	int m_nUsableSlot;
	OBJID m_objJewelId;

	float m_fRotate;
	float m_fAddRot;

	int m_nStatus;
	int m_nCount;
	int m_nDelay;
	int m_nAlpha;
	float m_nEyeYPos;
	BOOL m_bFlash;
	
public:
	CWndSmeltJewel(); 
	~CWndSmeltJewel();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void OnDestroy( void );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL Process ();
	
	void SetDescription(CHAR* szChar);
	void ReceiveResult(int result);
	void InitializeJewel(CItemElem* pItemElem);
	void SetJewel(CItemElem* pItemElem);
};

class CWndChangeWeapon : public CWndNeuz
{
public:
	class CWeaponReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	class COrichalcum2Receiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	class CJewelReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	static constexpr UINT WIDC_WeaponReceiver = 1500;
	static constexpr UINT WIDC_OrichalcumReceiver = 1501;
	static constexpr UINT WIDC_JewelReceiver = 1502;

	CWeaponReceiver m_weaponReceiver;
	COrichalcum2Receiver m_orichalcumReceiver;
	CJewelReceiver m_jewelReceiver;

	int m_nWeaponType;
	bool m_bIsSendChange;
	
public:
	CWndChangeWeapon(int nType);
	
	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
	
	void ReceiveResult(int result);
	void SetItem(CItemElem & pItemElem);

private:
	void SetupText();
	void UpdateStartButtonStatus();
};

class CWndRemoveJewelConfirm : public CWndNeuz 
{
public:
	CWndInventory* m_pInventory;
	CItemElem *	m_pUpgradeItem;

public: 
	CWndRemoveJewelConfirm(); 
	virtual ~CWndRemoveJewelConfirm(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

	void SetItem(CItemElem *	m_pItem);
};

/*******************************
	전승 시스템 관련 Window
********************************/

class CWndHeroSkillUp : public CWndNeuz {
public:
	static constexpr UINT WIDC_Receivers = 1500;

	struct IconDraw {
		CPoint topLeft = CPoint(0, 0);
		CTexture * texture = nullptr;
	};

private:
	CWndOnlyOneItemReceiver m_rDiamond;
	CWndOnlyOneItemReceiver m_rEmerald;
	CWndOnlyOneItemReceiver m_rSapphire;
	CWndOnlyOneItemReceiver m_rRuby;
	CWndOnlyOneItemReceiver m_rTopaz;
	std::array<IconDraw, 5> m_legend;

	bool m_bSendHeroSkillup = false;

	[[nodiscard]] static CWndItemReceiver::Features GetDrawFeatures() {
		return CWndItemReceiver::Features{
			.colorWhenHoverWithItem = 0x60ffff00
		};
	}

	[[nodiscard]] std::optional<std::array<OBJID, 5>> ReceiversToObjid() const;
	void UpdateOkButton();
	
public:
	CWndHeroSkillUp();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void ReceiveResult(int nresult);
	void SetJewel(CItemElem* pItemElem);
};


#ifdef __TRADESYS

#define MAX_LIST_COUNT 15

class CWndDialogEvent : public CWndNeuz
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
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void SetDescription(CHAR* szChar);
	void SetMMI(int mmi);
	void ReceiveResult(int result);
};
#endif //__TRADESYS

#define MAX_FLOOR_COUNT 15

class CWndHeavenTower : public CWndNeuz
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
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void InitText();
	void InitWnd();
};

class CWndHeavenTowerEntranceConfirm : public CWndMessageBox
{ 
public: 
	DWORD	m_nFloor;

public:
	void	SetValue( CString str, DWORD nFloor );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 




class CWndRemoveAttribute : public CWndNeuz
{
public:
	class CWndAttributedItem : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	class CWndConfirm : public CWndNeuz {
	public:
		BOOL Initialize(CWndBase * pWndParent, DWORD _ = MB_OK) override;
		BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
		void OnInitialUpdate() override;
	};
	
public:
	CWndAttributedItem m_receiver;
	CWndConfirm * m_pWndConfirm = nullptr;

public: 
	~CWndRemoveAttribute() override; 
	
	BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
	
	void SetDescription(CHAR* szChar);
	void ReceiveResult(BOOL result);
	void SetWeapon(CItemElem* pItemElem);
};


class CWndRemovePiercing : public CWndNeuz
{
public:
	static constexpr UINT WIDC_Receiver = 901;

	class CWndPiercedItemReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	CWndPiercedItemReceiver m_receiver;
	
public: 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void SetDescription(CHAR* szChar);
	void SetItem(CItemElem* pItemElem);
};

class CWndRemoveJewel : public CWndNeuz
{
public:
	class CWndJeweledItem : public CWndItemReceiver {
	public:
		static bool IsAWeapon(const ItemProp * itemProp);
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	class CWndMoonstoneReceiver : public CWndItemReceiver {
	public:
		CWndMoonstoneReceiver();
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	struct DisplayedJewel {
		UINT slotWID;
		UINT infoWID;

		DWORD jewelItemID;
		CTexture * texture;
	};

private:
	static constexpr UINT WIDC_Weapon = 1500;
	static constexpr UINT WIDC_Moon = 1501;

	CWndJeweledItem m_weaponReceiver;
	CWndMoonstoneReceiver m_moonstoneReceiver;

	std::array<DisplayedJewel, 5> m_displayed;
	
public: 
	CWndRemoveJewel(); 

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnDraw( C2DRender* p2DRender ) override;
	void OnInitialUpdate() override;
	void OnMouseWndSurface(CPoint point) override;

	void SetItem(CItemElem * pItemElem);

private:
	void ResetJewel();
	[[nodiscard]] static std::pair<DWORD, DWORD> GetTextAndColorOfJewel(DWORD jewelId);
	void UpdateDisplayedJewel();
	void UpdateStartButtonStatus();
};

class CWndChangeAttribute : public CWndNeuz 
{
public:
	CWndText* m_pText;
	int m_nAttributeNum;
	int m_nAttributeStaticID[5];
	int m_nTooltipTextIndx[6];
	CItemElem* m_pItemElem;
	CItemElem* m_pChangeItem;
	CTexture* m_pTexture;

public: 
	CWndChangeAttribute(); 
	virtual ~CWndChangeAttribute();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL Process();

public:
	void SetChangeItem( CItemElem* pItemElem );
	void FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend);
};

class CWndCoupleMessage : public CWndMessageBox
{
public:
	enum { CM_SENDPROPOSE = 0, CM_RECEIVEPROPOSE, CM_CANCELCOUPLE };

	CString m_strText;
	int m_nMode;
	CObj* m_pTargetObj;

public:	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
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
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
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
	CWndText* m_pText;
	CTexture* m_pSkillBgTexture;

public: 
	CWndCoupleTabSkill(); 
	virtual ~CWndCoupleTabSkill();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
};

class CWndCoupleManager : public CWndNeuz 
{
public:
	CWndCoupleTabInfo	m_wndCoupleTabInfo;
	CWndCoupleTabSkill	m_wndCoupleTabSkill;
public: 
	CWndCoupleManager(); 
	virtual ~CWndCoupleManager();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
};

#ifdef __FUNNY_COIN
class CWndFunnyCoinConfirm : public CWndNeuz 
{
public:
	DWORD m_dwItemId;
	CItemElem* m_pItemElem;

public: 
	CWndFunnyCoinConfirm(); 
	virtual ~CWndFunnyCoinConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	void SetInfo(DWORD dwItemId, CItemElem* pItemElem);
};
#endif //__FUNNY_COIN


class CWndSmeltSafety : public CWndNeuz
{
public:
	enum WndMode { WND_NORMAL, WND_ACCESSARY, WND_PIERCING, WND_ELEMENT };
	enum { SMELT_MAX = 10 };
	enum { ENCHANT_TIME = 2 };
	enum { EXTENSION_PIXEL = 32, HALF_EXTENSION_PIXEL = EXTENSION_PIXEL / 2 };
	enum { GENERAL_NON_USING_SCROLL2_LEVEL = 7, ELEMENTAL_NON_USING_SCROLL2_LEVEL = 10 };

private:
	WndMode m_eWndMode;
	CItemElem* m_pItemElem;
	CTexture* m_pItemTexture;
	CTexture* m_pNowGaugeTexture;
	CTexture* m_pSuccessTexture;
	CTexture* m_pFailureTexture;
	int m_nMaterialCount;
	int m_nScroll1Count;
	int m_nScroll2Count;
	int m_nResultCount;
	BOOL m_bStart;
	bool m_bResultSwitch;
	DWORD m_dwEnchantTimeStart;
	DWORD m_dwEnchantTimeEnd;
	float m_fGaugeRate;
	int m_nValidSmeltCounter;
	int m_nCurrentSmeltNumber;
	int m_nResultStaticID[SMELT_MAX];
	GENMATDIEINFO m_Material[SMELT_MAX];
	GENMATDIEINFO m_Scroll1[SMELT_MAX];
	GENMATDIEINFO m_Scroll2[SMELT_MAX];
	bool m_bResultStatic[SMELT_MAX];
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferSuccessImage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferFailureImage;
	ItemProp* m_pSelectedElementalCardItemProp;

public:
	CWndSmeltSafety(WndMode eWndMode);
	virtual ~CWndSmeltSafety();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL Process();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void SetItem(CItemElem* pItemElem);
	void RefreshInformation(void);
	void RefreshText(void);
	void RefreshValidSmeltCounter(void);
	void StopSmelting(void);
	void DisableScroll2(void);
	void ResetData(void);
	void AddListItem(GENMATDIEINFO* pListItem, CItemElem* pItemElem);
	void SubtractListItem(GENMATDIEINFO* pListItem);
	void DrawListItem(C2DRender* p2DRender);
	BOOL IsDropMaterialZone(CPoint point);
	BOOL IsDropScroll1Zone(CPoint point);
	BOOL IsDropScroll2Zone(CPoint point);
	BOOL IsAcceptableMaterial(ItemProp* pItemProp);
	BOOL IsAcceptableScroll1(ItemProp* pItemProp);
	BOOL IsAcceptableScroll2(ItemProp* pItemProp);
	int GetNowSmeltValue(void);
	int GetDefaultMaxSmeltValue(void);
	void SetResultSwitch(bool bResultSwitch) { m_bResultSwitch = bResultSwitch; }
	void SetCurrentSmeltNumber(int nCurrentSmeltNumber) { m_nCurrentSmeltNumber = nCurrentSmeltNumber; }
	void SetResultStatic(bool bResultStatic, int nIndex) { m_bResultStatic[nIndex] = bResultStatic; }
	CItemElem* GetItemElement(void) const { return m_pItemElem; }
	bool GetResultSwitch(void) const { return m_bResultSwitch; }
	int GetCurrentSmeltNumber(void) const { return m_nCurrentSmeltNumber; }
	int GetResultStaticID(int nIndex) const { return m_nResultStaticID[nIndex]; }
};

class CWndSmeltSafetyConfirm : public CWndNeuz
{
public:
	enum ErrorMode { WND_ERROR1, WND_ERROR2, WND_ERROR3 };

private:
	ErrorMode m_eErrorMode;
	CItemElem* m_pItemElem;

public:
	CWndSmeltSafetyConfirm(ErrorMode eErrorMode);
	virtual ~CWndSmeltSafetyConfirm();

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroy( void );

	void SetWndMode(CItemElem* pItemElem);
};


class CWndEquipBindConfirm : public CWndNeuz
{
public:
	enum EquipAction { EQUIP_DOUBLE_CLICK, EQUIP_DRAG_AND_DROP };

public:
	CWndEquipBindConfirm(EquipAction eEquipAction);
	virtual ~CWndEquipBindConfirm(void);

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetInformationDoubleClick(CItemElem * pItemBase, DWORD dwObjId);
	void SetInformationDragAndDrop(CItemElem * pItemElem);

private:
	void EquipItem( void );

private:
	EquipAction m_eEquipAction;
	CItemElem * m_pItemBase;
	DWORD m_dwObjId;
	CItemElem* m_pItemElem;
};

class CWndRestateConfirm : public CWndNeuz
{
public:
	CWndRestateConfirm(DWORD dwItemID);
	virtual ~CWndRestateConfirm(void);

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
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

class CWndCampusInvitationConfirm : public CWndNeuz
{
public:
	CWndCampusInvitationConfirm( u_long idSender = 0, const CString& rstrSenderName = _T( "" ) );
	virtual ~CWndCampusInvitationConfirm( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	u_long m_idSender;
	CString m_strSenderName;
};

class CWndCampusSeveranceConfirm : public CWndNeuz
{
public:
	CWndCampusSeveranceConfirm( u_long idTarget = 0, const CString& rstrTargetName = _T( "" ) );
	virtual ~CWndCampusSeveranceConfirm( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	u_long m_idTarget;
	CString m_strTargetName;
};

#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)

