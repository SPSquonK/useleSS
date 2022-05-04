#pragma once

#if !(defined(__CLIENT) || defined(__WORLDSERVER) || defined(__DBSERVER) || defined(__CORESERVER))
static_assert(false, "ItemElem.h should only be included in Client, World, DB or CoreServer");
#endif



#if defined(__CLIENT) || defined(__WORLDSERVER)

class CItemElem final {
public:
	OBJID       m_dwObjId;		// 아이템 콘테이너 안에서의 ID (아이템의 ID로 사용)
	DWORD		m_dwObjIndex;	// 아이템 콘테이너 안에서의 Index
	DWORD		m_dwItemId;		// 아이템 식별 인덱스( Property에서 사용 )
	int 		m_nExtra;		// 거래시 물품개수 or 개인상점에 등록한 갯수 
	TCHAR       m_szItemText[32];
	int			m_nCost;

#ifdef __CLIENT
	CTexture * m_pTexture;
#endif

private:
	SERIALNUMBER		m_liSerialNumber;

public:

	BOOL			IsEmpty() { return m_dwItemId ? FALSE : TRUE; }
	BOOL			IsQuest();

	[[nodiscard]] ItemProp * GetProp() const { return prj.GetItemProp(m_dwItemId); }
	CTexture * GetTexture();
	int				GetCost();							// 가격을 얻는다.
	void			SetExtra(int nExtra);			// 확장 데이타를 세팅 
	[[nodiscard]] int GetExtra() const noexcept;							// 확장 데이타를 얻기 
	DWORD			GetChipCost();

	void	SetSerialNumber(void);
	SERIALNUMBER	GetSerialNumber(void) { return m_liSerialNumber; }
	void	SetSerialNumber(SERIALNUMBER liSerialNumber) { m_liSerialNumber = liSerialNumber; }
private:
	CPiercing	m_piercing;
public:
	void	SetPiercingSize(int nSize) { m_piercing.SetPiercingSize(nSize); }
	int		GetPiercingSize() const { return m_piercing.GetPiercingSize(); }
	void	SetPiercingItem(int nth, DWORD dwItem) { m_piercing.SetPiercingItem(nth, dwItem); }
	DWORD	GetPiercingItem(int nth) const { return m_piercing.GetPiercingItem(nth); }
	void	CopyPiercing(const CPiercing & piercing) { m_piercing = piercing; }
	void	SerializePiercing(CAr & ar) { m_piercing.Serialize(ar); }
	BOOL	IsPiercedItem() { return m_piercing.IsPiercedItem(); }
	void	SetUltimatePiercingSize(int nSize) { m_piercing.SetUltimatePiercingSize(nSize); }
	int		GetUltimatePiercingSize() { return m_piercing.GetUltimatePiercingSize(); }
	void	SetUltimatePiercingItem(int nth, DWORD dwItem) { m_piercing.SetUltimatePiercingItem(nth, dwItem); }
	DWORD	GetUltimatePiercingItem(int nth) { return m_piercing.GetUltimatePiercingItem(nth); }

	BOOL	IsPierceAble(DWORD dwTargetItemKind3 = NULL_ID, BOOL bSize = FALSE);

	int			m_nAbilityOption;	// 추가 능력치 가변 옵션

public:
	enum { expired = 0x01, binds = 0x02, isusing = 0x04, };

	int			m_nRepair;			// 수리 횟수
	BYTE		m_nRepairNumber;
	int			m_nHitPoint;
	short		m_nItemNum;
	BYTE		m_byFlag;
	DWORD		m_idGuild;			// 아이템에 길드 번호가 붙는 경우(망토)

#ifdef __CLIENT
	BOOL		m_bRepair;
#endif	// __CLIENT

	BYTE   	    m_bItemResist;				// 어느 속성 인가? / TEXT형태로 궤스트를 작동시키는 아이템에서 TRUE이면 더이상 퀘스트를 작동할 수 없게 된다.
	int			m_nResistAbilityOption;		// 속성 추가 능력치 가변 옵션
	int			m_nResistSMItemId;

#ifdef __WORLDSERVER
	BOOL	m_bQuery;
#endif	// __WORLDSERVER

	/*
#ifdef __GIFTBOX0213
#ifdef __WORLDSERVER
	int		m_nQueryGiftbox;
#endif	// __WORLDSERVER
#endif	// __GIFTBOX0213
	*/
	BOOL	IsPerin(void) { return m_dwItemId == II_SYS_SYS_SCR_PERIN; }
	BOOL	IsCollector(BOOL bRefinable = FALSE) { return(m_dwItemId == II_GEN_TOO_COL_NORMAL || (!bRefinable && m_dwItemId == II_GEN_TOO_COL_EVENT)); }
	BOOL	IsAccessory(void) { return g_AccessoryProperty.IsAccessory(m_dwItemId); }
	BOOL	IsActiveTicket(DWORD dwItemId);
	// 비트별 연산
	// 8|10|8|10|8|10|8	= 64
private:
	__int64		m_iRandomOptItemId;

	//	mulcom	BEGIN100405	각성 보호의 두루마리
	__int64		m_n64NewRandomOption;
	//	mulcom	END100405	각성 보호의 두루마리

public:
	[[nodiscard]] __int64 GetRandomOptItemId() const noexcept { return m_iRandomOptItemId; }
	void	SetRandomOptItemId(__int64 iRandomOptItemId) { m_iRandomOptItemId = iRandomOptItemId; }
	[[nodiscard]] int		GetRandomOpt() const { return static_cast<int>(m_iRandomOptItemId & 0x00000000000000FF); }
	void	SetRandomOpt(int nRandomOpt) { m_iRandomOptItemId = (m_iRandomOptItemId & 0xFFFFFFFFFFFFFF00) | static_cast<__int64>(nRandomOpt & 0x00000000000000FF); }
	__int64 * GetRandomOptItemIdPtr(void) { return &m_iRandomOptItemId; }


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	__int64		GetNewRandomOption();
	__int64 * GetNewRandomOptionPtr();
	void		ResetNewRandomOption();
	void		SelectNewRandomOption();
	bool		SelectRandomOption(BYTE bySelectFlag);
	//	mulcom	END100405	각성 보호의 두루마리


	//	0x8000000000000000
	//	0x4000000000000000
	enum { eNoLevelDown, e5LevelDown, e10LevelDown, };
	int		GetLevelDown(void);	// 64|63
	void	SetLevelDown(int i = eNoLevelDown);
	DWORD	GetLimitLevel(void);
	BOOL	IsLimitLevel(CMover * pMover);	// 착용이 불가능한 레벨인가?
/*
#ifdef __XPET2
	int	m_nMaxLevel;	// 수명
	int m_nLevel;		// 레벨(나이)
	int m_nStr, m_nDex, m_nInt, m_nSta;		// 스탯
	int m_nHungry;		// 배고픈정도 -10 ~ 10		-10이하는 존내배고픔 / -10 ~ -5 배고픔 / -5 ~ 5 보통 / 5이상 배부름
	int m_nFeeling;		// 기분. -10 ~ 10
	int m_nHealth;		// 건강 -10 ~ 10
	int m_nConstitution;	// 체질(영양상태) -10 ~ 10
#endif
*/

	CPet * m_pPet;		// 펫 객체, 첫 소환 時 생성
	BOOL	IsPet(void) { return GetProp()->dwItemKind3 == IK3_EGG; }
	BOOL	IsEatPet(void) { return GetProp()->dwItemKind3 == IK3_PET; }
	BOOL	IsFeed(void) { return GetProp()->dwItemKind3 == IK3_FEED; }
	BOOL	IsEgg(void);
	CString		GetName(void);

public:
	//	Constructions
	CItemElem();
	~CItemElem();

	//	Attributes
	void			Empty();
	int			GetAttrOption();		// 아이템의 +옵션값과 속성/속성레벨값을 합쳐서 리턴.
	[[nodiscard]] int GetAbilityOption() const noexcept { return m_nAbilityOption; }
	int			GetItemResist(void) const;
	int			GetResistAbilityOption(void) const;
	int * GetAbilityOptionPtr() { return &m_nAbilityOption; }
	const int * GetAbilityOptionPtr() const noexcept { return &m_nAbilityOption; }
	void		SetAbilityOption(int nAbilityOption) { m_nAbilityOption = nAbilityOption; }
	int			GetGold();


	void SetTexture();
	void SetTexture(CTexture * const pTexture) {
#ifdef __CLIENT
		m_pTexture = pTexture;
#endif
	}

	//	PIERCINGINFO	m_piercingInfo;
	[[nodiscard]] SmallDstList GetPiercingAvail() const;
	BOOL	m_bCharged;			// 상용화 아이템인지 확인
	DWORD	m_dwKeepTime;		// 지속시간

	BOOL	IsExpiring(void) {
		if (m_dwKeepTime) {
			if (time_null() > (time_t)m_dwKeepTime)
				return TRUE;
		}
		return FALSE;
	}
	void	ResetFlag(BYTE byFlag) { m_byFlag &= ~byFlag; }
	void	SetFlag(BYTE byFlag) { m_byFlag |= byFlag; }
	[[nodiscard]] BOOL IsFlag(BYTE byFlag) const { return (m_byFlag & byFlag) ? TRUE : FALSE; }
	BOOL	IsBinds();
	BOOL	IsUndestructable(void);
	BOOL	IsCharged();
	BOOL	IsLogable();

	static BOOL	IsDiceRefineryAble(const ItemProp * pProp);
	static BOOL	IsEleRefineryAble(const ItemProp * pProp);
	static BOOL	IsElementalCard(const DWORD dwItemID);

	//	Operations
	void		UseItem();
	CItemElem & operator = (CItemElem & ie);
	void		Serialize(CAr & ar);

	BOOL	IsVisPet() { return (GetProp() && GetProp()->IsVisPet()) || IsTransformVisPet(); }
	void	SetSwapVisItem(int nPos1, int nPos2);
	void	SetVisKeepTimeSize(int nSize) { m_piercing.SetVisKeepTimeSize(nSize); }
	void	SetVisKeepTime(int nth, time_t tmKeep) { m_piercing.SetVisKeepTime(nth, tmKeep); }
	time_t	GetVisKeepTime(int nth) { return m_piercing.GetVisKeepTime(nth); }
	DWORD	GetVisPetSfxId();
	BOOL	IsTransformVisPet() { return GetProp() && GetProp()->dwItemKind3 == IK3_PET && m_bTranformVisPet; }

	BOOL	m_bTranformVisPet;
};


#endif

#if defined(__DBSERVER) || defined(__CORESERVER)
class CItemElem final {
public:
	OBJID       m_dwObjId;		// 아이템 컨테이너에서 몇번째 
	DWORD		m_dwItemId;		// IID_ 값 
	TCHAR       m_szItemText[32];

	DWORD		m_dwObjIndex;	// 인벤토리에서 몇번째 
private:
	SERIALNUMBER	m_liSerialNumber;
public:

	BOOL	IsEmpty() { return m_dwItemId ? FALSE : TRUE; }
	SERIALNUMBER	GetSerialNumber(void) { return m_liSerialNumber; }
	void	SetSerialNumber(SERIALNUMBER liSerialNumber) { m_liSerialNumber = liSerialNumber; }

private:
	CPiercing	m_piercing;
public:
	void	SetPiercingSize(int nSize) { m_piercing.SetPiercingSize(nSize); }
	int		GetPiercingSize() { return m_piercing.GetPiercingSize(); }
	void	SetPiercingItem(int nth, DWORD dwItem) { m_piercing.SetPiercingItem(nth, dwItem); }
	DWORD	GetPiercingItem(int nth) const { return m_piercing.GetPiercingItem(nth); }
	void	CopyPiercing(const CPiercing & piercing) { m_piercing = piercing; }
	void	SerializePiercing(CAr & ar) { m_piercing.Serialize(ar); }
	BOOL	IsPiercedItem() { return m_piercing.IsPiercedItem(); }
#ifdef __DBSERVER
	void	SetUltimatePiercingSize(int nSize) { m_piercing.SetUltimatePiercingSize(nSize); }
	int		GetUltimatePiercingSize() { return m_piercing.GetUltimatePiercingSize(); }
	void	SetUltimatePiercingItem(int nth, DWORD dwItem) { m_piercing.SetUltimatePiercingItem(nth, dwItem); }
	DWORD	GetUltimatePiercingItem(int nth) { return m_piercing.GetUltimatePiercingItem(nth); }

	ItemProp * GetProp() { return prj.GetItemProp(m_dwItemId); }
	BOOL	IsPierceAble(DWORD dwTargetItemKind3 = NULL_ID, BOOL bSize = FALSE);
#endif // __DBSERVER

private:
	int		m_nAbilityOption;

public:
	enum { expired = 0x01, binds = 0x02, };

	int		m_nRepair;
	BYTE	m_nRepairNumber;
	BYTE	m_byFlag;
	short	m_nItemNum;
	int		m_nHitPoint;
	DWORD	m_idGuild;
	BYTE	m_bItemResist;				// 어느 속성 인가?
	int		m_nResistAbilityOption;		// 속성 추가 능력치 가변 옵션
	int		m_nResistSMItemId;

	//	PIERCINGINFO	m_piercingInfo;
	BOOL	m_bCharged;					// 상용화 아이템인지 확인
	DWORD	m_dwKeepTime;				// 지속시간
	// 비트별 연산
	// 12|12|16|16|8	= 64
private:
	__int64		m_iRandomOptItemId;
public:
	__int64		GetRandomOptItemId(void) { return m_iRandomOptItemId; }
	void	SetRandomOptItemId(__int64 iRandomOptItemId) { m_iRandomOptItemId = iRandomOptItemId; }
	int		GetRandomOpt(void) { return static_cast<int>(m_iRandomOptItemId & 0x00000000000000FF); }

	CPet * m_pPet;

public:
	//	Constructions
	CItemElem();
	~CItemElem();

	//	Operations
	int		GetOption(void) { return m_nAbilityOption; }
	void	SetOption(int nOption) { m_nAbilityOption = nOption; }
	BOOL	IsFlag(BYTE byFlag) { return (m_byFlag & byFlag) ? TRUE : FALSE; }
	BOOL	IsInvalid(void);

	void	Empty() {
		m_szItemText[0] = '\0';
		m_dwItemId = 0;
		SAFE_DELETE(m_pPet);
	}

	CItemElem & operator = (CItemElem & ie);
	void	Serialize(CAr & ar);

#ifdef __DBSERVER
	BOOL	IsVisPet() { return (GetProp() && GetProp()->IsVisPet()) || IsTransformVisPet(); }
	void	SetVisKeepTimeSize(int nSize) { m_piercing.SetVisKeepTimeSize(nSize); }
	void	SetVisKeepTime(int nth, time_t tmKeep) { m_piercing.SetVisKeepTime(nth, tmKeep); }
	time_t	GetVisKeepTime(int nth) { return m_piercing.GetVisKeepTime(nth); }
	BOOL	IsTransformVisPet() { return GetProp() && GetProp()->dwItemKind3 == IK3_PET && m_bTranformVisPet; }

	BOOL	m_bTranformVisPet;
#endif // __DBSERVER
};

#endif
