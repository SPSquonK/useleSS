#pragma once

#include <boost/container/static_vector.hpp>

namespace GroupQuest {
  static constexpr size_t MaxQuestTitleLen = 32;
  static constexpr size_t MaxState = 16;
  static constexpr size_t MaxQuestDescLen = 260;
  static constexpr size_t MaxQuest = 256;

  enum class ProcessState : BYTE {
    Ready = 0, Wormon = 1, GetItem = 2
  };

  struct QuestProp {
  protected:
    QuestProp() {
      ZeroMemory(szDesc, sizeof(szDesc));
    }
  public:

    char	szTitle[MaxQuestTitleLen] = "";
    int		nLevel = 0;
    char	szDesc[MaxState][MaxQuestDescLen];
    DWORD	dwWorldId = 0;
    int	x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;


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

    void SetGetItemState();
  };

  struct QuestRect {
    int nId;
    DWORD dwWorldId;
    CRect rect;
  };

  struct WORMON {
    DWORD       dwWormon = 0;
    D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
  };


#ifdef __WORLDSERVER
  class CQuestProcessor {
  protected:
    std::array<QuestElem, MaxQuest> m_pElem;
    boost::container::static_vector<QuestRect, MaxQuest> m_rects;

    CQuestProcessor() = default;

    void	_SetQuest(int nQuestId, int nState, int ns, int nf, u_long idParty, OBJID objidWormon);
    [[nodiscard]] bool IsOutOfRange(int nQuestId, const char * caller) const;

  public:
    
    QuestElem * GetQuest(int nQuestId);
    void RemoveQuest(int nQuestId);
    [[nodiscard]] bool IsQuesting(int nQuestId) const noexcept;

    void AddQuestRect(QuestRect rect) { m_rects.emplace_back(rect); }
    [[nodiscard]] int PtInQuestRect(DWORD worldId, const D3DXVECTOR3 & vPos) const;
    [[nodiscard]] const CRect * GetQuestRect(int nId) const;
    [[nodiscard]] const QuestRect * GetQuestQuestRect(int nId) const;
  };
#endif

}


