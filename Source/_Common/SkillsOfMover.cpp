#include "StdAfx.h"
#include "SkillsOfMover.h"
#include <algorithm>

const ItemProp * SKILL::GetProp() const {
  return prj.GetSkillProp(dwSkill);
}

CAr & operator<<(CAr & ar, const MoverSkills & self) {
  ar << static_cast<std::uint32_t>(self.size());
  
  if (self.size() > MAX_SKILL_JOB) {
    Error("Too many skills to send - %zu/%d skills", self.size(), MAX_SKILL_JOB);
  }
  
  for (const SKILL & skill : self) {
    ar << skill.dwSkill << skill.dwLevel;
  }

  return ar;
}

CAr & operator>>(CAr & ar, MoverSkills & self) {
  std::uint32_t size; ar >> size;

  if (size > MAX_SKILL_JOB) {
    size = MAX_SKILL_JOB;
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
  constexpr auto GetAllJobsOfLine = [](int jobId) -> boost::container::small_vector<int, 6> {
    boost::container::small_vector<int, 6> jobs;

    while (jobId != JOB_VAGRANT) {
      const auto it = std::ranges::find(jobs, jobId);
      if (it != jobs.end()) break;

      jobs.insert(jobs.begin(), jobId);

      jobId = prj.m_aJob[jobId].dwJobBase;
    }

    jobs.insert(jobs.begin(), JOB_VAGRANT);

    return jobs;
  };

  MoverSkills list;

  for (const int jobId : GetAllJobsOfLine(job)) {
    ItemProp ** apSkillProp = prj.m_aJobSkill[jobId];
    int nJobNum = prj.m_aJobSkillNum[jobId];
    for (int i = 0; i < nJobNum; i++) {
      ItemProp * pSkillProp = apSkillProp[i];
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
