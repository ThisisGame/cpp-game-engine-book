Parameter Names {#parameter_names_page}
===============

As additional meta information it is possible to provide the names of arguments for 
@ref rttr::method "methods" or @ref rttr::constructor "constructors".
This information is then accessible via an object of type @ref rttr::parameter_info "parameter_info".

Please take a look at following example:

\code{.cpp}
using namespace rttr;

void set_window_geometry(const char* name, int w, int h) {...}

RTTR_REGISTRATION
{
    registration::method("set_window_geometry", &set_window_geometry)
    (
        parameter_names("window name", "width", "height")
    );
}
\endcode

The names has to a string literal (i.e. const char*) and provided via the function: @ref rttr::parameter_names() "parameter_names()".
Place the call in the `()` operator of the returned @ref rttr::registration::bind "bind" object.
\remark
It is not possible to provide just one name, when you use this function, you have to provide names for **all** arguments.


The function has following synopsis:
~~~~{.cpp}
    template<typename...TArgs>
    detail::parameter_names<detail::decay_t<TArgs>...> parameter_names(TArgs&&...args)
~~~~
The names can be retrieved via the @ref rttr::parameter_info "parameter_info" class.
Take a look at the following example:

\code{.cpp}
int main()
{
    method meth = type::get_global_method("set_window_geometry");
    std::vector<parameter_info> param_list = meth.get_parameter_infos();
    for (const auto& info : param_list)
    {
        // print all names of the parameter types and its position in the paramter list
        std::cout << " name: '" << info.get_type().get_name() << "'\n"
                  << "index: " << info.get_index()
                               << std::endl;
    }
}
\endcode

Output:

     name: 'window name'
    index: 0
     name: 'width'
    index: 1
     name: 'height'
    index: 2

<hr>

<div type="button" class="btn btn-default doxy-button">[previous](@ref default_arguments_page "Default Arguments")</div><div class="btn btn-default doxy-button">[next](@ref register_policies_page "Register Policies")</div>
