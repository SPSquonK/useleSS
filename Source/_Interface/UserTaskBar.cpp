#include "stdafx.h"
#include "UserTaskBar.h"
#ifdef __WORLDSERVER
#include "User.h"
#endif // __WORLDSERVER
#ifdef __CLIENT
#include "DPClient.h"
#endif

#include <ranges>
#include <numeric>

CTaskbar::CTaskbar() {
	SHORTCUT emptyShortcut;
	emptyShortcut.Empty();

	m_aSlotApplet.fill(emptyShortcut);

	std::ranges::for_each(m_aSlotItem, [&](auto & items) {
		items.fill(emptyShortcut);
		});

	m_aSlotQueue.fill(emptyShortcut);

	m_nActionPoint = 0;
}

size_t CTaskbar::CountNumberOfChats() const {
	constexpr auto IsChat = [](const SHORTCUT & shortcut) {
		return shortcut.m_dwShortcut == ShortcutType::Chat;
	};

	constexpr auto NbOfChats = []<size_t N>(const std::array<SHORTCUT, N> &shortcuts) {
		return std::ranges::count_if(shortcuts, IsChat);
	};

	auto ChatsInEachItems = m_aSlotItem | std::views::transform(NbOfChats);

	return NbOfChats(m_aSlotApplet) + std::reduce(ChatsInEachItems.begin(), ChatsInEachItems.end());
}

void CTaskbar::RemoveAll(const ShortcutType type) {
	for (int nSlot = 0; nSlot < MAX_SLOT_APPLET; ++nSlot) {
		if (m_aSlotApplet[nSlot].m_dwShortcut == type) {
#ifdef __CLIENT
			g_DPlay.SendRemoveAppletTaskBar(nSlot);
#endif
			m_aSlotApplet[nSlot].Empty();
		}
	}

	for (int nSlot = 0; nSlot < MAX_SLOT_ITEM_COUNT; ++nSlot) {
		for (int nIndex = 0; nIndex < MAX_SLOT_ITEM; ++nIndex) {
			if (m_aSlotItem[nSlot][nIndex].m_dwShortcut == type) {
#ifdef __CLIENT
				g_DPlay.SendRemoveItemTaskBar(nSlot, nIndex);
#endif
				m_aSlotItem[nSlot][nIndex].Empty();
			}
		}
	}

	if (type == ShortcutType::Skill) {
		if (!m_aSlotQueue[0].IsEmpty()) {
			memset(&m_aSlotQueue, 0, sizeof(m_aSlotQueue));
#ifdef __CLIENT
			g_DPlay.SendSkillTaskBar();
#endif
		}
	}
}

CAr & operator<<(CAr & ar, const CTaskbar & self) {
	constexpr auto PushShortcut = [](CAr & ar, const SHORTCUT & shortcut, const auto ... extra) {
		if (shortcut.IsEmpty()) return false;

		((ar << extra), ...);
		ar << shortcut;

		return true;
	};

	const auto appletsSize = ar.PushBack<size_t>(0);
	for (int i = 0; i < MAX_SLOT_APPLET; ++i) {
		if (PushShortcut(ar, self.m_aSlotApplet[i], i)) {
			++(*appletsSize);
		}
	}

	const auto itemssSize = ar.PushBack<size_t>(0);
	for (int i = 0; i < MAX_SLOT_ITEM_COUNT; ++i) {
		for (int j = 0; j < MAX_SLOT_ITEM; ++j) {
			if (PushShortcut(ar, self.m_aSlotItem[i][j], i, j)) {
				++(*itemssSize);
			}
		}
	}

	const auto queueSize = ar.PushBack<size_t>(0);
	for (int i = 0; i < MAX_SLOT_QUEUE; ++i) {
		if (PushShortcut(ar, self.m_aSlotQueue[i], i)) {
			++(*queueSize);
		}
	}

	ar << self.m_nActionPoint;
	return ar;
}

CAr & operator>>(CAr & ar, CTaskbar & self) {
	ar.IsUnsafe();

	self = CTaskbar();

	size_t nbShortcut;

	ar >> nbShortcut;
	for (size_t i = 0; i != nbShortcut; ++i) {
		int nIndex; ar >> nIndex;
		ar >> self.m_aSlotApplet[nIndex];
	}

	ar >> nbShortcut;
	for (size_t i = 0; i != nbShortcut; ++i) {
		int nIndex1, nIndex2;
		ar >> nIndex1 >> nIndex2;
		ar >> self.m_aSlotItem[nIndex1][nIndex2];
	}

	ar >> nbShortcut;
	for (size_t i = 0; i != nbShortcut; ++i) {
		int nIndex; ar >> nIndex;
		ar >> self.m_aSlotQueue[nIndex];
	}

	ar >> self.m_nActionPoint;
	return ar;
}


#ifdef __WORLDSERVER

// 스킬바 사용이 완전히 끝났을때
void CTaskbar::OnEndSkillQueue(CUser * pUser) {
	pUser->m_nUsedSkillQueue = -1;
	pUser->AddHdr(GETID(pUser), SNAPSHOTTYPE_ENDSKILLQUEUE);
	((CMover *)pUser)->ClearCmd();
	TRACE("\nOnEndSkillQueue\n");
}

int	CTaskbar::SetNextSkill(CUser * pUser) {
	pUser->m_nUsedSkillQueue++;		// 큐인덱스 다음으로...

	int nAP = m_nActionPoint;

	if (!(pUser->IsSMMode(SM_ACTPOINT))) {
		switch (pUser->m_nUsedSkillQueue) {
			case 1:	nAP -= 6;	break;		// 액션 포인트 소모량. 다음슬롯을 사용할수있는지 미리 알아보기 위함.
			case 2:	nAP -= 8;	break;
			case 3:	nAP -= 11;	break;
			case 4:	nAP -= 30;	break;
		}
	}

	LPSHORTCUT pShortcut = &m_aSlotQueue[pUser->m_nUsedSkillQueue];
	BOOL bResult = nAP < 0;
	if ((pUser->m_nUsedSkillQueue >= MAX_SLOT_QUEUE) || pShortcut->IsEmpty() || (nAP < 0))	// 큐진행이 끝까지 갔거나 || 큐가 비었거나 || AP가 없거나.
	{
		OnEndSkillQueue(pUser);
		return 0;		// 큐실행 끝.
	} else {
		if (nAP < 0)	nAP = 0;
		m_nActionPoint = nAP;
		pUser->AddSetActionPoint(nAP);		// 액숀포인트 클라에 갱신.
		OBJID idTarget = pUser->m_idSetTarget;
		TRACE("다음스킬사용 시도%d, ", pShortcut->m_dwId);
		if (pUser->CMD_SetUseSkill(idTarget, pShortcut->m_dwId, SUT_QUEUEING) == 0)		// 실행할 명령을 셋팅. 이동 + 스킬사용이 합쳐진 명령.
		{
			TRACE("다음스킬사용 실패 %d, ", pShortcut->m_dwId);
			SetNextSkill(pUser);	// 스킬사용에 실패했다면 다음 스킬 사용하도록 넘어감.
		}
		return 1;
	}

	return 1;
}
#endif // __WORLDSERVER

