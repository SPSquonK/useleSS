#include "StdAfx.h"
#include "ItemContainer.h"

CItemContainer::~CItemContainer() {
	SAFE_DELETE_ARRAY(m_apItem);
	SAFE_DELETE_ARRAY(m_apIndex);
}

CItemContainer::CItemContainer(const CItemContainer::ContainerTypes containerType) {
	SetItemContainer(containerType);
}

void CItemContainer::SetItemContainer(const CItemContainer::ContainerTypes containerType) {
	switch (containerType) {
		case ContainerTypes::INVENTORY:
			SetItemContainer(MAX_INVENTORY, MAX_HUMAN_PARTS);
			break;
		case ContainerTypes::GUILDBANK:
			SetItemContainer(MAX_GUILDBANK);
			break;
		case ContainerTypes::POCKET0:
			SetItemContainer(POCKET_SLOT_SIZE_0);
			break;
		case ContainerTypes::POCKET12:
			SetItemContainer(POCKET_SLOT_SIZE_1_2);
			break;
		case ContainerTypes::TRADE:
			SetItemContainer(MAX_TRADE);
			break;
		case ContainerTypes::VENDOR:
			SetItemContainer(MAX_VENDOR_INVENTORY);
			break;
		case ContainerTypes::BANK:
			SetItemContainer(MAX_BANK);
			break;
	}
}

void CItemContainer::SetItemContainer(DWORD dwItemMax, DWORD dwExtra) {
	m_dwIndexNum = dwItemMax;
	m_dwItemMax = dwItemMax;
	if (dwExtra != NULL_ID)
		m_dwItemMax += dwExtra;

	SAFE_DELETE_ARRAY(m_apItem);
	SAFE_DELETE_ARRAY(m_apIndex);

	m_apItem = new CItemElem[m_dwItemMax];
	m_apIndex = new DWORD[m_dwItemMax];
	Clear();
}

CItemElem * CItemContainer::GetAt(DWORD dwIndex) {
	DWORD dwIdx = m_apIndex[dwIndex];
	if (dwIdx == NULL_ID)
		return NULL;
	CItemElem * pItemElem = &m_apItem[dwIdx];
	if (pItemElem->IsEmpty())
		return NULL;
	return pItemElem;
}

const CItemElem * CItemContainer::GetAt(DWORD dwIndex) const {
	DWORD dwIdx = m_apIndex[dwIndex];
	if (dwIdx == NULL_ID)
		return nullptr;
	const CItemElem * pItemElem = &m_apItem[dwIdx];
	if (pItemElem->IsEmpty())
		return nullptr;
	return pItemElem;
}

CItemElem * CItemContainer::GetEquip(DWORD dwIndex) {
	if (m_dwIndexNum) {
		if (dwIndex < 0 || dwIndex >= (m_dwItemMax - m_dwIndexNum)) {
			WriteError("GETEQUIP//%d, %d, %d", m_dwIndexNum, m_dwItemMax - m_dwIndexNum, dwIndex);
			return NULL;
		}
		return GetAt(m_dwIndexNum + dwIndex);
	}
	return NULL;
}

const CItemElem * CItemContainer::GetEquip(DWORD dwIndex) const {
	if (m_dwIndexNum) {
		if (dwIndex < 0 || dwIndex >= (m_dwItemMax - m_dwIndexNum)) {
			WriteError("CONST GETEQUIP//%d, %d, %d", m_dwIndexNum, m_dwItemMax - m_dwIndexNum, dwIndex);
			return nullptr;
		}
		return GetAt(m_dwIndexNum + dwIndex);
	}
	return nullptr;
}

CItemElem * CItemContainer::GetAtId(DWORD dwId) {
	if (dwId >= m_dwItemMax)
		return nullptr;

	CItemElem * pItemElem = &m_apItem[dwId];
	if (pItemElem->IsEmpty())
		return NULL;
	return pItemElem;
}

BOOL CItemContainer::DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex) {
	dwDstIndex += m_dwIndexNum;		// dwDstIndex = 가방크기(42) + PARTS_???;

	if (dwSrcIndex == dwDstIndex)
		return FALSE;

	if (dwSrcIndex >= m_dwItemMax || dwDstIndex >= m_dwItemMax)
		return FALSE;

	for (DWORD i = 0; i < m_dwItemMax; i++) {
		if (m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex == NULL_ID) {
			if (m_apIndex[dwSrcIndex] >= m_dwItemMax)
				return FALSE;
			m_apIndex[dwDstIndex] = m_apIndex[dwSrcIndex];
			m_apIndex[dwSrcIndex] = i;
			m_apItem[m_apIndex[dwSrcIndex]].m_dwObjIndex = dwSrcIndex;
			m_apItem[m_apIndex[dwDstIndex]].m_dwObjIndex = dwDstIndex;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CItemContainer::UnEquip(DWORD dwIndex) {
	dwIndex += m_dwIndexNum;
	if (dwIndex >= m_dwItemMax)
		return FALSE;

	DWORD dwId = m_apIndex[dwIndex];

	if (dwId >= m_dwItemMax)
		return FALSE;

	for (DWORD i = 0; i < m_dwIndexNum; i++) {
		if (m_apIndex[i] >= m_dwItemMax)
			continue;

		if (m_apItem[m_apIndex[i]].IsEmpty()) {
			m_apItem[m_apIndex[i]].m_dwObjIndex = NULL_ID;
			m_apIndex[dwIndex] = NULL_ID;
			m_apItem[dwId].m_dwObjIndex = i;
			m_apIndex[i] = dwId;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CItemContainer::IsEquip(DWORD dwObjId) const {
	if (!m_apItem) {
		WriteError("CONTAINER//0");
		return FALSE;
	}

	if (dwObjId >= m_dwItemMax)
		return FALSE;

	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwIndexNum)
		return TRUE;

	return FALSE;
}

void CItemContainer::RemoveAtId(OBJID dwObjId) {
	if (dwObjId >= m_dwItemMax)
		return;
	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwItemMax)
		return;
	m_apItem[dwObjId].Empty();
	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwIndexNum) {
		m_apIndex[m_apItem[dwObjId].m_dwObjIndex] = NULL_ID;
		m_apItem[dwObjId].m_dwObjIndex = NULL_ID;
	}
}

void CItemContainer::Clear() {
	for (DWORD i = 0; i < m_dwItemMax; i++) {
		m_apItem[i].Empty();
		m_apItem[i].m_dwObjId = i;
		if (i < m_dwIndexNum) {
			m_apItem[i].m_dwObjIndex = i;
			m_apIndex[i] = i;
		} else {
			m_apIndex[i] = NULL_ID;
		}
	}
}

void CItemContainer::Copy(CItemContainer & rItemContainer) {
	m_dwIndexNum = rItemContainer.m_dwIndexNum;
	memcpy((void *)m_apIndex, (void *)rItemContainer.m_apIndex, sizeof(DWORD) * m_dwItemMax);
	for (u_long i = 0; i < m_dwItemMax; i++) {
		m_apItem[i].m_dwObjId = rItemContainer.m_apItem[i].m_dwObjId;
		m_apItem[i].m_dwObjIndex = rItemContainer.m_apItem[i].m_dwObjIndex;
		if (rItemContainer.m_apItem[i].IsEmpty() == FALSE)
			m_apItem[i] = rItemContainer.m_apItem[i];
		else
			m_apItem[i].Empty();
	}
}

void CItemContainer::Serialize(CAr & ar)	// 0-673	// 466-631
{
	DWORD	adwObjIndex[CItemContainer::MAX_SIZE];

	unsigned char chSize = 0;

	if (ar.IsStoring()) {

		ar.Write(m_apIndex, sizeof(DWORD) * m_dwItemMax);

		u_long uOffset = ar.GetOffset();
		ar << chSize;

		for (u_char ch = 0; ch < m_dwItemMax; ch++)	// 0-504
		{
			if (m_apItem[ch].IsEmpty() == FALSE) {
				ar << ch;
				m_apItem[ch].Serialize(ar);
				chSize++;
			}
			adwObjIndex[ch] = m_apItem[ch].m_dwObjIndex;
		}

		ar.Write(adwObjIndex, sizeof(DWORD) * m_dwItemMax);

		int nBufSize;
		LPBYTE lpBuf = ar.GetBuffer(&nBufSize);
		*(lpBuf + uOffset) = chSize;
	} else {
		ar.Read(m_apIndex, sizeof(DWORD) * m_dwItemMax);
		// Clear
		for (u_long i = 0; i < m_dwItemMax; i++)
			m_apItem[i].Empty();

		ar >> chSize;

		unsigned char ch;
		for (u_int i = 0; i < chSize; i++) {
			ar >> ch;
			m_apItem[ch].Serialize(ar);
		}

		ar.Read(adwObjIndex, sizeof(DWORD) * m_dwItemMax);
		for (unsigned int i = 0; i < m_dwItemMax; i++) {
			m_apItem[i].m_dwObjIndex = adwObjIndex[i];
		}
	}
}

BOOL CItemContainer::IsFull(CItemElem * pElem, ItemProp * pItemProp, short nNum) {
	int nId;
	short nNumtmp = nNum;
	CItemElem * pElemtmp;
	for (DWORD i = 0; i < m_dwIndexNum; i++) {
		nId = m_apIndex[i];
		if (nId < 0 || nId >= (int)(m_dwItemMax))
			continue;
		pElemtmp = (CItemElem *)&m_apItem[nId];
		if (pElemtmp->IsEmpty()) {
			if (nNumtmp > (short)pItemProp->dwPackMax)
				nNumtmp -= (short)pItemProp->dwPackMax;
			else
				return FALSE;
		} else if (pElem->m_dwItemId == pElemtmp->m_dwItemId
			&& pElem->m_byFlag == pElemtmp->m_byFlag && pElem->m_bCharged == pElemtmp->m_bCharged) {
			if (pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax)
				nNumtmp -= ((short)pItemProp->dwPackMax - pElemtmp->m_nItemNum);
			else
				return FALSE;
		}
	}

	return TRUE;
}

CItemElem * CItemContainer::GetAtItemId(DWORD dwItemId) {
	for (DWORD i = 0; i < m_dwItemMax; i++) {
		if (m_apItem[i].m_dwItemId == dwItemId)
			return &m_apItem[i];
	}
	return NULL;
}

CItemElem * CItemContainer::GetNewSlotOf(DWORD dwItemID, OBJID formerSlot) {
	for (DWORD i = 0; i != m_dwIndexNum; ++i) {
		if (m_apItem[i].m_dwItemId == dwItemID && i != formerSlot)
			return &m_apItem[i];
	}
	return nullptr;
}


int CItemContainer::GetAtItemNum(DWORD dwItemId) const {
	int nResult = 0;
	for (DWORD i = 0; i < m_dwItemMax; i++) {
		if (m_apItem[i].m_dwItemId == dwItemId)
			nResult += m_apItem[i].m_nItemNum;
	}
	return nResult;
}


void CItemContainer::SetAtId(OBJID dwObjId, CItemElem * pItemElem) {
	if (dwObjId >= m_dwItemMax)
		return;
	m_apItem[dwObjId] = *pItemElem;
	m_apItem[dwObjId].m_dwObjId = dwObjId;
}
#ifdef __CLIENT
void CItemContainer::SetAt(DWORD dwIndex, CItemElem * pItemElem) {
	CItemElem * pItemElem2 = &m_apItem[m_apIndex[dwIndex]];
	*pItemElem2 = *pItemElem;
	pItemElem2->m_dwObjId = m_apIndex[dwIndex];
	pItemElem2->m_dwObjIndex = dwIndex;
}

#endif // __CLIENT

void CItemContainer::Swap2(DWORD dwSrcIndex, DWORD dwDstIndex) {
	DWORD dwItem = m_apIndex[dwSrcIndex];
	m_apIndex[dwSrcIndex] = m_apIndex[dwDstIndex];
	m_apIndex[dwDstIndex] = dwItem;

	DWORD dwSrcItem = m_apIndex[dwSrcIndex];
	DWORD dwDstItem = m_apIndex[dwDstIndex];
	if (dwSrcItem != NULL_ID)
		m_apItem[dwSrcItem].m_dwObjIndex = dwSrcIndex;
	if (dwDstItem != NULL_ID)
		m_apItem[dwDstItem].m_dwObjIndex = dwDstIndex;
}

void CItemContainer::Swap(DWORD dwSrcIndex, DWORD dwDstIndex) {
	if (dwSrcIndex == dwDstIndex || dwSrcIndex >= m_dwItemMax || dwDstIndex >= m_dwItemMax)
		return;

	CItemElem * pItemElemSrc = GetAtId(m_apIndex[dwSrcIndex]);
	CItemElem * pItemElemDst = GetAtId(m_apIndex[dwDstIndex]);

	if (pItemElemSrc && pItemElemDst && pItemElemSrc->m_dwItemId == pItemElemDst->m_dwItemId && pItemElemSrc->m_byFlag == pItemElemDst->m_byFlag && pItemElemSrc->m_bCharged == pItemElemDst->m_bCharged) {
		ItemProp * pItemProp;
		int nPackMax;
		if ((pItemProp = pItemElemSrc->GetProp()) && (nPackMax = pItemProp->dwPackMax) > 1) {
			short nRemnant = (short)nPackMax - pItemElemDst->m_nItemNum;
			if (nRemnant >= pItemElemSrc->m_nItemNum) {
				pItemElemDst->m_nItemNum += pItemElemSrc->m_nItemNum;
				RemoveAtId(m_apIndex[dwSrcIndex]);
			} else {
				pItemElemDst->m_nItemNum += nRemnant;
				pItemElemSrc->m_nItemNum -= nRemnant;
			}
			return;
		}
	}
	Swap2(dwSrcIndex, dwDstIndex);
}



BOOL CItemContainer::Add(CItemElem * pElem, BYTE * pnId, short * pnNum, BYTE * pnCount) {
	if (pnId)
		*pnCount = 0;
	ItemProp * pItemProp = pElem->GetProp();
	if (!pItemProp)
		return FALSE;
	BOOL bPackItem = TRUE;
	int nId;

	short nNumtmp = ((CItemElem *)pElem)->m_nItemNum;
	if (IsFull(pElem, pItemProp, nNumtmp))	// ¾ÆÀÌÅÛÀ» ³ÖÀ»¼ö ÀÖ´ÂÁö °Ë»ç
		return FALSE;

	if (pItemProp->dwPackMax == 1)
		bPackItem = FALSE;

	CItemElem * pElemtmp;
	if (bPackItem) {
		for (DWORD i = 0; i < m_dwIndexNum; i++) {
			nId = m_apIndex[i];
			if (nId < 0 || nId >= (int)(m_dwItemMax))
				continue;
			pElemtmp = (CItemElem *)&m_apItem[nId];

			if (pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax
				&& pElem->m_byFlag == pElemtmp->m_byFlag && pElem->m_bCharged == pElemtmp->m_bCharged) {
				if (pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax) {
					nNumtmp -= ((short)pItemProp->dwPackMax - pElemtmp->m_nItemNum);
					pElemtmp->m_nItemNum = (short)pItemProp->dwPackMax;
					if (pnId) {
						pnId[*pnCount] = nId;
						pnNum[*pnCount] = pElemtmp->m_nItemNum;
						(*pnCount)++;
					}
				} else {
					pElemtmp->m_nItemNum += nNumtmp;
					if (pnId) {
						pnId[*pnCount] = nId;
						pnNum[*pnCount] = pElemtmp->m_nItemNum;
						(*pnCount)++;
					}
					nNumtmp = 0;
					break;
				}
			}

		}
	}

	if (nNumtmp > 0) {
		for (DWORD i = 0; i < m_dwIndexNum; i++) {
			nId = m_apIndex[i];
			if (nId < 0 || nId >= (int)(m_dwItemMax))
				continue;
			pElemtmp = (CItemElem *)&m_apItem[nId];
			if (pElemtmp->IsEmpty())	// ºó°ø°£ÀÎÁö °Ë»ç
			{
				*pElemtmp = *((CItemElem *)pElem);
				pElemtmp->m_dwObjId = nId;
				pElemtmp->m_dwObjIndex = i;
#ifdef __CLIENT
				pElemtmp->SetTexture();
#endif

				if (nNumtmp > (short)pItemProp->dwPackMax) {
					pElemtmp->m_nItemNum = (short)pItemProp->dwPackMax;
					nNumtmp -= (short)pItemProp->dwPackMax;
					if (pnId) {
						pnId[*pnCount] = nId;
						pnNum[*pnCount] = pElemtmp->m_nItemNum;
						(*pnCount)++;
					}
				} else {
					pElemtmp->m_nItemNum = nNumtmp;
					nNumtmp = 0;
					if (pnId) {
						pnId[*pnCount] = nId;
						pnNum[*pnCount] = pElemtmp->m_nItemNum;
						(*pnCount)++;
					}
					break;
				}
			}
		}
	}
	if (nNumtmp > 0)
		Error("template <class T> BOOL CItemContainer<T>::Add( T* pElem... : nNumtmp > 0");
	return TRUE;
}

CItemElem * CItemContainer::GetItem(const ItemProp * itemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption) {
	for (DWORD i = 0; i < m_dwIndexNum; i++) {
		DWORD nId = m_apIndex[i];
		if (nId < 0 || nId >= (int)(m_dwItemMax))
			continue;
		
		CItemElem * pElemtmp = &m_apItem[nId];

		if (pElemtmp->GetProp() == itemProp
#if defined(__DBSERVER) || defined(__CORESERVER)
			&& pElemtmp->GetOption() == nAbilityOption
#else
			&& pElemtmp->GetAbilityOption() == nAbilityOption
#endif
			&& pElemtmp->m_bItemResist == nItemResist
			&& pElemtmp->m_nResistAbilityOption == nResistAbilityOption) {
			return pElemtmp;
		}
	}

	return nullptr;
}
