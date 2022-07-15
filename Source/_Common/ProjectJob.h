#pragma once

namespace Project {
  class Jobs {
  public:
    struct JOBITEM {
      std::array<std::vector<DWORD>, 2> adwMale;
    };

    std::array<std::vector<const ItemProp *>, MAX_JOB> skills;
    std::array<JOBITEM, MAX_JOB> items;

    void LoadSkills(CFixedArray<ItemProp> & aPropSkill);
    bool LoadJobItem(LPCTSTR lpszFileName);

  };

}
