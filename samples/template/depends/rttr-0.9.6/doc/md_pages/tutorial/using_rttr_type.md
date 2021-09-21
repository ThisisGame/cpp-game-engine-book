Retrieving rttr::type objects  {#rttr_type_get_page}
=============================

RTTR uses an own, in C++ implemented, alternative to the build in <a target="_blank" href=http://en.wikipedia.org/wiki/Run-time_type_information>RTTI</a> mechanism of C++. 
The reason for that are problems when using `typeid` <a target="_blank" href=https://svn.boost.org/trac/boost/ticket/754>across shared boundaries</a>, 
<a target="_blank" href=http://tinodidriksen.com/2010/04/14/cpp-dynamic-cast-performance/>execution speed</a> and sometimes <a target="_blank" href=http://stackoverflow.com/questions/28553165/inconsistent-typeid-for-nested-template-function-in-visual-studio>not correct</a> implemented.
That means it is possible to disable RTTI completly in your target application and use RTTR instead.

The entry point for all type related operation is the class [type](@ref rttr::type). 
It is the central class for retrieving type objects or also querying all kind information about types.

A [type](@ref rttr::type) object cannot be created directly. It is only possible to retrieve it with one of the three static getter member functions of the [type](@ref rttr::type) class.

~~~~{.cpp}
    #include <rttr/type> // when working with type objects, this is the only header you have to include
    //...
    using namespace rttr;

    type my_int_type  = type::get<int>();     // statically or

    type my_bool_type = type::get(true);      // dynamically
~~~~

rttr::type::get<T>()
--------------------

This function just expects one template argument. Use it to check against a known type.

~~~~{.cpp}
    type::get<int>() == type::get<int>();     // yields to true

    type::get<int>() == type::get<bool>();    // yields to false
~~~~

Remark that when comparing types, internally always a simple integer comparison is done, 
instead how often with <a target="_blank" href=http://en.cppreference.com/w/cpp/language/typeid>typeid</a>, a string comparison.
 
rttr::type::get<T>(T&& obj)
---------------------------

This function takes a universal reference and returns from every given object the corresponding [type](@ref rttr::type) object.

~~~~{.cpp}
    int int_obj = 23;
    int* int_obj_ptr         = &int_obj;
    const int* c_int_obj_ptr = int_obj_ptr;
  
    type::get<int>()        == type::get(int_obj);        // yields to true
    type::get<int*>()       == type::get(int_obj_ptr);    // yields to true
    type::get<const int*>() == type::get(c_int_obj_ptr);  // yields to true
 ~~~~
 
When this function is called for a <a target="_blank" href=http://en.cppreference.com/w/cpp/language/value_category#glvalue>glvalue</a> expression whose type is a polymorphic class type,
then the result refers to a [type](@ref rttr::type) object representing the type of the most derived object.

~~~~{.cpp}
    struct Base {};
    struct Derived : Base {};
    Derived d;
    Base& base = d;

    type::get<Derived>()  == type::get(base)        // yields to true
    type::get<Base>()     == type::get(base)        // yields to false

    // REMARK when called with pointers:
    Base* base_ptr = &d;
    type::get<Derived>()  == type::get(base_ptr);   // yields to false
    type::get<Base*>()    == type::get(base_ptr);   // yields to true
~~~~

If the type of the expression is a cv-qualified type, the result of the @ref rttr::type::get<T>() "type::get()"
expression refers to a [type](@ref rttr::type) object representing the cv-unqualified type.

~~~~{.cpp}
class D { ... };
D d1;
const D d2;
type::get(d1)  == type::get(d2);         // yields true
type::get<D>() == type::get<const D>();  // yields true
type::get<D>() == type::get(d2);         // yields true
type::get<D>() == type::get<const D&>(); // yields true
type::get<D>() == type::get<const D*>(); // yields false
~~~~

Any `top level` cv-qualifier of the given type `T` will be removed.

rttr::type::get_by_name(string_view)
------------------------------------

This function just expects the name of the type. This is useful when you know only the name of the type and cannot include the type itself into the source code.

~~~~{.cpp}
type::get_by_name("int")  == type::get<int>();   // yields to true
type::get_by_name("bool") == type::get<int>();  // yields to false
type::get_by_name("MyNameSpace::MyStruct") == type::get<MyNameSpace::MyStruct>();  // yields to true
~~~~

\remark Before using the function \ref type::get_by_name(), you have to use one time the function via \ref type::get<T>(), otherwise the type is not registered in the type system.

Container Support
-----------------
Unlike the <a target="_blank" href=http://en.cppreference.com/w/cpp/types/type_info>type_info</a> class, 
the [rttr::type](@ref rttr::type) class will not need and additional <a target="_blank" href=http://en.cppreference.com/w/cpp/types/type_index>wrapper</a> to place type objects inside containers:

~~~~{.cpp}
std::vector<rttr::type> type_list;
std::map<rttr::type, std::string> mapping;
std::unordered_map<rttr::type, std::string> type_names;
~~~~

Summary
-------
- [type](@ref rttr::type) objects cannot be created, only retrieved via static member functions in the type class itself
- [type](@ref rttr::type) objects are lightweight, they contain only a single integer ID
- every unique C++ type maps to the same unique ID, which will be used for comparing [types](@ref rttr::type)
- [type](@ref rttr::type) objects can be easily placed inside containers or maps
- retrieving a [type](@ref rttr::type) object results always in the cv-unqualified type (`type::get(T)==type::get(const T))`; same like `typeid`)
- retrieving a [type](@ref rttr::type) object of a gvalue expression whose type is a polymorphic class, will refer to a [type](@ref rttr::type) object representing the type of the most derived class

<hr>

<div class="btn btn-default doxy-button" disabled="true">previous</div><div class="btn btn-default  doxy-button">[next](@ref rttr_type_info_page "Query information from rttr::type")</div>

