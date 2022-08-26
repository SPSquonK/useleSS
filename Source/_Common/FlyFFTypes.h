#pragma once

#include "NamedType/named_type.hpp"

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
  fluent::Comparable, useless_util::ArchivableType, useless_util::HasProjProp<QuestProp>::Type
>;

static constexpr auto QuestIdNone = QuestId(0);
