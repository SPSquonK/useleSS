#pragma once


namespace sqktd {
  // From the type of of pointer to member field, enables
  // to access the name of the class and the type of the field
  template<typename AnyType> struct PointerToMemberInfo {};

  template<typename TheClass, typename TheMember>
  struct PointerToMemberInfo<TheMember TheClass:: *> {
    // The type of the class
    using Class = TheClass;
    // The type of the member
    using Member = TheMember;
  };

  // Return true if Wanted is one of the listed type in Possiblities
  template<typename Wanted, typename ... Possibilities>
  static constexpr bool IsOneOf = (std::is_same_v<Wanted, Possibilities> || ...);

}

