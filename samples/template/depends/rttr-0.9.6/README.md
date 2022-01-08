[![Version](https://badge.fury.io/gh/rttrorg%2Frttr.svg)](https://github.com/rttrorg/rttr/releases/latest)
[![Travis status](https://travis-ci.org/rttrorg/rttr.svg?branch=master)](https://travis-ci.org/rttrorg/rttr)
[![Appveyor status](https://ci.appveyor.com/api/projects/status/github/rttrorg/rttr?svg=true&branch=master)](https://ci.appveyor.com/project/acki-m/rttr)
[![Coverage Status](https://coveralls.io/repos/rttrorg/rttr/badge.svg?branch=master&service=github)](https://coveralls.io/github/rttrorg/rttr)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/783/badge)](https://bestpractices.coreinfrastructure.org/projects/783)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/9821799170644782ac8d7885d393e686)](https://www.codacy.com/app/acki-m/rttr?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=rttrorg/rttr&amp;utm_campaign=Badge_Grade)
[![Documentation](https://img.shields.io/badge/docs-latest-blue.svg)](http://www.rttr.org/doc/master/classes.html)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/rttrorg/rttr/master/LICENSE.txt)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JQ65KGGCSUZMS)

<a target="_blank" href="http://www.rttr.org/news/major-release-0-9-6">!New Release - 0.9.6!</a>
==

RTTR
====
> C++ Reflection Library

RTTR stands for Run Time Type Reflection.
It describes the ability of a computer program to introspect and modify an object at runtime. It is also the name of the library itself, which is written in C++ and released as open source library.
You can find more information on: <a target="_blank" href="http://www.rttr.org">www.rttr.org</a>

----------

How to Use
----------

### Manual registration
```cpp
#include <rttr/registration>
using namespace rttr;

struct MyStruct { MyStruct() {}; void func(double) {}; int data; };

RTTR_REGISTRATION
{
    registration::class_<MyStruct>("MyStruct")
         .constructor<>()
         .property("data", &MyStruct::data)
         .method("func", &MyStruct::func);
}
```
### Iterate over members
```cpp
type t = type::get<MyStruct>();
for (auto& prop : t.get_properties())
    std::cout << "name: " << prop.get_name();

for (auto& meth : t.get_methods())
    std::cout << "name: " << meth.get_name();
```

### Constructing types
```cpp
type t = type::get_by_name("MyStruct");
variant var = t.create();    // will invoke the previously registered ctor

constructor ctor = t.get_constructor();  // 2nd way with the constructor class
var = ctor.invoke();
std::cout << var.get_type().get_name(); // prints 'MyStruct'
```

### Set/get properties
```cpp
MyStruct obj;

property prop = type::get(obj).get_property("data");
prop.set_value(obj, 23);

variant var_prop = prop.get_value(obj);
std::cout << var_prop.to_int(); // prints '23'
```

### Invoke Methods:
```cpp
MyStruct obj;

method meth = type::get(obj).get_method("func");
meth.invoke(obj, 42.0);

variant var = type::get(obj).create();
meth.invoke(var, 42.0);
```

Features
---------
- reflect constructors, methods, data member or enums
- classes; with *single*-, *multiple*- and *virtual*-inheritance
- constructors (arbitrary argument count)
- methods (*virtual*, *abstract*, *overloaded*, arbitrary argument count)
- arrays (incl. raw-arrays; arbitrary dimension count)
- ability to invoke properties and methods of classes from any arbitrary class level
- no header pollution; the reflection information is created in the cpp file to minimize compile time when modifying the data
- working with custom types without the need of having the declaration of the type available at compile time (useful for plugins)
- possibility to add additional *metadata* to all reflection objects
- possibility to add *default arguments* to methods or constructors
- adjust registration behaviour through *policies*
- minimal macro usage
- **no** additional 3rd party dependencies are needed
- **no** rtti required; contains a faster and across shared libraries working replacement
- **no** exceptions (this feature come with cost and is also regularly disabled on consoles)
- **no** external compiler or tool needed, only standard ISO C++11

Portability
-----------
Tested and compiled with:
- Microsoft Visual Studio 2013, 2015 & 2017
- GCC 4.8.1
- Clang 3.7
- MinGW 4.8.2

License
-------

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

RTTR is released under the terms of the [MIT](LICENSE.txt) license,
so it is free to use in your free or commercial projects.

Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


Installation
------------
The installation guide can be found [here][rttr.install].


Get Started:
------------
Take a look at the [documentation][rttr.docs] or start with the [tutorial][rttr.tutorial].

Donation:
---------
When you use RTTR and you would like to say thank you for its development,
I am happy to receive any donation.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JQ65KGGCSUZMS)

<!-- Links -->
[rttr.docs]: http://www.rttr.org/doc/master/classes.html
[rttr.install]: http://www.rttr.org/doc/master/building_install_page.html
[rttr.tutorial]: http://www.rttr.org/doc/master/tutorial_page.html
