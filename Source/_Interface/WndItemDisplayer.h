#pragma once

#include "WndBase.h"


/// A CWndBase component that can display an item.
/// 
/// @SPSquonk, 2022-05
/// - License: http://squonk.fr/SquonK-Hidden-Boss-License.txt
class CWndItemDisplayer : public CWndBase {
  std::optional<CItemElem> m_item = std::nullopt;
  DWORD m_defaultTooltip = 0;

public:
  CWndItemDisplayer() {
    m_dwStyle |= WBS_NOFRAME | WBS_CHILD | WBS_NODRAWFRAME;
  }

  // Changes the tooltip displayed when hovering the slot
  void SetTooltipId(const DWORD tooltip) { m_defaultTooltip = tooltip; }

  void ResetItem() { m_item = std::nullopt; }
  void SetItem(/* const */ CItemElem & item);

  /// Return the ID of the currently displayed item. Returns NULL if none
  [[nodiscard]] DWORD GetItemId() const;

  void OnDraw(C2DRender * p2DRender) override;
  void OnInitialUpdate() override;
};
