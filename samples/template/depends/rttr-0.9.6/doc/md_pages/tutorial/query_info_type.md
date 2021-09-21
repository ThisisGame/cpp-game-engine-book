Query information from rttr::type  {#rttr_type_info_page}
=================================

A [type](@ref rttr::type) object contains automatically a lot of useful meta information, 
which exceeds the information of the standard <a target="_blank" href=http://en.cppreference.com/w/cpp/types/type_info>type_info</a> object.

~~~~{.cpp}
struct D { ... };

type::get<D>().get_name();                // returns 'struct D'; remark this is a compiler specific defined string (not portable)
type::get<D>().is_class();                // true
type::get<D>().is_pointer();              // false
type::get<D*>().is_pointer();             // true
type::get<D>().is_array();                // false
type::get<D[50]>().is_array();            // true
type::get<std::vector<D>>().is_array();   // true
type::get<D>().is_arithmetic();           // false
type::get<D>().is_enumeration();          // false
~~~~

The returned name of the function @ref rttr::type::get_name() "get_name()" returns a compiler depended string. 
So don't use it directly when comparing with a string literal, but it is very useful for debugging purposes.

This functionality gets more useful when working with other components like [properties](@ref rttr::property) or [variants](@ref rttr::variant). 
For example: when you iterate over all properties of class you might not want to store the values of pointer types.
Or you might want to group stuff together, like: *give me all primitive properties of a class* and so on.

It is also possible to retrieve information about the inheritance graph of a class.
~~~~{.cpp}
struct Base { RTTR_ENABLE() };
struct Derived : Base { RTTR_ENABLE(Base) };

Derived d;

std::vector<type> base_list = type::get(d).get_base_classes();

for (auto& t : base_list)
  std::cout << t.get_name() << std::endl; // 'struct Base'
~~~~

Or use a shorthand method to check if a type is derived from another:

~~~~{.cpp}
type::get(d).is_derived_from<Base>(); // true
~~~~

The meta information presented in the first code snipped (e.g. @ref rttr::type::is_class() "is_class()", @ref rttr::type::is_pointer() "is_pointer()")  will work without registering anything manually to the type system.
In order to retrieve class hierarchy informations, like @ref rttr::type::get_base_classes() "get_base_classes()" or @ref rttr::type::is_derived_from() "is_derived_from()", 
you will have to add manually some information to the type system.
How this will be done, is discussed in the [next](@ref rttr_type_class_hierachy_page "Using RTTR_ENABLE") chapter.

Summary
-------
- with a [type](@ref rttr::type) object you can retrieve additional meta information at runtime

<hr>

<div class="btn btn-default doxy-button">[previous](@ref rttr_type_get_page "Retrieving rttr::type objects")</div><div class="btn btn-default doxy-button">[next](@ref rttr_type_class_hierachy_page "Using RTTR_ENABLE")</div>
