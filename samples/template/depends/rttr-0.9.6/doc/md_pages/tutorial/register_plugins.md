Register Plugins {#register_plugins}
=================
RTTR has build in support to register your types into shared libraries, which can be loaded 
and unloaded at runtime. Furthermore, it has a simple wrapper class called \ref rttr::library "library"
which wraps the platform dependent calls to load the library.

See following example:
~~~~{.cpp}
#include <rttr/registration>

struct MyPluginClass
{
    MyPluginClass(){}

    void perform_calculation()
    {
        value += 12;
    }
    
    void perform_calculation(int new_value)
    {
        value += new_value;
    }

    int value = 0;
};

RTTR_PLUGIN_REGISTRATION // remark the different registration macro!
{
   rttr::registration::class_<MyPluginClass>("MyPluginClass")
        .constructor<>()
        .property("value", &MyPluginClass::value)
        .method("perform_calculation", rttr::select_overload<void(void)>(&MyPluginClass::perform_calculation))
        .method("perform_calculation", rttr::select_overload<void(int)>(&MyPluginClass::perform_calculation))
        ;
}
~~~~
In order to register your types inside a plugin, you have to use the macro
\ref RTTR_PLUGIN_REGISTRATION.
Then the containing code will be executed every time you load the library
and it makes sure to unregister yours types, when your library will be unloaded.

Now the following code will load the plugin into your application:
~~~~{.cpp}
#include <rttr/type>

int main(int argc, char** argv)
{
    using namespace rttr;
    
    // no suffix is needed, RTTR will automatically append the platform specific file suffix
    library lib("MyPlugin");

    if (!lib.load())
    {
        std::cerr << lib.get_error_string() << std::endl;
        return -1;
    }

    // print all classes contained in the library
    for (auto t : lib.get_types()) // returns all registered types from this library
    {
        if (t.is_class() && !t.is_wrapper())
            std::cout << t.get_name() << std::endl;
    }
    // we cannot use the actual type, to get the type information,
    // thus we use string to retrieve it
    auto t = type::get_by_name("MyPluginClass");
    
    // iterate over all methods of the class
    for (auto meth : t.get_methods())
    {
        std::cout << meth.get_signature() << std::endl;
    }
    
    // work with the new type
    auto var = t.create();
    t.invoke("perform_calculation", var, {});
    std::cout << t.get_property_value("value", var).to_int() << std::endl; // prints "12"

    return 0;
}
~~~~
Output:
~~~~{.cpp}
MyPluginClass
perform_calculation( )
perform_calculation( int )
12
~~~~
\remark When you compile your plugin with the `gcc` toolchain, make sure you use the compiler option: `-fno-gnu-unique`.
otherwise the unregistration will not work properly.

Summary
-------
- Using plugins you can work with types without having access to the concrete type itself
- You don't have to explicit export (e.g. using `__declspec( dllexport )`) your types in the shared library
- You can export classes, without a generic abstract interface
- You can export overloaded methods (not possible in C)
- With all this functionality, it is easily possible to implement \b hot-reload of shared libraries. 
You could serialize your object into JSON-Format, unload the library, load the new version and deserialize it again.
- \remark Make sure you throw away all retrieved items (\ref rttr::type "types", \ref rttr::property "properties", \ref rttr::method "methods" etc...) of the loaded library when unloading.
 Otherwise UB may occur. (e.g. Invoking a method of an unloaded library is not possible)
  
<hr>

<div type="button" class="btn btn-default doxy-button">[previous](@ref register_policies_page "Register Policies")</div><div class="btn btn-default doxy-button">[finished](@ref tutorial_page "Tutorial")</div>
