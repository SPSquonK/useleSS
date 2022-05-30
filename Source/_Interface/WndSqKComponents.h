#pragma once

#include "WndBase.h"

// SquonK's CWndBase components for cleaner code and a brighter future.
// 
// @SPSquonK, 2022-05
// - License: http://squonk.fr/SquonK-Hidden-Boss-License.txt
// - or the Boost Software License 1.0


/// A CWndBase component that can display an item.
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
/// - May notify with (WNM_ItemReceiver_Changed, new item)
/// - User must derive it to define the CanReceiveItem() method
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

  /// Tells if an item can be received by this receiver. If verbose is false,
  /// the function may not emit any error message.
  virtual bool CanReceiveItem(const CItemElem & itemElem, bool verbose = true) = 0;

  /// Return the item cointained in the receiver
  CItemElem * GetItem() { return m_item; }
  /// Changes the tooltip displayed when hovering the slot
  void SetTooltipId(const DWORD tooltip) { m_defaultTooltip = tooltip; }

  // CWndBase functions

  void OnInitialUpdate() override;
  void OnDraw(C2DRender * p2DRender) override;
  void OnMouseWndSurface(CPoint point) override;
  BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point) override;
  void OnRButtonUp(UINT, CPoint) override;
  void OnLButtonDblClk(UINT, CPoint) override;

  // Actually receive an item. Most of the time, this is handled
  // by OnDropIcon.

  bool SetAnItem(CItemElem * itemElem, SetMode setMode);

private:
  void NotifyChange();
  void ResetItem();

  void ResetItemWithNotify();
};


