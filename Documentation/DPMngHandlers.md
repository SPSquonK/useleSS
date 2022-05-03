# DPMngHandlers

The classes that inherit from `CDPMng` tends to have code
that uses the macros `USES_PFNENTRIES`, `BEGIN_MSG`, `ON_MSG`,
`GETTYPE`, `theClass` and `theParameters`.

A choice that was made is to replace these macros
with a proper class.


## Motivations

- The multiple definitions of the macros `theClass` and `theParameters`
makes the `#include` order dependant, which is terrible design.
- Macros are bad in general as they are "code that is not C++". While
macros were usefull in the past, the evolutions of the language tend to 
render them useless.
- There is code duplication in the multiple `UserMessageHandler` definitions.

## Design goal

- The handler class should become at some point a template class used in every `CDPMng` class.
- Some operations should be possible: for example, finding the instance of the User that sent the message



## Not a design goal

Note that this kind of code is possible :

```cpp
OnMsg(PACKETTYPE_BUY_PVENDOR_ITEM, OnBuyPVendorItem);
```

```cpp
void OnBuyPVendorItem(CUser & pUser, OBJID objidVendor, BYTE nItem, DWORD dwItemId, short nNum);
// Automatically find the user and extract the 4 variables from the CAr
```

I used this approach on SFlyFF but it faces two issues:
- This kind of signature is not compatible with every method ; but we can just use the CAr & class as a parameter to get the "leftovers data".
- This makes the compilation way slower











