Policies {#register_policies_page}
========

Sometimes it is necessary to adjust the default registration behaviour of RTTR. Therefore policies were introduced.
There are policies for @ref rttr::policy::ctor "constructors", @ref rttr::policy::meth "methods" and @ref rttr::policy::prop "properties" available.

@ref ctor_policy_link "Constructors":
- as_object
- as_std_shared_ptr
- as_raw_ptr

@ref prop_policy_link "Properties":
- bind_as_ptr
- as_reference_wrapper

@ref meth_policy_link "Methods":
- discard_return
- return_ref_as_ptr

For easier usage, the policies are grouped to its corresponding items; e.g. all policies for methods
can be found under `policy::meth`; all policies for properties under `policy::prop` etc.


# Constructors {#ctor_policy_link}

### as_object ###

The \ref rttr::policy::ctor::as_object "as_object" policy will create an instance of a class with automatic storage.

Objects with automatic storage duration are automatically destroyed when the block in which they are created exits.
Which is in our case the \ref variant.
However, that means also you don't have to deal with pointers or wrappers. In order to use this creation policy,
the object must be *copy constructible*.

See following example code:
~~~~{.cpp}
using namespace rttr;
struct Foo
{
};

RTTR_REGISTRATION
{
     registration::class_<Foo>("Foo")
                  .constructor<>()
                   (
                       policy::ctor::as_object
                   );
}

int main()
{
  variant var = type::get<Foo>().create();   // creates a new instance of 'Foo' and moves the content into variant 'var'
  std::cout << var.is_type<Foo>();           // prints "true"
  variant var2 = var;                        // creates a new instance of 'Foo', through copy construction
  return 0;                                  // the memory of the two 'Foo' instances is freed automatically
}
~~~~

###as_std_shared_ptr###

The @ref rttr::policy::ctor::as_std_shared_ptr "as_std_shared_ptr" policy will create an 
instance of a class through `std::make_shared<T>`.

That means the object is \ref type::is_wrapper() "wrapped" into a `std::shared_ptr<T>`.
The wrapped object is destroyed and its memory deallocated when either of the following happens: 
- the last remaining variant object (which contains the `shared_ptr` owning the object is destroyed.
- the last remaining variant owning the `shared_ptr` is assigned another object.

The object is destroyed using the default deleter of `std::shared_ptr`.

See following example code:
 
~~~~{.cpp}
using namespace rttr;
struct Foo
{
};

RTTR_REGISTRATION
{
     registration::class_<Foo>("Foo")
                  .constructor<>()
                   (
                       policy::ctor::as_std_shared_ptr
                   );
}

int main()
{
  variant var = type::get<Foo>().create();
  std::cout << var.is_type<std::shared_ptr<Foo>>();  // prints "true"
  return 0;                                          // the memory for contained 'Foo' instance is freed automatically,
}                                                    // because the var object is gone out of scope
~~~~

###as_raw_ptr###

The @ref rttr::policy::ctor::as_raw_ptr "as_raw_ptr" policy will create an instance of a class as raw pointer.

That means the object is created with a `new`-expression and its lifetime lasts 
until it is destroyed using a `delete`-expression. 
In order to invoke the delete expression use the corresponding \ref destructor.

See following example code:
~~~~{.cpp}
using namespace rttr;
struct Foo
{
};

RTTR_REGISTRATION
{
     registration::class_<Foo>("Foo")
                  .constructor<>()
                  (
                      policy::ctor::as_raw_ptr
                  );
}

int main()
{
  variant var = type::get<Foo>().create();
  std::cout << var.is_type<Foo*>();          // prints "true"
  var.get_type().destroy(var);               // free's the memory with 'delete'
  std::cout << var.is_valid();               // prints "false"
  return 0;
}
~~~~


# Properties {#prop_policy_link}

###bind_as_ptr###
The motivation for @ref rttr::policy::prop::bind_as_ptr "bind_as_ptr" policy is to avoid expensive copies when returning a property.
The default registration behaviour of RTTR is to return all values by copying the content in a variant.
When you have primitive data types like integer or doubles you are good to go with the standard binding behaviour.
But when you have big arrays, it would be a waste to always copy the content when retrieving or setting the value, therefore this policy was introduced.

Example:
~~~~{.cpp}
struct Foo
{
    std::vector<int> vec;
};

RTTR_REGISTRATION
{
    using namespace rttr;
    
    registration::class_<Foo>("Foo")
                 .property("vec", &Foo::vec)
                 (
                     policy::prop::bind_as_ptr
                 )
}

int main()
{
    Foo obj;
    property prop = type::get<Foo>().get_property("vec");
    variant var = prop.get_value(obj);
    std::cout << var.is_type<std::vector<int>*>(); // prints "true"
    
    // not really necessary, but remark that now a 'std::vector<int>*' is expected
    prop.set_value(obj, var);
}
~~~~

###as_reference_wrapper###
The motivation for the @ref rttr::policy::prop::as_reference_wrapper "as_reference_wrapper" policy is the 
same like the @ref rttr::policy::prop::bind_as_ptr "prop::bind_as_ptr" policy,
avoid copy of data. The value will be wrapped inside a `std::reference_wrapper<T>`

Example:

~~~~{.cpp}
struct Foo
{
    std::vector<int> vec;
};

RTTR_REGISTRATION
{
    using namespace rttr;
    
    registration::class_<Foo>("Foo")
                 .property("vec", &Foo::vec)
                 (
                     policy::prop::as_reference_wrapper
                 )
}

int main()
{
    Foo obj;
    property prop = type::get<Foo>().get_property("vec");
    variant var = prop.get_value(obj);
    std::cout << var.is_type< std::reference_wrapper<std::vector<int>> >(); // prints "true"
    
    // not really necessary, but remark that now a 'std::reference_wrapper<std::vector<int>>' is expected
    prop.set_value(obj, var);
}
~~~~

# Methods {#meth_policy_link}

###discard_return###
Sometimes it is necessary that the client caller should ignore the return value of a method call.
Therefore @ref rttr::policy::meth::discard_return "discard_return" policy was introduced.

Example:
~~~~{.cpp}
struct Foo
{
    int calculate_func() { return 42; }
};

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Foo>("Foo")
                 .method("get_value", &Foo::calculate_func)
                 (
                     policy::meth::discard_return
                 );
}

int main()
{
    Foo obj;
    method meth  = type::get<Foo>().get_method("calculate_func");
    variant var = meth.invoke(obj);
    std::cout << var.is_type<void>(); // prints "true"
}
~~~~

###return_ref_as_ptr###
The motivation for the @ref rttr::policy::meth::return_ref_as_ptr "return_ref_as_ptr" policy is the same like the @ref rttr::policy::prop::bind_as_ptr "prop::bind_as_ptr" policy.
When you really need to get a reference to the return value of a method call you have to use this policy,
otherwise the returned reference will be copied into the variant.

Example:

~~~~{.cpp}
struct Foo
{
    std::string& get_text() { static std::string text; return text; }
};

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Foo>("Foo")
                 .method("get_text", &Foo::get_text)
                 (
                     policy::meth::return_ref_as_ptr
                 );
}

int main()
{
    Foo obj;
    method meth  = type::get<Foo>().get_method("get_text");
    variant var = meth.invoke(obj);
    std::cout << var.is_type<std::string*>(); // prints "true"
}
~~~~

A closing hint: you can of course build your own policies in that way, that you build wrappers around your properties or methods and 
then bind the wrapper instead of the original accessor.

<hr>

<div type="button" class="btn btn-default doxy-button">[previous](@ref parameter_names_page "Parameter Names")</div><div class="btn btn-default doxy-button">[next](@ref register_plugins "Register Plugins")</div>
