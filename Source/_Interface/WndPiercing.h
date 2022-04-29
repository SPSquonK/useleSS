#pragma once

class CPiercingMessageBox;

class CWndPiercing final : public CWndNeuz {
public:
	struct Slot {
		CItemElem * m_item = nullptr;
		CRect m_rect = CRect();

		[[nodiscard]] constexpr operator bool() const noexcept { return m_item; }
		[[nodiscard]] bool IsIn(const CPoint point) const { return PtInRect(&m_rect, point); }
		void Clear();
		void Set(CItemElem * item);
	};

	CSfx * m_pSfx = nullptr;
	CPiercingMessageBox * m_pPiercingMessageBox = nullptr;

	std::array<Slot, 3> m_slots;
	
	CWndPiercing(); 
	~CWndPiercing(); 
	
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	
	virtual BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void	OnDraw( C2DRender* p2DRender ); 
	virtual	void	OnInitialUpdate(); 
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void	OnDestroy( void );
	virtual void	OnRButtonUp( UINT nFlags, CPoint point );
}; 

class CPiercingMessageBox final : public CWndMessageBox {
public:
	std::array<OBJID, 3> m_Objid;

	explicit CPiercingMessageBox(const std::array<CWndPiercing::Slot, 3> & slots);
	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD dwWndId = 0) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
};
