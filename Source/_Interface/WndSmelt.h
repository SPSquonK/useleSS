#pragma once

#include "WndField.h"
#include "UltimateWeapon.h"

class CWndRandomScrollConfirm final : public CWndNeuz
{ 
public:
	BOOL bFlag = FALSE;
	OBJID objid = NULL_ID, objid1 = NULL_ID;
	void SetItem( OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf = FALSE );
public: 
	~CWndRandomScrollConfirm() override = default;
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndMixJewelConfirm final : public CWndNeuz {
public:
	BOOL Initialize(CWndBase * pWndParent = nullptr);

	void OnDestroy() override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
};

class CWndMixJewel final : public CWndNeuz
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

	BOOL Initialize( CWndBase* pWndParent = nullptr );
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

class CWndExtraction final : public CWndNeuz {
public:
	class Receiver : public CWndItemReceiver {
		bool CanReceiveItem(const CItemElem & itemElem, bool) override;
	};

private:
	static constexpr UINT WIDC_Receiver = 1500;

	Receiver m_receiver;

public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
	
	void ReceiveResult(CUltimateWeapon::MakeGemAnswer result);
	void SetWeapon(CItemElem & pItemElem);
};

class CWndSmeltJewel final : public CWndNeuz
{
public:
	CModelObject* m_pMainItem;
	CItemElem* m_pItemElem;
	CItemElem* m_pJewelElem;
	
	DWORD m_dwJewel[5];
	int m_nJewelSlot[5];
	int m_nJewelCount;
	int m_nUsableSlot;
	OBJID m_objJewelId;

	float m_fRotate;
	float m_fAddRot;

  enum class Status { Base, Smelting };
	Status m_nStatus;
	int m_nCount;
	int m_nDelay;
	int m_nAlpha;
	float m_nEyeYPos;

  struct {
    CTexture * blue;
    CTexture * black;
    CTexture * red;
  } m_disableTextures;
	
public:
	CWndSmeltJewel(); 
	~CWndSmeltJewel();
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void OnDestroy( void );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL Process ();
	
	void ReceiveResult(int result);
	void InitializeJewel(CItemElem* pItemElem);
	void SetJewel(CItemElem* pItemElem);

  [[nodiscard]] static bool IsJewel(DWORD itemId);

  struct WeaponCameraAngle {
    explicit WeaponCameraAngle(const ItemProp * pItemProp);
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt;
  };
};

class CWndChangeWeapon final : public CWndNeuz
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
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
	
	void ReceiveResult(int result);
	void SetItem(CItemElem & pItemElem);

private:
	void SetupText();
	void UpdateStartButtonStatus();
};

class CWndRemoveJewelConfirm final : public CWndNeuz 
{
public:
	CWndInventory* m_pInventory;
	CItemElem *	m_pUpgradeItem;

public: 
	CWndRemoveJewelConfirm(); 
	virtual ~CWndRemoveJewelConfirm(); 
	
	virtual void OnDestroy();
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

	void SetItem(CItemElem *	m_pItem);
};

/*******************************
	전승 시스템 관련 Window
********************************/

class CWndHeroSkillUp final : public CWndNeuz {
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
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void ReceiveResult(int nresult);
	void SetJewel(CItemElem* pItemElem);
};


class CWndRemoveAttribute final : public CWndNeuz
{
public:
	class CWndAttributedItem : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	class CWndConfirm final : public CWndNeuz {
	public:
		BOOL Initialize(CWndBase * pWndParent);
		BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
		void OnInitialUpdate() override;
	};
	
public:
	CWndAttributedItem m_receiver;
	CWndConfirm * m_pWndConfirm = nullptr;

public: 
	~CWndRemoveAttribute() override; 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override; 
	void OnInitialUpdate() override; 
	
	void ReceiveResult(BOOL result);
	void SetWeapon(CItemElem* pItemElem);
};


class CWndRemovePiercing final : public CWndNeuz
{
public:
	static constexpr UINT WIDC_Receiver = 901;

	class CWndPiercedItemReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	CWndPiercedItemReceiver m_receiver;
	
public: 
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	
	void SetItem(CItemElem* pItemElem);
};

class CWndRemoveJewel final : public CWndNeuz
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

	BOOL Initialize( CWndBase* pWndParent = nullptr );
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

class CWndChangeAttribute final : public CWndNeuz 
{
public:
	int m_nAttributeNum;
	int m_nAttributeStaticID[5];
	int m_nTooltipTextIndx[6];
	CItemElem* m_pItemElem;
	CItemElem* m_pChangeItem;
	CTexture* m_pTexture;

public: 
	CWndChangeAttribute(); 
	virtual ~CWndChangeAttribute();
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	void OnSetCursor() override;
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL Process();

public:
	void SetChangeItem( CItemElem* pItemElem );
	void FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend);
};

class CWndSmeltSafety final : public CWndNeuz
{
public:
	enum WndMode { WND_NORMAL, WND_ACCESSARY, WND_PIERCING, WND_ELEMENT };

	struct GENMATDIEINFO {
		LPWNDCTRL wndCtrl;
		BOOL isUse;
		int staticNum;
		CItemElem * pItemElem;

		void OnInitialUpdate(LPWNDCTRL wndCtrl, int staticNum);
		void OnDestruction();
		void RemoveItem();

		void AddListItem(CItemElem * pItemElem);
		void SubtractListItem();

		void Render(C2DRender * p2DRender, const ItemProp * pItemProp);
	};

	struct GenLine {
		int resultStaticId;
		GENMATDIEINFO material;
		GENMATDIEINFO scroll1;
		GENMATDIEINFO scroll2;
		bool resultStatic;

		void SendUpgradeRequestToServer(CItemElem * upgradedItem);
		void OnDestruction(bool destroyScroll2);
	};

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

	GenLine m_genLines[SMELT_MAX];

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferSuccessImage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferFailureImage;
	const ItemProp* m_pSelectedElementalCardItemProp;

public:
	CWndSmeltSafety(WndMode eWndMode);
	virtual ~CWndSmeltSafety();

	BOOL Initialize( CWndBase* pWndParent = nullptr );
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
	void DrawListItem(C2DRender* p2DRender);
	[[nodiscard]] bool IsDropMaterialZone(CPoint point) const;
	[[nodiscard]] bool IsDropScroll1Zone(CPoint point) const;
	[[nodiscard]] bool IsDropScroll2Zone(CPoint point) const;
	[[nodiscard]] bool IsAcceptableMaterial(const ItemProp * pItemProp);
	[[nodiscard]] bool IsAcceptableScroll1(const ItemProp * pItemProp) const;
	[[nodiscard]] bool IsAcceptableScroll2(const ItemProp * pItemProp) const;
	[[nodiscard]] int GetNowSmeltValue() const;
	[[nodiscard]] int GetDefaultMaxSmeltValue() const;
	void SetResultSwitch(bool bResultSwitch) { m_bResultSwitch = bResultSwitch; }
	void SetCurrentSmeltNumber(int nCurrentSmeltNumber) { m_nCurrentSmeltNumber = nCurrentSmeltNumber; }
	void SetResultStatic(bool bResultStatic, int nIndex) { m_genLines[nIndex].resultStatic = bResultStatic; }
	CItemElem* GetItemElement(void) const { return m_pItemElem; }
	bool GetResultSwitch(void) const { return m_bResultSwitch; }
	int GetCurrentSmeltNumber(void) const { return m_nCurrentSmeltNumber; }
	int GetResultStaticID(int nIndex) const { return m_genLines[nIndex].resultStaticId; }

private:
	[[nodiscard]] std::span<GenLine> GenLinesUntilCurrentSmelt() { return std::span(m_genLines, m_nCurrentSmeltNumber); }
	[[nodiscard]] std::span<GenLine> GenLinesSinceCurrentSmelt() { return std::span(m_genLines + m_nCurrentSmeltNumber, m_genLines + SMELT_MAX); }
};

class CWndSmeltSafetyConfirm final : public CWndNeuz
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
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDestroy( void );

	void SetWndMode(CItemElem* pItemElem);
};


class CWndEquipBindConfirm final : public CWndNeuz
{
public:
	enum EquipAction { EQUIP_DOUBLE_CLICK, EQUIP_DRAG_AND_DROP };

public:
	CWndEquipBindConfirm(EquipAction eEquipAction);
	virtual ~CWndEquipBindConfirm(void);

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
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
