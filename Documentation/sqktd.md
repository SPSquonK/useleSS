# sqktd

## Copyable Unique Ptr

`#include "sqktd/copyable_unique_ptr.hpp`

Unique pointers can not be copied because they may contain
polymorphic objects.

a sqktd::copyable_unique_ptr can be copied. It uses the copy
constructor to produce a new object when copied.


## Enum Set

`#include "sqktd/enumset.hpp`

An `std::bitset` compatible with enum classes.

The enum class must have a `_MAX` field so enumset can know its size.

enumsets are *archivable*.


## Flasher

`#include "sqktd/flasher.hpp"`

Something that blink between two values with a certain speed.

May be used for things like buffs disappearing.

## Last Packets

`#include "sqktd/last_packets.hpp"`

Stores the last received packets and some of their content.

Does not produce any allocation so the structure is usable even if
the memory is corrupted.


## Maybe Owned Pointer

`#include "sqktd/maybe_owned_ptr.hpp"`

A `T *` does not contain any information about ownership, but we can
assume that in most cases it does not own the pointer.

An `std::unique_ptr<T>` always owns the pointer.


`sqktd::maybe_owned_ptr<T>` contains a pointer and knows if it is the owner
of the pointer.


## Mutexed Object

`#include "sqktd/mutexed_object.h`

`sqktd::mutexed_object<T, M>` is a wrapper around `T` that enforces that the
mutex of type `M` must be locked to access the data.


## Type traits

`#include "sqktd/type_traits.hpp"`

### Pointer ot Member Info

From a pointer to member, *i.e.* `&MyClass::AField`, `sqktd:PointerToMemberInfo` returns the type of the
class and the type of the field.

### IsOneOf

`sqktd::IsOneOf<A, B, ..., Z>` is equivalent to `sqtd::is_same_v<A, B> || sqtd::is_same_v<A, C> || ... || sqtd::is_same_v<A, Z>`.

