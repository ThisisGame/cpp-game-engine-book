Classes {#register_classes_page}
=======

For registering classes in RTTR you use a class called @ref rttr::registration::class_<T> "registration::class_<T>". 
Its name is supposed to resemble the C++ keyword, to make it look more intuitive. 
It has member functions for register @ref rttr::registration::class_<T>::constructor() "constructors", @ref rttr::registration::class_<T>::property() "properties",
@ref rttr::registration::class_<T>::method() "methods" and @ref rttr::registration::class_<T>::enumeration() "enums".
These functions have the same interface and work in the same way like register the global symbols.
Every call to these member functions, will return @ref rttr::registration::bind "bind" object, in order to chain more registration calls.

Let's start with a simple example. Consider the following C++ class:

~~~~{.cpp}
#include <rttr/type>

// test class.h
struct test_class
{
    test_class(int value) : m_value(value) {}
    void print_value() const { std::cout << m_value; }

    int m_value;

    RTTR_ENABLE()
};

~~~~

The registration process is now done at global scope in the cpp file.

~~~~{.cpp}
// test_class.cpp
#include <rttr/registration>

RTTR_REGISTRATION
{
    using namespace rttr;
        
    registration::class_<test_class>("test_class")
                 .constructor<int>()
                 .method("print_value", &test_class::print_value)
                 .property("value", &test_class::m_value);
}
~~~~

This will register the class `test_class` with a constructor that takes an integer as argument, a member function with the name `print_value`
and a property called `value`.

The property name has to be unique for this class type, but derived classes can register another property with the same name again.
Member functions can be overloaded, so a method can be registered with an already existing name multiple times. 
However when there exist a method with the same name and signature, then this function will not be registered and discarded.

Overloaded member functions
---------------------------
In order to register a overloaded member function, you have to disambiguate the member function pointer you pass to @ref rttr::registration::class_<T>::method() "method".
Therefore a helper function can be used: @ref rttr::select_overload() "select_overload<T>(T (ClassType::*func))"

Here's an example illustrating this:

~~~~{.cpp}
struct Foo
{
    void f() {}
    void f(int) {}
    void f(int) const {}
};


RTTR_REGISTRATION
{
    using namespace rttr;
    
    registration::class_<Foo>("Foo")
                 .method("f", select_overload<void(void)>(&Foo::f))
                 .method("f", select_overload<void(int)>(&Foo::f))
                 .method("f", select_const(&Foo::f));                // register a overloaded const method
}
~~~~

The last function is a overloaded const function, use therefore the 
@ref rttr::select_const() "select_const()" function.

Register constructor
--------------------

RTTR allows your to register constructors for classes. 
Because C++ doesn't allow to retrieve the member function pointer of a constructor you have to explicit specify all data types of a constructor.

Consider following class with three constructors:

~~~~{.cpp}
struct Foo
{
    Foo();
    Foo(int, double);
    Foo(const std::string&);
    
    static Foo* create();
};
~~~~

For registering three `Foo` constructors you now have to specify every parameter as template parameter 
in the member function @ref rttr::registration::class_<T>::constructor() "constructor()".
As second option, it is possible to register a static function as constructor.
In order to do this, just forward the function pointer.

~~~~{.cpp}
RTTR_REGISTRATION
{
    using namespace rttr;
    
    registration::class_<Foo>("Foo")
                 .constructor<>()
                 .constructor<int,double>()
                 .constructor<const std::string&>()
                 .constructor(&Foo::create);
}
~~~~

When a constructor is registered a destructor is registered automatically.
The used default policy for creating an instance is @ref rttr::policy::ctor::as_object "policy::ctor::as_object".

Register class properties
-------------------------
Register a public property can be easily done, consider following class:

~~~~{.cpp}
struct Foo
{
    int value;
};
~~~~

This class is registered like this:

~~~~{.cpp}
RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Foo>()
        .property("value", &Foo::value);
}
~~~~

With the @ref rttr::registration::class_<T>::property() "property()" member function you will register the member variable `Foo::value` 
with read and write access.

###Read Only Properties###
When you want a register a property with `read-only` access, 
then this is also possible with @ref rttr::registration::class_<T>::property_readonly "property_readonly()" member function.

~~~~{.cpp}
RTTR_REGISTRATION
{
    using namespace rttr;
    
    registration::class_<Foo>()
        .property_readonly("value", &Foo::value);
}
~~~~

###Private Properties###

When you have a class and the property is declared in private scope, then you can still register this property when you insert the macro: \ref RTTR_REGISTRATION_FRIEND
inside the class.
~~~~{.cpp}
class Foo
{
private:
    int value;

    RTTR_REGISTRATION_FRIEND
};
~~~~

This will make this class a friend to the registration system.

###Getter Setter For Properties###
You can also register getter and setter functions and make them look as if they were a public data member. Consider the following class:
~~~~{.cpp}
class Foo
{
public:
    void set_value(int x) { m_value = x; }
    int get_value() const { return m_value; }
    
private:
    int m_value;
};
~~~~

This is the registration code:

~~~~{.cpp}
RTTR_REGISTRATION
{
    using namespace rttr;
    
    registration::class_<Foo>()
        .property("value", &Foo::get_value, &Foo::set_value);
}
~~~~

This way, accessing the property will now call these functions, instead the property directly. Remark that the getter function needs the be `const`.

The following sub sections will now show how to retrieve these informations for creating, invoking and setting properties of an instance of this class.

Access Level Of Class Members
------------------------------
With RTTR it is also possible to specify on of the three access modifiers during the registration.

~~~~{.cpp}
using namespace rttr;

struct Foo
{
public:
    Foo() {}
protected:
    void func() {}
private:
    int value;

    RTTR_REGISTRATION_FRIEND
};

RTTR_REGISTRATION
{
    registration::class_<Foo>("Foo")
                  .constructor<>(registration::public_access)   // not necessary, because that is the default
                  .method("func", &Foo::func, registration::protected_access)
                  .property("value", &Foo::value, registration::private_access);
}
~~~~
The access level has to be specified as last argument in the corresponding \ref rttr::registration::class_ "class_" registration function.
Use therefore one of the three static member variables in the \ref rttr::registration "registration" class.
This information can be retrieved at runtime for \ref rttr::property::get_access_level() "properties", \ref rttr::method::get_access_level() "methods" or 
\ref rttr::constructor::get_access_level() "constructors". via the function `get_access_level()`, which will return a enum value of type \ref rttr::access_levels "access_levels".


Create/destroy of classes
-------------------------
There are two options for creating/destroying a class. 
-# use just the @ref rttr::type::create "type" interface or 
-# retrieve a @ref rttr::constructor "constructor" and @ref rttr::destructor "destructor" object from the @ref rttr::type "type" class.

~~~~{.cpp}
int main()
{
    using namespace rttr;
    // option 1
    type class_type = type::get_by_name("test_class");
    if (class_type)
    {
        variant obj = class_type.create({23});
        if (obj.get_type().is_pointer())
            class_type.destroy(obj);
    }
    // option 2
    if (class_type)
    {
       constructor ctor = class_type.get_constructor({type::get<int>()});
       variant obj = ctor.invoke(23);
       if (obj.get_type().is_pointer())
       {
            destructor dtor = class_type.get_destructor();
            dtor.invoke(obj);
       }
    }
}
~~~~
The objects which are constructed are created on the heap and stored as pointer in the variant object.

Invoke member functions
-----------------------
Invoking a member function works in the same way like invoking global function. The only difference is, that you have to provide
the instance of the class.

~~~~{.cpp}
int main()
{
    using namespace rttr;
    
    test_class obj(42);
    type class_type = type::get_by_name("test_class");
    // option 1
    class_type.invoke("print_value", obj, {}); // print 42
    
    // option 2
    method print_meth = class_type.get_method("print_value");
    print_meth.invoke(obj); // prints "42"
}
~~~~

The invoke function also except to use variants. So when you create the object via the type constructor you can use the returned variant to invoke a method:

~~~~{.cpp}
int main()
{
    using namespace rttr;
    
    variant obj_var = type::get_by_name("test_class").create({42});
    type::get_by_name("test_class").invoke("print_value", obj_var, {}); // print 42
}
~~~~

It is possible to invoke a method, when the instance is wrapped inside a wrapper class, for example `std::shared_ptr<T>`.

~~~~{.cpp}
int main()
{
    std::shared_ptr<test_class> obj = std::make_shared<test_class>(23);

    method meth = type::get_by_name("test_class").get_method("print_value");

    method.invoke(obj);         // successful invoke
    method.invoke(obj.get());   // successful invoke
    method.invoke(*obj.get());  // successful invoke

    variant var = obj;
    // or use the variant
    method.invoke(var);         // successful invoke

    return 0;
}
~~~~
When you want to use RTTR with a custom wrapper type, you have provide a specialization of the class template @ref rttr::wrapper_mapper<T> "wrapper_mapper<T>".

Set/Get property of a class
---------------------------
Properties can be also set and get in two steps:

~~~~{.cpp}
int main()
{
    using namespace rttr;
    
    test_class obj(0);
    type class_type = type::get_by_name("test_class");
    // option 1
    bool success = class_type.set_property_value("value", obj, 50);
    std::cout << obj.m_value; // prints "50"
    
    // option 2
    property prop = class_type.get_property("value");
    success = prop.set_value(obj, 24);
    std::cout << obj.m_value; // prints "24"
}
~~~~

In difference to the global properties, a valid \ref type object and an instance (object) of the class is now needed to set and get the value.
It doesn't matter in what hierarchy level the object is or if its a pointer, an object on the stack or wrapped inside a variant.
RTTR will try to cast the given object to the class type where the property was registered to.

<hr>

<div type="button" class="btn btn-default doxy-button">[previous](@ref register_variant_page "Variant")</div><div class="btn btn-default doxy-button">[next](@ref register_metadata_page "Register Metadata")</div>
