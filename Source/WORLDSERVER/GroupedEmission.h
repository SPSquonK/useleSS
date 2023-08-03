#pragma once

#include "party.h"
#include "guild.h"
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

	for (CUser * const user : this | AllMembers) {
		user->AddBlock(buffer, blockSize);
	}
}


template<WORD SnapshotId, typename... Ts>
void CGuild::SendSnapshotNoTarget(const Ts & ... ts) const {
	SendSnapshotWithTarget<SnapshotId, Ts...>(NULL_ID, ts...);
}

template<WORD SnapshotId, typename... Ts>
void CGuild::SendSnapshotWithTarget(DWORD targetId, const Ts & ... ts) const {
	CAr ar;
	ar << targetId;
	ar << SnapshotId;
	ar.Accumulate<Ts...>(ts...);

	int blockSize = 0;
	BYTE * const buffer = ar.GetBuffer(&blockSize);

	for (CUser * const user : this | AllMembers) {
		user->AddBlock(buffer, blockSize);
	}
}

