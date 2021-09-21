Default Arguments {#default_arguments_page}
=================

When you want to register a @ref rttr::method "method" or a @ref rttr::constructor "constructor" which contains default arguments,
you have to provide them RTTR explicitly. 
The reason for this is, default arguments are not part of the function signature.
Nevertheless, RTTR provides a mechanism to register functions with default arguments.

Please take a look at following example:

\code{.cpp}
using namespace rttr;

void my_function(int a, bool b, const std::string& text, const int* ptr);

RTTR_REGISTRATION
{
    registration::method("my_function", &my_function)   
    (
        default_arguments(true, std::string("default text"), nullptr)
    );
}
\endcode

The default arguments has to be provided via the function: @ref rttr::default_arguments() "default_arguments()".
Place the call in the `()` operator of the returned @ref rttr::registration::bind "bind" object.

The function has following synopsis:
~~~~{.cpp}
    template<typename...TArgs>
    detail::default_args<TArgs...> default_arguments(TArgs&&...args)
~~~~
The values will be copied internally and will be provided during invoke of the method or constructor,
when the corresponding argument is missing:

\code{.cpp}
int main()
{
    using namespace rttr;
    method meth = type::get_global_method("my_function");
    // the default values: 'true', 'std::string("default text"'), 'nullptr' will be forwarded by RTTR automatically
    variant var = meth.invoke(instance(), 23);
    std::cout << var.is_valid(); // prints 'true'

    // the default value: 'nullptr' will be forwarded by RTTR automatically
    var = meth.invoke(instance(), 23, true, std::string("text"));
    std::cout << var.is_valid(); // prints 'true'
}
\endcode

\remark The given arguments must match the signature from the starting position to the right most argument.

The following snippet will **not** compile and will raise a *static_assert*:
\code{.cpp}
using namespace rttr;

void my_function(int a, bool b, const std::string& text, const int* ptr);

RTTR_REGISTRATION
{
    registration::method("my_function", &my_function)   
    (
        default_arguments(std::string("default text")) // default value for "ptr" not given
    );
}
\endcode

Every @ref rttr::method "method" and @ref rttr::constructor "constructor" can have default arguments.

<hr>

<div type="button" class="btn btn-default doxy-button">[previous](@ref register_metadata_page "Register Metadata")</div><div class="btn btn-default doxy-button">[next](@ref parameter_names_page "Parameter Names")</div>
