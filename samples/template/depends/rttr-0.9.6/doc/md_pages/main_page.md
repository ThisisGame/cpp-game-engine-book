RTTR - Run Time Type Reflection {#main_page}
===============================

Introduction
------------

The _run time type reflection_ (RTTR) library adds the missing feature of reflection to C++.
That means the programmer can introspect an object at runtime of what kind of properties, methods and constructors it consist.
This is extremely useful when a tight but also high dynamic coupling between software modules is required.
The main use cases are for example serialisation, UI creation, binding to arbitrary programming languages (JavaScript, Lua etc.) and network communication.

It works without any extra preprocess pass to reflect your type, only standard C++11 language features are used.
However, in order to reflect your types it is required to manually register your classes with its properties and methods.
While registering methods it is usually not necessary to know the exact signature, since the library
will generate a wrapper class for this depending on the compile-time type of the method.
In order to avoid header pollution, the registration process should be done completely in the cpp file. There is also no need to derive from some super base class.

RTTR is released under the terms of the [MIT license](@ref license_page).

<hr>

Features
--------
RTTR supports:
- a faster and across shared libraries working replacement of rtti
- classes; with *single*\-, *multiple*\- and *virtual*\-inheritance (requires one macro placement inside class)
- constructors (arbitrary argument count)
- properties
- methods (*virtual*, *abstract*, *overloaded*, arbitrary argument count)
- enums (C++11 enum class)
- arrays (incl. raw-arrays; arbitrary dimension count)
- ability to invoke properties and methods of classes from any arbitrary class level
- no header pollution; the reflection information is created in the cpp file to minimize compile time when modifying the data
- working with custom types without the need of having the declaration of the type available at compile time (useful for plugins)
- possibility to add additional *metadata* to all reflection objects
- possibility to add *default arguments* to methods or constructors
- adjust registration behaviour through *policies*
- minimal macro usage
- **no** exceptions (this feature come with <a target="_blank" href=http://preshing.com/20110807/the-cost-of-enabling-exception-handling/>cost</a> and is also regularly disabled on consoles)
- **no** external compiler or tool needed, only standard ISO C++11

Portability
-----------
RTTR is compiled and tested on following platforms:
- Microsoft Visual Studio 12 (2013), Update 4 - x32 and x64
- gcc 4.8.1 - x32 and x64

Content
-------
- [Tutorial](@ref tutorial_page)
- [Building & Installation](@ref building_install_page)
- [License](@ref license_page)
- [Classes](./annotated.html)

<hr>

Acknowledgements
----------------
The following persons has been very helpful:

- Ville Voutilainen, for reviewing and improving the code base
- Paul Mensonides, for given me an introduction in template and preprocessor metaprogramming

