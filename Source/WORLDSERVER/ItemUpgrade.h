#pragma once

struct __ATTRIBUTE_ENCHANT
{
	int nProb;
	int nAddDamageRate;
	int nDefenseRate;
	int nAddAtkDmgRate;
	__ATTRIBUTE_ENCHANT()
	{
		nProb = 0;
		nAddDamageRate = 0;
		nDefenseRate = 0;
		nAddAtkDmgRate = 0;
	}
};

class CItemUpgrade {
public:
	struct PiercingUpgrade {
	public:
		[[nodiscard]] unsigned int GetSizeProb(const CItemElem & pItemElem) const;
		void LoadScript(CLuaBase & lua);

	private:
		std::map<int, unsigned int>	m_mapSuitProb;
		std::map<int, unsigned int>	m_mapWeaponProb;
	};

private:
	PiercingUpgrade m_piercing;

public:
	CItemUpgrade(void);
	~CItemUpgrade(void);
	static CItemUpgrade*	GetInstance( void );

	static bool IsInTrade(const CUser & pUser);
	static CItemElem * GetModifiableItem(CUser & pUser, DWORD dwId);

	void	LoadScript();
	void	OnPiercingSize( CUser* pUser, DWORD dwId1, DWORD dwId2, DWORD dwId3 );
	void	OnPiercing( CUser* pUser, DWORD dwItemId, DWORD dwSocketCard );
	static void OnPiercingRemove(CUser & pUser, DWORD objId);

	BYTE	OnSmeltSafety( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
	void	OnEnchant( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetAttributeDamageFactor( int nAbilityOption );
	int		GetAttributeDefenseFactor( int nAbilityOption );
	int		GetAttributeAddAtkDmgFactor( int nAbilityOption );
	int		GetMaxGeneralEnchantSize() { return m_mapGeneralEnchant.size(); }
	int		GetMaxAttributeEnchantSize() { return m_mapAttributeEnchant.size(); }
	void	ChangeAttribute( CUser* pUser, DWORD dwTargetItem, DWORD dwUseItem , SAI79::ePropType nAttribute );
#ifdef __SYS_ITEMTRANSY
	void	OnItemTransy( CUser* pUser, OBJID objidTarget, OBJID objidTransy, DWORD dwChangeId, BOOL bCash );
private:
	BOOL	RunItemTransy( CUser* pUser, CItemElem* pItemElemTarget, DWORD dwChangeId );
	int		m_nItemTransyLowLevel, m_nItemTransyHighLevel;
#endif // __SYS_ITEMTRANSY

private:
	
private:
	void	RefineAccessory( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	void	RefineCollector( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	BYTE	SmeltSafetyNormal( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
	BYTE	SmeltSafetyGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
	BYTE	SmeltSafetyAccessory( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr );
	BYTE	SmeltSafetyPiercingSize( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr );
	BYTE	SmeltSafetyAttribute(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
public:

	void	EnchantGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetGeneralEnchantProb( int nAbilityOption );
	void	EnchantAttribute( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetAttributeEnchantProb( int nAbilityOption );
	
	DWORD	WhatEleCard( DWORD dwItemType );	// °ð »èÁ¦

	std::map<int, int>	m_mapGeneralEnchant;
	std::map<int, __ATTRIBUTE_ENCHANT> m_mapAttributeEnchant;

public:
	void	PetVisSize( CUser* pUser, OBJID objIdMaterial );
	void	SetPetVisItem( CUser* pUser, OBJID objIdVis );
	void	RemovePetVisItem( CUser* pUser, int nPosition, BOOL bExpired = FALSE );
	void	PutPetVisItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nPosition );
	void	SwapVis( CUser* pUser, int nPos1, int nPos2 );
	void	ChangeVisPetSfx( CUser* pUser, CItemElem* pItemElemPet );
	void	TransFormVisPet( CUser* pUser, OBJID objIdMaterial );

private:

};

