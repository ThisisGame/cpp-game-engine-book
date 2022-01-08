5 minute Tutorial  {#five_minute_tutorial_page}
=================
This shortcut tutorial will show you all basic information to use RTTR.

Class declaration
------------------
Suppose you have a class called `node` which you want to introspect with RTTR.
~~~~{.cpp}
// node.h
#include <rttr/type>

namespace ns_3d
{
class node
{
public:
    node(std::string name, node* parent = nullptr);
    virtual ~node();

    void set_name(const std::string& name);
    const std::string& get_name() const;

    std::vector<node*> get_children() const;
    
    void set_visible(bool visible, bool cascade = true);

    virtual void render();

private:
    node*               m_parent;
    std::string         m_name;
    std::vector<node*>  m_children;

    RTTR_ENABLE()
    RTTR_REGISTRATION_FRIEND
};
}
~~~~
The standard include file of rttr is: `<rttr/type>`<br>
Remark the two added macros: \ref RTTR_ENABLE() and \ref RTTR_REGISTRATION_FRIEND. They are optional.<br>
However, when you use class hierarchies you should add to every class: \ref RTTR_ENABLE().<br>
When you want to reflect private data of a class, add: \ref RTTR_REGISTRATION_FRIEND.

Registration
------------
Now comes the registration part, this is usually done in the corresponding source file.

~~~~{.cpp}
// node.cpp
#include <rttr/registration>

RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace ns_3d;
    
    registration::class_<node>("ns_3d::node")
        .constructor<std::string, node*>()
        (
            policy::ctor::as_std_shared_ptr, // should create an instance of the class as shared_ptr<ns_3d::node>
            default_arguments(nullptr)       // second argument is optional, so we provide the default value for it
        )
        .property("name", &node::get_name, &node::set_name)
        (
            metadata("TOOL_TIP", "Set the name of node.")  // stores metadata associated with this property
        )
        // register directly a member object pointer; mark it as 'private'
        .property("parent", &ns_3d::node::m_parent, registration::private_access)
        .property_readonly("children", &node::get_children) // a read-only property; so not set possible
        .method("set_visible", &node::set_visible)
        (
            default_arguments(true),              // the default value for 'cascade'
            parameter_names("visible", "cascade") // provide the names of the parameter; optional, but might be useful for clients
        )
        .method("render", &node::render)
        ;
}
~~~~
The standard include file for registration types in rttr is: `<rttr/registration>`<br>Include this file only when you want to register something.
The \ref rttr::registration "registration" class is here the entry point. 
This registration process creates internally wrapper classes, which store for example function pointers or object pointers of the specific class.
For these pointers you have to provide the data manually.

Yeah, and that's it. Now you can use RTTR to retrieve this information.

Basic usage
-----------
~~~~{.cpp}
#include <rttr/type>
#include <iostream>

int main(int argc, char *argv[])
{
    using namespace rttr;

    type t = type::get_by_name("ns_3d::node");
    // will create an instance of ns_3d::node as std::shared_ptr<ns_3d::node>
    variant var = t.create({std::string("MyNode")});
    std::cout << var.get_type().get_name() << "\n";

    // sets/gets a property
    property prop = t.get_property("name");
    // remark: you can also set a member, although the instance is of type: 'std::shared_ptr<T>'
    prop.set_value(var, std::string("A New Name"));
    std::cout << prop.get_value(var).to_string() << "\n";
    // retrieve the stored meta data of the property
    std::cout << "MetaData TOOL_TIP: " << prop.get_metadata("TOOL_TIP").to_string() << "\n";

    // invoke a method
    method meth = t.get_method("set_visible");
    // remark: the 2nd argument will be provided automatically, because it has a default argument
    variant ret = meth.invoke(var, true);
    // a valid return value indicates a successful invoke
    std::cout << std::boolalpha << "invoke of method 'set_visible' was successfully: " << ret.is_valid() << "\n\n";

    // retrieve all properties
    std::cout << "'node' properties:" << "\n";
    for (auto& prop : t.get_properties())
    {
        std::cout << "  name: " << prop.get_name() << "\n";
        std::cout << "    type: " << prop.get_type().get_name() << "\n";
    }
    std::cout << "\n";
    // retrieve all methods
    std::cout << "'node' methods:" << "\n";
    for (auto& meth : t.get_methods())
    {
        std::cout << "  name: " << meth.get_name();
        std::cout << "  signature: " << meth.get_signature() << "\n";
        for (auto& info : meth.get_parameter_infos())
        {
            std::cout << "    param " << info.get_index() << ": name: "<< info.get_name() << "\n";
        }
    }

    return 0;
}
~~~~
Remark, that there is actual no include of `node.h`.
See the output below:
###Output###

    class std::shared_ptr<class ns_3d::node>
    A New Name
    MetaData TOOL_TIP: Set the name of node.
    invoke of method 'set_visible' was successfully: true

    'node' properties:
      name: name
        type: std::string
      name: parent
        type: ns_3d::node*
      name: children
        type: class std::vector<class ns_3d::node *,class std::allocator<class ns_3d::node *> >

    'node' methods:
      name: set_visible  signature: set_visible( bool, bool )
        param 0: name: visible
        param 1: name: cascade
      name: render  signature: render( )


Derived classes
----------------
Suppose you create now from `node` a derived class called `mesh`.
~~~~{.cpp}
// mesh.h
#include <rttr/type>

class mesh : public node
{
public:
    static std::shared_ptr<mesh> create_mesh(std::string file_name);

    virtual void render();

    enum class render_mode
    {
        POINTS,
        WIREFRAME,
        SOLID
    };

    void set_render_mode(render_mode mode);
    render_mode get_render_mode() const;

private:
    mesh(std::string name, node* parent = nullptr);

    RTTR_ENABLE(node) // include the names of all direct base classes
};
~~~~
Now you put in \ref RTTR_ENABLE() the name of the base class, in this case: `node`.

Registration part
-----------------

~~~~{.cpp}
// mesh.cpp
RTTR_REGISTRATION
{
    using namespace ns_3d;
    using namespace rttr;
    registration::class_<mesh>("ns_3d::mesh")
        .constructor(&mesh::create_mesh)
        .property("render_mode", &mesh::get_render_mode, &mesh::set_render_mode)
        .enumeration<mesh::render_mode>("ns_3d::render_mode")
        (
            value("POINTS",     mesh::render_mode::POINTS),
            value("WIREFRAME",  mesh::render_mode::WIREFRAME),
            value("SOLID",      mesh::render_mode::SOLID)
        );
}
~~~~
Remark that it is not necessary to register again the `render()` method.

Basic Usage
-----------
~~~~{.cpp}
#include <rttr/type>
#include <iostream>
#include "mesh.h"

int main(int argc, char *argv[])
{
    std::shared_ptr<ns_3d::node> obj = ns_3d::mesh::create_mesh("House.obj");
    
    std::cout << type::get(obj).get_name() << "\n";                     // prints 'std::shared_ptr<ns_3d::node>'
    std::cout << type::get(obj).get_wrapped_type().get_name() << "\n";  // prints 'ns_3d::node*'
    std::cout << type::get(*obj.get()).get_name() << "\n";              // prints 'ns_3d::mesh'

    // for glvalue expressions the most derived type is returned, in this case: 'ns_3d::mesh'; like typeid()
    type t = type::get(*obj.get());
    
    std::cout << "\n";

    std::cout << "'mesh' properties:" << "\n";
    for (auto& prop : t.get_properties())
    {
        std::cout << "  name: " << prop.get_name() << "\n";
        std::cout << "    type: " << prop.get_type().get_name() << "\n";
    }
    
    property prop = t.get_property("render_mode");
    // set the property of the derived type, although we hold only a shared_ptr of the base class
    bool ret = prop.set_value(obj, ns_3d::mesh::render_mode::SOLID); // yields to 'true'; when set was possible

    std::cout << "\n";

    method meth = t.get_method("render");
    std::cout << meth.get_declaring_type(). get_name() << "\n"; // prints 'ns_3d::node'

    std::shared_ptr<ns_3d::mesh> obj_derived = std::dynamic_pointer_cast<ns_3d::mesh>(obj);
    // invoke the method, although we have the most derived type now
    variant var = meth.invoke(obj_derived);
    std::cout << std::boolalpha << "invoke of method 'render' was successfully: " << var.is_valid() << "\n";
}
~~~~

See the output below:
###Output###

    class std::shared_ptr<class ns_3d::node>
    ns_3d::node*
    ns_3d::mesh

    'mesh' properties:
      name: name
        type: std::string
      name: parent
        type: ns_3d::node*
      name: children
        type: class std::vector<class ns_3d::node *,class std::allocator<class ns_3d::node *> >
      name: render_mode
        type: ns_3d::render_mode

    ns_3d::node
    invoke of method 'render' was successfully: true

That's it. 

However, in order to see all the possibilities of RTTR, it is recommend to go through the in-depth tutorial: [Start Tutorial](@ref rttr_type_get_page)       

<hr>

<div class="btn btn-default doxy-button">[back](@ref tutorial_page "Tutorial")</div>
