#pragma once

// WndTListBox Class
// @SPSquonK 2022-06~07, Under the Boost License
//
// This class enables to build a listbox with:
// - Any class used as an item
// - An utility class that will explain how to draw an item

#include <boost/container/stable_vector.hpp>
#include <concepts>

namespace WndTListBox {
  /// Default used colors for a listbox
  namespace Color {
    static constexpr DWORD Font    = D3DCOLOR_ARGB(255, 64, 64, 64);
    static constexpr DWORD Select  = D3DCOLOR_ARGB(255, 64, 64, 255);
    static constexpr DWORD OnMouse = D3DCOLOR_ARGB(255, 255, 128, 0);
    static constexpr DWORD Invalid = D3DCOLOR_ARGB(255, 170, 170, 170);
  }

  /// Extra arguments for Displayer Render method
  struct DisplayArgs {
    /// True if the current element is selected
    bool isSelected;
    /// True if the current element is valid
    bool isValid;
    /// Position of the current element
    int i;
    /// Mouse position
    CPoint mousePosition;
  };

  template<typename T>
  struct DefaultDisplayer;

  template<typename Item, typename Displayer>
  concept DisplayerOf = requires(Displayer d, Item & item, C2DRender * p2DRender, CRect rect, DWORD color, const WndTListBox::DisplayArgs & misc) {
    d.Render(p2DRender, rect, item, color, misc);
  };
}

class CWndNeuz;

/// The CWndTListBox component enables to draw listbox with a
/// specified class used as list items.
/// 
/// It avoids using weird hacks like having a second layer of drawing
/// or dynamic casting values.
template<typename T, typename Displayer = WndTListBox::DefaultDisplayer<typename T>>
requires (WndTListBox::DisplayerOf<T, Displayer>)
class CWndTListBox : public CWndBase
{
public:
  static constexpr int VSCROLL_WIDTH = 16;
  

public:
  Displayer displayer;

private:
  struct Listed {
    T item;
    std::optional<CRect> rect = std::nullopt;
    bool isValid = true;
  };

  struct ViewedItems { int first; int last; };

  struct UpdateRectsCache {
    size_t listedSize;
    int scrollPos;
    ViewedItems correspondingView;
  };

  boost::container::stable_vector<Listed> m_listed;
  CWndScrollBar m_wndScrollBar;

  int m_nCurSelect = -1;
  Listed * m_pFocusItem = nullptr;
  std::optional<UpdateRectsCache> m_updateRectsCache = std::nullopt;
  
  unsigned int m_lineHeight = 0;
  DWORD m_selectColor = WndTListBox::Color::Select;

public:
  CWndTListBox();

  void Create(DWORD dwListBoxStyle, RECT & rect, CWndBase * pParentWnd, UINT nID);
  [[nodiscard]] int GetCurSel() const { return m_nCurSelect; }
  [[nodiscard]] void SetCurSel(int index) {
    if (index == -1) {
      m_nCurSelect = -1;
      m_pFocusItem = nullptr;
      return;
    }

    if (index < 0 || std::cmp_greater_equal(index, m_listed.size())) {
      return;
    }

    m_nCurSelect = index;
    m_pFocusItem = &m_listed[index];
  }
  [[nodiscard]]       T * GetCurSelItem()       { return m_nCurSelect >= 0 ? &m_listed[m_nCurSelect].item : nullptr; }
  [[nodiscard]] const T * GetCurSelItem() const { return m_nCurSelect >= 0 ? &m_listed[m_nCurSelect].item : nullptr; }
  [[nodiscard]] std::pair<int, const T *> GetSelection() {
    return std::make_pair<int, const T *>(GetCurSel(), GetCurSelItem());
  }


  void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE) override;
  void OnInitialUpdate() override;
  void OnDraw(C2DRender * p2DRender) override;

  void OnLButtonUp(UINT nFlags, CPoint point) override;
  void OnLButtonDown(UINT nFlags, CPoint point) override;
  void OnRButtonUp(UINT nFlags, CPoint point) override;
  void OnLButtonDblClk(UINT nFlags, CPoint point) override;
  void OnSize(UINT nType, int cx, int cy) override;
  BOOL OnEraseBkgnd(C2DRender * p2DRender) override;
  void OnSetFocus(CWndBase * pOldWnd) override;
  BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) override;

  T & Add(T item, bool isValid = true);
  void Erase(int index);
  // TODO: Swap
  // TODO: GetAt
  T & operator[](int index) { return m_listed[index].item; }
  // TODO: operator[] const
  // TODO: ChangeValidity
  void ResetContent();
  [[nodiscard]] bool IsEmpty() const { return m_listed.empty(); }
  [[nodiscard]] size_t GetSize() const noexcept { return m_listed.size(); }
  
  template<typename F>
  [[nodiscard]] int Find(F f) const {
    for (int i = 0; std::cmp_less(i, m_listed.size()); ++i) {
      if (f(m_listed[i].item)) {
        return i;
      }
    }

    return -1;
  }

  void SetLineHeight(unsigned int lineHeight) { m_lineHeight = lineHeight; }
  void SetLineSpace(unsigned int lineSpace);
  void ChangeSelectColor(DWORD color) { m_selectColor = color; }

private:
  void PaintListBox(C2DRender * p2DRender);
  ViewedItems UpdateRects();
  bool UpdateRectsNeedUpdate();

  [[nodiscard]] unsigned int GetLineHeight() const { return m_lineHeight; }
};

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
CWndTListBox<T, D>::CWndTListBox() {
  m_byWndType = WTYPE_LISTBOX;
  m_strTexture = DEF_CTRL_TEXT;
  m_bTile = TRUE;
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::Create(DWORD dwListBoxStyle, RECT & rect, CWndBase * pParentWnd, UINT nID) {
  CWndBase::Create(dwListBoxStyle | WBS_CHILD, rect, pParentWnd, nID);
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::SetWndRect(CRect rectWnd, BOOL bOnSize) {
  m_rectWindow = rectWnd;
  m_rectClient = m_rectWindow;
  m_rectClient.DeflateRect(4, 4);
  if (IsWndStyle(WBS_VSCROLL))
    m_rectClient.right -= 15;
  m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

  if (bOnSize) {
    OnSize(0, m_rectClient.Width(), m_rectClient.Height());
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnInitialUpdate() {
  CRect rect = GetWindowRect();

  m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
  m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

  if (GetLineHeight() == 0) {
    SetLineSpace(1);
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::SetLineSpace(unsigned int lineSpace) {
  m_lineHeight = m_pFont->GetMaxHeight() + lineSpace + lineSpace;
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnDraw(C2DRender * p2DRender) {
  PaintListBox(p2DRender);

  if (IsWndStyle(WBS_VSCROLL)) {
    const int nPage = GetClientRect().Height() / GetLineHeight();
    const int nRange = static_cast<int>(m_listed.size());

    m_wndScrollBar.SetVisible(TRUE);
    m_wndScrollBar.SetScrollRange(0, nRange);
    m_wndScrollBar.SetScrollPage(nPage);
  } else {
    m_wndScrollBar.SetVisible(FALSE);
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
CWndTListBox<T, D>::ViewedItems CWndTListBox<T, D>::UpdateRects() {
  if (!UpdateRectsNeedUpdate()) {
    return m_updateRectsCache->correspondingView;
  }
  
  int i;

  // Above window
  for (i = 0; i < m_wndScrollBar.GetScrollPos(); ++i) {
    if (std::cmp_greater_equal(i, m_listed.size())) break;
    m_listed[i].rect = std::nullopt;
  }

  // On window
  CPoint pt(3, 3);

  const int nScrollBarWidth = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetClientRect().Width() : 0;

  const int spanBegin = i;
  for (i = m_wndScrollBar.GetScrollPos(); std::cmp_less(i, m_listed.size()); ++i) {
    const CRect allocatedRect(
      pt.x,
      pt.y,
      pt.x + m_rectWindow.Width() - nScrollBarWidth,
      pt.y + GetLineHeight()
    );

    if (allocatedRect.top > m_rectWindow.bottom) break;

    m_listed[i].rect = allocatedRect;

    pt.y = allocatedRect.bottom;
  }
  const int spanEnd = i;

  // Under window
  while (std::cmp_less(i, m_listed.size())) {
    m_listed[i].rect = std::nullopt;
    ++i;
  }

  m_updateRectsCache = UpdateRectsCache{
    .listedSize = m_listed.size(),
    .scrollPos = m_wndScrollBar.GetScrollPos(),
    .correspondingView = ViewedItems{ spanBegin, spanEnd }
  };

  return m_updateRectsCache->correspondingView;
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
bool CWndTListBox<T, D>::UpdateRectsNeedUpdate() {
  // TODO: check m_updateRectsCache
  return true;
}


template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::PaintListBox(C2DRender * p2DRender) {
  const CPoint mousePoint = GetMousePoint();

  const auto displayed = UpdateRects();

  for (int i = displayed.first; i != displayed.last; ++i) {
    DWORD textColor;
    if (!m_listed[i].isValid) {
      textColor = WndTListBox::Color::Invalid;
    } else if (i == m_nCurSelect) {
      textColor = m_selectColor;
    } else if (m_listed[i].rect->PtInRect(mousePoint)) {
      textColor = WndTListBox::Color::OnMouse;
    } else {
      textColor = WndTListBox::Color::Font;
    }

    WndTListBox::DisplayArgs args = {
      .isSelected = i == m_nCurSelect,
      .isValid = m_listed[i].isValid,
      .i = i,
      .mousePosition = mousePoint
    };

    displayer.Render(p2DRender, m_listed[i].rect.value(), m_listed[i].item, textColor, args);
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnLButtonUp(UINT nFlags, CPoint point) {
  const auto displayed = UpdateRects();

  for (int i = displayed.first; i != displayed.last; ++i) {
    Listed & listed = m_listed[i];

    if (!listed.isValid) continue;

    if (listed.rect->PtInRect(point)) {
      if (m_pFocusItem == &listed) {
        CWndBase * pWnd = m_pParentWnd;
        pWnd->OnChildNotify(WNM_SELCHANGE, m_nIdWnd, (LRESULT *)&listed);
        return;
      }
    }
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnLButtonDown(UINT nFlags, CPoint point) {
  const auto displayed = UpdateRects();

  for (int i = displayed.first; i != displayed.last; ++i) {
    Listed & listed = m_listed[i];

    if (!listed.isValid) continue;

    if (listed.rect->PtInRect(point)) {
      m_nCurSelect = i;
      m_pFocusItem = &listed;
    }
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnRButtonUp(UINT nFlags, CPoint point) {
  const auto displayed = UpdateRects();

  for (int i = displayed.first; i != displayed.last; ++i) {
    Listed & listed = m_listed[i];

    if (!listed.isValid) continue;

    if (listed.rect->PtInRect(point)) {
      if (m_pFocusItem == &listed) {
        CWndBase * pWnd = m_pParentWnd;
        pWnd->OnChildNotify(WNM_SELCANCEL, m_nIdWnd, (LRESULT *)&listed);
        return;
      }
    }
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnLButtonDblClk(UINT nFlags, CPoint point) {
  const auto displayed = UpdateRects();

  for (int i = displayed.first; i != displayed.last; ++i) {
    Listed & listed = m_listed[i];

    if (!listed.isValid) continue;

    if (listed.rect->PtInRect(point)) {
      if (m_pFocusItem == &listed) {
        CWndBase * pWnd = m_pParentWnd;
        while (pWnd->GetStyle() & WBS_CHILD)
          pWnd = pWnd->GetParentWnd();
        pWnd->OnChildNotify(WNM_DBLCLK, m_nIdWnd, (LRESULT *)m_pFocusItem);
        return;
      }
    }
  }
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnSize(UINT nType, int cx, int cy) {
  CRect rect = GetWindowRect();
  if (IsWndStyle(WBS_VSCROLL))
    rect.left = rect.right - VSCROLL_WIDTH;
  m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));
  m_wndScrollBar.SetWndRect(rect);
  CWndBase::OnSize(nType, cx, cy);
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
BOOL CWndTListBox<T, D>::OnEraseBkgnd(C2DRender * p2DRender) {
  return TRUE;
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::OnSetFocus(CWndBase * pOldWnd) {
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
BOOL CWndTListBox<T, D>::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  m_wndScrollBar.MouseWheel(zDelta);
  return TRUE;
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
T & CWndTListBox<T, D>::Add(T item, bool isValid) {
  m_updateRectsCache = std::nullopt;
  return m_listed.emplace_back(Listed{ .item = item, .isValid = isValid }).item;
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::Erase(int index) {
  if (index < 0 || std::cmp_greater_equal(index, m_listed.size())) return;
  m_listed.erase(m_listed.begin() + index);
  m_updateRectsCache = std::nullopt;
}

template<typename T, typename D>
  requires (WndTListBox::DisplayerOf<T, D>)
void CWndTListBox<T, D>::ResetContent() {
  m_nCurSelect = -1;
  m_pFocusItem = nullptr;
  m_listed.clear();
  m_wndScrollBar.SetScrollPos(0, FALSE);
  m_updateRectsCache = std::nullopt;
}

namespace WndTListBox {
  template<typename T>
  struct DefaultDisplayer {
    void Render(
      C2DRender * const p2DRender, const CRect rect,
      T & item, const DWORD color, const WndTListBox::DisplayArgs & misc
    ) const {
      static_assert(WndTListBox::DisplayerOf<T, DefaultDisplayer>);

      item.Render(p2DRender, rect, color, misc);
    }
  };

  template<typename T>
  struct BasicDisplayer {
    void Render(
      C2DRender * const p2DRender, const CRect rect,
      T & item, const DWORD color, const WndTListBox::DisplayArgs & misc
    ) const {
      static_assert(WndTListBox::DisplayerOf<T, BasicDisplayer>);
      
      CString & s = item.text;
      p2DRender->TextOut(rect.left, rect.top, s.GetString(), color);
    }
  };
}

template<typename T>
using CWndBasicTListBox = CWndTListBox<T, WndTListBox::BasicDisplayer<T>>;
