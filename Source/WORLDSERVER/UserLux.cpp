#include "stdafx.h"
#include "defineText.h"
#include "User.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#include "WorldMng.h"

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#include "UserMacro.h"

#ifdef __XUZHU
int g_nCC = 0;
#endif

// tmMaxEscape : 탈출의 쿨타임
void CUser::AddEscape(DWORD tmMaxEscape) {
	SendSnapshotNoTarget<SNAPSHOTTYPE_DO_ESCAPE, DWORD>(tmMaxEscape);
}

// 액션포인트 갱신.
void CUser::AddSetActionPoint(int nAP) {
	SendSnapshotNoTarget<SNAPSHOTTYPE_SETACTIONPOINT, int>(nAP);
}

void CUser::AddSetTarget(OBJID idTarget) {
	SendSnapshotNoTarget<SNAPSHOTTYPE_SETTARGET, OBJID>(idTarget);
}

// ----------------------------------------------------------------------------------
//
//   CUserMng Lux
//
// ----------------------------------------------------------------------------------


void	CUserMng::AddCreateSkillEffect(CMover * pAttacker, OBJID idTarget, DWORD dwSkill, DWORD dwLevel) {
	BroadcastAround<SNAPSHOTTYPE_ACTIVESKILL, OBJID, DWORD, DWORD>(pAttacker,
		idTarget, dwSkill, dwLevel
	);
}

// pMover가 스턴 된것을 주위 유저클라에 알림.
void	CUserMng::AddSetStun( CMover *pMover, int nMaxTime ) {
	BroadcastAround<SNAPSHOTTYPE_SETSTUN, int>(pMover, nMaxTime);
}

void	CUserMng::AddDoApplySkill(CCtrl * pCtrl, OBJID idTarget, DWORD dwSkill, DWORD dwLevel)
{
	BroadcastAround<SNAPSHOTTYPE_DOAPPLYUSESKILL, OBJID, DWORD, DWORD>(pCtrl,
		idTarget, dwSkill, dwLevel
	);
}

// SendActMsg를 주위 유저들에게 보냄 .
void	CUserMng::AddSendActMsg(CMover * pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3)
{
	BroadcastAround<SNAPSHOTTYPE_SENDACTMSG, int, BOOL, int, int, int>(pMover,
		dwMsg, pMover->IsFly(), nParam1, nParam2, nParam3
	);
}

// Mover를 어느방향으로 밀리게 하다.
// pMover : 밀리는 무버
// vPos : 밀리는 시점에서의 vPos - 정확한 동기화때문에 좌표와 각도까지 필요하다.
// fAngle : 밀리는 시점에서의 Angle - 
// fPushAngle : 미는 각도.
// fPower : 미는 힘.
void	CUserMng::AddPushPower( CMover *pMover, D3DXVECTOR3 vPos, FLOAT fAngle, FLOAT fPushAngle, FLOAT fPower )
{
	if( (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) || pMover->m_pActMover->GetState() & OBJSTA_STUN ) // 데미지 플라이중이면 리턴
		return;

	BroadcastAround<SNAPSHOTTYPE_PUSHPOWER, D3DXVECTOR3, FLOAT, FLOAT, FLOAT>(pMover,
		vPos, fAngle, fPushAngle, fPower
	);
}

void	CUserMng::AddRemoveSkillInfluence(CMover * pMover, WORD wType, WORD wID) {
	BroadcastAround<SNAPSHOTTYPE_REMOVESKILLINFULENCE, WORD, WORD>(pMover, wType, wID);
}

void CUserMng::AddSetPosAngle(CCtrl * pCtrl, const D3DXVECTOR3 & vPos, FLOAT fAngle) {
	BroadcastAround<SNAPSHOTTYPE_SETPOSANGLE, D3DXVECTOR3, FLOAT>(pCtrl, vPos, fAngle);
}

