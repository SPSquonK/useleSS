#pragma once

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

// A function that does not return void must return a value
#pragma warning(error: 4715 4716)

// Discarding a [[nodiscard]] is an error
#pragma warning(error: 4834)

// Disables things like int `int & const x`
#pragma warning(error: 4227)

// Unused unitialized local variables = error
#pragma warning(error: 4101)

// We prefer POSIX functions rather than the ISO C++ variants
// If we want to do something safe, we use
// string / CString / string_view instead of C style functions anyway
#pragma warning(disable: 4996)

// Other things
#pragma warning(error: 4045 4066 4067 4068 4098 4138 4146 4156 4179 5030 5222)


namespace std_ {
  // Overloaded helper for std::visit. Should be in std but for some reason
  // it is not there
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

// ifdefs but better
namespace useless_params {
  // Guild master can change their guild logo after choosing one.
  static constexpr bool CanChangeLogo = true;
  
  // Items of the wrong gender are automatically morphed when trying to equip them.
  static constexpr bool AutoMorph = true;
  // Use reflexive morph
  static constexpr bool ReflexiveMorph = true;

}

