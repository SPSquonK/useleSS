#pragma once

#if !(defined(__CLIENT) || defined(__WORLDSERVER) || defined(__DBSERVER) || defined(__CORESERVER))
static_assert(false, "ItemContainer.h should only be included in Client, World, DB or CoreServer");
#endif

// defined(__CLIENT) || defined(__WORLDSERVER)
// defined(__DBSERVER) || defined(__CORESERVER)

#include "ItemElem.h"

#pragma once
#include <algorithm>


typedef BYTE ItemPos;

class CItemContainer {
public:
	static constexpr int MAX_SIZE = max(max(MAX_INVENTORY + MAX_HUMAN_PARTS, MAX_GUILDBANK), MAX_VENDOR_INVENTORY);

public:
	LPDWORD	m_apIndex = nullptr;
	DWORD	m_dwIndexNum = 0;
	CItemElem * m_apItem = nullptr;
	DWORD	m_dwItemMax = 0;

	void SetItemContainer(DWORD dwItemMax, DWORD dwExtra = NULL_ID);


public:
	enum class ContainerTypes {
		INVENTORY, GUILDBANK, POCKET0, POCKET12, TRADE, VENDOR, BANK
	};

	CItemContainer() = default;
	CItemContainer(CItemContainer::ContainerTypes containerType);
	virtual ~CItemContainer();
	CItemContainer(const CItemContainer &) = delete;
	CItemContainer & operator=(const CItemContainer &) = delete;

	void SetItemContainer(CItemContainer::ContainerTypes containerType);
	void Clear();

	[[nodiscard]] CItemElem * GetAt(DWORD dwIndex);
	[[nodiscard]] const CItemElem * GetAt(DWORD dwIndex) const;

	BOOL DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex);
	BOOL UnEquip(DWORD dwIndex);
	[[nodiscard]] BOOL IsEquip(DWORD dwObjId) const;

	[[nodiscard]] CItemElem * GetEquip(DWORD dwIndex);
	[[nodiscard]] const CItemElem * GetEquip(DWORD dwIndex) const;

	BOOL Add(CItemElem * pElem, BYTE * pnId = nullptr, short * pnNum = nullptr, BYTE * pnCount = nullptr);

	[[nodiscard]] BOOL	IsFull(CItemElem * pElem, ItemProp * pItemProp, short nNum);

	void	Copy(CItemContainer & rItemContainer);
	void	Serialize(CAr & ar);


#ifdef __CLIENT
	void SetAt(DWORD dwIndex, DWORD dwItemId);
	void SetAt(DWORD dwIndex, CItemElem * pItemElem);

	DWORD	Find(DWORD dwItemId) {
		if (dwItemId == 0)
			return NULL_ID;
		for (int i = 0; i < (int)(m_dwIndexNum); i++) {
			DWORD nId = m_apIndex[i];
			if (nId < 0 || nId >= m_dwItemMax)
				continue;
			if (m_apItem[nId].m_dwItemId == dwItemId)
				return nId;
		}
		return NULL_ID;
	}
#endif	// __CLIENT

	DWORD	GetItemCount(DWORD dwItemId) {
		if (dwItemId == 0)
			return 0;
		int nCount = 0;
		for (int i = 0; i < (int)(m_dwItemMax); i++) {
			CItemElem * pItemElem = (CItemElem *)&m_apItem[i];
			if (pItemElem->m_dwItemId == dwItemId)
				nCount += pItemElem->m_nItemNum;
		}
		return(nCount);
}

	// Equip 관련 

	void	Swap(DWORD dwSrcIndex, DWORD dwDstIndex);
	void	SetAtId(OBJID dwObjId, CItemElem * pItemElem);

	CItemElem * GetAtId(OBJID dwObjId);
	CItemElem * GetAtItemId(DWORD dwItemId);
	CItemElem * GetNewSlotOf(DWORD dwItemID, OBJID formerSlot);
	CItemElem * GetNewSlotOf(CItemElem * formerItem) {
		return GetNewSlotOf(formerItem->m_dwItemId, formerItem->m_dwObjId);
	}
	[[nodiscard]] int		GetAtItemNum(DWORD dwItemId) const;
	void	RemoveAtId(OBJID dwObjId);

	[[nodiscard]] int		GetSize() const { return int(m_dwIndexNum); }
	[[nodiscard]] int		GetMax() const { return int(m_dwItemMax); }

	[[nodiscard]] bool IsFull() {
		return GetCount() >= GetSize();
	}

	[[nodiscard]] int		GetEmptyCount() {
		int nCount = 0;
		for (DWORD i = 0; i < m_dwItemMax; i++) {
			if (m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex < m_dwIndexNum)
				nCount++;
		}
		return(nCount);
	}
	[[nodiscard]] int		GetCount() {
		int nCount = 0;
		for (DWORD i = 0; i < m_dwItemMax; i++) {
			if (m_apItem[i].IsEmpty() == FALSE && m_apItem[i].m_dwObjIndex < m_dwIndexNum)
				nCount++;
		}
		return(nCount);
	}

	CItemElem * GetItem(const ItemProp * itemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption);

private:
	void	Swap2(DWORD dwSrcIndex, DWORD dwDstIndex);

};

