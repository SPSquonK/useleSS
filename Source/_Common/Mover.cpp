#include "stdafx.h"
#include "..\_AIInterface\ActionMover.h"
#include "Mover.h"
#include "ModelObject.h"
#include "defineText.h"
#include "defineItem.h"
#include "defineObj.h"
#include "defineSound.h"
#include "defineSkill.h"
#include "lang.h"
#include "ModelObject.h"  
#include "authorization.h"
#include "CreateObj.h"
#include "eveschool.h"
#include <ranges>

#include "..\_aiinterface\aipet.h"

#include "pet.h"

#ifdef __WORLDSERVER
#include "slord.h"
#endif	// __WORLDSERVER

#ifdef __SYS_TICKET
#ifdef __WORLDSERVER
#include "ticket.h"
#endif	// __WORLDSERVER
#endif	// __SYS_TICKET

#include "randomoption.h"

#include "playerdata.h"

	#include "honor.h"

#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ

#ifdef __CLIENT
	#include "..\_Common\ParticleMng.h"
	#include "Resdata.h"
	#include "DPClient.h"
	#include "DialogMsg.h"
	#include "GuildRender.h"
#else	// __CLIENT
	#include "User.h"
	#include "UserMacro.h"
	#include "WorldMng.h"
	#include "DPSrvr.h"
	#include "DPCoreClient.h"
	#include "dpdatabaseclient.h"
	#include "spevent.h"
#endif	// __CLIENT

	#include "..\_AIInterface\AIInterface.h"

#include "party.h"
#ifdef __WORLDSERVER
#include "GroupUtils.h"
#endif

#include "guild.h"
#include "guildwar.h"

#include "definequest.h"
#include "Ship.h"
#ifdef __WORLDSERVER
	#include "User.h"
#endif

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER

#include "SecretRoom.h"

#include "CreateMonster.h"

extern	BOOL CanAdd( DWORD dwGold, int nPlus );

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __WORLDSERVER
	#ifdef __VM_0819
	CMoverPool*	CMover::m_pPool	= new CMoverPool( 1024, "CMover" );
	#else	// __VM_0819
	CMoverPool*	CMover::m_pPool	= new CMoverPool( 1024 );
	#endif	// __VM_0819
#else	// __WORLDSERVER
	#ifdef __VM_0819
	CMoverPool*	CMover::m_pPool	= new CMoverPool( 128, "CMover" );
	#else	// __VM_0819
	CMoverPool*	CMover::m_pPool	= new CMoverPool( 128 );
	#endif	// __VM_0819
#endif	// __WORLDSERVER
#endif	// __MEM_TRACE
#endif	// __VM_0820


const int	MAX_DIALOGFILENAME = 32;

#ifdef __CLIENT
// ?????? ?????? ???? ???????? 
BOOL CMover::m_bQuestEmoticonAdd = TRUE;
FLOAT CMover::m_fQuestEmoticonScale = 1.0f;

LPDIRECT3DTEXTURE9 CMover::m_pTextureEye[2][MAX_HEAD] = { 0, };
LPDIRECT3DTEXTURE9 CMover::m_pTextureEyeFlash[2][MAX_HEAD] = { 0, };

#endif

#include "collecting.h"
// ???????? ???? ???? ??????..

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning ( disable : 4355 )

CMover::CMover()
#ifdef __BUFF_1107
:
m_buffs( this )
#endif	// __BUFF_1107
{
	Init();
}

CMover::~CMover()  
{
#ifdef __CLIENT
	SAFE_DELETE( m_pLadolf );		// g_Object3DMng?? ???????????? ??????
	SAFE_DELETE( m_pAngel );
#ifdef __EVE_BALLOON
	SAFE_DELETE( m_pBalloon );
#endif //__EVE_BALLOON
#endif //__CLIENT

#ifdef __WORLDSERVER
	SAFE_DELETE( m_pNpcProperty );
#endif

#ifdef __WORLDSERVER
	CCreateMonster::GetInstance()->RemoveInfo( this );
#endif // __WORLDSERVER

	CMover* pOther = m_vtInfo.GetOther();
	if( IsValidObj( pOther ) )
	{
		if( pOther->m_vtInfo.GetOther() == this )
			pOther->m_vtInfo.TradeClear();
	}

#ifdef __CLIENT
	m_vtInfo.VendorClose();
	g_DialogMsg.ClearMessage( (CObj*)this );
#endif	// __CLIENT

	SAFE_DELETE( m_pActMover );

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		SAFE_DELETE( m_ShopInventory[ i ] );

	if( GetWorld() )
	{
	#if !defined(__CLIENT)
	#ifdef __LAYER_1021
		GetWorld()->m_respawner.Increment( GetRespawn(), GetRespawnType(), m_bActiveAttack, GetLayer() );
	#else	// __LAYER_1021
		GetWorld()->m_respawner.Increment( GetRespawn(), GetRespawnType(), m_bActiveAttack );
	#endif	// __LAYER_1021
	#endif
	}

#ifdef __CLIENT
	m_pSfxWing = NULL;
	m_pSfxBuffPet = NULL;

#ifdef __BS_EFFECT_LUA
	CSfxModelMng::GetThis()->SubData( GetId() );
#endif //__BS_EFFECT_LUA

#endif	// __CLIENT
}

void CMover::Init()
{
	m_dwMotion		    = -1;
	m_dwMotionOption    = 0;
	m_dwType			= OT_MOVER;
	m_vMarkingPos	    = D3DXVECTOR3( 0, 0, 0 );
	m_nCount			= xRandom( 128 );		// ?????? ???? ?????? ???????????? ?????? ?????? ???? ???? ?????? ?????? ?????? ????.
	m_bPlayer			= FALSE;
	m_dwTypeFlag		= 0;
	m_pRide				= NULL;  
	m_pAIInterface		= NULL;
	m_nHitPoint			= 100;
	m_nManaPoint		= 100;
	m_nFatiguePoint		= 100;
	m_vDestPos			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_idDest			= NULL_ID;
	m_fArrivalRange		= 0.0f;
	SetSex( SEX_FEMALE );
	m_dwVirtItem		= II_WEA_HAN_HAND;
	m_dwVirtType		= VT_ITEM;
	m_nHitPoint			= 100;
	m_nManaPoint		= 100;
	m_nFatiguePoint		= 100;
	m_dwRideItemIdx		= II_RID_RID_STI_MAGIC;
	m_nRemainGP			= 0;
	m_nDefenseMin		= 0;
	m_nDefenseMax		= 0;
	m_nJob				= -1;
	memset( m_szName, 0, sizeof(TCHAR) );
	m_nAdjHitRate		= 0;
	m_nAdjParry         = 0;
	m_nStr				= 15;
	m_nSta				= 15;
	m_nDex				= 15;
	m_nInt				= 15;
	m_nLevel			= 1;
	m_nExp1				= 0;
	m_nDeathExp			= 0;
	m_nDeathLevel		= 0;
	m_dwFace			= 0;
	m_dwSkinSet			= 0;
	m_dwHairMesh		= 0;
	m_dwHairColor		= 0;
	m_dwHeadMesh		= 0;
	m_nTutorialState	= 0;
	m_nFxp				= 0;
	m_szCharacterKey[ 0 ] = 0;
	{
		int i;
		for( i = 0; i < MAX_ADJPARAMARY; i ++ )		m_adjParamAry[i] = 0;
		for( i = 0; i < MAX_ADJPARAMARY; i ++ )		m_chgParamAry[i] = 0x7FFFFFFF;	// ?????? ????.
	}

	ClearEquipInfo();

	m_vtInfo.Init( this );

#ifdef __WORLDSERVER
	m_Pocket.Init( this );
#endif	// __WORLDSERVER

	m_idPlayer			= NULL_ID;
	m_dwAuthorization	= AUTH_GENERAL;
	m_idMarkingWorld	= NULL_ID;
	m_dwRegionAttr		= 0;
	m_dwOldRegionAttr	= 0;
	m_bPositiveX		= m_bPositiveZ	= false;
	m_fDestAngle		= -1.0f;
	m_bLeft				= false;
	m_bForward			= true;
	m_dwMode			= 0;
	memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	m_fWaitQueryGetPos	= FALSE;
	m_nCorr				= 0;
	m_uRemnantCorrFrm	= 0;
	m_fHairColorR		= 1.0f;
	m_fHairColorG		= 1.0f;
	m_fHairColorB		= 1.0f;
	m_dwHairColor		= D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f );
	m_quests = nullptr;
#ifdef __WORLDSERVER
	m_timerQuestLimitTime.Set( MIN( 1 ) );
	m_dwPKTargetLimit = 0;
#else // __WORLDSERVER
	m_nQuestEmoticonIndex = -1; 
	m_bShowQuestEmoticon = TRUE;
#endif // __WORLDSERVER
	m_idparty			= 0;

	m_oaCmd				= m_oaAct = OBJACT_NONE;
	memset( m_nCParam, 0, sizeof(m_nCParam) );
	memset( m_nAParam, 0, sizeof(m_nAParam) );
	m_dwFlag			= 0;
	m_nAtkCnt			= 0;
	m_wStunCnt			= 0;
#ifndef __BUFF_1107
	m_SkillState.Init();
	m_SkillState.SetMover( this );
#endif	// __BUFF_1107
	m_idAttacker		= NULL_ID;
	m_idTargeter		= NULL_ID;
	m_idTracking		= NULL_ID;
	m_fExpRatio			= 1.0f;
	m_idMurderer		= NULL_ID;
	m_tmActionPoint		= timeGetTime();
	m_nDead				= 0;
	m_dwPKTime			= 0;
	m_nPKValue			= 0;
	m_dwPKPropensity	= 0;
	m_dwPKExp			= 0;
	m_wDarkCover        = 0;

	m_idLastHitMover	= NULL_ID;			// this?? ?????????? ???? ??????????
	m_nFame				= 0;				// ????
	m_nDuel				= 0;				// ??????????.
	m_idDuelOther		= NULL_ID;
	m_idDuelParty		= 0;
	m_nFuel				= -1;				// ???????? -1????.
	m_tmAccFuel			= 0;				// 0???? ???? ?????? ????.
	m_fAniSpeed			= 1.0f;
	m_idPoisonAttacker	= NULL_ID;
	m_wPoisonCnt		= 0;
	m_tmPoisonUnit		= 0;
	m_wPoisonDamage		= 0;
	m_wDarkCnt			= 0;
	m_wDarkVal			= 0;
	m_idBleedingAttacker	= NULL_ID;
	m_wBleedingCnt		= 0;
	m_tmBleedingUnit	= 0;
	m_wBleedingDamage	= 0;
	m_nMovePattern		= 0;
	m_nMoveEvent		= 0;
	m_nMoveEventCnt		= 0;


	m_idGuild			= 0;
	m_idGuildCloak		= 0;
	m_bGuildBank		= FALSE;
	m_idWar				= WarIdNone;

	m_nPlusMaxHitPoint = 0;
	memset( m_dwSMTime, 0, sizeof( DWORD ) * SM_MAX );
	m_nAttackResistLeft = (BYTE)0xff;
	m_nAttackResistRight = (BYTE)0xff;
	m_nDefenseResist = (BYTE)0xff;

#ifdef __WORLDSERVER
	m_bLastPK		= FALSE;
	m_bLastDuelParty = FALSE;
	m_bGuildCombat = FALSE;
	m_tGuildMember	= CTime::GetCurrentTime();

	m_nQuestKeeping	= 0;			// ?????????? ???????? ???????? ???? 
	m_nPartyQuestKeeping	= 0;
	
//	m_idCollecter	= NULL_ID;
//	m_nResource		= 0;
//	m_nCollectOwnCnt = 0;
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_nDmgCnt			= 0;
	m_nWaterCircleCount = 0;
	m_idSfxHit		= 0;
	m_dwReqFlag		= 0;
	m_pCloakTexture = NULL;
	m_fDestScale	= 1.0f;
	m_fDestScaleSlerp = 0;
#else // CLIENT
	m_nReflexDmg = 0;
#endif	// not Client

	m_pActMover = new CActionMover( this );
	m_pActMover->SetMover( this );

	m_dwTickRecovery = ::timeGetTime() + NEXT_TICK_RECOVERY;
	m_dwTickRecoveryStand = ::timeGetTime() + NEXT_TICK_RECOVERYSTAND;
	m_dwTickDuel = ::timeGetTime() + NEXT_TICK_DUEL;
	m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;
	m_nDuelState = 0;
	
	m_Resurrection_Data = std::nullopt;

	m_bRegenItem		= TRUE;
	m_bActiveAttack		= FALSE;
	m_dwGold			= 0;		// 0???? ????.  -xuzhu-

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		m_ShopInventory[ i ] = 0;
	
	m_bPVPEnd	=	FALSE;

	m_nLegend	=	LEGEND_CLASS_NORMAL;
	memset( dwRemoveSfxObj, 0, sizeof( DWORD ) * MAX_SKILLBUFF_COUNT );

#ifdef	__CLIENT
	m_dwLadolfFlag = 0;
	m_pLadolf = NULL;
	m_pAngel = NULL;
	m_pAngelFlag = FALSE;
	m_dwAngelKind = 0;

#ifdef __EVE_BALLOON
	m_pBalloonFlag = FALSE;
	m_pBalloon = NULL;
	m_dwBalloonKind = 0;
	m_fBalloonYPos = 1.0f;
	m_bBalloon = TRUE;
#endif	//__EVE_BALLOON

	m_eWingStateFlag = FLOATING;
	m_fOldLengthSq = 0.0f;

#endif //__CLIENT
	m_fCrrSpd	= 0.0f;
	m_dwStateMode = 0;
	m_nReadyTime = 0;
	m_dwUseItemId = 0;
	m_bItemFind = TRUE;
	m_dwTickCheer= 0;
	m_nCheerPoint	= 0;
	m_pIADestPos = NULL;
	SetEatPetId( NULL_ID );

	m_tmReUseDelay.clear();

#ifdef __WORLDSERVER
	m_pNpcProperty = NULL;
	m_dwTickCreated	= 0;
#endif
	
	m_nGuildCombatState = 0;
	m_dwCtrlReadyTime   = 0xffffffff;
	m_dwCtrlReadyId     = NULL_ID;

	m_nSkillLevel = 0;
	m_nSkillPoint = 0;

	m_dwMotionArrive = OBJMSG_STAND;

	m_SkillTimerStop = FALSE;
	m_nAngelExp = 0;				/// ???? ??????
	m_nAngelLevel = 0;				/// ???? Level
	
#ifdef __CLIENT
	m_tmEye[0].Set( SEC(6) );
	m_tmEye[1].Set( 200 );
#endif //__CLIENT
	
#ifdef __CLIENT
	m_SkillTimer.Set(1000); //Function Key?????? ???? ???? Skill Delay??.
#endif //__CLIENT

	m_dwPetId	= NULL_ID;
	m_nHealCnt	= 0;
	m_fSpeedFactor	= 1.0F;
	
#ifdef __CLIENT
	m_pSfxWing = NULL;
	m_dwWingTime = -1;

	m_pSfxBuffPet = NULL;
#endif //__CLIENT

#ifdef __JEFF_9_20
	m_dwMute	= 0;
#endif	// __JEFF_9_20

	m_nHonor = -1;					// ???????? 
	m_dwHonorCheckTime = 0;			//???? ????????
	memset( m_aHonorTitle, 0, sizeof( int ) * MAX_HONOR_TITLE );// ????????
	m_strTitle.Empty();
#ifdef __VTN_TIMELIMIT
	m_nAccountPlayTime = -1;
#endif // __VTN_TIMELIMIT
	m_objIdVisPet = NULL_ID;
	m_dwMoverSfxId = NULL_ID;
	m_idCampus = 0;
	m_nCampusPoint = 0;
}

// AI ?????? ???? : ?????? ???? ????, ???? ?????? ???????? ?????? 
void CMover::SendAIMsg( DWORD dwMsg, DWORD dwParam1, DWORD dwParam2 )
{
	if( m_pAIInterface )
		m_pAIInterface->SendAIMsg( dwMsg, dwParam1, dwParam2 );
}
// AI ?????? ???? : ?????? ???? ????, ???? ?????? ???????? ?????? 
void CMover::PostAIMsg( DWORD dwMsg, DWORD dwParam1, DWORD dwParam2 )
{
	if( m_pAIInterface )
		m_pAIInterface->PostAIMsg( dwMsg, dwParam1, dwParam2 );
}

//raiders.2006.11.28	 trade???? ?????? ???????? ???? ????
BOOL CMover::AddGold( int nGold, BOOL bSend )
{
	if( nGold == 0 )
		return TRUE;

//#ifdef __WORLDSERVER
//	if( m_vtInfo.TradeGetGold() != 0 )
//		return FALSE;
//#endif	// __WORLDSERVER

#ifdef __PERIN_BUY_BUG
	__int64 i64Total = static_cast<__int64>( GetGold() ) + static_cast<__int64>( nGold );
	
	if( i64Total > static_cast<__int64>( INT_MAX ) )
		i64Total = static_cast<__int64>( INT_MAX );
	else if( i64Total < 0 )
		i64Total = 0;

	SetGold( static_cast<int>( i64Total ) );
#else // __PERIN_BUY_BUG
	int nTotal = GetGold() + nGold;

	if( nGold > 0 )
	{
		if( nTotal < 0 )		// overflow?
			nTotal = INT_MAX;
	}
	else	// nGold < 0
	{
		if( nTotal < 0 )		// underflow?
		{
//#ifdef __WORLDSERVER
//			Error( "AddGold: nTotal < 0" );
//#endif	// __WORLDSERVER
			nTotal	= 0;
//			return FALSE;
		}
	}

	SetGold( nTotal );
#endif // __PERIN_BUY_BUG
	
	if( bSend )
	{
	#ifdef __WORLDSERVER
#ifdef __PERIN_BUY_BUG
		g_UserMng.AddSetPointParam( this, DST_GOLD, static_cast<int>( i64Total ) );
#else // __PERIN_BUY_BUG
		g_UserMng.AddSetPointParam( this, DST_GOLD, nTotal );
#endif // __PERIN_BUY_BUG
	#endif	// __WORLDSERVER
	}

	return TRUE;
}

// ???????? ????.
void CMover::SetMovePattern( int nPattern )		
{
	m_nMovePattern = nPattern;
	m_nMoveEvent = 0;
	m_nMoveEventCnt = 0;
	ClearDest();		// ?????? ????.
}

// ?????? ?????????? ???????? ???? ???????? ??????????...
// ???? ?????????? ???????? ????...
int	CMover::GetHairCost( CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair )
{
	BYTE nOrignalR = (BYTE)( pMover->m_fHairColorR * 255 );
	BYTE nOrignalG = (BYTE)( pMover->m_fHairColorG * 255 );
	BYTE nOrignalB = (BYTE)( pMover->m_fHairColorB * 255 );

	int nHairCost = 0;
	int nHairColorCost = 0;
	
	if( nR != nOrignalR || nG != nOrignalG || nB != nOrignalB )
		nHairColorCost = HAIRCOLOR_COST;
	else
		nHairColorCost = 0;

	if( pMover->m_dwHairMesh+1 != nHair+1 )
		nHairCost = HAIR_COST;
	else
		nHairCost = 0;

	return (nHairCost + nHairColorCost);
}

#ifdef __WORLDSERVER
void CMover::SubSMMode()
{
	time_t tmCur = (time_t)( CTime::GetCurrentTime().GetTime() );
	for( int i = 0 ; i < SM_MAX ; ++i )
	{
		if( m_dwSMTime[i] > 0 )
		{
			if( g_AddSMMode.bSMModetime[i] )	// ????????
			{
				if( m_dwSMTime[i] < (DWORD)( tmCur ) )
				{
					m_dwSMTime[i] = 0;
				}
			}
			else								// ??????
			{
				if( i != SM_REVIVAL )
					--m_dwSMTime[i];	
			}

			if( m_dwSMTime[i] == 0 )
			{
				CItemElem* pItemElemParts = NULL;
				if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT )
				{
					int nAttackResist;
					if( i == SM_RESIST_ATTACK_LEFT )
						nAttackResist = m_nAttackResistLeft;
					else
						nAttackResist = m_nAttackResistRight;

					switch( nAttackResist )
					{
					case SAI79::FIRE:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_FIREASTONE;
						break;
					case SAI79::WATER:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_WATEILSTONE;
						break;
					case SAI79::WIND:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_WINDYOSTONE;
						break;
					case SAI79::ELECTRICITY:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_LIGHTINESTONE;
						break;
					case SAI79::EARTH:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_EARTHYSTONE;
						break;
					}
					if( i == SM_RESIST_ATTACK_LEFT )
					{
						m_nAttackResistLeft = (BYTE)0xff;
						pItemElemParts = m_Inventory.GetEquip( PARTS_LWEAPON );
					}
					else
					{
						m_nAttackResistRight = (BYTE)0xff;
						pItemElemParts = m_Inventory.GetEquip( PARTS_RWEAPON );
					}
					
				}
				else if( i == SM_RESIST_DEFENSE )
				{
					switch( m_nDefenseResist )
					{
					case SAI79::FIRE:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEFIREASTONE;
						break;
					case SAI79::WATER:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEWATEILSTONE;
						break;
					case SAI79::WIND:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEWINDYOSTONE;
						break;
					case SAI79::ELECTRICITY:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DELIGHTINESTONE;
						break;
					case SAI79::EARTH:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEEARTHYSTONE;
						break;
					}
					m_nDefenseResist = (BYTE)0xff;
					pItemElemParts = m_Inventory.GetEquip( PARTS_UPPER_BODY );
				}

				if( pItemElemParts && ( i == SM_RESIST_DEFENSE || i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT ) )
				{
					pItemElemParts->m_nResistSMItemId = 0;
					((CUser*)this)->AddCommercialElem( pItemElemParts->m_dwObjId, 0 );
				}

				ItemProp* aItemprop = prj.GetItemProp( g_AddSMMode.dwSMItemID[i] );
				if( aItemprop )
				{
					if( i == SM_MAX_HP )
					{
						ResetDestParam( aItemprop->dwDestParam[0], aItemprop->nAdjParamVal[1], 1 );
					}
					else if( i == SM_MAX_HP50 )
					{
						ResetDestParam( aItemprop->dwDestParam[0], m_nPlusMaxHitPoint, 1 );
					}
					else if( i == SM_VELOCIJUMP )
					{
						if( aItemprop->dwDestParam1 != -1 )
							ResetDestParam( aItemprop->dwDestParam1, aItemprop->nAdjParamVal1 );
						if( aItemprop->dwDestParam2 != -1 )
							ResetDestParam( aItemprop->dwDestParam2, aItemprop->nAdjParamVal2 );
					}
					// 090917 mirchang - ???????? ?????? ???? ???? ????
					else if( i == SM_PARTYSKILL30 || i == SM_PARTYSKILL15 || i == SM_PARTYSKILL1 )
						g_DPCoreClient.SendUserPartySkill( m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1 );
					
					g_dpDBClient.SendLogSMItemUse( "2", (CUser*)this, NULL, aItemprop );
				}
				else
				{
					WriteLog( "%s, %d\r\n\tNo SMItem dwSMItemID[%d] : %d / Name : %s", __FILE__, __LINE__, i, g_AddSMMode.dwSMItemID[i], m_szName );
				}

				((CUser*)this)->AddSMMode( i, 0 );

			}
		}
	}
}

void CMover::ClearAllSMMode()
{
	time_t tmCur = (time_t)( CTime::GetCurrentTime().GetTime() );
	for( int i = 0 ; i < SM_MAX ; ++i )
	{
		if( m_dwSMTime[i] > 0 )
		{
			m_dwSMTime[i] = 0;
			CItemElem* pItemElemParts = NULL;
			if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT )
			{
				int nAttackResist;
				if( i == SM_RESIST_ATTACK_LEFT )
					nAttackResist = m_nAttackResistLeft;
				else
					nAttackResist = m_nAttackResistRight;

				switch( nAttackResist )
				{
				case SAI79::FIRE:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_FIREASTONE;
					break;
				case SAI79::WATER:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_WATEILSTONE;
					break;
				case SAI79::WIND:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_WINDYOSTONE;
					break;
				case SAI79::ELECTRICITY:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_LIGHTINESTONE;
					break;
				case SAI79::EARTH:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_EARTHYSTONE;
					break;
				}
				if( i == SM_RESIST_ATTACK_LEFT )
				{
					m_nAttackResistLeft = (BYTE)0xff;
					pItemElemParts = m_Inventory.GetEquip( PARTS_LWEAPON );
				}
				else
				{
					m_nAttackResistRight = (BYTE)0xff;
					pItemElemParts = m_Inventory.GetEquip( PARTS_RWEAPON );
				}
					
			}
			else if( i == SM_RESIST_DEFENSE )
			{
				switch( m_nDefenseResist )
				{
				case SAI79::FIRE:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEFIREASTONE;
					break;
				case SAI79::WATER:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEWATEILSTONE;
					break;
				case SAI79::WIND:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEWINDYOSTONE;
					break;
				case SAI79::ELECTRICITY:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DELIGHTINESTONE;
					break;
				case SAI79::EARTH:
					g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEEARTHYSTONE;
					break;
				}
				m_nDefenseResist = (BYTE)0xff;
				pItemElemParts = m_Inventory.GetEquip( PARTS_UPPER_BODY );
			}

			if( pItemElemParts && ( i == SM_RESIST_DEFENSE || i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT ) )
			{
				pItemElemParts->m_nResistSMItemId = 0;
				((CUser*)this)->AddCommercialElem( pItemElemParts->m_dwObjId, 0 );
			}

			ItemProp* aItemprop = prj.GetItemProp( g_AddSMMode.dwSMItemID[i] );
			if( aItemprop )
			{
				if( i == SM_MAX_HP )
				{
					ResetDestParam( aItemprop->dwDestParam[0], aItemprop->nAdjParamVal[1], 1 );
				}
				else if( i == SM_MAX_HP50 )
				{
					ResetDestParam( aItemprop->dwDestParam[0], m_nPlusMaxHitPoint, 1 );
				}
				else if( i == SM_VELOCIJUMP )
				{
					if( aItemprop->dwDestParam1 != -1 )
						ResetDestParam( aItemprop->dwDestParam1, aItemprop->nAdjParamVal1 );
					if( aItemprop->dwDestParam2 != -1 )
						ResetDestParam( aItemprop->dwDestParam2, aItemprop->nAdjParamVal2 );
				}
				// 090917 mirchang - ???????? ?????? ???? ???? ????
				else if( i == SM_PARTYSKILL30 || i == SM_PARTYSKILL15 || i == SM_PARTYSKILL1 )
					g_DPCoreClient.SendUserPartySkill( m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1 );
				g_dpDBClient.SendLogSMItemUse( "2", (CUser*)this, NULL, aItemprop );
			}
			else
			{
				WriteLog( "%s, %d\r\n\tNo SMItem dwSMItemID[%d] : %d / Name : %s", __FILE__, __LINE__, i, g_AddSMMode.dwSMItemID[i], m_szName );
			}

			((CUser*)this)->AddSMMode( i, 0 );

		}
	}
}


BOOL CMover::SetSMMode( int nType, DWORD dwTime )
{
	if( g_AddSMMode.bSMModetime[nType] )	// ????????
	{
		CTime tRealtime	= CTime::GetCurrentTime();
		CTimeSpan ctp( 0, 0, 0, dwTime );
		tRealtime += ctp;
		m_dwSMTime[nType] = (time_t)( tRealtime.GetTime() );
	}
	else	// ??????
	{
		m_dwSMTime[nType] = dwTime;
	}

	((CUser*)this)->AddSMMode( nType, m_dwSMTime[nType] );
	
	return TRUE;
}

#endif // __WORLDSERVER

LPCHARACTER CMover::GetCharacter()
{
	return prj.GetCharacter( m_szCharacterKey );
}

#ifdef __WORLDSERVER
void CMover::SetMarkingPos()
{
	m_vMarkingPos = GetPos();
	m_idMarkingWorld	= GetWorld()->GetID();
}
#endif

// NPC???? ?????????? ??????????.
void CMover::InitNPCProperty()
{
#ifdef __WORLDSERVER
	if( m_pNpcProperty == NULL )
		m_pNpcProperty = new CNpcProperty;
#endif
}

#if defined(__WORLDSERVER)	
void CMover::InitCharacter( LPCHARACTER lpCharacter )
{
	if( lpCharacter )
	{
#ifdef __OPT_MEM_0811
		if( lpCharacter->m_nEquipNum > 0 )
			m_Inventory.SetItemContainer(CItemContainer::ContainerTypes::INVENTORY);
#endif	// __OPT_MEM_0811
		strcpy( m_szName, lpCharacter->m_strName );
		for( int i = 0; i < lpCharacter->m_nEquipNum; i++ )
		{
			BYTE nId, nCount;
			short nNum;
			CItemElem itemElem;
			itemElem.m_dwItemId	= lpCharacter->m_adwEquip[ i ];
			itemElem.m_nItemNum	= 1;
			m_Inventory.Add( &itemElem, &nId, &nNum, &nCount );
			CItemElem* pAddItem = m_Inventory.GetAtId( nId );
			m_Inventory.DoEquip( pAddItem->m_dwObjIndex, itemElem.GetProp()->dwParts );
		}

		m_dwHairMesh = lpCharacter->m_dwHairMesh;
		m_dwHairColor = lpCharacter->m_dwHairColor;
		m_dwHeadMesh = lpCharacter->m_dwHeadMesh;

		AllocShopInventory( lpCharacter );
		LoadDialog();		// npcproperty->LoadDialog()
	#ifdef __OUTPUT_INFO_0803
		prj.OutputStore( lpCharacter->m_szKey, this );
	#endif	// __OUTPUT_INFO_0803
	}
	m_nHitPoint = GetMaxHitPoint();
}

BOOL CMover::LoadDialog()
{
	if( m_pNpcProperty )
	{
		LPCHARACTER lpCharacter = GetCharacter();
		if( lpCharacter )
			return m_pNpcProperty->LoadDialog( lpCharacter );
	}

	return FALSE;
}
#endif // __WORLDSERVER



void CMover::ProcessQuest()
{
#ifdef __WORLDSERVER 
	if( IsPlayer() == FALSE ) 
		return;
	BOOL bTimer = FALSE;
	if( m_timerQuestLimitTime.IsTimeOut() )
	{
		m_timerQuestLimitTime.Reset();
		bTimer = TRUE;
	}
	if (!m_quests) return;

	for (QUEST & quest : m_quests->current) {
		const QuestProp * pQuestProp = quest.GetProp();
		if (!pQuestProp) {
			//WriteError( "ProcessQuest : pQuestProp NULL????." );
			//CString string;
			//string.Format( "CMover::ProcessQuest : %s?? Quest %d?? pQuestProp NULL????.", m_szName, lpQuest->m_wId );
			//OutputDebugString( string );
			continue;
		}

		const D3DXVECTOR3 vPos = GetPos();
		// ?????? ?????????? ???????? ?????? ???? ???? ???? ???? 
		if (pQuestProp->m_dwEndCondPatrolWorld == GetWorld()->m_dwWorldID && pQuestProp->m_rectEndCondPatrol.PtInRect(CPoint((int)vPos.x, (int)vPos.z))) {
			if (quest.m_bPatrol == FALSE) {
				quest.m_bPatrol = TRUE;
				// ?????? ?????? ???????? ???? ??????. 
				// ?????? ????????????.
				// 20 ?????? 2?????? ????????????.
				((CUser *)this)->AddSetQuest(&quest);
			}
		}
		// ???? ???? ?????? ???? ?????? 
		if (pQuestProp->m_nEndCondLimitTime && bTimer) {
			if (quest.m_wTime && !(quest.m_wTime & 0x8000)) {
				quest.m_wTime--;
				((CUser *)this)->AddSetQuest(&quest);
			}
		}
	}

#else // __WORLDSERVER
	if( IsPlayer()  ) 
		return;
	LPCHARACTER lpCharacter = GetCharacter();
	if( lpCharacter )
	{
		CMover* pMover = GetActiveMover();
		m_nQuestEmoticonIndex = -1;
		// NPC ?????? ???????? ???????? ???? - ???? > ???? > ???? > ??????
		for (const auto & [nQuest, nItem] : lpCharacter->m_srcQuests) {
			LPQUEST lpQuest = pMover->GetQuest( nQuest );

			// ?????? ???????? ????
			if( lpQuest == NULL && !pMover->IsCompleteQuest(nQuest) )
			{
				// ???? ?????? ???? ?????????
				if( __IsBeginQuestCondition( pMover, nQuest.get() ) && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 1;
				// ?????? ???????? ???? ???????? ???? ?? ???? ???????? ??????????..
				else if( m_nQuestEmoticonIndex != 1 && __IsNextLevelQuest( pMover, nQuest.get() ) && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 4;
			}
			// ???????? ???????? ????
			if( lpQuest && lpQuest->m_wId.GetProp() && !pMover->IsCompleteQuest(nQuest) && lpQuest->m_nState != QS_END )
			{
				if( m_nQuestEmoticonIndex != 1 && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 2;
			}
		}

		for (const auto & [nQuest, nItem] : lpCharacter->m_dstQuests) {
			LPQUEST lpQuest = pMover->GetQuest(nQuest);
			
			// ???????? ???? ???? ???? 
			if( lpQuest && lpQuest->GetProp() && !pMover->IsCompleteQuest(nQuest) && lpQuest->m_nState != QS_END)
			{
				// ???? ?????? ???? ?????????
				if( __IsEndQuestCondition( pMover, nQuest.get() ) && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 3;
				// ???? ?????? ???? ?????? ?????? ?????? ???????? ???????? ??????..
				else if( m_nQuestEmoticonIndex != 3 && m_nQuestEmoticonIndex != 1 && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 2;
			}
		}
	}
#endif // __WORLDSERVER
}

#ifdef __WORLDSERVER
void CMover::ProcessAbnormalState()
{
	if( m_wStunCnt > 0 )
	{
		if( --m_wStunCnt <= 0 )
			SetStun( FALSE );	// ???? ???? ????.
	}

	if( m_wDarkCover > 0 )
	{
		if( --m_wDarkCover <= 0 )
			SetDarkCover( FALSE );	// ???? ???? ????.
	}

	if( GetAdjParam( DST_CHRSTATE ) & CHS_POISON )
	{
		if( IsLive() )
		{
			int nUnitCnt = (int)( (m_tmPoisonUnit / 1000.0f) * PROCESS_COUNT );
			if( nUnitCnt <= 0 )		
			{
				nUnitCnt = 1;
				SetPoison( FALSE );
			} 
			else
			{
				if( (m_nCount % nUnitCnt) == 0 )		// xx?????? ?????? ??????.
				{
					CCtrl *pAttacker = prj.GetCtrl( m_idPoisonAttacker );
					if( IsValidObj( pAttacker ) && pAttacker->GetType() == OT_MOVER )
					{
						m_pActMover->SendDamage( AF_FORCE, 
												 pAttacker->GetId(), 
												 m_wPoisonDamage );
					}
				}

				if( m_wPoisonCnt > 0 )	// ?? ???? ????????.. SkillInfluence?? ?????? ???? 0????
					if( --m_wPoisonCnt <= 0 )	// ?? ???? ??????
						SetPoison( FALSE );		// ?????? ????.

			}
		}
	}
	
	if( m_wDarkCnt > 0 )	// ???? ????????????...
	{
		if( --m_wDarkCnt <= 0 )
			SetDark( FALSE );
	}

	if( GetAdjParam( DST_CHRSTATE ) & CHS_BLEEDING )
	{
		if( IsLive() )
		{
			int nUnitCnt = (int)( (m_tmBleedingUnit / 1000.0f) * PROCESS_COUNT );
			if( nUnitCnt <= 0 )		
			{
				nUnitCnt = 1;
				SetBleeding( FALSE );
			} 
			else
			{
				if( (m_nCount % nUnitCnt) == 0 )		// xx?????? ?????? ??????.
				{
					CCtrl *pAttacker = prj.GetCtrl( m_idBleedingAttacker );
					if( IsValidObj( pAttacker ) && pAttacker->GetType() == OT_MOVER )
					{
						m_pActMover->SendDamage( AF_FORCE, 
												 pAttacker->GetId(), 
												 m_wBleedingDamage );
					}
				}

				if( m_wBleedingCnt > 0 )	// ???? ???? ????????..
					if( --m_wBleedingCnt <= 0 )	// ?? ???? ??????
						SetBleeding( FALSE );		// ?????? ????.
			}
		}
	}
	int nHeal	= GetAdjParam( DST_HEAL );
	if( nHeal > 0 )	// ???? ???? ??????,
	{
		if( m_nHealCnt > 0 )
			m_nHealCnt--;

		if( m_nHealCnt == 0 )
		{
			// HP?? 30% ?????? ???????? ??, ?????? ???????? HP?? ????????. ?????? 6??
//			if( IsLive() && GetHitPointPercent( 100 ) <= 30 )
			if( IsLive() )
			{
				m_nHealCnt	= (short)( PROCESS_COUNT * 6.0f );		// ?????? 6??
				SetPointParam( DST_HP, nHeal );
			}
		}
	}
}

// ???????? ???? ?????????? idAttacker, m_idTargeter?? ???????? ???????????? ??????.
void CMover::ProcessTarget()
{
	if( m_idAttacker != NULL_ID || m_idTargeter != NULL_ID )		
	{
		CMover *pAttacker = GETMOVER( m_idAttacker );
		if( IsValidObj( pAttacker ) )
		{
			if( !IsValidArea( pAttacker, 32.0f ) )
			{
				m_idAttacker = NULL_ID;
			}
		} 
		else
		{
			m_idAttacker = NULL_ID;		// ???????? ???? ?????????? 
		}

		CMover *pTargeter = GETMOVER( m_idTargeter );
		if( IsValidObj( pTargeter ) )
		{
			if( !IsValidArea( pTargeter, 32.0f ) )
			{
				m_idTargeter = NULL_ID;
			}
		} 
		else
		{
			m_idTargeter = NULL_ID;		// ???????? ???? ?????????? ??????.
		}
	}
}

void CMover::ProcessScript()
{ 
	if( IsPlayer() ) 
		return;

	if( m_szCharacterKey[ 0 ] && m_pNpcProperty->IsTimeOut() ) 
	{
		m_pNpcProperty->ResetTimer();
		m_pNpcProperty->RunDialog("#auto",NULL,0,GetId(),GetId(),0);
	}
}

void CMover::ProcessRegenItem()
{
	if( IsNPC() )
	{
		if( m_bRegenItem )
		{

			m_bRegenItem = FALSE;
			//
			// vender item ???? 
			//
			//#define VENDER_DELAY (60 * 5) // 5?? 
			LPCHARACTER pCharacter = GetCharacter();
			BOOL fShop	= FALSE;
			if( !pCharacter )
				return;

			for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i ++ )
			{
				if(pCharacter->m_vendor.m_type == CVendor::Type::RedChip) // ?????? ???????? vender?? ????
				{
					if(!pCharacter->m_vendor.m_venderItemAry2[i].empty())
					{
						fShop	= TRUE;
						m_ShopInventory[i]->Clear();		// m_pack?? ?? ??????.
						for (const auto & pVendor : pCharacter->m_vendor.m_venderItemAry2[i]) {
							CItemElem itemElem;
							itemElem.m_dwItemId	= pVendor.m_dwItemId;
							itemElem.m_nItemNum	= (short)( prj.GetItemProp( pVendor.m_dwItemId )->dwPackMax );
							itemElem.m_nHitPoint = prj.GetItemProp( pVendor.m_dwItemId )->dwEndurance;
							if( (int)itemElem.GetChipCost() < 1 )
								Error( "chip cost < 1 : npc = %s, item = %d", pCharacter->m_szKey, pVendor.m_dwItemId );
							else
								m_ShopInventory[i]->Add( &itemElem );
						}
					}
				}
				else
				{
					if( !pCharacter->m_vendor.m_venderItemAry[i].empty() )
					{
						fShop	= TRUE;
						{
							m_ShopInventory[i]->Clear();		// m_pack?? ?? ??????.

							ItemProp* apItemProp[MAX_VENDOR_INVENTORY];
							int cbSize	= 0;
							// generate
							for (const auto & pVendor : pCharacter->m_vendor.m_venderItemAry[i]) {
								GenerateVendorItem( apItemProp, &cbSize, MAX_VENDOR_INVENTORY, pVendor );
							}
							// sort
							for( int j = 0; j < cbSize - 1; j++ )
							{
								for( int k = j + 1; k < cbSize; k++ )
								{
									if( ( apItemProp[k]->dwItemKind1 < apItemProp[j]->dwItemKind1 ) ||
										( apItemProp[k]->dwItemKind1 == apItemProp[j]->dwItemKind1 && apItemProp[k]->dwItemRare < apItemProp[j]->dwItemRare ) )
									{
										ItemProp* ptmp	= apItemProp[j];
										apItemProp[j]	= apItemProp[k];
										apItemProp[k]	= ptmp;
									}
								}
							}
							// add
							for( int j = 0; j < cbSize; j++ )
							{
								CItemElem itemElem;
								itemElem.m_dwItemId	= apItemProp[j]->dwID;
								itemElem.m_nItemNum		= (short)( apItemProp[j]->dwPackMax );
								itemElem.m_nHitPoint	= apItemProp[j]->dwEndurance;
								if( m_ShopInventory[i]->Add( &itemElem ) == FALSE )
									break;
							}
						}
					}
				}
			}

			if( fShop )
				g_UserMng.AddVendor( this );

		}
	}
}

#endif	// __WORLDSERVER

CModel* CMover::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex )
{
	if( m_dwIndex == MI_FEMALE || m_dwIndex == MI_MALE )
		return prj.m_modelMng.LoadModel( pd3dDevice, dwType, dwIndex, TRUE );
	return prj.m_modelMng.LoadModel( pd3dDevice, dwType, dwIndex );
}



// ?? ?????? ???????? ???? ???????? ?????? ???? ???????? ??????????.
void CMover::InitProp( BOOL bInitAI )
{
	MoverProp* pProp = GetProp();
	if( pProp == NULL )
	{  
		Error( "CMover::InitProp - GetProp(%d) return NULL \n", GetIndex() );
		ASSERT( 0 );
		return;
	}

	if( m_szName[0] == '\0' )
		_tcscpy( m_szName, pProp->szName );

#ifdef __WORLDSERVER
	if( bInitAI )
		SetAIInterface( pProp->dwAI );
#else
	UNUSED_ALWAYS( bInitAI );
#endif	

	m_dwBelligerence = pProp->dwBelligerence;

	if( IsPlayer() )
	{
		m_Inventory.SetItemContainer( CItemContainer::ContainerTypes::INVENTORY );
		m_Pocket.Avail( 0 );
	}

	m_nSlot = 0;
	m_bBank = FALSE;
	for( int k = 0 ; k < 3 ; ++k )
	{
		if( IsPlayer() )
			m_Bank[k].SetItemContainer(CItemContainer::ContainerTypes::BANK) ;
		m_dwGoldBank[k] = 0;
		m_idPlayerBank[k] = 0;
	}
	ZeroMemory( m_ShopInventory, sizeof( m_ShopInventory ) );	
	
	m_jobSkills.clear();
	m_tmReUseDelay.clear();

	// ?????? ?????? 
	if (m_nJob != -1) {
		for (const ItemProp * pSkillProp : prj.jobs.skills[m_nJob]) {
			m_jobSkills.emplace_back(SKILL{ .dwSkill = pSkillProp->dwID, .dwLevel = 0 });
		}
	}

	// ???? ?????? ?????? 
	m_nLevel = pProp->dwLevel;
	if( m_nLevel < 1 ) 
		m_nLevel = 1;

	if( IsPlayer() )
	{
		if( m_nJob != -1 )
		{
			m_nHitPoint     = GetMaxHitPoint();
			m_nManaPoint    = GetMaxManaPoint();
			m_nFatiguePoint = GetMaxFatiguePoint();
		}
		else
		{
			m_nHitPoint     = 1;
			m_nManaPoint    = 1;
			m_nFatiguePoint = 1;
		}

		m_pActMover->m_fSpeed = pProp->fSpeed;
	}
	else
	{
		m_nStr = pProp->dwStr; 
		m_nSta = pProp->dwSta;
		m_nDex = pProp->dwDex;
		m_nInt = pProp->dwInt;

		m_nHitPoint     = GetMaxHitPoint();
		m_nManaPoint    = pProp->dwAddMp;
		m_nFatiguePoint = GetMaxFatiguePoint();
	}

	UpdateParam();
	m_dwVirtItem = pProp->dwAtk1;	// dwVirtItem?? ?????? dwAtk1???? ????.

	if( m_bPlayer == FALSE && pProp->dwFlying == 1 )	// ?????? ???????????
		m_dwTypeFlag |= OBJTYPE_FLYING;

//#ifdef __WORLDSERVER
//	m_nResource = pProp->dwMaterialAmount;		// ??????.
//#endif

	if (IsPlayer()) {
		m_quests = std::make_unique<MoverSub::Quests>();
	} else {
		m_quests = nullptr;
	}
}

#ifdef __WORLDSERVER
void CMover::InitLevel( int nJob, LONG nLevel ) {
	// ?????? ???????? ?????? ????????
	
	// ?????? ???? ?? ?????? ????.
	
	// 1. ?????? ?????? ???? ????
	m_jobSkills = MoverSkills::ForJob(nJob);

	// 2. ???? ?????? ?????? 
	m_nLevel	= 1;
	m_nStr	= m_nSta	= m_nDex	= m_nInt	= 15;
	m_nRemainGP = 0;
	m_nSkillLevel = 0;
	m_nSkillPoint = 0;	
			
	// 3. LP, GP, ????
	for( int i = 1 ; i < nLevel ; i++ )
	{
		m_nLevel	= i + 1;
		m_nRemainGP += prj.m_aExpCharacter[ m_nLevel ].dwLPPoint;
		if( nJob >= MAX_PROFESSIONAL && i > 59 )
			m_nRemainGP++;
	}

	if (nJob >= MAX_MASTER) { m_nRemainGP += 12; }

	m_nJob = nJob;

	int nPoint = 0;
	if( m_nLevel <= 20 )
		nPoint = 2 * m_nLevel - 2;	
	else if( m_nLevel <= 40 )
		nPoint = 3 * m_nLevel - 22;	
	else if( m_nLevel <= 60 )
		nPoint = 4 * m_nLevel - 62;	
	else if( m_nLevel <= 80 )
		nPoint = 5 * m_nLevel - 122;	
	else if( m_nLevel <= 100 )
		nPoint = 6 * m_nLevel - 202;	
	else
		nPoint = 7 * m_nLevel - 302;		

	if( m_nJob == JOB_MERCENARY )
		nPoint += 40;
	else if( m_nJob == JOB_ACROBAT )
		nPoint += 50;
	else if( m_nJob == JOB_ASSIST )
		nPoint += 60;
	else if( m_nJob == JOB_MAGICIAN )
		nPoint += 90;
	else if( m_nJob ==  JOB_KNIGHT || m_nJob ==  JOB_BLADE )
		nPoint += 120;
	else if( m_nJob ==  JOB_JESTER || m_nJob ==  JOB_RANGER )
		nPoint += 150;
	else if( m_nJob ==  JOB_RINGMASTER )
		nPoint += 160;
	else if( m_nJob ==  JOB_BILLPOSTER || m_nJob ==  JOB_PSYCHIKEEPER )
		nPoint += 180;
	else if( m_nJob ==  JOB_ELEMENTOR )
		nPoint += 390;
	m_nSkillLevel = m_nSkillPoint = nPoint;

	m_nDeathLevel = nLevel;

	const DWORD masterSkillLevel =
		nLevel <  72 ? 1 :
		nLevel <  84 ? 2 :
		nLevel <  96 ? 3 :
		nLevel < 108 ? 4 : 5;

	for (SKILL & skill : m_jobSkills) {
		const ItemProp * skillProp = skill.GetProp();
		if (skillProp && skillProp->dwItemKind1 == JTYPE_MASTER) {
			skill.dwLevel = masterSkillLevel;
		}
	}

	m_nExp1 = 0;
		
	if( m_nLevel >= 20 )
		SetFlightLv( 1 );
	else
		SetFlightLv( 0 );
		
	( (CUser*)this )->AddSetChangeJob( nJob );
	g_UserMng.AddNearSetChangeJob(this, nJob);

	g_dpDBClient.SendUpdatePlayerData( (CUser*)this );
	SetHitPoint( GetMaxHitPoint() );
	SetManaPoint( GetMaxManaPoint() );
	SetFatiguePoint( GetMaxFatiguePoint() );
	
	g_UserMng.AddSetLevel( this, (WORD)m_nLevel );
	CUser * self = static_cast<CUser *>(this);
	self->AddSetGrowthLearningPoint( m_nRemainGP );
	self->AddSetExperience( GetExp1(), (WORD)m_nLevel, m_nSkillPoint, m_nSkillLevel );
	self->m_playTaskBar.RemoveAll(ShortcutType::Skill);
	self->AddTaskBar();
	self->AddSetState( m_nStr, m_nSta, m_nDex, m_nInt, m_nRemainGP );
	self->CheckHonorStat();
	self->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( this, m_nHonor);
}
#endif // __WORLDSERVER

int   CMover::SetLevel( int nSetLevel )
{
#ifdef __WORLDSERVER
	m_nLevel = nSetLevel;
	m_nExp1 = 0;
	m_nDeathExp = GetExp1();
	m_nDeathLevel = nSetLevel - 1;
	( (CUser *)this )->AddSetExperience( GetExp1(), (WORD)m_nLevel,  m_nSkillPoint, m_nSkillLevel, m_nDeathExp, (WORD)m_nDeathLevel );		// ???????????? exp1,exp2?????? ?????? ????.
	g_UserMng.AddSetLevel( this, (WORD)m_nLevel );
	g_dpDBClient.SendLogLevelUp( this, 1 );	// ?????? ????
#endif
	return 0; 
}

int   CMover::AddGPPoint( int nAddGPPoint )
{
#ifdef __WORLDSERVER
	m_nRemainGP += nAddGPPoint;
	( (CUser*)this )->AddSetGrowthLearningPoint( m_nRemainGP );		// pUser???? GP?????????? ????.
	g_dpDBClient.SendLogLevelUp( this, 1 );	// ?????? ????
#endif
	return 0; 
}


BOOL CMover::InitSkillExp() {
	m_nSkillPoint = GetCurrentMaxSkillPoint();

	for (SKILL & skill : m_jobSkills) {
		const ItemProp * pSkillProp = skill.GetProp();
		if (!pSkillProp) return FALSE;

		if (pSkillProp->dwItemKind1 == JTYPE_MASTER
			|| pSkillProp->dwItemKind1 == JTYPE_HERO) {
			continue;
		}

		skill.dwLevel = 0;
	}

#ifdef __WORLDSERVER
	( (CUser*)this )->m_playTaskBar.RemoveAll(ShortcutType::Skill);
	( (CUser*)this )->AddTaskBar();
#endif //__WORLDSERVER
	return TRUE;
}

int CMover::GetCurrentMaxSkillPoint() const {
	int nCurrentMaxSkillPoint = m_nSkillPoint;
	for (const SKILL & skill : m_jobSkills) {
		const ItemProp * pSkillProp = skill.GetProp();
		if (!pSkillProp) continue;

		if (skill.dwLevel == 0) continue;

		if (pSkillProp->dwItemKind1 == JTYPE_MASTER
			|| pSkillProp->dwItemKind1 == JTYPE_HERO) {
			continue;
		}

		nCurrentMaxSkillPoint += skill.dwLevel * prj.GetSkillPoint(pSkillProp);
	}

	int nMaxPoint = 1280;	// ?????? JOB_ELEMENTOR ???? ??????
	if( m_nJob == JOB_KNIGHT || m_nJob == JOB_BLADE || m_nJob == JOB_KNIGHT_MASTER || m_nJob == JOB_BLADE_MASTER || m_nJob == JOB_KNIGHT_HERO || m_nJob == JOB_BLADE_HERO )
		nMaxPoint = 870;
	else if( m_nJob == JOB_JESTER || m_nJob == JOB_RANGER || m_nJob == JOB_JESTER_MASTER || m_nJob == JOB_RANGER_MASTER || m_nJob == JOB_JESTER_HERO || m_nJob == JOB_RANGER_HERO )
		nMaxPoint = 910;
	else if( m_nJob == JOB_RINGMASTER || m_nJob == JOB_RINGMASTER_MASTER || m_nJob == JOB_RINGMASTER_HERO )
		nMaxPoint = 902;
	else if( m_nJob == JOB_BILLPOSTER || m_nJob == JOB_PSYCHIKEEPER || m_nJob == JOB_BILLPOSTER_MASTER || m_nJob == JOB_PSYCHIKEEPER_MASTER || m_nJob == JOB_BILLPOSTER_HERO || m_nJob == JOB_PSYCHIKEEPER_HERO )
		nMaxPoint = 950;

	if( nCurrentMaxSkillPoint > nMaxPoint)
		nCurrentMaxSkillPoint = nMaxPoint;

	return nCurrentMaxSkillPoint;
}

#ifdef __CLIENT
//?????? ?????? 
void CMover::SetSfxBuffPet( const DWORD idEffect )
{
	if( m_pSfxBuffPet )
	{
		g_WorldMng.Get()->RemoveObj( m_pSfxBuffPet );
		m_pSfxBuffPet = NULL;
	}

	if( NULL_ID != idEffect )
		m_pSfxBuffPet =  CreateSfx( g_Neuz.m_pd3dDevice, idEffect, GetPos(), GetId(), GetPos(), GetId(), -1 );
}

#endif //__CLIENT
void CMover::ReState()
{
#ifdef __WORLDSERVER
	m_nStr	= m_nSta	= m_nDex	= m_nInt	= 15;
	m_nRemainGP = 0;

	int nLevelFor = m_nLevel;
	if( m_nLevel < m_nDeathLevel )
		nLevelFor = m_nDeathLevel;
	for( int i = 1 ; i < nLevelFor ; i++ )
	{
		m_nRemainGP += prj.m_aExpCharacter[ i + 1 ].dwLPPoint;
		if( IsJobTypeOrBetter(JTYPE_MASTER) && i > 59)
			m_nRemainGP++;
		if(IsJobTypeOrBetter(JTYPE_HERO) && i == MAX_LEVEL)
			m_nRemainGP+=12;
	}
	
	SetHitPoint( GetMaxHitPoint() );
	SetManaPoint( GetMaxManaPoint() );
	SetFatiguePoint( GetMaxFatiguePoint() );
	((CUser*)this)->AddSetState( m_nStr, m_nSta, m_nDex, m_nInt, m_nRemainGP );
	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( this, m_nHonor);
#endif // __WORLDSERVER
}

void CMover::ReStateOne( int nKind )
{
#ifdef __WORLDSERVER
	int nAdd = 0;
	switch( nKind )
	{
	case 0:		// Str
		{
			nAdd = m_nStr - 15;
			m_nStr = 15;
		}
		break;
	case 1:		// Sta
		{
			nAdd = m_nSta - 15;
			m_nSta = 15;
		}
		break;
	case 2:		// Dex
		{
			nAdd = m_nDex - 15;
			m_nDex = 15;
		}
		break;
	case 3:		// Int
		{
			nAdd = m_nInt - 15;
			m_nInt = 15;
		}
		break;
	}
	m_nRemainGP += nAdd;

	SetHitPoint( GetMaxHitPoint() );
	SetManaPoint( GetMaxManaPoint() );
	SetFatiguePoint( GetMaxFatiguePoint() );
	((CUser*)this)->AddSetState( m_nStr, m_nSta, m_nDex, m_nInt, m_nRemainGP );
	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( this, m_nHonor);
#endif // __WORLDSERVER
}

#ifdef __ADD_RESTATE_LOW
void CMover::ReStateOneLow( int nKind )
{
#ifdef __WORLDSERVER
	int nAdd = 0;
	switch( nKind )
	{
	case 0:		// Str
		{
			if( m_nStr >= RESTATE_LOW + 15 )
			{
				nAdd = RESTATE_LOW;
				m_nStr -= RESTATE_LOW;
			}
			else
			{
				nAdd = m_nStr - 15;
				m_nStr = 15;
			}
		}
		break;
	case 1:		// Sta
		{
			if( m_nSta >= RESTATE_LOW + 15 )
			{
				nAdd = RESTATE_LOW;
				m_nSta -= RESTATE_LOW;
			}
			else
			{
				nAdd = m_nSta - 15;
				m_nSta = 15;
			}
		}
		break;
	case 2:		// Dex
		{
			if( m_nDex >= RESTATE_LOW + 15 )
			{
				nAdd = RESTATE_LOW;
				m_nDex -= RESTATE_LOW;
			}
			else
			{
				nAdd = m_nDex - 15;
				m_nDex = 15;
			}
		}
		break;
	case 3:		// Int
		{
			if( m_nInt >= RESTATE_LOW + 15 )
			{
				nAdd = RESTATE_LOW;
				m_nInt -= RESTATE_LOW;
			}
			else
			{
				nAdd = m_nInt - 15;
				m_nInt = 15;
			}
		}
		break;
	}
	m_nRemainGP += nAdd;

	SetHitPoint( GetMaxHitPoint() );
	SetManaPoint( GetMaxManaPoint() );
	SetFatiguePoint( GetMaxFatiguePoint() );
	((CUser*)this)->AddSetState( m_nStr, m_nSta, m_nDex, m_nInt, m_nRemainGP );
	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( this, m_nHonor);
#endif // __WORLDSERVER
}
#endif // __ADD_RESTATE_LOW

#ifdef __WORLDSERVER

BOOL CMover::ReplaceInspection( REGIONELEM* pPortkey )
{
	BOOL bResult = TRUE;
	if( bResult != FALSE && pPortkey->m_uItemId != 0xffffffff )
	{
		int nCount = GetItemNum( pPortkey->m_uItemId );
		if( (DWORD)( nCount ) <= pPortkey->m_uiItemCount )
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}

		TRACE( "ReplaceInspection Item %d Count %d/%d Inspection %d \n", pPortkey->m_uItemId, nCount, pPortkey->m_uiItemCount, bResult );
	}
	if( bResult != FALSE && pPortkey->m_uiMinLevel != 0xffffffff )
	{
		if( pPortkey->m_uiMinLevel <= (UINT)( m_nLevel ) && (UINT)( m_nLevel ) <= pPortkey->m_uiMaxLevel)
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}

		TRACE( "ReplaceInspection Level (%d~%d) %d Inspection %d \n", pPortkey->m_uiMinLevel, pPortkey->m_uiMaxLevel, m_nLevel, bResult );
	}
	if( bResult != FALSE && pPortkey->m_iQuest != 0xffffffff )
	{
		LPQUEST pCurQuest = GetQuest( QuestId(pPortkey->m_iQuest) );
		if( pCurQuest && ( pPortkey->m_iQuestFlag == 0 || pPortkey->m_iQuestFlag == pCurQuest->m_nState ) )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Quest %d Flag %d/%d Inspection %d \n", pPortkey->m_iQuest, pPortkey->m_iQuestFlag, pCurQuest->m_nState, bResult );
	}
	if( bResult != FALSE && pPortkey->m_iJob != 0xffffffff )
	{
		if( pPortkey->m_iJob == m_nJob )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Job %d/%d Inspection %d \n", pPortkey->m_iJob, m_nJob, bResult );
	}
	if( bResult != FALSE && pPortkey->m_iGender != 0xffffffff )
	{
		if( pPortkey->m_iGender == GetSex() )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Gender %d/%d Inspection %d \n", pPortkey->m_iGender, GetSex(), bResult );
	}
	if( bResult != FALSE && pPortkey->m_bCheckParty != FALSE )
	{
		CParty* pParty	= g_PartyMng.GetParty( m_idparty );
		if( pParty != NULL && pParty->IsMember( m_idPlayer ) )
			bResult = TRUE;
		else 
			bResult = FALSE;
		TRACE( "ReplaceInspection Party Inspection %d \n", bResult );
	}
	if( bResult != FALSE && pPortkey->m_bCheckGuild != FALSE )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
		if( pGuild && pGuild->IsMember( m_idPlayer ) )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Guild Inspection %d \n", bResult );
	}	

	return bResult;
}

bool CMover::Replace(const CMover & pTarget, REPLACE_TYPE replaceType) {
	CWorld * world = pTarget.GetWorld();
	if (!world) return false;

	Replace(
		g_uIdofMulti, world->GetID(), pTarget.GetPos(),
		replaceType, pTarget.GetLayer()
	);
	return true;
}

// default type =  REPLACE_NORMAL
#ifdef __LAYER_1015
BOOL CMover::Replace( u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3 & vPos, REPLACE_TYPE type, int nLayer  )
#else	// __LAYER_1015
BOOL CMover::Replace( u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3 & vPos, REPLACE_TYPE type  )
#endif	// __LAYER_1015

{
	CWorld* pWorld	= GetWorld();
	if( !pWorld )
		return FALSE;
	if( dwWorldID == 0 || dwWorldID == NULL_ID )
		return FALSE;

#ifdef __LAYER_1015
	CWorld* pTargetWorld	= g_WorldMng.GetWorld( dwWorldID );
	if( !pTargetWorld || !pTargetWorld->m_linkMap.GetLinkMap( nLayer ) )
	{
		Error( "Layer not found" );
		return FALSE;
	}
#endif	// __LAYER_1015

	// ???????? 
	if( type == REPLACE_NORMAL )
	{
		if( IsAuthHigher( AUTH_GAMEMASTER ) == FALSE && IsFly() ) 
			return FALSE;
	}

	// ?????? ??????.
	ClearDest();
	SendActMsg( OBJMSG_ACC_STOP );
	SendActMsg( OBJMSG_STOP_TURN );
	SendActMsg( OBJMSG_STAND );

	if( IsCollecting() )
		StopCollecting();

	if( HasActivatedEatPet() )
		InactivateEatPet();


	for( int i = 0; i < pWorld->m_cbModifyLink; i++ )
	{
		if( pWorld->m_apModifyLink[i] == this )
		{
			pWorld->m_apModifyLink[i]	= NULL;
			m_vRemoval	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			break;
		}
	}

	const auto lpReplaceObj = std::ranges::find_if(
		pWorld->m_ReplaceObj,
		[&](const REPLACEOBJ & replaceObj) {
			return replaceObj.pObj == this;
		}
	);

	if (lpReplaceObj == pWorld->m_ReplaceObj.end()) {
		pWorld->m_ReplaceObj.push_back(REPLACEOBJ{ });
		lpReplaceObj->pObj = this;
	}

	lpReplaceObj->dwWorldID	 = dwWorldID;
	lpReplaceObj->vPos       = vPos;
	lpReplaceObj->uIdofMulti = uIdofMulti;
#ifdef __LAYER_1015
	lpReplaceObj->nLayer	= nLayer;
#endif	// __LAYER_1015

	if( IsPlayer() )
	{
		if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() )
			&& !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldID ) )
			//???? ???????? ??????????.
			CInstanceDungeonHelper::GetInstance()->LeaveDungeon( static_cast<CUser*>( this ), pWorld->GetID() );
	}

	return TRUE;
}

BOOL CMover::CreateItem( CItemElem * pItemBase, BYTE* pnId, short* pnNum, BYTE nCount )
{
	if( pItemBase->m_dwItemId == 0 )
		return FALSE;

	if(pItemBase->m_nHitPoint == -1 )
	{
		ItemProp* pItemProp		= pItemBase->GetProp();
		if( pItemProp )
			pItemBase->m_nHitPoint		= pItemProp->dwEndurance;
		else
			pItemBase->m_nHitPoint		= 0;
	}
	if( pItemBase->GetSerialNumber() == 0 )
		pItemBase->SetSerialNumber();
	BOOL fSuccess;

	if( nCount == 0 )
	{
		BYTE anId[MAX_INVENTORY];
		short anNum[MAX_INVENTORY];
		
		fSuccess	= m_Inventory.Add( pItemBase, anId, anNum, &nCount );

		if( fSuccess && IsPlayer() )
		{
			( (CUser*)this )->AddCreateItem( pItemBase, anId, anNum, nCount );
		}
		if( pnId != NULL )
			memcpy( pnId, anId, sizeof(BYTE) * nCount );
		if( pnNum != NULL )
			memcpy( pnNum, anNum, sizeof(short) * nCount );
	}
	else
	{
		CItemElem itemElem;
		itemElem	= *pItemBase;
		for( int i = 0; i < nCount; i++ )
		{
			itemElem.m_nItemNum		= pnNum[i];
			m_Inventory.SetAtId( pnId[i], &itemElem );
		}
		if( IsPlayer() )
			( (CUser*)this )->AddCreateItem( pItemBase, pnId, pnNum, nCount );
		fSuccess	= TRUE;
	}
	return fSuccess;
}

void CMover::RemoveItem(BYTE nId, short nNum) {
	CItemElem * pItemBase = GetItemId(nId);
	if (pItemBase) {
		UpdateItem(*pItemBase, UI::Num::Consume(nNum));
	}
}

// IK3?? ???????? ??) IK3_CLOAK
void CMover::RemoveItemIK3( DWORD dwItemKind3 )
{
	int	nSize = m_Inventory.GetMax();
	for( int i = 0 ; i < nSize; i++ )
	{
		CItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( IsUsableItem( pItemElem ) && pItemElem->GetProp()->dwItemKind3 == dwItemKind3 )
		{
			if( pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild == 0 )
				continue;

			if( m_Inventory.IsEquip( pItemElem->m_dwObjId ) )		
			{
				if( DoEquip( pItemElem, FALSE ) )
				{
					g_UserMng.AddDoEquip( this, -1, pItemElem, FALSE );
					UpdateItem(*pItemElem, UI::Num::RemoveAll());		// remove
				}
				else
				{
					// ???? full????, ???????????? ?????? ?? ????. 
					// WriteError( "RemoveItemIK3->DoEquip : %s %d, User = %s, Item = %d %d %d", 
					// __FILE__, __LINE__, GetName(), pItemElem->m_dwObjId, pItemElem->GetProp()->dwItemKind3, dwItemKind3 );
				}
			}
			else
			{
				UpdateItem(*pItemElem, UI::Num::RemoveAll());		// remove
			}
		}
	}
}

void CMover::RemoveVendorItem( CHAR chTab, BYTE nId, short nNum )
{
	short num;
	CItemElem* pItemElem = m_ShopInventory[chTab]->GetAtId( nId );

	if( pItemElem ) 
	{
		if( ( num = pItemElem->m_nItemNum - nNum ) > 0 )
			pItemElem->m_nItemNum	= num;
		else
			m_ShopInventory[chTab]->RemoveAtId( nId );
		g_UserMng.AddUpdateVendor( this, chTab, nId, num );
	}
}
#endif	// __WORLDSERVER

void CMover::UpdateItemBank(int nSlot, BYTE nId, short newQuantity) {
	CItemElem * pItemBase = GetItemBankId(nSlot, nId);

	if (pItemBase) {
		if (newQuantity == 0) {
			RemoveItemBankId(nSlot, nId);
		} else {
			pItemBase->m_nItemNum = newQuantity;
		}
	}
#ifdef __WORLDSERVER
	if (IsPlayer())
		((CUser *)this)->AddUpdateBankItem(nSlot, nId, newQuantity);
#endif
}

#ifdef __WORLDSERVER
// ?????????????? 1?? ???????? ??????????.
int	CMover::DoDropItemRandom( BOOL bExcludeEquip, CMover* pAttacker, BOOL bOnlyEquip )
{
	int i, nMax, nRealMax, nDropCnt;
	CItemElem* pItemElem;
	CItemElem *pElemBuff[ MAX_INVENTORY + MAX_EQUIPMENT ] = { NULL, };

	nRealMax = nDropCnt = 0;

	nMax = m_Inventory.GetMax();
	for( i = 0; i < nMax; ++i ) 		// ???? ???????? ?????? ???????? ???????? ???????? ??????????.
	{
		pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem == NULL )
			continue;
			
		if( pItemElem->IsQuest() )					// ?????? ?????? ???? 
			continue;

		if( pItemElem->IsCharged() )	// ???? ?????????? ????
			continue;

		if( pItemElem->IsEatPet()
			|| IsUsing( pItemElem )
			)
			continue;

		if( pItemElem->IsBinds() )
			continue;

		if( bOnlyEquip )
		{
			if( m_Inventory.IsEquip( pItemElem->m_dwObjId ) )		// ?????? ?????? ????  
				pElemBuff[ nRealMax++ ] = pItemElem;
		}
		else
		{
			if( bExcludeEquip && m_Inventory.IsEquip( pItemElem->m_dwObjId ) )		// ?????? ?????? ????  
				continue;
			
			pElemBuff[ nRealMax++ ] = pItemElem;
		}
	}

	while( nRealMax )	// ???????? ???????? ?????? ???????? ??.
	{
		int nIdx = xRandom( nRealMax );
		pItemElem = pElemBuff[ nIdx ];		// ?????? ?????????? ???????? ?????? ??????

		int nPartBuf = -1;
		CItemElem* pItemElemLWepon = m_Inventory.GetEquip( PARTS_LWEAPON );
		CItemElem* pItemElemRWepon = m_Inventory.GetEquip( PARTS_RWEAPON );
		if( pItemElem == pItemElemLWepon )
			nPartBuf = PARTS_LWEAPON;
		else if( pItemElem == pItemElemRWepon )
			nPartBuf = PARTS_RWEAPON; 
		if( m_Inventory.IsEquip( pItemElem->m_dwObjId ) )	// ???????? ?????? ???? 			
		{
			if( DoEquip( pItemElem, FALSE, nPartBuf ) )
				g_UserMng.AddDoEquip( this, nPartBuf, pItemElem, FALSE );
		}

		CItem* pItem = DropItem( pItemElem->m_dwObjId, 0, GetPos(), TRUE );
		if( pItem )	
		{
			UNUSED_ALWAYS( pAttacker );
			return 1;
		}

		// drop?? ?????? ???????????? ?????????? ??????.
		for( int j = nIdx; j < nRealMax-1; j ++ )		// ?????? ?????? ????.
			pElemBuff[j] = pElemBuff[j+1];
		nRealMax --;	// ?????? ???????????? ?????? ?????? ???? ????
	}		

	return 0;
}
#endif // __WORLDSERVER

CItem *CMover::_DropItemNPC( DWORD dwItemType, DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos )
{
#ifdef __WORLDSERVER
	if( IsInvalidObj(this) )	
		return NULL;

	CWorld *pWorld = GetWorld();
	if( pWorld == NULL )	
		return NULL;	

	CItemElem * pItemBase = GetItemId( dwID );
	if (!pItemBase) return nullptr;
	
	short nNum	= pItemBase->m_nItemNum;
	if( m_Inventory.IsEquip( pItemBase->m_dwObjId ) ) 
		return NULL;

	if( nDropNum == 0 )
		nDropNum = nNum;
	else if( nNum - (int)nDropNum < 0 )
		return NULL;
	
	CItem* pItem	        = new CItem;
	pItem->m_pItemBase    = new CItemElem;
	*(pItem->m_pItemBase) = *pItemBase;
	pItem->m_pItemBase->m_nItemNum = nDropNum;
		
	if( pItemBase->m_dwItemId == 0 ) 
		Error( "_DropItemNPC SetIndex: %s \n", GetName() ); 

	pItem->SetIndex( D3DDEVICE, pItemBase->m_dwItemId );
	if( pItem->m_pModel )
	{
		D3DXVECTOR3 v = vPos;
		v.y += ( pItem->m_pModel->m_vMax.y - pItem->m_pModel->m_vMin.y ) / 2.0f;
		pItem->SetPos( v );
		pItem->SetAngle( (float)( xRandom( 360 ) ) );
		pItem->m_dwDropTime = timeGetTime();	// ?????????????? ????.
		pWorld->ADDOBJ( (CObj*)pItem, TRUE, GetLayer() );
		RemoveItem( (BYTE)( dwID ), nDropNum );
		return pItem;
	}
	else
	{
		SAFE_DELETE( pItem );
	}
#endif // WORLDSERVER
	return NULL;
}

// ???????? ???? ?? ??????? PK - PK?? ???? ?????? TRUE
BOOL CMover::IsDropableState( BOOL bPK )
{
	if( IsInvalidObj(this) )	
		return FALSE;

#ifdef __WORLDSERVER
	CWorld *pWorld = GetWorld();
	if( pWorld == NULL )	
		return FALSE;

	if( m_pActMover->m_bGround == 0 || m_pActMover->IsFly() )	// ???????? ???????? ?????? ?????????????? ???? ???????????? ????.
	{
		if( bPK == FALSE ) //  PK???? ?????? ?????????? ??????.
		{
			((CUser*)this)->AddDefinedText( TID_GAME_FLIGHTDROP, "" );
			return FALSE;
		}
	}

	if( m_vtInfo.GetOtherID() != NULL_ID )		// ?????????????? ???? ???????????? ????
	{
		((CUser*)this)->AddDefinedText( TID_GAME_TRADELIMITITEM, "" );
		return FALSE;
	}

	if( m_vtInfo.VendorIsVendor() )
		return FALSE;

	if( IsMode( ITEM_MODE ) )
		return FALSE;

#endif // __WORLDSERVER
	return TRUE;
}	


// ?????? ???????? ?????? ?????????? ????????.
// dwType, dwID?? ???????? vPos?? ??????????.
// bPK == YES?? ?????????? PK?????? ???????? ??????????.
CItem* CMover::DropItem( DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos, BOOL bPK )
{
#ifdef __WORLDSERVER
	if( IsPlayer() == FALSE )	
		return _DropItemNPC( 0, dwID, nDropNum, vPos );	// ???? ?????????? ????????.
	
	CItemElem * pItemBase = GetItemId( dwID );
	if( IsDropable( pItemBase, bPK ) == FALSE )
		return NULL;

	short nNum	= pItemBase->m_nItemNum;
	if( nDropNum == 0 )
		nDropNum = nNum;
	else if( nDropNum > nNum )
		return NULL;

	CItem* pItem	      = new CItem;
	pItem->m_pItemBase	= new CItemElem;
	*pItem->m_pItemBase = *pItemBase;
	pItem->m_pItemBase->m_nItemNum		= nDropNum;
		
	if( pItemBase->IsQuest() )
		pItem->m_idHolder	= m_idPlayer;
	
	if( pItemBase->IsLogable() )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "D";
		aLogItem.SendName = GetName();
		aLogItem.RecvName = "GROUND";
		aLogItem.WorldId = GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = GetGold();
		g_DPSrvr.OnLogItem( aLogItem, pItemBase, nDropNum );
	}
	
	pItem->SetIndex( D3DDEVICE, pItemBase->m_dwItemId );

	if( pItem->m_pModel )
	{
		D3DXVECTOR3 v = vPos;
		v.y += ( pItem->m_pModel->m_vMax.y - pItem->m_pModel->m_vMin.y ) / 2.0f;
		pItem->SetPos( v );
		pItem->SetAngle( (float)( xRandom( 360 ) ) );
		pItem->m_dwDropTime = timeGetTime();	// ?????????????? ????.
		GetWorld()->ADDOBJ( (CObj*)pItem, TRUE, GetLayer() );

		RemoveItem( (BYTE)( dwID ), nDropNum );
		return pItem;
	}
	else
	{
		SAFE_DELETE( pItem );
	}

#endif // WORLDSERVER
	return NULL;
}

// TODO_OPTIMIZE: ?????? ?????? ?? ????????????. ( rect?? ?????????? ???? ???? ???? ???? ??????.)
REGIONELEM* CMover::UpdateRegionAttr()
{
	REGIONELEM* pPortkey = NULL;
	const DWORD dwCheck = ( RA_SAFETY | RA_PENALTY_PK | RA_PK | RA_FIGHT ) ;

	D3DXVECTOR3 vPos = GetPos();
	POINT pt = { (LONG)( vPos.x ), (LONG)( vPos.z ) };
	LPREGIONELEM lpRegionElem;
	int nSize = GetWorld()->m_aRegion.GetSize();
	DWORD	dwRegionAttr	= 0;
	for( int i = 0; i < nSize; i++ )
	{
		lpRegionElem = GetWorld()->m_aRegion.GetAt( i );
		if( lpRegionElem->m_rect.PtInRect( pt ) )
		{
			dwRegionAttr	|= lpRegionElem->m_dwAttribute;
			if( lpRegionElem->m_dwIdTeleWorld != WI_WORLD_NONE )
				pPortkey = lpRegionElem;
		}
	}

	if( dwRegionAttr & dwCheck )
	{
		m_dwOldRegionAttr	= m_dwRegionAttr;
		m_dwRegionAttr	= dwRegionAttr;
	}
	else
	{
		m_dwOldRegionAttr	= m_dwRegionAttr;
		m_dwRegionAttr	= GetWorld()->m_nPKMode;;
	}

	return pPortkey;
}

//???? ???????? ?????????? ???? ( ???? ????, ?????? ?????? ????, ???? ?????? ???? )
BOOL CMover::IsRegionMove( DWORD dwOldAttr, DWORD dwRegionAttr )
{
	if( ( dwOldAttr & RA_SAFETY ) == RA_SAFETY ) 
	{
		if( ( dwRegionAttr & RA_SAFETY ) != RA_SAFETY )
		{
			return TRUE;
		}
	}
	else if( ( dwOldAttr & RA_PENALTY_PK ) == RA_PENALTY_PK ) 
	{
		if( ( dwRegionAttr & RA_PENALTY_PK ) != RA_PENALTY_PK )
		{
			return TRUE;
		}
	}
	else if( ( dwOldAttr & RA_PK ) == RA_PK ) 
	{
		if( ( dwRegionAttr & RA_PK ) != RA_PK )
		{
			return TRUE;
		}
	}
	else if( ( dwOldAttr & RA_FIGHT ) == RA_FIGHT )
	{
		if( ( dwRegionAttr & RA_FIGHT ) != RA_FIGHT )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

DWORD	CMover::GetPKPVPRegionAttr()
{
	if( IsRegionAttr( RA_FIGHT ) )
		return RA_FIGHT;
	else if( IsRegionAttr( RA_PENALTY_PK ) )
		return RA_PENALTY_PK;
	else if( IsRegionAttr( RA_PK ) )
		return RA_PK;
	else
		return RA_SAFETY;
}

void CMover::SetStateMode( DWORD dwMode, BYTE nFlag )
{
	m_dwStateMode |= dwMode;
#ifdef __WORLDSERVER
	g_UserMng.AddStateMode( ((CUser*)this), nFlag );
#endif // __WORLDSERVER
}

void CMover::SetStateNotMode( DWORD dwMode, BYTE nFlag )
{
	m_dwStateMode &= (~dwMode);
#ifdef __WORLDSERVER
	g_UserMng.AddStateMode( ((CUser*)this), nFlag );
#endif // __WORLDSERVER
} 

BOOL CMover::IsUseItemReadyTime( const ItemProp* pItemProp, OBJID dwObjItemId )
{
	if( m_Inventory.IsEquip( dwObjItemId ) )
	{		
		return TRUE;
	}

	if( pItemProp->dwParts == PARTS_RIDE )
	{
		CWorld* pWorld = GetWorld();
		if( pWorld == NULL )
			return FALSE;
		
		int nLimitLv = pItemProp->dwFlightLimit;
		if( nLimitLv == NULL_ID )
			nLimitLv = 1;
#ifdef __NOLIMIT_RIDE_ITEM
		if( pItemProp->dwID == II_RID_RID_STI_MAGIC01 )
			nLimitLv = 0;
#endif // __NOLIMIT_RIDE_ITEM
		if( GetFlightLv() < nLimitLv ) // ?????????? ?????? ??????.
		{
			PrintString( this, TID_GAME_USEAIRCRAFT );	// ???????? ???????? ????????
			return FALSE;
		}
		
		int nAttr = pWorld->GetHeightAttribute( GetPos().x, GetPos().z );
		if( !pWorld->m_bFly || nAttr == HATTR_NOFLY ) // ??????????????????.
		{
			PrintString( this, TID_ERROR_NOFLY );
			return FALSE;
		}
	}
	
	if( pItemProp->dwSkillReadyType != 0 && 
		pItemProp->dwSkillReadyType != 0xffffffff )
	{
		if( m_vtInfo.IsVendorOpen() )	// ???????? ?????? ??????
		{
			return FALSE;
		}
		else if( IsStateMode( STATE_BASEMOTION_MODE ) )	// ???? ?????????? ?????? ????
		{
			PrintString( this, TID_PK_BLINK_LIMIT );	// ?????????? ???????? ????????
			return FALSE;		
		}
		else if( m_pActMover->IsState( OBJSTA_STAND ) == FALSE )	// ?????? ?????? ?????? ?????? ????
		{
			PrintString( this, TID_PK_STAND_LIMIT );	// ???????? ???????? ????????
			return FALSE;		
		}
		else if( IsFly() )
		{
#ifdef __CLIENT
			g_WndMng.PutString( prj.GetText( TID_PK_FLIGHT_NOUSE ), NULL, prj.GetTextColor( TID_PK_FLIGHT_NOUSE ) );
#endif // __CLINET
			return FALSE;
		}
	}
	return TRUE;
}

void CMover::ProcessRegion()
{
	if( FALSE == IsPlayer() )
		return;

	REGIONELEM* pPortkey = NULL;
	if( IsPosChanged() )
	{
		pPortkey = UpdateRegionAttr();
		SetPosChanged( FALSE );
	}

#ifdef __WORLDSERVER

//#if __VER < 8     // 8?? ???????? ???????? PVP??????????   Neuz, World
	if( IsRegionMove( m_dwOldRegionAttr, m_dwRegionAttr ) )
	{
		if( m_nDuel == 1 )
		{
			CMover *pDuelOther = prj.GetMover( m_idDuelOther );
			if( IsValidObj( pDuelOther ) )
			{
				pDuelOther->DuelResult( this );
				pDuelOther->ClearDuel();
				( (CUser*)pDuelOther )->AddSetDuel( pDuelOther );
				ClearDuel();
				( (CUser*)this )->AddSetDuel( this );
			}
		}
		else if( m_nDuel == 2 )
		{
			CParty* pParty	= g_PartyMng.GetParty( m_idparty );
			if( pParty && pParty->IsLeader( m_idPlayer ) )
			{
				CParty* pParty2		= g_PartyMng.GetParty( m_idDuelParty );
				if( pParty2 )
				{
					CUser* pWinner	= (CUser*)pParty2->GetLeader();
					if( pWinner )
						pWinner->DuelResult( this );
				}
			}
		}
	}
//#endif	// __VER < 8  

	if( pPortkey )
	{
		if( IsFly() )	// ReplaceInspection() ?? ???? ???? ?????? ????????.
			return;
		
		if(	CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pPortkey->m_dwIdTeleWorld ) )
		{
			if(	!CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( static_cast<CUser*>( this )->GetWorld()->GetID() ) ) 
				CInstanceDungeonHelper::GetInstance()->EnteranceDungeon( static_cast<CUser*>( this ), pPortkey->m_dwIdTeleWorld );
			else
				REPLACE( g_uIdofMulti, pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld, REPLACE_NORMAL, static_cast<CUser*>( this )->GetLayer() );
			return;
		}
		if( ReplaceInspection( pPortkey ) == FALSE )
			return;
		REPLACE( g_uIdofMulti, pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld, REPLACE_NORMAL, nTempLayer );
	}
#endif	// __WORLDSERVER
}

BOOL CMover::Read( CFileIO* pFile )
{
	CObj::Read( pFile );

	char szDialogFile[MAX_DIALOGFILENAME];
	DWORD dwExtraFlag = 0;

	pFile->Read( m_szName, sizeof( m_szName ) );
	pFile->Read( szDialogFile, sizeof( szDialogFile ) );
	pFile->Read( m_szCharacterKey, sizeof( m_szCharacterKey ) );	
	pFile->Read( &m_dwBelligerence, sizeof( m_dwBelligerence ) );
	pFile->Read( &dwExtraFlag, sizeof( dwExtraFlag ) );

	UpdateParam();

#if defined(__WORLDSERVER)	
	InitCharacter( GetCharacter() );
#endif
	
	return TRUE;
}

CItemElem* CMover::GetLWeaponItem()
{
	return GetWeaponItem( PARTS_LWEAPON );
}

CItemElem * CMover::GetWeaponItem(int nParts) {
	return IsPlayer() ? m_Inventory.GetEquip(nParts) : nullptr;
}

ItemProp* CMover::GetActiveHandItemProp( int nParts )
{
	if( IsPlayer() )
	{
#ifdef __CLIENT
		if( IsActiveMover() )
		{
			CItemElem* pItemElem = m_Inventory.GetEquip( nParts );
			if( pItemElem )
				return pItemElem->GetProp();
			return prj.GetItemProp( II_WEA_HAN_HAND );
		}
		else	// ???????????????? ?? ???????? m_adwEquipment?? dwObjId?? ???? dwItemId?? ?????? ????.
		{
			DWORD dwWeaponId	= m_aEquipInfo[nParts].dwId;
			if( dwWeaponId != NULL_ID )
				return prj.GetItemProp( dwWeaponId );
			return prj.GetItemProp( II_WEA_HAN_HAND );
		}
#else	// __CLIENT
		CItemElem* pItemElem = m_Inventory.GetEquip( nParts ); 
		if( pItemElem )
			return pItemElem->GetProp();
		return prj.GetItemProp( II_WEA_HAN_HAND );
#endif	// __CLIENT
	}

	// virt?? NPC???? ???????? ???? ?????? ???? ?????????? ?????? ???????? ?????? ?? ?????? ??????????. 
	// ?????? ???????? ???? ?????? ???????? ????????.
	// ?? ???????? ???? ??????, ?????? ?????? ??????, ???? ?????????? ?????? ??????.
	if( m_dwVirtItem != NULL_ID )
	{
		if( m_dwVirtType == VT_ITEM )
			return prj.GetItemProp( m_dwVirtItem );
		if( m_dwVirtType == VT_SKILL )
			return prj.GetSkillProp( m_dwVirtItem );
	}
	return NULL;
}

CItemElem * CMover::GetEquipItem(const int nParts) {
	return m_Inventory.GetEquip(nParts);
}

const CItemElem * CMover::GetEquipItem(const int nParts) const {
	return m_Inventory.GetEquip(nParts);
}

ItemProp* CMover::GetEquipItemProp( CItemContainer* pInventory, PEQUIP_INFO pEquipInfo, int nParts )
{
	ItemProp *pItemProp = NULL;
#if 1	
	if( pInventory == NULL  )
	{
		if( pEquipInfo[nParts].dwId != NULL_ID )
			pItemProp = prj.GetItemProp( pEquipInfo[nParts].dwId );
	} else
	{
		CItemElem *pItemElem = pInventory->GetEquip( nParts );		
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	}
	
#endif
	return pItemProp;
}

// ???????? ????.
// ?????? ?????? ???? ????.
BOOL CMover::IsDualWeapon()
{
#ifdef __CLIENT
	if( IsActiveMover() )
#else
	if( IsPlayer() )
#endif
	{
		CItemElem *pItemElemR = m_Inventory.GetEquip( PARTS_RWEAPON );
		CItemElem *pItemElemL = m_Inventory.GetEquip( PARTS_LWEAPON );
		if( pItemElemR && pItemElemL )
			return TRUE;
		else
			return FALSE;
	} else
	{
		if( m_aEquipInfo[PARTS_RWEAPON].dwId != NULL_ID && m_aEquipInfo[PARTS_LWEAPON].dwId != NULL_ID )
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}


void CMover::ProcessAniSpeed()
{
	if( IsPlayer() )
	{
		switch( m_pActMover->GetState() & OBJSTA_ATK_ALL )
		{
		case OBJSTA_ATK1:
		case OBJSTA_ATK2:
		case OBJSTA_ATK3:
		case OBJSTA_ATK4:
		case OBJSTA_ATK_MAGIC1:
		case OBJSTA_ATK_RANGE1:
			m_fAniSpeed = GetAttackSpeed();
			break;
		case OBJSTA_ATK_CASTING1:
		case OBJSTA_ATK_CASTING2:
		case OBJSTA_ATK_MAGICSKILL:
			m_fAniSpeed	= GetCastingAniSpeed();
			break;
		default:
			{
				m_fAniSpeed = 1.0f;		// ?????? ???? ????.			
			}
		}
	} 
	else
	{	
		m_fAniSpeed = 1.0f;
	}
}

#ifdef __CLIENT
void CMover::ProcessScaleSlerp()
{
	if( m_fDestScaleSlerp > 0 )		// ???? ?????? ???? ??????.
	{
		MoverProp* pMoverProp = GetProp();
		if( pMoverProp )
		{
			D3DXVECTOR3 vScale = GetScale();
			if( pMoverProp->dwClass != RANK_MATERIAL && pMoverProp->dwClass != RANK_SUPER && pMoverProp->dwClass != RANK_MIDBOSS )
			{
				LPCTSTR szErr = Error( "CMover::Process : ???????? ?????? ????????.%s %f, %f, %f %f %f", GetName(), m_fDestScaleSlerp, m_fDestScale, vScale.x, vScale.y, vScale.z );
				//ADDERRORMSG( szErr );
				m_fDestScaleSlerp = 0;
			}
			else
			{
				D3DXVECTOR3 vDestScale = D3DXVECTOR3( m_fDestScale, m_fDestScale, m_fDestScale );
				D3DXVec3Lerp( &vScale, &vDestScale, &vScale, m_fDestScaleSlerp );
				SetScale( vScale );
				m_fDestScaleSlerp -= 0.001f;
			}
		}
	}
}

void CMover::ProcessWaterCircle( const D3DXVECTOR3& vPosTemp, const D3DXVECTOR3& vPos )
{
	if( !IsMode( TRANSPARENT_MODE ) )	
	{
		LPWATERHEIGHT pWaterHeight = GetWorld()->GetWaterHeight( GetPos() );

		if(pWaterHeight && (pWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER)
		{
			FLOAT fHeight = (FLOAT)pWaterHeight->byWaterHeight;
			if( vPosTemp.y > fHeight && vPos.y <= fHeight && m_pActMover->IsActJump() ) 
			{
				CreateSfx( g_Neuz.m_pd3dDevice, XI_GEN_WATERCROWN01, D3DXVECTOR3( vPos.x, fHeight, vPos.z ) );
			}
			if( vPos != vPosTemp && vPos.y < fHeight && vPos.y > ( fHeight - 1.5f )  && m_nWaterCircleCount > 7 ) 
			{
				CreateSfx( g_Neuz.m_pd3dDevice, XI_GEN_WATERCIRCLE01, D3DXVECTOR3( vPos.x, fHeight + .01f, vPos.z) );
				m_nWaterCircleCount = 0;
			}
		}
		m_nWaterCircleCount++;
	}
}

// ???????????? ???? ???????????? ????...???????? ?????????????? ?????????????? ????.
// ?????????? ???????????? ?????? ?????? ???????? ??????...
void CMover::ProcessWalkSound()
{
	if( IsPlayer() && !( IsMode( TRANSPARENT_MODE ) ) )
	{
	#ifdef __Y_INTERFACE_VER3
		if( m_pActMover->GetMoveState() & OBJSTA_FMOVE || m_pActMover->GetMoveState() & OBJSTA_LMOVE || m_pActMover->GetMoveState() & OBJSTA_RMOVE )
	#else //__Y_INTERFACE_VER3
		if( m_pActMover->GetMoveState() & OBJSTA_FMOVE )							
	#endif //__Y_INTERFACE_VER3
		{
			CModelObject*	pModel = (CModelObject*)m_pModel;
			if( pModel->IsAttrHit() )
			{
				const auto pos = GetPos();
				PLAYSND( SND_WALK_LANDSOFT, &pos );
			}
		}
	}
}

void CMover::ProcessDustSFX()
{
	if( ! ( IsMode( TRANSPARENT_MODE ) ) )		// ?????????? ???????? ????.
	{
		CModelObject*	pModel = (CModelObject*)m_pModel;
		MOTION_ATTR* pAttr = pModel->IsAttrSound();
 		if( IsPlayer() && pAttr && pAttr->m_nSndID > 0 )  // ?????? ?????? ?????? ??????, pause?????? ?????? ???? ????
		{
			if( m_pActMover->IsActJump() )
				CreateSfx( g_Neuz.m_pd3dDevice, XI_NAT_DUST_JUMP, GetPos() );
			else
			if( m_pActMover->IsRun() && !m_pActMover->IsActAttack() )
				CreateSfx( g_Neuz.m_pd3dDevice, XI_NAT_DUST_RUN, GetPos() );
		}
	}
}

// ???? ???? 
void CMover::ProcessETC()
{
	int nAbilityOption	= GetSetItemClient();
	if( nAbilityOption >= 3 && !(m_dwFlag & MVRF_SETITEMSFX))
    {
		m_dwFlag |= MVRF_SETITEMSFX;
		CreateAbilityOption_SetItemSFX( nAbilityOption );
	}

	CModelObject*	pModel = (CModelObject*)m_pModel;
	if( pModel->m_SparkInfo.m_bUsed )
	{
		if( pModel->m_SparkInfo.m_nCnt != 0 )
		{
			pModel->m_SparkInfo.m_fLerp -= 0.05f;

			if( pModel->m_SparkInfo.m_fLerp < 0.0f )
			{
				pModel->m_SparkInfo.m_bUsed = FALSE;
			}
		}
		
		pModel->m_SparkInfo.m_nCnt++;
	}

	if( (m_nCount & 131071) == 0 ) // 30??????
	{
		if( xRandom(100) > 50 )
		{
			ItemProp* pItemProp = NULL;

			if( IsActiveMover() )
			{
				CItemElem* pItemElem	= GetEquipItem( PARTS_HAT );

				if( pItemElem )
					pItemProp = pItemElem->GetProp();
			}
			else
			{
				DWORD dwId	= m_aEquipInfo[PARTS_HAT].dwId;
				O3D_ELEMENT*	pElement = NULL;
				
				if( dwId != NULL_ID )
					pItemProp	= prj.GetItemProp( dwId );
			}


			if( pItemProp )
			{
				if( GetSex() == SEX_MALE )
				{
					if( pItemProp->dwID == II_ARM_M_CHR_DARKVADER01HAT )
					{
						char szFormat[128] = {0};
						sprintf( szFormat, "/s %s", prj.GetText(TID_GAME_FATHER) );
						g_DPlay.SendChat( szFormat );
					}
				}
				else
				{
					if( pItemProp->dwID == II_ARM_F_CHR_DARKVADER01HAT )
					{
						char szFormat[128] = {0};
						sprintf( szFormat, "/s %s", prj.GetText(TID_GAME_MOTHER) );
						g_DPlay.SendChat( szFormat );
					}
				}
			}		
		}
	}
	
	// ???????? ????????...?????? ?????? ???? ?????? ???? ???????? ????????.
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_GUILDWAR )
	{
		CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
		
		if( IsPlayer() && g_pPlayer && pWndWorld )
		{
			BOOL bFlag = FALSE;
			int nActivePlayer = pWndWorld->IsGCStatusPlayerWar( g_pPlayer->m_idPlayer );
			int nPlayer = pWndWorld->IsGCStatusPlayerWar( m_idPlayer );
			
			// ??????(??????)?? ??????????
			if( nActivePlayer == 1 )
			{
				// ???????? ?????????? ????????
				if( nPlayer != 1 )
				{
					bFlag = TRUE;
				}
			}
			else
			// ??????(??????)?? ??????????
			if( nActivePlayer == 0 )
			{
				// ?????????? ????????
				if( nPlayer == -1 )
				{
					bFlag = TRUE;
				}
			}

			if( bFlag )
			{
				m_dwMode |= GCWAR_NOT_CLICK_MODE;
				m_dwMode |= GCWAR_RENDER_SKIP_MODE;
			}
			else
			{
				m_dwMode &= ~GCWAR_NOT_CLICK_MODE;
				m_dwMode &= ~GCWAR_RENDER_SKIP_MODE;				
			}
		}
	}
	else
	{
		m_dwMode &= ~GCWAR_NOT_CLICK_MODE;
		m_dwMode &= ~GCWAR_RENDER_SKIP_MODE;				
	}
#ifdef __QUIZ
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_QUIZ )
	{
		CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
		
		if( IsPlayer() && CQuiz::GetInstance()->GetZoneType( this ) != CQuiz::ZONE_QUIZ && pWndWorld )
			m_dwMode |= QUIZ_RENDER_SKIP_MODE;
		else
			m_dwMode &= ~QUIZ_RENDER_SKIP_MODE;
	}
	else
		m_dwMode &= ~QUIZ_RENDER_SKIP_MODE;
#endif // __QUIZ
}

extern 	void CreateFlyParticle( CMover *pMover, float fAngX, int nType );

#endif // __CLIENT


void CMover::ProcessIAObjLink( D3DXVECTOR3& vPos )
{
	if( GetIAObjLink() )
	{
		D3DXMATRIX mLocal, mTrans, mRot, mWorld, *pmWorldIA;
		D3DXMatrixTranslation( &mTrans, vPos.x, vPos.y, vPos.z );
		D3DXMatrixRotationY( &mRot, D3DXToRadian( -GetAngle() ) );
		mWorld = m_matScale;

		pmWorldIA = GetIAObjLink()->GetMatrixWorldPtr();
		D3DXMatrixMultiply( &mWorld, &mWorld, &mRot );
		D3DXMatrixMultiply( &mWorld, &mWorld, &mTrans );
		D3DXMatrixMultiply( &mLocal, &mWorld, GetIAObjLink()->GetInvTM() );	// IA???????????????? ???? ????.
		D3DXMatrixMultiply( &mWorld, &mLocal, pmWorldIA );

		vPos.x = mWorld._41;
		vPos.y = mWorld._42;
		vPos.z = mWorld._43;
		
		D3DXQUATERNION qRot;
		D3DXVECTOR3 vYPW;
		D3DXQuaternionRotationMatrix( &qRot, &mWorld );
		QuaternionRotationToYPW( qRot, vYPW );
		FLOAT fAngNew = -D3DXToDegree(vYPW.y);
		SetAngle( fAngNew );
	}
}

void CMover::ProcessActionPoint()
{
	// 3?????? ?????????? ???? ????.
	if( (int)(g_tmCurrent - m_tmActionPoint) > (1000 * 3) )		
	{
		FLOAT fTime = (g_tmCurrent - m_tmActionPoint) / 1000.0f;	// ?????? ?????? ?????? ?????? ????.
		fTime /= 3.0f;		// 3?????? ??????????.
		if( fTime < 0 )	fTime = 0;

	#ifdef __WORLDSERVER
		((CUser*)this)->m_playTaskBar.m_nActionPoint += (int)(fTime * 2.0f);
		if( ((CUser*)this)->m_playTaskBar.m_nActionPoint > 100 )
			((CUser*)this)->m_playTaskBar.m_nActionPoint = 100;

		if( m_nDuel == 1 )		// ?????????? / ??????????.
		{
			CMover *pDstDuel = prj.GetMover( m_idDuelOther );
			if( IsInvalidObj(pDstDuel) )		// ?????????? ??????????.
			{
				( (CUser*)this )->AddDuelCancel( NULL_ID );
				ClearDuel();
			}
		}
	#else
		if( IsActiveMover() )
		{
			g_WndMng.m_pWndTaskBar->m_nActionPoint += (int)(fTime * 2.0f);
			if( g_WndMng.m_pWndTaskBar->m_nActionPoint > 100 )
				g_WndMng.m_pWndTaskBar->m_nActionPoint = 100;

			g_Neuz.m_NeuzEnemy.CheckInvalid();
		}
	#endif // CLIENT
		
		m_tmActionPoint = g_tmCurrent;
	} 
}

//
//
//

#ifdef __WORLDSERVER
	#include "GlobalTime.h"
	extern float	r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;
	extern BOOL     g_bProfiling;

	class CProfileSample
	{
	public:
		CStopwatch&		m_stopWatch;
		float&			m_fValue;

		CProfileSample( CStopwatch& stopWatch, float& value ) : m_stopWatch( stopWatch ), m_fValue( value )
		{
			if( g_bProfiling )
				stopWatch.PlayEx();
		}

		~CProfileSample() 
		{
			if( g_bProfiling )
				m_fValue += m_stopWatch.StopEx();
		}
	};

	#ifdef __PROFILE_RUN
		#define PROFILE( stopWatch, value ) ((void)0);
	#else
		#define PROFILE( stopWatch, value ) CProfileSample __sample( stopWatch, value )
	#endif
#else
	#define PROFILE( stopWatch, value ) ((void)0);
#endif

#ifdef __WORLDSERVER
#define	CF_SEC	15
#else	// __WORLDSERVER
#define	CF_SEC	63
#endif	// __WORLDSERVER

void CMover::Process()
{
	_PROFILE("CMover::Process()");
	
	if( IsDelete() )
		return;

	CModelObject*	pModel = (CModelObject*)m_pModel;
	D3DXVECTOR3		vPos, vPosTemp;
	BOOL			bCollision;	

#if defined(__WORLDSERVER) && !defined(__PROFILE_RUN)
	CStopwatch		sw;
#endif

#ifdef __WORLDSERVER
	if( m_2pc.size() == 0 && m_dwAIInterface != AII_PET )
		return;
#endif	// __WORLDSERVER
#ifdef __CLIENT
	//if(IsPlayer()) 
	//{
		O3D_ELEMENT* pMask = pModel->GetParts(PARTS_MASK);
		if(pMask)
		{
			if(IsOptionRenderMask() )
				pModel->SetEffectOff(PARTS_MASK, XE_HIDE);
			else
				pModel->SetEffect(PARTS_MASK, XE_HIDE);
		}
	//}
#endif	// __CLIENT
	// ???? ???? 
#ifdef __CLIENT
	if( m_nCorr > 0 )
		--m_nCorr;
	if( m_nDmgCnt > 0 )		
		--m_nDmgCnt;
#else	
	if( m_nDead > 0 )		
		--m_nDead;
#endif	


#ifdef __CLIENT
	if( IsNPC() && IsFlyingNPC() )				// ???????? ?????? ????
		CreateFlyParticle( this, GetAngleX(), 0 );
	ProcessScaleSlerp();
	Interpolate();

	if( m_pRide && m_pRide->m_pBone )
	{
		WingMotionSetting( pModel );
	}

	if( m_pRide )
		m_pRide->FrameMove( NULL, GetRideFrameSpeed() );

	if( m_pLadolf )
		m_pLadolf->FrameMove();

	if( IsPlayer() && HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		if( m_pAngel == NULL )
		{
			m_pAngel = new CModelObject;
			m_pAngel->InitDeviceObjects( g_Neuz.m_pd3dDevice );
			
#ifdef __BUFF_1107
			IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
			ItemProp* pItemProp	= NULL;
			if( pBuff )
				pItemProp	= pBuff->GetProp();
			if( pItemProp )
#else	// __BUFF_1107
			LPSKILLINFLUENCE itemskill = m_SkillState.GetItemBuf(IK3_ANGEL_BUFF);
			ItemProp *pItemProp = prj.GetItemProp( itemskill->wID );
			if( itemskill && pItemProp )
#endif	// __BUFF_1107
			{
				switch(pItemProp->dwSfxElemental)
				{
					case ELEMENTAL_ANGEL_BLUE: m_dwAngelKind = 13; break;
					case ELEMENTAL_ANGEL_RED:  m_dwAngelKind = 16; break;
					case ELEMENTAL_ANGEL_WHITE: m_dwAngelKind = 19; break;
					case ELEMENTAL_ANGEL_GREEN: m_dwAngelKind = 22; break;
				}

				CString textFile;
				textFile.Format("%s.chr", pItemProp->szTextFileName);
				m_pAngel->LoadBone( textFile );
				textFile.Format("%s.o3d", pItemProp->szTextFileName);
				m_pAngel->LoadElement( textFile, 0 );
				textFile.Format("%s_stand01.ani", pItemProp->szTextFileName);
				m_pAngel->LoadMotion( textFile );

				m_AngelPos = GetPos();

				m_AngelWorldM = m_matWorld;
				D3DXMATRIX mScal;			
				D3DXMatrixScaling( &mScal, 0.2f, 0.2f, 0.2f );	
				
				m_AngelWorldM = mScal * m_matRotation;

				m_AngelPos.x += 1.0f;
				m_AngelWorldM._41 = m_AngelPos.x;
				m_AngelWorldM._42 = m_AngelPos.y + 1.0f;
				m_AngelWorldM._43 = m_AngelPos.z;
				
				m_pAngelFlag = TRUE;
			}
		}
	}
	else
	{
		m_pAngelFlag = FALSE;
		if( m_pAngel )
		{
			SAFE_DELETE(m_pAngel);
			m_pAngel = NULL;
		}
	}

	if( m_pAngel )
	{
		m_pAngel->FrameMove();
		AngelMoveProcess();
	}

#ifdef __EVE_BALLOON
	if( IsPlayer() != FALSE && HasBuffByIk3( IK3_BALLOON ) )
	{
		if( m_pBalloon == NULL )
		{
			m_pBalloon = new CModelObject;
			m_pBalloon->InitDeviceObjects( g_Neuz.m_pd3dDevice );
			
#ifdef __BUFF_1107
			IBuff* pBuff = m_buffs.GetBuffByIk3( IK3_BALLOON );
			if( pBuff != NULL )
			{
				ItemProp *pItemProp = pBuff->GetProp();
#else	// __BUFF_1107
			LPSKILLINFLUENCE itemskill;
			for( int i=0; i<MAX_SKILLINFLUENCE; i++)
			{
				LPSKILLINFLUENCE pSkill = m_SkillState.Get(i);
				if( pSkill->wID == II_SYS_SYS_EVE_BALLOON
					|| pSkill->wID == II_SYS_SYS_EVE_BALLOON01
					|| pSkill->wID == II_SYS_SYS_EVE_BALLOON02
					|| pSkill->wID == II_SYS_SYS_EVE_BALLOON_01
					|| pSkill->wID == II_SYS_SYS_EVE_BALLOON01_01
					|| pSkill->wID == II_SYS_SYS_EVE_BALLOON02_01
 				)
					itemskill = pSkill;
			}
			if(itemskill)
			{
				ItemProp *pItemProp = prj.GetItemProp( itemskill->wID );
#endif	// __BUFF_1107
				if( pItemProp )
				{
					TCHAR szModelName[ MAX_PATH ];
					prj.m_modelMng.MakeModelName( szModelName, OT_ITEM, pItemProp->dwID );
					m_pBalloon->LoadElement( szModelName, 0 );
					
					m_BalloonPos = GetPos();
					
					m_BalloonWorldM = m_matWorld;
					D3DXMATRIX mScal;			
					D3DXMatrixScaling( &mScal, 1.5f, 1.5f, 1.5f );	
					
					m_BalloonWorldM = mScal;// * m_matRotation;
					
					m_BalloonPos.x += 1.0f;
					m_BalloonWorldM._41 = m_BalloonPos.x;
					m_BalloonWorldM._42 = m_BalloonPos.y + 1.0f;
					m_BalloonWorldM._43 = m_BalloonPos.z;
					
					m_pBalloonFlag = TRUE;
				}
			}
		}
	}
	else
	{
		m_pBalloonFlag = FALSE;
		if( m_pBalloon )
		{
			SAFE_DELETE(m_pBalloon);
			m_pBalloon = NULL;
			m_fBalloonYPos = 1.0f;
		}
	}
	
	if( m_pBalloon )
	{
		m_pBalloon->FrameMove();
		BalloonMoveProcess();
	}
#endif //__EVE_BALLOON

	if( g_tmCurrent - m_dwWingTime > 1100 )
	{
		if(HasBuff( BUFF_ITEM, II_CHR_MAG_TRI_ANGELWING ) || HasBuff( BUFF_ITEM, II_SYS_SYS_EVE_WINGS ))
		{
			if(m_pSfxWing == NULL)				
				m_pSfxWing = CreateSfx( D3DDEVICE, XI_NAT_WINGANGEL01, GetPos(), GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );				
		}
		else
		{
			if(m_pSfxWing != NULL)
			{
				m_pSfxWing->Delete();
				m_pSfxWing = NULL;
			}
		}
		m_dwWingTime = g_tmCurrent;
	}
#endif // __CLIENT

	// 1?? 1?? ?????? ????
	if( IsPlayer() )
	{
#ifdef __MAP_SECURITY
#ifdef __CLIENT
		if( IsActiveMover() && ( ( m_nCount & 31 ) == 0 ) )
			g_WorldMng.CheckMapKey();
#endif // __CLIENT
#endif // __MAP_SECURITY

		if( ( m_nCount & CF_SEC ) == 0 )
		{
			ProcessPet();
#ifdef __WORLDSERVER
			ProcessCollecting();
			( (CUser*)this )->m_Pocket.ProcessExpiring();
#else	// ProcessCollecting
			if( IsActiveMover() )
				ProcessCollecting();
#endif	// ProcessCollecting
#ifdef __JEFF_9_20
			if( m_dwMute > 0 )
				m_dwMute--;
#endif	// __JEFF_9_20
		}
	}

	if( ( m_nCount & CF_SEC ) == 0 )
	{
		PROFILE( sw, r1 );
		ProcessBuff();
	}

	{
		PROFILE( sw, r2 );
		ProcessMove();	
	}

	{
		PROFILE( sw, r3 );
		ProcessAI();							// 1?? ???????? ProcessAI?? ?????? ?????????? ????.
	}

	vPosTemp = vPos	= GetPos();

	{
		PROFILE( sw, r4 );
		m_pActMover->ProcessPreAction();		// ???? ???? ?????? ???? ??????	 ?????? ??????
		ProcessAniSpeed();						// ???? ?????????? ???? ????.
	}

	{
		PROFILE( sw, r5 );

	#ifdef __X15
		for( int i = 0; i < 4; ++i )
	#endif
		{
			ProcessMovePattern();
			
			m_pActMover->ProcessAction( &vPos );

		#ifdef __CLIENT
			ProcessWalkSound();
		#endif //__CLIENT
			
			if( IsAnimate() )
			{
				FLOAT fFactor	= 1.0F;
				
				if( !( IsPlayer() && IsFly() ) && ( m_pActMover->IsState( OBJSTA_FMOVE ) || m_pActMover->IsState( OBJSTA_TURN_ALL ) ) )
				{
					int nMaxFrame	= pModel->GetMaxFrame();
					FLOAT fSpeed	= GetSpeed( m_pActMover->m_fSpeed );
//					FLOAT fMaxWidth	= pModel->GetMaxWidth();
//					FLOAT fMaxHeight	= pModel->GetMaxHeight();
					MoverProp* pProp	= GetProp();
					if( IsNPC() )
					{
						switch( pProp->dwAI )
						{
							case AII_MONSTER:
							case AII_PET:
							case AII_EGG:
								{
									fFactor	= fSpeed * nMaxFrame / 1.1F;
									break;
								}
						}
					}
					else
						fFactor	= fSpeed * nMaxFrame / 2.2F;

					if( fabs( pProp->fFrame - 0.0F ) < 0.000001F )		// 0???? ????
						fFactor		= 1.0F;
					else if( pProp->dwFlying == 1 )
						fFactor		= 1.0F;
					else
						fFactor		*= pProp->fFrame;
					if( fabs( fFactor - 0.0F ) < 0.000001F )		// fFactor?? 0???? ???? ??
						fFactor		= 1.0F;
				}
				pModel->FrameMove( &vPos, m_fAniSpeed * fFactor );		// ?????????? ?????? ????
			#ifdef __CLIENT			
				ProcessDustSFX();
			#endif // CLIENT
			}
		}	// for( 4 )
		
		if( m_nAtkCnt > 0 )		// ?????????? ?????? ?????? ???????? ???? ???? ????.
			++m_nAtkCnt;
	}
	/*
#ifdef __CLIENT
	if(this == g_pPlayer)
	{
		int n = 0;
	}
#endif*/
	{
		PROFILE( sw, r6 );
		bCollision = m_pActMover->ProcessCollision( &vPos );	// ???????? ?????? & ????????.
	}

#ifdef __CLIENT
	ProcessWaterCircle( vPosTemp, vPos );		// ???? ???? - ?????? ???????? ????
#endif

	{
		PROFILE( sw, r7 );
		ProcessIAObjLink( vPos );
	}

	{
		PROFILE( sw, r8 );
	#ifdef __CLIENT
		if( GetWorld()->GetLandscape( vPos ) )
	#endif
		{
			SetPos( vPos );
			if( bCollision )
				OnActCollision();
		}
	}

	{
		PROFILE( sw, r9 );
		ProcessRegion();
		ProcessQuest();
	#ifdef __WORLDSERVER
		ProcessDeleteRespawn();
	#endif // __WORLDSERVER
	}

	#ifdef __WORLDSERVER
	{
		PROFILE( sw, r10 );
		ProcessRegenItem();
		ProcessScript();
	}
	#endif	// __WORLDSERVER
	
	{
		PROFILE( sw, r11 );
	#ifdef __WORLDSERVER
		ProcessRecovery();	// ??????????, ??????????, ?????????? ???? ????  ?????? ?????????? ???????? ????.
		ProcessAbnormalState();

		if( IsNPC() )
		{
//			if( m_nCollectOwnCnt > 0 )
//				--m_nCollectOwnCnt;

			if( (m_nCount & 127) == 0 )		// ?? 8?????? ??????
			{
				ProcessTarget();
				ProcInstantBanker();
				ProcInstantGC();
			}
		}
	#endif // __WORLDSERVER

		if( IsPlayer() && IsLive() )
			ProcessActionPoint();
	}	
#ifdef __WORLDSERVER
	ProcessSFXDamage();
#endif	// __WORLDSERVER

#if defined(__CLIENT)
	ProcessETC();
	ProcessEyeFlash();

	if( NULL_ID != m_dwMoverSfxId && !m_pSfxBuffPet )		//gmpbigsun: buffpet effect
		SetSfxBuffPet( m_dwMoverSfxId );
	
#endif //defined(__CLIENT) 

	++m_nCount;		// ?????? ???????? ???? ?????? ??????. ?????? ???? 0???? ?????? ???? ????.

}

#ifdef __CLIENT

void CMover::AngelMoveProcess()
{
	if( m_pAngelFlag && m_pAngel != NULL )
	{
		D3DXVECTOR3 vDist = GetPos() - m_AngelPos;
		FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
		
		if(fDistSq > 100.0f * 100.0f) //Teleport?? ?????? ?????? ???? ???? ?????????? ??.
		{
			if(m_pAngel)
				SAFE_DELETE(m_pAngel);
			m_pAngel = NULL;
			m_pAngelFlag = FALSE;
			return;
		}
		if( fDistSq > 1.0f * 1.0f )
		{
			D3DXVECTOR3 vf1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			D3DXVECTOR3 vf2 = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
			D3DXVECTOR3 vf3;

			FLOAT fSerp = fDistSq / 10.0f;

			D3DXVec3Lerp( &vf3, &vf1, &vf2, fSerp );

			FLOAT fSpeed = 0.0f;
			m_AngelWorldM = m_matWorld;
			D3DXMATRIX mScal;			
			D3DXMatrixScaling( &mScal, 0.2f, 0.2f, 0.2f );	

			m_AngelWorldM = mScal * m_matRotation;

			D3DXVec3Normalize( &vDist, &vDist );

			fSpeed = GetSpeed( m_pActMover->m_fSpeed ) * vf3.x;

			vDist = vDist * fSpeed;

			m_AngelPos.x += vDist.x;
			m_AngelPos.y += vDist.y;
			m_AngelPos.z += vDist.z;

			m_AngelWorldM._41 = m_AngelPos.x;
			m_AngelWorldM._42 = m_AngelPos.y + 1.0f;
			m_AngelWorldM._43 = m_AngelPos.z;
		}

		CreateAngelParticle( m_AngelPos );
	}
}

#ifdef __EVE_BALLOON
void CMover::BalloonMoveProcess()
{
	if( m_pBalloonFlag && m_pBalloon != NULL )
	{
		D3DXVECTOR3 vDist = GetPos() - m_BalloonPos;
		FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
		
		D3DXMATRIX mScal, mRot;
		if(fDistSq > 100.0f * 100.0f) //Teleport?? ?????? ?????? ???? ???? ?????????? ??.
		{
			if(m_pBalloon)
				SAFE_DELETE(m_pBalloon);
			m_pBalloon = NULL;
			m_pBalloonFlag = FALSE;
			return;
		}
		if( fDistSq > 1.0f * 1.0f )
		{
			D3DXVECTOR3 vf1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			D3DXVECTOR3 vf2 = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
			D3DXVECTOR3 vf3;
			
			FLOAT fSerp = fDistSq / 10.0f;
			
			D3DXVec3Lerp( &vf3, &vf1, &vf2, fSerp );
			
			FLOAT fSpeed = 0.0f;
			m_BalloonWorldM = m_matWorld;			
			D3DXMatrixScaling( &mScal, 1.5f, 1.5f, 1.5f );	
			
			m_BalloonWorldM = mScal;// * m_matRotation;

			D3DXVec3Normalize( &vDist, &vDist );
			
			fSpeed = GetSpeed( m_pActMover->m_fSpeed ) * vf3.x;
			
			vDist = vDist * fSpeed;
			
			m_BalloonPos.x += vDist.x;
			m_BalloonPos.y += vDist.y;
			m_BalloonPos.z += vDist.z;
			
			m_BalloonWorldM._41 = m_BalloonPos.x;
			m_BalloonWorldM._42 = m_BalloonPos.y + m_fBalloonYPos;
			m_BalloonWorldM._43 = m_BalloonPos.z;

			if( m_bBalloon == TRUE )
			{
				m_fBalloonYPos += 0.006f;
				
				if( m_fBalloonYPos > 1.5 )
					m_bBalloon = FALSE;
			}
			else
			{
				m_fBalloonYPos -= 0.006f;
				
				if( m_fBalloonYPos < 0.5 )
					m_bBalloon = TRUE;
			}
		}
		else
		{
			m_BalloonWorldM = m_matWorld;
			D3DXMATRIX mScal;			
			D3DXMatrixScaling( &mScal, 1.5f, 1.5f, 1.5f );
			
			m_BalloonWorldM = mScal;// * m_matRotation;
			
			m_BalloonWorldM._41 = m_BalloonPos.x;
			m_BalloonWorldM._42 = m_BalloonPos.y + m_fBalloonYPos;
			m_BalloonWorldM._43 = m_BalloonPos.z;
			
			if( m_bBalloon == TRUE )
			{
				m_fBalloonYPos += 0.002f;
				
				if( m_fBalloonYPos > 1.3 )
					m_bBalloon = FALSE;
			}
			else
			{
				m_fBalloonYPos -= 0.002f;
				
				if( m_fBalloonYPos < 0.7 )
					m_bBalloon = TRUE;
			}
		}

		D3DXMatrixRotationY(&mRot,timeGetTime()/3000.0f);
		m_BalloonWorldM = mRot * m_BalloonWorldM;
	}
}
#endif //__EVE_BALLOON

void CMover::InitInterpolation()
{
	m_vScrPos	= GetPos();
	m_fScrAngle		= GetAngle();
	m_fScrAngleX	= GetAngleX();
	m_vOldRotate	= D3DXVECTOR3( 0.0F, 0.0F, 0.0F );

	D3DXMatrixTranslation( &m_matTrans, m_vScrPos.x, m_vScrPos.y, m_vScrPos.z );
	D3DXMATRIX matRotX;
	D3DXMatrixRotationX( &matRotX, D3DXToRadian( m_fScrAngleX ) );
	m_matRotation	*= matRotX;
	D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fScrAngle ), D3DXToRadian( -m_fScrAngleX ), 0 );
	SetUpdateMatrix( TRUE );
}

void CMover::Interpolate()
{
	D3DXVECTOR3 vPos;
	float fAngle, fAngleX;

	vPos	= GetPos();
	fAngle	= GetAngle();
	fAngleX	= GetAngleX();

	if( TRUE == IsActiveObj() )
	{
//		pos
		if( fabs( vPos.x - m_vScrPos.x ) > 0.001 || fabs( vPos.y - m_vScrPos.y ) > 0.001 || fabs( vPos.z - m_vScrPos.z ) > 0.001 )	// dif
		{
			m_vScrPos	= vPos;
			D3DXMatrixTranslation( &m_matTrans, m_vScrPos.x, m_vScrPos.y, m_vScrPos.z );
			SetUpdateMatrix( TRUE );
		}
//		angle
		if( fabs( fAngle - m_fScrAngle ) > 0.001 || fabs( fAngleX - m_fScrAngleX ) > 0.001 )
		{
			m_fScrAngle		= fAngle;
			D3DXMatrixRotationY( &m_matRotation, D3DXToRadian( -m_fScrAngle ) );


			m_fScrAngleX	= fAngleX;
			D3DXMATRIX matRotX;
			D3DXMatrixRotationX( &matRotX, D3DXToRadian( m_fScrAngleX ) );

			m_matRotation	*= matRotX;
			D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fScrAngle ), D3DXToRadian( -m_fScrAngleX ), 0 );
			SetUpdateMatrix( TRUE );
		}
	}
	else
	{
		float d;
//		pos
		if( fabs( vPos.x - m_vScrPos.x ) > 0.001 || fabs( vPos.y - m_vScrPos.y ) > 0.001 || fabs( vPos.z - m_vScrPos.z ) > 0.001 )	// dif
		{
			D3DXVECTOR3 v	= vPos - m_vScrPos;
			d	= D3DXVec3LengthSq( &v );
			if( d > 0.001 )
				m_vScrPos	+= v / 8;
			else
				m_vScrPos	= vPos;
			D3DXMatrixTranslation( &m_matTrans, m_vScrPos.x, m_vScrPos.y, m_vScrPos.z );
			SetUpdateMatrix( TRUE );
		}

//		angle
		if( fabs( fAngle - m_fScrAngle ) > 0.001 || fabs( fAngleX - m_fScrAngleX ) > 0.001 )
		{
			d	= fAngle - m_fScrAngle;
			if( fabs( d ) < 4 )
				m_fScrAngle		= fAngle;
			if( d < -180 )
				d	+= 360;
			else if( d > 180 )
				d	-= 360;
			
			m_fScrAngle		+= d / 8;

			if( m_fScrAngle > 360 )
				m_fScrAngle		-= 360;
			else if( m_fScrAngle < 0 )
				m_fScrAngle		+= 360;

			D3DXMatrixRotationY( &m_matRotation, D3DXToRadian( -m_fScrAngle ) );

			m_fScrAngleX	= fAngleX;
			if( m_fScrAngleX > 45 )
				m_fScrAngleX	= 45;
			else if( m_fScrAngleX < -45 )
				m_fScrAngleX	= -45;

			D3DXMATRIX matRotX;
			D3DXMatrixRotationX( &matRotX, D3DXToRadian( m_fScrAngleX ) );

			m_matRotation	*= matRotX;
			D3DXMatrixRotationYawPitchRoll( &m_matRotation, D3DXToRadian( -m_fScrAngle ), D3DXToRadian( -m_fScrAngleX ), 0 );

			if( 0 )	//if( GetProp()->dwOrthograde == 0 && fabs( m_pWorld->GetLandHeight( m_vScrPos ) - m_vScrPos.y ) < 0.1F )
			{
				D3DXVECTOR3 vScrPos	= m_vScrPos;
				D3DXVECTOR3 vec3Tri[3];
				GetWorld()->GetLandTri( vScrPos.x, vScrPos.z, vec3Tri );
				D3DXVECTOR3 vVector1 = vec3Tri[2] - vec3Tri[0];
				D3DXVECTOR3 vVector2 = vec3Tri[1] - vec3Tri[0];
				D3DXVECTOR3 vNormal;
				D3DXVec3Cross( &vNormal, &vVector1, &vVector2);	
				D3DXVec3Normalize( &vNormal, &vNormal );
				
				D3DXVECTOR3 v3Up = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
				D3DXVECTOR3 v3Cross;
				FLOAT fDot;
				FLOAT fTheta;
				D3DXVec3Cross( &v3Cross, &v3Up, &vNormal );
				fDot = D3DXVec3Dot( &v3Up, &vNormal );
				fTheta = acos( fDot );
				
				D3DXQUATERNION qDirMap;
				D3DXQuaternionRotationAxis( &qDirMap, &v3Cross, fTheta );
				
				D3DXVECTOR3 vYPW;
				QuaternionRotationToYPW( qDirMap, vYPW );

				D3DXVECTOR3 vRotate;
				vRotate.x = D3DXToDegree(vYPW.x);
				vRotate.y = D3DXToDegree(vYPW.y);
				vRotate.z = D3DXToDegree(vYPW.z);

				d	= vRotate.x - m_vOldRotate.x;
				if( d < -180 )
					d	+= 360;
				else if( d > 180 )
					d	-= 360;
	//			if( d > 0.5F )
					vRotate.x	= m_vOldRotate.x + d / 4.0F;

				if( vRotate.x > 360 )
					vRotate.x	-= 360;
				else if( vRotate.x < 0 )
					vRotate.x	+= 360;

				d	= vRotate.z - m_vOldRotate.z;
				if( d < -180 )
					d	+= 360;
				else if( d > 180 )
					d	-= 360;
	//			if( d > 0.5F )
					vRotate.z	= m_vOldRotate.z + d / 4.0F;

				if( vRotate.z > 360 )
					vRotate.z	-= 360;
				else if( vRotate.z < 0 )
					vRotate.z	+= 360;

				d	= vRotate.y - m_vOldRotate.y;
				if( d < -180 )
					d	+= 360;
				else if( d > 180 )
					d	-= 360;
	//			if( d > 0.5F )
					vRotate.y	= m_vOldRotate.y + d / 4.0F;

				if( vRotate.y > 360 )
					vRotate.y	-= 360;
				else if( vRotate.y < 0 )
					vRotate.y	+= 360;
				
				m_vOldRotate	= vRotate;

				D3DXMATRIX matRotLand;
				D3DXMatrixRotationYawPitchRoll( &matRotLand, D3DXToRadian( vRotate.y ), D3DXToRadian( vRotate.x ), D3DXToRadian( vRotate.z ) );
				m_matRotation	*= matRotLand;
			}
			SetUpdateMatrix( TRUE );
		}
	}
}
void CMover::CreateAbilityOption_SetItemSFX( int nAbilityOption )
{
	DWORD dwSfx = 0;

//	nAbilityOption = 4;
	
	switch( nAbilityOption )
	{
	case 3: dwSfx = XI_GEN_ITEM_SETITEM03; break;
	case 4: dwSfx = XI_GEN_ITEM_SETITEM04; break;
	case 5: dwSfx = XI_GEN_ITEM_SETITEM05; break;
	case 6: dwSfx = XI_GEN_ITEM_SETITEM06; break;
	case 7: dwSfx = XI_GEN_ITEM_SETITEM07; break;
	case 8: dwSfx = XI_GEN_ITEM_SETITEM08; break;
	case 9: dwSfx = XI_GEN_ITEM_SETITEM09; break;
	case 10: dwSfx = XI_GEN_ITEM_SETITEM10; break;
	}
	
	CSfxPartsLinkShoulder* pSfx = NULL;
	D3DXVECTOR3 v3Scal = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	
	if( nAbilityOption == 3 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	/*
	else
	if( nAbilityOption == 4 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3( 2.2f, 2.2f, 2.2f ));
	}
	*/
	else
	if( nAbilityOption == 4 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3( 3.0f, 3.0f, 3.0f ));
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 5 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????, ???? ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 6 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????, ???? ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 7 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????, ???? ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ??	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 8 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????, ???? ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ??	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 9 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????, ???? ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ??	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ??
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 26;	
		pSfx->SetScale(D3DXVECTOR3( 1.5f, 1.5f, 1.5f ));		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 29;
		pSfx->SetScale(D3DXVECTOR3( 1.5f, 1.5f, 1.5f ));
		pSfx->m_nOldAbilityOption = nAbilityOption;
		
		
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 10 )
	{
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????, ???? ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3( 3.0f, 4.5f, 3.0f ));
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 7;		
		pSfx->SetScale(D3DXVECTOR3( 3.0f, 3.0f, 3.0f ));		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ??	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;	
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ????
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// ??
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 26;			
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 29;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
}

#endif //__CLIENT

//int nItemKind, int nItemKind2, int nNumMax, int nUniqueMin, int nUniqueMax, int nTotalNum, CAnim* pAnimParent, int nMaterialCount )
void CMover::GenerateVendorItem( ItemProp** apItemProp, int* pcbSize, int nMax, const CVendor::CategoryItem & pVendor )
{
	CPtrArray* pItemKindAry		= prj.GetItemKindAry( pVendor.m_nItemkind3 );
	ItemProp* pItemProp		= NULL;
	int cbSizeOld	= *pcbSize;

	ASSERT( pVendor->m_nUniqueMin >= 0 );
	ASSERT( pVendor->m_nUniqueMax >= 0 );
	
	if( *pcbSize >= nMax )
		return;

	int nMinIdx	= -1, nMaxIdx	= -1;

	for( int j = pVendor.m_nUniqueMin; j <= pVendor.m_nUniqueMax; j++ )
	{
		nMinIdx		= prj.GetMinIdx( pVendor.m_nItemkind3, j );
		if( nMinIdx != -1 )
			break;
	}
	for( int j = pVendor.m_nUniqueMax; j >= pVendor.m_nUniqueMin; j-- )
	{
		nMaxIdx		= prj.GetMaxIdx( pVendor.m_nItemkind3, j );
		if( nMaxIdx != -1 )
			break;
	}
	if( nMinIdx < 0 )
	{
		WriteError( "VENDORITEM//%s//%d-%d//%d", GetName(), pVendor.m_nUniqueMin, pVendor.m_nUniqueMax, pVendor.m_nItemkind3 );
		return;
	}

	for( int k = nMinIdx; k <= nMaxIdx; k++ )
	{
		pItemProp	= (ItemProp*)pItemKindAry->GetAt( k );

		if( ( NULL == pItemProp ) ||
			( pItemProp->dwShopAble == (DWORD)-1 ) ||
			( pVendor.m_nItemJob != -1 && (DWORD)pItemProp->dwItemJob != pVendor.m_nItemJob ) )
			continue;
		
		if( *pcbSize >= nMax )
			break;

		apItemProp[*pcbSize]	= pItemProp;
		(*pcbSize)++;
	}
}



void CMover::EnforcedGhostCorr()
{
	if( m_CorrAction.fValid == TRUE && ( m_uRemnantCorrFrm - 1 ) == 0 ) {
		if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
			ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.fAngleX,
						m_CorrAction.fAccPower, m_CorrAction.fTurnAngle, 
						m_CorrAction.dwState, m_CorrAction.dwStateFlag,
						m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
						m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
		else {
			ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.dwState,
						m_CorrAction.dwStateFlag, m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
						m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
		memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	}
}

void CMover::ApproachGhostAngle()
{
	if( m_fDestAngle > -0.001f )
	{
		float f	= m_pActMover->IsFly() ? 0.601f : 4.001f;
		if( fabs( GetAngle() - m_fDestAngle ) < f ) 
		{
			if( m_CorrAction.fValid == FALSE ) 
			{
				SetAngle( m_fDestAngle );
				ClearDestAngle();
				SendActMsg( OBJMSG_STOP_TURN );
			}
			else 
			{
				ClearDestAngle();
				if( (m_CorrAction.dwStateFlag & OBJSTAF_FLY) == 0 ) 
				{
					ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.dwState,
						m_CorrAction.dwStateFlag, m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
						m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
				}
				memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
			}
		}
		else 
		{
			f = m_pActMover->IsFly() ? 0.6f * 100.0f : 0.0f;
			if( m_bLeft )
				SendActMsg( OBJMSG_LTURN, (int)f );
			else
				SendActMsg( OBJMSG_RTURN, (int)f );
		}
	}
}

// ???? ?????? ???????? ???? ?????? 
void CMover::OnArrive( DWORD dwParam1, DWORD dwParam2 )
{
#ifdef __WORLDSERVER
	if( IsPlayer() )
		((CUser* )this)->OnMsgArrival( dwParam1 );    // dwParam1 - pickup????
	else
		PostAIMsg( AIMSG_ARRIVAL, dwParam1, dwParam2 );
#endif // __WORLDSERVER
}

void CMover::OnArriveAtPos()
{  
	if( m_CorrAction.fValid == FALSE )
	{
		SendActMsg( m_dwMotionArrive );			
		OnArrive( NULL_ID, 1 );
	}
	else {
		if( m_CorrAction.fHalf )
		{
			HalfForceSet( m_CorrAction.v, m_CorrAction.vd,
								m_CorrAction.f, m_CorrAction.fAngleX,
								m_CorrAction.fAccPower, m_CorrAction.fTurnAngle );
		}
		else
		{
			if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
				ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.fAngleX,
					m_CorrAction.fAccPower, m_CorrAction.fTurnAngle, 
					m_CorrAction.dwState, m_CorrAction.dwStateFlag,
					m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
					m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
			}
			else {
				ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.dwState,
					m_CorrAction.dwStateFlag, m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
					m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
			}
		}
		memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	}
	ClearDestPos();
}

BOOL CMover::AddItem(CItemElem * const pItemBase) {
	return m_Inventory.Add(pItemBase);
}

BOOL CMover::AddItemBank(const int nSlot, CItemElem * const pItemElem) {
	return m_Bank[nSlot].Add(pItemElem);
}

CItemElem * CMover::GetItemId(const DWORD dwId) {
	return m_Inventory.GetAtId(dwId);
}

const ItemProp * CMover::GetItemIdProp(const DWORD dwId) {
	CItemElem * item = GetItemId(dwId);
	if (!item) return nullptr;
	return item->GetProp();
}

CItemElem * CMover::GetItemBankId(int nSlot, DWORD dwId) {
	return m_Bank[nSlot].GetAtId(dwId);
}

void CMover::RemoveItemId( DWORD dwId )
{
	if( m_Inventory.IsEquip( dwId ) )
		DoEquip( m_Inventory.GetAtId( dwId ), FALSE );		// ?????? ????, ???? ???? ?????? ???????? ???????? ?????? ??????.
	m_Inventory.RemoveAtId( dwId );
}

void CMover::SetKeeptimeInven( DWORD dwItemId, DWORD dwTime )
{
	int	nSize = m_Inventory.GetMax();
	for( int i = 0 ; i < nSize; i++ )
	{
		CItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem->GetProp()->dwID == dwItemId )
		{
			SetKeeptimeItem( pItemElem, dwTime );
		}
	}		
}
void CMover::SetKeeptimeBank( DWORD dwItemId, DWORD dwTime )
{
	int	nSize = m_Bank[m_nSlot].GetMax();
	for( int i = 0 ; i < nSize; i++ )
	{
		CItemElem* pItemElem = m_Bank[m_nSlot].GetAtId( i );
		if( pItemElem->GetProp()->dwID == dwItemId )
		{
			SetKeeptimeItem( pItemElem, dwTime );
		}
	}			
}
void CMover::SetKeeptimeItem( CItemElem* pItemElem, DWORD dwTime )
{	
	pItemElem->m_dwKeepTime = dwTime;
}

void CMover::RemoveItemBankId( int nSlot, DWORD dwId )
{
	m_Bank[nSlot].RemoveAtId( dwId );
}


#ifdef __CLIENT
void CMover::DialogOut( LPCTSTR lpszText )
{
	g_DialogMsg.AddMessage( this, lpszText, 0xffffffff );
}
#endif

// bSWDForce?????? Option?? ???? ?????? ????????.
// 0x01 : ????
// 0x02 : ????????(+100??????..)
// 0x04 : Motion Transition ???????? ????.
BOOL CMover::SetMotion( DWORD dwMotion, int nLoop, DWORD dwOption )
{
	CModelObject* pModel = (CModelObject*)m_pModel;
	DWORD	dwOrigMotion = dwMotion;
	ItemProp* pItemProp = GetActiveHandItemProp();
	if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) && (dwOption & MOP_FIXED) == 0 )
	{
#if defined(__CLIENT) 
		if( pItemProp == NULL )
		{
			if( IsPlayer() )
			{
				LPCTSTR szErr = Error( "CMover::SetMotion : Player %s %d %d", GetName(), IsActiveMover(), m_aEquipInfo[PARTS_RWEAPON].dwId );
				//ADDERRORMSG( szErr );
			}
			else
			{
				LPCTSTR szErr = Error( "CMover::SetMotion : NPC %s %d", GetName(), m_dwVirtItem );
				//ADDERRORMSG( szErr );
			}
			
			return FALSE;
		}
#endif // __CLIENT

		// ???? ???? dwItemKind3?? virtual ??????????.
		// ?????????? ???? ?????? ?????? ?????? ??????????, ???? virtual ?????????? ?????? ????????.
		int nIndex = GetIndex();
		if( !IsPlayer() || ( IsPlayer() && 
			( nIndex == MI_MALE || 
			  nIndex == MI_FEMALE || 
			  nIndex == MI_CROWNIBLIS || 
			  nIndex == MI_CROWNSHADE || 
			  nIndex == MI_CROWNBUBBLE || 
			  nIndex == MI_DWARPETMAS ) ) )  
		{
			BOOL bMasterPlayer = FALSE;
			
			if( nIndex == MI_CROWNIBLIS || nIndex == MI_CROWNSHADE || nIndex == MI_CROWNBUBBLE || nIndex == MI_DWARPETMAS ) 
				bMasterPlayer = TRUE;
			
			if( IsDualWeapon() && !bMasterPlayer )
			{
				dwMotion += 900;		// ???????? ???? ?????? ?????? ???? ?????? ?????? ????.
			} 
			else
			{
				BOOL bHanded = ( pItemProp->dwHanded == HD_TWO ) ? TRUE : FALSE;	// ???????? ?????????
				switch( pItemProp->dwItemKind3 )
				{
				case IK3_YOYO: 
					if( !bMasterPlayer ) dwMotion += MTI_STAND_14;
					else dwMotion += 200;
					break;
				case IK3_BOW: 
					if( !bMasterPlayer ) dwMotion += MTI_STAND_13;
					else dwMotion += 200;
					break;               
				case IK3_SWD: 
					if( bHanded )	dwMotion += 800;	
					else			dwMotion += 200; 
					break;
				case IK3_AXE: 
					if( bHanded )	dwMotion += 800;
					else			dwMotion += 500; 
					break;               
				case IK3_WAND:

					if( IsInteriorityJob(JOB_PSYCHIKEEPER) )	dwMotion += MTI_STAND_11;
					else								dwMotion += MTI_STAND_02; 
					break; 
				case IK3_CHEERSTICK: dwMotion += 400; break; 
				case IK3_STAFF: 
					dwMotion += MTI_STAND_05; 
					break; 
				case IK3_KNUCKLEHAMMER: dwMotion += 700; break; 
				default: 
					if( IsPlayer() ) dwMotion += 200; 
					break;
				}
			}
		}
	}
	if( m_dwMotion == dwOrigMotion && m_dwMotionOption == dwOption )	// ???? ?????? ?????? ??????...
	{
		if( nLoop == ANILOOP_LOOP )		return FALSE;	// ???????? ???? ?? ????
		if( pModel->m_bEndFrame == FALSE )		// ???? ?????????????????? 
			return FALSE;						// ????.
		if( pModel->m_bEndFrame && nLoop == ANILOOP_CONT )	// ???????????? ?????????? ?????????? ?????? ?????????? ????
			return FALSE;
	}
#ifdef _DEBUG
	if( dwOrigMotion == MTI_WALK )
	{
		int a = 0;
	}
#endif
	prj.m_modelMng.LoadMotion( m_pModel, m_dwType, m_dwIndex, dwMotion );
	m_dwMotion = dwOrigMotion;		// +100 ???? ???? ???? ????????.
	m_dwMotionOption = dwOption;

	pModel->m_bEndFrame = FALSE;
	pModel->SetLoop( nLoop );

	// ?? ???? ?????????? ?????? Mover???? ?????? ???? ?????? ????????.
	if(m_pModel && m_pModel->m_pModelElem && (m_pModel->m_pModelElem->m_dwIndex == MI_NPC_SNOWMAN01 || m_pModel->m_pModelElem->m_dwIndex == MI_NPC_SNOWMAN02))
		dwOption |= MOP_NO_TRANS;

	if( dwOption & MOP_NO_TRANS )		pModel->SetMotionBlending( FALSE );
	else								pModel->SetMotionBlending( TRUE );
	if( dwOption & MOP_SWDFORCE )
	{
		if( IsPlayer() )
		{
			int nOption = 0;
			DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			if( IsActiveMover() )
			{
				CItemElem *pItemElem = GetWeaponItem();
				if( pItemElem )
					nOption = pItemElem->GetAbilityOption();
			} 
			else
			{	
				// ?? ?????????? ?????????????????? ?????????? ???? ??????.
				nOption		= m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF;
			}

			if( nOption == 10 )
				dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
			else if( nOption == 9 )
				dwColor = D3DCOLOR_ARGB( 255, 255, 150, 0 );
			else if( nOption >= 7 )
				dwColor = D3DCOLOR_ARGB( 255, 198, 0, 255 );
			else if( nOption >= 5 )
				dwColor = D3DCOLOR_ARGB( 255, 123, 82, 255 );
			else if( nOption >= 3)
				dwColor = D3DCOLOR_ARGB( 255, 129, 221, 251 );
			else if( nOption >= 1 )
				dwColor = D3DCOLOR_ARGB( 255, 201, 251, 190 );
			else
				dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );

			pModel->MakeSWDForce( PARTS_RWEAPON, pItemProp->dwItemKind3, (dwOption & MOP_HITSLOW) ? TRUE : FALSE, dwColor, m_fAniSpeed );
			pModel->MakeSWDForce( PARTS_LWEAPON, pItemProp->dwItemKind3, (dwOption & MOP_HITSLOW) ? TRUE : FALSE, dwColor, m_fAniSpeed );
		}
	}
	else
	{
		if( pModel->m_pForce )
			pModel->m_pForce->Clear();
		if( pModel->m_pForce2 )
			pModel->m_pForce2->Clear();
	}

	return TRUE;
}
BOOL CMover::InitMotion(DWORD dwMotion)
{
	prj.m_modelMng.LoadMotion( m_pModel, m_dwType, m_dwIndex, dwMotion );
	m_dwMotion = dwMotion;
	return TRUE;
}


//???? ?????? ??????.
int CMover::GetEnemyCount()
{
	int nCount = 0;
	nCount = m_idEnemies.size();
	return nCount;
}

OBJID CMover::GetMaxEnemyHitID() {
	auto it = std::max_element(m_idEnemies.begin(), m_idEnemies.end(),
		[](const auto & lhs, const auto & rhs) {
			return lhs.second > rhs.second;
		});

	if (it == m_idEnemies.end()) return NULL_ID;
	return it->first;
}

// ?????????? ???? ?????? ????????.
void CMover::AddEnemy(const OBJID objid, const int nHit) {
	m_idEnemies[objid] += nHit;
}

void CMover::RemoveEnemy(const OBJID objid) {
	auto i = m_idEnemies.find(objid);
	if (i == m_idEnemies.end()) return;

	if (m_pActMover->GetObjHit() == objid)
		m_pActMover->SetObjHit(NULL_ID);

#ifdef __WORLDSERVER
	PostAIMsg(AIMSG_DSTDIE, objid);
#endif

	if (GetDestId() == objid) {
		ClearDestObj();
		SendActMsg(OBJMSG_STAND);
	}

	m_idEnemies.erase(i);
}

void CMover::RemoveAllEnemies() {
	auto range = m_idEnemies
		| std::views::keys
		| std::views::transform([](const OBJID objid) { return prj.GetMover(objid); })
		| std::views::filter(IsValidObj);

	for (CMover * const pEnemy : range) {
		pEnemy->RemoveEnemy(GetId());
	}

	m_idEnemies.clear();
}

//
// this?? ???? ?????? ????. - ???????? ???? ???????? ???????? ?????? ????????.
// Attacker?? NULL?? ???? ????.  ???????????? ?? ?????? ????.
int CMover::DoDie( CCtrl *pAttackCtrl, DWORD dwMsg )
{
#ifdef __WORLDSERVER
	if( IsPlayer() && m_nDead )
	{
		Error( "CMover::DoDie : %s ???? ?? 5?????? ????????. ???????????", GetName() );
		return 0;
	}

	// ???????????? ???????? ???? ????.
	BOOL	bBehavior = FALSE;
	CMover *pAttacker = NULL;

	if( pAttackCtrl && pAttackCtrl->GetType() == OT_MOVER )		// ???????? ???????? ???? ?????? ?????? ????.
		pAttacker = (CMover *)pAttackCtrl;

	m_bLastPK = FALSE;
	if( pAttacker && pAttacker != this && pAttacker->IsPlayer() )		// ???????? ??????, ???????????? ?????? ??????.
	{
		PVP_MODE mode = GetPVPCase( pAttacker ); 
		if( mode == PVP_MODE_PK )	// PK????
		{
#ifdef __JEFF_11_4
#endif	// __JEFF_11_4
			if( g_eLocal.GetState( EVE_PKCOST )
#ifdef __JEFF_11_4
				&& GetWorld()->IsArena() == FALSE
#endif	// __JEFF_11_4
				)
				m_bLastPK = FALSE;	// ?????? ???? 
			else
				m_bLastPK = TRUE;	// ?????? ???? ???? 						
		}
		else
			m_bLastPK = TRUE;	// ?????? ???? ???? 
		
		m_bGuildCombat = FALSE;
		if( GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
			m_bGuildCombat = TRUE;
		if( g_GuildCombat1to1Mng.IsPossibleUser( (CUser*)this ) )
			m_bGuildCombat = TRUE;
		if(IsNPC())
		{
            ((CUser*)pAttacker)->SetHonorAdd(GetIndex(),HI_HUNT_MONSTER);
		}
	}

	m_nAtkCnt = 0;		// ???? ?? ?????? ???????????? ?????? ???? ???????? ?? ?? ????????.
	
	if( IsPlayer() )
	{
		if( IsStateMode( STATE_BASEMOTION_MODE ) )
		{
			SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_OFF );
			m_nReadyTime = 0;
			m_dwUseItemId = 0;
		}

		SetStun( FALSE );		// ?????????? ???????? ???? ?????? ?????? ??????.
		SetPoison( FALSE );
		SetDark( FALSE );
		SetBleeding( FALSE );
	}
	
	BOOL fValid	= IsValidObj( this );
	
	if( IsPlayer() && m_pActMover->IsFly() )	// ???????? ???? ????????
		UnequipRide();
	
	SendActMsg( OBJMSG_STOP );
	SendActMsg( OBJMSG_DIE, dwMsg, (int)pAttacker );
	
	// ?????????? ???? ???????? ???? ???????? ???????????? ??????. ???????? ?????? ?????? ???????? ?????? ??????.
	if( IsPlayer() )
	{
		CMover *pLastHit = prj.GetMover( m_idLastHitMover );
		if( IsValidObj( pLastHit ) )
		{
			if( pLastHit->m_idAttacker == GetId() )	// pLastHit?? ???????? ???? ???????? ???????? ??????,.
				pLastHit->m_idAttacker = NULL_ID;	
			if( pLastHit->m_idTargeter == GetId() )
				pLastHit->m_idTargeter = NULL_ID;
		}

		// ?????? ?????????? ???? ????
		if( fValid )
		{
			g_dpDBClient.SendLogPlayDeath( this, pAttacker );
			// ?????????? ?????? ???? ???? ???????? ???? ?? ???? ???????? ??????.	// 0723
			if( IsValidObj( pAttacker ) == FALSE || pAttacker->IsNPC() )
//			if( m_bLastPK == FALSE && m_bGuildCombat == FALSE )
			{
				// ?????? ???? ?? ???????? ???? ????
				CPet* pPet	= GetPet();
				if( pPet && pPet->GetLevel() != PL_EGG )	// ?? '??'?? ???????? ?????? ????	// 0723
				{
					CItemElem* pItemElem	= GetPetItem();
					int nLife	= (int)pPet->GetLife();
					if( --nLife >= 0 )
					{
						// ?????? 0 ????????, ????
						pPet->SetLife( nLife );
						pPet->SetEnergy( CPetProperty::GetInstance()->GetMaxEnergy( pPet->GetLevel() ) );
	//					pPet->SetExp( 0 );

						g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_DEATH, pPet );
					}
					else
					{
						pPet->SetLife( 0 );
						pPet->SetEnergy( 0 );
						pPet->SetExp( 0 );
						pItemElem->SetFlag( CItemElem::expired );
						UpdateItem(*pItemElem, UI::Flag::Sync);

						g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_DEATH, pPet );
					}
					PetRelease();
					( (CUser*)this )->AddPetState( pItemElem->m_dwObjId , pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp() );
				}
			}
		}
		// ?????? ?????????? ???? 
		// ???????? ???????? ?????? ?????????? 
		// ?? ???????????? ?????? ??????.
	}
	
	if( bBehavior ) {
		g_UserMng.AddMoverBehavior2
			( this, GetPos(), m_pActMover->m_vDelta, GetAngle(), GetAngleX(), m_pActMover->m_fAccPower, m_pActMover->m_fTurnAngle, 
			m_pActMover->GetState(), m_pActMover->GetStateFlag(), m_dwMotion, m_pActMover->m_nMotionEx, m_pModel->m_nLoop, m_dwMotionOption, 
			g_TickCount.GetTickCount(), TRUE );
	}
	if( fValid )
	{
//		g_DPCoreClient.SendMoverDeath( this, pAttacker, dwMsg );
		g_UserMng.AddMoverDeath( this, pAttacker, dwMsg );
		
		RemoveAllEnemies();		// pAttacker->SendAIMsg( AIMSG_DSTDIE, GetId() );
	}	// fValid
	
	if( pAttacker )
		pAttacker->m_pActMover->SetObjHit( NULL_ID );
	
	// ?????? ?????? AI???? ?????? ????????.
	if( IsValidObj(pAttacker) && pAttacker->IsNPC() )
	{
		pAttacker->SendAIMsg( AIMSG_INIT_TARGETCLEAR );		// ???????? ?????? ?????????? ?????????? ??????.
	}

	// ???????? ?????? ???????? ???????? ???????? ?????? ???????? ?? ?????? ???? 
	if( IsPlayer() == FALSE && pAttacker && pAttacker->IsPlayer() )
	{
		if (pAttacker->m_quests) {
			for (QUEST & quest : pAttacker->m_quests->current) {
				const QuestProp * pQuestProp = quest.GetProp();
				if (!pQuestProp) continue;

				for (int j = 0; j < 2; ++j) {
					// ???? ???? ???????? ???? ?????? ???? NPC?????
					if (pQuestProp->m_nEndCondKillNPCIdx[j] != GetIndex())	continue;

					// ???? ???????? ????
					if (pQuestProp->m_nBeginCondParty == 2) {
						CParty * pParty = g_PartyMng.GetParty(pAttacker->m_idparty);
						// ?????????? ???? ???????? ???????? ??????..
						if (pParty && pParty->IsMember(pAttacker->m_idPlayer)) {
							for (int k = 0; k < pParty->GetSizeofMember(); ++k) {
								PartyMember * pPartyMember = &pParty->m_aMember[k];
								CMover * pMember = prj.GetUserByID(pPartyMember->m_uPlayerId);
								if (IsValidObj(pMember) && IsValidArea(pMember, 64.0f)) {
									LPQUEST pMemberQuest = pMember->GetQuest(quest.m_wId);
									if (pMemberQuest && pMemberQuest->m_nKillNPCNum[j] < pQuestProp->m_nEndCondKillNPCNum[j]) {
										++pMemberQuest->m_nKillNPCNum[j];
										((CUser *)pMember)->AddSetQuest(pMemberQuest);
									}
								}
							}
						}
					} else
						if (quest.m_nKillNPCNum[j] < pQuestProp->m_nEndCondKillNPCNum[j]) {
							++quest.m_nKillNPCNum[j];
							((CUser *)pAttacker)->AddSetQuest(&quest);
						}
				}
			}
		}

	}

#else // WORLDSERVER
 #ifndef __CLIENT
	CMover *pAttacker = NULL;
	if( pAttackCtrl && pAttackCtrl->GetType() == OT_MOVER )		// ???????? ???????? ???? ?????? ?????? ????.
		pAttacker = (CMover *)pAttackCtrl;
	SendActMsg( OBJMSG_DIE, dwMsg, (int)pAttacker );
 #endif
#endif // not WORLDSERVER


#if defined(__WORLDSERVER)
	if( GetType() == OT_MOVER )
	{
		RemoveCommonBuffs();
		g_UserMng.AddRemoveAllSkillInfluence( this );
	}

	if (m_nGuildCombatState) {
		if (IsPlayer() && pAttacker && pAttacker->IsPlayer() && GetWorld()) {
			CUser * thisPlayer = static_cast<CUser *>(this);
			CUser * attackerPlayer = static_cast<CUser *>(pAttacker);

			GetWorld()->OnDie(thisPlayer, attackerPlayer);
		}
	}

	if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( this ) )
	{
		if( IsNPC() && pAttacker->IsPlayer() )
			CSecretRoomMng::GetInstance()->SetSecretRoomKill( GetWorld()->GetID(), GetProp()->dwID );
		if( IsPlayer() && CSecretRoomMng::GetInstance()->IsGuildMaster( (CUser*)this ) )
			CSecretRoomMng::GetInstance()->SetFailGuild( (CUser*)this );
	}

	if( IsNPC() )
		CCreateMonster::GetInstance()->SetState( GetId(), 'D' );

	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( GetWorld()->GetID() ) )
	{
		if( !IsPlayer() && pAttacker->IsPlayer() )
			CInstanceDungeonHelper::GetInstance()->SetInstanceDungeonKill( GetWorld()->GetID(), static_cast<DWORD>( this->GetLayer() ), GetProp()->dwID );
	}
#ifdef __EVENTLUA_SPAWN
	if( IsNPC() )
		prj.m_EventLua.RemoveSpawnedMonster( GetId() );
#endif // __EVENTLUA_SPAWN

#endif //__WORLDSERVER	

	return 1;
}



void	CMover::EndPVP(int	nPVPHP)
{
	if(m_bPVPEnd)
		m_bPVPEnd = FALSE;		
	else
		SetPointParam( DST_HP, nPVPHP );	// 2007/11 ????(HP ?????? ????)
		//m_nHitPoint = nPVPHP;
}
//
// ?????????? ????
void	CMover::DoPVPEnd( CCtrl *pAttackCtrl, bool bWinner , DWORD dwMsg )
{
#ifdef __WORLDSERVER

	CMover *pAttacker = NULL;

	if( pAttackCtrl && pAttackCtrl->GetType() == OT_MOVER )		//  ???????? ???? ?????? ?????? ????.
		pAttacker = (CMover *)pAttackCtrl;
	else
		return;

	if( IsPlayer() )
	{
		if( IsStateMode( STATE_BASEMOTION_MODE ) )
		{
			SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_OFF );
			m_nReadyTime = 0;
			m_dwUseItemId = 0;
		}
	}
	
	SendActMsg( OBJMSG_STOP );
	pAttacker->SendActMsg( OBJMSG_STOP );
	
	m_nDead = PROCESS_COUNT * 3;		// ???? ?? 3?????? ????
	pAttacker->m_nDead = PROCESS_COUNT * 3;		// ???? ?? 3?????? ????

	// ??????????  ???? ???????? ???????????? ??????. 
	if( IsPlayer() )
	{
		CMover *pLastHit = prj.GetMover( m_idLastHitMover );
		if( IsValidObj( pLastHit ) )
		{
			if( pLastHit->m_idAttacker == GetId() )	// pLastHit?? ???????? ???? ???????? ???????? ??????,.
				pLastHit->m_idAttacker = NULL_ID;	
			if( pLastHit->m_idTargeter == GetId() )
				pLastHit->m_idTargeter = NULL_ID;
		}
	}

	m_bPVPEnd = TRUE;	
	
	if( pAttacker )
	{
		pAttacker->m_pActMover->SetObjHit( NULL_ID );
	}

#endif //__WORLDSERVER	

}

#ifdef __WORLDSERVER


int	CMover::SubPK( CMover *pAttacker, int nReflect )
{
	if( !g_eLocal.GetState( EVE_PK )
#ifdef __JEFF_11_4
		|| GetWorld()->IsArena()
#endif	// __JEFF_11_4
		)
		return 1;

	if( GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
		return 1;

	if( pAttacker->GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
		return 1;

	if( g_GuildCombat1to1Mng.IsPossibleUser( (CUser*)this ) && g_GuildCombat1to1Mng.IsPossibleUser( (CUser*)pAttacker ) )
		return 1;

	if( g_eLocal.GetState( EVE_PKCOST ) 
#ifdef __JEFF_11_4
		&& GetWorld()->IsArena() == FALSE
#endif	// __JEFF_11_4
		)
	{
		if( IsPlayer() && IsChaotic() )		
		{
#if !defined(__INTERNALSERVER)
			if( m_dwAuthorization < AUTH_HELPER )
#endif
			{
				// ?????? ???? 
				// - ???? ????( ???????? ???????? ???? - ?????????? ???????? ???????? ???? )
				CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( GetPKPropensity() );
				int nInvenDrop = xRandom( Propensity.nInvenDorpMin, Propensity.nInvenDorpMax + 1 );
				for( int i=0; i < nInvenDrop; ++i )
				{
					if( DoDropItemRandom( TRUE, pAttacker ) == 0 )	
						break;
				}

				// - ???? ????
				int nEquipDrop = xRandom( Propensity.nEquipDorpMin, Propensity.nEquipDorpMax + 1 );
				for( int i=0; i < nEquipDrop; ++i )
				{
					if( DoDropItemRandom( FALSE, pAttacker, TRUE ) == 0 )	
						break;
				}				
			}
		}				
	}

	if( !pAttacker->IsPlayer() || IsNPC() )
		return 1;

	BOOL bAdd = FALSE;
	if( pAttacker->IsChaotic() )	// ????????
	{
		if( !IsChaotic() )
			bAdd = TRUE;
	}
	else							// ????????, ????????
	{
		if( !(IsChaotic() || IsPKPink()) )
			bAdd = TRUE;
	}
	
		/// ????????, ?????? ??????
	if( bAdd && nReflect == 0 
		&& pAttacker != this ) // chipi_081119 ?????? ?????? ???? ???? ???????? ???????? ???????? ???? ????...
	{
		if( IsPlayer() && pAttacker->IsPlayer() )
		{
			if( !pAttacker->IsChaotic() )
				((CUser*)pAttacker)->AddPlaySound( SND_PC_CHAOS );
			CString szSendMsg;
			szSendMsg.Format( prj.GetText(TID_GAME_PK_DEFENDER) , pAttacker->GetName() );
			((CUser*)this)->AddText( szSendMsg );
			szSendMsg.Format( prj.GetText(TID_GAME_PK_ATTACKER) , GetName() );
			((CUser*)pAttacker)->AddText( szSendMsg );
		}

		pAttacker->SetPKValue( pAttacker->GetPKValue() + 1 );
		((CUser*)pAttacker)->AddPKValue();
		pAttacker->SetPKPropensity( pAttacker->GetPKPropensity() + NextPKPropensity( pAttacker->GetPKValue() ) );
		g_UserMng.AddPKPropensity( pAttacker );
		g_dpDBClient.SendLogPkPvp( pAttacker, this, 0, 'P' );
#ifndef __MAINSERVER
if(!pAttacker->IsPlayer())
	FILEOUT( "..\\HonorError.txt", "subpk()AddHonorListAck()\n" );
#endif // __MAINSERVER
		((CUser*)pAttacker)->SetHonorAdd(HS_PK_COUNT,HI_COUNT_CHECK);
	}
	return 1;
}

int CMover::SubDuel( CMover *pAttacker )
{
	if( m_nDuel )
		pAttacker->DuelResult( this );	// ?????? ???? ????.

	return 1;
}

int CMover::SubWar( CMover *pAttacker )
{
#ifdef __WORLDSERVER
	// ?????? ?????? ????????.
	if( g_eLocal.GetState( EVE_GUILDWAR ) == 0 )	
		return 0;

	// ???? ????????????(???????????? ?????? ??????.)
	if( m_idGuild == pAttacker->m_idGuild )
		return 0;

	// ?????????????? ??????.
	g_DPCoreClient.SendWarDead( m_idPlayer );
#endif	// __WORLDSERVER
	return 1;
}

PVP_MODE CMover::GetPVPCase( CMover *pAttacker )
{
	BOOL bPKEnable = FALSE;
	if( g_eLocal.GetState( EVE_18 ) == 1 )			// 18?? ???? 
		bPKEnable = TRUE;

	if( bPKEnable == FALSE || 
		pAttacker == NULL  || 
		pAttacker->IsNPC() || // ???????? NPC?? ?? ????.
		IsNPC() )			  // ???????? NPC?? ??????
		return PVP_MODE_NONE;

	// ???? ?????? ?????? ????????!
	if( m_idWar != WarIdNone && pAttacker->m_idWar == m_idWar )	// ???? ???? ?????? ???????? ??????.
	{
		return PVP_MODE_GUILDWAR;
	}
				
	if( m_nDuel == 0 || m_nDuelState == 300 )	// PK
	{
/*		if( GetTickCount() < m_dwPKTargetLimit )		// PK???????????????????? ?????? ????  
		{
			return PVP_MODE_NONE;
		}	*/	// mirchang 100114 ?????? ?? ???????? PK???????? ?????? ?????? ???? ??????. ???? ?????? ???? ?????????? ???????? ?????? ?????? ?????? ????..
		return PVP_MODE_PK;
	} 
	else				//???? ????.	
	{
		return PVP_MODE_DUEL; 
	}
}

// this?? ???? ???? PVP(PK, ????, ??????)????
void CMover::SubPVP( CMover *pAttacker, int nReflect )
{
	if( m_nDuel == 1 )
	{
		if( GetId() != pAttacker->m_idDuelOther )
		{
			CUser* pOther	= prj.GetUser( m_idDuelOther );
			if( IsValidObj( (CObj*)pOther ) )
			{
				pOther->AddDuelCancel( pOther->m_idDuelOther );
				pOther->ClearDuel();
			}
			( (CUser*)this )->AddDuelCancel( m_idDuelOther );
			ClearDuel();
			//return;	// mirchang 100114 ???? ?? ???? ???????? PK?? ???????? SubPK() ?? ??????..
		}
	}
	else if( m_nDuel == 2 )
	{
		if( m_idDuelParty != pAttacker->m_idparty || pAttacker->m_nDuelState == 300 )
		{
			CParty *pLoseParty = g_PartyMng.GetParty( m_idparty );

			if( pLoseParty && pLoseParty->IsLeader( m_idPlayer ) )
			{
				g_DPCoreClient.SendSetPartyDuel( m_idDuelParty, pLoseParty->m_uPartyId, FALSE );

				CParty* pParty2	= g_PartyMng.GetParty( m_idDuelParty );
				if( pParty2 )
					pParty2->DoDuelPartyCancel( pLoseParty );
				pLoseParty->DoDuelPartyCancel( pParty2 );
			}
		}
	}

	PVP_MODE mode = GetPVPCase( pAttacker );
	switch( mode )
	{
	case PVP_MODE_GUILDWAR:
		SubWar( pAttacker );
		break;
	case PVP_MODE_NONE:
	case PVP_MODE_PK:
		SubPK( pAttacker, nReflect );		// PK?? ????????.
		break;
	case PVP_MODE_DUEL:
		SubDuel( pAttacker );
		break;
	}
}

// this?? ??????????.
// this?? ???????? ???? fRange ?????? ???? ?????????? ???????? ????????.
void CMover::SubAroundExp( float fRange )
{
	static constexpr size_t MaxAroundUser = 512;

	boost::container::small_vector<CUser *, MaxAroundUser> pList;

	const D3DXVECTOR3	vPos = GetPos();

	fRange *= fRange;		// Sq ???????? ????.

	// ???????? ???? ?????? ??????.
	FOR_VISIBILITYRANGE(this) {
		CUser * pUser = USERPTR;
		const D3DXVECTOR3 vDist = vPos - pUser->GetPos();
		const FLOAT fDistSq = D3DXVec3LengthSq(&vDist);		// ???????? ?????????? ????Sq?? ???? 
		if (fDistSq <= fRange)				// ???? fRange ???? ?????? ???? ??????
		{
			pList.emplace_back(pUser);
		}
	}
	NEXT_VISIBILITYRANGE( this )

	if (pList.empty()) {
		Error("CMover::SubAroundExp %s?????? ?????? ????. ", GetName());
		return;
	}

	const EXPINTEGER nExp = GetProp()->nExpValue / static_cast<EXPINTEGER>(pList.size());	// 1???? ?????? ???????? ??????.
	// ?????? ?????????? ???????? ??.
	
	for (CUser * pUser : pList) {
		const EXPINTEGER reward = std::min(nExp, prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp);
		pUser->EarnExperience(reward, true, true);
	}
}

void CMover::AddKillRecovery()
{
	if( IsPlayer() == FALSE )	return; // this(Attacker)?? ???????? ????????.
	
	int nHPPoint = GetParam( DST_KILL_HP, 0 );
	int nMPPoint = GetParam( DST_KILL_MP, 0 );
	int nFPPoint = GetParam( DST_KILL_FP, 0 );
	
	BOOL bSfx = FALSE;
	if( nHPPoint )
	{
		int nHP	   = GetPointParam( DST_HP );
		int nHPMax = GetMaxPoint( DST_HP );
		if( nHP + nHPPoint >= nHPMax )	// ?????? ???????? ?????????? ????.
			nHP = nHPMax;
		else
			nHP = nHP + nHPPoint;
		SetPointParam( DST_HP, nHP );
		bSfx = TRUE;
	}
	
	if( nMPPoint )
	{
		int nMP	   = GetPointParam( DST_MP );
		int nMPMax = GetMaxPoint( DST_MP );
		if( nMP + nMPPoint >= nMPMax )	// ?????? ???????? ?????????? ????.
			nMP = nMPMax;
		else
			nMP = nMP + nMPPoint;
		SetPointParam( DST_MP, nMP );
		bSfx = TRUE;
	}
	
	if( nFPPoint )
	{
		int nFP	   = GetPointParam( DST_FP );
		int nFPMax = GetMaxPoint( DST_FP );
		if( nFP + nFPPoint >= nFPMax )	// ?????? ???????? ?????????? ????.
			nFP = nFPMax;
		else
			nFP = nFP + nFPPoint;
		SetPointParam( DST_FP, nFP );
		bSfx = TRUE;
	}
	
	if( bSfx )
		g_UserMng.AddCreateSfxObj( this, XI_KILL_RECOVERY );
}

// pDead?? ?????? ?????? ????
int CMover::SubExperience( CMover *pDead )
{
	if( IsPlayer() == FALSE )	return 0;	// this(Attacker)?? ???????? ????????.
	if( pDead->IsPlayer() )	return 0;			// ???????? ?????????? ?????? ?????????? ????.
	
	MoverProp* pMoverProp	= pDead->GetProp();
	ASSERT( pMoverProp );

#ifdef __WORLDSERVER
	EXPFLOAT fExpValue = 0;
	float	fFxpValue = 0.0f;
	#ifdef __S1108_BACK_END_SYSTEM
//			fExpValue	= pMoverProp->nExpValue * static_cast<EXPFLOAT>( prj.m_fMonsterExpRate ) * static_cast<EXPFLOAT>( pMoverProp->m_fExp_Rate );
//			fFxpValue	= pMoverProp->nFxpValue * prj.m_fMonsterExpRate * pMoverProp->m_fExp_Rate;
			fExpValue	= pMoverProp->nExpValue * static_cast<EXPFLOAT>( pMoverProp->m_fExp_Rate );
			fFxpValue	= pMoverProp->nFxpValue * pMoverProp->m_fExp_Rate;
	#else // __S1108_BACK_END_SYSTEM
//			fExpValue	= pMoverProp->nExpValue * static_cast<EXPFLOAT>( prj.m_fMonsterExpRate );
// 			fFxpValue	= pMoverProp->nFxpValue * prj.m_fMonsterExpRate;
			fExpValue	= pMoverProp->nExpValue;
 			fFxpValue	= pMoverProp->nFxpValue;
	#endif // __S1108_BACK_END_SYSTEM
#else	// __WORLDSERVER
	EXPFLOAT fExpValue	= static_cast<EXPFLOAT>( pMoverProp->nExpValue );
	float	fFxpValue	= pMoverProp->nFxpValue;
#endif	// __WORLDSERVER

	AddExperienceKillMember( pDead, fExpValue, pMoverProp, fFxpValue );
	return 1;
}

bool CMover::IsValidArea(const CMover * const pMover, const float fLength) const {
	if (!IsValidObj(this)) return false;
	if (!IsValidObj(pMover)) return false;

	/*2008.04.11 ???? ???? ????*/
	if (GetWorld() != pMover->GetWorld()) return false;

#ifdef __LAYER_1015
	// 2008.12.26 ?????? ????
	if (GetLayer() != pMover->GetLayer()) return false;
#endif

	const D3DXVECTOR3 vDist = pMover->GetPos() - GetPos();
	const float fDist = D3DXVec3LengthSq( &vDist );
	return fDist < fLength * fLength;
}

#endif	// __WORLDSERVER not client




// ?????????? ???????? ??????.
BOOL CMover::IsAttackAble( CObj *pObj )
{
	if( HasBuffByIk3(IK3_TEXT_DISGUISE) )
		return FALSE;

	BOOL bAble	= FALSE;	

	CWorld* pWorld = GetWorld();
	if( pObj )
	{
		if( IsStateMode( STATE_BASEMOTION_MODE ) )	
		{
		#ifdef __CLIENT
			g_DPlay.SendStateModeCancel( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
		#else
			SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
			m_nReadyTime = 0;
			m_dwUseItemId = 0;
		#endif 
			return FALSE;		
		}
		
		if( pObj->GetType() == OT_MOVER )
		{
		#ifdef __CLIENT
			CMover *pMover = (CMover *)pObj;

				if( pMover->IsPlayer() )
				{
					if( g_eLocal.GetState( EVE_18 ) )
					{
						if( IsPVPTarget( pMover ) )			// pvp
						{
							if( m_nDuelState == 1 && !IsFly() && !pMover->IsFly() )
								bAble	= TRUE;
						}
						else if( IsWarTarget( pMover ) )	// war
							bAble	= TRUE;
						else if( IsSchoolTarget( pMover ) )	// event
							bAble	= TRUE;
#ifdef __JEFF_11_4
						else if( IsArenaTarget( pMover ) )
							bAble	= TRUE;
#endif	// __JEFF_11_4
						else								// pk
						{
							bAble	= IsPKAttackAble( pMover );
							if( bAble )
								( (CWndWorld *)g_WndMng.m_pWndWorld )->SetLastTarget( pMover->GetId() );
						}
					}

/*#if __VER >= 8 // 8?? ???? 061226 ma
					if( IsPVPTarget( pMover ) )			// pvp
					{
						if( m_nDuelState == 1 && !IsFly() && !pMover->IsFly() )
							bAble	= TRUE;
					}
#endif // __VER >= 8 // 8?? ???? 061226 ma*/

					if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
					{
						if( pWorld->GetID() == WI_WORLD_GUILDWAR )
						{
							if( IsGuildCombatTarget( pMover ) ) // GuildCombat
								bAble = TRUE;
						}
					}
 					if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
 					{
 						if( g_GuildCombat1to1Mng.IsPossibleMover( pMover ) && IsGuildCombatTarget( pMover ) )
 							bAble = TRUE;
 					}
				}
				else
				{
					bAble	= IsAttackAbleNPC( pMover );
				}
		#else // __CLIENT			
				if( GetHitType( (CMover *)pObj, TRUE, 0 ) == HITTYPE_FAIL )
					bAble = FALSE;
				else
					bAble = TRUE;
		#endif // __CLIENT
		}
		else
		{
			// no mover
			bAble	= TRUE;
		}
		
	#ifdef __CLIENT
		if( !bAble )
			( (CWndWorld *)g_WndMng.m_pWndWorld )->m_bAutoAttack	= FALSE;
	#endif
	}
	return bAble;
}

BOOL CMover::IsAttackAbleNPC( CMover* pNPC )
{
	if( HasBuffByIk3(IK3_TEXT_DISGUISE) )
		return FALSE;
	
	BOOL bAble = TRUE;
	if( pNPC->IsDie() )	// ???? ???????? ?????? ?????????? ?????? ???????? ????.
	{
	#ifdef __CLIENT
		g_WorldMng()->SetObjFocus( NULL );
	#else
		if( IsPlayer() )
			((CUser *)this)->AddSetTarget( NULL_ID );
	#endif
		bAble = FALSE;
	} 
	else
	{
		MoverProp *pMoverProp = pNPC->GetProp();	

		if( pMoverProp->bKillable == 0 )	
			return FALSE;		// ?????????????? NPC?? ?????? ????????.
		
		// ?????????? ??????.
		if( pNPC->IsPeaceful() == TRUE )	// ???????????? ???? ????
		{
			bAble = FALSE;
		}
		else
		{
			if( pMoverProp->dwClass == RANK_GUARD )					// ?????????? ?????? ????????.  
			{
				if( pMoverProp->nChaotic < 0 && IsChaotic() )			// ?????? ???? ?????? ????????.
					bAble = FALSE;
				else if( pMoverProp->nChaotic > 0 && !IsChaotic() )		// ?????? ???? ?????? ????????.
					bAble = FALSE;
			}

			if( m_pActMover->IsFly() && pMoverProp->dwFlying == 0 )	// ???????? ?????????? ?????? ???????? ??????
				bAble = FALSE;
			else if( m_pActMover->IsFly() == FALSE && pMoverProp->dwFlying == 1 )	// ?????? ???? / ???? ??????
				bAble = FALSE;
		}
	}

#ifdef __CLIENT
	if( bAble == TRUE )
		if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )	// NPC???? ???????? ?????? ?????? ???????? ????????.
			((CWndWorld *)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
#endif // Client
				
	return bAble;
}

#ifdef __CLIENT
BOOL CMover::IsPKAttackAble( CMover* pMover )
{
#ifdef __JEFF_11_4
	if( IsArenaTarget( pMover ) )
		return TRUE;
#endif	// __JEFF_11_4
	if( !g_eLocal.GetState( EVE_PK ) )
		return FALSE;
	return IsPKInspection( pMover );
}

BOOL CMover::IsPKInspection( CMover* pOther )
{

	DWORD dwRegionAttr	= GetPKPVPRegionAttr();
	BOOL bAble = TRUE;

	int nAttackerPK, nDefenderPK;
	nAttackerPK = nDefenderPK = 0;
	if( nAttackerPK = IsPKPVPInspectionBase( dwRegionAttr, FALSE ) )
		bAble	= FALSE;
	if( nDefenderPK = pOther->IsPKPVPInspectionBase( dwRegionAttr, FALSE ) )
		bAble	= FALSE;

	if( bAble == FALSE && ( pOther->IsChaotic() || (GetAsyncKeyState(VK_CONTROL) & 0x8000)) )
	{
		if(GetWorld() && GetWorld()->GetID() == WI_WORLD_GUILDWAR && IsGuildCombatTarget( pOther ) )	// ???????? ???? ?????????? ???????? ???????? ????
			nAttackerPK = nDefenderPK = 9;
 		else if( g_GuildCombat1to1Mng.IsPossibleMover( this ) )
 			nAttackerPK = nDefenderPK = 9;
		else if(!IsFly() && !pOther->IsFly())
		{
			CWndWorld* pWndWorld	= (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			if( pWndWorld && pWndWorld->m_bLButtonDown && !pWndWorld->m_bLButtonDowned )
			{
				if( nAttackerPK == 1 || nDefenderPK == 1 )
					g_WndMng.PutString( prj.GetText(TID_GMAE_PK_NOT_AREA), NULL, prj.GetTextColor(TID_GMAE_PK_NOT_AREA) );		
				else if( nAttackerPK == 5 )
					g_WndMng.PutString( prj.GetText(TID_GAME_PK_LIMIT_LEVEL0), NULL, prj.GetTextColor(TID_GAME_PK_LIMIT_LEVEL0) );
				else if( nDefenderPK == 5 )
					g_WndMng.PutString( prj.GetText(TID_GAME_PK_LIMIT_LEVEL1), NULL, prj.GetTextColor(TID_GAME_PK_LIMIT_LEVEL1) );
			}
		}		
	}
	if( bAble )
	{
		if( pOther->m_vtInfo.IsVendorOpen() )
		{
			if( IsForceAttack() )
			{
				;
			}
			else if( !( g_Neuz.m_NeuzEnemy.IsPKing( pOther->GetId() ) ) )
				bAble	= FALSE;
		}
		else
		{
			if( dwRegionAttr == RA_PENALTY_PK )
			{
				if( IsForceAttack() )
				{
					;
				}
				else if( !pOther->IsChaotic() )
				{
					bAble	= FALSE;
				}
			}
		}
	}

	return bAble;
}
#endif // CLIENT

#ifdef __WORLDSERVER
BOOL CMover::IsPVPInspection( CMover* pMover, int nFlag )
{
	int nError	= 0;

	DWORD dwAttr	= GetPKPVPRegionAttr();
	DWORD dwWorldIDtmp = 0;
	DWORD dwDestWorldIDtmp = 1;

	if( GetWorld() && pMover->GetWorld() )
	{
		dwWorldIDtmp = GetWorld()->GetID();
		dwDestWorldIDtmp = pMover->GetWorld()->GetID();
	}
	
	switch( nFlag )
	{
		case 1:		// ???? PVP
			{
//				1
				if( abs( GetLevel() - pMover->GetLevel() ) >= 30 )	// ???? 30???? ??????
				{
					((CUser*)this)->AddDefinedText( TID_PK_LEVEL_GAP, "" );	// ?????? 30???? ?????? ???? PVP?? ???? ????????
					return FALSE;
				}
//				1	// 2	// 3	// 4	// 5

				if( ( nError = IsPVPInspectionBase( dwAttr,dwDestWorldIDtmp ) ) == 0 )
				{
					nError = pMover->IsPVPInspectionBase( dwAttr ,dwWorldIDtmp);
				}
				if( nError != 0 )
				{
					switch( nError )
					{
					case 1:
						((CUser*)this)->AddDefinedText( TID_PK_SAFETY_NO, "" );	// ?????????????? PVP???? ????????
						break;
					case 2:
						((CUser*)this)->AddDefinedText( TID_PK_SAME_NO, "" );	// ?????????? ?????? PVP?? ???? ????????
						break;
					case 3:
						((CUser*)this)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1?????? ?????? PVP?? ???? ????????
						break;
					}
					return FALSE;
				}
				return TRUE;
			}
		case 2:		// ???? PVP
			{
				CParty* pSrc	= g_PartyMng.GetParty( m_idparty );
				CParty* pDest	= g_PartyMng.GetParty( pMover->m_idparty );
				if( pSrc && pDest )
				{
					DWORD dwTick	= GetTickCount();
					u_long anPlayer[MAX_PTMEMBER_SIZE*2];
					int	nSize	= 0;
					for( int i = 0; i < pSrc->GetSizeofMember(); i++ )
						anPlayer[nSize++]	= pSrc->GetPlayerId( i );
					for( int i = 0; i < pDest->GetSizeofMember(); i++ )
						anPlayer[nSize++]	= pDest->GetPlayerId( i );

					CUser* pLeaderSrc	= (CUser*)pSrc->GetLeader();
					CUser* pLeaderDest	= (CUser*)pDest->GetLeader();
					if( IsValidObj( (CObj*)pLeaderSrc ) && IsValidObj( (CObj*)pLeaderDest ) )
					{
						if( abs( pLeaderSrc->GetLevel() - pLeaderDest->GetLevel() ) >= 30 )
						{
							((CUser*)this)->AddDefinedText( TID_PK_LEVEL_GAP, "" );	// ?????? 30???? ?????? ???? PVP?? ???? ????????
							return FALSE;
						}
					}
					else
					{
						//
						return FALSE;
					}

					for( int i = 0; i < nSize; i++ )
					{
						CUser* pPlayer	= g_UserMng.GetUserByPlayerID( anPlayer[i] );
						if( IsValidObj( (CObj*)pPlayer ) )
						{
							if( ( nError = pPlayer->IsPVPInspectionBase( dwAttr,dwWorldIDtmp ) ) )
							{
								if( nError != 0 )
								{
									switch( nError )
									{
									case 1:
										((CUser*)this)->AddDefinedText( TID_PK_SAFETY_NO, "" );	// ?????????????? PVP???? ????????
										break;
									case 2:
										((CUser*)this)->AddDefinedText( TID_PK_SAME_NO, "" );	// ?????????? ?????? PVP?? ???? ????????
										break;
									case 3:
										((CUser*)this)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1?????? ?????? PVP?? ???? ????????
										break;
									}
									return FALSE;
								}
							}
						}
					}
					return TRUE;
				}
				return FALSE;
			}
	}
	return TRUE;
}


#endif	// __WORLDSERVER

int CMover::IsPKPVPInspectionBase( DWORD dwRegionAttr, BOOL bPVP )
{
	DWORD dwAttr = GetPKPVPRegionAttr();

	if( dwRegionAttr == RA_SAFETY )		// ???????? ????
		return 1;
	if( dwRegionAttr != dwAttr )		// ?????????? ?????? ????
		return 2;
	
#ifdef __CLIENT
	if( GetAsyncKeyState( VK_MENU ) & 0x8000 )
		return 9;
#endif	// __CLIENT


	if( bPVP && (dwAttr == RA_FIGHT || dwAttr != RA_PK) )	// PVP?? Penalty ???????? ????
	{
		if( IsBaseJob() )
			return 3;
	}

	if( !bPVP && GetLevel() <= prj.m_PKSetting.nLimitLevel )	// PK
		return 5;

	if( bPVP )
	{
		if( m_nDuelState != 0 )
			return 9;
	}

	if( IsFly() )
		return 1;
	if (m_idWar != WarIdNone) return 1;

	return 0;
}

//8?? ???????? ???????? PVP??????????   Neuz, World
int CMover::IsPVPInspectionBase( DWORD dwRegionAttr,DWORD dwWorldID, BOOL bPVP )
{

#ifdef __CLIENT
	if( GetAsyncKeyState( VK_MENU ) & 0x8000 )
		return 9;
#endif	// __CLIENT


	if( IsBaseJob() )
		return 3;

	if( bPVP )
	{
		if( m_nDuelState != 0 )
			return 9;
	}

	if(GetWorld() && GetWorld()->GetID() == WI_WORLD_GUILDWAR)
		return	10;

	if(GetWorld() && GetWorld()->GetID() != dwWorldID)
		return	2;

	if( IsFly() )
		return 1;

	if (m_idWar != WarIdNone) return 1;

#ifdef __QUIZ
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_QUIZ )
		return	1;
#endif // __QUIZ

	return 0;
}


BOOL CMover::SubPKPVPInspectionBase( CMover* pMover, CMover* pMover1, DWORD dwPKAttr, int nFlag )
{
	if( IsBaseJob() ) // ?????? ??????
	{
#ifdef __CLIENT
		g_WndMng.PutString( prj.GetText( TID_PK_CHANGEJOB_NO ), NULL, prj.GetTextColor( TID_PK_CHANGEJOB_NO ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
		((CUser*)this)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1?????? ?????? PVP?? ???? ????????
		if( IsValidObj( pMover ) )
			((CUser*)pMover)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1?????? ?????? PVP?? ???? ????????
#endif // __WORLDSERVER
//		pMover->PrintString( pMover1, TID_GAME_PVP_LOWLEVEL2 );
		return FALSE;
	}
	if( IsFly() )	// ???? ????
	{
#ifdef __CLIENT
		g_WndMng.PutString( prj.GetText( TID_PK_NOFLIGHT_DURING ), NULL, prj.GetTextColor( TID_PK_NOFLIGHT_DURING ) ); // ?????????? ?????? ???? ????????
#endif // __CLIENT
#ifdef __WORLDSERVER
		((CUser*)this)->AddDefinedText( TID_PK_NOFLIGHT_DURING, "" );	// ?????????? ?????? ???? ????????
		if( IsValidObj( pMover ) )
			((CUser*)pMover)->AddDefinedText( TID_PK_NOFLIGHT_DURING, "" );	// ?????????? ?????? ???? ????????
#endif // __WORLDSERVER		
//		pMover->PrintString( pMover1, TID_GAME_PVP_LOWLEVEL2 );
		return FALSE;
	}

#ifdef __WORLDSERVER
	if( m_idWar != WarIdNone )	// ???????? ????, PK ????.
	{
		if( g_eLocal.GetState( EVE_GUILDWAR ) )
		{
			pMover->PrintString( pMover1, TID_GAME_GUILDWARERRORDUEL );	// ???????? ???? ??????.
			return FALSE;
		}
	} 
	else
	{
		if( m_nDuelState != 0 )
		{
			return FALSE;
		}
	}
#endif __WORLDSERVER

	if( IsRegionAttr( dwPKAttr ) == FALSE )	// ???? ?????? ?????? ????
	{
#ifdef __CLIENT
		g_WndMng.PutString( prj.GetText( TID_PK_SAME_NO ), NULL, prj.GetTextColor( TID_PK_SAME_NO ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
		((CUser*)this)->AddDefinedText( TID_PK_SAME_NO, "" );	// ?????????? ?????? PVP?? ???? ????????
		if( IsValidObj( pMover ) )
			((CUser*)pMover)->AddDefinedText( TID_PK_SAME_NO, "" );	// ?????????? ?????? PVP?? ???? ????????
#endif // __WORLDSERVER	
//		pMover->PrintString( pMover1, TID_GAME_PVP_SAFEZONENOATTACK );	//g_WndMng.PutString( prj.GetText( TID_GAME_PVP_SAFEZONENOATTACK ), NULL, prj.GetTextColor( TID_GAME_PVP_SAFEZONENOATTACK ) );
		return FALSE;
	}
	return TRUE;
}

void CMover::PrintString( CMover* pMover, DWORD dwId )
{
#ifdef __CLIENT
	g_WndMng.PutString( prj.GetText( dwId ), NULL, prj.GetTextColor( dwId ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
	((CUser*)this)->AddDefinedText( dwId, "" );
	if( IsValidObj( pMover ) )
		((CUser*)pMover)->AddDefinedText( dwId, "" );
#endif // __WORLDSERVER
}
// this?? ??????
// this?? pTarget?? ???????????? ????.
// pTarget?? ?????? IsValid ?????? ???? ???????? ????.
#ifdef __WORLDSERVER
int CMover::IsSteal( CMover *pTarget )
{
	if( IsNPC() || ::GetLanguage() == LANG_JAP )	
		return 0;		// ???????? NPC?? ???? ???????? ????.

	if( ::GetLanguage() == LANG_THA && pTarget->GetIndex() == MI_MRPUMPKIN5 )	// ?????? ???? ???? ???????????? ?????? ??????.
		return 0;												

	if( pTarget->GetIndex() == MI_DEMIAN5 || pTarget->GetIndex() == MI_KEAKOON5 || pTarget->GetIndex() == MI_MUFFRIN5 )
		return 0;		// ?????? ?????????????? ?????? ???? ????
		
	// ?????????? ?????? ????????.
	if( pTarget->IsNPC() )	// ??????:????????, ??????:NPC 
	{
		BOOL bStealCheck = TRUE;
		if( pTarget->m_idTargeter == NULL_ID )	
		{
			return 0;	//???????? ???????? ???? ?????? ?????? ????????.
		}
		CMover *pOtherPlayer = prj.GetMover( pTarget->m_idTargeter );		// pTarget?? ???? ???? ????.
		if( IsValidObj(pOtherPlayer) )
		{
			// ???????? ?????? ???? pTarget?? ?????? ?????? ???? ?????? ?????? ??????.
			if( m_idparty && (pOtherPlayer->m_idparty == m_idparty) )
				bStealCheck = FALSE;
			if( m_idWar != WarIdNone && m_idGuild && (pOtherPlayer->m_idGuild == m_idGuild) ) // ???????? ???????? ???????? ?????? ?????? ??????.
				bStealCheck = FALSE;
			if( g_eLocal.GetState( EVE_SCHOOL ) )
				bStealCheck	= FALSE;
			if( pTarget->GetProp()->dwClass == RANK_SUPER )		// ???????? ????????.
				bStealCheck = FALSE;
		} else
			bStealCheck = FALSE;	// pTarget?? ?????? ?????? ???????? ?????? ?????? ??????.

		if( bStealCheck && 
			pTarget->m_idTargeter != NULL_ID && pTarget->m_idTargeter != GetId()) // pTarget?? ???????? ???? ???? ???????????? ???????? ????.
		{
			((CUser*)this)->AddDefinedText( TID_GAME_PRIORITYMOB, "" );	// ???????? ???????????? ??????????.
			return 1;		// ????.
		}
	}

	return 0;	// ?????? ????.
}


// 
// ???????? ?????? ???? ????.
//
void  CMover::GetDieDecExp( int nLevel, FLOAT& fRate, FLOAT& fDecExp, BOOL& bPxpClear, BOOL& bLvDown )
{
	bPxpClear = FALSE;

	// HP????????(%)
	DWORD	veci = 0;
	for( veci = 0 ; veci < prj.m_vecRevivalPenalty.size() ; ++veci )
	{
		if( veci == 0 && nLevel <= prj.m_vecRevivalPenalty[veci].nLevel )
			break;
		if( veci != 0 && prj.m_vecRevivalPenalty[veci-1].nLevel < nLevel && nLevel <= prj.m_vecRevivalPenalty[veci].nLevel )
			break;
	}
	if( prj.m_vecRevivalPenalty.size() <= veci )
	{
		veci = prj.m_vecRevivalPenalty.size()-1;
	}
	fRate = (float)prj.m_vecRevivalPenalty[veci].nValue / 10000.0f;

	// ??????????(%)
	for( veci = 0 ; veci < prj.m_vecDecExpPenalty.size() ; ++veci )
	{
		if( veci == 0 && nLevel <= prj.m_vecDecExpPenalty[veci].nLevel )
			break;
		if( veci != 0 && prj.m_vecDecExpPenalty[veci-1].nLevel < nLevel && nLevel <= prj.m_vecDecExpPenalty[veci].nLevel )
			break;
	}
	if( prj.m_vecDecExpPenalty.size() <= veci )
	{
		veci = prj.m_vecDecExpPenalty.size()-1;
	}
	fDecExp = (float)prj.m_vecDecExpPenalty[veci].nValue / 10000.0f;

	// ????????(??/??)
	for( veci = 0 ; veci < prj.m_vecLevelDownPenalty.size() ; ++veci )
	{
		if( veci == 0 && nLevel <= prj.m_vecLevelDownPenalty[veci].nLevel )
			break;
		if( veci != 0 && prj.m_vecLevelDownPenalty[veci-1].nLevel < nLevel && nLevel <= prj.m_vecLevelDownPenalty[veci].nLevel )
			break;
	}
	if( prj.m_vecLevelDownPenalty.size() <= veci )
	{
		veci = prj.m_vecLevelDownPenalty.size()-1;
	}
	bLvDown = (prj.m_vecLevelDownPenalty[veci].nValue)? TRUE : FALSE;

}
void CMover::GetDieDecExpRate( FLOAT& fDecExp, DWORD dwDestParam, BOOL bResurrection )
{
	// ????????/???????? ?????????? ?????? ????.
	if( dwDestParam != 0 )
	{
		FLOAT fAddDec = (float)(100 - dwDestParam) / 100.0f;
		fDecExp = fDecExp - (fDecExp * fAddDec);
	}
	
	if( IsChaotic() && ( IsSMMode( SM_REVIVAL ) || bResurrection ) )
		fDecExp = fDecExp * 0.9f;
	else if( IsSMMode( SM_REVIVAL ) )
		fDecExp = 0.0f;
}

float CMover::SubDieDecExp( BOOL bTransfer, DWORD dwDestParam, BOOL bResurrection )
{
	if( IsNPC() )	
		return 0.0f;

	float fRate = 0.1f, fDecExp = 0.0f;
	BOOL  bPxpClear = FALSE, bLvDown = FALSE;
	int nLevel	= GetLevel();

	GetDieDecExp( nLevel, fRate, fDecExp, bPxpClear, bLvDown );

	if( m_bLastPK )		// ?????? ?????? ????????...
		return fRate;

	if( m_bGuildCombat )
		return fRate;

	if( m_bLastDuelParty )
	{
		m_bLastDuelParty = FALSE;
		return fRate;
	}

//#if __VER < 8  
	bLvDown = FALSE;	// ???? ?? ?????? ???????? ???? ???? ???? ????
//#endif //  __VER < 8  

	if( fDecExp )
	{
		if( IsAfterDeath() == FALSE ) 
		{
			m_nDeathExp = m_nExp1;		// ???? ???????? ?????? ?? ???????? ???? 
			m_nDeathLevel = m_nLevel;	// ???? ???? ???? 
		}

		GetDieDecExpRate( fDecExp, dwDestParam, bResurrection );


		if( bResurrection == FALSE && IsSMMode( SM_REVIVAL ) )
			((CUser*)this)->SetSMMode( SM_REVIVAL, 0 );

		bLvDown = DecExperiencePercent( fDecExp, bPxpClear, bLvDown );

		if( bTransfer )
		{
			if( bLvDown )	// ???? ?????? ?????????
			{
				g_UserMng.AddSetLevel( this, (WORD)m_nLevel );		// this?????? ?????? ?????????????? ????.
				((CUser *)this)->AddSetGrowthLearningPoint( m_nRemainGP );		// ???????????? GP?????????? ????.
			}
			else
				( (CUser *)this )->AddSetExperience( GetExp1(), (WORD)m_nLevel,  m_nSkillPoint, m_nSkillLevel, m_nDeathExp, (WORD)m_nDeathLevel );		// ???????????? exp1,exp2?????? ?????? ????.
		}
	}
	
	return fRate;	// ???????? ?????? HP?? ??????????.
}

#endif // WORLDSERVER



BOOL CMover::DropItemByDied( CMover* pAttacker )
{
	BOOL bResult;
	OBJID objid = GetMaxEnemyHitID();
	if( objid != NULL_ID )
	{
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) )
			bResult = DropItem( pMover );
		else
			objid = NULL_ID;
	}

	if( objid == NULL_ID )
		bResult = DropItem( pAttacker );

	return bResult;
}

// ???? ?????? ???????? ???????? ????
// ?????? ???????? ??????????.
// ?????? ?????? ?????????????? this??.
BOOL CMover::DropItem( CMover* pAttacker )
{
		MoverProp* lpMoverProp	= GetProp();
#ifdef __WORLDSERVER
	if( pAttacker->IsPlayer() && IsNPC() )
	{
#ifdef __VTN_TIMELIMIT
		//	mulcom	BEGIN100315	?????? ???? ????
 		if( ::GetLanguage() == LANG_VTN )
 		{
			if( pAttacker->m_nAccountPlayTime > MIN( 300 ) )
			{
				return	TRUE;
			}
		}
		//	mulcom	END100315	?????? ???? ????
#endif // __VTN_TIMELIMIT

		if( !lpMoverProp )
			return FALSE;

		if( m_nQuestKeeping > 0 )
		{
			CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
			GroupQuest::QuestElem * pElem	= pProcessor->GetQuest( m_nQuestKeeping );
			if (pElem && pElem->objidWormon == GetId()) {
				CGuild * pGuild = pAttacker->GetGuild();
				if (pGuild) {
					g_dpDBClient.SendUpdateGuildQuest(pGuild->m_idGuild, pElem->nId, pElem->ns);

					pGuild->SetQuest(pElem->nId, pElem->ns);
					pElem->SetGetItemState();
				}
			}
		}

		if( m_nPartyQuestKeeping > 0 )
		{
			CPartyQuestProcessor* pProcessor	= CPartyQuestProcessor::GetInstance();
			GroupQuest::QuestElem * pElem	= pProcessor->GetQuest( m_nPartyQuestKeeping );
			if (pElem && pElem->objidWormon == GetId()) {
				CParty * pParty = g_PartyMng.GetParty(pAttacker->m_idparty);
				if (pParty) {
					pElem->SetGetItemState();
					pProcessor->SendQuestLimitTime(GroupQuest::ProcessState::GetItem, MIN(20), pAttacker->m_idparty);
				}
			}
		}

		if( ( pAttacker->m_nLevel - (int)lpMoverProp->dwLevel ) < 10 )
		{
			int nNum	= 0;
			CEventItem* pEventItem	= CEventGeneric::GetInstance()->GetItem( &nNum );
			while( pEventItem )
			{
#ifdef __BUGFIX_0326
				if( lpMoverProp->dwFlying )	//
				{
					CItemElem itemElem;
					itemElem.m_dwItemId	= pEventItem->m_dwItemId;
					itemElem.m_nItemNum		= nNum;
					itemElem.SetSerialNumber();
					if( pAttacker->CreateItem( &itemElem ) == TRUE )
					{
						ItemProp* pItemProp	= itemElem.GetProp();
						if( pItemProp )
							( (CUser*)pAttacker )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
					}
				}
				else
#endif	// __BUGFIX_0326
				{
					CItemElem* pItemElem	= new CItemElem;
					pItemElem->m_dwItemId	= pEventItem->m_dwItemId;
					pItemElem->m_nItemNum	= nNum;
					pItemElem->m_nHitPoint	= -1;
					pItemElem->SetSerialNumber();
					CItem* pItem	= new CItem;
					pItem->m_pItemBase	= pItemElem;
					pItem->m_idOwn	= pAttacker->GetId();
					pItem->m_dwDropTime		= timeGetTime();
					pItem->m_bDropMob	= TRUE;
					pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
					D3DXVECTOR3 vPos = GetPos();
					vPos.x += ( xRandomF(2.0f) - 1.0f );
					vPos.z += ( xRandomF(2.0f) - 1.0f );
					vPos.y = GetPos().y;
					pItem->SetPos( vPos );
					GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
				}
				pEventItem	= CEventGeneric::GetInstance()->GetItem( &nNum );
			}
			std::map<DWORD, int> mapItemList = prj.m_EventLua.GetItem( lpMoverProp->dwLevel );

			for( auto it=mapItemList.begin(); it!=mapItemList.end(); it++ )
			{
	#ifdef __BUGFIX_0326
				if( lpMoverProp->dwFlying )	//
				{
					CItemElem itemElem;
					itemElem.m_dwItemId	= it->first;
					itemElem.m_nItemNum	= it->second;
					itemElem.SetSerialNumber();
					if( pAttacker->CreateItem( &itemElem ) == TRUE )
					{
						ItemProp* pItemProp	= itemElem.GetProp();
						if( pItemProp )
							( (CUser*)pAttacker )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
					}
				}
				else
	#endif	// __BUGFIX_0326
				{
					CItemElem* pItemElem	= new CItemElem;
					pItemElem->m_dwItemId	= it->first;
					pItemElem->m_nItemNum	= it->second;
					pItemElem->m_nHitPoint	= -1;
					pItemElem->SetSerialNumber();
					CItem* pItem	= new CItem;
					pItem->m_pItemBase	= pItemElem;
					pItem->m_idOwn	= pAttacker->GetId();
					pItem->m_dwDropTime		= timeGetTime();
					pItem->m_bDropMob	= TRUE;
					pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
					D3DXVECTOR3 vPos = GetPos();
					vPos.x += ( xRandomF(2.0f) - 1.0f );
					vPos.z += ( xRandomF(2.0f) - 1.0f );
					vPos.y = GetPos().y;
					pItem->SetPos( vPos );
					GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
				}
			}
		}

		short nNum;
		u_long uSizeOfQuestItem	= lpMoverProp->m_QuestItemGenerator.GetSize();
		for( u_long i = 0; i < uSizeOfQuestItem; i++ ) 
		{
			QUESTITEM * pQuestItem	= lpMoverProp->m_QuestItemGenerator.GetAt( i );
			LPQUEST lpQuest = pAttacker->GetQuest( pQuestItem->dwQuest );
			if( lpQuest && lpQuest->m_nState == pQuestItem->dwState ) 
			{
				DWORD dwProbability	= pQuestItem->dwProbability;

				if( pQuestItem->dwQuest == QuestId(QUEST_VALENTINE) || pQuestItem->dwQuest == QuestId(QUEST_WHITEDAY) )
				{
					float f;
					int  d	= pAttacker->m_nLevel - (int)lpMoverProp->dwLevel;
					if( d <= 1 )	f	= 1.0f;
					else if( d <= 2 )	f	= 0.8f;
					else if( d <= 4 )	f	= 0.6f;
					else if( d <= 7 )	f	= 0.3f;
					else	f	= 0.1f;
					dwProbability	= (DWORD)( dwProbability * f );
				}

				if( xRandom( 3000000000 ) <= dwProbability ) 
				{
					if( pQuestItem->dwNumber == 0 )
						Error( "CMover::DropItem : %s?? quest item drop %d?????? dwNumber?? 0", GetName(), i );
					nNum	= (short)( xRandom( pQuestItem->dwNumber ) + 1 );
					if( pQuestItem->dwIndex == 0 )
					{
						WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, lpMoverProp->szName );
						break;
					}
					CParty* pParty	= g_PartyMng.GetParty( pAttacker->m_idparty );
					if( pParty && pParty->IsMember( pAttacker->m_idPlayer ) )	// party
					{
						for( int j = 0; j < pParty->GetSizeofMember(); ++j )
						{
							PartyMember* pPartyMember	= &pParty->m_aMember[j];
							CMover* pMember		= prj.GetUserByID( pPartyMember->m_uPlayerId );
							if( !IsValidObj( pMember ) || !IsValidArea( pMember, 64.0f ) )
								continue;
							
							LPQUEST pMemberQuest	= pMember->GetQuest( pQuestItem->dwQuest );
							if( pMemberQuest && pMemberQuest->m_nState == pQuestItem->dwState )
							{
								BYTE nId;
								CItemElem itemElem;
								itemElem.m_dwItemId		= pQuestItem->dwIndex;
								itemElem.m_nItemNum		= nNum;
								itemElem.m_nHitPoint	= -1;
								if( pMember->CreateItem( &itemElem, &nId ) == FALSE )
								{
									CItemElem* pItemElem	= new CItemElem;
									pItemElem->m_dwItemId	= pQuestItem->dwIndex;
									pItemElem->m_nItemNum	= nNum;
									ItemProp* pItemProp		= pItemElem->GetProp();
									if( pItemProp )
										pItemElem->m_nHitPoint		= pItemProp->dwEndurance;
									pItemElem->SetSerialNumber();
									CItem* pItem	= new CItem;
									pItem->m_pItemBase	= pItemElem;
									if( pItemElem->m_dwItemId == 0 )
										Error("DropItem:1st %s\n", GetName() );
									pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
									pItem->SetPos( pMember->GetPos() );
									pItem->SetAngle( (float)( xRandom( 360 ) ) );
									pItem->m_idHolder	= pMember->m_idPlayer;
									pItem->m_dwDropTime		= timeGetTime();
									pMember->GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
								}
								else
								{
									( (CUser*)pMember )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", prj.GetItemProp( pQuestItem->dwIndex )->szName );
									CItemElem * pItemBase	= pMember->GetItemId( nId );
									if( pItemBase )
									{
										LogItemInfo aLogItem;
										aLogItem.Action = "Q";
										aLogItem.SendName = pMember->m_szName;
										aLogItem.RecvName = "QUEST";
										aLogItem.WorldId = pMember->GetWorld()->GetID();
										aLogItem.Gold = aLogItem.Gold2 = pMember->GetGold();
										g_DPSrvr.OnLogItem( aLogItem, pItemBase, pItemBase->m_nItemNum );
									}
								}
							}
						}
					}
					else	// solo
					{
						BYTE nId;
						CItemElem itemElem;
						itemElem.m_dwItemId		= pQuestItem->dwIndex;
						itemElem.m_nItemNum		= nNum;
						itemElem.m_nHitPoint	= -1;
						if( pAttacker->CreateItem( &itemElem, &nId ) == FALSE )
						{
							CItemElem* pItemElem	= new CItemElem;
							pItemElem->m_dwItemId	= pQuestItem->dwIndex;
							pItemElem->m_nItemNum	= nNum;
							ItemProp* pItemProp		= pItemElem->GetProp();
							if( pItemProp )
								pItemElem->m_nHitPoint		= pItemProp->dwEndurance;
							pItemElem->SetSerialNumber();
							CItem* pItem	= new CItem;
							pItem->m_pItemBase	= pItemElem;
							if( pItemElem->m_dwItemId == 0 )
								Error("DropItem:1st %s\n", GetName() );
							pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
							pItem->SetPos( pAttacker->GetPos() );
							pItem->SetAngle( (float)( xRandom( 360 ) ) );
							pItem->m_idHolder	= pAttacker->m_idPlayer;
							pItem->m_dwDropTime		= timeGetTime();
							pAttacker->GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
						}
						else
						{
							( (CUser*)pAttacker )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", prj.GetItemProp( pQuestItem->dwIndex )->szName );
							CItemElem * pItemBase	= pAttacker->GetItemId( nId );
							if( pItemBase )
							{
								LogItemInfo aLogItem;
								aLogItem.Action = "Q";
								aLogItem.SendName = pAttacker->m_szName;
								aLogItem.RecvName = "QUEST";
								aLogItem.WorldId = pAttacker->GetWorld()->GetID();
								aLogItem.Gold = aLogItem.Gold2 = pAttacker->GetGold();
								g_DPSrvr.OnLogItem( aLogItem, pItemBase, pItemBase->m_nItemNum );
							}
						}
					}
				}
			}
		}

		int nloop = 1;
		BOOL bUnique = FALSE;
		if( pAttacker->m_idparty ) // ???? ?????? ???? ????
		{
			CParty* pParty = g_PartyMng.GetParty( pAttacker->m_idparty );
			if( pParty )
			{
				// ???????????? ?????????????? ?????? ???????? ????????
				if( pParty->m_nKindTroup == 1 )
				{
					if( pParty->m_nModeTime[PARTY_GIFTBOX_MODE] || pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
					{
						if( pParty->m_nModeTime[PARTY_PARSKILL_MODE] )
						{
							if( pParty->m_nModeTime[PARTY_GIFTBOX_MODE] )
								nloop = 2;
							if( pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
								bUnique = TRUE;
						}
						else
						{
							CUser* pLeader = g_UserMng.GetUserByPlayerID( pParty->m_aMember[0].m_uPlayerId );
							if( IsValidObj( (CObj*)pLeader ) )
							{
								if( pLeader->IsValidArea( pAttacker, 255.0f ) )
								{
									if( pParty->m_nModeTime[PARTY_GIFTBOX_MODE] )
										nloop = 2;
									if( pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
										bUnique = TRUE;
								}
							}
						}
					}
				}
			}
		}
		if( pAttacker->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_GET01 ) )
			nloop += 1;
		if( pAttacker->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_GET02 ) )
			nloop += 2;
#ifdef __DST_GIFTBOX
			nloop += pAttacker->GetAdjParam( DST_GIFTBOX );
#endif // __DST_GIFTBOX
		D3DXVECTOR3 vPos;		// ?????? ????.

#ifdef __EVENT_MONSTER
		// ?????? ???????? ?????? ???? ?????? ????????.
		if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
			if( !CEventMonster::GetInstance()->IsGiftBoxAble() )
				nloop = 1;
#endif // __EVENT_MONSTER
		
		for( int k = 0 ; k < nloop ; k++ )
		{
			// ?????? ???? ????		
			int nProbability	= 100, nPenyaRate	= 100;
			BOOL bAdjDropRate = TRUE;	// ?????????? ???????? ???? ???????????? ????????.

			if( GetIndex() == MI_CLOCKWORK1 )	//???????????? ???????????? ????.
				bAdjDropRate = FALSE;

			if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
				bAdjDropRate = FALSE;		// ???????????? ???????????? ?????? ?????? ????.
#ifdef __EVENT_MONSTER
			if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
				bAdjDropRate = FALSE;
#endif // __EVENT_MONSTER
#ifdef __EVENTLUA_SPAWN
			if( prj.m_EventLua.IsEventSpawnMonster( lpMoverProp->dwID ) )
				bAdjDropRate = FALSE;
#endif // __EVENTLUA_SPAWN
			if( bAdjDropRate )
			{
				int d	= pAttacker->m_nLevel - (int)lpMoverProp->dwLevel;
				if( d <= 1 )	{	nProbability	= 100;	nPenyaRate	= 100;	}
				else if( d <= 2 )	{	nProbability	= 80;	nPenyaRate	= 100;	}
				else if( d <= 4 )	{	nProbability	= 60;	nPenyaRate	= 80;	}
				else if( d <= 7 )	{	nProbability	= 30;	nPenyaRate	= 65;	}
				else	{	nProbability	= 10;	nPenyaRate	= 50;	}
			}

			FLOAT fItemDropRate		= nProbability * GetItemDropRateFactor( pAttacker );

			if( xRandom( 100 ) < fItemDropRate )	// ???????? ???????? ???? ????. ???????? ???? ???? ???? ?????????? ??????.
			{
				int nSize	= lpMoverProp->m_DropItemGenerator.GetSize();
				int nNumber	= 0;
				DROPITEM* lpDropItem;
		
				for( int i = 0; i < nSize; i++ )
				{
					if( ( lpDropItem = lpMoverProp->m_DropItemGenerator.GetAt( i, bUnique, GetPieceItemDropRateFactor( pAttacker ) ) ) != NULL )
					{
						if( lpDropItem->dtType == DROPTYPE_NORMAL )
						{
							DWORD dwNum		= lpDropItem->dwNumber;
							if(  dwNum == (DWORD)-1 )
								dwNum	= 1;

							if( lpMoverProp->dwFlying )
							{
								CItemElem itemElem;
								itemElem.m_dwItemId	= lpDropItem->dwIndex;
								itemElem.m_nItemNum	= (short)( xRandom( dwNum ) + 1 );
								itemElem.SetAbilityOption( lpDropItem->dwLevel );
								
								if( pAttacker->CreateItem( &itemElem ) == TRUE )
								{	// log
									ItemProp* pItemProp	= itemElem.GetProp();
									if( pItemProp )
									{
										( (CUser*)pAttacker )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
										if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
										{
											switch( pItemProp->nLog )
											{
												case 1:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), itemElem.GetAbilityOption() );	break;
												case 2:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 200 );	break;
												case 3:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 100 );	break;
											}
										}
									}
									if( lpDropItem->dwNumber != (DWORD)-1 )
										nNumber++;
									if(  (DWORD)( nNumber ) >= lpMoverProp->m_DropItemGenerator.m_dwMax )
										break;
								}
								continue;
							}
							CItemElem* pItemElem	= new CItemElem;
							pItemElem->m_dwItemId	= lpDropItem->dwIndex;
							pItemElem->m_nItemNum	= (short)( xRandom( dwNum ) + 1 );
							ItemProp* pItemProp		= pItemElem->GetProp();
							if( pItemProp )
							{
								pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
								pItemElem->SetRandomOpt(g_RandomOptItemGen.GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp, lpMoverProp->dwClass ) );
							}
							pItemElem->SetAbilityOption( lpDropItem->dwLevel );		// ???? ?????? +1, +2 ??????.
							pItemElem->SetSerialNumber();
							CItem* pItem	= new CItem;
							pItem->m_pItemBase	= pItemElem;
							BOOL bJJim = TRUE;		// ?????? ???????? ???????????? ??????.
							if( lpMoverProp->dwClass == RANK_SUPER )		// ???????? ?????? ???????? ?????? ?????? ????.
								bJJim = FALSE;
							if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
								bJJim = FALSE;		// ?????? ?????????????? ?????????? ????. ?????? ?????? ????
							if( bJJim )
							{
								pItem->m_idOwn = pAttacker->GetId();	// ?? ???????? ?????? pAttacker(??????)?????? ????.
								pItem->m_dwDropTime = timeGetTime();	// ???? ?????????? ?????? ??????.
							}
							pItem->m_bDropMob = TRUE;		// ???? ?????? ???? ???? ?????? ????.
							if( pItem->m_pItemBase->m_dwItemId == 0 ) Error("DropItem:2nd %s\n", GetName() );
							pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );

							vPos = GetPos();
							vPos.x += ( xRandomF(2.0f) - 1.0f );
							vPos.z += ( xRandomF(2.0f) - 1.0f );
							vPos.y = GetPos().y;
					#ifdef __EVENT_MONSTER
							// ?????? ???????? ?????? ???????? ???????? ID?? ????????(???? ?? ????...)
							if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
							{
								// ?????? ???????? ?????? ???????? ??????.
								pItem->m_idOwn	= pAttacker->GetId();
								pItem->m_dwDropTime		= timeGetTime();

								pItem->m_IdEventMonster = lpMoverProp->dwID;
								float fItemDropRange = CEventMonster::GetInstance()->GetItemDropRange(); 
								vPos = GetPos();
								vPos.x += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f) );
								vPos.z += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f) );
							}
					#endif // __EVENT_MONSTER
							pItem->SetPos( vPos );

							GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );

							if( lpDropItem->dwNumber != (DWORD)-1 )
								nNumber++;

							if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
							{
								switch( pItemProp->nLog )
								{
								case 1:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, pItemElem->GetAbilityOption() );	// ???? ?????? +??????
									break;
								case 2:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 200 );	// ?????? ??????
									break;
								case 3:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 100 );	// ?????? ??????
									break;
								}
							}

							if( nNumber == lpMoverProp->m_DropItemGenerator.m_dwMax )
								break;
						} else
						// ???? ?????????????? ????.
						if( lpDropItem->dtType == DROPTYPE_SEED && k == 0 )
						{
							int	nSeedID = 0;
							int nNumGold = lpDropItem->dwNumber + xRandom( lpDropItem->dwNumber2 - lpDropItem->dwNumber );	// Number ~ Number2 ?????? ??????.
							nNumGold	= nNumGold * nPenyaRate / 100;
	#ifdef __S1108_BACK_END_SYSTEM
							nNumGold	= (int)( nNumGold * prj.m_fGoldDropRate * lpMoverProp->m_fPenya_Rate );
							if( nNumGold == 0 )
								continue;
	#else // __S1108_BACK_END_SYSTEM
							nNumGold	*= prj.m_fGoldDropRate;
	#endif // __S1108_BACK_END_SYSTEM
							
							nNumGold	= (int)( nNumGold * prj.m_EventLua.GetGoldDropFactor() );


							if( lpMoverProp->dwFlying )
							{
								if( CanAdd( pAttacker->GetGold(), nNumGold ) )
								{
									pAttacker->AddGold( nNumGold );
									( (CUser*)pAttacker )->AddGoldText( nNumGold );
								}
							}
							else
							{
								// ???????? ???? ?????????? ?????? ???????? ????????.
								if( nNumGold <= (int)( prj.GetItemProp( II_GOLD_SEED1 )->dwAbilityMax ) )
									nSeedID = II_GOLD_SEED1;
								else if( nNumGold <= (int)( prj.GetItemProp( II_GOLD_SEED2 )->dwAbilityMax ) )
									nSeedID = II_GOLD_SEED2;
								else if( nNumGold <= (int)( prj.GetItemProp( II_GOLD_SEED3 )->dwAbilityMax ) )
									nSeedID = II_GOLD_SEED3;
								else 
									nSeedID = II_GOLD_SEED4;

								CItemElem* pItemElem	= new CItemElem;
								pItemElem->m_dwItemId	= nSeedID;
								pItemElem->m_nItemNum	= nNumGold;	// ??????
								pItemElem->m_nHitPoint	= nNumGold;
								CItem* pItem	= new CItem;
								pItem->m_pItemBase	= pItemElem;
								BOOL bJJim = TRUE;
								if( lpMoverProp->dwClass == RANK_SUPER )		// ???????? ?????? ???????? ?????? ?????? ????.
									bJJim = FALSE;
								if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
									bJJim = FALSE;		// ?????? ?????????????? ?????????? ????. ?????? ?????? ????
								if( bJJim )
								{
									pItem->m_idOwn = pAttacker->GetId();	// ?? ???????? ?????? pAttacker(??????)?????? ????.
									pItem->m_dwDropTime = timeGetTime();	// ???? ?????????? ?????? ??????.
								}
								pItem->m_bDropMob = TRUE;		// ???? ?????? ???? ???? ?????? ????.
								if( pItem->m_pItemBase->m_dwItemId == 0 ) Error("DropItem: 3rd %s\n", GetName() );
								pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );

								vPos = GetPos();
								vPos.x += ( xRandomF(2.0f) - 1.0f );
								vPos.z += ( xRandomF(2.0f) - 1.0f );
						#ifdef __EVENT_MONSTER
								// ?????? ???????? ?????? ???????? ???????? ID?? ????????(???? ?? ????...)
								if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
								{
									// ?????? ???????? ?????? ???????? ??????.
									pItem->m_idOwn	= pAttacker->GetId();
									pItem->m_dwDropTime		= timeGetTime();
									
									pItem->m_IdEventMonster = lpMoverProp->dwID;
									float fItemDropRange = CEventMonster::GetInstance()->GetItemDropRange(); 
									vPos = GetPos();
									vPos.x += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f) );
									vPos.z += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f));
								}
						#endif // __EVENT_MONSTER
								pItem->SetPos( vPos );
								GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
							}
						} // DROPTYPE_SEED
					} // if
					//////////////
					//  ???????? for-loop?????????? continue???? ???? ???????? ?????? ???? ???? ????.
					///////////
				} // for nSize

				nSize	= lpMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
				int nAbilityOption; //, nDropLuck;
				BOOL bDrop = FALSE;
				for( int i = 0; i < nSize; i++ )
				{
					bDrop = FALSE;
					pDropKind	= lpMoverProp->m_DropKindGenerator.GetAt( i );
					pItemKindAry	= prj.GetItemKindAry( pDropKind->dwIK3 );
					int nMinIdx	= -1,	nMaxIdx		= -1;
					for( int j = pDropKind->nMinUniq; j <= pDropKind->nMaxUniq; j++ )
					{
						nMinIdx		= prj.GetMinIdx( pDropKind->dwIK3, j );
						if( nMinIdx != -1 )
							break;
					}
					for( int j = pDropKind->nMaxUniq; j >= pDropKind->nMinUniq; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pDropKind->dwIK3, j );
						if( nMaxIdx != -1 )
							break;
					}
					if( nMinIdx < 0 || nMaxIdx < 0 )
					{
#ifdef __INTERNALSERVER
						WriteLog( "dropkind, 1, dwIndex = %d, nIndex = %d", lpMoverProp->dwID, i );
#endif	// __INTERNALSERVER
						continue;
					}
					ItemProp* pItemProp		= (ItemProp*)pItemKindAry->GetAt( nMinIdx + xRandom( nMaxIdx - nMinIdx + 1 ) );
					if( NULL == pItemProp )
					{
#ifdef __INTERNALSERVER
						WriteLog( "dropkind, 2, dwIndex = %d, nIndex = %d", lpMoverProp->dwID, i );
#endif	// __INTERNALSERVER
						continue;
					}

					nAbilityOption	= xRandom( 11 );	// 0 ~ 10

					DWORD dwAdjRand;
					
					for( int k = nAbilityOption; k >= 0; k-- )
					{
						DWORD dwPrabability		= (DWORD)( prj.m_adwExpDropLuck[( pItemProp->dwItemLV > 120? 119: pItemProp->dwItemLV-1 )][k]
						* ( (float)lpMoverProp->dwCorrectionValue / 100.0f ) );

						dwAdjRand	= xRandom( 3000000000 );
						if( bUnique && dwPrabability <= 10000000 )
							dwAdjRand	/= 2;
						
						if( dwAdjRand < dwPrabability )
						{
							if( lpMoverProp->dwFlying )
							{
								CItemElem itemElem;
								itemElem.m_dwItemId		= pItemProp->dwID;
								itemElem.m_nItemNum		= 1;
								itemElem.SetAbilityOption( k );
								itemElem.SetRandomOpt(g_RandomOptItemGen.GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp, lpMoverProp->dwClass ) );

								if( pAttacker->CreateItem( &itemElem ) == TRUE )
								{	// log
									( (CUser*)pAttacker )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
									if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
									{
										switch( pItemProp->nLog )
										{
											case 1:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), itemElem.GetAbilityOption() );	break;
											case 2:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 200 );	break;
											case 3:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 100 );	break;
										}
									}
									break;
								}
							}

							CItemElem* pItemElem	= new CItemElem;
							pItemElem->m_dwItemId	= pItemProp->dwID;
							pItemElem->m_nItemNum	= 1;
							pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
							pItemElem->SetAbilityOption( k );
							pItemElem->SetRandomOpt(g_RandomOptItemGen.GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp, lpMoverProp->dwClass ) );
							pItemElem->SetSerialNumber();
							CItem* pItem	= new CItem;
							pItem->m_pItemBase	= pItemElem;
							BOOL bJJim = TRUE;

							if( lpMoverProp->dwClass == RANK_SUPER )		// ???????? ?????? ???????? ?????? ?????? ????.
								bJJim = FALSE;
							if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
								bJJim = FALSE;		// ?????? ?????????????? ?????????? ????. ?????? ?????? ????
							if( bJJim )
							{
								pItem->m_idOwn	= pAttacker->GetId();
								pItem->m_dwDropTime		= timeGetTime();
							}
							pItem->m_bDropMob	= TRUE;
					#ifdef __EVENT_MONSTER
							if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
							{
								// ?????? ???????? ?????? ???????? ??????.
								pItem->m_idOwn	= pAttacker->GetId();
								pItem->m_dwDropTime		= timeGetTime();

								pItem->m_IdEventMonster = lpMoverProp->dwID;
							}
					#endif // __EVENT_MONSTER
							if( pItem->m_pItemBase->m_dwItemId == 0 ) Error("DropItem: 4th %s\n", GetName() );
							pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
							pItem->SetPos( GetPos() );
							GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
							bDrop = TRUE;

							if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
							{
								switch( pItemProp->nLog )
								{
								case 1:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, pItemElem->GetAbilityOption() );	break;
								case 2:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 200 );	break;
								case 3:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 100 );	break;
								}
							}
							break;
						}
					}

					if( GetProp()->dwClass == RANK_SUPER )
					{
						if( bDrop )		// KIND???????? ???? ???????????? 
							break;		// ?????? ?????????? ???? ????.
					}
				} // for( i = 0; i < nSize; i++ )
			}	// if( xRandom( 100 ) < fItemDropRate )
		}	// for( int k = 0 ; k < nloop ; k++ )
	}
	
	if( IsNPC() )	// ???? ????????..
	{
		MoverProp* pMoverProp	= GetProp();
		if( pMoverProp->m_nLoot == 2 )	// 2?? d????.  ?????? ???? ???????? ???? ????.
		{
			int nSize = m_Inventory.GetSize();
			int i;
			CItemElem *pElem = NULL;
			for( i = 0; i < nSize; i ++ )
			{
				pElem = m_Inventory.GetAt( i );
				if( pElem )
				{
					D3DXVECTOR3 vPos = GetPos();
				#ifdef __WORLDSERVER
					vPos.x += ( xRandomF(2.0f) - 1.0f );
					vPos.z += ( xRandomF(2.0f) - 1.0f );
				#endif
					CItem *pItem = DropItem( i, pElem->m_nItemNum, vPos, FALSE );
					pItem->m_idOwn	= pAttacker->GetId();
					pItem->m_dwDropTime		= timeGetTime();
				}
			}
		}
	}

#endif	// __WORLDSERVER

	return TRUE;
}

// this???????????? ????????(JJimer -_-;;;) ?? ???????? ???????? 
// this?? ???????????? ????.
void CMover::SetJJim( CMover *pJJimer )
{
	if( ::GetLanguage() == LANG_THA )	// ?????? ????
		if( GetIndex() == MI_MRPUMPKIN5 )	// ???? ????????????
			return;													// ?????? ??????.

	if( GetProp()->dwClass == RANK_SUPER )		// ???????? ???? ????.
		return;

#ifdef __EVENT_MONSTER
	// ?????? ???????? ???? ????
	if( CEventMonster::GetInstance()->IsEventMonster( GetProp()->dwID ) )
		return;
#endif // __EVENT_MONSTER
		
	// ???????? ???? / ?????????? NPC??????.
	if( pJJimer->IsPlayer() && IsNPC() ) //this != pJJimer )		// ?????????? ???????????? ????./ ?????????? ?????? ????.
	{
		m_idTargeter = pJJimer->GetId();
	}
}


CGuild* CMover::GetGuild()
{
#ifdef __WORLDSERVER
//	locked
	CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
	if( pGuild && pGuild->IsMember( m_idPlayer ) )
		return pGuild;
	return NULL;
#else	// __WORLDSERVER
#ifdef __CLIENT
	if( m_idGuild > 0 && CMover::GetActiveMover() &&
		 m_idGuild == CMover::GetActiveMover()->m_idGuild )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
		if( pGuild && pGuild->IsMember( m_idPlayer ) )
			return pGuild;
		return NULL;
	}
	else
	{
		return g_GuildMng.GetGuild( m_idGuild );
	}
#endif	// __CLIENT
#endif	// __WORLDSERVER
}

CGuildWar* CMover::GetWar()
{
	return g_GuildWarMng.GetWar(m_idWar);
}

#ifdef  __WORLDSERVER
// HP, MP, FP?????? ????????.
void CMover::ProcessRecovery()
{
	if( IsPlayer() == FALSE || IsDie() )
		return;

	DWORD dwCurTick = g_tmCurrent;


	if( dwCurTick > m_dwTickDuel )	// ???? ?????? 1???? 3, 2, 1, Fight
	{
		if( 100 <= m_nDuelState && m_nDuelState < 200 )		// 100 ~ 200 ?? ???? ?????? ???? ????
		{
			--m_nDuelState;
			if( 100 == m_nDuelState )	// ?????? Fight ???????? ????
			{
				m_nDuelState = 1;		// ???? ???????? ????
				if( m_nDuel == 1 )
				{
					CMover *pDuelOther = prj.GetMover( m_idDuelOther );
					if( pDuelOther )
					{
						((CUser*)this)->AddDuelStart( pDuelOther->m_idPlayer, 1 );	// Fight
						((CUser*)this)->AddDuelCount( 0 );
					}
				}
				else if( m_nDuel == 2 )
				{
					CParty* pParty = g_PartyMng.GetParty( m_idDuelParty );		// ???????? ????????
					if( pParty )
					{
						LPCSTR pszPartyName;
						if( pParty->m_nKindTroup ) // ????????
							pszPartyName = pParty->m_sParty;
						else // ????????
							pszPartyName = CPlayerDataCenter::GetInstance()->GetPlayerString( pParty->GetPlayerId( 0 ) );

						if( !pszPartyName )
						{
							Error( "Error Party PVP 2" );
						}
						else
						{
							int		j;
							OBJID	pDstMember[ MAX_PTMEMBER_SIZE ];
							memset( pDstMember, 0xff, sizeof(pDstMember) );
							CMover *pMember2;

							for( j = 0; j < pParty->m_nSizeofMember; j ++ )
							{
								pMember2 = (CMover *)g_UserMng.GetUserByPlayerID( pParty->m_aMember[j].m_uPlayerId );
								if( IsValidObj(pMember2) )
									pDstMember[j] = pMember2->GetId();
							}
							// ???? ?????? ???? ?????? ?????????????? ????
							((CUser *)this)->AddDuelPartyStart( pszPartyName, pParty->m_nSizeofMember, pDstMember, pParty->m_uPartyId, 1 );		// ?????? ?????? ID?? ????????. 2??????
							((CUser*)this)->AddDuelCount( 0 );
						}
					}
				}
			}
			else
			{
				((CUser*)this)->AddDuelCount( m_nDuelState - 100 );
			}
		}
		m_dwTickDuel = dwCurTick + NEXT_TICK_DUEL;		
	}

	if( m_nDuel != 0 && dwCurTick > m_dwTickEndDuel )
	{
		// ???????? 3?????? ?????? ??????
		if( m_nDuel == 1 )
		{
			CMover *pDuelOther = prj.GetMover( m_idDuelOther );
			if( pDuelOther )
			{
				pDuelOther->ClearDuel();
				( (CUser*)pDuelOther )->AddSetDuel( pDuelOther );
				ClearDuel();
				( (CUser*)this )->AddSetDuel( this );
			}
		}
		else if( m_nDuel == 2 )
		{
			CParty* pParty = g_PartyMng.GetParty( m_idparty );
			if( pParty && pParty->IsLeader( m_idPlayer ) )
			{
				u_long uidDuelParty = m_idDuelParty;
				CMover *pMember;
				CParty* pParty2 = g_PartyMng.GetParty( uidDuelParty );
				for( int k = 0 ; k < pParty->m_nSizeofMember ; ++k )
				{
					pMember	= (CMover *)g_UserMng.GetUserByPlayerID( pParty->m_aMember[k].m_uPlayerId );					
					if( IsValidObj( pMember ) )
					{
						if( pParty2 )
						{
							pParty2->DoDuelPartyCancel( pParty );
						}
						else 
						{
							pParty->DoDuelPartyCancel( NULL );
							Error( "CMover::ProcessRecovery : ???????? %s?? ????????. %d %d", pMember->GetName(), pMember->m_idDuelParty, uidDuelParty );
						}
					}
				}

				if( pParty2 )
				{
					for (CUser * user : AllMembers(*pParty2)) {
						pParty->DoDuelPartyCancel(pParty2);
					}
				}
				g_DPCoreClient.SendSetPartyDuel( m_idparty, uidDuelParty, FALSE );
			}
		}
	}
	
	if( m_pActMover->IsSit() )		
	{
		// ???????? ??
		float fRecovery = 1.0f;
		if( dwCurTick <= m_dwTickRecovery )	
			return;
		m_dwTickRecovery = dwCurTick + NEXT_TICK_RECOVERY;
		
		// ???? PARTY_STRETCHING_MODE?? ?? 
		CParty* pParty = g_PartyMng.GetParty( m_idparty );
		if( pParty && pParty->IsMember( m_idPlayer ) )
		{
			if( pParty->m_nKindTroup == 1 && pParty->m_nModeTime[PARTY_STRETCHING_MODE] )
			{
				BOOL nMItem = FALSE;
				CUser* pLeader = g_UserMng.GetUserByPlayerID( pParty->m_aMember[0].m_uPlayerId );
				if( pParty->m_nModeTime[PARTY_PARSKILL_MODE] )
				{
					nMItem = TRUE;
				}
				else
				{
					if( IsValidObj( (CObj*)pLeader ) )
					{
						if( IsValidArea( pLeader, 255.0f ) )
							nMItem = TRUE;
					}				
				}
				if( nMItem )
				{
					if( IsValidObj( pLeader ) && pLeader->IsInteriorityJob(JOB_ASSIST) )
						fRecovery = 1.8f;
					else
						fRecovery = 1.5f;
				}
			}
		}
		else
		{
			m_idparty	= 0;
		}
		IncHitPoint( (int)( GetHPRecovery() * fRecovery ) );
		IncManaPoint( (int)( GetMPRecovery() * fRecovery ) );
		IncFatiguePoint( (int)( GetFPRecovery() * fRecovery ) );
	} 
	else	
	{
		// ?????? ??
		m_dwTickRecovery = dwCurTick + NEXT_TICK_RECOVERY; // ???????? ?????? ??????.
		//if( m_pActMover->IsActAttack() == FALSE )	
		if( IsAttackMode() == FALSE )	// IsActAttack?? ?????????? ????, IsAttackMode?? ??????
		{
			// ?????????? ???????? ???????????? ??????.
			if( dwCurTick > m_dwTickRecoveryStand )	
			{
				m_dwTickRecoveryStand = dwCurTick + NEXT_TICK_RECOVERYSTAND;
				if( ! (GetAdjParam( DST_CHRSTATE ) & CHS_SETSTONE) )
				{
					IncHitPoint( GetHPRecovery() );
					IncManaPoint( GetMPRecovery() );
					IncFatiguePoint( GetFPRecovery() );
				}
			}
		} 
		else
		{
			// ???? ?????? ???? ???? ?????? ????????.
			m_dwTickRecoveryStand = dwCurTick + NEXT_TICK_RECOVERYSTAND;	
		}
	}
}
#endif // __WORLDSERVER

void CMover::AllocShopInventory( LPCHARACTER pCharacter )
{
	if( IsVendorNPC() )
	{
		m_ShopInventory[ 0 ] = new CItemContainer; m_ShopInventory[ 0 ]->SetItemContainer(CItemContainer::ContainerTypes::VENDOR);
		m_ShopInventory[ 1 ] = new CItemContainer; m_ShopInventory[ 1 ]->SetItemContainer(CItemContainer::ContainerTypes::VENDOR);
		m_ShopInventory[ 2 ] = new CItemContainer; m_ShopInventory[ 2 ]->SetItemContainer(CItemContainer::ContainerTypes::VENDOR);
		m_ShopInventory[ 3 ] = new CItemContainer; m_ShopInventory[ 3 ]->SetItemContainer(CItemContainer::ContainerTypes::VENDOR);
	}
}


static bool CVendor_IsShop(CVendor & vendor) {
	for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++) {
		if (!vendor.m_venderItemAry[i].empty())
			return TRUE;
	}
	if (vendor.m_type == CVendor::Type::RedChip) {
		for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++) {
			if (!vendor.m_venderItemAry2[i].empty())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::IsVendorNPC()
{
	LPCHARACTER pCharacter	= GetCharacter();
	if( !pCharacter )
		return FALSE;

	return CVendor_IsShop(pCharacter->m_vendor) ? TRUE : FALSE;
}

#ifdef __CLIENT	
BOOL CMover::EndMotion()
{
	if( !m_pActMover->IsActAttack() &&
		!m_pActMover->IsActDamage() &&
		!m_pActMover->IsActJump() && 
		!m_pActMover->IsFly() && 
		!m_pActMover->IsDie() && 
		!m_pActMover->IsSit() )
	{
		SetAngle( GetDegree(GetWorld()->GetCamera()->m_vPos, GetPos()) );		// ?????????? ???? ????.
		SetMotion( MTI_LOGOUT, ANILOOP_CONT, MOP_FIXED );
		return TRUE;
	}
	return FALSE;
}
#endif //__CLIENT

BOOL CMover::IsBullet( const ItemProp* pItemProp )
{
	// ?????????????? ???????? ??????
	if( pItemProp->dwLinkKindBullet != NULL_ID )
	{
		DWORD dwTip = TID_TIP_NEEDSKILLITEM;
		if( pItemProp->dwItemKind3 == IK3_BOW || pItemProp->dwLinkKind == IK3_BOW )
		{
			dwTip = TID_TIP_NEEDSATTACKITEM;			
		}
		
		ItemProp *pProp = GetActiveHandItemProp( PARTS_BULLET );
		if( pProp )
		{
			if( pProp->dwItemKind2 != pItemProp->dwLinkKindBullet )		// ?????????????? ?????? ???????? ?????? ????
			{
#ifdef __CLIENT
				g_WndMng.PutString( prj.GetText( dwTip ), NULL, prj.GetTextColor( dwTip ) );
				( (CWndWorld *)g_WndMng.m_pWndWorld )->m_bAutoAttack	= FALSE;
#endif // __CLIENT
#ifdef __WORLDSERVER
				((CUser*)this)->AddDefinedText( dwTip, "" );
#endif // __WORLDSERVER
				return FALSE;
			}
		} else
		{	// ????(?)?? ?????? ????
#ifdef __CLIENT
			g_WndMng.PutString( prj.GetText( dwTip ), NULL, prj.GetTextColor( dwTip ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
			((CUser*)this)->AddDefinedText( dwTip, "" );
#endif // __WORLDSERVER
			return FALSE;
		}
	}
	return TRUE;
}

#ifdef __WORLDSERVER
// ?????? nCount???? ????
// ?????? ???????? ???????? ?????????? ???????? ???? ?????? ????
// ???? ???????? ????
void CMover::ArrowDown( int nCount )
{
	if( IsNPC() )	return;
	CItemElem* pItemElem = m_Inventory.GetEquip( PARTS_BULLET );	// ???????? ???? ???? ?????? ????
	if( pItemElem )
	{
		// ???????? ?????? ???? ???? ??????. ???????? ???? ?????? ??????
		// ???????????? ???? ?????? ?????? ?????? ???????? ????
		DWORD dwItemId = pItemElem->m_dwItemId;
		BOOL bDoEquip = FALSE;
		for( int i = 0 ; i < 100 && 0 < nCount ; i++ )	// ?? 100???? ???????????
		{	// ?????? ?????? 4?????? ?????? 1???????? 4?????? ?????? ????
			BOOL bGetItem = FALSE;
			if( pItemElem->m_nItemNum <= nCount )
			{
				nCount -= pItemElem->m_nItemNum;
				pItemElem->m_nItemNum = 0;
				bGetItem = TRUE;
			}
			else
			{
				pItemElem->m_nItemNum -= nCount;
				nCount = 0;
			}
			UpdateItem(*pItemElem, UI::Num::Sync);
			if( bGetItem )
			{
				bDoEquip = FALSE;
				pItemElem = m_Inventory.GetAtItemId( dwItemId );
				if( pItemElem == NULL ) 
				{
					break;
				}
				bDoEquip = TRUE;
			}
		}
		
		if( bDoEquip ) 
		{
			if( TRUE == DoEquip( pItemElem, TRUE, pItemElem->GetProp()->dwParts ) )
			{
				g_UserMng.AddDoEquip( this, pItemElem->GetProp()->dwParts, pItemElem, TRUE );
			}
		}
	}
}
#endif // __WORLDSERVER


void CMover::ClearDuel()
{
	m_nDuel		= 0;
	m_idDuelOther	= NULL_ID;
	m_nDuelState	= 0;
	m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL; // 3 min
	SetPKTargetLimit( 10 );								   // PK?????? ?????? ??????	
}

void CMover::ClearDuelParty()
{
	m_nDuel		= 0;
	m_idDuelParty	= 0;
	m_nDuelState	= 0;
	m_dwTickEndDuel = ::timeGetTime() + NEXT_TICK_ENDDUEL;	// 3 min
	SetPKTargetLimit( 10 );									// PK?????? ?????? ??????
}

void CMover::SetPKTargetLimit( int nSec )
{
#if defined( __WORLDSERVER ) 
	m_dwPKTargetLimit = GetTickCount() + SEC( nSec );
#endif // defined( __WORLDSERVER )
}

#if defined(__WORLDSERVER)
BOOL CMover::IsItemRedyTime( ItemProp* pItemProp, OBJID dwObjid, BOOL bItemFind )
{
	if( pItemProp->dwSkillReadyType != 0 &&
		pItemProp->dwSkillReadyType != 0xffffffff )
	{
		if( pItemProp->dwItemKind2 == IK2_RIDING )
		{
			if( HasActivatedEatPet() || HasActivatedSystemPet() || HasPet() )
			{
				( (CUser*)this )->AddDefinedText( TID_GAME_CANNOT_FLY_WITH_PET );
				return FALSE;
			}
		}
		// ???????? ?????? ?????? ?????????? ???????? ???????? ?????? ??????
		if( IsStateMode( STATE_BASEMOTION_MODE ) ) 
		{
			if( m_nReadyTime != 0 )	// ??????
			{
				( (CUser*)this )->AddDefinedText(TID_PK_BLINK_LIMIT, "" );	// ?????????? ???????? ????????
				return FALSE;
			}
			else	// ?????? ?????? ????
			{
				SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_OFF );
			}
		}
		else	// ???? ?V??
		{
			if( pItemProp->dwItemKind2 == IK2_BLINKWING && m_pActMover->IsSit() )
			{
				( (CUser*)this )->AddDefinedText(TID_GAME_NOTSIT_BLINK, "" );	
				return FALSE;
			}
			m_nReadyTime = timeGetTime() + pItemProp->dwSkillReadyType;
			m_dwUseItemId = dwObjid;
			m_bItemFind = bItemFind;
			SetStateMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_ON );
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __WORLDSERVER

#ifdef __CLIENT
CSfx* CMover::CreateSfxArrow( DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget  )
{
	CSfx* pSfx = NULL;

	// ?????? ?????? ????...
	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)m_pModel;
	pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );		// ???? ???????? ????	
	
	pSfx = CreateSfx( D3DDEVICE, dwSfxObjArrow, vPos, GetId(), vPosDest , idTarget );
	
	if( dwSfxObjHit != NULL_ID )
	{
		if( pSfx ) 
		{
			((CSfxShoot*)pSfx)->m_dwSfxHit = dwSfxObjHit;
		}
	}
	
	return pSfx;
	
}
#endif
BOOL CMover::IsAfterDeath()	
{	
	if( m_nDeathLevel > m_nLevel )
		return TRUE;

	if( m_nDeathLevel == m_nLevel )
	{
		if( m_nExp1 == 0 )
			return TRUE;
		if( m_nDeathExp > m_nExp1 )
			return TRUE;
	}
	return FALSE;
}

int CMover::GetSetItem( CItemElem* pItemElem )
{
	static	DWORD adwParts[4]	= { PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CAP	};
	if( !pItemElem )
	{
		pItemElem	= GetEquipItem( PARTS_UPPER_BODY );
	}
	else
	{
		if( !IsSetItemPart( pItemElem->GetProp()->dwParts ) )
			return 0;
	}

	if( !pItemElem )
		return 0;

	if( pItemElem->IsFlag( CItemElem::expired ) )
		return 0;

	int nAbilityOption	= pItemElem->GetAbilityOption();
	DWORD dwParts	= pItemElem->GetProp()->dwParts;

	for( int i = 0; i < 4; i++ )
	{
		if( dwParts != adwParts[i] )
		{
			pItemElem	= GetEquipItem( adwParts[i] );
			if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
			{
				if( nAbilityOption > pItemElem->GetAbilityOption() )
					nAbilityOption	= pItemElem->GetAbilityOption();
				continue;
			}
			return 0;
		}
	}
	return nAbilityOption;
}

int CMover::GetSetItemParts(DWORD dwParts) const {
	const CItemElem * const pItemElem = GetEquipItem(dwParts);
	
	int nAbilityOption = 0;
	
	if (pItemElem) {
		nAbilityOption = pItemElem->GetAbilityOption();
	} else {
		if (IsActiveMover()) return 0;
		nAbilityOption	= (m_aEquipInfo[dwParts].nOption & 0xFF);
	}

	return nAbilityOption;
}
		
int CMover::GetSetItemClient() const {
	static constexpr auto adwParts = {
		PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CAP
	};

	int nAbilityOption = 10;
	
	for (const auto part : adwParts) {
		int nValue = GetSetItemParts(part);
		if (nValue == 0) return 0;
		if (nAbilityOption > nValue) nAbilityOption = nValue;
	}

	return nAbilityOption;
}

bool CMover::IsSetItemPart(const DWORD dwParts) {
	return dwParts == PARTS_UPPER_BODY
		|| dwParts == PARTS_HAND
		|| dwParts == PARTS_FOOT
		|| dwParts == PARTS_CAP;
}

void CMover::SetSetItemAvail( int nAbilityOption )
{
	PSETITEMAVAIL pSetItemAvail		= prj.GetSetItemAvail( nAbilityOption );
	if( pSetItemAvail )
	{
//		OutputDebugString( "SetSetItemAvail" );
		if( pSetItemAvail->nHitRate != 0 )
			SetDestParam( DST_ADJ_HITRATE, pSetItemAvail->nHitRate, NULL_CHGPARAM );
		if( pSetItemAvail->nBlock != 0 )
		{
			SetDestParam( DST_BLOCK_RANGE, pSetItemAvail->nBlock, NULL_CHGPARAM );
			SetDestParam( DST_BLOCK_MELEE, pSetItemAvail->nBlock, NULL_CHGPARAM );
		}
		if( pSetItemAvail->nMaxHitPointRate != 0 )
			SetDestParam( DST_HP_MAX_RATE, pSetItemAvail->nMaxHitPointRate, NULL_CHGPARAM );
		if( pSetItemAvail->nAddMagic != 0 )
			SetDestParam( DST_ADDMAGIC, pSetItemAvail->nAddMagic, NULL_CHGPARAM );
		if( pSetItemAvail->nAdded != 0 )
		{
			SetDestParam( DST_STR, pSetItemAvail->nAdded, NULL_CHGPARAM );
			SetDestParam( DST_DEX, pSetItemAvail->nAdded, NULL_CHGPARAM );
			SetDestParam( DST_INT, pSetItemAvail->nAdded, NULL_CHGPARAM );
			SetDestParam( DST_STA, pSetItemAvail->nAdded, NULL_CHGPARAM );
		}
	}
}

void CMover::ResetSetItemAvail( int nAbilityOption )
{
	PSETITEMAVAIL pSetItemAvail		= prj.GetSetItemAvail( nAbilityOption );
	if( pSetItemAvail )
	{
//		OutputDebugString( "ResetSetItemAvail" );
		if( pSetItemAvail->nHitRate != 0 )
			ResetDestParam( DST_ADJ_HITRATE, pSetItemAvail->nHitRate, TRUE );
		if( pSetItemAvail->nBlock != 0 )
		{
			ResetDestParam( DST_BLOCK_RANGE, pSetItemAvail->nBlock, TRUE );
			ResetDestParam( DST_BLOCK_MELEE, pSetItemAvail->nBlock, TRUE );
		}
		if( pSetItemAvail->nMaxHitPointRate != 0 )
			ResetDestParam( DST_HP_MAX_RATE, pSetItemAvail->nMaxHitPointRate, TRUE );
		if( pSetItemAvail->nAddMagic != 0 )
			ResetDestParam( DST_ADDMAGIC, pSetItemAvail->nAddMagic, TRUE );
		if( pSetItemAvail->nAdded != 0 )
		{
			ResetDestParam( DST_STR, pSetItemAvail->nAdded, TRUE );
			ResetDestParam( DST_DEX, pSetItemAvail->nAdded, TRUE );
			ResetDestParam( DST_INT, pSetItemAvail->nAdded, TRUE );
			ResetDestParam( DST_STA, pSetItemAvail->nAdded, TRUE );
		}
	}
}

void CMover::DestParamPiercingAvail( CItemElem* pItemElem, BOOL bSET )
{
	if( !pItemElem->IsPierceAble() )
		return;
	
	const auto dstDiff = pItemElem->GetPiercingAvail();

	if (bSET) {
		SetDSTs(dstDiff);
	} else {
		ResetDSTs(dstDiff);
	}
}

#ifdef __WORLDSERVER
BOOL CMover::Pierce( CItemElem *pSuit, DWORD dwItemId )
{
	for( int i = 0; i < pSuit->GetPiercingSize(); i++ )
	{
		if( pSuit->GetPiercingItem( i ) == 0 )
		{
			UpdateItem(*pSuit, UI::Piercing::Item{ UI::Piercing::Kind::Regular, i, dwItemId });
			return TRUE;
		}

	}
	return FALSE;
}
#endif

#ifdef __WORLDSERVER
void CMover::CheckTickCheer()
{
	if( m_nCheerPoint >= MAX_CHEERPOINT )
		return;

	DWORD dwTickCount	= GetTickCount();
	if( dwTickCount > m_dwTickCheer )
		SetCheerParam( m_nCheerPoint + 1, dwTickCount, TICK_CHEERPOINT );
}
#endif	// __WORLDSERVER

void CMover::SetCheerParam( int nCheerPoint, DWORD dwTickCount, DWORD dwRest )
{
#ifdef __WORLDSERVER	// chipi_080411
	BOOL bAdd = FALSE;
	if( m_nCheerPoint < nCheerPoint )
		bAdd = TRUE;
#endif // __WORLDSERVER
	m_nCheerPoint	= nCheerPoint;
	m_dwTickCheer	= dwTickCount + dwRest;
#ifdef __WORLDSERVER
	( (CUser*)this )->AddSetCheerParam( nCheerPoint, dwRest, /* chipi_080411 */ bAdd );
#endif	// __WORLDSERVER
}


BOOL CMover::NoDisguise( LPDIRECT3DDEVICE9 pd3dDevice )     
{ 
#ifdef __CLIENT
	// ???? ???? ?????????? ???? ???? ?? ???? ????. 
	if( GetIndex() == MI_MALE || GetIndex() == MI_FEMALE )
		return FALSE;
	// ???? ???? ???? 
	DWORD dwIndex = (GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	SetIndex( pd3dDevice, dwIndex, FALSE, FALSE );
#endif  //__CLIENT
	return TRUE;
}
BOOL CMover::Disguise( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex )
{
#ifdef __CLIENT
	return SetIndex( pd3dDevice, dwMoverIndex );
#endif // __CLIENT
	return TRUE;
}
BOOL CMover::SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex, BOOL bInitProp, BOOL bDestParam )
{ 
	MoverProp* pMoverProp = prj.GetMoverProp( dwMoverIndex );
	if( pMoverProp == NULL )
	{
		Error( "CMover.SetIndex: property not found: %d", dwMoverIndex );
		return FALSE;
	}
	// ???? ???? ???? 
	if( m_pModel && m_pModel->IsAniable() )
		SAFE_DELETE( m_pModel );
	SetTypeIndex( pd3dDevice, OT_MOVER, dwMoverIndex, bInitProp );//
	m_dwMotion = -1;
	SetMotion( MTI_STAND );
	// ?????? ?????????? ?????????? ????.
	if( m_bPlayer )
	// ?????????? ???????? ?????? ????. ???? npc?? ?????? ???????? ???????? ???? ???? ???? ????, 
	// ???? ?????? ?????? ?????? ?????? ?????? ???? ???? ??????, ???? ?????? ???? ??????.
	// ?????? RedoEquip?? ???????? ????????, ???? ?????? Inventory ???????? ?????? ???? ????.
	{
		if( dwMoverIndex == MI_MALE || dwMoverIndex == MI_FEMALE )
		{
		#if defined( __CLIENT )
			RedoEquip( !IsActiveMover(), bDestParam );
		#else
			RedoEquip( FALSE );
		#endif
			SetHairColor( m_dwHairColor );
		}
	}
	ResetScale();
	UpdateLocalMatrix();
	return TRUE;
}
BOOL CMover::IsDisguise()
{
	if( IsPlayer() )
	{
		return ( GetIndex() != MI_MALE && GetIndex() != MI_FEMALE );
	}
	return FALSE;
}


LPCTSTR CMover::GetName( BOOL bNickname )
{
	if( !bNickname )
		return (LPCTSTR)m_szName;

	if( m_dwAuthorization >= AUTH_GAMEMASTER && ( GetIndex() == MI_CROWNIBLIS || GetIndex() == MI_CROWNSHADE || GetIndex() == MI_CROWNBUBBLE ) )
	{
		MoverProp* pMoverProp	= GetProp();
		if( pMoverProp )
			return (LPCTSTR)pMoverProp->szName;
	}
	return (LPCTSTR)m_szName;
}

void CMover::SetDestParamSetItem( CItemElem* pItemElem )
{
	if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
	{
		
		if (const CSetItem * pSetItem = g_SetItemFinder.GetSetItemByItemId(pItemElem->m_dwItemId)) {
			const int nEquiped = GetEquipedSetItemNumber(*pSetItem);
			ITEMAVAIL itemAvail = pSetItem->GetItemAvail(nEquiped, false);
			SetDSTs(itemAvail);
		}
	}
	else
	{
		std::map<const CSetItem *, int> mapSetItem;
		for( int nParts = 0; nParts < MAX_HUMAN_PARTS; nParts++ )
		{
			pItemElem	= GetEquipItem( nParts );
			if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
			{
				if(const CSetItem * pSetItem = g_SetItemFinder.GetSetItemByItemId( pItemElem->m_dwItemId )) {
					++mapSetItem[pSetItem];
				}
			}
		}

		for (const auto & [pSetItem, nEquiped] : mapSetItem) {
			const ITEMAVAIL itemAvail = pSetItem->GetItemAvail(nEquiped, true);
			SetDSTs(itemAvail);
		}
	}
}

void CMover::ResetDestParamSetItem( CItemElem* pItemElem )
{
	if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
	{
		if (const CSetItem * pSetItem = g_SetItemFinder.GetSetItemByItemId(pItemElem->m_dwItemId)) {
			const int nEquiped = GetEquipedSetItemNumber(*pSetItem);
			const ITEMAVAIL itemAvail = pSetItem->GetItemAvail(nEquiped + 1, false);
			ResetDSTs(itemAvail);
		}
	}
}

int CMover::GetEquipedSetItemNumber(const CSetItem & pSetItem) const {
	int nEquiped = 0;

	for (const CSetItem::PartItem & partItem : pSetItem.m_components) {
		if (IsEquipedPartItem(partItem)) {
			nEquiped++;
		}
	}

	return nEquiped;
}

bool CMover::IsEquipedPartItem(const CSetItem::PartItem & partItem) const {
	const CItemElem * const pItemElem = GetEquipItem(partItem.part);
	if (!pItemElem) return false;
	if (pItemElem->m_dwItemId != partItem.itemId) return false;
	if (pItemElem->IsFlag(CItemElem::expired)) return false;
	return true;
}

void CMover::SetDestParamRandomOptExtension(CItemElem * pItemElem) {
	for (const auto & dst : g_xRandomOptionProperty.GetParams(*pItemElem)) {
		SetDestParam(dst.nDst, dst.nAdj, NULL_CHGPARAM);
	}
}

void CMover::SetDestParamRandomOpt( CItemElem* pItemElem )
{
	SetDestParamRandomOptOrigin( pItemElem );
	SetDestParamRandomOptExtension( pItemElem );
}

void CMover::SetDestParamRandomOptOrigin(const CItemElem * const pItemElem) {
	const int randomOpt = pItemElem->GetRandomOpt();
	if (randomOpt > 0) {
		if (const auto * const pRandomOptItem = g_RandomOptItemGen.GetRandomOptItem(randomOpt)) {
			SetDSTs(pRandomOptItem->ia);
		}
	}
}

void CMover::ResetDestParamRandomOptOrigin(const CItemElem * const pItemElem) {
	const int randomOpt = pItemElem->GetRandomOpt();
	if (randomOpt > 0) {
		if (const auto * const pRandomOptItem = g_RandomOptItemGen.GetRandomOptItem(randomOpt)) {
			ResetDSTs(pRandomOptItem->ia);
		}
	}
}

void CMover::ResetDestParamRandomOptExtension(CItemElem * pItemElem) {
	for (const auto & dst : g_xRandomOptionProperty.GetParams(*pItemElem)) {
		ResetDestParam(dst.nDst, dst.nAdj, TRUE);
	}
}

void CMover::ResetDestParamRandomOpt( CItemElem* pItemElem )
{
	ResetDestParamRandomOptOrigin( pItemElem );
	ResetDestParamRandomOptExtension( pItemElem );
}

#ifdef __CLIENT
// pModel?? ?????? ???? ??????....???????????? ???????? ?????? ??????...
void CMover::OverCoatItemRenderCheck( CModelObject* pModel )
{
	if( !pModel )
		return;
	
	// ?????????? ???????? ???????? ?????? ??????...			
	int nArryEquip1[5] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CLOAK };
	int nArryEquip2[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK2 };
	
	O3D_ELEMENT *pElem = NULL;
	O3D_ELEMENT *pElem2 = NULL;
	
	for( int j=0; j<5; j++ )
	{
		pElem = pModel->GetParts( nArryEquip1[j] );
		
		if( !pElem )
			continue;

		pElem2 = pModel->GetParts( nArryEquip2[j] );
			
		if( pElem2 )
		{
			if( IsActiveMover() )
			{
				CItemElem* pItemElemOvercoat = GetEquipItem( nArryEquip2[j] );

				if( pItemElemOvercoat )
				{
					if( pItemElemOvercoat->IsFlag( CItemElem::expired ) )
					{
						pElem->m_nEffect &= ~XE_HIDE;
						pElem2->m_nEffect |= XE_HIDE;
						continue;
					}
				}
			}
			else
			{
				DWORD dwId = m_aEquipInfo[ nArryEquip2[j] ].dwId;
				if( (m_aEquipInfo[ nArryEquip2[j] ].byFlag & CItemElem::expired) )
				{
					pElem->m_nEffect &= ~XE_HIDE;
					pElem2->m_nEffect |= XE_HIDE;					
					continue;
				}
			}

			pElem->m_nEffect |= XE_HIDE;
		}
	}
}
#endif	// __CLIENT

#ifdef __WORLDSERVER
void CMover::ProcInstantBanker()
{
	if( m_dwTickCreated && m_dwIndex == MI_INFO_PENG )
	{
		LPCHARACTER lpCharacter	= GetCharacter();
		if( lpCharacter && lstrcmp( lpCharacter->m_szKey, "MaFl_InstantBank" ) == 0 )
		{
			if( GetTickCount() > m_dwTickCreated + TIMEWAIT_INSTANT_BANKER )
				Delete();
		}
	}
}
void CMover::ProcInstantGC()
{
	if( CGuildCombat::WAR_STATE <= g_GuildCombatMng.m_nState  && m_dwIndex == MI_MADA_ACHABEN )
	{
		LPCHARACTER lpCharacter	= GetCharacter();
		if( lpCharacter && lstrcmp( lpCharacter->m_szKey, "Mada_Guildcombatshop" ) == 0 )
		{
			Delete();
		}
	}
}
#endif	// __WORLDSERVER

void CMover::AddSkillPoint( int nPoint )
{
	int nMaxPoint = 1280;	// ?????? JOB_ELEMENTOR ???? ??????
	if( IsPro() )
	{
		if( m_nJob == JOB_KNIGHT || m_nJob == JOB_BLADE )
			nMaxPoint = 870;
		else if( m_nJob == JOB_JESTER || m_nJob == JOB_RANGER )
			nMaxPoint = 910;
		else if( m_nJob == JOB_RINGMASTER )
			nMaxPoint = 902;
		else if( m_nJob == JOB_BILLPOSTER || m_nJob == JOB_PSYCHIKEEPER )
			nMaxPoint = 950;
	}

	if( m_nSkillLevel >= nMaxPoint )
		return;

	if( nMaxPoint < m_nSkillLevel + nPoint )	// ?? ?????? ???? ????(???? ???? ????)
		nPoint = nMaxPoint - m_nSkillLevel;	// ???????? ?????? ?????? ?????? ????

	m_nSkillPoint += nPoint;
	m_nSkillLevel += nPoint;
}

// ???????????
BOOL CMover::IsAttackMode()
{
	return ( m_nAtkCnt && m_nAtkCnt < (SEC1 * 10) );
}

BOOL CMover::IsDropable( CItemElem* pItemElem, BOOL bPK )
{
	return ( IsDropableState( bPK )
		&& IsUsableItem( pItemElem )
		&& !pItemElem->IsBinds()
		&& !( pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild )
		&& !m_Inventory.IsEquip( pItemElem->m_dwObjId )
		&& !IsUsing( pItemElem )
		&& !( pItemElem->GetProp()->dwParts == PARTS_RIDE && pItemElem->GetProp()->dwItemJob == JOB_VAGRANT )
		&& !pItemElem->IsCharged()
		&& pItemElem->GetProp()->dwItemKind3 != IK3_EGG
#ifdef __JEFF_11
		&& pItemElem->m_dwItemId != II_CHP_RED
#endif	// __JEFF_11
		&& pItemElem->m_dwItemId != II_SYS_SYS_SCR_SEALCHARACTER

	);
}

#ifdef __CLIENT
BOOL CMover::IsStateDbuff()
{
	int nAdjParam = GetAdjParam( DST_CHRSTATE );
	if( nAdjParam & CHS_STUN || nAdjParam & CHS_POISON || nAdjParam & CHS_DARK ||
		nAdjParam & CHS_GROGGY || nAdjParam & CHS_SILENT || nAdjParam & CHS_BLEEDING )
	{
		return TRUE;
	}

	return FALSE;
}
#endif


int __IsEndQuestCondition( CMover* pMover, int nQuestId )
{
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->GetQuest( QuestId(nQuestId) );
		if( pCurQuest == NULL || pCurQuest->m_nState == QS_END ) 
			return 0;
		// ???? ???? ???? 1
		if( pQuestProp->m_nEndCondLimitTime == 0 || pCurQuest->m_wTime )
			nResult++;
		// ???? ????  2
		if( pQuestProp->m_dwEndCondPatrolWorld == 0 || pCurQuest->m_bPatrol )
			nResult++;
		// NPC ???? ????  3,4
		for( int i = 0; i < 2; i++ )
		{
			if( pQuestProp->m_nEndCondKillNPCIdx[ i ] == 0 || pCurQuest->m_nKillNPCNum[ i ] >= pQuestProp->m_nEndCondKillNPCNum[ i ] ) 
				nResult++;
		}
		// ???? 5
		if( pQuestProp->m_nEndCondSkillIdx == 0 )
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill( pQuestProp->m_nEndCondSkillIdx );
			if( lpSkill )
			{
				if( pMover->CheckSkill( pQuestProp->m_nEndCondSkillIdx ) && lpSkill->dwLevel >= (DWORD)( pQuestProp->m_nEndCondSkillLvl ) )
					nResult++;
			}
		}
		// Penya 6
		if( pQuestProp->m_nEndCondGold == 0 )
			nResult++;
		else
		{
			if( pMover->GetGold() >= pQuestProp->m_nEndCondGold )
				nResult++;
		}
		// ???? 7
		if( pQuestProp->m_nEndCondLevelMin == 0 || ( pMover->GetLevel() >= pQuestProp->m_nEndCondLevelMin && pMover->GetLevel() <= pQuestProp->m_nEndCondLevelMax ) )
			nResult++;
		if( pQuestProp->m_nEndCondExpPercentMin == 0 || ( pMover->GetExpPercent() >= pQuestProp->m_nEndCondExpPercentMin && pMover->GetExpPercent() <= pQuestProp->m_nEndCondExpPercentMax ) )
			nResult++;


		if( pQuestProp->m_nEndCondPetExp == 0 )
			nResult++;
		else
		{
			CPet* pPet	= pMover->GetPet();
			if( pPet && pPet->GetExpPercent() >= pQuestProp->m_nEndCondPetExp )
				nResult++;
		}
		if( pQuestProp->m_nEndCondPetLevel == -1 )
			nResult++;
		else
		{
			CPet* pPet	= pMover->GetPet();
			if( pPet && pPet->GetLevel() == pQuestProp->m_nEndCondPetLevel )
				nResult++;
		}

		// ???? 8
		if( pQuestProp->m_nEndCondDisguiseMoverIndex == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nEndCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nEndCondDisguiseMoverIndex == pMover->GetIndex() )
				nResult++;
		}
		// ???????? ???? 9 
		if( pQuestProp->m_nEndCondState == 0 )
		{
			int i = QS_BEGIN + 1;
			for( ; i < QS_END; i++ ) // (1~13)
			{
				if( pQuestProp->m_questState[ i ] == NULL )
					break;
			}
			if( ( i - 1 ) == pCurQuest->m_nState )
				nResult++;
		}
		else
		if( pQuestProp->m_nEndCondState == pCurQuest->m_nState )
			nResult++;
		// ???? ?????? ???? 10,11,12,13,14, 15 
		if( pQuestProp->m_nEndCondCompleteQuestOper == 0 ) // 
			nResult += 6;
		else
		if( pQuestProp->m_nEndCondCompleteQuestOper == 1 ) // or
		{
			int i = NULL;
			for( ; i < 6; i++ )
			{
				if( pMover->IsCompleteQuest( pQuestProp->m_nEndCondCompleteQuest[ i ] ) )
					break;
			}
			if( i != 6 )
				nResult += 6;
		}
		else
		if( pQuestProp->m_nEndCondCompleteQuestOper == 2 ) // and
		{
			for( int i = 0; i < 6; i++ )
			{
				if( pQuestProp->m_nEndCondCompleteQuest[ i ] == QuestIdNone || pMover->IsCompleteQuest( pQuestProp->m_nEndCondCompleteQuest[ i ] ) )
					nResult++;
			}
		}
//		else
		// ???? ???? 16
		if( pQuestProp->m_nEndCondParty == 0 )
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
				bLeader	= pParty->m_aMember[0].m_uPlayerId == pMover->m_idPlayer;
			}
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
			bLeader	= ( g_Party.m_aMember[0].m_uPlayerId == pMover->m_idPlayer );
#endif
			if( pQuestProp->m_nEndCondParty == 1 && bParty == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nEndCondParty == 2 && bParty == TRUE )
			{
				if( pQuestProp->m_nEndCondPartyLeader == 0 || ( pQuestProp->m_nEndCondPartyLeader - 1 ) == bLeader )
				{
					if( pQuestProp->m_nEndCondPartyNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nEndCondPartyNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nEndCondPartyNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nEndCondPartyNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nEndCondPartyNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nEndCondPartyNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nEndCondPartyNum )
							nResult++;
					}						
				}
			}
		}
		// ???? ???? 17 
		if( pQuestProp->m_nEndCondGuild == 0 )
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
			CGuild* pGuild	= g_GuildMng.GetGuild( pMover->m_idPlayer );
			if( pGuild )
			{
				bGuild	= TRUE;
				nSize	= pGuild->GetSize();
				bLeader	= ( pGuild->m_idMaster == pMover->m_idPlayer );
			}
			if( pQuestProp->m_nEndCondGuild == 1 && bGuild == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nEndCondGuild == 2 && bGuild == TRUE )
			{
				if( pQuestProp->m_nEndCondGuildLeader == 0 || ( pQuestProp->m_nEndCondGuildLeader - 1 ) == bLeader )
				{
					if( pQuestProp->m_nEndCondGuildNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nEndCondGuildNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nEndCondGuildNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nEndCondGuildNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nEndCondGuildNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nEndCondGuildNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nEndCondGuildNum )
							nResult++;
					}
				}
			}
		}

		// ???? ???????? ?????? ?????? ?????? ????
		int nBufResult = nResult + MAX_QUESTCONDITEM;
		if( pQuestProp->m_nEndCondOneItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( int i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nEndCondOneItemNum )
			{
				QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondOneItem[ i ];
				if( pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == pMover->GetSex() )
				{
					if( pEndCondItem->m_nType == 0 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pMover->GetItemNum( pEndCondItem->m_nItemIdx ) >= pEndCondItem->m_nItemNum )
							{
								nResult = nBufResult;
								break;
							}
						}
						else
							nResult++;
					}
					else
					if( pEndCondItem->m_nType == 1 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pEndCondItem->m_nJobOrItem ) )
						{
							if( pMover->GetItemNum( pEndCondItem->m_nItemIdx ) >= pEndCondItem->m_nItemNum ) 
							{
								nResult = nBufResult;
								break;
							}
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		// ???? ?????? ????  17 + MAX_QUESTCONDITEM
		if( pQuestProp->m_nEndCondItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( int i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nEndCondItemNum )
			{
				QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[ i ];
				if( pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == pMover->GetSex() )
				{
					if( pEndCondItem->m_nType == 0 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pEndCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pEndCondItem->m_nItemIdx ) >= pEndCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
					else
					if( pEndCondItem->m_nType == 1 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pEndCondItem->m_nJobOrItem ) )
						{
							if( pEndCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pEndCondItem->m_nItemIdx ) >= pEndCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		if( pQuestProp->m_nEndCondTSP == 0 )
			++nResult;
		else
		{
			if( pMover->GetCampusPoint() >= pQuestProp->m_nEndCondTSP )
				++nResult;
		}
		// ?????? 17 + MAX_QUESTCONDITEM?? ?????? ???? ???? 
		if( nResult == 21+ MAX_QUESTCONDITEM + MAX_QUESTCONDITEM ) 
		{
			// ???? ???? ???? ?????? ?????? ?????? ???? ?????? 
			// ?????? ?????????? ???????? ???????? ???? ???????????? ?????? ????????.
			if( pCurQuest->m_wTime )
				pCurQuest->m_wTime |= 0x8000;
			return 1;
		}
	}

	return 0;
}

#ifdef __WORLDSERVER
#include "User.h"
#endif // __WORLDSERVER
int __IsBeginQuestCondition( CMover* pMover, int nQuestId )
{
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->GetQuest( QuestId(nQuestId) );
		BOOL bComplete = pMover->IsCompleteQuest( QuestId(nQuestId) );
		// ???????? ??????????, ???? ?????? ???? ???? ???????? ???? ???? ????. ???????? ???????? ?????? ???????? ???? ?????? ?????? ????.
		if( pCurQuest || bComplete ) 
			return 0;

#ifdef __WORLDSERVER
		// ???? ???????? ?????? ?? ?????????? ?????? ?????? ????
		int nItemCount = 0;
		for( int j=0; j<4; j++ )
			if( pQuestProp->m_nBeginSetAddItemIdx[ j ] )
				nItemCount++;
	
		if( IsValidObj( pMover ) && static_cast<CUser*>(pMover)->m_Inventory.GetEmptyCount() < nItemCount )
		{
			static_cast<CUser*>(pMover)->AddDefinedText( TID_GAME_LACKSPACE );
			return 0;
		}
		// ???? ???????? ?????? ?? ?????????? ?????? ?????? ????_END
#endif // __WORLDSERVER
		// ???? ?????? 1,2,3,4,5,6
		for( int i = 0; i < 6; i++ )
		{
			if( pQuestProp->m_anBeginCondPreviousQuest[ i ] == QuestIdNone ) 
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->GetQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				BOOL bPreComplete = pMover->IsCompleteQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				if( pQuestProp->m_nBeginCondPreviousQuestType == 0 )
				{
					if( pPreQuest || bPreComplete )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondPreviousQuestType == 1 )
				{
					if( pPreQuest == NULL && bPreComplete )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondPreviousQuestType == 2 )
				{
					if( pPreQuest && bPreComplete == FALSE )
						nResult++;
				}
			}
		}
		// ???? ?????? 7,8,9,10,11,12
		for( int i = 0; i < 6; i++ )
		{
			if( pQuestProp->m_anBeginCondExclusiveQuest[ i ] == QuestIdNone)
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->GetQuest( pQuestProp->m_anBeginCondExclusiveQuest[ i ] );
				BOOL bPreComplete = pMover->IsCompleteQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				if( pPreQuest == NULL && bPreComplete == FALSE )
					nResult++;
			}
		}
		// ???? 13
		if( pQuestProp->m_nBeginCondJobNum == 0 )
			nResult++;
		else
		for( int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++ )
		{
			if( pQuestProp->m_nBeginCondJob[ i ] == pMover->GetJob() )
			{
				nResult++;
				break;
			}
		}
		// ???? 14
		if( pQuestProp->m_nBeginCondLevelMin == 0 || ( pMover->GetLevel() >= pQuestProp->m_nBeginCondLevelMin && pMover->GetLevel() <= pQuestProp->m_nBeginCondLevelMax ) )
			nResult++;
		// ???? ???? 15
		if( pQuestProp->m_nBeginCondParty == 0 )
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
				
			}
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			if( pQuestProp->m_nBeginCondParty == 1 && bParty == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondParty == 2 && bParty == TRUE )
			{
				if( pQuestProp->m_nBeginCondPartyLeader == -1 || pQuestProp->m_nBeginCondPartyLeader == bLeader )
				{
					if( pQuestProp->m_nBeginCondPartyNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}			
				}
			}
		}
		// ???? ???? 16
		if( pQuestProp->m_nBeginCondGuild == 0 )
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
			/*
#ifdef __WORLDSERVER
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
			}
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			*/
			if( pQuestProp->m_nBeginCondGuild == 1 && bGuild == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondGuild == 2 && bGuild == TRUE )
			{
				if( pQuestProp->m_nBeginCondGuildLeader == -1 || pQuestProp->m_nBeginCondGuildLeader == bLeader )
				{
					if( pQuestProp->m_nBeginCondGuildNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}			
				}
			}
		}
		// ???? 17
		if( pQuestProp->m_nBeginCondSex == -1 || pQuestProp->m_nBeginCondSex == pMover->GetSex() )
			nResult++;
		// ???? 18
		if( pQuestProp->m_nBeginCondSkillIdx == 0 )
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill( pQuestProp->m_nBeginCondSkillIdx );
			if( lpSkill )
			{
				if( pMover->CheckSkill( pQuestProp->m_nBeginCondSkillLvl ) && lpSkill->dwLevel >= (DWORD)( pQuestProp->m_nBeginCondSkillLvl ) )
					nResult++;
			}
		}
		// PK Value 19
		if( pQuestProp->m_nBeginCondPKValue == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondPKValue <= pMover->GetPKValue() )			
				nResult++;
		}
		// ???????? ?????? ???? MAX_QUESTCONDITEM
		if( pQuestProp->m_nBeginCondNotItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( int i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nBeginCondNotItemNum )
			{
				QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondNotItem[ i ];
				if( pBeginCondItem->m_nSex == -1 || pBeginCondItem->m_nSex == pMover->GetSex() )
				{
					if( pBeginCondItem->m_nType == 0 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) < pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
					else
					if( pBeginCondItem->m_nType == 1 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pBeginCondItem->m_nJobOrItem ) )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) < pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		// ???? 21 
		if( pQuestProp->m_nBeginCondDisguiseMoverIndex == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondDisguiseMoverIndex == pMover->GetIndex() )
				nResult++;
		}

		if( pQuestProp->m_nBeginCondPetExp == 0 )
			nResult++;
		else
		{
			CPet* pPet	= pMover->GetPet();
			if( pPet && pPet->GetExpPercent() >= pQuestProp->m_nBeginCondPetExp )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondPetLevel == -1 )
			nResult++;
		else
		{
			CPet* pPet	= pMover->GetPet();
			if( pPet && pPet->GetLevel() == pQuestProp->m_nBeginCondPetLevel )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondTutorialState == -1
			|| pMover->GetTutorialState() >= pQuestProp->m_nBeginCondTutorialState )
			nResult++;

		// ???? ?????? ????  21 + MAX_QUESTCONDITEM
		if( pQuestProp->m_nBeginCondItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( int i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nBeginCondItemNum )
			{
				QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondItem[ i ];
				if( pBeginCondItem->m_nSex == -1 || pBeginCondItem->m_nSex == pMover->GetSex() )
				{
					if( pBeginCondItem->m_nType == 0 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) >= pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
					else
					if( pBeginCondItem->m_nType == 1 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pBeginCondItem->m_nJobOrItem ) )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) >= pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		if( pQuestProp->m_nBeginCondTSP == 0 )
			++nResult;
		else
		{
			if( pMover->GetCampusPoint() >= pQuestProp->m_nBeginCondTSP )
				++nResult;
		}

		if( nResult == 24 + MAX_QUESTCONDITEM + MAX_QUESTCONDITEM )
			return 1;
	}

	return 0;
}

int __IsNextLevelQuest( CMover* pMover, int nQuestId )
{
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->GetQuest( QuestId(nQuestId) );
		BOOL bComplete = pMover->IsCompleteQuest( QuestId(nQuestId) );
		// ???????? ??????????, ???? ?????? ???? ???? ???????? ???? ???? ????. ???????? ???????? ?????? ???????? ???? ?????? ?????? ????.
		if( pCurQuest || bComplete ) 
			return 0;

		// ???? ?????? 1,2,3,4,5,6
		for( int i = 0; i < 6; i++ )
		{
			if( pQuestProp->m_anBeginCondPreviousQuest[ i ] == QuestIdNone ) 
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->GetQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				BOOL bPreComplete = pMover->IsCompleteQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				if( pQuestProp->m_nBeginCondPreviousQuestType == 0 )
				{
					if( pPreQuest || bPreComplete )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondPreviousQuestType == 1 )
				{
					if( pPreQuest == NULL && bPreComplete )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondPreviousQuestType == 2 )
				{
					if( pPreQuest && bPreComplete == FALSE )
						nResult++;
				}
			}
		}
		// ???? ?????? 7,8,9,10,11,12
		for( int i = 0; i < 6; i++ )
		{
			if( pQuestProp->m_anBeginCondExclusiveQuest[ i ] == QuestIdNone ) 
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->GetQuest( pQuestProp->m_anBeginCondExclusiveQuest[ i ] );
				BOOL bPreComplete = pMover->IsCompleteQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				if( pPreQuest == NULL && bPreComplete == FALSE )
					nResult++;
			}
		}
		// ???? 13
		if( pQuestProp->m_nBeginCondJobNum == 0 )
			nResult++;
		else
		for( int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++ )
		{
			if( pQuestProp->m_nBeginCondJob[ i ] == pMover->GetJob() )
			{
				nResult++;
				break;
			}
		}
		// ???? 14
		if( pMover->GetLevel() < pQuestProp->m_nBeginCondLevelMin && pMover->GetLevel() + 5 >= pQuestProp->m_nBeginCondLevelMin )
			nResult++;
		// ???? ???? 15
		if( pQuestProp->m_nBeginCondParty == 0 )
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
				
			}
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			if( pQuestProp->m_nBeginCondParty == 1 && bParty == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondParty == 2 && bParty == TRUE )
			{
				if( pQuestProp->m_nBeginCondPartyLeader == -1 || pQuestProp->m_nBeginCondPartyLeader == bLeader )
				{
					if( pQuestProp->m_nBeginCondPartyNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}			
				}
			}
		}
		// ???? ???? 16
		if( pQuestProp->m_nBeginCondGuild == 0 )
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
			/*
#ifdef __WORLDSERVER
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
			}
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			*/
			if( pQuestProp->m_nBeginCondGuild == 1 && bGuild == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondGuild == 2 && bGuild == TRUE )
			{
				if( pQuestProp->m_nBeginCondGuildLeader == -1 || pQuestProp->m_nBeginCondGuildLeader == bLeader )
				{
					if( pQuestProp->m_nBeginCondGuildNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}			
				}
			}
		}
		// ???? 17
		if( pQuestProp->m_nBeginCondSex == -1 || pQuestProp->m_nBeginCondSex == pMover->GetSex() )
			nResult++;
		// ???? 18
		if( pQuestProp->m_nBeginCondSkillIdx == 0 )
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill( pQuestProp->m_nBeginCondSkillIdx );
			if( lpSkill )
			{
				if( pMover->CheckSkill( pQuestProp->m_nBeginCondSkillLvl ) && (int)( lpSkill->dwLevel ) >= pQuestProp->m_nBeginCondSkillLvl )
					nResult++;
			}
		}
		// PK Value 19
		if( pQuestProp->m_nBeginCondPKValue == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondPKValue <= pMover->GetPKValue() )			
				nResult++;
		}
		// ???????? ?????? ???? MAX_QUESTCONDITEM
		if( pQuestProp->m_nBeginCondNotItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( int i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nBeginCondNotItemNum )
			{
				QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondNotItem[ i ];
				if( pBeginCondItem->m_nSex == -1 || pBeginCondItem->m_nSex == pMover->GetSex() )
				{
					if( pBeginCondItem->m_nType == 0 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) < pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
					else
					if( pBeginCondItem->m_nType == 1 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pBeginCondItem->m_nJobOrItem ) )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) < pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		// ???? 21 
		if( pQuestProp->m_nBeginCondDisguiseMoverIndex == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondDisguiseMoverIndex == pMover->GetIndex() )
				nResult++;
		}

		if( pQuestProp->m_nBeginCondPetExp == 0 )
			nResult++;
		else
		{
			CPet* pPet	= pMover->GetPet();
			if( pPet && pPet->GetExpPercent() >= pQuestProp->m_nBeginCondPetExp )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondPetLevel == -1 )
			nResult++;
		else
		{
			CPet* pPet	= pMover->GetPet();
			if( pPet && pPet->GetLevel() == pQuestProp->m_nBeginCondPetLevel )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondTutorialState == -1
			|| pMover->GetTutorialState() >= pQuestProp->m_nBeginCondTutorialState )
			nResult++;

		// ???? ?????? ????  21 + MAX_QUESTCONDITEM
		if( pQuestProp->m_nBeginCondItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( int i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nBeginCondItemNum )
			{
				QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondItem[ i ];
				if( pBeginCondItem->m_nSex == -1 || pBeginCondItem->m_nSex == pMover->GetSex() )
				{
					if( pBeginCondItem->m_nType == 0 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) >= pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
					else
					if( pBeginCondItem->m_nType == 1 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pBeginCondItem->m_nJobOrItem ) )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) >= pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		if( pQuestProp->m_nBeginCondTSP == 0 )
			++nResult;
		else
		{
			if( pMover->GetCampusPoint() >= pQuestProp->m_nBeginCondTSP )
				++nResult;
		}

		if( nResult == 24 + MAX_QUESTCONDITEM + MAX_QUESTCONDITEM )
			return 1;
	}

	return 0;
}

void CMover::SetPKValue( int nValue )
{
	if( nValue >= 0 )	// overflow!!
		m_nPKValue = nValue;
}
void CMover::SetPKPropensity( DWORD dwValue )
{
	int nValue = dwValue;
	
	if( nValue >= 0 )	// overflow!!
		m_dwPKPropensity = dwValue;
}

DWORD CMover::NextPKPropensity( int nPKValue )
{
	int nResult = 60 * nPKValue * nPKValue;
	if( nResult < 0 )	// overflow!!
		nResult = 0x7fffffff;
	return nResult;
}

#ifdef __WORLDSERVER
void CMover::ProcessSFXDamage( void )
{
	ProcessSFXExpire();

//#ifdef __INFINITE_0227
//	DWORD dwTickCount	= GetTickCount();
//	int cbSFXInfo	= m_mapSFXInfo.size();
//	int cbSFXCount	= m_mapSFXCount.size();
//	int nInfo	= 0;
//	int nCount	= 0;
//#endif	// __INFINITE_0227

	auto iInfo	= m_mapSFXInfo.begin();
	while( iInfo != m_mapSFXInfo.end() )
	{
		auto iInfo2	= iInfo;
		++iInfo;
		OBJID idTarget	= iInfo2->first;
		std::queue<SFXHIT_INFO> &qInfo	= iInfo2->second;
		auto iCount	= m_mapSFXCount.find( idTarget );
		if( iCount == m_mapSFXCount.end() )
			continue;
		std::queue<SFXHIT_COUNT> &qCount	= iCount->second;
		// qInfo, qCount
		while( qInfo.size() > 0 && qCount.size() > 0 )
		{
			SFXHIT_INFO &si	= qInfo.front();
			SFXHIT_COUNT &sc	= qCount.front();
			if( si.dwSkill == sc.dwSkill )
			{
//#ifdef __INFINITE_0227
//					nInfo++;
//					nCount++;
//#endif	// __INFINITE_0227
				if( AttackBySFX( this, si ) == FALSE )	// ?????? ????
				{
					m_mapSFXInfo.erase( idTarget );
					m_mapSFXCount.erase( idTarget );
					break;
				}
				qInfo.pop();
				qCount.pop();
			}
			else
			{
				TRACE( "???? ????\n" );
				if( si.dwTickCount > sc.dwTickCount )
				{
					while( qCount.size() > 0 )
					{
//#ifdef __INFINITE_0227
//						nCount++;
//#endif	// __INFINITE_0227
						SFXHIT_COUNT &sc1	= qCount.front();
						if( si.dwSkill == sc1.dwSkill )
							break;
						qCount.pop();
					}
				}
				else if( si.dwTickCount < sc.dwTickCount )
				{
					while( qInfo.size() > 0 )
					{
//#ifdef __INFINITE_0227
//						nInfo++;
//#endif	// __INFINITE_0227
						SFXHIT_INFO &si1	= qInfo.front();
						if( si1.dwSkill == sc.dwSkill )
							break;
						qInfo.pop();
					}
				}
				else
				{
//#ifdef __INFINITE_0227
//					nInfo++;
//					nCount++;
//#endif	// __INFINITE_0227
					// print
					qInfo.pop();
					qCount.pop();
				}
			}
		}
	}

//#ifdef __INFINITE_0227
//	DWORD d	= GetTickCount() - dwTickCount;
//	if( d > 10 )	// > 10 ms
//		Error( "ProcessSFXDamage: %d ms, i = %d, c = %d, i2 = %d, c2 = %d, n = %s, j = %d",
//		d, cbSFXInfo, cbSFXCount, nInfo, nCount, m_szName, m_nJob );
//#endif	// __INFINITE_0227
}

void CMover::ProcessSFXExpire( void )
{
	DWORD dwTickCount	= GetTickCount();

//#ifdef __INFINITE_0227
//	int cbSFXInfo	= m_mapSFXInfo.size();
//	int cbSFXCount	= m_mapSFXCount.size();
//	int nInfo	= 0;
//	int nCount	= 0;
//#endif	// __INFINITE_0227
	
	// 1.1
	auto iInfo	= m_mapSFXInfo.begin();
	while( iInfo != m_mapSFXInfo.end() )
	{
		auto i2	= iInfo;
		++iInfo;
		OBJID idTarget	= i2->first;
		std::queue<SFXHIT_INFO> &q	= i2->second;
		
		while( q.size() > 0 )
		{
//#ifdef __INFINITE_0227
//			nInfo++;
//#endif	// __INFINITE_0227
			SFXHIT_INFO &si	= q.front();
			if( si.dwTickCount + SEC( 10 ) < dwTickCount )	// ???? ???? ????
			{
				q.pop();	// ???? ???? ????
				TRACE( "idTarget=%d, INFO ???????? ???? ??\n", idTarget );
				continue;
			}
			else
				break;
		}
		if( q.size() == 0 )
		{
			m_mapSFXInfo.erase( idTarget );
			TRACE( "idTarget=%d, INFO ???? ????\n", idTarget);
		}
	}
	// 1.2
	auto iCount	= m_mapSFXCount.begin();
	while( iCount != m_mapSFXCount.end() )
	{
		auto i2	= iCount;
		++iCount;
		OBJID idTarget	= i2->first;
		std::queue<SFXHIT_COUNT> &q	= i2->second;
		
		while( q.size() > 0 )
		{
//#ifdef __INFINITE_0227
//			nCount++;
//#endif	// __INFINITE_0227
			SFXHIT_COUNT &sc	= q.front();
			if( sc.dwTickCount + SEC( 10 ) < dwTickCount )	// ???? ???? ????
			{
				q.pop();	// ???? ???? ????
				TRACE( "idTarget=%d, COUNT ???????? ???? ??\n", idTarget );
				continue;
			}
			else
				break;
		}
		if( q.size() == 0 )
		{
			m_mapSFXCount.erase( idTarget );
			TRACE( "idTarget=%d, COUNT ???? ????\n", idTarget);
		}
	}
/*
#ifdef __INFINITE_0227
	DWORD	d	= GetTickCount() - dwTickCount;
	if( d > 10 )	// > 10 ms
		Error( "ProcessSFXExpire: %d ms, i = %d, c = %d, i2 = %d, c2 = %d, n = %s, j = %d",
		d, cbSFXInfo, cbSFXCount, nInfo, nCount, m_szName, m_nJob );
#endif	// __INFINITE_0227
*/
}
#endif	// __WORLDSERVER

#ifdef __CLIENT
void CMover::ProcessEyeFlash()
{
	if( IsPlayer() && m_pModel && !HasBuffByIk3(IK3_TEXT_DISGUISE) && IsDisguise() == FALSE )
	{
		CModelObject* pModelObj = (CModelObject*)m_pModel;
		
		O3D_ELEMENT* pElement = NULL;
		pElement = pModelObj->GetParts(PARTS_HEAD);
		
		if( pElement && pElement->m_pObject3D )
		{
			GMOBJECT* pGmObj = pElement->m_pObject3D->GetGMOBJECT(0);
			
			if( pGmObj )
			{
				if( m_fDistCamera <= 11.0f )
				{
					if( IsDie() )
					{
						(*pGmObj->m_pMtrlBlkTexture) = CMover::m_pTextureEyeFlash[m_bySex][m_dwHeadMesh];
					}
					else
					{
						if( m_tmEye[0].IsTimeOut() )
						{
							FLOAT fSec = 50;  // ?????????? ????...
							(*pGmObj->m_pMtrlBlkTexture) = CMover::m_pTextureEyeFlash[m_bySex][m_dwHeadMesh];
							m_tmEye[0].Set( fSec+1000 );
							m_tmEye[1].Set( fSec );
						}
						
						if( m_tmEye[1].IsTimeOut() )
						{
							// ???????? ????...
							FLOAT fSec = xRandomF( 2 ) + 5;
							fSec *= 1000.0f;

							if( xRandom( 100 ) < 10 )
								fSec = 500.0f;

							(*pGmObj->m_pMtrlBlkTexture) = CMover::m_pTextureEye[m_bySex][m_dwHeadMesh];
							m_tmEye[1].Set( fSec+1000 );
							m_tmEye[0].Set( fSec );
						}	
					}
				}
				else
				{
					if( IsDie() )
						(*pGmObj->m_pMtrlBlkTexture) = CMover::m_pTextureEyeFlash[m_bySex][m_dwHeadMesh];
					else
						(*pGmObj->m_pMtrlBlkTexture) = CMover::m_pTextureEye[m_bySex][m_dwHeadMesh];
				}
			}
		}
	}
}

void CMover::WingMotionSetting( const CModelObject* pModel )
{
	if( m_pRide == NULL && m_pRide->m_pBone == NULL )
		return;

	if( ( m_pActMover->GetState() & OBJSTA_STAND ) && ( D3DXVec3LengthSq( &m_pActMover->m_vDelta ) <= 0.000f ) && pModel->m_fFrameCurrent == 0.0f )
	{
		if( m_eWingStateFlag != FLOATING )
		{
			CString strMotion = m_pRide->GetMotionFileName( _T("stand") );
			assert( strMotion != _T("") );
			m_pRide->LoadMotion( strMotion );
			m_pRide->SetMotionBlending( TRUE );
			m_pRide->SetLoop( ANILOOP_LOOP );
			m_eWingStateFlag = FLOATING;
		}
	}
	else if( m_pActMover->GetStateFlag() & OBJSTAF_ACC )
	{
		if( m_eWingStateFlag != FLYING )
		{
			CString strMotion = m_pRide->GetMotionFileName( _T("walk") );
			assert( strMotion != _T("") );
			m_pRide->LoadMotion( strMotion );
			m_pRide->SetMotionBlending( TRUE );
			m_pRide->SetLoop( ANILOOP_LOOP );
			m_eWingStateFlag = FLYING;
		}
	}
	else if( ( m_pActMover->GetState() & OBJSTA_LTURN ) || ( m_pActMover->GetState() & OBJSTA_RTURN ) )
	{
		if( m_eWingStateFlag != TURNING )
			m_eWingStateFlag = TURNING;
	}
}

float CMover::GetRideFrameSpeed( void )
{
	float fFrameSpeed = 1.0f;
	if( m_pRide )
	{
		float fNowLengthSq = D3DXVec3LengthSq( &m_pActMover->m_vDelta );
		fFrameSpeed = 1.0f + ( fabs( fNowLengthSq - m_fOldLengthSq ) * 7000.0f );
		m_fOldLengthSq = fNowLengthSq;

		if( ( m_pActMover->GetStateFlag() & OBJSTAF_ACC ) && ( m_pActMover->GetStateFlag() & OBJSTAF_TURBO ) )// ???? ???????? ???? ?????? ??
			fFrameSpeed += 1.2f;

		if( fFrameSpeed < 1.0f )
			fFrameSpeed = 1.0f;

		if( fFrameSpeed > 4.0f )
			fFrameSpeed = 4.0f;
	}
	return fFrameSpeed;
}
#endif //__CLIENT

#ifdef __CLIENT
void CMover::CreateAngelParticle( D3DXVECTOR3 vPos )
{
	FLOAT fAngXZ, fAngH, fDist, fSpeed;
	D3DXVECTOR3 vVel;
		
	fAngXZ = GetAngle();
	fAngXZ += 180.0f; 
	fAngXZ = D3DXToRadian( fAngXZ );
	
	vPos.y += 1.1f;

	for( int j = 0; j < 2; j ++ )
	{
		vPos.y -= 0.03f;

		{
			fAngXZ = (float)(-45.0f + xRandomF(90));
			fAngXZ += GetAngle();
			fAngXZ += 180.0f;
			
			fAngXZ = D3DXToRadian( fAngXZ );
			fAngH = (float)(GetAngleX() + xRandomF(-30) + xRandomF(30) );
			fAngH = D3DXToRadian( fAngH );
			fSpeed = xRandomF(0.02f);
			
			fDist = cosf(-fAngH) * fSpeed;
			vVel.x = sinf(fAngXZ) * fDist;
			vVel.z = -cosf(fAngXZ) * fDist;
			vVel.y = -sinf(-fAngH) * fSpeed;		

			g_ParticleMng.CreateParticle( m_dwAngelKind + xRandom(3), vPos, vVel, GetPos().y + 0.5f );
		}	
	}
}

void CMover::CreatePetParticle( D3DXVECTOR3 vPos )
{
	FLOAT fAngXZ, fAngH, fDist, fSpeed;
	D3DXVECTOR3 vVel;
	
	fAngXZ = GetAngle();
	fAngXZ += 180.0f; 
	fAngXZ = D3DXToRadian( fAngXZ );
	
	for( int j = 0; j < 4; j ++ )
	{
		vPos.y -= 0.03f;
		
		{
			fAngXZ = (float)(-45.0f + xRandomF(360));
			fAngXZ += GetAngle();
			fAngXZ += 180.0f;
			
			fAngXZ = D3DXToRadian( fAngXZ );
			fAngH = (float)(GetAngleX() + xRandomF(-130) + xRandomF(130) );
			fAngH = D3DXToRadian( fAngH );
			fSpeed = xRandomF(0.015f);
			
			fDist = cosf(-fAngH) * fSpeed;
			vVel.x = sinf(fAngXZ) * fDist;
			vVel.z = -cosf(fAngXZ) * fDist;
			vVel.y = -sinf(-fAngH) * fSpeed;
		
			g_ParticleMng.CreateParticle( 13 + xRandom(11), vPos, vVel, GetPos().y + 0.5f );
		}	
	}
}

#endif //__CLIENT

void CMover::ProcessPet( void )
{
#ifdef __CLIENT
	// m_dwPetId ?? ??????????, m_pPet?? ??????, ????????.
	// ??????, ActiveMover?? ??????, ?????????? ???????? ????????.

	if( HasActivatedSystemPet() )
	{
		if( m_pet.GetObj() == NULL )
		{
			DWORD dwIndex	= 0;

			if( IsActiveMover() )
			{
				CItemElem* pItemElem	= GetPetItem();
				if( pItemElem && pItemElem->m_pPet )
					dwIndex	= pItemElem->m_pPet->GetIndex();
			}
			else	//
				dwIndex	= (DWORD)LOWORD( GetPetId() );
			
			if( dwIndex > 0 )
			{
				CMover* pPet	= (CMover *)CreateObj( D3DDEVICE, OT_MOVER, dwIndex );
#ifdef __PET_1024
				if( m_pet.HasName() )
					pPet->SetName( m_pet.GetName() );
#endif	// __PET_1024
				pPet->InitMotion( MTI_STAND );
				pPet->UpdateLocalMatrix();
				pPet->m_pAIInterface	= new CAIEgg( pPet, GetId() );
				pPet->m_dwAIInterface	= AII_EGG;
				pPet->m_dwAIInterfaceState	= 2;
				pPet->SetPos( GetPos() );
				pPet->m_vScrPos	= GetPos();
				GetWorld()->AddObj( pPet, FALSE );
				m_pet.SetObj( pPet );
				
				pPet->InitInterpolation();

				if( IsActiveMover() )
					lstrcpy( pPet->m_szCharacterKey, "MaFl_Pet" );
				CreatePetSfx();
				m_pet.SetLevelup( 0 );

				if( FALSE == g_WndMng.GetWndBase( APP_PET_STATUS ) && this->IsActiveMover() )
				{
					SAFE_DELETE( g_WndMng.m_pWndPetStatus );
					g_WndMng.m_pWndPetStatus = new CWndPetStatus;
					g_WndMng.m_pWndPetStatus->Initialize();
				}

			//	if( FALSE == g_WndMng.GetWndBase( APP_BUFFPET_STATUS ) )
			//	{
			//		SAFE_DELETE( g_WndMng.m_pWndBuffPetStatus );
			//		g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
			//		g_WndMng.m_pWndBuffPetStatus->Initialize();
		//		}
			}
		}				
	}
	else 
	{
		if( m_pet.GetObj() )
		{
			m_pet.GetObj()->Delete();
			m_pet.SetObj( NULL );
			
			if( g_WndMng.GetWndBase( APP_PET_STATUS )  && this->IsActiveMover() )
				g_WndMng.m_pWndPetStatus->Destroy();

		//	if( g_WndMng.GetWndBase( APP_BUFFPET_STATUS ) && this->IsActiveMover( ) )
		//		g_WndMng.m_pWndBuffPetStatus->Destroy();
		}
	}

	if( m_pet.GetObj() != NULL )
	{
		if(IsActiveMover())
		{
			CItemElem* m_pPetElem	= g_pPlayer->GetPetItem();
			if(m_pPetElem != NULL && m_pPetElem->m_pPet->GetLevel() == PL_S )
				CreatePetParticle( m_pet.GetObj()->GetScrPos());
		}
		else
		{
			DWORD dwLevel = (DWORD)HIWORD( GetPetId() );
			if(dwLevel == PL_S)
				CreatePetParticle( m_pet.GetObj()->GetScrPos());
		}
	}
#else	// __CLIENT
	ProcessPetAvail();
	ProcessPetEnergy();
	ProcessPetExp();
	ProcessVisPet();
#endif	// __CLIENT
}

CItemElem* CMover::GetPetItem( void )
{
	if( HasActivatedSystemPet() )
	{
		CItemElem * pItemElem = GetItemId(m_dwPetId);
		if( pItemElem && pItemElem->IsPet() )
			return pItemElem;
	}
	return NULL;
}

CPet* CMover::GetPet( void )
{
	CItemElem* pItemElem	= GetPetItem();
	if( pItemElem )
		return pItemElem->m_pPet;
	return NULL;
}

void CMover::PetLevelup( void )
{
#ifdef __CLIENT
	if( m_pet.GetObj() )	// ???? ????, ???????????? ????
	{
		m_pet.SetLevelup( PF_PET_LEVEL_UP );
		m_pet.GetObj()->Delete();
		m_pet.SetObj( NULL );
	}
#else	// __CLIENT
	CItemElem* pItemElem	= GetPetItem();
	CPet* pPet	= pItemElem->m_pPet;

	if( pPet->GetLevel() == PL_EGG )	// ????
	{
		pPet->SetKind( CPetProperty::GetInstance()->Hatch() );
		pPet->SetLife( 1 );
        ((CUser*)this)->SetHonorAdd(HS_HATCHING_EGG,HI_COUNT_CHECK);
	}

	pPet->IncLevel();
	pItemElem->m_dwItemId	= pPet->GetItemId();
	pPet->SetExp( 0 );
	pPet->SetEnergy( pPet->GetMaxEnergy() );
								
	BYTE nAvailLevel	= CPetProperty::GetInstance()->GetLevelupAvailLevel( pPet->GetLevel() );
	pPet->SetAvailLevel( pPet->GetLevel(), nAvailLevel );
	if( HasPet() )
		RemovePet();

	g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_LEVELUP, pPet );

	( (CUser*)this )->AddPet( pPet, PF_PET_LEVEL_UP );	// ??
	g_UserMng.AddPetLevelup( this, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// ??
#endif	// __CLIENT
}

#ifdef __WORLDSERVER
void CMover::ProcessPetAvail( void )
{
	CPet* pPet	= GetPet();
	if( pPet )
	{
		if( HasPet() == FALSE )
		{
			CItemElem* pItemElem	= GetPetItem();
			const SINGLE_DST dst = pPet->GetAvailDestParam();
			if( dst.nDst > 0 )
			{
				// wID: dwItemId
				// dwLevel: dwDestParam(H) + nParam(L)
				AddBuff( BUFF_PET, (WORD)( pItemElem->m_dwItemId ), MAKELONG( (WORD)dst.nAdj, (WORD)dst.nDst), 0 ); 
				SetDestParam(dst.nDst, dst.nAdj, NULL_CHGPARAM, TRUE);
			}
			SetDestParamRandomOptExtension( pItemElem );
		}
	}
	else
	{
		PetRelease();
	}
}

void CMover::ProcessPetEnergy( void )
{
//#ifdef __INTERNALSERVER
//	if( ( m_nCount & 3 ) != 0 )	// ?????? 0.2???? 1p ????
//#else	// __INTERNALSERVER
	if( ( m_nCount & 15 ) != 0 )	// ?????? 1???? 1p ????
//#endif	// __INTERNALSERVER
		return;

	CPet* pPet	= GetPet();
	if( pPet == NULL )
		return;

	if( pPet->GetLevel() == PL_EGG )
		return;

	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_A )
		|| HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_B )
		)	// ?? ???? ??????(A)	// ?? ???? ??????(B)	?????? ???? ???? ????.
		return;
	if( HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_SPETGOOD )
		&& pPet->GetLevel() == PL_S 
		)	// S?? ?? ???? ????	
		return;

	// ???? ???? ????
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET )
#ifdef __JEFF_11_1
		|| HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET02 )
#endif	// __JEFF_11_1
		)
	{
		// ?????? ???????? ?????? ??????????.
		CItemElem* ptr;
		int nMax	= m_Inventory.GetMax();
		for( int i = 0 ; i < nMax; i++ )
		{
			ptr	= m_Inventory.GetAtId( i );
			if( IsUsableItem( ptr ) && ptr->m_dwItemId == II_SYS_SYS_FEED_01 )	// ???? ?????? ??????
			{
				if( ( m_nCount & 0X1F ) == 0 )	// 2???? ???? 1????		// 0723
					UpdateItem(*ptr, UI::Num::ConsumeOne);
				return;
			}
		}
	}

	CItemElem* pItemElem	= GetPetItem();
	// ?????? 0??????, ???? 1 ????
	int nEnergy		= (int)pPet->GetEnergy();
	if( --nEnergy <= 0 )
	{
		int nLife	= (int)pPet->GetLife();
		if( --nLife >= 0 )
		{
			// ?????? 0 ????????, ????
			pPet->SetLife( nLife );
			pPet->SetEnergy( CPetProperty::GetInstance()->GetMaxEnergy( pPet->GetLevel() ) );
//			pPet->SetExp( 0 );
			
			g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_DEATH, pPet );
		}
		else
		{
			// ?????? 0 ????????, ???? ????
			pPet->SetLife( 0 );
			pPet->SetEnergy( 0 );
			pPet->SetExp( 0 );
			// ???? ???????? ????, ?????? ?????????? ????
			pItemElem->SetFlag( CItemElem::expired );
			UpdateItem(*pItemElem, UI::Flag::Sync);

			g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_DEATH, pPet );

			// ?? ???? ????
			PetRelease();
		}
	}
	else
	{
		pPet->SetEnergy( nEnergy );
	}
	( (CUser*)this )->AddPetState( pItemElem->m_dwObjId , pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp() );
}

void CMover::ProcessPetExp( void )
{
//#ifdef __INTERNALSERVER
//	if( ( m_nCount & 3 ) != 0 )	// 0.2??
//#else	// __INTERNALSERVER
	if( ( m_nCount & 1023 ) != 0 )	// // ???????? 1???? 1?? ????
//#endif	// // ???????? 1???? 1?? ????
		return;

	CPet* pPet	= GetPet();
	if( pPet == NULL )
		return;

	// ?????? ????????, S?? ??????, 
	if( pPet->GetLevel() != PL_EGG && pPet->GetLevel() != PL_S )
	{
		if( pPet->GetExp() == MAX_PET_EXP )
			return;

		float fFactor	= HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_B )? 1.5F: 1.0F;
		
		// 100000 = 100%
		DWORD dwExp	= (DWORD)( pPet->GetExp() + CPetProperty::GetInstance()->GetIncrementExp( pPet->GetLevel() ) * fFactor );
		if( dwExp > MAX_PET_EXP )
			dwExp	= MAX_PET_EXP;
		pPet->SetExp( dwExp );
		( (CUser*)this )->AddPetSetExp( dwExp );
	}
}

#endif	// __WORLDSERVER

#ifdef __CLIENT
void	CMover::CreatePetSfx( void )
{
	CMover* pPet	= m_pet.GetObj();
	if( !pPet )
		return;

	switch( m_pet.GetLevelup() )
	{
		case PF_PET_LEVEL_UP:
			//level up Sound
			PLAYSND( "PCSkillCastCheer.wav" );
			//level up Effect
			break;
		case PF_PET_LEVEL_DOWN:	// ???? : ?? ???????? ???? ???? ????
			// ???? ????
			break;
		case PF_PET_GET_AVAIL:	// ???? : ?? ???????? ???? ???? ????
			if( IsActiveMover() )
			{
				CWndPetMiracle* pWndMiracle = (CWndPetMiracle*)g_WndMng.GetWndBase( APP_PET_MIRACLE );
				if(pWndMiracle != NULL)
				{
					CPet* pCPet	= GetPet();
					int nLevel = pCPet->GetLevel();
					pWndMiracle->ReceiveResult(pCPet->GetAvailLevel(nLevel-1), pCPet->GetAvailLevel(nLevel)); 
				}
			}
			break;
	}
}
#endif	// __CLIENT

void CMover::PetRelease( void )
{
#ifdef __CLIENT
	if( m_pet.GetObj() )
	{
		m_pet.GetObj()->Delete();
		m_pet.SetObj( NULL );
	}
	SetPetId( NULL_ID );

	PLAYSND( "ItemGnCollector.wav" );

#else	// __CLIENT
	CItemElem* pItemElem	= GetPetItem();
	if( !( pItemElem && pItemElem->m_pPet ) )
		return;
	CPetProperty* pProperty	= CPetProperty::GetInstance();
	CPet* pPet	= pItemElem->m_pPet;
	PPETPENALTY pPenalty	= pProperty->GetPenalty( pPet->GetLevel() );
	if( pPenalty )
	{
		if( pPenalty->fExp > 0.0f )
		{
			//Pet ???? ?? ???? ?? D~A???????? ???????? ???? ????,
			// ???? 0%?????? ?? ???? ???????? ??????. 
			DWORD dwExp	= (DWORD)( 100000 * pPenalty->fExp / 100.0F );
			if( dwExp > pPet->GetExp() )
				dwExp	= pPet->GetExp();
			pPet->SetExp( pPet->GetExp() - dwExp );
		}
		if( pPenalty->wEnergy > 0 )
		{
			// S???????? HP?? ????????, ???? 5P ???????? ?? ???? ???????? ??????. 
			int nHP	= (int)pPet->GetEnergy();
			int nPenalty	= (int)pPenalty->wEnergy;
			if( nHP < 5 )
				nPenalty	= 0;
			else if( nHP - nPenalty < 5 )
				nPenalty	= nHP - 5;
			pPet->SetEnergy( nHP - (WORD)nPenalty );
		}
		( (CUser*)this )->AddPetState( pItemElem->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp() );
	}

	if( HasPet() )
		RemovePet();
//#if __VER >= 12 // __PET_0519	// RemovePet() ?????? ????
//	// ?????? ?? ???? ??, ?????? ?? ???? ???? ????
//	ResetDestParamRandomOptExtension( pItemElem );
//#endif	// __PET_0519

	g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_RELEASE, pItemElem->m_pPet );
	SetPetId( NULL_ID );
	g_UserMng.AddPetRelease( this );
#endif	// __CLIENT
}


// 11?? ???????? ???? ???????? ???? ???? ????
// ???? ???????? ???? ???? ???? ?????? ?????? ???????? ??????
BOOL CMover::IsUsingEatPet( CItemElem* pItemElem )
{
	if( HasActivatedEatPet() )	// ???????? ?????? ??????
	{
		CMover* pEatPet		= prj.GetMover( GetEatPetId() );	// ???? ?????? ??????
		if( IsValidObj( pEatPet ) )
		{
			// ?????? ???????? ?????? ???????? ????????
			CAIPet* pAIPet	= static_cast<CAIPet*>( pEatPet->m_pAIInterface );
			if( pAIPet && pAIPet->GetPetItemId() == pItemElem->m_dwObjId )
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::IsUsing( CItemElem* pItemElem )
{	// ???????? ???????????
	if( IsUsingEatPet( pItemElem ) )	// ???????? ????????
		return TRUE;

	if( GetPetId() == pItemElem->m_dwObjId )
		return TRUE;

	if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime > 0
		&& HasBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) ) && !pItemElem->IsFlag( CItemElem::expired ))
			return TRUE;

#ifdef __SYS_TICKET
	if( pItemElem == GetTicket() )
		return TRUE;
#endif	// __SYS_TICKET

	return FALSE;
}

void	CMover::SetHonorCount(int nIdx , int nCount )
{	
	if( nIdx < 0 || nIdx >= MAX_HONOR_TITLE)
		return;

	if( nCount < INT_MAX )
		m_aHonorTitle[nIdx] = nCount;	
}
void	CMover::SetHonorAdd(int nSmallIdx,int nLargeGroup,int nDefault )
{
#ifdef __WORLDSERVER
	ASSERT( IsPlayer() );
	HonorData*	pHonorData = CTitleManager::Instance()->GetHonorDataByID(nSmallIdx,nLargeGroup);
	if(pHonorData)
	{
		if( nDefault == 0)
			SetHonorCount(pHonorData->nID,m_aHonorTitle[pHonorData->nID] + 1);
		else
			SetHonorCount(pHonorData->nID,nDefault);
		if(pHonorData->nNeed == GetHonorTitle(pHonorData->nID) )
		{
			( (CUser*)this )->AddHonorListAck();
			g_dpDBClient.SendLogGetHonorTime(this,pHonorData->nID);
		}

	}

/*
		int nIdx = CTitleManager::Instance()->GetIdx(nSmallIdx,nLargeGroup);
		if( nIdx >= 0)
		{
			SetHonorCount(nIdx,m_aHonorTitle[nIdx] + 1);
			int nNeedCount = CTitleManager::Instance()->GetNeedCount(nSmallIdx,nLargeGroup);
			if( nNeedCount == GetHonorTitle(nIdx) )
			{
				( (CUser*)this )->AddHonorListAck();
				g_dpDBClient.SendLogGetHonorTime(this,nIdx);
			}
		}
*/
#endif	// __WORLDSERVER			
}

void	CMover::CheckHonorStat()
{
#ifdef __WORLDSERVER
	if( m_nHonor < -1 || m_nHonor >= MAX_HONOR_TITLE)
		m_nHonor = -1;


	for (int job = JOB_VAGRANT; job != MAX_JOB; ++job) {
		const int nIdx = CTitleManager::Instance()->GetIdx(job, HI_EARN_TITLE);
		if (nIdx != -1) {
			SetHonorCount(nIdx, IsInteriorityJob(job) ? 1 : 0);
		}
	}

	int nIdx = 0;
	nIdx = CTitleManager::Instance()->GetIdx(HS_STR,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nStr);

	nIdx = CTitleManager::Instance()->GetIdx(HS_STA,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nSta);


	nIdx = CTitleManager::Instance()->GetIdx(HS_DEX,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nDex);

	nIdx = CTitleManager::Instance()->GetIdx(HS_INT,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nInt);

//10	HI_COUNT_CHECK	HS_PVP_POINT01	10	IDS_TITLE_TXT_0011	//???? ??????			//	???? ??????
//11	HI_COUNT_CHECK	HS_PVP_POINT02	100	IDS_TITLE_TXT_0012	//???? ??????			//	???? ??????
//12	HI_COUNT_CHECK	HS_PVP_POINT03	1000	IDS_TITLE_TXT_0013	//???????? ??????			//	???????? ??????
//1/3	HI_COUNT_CHECK	HS_PVP_POINT04	4000	IDS_TITLE_TXT_0014	//???????? ??????			//	???????? ??????
//14	HI_COUNT_CHECK	HS_PVP_POINT05	10000	IDS_TITLE_TXT_0015	//?????? ??????			//	?????? ??????
//15	HI_COUNT_CHECK	HS_PVP_POINT06	20000	IDS_TITLE_TXT_0016	//???? ??????			//	???? ??????
//16	HI_COUNT_CHECK	HS_PVP_POINT07	50000	IDS_TITLE_TXT_0017	//?????? ??????			//	?????? ??????
//17	HI_COUNT_CHECK	HS_PVP_POINT08	1000000	IDS_TITLE_TXT_0018	//?????? ??????			//	?????? ??????
//18	HI_COUNT_CHECK	HS_PVP_POINT09	5000000	IDS_TITLE_TXT_0019	//???????? ??????			//	???????? ??????
//19	HI_COUNT_CHECK	HS_PVP_POINT10	100000000	IDS_TITLE_TXT_0020	//?????? ??????			//
	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT01,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT02,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT03,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT04,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT05,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT06,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT07,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT08,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT09,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);

	nIdx = CTitleManager::Instance()->GetIdx(HS_PVP_POINT10,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nFame);


	nIdx = CTitleManager::Instance()->GetIdx(HS_PK_COUNT,HI_COUNT_CHECK);
	SetHonorCount(nIdx,m_nPKValue);


	nIdx = CTitleManager::Instance()->GetIdx(HS_LORD,HI_EARN_TITLE);
	if( CSLord::Instance()->IsLord( m_idPlayer ) )
		SetHonorCount(nIdx,1);
	else
		SetHonorCount(nIdx,0);


	if( GetHonorTitle(m_nHonor) < CTitleManager::Instance()->GetNeedCount(m_nHonor,-1))
		m_nHonor = -1;

#endif	// __WORLDSERVER
}

void	CMover::CheckHonorTime()
{
#ifdef __WORLDSERVER
	// m_dwHonorCheckTime
	if( m_pActMover->GetActionState() == OBJSTA_COLLECT )	//??????
	{
		DWORD dwTick = GetTickCount();
		if( ( m_dwHonorCheckTime + 3600000 ) < dwTick )
		{
			m_dwHonorCheckTime = dwTick;
            ((CUser*)this)->SetHonorAdd(HS_COLLECT,HI_ELASPED_TIME);
		}
	}

	if( m_vtInfo.VendorIsVendor() || m_vtInfo.IsVendorOpen() )	//??????????
	{
		DWORD dwTick = GetTickCount();
		if( ( m_dwHonorCheckTime + 3600000 ) < dwTick )
		{
			m_dwHonorCheckTime = dwTick;
            ((CUser*)this)->SetHonorAdd(HS_TRADE,HI_ELASPED_TIME);
		}
	}
#endif	// __WORLDSERVER
}


#ifdef __WORLDSERVER
BOOL CMover::IsReturnToBegin( void )
{
	if( m_pAIInterface )
		return m_pAIInterface->IsReturnToBegin();
	return FALSE;
}
#endif	// __WORLDSERVER

void CMover::SetSpeedFactor( FLOAT fSpeedFactor )
{
	if( fabs( m_fSpeedFactor - fSpeedFactor ) > 0.000001F )
	{
		m_fSpeedFactor	= fSpeedFactor;
#ifdef __WORLDSERVER
		g_UserMng.AddSetSpeedFactor( this, fSpeedFactor );
#endif	// __WORLDSERVER
	}
}

void	CMover::ProcessCollecting( void )
{
#ifdef __CLIENT
	CItemElem* pCol		= GetCollector();
	// pCol	= ??????
	if( pCol == NULL )	
	{
		// ???? ????A
		g_WndMng.CloseCollecting();
	}
	else if( m_pActMover->GetActionState() == OBJSTA_COLLECT )
	{
		// ???? ????
		// SFX
		if( ( m_dwFlag & MVRF_COLLECT ) == 0 )
		{
			ItemProp *pHandProp	= GetActiveHandItemProp();
			if( pHandProp->dwSfxObj2 != NULL_ID )
			{
				D3DXVECTOR3 vSrc, vLocal	= D3DXVECTOR3( 0, 0.5f, 0 );
				( (CModelObject *)m_pModel )->GetForcePos( &vLocal, 0, PARTS_RWEAPON, GetMatrixWorld() );
				vSrc	= vLocal;
				CSfx *pSfx	= CreateSfx( D3DDEVICE, pHandProp->dwSfxObj2, vSrc, GetId(), D3DXVECTOR3( 0, 0, 0 ), NULL_ID, -1 );
				if( pSfx )
				{
					pSfx->SetAngle( -GetAngle() + 90.0f );
					//pSfx->SetAngleX(90.0f);
					m_dwFlag	|= MVRF_COLLECT;
				}
			}
			if(g_WndMng.m_pWndCollecting) g_WndMng.m_pWndCollecting->SetButtonCaption(true);
		}	// SFX
		if(g_WndMng.m_pWndCollecting) g_WndMng.m_pWndCollecting->Update();
			if(m_nCltTime < m_nMaxCltTime) ++m_nCltTime;
			else if(m_nCltTime >= m_nMaxCltTime) m_nCltTime = m_nMaxCltTime;
	}
	else
	{
		// ???? ???? ????
		g_WndMng.OpenCollecting();
		g_WndMng.m_pWndCollecting->SetButtonCaption(false);
		g_WndMng.m_pWndCollecting->Update();
	}
#endif	// __CLIENT
}

void	CMover::StartCollecting( void )
{
	ClearDest();
	m_dwFlag |= MVRF_NOACTION;
	m_dwMode |= DONMOVE_MODE;
	SendActMsg( OBJMSG_STOP );
	SendActMsg( OBJMSG_COLLECT );
#ifdef __CLIENT
	CItemElem*			 pCollector = GetCollector();
	CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
	if( pCollector ) m_nMaxCltTime	= pProperty->GetCool( pCollector->GetAbilityOption() ) + 1;
	m_nCltTime = 0;
#endif	// __CLIENT
}

void	CMover::StopCollecting( void )
{
	m_dwFlag &= (~MVRF_NOACTION);
	m_dwMode &= (~DONMOVE_MODE);
	m_pActMover->ResetState( OBJSTA_ACTION_ALL );
	m_pActMover->SetMoveState( OBJSTA_STAND );
	m_pActMover->RemoveStateFlag( OBJSTAF_ETC );
	SetMotion( MTI_STAND );
}

CItemElem* CMover::GetCollector( void )
{
	CItemElem* pCol		= GetWeaponItem();
	if( pCol && ( !pCol->IsCollector() || pCol->IsFlag( CItemElem::expired ) ) )
		pCol	= NULL;
	return pCol;
}

CItemElem*	CMover::GetItemId2( int nPocket, int nItem, BOOL bExpiration )
{
	if( nPocket < 0 )
		return m_Inventory.GetAtId( nItem );
	return m_Pocket.GetAtId( nPocket, nItem, bExpiration );
}

BOOL	CMover::CreateItem2( CItemElem* pItem, int nPocket )
{
#ifdef __WORLDSERVER
	if( nPocket < 0 )
		return CreateItem( pItem );
#endif	// __WORLDSERVER
	return m_Pocket.Add( nPocket, pItem );
}

void	CMover::RemoveItem2( int nItem, short nNum, int nPocket )
{
#ifdef __WORLDSERVER
	if( nPocket < 0 )
		RemoveItem( nItem, nNum );
	else
#endif	// __WORLDSERVER
		m_Pocket.RemoveAtId( nPocket, nItem, nNum );
}

#ifdef __SYS_TICKET
CItemElem*	CMover::GetTicket( void )
{
#ifdef __BUFF_1107
	IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_TICKET );
	if( !pBuff )
		return NULL;
#else	// __BUFF_1107
	LPSKILLINFLUENCE pInfluence		= m_SkillState.GetItemBuf( IK3_TICKET );
	if( !pInfluence )
		return NULL;
#endif	// __BUFF_1107

	CItemElem* pTicket;
	for( DWORD i = 0; i < m_Inventory.m_dwItemMax; i++ )
	{
		pTicket		= m_Inventory.GetAtId( i );
		if( pTicket && pTicket->m_dwKeepTime > 0 && pTicket->GetProp()->dwItemKind3 == IK3_TICKET
#ifdef __BUFF_1107
			&&  pTicket->m_dwItemId == (DWORD)pBuff->GetId()
#else	// __BUFF_1107
			&&  pTicket->m_dwItemId == (DWORD)pInfluence->wID
#endif	// __BUFF_1107
			&& !pTicket->IsFlag( CItemElem::expired ) )
			return pTicket;
	}
	return NULL;
}
#endif	// __SYS_TICKET

BOOL CMover::IsShoutFull( void )
{
	return	( IsSMMode( SM_SHOUT15 ) || IsSMMode( SM_SHOUT30 ) || IsSMMode( SM_SHOUT001 ) );
}

BOOL CMover::IsCommBank( void )
{
	return	( IsSMMode( SM_BANK15 ) || IsSMMode( SM_BANK30 ) || IsSMMode( SM_BANK001 ) );
}

#ifdef __BS_EFFECT_LUA
const char* CMover::GetNameO3D( )
{
	CObject3D* pObj = ((CModelObject*)m_pModel)->GetObject3D( );
	if( !pObj )
	{
		Error( "CMover::GetNameO3D, pObj == NULL" );
		return NULL;
	}

	return pObj->m_szFileName;
}

BOOL CMover::SetDataMTE( const char* alphaTex, const char* eff2ndTex )
{
	// ???? ???????? ????????.
	// ???? ?????? m_pMteData?? ????????.

	CObject3D* pObj = ((CModelObject*)m_pModel)->GetObject3D( );
	if( !pObj )
	{
		Error( "CMover::GetNameO3D, pObj == NULL" );
		return FALSE;
	}

	if( NULL == pObj->m_pMteData )
		pObj->m_pMteData = new CObject3D::MTE_DATA;

	CObject3D::MTE_DATA* pData = pObj->m_pMteData;

	char	szAlphaTexture[MAX_PATH];
	GetFileTitle( alphaTex, szAlphaTexture );
	strcat( szAlphaTexture, "-ef01.tga" );
	
	pData->_pTex[ 0 ] = TexturePool::Get()->GetTexture( DIR_SFXTEX, std::string(szAlphaTexture) );
	pData->_pTex[ 1 ] = TexturePool::Get()->GetTexture( DIR_SFXTEX, std::string(eff2ndTex) );

	if( NULL == pData->_pTex[ 0 ] || NULL == pData->_pTex[ 1 ] )
	{
		Error( "Failed loading MTE texture %s or %s", szAlphaTexture, eff2ndTex );
		return FALSE;
	}
	
	//ok... now you can use this data
	return TRUE;
}

#endif //__BS_EFFECT_LUA

int CMover::GetPerinNum( void )
{
	int nPerin	= 0;
	for( int i = 0; i < m_Inventory.GetMax(); i++ )
	{
		CItemElem * pItem = GetItemId(i);
		if( pItem && pItem->IsPerin() && ::IsUsableItem( pItem ) )
			nPerin	+= pItem->m_nItemNum;
	}
	return nPerin;
}

// ???????????? ?????? ?????? ???? ????????.
__int64 CMover::GetTotalGold( void )
{
	return static_cast<__int64>( GetPerinNum() ) * PERIN_VALUE + static_cast<__int64>( GetGold() );
}

#ifdef __WORLDSERVER
int CMover::RemovePerin( int nPerin )
{
	int nRest	= nPerin;
	for( int i = 0; i < m_Inventory.GetMax() && nRest > 0; i++ )
	{
		CItemElem * pItem = GetItemId(i);
		if( pItem && pItem->IsPerin() && ::IsUsableItem( pItem ) )
		{
			int nRemove		= nRest >= pItem->m_nItemNum? pItem->m_nItemNum: nRest;
			UpdateItem(*pItem, UI::Num::Consume(nRemove));
			nRest	-= nRemove;
		}
	}
	return nPerin - nRest;
}

// iGold?????? ?????? ?????? ???????? ???? ?????????? ????????.
// ?? 2008-10-08: ????
int CMover::RemoveTotalGold( __int64 iGold )
{
	ASSERT( iGold <= GetTotalGold() );
	int nPerin	= (int)( RemovePerin( (int)( iGold / PERIN_VALUE ) ) );
	__int64 iRest	= iGold - ( static_cast<__int64>( nPerin ) * PERIN_VALUE );
	if( iRest > GetGold() )		// ?????? ????????
	{
		RemovePerin( 1 );	// 1?? ?????? ???? ????????,
		iRest	=  -( PERIN_VALUE - iRest );	// '1?? - ??????' ?????? ???????? ?????? ??????.
	}
	AddGold( static_cast<int>( -iRest ), TRUE );
	return nPerin;
}
#endif	// __WORLDSERVER

#ifdef __CLIENT
CClientPet::CClientPet()
:
m_pObj( NULL ),
m_nLevelup( 0 )
{
#ifdef __PET_1024
	m_szName[0]		= '\0';
#endif	// __PET_1024
}

CClientPet::~CClientPet()
{
}

#ifdef __PET_1024
void CClientPet::SetName( const char * szName )
{
	strncpy( m_szName, szName, MAX_PET_NAME-1 );
	m_szName[MAX_PET_NAME-1]		= '\0';
}
#endif	// __PET_1024
#endif	// __CLIENT

