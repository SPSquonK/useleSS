#include "StdAfx.h"
#include "SkillsOfMover.h"
#include <algorithm>

const ItemProp * SKILL::GetProp() const {
  return prj.GetSkillProp(dwSkill);
}

CAr & operator<<(CAr & ar, const MoverSkills & self) {
  ar << static_cast<std::uint32_t>(self.size());
  
  if (std::cmp_greater(self.size(), prj.m_aPropSkill.GetSize())) {
    Error("Too many skills to send - %zu/%d skills", self.size(), prj.m_aPropSkill.GetSize());
  }
  
  for (const SKILL & skill : self) {
    ar << skill.dwSkill << skill.dwLevel;
  }

  return ar;
}

CAr & operator>>(CAr & ar, MoverSkills & self) {
  std::uint32_t size; ar >> size;

  if (std::cmp_greater(size, prj.m_aPropSkill.GetSize())) {
    size = prj.m_aPropSkill.GetSize();
  }

  self.clear();
  for (size_t i = 0; i != size; ++i) {
    SKILL skill; ar >> skill.dwSkill >> skill.dwLevel;
    self.emplace_back(skill);
  }

  return ar;
}

#if defined(__CLIENT) || defined(__WORLDSERVER)
MoverSkills MoverSkills::ForJob(int job) {
  MoverSkills list;

  for (const DWORD jobId : prj.GetAllJobsOfLine(job)) {
    for (const ItemProp * pSkillProp : prj.jobs.skills[jobId]) {
      const DWORD initLevel = pSkillProp->dwItemKind1 == JTYPE_MASTER ? 1 : 0;
      list.emplace_back(SKILL{ .dwSkill = pSkillProp->dwID, .dwLevel = initLevel });
    }
  }

  return list;
}
#endif

SKILL * MoverSkills::FindBySkillId(DWORD skillId) {
  const auto it = std::ranges::find_if(*this, [skillId](const SKILL & skill) {
    return skill.dwSkill == skillId;
    });
  return it != end() ? &*it : nullptr;
}

const SKILL * MoverSkills::FindBySkillId(DWORD skillId) const {
  const auto it = std::ranges::find_if(*this, [skillId](const SKILL & skill) {
    return skill.dwSkill == skillId;
    });
  return it != end() ? &*it : nullptr;
}
