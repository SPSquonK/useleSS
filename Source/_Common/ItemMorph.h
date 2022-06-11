#pragma once

namespace ItemMorph {
  /// Return the item prop that the given itemprop morphs to
  const ItemProp * GetTransyItem(const ItemProp & toMorph);

  // BOOL bCheck = FALSE, LPCTSTR lpszFileName

  /// Return true if the item could find a matchable item
  [[nodiscard]] bool _IsPotentiallyMorphable(const ItemProp & pItemProp);

  [[nodiscard]] bool IsMorphableTo(const ItemProp & lhs, const ItemProp & rhs);

  std::string BuildListOfExistingMorphs();
}
