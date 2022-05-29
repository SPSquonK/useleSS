#pragma once

#include "WndBase.h"
#include <functional>

/// A CWndBase component that can display an item.
/// 
/// @SPSquonK, 2022-05
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

/// A CWndBase component that can receive an item
/// 
/// May notify with (WNM_ItemReceiver_Changed, new item)
/// 
/// @SPSquonK, 2022-05
/// - License: http://squonk.fr/SquonK-Hidden-Boss-License.txt
class CWndItemReceiver : public CWndBase {
private:
  CItemElem * m_item = nullptr;
  DWORD m_defaultTooltip = 0;
  bool m_removableItem;

public:
  enum class Removable { Yes, No };
  enum class SetMode { Verbose, Silent, NeverFail };

  CWndItemReceiver(Removable removableItem = Removable::No)
    : m_removableItem(removableItem == Removable::Yes) {
    m_dwStyle |= WBS_NOFRAME | WBS_CHILD | WBS_NODRAWFRAME;
  }
  ~CWndItemReceiver() override;

  void OnInitialUpdate() override;
  // Changes the tooltip displayed when hovering the slot
  void SetTooltipId(const DWORD tooltip) { m_defaultTooltip = tooltip; }

  virtual bool CanReceiveItem(const CItemElem & itemElem, bool verbose = true) = 0;
  bool SetAnItem(CItemElem * itemElem, SetMode setMode);

  void OnDraw(C2DRender * p2DRender) override;
  void OnMouseWndSurface(CPoint point) override;
  BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point) override;
  void OnRButtonUp(UINT, CPoint) override;
  void OnLButtonDblClk(UINT, CPoint) override;

  CItemElem * GetItem() { return m_item; }

private:
  void NotifyChange();
  void ResetItem();

  void ResetItemWithNotify();
};


