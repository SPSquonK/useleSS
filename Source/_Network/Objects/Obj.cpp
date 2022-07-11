#include "stdafx.h"
#include "..\_Network\Objects\Obj.h"

#pragma warning ( disable : 4355 )

extern	CProject	prj;

#include "serialnumber.h"

int		CObj::m_nMethod		= METHOD_NONE;

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CObj::CObj()
#ifdef __LAYER_1015
:
m_nLayer( 0 )
#endif	// __LAYER_1015
{
	m_dwType		=	m_dwIndex	= m_dwMotion	= 0;
	m_vPos	= D3DXVECTOR3( 0, 0, 0 );
	m_vScale	= D3DXVECTOR3( 1, 1, 1 );
	m_fAngle	= 0;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CCtrl::CCtrl()
{
	m_dwType	= OT_CTRL;
	m_objid		= NULL_ID;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CItemElem::CItemElem()
{
	memset(m_szItemText, 0, sizeof(m_szItemText));
	m_dwItemId = 0;
	m_dwObjId = NULL_ID;
	m_dwObjIndex = NULL_ID;
	m_liSerialNumber = 0;

	m_byFlag			= 0;
	m_nItemNum			= 1;
	m_idGuild			= 0;
	m_nAbilityOption	= 0;
	m_nRepairNumber		= 0;
	m_nHitPoint			= 0;
	m_nRepair			= 0;
	
    m_bItemResist = SAI79::NO_PROP;
	m_nResistAbilityOption = 0;	
	m_nResistSMItemId = 0;

//	memset( &m_piercingInfo, 0x00, sizeof(m_piercingInfo) );

	m_bCharged	= FALSE;
	m_dwKeepTime = 0;
	m_iRandomOptItemId	= 0;

	m_pPet	= NULL;
	m_bTranformVisPet = FALSE;
}

CItemElem::~CItemElem()
{
	SAFE_DELETE( m_pPet );
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CItem::CItem()
{
	m_dwType = OT_ITEM;
	m_pItemBase = NULL;
}

CItem::~CItem()
{
	SAFE_DELETE( m_pItemBase );
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
//#ifdef __DBSERVER
#if defined(__DBSERVER)
#ifndef __VM_0820
#ifndef __MEM_TRACE
CMoverPool*	CMover::m_pPool	= new CMoverPool( 1024 );
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __DBSERVER

CMover::CMover()
#ifdef __BUFF_1107
: m_buffs( this )
#endif	// __BUFF_1107
{
	memset( m_aJobSkill, 0, sizeof(SKILL) * MAX_SKILL_JOB );
	m_dwBelligerence	= 0;
#ifdef __JEFF_9_20
	m_dwMute	= 0;
#else	// __JEFF_9_20
	m_dwFlyTime	= 0;
#endif	// __JEFF_9_20
	m_dwReturnWorldID	= 0;
	m_idGuildCloak	= 0;
	memset( m_szName, 0, sizeof(m_szName) );
	m_vMarkingPos	= D3DXVECTOR3( 0, 0, 0 );
	m_vReturnPos	= D3DXVECTOR3( 0, 0, 0 );
	m_dwType		= OT_MOVER;
	m_bPlayer	= FALSE;
	m_idPlayer	= (u_long)0xffffffff;
//	memset( m_szName, 0, sizeof(m_szName) );
	memset( m_szCharacterKey, 0, sizeof(m_szCharacterKey ) );
	m_nHitPoint		= m_nManaPoint	= m_nFatiguePoint	= 0;
	m_pActMover		= new CActionMover( this );
	m_Inventory.SetItemContainer( CItemContainer::ContainerTypes::INVENTORY );

	m_nSlot = 0;
	for( int i = 0 ; i < 3 ; ++i )
	{
		m_Bank[i].SetItemContainer(CItemContainer::ContainerTypes::BANK) ;
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
	m_nPlusMaxHitPoint = 0;
	m_nAttackResistLeft = (BYTE)0xff;
	m_nAttackResistRight = (BYTE)0xff;
	m_nDefenseResist = (BYTE)0xff;
	memset( m_dwSMTime, 0, sizeof( DWORD ) * SM_MAX );
#ifndef __BUFF_1107
	m_SkillState.Init();
	m_SkillState.SetMover( this );
#endif	// __BUFF_1107
	ClearEquipInfo();
	m_dwSkinSet	= 0;//SKINSET_00;
	m_dwHairMesh	= 0;//HAIRMESH_00;
	m_dwHairColor	= 0xffffffff;
	m_dwHeadMesh	= 0;
	m_nTutorialState	= 0;
	m_nFxp			= 0;
//	m_nTxp			= 0;
	m_bySex		= SEX_FEMALE;
	m_dwRideItemIdx		= 0;

  	m_dwGold	= 0;

	m_nJob	= -1;
	
	m_nStr	= m_nSta	= m_nDex	= m_nInt	= 15;
	m_nLevel	= 1;
	m_nExp1	= m_nDeathExp	= 0;
	m_nDeathLevel = 0;

	m_idMarkingWorld	= NULL_ID;

	m_dwAuthorization	= AUTH_GENERAL;
	m_dwMode	= 0;
	m_dwStateMode = 0;
	m_dwUseItemId = 0;
	m_nRemainGP	= 0;

	m_quests = std::make_unique<MoverSub::Quests>();

	m_idparty = 0;
	m_idGuild	= 0;
	m_idWar	= WarIdNone;
	m_dwPKTime			= 0;			/// 핑크 상태 시간
	m_nPKValue			= 0;			/// PK 수치
	m_dwPKPropensity	= 0;			/// PK 성향
	m_dwPKExp			= 0;			/// PK 성향 습득 경험치
	m_nAngelExp			= 0;			/// 엔젤 경험치
	m_nAngelLevel		= 0;			/// 엔젤 Level
	m_nFame		= 0;
	m_idMurderer	= 0;
#ifdef __DBSERVER
	m_dwWorldID		= 1;
#endif // __DBSERVER
	memset( m_szBankPass, 0, sizeof( m_szBankPass ) );

	m_nDuel	= 0;
	m_idDuelParty	= 0;
	m_nFuel		= -1;
	m_bActiveAttack		= FALSE;
	m_nMovePattern = m_nMoveEvent = m_nMoveEventCnt = 0;
	m_tmAccFuel = 0;
	m_dwPeriodicTick	= 0;
	m_dwTickCheer	= 0;
	m_nCheerPoint	= 0;
	m_nGuildCombatState = 0;
	m_nSkillLevel = 0;
	m_nSkillPoint = 0;

#ifdef __DBSERVER
#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	m_nEventFlag = 0;
#else // __EVENT_1101_2
	m_dwEventFlag	= 0;
#endif // __EVENT_1101_2
	m_dwEventTime	= 0;
	m_dwEventElapsed	= 0;
#endif	// __EVENT_1101
#ifdef __INVALID_LOGIN_0612
	m_bLogout	= FALSE;
	*m_szAccount	= '\0';
#endif	// __INVALID_LOGIN_0612
#endif	// __DBSERVER

	m_dwPetId	= NULL_ID;
	m_fSpeedFactor	= 1.0F;
	
#ifdef __EXP_ANGELEXP_LOG
	m_nExpLog = 0;
	m_nAngelExpLog = 0;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	m_nCoupon = 0;
#endif // __EVENTLUA_COUPON

#ifdef __JEFF_9_20
	m_dwMute	= 0;
#endif	// __JEFF_9_20
	m_nHonor = -1;					// 달인선택 
	memset( m_aHonorTitle, 0, sizeof( int ) * MAX_HONOR_TITLE );// 달인수치
	m_dwPCBangClass = 0;
	m_objIdVisPet = NULL_ID;
	m_dwMoverSfxId = NULL_ID;
	m_idCampus = 0;
	m_nCampusPoint = 0;
	m_nRestPoint = 0;
	m_tLogOut = 0;
	m_nUsedSkillQueue = -1;
}

CMover::~CMover()
{
	SAFE_DELETE( m_pActMover );
}

void CMover::InitProp( void )
{
#ifdef __DBSERVER
	MoverProp* pMvrProp	= GetProp();
	if( pMvrProp )
	{
		if( FALSE == m_bPlayer )
			_tcscpy( m_szName, pMvrProp->szName );

		memset( m_aJobSkill, 0, sizeof(SKILL) * ( MAX_SKILL_JOB ) );

		LPSKILL lpSkill;
		for( int i = 0; i < MAX_SKILL_JOB; i++ )
		{
			m_aJobSkill[ i ].dwSkill = NULL_ID;
		}
		
		if( m_nJob != -1 ) 
		{
			ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
			int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
			for( int i = 0; i < nJobNum; i++ )
			{
				ItemProp* pSkillProp = apSkillProp[ i ];
				lpSkill = &m_aJobSkill[ i ];
				lpSkill->dwSkill = pSkillProp->dwID;
			}
		}
		m_nHitPoint		= 77; //GetMaxHitPoint();
		m_nManaPoint	= 77; //GetMaxManaPoint();
		m_nFatiguePoint	= 77; //GetMaxFatiguePoint();
		memset( m_szBankPass, 0, sizeof( m_szBankPass ) );
		m_dwBelligerence	= pMvrProp->dwBelligerence;
	}
#endif	// __DBSERVER
}
// BOOL CMover::AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist, int nResistAbilityOption, int nCharged, int nPiercedSize, DWORD adwItemId0, DWORD adwItemId1, DWORD adwItemId2, DWORD adwItemId3, DWORD adwItemId4, DWORD m_dwKeepTime, __int64 iRandomOptItemId )
// {
// 	if( iSerialNumber == 0 )
// 		iSerialNumber	= CSerialNumber::GetInstance()->Get();
// 	
// 	CItemElem item;
// 
// 
// 	return m_Inventory.Add( dwId, (short)( dwNum ), nOption, iSerialNumber, nItemResist, nResistAbilityOption, NULL, NULL, NULL, nCharged, nPiercedSize, adwItemId0, adwItemId1, adwItemId2, adwItemId3, adwItemId4, m_dwKeepTime, iRandomOptItemId );
// }

BOOL CMover::AddItemBank( int nSlot, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber )
{
	if( iSerialNumber == 0 )
		iSerialNumber	= CSerialNumber::GetInstance()->Get();

	CItemElem item;
	item.m_dwItemId = dwId;
	item.m_nItemNum = static_cast<short>(dwNum);
	item.m_nAbilityOption = nOption;
	item.SetSerialNumber(iSerialNumber);
	
	return m_Bank[nSlot].Add(&item);
}

BOOL CMover::IsAbnormalPlayerData()
{
#ifndef __CORESERVER
	DWORD dwId;
	int nMax	= m_Inventory.m_dwItemMax;
	int nNumberofIndex	= m_Inventory.m_dwIndexNum;
	
	for( int i = 0; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
		{
			if( m_Inventory.m_apItem[dwId].m_dwObjIndex != i )
				return TRUE;
		}
	}
	
	DWORD dwObjIndex;
	for( int i = 0; i < nMax; i++ )
	{
		dwObjIndex	= m_Inventory.m_apItem[i].m_dwObjIndex;
		if( dwObjIndex != NULL_ID )
		{
			if( m_Inventory.m_apIndex[dwObjIndex] != i )
				return TRUE;
		}
		if( FALSE == m_Inventory.m_apItem[i].IsEmpty() )
		{
			ItemProp* pItemProp
				= prj.GetItemProp( m_Inventory.m_apItem[i].m_dwItemId );
			if( !pItemProp	/*It have not property*/
				|| pItemProp->dwItemKind3 == IK3_VIRTUAL/*It is monster's virtual weapon*/ )
				return TRUE;
		}
	}
	
	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset( adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS );
	for( int i = nNumberofIndex; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
			adwEquipment[i - nNumberofIndex]	= dwId;
	}
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( adwEquipment[i] != m_aEquipInfo[i].dwId )
			return TRUE;
	}
#endif // __CORESERVER	
	return FALSE;
}

BOOL CMover::NormalizePlayerData()
{
	int nMax	= m_Inventory.m_dwItemMax;
	int nNumberofIndex	= m_Inventory.m_dwIndexNum;
	DWORD dwId;
	for( int i = 0; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
		{
			if( m_Inventory.m_apItem[dwId].m_dwObjIndex != i )
			{
				
				DWORD dwIndex	= NULL_ID;
				for( int j = 0; j < nMax; j++ )
				{	
					if( m_Inventory.m_apItem[j].m_dwObjIndex == i )
					{
						dwIndex	= j;
						break;
					}
				}
				if( dwIndex != NULL_ID )
				{
					m_Inventory.m_apIndex[i]	= dwIndex;
				}
				else
				{
					if( i >= nNumberofIndex )
					{
						m_Inventory.m_apIndex[i]	= NULL_ID;
					}
					else
					{
						int j = NULL;
						for( ; j < nMax; j++ )
						{
							if( m_Inventory.m_apItem[j].m_dwObjIndex == NULL_ID )
							{
								m_Inventory.m_apItem[j].m_dwObjIndex	= i;
								m_Inventory.m_apIndex[i]	= j;
								break;
							}
						}
						if( j == nMax ) 
						{
							ASSERT( 0 );
							return FALSE;
						}
					}
				}
				
			}
		}
	}
	
	DWORD dwObjIndex;
	for( int i = 0; i < nMax; i++ )
	{
		dwObjIndex	= m_Inventory.m_apItem[i].m_dwObjIndex;
		if( dwObjIndex != NULL_ID )
		{
			if( m_Inventory.m_apIndex[dwObjIndex] != i )
				m_Inventory.m_apItem[i].m_dwObjIndex	= NULL_ID;
		}
	}
	
	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset( adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS );
	for( int i = nNumberofIndex; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
			adwEquipment[i - nNumberofIndex]	= dwId;
	}
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( m_aEquipInfo[i].dwId != adwEquipment[i] )
			m_aEquipInfo[i].dwId	= adwEquipment[i];
	}
	return TRUE;
}
#if !defined(__CORESERVER)
// IK3로 삭제하기 예) IK3_CLOAK
BOOL CMover::RemoveItemIK3()
{
	int	nSize	= m_Inventory.m_dwItemMax;
	CItemElem* pItemElem;
	BOOL bRemove;
	BOOL bResult	= FALSE;
	for( int i = 0 ; i < nSize; i++ )
	{
		bRemove = FALSE;
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
		 	ItemProp* pItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
			if( pItemProp )
			{
				if( pItemProp->dwItemKind3 == IK3_CLOAK )
				{
					if( pItemElem->m_idGuild != 0 )
					{
						if( m_idGuild != pItemElem->m_idGuild )
						{
							bRemove = TRUE;
						}
					}
				}
				else
				if( pItemProp->dwItemKind3 == IK3_DELETE )
				{
					bRemove = TRUE;
				}

				if( bRemove )
				{
					if( m_Inventory.IsEquip( pItemElem->m_dwObjId ) )		
					{
						if( m_Inventory.UnEquip( pItemProp->dwParts ) ) // 장비 해제
						{
							m_aEquipInfo[pItemProp->dwParts].dwId	= NULL_ID;
							m_Inventory.RemoveAtId( pItemElem->m_dwObjId);
							bResult	= TRUE;
						}
						else
						{
							WriteLog( "Not UnEquip RemoveItemIK3  : %s, %d, %d", m_szName, pItemElem->m_dwObjId, pItemProp->dwID );
						}
					}
					else
					{
						m_Inventory.RemoveAtId( pItemElem->m_dwObjId);
						bResult	= TRUE;
					}
				}
			}
			else
			{
				WriteLog( "Not ItemProp RemoveItemIK3  : %s, %d ", m_szName, pItemElem->m_dwObjId );
			}
		}
	}
	return bResult;
}
#endif // !__CORESERVER

// bSize는 피어싱 사이즈를 늘릴 수 있는지 검사할 때 TRUE값을 setting 한다.
// bSize를 TRUE로 할 경우 dwTagetItemKind3는 NULL_ID로 한다.
BOOL CItemElem::IsPierceAble( DWORD dwTargetItemKind3, BOOL bSize ) const
{
	if( !GetProp() )
		return FALSE;

	int nPiercedSize = GetPiercingSize();
	if( bSize ) // 피어싱 사이즈를 늘리는 경우
		nPiercedSize++;

	if( GetProp()->dwItemKind3 == IK3_SUIT )
	{
		if( nPiercedSize <= MAX_PIERCING_SUIT )
		{
			if( dwTargetItemKind3 == NULL_ID )
				return TRUE;
			else if( dwTargetItemKind3 == IK3_SOCKETCARD )
				return TRUE;
		}
	}

	else if( GetProp()->dwItemKind3 == IK3_SHIELD
		|| GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT
		|| GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC
		)
	{
		if( nPiercedSize <= MAX_PIERCING_WEAPON )
		{
			if( dwTargetItemKind3 == NULL_ID )
				return TRUE;
			else if( dwTargetItemKind3 == IK3_SOCKETCARD2 )
				return TRUE;
		}
	}

	return FALSE;
}


CItemElem& CItemElem::operator =( CItemElem & ie )
{
	_tcscpy(m_szItemText, ie.m_szItemText);
	m_dwItemId = ie.m_dwItemId;
	m_liSerialNumber = ie.m_liSerialNumber;

	m_nItemNum				= ie.m_nItemNum;
	m_nRepairNumber			= ie.m_nRepairNumber;
	m_nHitPoint				= ie.m_nHitPoint;
	m_byFlag				= ie.m_byFlag;
	m_nAbilityOption		= ie.m_nAbilityOption;
	m_idGuild				= ie.m_idGuild;
	m_bItemResist			= ie.m_bItemResist;
	m_nResistAbilityOption	= ie.m_nResistAbilityOption;
	m_nResistSMItemId		= ie.m_nResistSMItemId;
	m_dwKeepTime			= ie.m_dwKeepTime;
	m_piercing	= ie.m_piercing;
	m_bCharged				= ie.m_bCharged;
	m_iRandomOptItemId		= ie.GetRandomOptItemId();
	m_nRepair				= ie.m_nRepair;

	SAFE_DELETE( m_pPet );
	if( ie.m_pPet )
	{
		m_pPet	= new CPet;
		*m_pPet	= *ie.m_pPet;
	}
	m_bTranformVisPet		= ie.m_bTranformVisPet;

	return *this;
}

BOOL CItemElem::IsInvalid( void )
{
	ItemProp* pProp	= GetProp();
	if( !pProp )
		return TRUE;
	// 알이 아닌 펫이 펫 정보가 없으면 제거 대상 이다.
	if( pProp->dwItemKind3 == IK3_EGG && m_dwItemId != II_PET_EGG )
	{
		if( m_pPet == NULL )
			return TRUE;
		if( m_pPet->GetLevel() == PL_EGG )
			return TRUE;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
void CMover::Copy( CMover * pMover, BOOL bAll )
{
	m_nSlot	= pMover->m_nSlot;
	m_dwIndex	= pMover->m_dwIndex;
	m_idPlayer	= pMover->m_idPlayer;
	m_dwAuthorization	= pMover->m_dwAuthorization;
	m_idGuild	= pMover->m_idGuild;
	m_idWar		= pMover->m_idWar;
	m_dwMode	= pMover->m_dwMode;
	lstrcpy( m_szBankPass, pMover->m_szBankPass );
	m_dwPeriodicTick	= pMover->m_dwPeriodicTick;
#ifdef __RT_1025
	m_RTMessenger	= pMover->m_RTMessenger;
#else	// __RT_1025
	m_Messenger		= pMover->m_Messenger;
#endif	// __RT_1025
#ifdef __BUFF_1107
	m_buffs	= pMover->m_buffs;
#endif	// __BUFF_1107
	m_dwPCBangClass = pMover->m_dwPCBangClass;
	m_idCampus	= pMover->m_idCampus;
	m_nCampusPoint	= pMover->m_nCampusPoint;

	if( bAll )
	{
		m_Inventory.Copy( pMover->m_Inventory );
		m_Pocket.Copy( pMover->m_Pocket );
		memcpy( m_aEquipInfo, pMover->m_aEquipInfo, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
		m_UserTaskBar = pMover->m_UserTaskBar;
		lstrcpy( m_szName, pMover->m_szName );
		m_vScale	= pMover->m_vScale;
		m_dwMotion	= pMover->m_dwMotion;
#ifdef __DBSERVER
		m_dwWorldID	= pMover->m_dwWorldID;
#endif	// __DBSERVER
#ifdef __LAYER_1015
		SetLayer( pMover->GetLayer() );
#endif	// __LAYER_1015
		m_vPos	= pMover->m_vPos;
		m_fAngle	= pMover->m_fAngle;
		lstrcpy( m_szCharacterKey, pMover->m_szCharacterKey );
		m_nHitPoint	= pMover->m_nHitPoint;
		m_nManaPoint	= pMover->m_nManaPoint;
		m_nFatiguePoint	= pMover->m_nFatiguePoint;
		m_dwSkinSet	= pMover->m_dwSkinSet;
		m_dwHairMesh	= pMover->m_dwHairMesh;
		m_dwHairColor	= pMover->m_dwHairColor;
		m_dwHeadMesh	= pMover->m_dwHeadMesh;
		SetSex( pMover->GetSex() );
		m_dwRideItemIdx	= pMover->m_dwRideItemIdx;
		SetGold( pMover->GetGold() );
		m_nJob	= pMover->m_nJob;
		m_idparty	= pMover->m_idparty;
		m_dwPKTime			= pMover->m_dwPKTime;
		m_nPKValue			= pMover->m_nPKValue;
		m_dwPKPropensity	= pMover->m_dwPKPropensity;
		m_dwPKExp			= pMover->m_dwPKExp;
		m_nAngelExp			= pMover->m_nAngelExp;
		m_nAngelLevel		= pMover->m_nAngelLevel;
		m_idMurderer	= pMover->m_idMurderer;
		m_nFame	= pMover->m_nFame;
		m_pActMover->m_dwState	= pMover->m_pActMover->GetState();
		m_pActMover->m_dwStateFlag	= pMover->m_pActMover->GetStateFlag();
		m_nStr	= pMover->m_nStr;
		m_nSta	= pMover->m_nSta;
		m_nDex	= pMover->m_nDex;
		m_nInt	= pMover->m_nInt;
		m_nLevel	= pMover->m_nLevel;
		m_nExp1	= pMover->m_nExp1;
		m_nFuel	= pMover->m_nFuel;
		m_tmAccFuel	= pMover->m_tmAccFuel;
		m_nSkillLevel	= pMover->m_nSkillLevel;
		m_nSkillPoint	= pMover->m_nSkillPoint;
		
		m_idMarkingWorld	= pMover->m_idMarkingWorld;
		m_vMarkingPos	= pMover->m_vMarkingPos;
		m_nRemainGP	= pMover->m_nRemainGP;
		m_nTutorialState	= pMover->m_nTutorialState;
		m_nFxp	= pMover->m_nFxp;
		m_bPlayer	= TRUE;
		m_nDeathExp	= pMover->m_nDeathExp;
		m_nDeathLevel = pMover->m_nDeathLevel;
		memcpy( m_aJobSkill, pMover->m_aJobSkill, sizeof(m_aJobSkill) );
		if (pMover->m_quests) {
			m_quests = std::make_unique<MoverSub::Quests>(*pMover->m_quests);
		} else {
			m_quests = nullptr;
		}

		memcpy( m_dwSMTime, pMover->m_dwSMTime, sizeof(m_dwSMTime) );
		m_nPlusMaxHitPoint	= pMover->m_nPlusMaxHitPoint;
		m_nAttackResistLeft	= pMover->m_nAttackResistLeft;
		m_nAttackResistRight	= pMover->m_nAttackResistRight;
		m_nDefenseResist	= pMover->m_nDefenseResist;
#ifndef __BUFF_1107
		memcpy( m_SkillState.m_aSkillInfluence, pMover->m_SkillState.m_aSkillInfluence, sizeof(m_SkillState.m_aSkillInfluence) );
#endif	// __BUFF_1107
		m_dwReturnWorldID	= pMover->m_dwReturnWorldID;
		m_vReturnPos	= pMover->m_vReturnPos;
		
		for( int i = 0; i < 3; i++ )
			m_Bank[i].Copy( pMover->m_Bank[i] );
		memcpy( m_idPlayerBank, pMover->m_idPlayerBank, sizeof(m_idPlayerBank) );
		memcpy( m_dwGoldBank, pMover->m_dwGoldBank, sizeof( m_dwGoldBank) );

#ifdef __DBSERVER
#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
		m_nEventFlag	= pMover->m_nEventFlag;
#else // __EVENT_1101_2
		m_dwEventFlag	= pMover->m_dwEventFlag;
#endif // __EVENT_1101_2
		m_dwEventTime	= pMover->m_dwEventTime;
		m_dwEventElapsed	= pMover->m_dwEventElapsed;
#endif	// __EVENT_1101
#endif	// __DBSERVER

		SetPetId( pMover->GetPetId() );
		m_fSpeedFactor	= pMover->m_fSpeedFactor;

#ifdef __EXP_ANGELEXP_LOG
		m_nExpLog = pMover->m_nExpLog;
		m_nAngelExpLog = pMover->m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
		m_nCoupon = pMover->m_nCoupon;
#endif // __EVENTLUA_COUPON

#ifdef __JEFF_9_20
		m_dwMute	= pMover->m_dwMute;
#endif	// __JEFF_9_20
		m_nHonor	= pMover->m_nHonor;
		memcpy( m_aHonorTitle, pMover->m_aHonorTitle, sizeof(int) * MAX_HONOR_TITLE );

	m_nRestPoint = pMover->m_nRestPoint;
	m_tLogOut = pMover->m_tLogOut;

	}
}

void CMover::PeriodTick( void )
{
	char ch	= m_dwPeriodicTick == 0 ? 'O': 'P';
	BOOL bPut	= ( ch != 'P' );
	DWORD dwTick	= GetTickCount();
	if( !bPut )
	{
		if( m_dwPeriodicTick < dwTick )
			bPut	= TRUE;
	}
	if( bPut )
		m_dwPeriodicTick	= dwTick + MIN( 120 );
}


BOOL CMover::RemoveQuest( QuestId nQuestId )
{
	if (!m_quests) return FALSE;

	BOOL	bRemove	= FALSE;

	const auto i1 = std::ranges::find_if(
		m_quests->current,
		MoverSub::Quests::ById(nQuestId)
	);
	if (i1 != m_quests->current.end()) {
		bRemove = TRUE;
		m_quests->current.erase(i1);
	}

	const auto i2 = std::ranges::find(
		m_quests->completed, nQuestId
	);
	if (i2 != m_quests->completed.end()) {
		bRemove = TRUE;
		m_quests->completed.erase(i2);
	}

	const auto i3 = std::ranges::find(
		m_quests->checked, nQuestId
	);
	if (i3 != m_quests->checked.end()) {
		bRemove = TRUE;
		m_quests->checked.erase(i3);
	}

	return bRemove;
}

CItemElem* CMover::GetPetItem( void )
{
	return m_Inventory.GetAtId( m_dwPetId );
}