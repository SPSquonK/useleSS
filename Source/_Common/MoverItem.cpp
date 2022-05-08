#include "stdafx.h"
#include "mover.h"
#include "defineText.h"
#ifdef __WORLDSERVER
#include "user.h"
#endif	// WORLDSERVER


// pc, npc의 분리 
// m_nCost삭제 

// 거래 상대방을 얻는다.
CMover* CVTInfo::GetOther()
{
//	return m_pOther;
	if( m_objId == NULL_ID )
		return NULL;

	return prj.GetMover( m_objId );
}

// 거래 상대방을 정한다.
void CVTInfo::SetOther( CMover* pMover )
{
//	m_pOther = pMover;
	if( pMover )
	{
		m_objId = pMover->GetId();
	}
	else
	{
		m_objId = NULL_ID;
	}
}

LPCTSTR	CVTInfo::GetTitle()
{
	return m_strTitle.c_str();
}

void CVTInfo::SetTitle( LPCTSTR szTitle )
{
	m_strTitle = szTitle;
}

BOOL CVTInfo::IsVendorOpen()
{
	return (m_strTitle.empty() != true);
}

void CVTInfo::Init( CMover* pOwner )
{
	m_pOwner = pOwner;
	m_items_VT.fill(nullptr);
	TradeSetGold( 0 );							// raiders.2006.11.28 
	TradeClear();
	m_strTitle = "";
}




void CVTInfo::TradeClear()
{
	SetOther( NULL );
	for (CItemElem * & item : m_items_VT) {
		if (item) {
			item->SetExtra(0);
			item = nullptr;
		}
	}

#ifdef __WORLDSERVER	
	// raiders.2006.11.28	인벤돈 = 인벤돈 + 내 거래창 돈
	int nGold = TradeGetGold();
	if( nGold > 0 && m_pOwner )
		m_pOwner->AddGold( nGold );
	//
#endif

	TradeSetGold( 0 );
	TradeSetState( TRADE_STEP_ITEM );
}

void CVTInfo::TradeSetGold( DWORD dwGold )
{
	m_dwTradeGold = dwGold;
}

int CVTInfo::TradeGetGold()
{
	return m_dwTradeGold;
}

void CVTInfo::TradeSetItem( BYTE nId, BYTE i, short nItemNum )
{
	CItemElem * pItemBase = m_pOwner->GetItemId( nId );
	if( pItemBase )
	{
		m_items_VT[i] = pItemBase;
		pItemBase->SetExtra( nItemNum );
	}
}

BOOL CVTInfo::TradeClearItem( BYTE i )
{
	CItemElem * & pItemBase = m_items_VT[i];
	if( IsUsingItem( pItemBase ) )
	{
		pItemBase->SetExtra( 0 );		// clear - using flag 
		pItemBase = nullptr;

		return TRUE;
	}
	else
		return FALSE;
}

//raiders.2006.11.28  계산과정 변경 ( 인벤돈 = 인벤돈 + 상대방 거래창 돈 )
BOOL CVTInfo::TradeConsent()
{
	CMover* pTrader	= GetOther();
	if( pTrader == NULL )
		return FALSE;

	int cbI	= 0, cbYou	= 0;
	CItemContainer a;
	a.SetItemContainer( CItemContainer::ContainerTypes::TRADE );
	
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		CItemElem * pItemElem = m_items_VT[i];
		if (!pItemElem) continue;

		m_items_VT[i] = nullptr;

		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemElem->GetExtra();
			pItemElem->m_nItemNum = pItemElem->GetExtra();
			a.Add( pItemElem );
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
				m_pOwner->m_Inventory.RemoveAtId(pItemElem->m_dwObjId );	// 제거
		}
		else
		{
			a.Add( pItemElem );
			m_pOwner->m_Inventory.RemoveAtId(pItemElem->m_dwObjId );
		}
	}
		
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		CItemElem * pItemElem = pTrader->m_vtInfo.GetItem( i );
		if (!pItemElem) continue;

		pTrader->m_vtInfo.SetItem( i, NULL );

		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemElem->GetExtra();
			pItemElem->m_nItemNum = pItemElem->GetExtra();
			m_pOwner->m_Inventory.Add( pItemElem );
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
				pTrader->m_Inventory.RemoveAtId(pItemElem->m_dwObjId );	// 제거
		}
		else
		{
			m_pOwner->m_Inventory.Add( pItemElem );
			pTrader->m_Inventory.RemoveAtId(pItemElem->m_dwObjId );
		}
	}

	cbI		= a.GetCount();
	for( int i = 0; i < cbI; i++ )
	{
		CItemElem * pItemBase	= a.GetAtId(i);
		pTrader->m_Inventory.Add(pItemBase);
	}
	
	// step1. 줄돈과 뺄돈을 구해둔다.
	int nThisGold = pTrader->m_vtInfo.TradeGetGold();
	int nTraderGold = TradeGetGold();

	// step2. m_dwTradeGold를 clear
	TradeSetGold( 0 );						// 원복 안되게 
	TradeClear();
	pTrader->m_vtInfo.TradeSetGold( 0 );	// 원복 안되게 
	pTrader->m_vtInfo.TradeClear();

	// step3. 돈을 더한다.
	m_pOwner->AddGold( nThisGold, FALSE );						
	pTrader->AddGold( nTraderGold, FALSE );

	return TRUE;
}

DWORD CVTInfo::TradeSetItem2( BYTE nId, BYTE i, short & nItemNum )
{
	CItemElem * pItemBase = m_pOwner->GetItemId( nId );
	if( IsUsableItem( pItemBase ) == FALSE || m_items_VT[i] != nullptr)  
		return TID_GAME_CANNOTTRADE_ITEM;

	if( m_pOwner->m_Inventory.IsEquip( pItemBase->m_dwObjId ) ) 
		return TID_GAME_CANNOTTRADE_ITEM;
	
	if( pItemBase->IsQuest() )
		return TID_GAME_CANNOTTRADE_ITEM;

	if( pItemBase->IsBinds() )
		return TID_GAME_CANNOTTRADE_ITEM;

	if( m_pOwner->IsUsing(pItemBase ) )
		return TID_GAME_CANNOT_DO_USINGITEM;

	if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && pItemBase->m_idGuild != 0 )
		return TID_GAME_CANNOTTRADE_ITEM;
	
	if( pItemBase->GetProp()->dwParts == PARTS_RIDE && pItemBase->GetProp()->dwItemJob == JOB_VAGRANT )
		return TID_GAME_CANNOTTRADE_ITEM;
	
	if( nItemNum < 1)
		nItemNum = 1;
	if( nItemNum > pItemBase->m_nItemNum )
		nItemNum = pItemBase->m_nItemNum;
	
	TradeSetItem( nId, i, nItemNum );
	return 0;
}

void TradeLog( CAr & ar, CItemElem * pItemBase, short nItemCount )
{
	ar << nItemCount;
	ar << pItemBase->GetAbilityOption();
	ar << int(pItemBase->m_bItemResist);
	ar << int(pItemBase->m_nResistAbilityOption);
	ar << pItemBase->m_nHitPoint;
	ar << pItemBase->m_nRepair;
	ar << pItemBase->m_bCharged;
	ar << pItemBase->m_dwKeepTime;
	ar << pItemBase->GetPiercingSize();
	for( int i=0; i< pItemBase->GetPiercingSize(); i++ )
		ar << pItemBase->GetPiercingItem( i );
	ar << pItemBase->GetUltimatePiercingSize();
	for( int i=0; i< pItemBase->GetUltimatePiercingSize(); i++ )
		ar << pItemBase->GetUltimatePiercingItem( i );
	ar << pItemBase->GetRandomOptItemId();
		if(pItemBase->m_pPet )
		{
			CPet* pPet = pItemBase->m_pPet;

			ar << pPet->GetKind();
			ar << pPet->GetLevel();
			ar << pPet->GetExp();
			ar << pPet->GetEnergy();
			ar << pPet->GetLife();
			ar << pPet->GetAvailLevel( PL_D );
			ar << pPet->GetAvailLevel( PL_C );
			ar << pPet->GetAvailLevel( PL_B );
			ar << pPet->GetAvailLevel( PL_A );
			ar << pPet->GetAvailLevel( PL_S );
		}
		else
		{
			// mirchang_100514 TransformVisPet_Log
			if(pItemBase->IsTransformVisPet() == TRUE )
			{
				ar << (BYTE)100;
			}
			else
			{
				ar << (BYTE)0;
			}
			// mirchang_100514 TransformVisPet_Log
			ar << (BYTE)0;
			ar << (DWORD)0;
			ar << (WORD)0;
			ar << (WORD)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
		}
}

// nMinus - 나갈 돈 
// nPlus  - 들어올 돈 
BOOL CheckTradeGold( CMover* pMover, int nMinus, int nPlus )
{
	if( nMinus >= 0 )
	{
		if( pMover->GetGold() >= nMinus )
		{
			int nGold = pMover->GetGold() - nMinus;
			int nResult = nGold + nPlus;
			if( nPlus >= 0 )
			{
				if( nResult >= nGold )	// overflow가 아니면?
					return TRUE;
			}
			else
			{
				if( nResult >= 0 )		// underflow가 아니면?
					return TRUE;
			}

		}
	}

	return FALSE;
}
//raiders.2006.11.28  계산과정 변경 
//로그와 체크를 때어내면 클라와 같다. 통합해서 refactoring하자.
TRADE_CONFIRM_TYPE CVTInfo::TradeLastConfirm( CAr & ar )
{
	TRADE_CONFIRM_TYPE		result = TRADE_CONFIRM_ERROR;
	CMover*					pTrader	= GetOther();

	// gold_step1. GetGold(), AddGold() 함수가 원하는데로 동작되도록 보관 후에 clear해둔다.
	int nTraderGold        = pTrader->m_vtInfo.TradeGetGold();
	int nUserGold          = TradeGetGold();
	
	// gold_step2. 줄돈과 뺄돈을 구해둔다.
	int nThisAdd   = nTraderGold;
	int nTraderAdd = nUserGold;

	// gold_step3. overflow를 검사 
	if( CheckTradeGold( m_pOwner, 0, nTraderGold ) == FALSE ||
		CheckTradeGold( pTrader, 0, nUserGold ) == FALSE )
	{
		TradeClear();
		pTrader->m_vtInfo.TradeClear();
		return result;
	}

	// 교환 할 만큼 양쪽의 인벤토리 슬롯이 여유가 있는 지를 검사한다. 
	int nPlayers = 0;
	int nTraders = 0;

	for( int i = 0; i < MAX_TRADE; i++ )
	{
		if( GetItem( i ) )
			nPlayers++;

		if( pTrader->m_vtInfo.GetItem( i ) )
			nTraders++;
	}

	if( ( pTrader->m_Inventory.GetSize() - pTrader->m_Inventory.GetCount() ) < nPlayers )
		result = TRADE_CONFIRM_ERROR;
	else if( ( m_pOwner->m_Inventory.GetSize() - m_pOwner->m_Inventory.GetCount() ) < nTraders )
		result = TRADE_CONFIRM_ERROR;
	else
		result = TRADE_CONFIRM_OK;

	if( result == TRADE_CONFIRM_OK )
	{
		CItemContainer u;
		u.SetItemContainer(CItemContainer::ContainerTypes::TRADE);

		// gold_step4. 돈을 더한다.
		m_pOwner->AddGold( nThisAdd, FALSE );						
		pTrader->AddGold( nTraderAdd, FALSE );
		// TradeClear에서 원복이 안되기 위해서 
		pTrader->m_vtInfo.TradeSetGold( 0 );
		TradeSetGold( 0 );

		ar.WriteString( "T" );
		ar.WriteString( pTrader->GetName() );
		ar.WriteString( m_pOwner->GetName() );
		ar << m_pOwner->GetWorld()->GetID();
		ar << nTraderGold << nUserGold;
		ar << pTrader->GetGold() << m_pOwner->GetGold();
		ar << m_pOwner->m_idPlayer << m_pOwner->GetLevel() << m_pOwner->GetJob();
		ar << pTrader->m_idPlayer << pTrader->GetLevel() << pTrader->GetJob();
#ifdef __WORLDSERVER
		ar.WriteString( ( (CUser*)m_pOwner )->m_playAccount.lpAddr );
		ar.WriteString( ( (CUser*)pTrader )->m_playAccount.lpAddr );
#endif	// __WORLDSERVER
		u_long uSize1	= 0;
		u_long uOffset1	= ar.GetOffset();
		ar << (DWORD)0;
		u_long uSize2	= 0;
		u_long uOffset2	= ar.GetOffset();
		ar << (DWORD)0;

		// item_step1. m_pOwner->임시 
		for( int i = 0; i < MAX_TRADE; i++ )
		{
			CItemElem * pItemElem = m_items_VT[i];
			if (!pItemElem) continue;

			m_items_VT[i] = NULL;

			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short nTradeNum = pItemElem->m_nItemNum - pItemElem->GetExtra();
				pItemElem->m_nItemNum = pItemElem->GetExtra();
				u.Add( pItemElem );
				pItemElem->m_nItemNum = nTradeNum;
				pItemElem->SetExtra( 0 );
				if( nTradeNum == 0 )
					m_pOwner->m_Inventory.RemoveAtId(pItemElem->m_dwObjId );	// 제거
			}
			else
			{
				u.Add( pItemElem );	// 임시 버퍼에 추가
				m_pOwner->m_Inventory.RemoveAtId(pItemElem->m_dwObjId );	// 제거
			}
		}
		
		// item_step2. pTrader -> m_pOwner
		for( int i = 0; i < MAX_TRADE; i++ )
		{
			CItemElem * pItemBase = pTrader->m_vtInfo.GetItem( i );
			if( pItemBase == NULL )
				continue;

			pTrader->m_vtInfo.SetItem( i, NULL );
			uSize1++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();
			//ar.WriteString( pItemBase->GetProp()->szName );
			char szItemId[32] = {0, };
			_stprintf( szItemId, "%d", pItemBase->GetProp()->dwID );
			ar.WriteString( szItemId );

			if(pItemBase->GetProp()->dwPackMax > 1 )
			{
				int nTradeNum = pItemBase->m_nItemNum - pItemBase->GetExtra();
				pItemBase->m_nItemNum = pItemBase->GetExtra();
				m_pOwner->m_Inventory.Add(pItemBase);
				pItemBase->m_nItemNum = (short)nTradeNum;

				TradeLog( ar, pItemBase, pItemBase->GetExtra() );
				
				pItemBase->SetExtra( 0 );
				if( nTradeNum == 0 )
					pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
			else
			{
				TradeLog( ar, pItemBase, 1 );
				
				m_pOwner->m_Inventory.Add(pItemBase);	// pUser에 pTrader가 준 아이템을 추가
				pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
		}

		// item_step3. 임시 -> pTrader
		nPlayers = u.GetCount();	// 합침을 고려해서 구해둔다.
		for( int i = 0; i < nPlayers; i++ )
		{
			CItemElem * pItemBase = u.GetAtId( i );
			pTrader->m_Inventory.Add(pItemBase);
			uSize2++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();
			//ar.WriteString( pItemBase->GetProp()->szName );
			char szItemId[32] = {0, };
			_stprintf( szItemId, "%d", pItemBase->GetProp()->dwID );
			ar.WriteString( szItemId );
			TradeLog( ar, pItemBase, pItemBase->m_nItemNum );
		}

//		GETBLOCK( ar, lpBlock, nBlockSize );
		int nBufSize;	
		LPBYTE lpBlock	= ar.GetBuffer( &nBufSize );

		*(UNALIGNED u_long*)( lpBlock + uOffset1 )	= uSize1;
		*(UNALIGNED u_long*)( lpBlock + uOffset2 )	= uSize2;	
	}


	TradeClear();
	pTrader->m_vtInfo.TradeClear();
	return result;
}

TRADE_STATE CVTInfo::TradeGetState()
{
	return m_state;
}

void CVTInfo::TradeSetState( TRADE_STATE state )
{
	m_state = state;
}

///////////////////////////////////////////////////////////////////////////////
// 개인상점 
///////////////////////////////////////////////////////////////////////////////

BOOL CVTInfo::VendorClearItem( BYTE i ) {
	CItemElem * pItemBase = m_items_VT[i];
	if (!pItemBase) return FALSE;

	pItemBase->SetExtra( 0 );
	pItemBase->m_nCost = 0;
	m_items_VT[i] = nullptr;
	return TRUE;
}

void CVTInfo::VendorSetItem( BYTE nId, BYTE i, short nNum, int nCost ) {
	CItemElem * pItemBase = m_pOwner->GetItemId( nId );
	if (pItemBase) {
		m_items_VT[i] = pItemBase;
		pItemBase->SetExtra(nNum);
		pItemBase->m_nCost = nCost;
	}
}

// nNum - 남은 갯수 
void CVTInfo::VendorItemNum( BYTE i, short nNum )
{
	CItemElem * pItemBase = m_items_VT[i];
	if (!pItemBase) return;

	pItemBase->SetExtra( nNum );
	if (nNum == 0) {
#ifdef __CLIENT
		if (!m_pOwner->IsActiveMover()) {
			SAFE_DELETE(m_items_VT[i]);
		}
#endif
		m_items_VT[i] = nullptr;
	}
}

void CVTInfo::VendorClose( BOOL bClearTitle )
{
	for (CItemElem * & item : m_items_VT) {
		if (item) {
			item->SetExtra(0);
			item->m_nCost = 0;
#ifdef __CLIENT
			if (!m_pOwner->IsActiveMover()) {
				SAFE_DELETE(item);
			}
#endif	// __CLIENT
			item = nullptr;
		}
	}

	if( bClearTitle )
		m_strTitle = "";

	SetOther( NULL );	
}

// 나는 판매자 인가? 
BOOL CVTInfo::VendorIsVendor() const noexcept {
	static constexpr auto IsNotNull = [](const auto * ptr) { return ptr; };
	return std::ranges::any_of(m_items_VT, IsNotNull) ? TRUE : FALSE;
}

BOOL CVTInfo::IsTrading(const CItemElem * const pItemElem) const noexcept {
	const auto IsThisItem = [pItemElem](const CItemElem * const self) {
		return pItemElem == self;
	};
	return std::ranges::any_of(m_items_VT, IsThisItem) ? TRUE : FALSE;
}

//CDPSrvr::OnBuyPVendorItem
#ifdef __WORLDSERVER
BOOL CVTInfo::VendorSellItem( CMover* pBuyer, BYTE i, DWORD dwItemId, short nNum, VENDOR_SELL_RESULT& result )
{
	result.nRemain    = 0;
	result.nErrorCode = 0;

	if( IsVendorOpen() == FALSE )
		return FALSE;

	CItemElem * pItemElem = m_items_VT[i];
	if( IsUsingItem(pItemElem) == FALSE || pItemElem->m_dwItemId != dwItemId )
		return FALSE;

	if( nNum < 1 )
		nNum = 1;
	if( nNum > pItemElem->GetExtra() )
		nNum = (short)pItemElem->GetExtra();

//	06.10.26
	if(pItemElem->m_nCost > 0 && (float)pBuyer->GetGold() < (float)nNum * (float)pItemElem->m_nCost )
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}
	
	if( nNum == 0 )
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}

	CItemElem itemElem;
	itemElem	= *pItemElem;
	itemElem.m_nItemNum	 = nNum;

	if( pBuyer->CreateItem( &itemElem ) == FALSE )
	{
		result.nErrorCode = TID_GAME_LACKSPACE;
		return FALSE;
	}

	// CItemElem의 복사 연산자에 m_nCost는 제외되어 있다.
	int nCost	= pItemElem->m_nCost;
	pBuyer->AddGold( -(pItemElem->m_nCost * nNum ) );
	m_pOwner->AddGold(pItemElem->m_nCost * nNum );

	pItemElem->SetExtra(pItemElem->GetExtra() - nNum );
	int nRemain = pItemElem->GetExtra();
	if( nRemain <= 0 )
		m_items_VT[i] = nullptr;

#ifdef __WORLDSERVER
	g_UserMng.AddPVendorItemNum( (CUser*)m_pOwner, i, nRemain, pBuyer->GetName() );
#endif	// __WORLDSERVER

	m_pOwner->RemoveItem( (BYTE)pItemElem->m_dwObjId, nNum );

	result.item = itemElem;
	result.item.m_nCost	= nCost;
	result.nRemain = nRemain;
	return TRUE;
}
#endif // __WORLDSERVER