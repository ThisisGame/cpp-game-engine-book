Variant {#register_variant_page}
=======

The variant class acts as return value container for properties and methods. This class allows to store data of any type and convert between these types transparently.
It can hold one value at a time (using containers you can hold multiple types e.g. `std::vector<int>`). 
Remark that the content is copied into the variant class. Even raw arrays (e.g. `int[10]`) are copied.
When you would like to avoid copies, use pointer types or wrap your type in a `std::reference_wrapper<T>`
A typical usage is the following example:

~~~~{.cpp}
    using namespace rttr;
    variant var;
    var = 23;                               // copy integer
    int x = var.to_int();                   // x = 23
    
    var = "Hello World";                    // var contains now a std::string (implicit conversion of string literals to std::string)
    int y = var.to_int();                   // y = 0, because invalid conversion
    
    var = "42";                             // contains a std::string
    std::cout << var.to_int();              // convert std::string to integer and prints "42"
    
    int my_array[100];
    var = my_array;                         // copies the content of my_array into var
    auto& arr = var.get_value<int[100]>();  // extracts the content of var by reference
~~~~

<hr>

<div type="button" class="btn btn-default doxy-button">[previous](@ref register_enums_page "Register Enums")</div><div class="btn btn-default doxy-button">[next](@ref register_classes_page "Register Classes")</div>
