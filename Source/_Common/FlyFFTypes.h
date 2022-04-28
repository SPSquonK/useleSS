#pragma once

#include "NamedType/named_type.hpp"

namespace sqktd {
  template <typename T>
  struct ArchivableType : fluent::crtp<T, ArchivableType> {
    static constexpr bool Archivable = true;
    static_assert(true);
  };

  template <typename T> concept Archivable = T::Archivable;
}

using PartyId = fluent::NamedType<unsigned long, struct PartyIdTag,
  fluent::Comparable, sqktd::ArchivableType
>;
