#pragma once

#if !(defined(__CLIENT) || defined(__WORLDSERVER) || defined(__DBSERVER) || defined(__CORESERVER))
static_assert(false, "ItemContainer.h should only be included in Client, World, DB or CoreServer");
#endif


// defined(__CLIENT) || defined(__WORLDSERVER)
// defined(__DBSERVER) || defined(__CORESERVER)


#include "ItemElem.h"


#if defined(__CLIENT) || defined(__WORLDSERVER)


#endif


#if defined(__DBSERVER) || defined(__CORESERVER)

template <class T> class CItemContainer {
public:
	LPDWORD	m_apIndex;
	DWORD	m_dwIndexNum;
	T * m_apItem;
	DWORD	m_dwItemMax;

public:
	//	Constructions
	CItemContainer();
	//	CItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = 0xffffffff );
	virtual	~CItemContainer();
	//	Operations
	void Clear();
	void	SetItemContainer(DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = 0xffffffff);
	//	T*	Add( DWORD dwItemId );

	BOOL	Add(T * pElem, BYTE * pnId = NULL, short * pnNum = NULL, BYTE * pnCount = NULL);
#ifdef __SEND_ITEM_ULTIMATE
	BOOL	Add(DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, BYTE * pnId = NULL, short * pnNum = NULL, BYTE * pnCount = NULL, BOOL bCharged = FALSE, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwItemId4 = 0, DWORD dwKeepTime = 0, __int64 iRandomOptItemId = 0);
#else // __SEND_ITEM_ULTIMATE
	BOOL	Add(DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, BYTE * pnId = NULL, short * pnNum = NULL, BYTE * pnCount = NULL, BOOL bCharged = FALSE, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwKeepTime = 0, __int64 iRandomOptItemId = 0);
#endif // __SEND_ITEM_ULTIMATE
	BOOL	IsFull(ItemProp * pItemProp, short nNum, BOOL bCharged);

	T * GetAt(DWORD dwIndex);
	T * GetAtId(OBJID dwObjId);
	T * GetItem(ItemProp * pItemProp, int nAbilityOption);
	T * GetItem(ItemProp * pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption);
	BOOL IsEquip(DWORD dwObjId);
	void RemoveAt(DWORD dwIndex);
	void	RemoveAtId(OBJID dwObjId);

	T * GetEquip(DWORD dwIndex);
	BOOL	UnEquip(DWORD dwIndex);
	void	DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex);
	void	Copy(CItemContainer<T> & rItemContainer);
	void	Serialize(CAr & ar);
};

template <class T> CItemContainer<T>::CItemContainer() {
	m_apItem = NULL;
	m_apIndex = NULL;
}

/*
template <class T> CItemContainer<T>::CItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra )
{
	m_apItem = NULL;
	m_apIndex = NULL;

	SetItemContainer( dwItemType, dwItemMax, dwExtra );
}
*/

template <class T> CItemContainer<T>::~CItemContainer() {
	SAFE_DELETE_ARRAY(m_apItem);
	SAFE_DELETE_ARRAY(m_apIndex);
}

template <class T> void CItemContainer<T>::Clear() {
	for (DWORD i = 0; i < m_dwItemMax; i++) {
		m_apItem[i].Empty();
		SAFE_DELETE(m_apItem[i].m_pPet);
		m_apItem[i].m_dwObjId = i;
		if (i < m_dwIndexNum) {
			m_apItem[i].m_dwObjIndex = i;
			m_apIndex[i] = i;
		} else {
			//			m_apItem[i].m_dwObjIndex	= NULL_ID:
			m_apIndex[i] = NULL_ID;
		}
	}
}

template <class T> void CItemContainer<T>::SetItemContainer(DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra) {
	m_dwIndexNum = dwItemMax;
	m_dwItemMax = dwItemMax;
	if (dwExtra != 0xffffffff)
		m_dwItemMax += dwExtra;

	SAFE_DELETE_ARRAY(m_apItem);
	SAFE_DELETE_ARRAY(m_apIndex);

	m_apItem = new T[m_dwItemMax];
	m_apIndex = new DWORD[m_dwItemMax];
	for (u_long i = 0; i < m_dwItemMax; i++) {
		m_apItem[i].m_dwObjId = i;
		if (i < m_dwIndexNum) {
			m_apItem[i].m_dwObjIndex = i;
			m_apIndex[i] = i;
		} else
			m_apIndex[i] = NULL_ID;
	}
}

#ifdef __SEND_ITEM_ULTIMATE
template <class T> BOOL CItemContainer<T>::Add(DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist, int nResistAbilityOption, BYTE * pnId, short * pnNum, BYTE * pnCount, BOOL bCharged, int nPiercedSize, DWORD dwItemId0, DWORD dwItemId1, DWORD dwItemId2, DWORD dwItemId3, DWORD dwItemId4, DWORD dwKeepTime, __int64 iRandomOptItemId)
#else // __SEND_ITEM_ULTIMATE
template <class T> BOOL CItemContainer<T>::Add(DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist, int nResistAbilityOption, BYTE * pnId, short * pnNum, BYTE * pnCount, BOOL bCharged, int nPiercedSize, DWORD dwItemId0, DWORD dwItemId1, DWORD dwItemId2, DWORD dwItemId3, DWORD dwKeepTime, __int64 iRandomOptItemId)
#endif // __SEND_ITEM_ULTIMATE
{
#if defined(__DBSERVER)
	if (pnId)
		*pnCount = 0;
	ItemProp * pItemProp = prj.GetItemProp(dwItemId);
	ASSERT(pItemProp);
	BOOL bPackItem = TRUE;
	int nId;
	short nNumtmp = nNum;

	if (IsFull(pItemProp, nNum, bCharged))
		return FALSE;

	if (pItemProp->dwPackMax == 1)
		bPackItem = FALSE;

	CItemElem * pElemtmp;
	if (bPackItem) {
		for (DWORD i = 0; i < m_dwIndexNum; i++)	// 합쳐질수 있으면 합치기
		{
			nId = m_apIndex[i];
			pElemtmp = (CItemElem *)&m_apItem[nId];

			if (pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax
				&& pElemtmp->m_bCharged == bCharged && pElemtmp->m_byFlag == 0)	// 아이템 플래그 0 아이템 머지 가능
			{
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
			pElemtmp = (CItemElem *)&m_apItem[nId];
			if (pElemtmp->IsEmpty()) {
				pElemtmp->m_dwItemId = dwItemId;
				pElemtmp->m_dwObjId = nId;
				pElemtmp->m_dwObjIndex = i;
				ItemProp * pItemProp = prj.GetItemProp(dwItemId);
				if (pItemProp)
					pElemtmp->m_nHitPoint = pItemProp->dwEndurance;
				else
					pElemtmp->m_nHitPoint = 0;

				pElemtmp->m_nRepair = 0;
				pElemtmp->SetOption(nOption);
				pElemtmp->SetSerialNumber(iSerialNumber);
				pElemtmp->m_bItemResist = nItemResist;
				pElemtmp->m_nResistAbilityOption = nResistAbilityOption;
				pElemtmp->m_bCharged = bCharged;
				pElemtmp->m_dwKeepTime = 0;

				pElemtmp->SetPiercingSize(nPiercedSize);
				pElemtmp->SetPiercingItem(0, dwItemId0);
				pElemtmp->SetPiercingItem(1, dwItemId1);
				pElemtmp->SetPiercingItem(2, dwItemId2);
				pElemtmp->SetPiercingItem(3, dwItemId3);
#ifdef __SEND_ITEM_ULTIMATE
				pElemtmp->SetPiercingItem(4, dwItemId4);
#endif // __SEND_ITEM_ULTIMATE
				if (dwKeepTime)
					pElemtmp->m_dwKeepTime = dwKeepTime;
				pElemtmp->SetRandomOptItemId(iRandomOptItemId);

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
		Error("template <class T> BOOL CItemContainer<T>::Add( DWORD dwItemId... : nNumtmp > 0");
	return TRUE;
#endif	// __DBSERVER
	return FALSE;
}

#ifndef __CORESERVER

template <class T> BOOL CItemContainer<T>::Add(T * pElem, BYTE * pnId, short * pnNum, BYTE * pnCount) {
	if (pnId)
		*pnCount = 0;
	ItemProp * pItemProp = prj.GetItemProp(pElem->m_dwItemId);
	if (!pItemProp)
		return FALSE;
	BOOL bPackItem = TRUE;
	int nId;

	short nNumtmp = ((CItemElem *)pElem)->m_nItemNum;
	if (IsFull(pItemProp, nNumtmp, pElem->m_bCharged))	// 아이템을 넣을수 있는지 검사
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

			//if( pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax )
			if (pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax
				&& pElemtmp->m_bCharged == pElem->m_bCharged && pElemtmp->m_byFlag == 0) // mirchang 100114 아이템 겹침 문제 수정
			{
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
			if (pElemtmp->IsEmpty())	// 빈공간인지 검사
			{
				*pElemtmp = *((CItemElem *)pElem);
				pElemtmp->m_dwObjId = nId;
				pElemtmp->m_dwObjIndex = i;

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

	return TRUE;
}

#endif

template <class T> BOOL CItemContainer<T>::IsFull(ItemProp * pItemProp, short nNum, BOOL bCharged) {
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
		} else if (pElemtmp->m_dwItemId == pItemProp->dwID && pElemtmp->m_bCharged == bCharged && pElemtmp->m_byFlag == 0) {
			if (pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax)
				nNumtmp -= ((short)pItemProp->dwPackMax - pElemtmp->m_nItemNum);
			else
				return FALSE;
		}
	}

	return TRUE;
}

template <class T> T * CItemContainer<T>::GetAt(DWORD dwIndex) {
	DWORD dwIdx = m_apIndex[dwIndex];
	if (dwIdx == NULL_ID)
		return NULL;
	T * pItemElem = &m_apItem[dwIdx];
	if (pItemElem->IsEmpty())
		return NULL;
	return pItemElem;
}

template <class T> T * CItemContainer<T>::GetAtId(OBJID dwObjId) {
	if (dwObjId >= m_dwItemMax)	return NULL;
	T * pItemElem = &m_apItem[dwObjId];
	if (pItemElem->IsEmpty())
		return NULL;
	return pItemElem;
}

template <class T> T * CItemContainer<T>::GetItem(ItemProp * pItemProp, int nAbilityOption) {
	int nId;
	for (int i = 0; i < m_dwItemMax; i++) {
		nId = m_apIndex[i];
		if (0 <= nId && m_apItem[nId].IsEmpty() == FALSE) {
			T * pItemElem = &m_apItem[nId];
			if (pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption) {
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> T * CItemContainer<T>::GetItem(ItemProp * pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption) {
	int nId;
	for (DWORD i = 0; i < m_dwItemMax; i++) {
		nId = m_apIndex[i];
		if (0 <= nId && m_apItem[nId].IsEmpty() == FALSE) {
			T * pItemElem = &m_apItem[nId];
			if (pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption && pItemElem->m_bItemResist == nItemResist && pItemElem->m_nResistAbilityOption == nResistAbilityOption) {
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> BOOL CItemContainer<T>::IsEquip(DWORD dwObjId) {
	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwIndexNum)
		return TRUE;
	return FALSE;
}
template <class T> void CItemContainer<T>::RemoveAt(DWORD dwIndex) {
	m_apItem[m_apIndex[dwIndex]].Empty();
	if (dwIndex >= m_dwIndexNum) {
		m_apItem[m_apIndex[dwIndex]].m_dwObjIndex = NULL_ID;
		m_apIndex[dwIndex] = NULL_ID;
	}
	//for( int i = dwIndex; i < m_dwIndexNum - 1; i++ )
	//	m_apIndex[ i ] = m_apIndex[ i + 1 ];
	//m_dwIndexNum--;
}

template <class T> void CItemContainer<T>::RemoveAtId(OBJID dwObjId) {
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

template <class T> T * CItemContainer<T>::GetEquip(DWORD dwIndex) {
	return GetAt(m_dwIndexNum + dwIndex);
}

template <class T> BOOL CItemContainer<T>::UnEquip(DWORD dwIndex) {
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

template <class T> void CItemContainer<T>::DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex) {
	dwDstIndex += m_dwIndexNum;
	for (int i = 0; i < m_dwItemMax; i++) {
		if (m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex == NULL_ID) {
			m_apIndex[dwDstIndex] = m_apIndex[dwSrcIndex];
			m_apIndex[dwSrcIndex] = i;
			m_apItem[m_apIndex[dwSrcIndex]].m_dwObjIndex = dwSrcIndex;
			m_apItem[m_apIndex[dwDstIndex]].m_dwObjIndex = dwDstIndex;
			return;
		}
	}
}

template <class T> void CItemContainer<T>::Copy(CItemContainer<T> & rItemContainer) {
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

template <class T> void CItemContainer<T>::Serialize(CAr & ar) {
	//	for( int i = 0; i < m_dwItemMax; i++ )	// 462
	//		m_apItem[i].Serialize( ar );
	DWORD	adwObjIndex[128];

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
		for (u_int i = 0; i < m_dwItemMax; i++)
			m_apItem[i].Empty();

		ar >> chSize;

		unsigned char ch;
		for (u_char i = 0; i < chSize; i++) {
			ar >> ch;
			m_apItem[ch].Serialize(ar);
		}

		ar.Read(adwObjIndex, sizeof(DWORD) * m_dwItemMax);
		for (DWORD i = 0; i < m_dwItemMax; i++) {
			m_apItem[i].m_dwObjIndex = adwObjIndex[i];
		}
	}
}


#endif






