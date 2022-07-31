#include "stdafx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "defineItem.h"
#include "party.h"
#include "sqktd.h"

#define II_SYS_SYS_SCR_PARTYEXPUP01_01 20296
#define II_SYS_SYS_SCR_PARTYSKILLUP01_01 20297

#ifdef __CORESERVER
#include "dpcoresrvr.h"
#include "player.h"
#endif // __CORESERVER

#ifdef __WORLDSERVER
#include "worldmng.h"
#include "User.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "slord.h"
#include "GroupUtils.h"
#endif	// __WORLDSERVER

#include "playerdata.h"

#ifndef __VM_0820
#ifndef __MEM_TRACE
	MemPooler<CParty>*	CParty::m_pPool		= new MemPooler<CParty>( 512 );
#endif	// __MEM_TRACE
#endif	// __VM_0820

#include "InstanceDungeonParty.h"
/*--------------------------------------------------------------------------------*/

CParty::CParty() {
	memset( m_sParty, 0, sizeof(m_sParty) );
	for( int i = 0 ; i < MAX_PARTYMODE ; i++ )
	{
		m_nModeTime[i] = 0;
	}
}

void CParty::InitParty()
{
	m_uPartyId	= 0;
	m_nSizeofMember	= 0;
	memset( m_sParty, 0, sizeof(m_sParty) );
	m_nLevel = 1;
	m_nExp = m_nPoint = 0;
	m_nTroupsShareExp = ShareExpMode::Level;
	m_nTroupeShareItem = ShareItemMode::Self;
	m_nKindTroup = 0;							// 단막극단
	m_nReferens = 0;	
	for (int i = 0; i < MAX_PTMEMBER_SIZE; i++) {
		m_aMember[i] = PartyMember();
	}
}

#ifndef __CORESERVER
CMover * CParty::GetLeader() 
{ 
	return prj.GetUserByID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
}
#endif // __CORESERVER

int CParty::FindMember( u_long uPlayerId )
{
	for( int i = 0; i < m_nSizeofMember; i++ )
		if( m_aMember[i].m_uPlayerId == uPlayerId )
			return i;
	return -1;
}

BOOL CParty::NewMember( u_long uPlayerId )
{
	
	if( IsMember( uPlayerId ) == FALSE && m_nSizeofMember < MAX_PTMEMBER_SIZE )
	{
		m_aMember[m_nSizeofMember] = PartyMember(uPlayerId);
		m_nSizeofMember++;
		return TRUE;
	}
	return FALSE;
}

BOOL CParty::DeleteMember( u_long uPlayerId )
{
	int Findid = FindMember( uPlayerId );
	if( Findid >= 0)
	{
		for( int i = Findid ; i < m_nSizeofMember-1 ; i++ )
		{
			memcpy( &m_aMember[i], &m_aMember[i+1], sizeof(PartyMember) );
		}
		m_nSizeofMember--;
#ifdef __WORLDSERVER
		CInstanceDungeonParty::GetInstance()->SetPartyLeaveTime( uPlayerId );
#endif // __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

void CParty::ChangeLeader( u_long uLeaderId )
{
	int changeIndex = FindMember( uLeaderId );
	SwapPartyMember( 0, changeIndex );
}

CAr & operator<<(CAr & ar, const CParty & self) {
	ar << self.m_uPartyId << self.m_nKindTroup << self.m_nSizeofMember;
	ar << self.m_nLevel << self.m_nExp << self.m_nPoint;
	ar << self.m_nTroupsShareExp << self.m_nTroupeShareItem;
	ar << self.m_idDuelParty;
	for (int i = 0; i < MAX_PARTYMODE; i++) {
		ar << self.m_nModeTime[i];
	}
	
	if (self.m_nKindTroup) ar.WriteString(self.m_sParty);

	for (int i = 0; i < self.m_nSizeofMember; i++) {
		ar << self.m_aMember[i].m_uPlayerId;

#ifdef __CORESERVER
		ar << self.m_aMember->m_remove.has_value();
#else
		static_assert(std::is_same_v<decltype(PartyMember::m_remove), bool>);
		ar << self.m_aMember[i].m_remove;
#endif
	}
	return ar;
}

#ifndef __CORESERVER
CAr & operator>>(CAr & ar, CParty & self) {
	ar >> self.m_uPartyId >> self.m_nKindTroup >> self.m_nSizeofMember;
	ar >> self.m_nLevel >> self.m_nExp >> self.m_nPoint;
	ar >> self.m_nTroupsShareExp >> self.m_nTroupeShareItem;
	ar >> self.m_idDuelParty;
	for( int i = 0 ; i < MAX_PARTYMODE  ; i++ )
	{
		ar >> self.m_nModeTime[i];
	}

	if(self.m_nKindTroup ) ar.ReadString(self.m_sParty );

	for (int i = 0; i < self.m_nSizeofMember; i++) {
		ar >> self.m_aMember[i].m_uPlayerId;
		static_assert(std::is_same_v<decltype(PartyMember::m_remove), bool>);
		ar >> self.m_aMember[i].m_remove;
	}

	return ar;
}
#endif

void CParty::SwapPartyMember(int first, int Second) {
	std::swap(m_aMember[first], m_aMember[Second]);
}

int CParty::GetPartyModeTime( int nMode )
{
	return m_nModeTime[nMode];	
}
void CParty::SetPartyMode( int nMode, DWORD dwSkillTime ,int nCachMode )
{
	if( nCachMode == 1)
		m_nModeTime[nMode] += (int)dwSkillTime;	
	else
		m_nModeTime[nMode] = (int)dwSkillTime;	
}

#ifdef __WORLDSERVER
void CParty::SetPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp )
{
	g_DPCoreClient.SendPartyLevel( pUser, dwLevel, dwPoint, dwExp );
}
#endif // __WORLDSERVER

void CParty::GetPoint( int nTotalLevel, int nMemberSize, int nDeadLeavel )
{
#ifdef __WORLDSERVER
	if ((nTotalLevel / nMemberSize) - nDeadLeavel >= 5) return;

	if (m_nKindTroup == 0 && m_nLevel >= MAX_PARTYLEVEL) return;

	CMover* pMover = GetLeader();

	BOOL bSuperLeader = FALSE;
	if (pMover && pMover->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SUPERLEADERPARTY)) {
		bSuperLeader = TRUE;
	}

	BOOL bLeaderSMExpUp = FALSE;
	if( pMover && ( pMover->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01 ) || pMover->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP02 )
			|| pMover->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01_01 ) ) )
	{
		bLeaderSMExpUp = TRUE;
	}

	g_DPCoreClient.SendAddPartyExp( m_uPartyId, nDeadLeavel, bSuperLeader , bLeaderSMExpUp );
	
#endif // __WORLDSERVER
}

void CParty::DoUsePartySkill( u_long uPartyId, u_long uLeaderid, int nSkill )
{
#ifdef __WORLDSERVER
	if (!IsLeader(uLeaderid) || m_nKindTroup != 1) {
		// 리더가 아니거나 순회극단이 아닌경우
		return;
	}

	const ItemProp * const pItemProp = prj.GetPartySkill(nSkill);
	if (!pItemProp) return;

	CUser * pLeader = g_UserMng.GetUserByPlayerID(uLeaderid);	// 리더의 포인터

	if (GetLevel() < static_cast<int>(pItemProp->dwReqDisLV)) {
		if (IsValidObj(pLeader)) {
			pLeader->AddSendErrorParty(ERROR_NOTPARTYSKILL);
		}
		//이 스킬은 배우지를 못했습니다.
		return;
	}

	if (!IsValidObj(pLeader)) return;

	BOOL nHasCashSkill = FALSE;

	if (pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01)
		|| pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02)
		|| pLeader->HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01)) {

		if (nSkill == ST_LINKATTACK
			|| nSkill == ST_FORTUNECIRCLE
			|| nSkill == ST_STRETCHING
			|| nSkill == ST_GIFTBOX) {
			nHasCashSkill = TRUE;
		}
	}

	DWORD dwSkillTime	= pItemProp->dwSkillTime;
	const int nRemovePoint	= pItemProp->dwExp;
	const int nFPoint	= int( GetPoint() - pItemProp->dwExp);

	// 군주의 극단
	// 군주가 극단장 으로써 극단스킬 사용 시,
	// 지속시간 4배 증가(소모 포인트는 동일)
	if( CSLord::Instance()->IsLord( uLeaderid ) )
		dwSkillTime		*= 4;

	if (nFPoint < 0) {
		pLeader->AddSendErrorParty(ERROR_NOTPARTYPOINT);
		return;
	}

	// TODO: remove capture from those lambdas

	const auto useStCall = [&]() {
		g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);

		for (CUser * pMember : AllMembers(*this)) {
			pMember->AddPartySkillCall(pLeader->GetPos());		// 각 멤버들에게 단장이 좌표를 전송함.
		}
	};

	const auto useStBlitz = [&]() {
		if (pLeader->m_idSetTarget == NULL_ID) {
			pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_BLITZ);
			return;
		}

		CMover * pT = prj.GetMover(pLeader->m_idSetTarget);
		if (!pT || pT->IsPlayer()) {
			pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_BLITZ);
			return;
		}

		g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);
		// 단장이 타겟으로 집중공격 표시
		// TODO: why PartyGiftBox and ForturneTeller? It doesn't make any sense
		const bool skipNearCheck = m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE];
		for (CUser * pMember : AllMembers(*this)) {
			if (skipNearCheck || pLeader->IsNearPC(pMember)) {
				// 각 멤버들에게 단장타겟으로 잡은 무버의 아이디를 보냄.
				pMember->AddPartySkillBlitz(pLeader->m_idSetTarget);
			}
		}
	};
	
	const auto useStRetreat = [&]() {
		g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);

		const bool skipNearCheck = m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE];

		for (CUser * pMember : AllMembers(*this)) {
			if (skipNearCheck || pLeader->IsNearPC(pMember)) {
				pMember->AddHdr(pMember->GetId(), SNAPSHOTTYPE_PARTYSKILL_RETREAT);
			}
		}
	};

	const auto useStSphereCircle = [&]() {
		if (pLeader->m_idSetTarget == NULL_ID) {
			pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_SPHERECIRCLE);
			return;
		}

		CMover * pT = prj.GetMover(pLeader->m_idSetTarget);
		if (!pT || pT->IsPlayer()) {
			pLeader->AddSendErrorParty(ERROR_NOTTARGET, ST_SPHERECIRCLE);
			return;
		}

		const bool skipNearCheck = m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE];

		g_DPCoreClient.SendRemovePartyPoint(uPartyId, pItemProp->dwExp);

		for (CUser * pMember : AllMembers(*this)) {
			if (skipNearCheck || pLeader->IsNearPC(pMember)) {
				pMember->AddHdr(pLeader->m_idSetTarget, SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE);
				pMember->m_dwFlag |= MVRF_CRITICAL;
			}
		}
	};

	const auto transmitToCore = [&](int nMode) {
		g_DPCoreClient.SendUserPartySkill(uLeaderid, nMode, dwSkillTime, nRemovePoint, nHasCashSkill);
	};

	switch (nSkill) {
		case ST_CALL:          useStCall();                              break;
		case ST_BLITZ:         useStBlitz();                             break;
		case ST_RETREAT:       useStRetreat();                           break;
		case ST_SPHERECIRCLE:  useStSphereCircle();                      break;
		case ST_LINKATTACK:    transmitToCore(PARTY_LINKATTACK_MODE);    break;
		case ST_FORTUNECIRCLE: transmitToCore(PARTY_FORTUNECIRCLE_MODE); break;
		case ST_STRETCHING:    transmitToCore(PARTY_STRETCHING_MODE);    break;
		case ST_GIFTBOX:       transmitToCore(PARTY_GIFTBOX_MODE);       break;
	}
#endif	// __WORLDSERVER
}

//
//
// idDstParty가 this파티가 결투를 시작했다는걸 모든 멤버들에게 알림.
#ifdef __WORLDSERVER
void CParty::DoDuelPartyStart( CParty *pDst )
{
	
	m_idDuelParty = pDst->m_uPartyId;		// this파티의 상대
	
	LPCSTR pszLeader;
	if( pDst->m_nKindTroup )	// 순회극단 극단 이름 보냄
	{
		pszLeader = pDst->m_sParty;
	}
	else	// 단막극단 일때는 리더이름을 보냄
	{
		pszLeader	= CPlayerDataCenter::GetInstance()->GetPlayerString( pDst->GetPlayerId( 0 ) );
	}
	
	if( !pszLeader )
	{
		Error( "Error Party PVP 1" );
		return;
	}

	for (CUser * pMember : AllMembers(*this)) {

		pMember->m_nDuel = 2;		// 2는 파티듀얼중.
		pMember->m_idDuelParty = m_idDuelParty;
		pMember->m_nDuelState = 104;
		// 상대방 파티의 멤버아이디를 다 꺼냄.

		OBJID	pDstMember[MAX_PTMEMBER_SIZE];
		memset(pDstMember, 0xff, sizeof(pDstMember));

		for(int j = 0; j < pDst->m_nSizeofMember; j ++ )
		{
			CUser * pMember2 = g_UserMng.GetUserByPlayerID( pDst->m_aMember[j].m_uPlayerId );
			if( IsValidObj(pMember2) )
				pDstMember[j] = pMember2->GetId();
		}
		// 상대 파티에 대한 정보를 우리멤버들에게 보냄
		pMember->AddDuelPartyStart( pszLeader , pDst->m_nSizeofMember, pDstMember, pDst->m_uPartyId );		// 상대방 멤버의 ID를 다보낸다.
	}
}	

//
// 파티원들에게 bWin에 따라 승/패 사실을 알리고 듀얼을 종료시킴.
//
void CParty::DoDuelResult( CParty *pDuelOther, BOOL bWin, int nAddFame, float fSubFameRatio )
{
	for (CUser * pMember : AllMembers(*this)) {
		pMember->AddDuelPartyResult( pDuelOther, bWin );		// 각 멤버들에게 승/패 사실을 알림. / 상대파티원 리스트도 보냄.
		pMember->ClearDuelParty();

		if( bWin )
		{
			pMember->m_nFame += nAddFame;	// 이긴측이면 명성 증가
			pMember->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nAddFame );	// xxx 명성 포인트가 증가
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogPkPvp( pMember, NULL, nAddFame, 'E' );
#endif // __WORLDSERVER
		} 
		else
		{
			int nDecVal;
			fSubFameRatio = fSubFameRatio / 100.0f;	// 비율로 환산.
			nDecVal = (int)(pMember->m_nFame * fSubFameRatio);
			if( nDecVal == 0 )	nDecVal = 1;
			pMember->m_nFame -= nDecVal;
			if( pMember->m_nFame < 0 )	pMember->m_nFame = 0;
			pMember->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx 명성 포인트가 감소.
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogPkPvp( NULL, pMember, nAddFame, 'E' );
#endif // __WORLDSERVER
		}
			
		// 바뀐 명성치를 pMember의 주위에 날려줌.
		g_UserMng.AddSetFame( pMember, pMember->m_nFame );
		
	}
	m_idDuelParty = 0;
}


void CParty::DoUsePartyReCall( u_long uPartyId, u_long uLeaderid, int nSkill ) {
	// TODO: Why is it a dead function?
	CUser* pUser = g_UserMng.GetUserByPlayerID( uLeaderid );
	if (!IsValidObj(pUser)) return;
	
	for (CUser * pMember : AllMembers(*this)) {
		if (pMember->m_idPlayer != pUser->m_idPlayer) {
			// TODO: Why does it goes to the core?
#ifdef __LAYER_1015
			g_DPCoreClient.SendSummonPlayer(pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), pMember->m_idPlayer, pUser->GetLayer());
#else	// __LAYER_1015
			g_DPCoreClient.SendSummonPlayer(pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), pMember->m_idPlayer);
#endif	// __LAYER_1015
		}
	}
}

#endif // worldserver

//
// 극단 듀얼 해제
// 
void CParty::DoDuelPartyCancel( CParty* pDuelParty ) {
	// pDuelParty may be nullptr and it is ok

#ifdef __WORLDSERVER
	for (CUser * pMember : AllMembers(*this)) {
		pMember->AddDuelPartyCancel(pDuelParty);		// 각 멤버들에게 듀얼이 취소되었다고 알림.
		
		if (pMember->m_idDuelParty != m_idDuelParty)
			Error("CParty::DoDuelPartyCancel : 파티멤버 %s의 정보이상. %d %d", pMember->GetName(), pMember->m_idDuelParty, m_idDuelParty);
		pMember->ClearDuelParty();
	}
#endif	// __CORESERVER

#ifndef __CORESERVER
	m_idDuelParty = 0;		// 파티 해제
#endif
}	

#ifdef __WORLDSERVER
void CParty::ReplaceLodestar(const CRect & rect) const {
	for (CUser * pUser : AllMembers(*this)) {
		CWorld * pWorld = pUser->GetWorld();
		if (!pWorld) continue;

		const POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z };
		if (!rect.PtInRect(point)) continue;

		const RegionElem * const pRgnElem = g_WorldMng.GetRevival(*pWorld, pUser->GetPos(), false);

		if (pRgnElem) {
			pUser->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
		}
	}
}

void CParty::Replace(DWORD dwWorldId, const D3DXVECTOR3 & vPos, BOOL) const {
	for (CUser * pMember : AllMembers(*this)) {
		pMember->REPLACE(g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nTempLayer);
		pMember->m_buffs.RemoveBuffs(RBF_COMMON, 0);
	}
}

void CParty::Replace( DWORD dwWorldId, LPCTSTR sKey ) const {
	const RegionElem * const pRgnElem = g_WorldMng.GetRevivalPos(dwWorldId, sKey);
	if (!pRgnElem) return;

	for (CUser * pMember : AllMembers(*this)) {
		pMember->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
	}
}

bool CParty::ReplaceChkLv(const int Lv) const {
	auto range = AllMembers(*this);

	return std::all_of(range.begin(), range.end(),
		[Lv](const CUser * const pMember) {
			return pMember->GetLevel() <= Lv;
		}
	);
}
#endif //__WORLDSERVER



CParty g_Party;
/*--------------------------------------------------------------------------------*/
#ifndef __CLIENT

void CPartyMng::Clear( void )
{
#ifndef __WORLDSERVER
	m_AddRemoveLock.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER

	for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
		safe_delete( i->second );

	m_2PartyPtr.clear();
	
#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER

#ifndef __WORLDSERVER
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER
}

// 극단 생성
// uLeaderPlayerId : 단장, uPartyId : 1번째 단원
// 극단를 생설할때는 2명으로 생성함( 혼자서는 극단를 생성할수 없음 )
u_long	CPartyMng::NewParty( u_long uLeaderId, u_long uMemberId, u_long uPartyId ) {
//	locked
	if( 0 == uPartyId )
	{
		m_id++;
	}
	else
	{
		m_id = uPartyId;
	}

	
	if( NULL == GetParty( m_id ) ) // NULL 이면 극단이 없으므로 만들어야 한다
	{
		CParty* pParty	= new CParty;
		
//		pParty->Lock();
		pParty->SetPartyId( m_id );
		if( TRUE == pParty->NewMember( uLeaderId ) && TRUE == pParty->NewMember( uMemberId ) )
		{
//			m_2Party.SetAt( m_id, pParty );
			m_2PartyPtr.insert( C2PartyPtr::value_type( m_id, pParty ) );
			pParty->m_nGetItemPlayerId = pParty->m_aMember[0].m_uPlayerId;
//			pParty->Unlock();
			return m_id;
		}
		else // 극단에 소속되어 있다
		{
//			pParty->Unlock();
			safe_delete( pParty );
		}
	}
	return 0;
}

// 극단 해체
// 해체는 사람이 부족하여 자동으로 해체
// 단독적으로 해체할수 없음
BOOL CPartyMng::DeleteParty( u_long uPartyId )
{
	CParty* pParty = GetParty( uPartyId );
	if( pParty )
	{
		if( pParty->m_idDuelParty )		// 극단듀얼중일때 극단이 해체되었으면
		{
			CParty *pDstParty = GetParty( pParty->m_idDuelParty );		// 상대 파티원들에게도 this파티가 해체되어 듀얼이해제됐다는걸 알림.
			if( pDstParty )
			{
				pDstParty->DoDuelPartyCancel( pParty );
			} 
			else
			{
				Error( "CPartyMng::DeleteParty : 상대파티를 찾을 수 없음 %d", pParty->m_idDuelParty );
			}

	#ifdef __WORLDSERVER
			g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pParty->m_idDuelParty, FALSE );
	#endif // __WORLDSERVER
			pParty->DoDuelPartyCancel( pDstParty );	// this파티원들에게도 듀얼해제됐다는걸 알림.
		}
#ifdef __WORLDSERVER
		CInstanceDungeonParty::GetInstance()->DestroyAllDungeonByDungeonID( uPartyId );
#endif // __WORLDSERVER

		safe_delete( pParty );
		m_2PartyPtr.erase( uPartyId );
		return TRUE;
	}
	return FALSE;
}

CParty * CPartyMng::GetParty(const u_long uPartyId) {
	return sqktd::find_in_map(m_2PartyPtr, uPartyId);
}

CAr & operator<<(CAr & ar, const CPartyMng & self) {
#ifndef __WORLDSERVER
	self.m_AddRemoveLock.Enter(theLineFile);	// lock1
#endif	// __WORLDSERVER
	ar << self.m_id;
	ar << self.m_2PartyPtr.size();

	for (const CParty * party : self.m_2PartyPtr | std::views::values) {
		ar << *party;
	}

#ifndef __WORLDSERVER
	self.m_AddRemoveLock.Leave(theLineFile);	// unlock1
#endif	// __WORLDSERVER
	return ar;
}

#ifndef __CORESERVER
CAr & operator>>(CAr & ar, CPartyMng & self) {
	self.Clear();

	ar >> self.m_id;

	size_t s; ar >> s;

	for (size_t i = 0; i != s; ++i) {
		CParty * party = new CParty();
		self.m_2PartyPtr.emplace(party->m_uPartyId, party);
	}

	return ar;
}
#endif

#ifdef __CORESERVER
LPCSTR CPartyMng::GetPartyString( u_long uidPlayer )
{
	auto i	= m_2PartyNameLongPtr.find( uidPlayer );
	if( i != m_2PartyNameLongPtr.end() )
		return( i->second.data() );
	return NULL;
}

u_long CPartyMng::GetPartyID(const char * szPartyName) const {
	return sqktd::find_in_map(m_2PartyNameStringPtr, szPartyName, 0);
}

void CPartyMng::AddPartyName( u_long uidPlayer, const char* szPartyName )
{
	m_2PartyNameLongPtr.emplace(uidPlayer, szPartyName);
	m_2PartyNameStringPtr.emplace(szPartyName, uidPlayer);
}

void CPartyMng::RemovePartyName( u_long uidPlayer, const char* szPartyName )
{
	auto iter = m_2PartyNameLongPtr.find( uidPlayer );
	auto iter1 = m_2PartyNameStringPtr.find( szPartyName );

	if( iter != m_2PartyNameLongPtr.end() )
		m_2PartyNameLongPtr.erase( iter );

	if( iter1 != m_2PartyNameStringPtr.end() )
		m_2PartyNameStringPtr.erase( iter1 );
}

BOOL CPartyMng::CreateWorkers( void )
{
	DWORD dwThreadId;
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _Worker, this, 0, &dwThreadId );
	if( m_hWorker == NULL )
		return FALSE;
	return TRUE;
}

void CPartyMng::CloseWorkers( void )
{
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
}

UINT CPartyMng::_Worker( LPVOID pParam )
{
	CPartyMng* pPartyMng	= (CPartyMng*)pParam;
	pPartyMng->Worker();
	return 0;
}

void CPartyMng::Worker( void )
{
	CPlayer* pMember;

	HANDLE hHandle	= m_hCloseWorker;
	while( WaitForSingleObject( hHandle, 1000 ) == WAIT_TIMEOUT )
	{
		CTime timeCurr	= CTime::GetCurrentTime();

		CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
		CMclAutoLock	Lock2( m_AddRemoveLock );

		for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); )
		{
			CParty* pParty = (CParty*)i->second;
			++i;
			if( pParty->m_nReferens > 0 )
			{
				for( int j = 1; j < pParty->m_nSizeofMember; j++ )
				{
					if( pParty->m_aMember[j].m_uPlayerId != 0 && pParty->m_aMember[j].m_remove )
					{
						if( timeCurr.GetTime() - pParty->m_aMember[j].m_remove->GetTime() > AllowedLimboTime )	// 10분 : 60 * 10 // 지금은 1분
						{
							u_long idMember		= pParty->GetPlayerId( j );
							if( pParty->DeleteMember( idMember ) )
							{
								pParty->m_nReferens--;
								
								g_dpCoreSrvr.SendRemoveParty( pParty->m_uPartyId, pParty->m_aMember[0].m_uPlayerId, idMember );
								
								if( pParty->GetSizeofMember() < 2 )
								{
									pMember		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( 0 ) );
									if( pMember )
										pMember->m_uPartyId	= 0;
									DeleteParty( pParty->m_uPartyId );
									pParty = nullptr;
									break;
								}

								if( pParty->m_nReferens == 0 )
									break;
							}
						}
					}
				}	// for

				if (pParty == nullptr) continue;
			}	// if

			// 파티모드를 체크
			for( int j = 0 ; j < MAX_PARTYMODE ; j++ )
			{
				if( pParty->m_nModeTime[j] )
				{
					if( j == PARTY_PARSKILL_MODE)
						continue;
					pParty->m_nModeTime[j] -= 1000;
					if( pParty->m_nModeTime[j] <= 0 )
					{
						pParty->m_nModeTime[j] = 0;
						g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, j, FALSE );
					}
				}
			}
		}	// for
//		TRACE( "Worker Party\n" );
	}
}

void CPartyMng::AddConnection( CPlayer* pPlayer )
{
	CParty* pParty;
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	pParty	= GetParty( pPlayer->m_uPartyId );
	if( pParty )
	{
		int i	= pParty->FindMember( pPlayer->uKey );
		if( i < 0 )
		{
			pPlayer->m_uPartyId		= 0;
			return;
		}

		pParty->m_aMember[i].m_remove	= std::nullopt;
		pParty->m_nReferens--;

		BEFORESENDDUAL( ar, PACKETTYPE_ADDPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );
	}
	else
		pPlayer->m_uPartyId		= 0;
}

void CPartyMng::RemoveConnection( CPlayer* pPlayer )
{
	if( pPlayer->m_uPartyId == 0 )
		return;

	CParty* pParty;
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	pParty	= GetParty( pPlayer->m_uPartyId );
	if( pParty )
	{
		int i	= pParty->FindMember( pPlayer->uKey );
		if( i < 0 )
			return;
		pParty->m_aMember[i].m_remove	= CTime::GetCurrentTime();
		pParty->m_nReferens++;

		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );

		if( pParty->m_nModeTime[PARTY_PARSKILL_MODE] )
			g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );

		if( i == 0 )
		{
			bool fRemoveParty	= true;
			for( int j = 1; j < pParty->m_nSizeofMember; j++ )
			{
				if (!pParty->m_aMember[j].m_remove)
				{
					fRemoveParty	= false;
					pParty->SwapPartyMember( 0, j );
					break;
				}
			}
			for( int k = 0 ; k < MAX_PARTYMODE ; k++ )
			{
				if( pParty->m_nModeTime[k] )
				{
					if( k == PARTY_PARSKILL_MODE)
						continue;
					pParty->m_nModeTime[k] = 0;
				}
			}

			if( fRemoveParty )
			{
				CPlayer* pPlayer;
				for( int j = 0; j < pParty->m_nSizeofMember; j++ )
				{
					pPlayer		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( j ) );
					if( pPlayer )
						pPlayer->m_uPartyId		= 0;
				}
				DeleteParty( pParty->m_uPartyId );
			}
		}
	}
}
#endif // __CORESERVER

#ifdef __WORLDSERVER

void CPartyMng::PartyMapInfo() {
	static constexpr float PARTY_MAP_AROUND = 32.0f * 32.0f;	// m_nVisibilityRange에 영향을 받는다.

	if (++m_nSecCount < PARTY_MAP_SEC) return;
	m_nSecCount = 0;

	for (CParty * pParty : m_2PartyPtr | std::views::values) {

		for (int j = 0 ; j < pParty->GetSizeofMember() ; ++j) {
			CUser * pMover = prj.GetUserByID(pParty->GetPlayerId(j));
			if (!IsValidObj(pMover)) continue;
			
			const bool changedPos = pMover->GetPos() != pParty->GetPos(j);
			if (!changedPos) continue;

			pParty->SetPos(j, pMover->GetPos());

			for (CUser * const pSendMover : AllMembers(*pParty)) {
				if (pMover == pSendMover) continue;

				const D3DXVECTOR3 vPosBuf = pSendMover->GetPos() - pMover->GetPos();
				const float fDist = D3DXVec3LengthSq(&vPosBuf);
				const bool isOutOfVisibilityRange = fDist > PARTY_MAP_AROUND;

				if (isOutOfVisibilityRange) {
					pSendMover->AddPartyMapInfo(j, pMover->GetPos());
				}
			}
		}
	}
}

#endif // __WORLDSERVER

CPartyMng	g_PartyMng;
#endif // not client

