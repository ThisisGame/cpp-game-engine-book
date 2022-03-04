# stduuid
A C++17 cross-platform single-header library implementation **for universally unique identifiers**, simply know as either UUID or GUID (mostly on Windows). A UUID is a 128-bit number used to uniquely identify information in computer systems, such as database table keys, COM interfaces, classes and type libraries, and many others.

[![Build Status](https://travis-ci.org/mariusbancila/stduuid.svg?branch=master)](https://travis-ci.org/mariusbancila/stduuid)
[![Tests status](https://ci.appveyor.com/api/projects/status/0kw1n2s2xqxu5m62?svg=true&pendingText=tests%20-%20pending&failingText=tests%20-%20FAILED&passingText=tests%20-%20OK)](https://ci.appveyor.com/project/mariusbancila/stduuid)

For information about UUID/GUIDs see:
* [Universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier)
* [A Universally Unique IDentifier (UUID) URN Namespace](https://www.ietf.org/rfc/rfc4122.txt)

## Library overview
Although the specification puts the uuid library in the `std` namespace, this implementation uses the namespace `uuids` for this purpose, in order to make the library usable without violating the restrictions imposed on the `std` namespace. The following types and utilities are available:

Basic types:

| Name | Description |
| ---- | ----------- |
| `uuid` | a class representing a UUID; this can be default constructed (a nil UUID), constructed from a range (defined by a pair of iterators), or from a `span`. |
| `uuid_variant` | a strongly type enum representing the type of a UUID |
| `uuid_version` | a strongly type enum representing the version of a UUID |

Generators:

| Name | Description |
| ---- | ----------- |
| `basic_uuid_random_generator` | a function object that generates version 4 UUIDs using a pseudo-random number generator engine. |
| `uuid_random_generator` | a `basic_uuid_random_generator` using the Mersenne Twister engine (`basic_uuid_random_generator<std::mt19937>`) |
| `uuid_name_generator` | a function object that generates version 5, name-based UUIDs using SHA1 hashing. |
| `uuid_system_generator` | a function object that generates new UUIDs using operating system resources (`CoCreateGuid` on Windows, `uuid_generate` on Linux, `CFUUIDCreate` on Mac) <br><br> **Note**: This is not part of the standard proposal. It is available only if the `UUID_SYSTEM_GENERATOR` macro is defined. |
| `uuid_time_generator` | an experimental function object that generates time-based UUIDs.<br><br> **Note**:This is an experimental feature and should not be used in any production code. It is available only if the `UUID_TIME_GENERATOR` macro is defined. |

Utilities:

| Name | Description |
| ---- | ----------- |
| `std::swap<>` | specialization of `swap` for `uuid` |
| `std::hash<>` | specialization of `hash` for `uuid` (necessary for storing UUIDs in unordered associative containers, such as `std::unordered_set`) |

Constants: 

| Name | Description |
| ---- | ----------- |
| `uuid_namespace_dns` | Namespace ID for name-based uuids when name string is a fully-qualified domain name. |
| `uuid_namespace_url` | Namespace ID for name-based uuids when name string is a URL. |
| `uuid_namespace_oid` | Namespace ID for name-based uuids when mame string is an ISO OID (See https://oidref.com/, https://en.wikipedia.org/wiki/Object_identifier). |
| `uuid_namespace_x500` | Namespace ID for name-based uuids when name string is an X.500 DN, in DER or a text output format (See https://en.wikipedia.org/wiki/X.500, https://en.wikipedia.org/wiki/Abstract_Syntax_Notation_One). |

Other:

| Name | Description |
| ---- | ----------- |
| `operator==` and `operator!=` | for UUIDs comparison for equality/inequality |
| `operator<` | for comparing whether one UUIDs is less than another. Although this operation does not make much logical sense, it is necessary in order to store UUIDs in a std::set. |
| `operator<<` | to write a UUID to an output stream using the canonical textual representation. |
| `to_string()` | creates a string with the canonical textual representation of a UUID. |

## Library history
This library is an implementation of the proposal [P0959](P0959.md).

**As the proposal evolves based on the standard committee and the C++ community feedback, this library implementation will reflect those changes.** 

See the revision history of the proposal for history of changes.

## Using the library
The following is a list of examples for using the library:
* Creating a nil UUID

  ```cpp
  uuid empty;
  assert(empty.is_nil());
  ```

* Creating a new UUID

  ```cpp
  uuid const id = uuids::uuid_system_generator{}();
  assert(!id.is_nil());
  assert(id.version() == uuids::uuid_version::random_number_based);
  assert(id.variant() == uuids::uuid_variant::rfc);
  ```

* Creating a new UUID with a default random generator

  ```cpp
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size> {};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};

  uuid const id = gen();
  assert(!id.is_nil());
  assert(id.as_bytes().size() == 16);
  assert(id.version() == uuids::uuid_version::random_number_based);
  assert(id.variant() == uuids::uuid_variant::rfc);
  ```

* Creating a new UUID with a particular random generator

  ```cpp
  std::random_device rd;
  auto seed_data = std::array<int, 6> {};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::ranlux48_base generator(seq);

  uuids::basic_uuid_random_generator<std::ranlux48_base> gen(&generator);
  uuid const id = gen();
  assert(!id.is_nil());
  assert(id.as_bytes().size() == 16);
  assert(id.version() == uuids::uuid_version::random_number_based);
  assert(id.variant() == uuids::uuid_variant::rfc);
  ```

* Creating a new UUID with the name generator

  ```cpp
  uuids::uuid_name_generator gen(uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43").value());
  uuid const id = gen("john");

  assert(!id.is_nil());
  assert(id.version() == uuids::uuid_version::name_based_sha1);
  assert(id.variant() == uuids::uuid_variant::rfc);
  ```

* Create a UUID from a string

  ```cpp
  auto str = "47183823-2574-4bfd-b411-99ed177d3e43"s;
  auto id = uuids::uuid::from_string(str);
  assert(id.has_value());
  assert(uuids::to_string(id.value()) == str);

  // or

  auto str = L"47183823-2574-4bfd-b411-99ed177d3e43"s;
  uuid id = uuids::uuid::from_string(str).value();
  assert(uuids::to_string<wchar_t>(id) == str);
  ```

* Creating a UUID from a sequence of 16 bytes

  ```cpp
  std::array<uuids::uuid::value_type, 16> arr{{
     0x47, 0x18, 0x38, 0x23,
     0x25, 0x74,
     0x4b, 0xfd,
     0xb4, 0x11,
     0x99, 0xed, 0x17, 0x7d, 0x3e, 0x43}};
  uuid id(arr);

  assert(uuids::to_string(id) == "47183823-2574-4bfd-b411-99ed177d3e43");

  // or

  uuids::uuid::value_type arr[16] = {
     0x47, 0x18, 0x38, 0x23,
     0x25, 0x74,
     0x4b, 0xfd,
     0xb4, 0x11,
     0x99, 0xed, 0x17, 0x7d, 0x3e, 0x43 };
  uuid id(std::begin(arr), std::end(arr));
  assert(uuids::to_string(id) == "47183823-2574-4bfd-b411-99ed177d3e43");
  
  // or 
  
  uuids::uuid id{{
     0x47, 0x18, 0x38, 0x23,
     0x25, 0x74,
     0x4b, 0xfd,
     0xb4, 0x11,
     0x99, 0xed, 0x17, 0x7d, 0x3e, 0x43}};

  assert(uuids::to_string(id) == "47183823-2574-4bfd-b411-99ed177d3e43");
  ```
  
* Comparing UUIDs

  ```cpp
  uuid empty;
  uuid id = uuids::uuid_system_generator{}();
  assert(empty == empty);
  assert(id == id);
  assert(empty != id);
  ```

* Swapping UUIDs

  ```cpp
  uuid empty;
  uuid id = uuids::uuid_system_generator{}();

  assert(empty.is_nil());
  assert(!id.is_nil());

  std::swap(empty, id);

  assert(!empty.is_nil());
  assert(id.is_nil());

  empty.swap(id);

  assert(empty.is_nil());
  assert(!id.is_nil());
  ```
  
* Converting to string

  ```cpp
  uuid empty;
  assert(uuids::to_string(empty) == "00000000-0000-0000-0000-000000000000");
  assert(uuids::to_string<wchar_t>(empty) == L"00000000-0000-0000-0000-000000000000");
  ```

* Using with an orderered associative container

  ```cpp
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size> {};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 engine(seq);
  uuids::uuid_random_generator gen(&engine);
   
  std::set<uuids::uuid> ids{uuid{}, gen(), gen(), gen(), gen()};

  assert(ids.size() == 5);
  assert(ids.find(uuid{}) != ids.end());
  ```

* Using in an unordered associative container

  ```cpp
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size> {};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 engine(seq);
  uuids::uuid_random_generator gen(&engine);

  std::unordered_set<uuids::uuid> ids{uuid{}, gen(), gen(), gen(), gen()};

  assert(ids.size() == 5);
  assert(ids.find(uuid{}) != ids.end());
  ```

* Hashing UUIDs

  ```cpp
  using namespace std::string_literals;
  auto str = "47183823-2574-4bfd-b411-99ed177d3e43"s;
  uuid id = uuids::uuid::from_string(str).value();

  auto h1 = std::hash<std::string>{};
  auto h2 = std::hash<uuid>{};
  assert(h1(str) == h2(id));
  ```
  
### Random uuids

If you generate uuids using the `basic_uuid_random_generator` and [std::random_device](https://en.cppreference.com/w/cpp/numeric/random/random_device) to seed a generator, keep in mind that this might not be non-deterministic and actually generate the same sequence of numbers:

> std::random_device may be implemented in terms of an implementation-defined pseudo-random number engine if a non-deterministic source (e.g. a hardware device) is not available to the implementation. In this case each std::random_device object may generate the same number sequence.

This could be a problem with MinGW. See [Bug 85494 - implementation of random_device on mingw is useless](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85494). This was fixed in GCC 9.2.

A portable alternative is to use the [Boost.Random](https://www.boost.org/doc/libs/1_78_0/doc/html/boost_random.html) library.

## Support
The library is supported on all major operating systems: Windows, Linux and Mac OS.

## Dependencies
If you use the library in a project built with C++20, then you can use `std::span`. This is used by default, if the header is supported by your compiler. The check is done with the [__cpp_lib_span](https://en.cppreference.com/w/cpp/utility/feature_test) feature-test macro.

Otherwise, such as when building with C++17, `std::span` is not available. However, the [Microsoft Guidelines Support Library](https://github.com/Microsoft/GSL) (aka GSL) can be used for its `span` implementation (from which the standard version was defined). The stduuid library defaults to use this implementation if `std::span` is not available.

To ensure `gsl::span` can be used, make sure the GSL library is available, and the GSL include directory is listed in the include directories for the project.

If you use cmake to build the test project, make sure the variable called `UUID_USING_CXX20_SPAN` is not defined, or it's value is `OFF` (this is the default value). This will ensure the `gsl` directory will be included in the search list of header directories.

## Testing
A testing project is available in the sources. To build and execute the tests do the following:
* Clone or download this repository
* Create a `build` directory in the root directory of the sources
* Run the command `cmake ..` from the `build` directory; if you do not have CMake you must install it first.
* Build the project created in the previous step
* Run the executable.

**Examples**

To generate a project files for Visual Studio 2019, you can run the following commands:
```
cd build
cmake -G "Visual Studio 17" -A x64 ..
```

To enable the operating system uuid generator set the `UUID_SYSTEM_GENERATOR` variable to `ON`.
```
cd build
cmake -G "Visual Studio 17" -A x64 -DUUID_SYSTEM_GENERATOR=ON ..
```

To enable the experimental time-based uuid generator set the `UUID_TIME_GENERATOR` variable to `ON`.
```
cd build
cmake -G "Visual Studio 17" -A x64 -DUUID_TIME_GENERATOR=ON ..
```

## Credits
The SHA1 implementation is based on the [TinySHA1](https://github.com/mohaps/TinySHA1) library.
