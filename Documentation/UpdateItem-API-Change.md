# UpdateItem API Change

The UpdateItem API has been changed to use variants.

## Usage

The `UI::Variant` variant contains all possibles types that contain data about
the change of an item.

All types in this variant must:
- Be sendable / receivable by using a `CAr`
- Provide the operator `void operator()(CItemElem & itemElem, CMover? & mover)` that will modify `itemElem` accordingly to the content of the struct.

The idea of this new `CMover::UpdateItem` API is:
- You build an `UI::Variant` instance that describes the modification to apply
- You call `mover.UpdateItem(itemToModify, uiVariant)` server side, which will modify the item and send to the client the modification.

It is also possible to provide to UpdateItem a function that returns an UIVariant. The function can either have no parameter or take a `const CItemElem &` as a parameter.

It is useful to use the UpdateItem function like this: `pUser->UpdateItem(*pItemMain, UI::AbilityOption::Up);` which means "just increase by one the ability option level of this item".

It avois repetition, mistakes and provides better intuition about what we are trying to achieve.


## Implementation and design choices

I wrote the reasonning behing the implementation in french on my blog : https://www.sà.fr/2022/06/04/uivariant/

