#pragma once

#include "sqktd/type_traits.hpp"
#include <variant>
#include "ar.h"

class CItemElem;
class CMover;

// == Variant UI API
// @SPSquonK 2022-06
// Under the Boost License

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
    using SynchronizedType = typename sqktd::PointerToMemberInfo<decltype(Field)>::Member;
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
   

  struct RandomOptItemFromRandomScroll {
    static constexpr bool Archivable = true;
    std::uint8_t nKind;

    explicit RandomOptItemFromRandomScroll(std::uint8_t nKind = 0) : nKind(nKind) {}

    void operator()(CItemElem & itemElem) const;
  };

  struct Element {
    static constexpr bool Archivable = true;
    static constexpr int DoNotChangeAO = INT_MIN;
    BYTE kind;
    int abilityOption;

    [[nodiscard]] static Element None();
    [[nodiscard]] static Element Increase(const CItemElem & itemElem);
    [[nodiscard]] static Element Decrease(const CItemElem & itemElem);
    [[nodiscard]] static Element Change(SAI79::ePropType target);

    [[nodiscard]] static Element ActivatedQuest(const CItemElem & itemElem);

    void operator()(CItemElem & itemElem, CMover & mover) const;
  };

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

  struct Num {
    static constexpr bool Archivable = true;
    short newQuantity = 0;
    bool startCooldown = false;

    static Num RemoveAll() { return Num{ 0, false }; }
    static Num ConsumeOne(const CItemElem & itemElem);
    static Num Sync(const CItemElem & itemElem);

    static auto Consume(const short quantity) {
      return [quantity](const CItemElem & itemElem) {
        return Num{
          .newQuantity = static_cast<short>(itemElem.m_nItemNum - quantity),
          .startCooldown = false
        };
      };
    }

    void operator()(CItemElem & itemElem, CMover & mover) const;
  };

  struct HitPoint {
    static constexpr bool Archivable = true;
    int newQuantity;

    static HitPoint Decrement(const CItemElem & itemElem);

    void operator()(CItemElem & itemElem, CMover & mover) const;
  };

  struct Repair {
    static constexpr bool Archivable = true;
    int newHitPoint;
    int repairNumber;

    static auto From(int nRepair) {
      return [nRepair](const CItemElem & itemElem) {
        return Repair{
          .newHitPoint = static_cast<int>(itemElem.GetProp()->dwEndurance),
          .repairNumber = itemElem.m_nRepairNumber + nRepair
        };
      };
    }

    void operator()(CItemElem & itemElem, CMover & mover) const;
  };

  struct Flag {
    static constexpr bool Archivable = true;
  private:
    DWORD m_objIndex;
    decltype(CItemElem::m_byFlag) m_flags;

    explicit Flag(const CItemElem & itemElem);

  public:
    Flag() = default;

    [[nodiscard]] static Flag Sync(const CItemElem & itemElem) { return Flag(itemElem); }

    void operator()(CItemElem & itemElem, CMover & mover) const;
#ifdef __CLIENT
    void operator()(CMover & mover) const;
#endif
  };

  struct ChangeItemId {
    static constexpr bool Archivable = true;
  private:
    DWORD m_newItemId;

    explicit ChangeItemId(DWORD newId) : m_newItemId(newId) {}

  public:
    ChangeItemId() = default;

    [[nodiscard]] static std::optional<ChangeItemId> MorphItem(const CItemElem & itemElem);
    
    void operator()(CItemElem & itemElem, CMover & mover) const;
  };

  using Variant = std::variant<
    Num, AbilityOption, Element, 
    HitPoint, Repair, Flag, 
    RandomOptItem, RandomOptItemFromRandomScroll, KeepTime,
    Piercing::Size, Piercing::Item, 
    PetVis::Size, PetVis::Item, PetVis::ItemSwap, PetVis::TransformToVisPet,
    ChangeItemId
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

