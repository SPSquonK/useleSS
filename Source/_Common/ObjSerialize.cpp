#include "stdafx.h"

#if defined(__DBSERVER) || defined(__CORESERVER) || defined(__ACTIVEXPLAYER) || defined(__VPW)
#include "..\_Network\Objects\Obj.h"
#else
#include "Mover.h"
#endif


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void CCtrl::Serialize( CAr & ar )	// 33
{
	CObj::Serialize( ar );

	if( ar.IsStoring() )
	{
		ar << m_objid;
	}
	else
	{
		ar >> m_objid;
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CAr & operator<<(CAr & ar, const CItemElem & self) {
	ar << self.m_dwObjId << self.m_dwItemId;
	ar << self.m_liSerialNumber;
	ar.WriteString(self.m_szItemText);

	ar << self.m_nItemNum;
	ar << self.m_nRepairNumber << self.m_nHitPoint;
	ar << self.m_nRepair;
	ar << self.m_byFlag;
	ar << self.m_nAbilityOption;
	ar << self.m_idGuild;				// 길드번호도 저장.
	ar << self.m_bItemResist;			// 어느 속성 인가?
	ar << self.m_nResistAbilityOption;	// 속성 추가 능력치 가변 옵션
	ar << self.m_nResistSMItemId;
	ar << self.m_piercing;
	ar << self.m_bCharged;
	ar << self.m_iRandomOptItemId;
	ar << self.m_dwKeepTime;
	if (self.m_dwKeepTime) {
		time_t t = self.m_dwKeepTime - time_null();
		ar << t;
	}
	if (self.m_pPet) {
		ar << (BYTE)0x01;
		ar << *self.m_pPet;
	} else {
		ar << (BYTE)0x00;
	}
	ar << self.m_bTranformVisPet;

	return ar;
}

CAr & operator>>(CAr & ar, CItemElem & self) {
	ar >> self.m_dwObjId >> self.m_dwItemId;
	ar >> self.m_liSerialNumber;
	ar.ReadString(self.m_szItemText, 32);

	ar >> self.m_nItemNum;
	ar >> self.m_nRepairNumber;
	ar >> self.m_nHitPoint;
	ar >> self.m_nRepair;
	ar >> self.m_byFlag;
	ar >> self.m_nAbilityOption;
	ar >> self.m_idGuild;
	ar >> self.m_bItemResist;
	ar >> self.m_nResistAbilityOption;
	ar >> self.m_nResistSMItemId;
	ar >> self.m_piercing;
	ar >> self.m_bCharged;
	ar >> self.m_iRandomOptItemId;
	ar >> self.m_dwKeepTime;

	if (self.m_dwKeepTime) {
		time_t t;
		ar >> t;
#ifdef __CLIENT
		self.m_dwKeepTime = time_null() + t;
#endif	// __CLIENT
	}
	SAFE_DELETE(self.m_pPet);
	BYTE bPet;
	ar >> bPet;
	if (bPet) {
		self.m_pPet = new CPet;
		ar >> *self.m_pPet;
	}
	ar >> self.m_bTranformVisPet;

#ifdef __CLIENT
	self.SetTexture();
#endif

	return ar;
}

CAr & operator<<(CAr & ar, const EQUIP_INFO_ADD & self) {
	ar << self.iRandomOptItemId
		<< self.piercing
		<< self.bItemResist
		<< self.nResistAbilityOption;
	return ar;
}

CAr & operator>>(CAr & ar, EQUIP_INFO_ADD & self) {
	ar >> self.iRandomOptItemId
		>> self.piercing
		>> self.bItemResist
		>> self.nResistAbilityOption;
	return ar;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*
#ifdef __XPET2
void CPetElem::Serialize( CAr & ar )	// 18
{
	CItemBase::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_nMaxLevel << m_nLevel << m_dwHungry << m_dwFeeling;
	} else
	// Loading
	{
		ar >> m_nMaxLevel >> m_nLevel >> m_dwHungry >> m_dwFeeling;
	}
}
#endif // xPet2
*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void CItem::Serialize( CAr & ar )	// 45
{
	CCtrl::Serialize( ar );		// 33
	
	if( ar.IsStoring() )
	{
		ar << *m_pItemBase;
	}
	else
	{
		SAFE_DELETE( m_pItemBase );
		m_pItemBase	= new CItemElem;
		ar >> *m_pItemBase;
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// 지웠음. 따로 보관함...xuzhu
/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

