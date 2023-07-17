#include "stdafx.h"
#include "AIInterface.h"
#include "DefineSkill.h"

#ifndef __CLIENT
#include "User.h"
#endif	// __CLIENT


BEGIN_AISTATE_MAP( CAIInterface, CAIInterface )
END_AISTATE_MAP()

CAIInterface::CAIInterface()
{
	m_pObj = NULL;
	m_pStateFunc = NULL;
	m_uParty	= 0;
}
CAIInterface::CAIInterface( CObj* pObj ) 
{ 
	m_pObj = pObj;
	m_pStateFunc = NULL;
	m_uParty	= 0;
}

CAIInterface::~CAIInterface()
{
}



FUNCAISTATE CAIInterface::GetAIStateFunc( DWORD dwState)
{
	const STATEMAP* lpCurrentStateMap = GetStateMap();
	const STATEMAP_ENTRIES* lpEntries = lpCurrentStateMap->lpStateMapEntries;
	// 하부 객체부터 일치하는 매시지를 찾는다. 
	while( lpCurrentStateMap->lpBaseStateMap->lpStateMapEntries != lpEntries )
	{
		int i = 0;
		while(  lpEntries[ i ].pStateFunc )
		{
			if( lpEntries[ i ].dwAIStateId == dwState )
			{
				return lpEntries[ i ].pStateFunc;
			}
			i++;
		}
		// 발견되지 않았다! 상위 객체로 포인터를 바꾼다.
		lpCurrentStateMap = lpCurrentStateMap->lpBaseStateMap;
		lpEntries = lpCurrentStateMap->lpStateMapEntries;
	}
	return NULL;
}


void CAIInterface::RouteMessage()
{
	AIMSG msg;

	while( !m_MsgQueue.empty() )
	{
		msg	= m_MsgQueue.front();
		m_MsgQueue.pop();
		if( msg.dwMessage == AIMSG_SETSTATE )
		{
			msg.dwMessage	= AIMSG_INIT;
			m_pStateFunc	= GetAIStateFunc( msg.dwParam1 );
		}
		if( m_pStateFunc )
			( this->*( m_pStateFunc ) ) ( msg );
	}
	if( m_pStateFunc )
	{
		msg.dwMessage	= AIMSG_PROCESS;
		msg.dwParam1	= 0;
		msg.dwParam2	= 0;
		( this->*( m_pStateFunc ) ) ( msg );
	}
}

void CAIInterface::SendAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
{
	AIMSG msg;
	msg.dwMessage	= dwMessage;
	msg.dwParam1	= dwParam1;
	msg.dwParam2	= dwParam2;
	if( dwMessage == AIMSG_SETSTATE )
	{
		msg.dwMessage	= AIMSG_INIT;
		m_pStateFunc	= GetAIStateFunc( dwParam1 );
	}
	if( m_pStateFunc )
		( this->*( m_pStateFunc ) ) ( msg );
}

void CAIInterface::PostAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
{
	switch( dwMessage )
	{
		case AIMSG_ARRIVAL:
			{
				AIMSG msg;
				msg.dwMessage	= dwMessage;
				msg.dwParam1	= dwParam1;
				msg.dwParam2	= dwParam2;
				m_MsgQueue.push( msg );
				break;
			}
		default:	SendAIMsg( dwMessage, dwParam1, dwParam2 );	break;
	}
}


CMover* CAIInterface::ScanTargetNext( CObj* pObjCenter, int nRange, OBJID dwIdTarget, u_long uParty )
{
	// 타겟이 없어졌을 때, 공격 조건을 만족하는 주변에 같은 파티원을 검색한다.
	// 기존 타겟이 파티가 없었거나, 가드면 무시
	if( uParty == 0 || nRange <= 0 || GetMover()->GetProp()->dwClass == RANK_GUARD )
		return NULL;

	TRACE( "ScanTargetNext\n" );
	D3DXVECTOR3 vPos	= pObjCenter->GetPos();
	D3DXVECTOR3 vDist;

	BOOL bFlyMob	= GetMover()->IsFlyingNPC();
	float fRadius	= (float)( nRange * nRange );

	for (CObj * pObj : 
		LinkMapRange(pObjCenter->GetWorld(), vPos, nRange, LinkType::Player, GetMover()->GetLayer())
		)	{
		CMover * pTarget	= (CMover *)pObj;
		if( pTarget->IsLive() && pTarget->GetId() != dwIdTarget ) 
		{
			if( bFlyMob == pTarget->IsFly() || bFlyMob == FALSE  )
			{
				vDist	= pTarget->GetPos() - vPos;				
				if( D3DXVec3LengthSq( &vDist ) < fRadius && pTarget->m_idparty == uParty )
				{
					if( pTarget->IsMode( TRANSPARENT_MODE ) == FALSE )
						if( !pTarget->HasBuffByIk3( IK3_TEXT_DISGUISE ) )
							return pTarget;
				}
			}
		}
	}

	return NULL;
}

// pObjCenter를 중심으로 nRangeMeter반경내에 들어오는넘들중 가장먼저 걸리는 넘 골라냄/
// nJobCond : 해당 직업만 감시함. 직업검사 안함.
CMover* CAIInterface::ScanTarget( CObj* pObjCenter, int nRangeMeter, int nJobCond, QuestId dwQuest, DWORD dwItem, int nChao )
{
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;

	bool bCondScan = nJobCond || dwQuest || dwItem || nChao;
	if (!bCondScan) return nullptr;

	const bool bGuard = ( GetMover()->GetProp()->dwClass == RANK_GUARD );
	BOOL bFlyMob = GetMover()->IsFlyingNPC();

	float fRadius = (float)( nRangeMeter * nRangeMeter );	// 거리 

	for (CUser * pTarget : LinkMapRange(pObjCenter->GetWorld(), vPos, nRangeMeter, LinkType::Player, GetMover()->GetLayer()))
	{
		if (!pTarget->IsLive()) continue;

		// 조건 스켄이라면 프랍과 일치하는 놈만 가능하다.
		// 
		// 
		// 1. 직업 조건 체크    
		if (nJobCond != 0 && nJobCond != JOB_ALL && pTarget->GetJob() != nJobCond)		// JOB_ALL : 모든직업
			continue;

		// 2. 아이템 조건 체크  
		if (dwItem != 0 && pTarget->GetItemNum(dwItem) == 0)
			continue;

		// 3. 퀘스트 조건 체크 
		if (dwQuest && pTarget->GetQuest(dwQuest) == nullptr)
			continue;

		//4. 카오 조건 체크  
		if( nChao != 0 ) {
			// 100 = 카오유저, 101 = 비카오
			if( nChao == 100 && pTarget->IsChaotic()) {
				// ok
			} else if( nChao == 101 && !pTarget->IsChaotic()) {
				// ok
			} else {
				continue;
			}
		}
		
		// 가드는 무조건 공격			
		// 비행몹은 비행플레이어만 공격, 지상몹은 지상플레이어만만 공격 - 1. true true  2. false false

		if (pTarget->IsMode(TRANSPARENT_MODE)) continue;
		if (pTarget->HasBuffByIk3(IK3_TEXT_DISGUISE)) continue;

		if( bGuard || bFlyMob == pTarget->IsFly() )		
		{
			const D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
			if( D3DXVec3LengthSq( &vDist ) < fRadius )	// 두 객체간의 거리가 범위 이내이면 
			{
				return pTarget;
			}
		}
		
	}

	return nullptr;
}

// 반경내에서 가장 강한넘(레벨로)을 골라냄.
CMover* CAIInterface::ScanTargetStrong( CObj* pObjCenter, FLOAT fRangeMeter  )
{
	const D3DXVECTOR3 vPos = pObjCenter->GetPos();
	
	// 지름 
	const FLOAT fRadius = fRangeMeter * fRangeMeter;
	

	CMover * pObjMax = NULL;	// 가장쎈넘 포인터.
	for( CMover * pObj : LinkMapRange(pObjCenter->GetWorld(), vPos, (int)( fRangeMeter / MPU ), LinkType::Player, GetMover()->GetLayer()) )
	{
		const D3DXVECTOR3 vDist = pObj->GetPos() - vPos;	// 두좌표간 벡터
		const float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
		if( fDistSq < fRadius )	
		{
			if (!pObj->IsMode(TRANSPARENT_MODE)) {
				if (!pObjMax || pObj->GetLevel() > pObjMax->GetLevel()) {
					pObjMax = pObj;
				}

			}
		}
	}

	return pObjMax;
}

// 반경내에서 오버힐 하는 어시 찾아서 죽이자.
CMover* CAIInterface::ScanTargetOverHealer( CObj* pObjCenter, FLOAT fRangeMeter  )
{
#ifndef __CLIENT
	const D3DXVECTOR3 vPos = pObjCenter->GetPos();
	
	// 지름 
	const FLOAT fRadius = fRangeMeter * fRangeMeter;
	
	for (CUser * pMover : LinkMapRange(pObjCenter->GetWorld(), vPos, (int)( fRangeMeter / MPU ), LinkType::Player, GetMover()->GetLayer() ))
	{
		const D3DXVECTOR3 vDist = pMover->GetPos() - vPos;	// 두좌표간 벡터
		float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
		if( fDistSq < fRadius )	
		{
			if(  ! pMover->IsMode( TRANSPARENT_MODE ) )
			{

					if( pMover->m_nOverHeal > 0 )		// 오버힐러를 찾았다.
						return pMover;
			}
		}
	}

#endif	// __CLIENT
	return NULL;
}


