// UltimateWeapon.cpp: implementation of the CUltimateWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UltimateWeapon.h"
#ifdef __WORLDSERVER
#include "ItemUpgrade.h"
#include "User.h"

#include "DPSrvr.h"
#include "dpdatabaseclient.h"
#include "definetext.h"
#include "defineSound.h"
#include "defineObj.h"
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUltimateWeapon::CUltimateWeapon()
{
#ifdef __WORLDSERVER
	m_nSetGemProb = 0;
	m_nRemoveGemProb = 0;
	m_nGen2UniProb = 0;
	m_nUni2UltiProb= 0;
#endif // __WORLDSERVER
}

CUltimateWeapon::~CUltimateWeapon()
{

}

BOOL CUltimateWeapon::Load_GemAbility()
{
	CScanner s;
	
	if( !s.Load( "Ultimate_GemAbility.txt" ) )
		return FALSE;
	
	int nMaxGemNum = 0;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "ABILITY" ) )
		{
			__GEMABILITYKIND GemAbilityKind;
			s.GetToken();
			GemAbilityKind.nAbility = CScript::GetDefineNum( s.Token );
			
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				__GEMABILITY GemAbility;
				GemAbility.dwGemItemId = CScript::GetDefineNum( s.Token );
				s.GetToken();
				GemAbility.dwAbilityItemId = CScript::GetDefineNum( s.Token );
				for( int i=0; i<5; i++ )
					GemAbility.vecAbility.push_back( s.GetNumber() );
				GemAbilityKind.vecAbilityKind.push_back( GemAbility );
				s.GetToken();
			}
			m_vecGemAbilityKind.push_back( GemAbilityKind );
		}
		else if( s.Token == _T( "LEVEL_GEM" ) )
		{
			__MAKEGEMITEM	MakeGemItem;
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				MakeGemItem.dwItemID = CScript::GetDefineNum( s.Token );
				if( MakeGemItem.dwItemID == (DWORD)-1 )		// cr
				{
					Error( "UltimateWeapon::Load_UltimateWeapon() 아이템이 없는것을 설정했음" );
					return FALSE;
				}
				MakeGemItem.nMinLevel = s.GetNumber();
				MakeGemItem.nMaxLevel = s.GetNumber();

				m_vecMakeGemItem.push_back( MakeGemItem );
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

// 아이템 착용 레벨에 따른 보석 종류
DWORD CUltimateWeapon::GetGemKind( DWORD dwLimitLevel )
{
	for( DWORD i=0; i<m_vecMakeGemItem.size(); i++ )
	{
		if( (int)( dwLimitLevel ) <= m_vecMakeGemItem[i].nMaxLevel &&
			(int)( dwLimitLevel ) >= m_vecMakeGemItem[i].nMinLevel )
			return m_vecMakeGemItem[i].dwItemID;
	}

	return NULL_ID;
}

#ifdef __WORLDSERVER
BOOL CUltimateWeapon::Load_UltimateWeapon()
{
	CScanner s;
	
	if( !s.Load( "Ultimate_UltimateWeapon.txt" ) )
		return FALSE;
	

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "SET_GEM" ) )
			m_nSetGemProb = s.GetNumber();
		else if( s.Token == _T( "REMOVE_GEM" ) )
			m_nRemoveGemProb = s.GetNumber();
		else if( s.Token == _T( "GENERAL2UNIQUE" ) )
			m_nGen2UniProb = s.GetNumber();
		else if( s.Token == _T( "UNIQUE2ULTIMATE" ) )
			m_nUni2UltiProb = s.GetNumber();
		else if( s.Token == _T( "MAKE_GEM" ) )
		{
			__MAKEGEMPROB infoMakeGem;
			s.GetToken();	// {
			s.GetToken();
			while( *s.token != '}' )
			{
				int nEnchant = atoi( s.Token );
				infoMakeGem.dwGeneralProb = s.GetNumber();
				infoMakeGem.nGeneralNum = s.GetNumber();
				infoMakeGem.dwUniqueProb = s.GetNumber();
				infoMakeGem.nUniqueNum = s.GetNumber();

				m_mapMakeGemProb.emplace(nEnchant, infoMakeGem);
				s.GetToken();
			}
		}
		else if( s.Token == _T( "ULTIMATE_ENCHANT" ) )
		{
			int nNum = 0;
			int nProb= 0;
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				nNum = atoi( s.Token );
				nProb = s.GetNumber();
				m_mapUltimateProb.emplace( nNum, nProb);
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

// 보석 합성시 부여 되는 능력(능력)
DWORD CUltimateWeapon::GetGemAbilityKindRandom( DWORD dwGemItemid )
{
	if( m_vecGemAbilityKind.empty() == TRUE )
		return NULL_ID;

	int nRandom = xRandom( m_vecGemAbilityKind.size() );

	for(DWORD i=0; i<m_vecGemAbilityKind[nRandom].vecAbilityKind.size(); i++ )
	{
		if( m_vecGemAbilityKind[nRandom].vecAbilityKind[i].dwGemItemId == dwGemItemid )
			return m_vecGemAbilityKind[nRandom].vecAbilityKind[i].dwAbilityItemId;
	}
	return NULL_ID;
}

// 보석 생성 - 무기 아이템 파괴
CUltimateWeapon::MakeGemAnswer CUltimateWeapon::MakeGem(CUser & pUser, OBJID objItemId) {
	if (CItemUpgrade::IsInTrade(pUser)) return Answer::Cancel{};
	
	CItemElem * pItemElem = pUser.m_Inventory.GetAtId(objItemId);
	if (!IsUsableItem(pItemElem)) return Answer::Cancel{};
	if (pUser.m_Inventory.IsEquip(objItemId)) {
		pUser.AddDefinedText(TID_GAME_ULTIMATE_ISEQUIP, "");
		return Answer::Cancel{};
	}

	// 무기가 아닐 때
	if (pItemElem->GetProp()->dwItemKind1 != IK1_WEAPON) {
		return Answer::Cancel{};
	}
	// 일반무기나 유니크 아이템이 아닐 때
	if(	pItemElem->GetProp()->dwReferStat1 != WEAPON_GENERAL &&
		pItemElem->GetProp()->dwReferStat1 != WEAPON_UNIQUE )
	{
		return Answer::Cancel{};
	}
	
	// 레벨에 해당하는 보석 
	const DWORD dwItemId = GetGemKind( pItemElem->GetProp()->dwLimitLevel1 );
	if( dwItemId == NULL_ID )
		return Answer::Cancel{};

	const int nOpt = pItemElem->GetAbilityOption();
	const auto it = m_mapMakeGemProb.find(nOpt);
	
	if( it == m_mapMakeGemProb.end() )
		return Answer::Cancel{};

	DWORD dwProb = it->second.dwGeneralProb;
	int nItemNum = it->second.nGeneralNum;
	if( pItemElem->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		dwProb = it->second.dwUniqueProb;
		nItemNum = it->second.nUniqueNum;
	}
	// 인벤토리가 부족할 때 - 취소
	CItemElem itemElemTemp;
	itemElemTemp.m_dwItemId = dwItemId;
	ItemProp* pItemprop = itemElemTemp.GetProp();
	if( !pItemprop )
		return Answer::Cancel{};

	// 무기를 삭제되서 1칸이 생기므로 2칸이상 일때만 검사
	if( (DWORD)( nItemNum ) > pItemprop->dwPackMax 
		&& pUser.m_Inventory.IsFull( &itemElemTemp, pItemprop, (short)( nItemNum - pItemprop->dwPackMax ) ) )
		return Answer::Inventory{};
	
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser.GetName();
	aLogItem.RecvName = "ULTIMATE_MAKEGEM";
	aLogItem.WorldId = pUser.GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser.GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );	
	pUser.RemoveItem( (BYTE)( objItemId ), 1 );
	
	const DWORD nRandom = xRandom( 1000000 );

	if (nRandom >= dwProb) {
		aLogItem.RecvName = "ULTIMATE_MAKEGEM_FAILED";
		g_DPSrvr.OnLogItem(aLogItem);
		return Answer::Fail{};
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = dwItemId;
	itemElem.m_nItemNum	= nItemNum;
	itemElem.SetSerialNumber();
	itemElem.m_nHitPoint	= 0;

	pUser.CreateItem( &itemElem );
	aLogItem.RecvName = "ULTIMATE_MAKEGEM_SUCCESS";
	g_DPSrvr.OnLogItem( aLogItem, &itemElem, nItemNum );
	return MakeGemSuccess{ dwItemId, nItemNum };
}

// 무기에 보석 합성
int CUltimateWeapon::SetGem( CUser* pUser, OBJID objItemId, OBJID objGemItemId )
{
	CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	CItemElem* pGemItemElem	= pUser->m_Inventory.GetAtId( objGemItemId );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pGemItemElem ) )
		return ULTIMATE_CANCEL;

	// 얼터멋 웨폰이 아닐 때
	if( pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
		return ULTIMATE_ISNOTULTIMATE;


	// 장착되어 있으면 중단
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
		
	int nCount = NULL;
	for( ; nCount < pItemElem->GetUltimatePiercingSize(); nCount++ )
		if( pItemElem->GetUltimatePiercingItem( nCount ) == 0 )
			break;
		
	// 빈곳이 없으면 중단
	if( nCount == pItemElem->GetUltimatePiercingSize() )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_GEMSPACE, "" );
		return ULTIMATE_CANCEL;
	}

	DWORD dwSetItemId = GetGemAbilityKindRandom( pGemItemElem->m_dwItemId );
	if( dwSetItemId == NULL_ID )
		return ULTIMATE_CANCEL;
	
	// 보석 삭제
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_PIERCING";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pGemItemElem, 1 );	
	pUser->RemoveItem( (BYTE)( objGemItemId ), 1 );
	
	int nRandom = xRandom( 1000000 );
	if( nRandom < m_nSetGemProb )
	{
		// 보석 합성
		pUser->UpdateItem(*pItemElem, UI::Piercing::Item{ UI::Piercing::Kind::Ultimate, nCount, dwSetItemId });
		aLogItem.RecvName = "ULTIMATE_PIERCING_SUCCESS";
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
		return ULTIMATE_SUCCESS;	
	}
	aLogItem.RecvName = "ULTIMATE_PIERCING_FAILED";
	g_DPSrvr.OnLogItem( aLogItem, pItemElem );
	return ULTIMATE_FAILED;
}

// 무기에 마지막으로 합성된 보석 제거
int CUltimateWeapon::RemoveGem( CUser* pUser, OBJID objItemId, OBJID objItemGem )
{
	CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	CItemElem* pItemElemGem	= pUser->m_Inventory.GetAtId( objItemGem );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pItemElemGem ) )
		return ULTIMATE_CANCEL;
	
	if( pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
		return ULTIMATE_ISNOTULTIMATE;
	
	// 합성된 보석이 없을 때 X
	if( pItemElem->GetUltimatePiercingItem( 0 ) == 0 )
		return ULTIMATE_CANCEL;

	if( pItemElemGem->m_dwItemId != II_GEN_MAT_MOONSTONE
		&& pItemElemGem->m_dwItemId != II_GEN_MAT_MOONSTONE_1 )
		return ULTIMATE_CANCEL;
 
	// 장착 되어 있을 때 X
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}	

	// 문스톤 삭제
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem, 1 );
	pUser->RemoveItem( (BYTE)( objItemGem ), 1 );
	
	int nRandom = xRandom( 1000000 );
	if( nRandom < m_nRemoveGemProb )
	{
		// 성공시 - 보석 제거
		for( int i=pItemElem->GetUltimatePiercingSize()-1; i>=0; i-- )
		{
			if( pItemElem->GetUltimatePiercingItem( i ) != 0 )
			{
				pUser->UpdateItem(*pItemElem, UI::Piercing::Item{ UI::Piercing::Kind::Ultimate, i, 0 });
				aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE_SUC";
				g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
				break;
			}
		}
		return ULTIMATE_SUCCESS;
	}
	aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE_FAIL";
	g_DPSrvr.OnLogItem( aLogItem, pItemElem );
	return ULTIMATE_FAILED;
}

// 빛나는 오리칼쿰 생성 - 오리칼쿰 5개, 문스톤 5개 합성
CUltimateWeapon::Result CUltimateWeapon::MakeOrichalcum2(CUser & pUser, const std::array<OBJID, MAX_JEWEL> & objItemId) {
	// ~~ Transform into a map the array
	std::map<OBJID, unsigned int> mapObjId;

	for (const OBJID objid : objItemId) {
		++mapObjId[objid];
	}

	// ~~ Check number of orichalcum and moonstones
	static constexpr auto IsOrichalcum = [](const CItemElem & item) {
		return item.m_dwItemId == II_GEN_MAT_ORICHALCUM01 || item.m_dwItemId == II_GEN_MAT_ORICHALCUM01_1;
	};
	static constexpr auto IsMoonstone = [](const CItemElem & item) {
		return item.m_dwItemId == II_GEN_MAT_MOONSTONE || item.m_dwItemId == II_GEN_MAT_MOONSTONE_1;
	};

	unsigned int numberOfOrichalcum = 0;
	unsigned int numberOfMoonstone = 0;

	for (const auto & [objid, quantity] : mapObjId) {
		const CItemElem * const pItemElem = pUser.m_Inventory.GetAtId(objid);
		if (!IsUsableItem(pItemElem)) {
			return Result::Cancel;
		}

		if (quantity > pItemElem->m_nItemNum) {
			return Result::Cancel;
		}

		if (IsOrichalcum(*pItemElem)) {
			numberOfOrichalcum += quantity;
		} else if (IsMoonstone(*pItemElem)) {
			numberOfMoonstone += quantity;
		} else {
			return Result::Cancel;
		}
	}

	if (numberOfOrichalcum != 5 || numberOfMoonstone != 5) {
		return Result::Cancel;
	}
	
	// ~~ Create Orichalcum2
	CItemElem itemElemTemp;
	itemElemTemp.m_dwItemId = II_GEN_MAT_ORICHALCUM02;
	if( !itemElemTemp.GetProp() || pUser.m_Inventory.IsFull( &itemElemTemp, itemElemTemp.GetProp(), 1 ) )
		return Result::Inventory;
	
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser.GetName();
	aLogItem.RecvName = "ULTIMATE_MAKEITEM";
	aLogItem.WorldId = pUser.GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser.GetGold();
	

	for (const auto & [objid, quantity] : mapObjId) {
		CItemElem * pItemElem = pUser.m_Inventory.GetAtId(objid);
		g_DPSrvr.OnLogItem(aLogItem, pItemElem, quantity);
		pUser.RemoveItem(objid, quantity);
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = II_GEN_MAT_ORICHALCUM02;
	itemElem.m_nItemNum	= 1;
	itemElem.SetSerialNumber();
	itemElem.m_nHitPoint	= 0;
	
	pUser.CreateItem(&itemElem);
	aLogItem.RecvName = "ULTIMATE_MAKEITEM_SUCCESS";
	g_DPSrvr.OnLogItem(aLogItem, &itemElem, 1);
	
	return Result::Success;
}

// 무기변환(일반->유니크, 유니크->얼터멋)
int CUltimateWeapon::TransWeapon( CUser* pUser, OBJID objItem, OBJID objGem1, OBJID objGem2 )
{
	CItemElem* pItemElemWeapon	= pUser->m_Inventory.GetAtId( objItem );
	CItemElem* pItemElemGem1	= pUser->m_Inventory.GetAtId( objGem1 );
	CItemElem* pItemElemGem2	= pUser->m_Inventory.GetAtId( objGem2 );
	if( !IsUsableItem( pItemElemWeapon )
		|| !IsUsableItem( pItemElemGem1 )
		|| !IsUsableItem( pItemElemGem2 ) )
		return ULTIMATE_CANCEL;
	
	// 장착하고 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItem ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
		
	// 무기가 아니면 취소
	if( pItemElemWeapon->GetProp()->dwItemKind1 != IK1_WEAPON )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "무기가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}

	// 타겟 ID 가 없을 때
	if( pItemElemWeapon->GetProp()->dwReferTarget1 == NULL_ID )
	{
		return ULTIMATE_CANCEL;	
	}
	
	// 일반, 유니크 아이템이 아닐 때
	if( pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_GENERAL &&
		  pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_UNIQUE )
	{
		return ULTIMATE_CANCEL;
	}

	DWORD dwItemId = GetGemKind( pItemElemWeapon->GetProp()->dwLimitLevel1 );
	if( dwItemId != pItemElemGem1->m_dwItemId || pItemElemGem2->m_dwItemId != II_GEN_MAT_ORICHALCUM02 )
		return ULTIMATE_CANCEL;

	CString strTemp = "UNIQUE_"; 
	CString strLog;
	int nProb = m_nGen2UniProb;
	if( pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		if( pItemElemWeapon->GetAbilityOption() != 10 )
			return ULTIMATE_CANCEL;
		nProb = m_nUni2UltiProb;
		strTemp = "ULTIMATE_";
	}
	// propItem.txt -> dwReferTarget2(참조타겟2)에 값이 변환 확률이 된다.
	// 값이 없으면 Ultimate_UltimateWeapon.txt에 정의된 값이 변환 확률이 된다.
	if( pItemElemWeapon->GetProp()->dwReferTarget2 != NULL_ID )
		nProb = pItemElemWeapon->GetProp()->dwReferTarget2;
	
	BOOL bSmelprot3	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 )
		&& pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		bSmelprot3	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 );
		
		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT3 );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}

	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	strLog = strTemp + "TRANS";
	aLogItem.RecvName = (LPCTSTR)strLog;
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem1, 1 );
	pUser->RemoveItem( (BYTE)( objGem1 ), 1 );
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem2, 1 );
	pUser->RemoveItem( (BYTE)( objGem2 ), 1 );

	int nRandom = xRandom( 1000000 );
	if( nRandom < nProb )
	{
		CItemElem CreateItem;
		CreateItem	= *pItemElemWeapon;
		CreateItem.m_dwItemId = pItemElemWeapon->GetProp()->dwReferTarget1;
		CreateItem.SetSerialNumber();

		if( pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
			CreateItem.SetAbilityOption( 0 );
		

		g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
		pUser->RemoveItem( (BYTE)( objItem ), 1 );
		if( pUser->CreateItem( &CreateItem ) )
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_SUCCESS";
			strLog = strTemp + "TRANS_SUCCESS";
			aLogItem.RecvName = (LPCTSTR)strLog;
			g_DPSrvr.OnLogItem( aLogItem, &CreateItem, 1 );
			return ULTIMATE_SUCCESS;
		}
	}
	else //실패
	{
		if( !bSmelprot3 )
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_FAILED";
			strLog = strTemp + "TRANS_FAILED";
			aLogItem.RecvName = (LPCTSTR)strLog;
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
			pUser->RemoveItem( (BYTE)( objItem ), 1 );
		}
		else
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_PROTECT";
			strLog = strTemp + "TRANS_PROTECT";
			aLogItem.RecvName = (LPCTSTR)strLog;
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon );
		}
	}
	
	return ULTIMATE_FAILED;
}

// 얼터멋 웨폰 제련
int CUltimateWeapon::EnchantWeapon( CUser* pUser, OBJID objItem, OBJID objItemGem )
{
	CItemElem* pItemElemWeapon	= pUser->m_Inventory.GetAtId( objItem );
	CItemElem* pItemElemGem	= pUser->m_Inventory.GetAtId( objItemGem );
	if( !IsUsableItem( pItemElemWeapon ) || !IsUsableItem( pItemElemGem ) )
		return ULTIMATE_CANCEL;

	//얼터멋 웨폰이 아니거나 빛나는 오리칼쿰이 아니면 취소
	if( pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_ULTIMATE ||
		pItemElemGem->m_dwItemId != II_GEN_MAT_ORICHALCUM02 )
		return ULTIMATE_ISNOTULTIMATE;

	// 장착하고 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItem ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}

	int pAbilityOpt = pItemElemWeapon->GetAbilityOption();
	if( (pAbilityOpt+1) > 10 )
	{
		pUser->AddDefinedText( TID_UPGRADE_MAXOVER , "" );
		return ULTIMATE_CANCEL;
	}

	BOOL bSmelprot3	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 ) )
	{
		bSmelprot3	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 );
		
		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT3 );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	
	//보석 소진
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_ENCHANT";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem, 1 );	
	pUser->RemoveItem( (BYTE)( objItemGem ), 1 );

	int nRandom = xRandom( 1000000 );
	const auto it = m_mapUltimateProb.find( pAbilityOpt+1 );
	if( it == m_mapUltimateProb.end() )
		return ULTIMATE_CANCEL;
	
	if( nRandom < it->second )
	{
		pUser->UpdateItem(*pItemElemWeapon, UI::AbilityOption::Up);
		if( pAbilityOpt >= 5 )
			pUser->UpdateItem( *pItemElemWeapon, UI::Piercing::Size::Ultimate );
		aLogItem.RecvName = "ULTIMATE_ENCHANT_SUCCESS";
		g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
		return ULTIMATE_SUCCESS;
	}
	else
	{
		if( bSmelprot3 )	// 보호의 두루마리(최상급) 사용시
		{
			aLogItem.RecvName = "ULTIMATE_ENCHANT_PROTECT";
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon );
		}
		else	// 얼터멋 웨폰 파괴
		{
			aLogItem.RecvName = "ULTIMATE_ENCHANT_FAILED";
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
			pUser->RemoveItem( (BYTE)( objItem ), 1 );
		}
	}	
	return ULTIMATE_FAILED;
}


BYTE CUltimateWeapon::SmeltSafetyUltimate( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr )
{
	//	얼터멋웨폰이 아니면 리턴
	if( pItemMain->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}

	//	최상급 보호의 두루마리가 아니면 리턴
	if( pItemProtScr->GetProp()->dwID != II_SYS_SYS_SCR_SMELPROT3)
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}

	//	제련수치가 max치를 넘었을때 리턴
	if( pItemMain->GetAbilityOption() >= 10 )
	{
		//pUser->AddDefinedText( TID_UPGRADE_MAXOVER );
		return 3;
	}


	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_ENC_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();
	
	// 제련아템 삭제 - 성공이던, 실패던...
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), 1 );
	//	두루마리 로그남길것
	ItemProp* pItemProp = pItemProtScr->GetProp();
	if( pItemProp )
	{
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemProtScr, pItemProp );
		g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	pUser->RemoveItem( (BYTE)( pItemProtScr->m_dwObjId ), 1 );

	// 제련 확률
	const auto it = m_mapUltimateProb.find( pItemMain->GetAbilityOption() + 1 );
	if( it == m_mapUltimateProb.end() )
		return 0;
	
	if( (int)( xRandom( 1000000 ) ) > it->second )
	{
		// 실패
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if( ( pUser->IsMode( TRANSPARENT_MODE ) ) == 0 )
			g_UserMng.AddCreateSfxObj(pUser, XI_INT_FAIL);

		aLogItem.RecvName = "ULTIMATE_ENC_FAIL_SMELTSAFETY";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 2;
	}
	else
	{
		// 성공
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0 )
			g_UserMng.AddCreateSfxObj(pUser, XI_INT_SUCCESS);
		
		pUser->UpdateItem(*pItemMain, UI::AbilityOption::Up);
		if (pItemMain->GetAbilityOption() > 5) {
			pUser->UpdateItem(*pItemMain, UI::Piercing::Size::Ultimate);
		}
		
		aLogItem.RecvName = "ULTIMATE_ENC_SUC_SMELTSAFETY";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 1;
	}
	return 0;
}
#endif // __WORLDSERVER

// 보석 합성 후 능력치 적용
void CUltimateWeapon::SetDestParamUltimate( CMover* pMover, CItemElem* pItemElem, BOOL bEquip, DWORD dwParts )
{
	
	if( dwParts != PARTS_RWEAPON )
		return;

	for (const auto & pair : GetDestParamUltimate(pItemElem)) {
		if( bEquip )
			pMover->SetDestParam(pair.first, pair.second, NULL_CHGPARAM);
		else 
			pMover->ResetDestParam(pair.first, pair.second, TRUE);
	}
}

// 합성된 보석들의 종류 및 능력
std::map<int, int> CUltimateWeapon::GetDestParamUltimate(const CItemElem * pItemElem) const {
	if( !pItemElem || pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
		return {};
	
	boost::container::small_flat_map<DWORD, int, 10> mapItem;
	for (int i=0; i<pItemElem->GetUltimatePiercingSize(); i++) {
		const DWORD piercingItem = pItemElem->GetUltimatePiercingItem(i);
		if (piercingItem != 0) {
			mapItem[piercingItem] += 1;
		}
	}
	
	std::map<int, int> mapDst;
	for (const auto & [itemId, qtt] : mapItem) {
		const ItemProp * const pItemProp	= prj.GetItemProp(itemId);
		const int nResultDst = GetDST(itemId, qtt, pItemProp->dwReferStat2 );
		mapDst[pItemProp->dwReferStat2] += nResultDst;
	}
	return mapDst;
}

// 합성된 보석의 종류와 갯수에 따른 능력치
int CUltimateWeapon::GetDST( DWORD dwItemId, int nItemNum, int nDSTInfo ) const
{
	int nResultDST = 0;
	__GEMABILITYKIND	GemAbilityKind;

	BOOL bFind = FALSE;
	for( DWORD i=0; i<m_vecGemAbilityKind.size(); i++ )
	{
		if( m_vecGemAbilityKind[i].nAbility == nDSTInfo )
		{
			GemAbilityKind = m_vecGemAbilityKind[i];
			bFind = TRUE;
			break;
		}
	}

	if( !bFind )
		return 0;

	for( DWORD i = 0 ; i < GemAbilityKind.vecAbilityKind.size() ; ++i )
	{
		if( dwItemId == GemAbilityKind.vecAbilityKind[i].dwAbilityItemId )
		{
			nResultDST = GemAbilityKind.vecAbilityKind[i].vecAbility[nItemNum-1];
			break;
		}
	}	
	return nResultDST;
}
