#pragma once

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

// A function that does not return void must return a value
#pragma warning(error: 4715 4716)

// Discarding a [[nodiscard]] is an error
#pragma warning(error: 4834)

// Disables things like int `int & const x`
#pragma warning(error: 4227)

// Unused local variables = error
#pragma warning(error: 311)

// We prefer POSIX functions rather than the ISO C++ variants
// If we want to do something safe, we use
// string / CString / string_view instead of C style functions anyway
#pragma warning(disable: 4996)

