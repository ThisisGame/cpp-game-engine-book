rttr_cast vs. dynamic_cast  {#rttr_type_rttr_cast_page}
==========================

The @ref rttr::rttr_cast "rttr_cast" function behaves similarly to the standard C++
`dynamic_cast`. However, it has some advantages, it doesn't require RTTI support, 
it works across dynamic library boundaries and it performs faster. 
Run the `benchmark` target to perform this test on your local machine.

rttr::rttr_cast<T>()
--------------------

The function @ref rttr::rttr_cast "rttr_cast" allows the client to cast between class hierarchies up and down, 
cross casts between unrelated classes and even class hierarchies with virtual inheritance. 
The target type `T` can be also in the middle of the hierarchy.

~~~~{.cpp}
struct A { RTTR_ENABLE() };
struct B : A { RTTR_ENABLE(A) };
struct C : B { RTTR_ENABLE(B) };
C c;
A* a = &c;
B* b = rttr_cast<B*>(a); // successful
~~~~

Cross casts are also possible:

~~~~{.cpp}
struct A { RTTR_ENABLE() };
struct B { RTTR_ENABLE() };
struct C : A , B { RTTR_ENABLE(A, B) };
C c;
A* a = &c;
B* b = rttr_cast<B*>(a); // successful; remark that class A and B are unrelated from each other
~~~~

A [type](@ref rttr::type) object knows from which parent class it is derived. Assumed this information is given via @ref RTTR_ENABLE.

@remark Because exception are not supported the target type `T` can only be a pointer type.

Summary
-------
- when it's necessary to perform down casts or cross casts with a given instance, then use `rttr_cast`

<hr>

<div class="btn btn-default doxy-button">[previous](@ref rttr_type_class_hierachy_page "Using RTTR_ENABLE")</div><div class="btn btn-default doxy-button">[next](@ref tutorial_page "Tutorial page")</div>
