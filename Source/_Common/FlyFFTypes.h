#pragma once

#include "NamedType/named_type.hpp"
#include <concepts>

namespace useless_util {
  template <typename T>
  struct ArchivableType : fluent::crtp<T, ArchivableType> {
    static constexpr bool Archivable = true;
    static_assert(true);
  };

  template <typename T> concept Archivable = T::Archivable;

  struct IsEmptyClass {};
  
  template <typename T>
  concept EmptyArchivable = 
    std::derived_from<T, IsEmptyClass>
    && (sizeof(T) == sizeof(IsEmptyClass));

  template <typename PropType>
  struct HasProjProp {
    template<typename T>
    struct Type : fluent::crtp<T, Type> {
      [[nodiscard]] const PropType * GetProp() const { return PropType::Get(this->underlying()); }
    };
  };

  template<typename T>
  struct BoolEvaluation : fluent::crtp<T, BoolEvaluation> {
    explicit operator bool() const noexcept {
      return this->underlying().get() != 0;
    }
  };

  template<typename T>
  struct NamedTypeFrom : fluent::crtp<T, NamedTypeFrom> {
    [[nodiscard]] static T From(std::integral auto value) {
      using Underlying = typename T::UnderlyingType;
      if (std::cmp_less(value, std::numeric_limits<Underlying>::min())) {
        return T(0);
      } else if (std::cmp_greater(value, std::numeric_limits<Underlying>::max())) {
        return T(0);
      } else {
        return T(static_cast<Underlying>(value));
      }
    }
  };
}

using PartyId = fluent::NamedType<unsigned long, struct PartyIdTag,
  fluent::Comparable, useless_util::ArchivableType
>;

using WarId = fluent::NamedType<unsigned long, struct WarIdTag,
  fluent::Comparable, useless_util::ArchivableType
>;

static constexpr auto WarIdNone = WarId(0);

struct QuestProp;

using QuestId = fluent::NamedType<unsigned short, struct QuestIdTag,
  fluent::Comparable, useless_util::ArchivableType, useless_util::HasProjProp<QuestProp>::Type,
  useless_util::BoolEvaluation, useless_util::NamedTypeFrom
>;

static constexpr auto QuestIdNone = QuestId(0);
