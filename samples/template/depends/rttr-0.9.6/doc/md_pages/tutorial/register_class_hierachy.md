Register Class Hierarchy {#rttr_type_class_hierachy_page}
========================

Within the current C++ standard it is not possible to extract a class hierarchy automatically from a certain type. 
Therefore the programmer has to put a certain macro inside every class in order to provide this information. 
Additionally this macro will be needed to retrieve the information about the most derived [type](@ref rttr::type) of a current instance.

The macro you have to insert in the class declaration is named: @ref RTTR_ENABLE()

Suppose we have a base struct called `Base`:
~~~~{.cpp}
struct Base
{
    RTTR_ENABLE()
};
~~~~
Place the macro \ref RTTR_ENABLE() somewhere in the class, it doesn't matter if its under the public, protected or private class accessor section.

Into the derived class you put the same macro, but now as argument the name of the parent class.
Which is in this case `Base`.
~~~~{.cpp}
struct Derived : Base
{
    RTTR_ENABLE(Base)
};
~~~~

When you use multiple inheritance you simply separate every class with a comma.
~~~~{.cpp}
struct MultipleDerived : Base, Other
{
    RTTR_ENABLE(Base, Other)
};
~~~~
Remark that the order in which you declare here the multiple inheritance, has an impact later when retrieving properties of a class.
So it is best practice to use the same order like in your class.
RTTR supports to register even virtual base classes. 
@remark The only limitation you have is: It is **not** possible to register a class twice in the same class hierarchy.

When no class hierarchies are used at all, it is **not necessary** to use the macro. However it is best practice to place it inside every class.
This macro will also enable the possible usage of an own cast operator called: `rttr_cast`.
How this will be done, is discussed in the [next](@ref rttr_type_rttr_cast_page "Using rttr_cast") chapter.

Summary
-------
- to retrieve meta information of derived and base classes or using @ref rttr::rttr_cast "rttr_cast",
  it is necessary to place the macro: @ref RTTR_ENABLE() inside every class declaration
- the macro is **not** needed when working with classes which will be not inherited, e.g. POD classes

<hr>

<div class="btn btn-default doxy-button">[previous](@ref rttr_type_info_page "Query information from rttr::type")</div><div class="btn btn-default doxy-button">[next](@ref rttr_type_rttr_cast_page "rttr_cast vs. dynamic_cast")</div>
