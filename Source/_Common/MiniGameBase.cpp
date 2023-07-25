#include "stdafx.h"
#include "MsgHdr.h"

#ifdef __WORLDSERVER
#include ".\minigamebase.h"
#include "User.h"

CMiniGameBase::~CMiniGameBase() {}

void CMiniGameBase::SendPacket(CUser * pUser, const __MINIGAME_PACKET & MiniGamePacket) {
	pUser->SendSnapshotNoTarget<SNAPSHOTTYPE_RAINBOWRACE_MINIGAMESTATE, __MINIGAME_PACKET>(MiniGamePacket);
}

#endif // __WORLDSERVER

