# Singleton vs Global Variable


## Singleton

```cpp
// .h
struct MyClass {
    static MyClass * GetInstance();
};
```


## Global Variable

```cpp
// .h
struct MyClass {

};

extern MyClass g_MyClass;
```


## I prefer a global variables

- Singleton is a cute way to have a global variable in Java (you can't have any "real" global variable)
- `MyClass::GetInstance()` is longer than `g_MyClass`
- Most singleton classes are classes that either:
    - follows the pattern "load some things in CProject::OpenProject" then never touch it again
    - contains data related to the game state
    - cnontains network classes
- All singletons classes are accessed when the executable is started: lazy loading brings no benefit
- Having a `GetInstance()` bounds the class to only have one instance. When using a global variable, it would be
possible to have multiple instances if required. Note that this point is very minor.


Unlike what FlyFF developers have been doing, I prefer to put a single extern in the header. This is to limit the
number of times where the `extern MyClass g_MyClass` is repeated: if I'm including the header in which there is
a class used as a global variable, I will probably use `g_MyClass` anyway.

## The best of both worlds?

```cpp
// .h
class MyClass {
    MyClass & Instance;
};

// .cpp
static MyClass s_MyClassInstance;
MyClass & MyClass::Instance = s_MyClassInstance;
```

- Global instance is easy to find (`NameOfTheClass::Instance`).
- But I don't know how to compiles. Is this pattern slower than `extern MyClass g_myClass;`?



