#include "stdafx.h"


#include "lordskill.h"
#include "definelordskill.h"

#ifdef __WORLDSERVER
#include "definetext.h"
#include "slord.h"
#include "user.h"
#include "dpsrvr.h"
#endif	// __WORLDSERVER


CLordSkillComponent::CLordSkillComponent(int nId, CScript & s)
	: m_nId(nId)
{
#ifndef __CLIENT
	DWORD m_dwSrcSfx, m_dwDstSfx;
#endif
#ifdef __DBSERVER
	FLOAT	m_fRange;
#endif

	s.GetToken();
	strncpy(m_szName, s.token, CLordSkillComponent::eMaxName);
	m_szName[CLordSkillComponent::eMaxName - 1] = '\0';

	s.GetToken();
	strncpy(m_szDesc, s.token, CLordSkillComponent::eMaxDesc);
	m_szDesc[CLordSkillComponent::eMaxDesc - 1] = '\0';

	m_nTargetType = s.GetNumber();
	m_nItem = s.GetNumber();
	m_nCooltime = s.GetNumber();

	s.GetToken();
	strncpy(m_szIcon, s.token, CLordSkillComponent::eMaxIcon);
	m_szIcon[CLordSkillComponent::eMaxIcon - 1] = '\0';

	m_bPassive = static_cast<BOOL>(s.GetNumber());

	m_dwSrcSfx = s.GetNumber();
	m_dwDstSfx = s.GetNumber();
	m_fRange = s.GetFloat();
}

#ifdef __CLIENT
CTexture* CLordSkillComponent::GetTexture( void )
{
	if( !m_pTexture )
		m_pTexture	= CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, m_szIcon ), 0xffff00ff );
	return m_pTexture;
}
#endif	// __CLIENT

int CLordSkillComponent::Tick( void )
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
CLordSkill::CLordSkill( CLord*	pLord )
:
m_pLord( pLord )
{
}

CLordSkillComponentExecutable * CLordSkill::GetSkill(int nId) {
	for (const auto & pSkill : m_vComponents) {
		if (pSkill->GetId() == nId) {
			return pSkill.get();
		}
	}

	return nullptr;
}

std::unique_ptr<CLordSkillComponentExecutable> CLordSkill::CreateSkillComponent(CScript & script) {
	const int nId = script.GetNumber();
	if (script.tok == FINISHED) return nullptr;
	script.GetNumber(); // skip type
	
	return std::make_unique<CLordSkillComponentExecutable>(nId, script);
}

void CLordSkill::Reset() {
	for (const auto & pLordSkillComponent : m_vComponents) {
		pLordSkillComponent->SetTick(0);
	}
}

BOOL CLordSkill::Initialize( const char* szFile )
{
	CScript s;
	if( !s.Load( szFile, FALSE ) )
		return FALSE;

	while (true) {
		std::unique_ptr<CLordSkillComponentExecutable> pComponent = CreateSkillComponent(s);
		if (pComponent) {
			m_vComponents.emplace_back(std::move(pComponent));
		} else {
			break;
		}
	}

	return TRUE;
}

CAr & CLordSkill::WriteTick(CAr & ar) const {
	ar << m_vComponents.size();
	for (const auto & pLordSkillComponent : m_vComponents) {
		ar << pLordSkillComponent->GetId();
		ar << pLordSkillComponent->GetTick();
	}
	return ar;
}

CAr & CLordSkill::ReadTick(CAr & ar) {
	size_t nSize;
	ar >> nSize;
	for (size_t i = 0; i < nSize; i++) {
		const auto [nSkill, nTick] = ar.Extract<int, int>();
		
		if (CLordSkillComponent * pSkill = GetSkill(nSkill)) {
			pSkill->SetTick(nTick);
		}
	}
	return ar;
}

#ifdef __WORLDSERVER
int CLordSkillComponentExecutable::IsExecutable( CUser* pUser, const char* szTarget, u_long & idTarget )
{
	if( !CSLord::Instance()->IsLord( pUser->m_idPlayer ) )
		return TID_GAME_LORD_SKILL_USE_E001;
	if( IsPassive() )
		return TID_GAME_LORD_SKILL_USE_E003;
	idTarget	= pUser->m_idPlayer;
	if( GetTargetType() != LTT_SELF )
	{
		idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( const_cast<char*>( szTarget ) );
		CUser* pTarget	= g_UserMng.GetUserByPlayerID( idTarget );
		if( !IsValidObj( pTarget ) || ( GetTargetType() == LTT_OTHER && pTarget == pUser ) )
			return TID_GAME_LORD_SKILL_USE_E004;
		if( GetRange() != 0.0F )
		{
			D3DXVECTOR3 v	= pUser->GetPos() - pTarget->GetPos();
			FLOAT fLengthSq	= D3DXVec3LengthSq( &v );
			if( fLengthSq > GetRange() * GetRange() )
				return TID_GAME_LORD_SKILL_USE_E008;
		}
		// 12-2 康	- 일반화 대상
		if( GetId() == LI_RAGE && pTarget->IsAuthHigher( AUTH_GAMEMASTER ) )
			return TID_GAME_LORD_SKILL_USE_E004;
	}
	return 0;
}

int CLordSkillComponentExecutable::IsTeleportable( CUser* pUser, CUser* pTarget )
{
	ASSERT( pTarget );
	if( pTarget->IsAuthHigher( AUTH_GAMEMASTER ) )
		return TID_GAME_LORD_SKILL_USE_E004;
	if( !pUser->IsTeleportable() )
		return TID_GAME_LORD_SKILL_USE_E009;
	if( !pTarget->IsTeleportable() )
		return TID_GAME_LORD_SKILL_USE_E010;
	return 0;
}

void CLordSkillComponentExecutable::PutSkillLog( CUser* pUser )
{
	g_DPSrvr.PutPenyaLog( pUser, "s", GetName(), 0 );
}
#endif	// __WORLDSERVER
