#pragma once

#include <variant>
#include "ar.h"

class CItemElem;
class CMover;

namespace UI {
  // 2 meta for me ~ SquonK Hidden Boss License on Synchronizer class
  template<auto Field>
    requires requires(CItemElem & itemElem) { itemElem.*Field; }
  struct Synchronizer {
    using SynchronizedType = decltype(CTtemElem().*Field);
    SynchronizedType synchronizedValue;

    static Synchronizer Sync(const CItemElem & itemElem) {
      return Synchronizer{ itemElem.*Field };
    }





    friend CAr & operator<<(CAr & ar, const Synchronizer & sync) {
      return ar << sync.value;
    }

    friend CAr & operator>>(CAr & ar, Synchronizer & sync) {
      return ar >> sync.value;
    }

  };


  // Changes in the RandomOpt field
  // TODO: Current API is weird. This struct should manage the change
  // server side
  struct RandomOptItem {
    static constexpr bool Archivable = true;
    __int64 value = 0;
    RandomOptItem() = default;
    RandomOptItem(__int64 value) : value(value) {}

    void operator()(CItemElem & itemElem) const;
  };

  struct TransformToVisPet {
    static constexpr bool Archivable = true;
    void operator()(CItemElem & itemElem) const;
  };

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


  using Variant = std::variant<
    RandomOptItem,
    Piercing::Size, Piercing::Item, 
    PetVis::Size, PetVis::Item, PetVis::ItemSwap, TransformToVisPet
  >;
}

