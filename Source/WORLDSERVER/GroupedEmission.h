#pragma once

#include "party.h"
#include "GroupUtils.h"

template<WORD SnapshotId, typename... Ts>
void CParty::SendSnapshotNoTarget(Ts ... ts) const {
	SendSnapshotWithTarget<SnapshotId, Ts...>(NULL_ID, ts...);
}

template<WORD SnapshotId, typename... Ts>
void CParty::SendSnapshotWithTarget(DWORD targetId, Ts ... ts) const {
	CAr ar;
	ar << targetId;
	ar << SnapshotId;
	ar.Accumulate<Ts...>(ts...);
	
	int blockSize = 0;
	BYTE * const buffer = ar.GetBuffer(&blockSize);

	for (CUser * const user : AllMembers(*this)) {
		user->AddBlock(buffer, blockSize);
	}
}
