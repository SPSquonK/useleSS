#pragma once

#include <variant>
#include "ar.h"

class CItemElem;
class CMover;

namespace UI {

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



  using Variant = std::variant<
    RandomOptItem
  
  >;
}

