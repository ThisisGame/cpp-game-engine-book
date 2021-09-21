Methods {#register_methods_page}
========

As already mentioned to register global methods to RTTR use the function \ref rttr::registration::method() "registration::method()".
It has following synopsis:
~~~~{.cpp}
    template<typename F>
    rttr::registration::bind rttr::registration::method(string_view name, F function);
~~~~
- `name` is the name of the function
- `F` is the function pointer or std::function your want to register

For example when you want to register the byte string to integer conversion function: `int atoi (const char * str);`

Do it in the following way:
~~~~{.cpp}
#include <rttr/registration>
#include <cstdlib>

RTTR_REGISTRATION
{
    using namespace rttr;
    registration::method("atoi", &atoi);
}
~~~~

Overloaded Methods
------------------
When you want to register a function which is overloaded (same name, different signature),
you should use the helper function: @ref rttr::select_overload<Signature>(Signature*) "select_overload<T>(T*)"

This will avoid the hassle with the function pointer signature.
For example the two functions: `float sin (float x);` and `double sin (double x);` can be registered like following:

~~~~{.cpp}
#include <rttr/registration>
#include <cmath>

RTTR_REGISTRATION
{
    using namespace rttr;
    registration::method("sin", select_overload<float(float)>(&sin))
                 .method("sin", select_overload<double(double)>(&sin));
}
~~~~
It is possible the chain multiple registration calls, because of the temporary returned @ref rttr::registration::bind "bind" object.

\remark The select_overload function cannot be used with MSVC x86 compiler, because of the different calling convention for global- and member-functions. 
        See here for a @ref rttr::select_overload<Signature>(Signature*) "workaround".

Invoke of methods
-----------------
Invoking a method with RTTR can be done in two ways:
- calling @ref rttr::type::invoke(string_view, std::vector< argument >) "type::invoke()" from the [type](@ref rttr::type) class.
- retrieving first a @ref rttr::method "method" object from @ref rttr::type::get_global_method(string_view, const std::vector< type >&) "type::get_global_method()" and then calling invoke

The first option needs less typing, but it is slower when you need to invoke the function several times.
For the second option you need more code to write, but it invokes the method faster.

The following example demonstrates the possibilities to invoke a method:
~~~~{.cpp}
using namespace rttr;
int main()
{
    // let asume we have registered the pow function: double pow( double base, double exp );
    // option 1
    variant return_value = type::invoke("pow", {9.0, 2.0});
    if (return_value.is_valid() && return_value.is_type<double>())
        std::cout << return_value.get_value<double>() << std::endl; // 81
    
    // option 2
    method meth = type::get_global_method("pow");
    if (meth) // check if the function was found
    {
        return_value = meth.invoke({}, 9.0, 3.0); // invoke with empty instance
        if (return_value.is_valid() && return_value.is_type<double>())
            std::cout << return_value.get_value<double>() << std::endl; // 729
        
    }
}
~~~~

The @ref rttr::type::invoke(string_view, std::vector< argument >) "type::invoke()" function will internally try 
to find a function based on the given name and the given types of the arguments. 
So finding the correct function when overloaded function are registered is automatically done.
Notice that you have to provide the arguments as a vector pack. Therefore an initializer list is quite handy to reduce typing.
The argument must match 100%, there is at the moment no conversion done. That means, when an integer argument is needed and you forward a double
value the function will **not** be called. The arguments will not be copied, instead they will be wrapped in an internal class and forwarded to the 
underlying function pointer. This class is called `argument` do **not** create this class on your own. The class will implicit wrap your argument value.

The return value of @ref rttr::type::invoke(string_view, std::vector< argument >) "type::invoke()" is a @ref rttr::variant "variant" object.
It indicates whether the function was called or not. RTTR does **not** use the exception mechanism of C++, therefore you have to check the return values when you are interested
in a successful call. The @ref rttr::variant object @ref rttr::variant::is_valid "is valid" when it was successfully called. 
When the function has a return value, then this value is also contained in the variant object.

When you prefer to hold a handle to the function use the getter @ref rttr::type::get_global_method(string_view, const std::vector< type >&) "type::get_global_method()"
to retrieve a @ref rttr::method "method" object. This has the advantage that you do not need to search for the function every time you want to invoke it.
Additionally this class provides @ref rttr::method::invoke "functions" to invoke a function without the need to create a vector of arguments.
The method object is lightweight and can be simply copied around different locations. The object stays valid till end of the `main()` function.

<hr>

<div class="btn btn-default doxy-button">[previous](@ref register_hello_world_page "Register Hello World")</div><div class="btn btn-default doxy-button">[next](@ref register_properties_page "Register Properties")</div>
