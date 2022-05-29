#pragma once

#include "ar.h"
#include <array>

#ifdef __WORLDSERVER
class CUser;
#endif

class CTaskbar {
public:
  enum class Operation : bool { Add = true, Remove = false };
  enum class BarName : bool { Applet = true, Item = false };

  static constexpr int LV2MAXAP =  10; // 2단계 스킬이 다차는데 필요한 action point
  static constexpr int LV3MAXAP =  25;
  static constexpr int LV4MAXAP =  50;
  static constexpr int LV5MAXAP = 100;

public:
  std::array<SHORTCUT, MAX_SLOT_APPLET> m_aSlotApplet;
  std::array<
    std::array<SHORTCUT, MAX_SLOT_ITEM>,
    MAX_SLOT_ITEM_COUNT
  > m_aSlotItem;
  std::array<SHORTCUT, MAX_SLOT_QUEUE> m_aSlotQueue;

  int m_nActionPoint; // 액션 포인트 - 시리얼라이즈 대상.

public:
  CTaskbar();
  // As there are no dynamic allocation, we don't really need a virtual destructor

  friend CAr & operator<<(CAr & ar, const CTaskbar & self);
  friend CAr & operator>>(CAr & ar, CTaskbar & self);

  bool CanAddShortcut(ShortcutType type, const SHORTCUT & atPlace);
  void RemoveAll(ShortcutType type);


#ifdef __WORLDSERVER
  void RemoveAllSkills();
  int  SetNextSkill(CUser * pUser);
  void OnEndSkillQueue(CUser * pUser);
#endif // __WORLDSERVER

private:
  [[nodiscard]] size_t CountNumberOfChats() const;

};
