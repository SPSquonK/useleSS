#pragma once

class CWndItemCtrlVendor : public CWndItemCtrl {
public:
	CWndItemCtrlVendor();
	BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0) override;
};

class CWndConfirmBuy final : public CWndNeuz
{
public:
	[[nodiscard]] static std::uint32_t GetBuyLimitForItem(const ItemProp & itemProp);

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;

private:
	void		OnOK();
	void		OnChangeBuyCount(std::int32_t dwBuy, bool allowZero = false);
	bool    CloseIfNoPrice();

	std::uint32_t GetCostOfItem() const;

public:
	CMover *     m_pMover     = nullptr;
	CItemElem *  m_pItemElem   = nullptr;
	DWORD			   m_dwItemId    = 0;
	CWndEdit *   m_pEdit       = nullptr;
	CWndStatic * m_pStatic     = nullptr;
	CWndStatic * m_pStaticGold = nullptr;
	int				   m_nBuyType    = 0;
};

class CWndWarning final : public CWndNeuz
{
public:
	CMover * m_pMover = nullptr;
	CItemElem * m_pItemElem = nullptr;

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
};

class CWndConfirmSell final : public CWndNeuz 
{ 
public: 
	CMover* m_pMover = nullptr;
	CItemElem* m_pItemElem = nullptr;
	CWndEdit* m_pEdit = nullptr;
	CWndStatic* m_pStatic = nullptr;
	CWndStatic* m_pStaticGold = nullptr;

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL Process( void );
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
}; 

class CWndShop final : public CWndNeuz 
{ 
public:
	CWndConfirmSell * m_pWndConfirmSell = nullptr;
	CWndWarning     * m_pWndWarning = nullptr;
public: 
	bool m_bSexSort   = false;
	bool m_bLevelSort = false;

	CMover* m_pMover = nullptr;
	CWndItemCtrlVendor m_wndItemCtrl[ MAX_VENDOR_INVENTORY_TAB ];
	~CWndShop() override;

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void OnDestroy( void );

private:
	bool OnDropItemFromInventory(CItemElem * pItemElem);
}; 

class CWndBeautyShopConfirm;
class CWndUseCouponConfirm;

class CWndBeautyShop final : public CWndNeuz 
{ 
public:
	CTexture         m_Texture;
	CWndConfirmSell* m_pWndConfirmSell;
	CModelObject*	 m_pModel;

	DWORD			 m_dwHairMesh;
	DWORD			 m_dwSelectHairMesh;
	BOOL			 m_bLButtonClick;
	CRect			 m_ColorRect[3];
	FLOAT			 m_fColor[3];
	int              m_nHairCost;
	int				 m_nHairColorCost;
	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];
	
#ifdef __Y_BEAUTY_SHOP_CHARGE
	BOOL			 m_bChange;
#endif //__Y_BEAUTY_SHOP_CHARGE
	CModelObject*	 m_pApplyModel;
	CWndEdit*		 m_pRGBEdit[3];
	int				 m_ChoiceBar;
	CModelObject*	 m_pHairModel;
	DWORD			 m_nHairNum[4];
	CWndBeautyShopConfirm* m_pWndBeautyShopConfirm;
	
	BOOL m_bUseCoupon;
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;
	
public: 
	CWndBeautyShop(); 
	~CWndBeautyShop(); 
	void    ReSetBar( FLOAT r, FLOAT g, FLOAT b );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	virtual void OnMouseWndSurface( CPoint point );
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void OnDestroy( void );

	void SetRGBToEdit(float color, int editnum);
	void SetRGBToBar(int editnum);
	void DrawHairKind(C2DRender* p2DRender, D3DXMATRIX matView);
	void UpdateModels();

	void UseHairCoupon(BOOL isUse);
}; 


class CWndUseCouponConfirm final : public CWndNeuz 
{	
public:
	BOOL  m_bUseCoupon;
	DWORD m_TargetWndId;
	BOOL  m_checkClose;
	int   m_MainFlag;
public: 
	CWndUseCouponConfirm(); 
	virtual ~CWndUseCouponConfirm(); 
	
	virtual void OnDestroy();
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	void SetInfo(DWORD targetWndId, int flag);
}; 

class CWndBeautyShopConfirm final : public CWndNeuz 
{
public:
	int m_ParentId;

public: 
	CWndBeautyShopConfirm(); 
	virtual ~CWndBeautyShopConfirm(); 
	
	virtual void OnDestroy();
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndFaceShop final : public CWndNeuz
{
public:
	static constexpr int MaxFriendlyFace = MAX_DEFAULT_HEAD;

	template<DWORD MinInclusive, DWORD MaxExclusive>
	static DWORD RingPlus(DWORD value) {
		value += 1;
		if (value == MaxExclusive) return MinInclusive;
		return value;
	}

	template<DWORD MinInclusive, DWORD MaxExclusive>
	static DWORD RingMinus(DWORD value) {
		if (value == MinInclusive) return MaxExclusive - 1;
		return value - 1;
	}

	std::unique_ptr<CModelObject> m_pMainModel;
	std::unique_ptr<CModelObject> m_pApplyModel;
	std::unique_ptr<CModelObject> m_pFriendshipFace;

	DWORD m_dwFriendshipFace;
	DWORD m_dwNewFace;
	DWORD m_nSelectedFace;
	std::array<DWORD, 4> m_nFriendshipFaceNum;
	std::array<DWORD, 4> m_nNewFaceNum;
	int m_nCost;
	int m_ChoiceBar;
	
	CWndBeautyShopConfirm* m_pWndBeautyShopConfirm;

	BOOL m_bUseCoupon;
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;	

public:
	CWndFaceShop(); 
	~CWndFaceShop();
	
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnDestroy( void );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
		
	
	void UpdateModels();

	void UseFaceCoupon(BOOL isUse);

private:
	void InitializeCurrentHead();
	void DrawFaces(bool ChoiceFlag, C2DRender * p2DRender, D3DXMATRIX matView);
};
