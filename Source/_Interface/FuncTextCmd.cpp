#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "FuncTextCmd.h"
#include "WorldMng.h"
#include "definequest.h"

#ifdef __CLIENT
#include "AppDefine.h"
#include "WndAdminCreateItem.h"
#include "WndIndirectTalk.h"
#include "WndChangeFace.h"
#include "dpclient.h"
#include "timeLimit.h"
#include "MsgHdr.h"
#endif // __CLIENT

#ifdef __WORLDSERVER
#include "User.h"
#include "UserMacro.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
#include "dpSrvr.h"
#include "eveschool.h"
#include "WorldDialog.h"
#include "ItemUpgrade.h"
#endif	// __WORLDSERVER

#include "randomoption.h"

#include "playerdata.h"

#include "SecretRoom.h"

#ifdef __CLIENT
#include "clord.h"
#endif	// __CLIENT

#include "Tax.h"

	#include "honor.h"

#ifdef __WORLDSERVER
#include "RainbowRace.h"
#endif // __WORLDSERVER

#include "guild.h"
#include "party.h"
#include "post.h"

#include "couplehelper.h"
#include "couple.h"

#include "ItemMorph.h"

#ifdef __QUIZ
#ifdef __WORLDSERVER
#include "Quiz.h"
#endif // __WORLDSERVER
#endif // __QUIZ

#include "GuildHouse.h"

#ifdef __WORLDSERVER
#include "CampusHelper.h"
#endif // __WORLDSERVER

CmdFunc::AllCommands g_textCmdFuncs;

#include "definesound.h"


#define TCM_CLIENT 0
#define TCM_SERVER 1
#define TCM_BOTH   2



BOOL TextCmd_InvenClear(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	if( !pUser->HasActivatedSystemPet() )
	{
		int nNum = 0;
		int	nSize	= pUser->m_Inventory.GetMax();
		
		for( int i = 0 ; i < nSize; ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( pItemElem )
			{
				pUser->UpdateItem(*pItemElem, UI::Num::RemoveAll());
			}
		}
	}
	else
		pUser->AddDefinedText( TID_GAME_PET_NOWUSE );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommandList( CScanner& scanner )  
{ 
	return TRUE;
}

BOOL TextCmd_Open( CScanner& scanner )  
{ 
#ifdef __CLIENT
	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	g_WndMng.CreateApplet( dwIdApplet );
#endif
	return TRUE;
}
BOOL TextCmd_Close( CScanner& scanner )  
{ 
#ifdef __CLIENT
	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	CWndBase* pWndBase = g_WndMng.GetWndBase( dwIdApplet );
	if( pWndBase ) pWndBase->Destroy();
#endif
	return TRUE;
}

BOOL TextCmd_Time( CScanner& scanner )  
{ 
#ifdef __CLIENT
	CString string;
	CTime time = CTime::GetCurrentTime();
	//time.Get
	string = time.Format( "Real Time - %H:%M:%S" );
	g_WndMng.PutString( string );
	string.Format( "Madrigal Time - %d:%d:%d\n", g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	g_WndMng.PutString( string );
	
#endif
	return TRUE;
}

BOOL TextCmd_ChangeShopCost(CScanner & scanner) {
#ifdef __WORLDSERVER
	FLOAT f = scanner.GetFloat();
	int nAllServer = scanner.GetNumber();

	if( f > 2.0f )
		f = 1.0f;
	else if( f < 0.5f )
		f = 1.0f;

	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_SHOPCOST );
		return TRUE;
	}

	prj.m_fShopCost = f;	
	g_UserMng.AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetMonsterRespawn(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );

	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 30 ) dwNum = 30;
		if( dwNum < 1 ) dwNum = 1;

		DWORD dwAttackNum	= scanner.GetNumber();
		if( dwAttackNum > dwNum ) dwAttackNum = dwNum;
		if( dwAttackNum < 1 ) dwAttackNum = 0;

		DWORD dwRect = scanner.GetNumber();
		if( dwRect > 255 ) dwRect = 255;
		if( dwRect < 1 ) dwRect = 1;

		DWORD dwTime = scanner.GetNumber();
		if( dwTime > 10800 ) dwTime = 10800;
		if( dwTime < 10 ) dwTime = 10;

		int nAllServer = scanner.GetNumber();
		if( nAllServer != 0 )
		{
			BOOL bFlying = FALSE;
			if( pMoverProp->dwFlying )
				bFlying = TRUE;
			g_DPCoreClient.SendSetMonsterRespawn( pUser->m_idPlayer, pMoverProp->dwID, dwNum, dwAttackNum, dwRect, dwTime, bFlying );
			return TRUE;
		}

		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = pMoverProp->dwID;
		ri.m_cb = dwNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if( pMoverProp->dwFlying != 0 )
			ri.m_vPos = vPos;
		ri.m_rect.left		= (LONG)( vPos.x - dwRect );
		ri.m_rect.right		= (LONG)( vPos.x + dwRect );
		ri.m_rect.top		= (LONG)( vPos.z - dwRect );
		ri.m_rect.bottom	= (LONG)( vPos.z + dwRect );
		ri.m_uTime			= (u_short)( dwTime );
		ri.m_cbTime			= 0;

		char chMessage[512] = {0,};
#ifdef __S1108_BACK_END_SYSTEM
			pWorld->m_respawner.Add( ri, TRUE );
#else // __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri );
#endif // __S1108_BACK_END_SYSTEM

		sprintf( chMessage, "Add Respwan Monster : %s(%d/%d) Rect(%d, %d, %d, %d) Time : %d", 
			pMoverProp->szName, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
		pUser->AddText( chMessage );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __S1108_BACK_END_SYSTEM

BOOL TextCmd_PropMonster( CScanner & scanner )
{
#ifdef __CLIENT
	char chMessage[1024] = {0,};
	if( 0 < prj.m_nAddMonsterPropSize )
	{
		for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
		{
			sprintf( chMessage, "Monster Prop(%s) AttackPower(%d), Defence(%d), Exp(%d), Hitpioint(%d), ItemDorp(%d), Penya(%d)", 
				prj.m_aAddProp[i].szMonsterName, prj.m_aAddProp[i].nAttackPower, prj.m_aAddProp[i].nDefence, prj.m_aAddProp[i].nExp,
				prj.m_aAddProp[i].nHitPoint, prj.m_aAddProp[i].nItemDrop, prj.m_aAddProp[i].nPenya	);
			g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
		}
	}
	else
	{
		sprintf( chMessage, "Monster Prop Not Data" );
		g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
	}
#endif	// __CLIENT
	return TRUE;
}
#endif // __S1108_BACK_END_SYSTEM

BOOL TextCmd_GameSetting(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->AddGameSetting();	
#endif // __WORLDSERVER
	return TRUE;	
}

BOOL TextCmd_ChangeFace(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	DWORD dwFace = scanner.GetNumber();
	if( dwFace < 0 || 4 < dwFace )
		return TRUE;
	
	if( (pUser->m_dwMode & NOTFRESH_MODE) || (pUser->m_dwMode & NOTFRESH_MODE2) )
	{
		pUser->m_dwHeadMesh = dwFace;
		g_UserMng.AddChangeFace( pUser->m_idPlayer, dwFace );
		if( pUser->m_dwMode & NOTFRESH_MODE )
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE;
			pUser->AddDefinedText( TID_CHANGEFACE_ONE, "" );
//			pUser->AddText( "?????????? ???? ??????????????" );
		}
		else
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE2;
			pUser->AddDefinedText( TID_CHANGEFACE_TWO, "" );
//			pUser->AddText( "?????????? 2?? ??????????????" );
		}
	}
	else
	{
		pUser->AddDefinedText( TID_CHANGEFACE_THREE, "" );
//		pUser->AddText( "?????????? 2?? ???? ???????? ???????? ????????" );
	}
#else // __WORLDSERVER
	CWndChangeSex* pWndChangeSex	= (CWndChangeSex*)g_WndMng.GetWndBase( APP_CHANGESEX );
	if( NULL == pWndChangeSex )
	{
		pWndChangeSex	= new CWndChangeSex;
		pWndChangeSex->Initialize();
	}
	pWndChangeSex->SetData( NULL_ID, NULL_ID );
	return FALSE;
#endif
	return TRUE;
}

BOOL TextCmd_AroundKill(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( pUser->GetWeaponItem() == NULL )
		return TRUE;

	CWorld* pWorld	= pUser->GetWorld();
	if( pWorld )
		pUser->SendDamageAround( AF_MAGICSKILL, (CMover*)pUser, OBJTYPE_MONSTER, 1, 64.0f, 0.0, 1.0f );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_PetLevel(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CPet* pPet	= pUser->GetPet();
	if( pPet && pPet->GetExpPercent() == 100 )
		pUser->PetLevelup();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_MakePetFeed( CScanner & s )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndPetFoodMill == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndPetFoodMill );
		g_WndMng.m_pWndPetFoodMill = new CWndPetFoodMill;
		g_WndMng.m_pWndPetFoodMill->Initialize( &g_WndMng, APP_PET_FOOD );
		return FALSE;
	}
#endif	// __CLIENT
	return TRUE;
}

BOOL	TextCmd_PetExp(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CPet* pPet	= pUser->GetPet();
	if( pPet && pPet->GetLevel() != PL_S )
	{
		pPet->SetExp( MAX_PET_EXP );
		pUser->AddPetSetExp( pPet->GetExp() );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Pet( CScanner & s )
{
	// /pet 1 100
#ifdef __WORLDSERVER
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	DWORD idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
	if( idPlayer == 0 )	//
		return TRUE;
	CUser* pTarget	= (CUser*)prj.GetUserByID( idPlayer );
	if( IsValidObj( pTarget ) == FALSE )
		return TRUE;
	CPet* pPet	= pTarget->GetPet();
	if( pPet == NULL )	//
		return TRUE;

	// kind
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nKind	= atoi( s.token );
	if( nKind >= PK_MAX )
		return TRUE;

	// exp
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nExpRate	= atoi( s.token );

	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nLevel		= s.Token.GetLength();

	if( nLevel > PL_S )
		return TRUE;

	BYTE anAvail[PL_MAX - 1]	= { 0,};
	char sAvail[2]	= { 0,};
	
	for( int i = 0; i < nLevel; i++ )
	{
		sAvail[0]	= s.Token.GetAt( i );
		sAvail[1]	= '\0';
		anAvail[i]	= atoi( sAvail );
		if( anAvail[i] < 1 || anAvail[i] > 9 )
			return TRUE;
	}

	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nLife	= atoi( s.token );
	if( nLife > 99 )
		nLife	= 99;

	CItemElem* pItemElem	= pTarget->GetPetItem();
	pPet->SetKind( nKind );
	pPet->SetLevel( nLevel );
	if( nLevel == PL_EGG )
		pPet->SetKind( 0 );	// initialize
	pItemElem->m_dwItemId	= pPet->GetItemId();
	pPet->SetEnergy( pPet->GetMaxEnergy() );
	DWORD dwExp		= pPet->GetMaxExp() * nExpRate / 100;
	pPet->SetExp( dwExp );

	for( int i = PL_D; i <= nLevel; i++ )
		pPet->SetAvailLevel( i, anAvail[i-1] );
	for( int i = nLevel + 1; i <= PL_S; i++ )
		pPet->SetAvailLevel( i, 0 );

	pPet->SetLife( nLife );

	if( pTarget->HasPet() )
		pTarget->RemovePet();

	g_dpDBClient.CalluspPetLog( pTarget->m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_LEVELUP, pPet );

	pTarget->AddPet( pPet, PF_PET_GET_AVAIL );	// ??
	g_UserMng.AddPetLevelup( pTarget, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// ??
#endif	// __WORLDSERVER
	return TRUE;
}


BOOL	TextCmd_MoveItem_Pocket( CScanner & s )
{
#ifdef __CLIENT
	int	nPocket1	= s.GetNumber();
	int nData	= s.GetNumber();
	int nNum	= s.GetNumber();
	int	nPocket2	= s.GetNumber();
	CItemElem* pItem	= NULL;
	if( nPocket1 < 0 )
		pItem	= g_pPlayer->m_Inventory.GetAt( nData );
	else
	{
		pItem	= g_pPlayer->m_Pocket.GetAtId( nPocket1, nData );
	}
	if( pItem )
		g_DPlay.SendMoveItem_Pocket( nPocket1, pItem->m_dwObjId, nNum, nPocket2 );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_AvailPocket( CScanner & s )
{
#ifdef __CLIENT
	int nPocket		= s.GetNumber();
	CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAt( 0 );
	if( pItemElem )
		g_DPlay.SendAvailPocket( nPocket, pItemElem->m_dwObjId );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_PocketView(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->AddPocketView();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RefineCollector(CScanner & s, CPlayer_ * pUser) {
// 0????
#ifdef __WORLDSERVER
	int nAbilityOption	= s.GetNumber();
	if( s.tok == FINISHED )
		nAbilityOption	= 0;
	if( nAbilityOption > 5 )
		nAbilityOption	= 5;
	CItemElem* pTarget	= pUser->m_Inventory.GetAt( 0 );
	if( pTarget && pTarget->IsCollector( TRUE ) )
	{
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		pUser->UpdateItem(*pTarget, UI::AbilityOption::Set(nAbilityOption));
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_StartCollecting(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->StartCollecting();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_StopCollecting(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->StopCollecting();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_DoUseItemBattery(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->DoUseItemBattery();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RefineAccessory(CScanner & s, CPlayer_ * pUser) {
	// 0????
#ifdef __WORLDSERVER
	int nAbilityOption	= s.GetNumber();
	if( s.tok == FINISHED )
		nAbilityOption	= 0;
	CItemElem* pTarget	= pUser->m_Inventory.GetAt( 0 );
	if( pTarget && pTarget->IsAccessory() )
		pUser->UpdateItem(*pTarget, UI::AbilityOption::Set(nAbilityOption));
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetRandomOption(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CItemElem * const pItemElem = pUser->m_Inventory.GetAt( 0 );
	if (!pItemElem) return TRUE;
	
	const int nRandomOptionKind = g_xRandomOptionProperty.GetRandomOptionKind(pItemElem);
	if (nRandomOptionKind < 0) return TRUE;

	g_xRandomOptionProperty.InitializeRandomOption( pItemElem->GetRandomOptItemIdPtr() );
	int cb	= 0;
	int nDst	= s.GetNumber();
	while (s.tok != FINISHED) {
		const int nAdj = s.GetNumber();
		if (s.tok == FINISHED) break;

		const bool isInvalid =
			// Bad DST
			nDst == 48 || nDst < 1 || nDst >= MAX_ADJPARAMARY
			// ADJ over the capacity of a 9 bits unsigned int
			|| nAdj < -512 || nAdj >= 512;

		if (!isInvalid) {
			g_xRandomOptionProperty.SetParam(pItemElem->GetRandomOptItemIdPtr(), nDst, nAdj);
			cb++;
		}

		if (cb >= MAX_RANDOM_OPTION) break;
		nDst = s.GetNumber();
	}

	pUser->UpdateItem(*pItemElem, UI::RandomOptItem::Sync);

#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GenRandomOption(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CItemElem* pItemElem	= pUser->m_Inventory.GetAt( 0 );
	if( pItemElem )
	{
		int nRandomOptionKind	= g_xRandomOptionProperty.GetRandomOptionKind( pItemElem );
		if( nRandomOptionKind >= 0 )	// ?????? ????, ?????? ?????? ?????? ????
		{
			g_xRandomOptionProperty.InitializeRandomOption( pItemElem->GetRandomOptItemIdPtr() );
			g_xRandomOptionProperty.GenRandomOption( pItemElem->GetRandomOptItemIdPtr(), nRandomOptionKind, pItemElem->GetProp()->dwParts );
			pUser->UpdateItem(*pItemElem, UI::RandomOptItem::Sync);
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PickupPetAwakeningCancel( CScanner & )
{
#ifdef __CLIENT
	SAFE_DELETE( g_WndMng.m_pWndPetAwakCancel );
	g_WndMng.m_pWndPetAwakCancel = new CWndPetAwakCancel;
	g_WndMng.m_pWndPetAwakCancel->Initialize(&g_WndMng);
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_InitializeRandomOption(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CItemElem* pItemElem	= pUser->m_Inventory.GetAt( 0 );
	if( pItemElem )
	{
		int nRandomOptionKind	= g_xRandomOptionProperty.GetRandomOptionKind( pItemElem );
		if( nRandomOptionKind >= 0 )	// ?????? ????, ?????? ?????? ?????? ????
		{
			g_xRandomOptionProperty.InitializeRandomOption( pItemElem->GetRandomOptItemIdPtr() );
			pUser->UpdateItem(*pItemElem, UI::RandomOptItem::Sync);
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemLevel(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int i	= s.GetNumber();
	if( s.tok == FINISHED )
		i	= 0;
	CItemElem* pTarget	= pUser->m_Inventory.GetAt( 0 );
	if( pTarget )
	{
		ItemProp* pProp	= pTarget->GetProp();
		if( pProp->dwParts != NULL_ID && pProp->dwLimitLevel1 != 0xFFFFFFFF )
		{
			pTarget->SetLevelDown( i );
			pUser->UpdateItem(*pTarget, UI::RandomOptItem::Sync);
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Level(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	CString strJob = scanner.Token;

	LONG nLevel = scanner.GetNumber();
	if (nLevel == 0) nLevel = 1;

	int nJob = JOB_VAGRANT;

	LONG nLegend = scanner.GetNumber();

	for (int i = 0; i < MAX_JOB; i++) {
		if (strJob == prj.jobs.info[i].szName || strJob == prj.jobs.info[i].szEName) {
			if (nLegend == 0) {
				nJob = i;
				break;
			} else {
				--nLegend;
			}
		}
	}

	pUser->InitLevel(nJob, nLevel);

#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __SFX_OPT
BOOL TextCmd_SfxLv( CScanner & scanner )
{
	int nLevel = scanner.GetNumber();
	if(nLevel > 5) nLevel = 5;
	if(nLevel < 0) nLevel = 0;
	g_Option.m_nSfxLevel = nLevel;

	return TRUE;
}
#endif

BOOL TextCmd_ChangeJob(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	CString strJob = scanner.Token;
	
	int nJob = JOB_VAGRANT;
	
	for (int i = 0; i < MAX_JOB; i++) {
		if (strJob == prj.jobs.info[i].szName || strJob == prj.jobs.info[i].szName) {
			nJob = i;
			break;
		}
	}
	
	char chMessage[MAX_PATH] = {0,};
	if( nJob == 0 )
	{
		sprintf( chMessage, "Error Job Name or Number" );
		pUser->AddText( chMessage );		
		return TRUE;
	}
	
	if( pUser->AddChangeJob( nJob ) )
	{
		( (CUser*)pUser )->AddSetChangeJob( nJob );
		g_UserMng.AddNearSetChangeJob(pUser, nJob);
		g_dpDBClient.SendLogLevelUp( (CUser*)pUser, 4 );
		g_dpDBClient.SendUpdatePlayerData( pUser );
		return TRUE;
	}
	else
	{
		sprintf( chMessage, "Error 1ch -> 2ch" );
		pUser->AddText( chMessage );		
		return TRUE;
	}

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_stat(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	CString strstat = scanner.Token;
	
	DWORD dwNum	= scanner.GetNumber();

	if( 2 <= strstat.GetLength() && strstat.GetLength() <= 7)
	{
		strstat.MakeLower();

		if( strcmp( strstat, "str" ) == 0 )
		{
			pUser->m_nStr = dwNum;
		}
		else
		if( strcmp( strstat, "sta" ) == 0 )
		{
			pUser->m_nSta = dwNum;
		}
		else
		if( strcmp( strstat, "dex" ) == 0 )
		{
			pUser->m_nDex = dwNum;
		}
		else
		if( strcmp( strstat, "int" ) == 0 )
		{
			pUser->m_nInt = dwNum;
		}
		else
		if( strcmp( strstat, "gp" ) == 0 )
		{
			pUser->m_nRemainGP = dwNum;
		}
		else
		if( strcmp( strstat, "restate" ) == 0 )
		{
			pUser->ReState();
			return FALSE;
		}
		else if( strcmp( strstat, "all" ) == 0 )
		{
			pUser->m_nStr = dwNum;
			pUser->m_nSta = dwNum;
			pUser->m_nDex = dwNum;
			pUser->m_nInt = dwNum;
		}
		else
		{
			strstat += "unknown setting target";
			pUser->AddText( strstat );
			return FALSE;
		}
	}
	else
	{
		strstat += "unknown setting target";
		pUser->AddText( strstat );
		return FALSE;
	}

	pUser->AddSetState( pUser->m_nStr, pUser->m_nSta, pUser->m_nDex, pUser->m_nInt, pUser->m_nRemainGP );
	pUser->CheckHonorStat();
	pUser->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( pUser, pUser->m_nHonor);
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetSnoop(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	s.GetToken();
	if( s.tok != FINISHED )
	{
		if( lstrcmp( pUser->GetName(), s.Token ) )
		{
			u_long idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
			if( idPlayer > 0 )
			{
				BOOL bRelease	= FALSE;
				s.GetToken();
				if( s.tok != FINISHED )
					bRelease	= (BOOL)atoi( s.Token );
				g_DPCoreClient.SendSetSnoop( idPlayer, pUser->m_idPlayer,  bRelease );
			}
			else
				pUser->AddReturnSay( 3, s.Token );
		}
	}
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_SetSnoopGuild(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	s.GetToken();
	if( s.tok != FINISHED )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( s.Token );
		if( pGuild )
		{
			BOOL bRelease	= FALSE;
			s.GetToken();
			if( s.tok != FINISHED )
				bRelease	= (BOOL)atoi( s.Token );
			g_DPCoreClient.SendSetSnoopGuild( pGuild->m_idGuild, bRelease );
		}
	}
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_QuerySetPlayerName(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	CString strPlayer	= scanner.Token;
	strPlayer.TrimLeft();
	strPlayer.TrimRight();
	g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, strPlayer, MAKELONG( 0xffff, 0 ) );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_QuerySetGuildName(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	CString strGuild	= scanner.Token;
	strGuild.TrimLeft();
	strGuild.TrimRight();
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, (LPSTR)(LPCSTR)strGuild, 0xff );
	}
	else
	{
		// is not kingpin
	}
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_CreateGuild(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	GUILD_MEMBER_INFO	info;
	info.idPlayer	= pUser->m_idPlayer;
	g_DPCoreClient.SendCreateGuild( &info, 1, scanner.Token );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_DestroyGuild( CScanner & )
{
#ifdef __CLIENT
	g_DPlay.SendDestroyGuild( g_pPlayer->m_idPlayer );
#endif
	return TRUE; 
}

BOOL TextCmd_RemoveGuildMember( CScanner & scanner )
{
#ifdef __CLIENT
	scanner.GetToken();
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	lstrcpy( lpszPlayer, scanner.Token );
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( lpszPlayer );
	if( idPlayer != 0 )
		g_DPlay.SendRemoveGuildMember( g_pPlayer->m_idPlayer, idPlayer );
	return TRUE;
#endif	// __CLIENT
	return FALSE;
}

BOOL TextCmd_GuildChat(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
#ifdef __JEFF_9_20
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#endif	// __JEFF_9_20

	char sChat[260]		= { 0, };
	scanner.GetLastFull();
	if( strlen( scanner.token ) >= 260 )
		return TRUE;
	strcpy( sChat, scanner.token );

	StringTrimRight( sChat );
	if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
		ParsingEffect(sChat, strlen(sChat) );
	
	RemoveCRLF( sChat );

	g_DPCoreClient.SendGuildChat( pUser, sChat );
	return TRUE;
#else	// __WORLDSERVER
#ifdef __CLIENT
	CString string	= scanner.m_pProg;
	g_WndMng.WordChange( string );

	if( ::GetLanguage() == LANG_THA )
		string = '"'+string+'"';

	CString strCommand;
	strCommand.Format( "/g %s", string );
	g_DPlay.SendChat( strCommand );
	return FALSE;
#endif	// __CLIENT
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_DeclWar( CScanner & scanner )
{
#ifdef __CLIENT
	char szGuild[MAX_G_NAME]	= { 0, };
	scanner.GetLastFull();
	if( strlen( scanner.token ) >= MAX_G_NAME )
		return TRUE;
	strcpy( szGuild, scanner.token );
	StringTrimRight( szGuild );
	g_DPlay.SendDeclWar( g_pPlayer->m_idPlayer, szGuild );
#endif	// __CLIENT
	return FALSE;
}


// ???? ?????????? ???????? ?????? ??????????.
BOOL TextCmd_GuildRanking( CScanner & )
{
#ifdef __WORLDSERVER
	// TRANS ???????? ???? ???? ?????? ???????? ???????? ??????.
	g_dpDBClient.UpdateGuildRankingUpdate();
#endif

	return TRUE;
}

// ???? ???????? DB?? ???????? ?????? ??????????.
BOOL TextCmd_GuildRankingDBUpdate( CScanner & )
{
#ifdef __WORLDSERVER
	// TRANS ???????? ???? ???? ?????? ???????? ???????? ??????.
	g_dpDBClient.UpdateGuildRankingUpdate();
#endif
	
	return TRUE;
}

BOOL TextCmd_GuildStat(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	CString strstat = scanner.Token;
	
	strstat.MakeLower();

	if( strstat == "logo" )
	{
		DWORD dwLogo = scanner.GetNumber();
		TRACE("guild Logo:%d\n", dwLogo);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_LOGO, dwLogo );
	}
	else
	if( strstat == "pxp" )
	{
		DWORD dwPxpCount = scanner.GetNumber();
		TRACE("guild pxpCount:%d\n", dwPxpCount);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PXPCOUNT, dwPxpCount );
	}
	else
	if( strstat == "penya" )
	{
		DWORD dwPenya = scanner.GetNumber();
		TRACE("guild dwPenya:%d\n", dwPenya);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PENYA, dwPenya);
	}
	else
	if( strstat == "notice" )
	{
		scanner.GetToken();
		TRACE("guild notice:%s\n", scanner.Token);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_NOTICE, (DWORD)(LPCTSTR)scanner.Token);
	}
	else
	{
		strstat += "syntax error guild stat command.";
		pUser->AddText( strstat );
	}

#endif 
	return FALSE;
}

BOOL TextCmd_SetGuildQuest(CScanner & s) {
#ifdef __WORLDSERVER

	s.GetToken();
	char sGuild[MAX_G_NAME];
	lstrcpy( sGuild, s.Token );
	int nQuestId	= s.GetNumber();
	int nState		= s.GetNumber();
	GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( nQuestId );
	if( !pProp )
		return FALSE;

	CGuild* pGuild	= g_GuildMng.GetGuild( sGuild );
	if( pGuild )
	{
		if( nState < QS_BEGIN || nState > QS_END )
		{
		}
		else
		{
			pGuild->SetQuest( nQuestId, nState );
			g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, nQuestId, nState );
		}
	}
	return TRUE;

#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_PartyLevel(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	DWORD dwLevel = scanner.GetNumber();
	DWORD dwPoint = scanner.GetNumber();
	if( dwPoint == 0 )
		dwPoint = 100;

	TRACE("plv LV:%d POINT:%d\n", dwLevel, dwPoint);
	
	CParty* pParty;
	pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty )
	{
		pParty->SetPartyLevel( pUser, dwLevel, dwPoint, pParty->GetExp() );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL  TextCmd_InitSkillExp(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( pUser->InitSkillExp() == TRUE )
		pUser->AddInitSkill();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SkillLevel(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	const DWORD dwSkillKind	= scanner.GetNumber();
	const DWORD dwSkillLevel = scanner.GetNumber();

	SKILL * pSkill = pUser->GetSkill( dwSkillKind );
	if (!pSkill) return FALSE;

	pSkill->dwLevel = dwSkillLevel;
	pUser->AddSetSkill(*pSkill);
#endif // __WORLDSERVER
#ifdef __CLIENT

	DWORD dwSkillLevel = scanner.GetNumber();

	CWndSkillTreeEx* pSkill = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
	if( pSkill )
	{
		int nIndex = pSkill->GetCurSelect();
		if( nIndex == -1 )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "???????? ???? ?????? ???????? ????????", NULL, 0xffff0000 );
			return FALSE;
		}
		LPSKILL pSkillbuf = pSkill->GetSkill( nIndex );
		if( pSkillbuf == NULL ) 
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "???????? ???? ?????? ???????? ????????", NULL, 0xffff0000 );
			return FALSE;
		}

		ItemProp* pSkillProp = prj.GetSkillProp( pSkillbuf->dwSkill );

		if( pSkillProp->dwExpertMax < 1 || pSkillProp->dwExpertMax < dwSkillLevel )
		{
			char szMessage[MAX_PATH];
			sprintf ( szMessage, prj.GetText(TID_GAME_SKILLLEVELLIMIT), pSkillProp->szName, pSkillProp->dwExpertMax );
//			sprintf ( szMessage, "%s' ?????? 1 ~ %d ???? ?????? ?????? ????????", pSkillProp->szName, pSkillProp->dwExpertMax );

			g_WndMng.PutString( szMessage, NULL, 0xffff0000 );
			return FALSE;
		}
		char szSkillLevel[MAX_PATH];
		sprintf( szSkillLevel, prj.GetText(TID_GAME_GAMETEXT001), pSkillbuf->dwSkill, dwSkillLevel );
		scanner.SetProg( szSkillLevel );		
		//sprintf( scanner.pBuf, "/???? %d %d", pSkillbuf->dwSkill, dwSkillLevel );
	}
	else
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
//		g_WndMng.PutString( "???????? ???? ?????? ???????? ????????", NULL, 0xffff0000 );
		return FALSE;
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SkillLevelAll(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	for (SKILL & skill : pUser->m_jobSkills) {
		const ItemProp * pSkillProp = skill.GetProp();
		if (!pSkillProp) continue;

		skill.dwLevel = pSkillProp->dwExpertMax;
		pUser->AddSetSkill(skill);
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_whisper(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	static	CHAR	lpString[260];

	if( pUser->IsMode( SAYTALK_MODE ) )
		return TRUE;
#ifdef __JEFF_9_20
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#endif	// __JEFF_9_20

	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
			u_long idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
			if( idPlayer > 0 ) 
			{
				scanner.GetLastFull();
				if( strlen( scanner.token ) >= 260 )	
					return TRUE;
				strcpy( lpString, scanner.token );
				
				StringTrimRight( lpString );
				
				if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
				{
					ParsingEffect(lpString, strlen(lpString) );
				}
				RemoveCRLF( lpString );
				
				if( 0 < strlen(lpString) )
					g_DPCoreClient.SendWhisper( pUser->m_idPlayer, idPlayer, lpString );
			}
			else 
			{
				//scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
				pUser->AddReturnSay( 3, scanner.Token );
			}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// ???? ???????? ????????.
	}
#endif	// __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_say(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	static	CHAR	lpString[1024];

	if( pUser->IsMode( SAYTALK_MODE ) )
		return TRUE;
#ifdef __JEFF_9_20
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#endif	// __JEFF_9_20

	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
		if( idPlayer > 0 ) 
		{
			scanner.GetLastFull();
			if( strlen( scanner.token ) > 260 )	//
				return TRUE;

			strcpy( lpString, scanner.token );

			StringTrimRight( lpString );
			
			if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
			{
				ParsingEffect(lpString, strlen(lpString) );
			}
			RemoveCRLF( lpString );
			
			g_DPCoreClient.SendSay( pUser->m_idPlayer, idPlayer, lpString );
		}
		else 
		{
			//scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// ???? ???????? ????????.
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ResistItem(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	DWORD dwObjId	= scanner.GetNumber();
	BYTE bItemResist = scanner.GetNumber();
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();

	if( bItemResist < 0 || 5 < bItemResist )
	{
		return FALSE;
	}
	if( nResistAbilityOption < 0 || CItemUpgrade::GetInstance()->GetMaxAttributeEnchantSize() < nResistAbilityOption 
		|| nAbilityOption < 0 || CItemUpgrade::GetInstance()->GetMaxGeneralEnchantSize() < nAbilityOption )
	{
		return FALSE;
	}

	if( bItemResist == 0 )
	{
		nResistAbilityOption = 0;
	}

	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
		return FALSE;
	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return FALSE;
	}

	pUser->UpdateItem(*pItemElem0,
		UI::Element{ .kind = bItemResist, .abilityOption = nResistAbilityOption }
	);
	
	pUser->UpdateItem(*pItemElem0, UI::AbilityOption::Set(nAbilityOption));
	if (nAbilityOption > 5 && pItemElem0->GetProp()->dwReferStat1 == WEAPON_ULTIMATE) {
		pUser->UpdateItem(*pItemElem0, UI::Piercing::Size::Ultimate);
	}
#else // __WORLDSEVER
#ifdef __CLIENT
	if( g_WndMng.m_pWndUpgradeBase == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
		g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
		g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
		return FALSE;
	}

	BYTE bItemResist = scanner.GetNumber();
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();
	if( bItemResist < 0 || 5 < bItemResist )
	{
		return FALSE;
	}
	if( nResistAbilityOption < 0 || 20 < nResistAbilityOption || nAbilityOption < 0 || 10 < nAbilityOption )
	{
		return FALSE;
	}

	if( bItemResist == 0 )
	{
		nResistAbilityOption = 0;
	}

	if( g_WndMng.m_pWndUpgradeBase )
	{
		if( g_WndMng.m_pWndUpgradeBase->m_slots[0] )
		{
			DWORD dwObjId = g_WndMng.m_pWndUpgradeBase->m_slots[0]->m_dwObjId;
			char szSkillLevel[MAX_PATH];
			sprintf( szSkillLevel, "/ritem %d %d %d %d", dwObjId, bItemResist, nResistAbilityOption, nAbilityOption );
			scanner.SetProg( szSkillLevel );		
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __WORLDSERVER
namespace SqKItemEnchant {
	void ItemEnchant(CScanner & scanner, CUser * user, CItemElem * item);
}
#endif

BOOL TextCmd_ItemEnchant(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CItemElem * pItemElem = pUser->m_Inventory.GetAt(0);
	if (!pItemElem) return TRUE;

	SqKItemEnchant::ItemEnchant(scanner, pUser, pItemElem);
#endif
	return TRUE;
}

BOOL TextCmd_CommercialElem( CScanner& )
{
#ifdef __CLIENT
	SAFE_DELETE( g_WndMng.m_pWndCommerialElem );
	g_WndMng.m_pWndCommerialElem = new CWndCommercialElem;
	g_WndMng.m_pWndCommerialElem->Initialize( &g_WndMng, APP_COMMERCIAL_ELEM );
#endif // __CLIENT
	return FALSE;
}

BOOL TextCmd_Piercing(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	DWORD dwObjId	= scanner.GetNumber();
	int bPierNum = scanner.GetNumber();

	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
		return FALSE;

	if( bPierNum < 0 || bPierNum > MAX_PIERCING )
		return FALSE;

	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return FALSE;
	}

	if( bPierNum < pItemElem0->GetPiercingSize() )
	{
		pUser->UpdateItem(*pItemElem0, UI::Piercing::Size{ UI::Piercing::Kind::Regular, bPierNum });
		return TRUE;
	}

	for( int i=1; i<=bPierNum; i++ )
	{
		if( pItemElem0->IsPierceAble( NULL_ID, TRUE ) )
			pUser->UpdateItem(*pItemElem0, UI::Piercing::Size{ UI::Piercing::Kind::Regular, i });
	}
#else // __WORLDSEVER
#ifdef __CLIENT
	const int bPierNum = scanner.GetNumber();
	if (bPierNum < 0 || bPierNum > MAX_PIERCING) return FALSE;

	const CWndPiercing * const pWndPiercing = (CWndPiercing*)g_WndMng.GetWndBase(APP_PIERCING);
	if (!pWndPiercing) return FALSE;

	if (!pWndPiercing->m_slots[0]) return FALSE;

	const DWORD dwObjId = pWndPiercing->m_slots[0].m_item->m_dwObjId;
	char szSkillLevel[MAX_PATH];
	sprintf(szSkillLevel, "/pier %d %d", dwObjId, bPierNum);
	scanner.SetProg(szSkillLevel);

#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL IsRight( CString &str )
{
	if( str.GetLength() == 1 )
	{
		if( str.Find( "#", 0 ) == 0 )
			return FALSE;
	}
	else
	if( str.GetLength() == 2 )
	{
		if( str.Find( "#u", 0 ) == 0 )
			return FALSE;
		
		if( str.Find( "#b", 0 ) == 0 )
			return FALSE;
	}
	
	int nFind = str.Find( "#c", 0 );	
	if( nFind != -1 )
	{
		if( (str.GetLength() - nFind) < 8 )
			return FALSE;
	}
		
		return TRUE;
}

BOOL TextCmd_shout(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( pUser->IsMode( SHOUTTALK_MODE ) )
		return FALSE;
#ifdef __QUIZ
	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		return FALSE;
#endif // __QUIZ
#ifdef __JEFF_9_20
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return FALSE;
	}
	if( ( // ???? & ????
		( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
		|| ( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_IND )
		|| ::GetLanguage() == LANG_GER
		|| ::GetLanguage() == LANG_FRE
		|| ::GetLanguage() == LANG_VTN
		|| ::GetLanguage() == LANG_POR
		|| ::GetLanguage() == LANG_RUS
		)
		&& pUser->GetLevel() < 20 )
	{
		pUser->AddDefinedText( TID_GAME_CANT_SHOUT_BELOW_20 );
		return FALSE;
	}
#endif	// __JEFF_9_20

	if( ( GetLanguage() == LANG_TWN || GetLanguage() == LANG_HK )  && pUser->GetLevel() < 15 )
	{
		pUser->AddDefinedText( TID_GAME_CANNOT_SHOUT_BELOW_15_LEVEL );
		return FALSE;
	}

	char szString[1024];
	szString[0] = '\0';

	scanner.GetLastFull();
	if( strlen( scanner.token ) > 260 )	//
		return TRUE;

	strcpy( szString, scanner.token );
	StringTrimRight( szString );

	if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
	{
		ParsingEffect( szString, strlen(szString) );
	}
	RemoveCRLF( szString );

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_SHOUT;
	arBlock << GETID( pUser );
	arBlock.WriteString(pUser->GetName());
	arBlock.WriteString( szString );
	DWORD dwColor	= 0xffff99cc;
	if( pUser->HasBuff(  BUFF_ITEM, II_SYS_SYS_LS_SHOUT ) )
		dwColor		= 0xff00ff00;
	arBlock << dwColor;
	GETBLOCK( arBlock, lpBlock, uBlockSize );

	int nRange = 0x000000ff;
	if( pUser->IsShoutFull() )
		nRange = 0;
	g_UserMng.AddShout( pUser, nRange, lpBlock, uBlockSize );

#else	// __WORLDSERVER
#ifdef __CLIENT

	static CTimer timerDobe;
	static CString stringBack;
	static CTimeLimit timeLimit( g_Neuz.m_nShoutLimitCount, g_Neuz.m_dwShoutLimitSecond );

	CString string = scanner.m_pProg; 

	CString strTrim = string;
	strTrim.TrimLeft();
		
	if( IsRight( strTrim ) == FALSE )
		return FALSE;

#ifdef __BAN_CHATTING_SYSTEM
	if( g_WndMng.GetBanningTimer().IsTimeOut() == FALSE )
	{
		CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
		if( pWndChat )
		{
			int nOriginalSecond = static_cast< int >( CWndMgr::BANNING_MILLISECOND - static_cast< int >( g_WndMng.GetBanningTimer().GetLeftTime() ) ) / 1000;
			int nMinute = static_cast< int >( nOriginalSecond / 60 );
			int nSecond = static_cast< int >( nOriginalSecond % 60 );
			CString strMessage = _T( "" );
			// ???? ???? ???? ???????? ???? ????????. (???? ????: %d?? %d??)
			strMessage.Format( prj.GetText( TID_GAME_ERROR_CHATTING_3 ), nMinute, nSecond );
			pWndChat->PutString( strMessage, 0xffff0000 );
			return FALSE;
		}
	}
	else
	{
		if( stringBack != string || timerDobe.TimeOut() )
		{
			if( !g_pPlayer->IsShoutFull()
				|| ( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
				|| ( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_IND )
				|| ::GetLanguage() == LANG_GER
				|| ::GetLanguage() == LANG_FRE
				|| ::GetLanguage() == LANG_JAP
				|| ::GetLanguage() == LANG_VTN
				|| ::GetLanguage() == LANG_POR
				|| ::GetLanguage() == LANG_RUS
				)
			{
				if( g_Neuz.m_nShoutLimitCount > 0 && timeLimit.Check() == TRUE ) // ?????? ??????.
				{
					//???????? %n?????? %d???? ??????????.
					char szMsg[256];
					sprintf( szMsg, prj.GetText( TID_GAME_LIMIT_SHOUT ), g_Neuz.m_dwShoutLimitSecond / 1000, g_Neuz.m_nShoutLimitCount );
					g_WndMng.PutString( szMsg, NULL, prj.GetTextColor( TID_GAME_LIMIT_SHOUT ) );
					return FALSE;
				}
			}

			stringBack = string;
			timerDobe.Set( SEC( 3 ) );
			g_WndMng.WordChange( string );

			if( g_WndMng.IsShortcutCommand() == TRUE )
			{
				if( g_WndMng.GetShortcutWarningTimer().IsTimeOut() == FALSE )
				{
					g_WndMng.SetWarningCounter( g_WndMng.GetWarningCounter() + 1 );
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						if( g_WndMng.GetWarningCounter() >= CWndMgr::BANNING_POINT )
						{
							// ?????? ???????? ?????? %d?? ???? ???? ???? ???????? ????????????.
							CString strChattingError1 = _T( "" );
							strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
							pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
							g_WndMng.SetWarningCounter( 0 );
							g_WndMng.GetBanningTimer().Reset();
						}
						else
						{
							// ???? ???????? ?????? ???????? ???????? ??????????.
							pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
						}
					}
				}
				else
				{
					CString strCommand = _T( "/s " ) + string;
					g_DPlay.SendChat( strCommand );
				}
				g_WndMng.GetShortcutWarningTimer().Reset();
			}
			else
			{
				if( g_WndMng.GetWarningTimer().IsTimeOut() == FALSE )
				{
					g_WndMng.SetWarningCounter( g_WndMng.GetWarningCounter() + 1 );
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						if( g_WndMng.GetWarningCounter() >= CWndMgr::BANNING_POINT )
						{
							// ?????? ???????? ?????? %d?? ???? ???? ???? ???????? ????????????.
							CString strChattingError1 = _T( "" );
							strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
							pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
							g_WndMng.SetWarningCounter( 0 );
							g_WndMng.GetBanningTimer().Reset();
						}
						else
						{
							// ???? ???????? ?????? ???????? ???????? ??????????.
							pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
						}
					}
				}
				else
				{
					if( g_WndMng.GetWarning2Timer().IsTimeOut() == FALSE )
					{
						g_WndMng.SetWarning2Counter( g_WndMng.GetWarning2Counter() + 1 );
						CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
						if( pWndChat )
						{
							if( g_WndMng.GetWarning2Counter() >= CWndMgr::BANNING_2_POINT )
							{
								// ?????? ???????? ?????? %d?? ???? ???? ???? ???????? ????????????.
								CString strChattingError1 = _T( "" );
								strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
								pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
								g_WndMng.SetWarning2Counter( 0 );
								g_WndMng.GetBanningTimer().Reset();
							}
							else
							{
								CString strCommand = _T( "" );
								strCommand.Format( "/s %s", string );
								g_DPlay.SendChat( strCommand );
							}
						}
					}
					else
					{
						CString strCommand = _T( "" );
						strCommand.Format( "/s %s", string );
						g_DPlay.SendChat( strCommand );
					}
				}
				g_WndMng.GetWarningTimer().Reset();
				g_WndMng.GetWarning2Timer().Reset();
			}
		}
		else
		{
			CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
			if( pWndChat )
				g_WndMng.PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), NULL, prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
		}
	}
#else // __BAN_CHATTING_SYSTEM
	if( stringBack != string || timerDobe.TimeOut() )
	{
		if( !g_pPlayer->IsShoutFull()
			|| ( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
			|| ( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_IND )
			|| ::GetLanguage() == LANG_GER
			|| ::GetLanguage() == LANG_FRE
			|| ::GetLanguage() == LANG_JAP
			|| ::GetLanguage() == LANG_VTN
			|| ::GetLanguage() == LANG_POR
			|| ::GetLanguage() == LANG_RUS
		)
			if( g_Neuz.m_nShoutLimitCount > 0 && timeLimit.Check() == TRUE ) // ?????? ??????.
			{
				//???????? %n?????? %d???? ??????????.
				char szMsg[256];
				sprintf( szMsg, prj.GetText( TID_GAME_LIMIT_SHOUT ), g_Neuz.m_dwShoutLimitSecond / 1000, g_Neuz.m_nShoutLimitCount );
				g_WndMng.PutString( szMsg, NULL, prj.GetTextColor( TID_GAME_LIMIT_SHOUT ) );
				return FALSE;
			}

		stringBack = string;
		timerDobe.Set( SEC( 3 ) );
		g_WndMng.WordChange( string );

		CString strCommand;
		strCommand.Format( "/s %s", string );
		g_DPlay.SendChat( strCommand );
	}
	else
	{
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
		if( pWndChat )
			g_WndMng.PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), NULL, prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
	}
#endif // __BAN_CHATTING_SYSTEM
	return FALSE;
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PartyChat(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	static CHAR lpString[260];
	
#ifdef __JEFF_9_20
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#endif	// __JEFF_9_20
	
	lpString[0] = '\0';
	
	scanner.GetLastFull();
	if( lstrlen( scanner.token ) >= 260 )
		return TRUE;
	strcpy( lpString, scanner.token );

	StringTrimRight( lpString );	

	if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
	{
		ParsingEffect(lpString, strlen(lpString) );
	}
	RemoveCRLF( lpString );
	
	CParty* pParty;
	
	pParty	= g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty && pParty->IsMember( pUser->m_idPlayer ))
	{
		// ?????? ?????? ???????????? ????
		g_DPCoreClient.SendPartyChat( pUser, lpString );
	}
	else
	{
		// ???????????? ?????? ????????
		pUser->AddSendErrorParty( ERROR_NOPARTY );
	}

	// ?????????????? ???????????????? ???? ??????
#else // __WORLDSERVER
#ifdef __CLIENT
	if( g_Party.GetSizeofMember() >= 2 )
	{
		CString string = scanner.m_pProg;
		g_WndMng.WordChange( string );

		if( ::GetLanguage() == LANG_THA )
			string = '"'+string+'"';
		
		CString strCommand;
		strCommand.Format( "/p %s", string );
		g_DPlay.SendChat( strCommand );
	}
	else
	{
		// ???????? ???????? 
		// ?????? ???????? ???? ?????????? ???? ????????.
		//g_WndMng.PutString( "?????? ???????? ???? ?????????? ???? ????????", NULL, 0xff99cc00 );
		g_WndMng.PutString( prj.GetText( TID_GAME_PARTYNOTCHAT ), NULL, prj.GetTextColor( TID_GAME_PARTYNOTCHAT ) );
		
	}
	return FALSE;
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_Music(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	u_long idmusic	= scanner.GetNumber();
	g_DPCoreClient.SendPlayMusic( pUser->GetWorld()->GetID(), idmusic );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Sound(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	u_long idsound	= scanner.GetNumber();
	g_DPCoreClient.SendPlaySound( pUser->GetWorld()->GetID(), idsound );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Summon(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	TCHAR lpszPlayer[32];

	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
		if( idPlayer > 0 ){
			strcpy( lpszPlayer, scanner.Token );
#ifdef __LAYER_1015
			g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), idPlayer, pUser->GetLayer() );
#else	// __LAYER_1015
			g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), idPlayer );
#endif	// __LAYER_1015
		}
		else {
//			scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else 
	{
		pUser->AddReturnSay( 2, " " );  		// ???? ???????? ????????.		
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __PET_1024
BOOL TextCmd_ClearPetName( CScanner &)
{
#ifdef __CLIENT
	g_DPlay.SendClearPetName();
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_SetPetName(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CPet* pPet	= pUser->GetPet();
	if( !pPet )
		return TRUE;
	s.GetToken();
	pPet->SetName( s.token );
	g_UserMng.AddSetPetName( pUser, s.token );
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __PET_1024

#ifdef __LAYER_1020
BOOL TextCmd_CreateLayer(CScanner & s) {
#ifdef __WORLDSERVER
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
//		pWorld->m_linkMap.CreateLinkMap( nLayer );
		pWorld->CreateLayer( nLayer );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_DeleteLayer( CScanner & s )
{
#ifdef __WORLDSERVER
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
//		pWorld->m_linkMap.DeleteLinkMap( nLayer );
		pWorld->ReleaseLayer( nLayer );		// do not call ReleaseLayer directly
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Layer(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
		CLinkMap* pLinkMap	= pWorld->m_linkMap.GetLinkMap( nLayer );
		if( pLinkMap )
		{
			FLOAT x	= s.GetFloat();
			FLOAT z	= s.GetFloat();
			if( pWorld->VecInWorld( x, z ) && x > 0 && z > 0 )	
				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), D3DXVECTOR3( x, 0, z ), REPLACE_NORMAL, nLayer );
			else
				pUser->AddText( "OUT OF WORLD" );
		}
		else
		{
			pUser->AddText( "LAYER NO EXISTS" );
		}
	}
	else
	{
		pUser->AddText( "UNDEFINED WORLD" );
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __LAYER_1020

BOOL TextCmd_NextCoupleLevel(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( pUser->m_idPlayer );
	if( pCouple )
	{
		if( pCouple->GetLevel() < CCouple::eMaxLevel )
		{
			int nExperience	= CCoupleProperty::Instance()->GetTotalExperience( pCouple->GetLevel() + 1 ) - pCouple->GetExperience();
			g_dpDBClient.SendQueryAddCoupleExperience( pUser->m_idPlayer, nExperience );
		}
		else
			pUser->AddText( "MAX COUPLE LEVEL" );
	}
	else
		pUser->AddText( "COUPLE NOT FOUND" );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Propose(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	s.GetToken();
	CCoupleHelper::Instance()->OnPropose( pUser, s.token );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Refuse(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CCoupleHelper::Instance()->OnRefuse( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Couple(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CCoupleHelper::Instance()->OnCouple( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Decouple(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CCoupleHelper::Instance()->OnDecouple( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ClearPropose( CScanner & s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendClearPropose();
#endif	// __WORLDSERVER
	return TRUE;
}
/*
BOOL TextCmd_CoupleState( CScanner & s )
{
#ifdef __CLIENT
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple();
	if( pCouple )
	{
		char szText[200]	= { 0,};
		const char* pszPartner	= CPlayerDataCenter::GetInstance()->GetPlayerString( pCouple->GetPartner( g_pPlayer->m_idPlayer ) );
		if( !pszPartner )	pszPartner	= "";
		sprintf( szText, "%s is partner.", pszPartner );
		g_WndMng.PutString( szText );
	}
	else
	{
		g_WndMng.PutString( "null couple." );
	}
#endif	// __CLIENT
	return TRUE;
}
*/

BOOL TextCmd_Teleport(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	TCHAR *lpszPlayer = NULL;
	int x, z;

	// ???????????? ???? ???????? 
	int nTok = scanner.GetToken();
	if( nTok != NUMBER )
	{
		// player
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
		CUser* pUserTarget = static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
		if( IsValidObj( pUserTarget ) )
		{
			CWorld* pWorld = pUserTarget->GetWorld();
			if( pWorld )
			{
				if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
					if( pWorld != pUser->GetWorld() || pUser->GetLayer() != pUserTarget->GetLayer() )
						return TRUE;

				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), pUserTarget->GetPos(), REPLACE_NORMAL, pUserTarget->GetLayer() );
			}
		}
		else 
		{
		#ifdef _DEBUG
			// ?????????? ???????? NPC?? ??????.
			CWorld* pWorld	= pUser->GetWorld();
			CMover* pMover = pWorld->FindMover( scanner.Token );
			if( pMover )
			{
				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), pMover->GetPos(), REPLACE_NORMAL, pMover->GetLayer() );
				return TRUE;
			}
		#endif // _DEBUG
			pUser->AddReturnSay( 3, scanner.m_mszToken );
		}
	}
	// ?????? ?????????? ???? ????.
	DWORD dwWorldId = atoi( scanner.token );
	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId ) )
	{
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() != dwWorldId )
			return TRUE;
	}
	if( g_WorldMng.GetWorldStruct( dwWorldId ) )
	{
		// ?????? ?????????? ?????????? ???? ??
		if( scanner.GetToken() != NUMBER )
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( dwWorldId, scanner.token );
			if( NULL != pRgnElem )
				pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
		}
		// ???????? ?????? ???? 
		else
		{
			x = atoi( scanner.token );
			z = scanner.GetNumber();

			CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
			if( pWorld && pWorld->VecInWorld( (FLOAT)( x ), (FLOAT)( z ) ) && x > 0 && z > 0 )
			{
				int nLayer	= pWorld == pUser->GetWorld()? pUser->GetLayer(): nDefaultLayer;
				pUser->REPLACE( g_uIdofMulti, dwWorldId, D3DXVECTOR3( (FLOAT)x, 0, (FLOAT)z ), REPLACE_NORMAL, nLayer );
			}
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Out(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
//	TCHAR lpszPlayer[MAX_PLAYER];
	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token) )
	{	
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
		if( idPlayer > 0 ) {
			g_DPCoreClient.SendKillPlayer( pUser->m_idPlayer, idPlayer );
		}
		else {
//			scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  		// ???? ???????? ????????.
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RemoveNpc(CScanner & s) {
#ifdef __WORLDSERVER
	OBJID objid	= (OBJID)s.GetNumber();

	CMover* pMover	= prj.GetMover( objid );
	if( IsValidObj( (CObj*)pMover ) )
	{
		if( pMover->IsNPC() )
			pMover->Delete();
	}
#endif	// __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_CreateItem2(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	s.GetToken();
	ItemProp* pItemProp	= prj.GetItemProp( s.Token );
	if( pItemProp && pItemProp->dwItemKind3 != IK3_VIRTUAL )
	{
		int nRandomOptItemId	= s.GetNumber();
		
		if (const auto * const pRandomOptItem = g_RandomOptItemGen.GetRandomOptItem(nRandomOptItemId)) {
			CItemElem itemElem;
			itemElem.m_dwItemId		= pItemProp->dwID;
			itemElem.m_nItemNum	= 1;
			itemElem.m_nHitPoint	= -1;
			itemElem.SetRandomOpt(pRandomOptItem->nId);
			pUser->CreateItem( &itemElem );
		}
	}
	return TRUE;
#else	// __WORLDSERVER
	return TRUE;
#endif	// __WORLDSERVER
}

BOOL TextCmd_CreateItem(CScanner & scanner, CPlayer_ * pUser) {
	scanner.GetToken();

#ifdef __CLIENT
	// ??????????????
	if( scanner.tok == FINISHED )
	{
		if( g_WndMng.GetWndBase( APP_ADMIN_CREATEITEM ) == NULL )
		{
			CWndAdminCreateItem* pWndAdminCreateItem = new CWndAdminCreateItem;
			pWndAdminCreateItem->Initialize();
		}
		return FALSE;
	}
	return TRUE;
#else   // __CLIENT
	DWORD dwNum;
	DWORD dwCharged		= 0;
	ItemProp* pProp	= NULL;

	if( scanner.tokenType == NUMBER )
		pProp	= prj.GetItemProp( _ttoi( scanner.Token ) );
	else
		pProp	= prj.GetItemProp( scanner.Token );

	if( pProp && pProp->dwItemKind3 != IK3_VIRTUAL )
	{
		if( pProp->dwItemKind3 == IK3_EGG && pProp->dwID != II_PET_EGG )	// ???????? ?????????? ?? ???? "??"?? ?????? ???? ????????.
			return TRUE;

		dwNum	= scanner.GetNumber();
		dwNum	= ( dwNum == 0? 1: dwNum );
		dwCharged	= scanner.GetNumber();
		dwCharged	= ( dwCharged == 0 ? 0 : 1 );
		
		CItemElem itemElem;
		itemElem.m_dwItemId		= pProp->dwID;
		itemElem.m_nItemNum		= (short)( dwNum );
		itemElem.m_nHitPoint	= -1;
		itemElem.m_bCharged		= dwCharged;

		pUser->CreateItem( &itemElem );
	}
#endif	// !__CLIENT 
	return TRUE;
}

BOOL TextCmd_LocalEvent( CScanner & s )
{
#ifdef __WORLDSERVER
	int id	= s.GetNumber();
	if( id != EVE_18 )	// ?? ???????? 18?? ?????? ???????? ?????????? ???????? ?????? ???? ???? ????
	{
		BYTE nState		= (BYTE)s.GetNumber();
		if( g_eLocal.SetState( id, nState ) )		g_UserMng.AddSetLocalEvent( id, nState );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

// ???????? ???????? ???? ???? 
BOOL TextCmd_CreateChar(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	D3DXVECTOR3 vPos = pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	scanner.GetToken();
	CString strName = scanner.Token;

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

		BOOL bActiveAttack = scanner.GetNumber();
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pMover ) return FALSE; // ASSERT( pObj );
			strcpy( pMover->m_szCharacterKey, strName );
			pMover->InitNPCProperty();
			pMover->InitCharacter( pMover->GetCharacter() );
			pMover->SetPos( vPos );
			pMover->InitMotion( MTI_STAND );
			pMover->UpdateLocalMatrix();
			if( bActiveAttack )
				pMover->m_bActiveAttack = bActiveAttack;
			pWorld->ADDOBJ( pMover, TRUE, pUser->GetLayer() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CreateCtrl(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	DWORD dwID	= s.GetNumber();
	
	if( dwID == 0 )
		return FALSE;

	CCtrl* pCtrl	= (CCtrl*)CreateObj( D3DDEVICE, OT_CTRL, dwID );
	if( !pCtrl )
		return FALSE;

	pCtrl->SetPos( vPos );
	pWorld->ADDOBJ( pCtrl, TRUE, pUser->GetLayer() );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PostMail( CScanner & s )
{
#ifdef __CLIENT
	DWORD dwIndex	= s.GetNumber();
	short nItemNum	= s.GetNumber();
	char lpszReceiver[MAX_PLAYER]	= { 0, };
	s.GetToken();
	lstrcpy( lpszReceiver, s.Token );
	int nGold	= s.GetNumber();
	char lpszTitle[MAX_MAILTITLE]	= { 0, };
	s.GetToken();
	lstrcpy( lpszTitle, s.Token );
	char lpszText[MAX_MAILTEXT]	= { 0, };
	s.GetToken();
	lstrcpy( lpszText, s.Token );

	CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAt( dwIndex );
	if( pItemElem )
	{
		g_DPlay.SendQueryPostMail( (BYTE)( pItemElem->m_dwObjId ), nItemNum, lpszReceiver, nGold, lpszTitle, lpszText );
	}
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_RemoveMail( CScanner & s )
{
#ifdef __CLIENT
	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryRemoveMail( nMail );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_GetMailItem( CScanner & s )
{
#ifdef __CLIENT
	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryGetMailItem( nMail );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_GetMailGold( CScanner & s )
{
#ifdef __CLIENT
	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryGetMailGold( nMail );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_Lua(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	s.GetToken();
	s.Token.MakeLower();
		
	if( s.Token == "event" )
	{
		pUser->AddText( "Event.lua Reload..." );
		Error( "Event.lua Reload... - %s", pUser->GetName() );
		g_dpDBClient.SendEventLuaChanged();
	}
	else if( s.Token == "ms" )
	{
		CMonsterSkill::GetInstance()->Clear();
		CMonsterSkill::GetInstance()->LoadScript();
	}
	else if( s.Token == "rr" )
	{
		CRainbowRaceMng::GetInstance()->LoadScript();
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LuaEventList(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	prj.m_EventLua.GetAllEventList( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LuaEventInfo(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	prj.m_EventLua.GetEventInfo( pUser, s.GetNumber() );
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __JEFF_9_20
BOOL TextCmd_Mute(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
	
	if( idPlayer == 0 )
	{
		pUser->AddText( "player not found" );
		return TRUE;
	}
	CUser* pTarget	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( IsValidObj( pTarget ) )
	{
		DWORD dwMute	= (DWORD)s.GetNumber();
		if( s.tok == FINISHED )
			return TRUE;
		pTarget->m_dwMute	= dwMute;
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __JEFF_9_20

BOOL TextCmd_AngelExp(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int nAddExp	= s.GetNumber();

	if( pUser->HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		int nAngel = 100;
	#ifdef __BUFF_1107
		IBuff* pBuff	= pUser->m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		WORD wId	= ( pBuff? pBuff->GetId(): 0 );
	#else	// __BUFF_1107
		LPSKILLINFLUENCE lpSkillIn = pUser->m_SkillState.GetItemBuf( IK3_ANGEL_BUFF );
		WORD wId	= ( lpSkillIn? lpSkillIn->wID: 0 );
	#endif	// __BUFF_1107
		if( wId )
		{
			ItemProp* pItemProp = prj.GetItemProp( wId );
			if( pItemProp )
				nAngel = (int)( (float)pItemProp->nAdjParamVal1 );
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;

		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[pUser->m_nAngelLevel].nExp1 / 100 * nAngel;
		if( pUser->m_nAngelExp < nMaxAngelExp )
		{
			pUser->m_nAngelExp += nAddExp;
			BOOL bAngelComplete = FALSE;
			if( pUser->m_nAngelExp > nMaxAngelExp )
			{
				pUser->m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}
			pUser->AddAngelInfo( bAngelComplete );
		}
	}
#endif // __WORLDSERVER
	return TRUE;	
}

#ifdef __EVENT_1101
BOOL TextCmd_CallTheRoll(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int nCount	= s.GetNumber();
#ifdef __EVENT_1101_2
	pUser->m_nEventFlag = 0;
#else // __EVENT_1101_2
	pUser->m_dwEventFlag	= 0;
#endif // __EVENT_1101_2
	pUser->m_dwEventTime	= 0;
	pUser->m_dwEventElapsed	= 0;
	for(  int i = 0; i < nCount; i++ )
#ifdef __EVENT_1101_2
		pUser->SetEventFlagBit( 62 - i );
#else // __EVENT_1101_2
		pUser->SetEventFlagBit( 31 - i );
#endif // __EVENT_1101_2
#endif	// __WORLDSERVER
	return TRUE;
}

#endif	// __EVENT_1101

BOOL TextCmd_CreatePc(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __PERF_0226
#ifdef __WORLDSERVER
	int nNum = scanner.GetNumber();
	for( int i=0; i<nNum; i++ )
	{
		int nSex	= xRandom( 2 );
		DWORD dwIndex	= ( nSex == SEX_FEMALE? MI_FEMALE: MI_MALE );

		CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, dwIndex );
		if( NULL == pMover )	
			return FALSE;
		pMover->SetPos( pUser->GetPos() );
		pMover->InitMotion( MTI_STAND );
		pMover->UpdateLocalMatrix();
		SAFE_DELETE( pMover->m_pAIInterface );
		pMover->SetAIInterface( AII_MONSTER );
		pMover->m_Inventory.SetItemContainer( CItemContainer::ContainerType::INVENTORY ); 

		static DWORD adwParts[5]	= {	PARTS_CAP, PARTS_HAND, PARTS_UPPER_BODY, PARTS_FOOT, PARTS_RWEAPON };
		for( int i = 0; i < 5; i++ )
		{
			CItemElem itemElem;
			ItemProp* pProp	= CPartsItem::GetInstance()->GetItemProp( ( i == 4? SEX_SEXLESS: nSex ), adwParts[i] );
			if( pProp )
			{
				CItemElem	itemElem;
				itemElem.m_dwItemId	= pProp->dwID;
				itemElem.m_nItemNum	= 1;
				itemElem.SetAbilityOption( xRandom( 10 ) );
				BYTE nId, nCount;
				short nNum;
				pMover->m_Inventory.Add( &itemElem, &nId, &nNum, &nCount );
				CItemElem* pAddItem	= pMover->m_Inventory.GetAtId( nId );
				pMover->m_Inventory.DoEquip( pAddItem->m_dwObjIndex, pProp->dwParts );
			}
		}
		pUser->GetWorld()->ADDOBJ( pMover, TRUE, pUser->GetLayer() );
	}
#endif	// __WORLDSERVER
#endif	// __PERF_0226
	return TRUE;
}

BOOL TextCmd_CreateNPC(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	CString strName = scanner.Token;

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		if( pMoverProp->dwAI != AII_MONSTER
			&& pMoverProp->dwAI != AII_CLOCKWORKS
			&& pMoverProp->dwAI != AII_BIGMUSCLE
			&& pMoverProp->dwAI != AII_KRRR
			&& pMoverProp->dwAI != AII_BEAR
			&& pMoverProp->dwAI != AII_METEONYKER
		)
			return TRUE;

		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

		BOOL bActiveAttack = scanner.GetNumber();
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	
				return FALSE;	
			pObj->SetPos( vPos );
			pObj->InitMotion( MTI_STAND );
			pObj->UpdateLocalMatrix();

			if( bActiveAttack )
				((CMover*)pObj)->m_bActiveAttack = bActiveAttack;
			
			((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // ?????? ?????? ???? ?????? ????
			pWorld->ADDOBJ( pObj, TRUE, pUser->GetLayer() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Invisible(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->m_dwMode |= TRANSPARENT_MODE;
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoInvisible(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->m_dwMode &= (~TRANSPARENT_MODE);
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoUndying(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	g_UserMng.AddModifyMode( pUser );
#endif
	return TRUE;
}

// exp???? ???? ????. ???????????? ????.
BOOL TextCmd_ExpUpStop(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( pUser->m_dwMode & MODE_EXPUP_STOP )
		pUser->m_dwMode &= (~MODE_EXPUP_STOP);
	else
		pUser->m_dwMode |= MODE_EXPUP_STOP;
	
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	
	return TRUE;
}
BOOL TextCmd_PartyInvite(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
	u_long uidPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
	if( 0 < uidPlayer )
	{
		CUser* pUser2	= g_UserMng.GetUserByPlayerID( uidPlayer );	
		if( IsValidObj( pUser2 ) )
			g_DPSrvr.InviteParty( pUser->m_idPlayer, pUser2->m_idPlayer );
		else
			pUser->AddDefinedText( TID_DIAG_0060, "\"%s\"", scanner.Token );
	}
	else
	{
		pUser->AddDefinedText( TID_DIAG_0061, "\"%s\"", scanner.Token );
	}
#endif // __WORLDSERVER

	return TRUE;
}
BOOL TextCmd_GuildInvite(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
	u_long uidPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
	if( 0 < uidPlayer )
	{
		CUser* pUser2	= g_UserMng.GetUserByPlayerID( uidPlayer );	
		if( IsValidObj( pUser2 ) )
		{
			g_DPSrvr.InviteCompany( pUser, pUser2->GetId() );
		}
		else
			pUser->AddDefinedText( TID_DIAG_0060, "\"%s\"", scanner.Token );
	}
	else
	{
		pUser->AddDefinedText( TID_DIAG_0061, "\"%s\"", scanner.Token );
	}
#endif // __WORLDSERVER

	return TRUE;
}

BOOL bCTDFlag	= FALSE;

BOOL TextCmd_CTD(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( g_eLocal.GetState( EVE_WORMON ) == 0 )
	{
		CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
		const CRect * pRect	= pProcessor->GetQuestRect( QUEST_WARMON_LV1 );
		if( pRect )
		{
			OutputDebugString( "recv /ctd" );
			REGIONELEM re;
			memset( &re, 0, sizeof(REGIONELEM) );
			re.m_uItemId	= 0xffffffff;
			re.m_uiItemCount	= 0xffffffff;
			re.m_uiMinLevel	= 0xffffffff;
			re.m_uiMaxLevel	= 0xffffffff;
			re.m_iQuest	= 0xffffffff;
			re.m_iQuestFlag	= 0xffffffff;
			re.m_iJob	= 0xffffffff;
			re.m_iGender	= 0xffffffff;
			re.m_dwAttribute	= RA_DANGER | RA_FIGHT;
			re.m_dwIdMusic	= 121;
			re.m_bDirectMusic	= TRUE;
			re.m_dwIdTeleWorld	= 0;
			re.m_rect = *pRect;
			lstrcpy( re.m_szTitle, "Duel Zone" );

			CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
			if( pWorld )
			{
				LPREGIONELEM ptr	= pWorld->m_aRegion.GetAt( pWorld->m_aRegion.GetSize() - 1 );
				if( ptr->m_dwAttribute != ( RA_DANGER | RA_FIGHT ) )
					pWorld->m_aRegion.AddTail( &re );
				pUser->AddText( "recv /ctd" );
				g_UserMng.AddAddRegion( WI_WORLD_MADRIGAL, re );
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Undying(CScanner & scanner, CPlayer_ * pUser) {
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	pUser->m_dwMode |= MATCHLESS_MODE;
#ifdef __WORLDSERVER
	g_UserMng.AddModifyMode( pUser );
#endif
	return TRUE;
}

BOOL TextCmd_Undying2(CScanner &, CPlayer_ * pUser) {
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode |= MATCHLESS2_MODE;
#ifdef __WORLDSERVER
	g_UserMng.AddModifyMode( pUser );
#endif
	return TRUE;
}


BOOL TextCmd_NoDisguise(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->NoDisguise( NULL );
	g_UserMng.AddNoDisguise( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __WORLDSERVER
BOOL DoDisguise( CUser* pUser, DWORD dwIndex )
{
	pUser->Disguise( NULL, dwIndex );
	g_UserMng.AddDisguise( pUser, dwIndex );
	return TRUE;
}
#endif // __WORLDSERVER


BOOL TextCmd_Disguise(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp )
		DoDisguise( pUser, pMoverProp->dwID );
#endif
	return TRUE;
}
BOOL TextCmd_Freeze(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
		if( idFrom > 0 && idTo > 0 ) 
		{
			// 1 : ???? m_dwMode
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)1, idFrom, idTo );					
		}
		else 
		{
//			scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// ???? ???????? ????????.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoFreeze(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
		if( idFrom > 0 && idTo > 0 ) 
		{
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)0, idFrom, idTo );	// 0 : ?? m_dwMode
		}
		else 
		{
			//scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// ???? ???????? ????????.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_Talk(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();

	u_long idFrom, idTo;
	idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
	idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
	if( idFrom > 0 && idTo > 0 ) 
	{
		g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)0, idFrom, idTo );	// 0 : ?? m_dwMode
	}
	else 
	{
		//scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
		pUser->AddReturnSay( 3, scanner.Token );
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoTalk(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	
	scanner.GetToken();

	{
		u_long idFrom, idTo;
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
		if( idFrom > 0 && idTo > 0 ) 
		{
			g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)1, idFrom, idTo );	// 1 : ????
		}
		else 
		{
			//scanner.Token???? ?????? ???? ???????? ?? ?????? ???????? ??????.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_GetGold(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int nGold = scanner.GetNumber();	
	pUser->AddGold( nGold );
#endif	// __WORLDSERVER	
	return TRUE;
}

// /???? npcId "????"
BOOL TextCmd_indirect( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	DWORD dwIdNPC = scanner.GetNumber();
	TCHAR szString[ 1024 ];

	scanner.GetLastFull();
	if( strlen( scanner.token ) > 260 )
		return TRUE;
	strcpy( szString, scanner.token );
	StringTrimRight( szString );

	if( szString[ 0 ] )
	{
		CMover* pMover = prj.GetMover( dwIdNPC );
		if( pMover )
			g_UserMng.AddChat( (CCtrl*)pMover, (LPCSTR)szString );
	}
#else // __WORLDSERVER
	scanner.GetToken();
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		if( scanner.tok == FINISHED )
		{
			if( g_WndMng.GetWndBase( APP_ADMIN_INDIRECT_TALK ) == NULL )
			{
				CWndIndirectTalk* pWndIndirectTalk = new CWndIndirectTalk;
				pWndIndirectTalk->Initialize();
			}
			return FALSE;
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}
/*
BOOL TextCmd_sbreport( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Report();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbready( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Ready();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbstart( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Start();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbstart2( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Start2();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbend( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->End();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_OpenBattleServer( CScanner & s )
{
#ifdef __WORLDSERVER
	BEFORESENDDUAL( ar, PACKETTYPE_OPEN_BATTLESERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CloseBattleServer( CScanner & s )
{
#ifdef __WORLDSERVER
	BEFORESENDDUAL( ar, PACKETTYPE_CLOSE_BATTLESERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
#endif	// __WORLDSERVER
	return TRUE;
}
*/

BOOL TextCmd_ItemMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetMode( ITEM_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemNotMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetNotMode( ITEM_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_AttackMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetMode( NO_ATTACK_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_EscapeReset(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CMover* pMover = prj.GetUserByID( scanner.GetNumber() );
	if( IsValidObj( pMover ) ) {
		pMover->SetSMMode( SM_ESCAPE, 0 );
	}
	else {
		pUser->SetSMMode( SM_ESCAPE, 0 );
	}
#else	// __WORLDSERVER
#ifdef __CLIENT
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/EscapeReset %d", pMover->m_idPlayer  );
		}

		g_DPlay.SendChat( (LPCSTR)strSend );

		return FALSE;
	}
#endif //__CLIENT
#endif	// __WORLDSERVER
	return TRUE;
}



BOOL TextCmd_AttackNotMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetNotMode( NO_ATTACK_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetMode( COMMUNITY_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityNotMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetNotMode( COMMUNITY_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetMode( OBSERVE_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveNotMode(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->SetNotMode( OBSERVE_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Onekill(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->m_dwMode |= ONEKILL_MODE;
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode |= ONEKILL_MODE;
#endif
#endif
	return TRUE;
}
BOOL TextCmd_Position( CScanner& scanner )          
{ 
#ifdef __CLIENT
	CString string;
	D3DXVECTOR3 vPos = g_pPlayer->GetPos();
	//string.Format( "???????? : x = %f, y = %f, z = %f", vPos.x, vPos.y, vPos.z );
	string.Format( prj.GetText(TID_GAME_NOWPOSITION), vPos.x, vPos.y, vPos.z );
	g_WndMng.PutString( string, NULL, prj.GetTextColor( TID_GAME_NOWPOSITION ) );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_NoOnekill(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->m_dwMode &= (~ONEKILL_MODE);
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~ONEKILL_MODE);
#endif
#endif
	return TRUE;
}
BOOL TextCmd_ip(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	scanner.GetToken();

	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
	if( idPlayer > 0 )
		g_DPCoreClient.SendGetPlayerAddr( pUser->m_idPlayer, idPlayer );
	else 
		pUser->AddReturnSay( 3, scanner.Token );
#else	// __WORLDSERVER
	#ifdef __CLIENT
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/ip %s", pMover->GetName() );
		}
		else
		{
			strSend.Format( "%s", scanner.m_pBuf );
		}
		g_DPlay.SendChat( (LPCSTR)strSend );
		return FALSE;
	}
	#endif //__CLIENT
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_userlist(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	g_DPCoreClient.SendGetCorePlayer( pUser->m_idPlayer );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_count(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	g_DPCoreClient.SendGetPlayerCount( pUser->m_idPlayer );

	char szCount[128]	= { 0, };
	sprintf( szCount, "Players online: %d", g_UserMng.GetCount() );
	pUser->AddText( szCount );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_System( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CHAR szString[512] = "";

	scanner.GetLastFull();
	if( strlen( scanner.token ) >= 512 )
		return TRUE;
	strcpy( szString, scanner.token );
	StringTrimRight( szString );

	g_DPCoreClient.SendSystem( szString );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LoadScript( CScanner & scanner )
{
#ifdef __WORLDSERVER
	#if defined(__REMOVE_SCIRPT_060712)
		if( CWorldDialog::GetInstance().Reload() == FALSE )
			Error("WorldScript.dll reload error");
	#else
		CUser* pUser	 = (CUser*)scanner.dwValue;
		pUser->GetWorld()->LoadAllMoverDialog();
	#endif
#endif
	return TRUE;
}

BOOL TextCmd_FallSnow( CScanner & scanner )
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendFallSnow();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_FallRain( CScanner & scanner )
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendFallRain();
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_StopSnow( CScanner & scanner )
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendStopSnow();
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_StopRain( CScanner & scanner )
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendStopRain();
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __CLIENT

BOOL TextCmd_tradeagree( CScanner & scanner )
{
	g_Option.m_bTrade = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEAGREE ), NULL, prj.GetTextColor( TID_GAME_TRADEAGREE ) );
	return TRUE;
}
BOOL TextCmd_traderefuse( CScanner & scanner )
{
	g_Option.m_bTrade = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEREFUSE ), NULL, prj.GetTextColor( TID_GAME_TRADEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_whisperagree( CScanner & scanner )
{
	g_Option.m_bSay = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERAGREE ), NULL, prj.GetTextColor( TID_GAME_WHISPERAGREE ) );
	return TRUE;
}
BOOL TextCmd_whisperrefuse( CScanner & scanner )
{
	g_Option.m_bSay = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERREFUSE ), NULL, prj.GetTextColor( TID_GAME_WHISPERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_messengeragree( CScanner & scanner )
{
	g_Option.m_bMessenger = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERAGREE ), NULL, prj.GetTextColor( TID_GAME_MSGERAGREE ) );
	return TRUE;
}
BOOL TextCmd_messengerrefuse( CScanner & scanner )
{
	g_Option.m_bMessenger = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERREFUSE ), NULL, prj.GetTextColor( TID_GAME_MSGERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_stageagree( CScanner & scanner )
{
	g_Option.m_bParty = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEAGREE ), NULL, prj.GetTextColor( TID_GAME_STAGEAGREE ) );
	return TRUE;
}
BOOL TextCmd_stagerefuse( CScanner & scanner )
{
	g_Option.m_bParty = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEREFUSE ), NULL, prj.GetTextColor( TID_GAME_STAGEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_connectagree( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNAGREE ), NULL, prj.GetTextColor( TID_GAME_CONNAGREE ) );
	return TRUE;
}
BOOL TextCmd_connectrefuse( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNREFUSE ), NULL, prj.GetTextColor( TID_GAME_CONNREFUSE ) );
	return TRUE;
}
BOOL TextCmd_shoutagree( CScanner & scanner )
{
	g_Option.m_bShout = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTAGREE ), NULL, prj.GetTextColor( TID_GAME_SHOUTAGREE ) );
	return TRUE;
}
BOOL TextCmd_shoutrefuse( CScanner & scanner )
{
	g_Option.m_bShout = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTREFUSE ), NULL, prj.GetTextColor( TID_GAME_SHOUTREFUSE ) );
	return TRUE;
}

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
BOOL TextCmd_BlockUser( CScanner & scanner )
{
	if( prj.m_setBlockedUserID.size() >= CProject::BLOCKING_NUMBER_MAX )
	{
		// ???? ?????? ???? ???? ??????????????. ???? ?????? ?????? ???? ???? ?????? ????????.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_FULL_BLOCKED_USER_LIST ), NULL, prj.GetTextColor( TID_GAME_ERROR_FULL_BLOCKED_USER_LIST ) );
		return FALSE;
	}
	scanner.GetToken();
	CString strUserName = scanner.token;
	if( strUserName == _T( "" ) )
	{
		// ???????? ???? ????????????. ???????? ?????? ????????.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_INVALID_USER_ID ), NULL, prj.GetTextColor( TID_GAME_ERROR_INVALID_USER_ID ) );
		return FALSE;
	}
	if( g_pPlayer && g_pPlayer->GetName( TRUE ) == strUserName )
	{
		// ???? ???????? ?????? ?? ????????. ???????? ?????? ????????.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_MY_CHARACTER_CANT_BLOCKING ), NULL, prj.GetTextColor( TID_GAME_ERROR_MY_CHARACTER_CANT_BLOCKING ) );
		return FALSE;
	}
	const auto BlockedUserIterator = prj.m_setBlockedUserID.find( strUserName );
	if( BlockedUserIterator != prj.m_setBlockedUserID.end() )
	{
		// ???? ???? ???????? ???? ??????????.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_ALREADY_BLOCKED ), NULL, prj.GetTextColor( TID_GAME_ERROR_ALREADY_BLOCKED ) );
	}
	else
	{
		prj.m_setBlockedUserID.insert( strUserName );

		if( g_WndMng.m_pWndChattingBlockingList )
		{
			g_WndMng.m_pWndChattingBlockingList->UpdateInformation();
		}

		CString strMessage = _T( "" );
		// %s ???? ?????? ??????????????.
		strMessage.Format( prj.GetText( TID_GAME_USER_CHATTING_BLOCKING ), strUserName );
		g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_USER_CHATTING_BLOCKING ) );
	}
	return TRUE;
}

BOOL TextCmd_CancelBlockedUser( CScanner & scanner )
{
	scanner.GetToken();
	CString strUserName = scanner.token;
	const auto BlockedUserIterator = prj.m_setBlockedUserID.find( strUserName );
	if( BlockedUserIterator != prj.m_setBlockedUserID.end() )
	{
		prj.m_setBlockedUserID.erase( strUserName );

		if( g_WndMng.m_pWndChattingBlockingList )
		{
			g_WndMng.m_pWndChattingBlockingList->UpdateInformation();
		}

		CString strMessage = _T( "" );
		// %s ???? ???? ?????? ??????????????.
		strMessage.Format( prj.GetText( TID_GAME_USER_CHATTING_UNBLOCKING ), strUserName );
		g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_USER_CHATTING_UNBLOCKING ) );
	}
	else
	{
		// ???? ???? ?????? ???? ??????????.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_THERE_IS_NO_BLOCKED_TARGET ), NULL, prj.GetTextColor( TID_GAME_ERROR_THERE_IS_NO_BLOCKED_TARGET ) );
	}
	return TRUE;
}

BOOL TextCmd_IgnoreList( CScanner & scanner )
{
	if( g_WndMng.m_pWndChattingBlockingList )
	{
		SAFE_DELETE( g_WndMng.m_pWndChattingBlockingList );
	}

	g_WndMng.m_pWndChattingBlockingList = new CWndChattingBlockingList;
	if( g_WndMng.m_pWndChattingBlockingList )
	{
		g_WndMng.m_pWndChattingBlockingList->Initialize();
	}

	return TRUE;
}
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#endif // __CLIENT

BOOL TextCmd_QuestState(CScanner & s, CPlayer_ * pAdmin) {
#ifdef __WORLDSERVER
	CUser* pUser	= NULL;
	const QuestId nQuest	= QuestId(s.GetNumber());
	const int nState = s.GetNumber();
	s.GetToken();
	if( s.tok != FINISHED )
	{
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
		if( idPlayer )
			pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( pUser == NULL )
		{
			pAdmin->AddDefinedText( TID_DIAG_0061, "%s", s.Token );
			return TRUE;
		}
	}
	else
	{
		pUser	= pAdmin;
	}
	if( nState >= QS_BEGIN && nState < QS_END )
	{
		QUEST quest;
		if( pUser->SetQuest( nQuest, nState, &quest ) )
		{
			pUser->AddSetQuest( &quest );

			char pszComment[100]	= { 0, };
			sprintf( pszComment, "%s %d", pAdmin->GetName(), nState );
			g_dpDBClient.CalluspLoggingQuest(  pUser->m_idPlayer, nQuest, 11, pszComment);
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_BeginQuest(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	const QuestId nQuest = QuestId(s.GetNumber());
	QUEST quest;
	if( pUser->SetQuest( nQuest, 0, &quest ) )
		pUser->AddSetQuest( &quest );
#endif
	return TRUE;
}
BOOL TextCmd_EndQuest(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	const QuestId nQuest = QuestId(s.GetNumber());
	QUEST quest;
	if( pUser->SetQuest( nQuest, QS_END, &quest ) )
		pUser->AddSetQuest( &quest );
#endif
	return TRUE;
}
// ?????? ??, ?????? ???? ?? ?????? ???? 
BOOL TextCmd_RemoveQuest(CScanner & s, CPlayer_ * pAdmin) {
#ifdef __WORLDSERVER
	CUser* pUser	= NULL;
	const QuestId nQuest	= QuestId(s.GetNumber());
	s.GetToken();
	if( s.tok != FINISHED )
	{
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
		if( idPlayer )
			pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( pUser == NULL )
		{
			pAdmin->AddDefinedText( TID_DIAG_0061, "%s", s.Token );
			return TRUE;
		}
	}
	else
	{
		pUser	= pAdmin;
	}

	const auto questState = pUser->GetQuestState(nQuest);
	char pszComment[100]	= { 0, };
	sprintf(pszComment, "%s %d", pAdmin->GetName(), questState.value_or(-1));
	g_dpDBClient.CalluspLoggingQuest(pUser->m_idPlayer, nQuest, 40, pszComment);

	pUser->RemoveQuest( nQuest );
	pUser->AddRemoveQuest( nQuest );
#endif	// __WORLDSERVER
	return TRUE;
}
// ????, ???? ???? ???? 
BOOL TextCmd_RemoveAllQuest(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int nQuest = s.GetNumber();
	pUser->RemoveAllQuest();
	pUser->AddRemoveAllQuest();
#endif
	return TRUE;
}
// ?????? ???? 
BOOL TextCmd_RemoveCompleteQuest(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int nQuest = s.GetNumber();
	pUser->RemoveCompleteQuest();
	pUser->AddRemoveCompleteQuest();
#endif
	return TRUE;
}

BOOL TextCmd_PvpParam(CScanner & scanner, CPlayer_ * pUser) {
#if defined(__WORLDSERVER)
	int	nFame       = scanner.GetNumber();
	int nSlaughter  = scanner.GetNumber();

	pUser->m_nFame  = nFame;
	g_UserMng.AddSetFame( pUser, nFame );

#endif
	return TRUE;
}
BOOL TextCmd_PKParam(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int		nPKValue		= scanner.GetNumber();
	int		nPKPropensity	= scanner.GetNumber();

	if( nPKValue >= 0 )
	{
		pUser->SetPKValue( nPKValue );
		pUser->AddPKValue();
		pUser->CheckHonorStat();
		pUser->AddHonorListAck();
		g_UserMng.AddHonorTitleChange( pUser, pUser->m_nHonor);
	}

	if( nPKPropensity >= 0 )
	{
		pUser->SetPKPropensity( nPKPropensity );
		g_UserMng.AddPKPropensity( pUser );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_TransyItemList( CScanner& scanner, CPlayer_ * pUser )
{
#ifdef __CLIENT
	CString szMsg;
	scanner.GetToken();
	if( scanner.tokenType == STRING )
	{
		szMsg = "Wait : Write " + scanner.Token;
		g_WndMng.PutString( szMsg );

		std::string toOutput = ItemMorph::BuildListOfExistingMorphs();

		FILEOUT(scanner.Token.GetString(), "%s", toOutput.c_str());

		szMsg = "Finish : Finish " + scanner.Token;
		g_WndMng.PutString( szMsg );
	}
	else
	{
		g_WndMng.PutString( "Error : Ex) /TransyItemList Transy.txt&" );
	}
#endif // __CLIENT
	return TRUE;
}

#ifdef _DEBUG
BOOL TextCmd_LoadToolTipColor( CScanner& scanner )
{
#ifdef __CLIENT
	return g_Option.LoadToolTip( "ToolTip.ini" );
#else
	return TRUE;
#endif // __CLIENT
}

#endif //_DEBUG


BOOL TextCmd_ReloadConstant( CScanner& scanner )
{
#ifdef __THA_0808
	if( ::GetLanguage() == LANG_THA )
		return TRUE;
#endif	// __THA_0808
#ifdef __WORLDSERVER
	g_DPCoreClient.SendLoadConstant();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatRequest(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	DWORD dwPenya = scanner.GetNumber();	
	g_GuildCombatMng.GuildCombatRequest( pUser, dwPenya );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatCancel(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	g_GuildCombatMng.GuildCombatCancel( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatOpen(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, "Not GuildCombat Open :: Not CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}

	g_GuildCombatMng.GuildCombatOpen();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatIn(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	g_GuildCombatMng.GuildCombatEnter( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatClose(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int	nClose       = scanner.GetNumber();

	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, "Not GuildCombat Close :: Is CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}

	if( nClose == 0 )
		g_GuildCombatMng.SetGuildCombatClose( TRUE );
	else
		g_GuildCombatMng.SetGuildCombatCloseWait( TRUE );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatNext(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, "Not GuildCombat Close :: Is CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}
	
	g_GuildCombatMng.m_dwTime = GetTickCount();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RemoveAttribute( CScanner& scanner )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndUpgradeBase == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
		g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
		g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
		return FALSE;
	}
	
	if( g_WndMng.m_pWndUpgradeBase )
	{
		if( g_WndMng.m_pWndUpgradeBase->m_slots[0] )
		{
			DWORD dwObjId = g_WndMng.m_pWndUpgradeBase->m_slots[0]->m_dwObjId;
			g_DPlay.SendRemoveAttribute( dwObjId );
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
	return TRUE;	
}

BOOL	TextCmd_GC1to1Open(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( g_GuildCombat1to1Mng.m_nState != g_GuildCombat1to1Mng.GC1TO1_OPEN )
	{
		BOOL bSrvrDown = FALSE;
		if( scanner.GetNumber() == 1 )
			bSrvrDown = TRUE;
		g_GuildCombat1to1Mng.GuildCombat1to1Open( bSrvrDown );
		return TRUE;
	}

	pUser->AddText( "already OPEN State!!!" );
#endif //__WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_GC1to1Close(CScanner &) {
#ifdef __WORLDSERVER
	g_GuildCombat1to1Mng.m_nState = g_GuildCombat1to1Mng.GC1TO1_WAR;
	for( DWORD i=0; i<g_GuildCombat1to1Mng.m_vecGuilCombat1to1.size(); i++ )
	{
		if( g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].m_nState != CGuildCombat1to1::GC1TO1WAR_CLOSEWAIT )
			g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].GuildCombat1to1CloseWait();
		g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].m_nWaitTime = -1;
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_GC1to1Next(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( g_GuildCombat1to1Mng.m_nState == g_GuildCombat1to1Mng.GC1TO1_CLOSE ) // GC1TO1_CLOSE
	{
		pUser->AddText( "Is CLOSE State!!!" );
		return TRUE;
	}
	if( g_GuildCombat1to1Mng.m_nState == g_GuildCombat1to1Mng.GC1TO1_WAR )
	{
		for( DWORD i=0; i<g_GuildCombat1to1Mng.m_vecGuilCombat1to1.size(); i++ )
			g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].m_nWaitTime = -1;
		return TRUE;
	}

	g_GuildCombat1to1Mng.m_nWaitTime = -1;
#endif // __WORLDERVER
	return TRUE;
}

#ifdef __EVENTLUA_COUPON
BOOL TextCmd_Coupon(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->m_nCoupon = s.GetNumber();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __EVENTLUA_COUPON

#ifdef __NPC_BUFF
BOOL TextCmd_RemoveAllBuff(CScanner &, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	pUser->RemoveAllBuff();
	pUser->ClearAllSMMode();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __NPC_BUFF

BOOL TextCmd_HeavenTower( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndHeavenTower)
		SAFE_DELETE(g_WndMng.m_pWndHeavenTower);

	g_WndMng.m_pWndHeavenTower = new CWndHeavenTower;

	if(g_WndMng.m_pWndHeavenTower)
		g_WndMng.m_pWndHeavenTower->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RemoveJewel( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRemoveJewel)
		SAFE_DELETE(g_WndMng.m_pWndRemoveJewel);

	g_WndMng.m_pWndRemoveJewel = new CWndRemoveJewel;

	if(g_WndMng.m_pWndRemoveJewel)
		g_WndMng.m_pWndRemoveJewel->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_TransEggs( CScanner& s )
{
#ifdef __CLIENT
	g_WndMng.CreateApplet( APP_INVENTORY );

	if(g_WndMng.m_pWndPetTransEggs)
		SAFE_DELETE(g_WndMng.m_pWndPetTransEggs);
	
	g_WndMng.m_pWndPetTransEggs = new CWndPetTransEggs;

	if(g_WndMng.m_pWndPetTransEggs)
		g_WndMng.m_pWndPetTransEggs->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomOpen(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_CLOSE )
		CSecretRoomMng::GetInstance()->SecretRoomOpen();
	else
		pUser->AddText( "Is Not Close State!!!" );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomEntrance( CScanner& s )
{
#ifdef __CLIENT
	if( s.GetNumber() == 1 )
	{
		g_DPlay.SendTeleportToSecretRoomDungeon();
		return TRUE;
	}

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if(pWndWorld)
	{
		for(int i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
		{
			pWndWorld->m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
			pWndWorld->m_stKillCountCiphers[i].szMyGuildKillCount = '0';
			pWndWorld->m_stKillCountCiphers[i].ptPos = CPoint(0,0);
			pWndWorld->m_stKillCountCiphers[i].fScaleX = 1.0f;
			pWndWorld->m_stKillCountCiphers[i].fScaleY = 1.0f;
			pWndWorld->m_stKillCountCiphers[i].nAlpha = 255;

//			pWndWorld->m_bDrawMyGuildKillCount[i] = TRUE;
//			pWndWorld->m_szMyGuildKillCount[i] = '0';
		}
	}
	
	if(g_WndMng.m_pWndSecretRoomMsg)
		SAFE_DELETE( g_WndMng.m_pWndSecretRoomMsg );

	g_DPlay.SendSecretRoomEntrance();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomNext( CScanner& s ) {
#ifdef __WORLDSERVER
	CSecretRoomMng::GetInstance()->m_dwRemainTime = 0;
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomTender( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomLineUp( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomLineUpOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomClose(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_WAR )
	{
		auto it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for( ; it!=CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); it++ )
		{
			CSecretRoomContinent* pSRCont = it->second;
			if( pSRCont && pSRCont->m_nState != SRCONT_CLOSE )
				pSRCont->m_dwRemainTime = 0;
				//pSRCont->SetContCloseWait();
		}
	}
	else	
		pUser->AddText( "Is Not War State!!!" );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomTenderView( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderView();

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomCheckTaxRate);
	g_WndMng.m_pWndSecretRoomCheckTaxRate = new CWndSecretRoomCheckTaxRate;

	if(g_WndMng.m_pWndSecretRoomCheckTaxRate)
	{
		g_WndMng.m_pWndSecretRoomCheckTaxRate->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomTenderCancelReturn( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderCancelReturn();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_ElectionRequirement( CScanner& s )
{
#ifdef __CLIENT
	IElection* pElection	= CCLord::Instance()->GetElection();
	char lpString[100]		= { 0,};
	sprintf( lpString, "election state : total(%d)/requirement(%d)", pElection->GetVote(), pElection->GetRequirement() );
	g_WndMng.PutString( lpString );
#endif	// __CLIENT
	return TRUE;
}

//#ifdef __INTERNALSERVER
BOOL TextCmd_RemoveTotalGold(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	__int64 iGold	= static_cast<__int64>( s.GetInt64() );
	if( iGold > pUser->GetTotalGold() )
	{
		char szText[100]	= { 0,};
		sprintf( szText, "TextCmd_RemoveTotalGold: %I64d", pUser->GetTotalGold() );
		pUser->AddText( szText );
	}
	else
		pUser->RemoveTotalGold( iGold );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionAddDeposit( CScanner& s )
{
#ifdef __CLIENT
	__int64 iDeposit	= static_cast<__int64>( s.GetNumber() );
	g_DPlay.SendElectionAddDeposit( iDeposit );
#endif	// __CLIENT
	return TRUE;
}
BOOL TextCmd_ElectionSetPledge( CScanner& s )
{
#ifdef __CLIENT
	char szPledge[CCandidate::nMaxPledgeLen]	= { 0,};
	
	s.GetLastFull();
	if( lstrlen( s.token ) >= CCandidate::nMaxPledgeLen )
		return TRUE;
	lstrcpy( szPledge, s.token );
	StringTrimRight( szPledge );
//	RemoveCRLF( szPledge );
	g_DPlay.SendElectionSetPledge( szPledge );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_ElectionIncVote( CScanner& s )
{
#ifdef __CLIENT
	s.GetToken();
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
	if( idPlayer > 0 )
		g_DPlay.SendElectionIncVote( idPlayer );
	else
		g_WndMng.PutString( prj.GetText( TID_GAME_ELECTION_INC_VOTE_E001 ) );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_ElectionProcess( CScanner& s )
{
#ifdef __WORLDSERVER
	BOOL bRun	= static_cast<BOOL>( s.GetNumber() );
	g_dpDBClient.SendElectionProcess( bRun );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionBeginCandidacy( CScanner& s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginCandidacy();
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_ElectionBeginVote( CScanner& s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginVote();
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_ElectionEndVote( CScanner& s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginEndVote();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionState( CScanner& s )
{
#ifdef __CLIENT
	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	pElection->State();
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_LEventCreate( CScanner & s )
{
#ifdef __CLIENT
	int iEEvent		= s.GetNumber();
	int iIEvent		= s.GetNumber();
	g_DPlay.SendLEventCreate( iEEvent, iIEvent );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_LEventInitialize( CScanner & s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendLEventInitialize();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LSkill( CScanner & s )
{
#ifdef __CLIENT
	int nSkill	= s.GetNumber();
	s.GetToken();
	char szTarget[MAX_PLAYER]	= { 0,};
	strncpy( szTarget, s.token, MAX_PLAYER );
	szTarget[MAX_PLAYER-1]	= '\0';
	g_DPlay.SendLordSkillUse( nSkill, szTarget );
#endif	// __CLIENT
	return TRUE;
}
//#endif	// __INTERNALSERVER

BOOL TextCmd_SetTutorialState(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int nTutorialState	= s.GetNumber();
	pUser->SetTutorialState( nTutorialState );
	pUser->AddSetTutorialState();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_TaxApplyNow( CScanner& s )
{
#ifdef __WORLDSERVER
	CTax::GetInstance()->SetApplyTaxRateNow();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_HonorTitleSet(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	int nIdx = -1;
	s.GetToken();
	if( s.tokenType == NUMBER ) 
	{
		nIdx	= _ttoi( s.Token );
	}
	else
	{
		nIdx	= CTitleManager::Instance()->GetIdxByName( s.Token );
	}

	if( nIdx < 0 ||  nIdx >= MAX_HONOR_TITLE )
		return FALSE;

	DWORD dwNum	= s.GetNumber();
	if( dwNum > 100000000 ) dwNum = 100000000;
	if( dwNum < 0 ) dwNum = 0;

	pUser->SetHonorCount(nIdx,dwNum);
	pUser->AddHonorListAck();
	g_dpDBClient.SendLogGetHonorTime(pUser,nIdx);
#endif // __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_RainbowRaceApp( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRaceApplicationOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRacePass(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CRainbowRace* pRainbowRace = CRainbowRaceMng::GetInstance()->GetRainbowRacerPtr( pUser->m_idPlayer );
	if( pRainbowRace )
	{
		pRainbowRace->SetNowGameComplete( pUser );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceOpen( CScanner& s )
{
#ifdef __WORLDSERVER
	if( CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_CLOSED )
		CRainbowRaceMng::GetInstance()->SetState( CRainbowRaceMng::RR_OPEN );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceNext( CScanner& s )
{
#ifdef __WORLDSERVER
	CRainbowRaceMng::GetInstance()->SetNextTime( 0 );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceInfo( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRaceInfo)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRaceInfo);

	g_WndMng.m_pWndRainbowRaceInfo = new CWndRainbowRaceInfo;

	if(g_WndMng.m_pWndRainbowRaceInfo)
		g_WndMng.m_pWndRainbowRaceInfo->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceRule( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRaceRule)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRaceRule);

	g_WndMng.m_pWndRainbowRaceRule = new CWndRainbowRaceRule;

	if(g_WndMng.m_pWndRainbowRaceRule)
		g_WndMng.m_pWndRainbowRaceRule->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceRanking( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRacePrevRankingOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRacePrize( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRacePrize)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRacePrize);

	g_WndMng.m_pWndRainbowRacePrize = new CWndRainbowRacePrize;

	if(g_WndMng.m_pWndRainbowRacePrize)
		g_WndMng.m_pWndRainbowRacePrize->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceKawiBawiBo( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_GAWIBAWIBO, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceDice( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_DICEPLAY, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceArithmetic( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_ARITHMATIC, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceStopWatch( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_STOPWATCH, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceTyping( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_TYPING, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceCard( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_PAIRGAME, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceLadder( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_LADDER, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceReqFininsh( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRaceReqFinish();
#endif // __CLIENT
	return TRUE;
}


BOOL TextCmd_ChangeAttribute( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndChangeAttribute)
		SAFE_DELETE(g_WndMng.m_pWndChangeAttribute);

	g_WndMng.m_pWndChangeAttribute = new CWndChangeAttribute;

	if(g_WndMng.m_pWndChangeAttribute)
		g_WndMng.m_pWndChangeAttribute->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_HousingVisitRoom( CScanner& s )
{
#ifdef __CLIENT
	s.GetToken();
	if( s.Token == "" )	// ???? ?? ?????? ????????...
		g_DPlay.SendHousingVisitRoom( g_pPlayer->m_idPlayer );
	else	// ?????????? ?????? ???? ???????? ??????...
	{
		DWORD dwPlayerId = CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
		if( dwPlayerId )
			g_DPlay.SendHousingVisitRoom( dwPlayerId );
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_HousingGMRemoveAll(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	CHousingMng::GetInstance()->ReqGMFunrnitureListAll( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
/*
#if __VER >= 14 // __SMELT_SAFETY
BOOL TextCmd_SmeltSafetyNormal( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_NORMAL);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SmeltSafetyAccessary( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_ACCESSARY);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SmeltSafetyPiercing( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_PIERCING);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

#endif //__SMELT_SAFETY
*/
BOOL TextCmd_SmeltSafetyElement( CScanner& s )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndSmeltSafety )
		SAFE_DELETE( g_WndMng.m_pWndSmeltSafety );

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety( CWndSmeltSafety::WND_ELEMENT );
	if( g_WndMng.m_pWndSmeltSafety )
		g_WndMng.m_pWndSmeltSafety->Initialize( NULL );
#endif // __CLIENT
	return TRUE;
}

#ifdef __QUIZ
BOOL TextCmd_QuizEventOpen( CScanner& s )
{
#ifdef __WORLDSERVER
	if( !CQuiz::GetInstance()->IsRun() )
		g_dpDBClient.SendQuizEventOpen( CQuiz::GetInstance()->GetType() );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_QuizEventEnterance( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendQuizEventEntrance();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_QuizStateNext(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( CQuiz::GetInstance()->IsRun() )
	{
		if( IsValidObj( pUser ) && pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		{
			CQuiz::GetInstance()->SetNextTime( 0 );
			CQuiz::GetInstance()->Process();
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_QuizEventClose( CScanner& s )
{
#ifdef __WORLDSERVER
	if( CQuiz::GetInstance()->IsRun() )
		CQuiz::GetInstance()->CloseQuizEvent();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __QUIZ

BOOL TextCmd_BuyGuildHouse( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendPacket<PACKETTYPE_GUILDHOUSE_BUY>();
#endif // __CLIENT
	return TRUE;
}

#ifdef __CLIENT
#include "WndGuildHouse.h"
#endif // __CLIENT
BOOL TextCmd_GuildHouseUpkeep( CScanner & s )
{
#ifdef __CLIENT
	if( !g_WndMng.m_pWndUpkeep )
	{
		g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
		g_WndMng.m_pWndUpkeep->Initialize( &g_WndMng, APP_CONFIRM_BUY_ );
	}
	else
	{
		g_WndMng.m_pWndUpkeep->Destroy( );
		g_WndMng.m_pWndUpkeep = NULL;
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_CampusInvite(CScanner & s, CPlayer_ * pRequest) {
#ifdef __WORLDSERVER
	if( !IsValidObj( pRequest ) )
		return FALSE;

	s.GetToken();
	u_long idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

	if( 0 < idTarget )
	{
		CUser* pTarget	= g_UserMng.GetUserByPlayerID( idTarget );	
		if( IsValidObj( pTarget ) )
		{
			CCampusHelper::GetInstance()->OnInviteCampusMember( pRequest, pTarget );
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idTarget );
			if( pPlayerData )
				pRequest->AddQueryPlayerData( idTarget, pPlayerData );
		}

		else
			pRequest->AddDefinedText( TID_DIAG_0061, "\"%s\"", s.Token );
	}
	else
		pRequest->AddDefinedText( TID_DIAG_0060, "\"%s\"", s.Token );
#endif // __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_RemoveCampusMember(CScanner & s, CPlayer_ * pRequest) {
#ifdef __WORLDSERVER
	if( !IsValidObj( pRequest ) )
		return FALSE;

	s.GetToken();
	u_long idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

	if( idTarget > 0 )
		CCampusHelper::GetInstance()->OnRemoveCampusMember( pRequest, idTarget );
		
	else
		pRequest->AddDefinedText( TID_DIAG_0060, "\"%s\"", s.Token );

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_UpdateCampusPoint(CScanner & s, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( !IsValidObj( pUser ) )
		return FALSE;

	int nCampusPoint = 0;
	nCampusPoint = s.GetNumber();

	if( IsValidObj( pUser ) )
		g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, nCampusPoint, TRUE, 'G' );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_InvenRemove(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __WORLDSERVER
	if( IsValidObj( pUser ) )
	{
		int nSize = pUser->m_Inventory.GetMax();
		for( int i = 0 ; i < nSize; ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) && !pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) && !pItemElem->IsPet() && !pItemElem->IsEatPet() )
				pUser->UpdateItem(*pItemElem, UI::Num::RemoveAll());
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

#include <variant>
#include "ar.h"
struct One { int x; };
CAr & operator<<(CAr & ar, One one) {
	return ar << one.x;
}
CAr & operator>>(CAr & ar, One & one) {
	return ar >> one.x;
}

struct Two_ { int x; int y; };
CAr & operator<<(CAr & ar, Two_ one) {
	return ar << one.x << one.y;
}
CAr & operator>>(CAr & ar, Two_ & one) {
	return ar >> one.x >> one.y;
}

struct Strings { std::string str; };
CAr & operator<<(CAr & ar, const Strings & one) {
	ar.WriteString(one.str.c_str());
	return ar;
}
CAr & operator>>(CAr & ar, Strings & one) {
	char xxx[256];
	ar.ReadString(xxx);
	one.str = xxx;
	return ar;
}

using COUCOU = std::variant<One, Two_, Strings>;

#include <format>


BOOL TextCmd_Arbitrary(CScanner & scanner, CPlayer_ * pUser) {
#ifdef __CLIENT
	scanner.GetToken();
	CString type = scanner.Token;

	COUCOU v;
	if (type == "One") {
		int x = scanner.GetNumber();
		v = One{ x };
	} else if (type == "Two") {
		int x = scanner.GetNumber();
		int y = scanner.GetNumber();
		v = Two_{ x, y };
	} else if (type == "Strings") {
		scanner.GetToken();
		CString content = scanner.Token;
		v = Strings{ std::string(content.GetString()) };
	} else if (type == "hack") {

		g_DPlay.SendPacket<PACKETTYPE_SQUONK_ARBITRARY_PACKET,
			std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t
		>(15, 15, 15, 15);

		return TRUE;

	} else {
		g_WndMng.PutString("You're drunk");
		return TRUE;
	}

	g_DPlay.SendPacket<PACKETTYPE_SQUONK_ARBITRARY_PACKET>(v);
		
#endif
	return TRUE;
}

#ifndef __CLIENT
void CDPSrvr::OnSquonKArbitraryPacket(CAr & ar, CUser & thisIsMe) {
	COUCOU v; ar >> v;

	if (v.valueless_by_exception()) {
		thisIsMe.AddText("Mistakes were made");
		return;
	}

	struct Visitor {
		std::string operator()(One one) {
			return std::format("One= {}", one.x);
		}

		std::string operator()(Two_ one) {
			return std::format("Two= {} {}", one.x, one.y);
		}
		std::string operator()(Strings one) {
			return std::format("Strings= {}", one.str);
		}
	};

	std::string x = std::visit(Visitor{}, v);

	thisIsMe.AddText(x.c_str());
}
#endif


#define ON_TEXTCMDFUNC( a, b, c, d, e, f, g, h ) AddCommand(m_allCommands, a, b, c, d, e, f, g, h);


TextCmdFunc::TextCmdFunc(
	BOOL(*withPlayer)(CScanner & scanner, CPlayer_ * player),
	const TCHAR * m_pCommand,
	const TCHAR * m_pAbbreviation,
	const TCHAR * m_pKrCommand,
	const TCHAR * m_pKrAbbreviation,
	DWORD m_nServer,
	DWORD m_dwAuthorization,
	const TCHAR * m_pszDesc
) : m_withPlayer(withPlayer), m_ignoresPlayer(false),
m_pCommand(m_pCommand), m_pAbbreviation(m_pAbbreviation),
m_pKrCommand(m_pKrCommand), m_pKrAbbreviation(m_pKrAbbreviation),
m_nServer(m_nServer), m_dwAuthorization(m_dwAuthorization),
m_pszDesc(m_pszDesc) {
	this->m_pCommand.MakeLower();
	this->m_pAbbreviation.MakeLower();
}

TextCmdFunc::TextCmdFunc(
	BOOL(*withoutPlayer)(CScanner & scanner),
	const TCHAR * m_pCommand,
	const TCHAR * m_pAbbreviation,
	const TCHAR * m_pKrCommand,
	const TCHAR * m_pKrAbbreviation,
	DWORD m_nServer,
	DWORD m_dwAuthorization,
	const TCHAR * m_pszDesc
) : m_withoutPlayer(withoutPlayer), m_ignoresPlayer(true),
m_pCommand(m_pCommand), m_pAbbreviation(m_pAbbreviation),
m_pKrCommand(m_pKrCommand), m_pKrAbbreviation(m_pKrAbbreviation),
m_nServer(m_nServer), m_dwAuthorization(m_dwAuthorization),
m_pszDesc(m_pszDesc) {
	this->m_pCommand.MakeLower();
	this->m_pAbbreviation.MakeLower();
}

template<typename F>
void AddCommand(
	std::vector<TextCmdFunc> & commands,
	F func,
	const TCHAR * pCommand,
	const TCHAR * pAbbreviation,
	const TCHAR * pKrCommand,
	const TCHAR * pKrAbbreviation,
	DWORD nServer,
	DWORD dwAuthorization,
	const TCHAR * pszDesc
) {
	commands.emplace_back(func,
		pCommand, pAbbreviation, pKrCommand, pKrAbbreviation,
		nServer, dwAuthorization, pszDesc
	);
}


CmdFunc::AllCommands::AllCommands() {
////////////////////////////////////////////////// AUTH_GENERAL begin/////////////////////////////////////////////////////
	ON_TEXTCMDFUNC( TextCmd_whisper,               "whisper",           "w",              "??????",         "??",      TCM_SERVER, AUTH_GENERAL      , "?????? [/???? ?????? ????]" )
	ON_TEXTCMDFUNC( TextCmd_say,                   "say",               "say",            "??",             "??",      TCM_SERVER, AUTH_GENERAL      , "?????? [/???? ?????? ????]" )
	ON_TEXTCMDFUNC( TextCmd_Position,              "position",          "pos",            "????",           "????",    TCM_CLIENT, AUTH_GENERAL      , "???? ?????? ??????????." )
	ON_TEXTCMDFUNC( TextCmd_shout,                 "shout",             "s",              "??????",         "??",      TCM_BOTH  , AUTH_GENERAL      , "?????? [/???? ?????? ????]" )
	ON_TEXTCMDFUNC( TextCmd_PartyChat,             "partychat",         "p",              "??????",         "??",      TCM_BOTH  , AUTH_GENERAL      , "???? ???? [/???? ????]" )
	ON_TEXTCMDFUNC( TextCmd_Time,                  "Time",              "ti",             "????",           "??",      TCM_CLIENT, AUTH_GENERAL      , "???? ???? [/????]" )
//	ON_TEXTCMDFUNC( TextCmd_ChangeFace,            "ChangeFace",        "cf",             "????????",       "????",    TCM_BOTH  , AUTH_GENERAL      , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildChat,             "GuildChat",         "g",              "??????",         "????",    TCM_BOTH, AUTH_GENERAL      , "??????" )
	ON_TEXTCMDFUNC( TextCmd_PartyInvite,           "PartyInvite",       "partyinvite",    "????????",       "????",    TCM_SERVER, AUTH_GENERAL      , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildInvite,           "GuildInvite",       "guildinvite",    "????????",       "????",    TCM_SERVER, AUTH_GENERAL      , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_CampusInvite,          "CampusInvite",		"campusinvite",   "????????",		"????",    TCM_SERVER, AUTH_GENERAL      , "???? ????" )
#ifdef __CLIENT
	ON_TEXTCMDFUNC( TextCmd_tradeagree,            "tradeagree",        "ta",             "????????",       "????",    TCM_CLIENT, AUTH_GENERAL      , "???? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_traderefuse,           "traderefuse",       "tr",             "????????",       "????",    TCM_CLIENT, AUTH_GENERAL      , "???? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_whisperagree,          "whisperagree",      "wa",             "??????????",     "????",    TCM_CLIENT, AUTH_GENERAL      , "?????? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_whisperrefuse,         "whisperrefuse",     "wr",             "??????????",     "????",    TCM_CLIENT, AUTH_GENERAL      , "?????? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_messengeragree,        "messengeragree",    "ma",             "??????????",     "????",    TCM_CLIENT, AUTH_GENERAL      , "?????? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_messengerrefuse,       "messengerrefuse",   "mr",             "??????????",     "????",    TCM_CLIENT, AUTH_GENERAL      , "?????? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_stageagree,            "stageagree",        "ga",             "????????",       "????",    TCM_CLIENT, AUTH_GENERAL      , "???? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_stagerefuse,           "stagerefuse",       "gr",             "????????",       "????",    TCM_CLIENT, AUTH_GENERAL      , "???? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_connectagree,          "connectagree",      "ca",             "????????",       "????",    TCM_CLIENT, AUTH_GENERAL      , "???????? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_connectrefuse,         "connectrefuse",     "cr",             "????????????",   "????",    TCM_CLIENT, AUTH_GENERAL      , "???????? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_shoutagree,            "shoutagree",        "ha",             "??????????",     "????",    TCM_CLIENT, AUTH_GENERAL      , "?????? ???? [/????] " )
	ON_TEXTCMDFUNC( TextCmd_shoutrefuse,           "shoutrefuse",       "hr",             "??????????",     "????",    TCM_CLIENT, AUTH_GENERAL      , "?????? ???? [/????] " )
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	ON_TEXTCMDFUNC( TextCmd_BlockUser,             "ignore",             "ig",             "????????",       "????",    TCM_CLIENT, AUTH_GENERAL      , "???????? [/???? ??????]" )
	ON_TEXTCMDFUNC( TextCmd_CancelBlockedUser,     "unignore",           "uig",            "????????????",   "??????",  TCM_CLIENT, AUTH_GENERAL      , "???????????? [/???? ??????]" )
	ON_TEXTCMDFUNC( TextCmd_IgnoreList,            "ignorelist",         "igl",            "????????????",   "??????",  TCM_CLIENT, AUTH_GENERAL      , "???? ???? ????" )
#endif // __YS_CHATTING_BLOCKING_SYSTEM
#endif //__CLIENT
////////////////////////////////////////////////// AUTH_GENERAL end/////////////////////////////////////////////////////
	// GM_LEVEL_1
	ON_TEXTCMDFUNC( TextCmd_Teleport,              "teleport",          "te",             "????????",       "????",    TCM_SERVER, AUTH_GAMEMASTER   , "????????" )
	ON_TEXTCMDFUNC( TextCmd_Invisible,             "invisible",         "inv",            "????",           "????",    TCM_SERVER, AUTH_GAMEMASTER   , "??????" )
	ON_TEXTCMDFUNC( TextCmd_NoInvisible,           "noinvisible",       "noinv",          "????????",       "????",    TCM_SERVER, AUTH_GAMEMASTER   , "?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_Summon,                "summon",            "su",             "????",           "????",    TCM_SERVER, AUTH_GAMEMASTER   , "????????" )
	ON_TEXTCMDFUNC( TextCmd_count,                 "count",             "cnt",            "????????",       "????????",TCM_SERVER, AUTH_GAMEMASTER   , "?????? ??????" )
	
	// GM_LEVEL_2
	ON_TEXTCMDFUNC( TextCmd_Out,                   "out",               "out",            "????",           "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "????" )
	ON_TEXTCMDFUNC( TextCmd_Talk,                  "talk",              "nota",           "??????",         "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "?????? ?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_NoTalk,                "notalk",            "ta",             "??????",         "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "?????? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_ip,                    "ip",                "ip",             "??????",         "??????",  TCM_BOTH  , AUTH_GAMEMASTER2     , "???? IP????" )
#ifdef __JEFF_9_20
	ON_TEXTCMDFUNC( TextCmd_Mute,					"Mute",				"mute",          "??????",     "??????",    TCM_SERVER,  AUTH_GAMEMASTER2, "" )	
#endif	// __JEFF_9_20
	ON_TEXTCMDFUNC( TextCmd_GuildRanking,          "GuildRanking",       "ranking",        "????",           "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "" )
	ON_TEXTCMDFUNC( TextCmd_FallSnow,              "FallSnow",           "fs",             "??????",         "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "?? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_StopSnow,              "StopSnow",           "ss",             "??????",         "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "?? ?????? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_FallRain,              "FallRain",           "frain",          "??????",         "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "?? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_StopRain,              "StopRain",           "sr",             "??????",         "????",    TCM_SERVER, AUTH_GAMEMASTER2   , "?? ?????? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_System,                "system",             "sys",            "????",           "??",      TCM_SERVER, AUTH_GAMEMASTER2   , "?????? ??????" )

	// GM_LEVEL_3
	ON_TEXTCMDFUNC( TextCmd_PvpParam,              "PvpParam",           "p_Param",        "PVP????",        "????",    TCM_SERVER, AUTH_GAMEMASTER3, "PVP(????)????" )
	ON_TEXTCMDFUNC( TextCmd_PKParam,			   "PKParam",			 "pkparam",		   "PK????",		 "pk????",  TCM_SERVER, AUTH_GAMEMASTER3, "????????" )
	ON_TEXTCMDFUNC( TextCmd_Undying,               "undying",            "ud",             "????",           "??",      TCM_BOTH  , AUTH_GAMEMASTER3   , "????" )
	ON_TEXTCMDFUNC( TextCmd_Undying2,              "undying2",           "ud2",            "??????",         "????",    TCM_BOTH  , AUTH_GAMEMASTER3   , "??????" )
	ON_TEXTCMDFUNC( TextCmd_NoUndying,             "noundying",          "noud",           "????????",       "????",    TCM_BOTH  , AUTH_GAMEMASTER3   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_Onekill,               "onekill",            "ok",             "????",           "????",    TCM_BOTH  , AUTH_GAMEMASTER3   , "???? ?????? ??????" )
	ON_TEXTCMDFUNC( TextCmd_NoOnekill,             "noonekill",          "nook",           "????????",       "????",    TCM_BOTH  , AUTH_GAMEMASTER3   , "???? ?????? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_AroundKill,            "aroundkill",         "ak",             "????",           "??",      TCM_SERVER, AUTH_GAMEMASTER3   , "?????????? ???? ?????? ??????" )
	ON_TEXTCMDFUNC( TextCmd_stat,                  "stat",               "stat",           "????",           "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "???? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_Level,                 "level",              "lv",             "????",           "??",      TCM_SERVER, AUTH_GAMEMASTER3   , "???? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_InitSkillExp,          "InitSkillExp",       "InitSE",         "??????????",     "????",    TCM_SERVER, AUTH_GAMEMASTER3, "??????????" )
	ON_TEXTCMDFUNC( TextCmd_SkillLevel,            "skilllevel",         "slv",            "????????",       "????",    TCM_BOTH  , AUTH_GAMEMASTER3   , "???????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_SkillLevelAll,         "skilllevelAll",      "slvAll",         "??????????",     "??????",  TCM_BOTH  , AUTH_GAMEMASTER3   , "???????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_BeginQuest,            "BeginQuest",         "bq",             "??????????",     "????",    TCM_SERVER, AUTH_GAMEMASTER3, "?????? ???? [ID]" )
	ON_TEXTCMDFUNC( TextCmd_EndQuest,              "EndQuest",           "eq",             "??????????",     "????",    TCM_SERVER, AUTH_GAMEMASTER3, "?????? ???? [ID]" )
	ON_TEXTCMDFUNC( TextCmd_RemoveQuest,           "RemoveQuest",        "rq",             "??????????",     "????",    TCM_SERVER, AUTH_GAMEMASTER3, "?????? ???? [ID]" )
	ON_TEXTCMDFUNC( TextCmd_RemoveAllQuest,        "RemoveAllQuest",     "raq",            "??????????????", "??????",  TCM_SERVER, AUTH_GAMEMASTER3, "?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_RemoveCompleteQuest,   "RemoveCompleteQuest","rcq",            "??????????????", "??????",  TCM_SERVER, AUTH_GAMEMASTER3, "?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_ChangeJob,             "changejob",			 "cjob",           "????",           "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_Freeze,                "freeze",             "fr",             "????",           "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "???????? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_NoFreeze,              "nofreeze",           "nofr",           "????????",       "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "???????? ?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_PartyLevel,            "PartyLevel",         "plv",            "????????",       "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "???????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildStat,             "GuildStat",          "gstat",          "????????",       "??????",  TCM_SERVER, AUTH_GAMEMASTER3   , "???? ????????" )
	ON_TEXTCMDFUNC( TextCmd_CreateGuild,           "createguild",        "cg",             "????????",       "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_DestroyGuild,          "destroyguild",       "dg",             "????????",       "????",    TCM_CLIENT, AUTH_GAMEMASTER3   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatIn,         "GCIn",               "gcin",           "??????????",     "??????",  TCM_BOTH  , AUTH_GAMEMASTER3, "???????? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatOpen,       "GCOpen",             "gcopen",         "??????????",     "??????",  TCM_BOTH  , AUTH_GAMEMASTER3, "???????? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatClose,      "GCClose",            "gcclose",        "??????????",     "??????",  TCM_BOTH  , AUTH_GAMEMASTER3, "???????? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatNext,       "GCNext",             "gcNext",         "??????????",     "??????",  TCM_BOTH  , AUTH_GAMEMASTER3, "???????? ????" )	
	ON_TEXTCMDFUNC( TextCmd_indirect,              "indirect",           "id",             "????",           "????",    TCM_BOTH  , AUTH_GAMEMASTER3   , "???????? ???????? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_CreateNPC,             "createnpc",          "cn",             "??????????",     "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "npc????" )
	ON_TEXTCMDFUNC( TextCmd_LuaEventList,     "EVENTLIST",         "eventlist",          "??????????",     "??????????",    TCM_SERVER,  AUTH_GAMEMASTER3, "" )
	ON_TEXTCMDFUNC( TextCmd_LuaEventInfo,     "EVENTINFO",         "eventinfo",          "??????????",     "??????????",    TCM_SERVER,  AUTH_GAMEMASTER3, "" )	
	ON_TEXTCMDFUNC( TextCmd_GameSetting,           "gamesetting",        "gs",             "????????",       "????",    TCM_SERVER, AUTH_GAMEMASTER3   , "???? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_RemoveNpc,             "rmvnpc",             "rn",             "????",           "??",      TCM_SERVER, AUTH_GAMEMASTER3, "NPC????" )

	// GM_LEVEL_4
	ON_TEXTCMDFUNC( TextCmd_Disguise,				"disguise",           "dis",            "????",           "??",      TCM_SERVER, AUTH_ADMINISTRATOR   , "????" )
	ON_TEXTCMDFUNC( TextCmd_NoDisguise,				"noDisguise",         "nodis",          "????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_ItemEnchant, "itemenchant", "ie", "ie", "ie", TCM_SERVER, AUTH_ADMINISTRATOR, "Change various item enchants")
	ON_TEXTCMDFUNC( TextCmd_ResistItem,				"ResistItem",         "ritem",          "??????????",     "????",    TCM_BOTH  , AUTH_ADMINISTRATOR, "??????????" )
	ON_TEXTCMDFUNC( TextCmd_GetGold,				"getgold",            "gg",             "????",           "??",      TCM_SERVER, AUTH_ADMINISTRATOR, "?? ????" )
	ON_TEXTCMDFUNC( TextCmd_CreateItem,				"createitem",         "ci",             "??????????",     "????",    TCM_BOTH  , AUTH_ADMINISTRATOR, "??????????" )
	ON_TEXTCMDFUNC( TextCmd_CreateItem2,			"createitem2",        "ci2",            "??????????2",    "????2",   TCM_SERVER, AUTH_ADMINISTRATOR, "??????????2" )
	ON_TEXTCMDFUNC( TextCmd_QuestState,				"QuestState",         "qs",             "??????????",     "????",    TCM_SERVER, AUTH_ADMINISTRATOR, "?????? ???? [ID] [State]" )
	ON_TEXTCMDFUNC( TextCmd_LoadScript,				"loadscript",         "loscr",          "????????????",   "????",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "???????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_ReloadConstant,			"ReloadConstant",     "rec",            "??????????????", "????",    TCM_SERVER, AUTH_ADMINISTRATOR, "?????? ????????????" )
	ON_TEXTCMDFUNC( TextCmd_CTD,					"ctd",				 "ctd",            "????????????",   "????",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "?????? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_Piercing,				"Piercing",           "pier",           "??????",         "????",    TCM_BOTH  , AUTH_ADMINISTRATOR, "??????(????)" )
	ON_TEXTCMDFUNC( TextCmd_PetLevel,				"petlevel",         "pl",          "??????",     "????",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_PetExp,					"petexp",         "pe",          "????????",     "????",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_MakePetFeed,			"makepetfeed",         "mpf",          "??????????",     "????",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_Pet,					"Pet",         "pet",          "??",     "??",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_Lua,					"Lua",         "lua",          "????",     "????",    TCM_SERVER,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GC1to1Open,				"GC1TO1OPEN",		"gc1to1open",			"??????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GC1to1Close,			"GC1TO1CLOSE",		"gc1to1close",			"??????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GC1to1Next,				"GC1TO1NEXT",		"gc1to1next",			"??????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RefineAccessory,		"RefineAccessory",	"ra",	"????????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RefineCollector,		"RefineCollector",	"rc",	"??????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GenRandomOption,		"GenRandomOption",	"gro",	"????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_InitializeRandomOption,	"InitializeRandomOption",	"iro",	"????????????", "????????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SetRandomOption,		"SetRandomOption",	"sro",	"????????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
#ifdef __PET_1024
	ON_TEXTCMDFUNC( TextCmd_SetPetName,             "SetPetName",           "setpetname",             "??????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "??????" )
	ON_TEXTCMDFUNC( TextCmd_ClearPetName,           "ClearPetName",           "cpn",             "??????????",       "??????",    TCM_CLIENT, AUTH_ADMINISTRATOR , "??????????" )
#endif	// __PET_1024
	ON_TEXTCMDFUNC( TextCmd_Propose,				"Propose",           "propose",             "????????",       "????????",    TCM_SERVER, AUTH_ADMINISTRATOR , "????????" )
	ON_TEXTCMDFUNC( TextCmd_Refuse,					"Refuse",           "refuse",             "????????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "????????????" )
	ON_TEXTCMDFUNC( TextCmd_Couple,					"Couple",           "couple",             "????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "????" )
	ON_TEXTCMDFUNC( TextCmd_Decouple,				"Decouple",           "decouple",             "????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "????????" )
	ON_TEXTCMDFUNC( TextCmd_ClearPropose,           "ClearPropose",           "clearpropose",             "??????????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "??????????????" )
//	ON_TEXTCMDFUNC( TextCmd_CoupleState,            "CoupleState",           "couplestate",             "????????",       "????",    TCM_CLIENT, AUTH_ADMINISTRATOR , "????????" )
	ON_TEXTCMDFUNC( TextCmd_NextCoupleLevel,        "NextCoupleLevel",           "ncl",             "??????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "??????????" )
#ifdef __NPC_BUFF
	ON_TEXTCMDFUNC( TextCmd_RemoveAllBuff,			"RemoveBuff",		"rb",			"????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif // __NPC_BUFF
	ON_TEXTCMDFUNC( TextCmd_HonorTitleSet,			"HonorTitleSet", "hts", "????????", "????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )


	ON_TEXTCMDFUNC(TextCmd_Arbitrary, "sqk", "sqk", "sqk", "sqk", TCM_CLIENT, AUTH_ADMINISTRATOR, "");

// ???????? ?????? 
	ON_TEXTCMDFUNC( TextCmd_Open,                  "open",               "open",           "????",           "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_Close,                 "close",              "close",          "????",           "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_Music,                 "music",              "mu",             "????",           "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "????????" )
	ON_TEXTCMDFUNC( TextCmd_Sound,                 "sound",              "so",             "????",           "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_LocalEvent,            "localevent",         "le",             "??????????",     "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "??????????" )
	ON_TEXTCMDFUNC( TextCmd_CommercialElem,        "CommercialElem",     "CommercialElem", "??????????",     "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "?????????? ??????" )
	ON_TEXTCMDFUNC( TextCmd_QuerySetPlayerName,    "SetPlayerName",      "spn",            "????????????",   "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "???????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_QuerySetGuildName,     "SetGuildName",       "sgn",            "????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "???? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_DeclWar,               "DeclWar",            "declwar",        "??????????",     "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_RemoveGuildMember,     "rgm",                "rgm",            "????????",       "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildRankingDBUpdate,  "GuildRankingUpdate", "rankingupdate",  "????",           "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_ItemMode,              "gmitem",             "gmitem",         "??????????",     "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "?????? ?????? ???????????? ????" )
	ON_TEXTCMDFUNC( TextCmd_ItemNotMode,           "gmnotitem",          "gmnotitem",      "??????????",     "??????",  TCM_SERVER, AUTH_ADMINISTRATOR   , "?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_AttackMode,            "gmattck",            "gmattck",        "????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "???? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_AttackNotMode,         "gmnotattck",         "gmnotattck",     "????????",       "??????",  TCM_SERVER, AUTH_ADMINISTRATOR   , "???? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_CommunityMode,         "gmcommunity",        "gmcommunity",    "????????????",   "????",    TCM_SERVER, AUTH_ADMINISTRATOR	, "????, ????, ????, ????, ???? ?????? ????" )
	ON_TEXTCMDFUNC( TextCmd_CommunityNotMode,      "gmnotcommunity",     "gmnotcommunity", "????????????",   "??????",  TCM_SERVER, AUTH_ADMINISTRATOR   , "???????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_ObserveMode,           "gmobserve",          "gmobserve",      "????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "??????, ????????, ????????, ???? ???? ??????" )
	ON_TEXTCMDFUNC( TextCmd_ObserveNotMode,        "gmnotobserve",       "gmnotobserve",   "????????",       "??????",  TCM_SERVER, AUTH_ADMINISTRATOR   , "???? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_EscapeReset,           "EscapeReset",        "EscapeReset",    "??????????",     "????",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "????(??????) ???? ??????" )
	ON_TEXTCMDFUNC( TextCmd_userlist,              "userlist",           "ul",             "????????????",   "????????",TCM_SERVER, AUTH_ADMINISTRATOR, "?????? ??????" )
	//ON_TEXTCMDFUNC( TextCmd_sbready,               "sbready",            "sbready",        "sbready",        "sbready", TCM_SERVER, AUTH_ADMINISTRATOR   , "sbready" )
	//ON_TEXTCMDFUNC( TextCmd_sbstart,               "sbstart",            "sbstart",        "sbstart",        "sbstart", TCM_SERVER, AUTH_ADMINISTRATOR   , "sbstart" )
	//ON_TEXTCMDFUNC( TextCmd_sbstart2,              "sbstart2",           "sbstart2",       "sbstart2",       "sbstart2",TCM_SERVER, AUTH_ADMINISTRATOR   , "sbstart2" )
	//ON_TEXTCMDFUNC( TextCmd_sbend,                 "sbend",              "sbend",          "sbend",          "sbend",   TCM_SERVER, AUTH_ADMINISTRATOR   , "sbend" )
	//ON_TEXTCMDFUNC( TextCmd_sbreport,              "sbreport",           "sbreport",       "sbreport",       "sbreport",TCM_SERVER, AUTH_ADMINISTRATOR   , "sbreport" )
	//ON_TEXTCMDFUNC( TextCmd_OpenBattleServer,      "bsopen",             "bsopen",         "bsopen",         "bsopen",  TCM_SERVER, AUTH_ADMINISTRATOR   , "bsopen" )
	//ON_TEXTCMDFUNC( TextCmd_CloseBattleServer,     "bsclose",            "bsclose",        "bsclose",        "bsclose", TCM_SERVER, AUTH_ADMINISTRATOR   , "bsclose" )
	ON_TEXTCMDFUNC( TextCmd_SetGuildQuest,         "SetGuildQuest",      "sgq",            "??????????",     "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "???? ?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_SetSnoop,              "Snoop",              "snoop",          "????",           "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "????" )
	ON_TEXTCMDFUNC( TextCmd_SetSnoopGuild,         "SnoopGuild",         "sg",             "????????????",   "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "???? ???? ????" )

	ON_TEXTCMDFUNC( TextCmd_GuildCombatRequest,    "GCRequest",          "gcrquest",       "??????????",     "??????",  TCM_BOTH  , AUTH_ADMINISTRATOR, "???????? ????" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatCancel,     "GCCancel",           "gccancel",       "??????????",     "??????",  TCM_BOTH  , AUTH_ADMINISTRATOR, "???????? ????" )
	ON_TEXTCMDFUNC( TextCmd_PostMail,              "PostMail",           "pm",             "????????",       "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_RemoveMail,            "RemoveMail",         "rm",             "????????",       "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_GetMailItem,           "GetMailItem",        "gm",             "????????",       "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "???? ????" )
	ON_TEXTCMDFUNC( TextCmd_GetMailGold,           "GetMailGolg",        "gmg",            "????",           "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "????" )
	ON_TEXTCMDFUNC( TextCmd_InvenClear,            "InvenClear",         "icr",            "????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR, "?????????? ?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_ExpUpStop,             "ExpUpStop",          "es",             "??????????",     "????",    TCM_SERVER, AUTH_ADMINISTRATOR, "???????? ?????? ?????? ?????? ????" )

#ifdef _DEBUG
	ON_TEXTCMDFUNC( TextCmd_CreateChar,            "createchar",         "cc",             "??????????",     "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "??????????" )
	ON_TEXTCMDFUNC( TextCmd_CreateCtrl,            "createctrl",         "ct",             "??????????",     "????",    TCM_SERVER, AUTH_ADMINISTRATOR   , "ctrl????" )
	ON_TEXTCMDFUNC( TextCmd_SetMonsterRespawn,     "setmonsterrespawn",  "smr",            "??????????????", "??????",  TCM_SERVER, AUTH_ADMINISTRATOR, "?????? ???? ????" )
	ON_TEXTCMDFUNC( TextCmd_TransyItemList,        "TransyItemList",     "til",            "????????????",   "??????",  TCM_CLIENT, AUTH_ADMINISTRATOR, "??????????????????" )
	ON_TEXTCMDFUNC( TextCmd_LoadToolTipColor,      "LoadToolTip",        "ltt",            "????????",       "??????",  TCM_CLIENT, AUTH_ADMINISTRATOR, "???? ???? ????" )
#endif

#ifdef __S1108_BACK_END_SYSTEM
	ON_TEXTCMDFUNC( TextCmd_PropMonster,           "monstersetting",     "ms",             "??????????",     "????",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "?????? ???? ????" )
#else
	ON_TEXTCMDFUNC( TextCmd_ChangeShopCost,	       "changeshopcost",	 "csc",	           "????????????",   "??????",  TCM_SERVER, AUTH_ADMINISTRATOR, "???????????? Min(0.5) ~ Max(2.0)"  )
#endif // __S1108_BACK_END_SYSTEM

#ifdef __EVENT_1101
	ON_TEXTCMDFUNC( TextCmd_CallTheRoll,			"CallTheRoll",        "ctr",            "????????",       "????",  TCM_BOTH,	AUTH_ADMINISTRATOR, "???? ???? ??????" )
#endif	// __EVENT_1101
	
	ON_TEXTCMDFUNC( TextCmd_AngelExp,				"AExp",		"aexp",			"??????????", "????",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	
	ON_TEXTCMDFUNC( TextCmd_RemoveAttribute,		"RemAttr",		"remattr",			"????????", "????",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_StartCollecting,		"StartCollecting",	"col1",	"????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_StopCollecting,			"StopCollecting",	"col2",	"??????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_DoUseItemBattery,		"Battery",	"battery",	"??????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_AvailPocket,			"AvailPocket",	"ap",	"??????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_PocketView,				"PocketView",	"pv",	"??????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_MoveItem_Pocket,		"MoveItemPocket",	"mip",	"??????????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_ItemLevel,				"ItemLevel",	"il",	"????", "????",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )

#ifdef __EVENTLUA_COUPON
	ON_TEXTCMDFUNC( TextCmd_Coupon,					"COUPON",		"coupon",			"????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif // __EVENTLUA_COUPON

//#ifdef __PERF_0226
	ON_TEXTCMDFUNC( TextCmd_CreatePc,				"CreatePc",		"cp",			"cp", "cp",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
//#endif	// __PERF_0226
#ifdef __SFX_OPT
	ON_TEXTCMDFUNC( TextCmd_SfxLv,					"SfxLevel",		"sl",			"sl", "sl",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif	

	ON_TEXTCMDFUNC( TextCmd_SecretRoomOpen,			"SROPEN",		"sropen",			"????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomNext,			"SRNEXT",		"srnext",			"????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomEntrance,		"SRENTRANCE",		"srentrance",			"????????????", "??????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTender,		"SRTENDER",		"srtender",			"????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomLineUp,		"SRLINEUP",		"srlineup",			"????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomClose,		"SRCLOSE",		"srclose",			"????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTenderView,	"SRVIEW",		"srview",			"????????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTenderCancelReturn, "SRCANCEL",		"srcancel",		"????????????????", "????",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_ElectionRequirement,	"ElectionRequirement", "er", "????????????", "??????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
//#ifdef __INTERNALSERVER
	ON_TEXTCMDFUNC( TextCmd_ElectionAddDeposit,		"ElectionAddDeposit", "ead", "????????", "????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionSetPledge,		"ElectionSetPledge", "esp", "????????????", "????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionIncVote,		"ElectionIncVote", "eiv", "????????", "????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionProcess,		"ElectionProcess", "ep", "????????????", "????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionBeginCandidacy,	"ElectionBeginCandidacy", "ebc", "??????????????", "??????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionBeginVote,		"ElectionBeginVote", "ebv", "????????????", "??????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionEndVote,		"ElectionEndVote", "eev", "????????????", "??????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionState,			"ElectionState", "estate", "????????????", "??????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LEventCreate,			"LEventCreate", "lecreate", "??????????????", "??????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LEventInitialize,		"LEventInitialize", "leinitialize", "????????????????", "??????", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LSkill,					"LSkill", "lskill", "????????", "????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RemoveTotalGold,		"RemoveTotalGold", "rtg", "??????", "????", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
//#endif	// __INTERNALSERVER

	ON_TEXTCMDFUNC( TextCmd_SetTutorialState,		"SetTutorialState", "sts", "????????????", "????", TCM_SERVER, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_TaxApplyNow,			"TaxApplyNow", "tan", "????????", "????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_HeavenTower,			"HeavenTower", "HTower", "????????", "????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_RemoveJewel,			"RemoveJewel", "RJewel", "????????", "????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_TransEggs,				"TransEggs", "TEggs", "??????", "????", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_PickupPetAwakeningCancel,	"PickupPetAwakeningCancel",	"ppac",	"??????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )

#ifdef __LAYER_1020
	ON_TEXTCMDFUNC( TextCmd_CreateLayer,            "CreateLayer",           "cl",             "??????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "??????????" )
	ON_TEXTCMDFUNC( TextCmd_DeleteLayer,            "DeleteLayer",           "dl",             "??????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "??????????" )
	ON_TEXTCMDFUNC( TextCmd_Layer,					"Layer",           "lay",             "??????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR , "??????????" )
#endif	// __LAYER_1020

	ON_TEXTCMDFUNC( TextCmd_RainbowRaceApp,			"RRApp",	"rrapp",	"????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceOpen,		"RROpen",	"rropen",	"????????????", "????",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceNext,		"RRNext",	"rrnext",	"????????????", "????",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRacePass,		"RRPass",	"rrpass",	"????????????", "????",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceInfo,		"RRinfo",	"rrinfo",	"????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceRule,		"RRRule",	"rrrule",	"????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceRanking,		"RRRanking","rrranking","????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRacePrize,		"RRPrize",	"rrprize",	"????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_RainbowRaceKawiBawiBo,	"RRKawiBawiBo",	"rrkawibawibo",	"??????????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceDice,		"RRDice",	"rrdice",	"??????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceArithmetic,	"RRArithmetic",	"rrarithmetic",	"????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )	
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceStopWatch,	"RRStopWatch",	"rrstopwatch",	"????????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceTyping,		"RRTyping",	"rrtyping",	"????????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceCard,		"RRCard",	"rrcard",	"????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceLadder,		"RRLadder",	"rrladder",	"??????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceReqFininsh,	"RRFINISH",	"rrfinish",	"????????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )

//#ifdef __EXT_ENCHANT
//	ON_TEXTCMDFUNC( TextCmd_ChangeAttribute,		"CHATTRIBUTE",	"chattribute",	"????????", "????",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//#endif //__EXT_ENCHANT
	ON_TEXTCMDFUNC( TextCmd_HousingVisitRoom,		"HousingVisit",	"hv",	"????", "????",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_HousingGMRemoveAll,		"HousingGMRemoveAll",	"hgmra",	"????????", "????",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )
/*
#if __VER >= 14 // __SMELT_SAFETY
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyNormal,		"SmeltSafetyNormal",	"ssn",	"????????????", "??????",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyAccessary,	"SmeltSafetyAccessary",	"ssa",	"????????????", "??????",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyPiercing,	"SmeltSafetyPiercing",	"ssp",	"??????????????", "??????",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
#endif //__SMELT_SAFETY
*/
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyElement,		"SmeltSafetyElement",	"sse",	"????????????", "??????",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
#ifdef __QUIZ
	ON_TEXTCMDFUNC( TextCmd_QuizEventOpen,			"QuizEventOpen",		"qeo",		"????????", "????", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizEventEnterance,		"QuizEventEnterance",	"qee",		"????????", "????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizStateNext,			"QuizStateNext",		"qsn",		"????????", "????", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizEventClose,			"QuizEventClose",		"qec",		"????????", "????", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif // __QUIZ

	ON_TEXTCMDFUNC( TextCmd_BuyGuildHouse,			"BuyGuildHouse",		"bgh",		"??????????????",	"??????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GuildHouseUpkeep,		"GuildHouseUpkeep",		"ghu",		"????????????????",	"??????", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_RemoveCampusMember,		"RemoveCampusMember",	"rcm",		"????????",		"????",		TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_UpdateCampusPoint,		"UpdateCampusPoint",	"ucp",		"????????????",	"??????",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_InvenRemove,            "InvenRemove",         "irm",       "????????",       "????",    TCM_SERVER, AUTH_ADMINISTRATOR, "" )

}

BOOL CmdFunc::AllCommands::ParseCommand(LPCTSTR lpszString, CPlayer_ * pMover, BOOL bItem) {
	CScanner scanner;
	scanner.SetProg( (LPTSTR)lpszString );
	scanner.GetToken(); // skip /
	scanner.GetToken(); // get command

	CString commandName = scanner.Token;
	commandName.MakeLower();

	for (const auto & textCmdFunc : m_allCommands) {
		if (::GetLanguage() != LANG_KOR) {
			//if (textCmdFunc.m_pCommand == "open") break;
		}

		if(commandName == textCmdFunc.m_pCommand || commandName == textCmdFunc.m_pAbbreviation ||
			commandName == textCmdFunc.m_pKrCommand || commandName == textCmdFunc.m_pKrAbbreviation ) {

			if( bItem == FALSE && textCmdFunc.m_dwAuthorization > pMover->m_dwAuthorization )
				break;

		#ifdef __CLIENT
			if (textCmdFunc.m_nServer == TCM_CLIENT || textCmdFunc.m_nServer == TCM_BOTH) {
				if (textCmdFunc.Call(scanner, pMover)) {
					if (textCmdFunc.m_nServer == TCM_BOTH) {
						g_DPlay.SendChat(scanner.m_pBuf);
					}
				}
			} else {
				g_DPlay.SendChat(lpszString);
			}
		#else	// __CLIENT
			if (textCmdFunc.m_nServer == TCM_SERVER || textCmdFunc.m_nServer == TCM_BOTH) {
				textCmdFunc.Call(scanner, pMover);
			}
		#endif	// __CLIENT
			return TRUE;
		}
	}

#ifdef __CLIENT
	BOOL bSkip = FALSE;
	CString strTemp = lpszString;
	
	if( strTemp.Find( "#", 0 ) >= 0 )
		bSkip = TRUE;

	int nstrlen = strlen(lpszString);

	if( !bSkip )
	{
		TCHAR	szText[MAX_EMOTICON_STR];

		if(nstrlen < MAX_EMOTICON_STR)
			strcpy( szText, lpszString );
		else
		{
			strncpy(szText, lpszString, MAX_EMOTICON_STR);
			szText[MAX_EMOTICON_STR] = NULL;
		}

		// ???????? ????
		for( int j=0; j < MAX_EMOTICON_NUM; j++ )
		{
			if( stricmp( &(szText[1]), g_DialogMsg.m_EmiticonCmd[ j ].m_szCommand ) == 0 ) {
				g_DPlay.SendChat( (LPCSTR)lpszString );
				return TRUE;
			}
		}
	}
#endif	//__CLIENT

	return FALSE;
}

void RemoveCRLF( char* szString )
{
	CString str		= szString;
	str.Replace( "\\n", " " );
	lstrcpy( szString, (LPCSTR)str );
}

void ParsingEffect( TCHAR* pChar, int nLen )
{
	CString strTemp;
	
	for( int i = 0; i < nLen; i++ )
	{
		if( pChar[ i ] == '#' ) // ???? ????
		{
			if( ++i >= nLen )
				break;
			switch( pChar[ i ] )
			{
			case 'c':
				{
					if( ++i >= nLen )
						break;
					
					i += 7;
				}
				break;
			case 'u':
				break;
			case 'b':
				break;
			case 's':
				break;
				
			case 'l':
				{
					if(++i >= nLen)
						break;
					
					i += 3;
				}				
				break;
			case 'n':
				if( ++i >= nLen )
					break;
				
				{					
					switch( pChar[ i ] )
					{
					case 'c':
						break;
					case 'b':
						break;
					case 'u':
						break;
					case 's':
						break;
					case 'l':
						break;
					}
				}
				break;
			default: // ?????????? ???? ?????? ???? 
				{
					// #?????? ????????
					strTemp += pChar[ i - 1 ];
					strTemp += pChar[ i ];
				}
				break;
			}
		}
		else
		{
			if( pChar[ i ] == '\\' && pChar[ i+1 ] == 'n' )
			{
				strTemp += '\n';
				i+=1;
			}
			else
			{
				strTemp += pChar[ i ];
				int nlength = strTemp.GetLength();
			}
		}
	}
	
//	memcpy( pChar, strTemp, sizeof(TCHAR)*nLen );
	strcpy( pChar, strTemp );
}
