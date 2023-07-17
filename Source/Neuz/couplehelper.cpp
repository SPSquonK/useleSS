#include "stdafx.h"
#include "couplehelper.h"
#include "playerdata.h"
#include "definetext.h"

CCoupleHelper CCoupleHelper::Instance;

BOOL CCoupleHelper::Initialize() {
	return CCoupleProperty::Instance()->Initialize();
}

void CCoupleHelper::OnCouple(CAr & ar) {
	election::OutputDebugString("C: CCoupleHelper.OnCouple");
	m_pCouple = std::make_unique<CCouple>();
	ar >> *m_pCouple;
}

void CCoupleHelper::OnProposeResult( CAr & ar )
{
	u_long idProposer;
	char szProposer[MAX_PLAYER]	= { 0,};
	ar >> idProposer;
	ar.ReadString( szProposer, MAX_PLAYER );
	election::OutputDebugString( "C: CCoupleHelper.OnProposeResult: %d, %s", idProposer, szProposer );
	char szText[200]	= { 0,};
	sprintf( szText, prj.GetText( TID_GAME_RECEIVEPROPOSAL ), szProposer );
	OutputDebugString( szText );

	// 프러포즈 받은 메세지 창 출력
	SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);
	g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
	g_WndMng.m_pWndCoupleMessage->SetMessageMod(szText, CWndCoupleMessage::CM_RECEIVEPROPOSE);
	g_WndMng.m_pWndCoupleMessage->Initialize();
}

void CCoupleHelper::OnCoupleResult( CAr & ar )
{
	if( !g_pPlayer )	return;
	u_long idPartner;
	char szPartner[MAX_PLAYER]	= { 0,};
	ar >> idPartner;
	ar.ReadString( szPartner, MAX_PLAYER );
	election::OutputDebugString( "C: CCoupleHelper.OnCoupleResult: %d, %s", idPartner, szPartner );

	m_pCouple = std::make_unique<CCouple>(g_pPlayer->m_idPlayer, idPartner);

	g_WndMng.PutString(TID_GAME_COUPLE_S02, szPartner);		// %s님과 커플이 되었습니다.
}

void CCoupleHelper::OnDecoupleResult() {
	election::OutputDebugString("C: CCoupleHelper.OnDecoupleResult");
	CCouple * pCouple = GetCouple();
	if (pCouple) {
		Clear();
		g_WndMng.PutString(TID_GAME_COUPLE_S03);		// %s님과 커플이 되었습니다.
	}
}

void CCoupleHelper::OnAddCoupleExperience(CAr & ar) {
	int nExperience;
	ar >> nExperience;
	CCouple * pCouple = GetCouple();
	if (!pCouple) return;

	const BOOL bLevelup = pCouple->AddExperience(nExperience);
	if (bLevelup) {
		g_WndMng.PutString(TID_GAME_COUPLE_LEVELUP, pCouple->GetLevel());		// 커플이 %d레벨이 되었습니다.
	}
}
