#pragma once

namespace sqktd {
  template<bool Boolean, typename A, typename B>
  struct ChooseType {};

  template<typename A, typename B>
  struct ChooseType<true, A, B> { using type = A; };

  template<typename A, typename B>
  struct ChooseType<false, A, B> { using type = B; };

  template<bool Boolean, typename A, typename B>
  using ChooseTypeV = ChooseType<Boolean, A, B>::type;
}

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
  
  using CurrentMorph = sqktd::ChooseTypeV<!::useless_params::ReflexiveMorph, VanillaMorph, ReflexiveMorph>;

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
