#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineQuest.h"
#include "resdata.h"
#include "defineObj.h"
#include <algorithm>
#ifdef __WORLDSERVER
#include "DialogMsg.h"
#include "user.h"
#include "dpcoreclient.h"
#include "spevent.h"

#ifdef __WORLDSERVER
#include "slord.h"
#endif	// __WORLDSERVER 
#ifdef __CLIENT
#include "clord.h"
#endif	// __CLIENT

#ifdef __CLIENT
#include "wndfield.h"
#endif	// __CLIENT

#include "dpdatabaseclient.h"
#endif	// __WORLDSERVER

#ifdef __CLIENT			
#include "Dialogmsg.h"
#endif

#include "Party.h"

#ifdef __WORLDSERVER
#include "DPSrvr.h"
#endif // __WORLDSERVER

#include "Project.h"
#include "ProjectCmn.h"


#ifdef __WORLDSERVER
#include "Environment.h"
#include "PCBang.h"
#include "GuildHouse.h"
#include "CampusHelper.h"
#endif // __WORLDSERVER

SKILL * CMover::GetSkill(DWORD dwSkill) {
	const auto it = std::ranges::find_if(m_jobSkills,
		[dwSkill](const SKILL & skill) { return skill.dwSkill == dwSkill; }
		);
	return it != m_jobSkills.end() ? &*it : nullptr;
}

const SKILL * CMover::GetSkill(const DWORD dwSkill) const {
	const auto it = std::ranges::find_if(m_jobSkills,
		[dwSkill](const SKILL & skill) { return skill.dwSkill == dwSkill; }
	);
	return it != m_jobSkills.end() ? &*it : nullptr;
}

BOOL CMover::CheckSkill(const DWORD dwSkill) const {
	const ItemProp * pSkillProp = prj.GetSkillProp(dwSkill);

	if (pSkillProp == NULL || pSkillProp->nLog == 1)
		return FALSE;
	if (!HasLevelForSkill(*pSkillProp)) return FALSE;

	if (pSkillProp->dwReSkill1 != 0xffffffff) {
		const SKILL * pSkillBuf = GetSkill(pSkillProp->dwReSkill1);
		if (pSkillBuf && pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1) {
			return FALSE;
		}
	}

	if (pSkillProp->dwReSkill2 != 0xffffffff) {
		const SKILL * pSkillBuf = GetSkill(pSkillProp->dwReSkill2);
		if (pSkillBuf && pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2) {
			return FALSE;
		}
	}

	return TRUE;
}


// �ɸ��Ͱ� �������� ��ų�߿� �ɸ��ͷ������� Ȱ��ȭ �ȵȰ͵� Ȱ������ üũ�Ͽ� �������
void CMover::PutLvUpSkillName_1( DWORD dwLevel )
{
#ifdef __CLIENT
	for (const SKILL & skill : m_jobSkills) {

		const ItemProp * pSkillProp = skill.GetProp();
		
		if( pSkillProp && dwLevel == pSkillProp->dwReqDisLV )
		{
			if( pSkillProp->dwReSkill1 != -1 )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill1);

				if( pSkillProp->dwReSkillLevel1 > pSkill->dwLevel )
					continue;
			}
			if( pSkillProp->dwReSkill2 != -1 )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill2);
				
				if( pSkillProp->dwReSkillLevel2 > pSkill->dwLevel )
					continue;
			}

			g_WndMng.PutString(TID_GAME_REAPSKILL, pSkillProp->szName);
		}
	}
#endif //__CLIENT
}		

BOOL IsSkillParent( CMover* pMover, DWORD dwSkill, DWORD dwChildSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReSkill1 == dwChildSkill || pSkillProp->dwReSkill2 == dwChildSkill )
		return TRUE;

	return FALSE;
}

BOOL IsActive( CMover* pMover, DWORD dwSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if (pSkillProp->dwReSkill1 != NULL_ID) {
		SKILL * pSkill1 = pMover->GetSkill(pSkillProp->dwReSkill1);
		if (pSkill1 == NULL) return FALSE;

		const ItemProp * pSkillProp1 = prj.GetSkillProp(pSkill1->dwSkill);
		if (pSkillProp1 == NULL) return FALSE;

		if (!pMover->HasLevelForSkill(*pSkillProp1)) return FALSE;

		if (pSkillProp->dwReSkillLevel1 != pSkill1->dwLevel) return FALSE;
	}

	if (pSkillProp->dwReSkill2 != NULL_ID) {
		SKILL * pSkill1 = pMover->GetSkill(pSkillProp->dwReSkill2);
		if (pSkill1 == NULL) return FALSE;

		const ItemProp * pSkillProp1 = prj.GetSkillProp(pSkill1->dwSkill);
		if (pSkillProp1 == NULL) return FALSE;

		if (!pMover->HasLevelForSkill(*pSkillProp1)) return FALSE;

		if (pSkillProp->dwReSkillLevel2 != pSkill1->dwLevel) return FALSE;
	}

	if (!pMover->HasLevelForSkill(*pSkillProp)) return FALSE;

	return TRUE;
}


// ��ų �������� �ɸ��� �������� �ȳ��Դ� ��ų�� �˻��Ͽ� ���� ���
void CMover::PutLvUpSkillName_2( DWORD dwSkill )
{
#ifdef __CLIENT
	LPSKILL pSkill = GetSkill( dwSkill );
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	for (const SKILL & skill : m_jobSkills) {
		if (IsSkillParent(this, skill.dwSkill, dwSkill)) {
			if (IsActive(this, skill.dwSkill)) {
				const ItemProp * pSkillProp1 = skill.GetProp();
				g_WndMng.PutString(TID_GAME_REAPSKILL, pSkillProp1->szName);
			}
		}
	}
#endif // CLIENT
}

#ifdef __WORLDSERVER
/// �ŷ�, ����, ���λ�����, ������ FALSE
int CMover::GetSummonState()
{
	// �ŷ�
	if( m_vtInfo.GetOther() != NULL )
		return 1;
	  // â��
	if( m_bBank )
		return 1;
	  // ���â��
	if( m_bGuildBank )
		return 1;

	// ����
	if( IsDie() )
		return 2;

	// ���λ�����
	if( m_vtInfo.VendorIsVendor() || m_vtInfo.IsVendorOpen() )
		return 3;

	// ������
	if( IsAttackMode() )
		return 4;
	
	// ������
	if( ( IsFly() ) )
		return 5;


	if( m_nDuel )
		return 6;


	return 0;
}
#endif //


#ifdef __CLIENT
LPCTSTR CMover::GetJobString()
{
	if( m_nJob != -1 )
		return prj.m_aJob[ m_nJob ].szName;
	return _T( "" );
}
#endif	// __CLIENT

BOOL  CMover::IsJobType( DWORD dwJobType ) 
{ 
	if( IsExpert() )
	{
		if( dwJobType == JTYPE_EXPERT ) 
			return TRUE;
		return FALSE;
	}
	return JTYPE_BASE == dwJobType;
}
BOOL CMover::IsBaseJob()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_BASE;
}
BOOL CMover::IsExpert()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_EXPERT;
}

bool CMover::IsPro()        const { return prj.m_aJob[m_nJob].dwJobType == JTYPE_PRO; }
bool CMover::IsMaster()     const { return prj.m_aJob[m_nJob].dwJobType == JTYPE_MASTER; }
bool CMover::IsHero()       const { return prj.m_aJob[m_nJob].dwJobType == JTYPE_HERO; }
bool CMover::IsLegendHero() const { return prj.m_aJob[m_nJob].dwJobType == JTYPE_LEGEND_HERO; }
bool CMover::IsJobTypeOrBetter(DWORD jobType) const {
	const DWORD myJobType = prj.m_aJob[m_nJob].dwJobType;
	return myJobType >= jobType;
}

bool CMover::HasLevelForSkill(const ItemProp & skillProp) const {
	const int level = GetLevel();
	
	// Have the required level?
	const int reqLevel = static_cast<int>(skillProp.dwReqDisLV);
	if (reqLevel <= level) return true;
	
	// Master+ can bypass level requirement of skills that requires a level <= 120
	return reqLevel <= MAX_LEVEL && IsJobTypeOrBetter(JTYPE_MASTER);
}

BYTE	CMover::GetLegendChar()
{
	if(IsMaster())
		return LEGEND_CLASS_MASTER;
	else if(IsJobTypeOrBetter(JTYPE_HERO))
		return	LEGEND_CLASS_HERO;
	else
		return LEGEND_CLASS_NORMAL;
}

bool CMover::IsInteriorityJob(const int nJob, const int characterJob) {
	if (nJob == JOB_VAGRANT || nJob == characterJob) return true;
	const auto allMyJobs = prj.GetAllJobsOfLine(characterJob);
	return std::ranges::find(allMyJobs, nJob) != allMyJobs.end();
}

int CMover::GetExpPercent() const {
	EXPINTEGER exp = GetExp1();
	EXPINTEGER maxExp = GetMaxExp1();

	if (maxExp >= EXPINTEGER(1024 * 1024 * 1024)) {
		// Avoid overflow when multiplying. 1024 is a power of 2 so it is fast
		exp /= EXPINTEGER(1024 * 1024 * 1024);
		maxExp /= EXPINTEGER(1024 * 1024 * 1024);
	}

	return exp * 10000 / maxExp;
}


int   CMover::GetRemainGP()
{
	return m_nRemainGP;
}

// ���߷� �϶� ���´� ��Ţ�̺�Ʈ�� ������� �ü� �ֱ⶧���� GetParm()�� ���⼭ ���� ����ؾ��Ѵ�.
int	CMover::GetAdjHitRate( void )
{
	return GetParam( DST_ADJ_HITRATE, m_nAdjHitRate );
}

// ȸ���� 
int CMover::GetParrying()
{
	if( IsPlayer() )
		return	( (int)( (GetDex() * 0.5f) + GetParam( DST_PARRY, m_nAdjParry ) ) );
	else
		return GetProp()->dwER;
}


void CMover::UpdateParam()
{
	if( IsPlayer() ) 
	{
		SumEquipDefenseAbility( &m_nDefenseMin, &m_nDefenseMax ); 
		SumEquipAdjValue( &m_nAdjHitRate, &m_nAdjParry );
	}
	else
	{
		m_nDefenseMin = m_nDefenseMax = GetNaturalArmor();
		m_nAdjHitRate = 0;
		m_nAdjParry = 0;
	}
}
// �������� ������ ��´�. 
int CMover::GetItemAbility( int nItem )
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	if( nAbilityMax == nAbilityMin )
		return nAbilityMin;
	return nAbilityMin + ( xRand() % (nAbilityMax - nAbilityMin ) );
}
// �������� �ּ� ������ ��´�.
int CMover::GetItemAbilityMin( int nItem ) const
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	return nAbilityMin;
}
// �������� �ִ� ������ ��´�.
int CMover::GetItemAbilityMax( int nItem ) const
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	return nAbilityMax;
}

// ��Ʈ����Ʈ ����
void CMover::IncHitPoint(int nVal) 
{ 
	SetPointParam( DST_HP, m_nHitPoint + nVal );
}
// ��������Ʈ ����
void CMover::IncManaPoint(int nVal) 
{ 
#ifdef __WORLDSERVER 			
	if( IsSMMode( SM_MAINTAIN_MP ) && nVal < 0 )
		return;
#ifdef __EVENT1206
	if( IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) ) //  raiders_fix051217
		return;
#endif	// __EVENT1206
#endif // __WORLDSERVER
	SetPointParam( DST_MP, m_nManaPoint + nVal );
}

void CMover::IncFatiguePoint(int nVal) 
{ 
#ifdef __WORLDSERVER
	if( IsSMMode( SM_MAINTAIN_FP ) && nVal < 0 )
		return;
#ifdef __EVENT1206
	if( IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) )	// raiders_fix051217
		return;
#endif	// __EVENT1206
#endif // __WORLDSERVER
	SetPointParam( DST_FP, m_nFatiguePoint + nVal );
}

// ������� ��� ó��.
// this�� ���ڰ� �ȴ�.
// pLose�� �й���.

#ifdef __WORLDSERVER


void CMover::ChangeFame( CMover *pLose )
{
	int nVal = 0;
	float fVal = 0;
	int nGap = 0;

	nGap = pLose->GetLevel() - GetLevel();		// ���ڿ� ������ ��������

	if( nGap >= 80 )	    { nVal = 100; fVal = 15.0f; }	// ���ڰ� ������ �� ���Ҵ�.
	else if( nGap >= 30 )	{ nVal = 50;  fVal = 10.0f; }
	else if( nGap >= 20 )	{ nVal = 30;  fVal = 8.0f;  }
	else if( nGap >= 10 )	{ nVal = 10;  fVal = 6.0f;  }
	else if( nGap <= -80 )	{ nVal = 1;   fVal = 1.0f;  }	// ���ڰ� ������ �� ���Ҵ�.
	else if( nGap <= -30 )	{ nVal = 2;   fVal = 2.0f;  }
	else if( nGap <= -20 )	{ nVal = 3;   fVal = 3.0f;  }
	else if( nGap <= -10 )	{ nVal = 4;   fVal = 4.0f;  }
	else                    { nVal = 5;   fVal = 5.0f;  }	// 9 ~ -9


	if( nVal )		// ���ڿ��� ����ġ ��
	{	
		if( this->IsFly() && pLose->IsFly() )	// ���� ����� ��쿡�� �й�� ����ϴ� ���� ����Ʈ�� ����������, �¸� �� ȹ���ϴ� ��������Ʈ�� ���� ������ 2��� �Ѵ�
			nVal *= 2;
			
		m_nFame += nVal;	
		g_UserMng.AddSetFame( this, m_nFame );
		((CUser*)this)->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nVal );	// xxx ���� ����Ʈ�� �����
		((CUser*)this)->CheckHonorStat();
		((CUser*)this)->AddHonorListAck();
		g_UserMng.AddHonorTitleChange( this, m_nHonor);
	}
	
	if( fVal )	// ������ ����ġ(�ۼ�Ʈ)�� ����.
	{
		int		nDecVal;
		fVal = fVal / 100.0f;	// ������ ȯ��.
		nDecVal = (int)(pLose->m_nFame * fVal);
		if( nDecVal <= 0 )	nDecVal = 1;
		pLose->m_nFame -= nDecVal;
		if( pLose->m_nFame < 0 )
			pLose->m_nFame = 0;

		g_UserMng.AddSetFame( pLose, pLose->m_nFame );
		((CUser*)pLose)->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx ���� ����Ʈ�� ����.
		g_dpDBClient.SendLogPkPvp( this, pLose, nVal, 'D' );
		((CUser*)pLose)->CheckHonorStat();
		((CUser*)pLose)->AddHonorListAck();
		g_UserMng.AddHonorTitleChange( pLose, pLose->m_nHonor);
	}
}
#endif // __WORLDSERVER

int	CMover::DuelResult( CMover *pLose )
{
#ifdef __WORLDSERVER
	if( pLose->m_nDuel == 1 )
	{
		if( pLose->GetId() != m_idDuelOther )
			return 0;
	}

	// ���ڴ� ������������ ����ġ ���
	// ���ڴ� ������������ ����ġ% �϶�
	if( pLose->m_nDuel == 1 )	// ���� PvP
	{
		ChangeFame( pLose );
		
#ifdef __PVPDEBUFSKILL
		//pLose->m_SkillState.SetMover( pLose );
		pLose->m_nDead = PROCESS_COUNT * 2;		// ���� �� 2�ʰ��� ����
		pLose->RemoveAttackerBuffs( pLose->m_idDuelOther );
		//m_SkillState.SetMover( this );
		m_nDead = PROCESS_COUNT * 2;		// ���� �� 2�ʰ��� ����
		RemoveAttackerBuffs( m_idDuelOther );
#endif // __PVPDEBUFSKILL
	}
	else
	// �ش� ���.
	if( pLose->m_nDuel == 2 )		// �ش� ������϶� �ش��峢���� ������ ��.
	{
		int nVal = 0;
		float fVal = 0;
		int nGap = 0;

		CParty *pLoseParty = g_PartyMng.GetParty( pLose->m_idparty );
		if( pLoseParty == NULL || pLoseParty->IsLeader( pLose->m_idPlayer ) == FALSE )		// ��Ƽ�� ���ų� �������� ������ �ƴϸ� �� ����.
			return 0;
		
		CParty *pParty = g_PartyMng.GetParty( m_idparty );		// �̱��� ��Ƽ.
		if( pParty == NULL )
			return 0;

		CMover* pLeader	= pParty->GetLeader();
		if( IsValidObj( pLeader ) )
			nGap	= pLose->GetLevel() - pLeader->GetLevel();
		else
			nGap	= pLose->GetLevel() - GetLevel();
		
		if( nGap >= 80 )	{ nVal = 100; fVal = 15.0f; }		// ���ڰ� ������ �� ���Ҵ�.
		else if( nGap >= 50 )	{ nVal = 50; fVal = 10.0f; }
		else if( nGap >= 20 )	{ nVal = 30; fVal = 8.0f; }
		else if( nGap >= 10 )	{ nVal = 10; fVal = 6.0f; }
		else if( nGap <= -80 )	{ nVal = 1; fVal = 1.0f; }		// ���ڰ� ������ �� ���Ҵ�.
		else if( nGap <= -50 )	{ nVal = 2; fVal = 2.0f; }
		else if( nGap <= -20 )	{ nVal = 3; fVal = 3.0f; }
		else if( nGap <= -10 )	{ nVal = 4; fVal = 4.0f; }
		else { nVal = 5; fVal = 5.0f; }	// 9 ~ -9

		g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pLoseParty->m_uPartyId, FALSE );

		pLose->m_bLastDuelParty = TRUE;
		pLoseParty->DoDuelResult( pParty, 0, nVal, fVal );		// ��Ƽ���鿡�� �й����� �˸��� ��� ������Ŵ.
		pParty->DoDuelResult( pLoseParty, 1, nVal, fVal );		// ��Ƽ���鿡�� �¸��� �˸�.
	}
#endif	// __WORLDSERVER

	return 1;
}

// ��Ʈ����Ʈ�� ���ϱ�
void CMover::SetHitPoint(int nVal) 
{
	m_nHitPoint = nVal;
//	m_nHitPoint += nVal;
	if( m_nHitPoint < 0 )
		m_nHitPoint = 0;
	if( m_nHitPoint > GetMaxHitPoint() )
		m_nHitPoint = GetMaxHitPoint(); 
}
// ���� ����Ʈ�� ���ϱ� 
void CMover::SetManaPoint(int nVal) 
{
	m_nManaPoint = nVal;
	if( m_nManaPoint < 0 )
		m_nManaPoint = 0;
	if( m_nManaPoint > GetMaxManaPoint() )
		m_nManaPoint = GetMaxManaPoint(); 
}
// ����Į ����Ʈ�� ���ϱ� 
void CMover::SetFatiguePoint(int nVal) 
{
	m_nFatiguePoint = nVal;
	if( m_nFatiguePoint < 0 )
		m_nFatiguePoint = 0;
	if( m_nFatiguePoint > GetMaxFatiguePoint() )
		m_nFatiguePoint = GetMaxFatiguePoint(); 
}

int CMover::GetPointParam(int nDestParameter )
{
	switch( nDestParameter )
	{
		case DST_HP:	return( GetHitPoint() );
		case DST_MP:	return( GetManaPoint() );
		case DST_FP:	return( GetFatiguePoint() );
		case DST_GOLD:	return( GetGold() );
	}
	return( 0 );
}

void CMover::SetPointParam( int nDstParameter, int nValue, BOOL bTrans )	// bTrans�� TRUE�̸� ��������
{
#ifdef __WORLDSERVER
	int nMax = nValue;
#endif // __WORLDSERVER
	if( nValue < 0 )	nValue	= 0;
	BOOL	bTransfer	= bTrans;

	switch( nDstParameter )
	{
		case DST_HP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxHitPoint() || nMax == 999999999 )	nValue	= GetMaxHitPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxHitPoint() )	nValue	= GetMaxHitPoint();
#endif // __WORLDSERVER
			if( m_nHitPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nHitPoint		= nValue;
			}
			break;
		case DST_MP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxManaPoint() || nMax == 999999999 )	nValue	= GetMaxManaPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxManaPoint() )	nValue	= GetMaxManaPoint();
#endif // __WORLDSERVER
			if( m_nManaPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nManaPoint	= nValue;
			}
			break;
		case DST_FP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() || nMax == 999999999 )	nValue	= GetMaxFatiguePoint();
#else // __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() )		nValue	= GetMaxFatiguePoint();
#endif // __WORLDSERVER
			if( m_nFatiguePoint != nValue )
			{
				bTransfer	= TRUE;
				m_nFatiguePoint		= nValue;
			}
			break;
		case DST_GOLD:
			//m_dwGold	= (DWORD)nValue;
#ifdef __PERIN_BUY_BUG	// chipi_091219 �߰�
#ifdef __WORLDSERVER
			Error( "SetPointParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]", static_cast<CUser*>(this)->GetName(), static_cast<CUser*>(this)->m_idPlayer, nValue );
			return;
#endif // __WORLDSERVE
#endif // __PERIN_BUY_BUG

	#ifdef __CLIENT
			if( IsActiveMover() )
				PLAYSND(SND_INF_TRADE);
	#endif //__CLIENT
			bTransfer	= TRUE;
			SetGold( nValue );
			break;
	}
#ifdef __WORLDSERVER
	if( bTransfer )
		g_UserMng.AddSetPointParam( this, nDstParameter, nValue );
#endif	// __WORLDSERVER
}
BOOL CMover::IncStrLevel()
{
	if( m_nRemainGP > 0 )
	{
		m_nStr += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
	return FALSE;
}
BOOL CMover::IncStaLevel()
{
	if( m_nRemainGP > 0 )
	{
		m_nSta += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
	return FALSE;
}
BOOL CMover::IncDexLevel()
{
	if( m_nRemainGP > 0 )
	{
		m_nDex += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
	return FALSE;
}
BOOL CMover::IncIntLevel()
{
	if( m_nRemainGP > 0 )
	{
		m_nInt += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
	return FALSE;
}

// ����ġ�� nExp��ŭ ��´�.  ���ٿ �ȴ�.
// bExp2Clear : pxp�� 0���� �Ұ��� ������.
// bLvDown : �����ٿ��� �Ұ��� ������.
BOOL CMover::DecExperience( EXPINTEGER nExp, BOOL bExp2Clear, BOOL bLvDown )
{
	if( nExp < 0 )	// ���� �׻� ����θ� ��� �Ѵ�.
		return FALSE;
	
	int nPrevLevel	= m_nLevel - 1;
	if( nPrevLevel < 1 )	// ���� 1���Ϸδ� ���̻� �������� ����.
		return FALSE;

	m_nExp1		-= nExp;	// nExp��ŭ ����ġ�� ����. Exp2�� 0%��.


	if( m_nExp1 < 0 )
	{
		if( bLvDown )	// ���ٿ��� �Ǵ� ��Ȳ.
		{
			--m_nLevel;
			m_nExp1 = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 + m_nExp1 );
			return TRUE;	// ���ٿ� ������ TRUE����
		}
		else
		{
			m_nExp1		= 0;
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogLevelUp( this, 7 );
#endif // __WORLDSERVER
		}
	}
	return FALSE;
}

// ���� ������ �ִ����ġ�� %�� ����ġ �϶�
// ex) fPercent = 0.2f;		// 20%
BOOL CMover::DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown )
{
	EXPINTEGER	nExpOld		= m_nExp1;		// ���̱��� exp
	EXPINTEGER	nMaxExp		= prj.m_aExpCharacter[m_nLevel+1].nExp1;	// ���̱��� ����ġġ �ƽ���.
	int	nOldLv	= m_nLevel;		// ���̱��� ����.

	if( fPercent == 0.0f )	
		return FALSE;	// 0%�� ��� ��Ȳ�̸� ����� �ʿ� ����.

	EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 * fPercent );	// ���緹���� �ִ����ġ * �ۼ�Ʈ
	bLvDown		= DecExperience( nDecExp, bExp2Clear, bLvDown );

	if( bLvDown )	// ������ ������ �ٿ�Ǿ�����.
	{
		if( m_nLevel - nOldLv < -1 )	// ������ �δܰ� �̻� �ٿ�Ǵ� ���� ����� �Ѵ�.
			Error( "CMover::DecExperiencePercent : %s �����ٿ� �̻�. %d -> %d  %I64d(%f)"
			, m_szName, nOldLv, m_nLevel, nDecExp, fPercent );

		EXPINTEGER	nDecExp2	= ( prj.m_aExpCharacter[m_nLevel+1].nExp1 - m_nExp1 ) + nExpOld;	// ���� ���� ����Ʈ.
		if( nDecExp2 > nDecExp )	// ���� ���ξ��� ���� �������� �ߴ� �纸�� ������ ���.
			Error( "1 CMover::DecExperiencePercent : %s(Lv%d) ����ġ�� �̻��ϰ� ����. %I64d %I64d(%f) %I64d %I64d"
			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, m_nExp1 );
	}
	else
	{
		EXPFLOAT fOldPercent = static_cast<EXPFLOAT>( nExpOld ) / static_cast<EXPFLOAT>( nMaxExp );		// ���̱��� �ۼ�Ʈ��.
		EXPFLOAT fCurPercent = static_cast<EXPFLOAT>( m_nExp1 ) / static_cast<EXPFLOAT>( nMaxExp );		// ������ �ۼ�Ʈ��.

		if( fOldPercent - fCurPercent > 0.060f )
			Error( "0 CMover::DecExperiencePercent : %s(Lv%d) ����ġ�� �̻��ϰ� ����. %I64d %I64d(%f) %I64d(%f) %I64d(%f)"
			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, fOldPercent, m_nExp1, fCurPercent );
	}

	return bLvDown;
}

// ���� ����ġ ȹ��
BOOL CMover::AddFxp( int nFxp )
{
	if( nFxp <= 0 )		// nExp�� ���̳ʽ��̰ų� 0�̸� ó�����ʿ� ����.
		return FALSE;

	if( m_nHitPoint <= 0 )		// �װ����� ����ġ ȹ�� ����.
		return FALSE;

	if( GetFlightLv() == 0 )
		return FALSE;

	int nNextLevel = GetFlightLv() + 1;

	m_nFxp += nFxp;		// �ϴ� ����ġ�� ���ϰ�.
	if( (DWORD)( m_nFxp ) >= prj.m_aFxpCharacter[nNextLevel].dwFxp ) // �������� �Ǿ����� Ȯ��.
	{
		int		FxpBuf;
		FxpBuf = m_nFxp - prj.m_aFxpCharacter[ nNextLevel ].dwFxp;		// �������ϰ��� �󸶳� �� �����ƴ���?
		m_nFxp = 0;
		SetFlightLv( nNextLevel );	// ���� ����.
		
#ifdef __CLIENT
		// �������� ����Ʈ
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
#endif	// __CLIENT
	
		// ���ٵ� ����ġ�� �ѹ��� ������ ������.
		if( FxpBuf > 0)
		{
			AddFxp( FxpBuf );
		}

		return TRUE;
	}
	return FALSE;
}

bool CMover::AddChangeJob(const int nJob) {
	if (nJob < 0 || nJob >= MAX_JOB) return false;

	const DWORD jobType = GetJobType(nJob);

	const MoverSkills previously = std::move(m_jobSkills);
	m_jobSkills = MoverSkills::ForJob(nJob);

	for (SKILL & newSkill : m_jobSkills) {
		const SKILL * prevSkill = previously.FindBySkillId(newSkill.dwSkill);
		if (prevSkill) {
			newSkill.dwLevel = prevSkill->dwLevel;
		}
	}

#ifdef __WORLDSERVER
#ifdef __S_NEW_SKILL_2
	g_dpDBClient.SaveSkill( (CUser*)this );
#endif // __S_NEW_SKILL_2
	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( this, m_nHonor);
#endif // __WORLDSERVER
	return true;
}

BOOL CMover::SetFxp( int nFxp, int nFlightLv )
{
	m_nFxp = nFxp;

	// ������ �ö�����.
	if( nFlightLv > GetFlightLv() )
	{
		SetFlightLv( nFlightLv );
#ifdef __CLIENT
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
#endif // CLIENT
		return TRUE;
	}

	return FALSE;
}

BOOL CMover::SetExperience( EXPINTEGER nExp1, int nLevel )
{
	m_nExp1		= nExp1;

	if( IsInvalidObj(this) )
		return 0;

	if( nLevel > m_nLevel )
	{
#ifdef __CLIENT
 		// 15�� �Ǹ� ���̻� �ʺ��ڰ� �ƴϹǷ� �ڵ����� �ʺ��ڵ��� ����.
		if( nLevel == 15 )		// 1���������� 15�� ���� define ������ �װɷ� �ٲ���. -xuzhu-
			g_Option.m_nInstantHelp = 0;
 		
		PutLvUpSkillName_1(nLevel);

		if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 ) ) )
			SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		if( m_nDeathLevel < nLevel )
			g_WndMng.PutString( prj.GetText( TID_GAME_LEVELUP ), NULL, prj.GetTextColor( TID_GAME_LEVELUP ) );
		
		if( g_WndMng.m_pWndWorld )
		{
			if( ::GetLanguage() != LANG_JAP )
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), g_WndMng.m_pWndWorld->m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), NULL );// CWndBase::m_Theme.m_pFontCaption );
		}
		
		// 2, 8, 12
		static	int nPatLv[4]	= { 2, 8, 12, 0	};
		for( int i = 0; i < 3; i++ ) {
			if( m_nLevel < nPatLv[i] && nLevel >= nPatLv[i] ) {
				g_WndMng.PutString(TID_GAME_PATTERNUP);
				break;
			}
			else if( m_nLevel >= nPatLv[2] )
				break;
		}

		if( IsJobType( JTYPE_BASE ) )
		{
			if( nLevel == 15 )
				g_WndMng.PutString(TID_EVE_LEVEL15);
		}
		if( m_nLevel < 20 && nLevel >= 20 )
		{
			g_WndMng.PutString(TID_GAME_FLYLVLUP);
			SetFlightLv( 1 );
		}
		else if( nLevel < 20 )
			SetFlightLv( 0 );
#endif	// __CLIENT
		m_nLevel	= nLevel;

		m_nHitPoint = GetMaxHitPoint();
		m_nManaPoint = GetMaxManaPoint();
		m_nFatiguePoint = GetMaxFatiguePoint();

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
				pWndWorld->GetAdvMgr()->AddAdvButton(APP_CHARACTER3);
				switch(GetLevel())
				{
				case 15:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_CHANGEJOB);
					break;						
				case 20:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif //__CLIENT		

		return TRUE;
	}
	else if( nLevel < m_nLevel )	// ���� �ٿ��� �Ǿ�����.
	{
		m_nLevel	= nLevel;
		if( nLevel < 20 )
			SetFlightLv( 0 );

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
				switch(GetLevel())
				{
				case 2:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER3);
					break;
				case 3:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_BERSERKERMODE);
					break;
				case 4:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_SKILL_BEFOREJOB);
					break;
				case 8:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_ESSENSE);
					break;
				case 15:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_CHANGEJOB);
					break;					
				case 16:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT);
					break;
				case 20:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif	// __CLIENT
	}


	return FALSE;
}


void CMover::SetEquipDstParam()
{
	if( IsPlayer() ) 
	{
		CItemElem* pItemElem;
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			if( pItemElem )
				SetDestParam( pItemElem->m_dwItemId, FALSE );	//don't send
		}
	}
}


/////////////////////////////////////////////////////////////////////////////// aaaaaaaaa
int CMover::SumEquipDefenseAbility( LONG* pnMin, LONG* pnMax )
{
	if( IsPlayer() )
	{
		int	nMin = 0, nMax = 0;
		CItemElem* pItemElem;
		ItemProp *pItemProp;

		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			//if(pItem && !pItem->IsBreakableItem() && (pItem->m_nHitPoint == 1 || pItem->m_nHitPoint == 0))
			//	continue; IK2_ARMOR
			// IK_ARMOR�� ���� üũ���� �����Ƿ� �Ӹ��� ���� ���潺���� ���� �ʵ��� �����Ѵ�.
			// ������ �����Ƽ�� ���潺�� ���Ե��� �ʴ´�
			//if( pItemElem && pItemElem->GetProp()->dwItemKind2 < IK2_WEAPON_DIRECT && pItemElem->GetProp()->dwItemKind2 > IK2_WEAPON_GUN )  //&& ( pItemElem->m_nHitPoint > 1 || pItemElem->m_nHitPoint == -1) )
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();

				if( pItemProp == NULL )
				{
					Error( "Parts Not Property = %d", i );
				}

				if( pItemProp )
				{
					if( pItemProp->dwItemKind2 == IK2_ARMOR || pItemProp->dwItemKind2 == IK2_ARMORETC )
					{
						int nOption = pItemElem->GetAbilityOption();
						int nOptionVal = 0;
						if( nOption > 0 ) 
							nOptionVal = (int)( pow( (float)( nOption ), 1.5f ) );
							
						float f		= GetItemMultiplier( pItemElem );
						nMin	+= (int)( pItemProp->dwAbilityMin * f ) + nOptionVal;
						nMax	+= (int)( pItemProp->dwAbilityMax * f ) + nOptionVal;
					}
				}
			}
		}
		// ��� �ջ��� �����ٸ� �� ��ҿ� ����ġ�� �����Ѵ�.
		nMin = GetParam( DST_ABILITY_MIN, nMin );
		nMax = GetParam( DST_ABILITY_MAX, nMax );
		if( pnMin ) *pnMin = nMin;
		if( pnMax ) *pnMax = nMax;
		return ( ( nMin + nMax ) / 2 ); // ���� ���� ��� ���̴�. �����ÿ��� ������� �ʴ´�. ����̳� ��Ÿ �뵵�� ����Ѵ�.
	}
	else
	{
	}
	
	return 0;
}

void CMover::SumEquipAdjValue( int* pnAdjHitRate, int* pnAdjParray )
{
	*pnAdjHitRate = 0;
	*pnAdjParray = 0;

	CItemElem* pItemElem;
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		pItemElem = GetEquipItem( i );
		if( pItemElem )  
		{
			if( pItemElem->GetProp() )
			{
				if( (int)pItemElem->GetProp()->nAdjHitRate > 0 )
					*pnAdjHitRate += pItemElem->GetProp()->nAdjHitRate;
				if( (int)pItemElem->GetProp()->dwParry > 0 )
					*pnAdjParray += pItemElem->GetProp()->dwParry;
			}
		}
	}
}

// �Ⱥ��̴� ����
BOOL CMover::SetDarkCover( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_INVISIBILITY )	// ����ȭ�� �鿪�̶�....�׷��͵� �ֳ�?
			return FALSE;	
		SetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, NULL_CHGPARAM );
		m_wDarkCover =	(short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_INVISIBILITY )
		{
			ResetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, TRUE );		// �����������
		}
		m_wDarkCover = 0;
	}

	return TRUE;
}

// ���ϻ��·� �����
BOOL CMover::SetStun( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		MoverProp *pMoverProp = GetProp();
		if( pMoverProp->dwClass == RANK_SUPER || pMoverProp->dwClass == RANK_MIDBOSS )
			return FALSE;			// ���۸��� ���Ͽ� �Ȱɸ���. �̷��� �������� ���Ͽ� �̹��� �ǰ� �����ϴ°� ����.

		if( GetAdjParam( DST_IMMUNITY ) & CHS_STUN ) 
			return FALSE;		// ���� �鿪���´�.

		SetDestParam( DST_CHRSTATE, CHS_STUN, NULL_CHGPARAM, FALSE );
		SendActMsg( OBJMSG_STUN );		// ���� ���� ����.
		m_wStunCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );		// �󸶵����̳� ���ϻ��°� �Ǵ���.
	}
	else
	{
		if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )	// �̰� �˻���ϸ� �Ź� Ŭ��� ResetDestParam�� ������.
			ResetDestParam( DST_CHRSTATE, CHS_STUN, FALSE);	// ���� ���� ����.
		m_wStunCnt = 0;
		m_dwFlag &= (~MVRF_NOACTION);				// �׼� ���� ���·� ��ȯ.
	}

#ifdef __WORLDSERVER
	g_UserMng.AddSetStun( this, tmMaxTime );
#endif // Worldserver

	return TRUE;
}


// this�� ���� �ɸ��� �Ѵ�.
// idAttacker : ������(��Ʈ���ϼ��� �ֽ�)
// tmMaxTime : 1/1000�� ����.
// tmUnit : ���� ���� ���������� ���ϲ���.
// nDamage : ��������
BOOL CMover::SetPoison( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_POISON ) 
			return FALSE;		// ���� �鿪���´�.
		SetDestParam( DST_CHRSTATE, CHS_POISON, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			Error( "SetPoison : %s ��ȿ���� ���ӽð��� �������� ����", GetName() );
		if( tmUnit == -1 )
			Error( "SetPoison : %s ��ȿ���� tick �ð��� �������� ����", GetName() );
		if( wDamage == -1 )
			Error( "SetPoison : %s ��ȿ���� ƽ�� �������� �������� ����", GetName() );
		
		m_wPoisonCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wPoisonDamage = wDamage;
		m_tmPoisonUnit = tmUnit;
		m_idPoisonAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_POISON )
			ResetDestParam( DST_CHRSTATE, CHS_POISON, TRUE );	// ������ ����
		m_wPoisonCnt = 0;
		m_wPoisonDamage = 0;
		m_tmPoisonUnit = 0;
	}

	return TRUE;
}

// this�� ������¿� ������ �Ѵ�
// tmMaxTime : ���� ���ӽð�(1/1000)
// nAdjHitRate : ���ӽð����� ������ ���߷�
// bApply : ����/����
BOOL CMover::SetDark( BOOL bApply, DWORD tmMaxTime, int nAdjHitRate )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_DARK ) 
			return FALSE;		// ���濡 �鿪���´�.
		SetDestParam( DST_CHRSTATE, CHS_DARK, NULL_CHGPARAM );
		SetDestParam( DST_ADJ_HITRATE, nAdjHitRate, NULL_CHGPARAM );
		m_wDarkCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
		if( nAdjHitRate == -1 )
			Error( "SetDark : %s ���ȿ���� ���߷� ����ġ�� �������� ����", GetName() );
		m_wDarkVal = nAdjHitRate;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_DARK )
		{
			ResetDestParam( DST_CHRSTATE, CHS_DARK, TRUE );		// �����������
			ResetDestParam( DST_ADJ_HITRATE, m_wDarkVal, TRUE );	// ���浿�� �����ƴ� ���߷� ����
//			if( m_wDarkCnt >= 0 )
//				Error( "SetDark : %d", m_wDarkVal );
		}
		m_wDarkCnt = 0;
		m_wDarkVal = 0;
	}
	return TRUE;
}

// this�� ������ ����Ų��. ���� ���.
// tmMaxTime : 1/1000�� ����.
// tmUnit : ���� ���� �������� ���ϲ���.
// nDamage : ��������
BOOL	CMover::SetBleeding( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_BLEEDING ) return FALSE;		// ������ �鿪���´�.
		
		SetDestParam( DST_CHRSTATE, CHS_BLEEDING, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			Error( "SetBleeding : %s ����ȿ���� ���ӽð��� �������� ����", GetName() );
		if( tmUnit == -1 )
			Error( "SetBleeding : %s ����ȿ���� tick �ð��� �������� ����", GetName() );
		if( wDamage == -1 )
			Error( "SetBleeding : %s ����ȿ���� ƽ�� �������� �������� ����", GetName() );
		
		m_wBleedingCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wBleedingDamage = wDamage;
		m_tmBleedingUnit = tmUnit;
		m_idBleedingAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_BLEEDING )
			ResetDestParam( DST_CHRSTATE, CHS_BLEEDING, TRUE );	// ������ ����
		m_wBleedingCnt = 0;
		m_wBleedingDamage = 0;
		m_tmBleedingUnit = 0;
	}
	return TRUE;
}

// dwState�� ������
// CHS_ALL�� �ϸ� ��� �����.
void CMover::RemoveDebuff( DWORD dwState )
{
	// ��ų�� �ɸ��� �̰ɷ� �����ؾ��ϰ�
	RemoveChrStateBuffs( dwState );
	// ��ų�� �ƴ� �����۰����ɷ� ���¸� �ɸ��� �̰ɷ� Ǯ��� �Ѵ�.
	if( dwState & CHS_DARK )
		SetDark( FALSE );
	if( dwState & CHS_POISON )
		SetPoison( FALSE );
	if( dwState & CHS_STUN )
		SetStun( FALSE );
	if( dwState & CHS_BLEEDING )
		SetBleeding( FALSE );
	
	if( dwState & CHS_DEBUFFALL )
	{
		RemoveDstParamBuffs( DST_SPEED );
		RemoveDstParamBuffs( DST_CHR_CHANCEPOISON );
		RemoveDstParamBuffs( DST_CHR_CHANCESTUN );
		RemoveDstParamBuffs( DST_CHR_CHANCEBLEEDING );
	}


	// �̰� �ȿ��� Reset�� ȣ���Ѵ�.
	SetDestParam( DST_CURECHR, dwState, NULL_CHGPARAM );
}

// this���� �ɸ� ������ 1�� ���ش�.
void CMover::RemoveBuffOne( DWORD dwSkill )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_ONCE | RBF_GOODSKILL, 0 );
#else	// __BUFF_1107
	m_SkillState.RemoveOneSkillBuff();		// �̷ο������ų �ϳ��� ���� �Ѵ�.
#endif	// __BUFF_1107
}	

void CMover::RemoveBuffAll( void )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_GOODSKILL, 0 );
#else	// __BUFF_1107
	m_SkillState.RemoveAllSkillBuff();		// �̷ο������ų�� ��� ã�� ���� �Ѵ�.
#endif	// __BUFF_1107
}




///////////////////////////////////////////////////////////////////////////////
// 
// ��, ����� �߰��ɷ�ġ�� ������ �̰ɷ� ����.
void CMover::SetDestParam(const int nIdx, const ItemProp & pProp, const BOOL bSend) {
	SetDestParam(pProp.dwDestParam[nIdx], pProp.nAdjParamVal[nIdx], pProp.dwChgParamVal[nIdx], bSend);
}

void CMover::ResetDestParam(const int nIdx, const ItemProp & pProp, const BOOL bSend) {
	ResetDestParam(pProp.dwDestParam[nIdx], pProp.nAdjParamVal[nIdx], bSend);
}

// ��ǥ �Ķ���͸� ���Ѵ�.
void CMover::SetDestParam(int nItemIdx, BOOL bSend) {
	const ItemProp * const pItemProp = prj.GetItemProp(nItemIdx);

	for (int i = 0; i != ItemProp::NB_PROPS; ++i) {
		SetDestParam(pItemProp->dwDestParam[i], pItemProp->nAdjParamVal[i], pItemProp->dwChgParamVal[i], bSend);
	}
}

#ifdef __WORLDSERVER
void CMover::SetDestParam(const ItemProp & itemProp) {
	for (int i = 0; i != ItemProp::NB_PROPS; ++i) {
		SetDestParam(itemProp.dwDestParam[i], itemProp.nAdjParamVal[i], itemProp.dwChgParamVal[i], TRUE);
	}
}

void CMover::ResetDestParam(const ItemProp & itemProp) {
	for (int i = 0; i != ItemProp::NB_PROPS; ++i) {
		ResetDestParam(itemProp.dwDestParam[i], itemProp.nAdjParamVal[i], TRUE);
	}
}
#endif

//--------------------------------------------------------------------------
//
// ��ǥ �Ķ��Ÿ�� ���Ѵ�.
void CMover::SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL bSend )
{
	BOOL fSuccess	= FALSE;

	if( (nDstParameter < 0 ) && ( nDstParameter != DST_ADJDEF_RATE && nDstParameter != DST_ADJDEF ) )
		return;
	
	// HP, MP���� SetPointParam���� ó���Ѵ�. �ٵ� SetDestParam �ϳ��� ���Ͻ������� ���� ������?
	switch( nDstParameter )
	{
#ifdef __WORLDSERVER
	case DST_HP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxHitPoint() );
			else
				SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );
		}
		return;
	case DST_MP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxManaPoint() );
			else
				SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );
			
		}
		return;
	case DST_FP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxFatiguePoint() );
			else
				SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );
		}
		return;
#else // __WORLDSERVER
	case DST_HP:	SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );	return;
	case DST_MP:	SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );	return;
	case DST_FP:	SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );	return;
#endif // __WORLDSERVER
	case DST_GOLD:	
#ifdef __PERIN_BUY_BUG	// chipi_091219 �߰�
		Error( "SetDestParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]", static_cast<CUser*>(this)->GetName(), static_cast<CUser*>(this)->m_idPlayer, nAdjParameterValue );
		return;
#endif // __PERIN_BUY_BUG

		if( AddGold( nAdjParameterValue ) == FALSE )	
			SetGold( 0 );		// underflow�̸� 0�� �����. �����ڵ�� ȣȯ�ǰ� �Ѵ�. ���� ���� 
		return;

	case DST_RESIST_ALL:
		SetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_FIRE,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WIND,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WATER,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_EARTH,			nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_STAT_ALLUP:
		SetDestParam( DST_STR,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_DEX,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_INT,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_STA,	nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		SetDestParam( DST_HP_MAX, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_CHR_DMG, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		SetDestParam( DST_ADJDEF, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_LOCOMOTION:
		SetDestParam( DST_SPEED,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), nChgParameterValue, bSend );
		return;
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		SetDestParam( DST_MASTRY_EARTH,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_FIRE,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_WATER,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_ELECTRICITY,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_WIND,	nAdjParameterValue, nChgParameterValue, bSend );
		return;
#endif	// __NEWWPN1024

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			SetDestParam( DST_HP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_MP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_FP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// MP
			nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// FP
			nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			SetDestParam( DST_KILL_HP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	} // switch

	// �� �˻縦 �� ������ �ȳְ� ����� �־��ĸ�
	// DST_GOLD, PXP, RESIST_ALL�������� MAX_ADJPARAMARY���� �� ū���� ����Ѵ�. 
	// �׷��� �׷��͵��� ������ ó���ϰ� �ٷ� �����ϰ�
	// �Ϲ����ΰ͵鸸 �������� �´�.
	if( nDstParameter >= MAX_ADJPARAMARY)
		return;
	
	if( nAdjParameterValue != 0 )	// 0�� ���°͵� �߸��ȰŴϱ� ����� ���� ���� ������ �ʹ�.
	{
		fSuccess	= TRUE;

		switch( nDstParameter )
		{
		case DST_CHRSTATE:		// ���º����� ����
		case DST_IMMUNITY:
			if( nAdjParameterValue != NULL_ID )
			{
				if( nAdjParameterValue == 0xffffffff )
				{
					Error( "SetDestParam : Adj == -1, %s", GetName() );
					return;
				}
				m_adjParamAry[nDstParameter] |= nAdjParameterValue;		// ��Ʈ�� �¿��� �ȴ�.
			} else
				fSuccess = FALSE;
			break;
		case DST_CURECHR:		// ��������
			ResetDestParam( DST_CHRSTATE, nAdjParameterValue, bSend );
			return;
		case DST_REFLECT_DAMAGE:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// �������� ��%�� �����ִ°�?
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// ��%�� Ȯ���� �ߵ��Ǵ°�?
			break;
		case DST_CHR_CHANCEBLEEDING:
		case DST_CHR_CHANCESTEALHP:
		case DST_CHR_CHANCEPOISON:
		case DST_CHR_CHANCEDARK:
		case DST_CHR_CHANCESTUN:
		case DST_AUTOHP:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// ���� Ȯ��.	// %�̸����� ȸ���ǰ� �Ҳ���.
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// ������ �ɷ����� ���ӽð�		// %HP�� ȸ���ɲ���.
			break;
		case DST_HEAL:
			m_nHealCnt	= (short)( PROCESS_COUNT * 6.0f );		// ��Ÿ�� 6��
			m_adjParamAry[nDstParameter]	+= nAdjParameterValue;	
			break;
		default:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// ����
			break;
		}
	}

	else if( nChgParameterValue != 0x7FFFFFFF ) 
	{
		fSuccess	= TRUE;

		// ��
		if( nDstParameter == DST_SPEED )
		{
			if( m_chgParamAry[nDstParameter] == 0x7FFFFFFF )
				m_chgParamAry[nDstParameter]	= 0;
			m_chgParamAry[nDstParameter]++;
		}
		else
		{
			m_chgParamAry[nDstParameter] = nChgParameterValue;	// ������ ��.
		}
	}
#ifdef __WORLDSERVER
	if( TRUE == ( fSuccess & bSend ) ) {
		g_UserMng.AddSetDestParam( this, nDstParameter, nAdjParameterValue, nChgParameterValue );
	}
#endif	// __WORLDSERVER
}

//
// ��ǥ �Ķ��Ÿ�� �����Ѵ�.
//
void CMover::ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend )
{
	switch( nDstParameter )
	{
	case DST_RESIST_ALL:
		ResetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_FIRE,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WIND,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WATER,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_EARTH,		nAdjParameterValue, TRUE );
		return;
	case DST_STAT_ALLUP:
		ResetDestParam( DST_STR,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_DEX,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_INT,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_STA,	nAdjParameterValue, TRUE );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		ResetDestParam( DST_HP_MAX, nAdjParameterValue, TRUE );
		ResetDestParam( DST_CHR_DMG, nAdjParameterValue, TRUE );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		ResetDestParam( DST_ADJDEF, nAdjParameterValue, TRUE );
		ResetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, TRUE );
		return;
	case DST_LOCOMOTION:
		ResetDestParam( DST_SPEED,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), TRUE );
		return;
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		ResetDestParam( DST_MASTRY_EARTH,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_FIRE,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_WATER,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_ELECTRICITY,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_WIND,	nAdjParameterValue, TRUE );
		return;
#endif	// __NEWWPN1024

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			ResetDestParam( DST_HP_RECOVERY, nAdjParameterValue, TRUE );
			// MP
			ResetDestParam( DST_MP_RECOVERY, nAdjParameterValue, TRUE );
			// FP
			ResetDestParam( DST_FP_RECOVERY, nAdjParameterValue, TRUE );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			ResetDestParam( DST_KILL_HP, nAdjParameterValue, TRUE );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, TRUE );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, TRUE );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// ������ ������ ���� ���� �������� ��
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
			// MP
			nMax	= GetMaxOriginManaPoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// ������ ������ ���� ���� �������� ��
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, TRUE );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, TRUE );
		}
	}


	// �� �˻縦 �� ������ �ȳְ� ����� �־��ĸ�
	// DST_GOLD, PXP, RESIST_ALL�������� MAX_ADJPARAMARY���� �� ū���� ����Ѵ�. 
	// �׷��� �׷��͵��� ������ ó���ϰ� �ٷ� �����ϰ�
	// �Ϲ����ΰ͵鸸 �������� �´�.
	
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		Error( "ResetDestParam %s : �߸��� �� nDstParam : %d", m_szName, nDstParameter );
		return;
	}
	if( nAdjParameterValue != 0 ) 
	{
		if( nDstParameter == DST_CHRSTATE )
			m_adjParamAry[nDstParameter] &= (~nAdjParameterValue);		// �ش��Ʈ ����.
		else
			m_adjParamAry[nDstParameter] += (-nAdjParameterValue);
	}

	// ��
	if( nAdjParameterValue == 0 )	//
	{
		if( nDstParameter == DST_SPEED && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
		{
			if( --m_chgParamAry[nDstParameter] == 0 )
				m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
		else
		{
			m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
	}
#ifdef __WORLDSERVER
	if( fSend == TRUE ) 
	{
	#ifdef	__SPEED_SYNC_0108 // ResetDestParam speed ����
		if( nDstParameter == DST_SPEED )
			g_UserMng.AddResetDestParamSync( this, nDstParameter, nAdjParameterValue, (int)m_adjParamAry[nDstParameter] );
		else
			g_UserMng.AddResetDestParam( this, nDstParameter, nAdjParameterValue );
	#else	// 	__SPEED_SYNC_0108 ResetDestParam speed ����
		g_UserMng.AddResetDestParam( this, nDstParameter, nAdjParameterValue );
	#endif	// 	__SPEED_SYNC_0108 ResetDestParam speed ����
	}
#endif	// __WORLDSERVER
}

#ifdef __SPEED_SYNC_0108		// ResetDestParam speed ����
void CMover::ResetDestParamSync( int nDstParameter,int nAdjParameterValue, int nParameterValue, BOOL fSend )
{
#ifdef __CLIENT
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		Error( "ResetDestParam %s : �߸��� �� nDstParam : %d", m_szName, nDstParameter );
		return;
	}

	switch( nDstParameter )
	{
		case DST_SPEED:
			{
				m_adjParamAry[nDstParameter] = nParameterValue;
				if( nAdjParameterValue == 0 && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
				{
					if( --m_chgParamAry[nDstParameter] == 0 )
						m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
				}
			}
			break;
	}
#endif	// __CLIENT
}
#endif // __SPEED_SYNC_0108		// ResetDestParam speed ����

// ���� �Ķ���͸� ��´�.
int CMover::GetAdjParam(const int nDestParameter) const {
	if (nDestParameter < MAX_ADJPARAMARY)
		return m_adjParamAry[nDestParameter];
	return 0;
}

// ��ü �Ķ���͸� ��´�.
int CMover::GetChgParam(const int nDestParameter) const {
	if (nDestParameter < MAX_ADJPARAMARY)
		return m_chgParamAry[nDestParameter];
	return 0x7FFFFFFF;
}

// �Ķ���͸� ��´�.
int CMover::GetParam(const int nDest, const int nParam) const {
	int nChgParam = GetChgParam( nDest );
	if( nChgParam != 0x7FFFFFFF ) 
		return nChgParam;

	int nAdjParam = GetAdjParam( nDest );
	if( nAdjParam )	
		return nParam + nAdjParam;
	else
		return nParam;
}

/////////////////////////////////////////////////////////////////////////////
// ������� �Ķ��Ÿ ��� �Լ��� 
// GetParam�� ����Ͽ� ������ ���� ���ش�.
//
// ��Ʈ ����Ʈ ��� 
int CMover::GetHitPoint()
{
	return GetParam( DST_HP, m_nHitPoint );
}
// ���� ����Ʈ ��� 
int CMover::GetManaPoint()
{
#ifdef __EVENT1206
	if( IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) )  // raiders_fix051217
		return GetMaxManaPoint();
#endif	// __EVENT1206
	return GetParam( DST_MP, m_nManaPoint );
}
// ����Į ����Ʈ ���
int CMover::GetFatiguePoint()
{
#ifdef __EVENT1206
	if(  IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) )	// raiders_fix051217
		return GetMaxFatiguePoint();
#endif	// __EVENT1206
	return GetParam( DST_FP, m_nFatiguePoint );
}

// �ִ� ��Ʈ ����Ʈ ��� 
int CMover::GetMaxHitPoint() {
	if( IsInvalidObj(this) )	
		return 0;

	float factor = 1.0f;
	int nResult = GetParam( DST_HP_MAX, GetMaxOriginHitPoint( FALSE ) );
	int nFactor = GetParam( DST_HP_MAX_RATE, 0 );

	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult		= 1;
#endif	// __JEFF_11
	return nResult; 
}

// �ִ� ���� ����Ʈ ��� 
int CMover::GetMaxManaPoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_MP_MAX, GetMaxOriginManaPoint( FALSE ) );
	int nFactor = GetParam( DST_MP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult		= 1;
#endif	// __JEFF_11
	return nResult; 
}

// �ִ� �Ƿ� ����Ʈ ���
int CMover::GetMaxFatiguePoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_FP_MAX, GetMaxOriginFatiguePoint( FALSE ) );
	int nFactor = GetParam( DST_FP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult		= 1;
#endif	// __JEFF_11
	return nResult; 
}

int CMover::GetMaxPoint(int nDest)
{
	switch( nDest )
	{
		case DST_HP:
			return GetMaxHitPoint();
		case DST_MP:
			return GetMaxManaPoint();
		case DST_FP:
			return GetMaxFatiguePoint();
	}
	
	return 0;
}

int CMover::GetMaxOriginHitPoint( BOOL bOriginal )
{
	if( IsInvalidObj(this) )	
		return 0;

	if( IsPlayer() )
	{
		int nSta;
		if( bOriginal )
			nSta = m_nSta;
		else
			nSta = GetSta();

		JobProp* pProperty = prj.GetJobProp( GetJob() ); 

		float a = (pProperty->fFactorMaxHP*m_nLevel)/2.0f;
		float b = a * ((m_nLevel+1.0f)/4.0f) * (1.0f + nSta/50.0f) + (nSta*10.0f) ;
		float maxHP = b + 80.f; 
		return (int)maxHP;
	}
	else
	{
		MoverProp *pMoverProp = GetProp();
#ifdef __S1108_BACK_END_SYSTEM
		return int( pMoverProp->dwAddHp * prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate ); 
#else // __S1108_BACK_END_SYSTEM
		return pMoverProp->dwAddHp; 
#endif // __S1108_BACK_END_SYSTEM
	}

	return 0;
}

int CMover::GetMaxOriginManaPoint( BOOL bOriginal )
{
	int nInt = 0;
	if( bOriginal )
		nInt = m_nInt;
	else
		nInt = GetInt();

	if( IsPlayer() )
	{
		// INT((((BaseLv*2) + (INT*8))*Job���) + 22)+(INT*Job���)) +  �� 
		// �� : �����ۿ����� �߰� ��� (%����) TDDO
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxMP;
		
		int nMaxMP = (int)( ((((m_nLevel*2.0f) + ( nInt*8.0f))*factor) + 22.0f)+( nInt*factor) );
		return nMaxMP;
	}
	return 	( ( m_nLevel * 2 ) + ( nInt * 8 ) + 22 );
}

int CMover::GetMaxOriginFatiguePoint( BOOL bOriginal )
{
	int nSta, nStr, nDex;
	if( bOriginal )
	{
		nSta = m_nSta;
		nStr = m_nStr;
		nDex = m_nDex;
	}
	else
	{
		nSta = GetSta();
		nStr = GetStr();
		nDex = GetDex();
	}

	if( IsPlayer() )
	{
		// (((BaseLv*2) + (STA*6))*Job���) +(STA*Job���) + �� + ��
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxFP;

		int nMaxFP = (int)( (((m_nLevel*2.0f) + (nSta*6.0f))*factor) +(nSta*factor) );
		return nMaxFP;
	}
	
	return ( ( m_nLevel * 2 ) + ( nStr * 7 ) + ( nSta *2 ) + ( nDex * 4 ) );
}

int CMover::GetHitPointPercent( int nPercent )
{
	int nMax = GetMaxHitPoint();
	if( nMax == 0 ) return 0;
//	return GetHitPoint() * nPercent / nMax;
	return MulDiv( GetHitPoint(), nPercent, nMax );
}
int CMover::GetManaPointPercent( int nPercent )
{
	int nMax = GetMaxManaPoint();
	if( nMax == 0 ) return 0;
//	return GetManaPoint() * nPercent / GetMaxManaPoint();
	return MulDiv( GetManaPoint(), nPercent, nMax );
}
int CMover::GetFatiguePointPercent( int nPercent )
{
	int nMax = GetMaxFatiguePoint();
	if( nMax == 0 ) return 0;
//	return GetFatiguePoint() * nPercent / GetMaxFatiguePoint();
	return MulDiv( GetFatiguePoint(),  nPercent, nMax );
}

// HP ȸ�� �� ���
/*
	HP Recovery : ȸ����	PC/NPC : int(((���緹�� + STA)/3.4)+2) + AddHpRecovery
	MP Recovery : ȸ����	PC/NPC : int(((���緹�� + INT)/4)+2) + Addecovery
	FP Recovery : ȸ����	PC/NPC : int(((���緹�� + STA + DEX)/4.5)+2) + AddHpRecovery

	HP Recovery : ȸ���ӵ�	PC/NPC : �޽�(����)�� 5�ʿ� �ѹ��� ȸ���ϸ� �޽����� �ʴ� ��Ȳ������ 12�ʿ� �ѹ��� ȸ���� - AddHpRecoveryTime
	MP Recovery : ȸ���ӵ�	PC/NPC : �޽�(����)�� 5�ʿ� �ѹ��� ȸ���ϸ� �޽����� �ʴ� ��Ȳ������ 12�ʿ� �ѹ��� ȸ���� - AddMpRecoveryTime
	FP Recovery : ȸ���ӵ�	PC/NPC : �޽�(����)�� 5�ʿ� �ѹ��� ȸ���ϸ� �޽����� �ʴ� ��Ȳ������ 12�ʿ� �ѹ��� ȸ���� - AddFpRecoveryTime

  */
int CMover::GetHPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorHPRecovery;
	}

	//(����/3)+ (MaxHP/ (500* ����))+(STA*Job���)
	int nValue = (int)( (GetLevel() / 3.0f) + (GetMaxHitPoint() / (500.f * GetLevel())) + (GetSta() * fFactor) );
	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // ȸ���� 10% ����
	return GetParam( DST_HP_RECOVERY, nValue );
}
// MP ȸ�� �� ���
int CMover::GetMPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorMPRecovery;
	}

	// ((����*1.5)+(MaxMP/(500*����))+(INT*Job���))*0.2
	int nValue = (int)( ((GetLevel() * 1.5f) + (GetMaxManaPoint() / (500.f * GetLevel())) + (GetInt() * fFactor)) * 0.2f );
	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // ȸ���� 10% ����
	return GetParam( DST_MP_RECOVERY, nValue );
}
// FP ȸ�� �� ��� 
int CMover::GetFPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorFPRecovery;
	}
	// ((����*2)+(MaxFP/(500*����))+(STA*Job���))*0.2
	int nValue = (int)( ((GetLevel() * 2.0f) + (GetMaxFatiguePoint() / (500.f * GetLevel())) + (GetSta() * fFactor)) * 0.2f );
	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // ȸ���� 10% ����
	return GetParam( DST_FP_RECOVERY, nValue );
}

#ifdef __CLIENT
// ���� �̸��� ��Ʈ������ �����ش�.
LPCTSTR CMover::GetFameName( void )
{
	static constexpr std::array<int, 10> acrobatTitles = {
		TID_GAME_ACR_FAME01, TID_GAME_ACR_FAME02, TID_GAME_ACR_FAME03, TID_GAME_ACR_FAME04,
		TID_GAME_ACR_FAME05, TID_GAME_ACR_FAME06, TID_GAME_ACR_FAME07, TID_GAME_ACR_FAME08,
		TID_GAME_ACR_FAME09, TID_GAME_ACR_FAME10
	};

	static constexpr std::array<int, 10> mercenaryTitles = {
		TID_GAME_MER_FAME01, TID_GAME_MER_FAME02, TID_GAME_MER_FAME03, TID_GAME_MER_FAME04,
		TID_GAME_MER_FAME05, TID_GAME_MER_FAME06, TID_GAME_MER_FAME07, TID_GAME_MER_FAME08,
		TID_GAME_MER_FAME09, TID_GAME_MER_FAME10
	};

	static constexpr std::array<int, 10> magicianTitles = {
		TID_GAME_MAG_FAME01, TID_GAME_MAG_FAME02, TID_GAME_MAG_FAME03, TID_GAME_MAG_FAME04,
		TID_GAME_MAG_FAME05, TID_GAME_MAG_FAME06, TID_GAME_MAG_FAME07, TID_GAME_MAG_FAME08,
		TID_GAME_MAG_FAME09, TID_GAME_MAG_FAME10
	};

	static constexpr std::array<int, 10> assistTitles = {
		TID_GAME_ASS_FAME01, TID_GAME_ASS_FAME02, TID_GAME_ASS_FAME03, TID_GAME_ASS_FAME04,
		TID_GAME_ASS_FAME05, TID_GAME_ASS_FAME06, TID_GAME_ASS_FAME07, TID_GAME_ASS_FAME08,
		TID_GAME_ASS_FAME09, TID_GAME_ASS_FAME10
	};

	static constexpr std::array<int, 10> thresholds = {
		10, 100, 1000, 4000, 10000, 20000, 50000, 1000000, 5000000, 100000000
	};

	size_t i = 0;
	while (i < thresholds.size() && m_nFame >= thresholds[i]) {
		++i;
	}

	if (i == 0) return "";
	
	switch (prj.GetProJob(m_nJob)) {
		case CProject::ProJob::Acrobat:   return prj.GetText(acrobatTitles[i - 1]);
		case CProject::ProJob::Mercenary: return prj.GetText(mercenaryTitles[i - 1]);
		case CProject::ProJob::Magician:  return prj.GetText(magicianTitles[i - 1]);
		case CProject::ProJob::Assist:    return prj.GetText(assistTitles[i - 1]);
		default:                          return "";
	}
}

#endif


// ���� ����� ��� 
int CMover::GetResistMagic()
{
	return GetParam( DST_RESIST_MAGIC, 0 );
}
int CMover::GetResistSpell( int nDestParam )
{
	MoverProp *pProp = GetProp();
	if( pProp == NULL )
		Error( "CMover::GetReistSpell : %d ������Ƽ �б� ����", GetName() );

	int		nResist = 0;

	switch( nDestParam )
	{
	case DST_RESIST_ELECTRICITY:	nResist = pProp->nResistElecricity;		break;
	case DST_RESIST_FIRE:			nResist = pProp->nResistFire;		break;
	case DST_RESIST_WATER:			nResist = pProp->nResistWater;		break;
	case DST_RESIST_EARTH:			nResist = pProp->nResistEarth;		break;
	case DST_RESIST_WIND:			nResist = pProp->nResistWind;		break;
	default:
		Error( "CMover::GetResistSpell : %s �Ķ���� �߸��� %d", GetName(), nDestParam );
		break;
	}
	return GetParam( nDestParam, nResist );
}

int CMover::GetNaturalArmor()
{
	return GetProp()->dwNaturalArmor;//GetParam(DST_NATURAL_ARMOR,GetProp()->m_dwNaturalArmor);
}

int CMover::GetStr() const {
	int nResult = m_nStr + GetParam( DST_STR, 0 );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetDex() const {
	int nResult = m_nDex + GetParam( DST_DEX, 0 );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetInt() const {
	int nResult = m_nInt + GetParam( DST_INT, 0 );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}

int CMover::GetSta() const {
	int nResult = m_nSta + GetParam( DST_STA, 0 );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}

FLOAT CMover::GetSpeed(FLOAT fSrcSpeed)
{
#ifdef __CLIENT
	if( m_dwAIInterface == AII_EGG && m_pAIInterface )
	{
		CAIEgg* pAI	= (CAIEgg*)m_pAIInterface;
		CMover* pOwner	= prj.GetMover( pAI->GetOwnerId() );
		if( IsValidObj( pOwner ) )
			return pOwner->GetSpeed( pOwner->m_pActMover->m_fSpeed );
	}
#endif	// __CLIENT

	fSrcSpeed	*= GetSpeedFactor();

	if( m_fCrrSpd != 0.0f )
		return m_fCrrSpd;
	int nChgValue	= GetChgParam( DST_SPEED );
//	if( nChgValue != 0x7FFFFFFF )
//		return fSrcSpeed * (nChgValue / 100.0f);		// ex) fSrcSpeed * 0.5f

	// ��
	if( nChgValue != 0x7FFFFFFF )
		return 0;

	int nAdjValue	= GetAdjParam( DST_SPEED );
	if( nAdjValue != 0 )
	{
#ifdef __JEFF_11
		fSrcSpeed = fSrcSpeed + ( fSrcSpeed * (nAdjValue / 100.0f) );
		if( fSrcSpeed < 0.0F )
			fSrcSpeed	= 0.0F;
#endif	// __JEFF_11
		return fSrcSpeed;
	}
#ifdef __JEFF_11
	if( fSrcSpeed < 0.0F )
		fSrcSpeed	= 0.0F;
#endif	// __JEFF_11

	return fSrcSpeed;
}




//
// ���� ���ۻ��¸� ������ ����
// ����ȭ �ܴ̿� ������� ����.
//

// ���� ������ ���� ����ȭ �ϰ� �̵� ������ �����Ѵ�.
void CMover::BehaviorActionForceSet( void )
{
	if( m_CorrAction.fValid && m_CorrAction.fBehavior == TRUE ) {
		if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
			ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f,
								m_CorrAction.fAngleX, m_CorrAction.fAccPower, m_CorrAction.fTurnAngle,
								m_CorrAction.dwState, m_CorrAction.dwStateFlag,
								m_CorrAction.dwMotion, m_CorrAction.nMotionEx, m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
		else {
			ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f,
								m_CorrAction.dwState, m_CorrAction.dwStateFlag,
								m_CorrAction.dwMotion, m_CorrAction.nMotionEx, m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
	}
	memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
}

void CMover::ActionForceSet( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
							 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption )
{
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	CModelObject* pModel = (CModelObject*)m_pModel;
#ifdef __SYNC_1217
	ClearDestPos();
#else	// __SYNC_1217
	ClearDest();
#endif	// __SYNC_1217
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	m_pActMover->ForcedSet( vDelta, dwState, dwStateFlag, nMotionEx );
	m_dwMotion = -1;
	SetMotion( nMotion, nLoop, dwMotionOption );
	pModel->SetMotionBlending( FALSE );
}

void CMover::ActionForceSet2( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
							 FLOAT fAngleX, FLOAT fAccPower, FLOAT fTurnAngle,
							 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption )
{
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	CModelObject* pModel = (CModelObject*)m_pModel;
	ClearDest();
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	SetAngleX( fAngleX );
	m_pActMover->ForcedSet2( vDelta, fAccPower, fTurnAngle, dwState, dwStateFlag, nMotionEx );
	m_dwMotion = -1;
#ifdef __WORLDSERVER	
	if( nMotion == 31 && (dwMotionOption & MOP_FIXED) == 0 )
		dwMotionOption |= MOP_FIXED;	
#endif

	SetMotion( nMotion, nLoop, dwMotionOption );
	pModel->SetMotionBlending( FALSE );
}

void CMover::HalfForceSet( D3DXVECTOR3 & vPos, D3DXVECTOR3 & vd, float fAngle, float fAngleX, float fAccPower, float fTurnAngle )
{
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	ClearDest();
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	SetAngleX( fAngleX );
	m_pActMover->HalfForcedSet( vd, fAccPower, fTurnAngle );
}

BOOL CMover::IsCompleteQuest(const QuestId nQuestId) const {
	return (m_quests && m_quests->IsCompletedQuest(nQuestId)) ? TRUE : FALSE;
}

bool MoverSub::Quests::IsCompletedQuest(const QuestId questId) const {
	const auto it = std::ranges::find(completed, questId);
	return it != completed.end();
}

std::optional<BYTE> CMover::GetQuestState(const QuestId nQuestId) const {
	if (!m_quests) return std::nullopt;

	const auto inCurrent = std::ranges::find_if(
		m_quests->current, MoverSub::Quests::ById(nQuestId)
	);
	if (inCurrent != m_quests->current.end()) {
		return inCurrent->m_nState;
	}

	if (m_quests->IsCompletedQuest(nQuestId)) {
		return QS_END;
	}

	return std::nullopt;
}

void MoverSub::Quests::Clear() {
	current.clear();
	completed.clear();
	checked.clear();
}

void CMover::RemoveAllQuest() {
	if (m_quests) m_quests->Clear();
}

void CMover::RemoveCompleteQuest() {
	if (m_quests) m_quests->completed.clear();
}


QUEST * CMover::FindQuest(const QuestId nQuestId) {
	if (!m_quests) return nullptr;
	return m_quests->FindQuest(nQuestId);
}

QUEST * MoverSub::Quests::FindQuest(const QuestId questId) {
	const auto it = std::ranges::find_if(current, ById(questId));
	if (it == current.end()) return nullptr;
	return &*it;
}

void MoverSub::Quests::RemoveQuest(const QuestId questId) {
	const auto itCurrent = std::ranges::find_if(current, ById(questId));
	if (itCurrent != current.end()) current.erase(itCurrent);

	const auto itCompleted = std::ranges::find(completed, questId);
	if (itCompleted != completed.end()) completed.erase(itCompleted);

	const auto itChecked = std::ranges::find(checked, questId);
	if (itChecked != checked.end()) checked.erase(itChecked);
}

BOOL CMover::RemoveQuest(const QuestId nQuestId ) {
	if (m_quests) m_quests->RemoveQuest(nQuestId);

#ifdef __CLIENT
	PlayMusic( BGM_EV_END );
#endif
	return TRUE;
}

BOOL CMover::SetQuest(const QuestId nQuestId, int nState, LPQUEST lpReturnQuest )
{
	QUEST newQuest;
	ZeroMemory( &newQuest, sizeof( QUEST ) );
	newQuest.m_wId = nQuestId;
	newQuest.m_nState = nState;

	LPQUEST lpQuest = FindQuest( nQuestId );
	// �̹� �Ϸ�� �ɽ�Ʈ��� ���� ���� 
	if( lpQuest == NULL && IsCompleteQuest( newQuest.m_wId ) )
		return FALSE;
	if( lpReturnQuest )
		memcpy( lpReturnQuest, &newQuest, sizeof( QUEST ) );
	return __SetQuest( lpQuest, &newQuest );
}
BOOL CMover::SetQuest( LPQUEST lpNewQuest )
{
	LPQUEST lpQuest = FindQuest( lpNewQuest->m_wId );
	// �̹� �Ϸ�� �ɽ�Ʈ��� ���� ���� 
	if( lpQuest == NULL && IsCompleteQuest( lpNewQuest->m_wId ) )
		return FALSE;
	return __SetQuest( lpQuest, lpNewQuest );
}
BOOL CMover::__SetQuest( LPQUEST lpQuest, LPQUEST lpNewQuest )
{
	if (!m_quests) return FALSE;

	// ����Ʈ�� �߰� �������� ���� �߰��Ѵ�.
	if( lpQuest == NULL )
	{
		if( m_quests->current.size() >= MAX_QUEST)
		{
			Error("SetQuestCnt : ����Ʈ �ʰ�");
			return FALSE;
		}
#ifdef __CLIENT
		PlayMusic( BGM_EV_START );
#endif
		if( m_quests->completed.size()  >= MAX_COMPLETE_QUEST)
			return FALSE;

		lpQuest = &m_quests->current.emplace_back();

		const QuestProp * pQuestProp = lpNewQuest->GetProp();
		if( pQuestProp && pQuestProp->m_nEndCondLimitTime  )
			lpNewQuest->m_wTime = pQuestProp->m_nEndCondLimitTime;
#ifdef __WORLDSERVER
		// Ÿ�̸� �ʱ�ȭ 
		m_timerQuestLimitTime.Reset();
#endif
	}
	else
	// ����Ʈ �Ϸ��. �׷��ٸ� ĳ������ ������ �������� 
	if( lpNewQuest->m_nState == QS_END )
	{
		// ���, �Ϸ� �˽�Ʈ �迭�� �־� �����Ѵ�.
		if (m_quests->completed.size() >= MAX_COMPLETE_QUEST) {
			return FALSE;
		}

		const auto itCurrent = std::ranges::find_if(m_quests->current,
			MoverSub::Quests::ById(lpNewQuest->m_wId)
			);
		if (itCurrent != m_quests->current.end()) {
			m_quests->current.erase(itCurrent);
		}

		const auto itChecked = std::ranges::find(m_quests->checked,
			lpNewQuest->m_wId
		);
		if (itChecked != m_quests->checked.end()) {
			m_quests->checked.erase(itChecked);
		}

		// ����Ʈ Ÿ���� �ǷڼҶ�� �Ϸ� �迭�� ���� �ʴ´�. 
		const QuestProp * pProp = lpNewQuest->GetProp();
		if (pProp && !pProp->m_bRepeat) {
			m_quests->completed.emplace_back(lpNewQuest->m_wId);
		}
		
		lpQuest = nullptr;
#if defined( __CLIENT )
			g_QuestTreeInfoManager.DeleteTreeInformation( lpNewQuest->m_wId.get() );
			D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
			rDestinationArrow = D3DXVECTOR3( -1.0F, 0.0F, -1.0F );
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )
		
	}

	if (lpQuest) memcpy(lpQuest, lpNewQuest, sizeof(QUEST));

#ifdef __CLIENT
	if (lpNewQuest->m_nState == QS_END) {
		PlayMusic(BGM_EV_END);
	}
#endif
	return TRUE;
}

int CMover::GetItemNum( DWORD dwItemId )
{
	int nNum	= 0;
	int nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	for( int i = 0 ; i < nSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			//raiders.06.05.02 (prevent abuse quest ending condition : trade & quest )
			if( !IsUsableItem( pItemElem ) )
				return 0;
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}

#ifdef __CLIENT
int CMover::GetItemNumForClient( DWORD dwItemId )
{
	int nNum	= 0;
	int nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	for( int i = 0 ; i < nSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}
#endif //__CLIENT
#ifdef __WORLDSERVER
// ��ũ��Ʈ���� ����ϴ� �Լ�
// ����Ʈ���� �������� ������ �� ����Ѵ�. ����Ʈ �����۵��� �ŷ��� ���λ����� �ø� �� ���� ������
// IsUsableItem�˻縦 ���� �ʴ´�.
int CMover::RemoveItemA( DWORD dwItemId, short nNum )
{
	if( nNum == -1 )
	{
		return RemoveAllItem( dwItemId );
	}

	int	nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	short nRemnant	= nNum;

	for( int i = 0 ; i < nSize && nRemnant > 0; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			if( nRemnant > pItemElem->m_nItemNum )
			{
				UpdateItem( *pItemElem, UI::Num::RemoveAll() );	// remove
				nRemnant	-= pItemElem->m_nItemNum;
			}
			else
			{
				UpdateItem(*pItemElem, UI::Num::Consume(nRemnant));
				nRemnant	= 0;
			}
		}
	}
	return ( nNum - nRemnant );
}

// ��ũ��Ʈ���� ����ϴ� �Լ� 
int CMover::RemoveAllItem( DWORD dwItemId )
{
	int nNum = 0;
	int	nSize	= m_Inventory.GetMax();

	for( int i = 0 ; i < nSize; ++i )
	{
		CItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			nNum += pItemElem->m_nItemNum;
			UpdateItem(*pItemElem, UI::Num::RemoveAll);		// remove
		}
	}
	return nNum;
}
#endif	// __WORLDSERVER

void CMover::OnTradeRemoveUser()
{
	if( IsPlayer() )
	{
		CMover* pOther = m_vtInfo.GetOther();
		if( IsValidObj( pOther ) && pOther->IsPlayer() )
		{	
			if( pOther->m_vtInfo.GetOther() == this )
			{
				pOther->m_vtInfo.TradeClear();
			}
		}
		m_vtInfo.TradeClear();
	}
}

#ifdef __WORLDSERVER

void CMover::Abrade( CMover* pAttacker, int nParts )
{
}

DWORD CMover::GetRandomPartsAbraded( void )
{
//	�ǵ� 50%
	int nRand	= xRand();
	if( nRand % 2 )
		return PARTS_SHIELD;
	nRand	= nRand % 100;
	if( nRand < 20 )	// 20%
		return PARTS_CAP;
	else if( nRand < 60 )	// 40%
		return PARTS_UPPER_BODY;
	else if( nRand < 85 )	// 25%
		return PARTS_HAND;
	else	// 15%
		return PARTS_FOOT;
}
#endif	// __WORLDSERVER



float CMover::GetItemEnduranceInfluence( int nEndurance )
{
	if( nEndurance == 0 )
		return 0.0f;
/*	else if( nEndurance <= 50 )
		return 0.6f;
	else if( nEndurance <= 75 )
		return 0.8f;
*/	return 1.0f;
}

int CMover::GetItemEnduranceWeight( int nEndurance )
{
	int nMinusMultyPly = 5;
/*	if( nEndurance <= 50 )
		return (int)3 * nMinusMultyPly;
	else if( nEndurance <= 75 )
		return (int)2 * nMinusMultyPly;
*/	return 1 * nMinusMultyPly;
}

#ifdef __CLIENT
void CMover::UpdateItem(const ItemPos dwId, const UI::Variant & operation) {
	// Client updates comes from DPSrvr, item may not exist
	UpdateItem(GetItemId(dwId), dwId, operation);
}
#endif

#ifdef __WORLDSERVER
void CMover::UpdateItem(CItemElem & itemElem, const UI::Variant & operation) {
	// WorldServer must know the item it wants to edit
	UpdateItem(&itemElem, itemElem.m_dwObjId, operation);
}
#endif

template<class> inline constexpr bool always_false_v = false;
void CMover::UpdateItem(CItemElem * itemElem, ItemPos dwId, const UI::Variant & operation) {
	// In WorldServer: assert(itemElem);

	if (itemElem) {
		std::visit(
			[&](auto && arg) {
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_invocable_v<T, CItemElem &, CMover &>) {
					arg(*itemElem, *this);
				} else if constexpr (std::is_invocable_v<T, CItemElem &>) {
					arg(*itemElem);
				} else {
					static_assert(always_false_v<T>, "Did not found an operator()(CItemElem &)");
				}
			}, operation
		);
	} else {
#ifdef __CLIENT
		if (const UI::Flag * const flagOp = std::get_if<UI::Flag>(&operation)) {
			(*flagOp)(*this);
		}
#endif
	}

#ifdef __WORLDSERVER
	if (IsPlayer()) {
		static_cast<CUser *>(this)->SendSnapshotThisId<
			SNAPSHOTTYPE_UPDATE_ITEM_VARIANT, ItemPos, UI::Variant
		>(dwId, operation);

		if (std::holds_alternative<UI::PetVis::TransformToVisPet>(operation)) {
			if (HasActivatedEatPet()) {
				InactivateEatPet();
				ActivateEatPet(itemElem);
			}
		}
	}
#endif
}

namespace UI {
	//void RandomOptItem::operator()(CItemElem & itemElem) const {
	//	itemElem.SetRandomOptItemId(value);
	//}

	void RandomOptItemFromRandomScroll::operator()(CItemElem & itemElem) const {
		itemElem.SetRandomOpt(nKind);
  }

	Element Element::None() {
		return Element{ .kind = SAI79::ePropType::NO_PROP, .abilityOption = 0 };
	}

	Element Element::Increase(const CItemElem & itemElem) {
		return Element{
			.kind = itemElem.m_bItemResist,
			.abilityOption = itemElem.m_nResistAbilityOption + 1
		};
	}

	Element Element::Decrease(const CItemElem & itemElem) {
		return Element{
			.kind = itemElem.m_bItemResist,
			.abilityOption = itemElem.m_nResistAbilityOption - 1
		};
	}

	Element Element::Change(SAI79::ePropType target) {
		return Element{
			.kind = static_cast<BYTE>(target),
			.abilityOption = DoNotChangeAO
		};
	}

	Element Element::ActivatedQuest(const CItemElem & itemElem) {
		return Element{
			.kind = TRUE,
			.abilityOption = itemElem.m_nResistAbilityOption
		};
	}

	void Element::operator()(CItemElem & itemElem, CMover & mover) const {
		itemElem.m_bItemResist = kind;

		if (abilityOption != DoNotChangeAO) {
			int targetRAO = abilityOption;
			if (targetRAO < 0) targetRAO = 0;
			if (targetRAO > 20) targetRAO = 20;
			itemElem.m_nResistAbilityOption = targetRAO;
		}

		mover.UpdateParam();
	}

	namespace Piercing {

		Size Size::IncrementRegular(const CItemElem & itemElem) {
			Size p;
			p.kind = Kind::Regular;
			p.newSize = itemElem.GetPiercingSize() + 1;
			return p;
		}

		Size Size::Ultimate(const CItemElem & itemElem) {
			return Size{
				.kind = Kind::Ultimate,
				.newSize = itemElem.GetAbilityOption() - 5
			};
		}


		void Size::operator()(CItemElem & itemElem) const {
			switch (kind) {
				case Kind::Regular : itemElem.SetPiercingSize(newSize);         break;
				case Kind::Ultimate: itemElem.SetUltimatePiercingSize(newSize); break;
			}
		}

		void Item::operator()(CItemElem & itemElem) const {
			switch (kind) {
				case Kind::Regular : itemElem.SetPiercingItem(position, itemId);         break;
				case Kind::Ultimate: itemElem.SetUltimatePiercingItem(position, itemId); break;
			}
		}
	}

	namespace PetVis {
		void TransformToVisPet::operator()(CItemElem & itemElem) const {
			itemElem.m_bTranformVisPet = TRUE;
			itemElem.m_bCharged = TRUE;
		}

		// == Size

		Size Size::DefaultVis() {
			return Size { .newSize = PETVIS_DEFAULT_VIS_SLOT_SZIE };
		}

		Size Size::Increase(const CItemElem & itemElem) {
			return Size{ .newSize = itemElem.GetPiercingSize() + 1 };
		}

		void Size::operator()(CItemElem & itemElem) const {
			itemElem.SetPiercingSize(newSize);
			itemElem.SetVisKeepTimeSize(newSize);

#ifdef __CLIENT
			const auto pos = g_pPlayer->GetPos();
			PLAYSND(SND_INF_EQUIPACCESSORY, &pos);
#endif // __CLIENT
		}


		// Items

		void Item::operator()(CItemElem & itemElem) const {
			itemElem.SetPiercingItem(position, itemId);
			itemElem.SetVisKeepTime(position, (time * 60) + time_null());

#ifdef __CLIENT
			const auto pos = g_pPlayer->GetPos();
			PLAYSND(SND_INF_EQUIPACCESSORY, &pos);
#endif // __CLIENT
		}

		void ItemSwap::operator()(CItemElem & itemElem) const {
			itemElem.SetSwapVisItem(from, to);

#ifdef __CLIENT
			const auto pos = g_pPlayer->GetPos();
			PLAYSND(SND_INF_EQUIPACCESSORY, &pos);
#endif // __CLIENT
		}
	}

	KeepTime KeepTime::FromProp(const CItemElem & itemElem) {
		const ItemProp * prop = itemElem.GetProp();
		if (!prop) {
			Error("Item %lu has no props", itemElem.m_dwItemId);
			return KeepTime{ .minutes = 1 };
		}
		return KeepTime{ .minutes = prop->dwAbilityMin };
	}

	void KeepTime::operator()(CItemElem & itemElem) const {
		const CTime tm = CTime::GetCurrentTime() + CTimeSpan(0, 0, minutes, 0);
		itemElem.m_dwKeepTime = static_cast<DWORD>(tm.GetTime());
	}

	AbilityOption AbilityOption::Up(const CItemElem & itemElem) {
		return AbilityOption{ .newLevel = itemElem.GetAbilityOption() + 1 };
	}

	AbilityOption AbilityOption::Down(const CItemElem & itemElem) {
		return AbilityOption{ .newLevel = itemElem.GetAbilityOption() - 1 };
	}

	void AbilityOption::operator()(CItemElem & itemElem, CMover & mover) const {
		int dwMax = 20;
		if (itemElem.IsAccessory()) dwMax = 20;
		if (itemElem.IsCollector(TRUE)) dwMax = 5;
		if (itemElem.GetAbilityOption() > dwMax) return;

		const int target = newLevel <= dwMax ? newLevel : dwMax;
		itemElem.SetAbilityOption(target);
		mover.UpdateParam();
	}

	Num Num::ConsumeOne(const CItemElem & itemElem) {
		return Num{
			.newQuantity = static_cast<short>(itemElem.m_nItemNum - 1),
			.startCooldown = false
		};
	}

	Num Num::Sync(const CItemElem & itemElem) {
		return Num{
			.newQuantity = itemElem.m_nItemNum,
			.startCooldown = false
		};
	}

	void Num::operator()(CItemElem & itemElem, CMover & mover) const {
		if (newQuantity <= 0) {
			mover.RemoveItemId(itemElem.m_dwObjId);
		} else {
			itemElem.m_nItemNum = newQuantity;
		}
	}


	HitPoint HitPoint::Decrement(const CItemElem & itemElem) {
		const auto target = itemElem.m_nHitPoint > 0 ? itemElem.m_nHitPoint - 1 : 0;
		return HitPoint{ .newQuantity = target };
	}

	void HitPoint::operator()(CItemElem & itemElem, CMover & mover) const {
		itemElem.m_nHitPoint = newQuantity;
		mover.UpdateParam();
	}

	void Repair::operator()(CItemElem & itemElem, CMover & mover) const {
		itemElem.m_nHitPoint = newHitPoint;
		itemElem.m_nRepairNumber = repairNumber > 100 ? 100 : repairNumber;
		mover.UpdateParam();
	}

	Flag::Flag(const CItemElem & itemElem) {
		m_objIndex = itemElem.m_dwObjIndex;
		m_flags = itemElem.m_byFlag;
	}

	void Flag::operator()(CItemElem & itemElem, CMover & mover) const {
#ifdef __CLIENT
		int nParts = m_objIndex - mover.m_Inventory.GetSize();
		if (nParts >= 0 && nParts < MAX_HUMAN_PARTS)
			mover.m_aEquipInfo[nParts].byFlag = m_flags;
		itemElem.m_byFlag = m_flags;
		mover.UpdateParts(FALSE);
#endif
		mover.UpdateParam();
	}

#ifdef __CLIENT
	void Flag::operator()(CMover & mover) const {
		int nParts = m_objIndex - mover.m_Inventory.GetSize();
		if (nParts >= 0 && nParts < MAX_HUMAN_PARTS) {
			mover.m_aEquipInfo[nParts].byFlag = m_flags;
			mover.UpdateParts(TRUE);
		}
	}
#endif

	void ChangeItemId::operator()(CItemElem & itemElem, CMover & mover) const {
		// TODO: in WS, if the item is equiped, stats should be removed
		
		itemElem.m_dwItemId = m_newItemId;

#ifdef __CLIENT
		itemElem.SetTexture();
		// TODO: update taskbar
#endif

		// TODO: in WS, if the item is equiped, stats should be applied again
	}
}

float CMover::GetItemDropRateFactor( CMover* pAttacker )
{
	float fFactor	= 1.0F;
#ifdef __WORLDSERVER
	fFactor		*= prj.m_fItemDropRate;
#ifdef __S1108_BACK_END_SYSTEM
	fFactor		*= GetProp()->m_fItemDrop_Rate;
#endif // __S1108_BACK_END_SYSTEM
#ifdef __ITEMDROPRATE
	fFactor		*= CEventGeneric::GetInstance()->GetItemDropRateFactor();
#endif // __ITEMDROPRATE
	fFactor		*= prj.m_EventLua.GetItemDropRate();
#endif	// __WORLDSERVER
	return fFactor;
}

float CMover::GetPieceItemDropRateFactor( CMover* pAttacker )
{
	float fFactor	= 1.0F;
#ifdef __WORLDSERVER
	fFactor		*= prj.m_EventLua.GetPieceItemDropRate();
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetIFactor();
	if( pAttacker->HasBuff( BUFF_ITEM, II_SYS_SYS_LS_CHEERING ) )
		fFactor		*=	1.1F;
	if( pAttacker->HasBuff( BUFF_ITEM, II_SYS_SYS_VOTE_THANKS ) )
		fFactor		*= 1.05F;
	if( pAttacker->HasBuff( BUFF_ITEM, II_COUPLE_BUFF_MIRACLE_01 ) )
		fFactor	*= 1.05F;
	if( pAttacker->IsPlayer() )
		fFactor *= CPCBang::GetInstance()->GetPieceItemDropFactor( static_cast<CUser*>( pAttacker ) );
#endif // __WORLDSERVER
	return fFactor;
}

float CMover::GetExpFactor( void )
{
	float fFactor	= 1.0f;
	if( IsMode( EVENT_OLDBOY_MODE ) )
		fFactor	*= 1.1f;

	BOOL bGlvCheer	= FALSE;
	BOOL bSpecial = FALSE;

	int nCountBuf = 0;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESE ) )
		++nCountBuf;
#ifdef __S_ADD_EXP
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS ) )
	{
		++nCountBuf;	bSpecial = TRUE;
	}
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS1 ) )
	{
		++nCountBuf;	bSpecial = TRUE;
	}
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS2 ) )
	{
		++nCountBuf;	bSpecial = TRUE;
	}
#endif // __S_ADD_EXP

	int nLevel = GetLevel();
	if( nCountBuf == 1 )
	{
		fFactor *= 1.5f;
	}
	else if( nCountBuf == 2 )
	{
		if( bSpecial || nLevel < 61 )
			fFactor *= 2.0f;
		else
			fFactor *= 1.5f;
	}
	else if( 3 <= nCountBuf )
	{
		if( bSpecial || nLevel < 41 )
			fFactor *= 2.5f;
		else if( nLevel < 61 )
			fFactor *= 2.0f;
		else
			fFactor *= 1.5f;
	}

	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) )
		fFactor	*= 1.5f;
	
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ) )
		fFactor	*= 2.0f;

	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_GLVCHEER ) )
	{
		bGlvCheer	= TRUE;
		fFactor	*= 1.10f;
	}

	if( !bGlvCheer && HasBuff( BUFF_ITEM, II_CHEERUP ) )
	{
#ifdef __EVENTLUA_CHEEREXP
		if( prj.m_EventLua.GetCheerExpFactor() > 1.05f )
			fFactor *= prj.m_EventLua.GetCheerExpFactor();
		else
#endif // __EVENTLUA_CHEEREXP
			fFactor	*= 1.05f;
	}

#ifdef __WORLDSERVER

	fFactor	*= CEventGeneric::GetInstance()->GetExpFactor();
	fFactor	*= prj.m_EventLua.GetExpFactor();
	fFactor		*= prj.m_fMonsterExpRate;
	int nDstExp = GetParam( DST_EXPERIENCE, 0 );
	if( 0 < nDstExp )
		fFactor *= ( 1.0f + ( nDstExp / 100.0f ) );

	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetEFactor();
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_LS_CHEERING ) )
		fFactor		*=	1.1F;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_VOTE_THANKS ) )
		fFactor		*=	1.05F;

#ifdef __ENVIRONMENT_EFFECT

	if( CEnvironment::GetInstance()->GetEnvironmentEffect() == TRUE )
	{
		fFactor *= prj.m_EventLua.GetWeatherEventExpFactor();
	}

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
	if( g_Environment.m_bRain )
		fFactor *= prj.m_EventLua.GetRainEventExpFactor();
#else // __EVENTLUA_RAIN
#ifdef __RAIN_EVENT
	if( g_Environment.m_bRain )
		fFactor *= 2.0f;
#endif // __RAIN_EVENT
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	if( g_Environment.m_bSnow )
		fFactor *= prj.m_EventLua.GetSnowEventExpFactor();
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#endif	// __WORLDSERVER

	return fFactor;
}

#ifdef __SKILL0517
int	CMover::GetSkillLevel( SKILL* pSkill )
{
	if( !pSkill )
		return 0;
	ItemProp* pSkillProp	= prj.GetSkillProp( pSkill->dwSkill );
	if( pSkillProp )
	{
		DWORD dwSkillLevel	= (DWORD)GetParam( DST_SKILL_LEVEL, pSkill->dwLevel );
		return(  dwSkillLevel > pSkillProp->dwExpertMax? pSkillProp->dwExpertMax: dwSkillLevel );
	}
	return 0;
}
#endif	// __SKILL0517