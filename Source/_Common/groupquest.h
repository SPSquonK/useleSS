#pragma once

namespace GroupQuest {

  enum class ProcessState : BYTE {
    Ready = 0, Wormon = 1, GetItem = 2
  };

  struct QuestElem {
    int nId = -1;
    int nState = 0;
    u_long idGroup = 0;

    DWORD	dwEndTime = 0;
    ProcessState	nProcess = GroupQuest::ProcessState::Ready;
    BYTE	ns = 0;
    BYTE	nf = 0;
    OBJID	objidWormon = NULL_ID;
    short	nCount = 0;
  };


}


