#pragma once

#include <array>

class CSfxHitArray final {
public:
	struct SfxHit {
		int		id;
		OBJID	objid;	// target
		DWORD	dwAtkFlags;
		DWORD	dwSkill;
		int		nMaxDmgCnt;
	};

private:
	static constexpr int nMaxSizeOfSfxHit = 32;
	std::array<SfxHit, nMaxSizeOfSfxHit> m_aSfxHit;
	int		m_id;

public:
	CSfxHitArray();

	int		Add( int id, OBJID objid, DWORD dwAtkFlags, DWORD dwSkill = 0xffffffff, int nMaxDmgCnt = 1 );	// return id, 스킬일경우 dwSkill에 값 넣을것.
	[[nodiscard]] const SfxHit * GetSfxHit(int id) const;
	bool	RemoveSfxHit(int id, BOOL bForce = FALSE);
};
