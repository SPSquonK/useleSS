#pragma once

#include <type_traits>

namespace ItemMorph {
  // TODO: a Morph concept?

  /// The official server morph system.
  struct VanillaMorph {
    
    static const ItemProp * GetTransyItem(const ItemProp & toMorph);

    /// Return true if the item could find a matchable item
    static bool _IsPotentiallyMorphable(const ItemProp & pItemProp);

    static bool IsMorphableTo(const ItemProp & lhs, const ItemProp & rhs);

    static std::string BuildListOfExistingMorphs();
  };

  /// SquonK's morph system: reflexion is performed on the item II names
  /// to find matching items.
  struct ReflexiveMorph {
    static const ItemProp * GetTransyItem(const ItemProp & toMorph);
    static bool _IsPotentiallyMorphable(const ItemProp & pItemProp);
    static bool IsMorphableTo(const ItemProp & lhs, const ItemProp & rhs);
    static std::string BuildListOfExistingMorphs();
  };
  
  using CurrentMorph = std::conditional_t<!::useless_params::ReflexiveMorph, VanillaMorph, ReflexiveMorph>;

  /// Return the item prop that the given itemprop morphs to
  inline const ItemProp * GetTransyItem(const ItemProp & toMorph) {
    return CurrentMorph::GetTransyItem(toMorph);
  }

  [[nodiscard]] inline bool IsMorphableTo(const ItemProp & lhs, const ItemProp & rhs) {
    return CurrentMorph::IsMorphableTo(lhs, rhs);
  }

  [[nodiscard]] inline std::string BuildListOfExistingMorphs() {
    return CurrentMorph::BuildListOfExistingMorphs();
  }
}
