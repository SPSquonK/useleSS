#include "stdafx.H"


#include "couplehelper.h"
#include "user.h"
#include "dpdatabaseclient.h"
#include "dpsrvr.h"
#include "playerdata.h"
#include "definetext.h"
#include "lord.h"
#include "defineobj.h"
extern	CUserMng	g_UserMng;

	#include "honor.h"

CCoupleHelper::CCoupleHelper()
{
}

CCoupleHelper::~CCoupleHelper()
{
	Clear();
}

void CCoupleHelper::Clear()
{
	SAFE_DELETE( m_pMgr );
}

CCoupleHelper* CCoupleHelper::Instance()
{
	static CCoupleHelper sCoupleHelper;
	return &sCoupleHelper;
}

BOOL CCoupleHelper::Initialize( CDPDatabaseClient* pdpClient, CDPSrvr* pdpServer )
{
	m_pMgr	= new CCoupleMgr;
	m_pdpClient	= pdpClient;
	m_pdpServer	= pdpServer;
	return CCoupleProperty::Instance()->Initialize();
}

void CCoupleHelper::OnPropose( CUser* pUser, const char* szPlayer )
{
	election::OutputDebugString( "S: CCoupleHelper.OnPropose: %s", szPlayer );
	u_long idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( const_cast<char*>( szPlayer )  );
	if( idPlayer == 0 )
	{
		pUser->AddDefinedText( TID_GAME_COUPLE_E00, "%s", szPlayer );	//%s님을 찾을 수 없습니다.
		return;
	}
	CUser* pTarget	= static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
	if( !IsValidObj( pTarget ) )
	{
		pUser->AddDefinedText( TID_GAME_COUPLE_E00 );	//%s님을 찾을 수 없습니다.
		return;
	}
	if( pUser == pTarget )
	{
		pUser->AddDefinedText( TID_GAME_COUPLE_E01 );	// 대상이 부적합 합니다. 커플이 아닌 이성 캐릭터에게만 가능합니다.
		return;
	}
	if( pUser->GetSex() == pTarget->GetSex() )
	{
		pUser->AddDefinedText( TID_GAME_COUPLE_E01 );	// 대상이 부적합 합니다. 커플이 아닌 이성 캐릭터에게만 가능합니다.
		return;
	}
	m_pdpClient->SendPropose( pUser->m_idPlayer, pTarget->m_idPlayer );
}

void CCoupleHelper::PlayProposeAnimation( CUser* pProposer, CUser* pTarget )
{
	ASSERT( pProposer );
	ASSERT( pTarget );
	FLOAT fAngle	= GetDegree( pTarget->GetPos(), pProposer->GetPos());
	pProposer->SetAngle( fAngle );

	pProposer->SendActMsg( OBJMSG_MOTION, MTI_CHEEROTHER, ANILOOP_1PLAY );
	g_UserMng.AddCreateSfxObj( pProposer, XI_CHEERSENDEFFECT );
	g_UserMng.AddCreateSfxObj( pTarget, XI_CHEERRECEIVEEFFECT );
	g_UserMng.AddMoverBehavior( pProposer, pProposer->GetPos(), pProposer->m_pActMover->m_vDelta,
		pProposer->GetAngle(), pProposer->m_pActMover->GetState(), pProposer->m_pActMover->GetStateFlag(), 
		pProposer->m_dwMotion, pProposer->m_pActMover->m_nMotionEx, pProposer->m_pModel->m_nLoop, pProposer->m_dwMotionOption, g_TickCount.GetTickCount(), TRUE );
}

void CCoupleHelper::OnProposeResult( CAr & ar )
{
	u_long idProposer, idTarget;
	int nResult;
	time_t t;
	ar >> idProposer >> idTarget >> nResult >> t;
	election::OutputDebugString( "S: CCoupleHelper.OnProposeResult: %d, %d, %d, %d", idProposer, idTarget, nResult, t );

	CUser* pProposer	= static_cast<CUser*>( prj.GetUserByID( idProposer ) );
	if( !IsValidObj( pProposer ) )
		return;

	if( nResult == 0 )
	{
		CUser* pTarget	= static_cast<CUser*>( prj.GetUserByID( idTarget ) );
		if( IsValidObj( pTarget ) )
		{
			pTarget->SetProposer( idProposer );
			pTarget->AddProposeResult( idProposer, pProposer->GetName() );
			PlayProposeAnimation( pProposer, pTarget );
		}
		const char* pszTarget	= CPlayerDataCenter::GetInstance()->GetPlayerString( idTarget );	
		if( !pszTarget )
			pszTarget	= "";
		pProposer->AddDefinedText( TID_GAME_COUPLE_S01, "%s", pszTarget );	// %s님에게 프러포즈 하였습니다.
	}
	else
	{
		if( t > 0 )
		{
			CTimeSpan ts( t );
			char szText[200]	= { 0,};
			// 프러포즈는 %d일 %d시간 %d분 후에 사용 가능합니다.
			sprintf( szText, prj.GetText( TID_GAME_COUPLE_M00 ), static_cast<int>( ts.GetDays() ), ts.GetHours(), ts.GetMinutes() );
			pProposer->AddText( szText );
		}
		else
			pProposer->AddDefinedText( nResult );
	}
}

void CCoupleHelper::OnRefuse( CUser* pUser )
{
	election::OutputDebugString( "S: CCoupleHelper.OnRefuse" );
	if( pUser->GetProposer() == 0 )
	{
		pUser->AddDefinedText( TID_GAME_COUPLE_E02 );	// 먼저 프러포즈를 받아야 합니다.
		return;
	}
	CUser* pProposer	= static_cast<CUser*>( prj.GetUserByID( pUser->GetProposer() ) );
	if( IsValidObj( pProposer ) )
		pProposer->AddDefinedText( TID_GAME_COUPLE_E03 );	// 상대방이 프러포즈를 거절 하였습니다. 
	const char* pszProposer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pUser->GetProposer() );
	if( pszProposer )
		pUser->AddDefinedText( TID_GAME_COUPLE_E04, "%s", pszProposer );	// %s님의 프러포즈를 거절 하였습니다.
	pUser->SetProposer( 0 );
}

void CCoupleHelper::OnCouple( CUser* pUser )
{
	election::OutputDebugString( "S: CCoupleHelper.OnCouple" );
	if( pUser->GetProposer() == 0 )
	{
		pUser->AddDefinedText( TID_GAME_COUPLE_E02 );	// 먼저 프러포즈를 받아야 합니다.
		return;
	}
	m_pdpClient->SendCouple( pUser->GetProposer(), pUser->m_idPlayer );
}

void CCoupleHelper::OnCoupleResult( CAr & ar )
{
	u_long idProposer, idTarget;
	int nResult;
	ar >> idProposer >> idTarget >> nResult;
	election::OutputDebugString( "S: CCoupleHelper.OnCoupleResult: %d, %d, %d", idProposer, idTarget, nResult );
	CUser* pProposer	= static_cast<CUser*>( prj.GetUserByID( idProposer ) );
	CUser* pTarget	= static_cast<CUser*>( prj.GetUserByID( idTarget ) );
	if( nResult == 0 )
	{
		m_pMgr->Couple( idProposer, idTarget );
		const char* pszProposer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idProposer );
		const char* pszTarget	= CPlayerDataCenter::GetInstance()->GetPlayerString( idTarget );
		if( !pszProposer )	pszProposer	= "";
		if( !pszTarget )	pszTarget	= "";
		if( IsValidObj( pProposer ) )
		{
			pProposer->AddCoupleResult( idTarget, pszTarget );
			pProposer->SetHonorAdd(	HS_COUPLE_COUNT,HI_COUNT_CHECK );
		}
		if( IsValidObj( pTarget ) )
		{
			pTarget->AddCoupleResult( idProposer, pszProposer );
			pTarget->SetHonorAdd(HS_COUPLE_COUNT,HI_COUNT_CHECK );
		}
	}
	else
	{
		if( IsValidObj( pTarget ) )
			pTarget->AddDefinedText( nResult );
	}
}

void CCoupleHelper::OnDecouple( CUser* pUser )
{
	election::OutputDebugString( "S: CCoupleHelper.OnDecouple" );
	CCouple* pCouple	= m_pMgr->GetCouple( pUser->m_idPlayer );
	if( !pCouple )
	{
		pUser->AddDefinedText( TID_GAME_COUPLE_E05 );	// 커플이 아닙니다.
		return;
	}
	m_pdpClient->SendDecouple( pUser->m_idPlayer );
}

void CCoupleHelper::OnDecoupleResult( CAr & ar )
{
	u_long idPlayer;
	int nResult;
	ar >> idPlayer >> nResult;
	election::OutputDebugString( "S: CCoupleHelper.OnDecoupleResult: %d, %d", idPlayer, nResult );
	CUser* pUser	= static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
	if( nResult == 0 )
	{
		CCouple* pCouple	= m_pMgr->GetCouple( idPlayer );
		ASSERT( pCouple );
		if( !pCouple )
			return;
		CUser* pPartner	= static_cast<CUser*>( prj.GetUserByID( pCouple->GetPartner( idPlayer ) ) );
		m_pMgr->Decouple( idPlayer );
		if( IsValidObj( pUser ) )
		{
			pUser->AddDecoupleResult();
			int nIdx = CTitleManager::Instance()->GetIdx(HS_COUPLE_LV,HI_COUNT_CHECK);
			if( nIdx >= 0)
			{	
				int nPrevLvl = pUser->GetHonorTitle(nIdx);
				pUser->SetHonorCount(nIdx,0);
				int nNeedCount = CTitleManager::Instance()->GetNeedCount(HS_COUPLE_LV,HI_COUNT_CHECK);
				if( nNeedCount >= nPrevLvl )
					pUser->AddHonorListAck();
			}
		}
		if( IsValidObj( pPartner ) )
		{
			pPartner->AddDecoupleResult();
			int nIdx = CTitleManager::Instance()->GetIdx(HS_COUPLE_LV,HI_COUNT_CHECK);
			if( nIdx >= 0)
			{	
				int nPrevLvl = pPartner->GetHonorTitle(nIdx);
				pPartner->SetHonorCount(nIdx,0);
				int nNeedCount = CTitleManager::Instance()->GetNeedCount(HS_COUPLE_LV,HI_COUNT_CHECK);
				if( nNeedCount >= nPrevLvl )
					pPartner->AddHonorListAck();
			}
		}
	}
	else
	{
		if( IsValidObj( pUser ) )
			pUser->AddDefinedText( nResult );
	}
}

void CCoupleHelper::OnAddCoupleExperience( CAr & ar )
{
	u_long idPlayer;
	int nExperience;
	ar >> idPlayer >> nExperience;
	CCouple* pCouple	= GetCouple( idPlayer );
	if( pCouple )
	{
		int nPreCouplelvl =	pCouple->GetLevel();
		pCouple->AddExperience( nExperience );
		CUser* pFirst	= static_cast<CUser*>( prj.GetUserByID( pCouple->GetFirst() ) );
		CUser* pSecond	= static_cast<CUser*>( prj.GetUserByID( pCouple->GetSecond() ) );
		if( IsValidObj( pFirst ) )
		{
			pFirst->RemoveIk3Buffs( IK3_COUPLE_BUFF );
			pFirst->AddAddCoupleExperience( nExperience );
			if(nPreCouplelvl < pCouple->GetLevel())
				pFirst->SetHonorAdd(HS_COUPLE_LV,HI_COUNT_CHECK,pCouple->GetLevel());
		}
		if( IsValidObj( pSecond ) )
		{
			pSecond->RemoveIk3Buffs( IK3_COUPLE_BUFF );
			pSecond->AddAddCoupleExperience( nExperience );
			if(nPreCouplelvl < pCouple->GetLevel())
				pSecond->SetHonorAdd(HS_COUPLE_LV,HI_COUNT_CHECK,pCouple->GetLevel());
		}
	}
}

void CCoupleHelper::OnUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	CCouple* pCouple	= GetCouple( idPlayer );
	if( pCouple )
	{
		u_long idPartner	= pCouple->GetPartner( idPlayer );
		CUser* pPartner		= g_UserMng.GetUserByPlayerID( idPartner );
		if( IsValidObj( pPartner ) )
			pPartner->AddQueryPlayerData( idPlayer, pPlayerData );
	}
}
