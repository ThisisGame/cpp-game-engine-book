Enums {#register_enums_page}
=====

RTTR allows also to register enumerated constants (enums). Therefore use the function @ref rttr::registration::enumeration() "registration::enumeration()".

It has following synopsis:

~~~~{.cpp}
    template<typename Enum_Type>
    registration rttr::registration::enumeration( string_view name );
~~~~

- `name` the declared name of this enum

~~~~{.cpp}
#include <rttr/registration>
using namespace rttr;

enum class E_Alignment
{
    AlignLeft       = 0x0001,
    AlignRight      = 0x0002,
    AlignHCenter    = 0x0004,
    AlignJustify    = 0x0008
};

RTTR_REGISTRATION
{
    registration::enumeration<E_Alignment>("E_Alignment")
                  (
                      value("AlignLeft",    E_Alignment::AlignLeft),
                      value("AlignRight",   E_Alignment::AlignRight),
                      value("AlignHCenter", E_Alignment::AlignHCenter),
                      value("AlignJustify", E_Alignment::AlignJustify)
                  );
}
~~~~
In order to add the individual enumerators you have to use the `()` operator of the returned @ref rttr::registration::bind "bind" object.
Then you call for every enumerator you want to add, the function @ref rttr::value() "value()".

It has following synopsis:
~~~~{.cpp}
    template<typename Enum_Type>
    detail::enum_data<Enum_Type> value(string_view name, Enum_Type value);
~~~~

The *name* is a `string_view` and the *value* is the enum value.

The class @ref rttr::enumeration "enumeration" contains several meta information about an enum with conversion 
functions between the value representation and its literal representation.

How to use the enumeration class shows following example:
~~~~{.cpp}
    using namespace rttr;  
    type enum_type = type::get_by_name("E_Alignment");
    if (enum_type && enum_type.is_enumeration())
    {
        enumeration enum_align = enum_type.get_enumeration();
        std::string name = enum_align.value_to_name(E_Alignment::AlignHCenter);
        std::cout << name; // prints "AlignHCenter"
      
        variant var = enum_align.name_to_value(name);
        E_Alignment value = var.get_value<E_Alignment>(); // stores value 'AlignHCenter'
    }
~~~~

@remark You can also use the @ref rttr::variant "variant" class to convert from an enum value to is integral or string representation.
~~~~{.cpp}
    variant var = E_Alignment::AlignHCenter;
    std::cout << var.to_int() << std::endl; // prints '4'
    std::cout << var.to_string() << std::endl; // prints 'AlignHCenter'
~~~~
<hr>

<div type="button" class="btn btn-default doxy-button">[previous](@ref register_properties_page "Register Properties")</div><div class="btn btn-default doxy-button">[next](@ref register_variant_page "Variant")</div>
