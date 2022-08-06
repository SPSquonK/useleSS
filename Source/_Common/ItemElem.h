#pragma once

#if !(defined(__CLIENT) || defined(__WORLDSERVER) || defined(__DBSERVER) || defined(__CORESERVER))
static_assert(false, "ItemElem.h should only be included in Client, World, DB or CoreServer");
#endif

class CItemElem final {
public:
	enum { expired = 0x01, binds = 0x02, isusing = 0x04, };
	// 0x8000000000000000 - 0x4000000000000000
	enum { eNoLevelDown, e5LevelDown, e10LevelDown, };

	DWORD		m_dwItemId;		// ItemId in Property

	OBJID       m_dwObjId;		// Id in the container
	DWORD		m_dwObjIndex;	// Index in the container

	TCHAR       m_szItemText[32];

#if defined(__CLIENT) || defined(__WORLDSERVER)
	int			m_nCost;
	int 		m_nExtra;		// Number of items in transaction / ...
#endif

#ifdef __CLIENT
	CTexture * m_pTexture;
#endif

	int          m_nAbilityOption;

private:
	CPiercing    m_piercing;
	SERIALNUMBER m_liSerialNumber;

public:
	int			m_nRepair;			// Number of repairs
	BYTE		m_nRepairNumber;
	int			m_nHitPoint;
	short		m_nItemNum;
	BYTE		m_byFlag;
	DWORD		m_idGuild;			// Guild for cloak


	BYTE   	    m_bItemResist;			//  Attribute kind
	int			m_nResistAbilityOption;	// Attribute value
	int			m_nResistSMItemId;

#ifdef __CLIENT
	BOOL		m_bRepair;
#endif	// __CLIENT

#ifdef __WORLDSERVER
	BOOL	m_bQuery;
#endif	// __WORLDSERVER

	// Bit organization: 8|10|8|10|8|10|8 = 64 BITS (not bytes)
	__int64		m_iRandomOptItemId;

#if defined(__CLIENT) || defined(__WORLDSERVER)
	__int64		m_n64NewRandomOption;
#endif

	BOOL	m_bCharged;			// Can't be traded?
	DWORD	m_dwKeepTime;		// Mobile game mechanic

#ifndef __CORESERVER
	BOOL	m_bTranformVisPet;
#endif

public:
	CItemElem();
	~CItemElem();
	CItemElem & operator = (CItemElem & ie);
	friend CAr & operator<<(CAr & ar, const CItemElem & item);
	friend CAr & operator>>(CAr & ar, CItemElem & item);

	void Empty();
	[[nodiscard]] BOOL IsEmpty() const noexcept { return m_dwItemId ? FALSE : TRUE; }

#ifndef __CORESERVER
	[[nodiscard]] ItemProp * GetProp() const { return prj.GetItemProp(m_dwItemId); }
#endif

#if defined(__CLIENT) || defined(__WORLDSERVER)
	[[nodiscard]] BOOL IsQuest();
	CTexture * GetTexture() const;

	[[nodiscard]] int GetCost() const;
	[[nodiscard]] DWORD GetChipCost() const;

	void SetExtra(int nExtra);
	[[nodiscard]] int GetExtra() const noexcept;

#endif

	[[nodiscard]] SERIALNUMBER GetSerialNumber() const noexcept { return m_liSerialNumber; }
	void SetSerialNumber(const SERIALNUMBER liSerialNumber) { m_liSerialNumber = liSerialNumber; }

#if defined(__CLIENT) || defined(__WORLDSERVER)
	void SetSerialNumber();
#endif

	void	SetPiercingSize(int nSize) { m_piercing.SetPiercingSize(nSize); }
	int		GetPiercingSize() const { return m_piercing.GetPiercingSize(); }
	void	SetPiercingItem(int nth, DWORD dwItem) { m_piercing.SetPiercingItem(nth, dwItem); }
	DWORD	GetPiercingItem(int nth) const { return m_piercing.GetPiercingItem(nth); }
	void	CopyPiercing(const CPiercing & piercing) { m_piercing = piercing; }
	[[nodiscard]] const CPiercing & GetPiercings() const { return m_piercing; }
	BOOL	IsPiercedItem() { return m_piercing.IsPiercedItem(); }

#ifndef __CORESERVER
	void	SetUltimatePiercingSize(int nSize) { m_piercing.SetUltimatePiercingSize(nSize); }
	int		GetUltimatePiercingSize() const { return m_piercing.GetUltimatePiercingSize(); }
	void	SetUltimatePiercingItem(int nth, DWORD dwItem) { m_piercing.SetUltimatePiercingItem(nth, dwItem); }
	DWORD	GetUltimatePiercingItem(int nth) const { return m_piercing.GetUltimatePiercingItem(nth); }

	BOOL	IsPierceAble(DWORD dwTargetItemKind3 = NULL_ID, BOOL bSize = FALSE) const;
#endif


#if defined(__CLIENT) || defined(__WORLDSERVER)
	BOOL	IsPerin(void) { return m_dwItemId == II_SYS_SYS_SCR_PERIN; }
	BOOL	IsCollector(BOOL bRefinable = FALSE) { return(m_dwItemId == II_GEN_TOO_COL_NORMAL || (!bRefinable && m_dwItemId == II_GEN_TOO_COL_EVENT)); }
	[[nodiscard]] bool IsAccessory() const { return g_AccessoryProperty.IsAccessory(m_dwItemId); }
	BOOL	IsActiveTicket(DWORD dwItemId);
#endif

	[[nodiscard]] __int64 GetRandomOptItemId() const noexcept { return m_iRandomOptItemId; }
	void	SetRandomOptItemId(__int64 iRandomOptItemId) { m_iRandomOptItemId = iRandomOptItemId; }
	[[nodiscard]] int GetRandomOpt() const { return static_cast<int>(m_iRandomOptItemId & 0x00000000000000FF); }

#if defined(__CLIENT) || defined(__WORLDSERVER)
	void	SetRandomOpt(int nRandomOpt) { m_iRandomOptItemId = (m_iRandomOptItemId & 0xFFFFFFFFFFFFFF00) | static_cast<__int64>(nRandomOpt & 0x00000000000000FF); }
	__int64 * GetRandomOptItemIdPtr() { return &m_iRandomOptItemId; }

	__int64	GetNewRandomOption();
	__int64 * GetNewRandomOptionPtr();
	void	ResetNewRandomOption();
	void	SelectNewRandomOption();
	bool	SelectRandomOption(BYTE bySelectFlag);
#endif


#if defined(__CLIENT) || defined(__WORLDSERVER)
	[[nodiscard]] int GetLevelDown() const;	// 64|63
	void	SetLevelDown(int i = eNoLevelDown);
	[[nodiscard]] DWORD	GetLimitLevel() const;
	[[nodiscard]] bool IsLimitLevel(const CMover * pMover) const;
#endif

	CPet * m_pPet;

#if defined(__CLIENT) || defined(__WORLDSERVER)
	[[nodiscard]] bool	IsPet() const { return GetProp()->dwItemKind3 == IK3_EGG; }
	[[nodiscard]] BOOL	IsEatPet() const { return GetProp()->dwItemKind3 == IK3_PET; }
	[[nodiscard]] BOOL	IsFeed() const { return GetProp()->dwItemKind3 == IK3_FEED; }
	[[nodiscard]] bool	IsEgg() const;
	[[nodiscard]] CString	GetName() const;

	[[nodiscard]] ItemProps::PiercingType GetPiercingType() const;
#endif


#if defined(__CLIENT) || defined(__WORLDSERVER)
	int			GetAttrOption();		// 아이템의 +옵션값과 속성/속성레벨값을 합쳐서 리턴.
	[[nodiscard]] int GetAbilityOption() const noexcept { return m_nAbilityOption; }
	int			GetItemResist(void) const;
	int			GetResistAbilityOption(void) const;
	int * GetAbilityOptionPtr() { return &m_nAbilityOption; }
	const int * GetAbilityOptionPtr() const noexcept { return &m_nAbilityOption; }
	void		SetAbilityOption(int nAbilityOption) { m_nAbilityOption = nAbilityOption; }
#endif
#if defined(__DBSERVER) || defined(__CORESERVER)
	int		GetOption(void) { return m_nAbilityOption; }
	void	SetOption(int nOption) { m_nAbilityOption = nOption; }
#endif

	[[nodiscard]] BOOL IsFlag(BYTE byFlag) const { return (m_byFlag & byFlag) ? TRUE : FALSE; }


#ifndef __CORESERVER
	BOOL	IsVisPet() const { return (GetProp() && GetProp()->IsVisPet()) || IsTransformVisPet(); }
	void	SetVisKeepTimeSize(int nSize) { m_piercing.SetVisKeepTimeSize(nSize); }
	void	SetVisKeepTime(int nth, time_t tmKeep) { m_piercing.SetVisKeepTime(nth, tmKeep); }
	time_t	GetVisKeepTime(int nth) const { return m_piercing.GetVisKeepTime(nth); }
	BOOL	IsTransformVisPet() const { return GetProp() && GetProp()->dwItemKind3 == IK3_PET && m_bTranformVisPet; }
#endif

#if defined(__CLIENT) || defined(__WORLDSERVER)
	void	SetSwapVisItem(int nPos1, int nPos2);
	DWORD	GetVisPetSfxId();
#endif


#if defined(__DBSERVER) || defined(__CORESERVER)
	BOOL	IsInvalid();
#endif

#if defined(__CLIENT) || defined(__WORLDSERVER)
	int			GetGold();

	void SetTexture();
	void SetTexture(CTexture * const pTexture) {
#ifdef __CLIENT
		m_pTexture = pTexture;
#endif
	}

	[[nodiscard]] SmallDstList GetPiercingAvail() const;


	BOOL	IsExpiring(void) {
		if (m_dwKeepTime) {
			if (time_null() > (time_t)m_dwKeepTime)
				return TRUE;
		}
		return FALSE;
	}
	void	ResetFlag(BYTE byFlag) { m_byFlag &= ~byFlag; }
	void	SetFlag(BYTE byFlag) { m_byFlag |= byFlag; }
	BOOL	IsBinds();
	BOOL	IsUndestructable(void);
	BOOL	IsCharged();
	BOOL	IsLogable();

	static BOOL	IsDiceRefineryAble(const ItemProp * pProp);
	static BOOL	IsEleRefineryAble(const ItemProp * pProp);
	static BOOL	IsElementalCard(const DWORD dwItemID);

	//	Operations
	void		UseItem();
#endif
};

/// Equipment information structure
struct EQUIP_INFO {
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
};
