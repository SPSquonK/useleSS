#include "stdafx.h"


#include "buff.h"
#include "defineskill.h"
#include "defineobj.h"


#if defined(__WORLDSERVER) || defined(__CLIENT)
#include "FuncTextCmd.h"
#endif

#ifdef __WORLDSERVER
#include "user.h"
#include "dpdatabaseclient.h"
#ifdef __BUFF_TOGIFT
#include "DPSrvr.h"
#endif // __BUFF_TOGIFT
#endif	// __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////
IBuff::IBuff()
:
m_wType( 0 )
, m_wId( 0 )
, m_dwLevel( 0 )
, m_tmTotal( 0 )
, m_tmInst( 0 )
, m_oiAttacker( NULL_ID )
#ifdef __CLIENT
, m_bSFX( FALSE )
#endif	// __CLIENT
, m_bRemove( FALSE )
{
}

void IBuff::SetTotal(const DWORD tmTotal) {
	m_tmTotal = std::max(tmTotal, GetRemain());
}

#ifndef __DBSERVER
BOOL IBuff::IsAddable( CMover* pMover )
{
	int nTotal	= static_cast<int>( GetTotal() );
	ASSERT( nTotal >= 0 );
	if( nTotal < 0 )
		return FALSE;
	return TRUE;
}

void IBuff::AddTotal( DWORD tmTotal )
{
	SetTotal( tmTotal );
	SetInst( ::timeGetTime() );		// +
}

#endif	// __DBSERVER

DWORD IBuff::GetRemain()
{
	if( GetInst() == 0 )
		return 0;
	
	DWORD dwElapsedTime = ::timeGetTime() - GetInst();
	if( dwElapsedTime > GetTotal() )	// 남은 시간은 총 버프 시간보다 클 수 없다.
		return 0;						// 실제로 버프 제거는 시간 만료 후 다음 프로세스에서 하기 때문에 발생 가능하다.

	return GetTotal() - dwElapsedTime;
}

IBuff& IBuff::operator =( const IBuff & buff )
{
	m_wType	= buff.m_wType;
	m_wId	= buff.m_wId;
	m_dwLevel	= buff.m_dwLevel;
	m_tmTotal	= buff.m_tmTotal;
	m_tmInst	= buff.m_tmInst;
	m_oiAttacker	= buff.m_oiAttacker;
#ifdef __CLIENT
	m_bSFX	= buff.m_bSFX;
#endif	// __CLIENT
	return *this;
}

void IBuff::SerializeLevel( CAr & ar )
{
	if( ar.IsStoring() )
	{
#ifdef __DBSERVER
		if( GetType() == BUFF_ITEM2 )
		{
			time_t t	= static_cast<time_t>( GetLevel() ) - time_null();
			ar << t;
		}
		else
		{
			ar << m_dwLevel;
		}
#else	// __DBSERVER
		ar << m_dwLevel;
#endif	// __DBSERVER
	}
	else
	{
#ifdef __DBSERVER
		if( GetType() == BUFF_ITEM2 )
		{
			time_t t;
			ar >> t;
			SetLevel( time_null() + t );
		}
		else
		{
			ar >> m_dwLevel;
		}
#else	// __DBSERVER
		ar >> m_dwLevel;
#endif	// __DBSERVER
	}
}

void IBuff::Serialize( CAr & ar, CMover* pMover )
{
	if( ar.IsStoring() )
	{
		ar << m_wType;
		ar << m_wId;
		SerializeLevel( ar );
#ifdef __WORLDSERVER
		DWORD tmTotal	= 0;
		if( GetTotal() > 0 )
		{
			if( GetInst() > 0 )
				tmTotal	= GetTotal() - ( g_tmCurrent - GetInst() );
			else
				tmTotal	= GetTotal();
		}
		if( static_cast<int>( tmTotal ) < 0 )
			tmTotal	= 0;
		ar << tmTotal;
#else	// __WORLDSERVER
		ar << m_tmTotal;
#endif	// __WORLDSERVER
	}
	else
	{
		ar >> m_wId;
		SerializeLevel( ar );
		DWORD tmTotal;
		ar >> tmTotal;
#ifdef __WORLDSERVER
		if( IsCommon() && pMover && tmTotal >= MIN( 60 ) )
			tmTotal	= MIN( 60 );
#endif	// __WORLDSERVER
		SetTotal( tmTotal );
	}
}

#ifndef __DBSERVER
BOOL IBuff::Timeover( CMover* pMover, DWORD tmCurrent )
{
	return tmCurrent - GetInst() >= GetTotal();
}

BOOL IBuff::Expire( CMover* pMover, DWORD tmCurrent )
{
	if( IsRemovable( pMover ) && Timeover( pMover, tmCurrent ) )
	{
#ifdef __WORLDSERVER
		OnExpire( pMover );
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

BOOL IBuff::Process( CMover* pMover, DWORD tmCurrent )
{
	if( GetRemove() )
		return TRUE;
	if( IsInvalid( pMover ) )
		return TRUE;
	if( pMover )
	{
#ifdef __CLIENT
		CreateSFX( pMover );
#endif	// __CLIENT
		if( GetInst() == 0 )
		{
			Apply( pMover );
			SetInst( tmCurrent );
		}
	}
	return Expire( pMover, tmCurrent );
}
#endif	// __DBSERVER

#ifdef __CLIENT
void IBuff::CreateSFX( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	if( pProp && !HasSFX() )
	{
		FLOAT fSkillTime	= (float)GetTotal() / 1000.0F;	// 초단위
		SetSFX();
		// 지속 효과: dwSfxObj4
		if( pProp->dwSfxObj4 != NULL_ID )
		{
			if( GetTotal() == 0 )
				CreateSfx( pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), 0 );
			else
				CreateSfx( pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), (int)( fSkillTime ) );
		}
	}
}
#endif	// __CLIENT

#ifndef __DBSERVER
////////////////////////////////////////////////////////////////////////////////

void IBuffItemBase::Release( CBuffMgr* pBuffMgr )
{
#ifdef __WORLDSERVER
	ItemProp* pProp	= GetProp();
	if( pProp )
	{
		for (int i = 0; i != ItemProp::NB_PROPS; ++i) {
			pBuffMgr->GetMover()->ResetDestParam(pProp->dwDestParam[i], pProp->nAdjParamVal[i], TRUE);
		}

		if( strlen( pProp->szTextFileName ) > 0 && IK3_ANGEL_BUFF != pProp->dwItemKind3 
			&& IK2_PAPERING != pProp->dwItemKind2
			)
		{
			if( NULL_ID != pProp->dwActiveSkill )
				pBuffMgr->RemoveBuff( BUFF_SKILL, (WORD)( pProp->dwActiveSkill ) );
			char szCommand[100]	= { 0,};
			CString strCommand	= pProp->szTextFileName;
			strCommand.Replace( '(', '\"' );
			strCommand.Replace( ')', '\"' ); 
			int n	= strCommand.Find( "/", 1 );
			if( n >= 0 )
			{
				strncpy(szCommand, &pProp->szTextFileName[n], strCommand.GetLength());
				CUser * user = dynamic_cast<CUser *>(pBuffMgr->GetMover());
				if (user) g_textCmdFuncs.ParseCommand(szCommand, user, TRUE);
			}
		}
#ifdef __BUFF_TOGIFT
		if( pProp->dwItemKind2 == IK2_BUFF_TOGIFT )
		{
			CUser* pUser = static_cast<CUser*>( pBuffMgr->GetMover() );
			ItemProp* pItemProp = prj.GetItemProp( pProp->dwReferTarget1 );
			if( IsValidObj( pUser ) && pItemProp )
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = pItemProp->dwID;
				itemElem.m_nItemNum = 1;
				itemElem.SetSerialNumber();
				if( pUser->CreateItem( &itemElem ) )
					g_DPSrvr.PutItemLog( pUser, "g", "BuffToGift", &itemElem, 1 );
				else
				{
					g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, pItemProp->szName, "" );
					g_DPSrvr.PutItemLog( pUser, "g", "BuffToGift_Post", &itemElem, 1 );
				}
			}
		}
#endif // __BUFF_TOGIFT
	}
#endif	// __WORLDSERVER
}

ItemProp* IBuffItemBase::GetProp()
{
	return prj.GetItemProp( GetId() );
}

BOOL IBuffItemBase::IsCommon()
{
	ItemProp* pProp	= GetProp();
	return !( pProp && pProp->bCharged );
}

BOOL IBuffItemBase::HasChrState( CMover*, DWORD dwChrState )
{
	ItemProp* pProp	= GetProp();
	if( pProp )
	{
		for( int i = 0; i < 2; i++ )
			if( pProp->dwDestParam[i] == DST_CHRSTATE && pProp->nAdjParamVal[i] & dwChrState )
				return TRUE;
	}
	return FALSE;
}

BOOL IBuffItemBase::IsDebuf( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->nEvildoing < 0;
}

#ifdef __CLIENT
BOOL IBuffItemBase::IsCommercial()
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->bCharged;	// ?	// pProp->dwID != II_PET_EGG
}
#endif	// __CLIENT

////////////////////////////////////////////////////////////////////////////////
BOOL CBuffItem::IsIk1( DWORD dwIk1 )
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->dwItemKind1 == dwIk1;
}

BOOL CBuffItem::IsIk3( DWORD dwIk3 )
{
	ItemProp* pProp	= GetProp();
	return pProp && pProp->dwItemKind3 == dwIk3;
}


void CBuffItem::Apply( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	if( pProp )
		pMover->ApplyParam( pMover, pProp, NULL, FALSE, 0 );
}

BOOL CBuffItem::IsRemovable( CMover* pMover )
{
	ItemProp* pProp	= GetProp();
	return !( pProp && 999999999 == pProp->dwSkillTime );
}

void CBuffItem::AddTotal( DWORD tmTotal )
{
	switch( GetId() )
	{
		case II_SYS_SYS_SCR_PET_FEED_POCKET02:
		case II_SYS_SYS_SCR_PET_TONIC_A:
		case II_SYS_SYS_SCR_PET_TONIC_B:
			tmTotal	+= GetRemain();
			break;
		default:
			break;
	}
	IBuff::AddTotal( tmTotal );
}

DWORD CBuffItem::GetDisguise()
{
	ItemProp* pProp	= GetProp();
	if( pProp && pProp->dwItemKind3 == IK3_TEXT_DISGUISE )
	{
		const char* ptr	= ::strstr( pProp->szTextFileName, "/dis" );
		if( ptr )
		{
			int n	= atoi( ptr + 5 );	// "/dis "
			if( n > 0 )
			{
				MoverProp* pMoverProp	= prj.GetMoverProp( n );
				if( pMoverProp )
					return pMoverProp->dwID;
			}
		}
	}
	return NULL_ID;
}

#ifdef __WORLDSERVER
void CBuffItem::OnExpire( CMover* pMover )
{
	if( GetId() == II_SYS_SYS_SCR_PET_FEED_POCKET02 )
	{
		ItemProp* pProp	= GetProp();
		if( pProp && pMover->IsPlayer() )
			g_dpDBClient.SendLogSMItemUse( "2", static_cast<CUser*>( pMover ), NULL, pProp );
	}
}
#endif	// __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////
BOOL CBuffSkill::IsInvalid( CMover* pMover )
{
#ifdef __WORLDSERVER
	switch( GetId() )
	{
		case SI_JST_SUP_POISON:
		case SI_JST_SUP_BLEEDING:
		case SI_JST_SUP_ABSORB:
		case SI_ACR_SUP_YOYOMASTER:
		case SI_JST_YOYO_CRITICALSWING:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_YOYO )
				return TRUE;
			break;
		case SI_RAG_SUP_FASTATTACK:
		case SI_ACR_SUP_BOWMASTER:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_RANGE_BOW )
				return TRUE;
			break;
		case SI_BLD_MASTER_ONEHANDMASTER:
			if( !pMover->IsDualWeapon() )
				return TRUE;
			break;
		case SI_KNT_MASTER_TWOHANDMASTER:
			if( pMover->GetActiveHandItemProp()->dwHanded != HD_TWO )
				return TRUE;
			break;
		case SI_MER_SUP_IMPOWERWEAPON:		// 
			{
				BOOL bRelease	= FALSE;
				CItemElem* pRight	= pMover->GetWeaponItem();
				CItemElem* pLeft	= pMover->GetLWeaponItem();
				if( pRight && pLeft )
					bRelease	= pRight->m_bItemResist == SAI79::NO_PROP
												&& pLeft->m_bItemResist == SAI79::NO_PROP;
				else if( pRight && !pLeft )
					bRelease	= pRight->m_bItemResist == SAI79::NO_PROP;
				else if( !pRight && !pLeft )
					bRelease	= TRUE;
				if( bRelease )
					return TRUE;
				break;
			}
		case SI_MER_SUP_BLAZINGSWORD:
		case SI_MER_SUP_SWORDMASTER:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_SWD )
				return TRUE;
			break;
		case SI_MER_SUP_SMITEAXE:
		case SI_MER_SUP_AXEMASTER:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_AXE )
				return TRUE;
			break;
		case SI_BIL_PST_ASMODEUS:
			if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_KNUCKLE )
				return TRUE;
			break;
		case SI_MER_SHIELD_PROTECTION:
		case SI_MER_SHIELD_PANBARRIER:
			if( pMover->GetActiveHandItemProp( PARTS_SHIELD )->dwItemKind3 != IK3_SHIELD )
				return TRUE;
			break;
	}
#endif	// __WORLDSERVER
	return FALSE;
}

void CBuffSkill::Apply( CMover* pMover )
{
#ifdef __CLIENT
	if( !pMover )	return;
#endif	// __CLIENT
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	BOOL bResult	= pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.Apply" );
	if( bResult )
		pMover->ApplyParam( pMover, pSkillProp, pAddSkillProp, FALSE, 0 );
}

void CBuffSkill::Release( CBuffMgr* pBuffMgr )
{
#ifdef __CLIENT
	CMover* pMover	= pBuffMgr->GetMover();
	if( pMover )
		pMover->OnEndSkillState( GetId(), GetLevel() );
#else	// __CLIENT
	pBuffMgr->GetMover()->OnEndSkillState( GetId(), GetLevel() );
#endif	// __CLIENT
}

BOOL CBuffSkill::IsRemovable( CMover* pMover )
{
#ifdef __CLIENT
	if( !pMover )	return TRUE;
#endif	// __CLIENT
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsRemovable" );
	return pAddSkillProp && pAddSkillProp->dwSkillTime > 0;
}

ItemProp* CBuffSkill::GetProp()
{
	return prj.GetSkillProp( GetId() );
}

BOOL CBuffSkill::IsAddable( CMover* pMover )
{
#ifdef __CLIENT
	ItemProp* pProp	= GetProp();
	return IBuff::IsAddable( pMover )
		&& !( pMover && pMover->IsPlayer() && pMover->m_nDead > 0 && pProp && pProp->nEvildoing < 0 );
#else	// __CLIENT
	ItemProp* pProp	= GetProp();
	return IBuff::IsAddable( pMover )
					&& !( pMover->IsPlayer() && pMover->m_nDead > 0 && pProp && pProp->nEvildoing < 0 );
#endif	// __CLIENT
}

BOOL CBuffSkill::IsGood( CMover* pMover )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsGood" );
	return pSkillProp && pSkillProp->nEvildoing >= 0;
}

BOOL CBuffSkill::HasChrState( CMover* pMover, DWORD dwChrState )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.HasChrState" );
	for( int i = 0; i < 2; i++ )
		if( pAddSkillProp->dwDestParam[i] == DST_CHRSTATE && pAddSkillProp->nAdjParamVal[i] & dwChrState )
			return TRUE;
	return FALSE;
}

BOOL CBuffSkill::HasDstParam( CMover* pMover, DWORD dwDstParam )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.HasDstParam" );
	for( int i = 0; i < 2; i++ )
		if( pSkillProp->nEvildoing < 0 && pAddSkillProp->dwDestParam[i] == dwDstParam )
			return TRUE;
	return FALSE;
}

BOOL CBuffSkill::IsDebuf( CMover* pMover )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsDebuf" );
	return pSkillProp && pSkillProp->nEvildoing < 0;
}

BOOL CBuffSkill::IsAttacker( CMover* pMover, OBJID oiAttacker )
{
	ASSERT( pMover );
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsAttacker" );
	if( pSkillProp && pSkillProp->nEvildoing < 0 && GetAttacker() == oiAttacker )
		return TRUE;
	return FALSE;
}

#ifdef __WORLDSERVER
void CBuffSkill::OnExpire( CMover* pMover )
{
	if( GetId() == SI_GEN_BURN )
	{
		ItemProp* pSkillProp	= prj.GetSkillProp( SI_GEN_BURN );
		int nMin	= pSkillProp->dwAbilityMin;
		int nMax	= pSkillProp->dwAbilityMax;
		int nDamage	= xRandom( nMin, nMax );
		pMover->SendDamage( AF_FORCE, GetAttacker(), nDamage );
		g_UserMng.AddCreateSfxObj( pMover, XI_SKILL_ELE_FIRE_BURINGFIELD02 );
	}
}
#endif	// __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////
void CBuffPet::Apply( CMover* pMover )
{
	pMover->SetDestParam( HIWORD( GetLevel() ), LOWORD( GetLevel() ), NULL_CHGPARAM, FALSE );
}

void CBuffPet::Release( CBuffMgr* pBuffMgr )
{
#ifdef __WORLDSERVER
	pBuffMgr->GetMover()->ResetDestParam( HIWORD( GetLevel() ), LOWORD( GetLevel() ), TRUE );
#endif	// __WORLDSERVER
}

////////////////////////////////////////////////////////////////////////////////
BOOL CBuffItem2::Timeover( CMover* pMover, DWORD tmCurrnet )
{
	time_t	t	= time_null();
	if( (DWORD)( t ) >= GetLevel() )
	{
#ifdef __WORLDSERVER
		ItemProp *pProp	= GetProp();
		if( pProp && pMover->IsPlayer() )
			g_dpDBClient.SendLogSMItemUse( "2", static_cast<CUser*>( pMover ), NULL, pProp );
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

void CBuffItem2::SerializeLevel( CAr & ar )
{
	if( ar.IsStoring() )
	{
		time_t t	= static_cast<time_t>( GetLevel() ) - time_null();
		ar << t;
	}
	else
	{
		time_t t;
		ar >> t;
		SetLevel( time_null() + t );
	}
}
#endif	// __DBSERVER

CBuffMgr::CBuffMgr( CMover* pMover )
:
m_pMover( pMover )
{
}

CBuffMgr::~CBuffMgr()
{
	Clear();
}

void CBuffMgr::Clear()
{
	for (auto & [_, ptr] : m_mapBuffs) {
		SAFE_DELETE(ptr);
	}
	m_mapBuffs.clear();
}

IBuff* CBuffMgr::CreateBuff( WORD wType )
{
	IBuff* ptr	= NULL;
#ifdef __DBSERVER
	ptr	= new IBuff;
#else	// __DBSERVER
	switch( wType )
	{
		case BUFF_SKILL:	ptr	= new CBuffSkill;	break;
		case BUFF_ITEM:		ptr	= new CBuffItem;	break;
		case BUFF_ITEM2:	ptr	= new CBuffItem2;	break;
		case BUFF_PET:		ptr	= new CBuffPet;		break;
		case BUFF_EQUIP:	ptr	= new CBuffEquip;	break;
		default:	return NULL;
	}
#endif	// __DBSERVER
	ptr->SetType( wType );
	return ptr;
}

#ifndef __DBSERVER
BOOL CBuffMgr::Overwrite( IBuff* pBuff )
{
	IBuff* pOld	= GetBuff( pBuff->GetType(), pBuff->GetId() );
	if( pOld )
	{
		if( pOld->GetLevel() == pBuff->GetLevel() )
		{
			pOld->AddTotal( pBuff->GetTotal() );
			return TRUE;
		}
		else if( pOld->GetLevel() < pBuff->GetLevel() )
			RemoveBuff( pOld, FALSE );
		else	// if( pOld->GetLevel() > pBuff->GetLevel() )	// ignore
			return TRUE;
	}
	return FALSE;
}

BOOL CBuffMgr::HasBuff( WORD wType, WORD wId ) const {
	return m_mapBuffs.contains(Key(wId, wType)) ? TRUE : FALSE;
}

IBuff * CBuffMgr::GetBuff(WORD wType, WORD wId) {
	const auto i = m_mapBuffs.find(Key(wId, wType));
	return i != m_mapBuffs.end() ? i->second : nullptr;
}

IBuff * CBuffMgr::GetBuffByIk3(DWORD dwIk3) {
	for (auto & [_key, pBuff] : m_mapBuffs) {
		if (pBuff->IsIk3(dwIk3))
			return pBuff;
	}
	return nullptr;
}

bool CBuffMgr::HasBuffByIk3(const DWORD dwIk3) const {
	return std::ranges::any_of(
		m_mapBuffs | std::views::values,
		[dwIk3](IBuff * pBuff) { return pBuff->IsIk3(dwIk3); }
	);
}

BOOL CBuffMgr::AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTotal, OBJID oiAttacker )
{
	IBuff* pBuff	= CreateBuff( wType );
	pBuff->SetId( wId );
	pBuff->SetLevel( dwLevel );
	pBuff->SetTotal( tmTotal );
	pBuff->SetInst( ::timeGetTime() );		// +
	pBuff->SetAttacker( oiAttacker );
	BOOL bNew	= AddBuff( pBuff );
	if( !bNew )
		SAFE_DELETE( pBuff );
	return bNew;
}

BOOL CBuffMgr::AddBuff( IBuff* pBuff )
{
	if( !pBuff->IsAddable( GetMover() ) )
		return FALSE;
	if( Overwrite( pBuff ) )
	{
#ifdef __WORLDSERVER
		g_UserMng.AddSetSkillState( GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal() );
#endif	// __WORLDSERVER
		return FALSE;
	}
	PrepareBS( pBuff );
	bool bResult	= Add( pBuff );	
#ifdef __WORLDSERVER
	if( bResult == true )
		g_UserMng.AddSetSkillState( GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal() );
#endif	// __WORLDSERVER
	return static_cast<BOOL>( bResult );
}

void CBuffMgr::RemoveBuff( IBuff* pBuff, BOOL bFake )
{
	ASSERT( pBuff );
	if( !pBuff->GetRemove() )	// pBuff에 대하여 RemoveBuff의 최초 호출 시 Release 호출
		pBuff->Release( this );
#ifdef __WORLDSERVER
	if( bFake )
	{
		pBuff->SetRemove();
		return;
	}
	g_UserMng.AddRemoveSkillInfluence( GetMover(), pBuff->GetType(), pBuff->GetId() );
#endif	// __WORLDSERVER
	int nResult	= m_mapBuffs.erase( Key( pBuff->GetId(), pBuff->GetType() ) );
	if( !nResult )
	{
		Error( "CBuffMgr.RemoveBuff: could not erase(type: %d, id: %d)", pBuff->GetType(), pBuff->GetId() );
		return;
	}
	SAFE_DELETE( pBuff );
}

void CBuffMgr::RemoveBuff( WORD wType, WORD wId, BOOL bFake )
{
	IBuff* pBuff	= GetBuff( wType, wId );
	if( pBuff )
		RemoveBuff( pBuff, bFake );
}

void CBuffMgr::PrepareBS( IBuff* pBuff )
{
	if( pBuff->GetType() == BUFF_SKILL && IsBSFull() )
	{
		IBuff* pFirst	= GetFirstBS();

		if( pFirst )
			RemoveBuff( pFirst, FALSE );
	}
}

bool CBuffMgr::IsBSFull() const {
	size_t nNum = 0;
	for (IBuff * pBuff : m_mapBuffs | std::views::values) {
		if (pBuff->GetType() == BUFF_SKILL) {
			++nNum;
		}
	}
	return nNum >= MAX_SKILLBUFF_COUNT;
}

IBuff * CBuffMgr::GetFirstBS() {
	for (IBuff * pBuff : m_mapBuffs | std::views::values) {
		if (pBuff->GetType() == BUFF_SKILL) {
			ItemProp * pProp = pBuff->GetProp();
			if (pProp && pProp->nEvildoing >= 0)
				return pBuff;
		}
	}

	return NULL;
}

void CBuffMgr::Process()
{
	std::stack<std::pair<WORD, WORD>> vTemp;

	for (IBuff * pBuff : m_mapBuffs | std::views::values) {
		const BOOL bExpired	= pBuff->Process( GetMover(), this->GetCurrentTime() );
		if( bExpired )
			vTemp.emplace( pBuff->GetId(), pBuff->GetType() );
	}

	while( !vTemp.empty() )
	{
		auto pair = vTemp.top();
		RemoveBuff( pair.second, pair.first, FALSE );
		vTemp.pop();
	}
}

void CBuffMgr::RemoveBuffs( DWORD dwFlags, DWORD dwParam )
{
	std::stack<std::pair<WORD, WORD>> vTemp;
	for (IBuff * pBuff : m_mapBuffs | std::views::values) {

		if(
			( ( dwFlags & RBF_UNCONDITIONAL )
				&& !pBuff->IsIk1( IK1_HOUSING )	// 하우징 버프는 "버프해제" 명령으로 삭제 안됨.
			)
			|| ( ( dwFlags & RBF_COMMON ) && GetMover() && pBuff->IsCommon() )
			|| ( ( dwFlags & RBF_IK3 ) && pBuff->IsIk3( dwParam ) )
			|| ( ( dwFlags & RBF_GOODSKILL ) && pBuff->IsGood( GetMover() ) )
			|| ( ( dwFlags & RBF_CHRSTATE ) && pBuff->HasChrState( GetMover(), dwParam ) )
			|| ( ( dwFlags & RBF_DSTPARAM ) && pBuff->HasDstParam( GetMover(), dwParam ) )
			|| ( ( dwFlags & RBF_DEBUF ) && pBuff->IsDebuf( GetMover() ) )
			|| ( ( dwFlags & RBF_ATTACKER ) && pBuff->IsAttacker( GetMover(), dwParam ) )
		)
		{
			vTemp.emplace( pBuff->GetType(), pBuff->GetId() );
			if( dwFlags & RBF_ONCE )
				break;
		}
	}
	while( !vTemp.empty() )
	{
		auto l	= vTemp.top();
		RemoveBuff( l.first, l.second );
		vTemp.pop();
	}
}

DWORD CBuffMgr::GetDisguise()
{
	for (IBuff * pBuff : m_mapBuffs | std::views::values) {
		DWORD dwIndex	= pBuff->GetDisguise();
		if( dwIndex != NULL_ID )
			return dwIndex;
	}
	return NULL_ID;
}
#endif	// __DBSERVER

#ifdef __WORLDSERVER
IBuff* CBuffMgr::GetBuffPet()
{
	for (IBuff * pBuff : m_mapBuffs | std::views::values) {
		if( pBuff->GetType() == BUFF_PET )
			return pBuff;
	}
	return NULL;
}

void CBuffMgr::RemoveBuffPet( IBuff* pBuff )
{
	ASSERT( pBuff );
	RemoveBuff( pBuff, FALSE );
	CItemElem* pItem	= GetMover()->GetPetItem();
	if( pItem )
		GetMover()->ResetDestParamRandomOptExtension( pItem );
}

void CBuffMgr::RemoveBuffPet()
{
	IBuff* pBuff	= GetBuffPet();
	if( pBuff )
		RemoveBuffPet( pBuff );
}
#endif	// __WORLDSERVER

void CBuffMgr::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		auto pSize = ar.PushBack<size_t>(0);
		for (const auto & [_, pBuff] : m_mapBuffs) {
			if (!pBuff->GetRemove()) {
				pBuff->Serialize(ar, GetMover());
				++*pSize;
			}
		}
	}
	else
	{
		Clear();
		size_t t;
		ar >> t;
		for( size_t i = 0; i < t; i++ )
		{
			WORD wType;
			ar >> wType;
			IBuff* pBuff	
				= CreateBuff( wType );
			pBuff->Serialize( ar, GetMover() );
			bool bResult	= Add( pBuff );
			ASSERT( bResult );
		}
#ifndef __DBSERVER
		Process();
#endif // __DBSERVER
	}
}

CBuffMgr& CBuffMgr::operator =( CBuffMgr & bm )
{
	Clear();
	for (const auto & [_, pSource] : bm.m_mapBuffs) {
		IBuff* pDest	= CreateBuff( pSource->GetType() );
		*pDest	= *pSource;
		bool bResult	= Add( pDest );
		ASSERT( bResult );
	}
	return *this;
}


#ifdef __CLIENT
int CBuffMgr::GetCommercialCount()
{
	return static_cast<int>(
		std::ranges::count_if(
			m_mapBuffs | std::views::values,
			[](IBuff * pBuff) { return pBuff->IsCommercial(); }
		)
		);
}

void CBuffMgr::ClearInst() {
	for (IBuff * pBuff : m_mapBuffs | std::views::values) {
		pBuff->SetInst(0);
	}
}
#endif	// __CLIENT

#ifdef __DBSERVER
void CBuffMgr::ToString( char* szString )
{
	char szBuff[256]	= { 0,};
	for (const auto & [_, pBuff] : m_mapBuffs) {
		if( pBuff->GetType() == BUFF_EQUIP )
			continue;
		if( pBuff->GetType() == BUFF_ITEM )
		{
			ItemProp* pProp	= prj.GetItemProp( pBuff->GetId() );
			if( pProp && pProp->dwItemKind1 == IK1_HOUSING )
				continue;
		}
		sprintf( szBuff, "%d,%d,%d,%d/", 
			pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal() );
		strcat( szString, szBuff );
	}
	strcat( szString, NullStr );
}

#endif	// DBSERVER

