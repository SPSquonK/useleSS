#pragma once

#include "WndBase.h"

class CWndItemDisplayer : public CWndBase {
  std::optional<CItemElem> m_item = std::nullopt;
  CTexture texture;
  DWORD m_defaultTooltip = 0;

public:
  void SetTooltipId(DWORD tooltip) { m_defaultTooltip = tooltip; }

  void ResetItem() { m_item = std::nullopt; }
  void SetItem(CItemElem & item);

  void OnDraw(C2DRender * p2DRender) override;

  void OnInitialUpdate() override;
  DWORD GetItemId() const;


};

