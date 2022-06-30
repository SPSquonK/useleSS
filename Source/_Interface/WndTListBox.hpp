#pragma once

#include <boost/container/stable_vector.hpp>
#include <span>

// SquonK, SKHBL

namespace WndTListBox {
  namespace Color {
    static constexpr DWORD Font = D3DCOLOR_ARGB(255, 64, 64, 64);
    static constexpr DWORD Select = D3DCOLOR_ARGB(255, 64, 64, 255);
    static constexpr DWORD OnMouse = D3DCOLOR_ARGB(255, 255, 128, 0);
    static constexpr DWORD Invalid = D3DCOLOR_ARGB(255, 170, 170, 170);
  }

  struct DisplayArgs {
    bool isSelected;
    bool isValid;
    int i;
  };
}

class CWndNeuz;

template<typename T, typename Displayer>
class CWndTListBox : public CWndBase {
public:
  static constexpr int VSCROLL_WIDTH = 16;
  
  struct Listed {
    T item;
    std::optional<CRect> rect = std::nullopt;
    bool isValid = true;
  };

  struct ViewedItems {
    int first;
    int last;
  };

  struct UpdateRectsCache {
    size_t listedSize;
    int scrollPos;
    ViewedItems correspondingView;
  };

  DWORD selectColor = WndTListBox::Color::Select;
  int lineHeight = 0;
  Displayer displayer;

private:
  boost::container::stable_vector<Listed> m_listed;
  CWndScrollBar m_wndScrollBar;

  int m_nCurSelect = -1;
  Listed * m_pFocusItem = nullptr;
  std::optional<UpdateRectsCache> m_updateRectsCache = std::nullopt;

public:
  static void Replace(CWndNeuz & window, UINT listboxId);

  CWndTListBox();

  void Create(DWORD dwListBoxStyle, RECT & rect, CWndBase * pParentWnd, UINT nID);
  [[nodiscard]] int GetCurSel() const { return m_nCurSelect; }
  [[nodiscard]] T * GetCurSelItem() { return m_nCurSelect >= 0 ? &m_listed[m_nCurSelect].item : nullptr; }

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

  Listed & Add(T item);
  void ResetContent();
  [[nodiscard]] bool IsEmpty() const { return m_listed.empty(); }

private:
  void PaintListBox(C2DRender * p2DRender);
  ViewedItems UpdateRects();
  bool UpdateRectsNeedUpdate();

  [[nodiscard]] int GetLineHeight() const;
};

template<typename T, typename D>
CWndTListBox<T, D>::CWndTListBox() {
  m_byWndType = WTYPE_LISTBOX;
  m_strTexture = DEF_CTRL_TEXT;
  m_bTile = TRUE;
}

template<typename T, typename D>
void CWndTListBox<T, D>::Create(DWORD dwListBoxStyle, RECT & rect, CWndBase * pParentWnd, UINT nID) {
  CWndBase::Create(dwListBoxStyle | WBS_CHILD, rect, pParentWnd, nID);
}

template<typename T, typename D>
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
void CWndTListBox<T, D>::OnInitialUpdate() {
  CRect rect = GetWindowRect();

  m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
  m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

  static constexpr int m_nLineSpace = 1;
  lineHeight = m_pFont->GetMaxHeight() + m_nLineSpace + m_nLineSpace;
}

template<typename T, typename D>
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

    pt.y = allocatedRect.top;
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
bool CWndTListBox<T, D>::UpdateRectsNeedUpdate() {
  // TODO: check m_updateRectsCache
  return true;
}


template<typename T, typename D>
void CWndTListBox<T, D>::PaintListBox(C2DRender * p2DRender) {
  const CPoint mousePoint = GetMousePoint();

  const auto displayed = UpdateRects();

  for (int i = displayed.first; i != displayed.last; ++i) {
    DWORD textColor;
    if (!m_listed[i].isValid) {
      textColor = WndTListBox::Color::Invalid;
    } else if (i == m_nCurSelect) {
      textColor = selectColor;
    } else if (m_listed[i].rect->PtInRect(mousePoint)) {
      textColor = WndTListBox::Color::OnMouse;
    } else {
      textColor = WndTListBox::Color::Font;
    }

    WndTListBox::DisplayArgs args = {
      .isSelected = i == m_nCurSelect,
      .isValid = m_listed[i].isValid,
      .i = i
    };

    displayer.Render(p2DRender, m_listed[i].item, m_listed[i].rect.value(), textColor, args);
  }
}

template<typename T, typename D>
int CWndTListBox<T, D>::GetLineHeight() const {
  return lineHeight;
}

template<typename T, typename D>
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
void CWndTListBox<T, D>::OnSize(UINT nType, int cx, int cy) {
  CRect rect = GetWindowRect();
  if (IsWndStyle(WBS_VSCROLL))
    rect.left = rect.right - VSCROLL_WIDTH;
  m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));
  m_wndScrollBar.SetWndRect(rect);
  CWndBase::OnSize(nType, cx, cy);
}

template<typename T, typename D>
BOOL CWndTListBox<T, D>::OnEraseBkgnd(C2DRender * p2DRender) {
  return TRUE;
}

template<typename T, typename D>
void CWndTListBox<T, D>::OnSetFocus(CWndBase * pOldWnd) {
}

template<typename T, typename D>
BOOL CWndTListBox<T, D>::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  m_wndScrollBar.MouseWheel(zDelta);
  return TRUE;
}

template<typename T, typename D>
CWndTListBox<T, D>::Listed & CWndTListBox<T, D>::Add(T item) {
  m_updateRectsCache = std::nullopt;
  return m_listed.emplace_back(Listed{ .item = item });
}

template<typename T, typename D>
void CWndTListBox<T, D>::ResetContent() {
  m_nCurSelect = -1;
  m_pFocusItem = NULL;
  m_listed.clear();
  m_wndScrollBar.SetScrollPos(0, FALSE);
  m_updateRectsCache = std::nullopt;
}
