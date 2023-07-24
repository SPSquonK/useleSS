#include "stdafx.h"
#include "defineText.h"
#include "defineItem.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpsrvr.h"
#include "user.h"
#include "worldmng.h"
#include "misc.h"
#include "guild.h"
#include "..\_Common\Ship.h"
#include "..\_aiinterface\AIPet.h"
#include "Party.h"
#include "..\_Network\ErrorCode.h"


void CDPSrvr::OnUseSkill(CAr & ar, CUser & pUser) {
	DWORD skillId;
	OBJID objid;
	int	 nUseType = 0;

	ar >> skillId >> objid >> nUseType;
	BOOL bControl;
	ar >> bControl;

	if (pUser.m_vtInfo.VendorIsVendor()) return;

#ifdef __S_SERVER_UNIFY
	if (pUser.m_bAllAction == FALSE) return;
#endif // __S_SERVER_UNIFY

	SKILL * pSkill = pUser.GetSkill(skillId);
	if (!pSkill) return;

	ItemProp * pSkillProp = prj.GetSkillProp(skillId);
	if (!pSkillProp) return;

	DWORD dwLevel = pSkill->dwLevel;

	if (dwLevel == 0 || dwLevel > pSkillProp->dwExpertMax) return;

	const BOOL fSuccess = pUser.DoUseSkillPre(skillId, objid, (SKILLUSETYPE)nUseType, bControl);

	if (fSuccess == TRUE)	// 스킬사용에 성공했고
	{
		if (nUseType == SUT_QUEUESTART)	// 스킬큐로 실행하라고 한거였다.
		{
			pUser.m_nUsedSkillQueue = 0;		// 스킬큐 실행중인 표시 남김.
		}
	}
	else	// 서버에서 UseSkill을 실패하면 그것을 그 클라한테 알려줘야 한다.
	{
		TRACE("Fail %d, ", skillId);
		pUser.AddHdr(pUser.GetId(), SNAPSHOTTYPE_CLEAR_USESKILL);
	}
}
/*
void CDPSrvr::OnDoCollect( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
}
*/
// 클라이언트로 부터 받은 에러코드를 로그로 남긴다.
void CDPSrvr::OnError( CAr & ar, DPID dpidCache, DPID dpidUser)
{
	int		nCode, nData;
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		ar >> nCode >> nData;
		switch( nCode )
		{
		case FE_GENERAL:
			break;
		case FE_INVALIDATTACKER:	// 클라이언트에서 어태커가 invalid한경우(유령몹 버그)
			{
#ifndef _DEBUG
				OBJID idAttacker = (OBJID)nData;
				CMover *pAttacker = prj.GetMover( idAttacker );
				if( IsValidObj(pAttacker) )
				{
					Error( "2OnError : FE_INVALIDATTACKER 피해자:%s(%f,%f,%f), 공격자:%s(%f,%f,%f)", pUser->GetName(), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z,
						pAttacker->GetName(), pAttacker->GetPos().x, pAttacker->GetPos().y, pAttacker->GetPos().z );
					pUser->AddCorrReq( pAttacker );	// 요청한 클라에게 인밸리드한 어태커를 다시 보내줌.
				}
				else
					Error( "2OnError : FE_INVALIDATTACKER 피해자:%s(%f,%f,%f) 이런젠장 서버에서도 pAttacker는 Invalid다. 0x%08x", 
					pUser->GetName(), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z,
					nData );
				
#endif // not debug
				//		case NEXT:
				
			}
			break;
		}
	} else
		Error( "CDPSrvr::OnError pUser - Invalid %d %d", dpidCache, dpidUser );
}

void CDPSrvr::OnShipActMsg( CAr & ar, CUser * pUser )
{
	DWORD dwMsg;
	OBJID idShip;
	int nParam1, nParam2;
	ar >> dwMsg >> nParam1 >> nParam2 >> idShip;
	
		if( IsInvalidObj( pUser->GetIAObjLink() ) )		return;
		if( idShip != pUser->GetIAObjLink()->GetId() )
		{
			Error( "OnShipActMsg : 클라에서 보내온 아이디(%d)와 서버에서의 아이디(%d)가 다르다", idShip, pUser->GetIAObjLink()->GetId() );
			return;
		}
		
		CShip *pShip = prj.GetShip( idShip );
		if( IsValidObj( pShip ) )
		{
			pShip->SendActMsg( (OBJMSG)dwMsg, nParam1, nParam2, 0 );
			//			g_DPCoreClient.SendShipActMsg( pUser, dwMsg, nParam1, nParam2 );
			g_UserMng.AddShipActMsg( pUser, pShip, dwMsg, nParam1, nParam2 );
		}
	
}

void CDPSrvr::OnLocalPosFromIA( CAr & ar, CUser * pUser )
{
	D3DXVECTOR3 vLocal;
	OBJID		idIA;
	ar >> vLocal >> idIA;
	
		// 클라이언트로부터 착지한지점의 상대좌표를 받았다.
		// 이 좌표를 서버에서 동기화 하자
		CShip *pIA = prj.GetShip( idIA );
		if( IsInvalidObj( pIA ) )	return;

		
		D3DXVECTOR3 vPos = pIA->GetPos() + vLocal;		// 서버상에서의 IA오브젝트와 클라에서 받은 로컬좌표를 합쳐서 새로운 좌표생성
		pUser->SetPos( vPos );
		pUser->SetIAObjLink( pIA );
	
}