#include "stdafx.h"
#include ".\itemupgrade.h"

#include "defineObj.h"
#include "defineSound.h"
#include "defineText.h"

#include "collecting.h"
#include "definesound.h"
#include "defineitem.h"

#include "User.h"
#include "DPSrvr.h"
#include "DPDatabaseClient.h"

#include "AIPet.h"

#include "ItemMorph.h"

CItemUpgrade::CItemUpgrade(void)
#ifdef __SYS_ITEMTRANSY
: m_nItemTransyLowLevel(1000000), m_nItemTransyHighLevel(2000000)
#endif // __SYS_ITEMTRANSY
{
	LoadScript();
}

CItemUpgrade::~CItemUpgrade(void)
{
	m_mapGeneralEnchant.clear();
	m_mapAttributeEnchant.clear();
}

CItemUpgrade* CItemUpgrade::GetInstance( void )
{	
	static CItemUpgrade sItemUpgrade;
	return &sItemUpgrade;
}

void CItemUpgrade::LoadScript()
{
	CLuaBase	lua;
	if( lua.RunScript( "ItemUpgrade.lua" ) != 0 )
	{
		Error( "CItemUpgrade::LoadScript() - ItemUpgrade.lua Run Failed!!!" );
		ASSERT(0);
	}

	m_piercing.LoadScript(lua);
	
	// 일반제련
	lua.GetGloabal( "tGeneral" );
	lua.PushNil();
	while( lua.TableLoop( -2 ) )
	{
		m_mapGeneralEnchant.emplace( static_cast<int>(lua.ToNumber(-2)), static_cast<int>(lua.ToNumber(-1)) );
		lua.Pop( 1 );
	}
	lua.Pop(0);

 	// 속성제련
	lua.GetGloabal( "tAttribute" );
 	lua.PushNil();
 	while( lua.TableLoop( -2 ) )
 	{
 		__ATTRIBUTE_ENCHANT attrEnchant;
		attrEnchant.nProb = static_cast<int>(lua.GetFieldToNumber( -1, "nProb" ));
		attrEnchant.nAddDamageRate = static_cast<int>(lua.GetFieldToNumber( -1, "nDamageRate" ));
		attrEnchant.nDefenseRate = static_cast<int>(lua.GetFieldToNumber( -1, "nDefenseRate" ));
		attrEnchant.nAddAtkDmgRate = static_cast<int>(lua.GetFieldToNumber( -1, "nAddAtkDmgRate" ));
		m_mapAttributeEnchant.emplace( static_cast<int>(lua.ToNumber(-2)), attrEnchant );  
		lua.Pop( 1 );
  	}
	lua.Pop(0);
#ifdef __SYS_ITEMTRANSY
	m_nItemTransyLowLevel = static_cast<int>( lua.GetGlobalNumber( "nItemTransyLowLevel" ) );
	m_nItemTransyHighLevel = static_cast<int>( lua.GetGlobalNumber( "nItemTransyHighLevel" ) );
	lua.Pop(0);
#endif // __SYS_ITEMTRANSY
}

void CItemUpgrade::PiercingUpgrade::LoadScript(CLuaBase & lua) {
	// 방어구 피어싱
	lua.GetGloabal("tSuitProb");
	lua.PushNil();
	while (lua.TableLoop(-2)) {
		m_mapSuitProb.emplace(static_cast<int>(lua.ToNumber(-2)), static_cast<unsigned int>(lua.ToNumber(-1)));
		lua.Pop(1);
	}
	lua.Pop(0);

	// 무기 피어싱
	lua.GetGloabal("tWeaponProb");
	lua.PushNil();
	while (lua.TableLoop(-2)) {
		m_mapWeaponProb.emplace(static_cast<int>(lua.ToNumber(-2)), static_cast<unsigned int>(lua.ToNumber(-1)));
		lua.Pop(1);
	}
	lua.Pop(0);
}

bool CItemUpgrade::IsInTrade(const CUser & pUser) {
	return pUser.m_vtInfo.IsInSomeKindOfTrade();
}

CItemElem * CItemUpgrade::GetModifiableItem(CUser & pUser, DWORD dwId) {
	CItemElem * pItemElem = pUser.m_Inventory.GetAtId(dwId);
	if (!pItemElem) return nullptr;
	if (!IsUsableItem(pItemElem)) return nullptr;
	if (pUser.m_Inventory.IsEquip(dwId)) return nullptr;
	return pItemElem;
}

void CItemUpgrade::OnPiercingSize( CUser* pUser, DWORD dwId1, DWORD dwId2, DWORD dwId3 )
{
	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwId1 );
	CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwId2 );
	CItemElem* pItemElem2	= pUser->m_Inventory.GetAtId( dwId3 );

	if( !IsUsableItem( pItemElem0 ) || !IsUsableItem( pItemElem1 ) )
		return;

	if( pUser->m_vtInfo.GetOther() || pUser->m_vtInfo.VendorIsVendor() ) // 거래 중이면...
		return;	
	
	if( pUser->m_Inventory.IsEquip( pItemElem0->m_dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT );
		return;
	}	

	//////////////// 첫번째 아이템 //////////////// 
	if( !pItemElem0->IsPierceAble( NULL_ID, TRUE ) )
	{
		pUser->AddDefinedText( TID_PIERCING_POSSIBLE_ITEM );
		return;
	}

	if( pItemElem1->GetProp()->dwID != II_GEN_MAT_MOONSTONE 
		&& pItemElem1->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1 )
	{
		pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );			// 피어싱에 필요한 주사위가 아니면 불가능
		return;
	}
	
	if( IsUsableItem( pItemElem2 ) && pItemElem2->m_dwItemId != II_SYS_SYS_SCR_PIEPROT )
	{
		pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );			// 상용아이템이 아니면 불가능
		return;
	}

	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "PIERCING";
	aLogItem.WorldId = pUser->GetWorld()->GetID();

	int nCost = 100000;

	if( nCost > 0 )
	{
		if( pUser->GetGold() < nCost )
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY , "" );
			return;
		}

		pUser->AddGold( -( nCost ) );

		aLogItem.Gold = pUser->GetGold() + nCost;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Action = "!";
		//aLogItem.ItemName = "SEED";
		_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
		aLogItem.itemNumber = nCost;
		g_DPSrvr.OnLogItem( aLogItem );
	}
	else
	{
		return;
	}
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

	int nPersent = 0;
	if( pItemElem1->GetProp()->dwID == II_GEN_MAT_MOONSTONE
		|| pItemElem1->GetProp()->dwID == II_GEN_MAT_MOONSTONE_1 )
		nPersent = m_piercing.GetSizeProb( *pItemElem0 );

	if( nPersent < (int)( xRandom( 10000 ) ) )
	{	// 실패
		if( pItemElem2 )								// 상용화 아이템을 사용하였으면...
			aLogItem.RecvName = "PIERCING_PROTECTED";
		aLogItem.Action = "!";
		g_DPSrvr.OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
		aLogItem.RecvName = "PIERCING";

		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		g_UserMng.AddCreateSfxObj(pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->AddDefinedText( TID_MMI_PIERCINGFAIL , "" );
		
		if( pItemElem2 == NULL )								// 상용화 아이템을 사용하지 않았다면 
			pUser->RemoveItem( (BYTE)( dwId1 ), (short)1 );	// 피어싱 대상 아이템은 삭제된다.			
	}
	else
	{	// 성공			
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
		g_UserMng.AddCreateSfxObj(pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			
		pUser->UpdateItem(*pItemElem0, UI::Piercing::Size::IncrementRegular);
		pUser->AddDefinedText( TID_MMI_PIERCINGSUCCESS , "" );

		aLogItem.Action = "#";
		g_DPSrvr.OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
	}
	aLogItem.Action = "!";
	g_DPSrvr.OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );

	// 다이스와 유료아이템 삭제한다.
	pUser->RemoveItem( (BYTE)( dwId2 ), (short)1 );

	if( dwId3 != NULL_ID )
	{
		aLogItem.Action = "!";
		if( IsUsableItem( pItemElem2 ) )
		{
			g_DPSrvr.OnLogItem( aLogItem, pItemElem2, pItemElem2->m_nItemNum );
			g_dpDBClient.SendLogSMItemUse( "2", pUser, pItemElem2, pItemElem2->GetProp() );
			pUser->RemoveItem( (BYTE)( dwId3 ), (short)1 );
		}
		else
		{
			g_DPSrvr.OnLogItem( aLogItem, NULL, 0 );
		}
	}
}

unsigned int CItemUpgrade::PiercingUpgrade::GetSizeProb(const CItemElem & pItemElem) const {
	using namespace ItemProps;
	const PiercingType piercingType = pItemElem.GetPiercingType();

	const std::map<int, unsigned int> * probsMap = nullptr;

	if (piercingType == PiercingType::LetterCard) {
		probsMap = &m_mapWeaponProb;
	} else if (piercingType == PiercingType::NumericCard) {
		probsMap = &m_mapSuitProb;
	}

	if (!probsMap) return 0;

	const auto it = probsMap->find(pItemElem.GetPiercingSize() + 1);
	if (it == probsMap->end()) return 0;

	return it->second;
}

void CItemUpgrade::OnPiercing( CUser* pUser, DWORD dwItemId, DWORD dwSocketCard )
{
	// 인벤토리에 있는지 장착되어 있는지 확인을 해야 함
	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwItemId );
	CItemElem* pItemElem1	= pUser->m_Inventory.GetAtId( dwSocketCard );
	if( !IsUsableItem( pItemElem0 ) || !IsUsableItem( pItemElem1 ) )
		return;

	// 장착되어 있는 아이템은 피어싱 못함
	if( pUser->m_Inventory.IsEquip( dwItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return;
	}			

	// 카드가 들어갈 아이템이 피어싱 가능한지 검사
	if( !pItemElem0->IsPierceAble() )
	{
		pUser->AddDefinedText(  TID_PIERCING_POSSIBLE_ITEM, "" );
		return;
	}

	//  IK3_SOCKETCARD?가 아니면 피어싱 못함
	if( !pItemElem0->IsPierceAble( pItemElem1->GetProp()->dwItemKind3 ) )
	{
		pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL , "" );			
		return;					
	}

	// 총 피어싱된수와 전체 수를 비교한다.
	int nSize = pItemElem0->GetPiercingSize();

	int nCount = 0;
	for( int j = 0; j < nSize; j++ )
	{
		if( pItemElem0->GetPiercingItem( j ) != 0 )
			nCount++;
	}

	// 빈곳이 없으면 중단
	if( nCount == nSize )
	{
		pUser->AddDefinedText( TID_PIERCING_ERROR_NOPIERCING, "" );
		return;
	}

	// 康
	if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
		return;
	if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
		return;

	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "PIERCING";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

	if( pUser->Pierce( pItemElem0, pItemElem1->m_dwItemId ) )
	{
		aLogItem.Action = "$";
		g_DPSrvr.OnLogItem( aLogItem, pItemElem0, pItemElem0->m_nItemNum );
		aLogItem.Action = "!";
		g_DPSrvr.OnLogItem( aLogItem, pItemElem1, pItemElem1->m_nItemNum );

		// 아이템 박기 성공~
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		// 재료 아이템 삭제
		pUser->RemoveItem( (BYTE)( dwSocketCard ), (short)1 );
	}

}

void CItemUpgrade::OnPiercingRemove(CUser & pUser, DWORD objId) {
	static constexpr int PiercingRemovalCost = 1000000;

	// User input sanitization
	if (IsInTrade(pUser)) return;

	CItemElem * pItemElem = GetModifiableItem(pUser, objId);
	if (!pItemElem) return;

	const ItemProps::PiercingType piercingType = pItemElem->GetPiercingType();

	if (!piercingType.IsOnEquipement()) return;

	// Is rich enough?
	if (pUser.GetGold() < PiercingRemovalCost) {
		pUser.AddDefinedText(TID_GAME_LACKMONEY);
		return;
	}

	// Find card to remove
	static constexpr auto FindSlotToRemove = [](const CItemElem & itemElem) -> std::optional<int> {
		for (int i = itemElem.GetPiercingSize() - 1; i >= 0; --i) {
			if (itemElem.GetPiercingItem(i) != 0) {
				return i;
			}
		}

		return std::nullopt;
	};

	const auto toRemove = FindSlotToRemove(*pItemElem);
	if (!toRemove) {
		pUser.AddDefinedText(TID_GAME_REMOVE_PIERCING_ERROR);
		return;
	}
	
	const int i = toRemove.value();

	// Apply change
	pUser.AddGold(-PiercingRemovalCost);
	pUser.AddDefinedText(TID_GAME_REMOVE_PIERCING_SUCCESS);
	pUser.UpdateItem(*pItemElem, UI::Piercing::Item{ UI::Piercing::Kind::Regular, i, 0 });

	// Log
	LogItemInfo aLogItem;
	aLogItem.Action = "$";
	aLogItem.SendName = pUser.GetName();
	aLogItem.RecvName = "PIERCING_REMOVE";
	aLogItem.WorldId = pUser.GetWorld()->GetID();
	aLogItem.Gold = pUser.GetGold() + PiercingRemovalCost;
	aLogItem.Gold2 = pUser.GetGold();
	aLogItem.Gold_1 = -PiercingRemovalCost;
	g_DPSrvr.OnLogItem(aLogItem, pItemElem, 1);
}

void	CItemUpgrade::OnEnchant( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial )
{
	if( !IsUsableItem( pItemMain ) || !IsUsableItem( pItemMaterial ) )
		return;
	// 대상이 장착중인가?
	if( pUser->m_Inventory.IsEquip( pItemMain->m_dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return;
	}
	
	switch( pItemMaterial->GetProp()->dwItemKind3 )
	{
		case IK3_ELECARD:	
			EnchantAttribute( pUser, pItemMain, pItemMaterial );
			break;

		case IK3_ENCHANT:	
			EnchantGeneral( pUser, pItemMain, pItemMaterial );
			break;

		default:
			if( pItemMain->IsAccessory() )
				RefineAccessory( pUser, pItemMain, pItemMaterial );

			else if( pItemMain->IsCollector() )
				RefineCollector( pUser, pItemMain, pItemMaterial );
			break;
	}
}

BYTE	CItemUpgrade::OnSmeltSafety( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr )
{
	// 재료에 따라 분기
	switch( pItemMaterial->GetProp()->dwItemKind3 )
	{
		//	오리칼쿰일때
		case IK3_ENCHANT:
			//	일반제련(일반 or 얼터멋웨폰인지 재검사)
			return SmeltSafetyNormal( pUser, pItemMain, pItemMaterial, pItemProtScr, pItemSmeltScr );

		//	문스톤일때
		case IK3_PIERDICE:
			//	악세서리인가
			if( pItemMain->IsAccessory() )
				return SmeltSafetyAccessory( pUser, pItemMain, pItemMaterial, pItemProtScr );
			//	피어싱인가
			else if( pItemMain->IsPierceAble( NULL_ID, TRUE ) )
				return SmeltSafetyPiercingSize( pUser, pItemMain, pItemMaterial, pItemProtScr );

		// 속성카드 일때
		case IK3_ELECARD:
			return SmeltSafetyAttribute( pUser, pItemMain, pItemMaterial, pItemProtScr, pItemSmeltScr );

		default:
			break;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyNormal( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr )
{
	//	오리칼쿰인가, 빛나는 오리칼쿰인가
	switch( pItemMaterial->GetProp()->dwID )
	{
		//	오리칼쿰일때 일반제련
		case II_GEN_MAT_ORICHALCUM01:
		case II_GEN_MAT_ORICHALCUM01_1:
			return SmeltSafetyGeneral( pUser, pItemMain, pItemMaterial, pItemProtScr, pItemSmeltScr );

		//	빛나는 오리칼쿰일때 얼터멋웨폰 제련
		case II_GEN_MAT_ORICHALCUM02:
			return prj.m_UltimateWeapon.SmeltSafetyUltimate( pUser, pItemMain, pItemMaterial, pItemProtScr );

		default:
			break;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr )
{
	//	제련가능한 아이템이 아닐경우 리턴
	if( !CItemElem::IsDiceRefineryAble(pItemMain->GetProp()) )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}
	
	//	얼터멋웨폰 이거나 일반보호의 두루마리가 아닐경우 리턴
	if( pItemMain->GetProp()->dwReferStat1 == WEAPON_ULTIMATE || pItemProtScr->GetProp()->dwID != II_SYS_SYS_SCR_SMELPROT )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}
	
	//	제련수치가 max치를 넘었을때 리턴
	if( pItemMain->GetAbilityOption() >= GetMaxGeneralEnchantSize() )
	{
		//pUser->AddDefinedText( TID_UPGRADE_MAXOVER );
		return 3;
	}
	
	// 1000단위의 성공 퍼센트
	int nPercent = GetGeneralEnchantProb( pItemMain->GetAbilityOption() );
	
	//	제련의 두루마리를 사용했다면
	if( pItemSmeltScr != NULL )
	{
		//	제련의 두루마리가 맞는지 확인
		if( IsUsableItem( pItemSmeltScr ) && pItemSmeltScr->GetProp()->dwID == II_SYS_SYS_SCR_SMELTING )
		{
			//	제련의 두루마리 사용가능 수치인가
			if( pItemMain->GetAbilityOption() < 7 )
			{
				nPercent += 1000;
				ItemProp* pItemProp = pItemSmeltScr->GetProp();
				if( pItemProp)
				{
					g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemSmeltScr, pItemProp );
					g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
				}
				pUser->RemoveItem( (BYTE)( pItemSmeltScr->m_dwObjId ), 1 );
			}
		}
		//	제련의 두루마리가 아닐때
		else
			return 0;
	}

	LogItemInfo aLogItem;
	aLogItem.Action = "N";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();

	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	// 제련아템 삭제 - 성공이던, 실패던...
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), 1 );
	//	보호의 두루마리 로그남길것
	ItemProp* pItemProp = pItemProtScr->GetProp();
	if( pItemProp )
	{
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemProtScr, pItemProp );
		g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	pUser->RemoveItem( (BYTE)( pItemProtScr->m_dwObjId ), 1 );

	// 해당 아이템의 속성, 일반 레벨을 얻어 확율을 꺼낸다.
	if( (int)( xRandom( 10000 ) ) > nPercent )
	{
		// 실패 메세지 출력
		//pUser->AddDefinedText( TID_UPGRADE_FAIL );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		
		aLogItem.Action = "F";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 2;
	}
	else
	{
		// 성공
		//pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		
		pUser->UpdateItem(*pItemMain, UI::AbilityOption::Up);
		aLogItem.Action = "H";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 1;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyAccessory(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr )
{
	// 재료가 문스톤인가?
	if( pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE && pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1 )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}
	//	악세서리 보호의 두루마리가 아닐경우 리턴
	if( pItemProtScr->GetProp()->dwID != II_SYS_SYS_SCR_SMELPROT4 )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}
	//	제련수치가 max치를 넘었을때 리턴
	if( pItemMain->GetAbilityOption() >= MAX_AAO )	// 20
	{
		//pUser->AddDefinedText( TID_GAME_ACCESSORY_MAX_AAO );
		return 3;
	}

	// log
	LogItemInfo aLogItem;
	aLogItem.SendName	= pUser->GetName();
	aLogItem.RecvName	= "UPGRADEITEM_SMELTSAFETY";
	aLogItem.WorldId	= pUser->GetWorld()->GetID();
	aLogItem.Gold	= pUser->GetGold();
	aLogItem.Gold2	= pUser->GetGold();

	aLogItem.Action	= "N";
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), 1 );
	// 액세서리 보호의 두루마리 로그 남길것
	ItemProp* pItemProp = pItemProtScr->GetProp();
	if( pItemProp )
	{
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemProtScr, pItemProp );
		g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	pUser->RemoveItem( (BYTE)( pItemProtScr->m_dwObjId ), 1 );

	// 제련 확률
	DWORD dwProbability = g_AccessoryProperty.GetProbability( pItemMain->GetAbilityOption() );

	if( xRandom( 10000 ) > dwProbability )	// 실패
	{
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

		aLogItem.Action	= "L";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 2;
	}
	else	// 성공
	{
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
			
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0)
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem(*pItemMain, UI::AbilityOption::Up);

		aLogItem.Action		= "H";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 1;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyPiercingSize(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr )
{
	// 재료가 문스톤인가?
	if( pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE && pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1 )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}
	//	피어싱 보호의 두루마리인가
	if( pItemProtScr->m_dwItemId != II_SYS_SYS_SCR_PIEPROT )
	{
		//pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );			// 상용아이템이 아니면 불가능
		return 0;
	}

	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "PIERCING_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();

	int nCost = 100000;

	if( 0 < nCost )
	{
		if( pUser->GetGold() < nCost )
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY );
			return 0;
		}

		pUser->AddGold( -( nCost ) );

		aLogItem.Gold = pUser->GetGold() + nCost;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Action = "!";
		//aLogItem.ItemName = "SEED";
		_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
		aLogItem.itemNumber = nCost;
		g_DPSrvr.OnLogItem( aLogItem );
	}
	else
	{
		return 0;
	}
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

	// 재료와 두루마리 삭제.
	aLogItem.Action = "!";
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), 1 );
	//	피어싱 보호의 두루마리 로그 남길것
	ItemProp* pItemProp = pItemProtScr->GetProp();
	if( pItemProp )
	{
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemProtScr, pItemProp );
		g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	pUser->RemoveItem( (BYTE)( pItemProtScr->m_dwObjId ), 1 );
	
	int nPercent = m_piercing.GetSizeProb( *pItemMain );

	if( nPercent < xRandom( 10000 ) )
	{	// 실패
		//pUser->AddDefinedText( TID_MMI_PIERCINGFAIL );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0)
			g_UserMng.AddCreateSfxObj( pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		
		aLogItem.Action = "!";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 2;
	}
	else
	{	// 성공
		//pUser->AddDefinedText( TID_MMI_PIERCINGSUCCESS );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0)
			g_UserMng.AddCreateSfxObj( pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		pUser->UpdateItem(*pItemMain, UI::Piercing::Size::IncrementRegular);

		aLogItem.Action = "#";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 1;
	}
	return 0;
}

void	CItemUpgrade::RefineAccessory( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial )
{
	// 재료가 문스톤인가?
	if( pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE && pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1 )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return;
	}
	if( pItemMain->GetAbilityOption() >= MAX_AAO )	// 20
	{
		pUser->AddDefinedText( TID_GAME_ACCESSORY_MAX_AAO );
		return;
	}

	// log
	LogItemInfo aLogItem;
	aLogItem.SendName	= pUser->GetName();
	aLogItem.RecvName	= "UPGRADEITEM";
	aLogItem.WorldId	= pUser->GetWorld()->GetID();
	aLogItem.Gold	= pUser->GetGold();
	aLogItem.Gold2	= pUser->GetGold();

	DWORD dwProbability		= g_AccessoryProperty.GetProbability( pItemMain->GetAbilityOption() );
	// 액세서리 보호의 두루마리
	BOOL bSmelprot	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4 ) )
	{
		bSmelprot	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4 );
		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT4 );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}

	if( xRandom( 10000 ) < dwProbability )	// 성공
	{
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
			
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0)
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem(*pItemMain, UI::AbilityOption::Up);

		aLogItem.Action		= "H";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
	}
	else	// 실패
	{
		pUser->AddDefinedText( TID_UPGRADE_FAIL );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

		if( !bSmelprot )
		{
			if( pItemMain->GetAbilityOption() >= 3 )		// 삭제
			{
				aLogItem.Action	= "L";
				g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
				pUser->UpdateItem(*pItemMain, UI::Num::RemoveAll);
			}
		}
	}
	aLogItem.Action	= "N";
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	pUser->UpdateItem(*pItemMaterial, UI::Num::ConsumeOne);
}

void	CItemUpgrade::RefineCollector( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial )
{
	// 재료가 문스톤인가?
	if( pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE && pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1 )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return;
	}

	CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
	if( pItemMain->GetAbilityOption() >= pProperty->GetMaxCollectorLevel() )
	{
		pUser->AddDefinedText( TID_GAME_MAX_COLLECTOR_LEVEL );
		return;
	}
	
	int nProb	= pProperty->GetEnchantProbability( pItemMain->GetAbilityOption() );
	if( nProb == 0 )
		return;

	// log
	LogItemInfo aLogItem;
	aLogItem.SendName	= pUser->GetName();
	aLogItem.RecvName	= "UPGRADEITEM";
	aLogItem.WorldId	= pUser->GetWorld()->GetID();
	aLogItem.Gold	= pUser->GetGold();
	aLogItem.Gold2	= pUser->GetGold();

	DWORD dwRand	= xRandom( 1000 );	// 0 - 999
	if( (int)( dwRand ) < nProb )
	{
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		pUser->UpdateItem(*pItemMain, UI::AbilityOption::Up);
		aLogItem.Action		= "H";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
	}
	else
	{
		pUser->AddDefinedText( TID_UPGRADE_FAIL );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		// log
	}
	aLogItem.Action	= "N";
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	pUser->UpdateItem(*pItemMaterial, UI::Num::ConsumeOne);
}

BYTE	CItemUpgrade::SmeltSafetyAttribute(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr )
{
	// 속성 제련이 가능한가
	if( !CItemElem::IsEleRefineryAble( pItemMain->GetProp() ) )
		return 0;

	// 속성은 한가지만
	if( pItemMain->m_bItemResist != SAI79::NO_PROP )
	{
		if( pItemMain->m_bItemResist != pItemMaterial->GetProp()->eItemType )
			return 0;
	}

	if (!IsUsableItem(pItemProtScr)) return 0;
	const ItemProp * const scrollProp = pItemProtScr->GetProp();
	if (!scrollProp) return 0;
	if (scrollProp->dwID != II_SYS_SYS_SCR_SMELPROT) return 0;

	// 속성 당 하나의 속성 제련 카드를 사용하도록 수정
	if( pItemMaterial->GetProp()->dwID != WhatEleCard( pItemMaterial->GetProp()->eItemType ) )
		return 0;

	// 최대 값을 넘는 경우 중단
	if( pItemMain->m_nResistAbilityOption >= GetMaxAttributeEnchantSize() )
		return 3;
	
	// 10000단위의 성공 퍼센트
	int nPercent = GetAttributeEnchantProb( pItemMain->m_nResistAbilityOption );

	//	속성 제련의 두루마리를 사용했다면
	if( pItemSmeltScr != NULL )
	{
		//	속성 제련의 두루마리가 맞는지 확인
		if( IsUsableItem( pItemSmeltScr ) && pItemSmeltScr->GetProp()->dwID == II_SYS_SYS_SCR_SMELTING2 )
		{
			//	속성 제련의 두루마리 사용가능 수치인가
			if( pItemMain->m_nResistAbilityOption < 10 )
			{
				nPercent	+= 1000;
				//	속성 제련의 두루마리 로그 남길것
				ItemProp* pItemProp = pItemSmeltScr->GetProp();
				if( pItemProp )
				{
					g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemSmeltScr, pItemProp );
					g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
				}
				pUser->RemoveItem( (BYTE)( pItemSmeltScr->m_dwObjId ), 1 );
			}
		}
		//	속성 제련의 두루마리가 아닐때
		else
			return 0;
	}

	LogItemInfo aLogItem;
	aLogItem.Action = "N";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();

	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	DWORD dwValue = pItemMaterial->GetProp()->eItemType;
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), (short)1 );

	ItemProp* pItemProp = pItemProtScr->GetProp();
	if( pItemProp )
	{
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemProtScr, pItemProp );
		g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	pUser->RemoveItem( (BYTE)( pItemProtScr->m_dwObjId ), 1 );

	// 해당 아이템의 속성, 일반 레벨을 얻어 확율을 꺼낸다.
	if( (int)( xRandom( 10000 ) ) > nPercent )
	{
		// 실패
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );

		if( !pUser->IsMode( TRANSPARENT_MODE ) )
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

		aLogItem.Action = "J";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 2;
	}
	else
	{
		// 성공
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );

		if( !pUser->IsMode( TRANSPARENT_MODE ) )
			g_UserMng.AddCreateSfxObj( (CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

		pUser->UpdateItem(*pItemMain, UI::Element::Increase);
		aLogItem.Action = "O";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 1;
	}
	return 0;
}

void	CItemUpgrade::EnchantGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial )
{	
	int*  pAbilityOption = pItemMain->GetAbilityOptionPtr();
	if( pAbilityOption == NULL )
		return;

	// 일반제련은 방어구, 무기
	if( pItemMain->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return;
	}

	if( pItemMaterial->GetProp()->dwID != II_GEN_MAT_ORICHALCUM01
		&& pItemMaterial->GetProp()->dwID != II_GEN_MAT_ORICHALCUM01_1 )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return;
	}

	if( !CItemElem::IsDiceRefineryAble(pItemMain->GetProp()) )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );			
		return;								
	}

	if( *pAbilityOption >= GetMaxGeneralEnchantSize() )
	{
		pUser->AddDefinedText( TID_UPGRADE_MAXOVER );			
		return;
	}

	// 1000단위의 성공 퍼센트 
	int nPercent = GetGeneralEnchantProb( *pAbilityOption );

	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();

	BOOL bSmelprot	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT ) )
	{
		bSmelprot	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT );

		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}

	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELTING ) )
	{
		if( *pAbilityOption < 7 )
		{
			nPercent	+= 1000;
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELTING );

			ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELTING );
			if( pItemProp )
				g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
		}
	}
#ifdef __SM_ITEM_2ND_EX
	BOOL bSmelprot2	= FALSE;
	if( !bSmelprot && pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT2 ) )
	{
		bSmelprot2	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT2 );
	}
#endif	// __SM_ITEM_2ND_EX

	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING ) )
	{
		if( *pAbilityOption < 7 )
		{
			nPercent	+= 10000;
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING );

			ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SUPERSMELTING );
			if( pItemProp )
				g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
		}				
	}

	// 해당 아이템의 속성, 일반 레벨을 얻어 확율을 꺼낸다.
	if( (int)( xRandom( 10000 ) ) > nPercent )
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_FAIL );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );

		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		// 실패하면 3이상이면 아이템 삭제
		if( *pAbilityOption >= 3 )
		{
			if( !bSmelprot )
			{	// 사용안하면 들어옴.. 대상 아이템 삭제 실패 로그
#ifdef __SM_ITEM_2ND_EX
				if( bSmelprot2 )
				{
					pUser->UpdateItem(*pItemMain, UI::AbilityOption::Down);
					aLogItem.Action = "9";
					g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
				}
				else
#endif	// __SM_ITEM_2ND_EX
				{
					aLogItem.Action = "L";
					g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
					pUser->RemoveItem( (BYTE)( pItemMain->m_dwObjId ), (short)1 );
				}
			}
			else
			{
				aLogItem.Action = "F";
				g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
			}
		}
	}
	else
	{
		// 성공
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );

		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem(*pItemMain, UI::AbilityOption::Up);
		aLogItem.Action = "H";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
	}

	aLogItem.Action = "N";
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	// 제련아템 삭제 - 성공이던, 실패던...
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), (short)1 );
}

int		CItemUpgrade::GetGeneralEnchantProb( int nAbilityOption )
{
	int nProb = 0;
	
	const auto it = m_mapGeneralEnchant.find( nAbilityOption + 1 );
	if( it != m_mapGeneralEnchant.end() )
		nProb = it->second;

	if( ::GetLanguage() != LANG_KOR && nAbilityOption >= 3 )	// 제련 4부터 10% 확률 감소(해외만)
		nProb = static_cast<int>(static_cast<float>(nProb) * 0.9f);
	
	return nProb;
}

void	CItemUpgrade::EnchantAttribute( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial )
{
	int*  pAbilityOption = &(pItemMain->m_nResistAbilityOption);
	if( pAbilityOption == NULL )
		return;

	// 2가지 속성은 제련할수 없음
	if( pItemMain->m_bItemResist != SAI79::NO_PROP )
	{
		if( pItemMain->m_bItemResist != pItemMaterial->GetProp()->eItemType )
		{
			pUser->AddDefinedText( TID_UPGRADE_ERROR_TWOELEMENT );								
			return;
		}
	}

	if( !CItemElem::IsEleRefineryAble(pItemMain->GetProp()) )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );			
		return;								
	}

	// 속성 당 하나의 속성 제련 카드를 사용하도록 수정
	DWORD dwReqCard	= WhatEleCard( pItemMaterial->GetProp()->eItemType );

	if( pItemMaterial->GetProp()->dwID != dwReqCard )
	{
		pUser->AddDefinedText( TID_UPGRADE_ERROR_WRONGUPLEVEL );			
		return;					
	}

	// 최대 값을 넘는 경우 중단
	if( *pAbilityOption >= GetMaxAttributeEnchantSize() )
	{
		pUser->AddDefinedText( TID_UPGRADE_MAXOVER );			
		return;
	}
	// 10000단위의 성공 퍼센트 
	int nPercent = GetAttributeEnchantProb( *pAbilityOption );

	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();


	BOOL bSmelprot	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT ) )
	{
		bSmelprot	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT );

		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELTING2 ) )	// 속성 제련의 두루마리
	{
		if( *pAbilityOption < 10 )
		{
			nPercent	+= 1000;
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELTING2 );
			ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELTING2 );
			if( pItemProp )
				g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
		}
	}
#ifdef __SM_ITEM_2ND_EX
	BOOL bSmelprot2	= FALSE;
	if( !bSmelprot && pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT2 ) )
	{
		bSmelprot2	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT2 );
	}
#endif	// __SM_ITEM_2ND_EX

	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING ) )
	{
		if( *pAbilityOption < 7 )
		{
			nPercent	+= 10000;
			pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING );

			ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SUPERSMELTING );
			if( pItemProp )
				g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
		}				
	}

	// 해당 아이템의 속성, 일반 레벨을 얻어 확율을 꺼낸다.
	if( (int)( xRandom( 10000 ) ) > nPercent )
	{
		// 실패 메세지 출력
		pUser->AddDefinedText( TID_UPGRADE_FAIL );
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );

		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		// 실패하면 nLevDown이상이면 아이템 삭제
		if( *pAbilityOption >= 3 )
		{
			if( !bSmelprot )
			{	// 사용안하면 들어옴.. 대상 아이템 삭제 실패 로그
#ifdef __SM_ITEM_2ND_EX
				if( bSmelprot2  )
				{
					pUser->UpdateItem(*pItemMain, UI::Element::Decrease);
					aLogItem.Action = "8";
					g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
				}
				else
#endif	// __SM_ITEM_2ND_EX
				{
					aLogItem.Action = "L";
					g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
					pUser->RemoveItem( (BYTE)( pItemMain->m_dwObjId ), (short)1 );
				}
			}
		}
		else
		{	// 사용을 하면 실패 로그
			aLogItem.Action = "J";
			g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
		}
	}
	else
	{
		// 성공
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );

		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0)
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem(*pItemMain, UI::Element::Increase);
		aLogItem.Action = "O";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );
	}

	aLogItem.Action = "N";
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	// 제련아템 삭제 - 성공이던, 실패던...
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), (short)1 );
}

void CItemUpgrade::ChangeAttribute( CUser* pUser, OBJID dwTargetItem, OBJID dwUseItem , SAI79::ePropType nAttribute )
{
	CItemElem* pTargetItemElem	= pUser->m_Inventory.GetAtId( dwTargetItem );
	CItemElem* pUseItemElem = pUser->m_Inventory.GetAtId( dwUseItem );		

	if( pUser->m_vtInfo.GetOther() )	// 거래중인 대상이 있으면?
		return;
	if( pUser->m_vtInfo.VendorIsVendor() )		// 내가 팔고 있으면?
		return;

	if( !IsUsableItem( pTargetItemElem ) || !IsUsableItem( pUseItemElem ) )
		return;

	// 대상이 장착중인가?
	if( pUser->m_Inventory.IsEquip( dwTargetItem ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return;
	}
	
	if( !CItemElem::IsEleRefineryAble( pTargetItemElem->GetProp() ) )	// 속성제련 가능한 아이템이 아니요..
		return;

	if( pUseItemElem->m_dwItemId != II_SYS_SYS_SCR_SOKCHANG )	// 속성변경 아이템이 아니네...
		return;

	if( nAttribute >= SAI79::END_PROP || nAttribute <= SAI79::NO_PROP )
		return;

	if( pTargetItemElem->m_bItemResist == nAttribute )	// 같은 속성인 경우 속성변경 불가!!
	{
		pUser->AddDefinedText( TID_GAME_NOCHANGE_SAME_ATTRIBUTE );
		return;
	}
	
	if( (pTargetItemElem->m_bItemResist != SAI79::NO_PROP) && (pTargetItemElem->m_nResistAbilityOption > 0) )
	{
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( ( pUser->IsMode( TRANSPARENT_MODE ) ) == 0 )
			g_UserMng.AddCreateSfxObj((CMover *)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		
		pUser->UpdateItem(*pTargetItemElem, UI::Element::Change(nAttribute));	// 속성 변경

		// 속성제련 변경 성공 로그
		LogItemInfo aLogItem;
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "CHANGE_ATTRIBUTE_TARGET";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Action = "O";
		g_DPSrvr.OnLogItem( aLogItem, pTargetItemElem );
		
		aLogItem.RecvName = "CHANGE_ATTRIBUTE_MATERIAL";
		g_DPSrvr.OnLogItem( aLogItem, pUseItemElem );
		pUser->RemoveItem( (BYTE)( dwUseItem ), 1 );	// 유료 아이템 삭제
	}
	else
		pUser->AddDefinedText( TID_GAME_NOTELEMENT );
}

int CItemUpgrade::GetAttributeEnchantProb( int nAbilityOption )
{
	const auto it = m_mapAttributeEnchant.find( nAbilityOption + 1 );
	if( it != m_mapAttributeEnchant.end() )
		return it->second.nProb;

	return 0;
}

int CItemUpgrade::GetAttributeDamageFactor( int nAbilityOption )
{
	if( nAbilityOption > GetMaxAttributeEnchantSize() )
		nAbilityOption = GetMaxAttributeEnchantSize();

	const auto it = m_mapAttributeEnchant.find( nAbilityOption );
	if( it != m_mapAttributeEnchant.end() )
		return it->second.nAddDamageRate;

	return 0;
}

int CItemUpgrade::GetAttributeDefenseFactor( int nAbilityOption )
{
	if( nAbilityOption > GetMaxAttributeEnchantSize() )
		nAbilityOption = GetMaxAttributeEnchantSize();

	const auto it = m_mapAttributeEnchant.find( nAbilityOption );
	if( it != m_mapAttributeEnchant.end() )
		return it->second.nDefenseRate;

	return 0;
}

int CItemUpgrade::GetAttributeAddAtkDmgFactor( int nAbilityOption )
{
	if( nAbilityOption > GetMaxAttributeEnchantSize() )
		nAbilityOption = GetMaxAttributeEnchantSize();

	const auto it = m_mapAttributeEnchant.find( nAbilityOption );
	if( it != m_mapAttributeEnchant.end() )
		return it->second.nAddAtkDmgRate;

	return 0;
}

DWORD CItemUpgrade::WhatEleCard( DWORD dwItemType )
{	// 속성 제련 용 카드의 종류가 
	// 속성 당 하나로 통합됨
	switch( dwItemType )
	{
	case SAI79::FIRE:
		return II_GEN_MAT_ELE_FLAME;
	case SAI79::WATER:
		return II_GEN_MAT_ELE_RIVER;
	case SAI79::ELECTRICITY:
		return II_GEN_MAT_ELE_GENERATOR;
	case SAI79::EARTH:
		return II_GEN_MAT_ELE_DESERT;
	case SAI79::WIND:
		return II_GEN_MAT_ELE_CYCLON;
	default:
		return 0;
	}
}

#ifdef __SYS_ITEMTRANSY
void CItemUpgrade::OnItemTransy( CUser* pUser, OBJID objidTarget, OBJID objidTransy, DWORD dwChangeId, BOOL bCash ) {
	if (IsInTrade(*pUser)) return;




	CItemElem* pItemElemTarget = pUser->m_Inventory.GetAtId( objidTarget );	
	if( !IsUsableItem( pItemElemTarget ) )
		return;

	// 장착되어 있는 아이템이면 리턴( 오라이~~~ ㅋㅋ )
	if( pUser->m_Inventory.IsEquip( objidTarget ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return;
	}

	if( bCash )
	{
		CItemElem* pItemElemTransy = pUser->m_Inventory.GetAtId( objidTransy );
		if( !IsUsableItem( pItemElemTransy ) )
			return;

		// 재료가 트랜지(ITM)인지 검사
		if( pItemElemTransy->GetProp()->dwID != II_CHR_SYS_SCR_ITEMTRANSY_A && pItemElemTransy->GetProp()->dwID != II_CHR_SYS_SCR_ITEMTRANSY_B )
			return;
		
		// 레벨 검사
		if( pItemElemTransy->GetProp()->dwID == II_CHR_SYS_SCR_ITEMTRANSY_A )
		{
			if( pItemElemTarget->GetProp()->dwLimitLevel1 > 60 )
				return;
		}
		else
		{
			if( pItemElemTarget->GetProp()->dwLimitLevel1 < 61 )
				return;
		}

		if( RunItemTransy( pUser, pItemElemTarget, dwChangeId ) )
		{
			g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemTransy, pItemElemTransy->GetProp(), "RemoveItem" );	
			pUser->RemoveItem( (BYTE)( objidTransy ), (short)1 );		// 기존 아이템 재료 삭제
		}
	}
	else
	{
		//	mulcom	BEGIN100312	페냐로 아이템 트렌지 금지.
		#ifdef	__ITEMTRANSY_PENYA
			int nPayPenya = 0;
			if( pItemElemTarget->GetProp()->dwLimitLevel1 < 61 )
				nPayPenya = m_nItemTransyLowLevel;
			else
				nPayPenya = m_nItemTransyHighLevel;

			if( pUser->GetGold() < nPayPenya || objidTransy != NULL_ID )
			{
				pUser->AddDefinedText( TID_GAME_LACKMONEY );
				return;
			}

			if( RunItemTransy( pUser, pItemElemTarget, dwChangeId ) )
			{
				pUser->AddGold( -nPayPenya );
				g_DPSrvr.PutPenyaLog( pUser, "O", "TRANSYITEM_PAY", nPayPenya );
			}
		#endif
		//	mulcom	END100312	페냐로 아이템 트렌지 금지.
	}
}

BOOL CItemUpgrade::RunItemTransy( CUser* pUser, CItemElem* pItemElemTarget, DWORD dwChangeId )
{
	const ItemProp * pItemProp = pItemElemTarget->GetProp();
	const ItemProp * pItemPropChange = prj.GetItemProp( dwChangeId );

	// 변경될 아이템의 조건이 맞는지 검사.
	if (!pItemProp || !pItemPropChange) return FALSE;
	if (!ItemMorph::IsMorphableTo(*pItemProp, *pItemPropChange)) return FALSE;

	// 아이템 트랜지 성공
	pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );			
	g_UserMng.AddCreateSfxObj(pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);			

	// 기존 아이템의 Elem 정보를 저장 하고 잇음...
	CItemElem ItemElemSend;
	ItemElemSend = *pItemElemTarget;
	ItemElemSend.m_dwItemId = pItemPropChange->dwID;
	ItemElemSend.m_nHitPoint	= pItemPropChange->dwEndurance;		// 내구력 100%

	g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemTarget, pItemElemTarget->GetProp(), "RemoveItem" );	
	g_dpDBClient.SendLogSMItemUse( "1", pUser, &ItemElemSend, ItemElemSend.GetProp(), "CreateItem" );	
	pUser->AddDefinedText( TID_GAME_ITEM_TRANSY_SUCCESS, "\"%s\" \"%s\"", pItemElemTarget->GetProp()->szName, ItemElemSend.GetProp()->szName );

	// 기존 아이템 재료 삭제
	pUser->RemoveItem( (BYTE)( pItemElemTarget->m_dwObjId ), (short)1 );
	
	// 새로운 아이템 지급
	pUser->CreateItem( &ItemElemSend );
	
	return TRUE;
}
#endif // __SYS_ITEMTRANSY

void CItemUpgrade::PetVisSize( CUser* pUser, OBJID objIdMaterial )
{
	if( !IsValidObj( pUser ) )
		return;

	CItemElem* pItemElemPet = pUser->GetVisPetItem();
	CItemElem* pItemElemMaterial = pUser->m_Inventory.GetAtId( objIdMaterial );

	if( !IsUsableItem( pItemElemPet ) || !pItemElemPet->IsVisPet() )
	{
		pUser->AddDefinedText( TID_GAME_BUFFPET_NOSUMMON02 );	
		return;
	}

	if( !IsUsableItem( pItemElemMaterial ) )
		return;

	if( pItemElemMaterial->m_dwItemId != II_SYS_SYS_VIS_KEY01 )
		return;


	if( !pItemElemPet->IsPierceAble( NULL_ID, TRUE ) ) // 비스 슬롯 확장 가능 검사
	{
		pUser->AddDefinedText( TID_GAME_BUFFPET_EXPANSION );
		return;
	}
		
	g_DPSrvr.PutItemLog( pUser, "!", "VIS_SLOT_MATERIAL", pItemElemMaterial );
	if( pItemElemMaterial->m_bCharged )
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemMaterial, pItemElemMaterial->GetProp() );	
	pUser->UpdateItem(*pItemElemPet, UI::PetVis::Size::Increase);
	g_DPSrvr.PutItemLog( pUser, "#", "VIS_SLOT_SIZE", pItemElemPet );
	pUser->RemoveItem( (BYTE)( objIdMaterial ), 1 );
}

void CItemUpgrade::SetPetVisItem( CUser* pUser, OBJID objIdVis )
{
	if( !IsValidObj( pUser ) )
		return;

	CItemElem* pItemElemPet = pUser->GetVisPetItem();
	CItemElem* pItemElemVis = pUser->m_Inventory.GetAtId( objIdVis );

	if( !IsUsableItem( pItemElemPet ) )
	{
		pUser->AddDefinedText( TID_GAME_BUFFPET_NOSUMMON01 );
		return;
	}
		
	if( !IsUsableItem( pItemElemVis ) )
		return;

	ItemProp* pVisProp = prj.GetItemProp( pItemElemVis->m_dwItemId );
	if( !pVisProp )
		return;

	if( !pItemElemPet->IsPierceAble( pVisProp->dwItemKind3 ) )	// 비스 장착 가능 검사.
		return;

	int nFirstEmptySlot = NULL_ID;
	for( int i=0; i<pItemElemPet->GetPiercingSize(); i++ )
	{
		DWORD dwVisId = pItemElemPet->GetPiercingItem( i );
		if( dwVisId == pVisProp->dwID )	// 이미 같은 종류의 비스가 장착되어 있다.
		{
			pUser->AddDefinedText( TID_GAME_BUFFPET_EQUIPVIS );
			return;
		}

		if( nFirstEmptySlot == NULL_ID && dwVisId == 0 )
			nFirstEmptySlot = i;
	}

	if( nFirstEmptySlot == NULL_ID )	// 비어있는 비스 슬롯이 없다.
	{
		pUser->AddDefinedText( TID_GAME_BUFFPET_LACKSLOT );
		return;
	}

	if (pUser->IsSatisfyNeedVis(*pItemElemPet, *pVisProp) != NeedVis::Success) {
		pUser->AddDefinedText(TID_GAME_BUFFPET_REQVIS);
		return;
	}

	
	pUser->ResetPetVisDST( pItemElemPet );
	pUser->UpdateItem(*pItemElemPet,
		UI::PetVis::Item{
			.position = nFirstEmptySlot,
			.itemId = pItemElemVis->m_dwItemId,
			.time = pVisProp->dwAbilityMin
		}
	);
	PutPetVisItemLog( pUser, "!", "VIS_MATERIAL", pItemElemPet, nFirstEmptySlot );
	if( pItemElemVis->m_bCharged )		// 상용화 아이템 로그
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElemVis, pVisProp );		
	g_DPSrvr.PutItemLog( pUser, "$", "VIS_PIERCING", pItemElemPet );
	pUser->RemoveItem( (BYTE)( objIdVis ), 1 );
	pUser->SetPetVisDST( pItemElemPet );
	ChangeVisPetSfx( pUser, pItemElemPet );
}

void CItemUpgrade::RemovePetVisItem( CUser* pUser, int nPosition, BOOL bExpired )
{
	if( !IsValidObj( pUser ) )
		return;

	CItemElem* const pItemElemPet = pUser->GetVisPetItem();
	if( !IsUsableItem( pItemElemPet ) )
		return;

	if (nPosition < 0 || nPosition >= pItemElemPet->GetPiercingSize()) return;

	const DWORD dwItemId = pItemElemPet->GetPiercingItem( nPosition );
	if( dwItemId  == 0 )	// 이미 비어있는 슬롯
		return;

	pUser->ResetPetVisDST( pItemElemPet );
	if( bExpired )
		PutPetVisItemLog( pUser, "$", "VIS_REMOVE_EXPIRED", pItemElemPet, nPosition );
	else
		PutPetVisItemLog( pUser, "$", "VIS_REMOVE_BYUSER", pItemElemPet, nPosition );
	pUser->UpdateItem(*pItemElemPet, UI::PetVis::Item::Empty(nPosition));
	ItemProp* pItemProp = prj.GetItemProp( dwItemId );
	if( pItemProp )
		pUser->AddDefinedText( TID_GAME_BUFFPET_REMOVEVIS, "\"%s\"", pItemProp->szName );
	pUser->SetPetVisDST( pItemElemPet );
	ChangeVisPetSfx( pUser, pItemElemPet );
}

void CItemUpgrade::PutPetVisItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nPosition )
{	// 아이템이 제거된 이후에 호출되지 않도록 주의해야 함
	LogItemInfo	log;
	log.Action	=  szAction;
	log.SendName	= pUser->GetName();
	log.RecvName	= szContext;
	log.WorldId		= pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. 
	log.Gold	= pUser->GetGold();
	log.Gold2	= pItem->GetVisKeepTime( nPosition ) - time_null();
	g_DPSrvr.OnLogItem( log, pItem, pItem->m_nItemNum );
}

void CItemUpgrade::SwapVis(CUser * pUser, int nPos1, int nPos2) {
	if (!IsValidObj(pUser)) return;

	CItemElem * pItemElemPet = pUser->GetVisPetItem();
	if (!IsUsableItem(pItemElemPet)) return;

	pUser->UpdateItem(*pItemElemPet, UI::PetVis::ItemSwap{ nPos1, nPos2 });
}

// 최고레벨 비스에 변화가 생긴 경우 SFX를 변화시킨다.
void CItemUpgrade::ChangeVisPetSfx( CUser* pUser, CItemElem* pItemElemPet )
{
	CMover* pVisPet = prj.GetMover( pUser->GetEatPetId() );
	if( IsValidObj( pUser ) && IsValidObj( pVisPet ) )
	{
		DWORD dwSfxId = pItemElemPet->GetVisPetSfxId();
		if( pVisPet->m_dwMoverSfxId != dwSfxId )
		{
			pVisPet->m_dwMoverSfxId = dwSfxId;
			g_UserMng.AddChangeMoverSfxId( pVisPet );
		}
	}
}

void CItemUpgrade::TransFormVisPet( CUser* pUser, OBJID objIdMaterial )
{
	CMover* pEatPet = prj.GetMover( pUser->GetEatPetId() );
	if( IsValidObj( pEatPet ) )
	{
		CAIPet* pAI = static_cast<CAIPet*>( pEatPet->m_pAIInterface );
		if( pAI )
		{
			CItemElem* pItemEatPet = pUser->m_Inventory.GetAtId( pAI->GetPetItemId() );
			CItemElem* pItemMaterial = pUser->m_Inventory.GetAtId( objIdMaterial );
			if( IsUsableItem( pItemEatPet ) && IsUsableItem( pItemMaterial ) )
			{
				if( pItemEatPet->IsVisPet() )
				{
					pUser->AddDefinedText( TID_GAME_PET_TRAN_FAILURE );
					return;
				}
								
				if( pItemMaterial->m_bCharged )		// 상용화 아이템 로그
					g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemMaterial, pItemMaterial->GetProp() );
				pUser->RemoveItem( (BYTE)( objIdMaterial ), 1 );
				pUser->UpdateItem(*pItemEatPet, UI::PetVis::TransformToVisPet());
				g_DPSrvr.PutItemLog( pUser, "!", "TRANSFORM_VISPET", pItemEatPet );
				pUser->AddDefinedText( TID_GAME_PET_TRAN_SUCCESS, "\"%s\"", pItemEatPet->GetProp()->szName );
			}
		}
	}
	else
	{
		pUser->AddDefinedText( TID_GAME_PET_TRAN_FAILURE );
	}
}
