#include "StdAfx.h"
#include "ItemElem.h"

namespace ItemElemFeatures {


	// bSize는 피어싱 사이즈를 늘릴 수 있는지 검사할 때 TRUE값을 setting 한다.
	// bSize를 TRUE로 할 경우 dwTagetItemKind3는 NULL_ID로 한다.
	BOOL Piercing::IsPierceAble(DWORD dwTargetItemKind3, BOOL bSize) {
		if (!GetProp())
			return FALSE;

		int nPiercedSize = GetPiercingSize();
		if (bSize) // 피어싱 사이즈를 늘리는 경우
			nPiercedSize++;

		if (GetProp()->dwItemKind3 == IK3_SUIT) {
			if (nPiercedSize <= MAX_PIERCING_SUIT) {
				if (dwTargetItemKind3 == NULL_ID)
					return TRUE;
				else if (dwTargetItemKind3 == IK3_SOCKETCARD)
					return TRUE;
			}
		}

		else if (GetProp()->dwItemKind3 == IK3_SHIELD
			|| GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT
			|| GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC
			) {
			if (nPiercedSize <= MAX_PIERCING_WEAPON) {
				if (dwTargetItemKind3 == NULL_ID)
					return TRUE;
				else if (dwTargetItemKind3 == IK3_SOCKETCARD2)
					return TRUE;
			}
		}

		else if (CItemElem::Cast(this)->IsVisPet()) {
			if (nPiercedSize <= MAX_VIS) {
				if (dwTargetItemKind3 == NULL_ID)
					return TRUE;
				else if (dwTargetItemKind3 == IK3_VIS)
					return TRUE;
			}
		}

		return FALSE;
	}


	ItemProp * Piercing::GetProp() const {
		return CItemElem::Cast(this)->GetProp();
	}

}

