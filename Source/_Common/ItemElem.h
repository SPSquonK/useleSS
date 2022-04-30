#pragma once

#include "Piercing.h"

class CItemElem;

namespace ItemElemFeatures {

  class Piercing {
  private:
    CPiercing	m_piercing;
    Piercing() = default;

		[[nodiscard]] ItemProp * GetProp() const;
  public:
    friend CItemElem;

		void CopyPiercing(const CPiercing & piercing) { m_piercing = piercing; }
		void	SerializePiercing(CAr & ar) { m_piercing.Serialize(ar); }

		void	SetPiercingSize(int nSize) { m_piercing.SetPiercingSize(nSize); }
		int		GetPiercingSize() const { return m_piercing.GetPiercingSize(); }
		void	SetPiercingItem(int nth, DWORD dwItem) { m_piercing.SetPiercingItem(nth, dwItem); }
		DWORD	GetPiercingItem(int nth) const { return m_piercing.GetPiercingItem(nth); }
		
		void	SetUltimatePiercingSize(int nSize) { m_piercing.SetUltimatePiercingSize(nSize); }
		int		GetUltimatePiercingSize() { return m_piercing.GetUltimatePiercingSize(); }
		void	SetUltimatePiercingItem(int nth, DWORD dwItem) { m_piercing.SetUltimatePiercingItem(nth, dwItem); }
		DWORD	GetUltimatePiercingItem(int nth) { return m_piercing.GetUltimatePiercingItem(nth); }

		BOOL	IsPierceAble(DWORD dwTargetItemKind3 = NULL_ID, BOOL bSize = FALSE);
		BOOL	IsPiercedItem() { return m_piercing.IsPiercedItem(); }
  };


}

