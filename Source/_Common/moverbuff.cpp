#include "stdafx.h"
#include "mover.h"

BOOL CMover::HasBuff( WORD wType, WORD wId )
{
	return m_buffs.HasBuff( wType, wId );
}

void CMover::RemoveBuff( WORD wType, WORD wId )
{
	m_buffs.RemoveBuff( wType, wId );
}

bool CMover::HasBuffByIk3(const DWORD dwIk3) const {
	return m_buffs.HasBuffByIk3( dwIk3 );
}

#ifdef __WORLDSERVER
BOOL CMover::HasPet()
{
	return m_buffs.HasPet();
}

void CMover::RemovePet()
{
	m_buffs.RemoveBuffPet();
}
#endif	// __WORLDSERVER

DWORD CMover::GetDisguise()
{
	return m_buffs.GetDisguise();
}

BOOL CMover::AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTime, OBJID oiAttacker )
{
	return m_buffs.AddBuff( wType, wId, dwLevel, tmTime, oiAttacker );
}

void CMover::RemoveCommonBuffs()
{
	m_buffs.RemoveBuffs( RBF_COMMON );
}

#ifdef __CLIENT
void CMover::ClearBuffInst()
{
	m_buffs.ClearInst();
}
#endif	// __CLIENT

void CMover::RemoveChrStateBuffs( DWORD dwChrState )
{
	m_buffs.RemoveBuffs( RBF_CHRSTATE, dwChrState );
}

void CMover::RemoveIk3Buffs( DWORD dwIk3 )
{
	m_buffs.RemoveBuffs( RBF_IK3, dwIk3 );
}

void CMover::RemoveAllBuff()
{
	m_buffs.RemoveBuffs( RBF_UNCONDITIONAL );
}

void CMover::ProcessBuff()
{
	m_buffs.Process();
}

void CMover::RemoveAttackerBuffs( OBJID oiAttacker )
{
	m_buffs.RemoveBuffs( RBF_ATTACKER, oiAttacker );
}

void CMover::RemoveDstParamBuffs( DWORD dwDstParam )
{
	m_buffs.RemoveBuffs( RBF_DSTPARAM, dwDstParam );
}

void CMover::RemoveDebufBuffs()
{
	m_buffs.RemoveBuffs( RBF_DEBUF );
}