Register Types - Hello World  {#register_hello_world_page}
============================
Let's start with the traditional hello world example.

~~~~{.cpp}
#include <rttr/registration>

static void f() { std::cout << "Hello World" << std::endl; }
using namespace rttr;

RTTR_REGISTRATION
{
    using namespace rttr;
    registration::method("f", &f);
}
  
int main()
{
    type::invoke("f", {});
}
// outputs: "Hello World"
~~~~

When you need to reflect a property, or like in this case a free function, you need to include first `#include <rttr/registration>`. 
This will include everything necessary for creating the reflection information. 

The macro @ref RTTR_REGISTRATION must be placed outside of any function or class, just place directly into in your cpp file.
This macro executes the register process before `main` is called, 
that has the advantage that this reflection information is directly available when `main` is called.
When on the other hand the register calls are invoked manually, then these calls must be synchronized with the calls retrieving the type information.
Otherwise invalid data will be returned. Remark that this macro can only be placed one-time in a source file, otherwise you will get an compile error.

The shortest way to invoke the function `f()` is to call @ref rttr::type::invoke(const std::string&, std::vector< argument >) "type::invoke()".
It requires the exact name of the free function and a vector of arguments. 
You can check whether the call was successful with checking the return value. 
When the returned @ref rttr::variant "variant" is @ref rttr::variant::is_valid() "valid" the call was successful, otherwise not.

<hr>

<div class="btn btn-default doxy-button" disabled="true">previous</div><div class="btn btn-default doxy-button">[next](@ref register_methods_page "Register Methods")</div>
