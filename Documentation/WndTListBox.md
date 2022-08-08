# CWndTListBox


## Why is CWndListBox bad?

- There is a lot of casting involved
- Drawing with other things than only a single string text is painfull.


## How to use CWndTListBox

### Design what to display

You can display pretty much anything. Let's say that the name of the thing
we want to display is `Item`.

The only requirement is that `Item` must be compatible with `std::vector`,
*i.e.* the copy if it exists is not glitched.


### Design how to display it

Build a `Displayer` class with the `Render` method.

```cpp
struct Displayer {
    void Render(
        C2DRender * const p2DRender, CRect rect,
        const Item & item, DWORD color, WndTListBox::DisplayArgs & misc
    ) const;
};
```

```cpp
void Displayer::Render(
	C2DRender * const p2DRender, const CRect rect,
	const Item & item, const DWORD color, const WndTListBox::DisplayArgs &
) const {
	p2DRender->TextOut(rect.left, rect.top, "An element", color);
}
```

### Add The TListBox to your window

In the `OnInitialUpdate` method of your class, replace the CWNdListBox
with the appropriate `CWndTListBox`


```cpp
ReplaceListBox<Item, Displayer>(WIDC_CONTROL1);
```

The appropriate type of `WIDC_CONTROL1` is now `CWndTListBox<Item, Displayer>`.

If you want to fill right now the list or change it, it is actually returned by the method:

```cpp
CWndTListBox<Item, Displayer> & list = ReplaceListBox<Item, Displayer>(WIDC_CONTROL1);
list.SetLineHeight(25); // each item now takes 25 pixels
list.Add(Item());
list.Add(Item());
list.Add(Item());
```








