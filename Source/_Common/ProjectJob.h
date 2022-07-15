#pragma once

namespace Project {
  enum class ProJob { Vagrant, Mercenary, Assist, Magician, Acrobat };

  class Jobs {
  public:
    struct JOB {
      TCHAR	szName[32];
      TCHAR	szEName[32];
      DWORD	dwJobBase;
      DWORD	dwJobType;
    };

    struct JOBITEM {
      std::array<std::vector<DWORD>, 2> adwMale;
    };

    std::array<std::vector<const ItemProp *>, MAX_JOB> skills;
    std::array<JOBITEM, MAX_JOB> items;

    void LoadSkills(CFixedArray<ItemProp> & aPropSkill);
    bool LoadJobItem(LPCTSTR lpszFileName);

#if defined(__WORLDSERVER) || defined(__CLIENT)
    JobProp	prop[MAX_JOB];
    JOB			info[MAX_JOB];

    [[nodiscard]] boost::container::small_vector<DWORD, 6> GetAllJobsOfLine(DWORD jobId) const;
    [[nodiscard]] ProJob GetProJob(DWORD jobId) const;
    [[nodiscard]] const JobProp * GetJobProp(int nIndex) const;

    bool LoadPropJob(LPCTSTR lpszFileName);
    void LoadJobInfo(CScript & script);

  private:
    [[nodiscard]] bool InfoIsConsistent() const;
  public:

#endif

#ifdef __CLIENT
    struct PlayerDataIcon {
      int job;
      int master;
      PlayerDataIcon(DWORD jobId, int level);
    };
#endif

  };

}
