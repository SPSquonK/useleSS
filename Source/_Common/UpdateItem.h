#pragma once

#include <variant>
#include "ar.h"

class CItemElem;
class CMover;

// == Variant UI API
// @SPSquonK 2022-06
// Under the Boost License

namespace sqktd {
  namespace _ {
    template<typename AnyType> struct PointerToMemberInfo{};

    template<typename TheClass, typename TheMember>
    struct PointerToMemberInfo<TheMember TheClass:: *> {
      using Class = TheClass;
      using Member = TheMember;
    };
  }
}

namespace UI {
  /// The generic Synchronizer class aims to synchronize values from World to
  /// Neuz.
  /// 
  /// Usage of Synchronizer is actually a code smell: it should be possible to
  /// generate an UI component that will update the values both for server and
  /// client, instead of relying on modifying first the data and then sending it
  /// to the client.
  template<auto Field>
    requires requires(CItemElem & itemElem) { itemElem.*Field; }
  struct Synchronizer {
    using SynchronizedType = typename sqktd::_::PointerToMemberInfo<decltype(Field)>::Member;
    SynchronizedType synchronizedValue{};

    Synchronizer() = default;
    explicit Synchronizer(SynchronizedType initial) : synchronizedValue(initial) {};

    static Synchronizer<Field> Sync(const CItemElem & itemElem) {
      return Synchronizer<Field>(itemElem.*Field);
    }

    void operator()(CItemElem & itemElem) const {
      itemElem.*Field = synchronizedValue;
    }
  };

  using RandomOptItem = Synchronizer<&CItemElem::m_iRandomOptItemId>;
    
//  struct RandomOptItem {
//    static constexpr bool Archivable = true;
//    __int64 value = 0;
//    RandomOptItem() = default;
//    RandomOptItem(__int64 value) : value(value) {}
//
//    void operator()(CItemElem & itemElem) const;
//  };

  namespace Piercing {
    enum class Kind { Regular, Ultimate };

    struct Size {
      static constexpr bool Archivable = true;
      Kind kind;
      int newSize;
      [[nodiscard]] static Size IncrementRegular(const CItemElem & itemElem);
      [[nodiscard]] static Size Ultimate(const CItemElem & itemElem);
      void operator()(CItemElem & itemElem) const;
    };

    struct Item {
      static constexpr bool Archivable = true;
      Kind kind;
      int position;
      DWORD itemId;
      void operator()(CItemElem & itemElem) const;
    };
  }

  namespace PetVis {
    struct TransformToVisPet {
      static constexpr bool Archivable = true;
      void operator()(CItemElem & itemElem) const;
    };

    struct Size {
      static constexpr bool Archivable = true;
      
      int newSize;
      [[nodiscard]] static Size DefaultVis();
      [[nodiscard]] static Size Increase(const CItemElem & itemElem);
      
      void operator()(CItemElem & itemElem) const;
    };

    struct Item {
      static constexpr bool Archivable = true;

      int position;
      DWORD itemId;
      DWORD time;

      [[nodiscard]] static Item Empty(const int position) {
        return Item{ .position = position, .itemId = 0, .time = 0 };
      }

      void operator()(CItemElem & itemElem) const;
    };

    struct ItemSwap {
      static constexpr bool Archivable = true;
      int from, to;

      void operator()(CItemElem & itemElem) const;
    };
  }

  struct KeepTime {
    static constexpr bool Archivable = true;
    DWORD minutes;

    static KeepTime FromProp(const CItemElem & itemElem);
    static KeepTime FromDays(const DWORD days) { return KeepTime{ .minutes = days * 24 * 60 }; }
    void operator()(CItemElem & itemElem) const;
  };

  struct AbilityOption {
    static constexpr bool Archivable = true;
    int newLevel;

    static AbilityOption Up(const CItemElem & itemElem);
    static AbilityOption Down(const CItemElem & itemElem);
    static AbilityOption Set(int newLevel) { return AbilityOption{ .newLevel = newLevel }; }
    void operator()(CItemElem & itemElem, CMover & mover) const;
  };

  using Variant = std::variant<
    AbilityOption,
    RandomOptItem, KeepTime, 
    Piercing::Size, Piercing::Item, 
    PetVis::Size, PetVis::Item, PetVis::ItemSwap, PetVis::TransformToVisPet
  >;
}

template<auto T2>
CAr & operator<<(CAr & ar, const UI::Synchronizer<T2> & sync) {
  return ar << sync.synchronizedValue;
}

template<auto T2>
CAr & operator>>(CAr & ar, UI::Synchronizer<T2> & sync) {
  return ar >> sync.synchronizedValue;
}

