#include "StdAfx.h"
#include "Taskbar.h"
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
      ++(*appletsSize);
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

