#include "stdafx.h"
#include "SfxHitArray.h"

CSfxHitArray::CSfxHitArray() {
	m_id = 0;
	memset(m_aSfxHit.data(), 0, sizeof(SfxHit) * nMaxSizeOfSfxHit);
}

int CSfxHitArray::Add( int id, OBJID objid, DWORD dwAtkFlags, DWORD dwSkill, int nMaxDmgCnt )
{
	if( id == 0 )	
		id	= ++m_id;

	for( int i = 0; i < nMaxSizeOfSfxHit; i++ )
	{
		if( m_aSfxHit[i].id == 0 )
		{
			m_aSfxHit[i].id		= id;
			m_aSfxHit[i].objid		= objid;
			m_aSfxHit[i].dwAtkFlags		= dwAtkFlags;
			m_aSfxHit[i].dwSkill = dwSkill;
			m_aSfxHit[i].nMaxDmgCnt = nMaxDmgCnt;
			return( id );
		}
	}

	// Korean dev: It is full so we clear it to be able to use it again
	// SquonK: ??? Can you justify why you are allowed to just clear the list?
	// Is it not that important? Why don't you add the sfx you just tried to add
	// after? In V21, why don't you use a vector or a small_vector?
	// TODO: answer the questions
	memset( m_aSfxHit.data(), 0, sizeof(m_aSfxHit));
	return( -1 );
}

const CSfxHitArray::SfxHit * CSfxHitArray::GetSfxHit(const int id) const {
	const auto it = std::find_if(
		m_aSfxHit.begin(), m_aSfxHit.end(),
		[id](const SfxHit & sfxHit) { return sfxHit.id == id; }
	);

	return it != m_aSfxHit.end() ? &*it : nullptr;
}

bool CSfxHitArray::RemoveSfxHit( int id, BOOL bForce ) {
	const auto it = std::find_if(
		m_aSfxHit.begin(), m_aSfxHit.end(),
		[id](const SfxHit & sfxHit) { return sfxHit.id == id; }
	);

	if (it == m_aSfxHit.end()) return false;
	
	// 카운트를 깎고 0이 됐을때만 완전히 삭제.
	if (--it->nMaxDmgCnt == 0 || bForce) {
		it->id = 0;
	}

	return true;
}
