# Change Log
All notable changes to this project will be documented in this file. This project adheres to [Semantic Versioning](http://semver.org/).

## [v0.9.6](https://github.com/rttrorg/rttr/releases/tag/v0.9.6) (2018-03-15)
[Full Changelog](https://github.com/rttrorg/rttr/compare/v0.9.5...v0.9.6)

Blog Entry: www.rttr.org/news/major-release-0-9-6

[![Generic badge](https://img.shields.io/badge/►-Features-98B808.svg?colorA=98B808)](https://github.com/acki-m/rttr/blob/feature-adjust-readme/ChangeLog.md)

- Added `variant_associative_view` class [\#57](https://github.com/rttrorg/rttr/issues/57)
- Added `variant_sequential_view` [\#46](https://github.com/rttrorg/rttr/issues/46)
- Added `string_view` class
- Added `library` class in order to load plugins at runtime [\#116](https://github.com/rttrorg/rttr/issues/116)
- Added `variant_cast` function in order to support move data from `variant` [\#108](https://github.com/rttrorg/rttr/issues/108) 
- Added possibility to convert raw nullptr pointers in variant class to base types [\#59](https://github.com/rttrorg/rttr/issues/59) 
- Return the template parameters of a type [\#58](https://github.com/rttrorg/rttr/issues/58)
- Added support for register base class properties [\#29](https://github.com/rttrorg/rttr/issues/29)
- Add implicit conversion from a wrapped value inside a variant [\#48](https://github.com/rttrorg/rttr/issues/48)
- Support for VS2017 [\#45](https://github.com/rttrorg/rttr/issues/#45)
- Added support for C++17 compilers [\#100, #105](https://github.com/rttrorg/rttr/issues/100)
- Custom registered type name should be used in class templates as well [\#40](https://github.com/rttrorg/rttr/issues/40)
- Added new comparators(>, <=, >=) for `variant` class [\#61](https://github.com/rttrorg/rttr/issues/61)
- Add support for new property policy: 'as_reference_wrapper' [\#20](https://github.com/rttrorg/rttr/issues/#20) 
- Add possibility to filter to class items, while retrieving, 
  e.g: retrieve private properties: `get_properties(filter_item::instance_item | filter_item::non_public_access)`
- Added JSON serialization example
- Added loading plugins example [\#116](https://github.com/rttrorg/rttr/issues/116)
- Added CI systems: Travis and Appveyor
- Added static code analysis tool Codacy (cppcheck underlying)
- Added code coverage tool: "CoverAlls"
- Added `variant::get_wrapped_value()` & `variant::extract_wrapped_value()` functions
- Donation button added: [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JQ65KGGCSUZMS)

[![Generic badge](https://img.shields.io/badge/►-Improvements-84E61B.svg?colorA=84E61B)](https://github.com/acki-m/rttr/blob/feature-adjust-readme/ChangeLog.md)

- Return ranges instead of vectors [\#6](https://github.com/rttrorg/rttr/issues/6)
- Remove branching when calling a wrapper method [\#19](https://github.com/rttrorg/rttr/issues/19)
- Add 'noexcept' everywhere where reasonable [\#17](https://github.com/rttrorg/rttr/issues/17)
- Add 'constexpr' everywhere where reasonable [\#16](https://github.com/rttrorg/rttr/issues/16)
- Added own make_unique impl, not supported in C++11
- Upgraded catch to version: 1.12.0 [\#120](https://github.com/rttrorg/rttr/issues/120)
- Increased test coverage: 92%
- Several docu improvements

[![Generic badge](https://img.shields.io/badge/►-Bugfixes-F70202.svg?colorA=F70202)](https://github.com/acki-m/rttr/blob/feature-adjust-readme/ChangeLog.md)

- class items inheritance of items, was not working when derived class had no items at all [\#89](https://github.com/rttrorg/rttr/issues/89)
- Raw arrays by value, were not recognized by `variant_sequential_view` [\#125](https://github.com/rttrorg/rttr/issues/125)
- Explicit unloading of libraries which contain registered RTTR types, lead to crash [\#113](https://github.com/rttrorg/rttr/issues/113)
- Fix install handling for Mac/Linux #101, [\#102](https://github.com/rttrorg/rttr/issues/102)
- Added support for variant properties [\#47](https://github.com/rttrorg/rttr/issues/47)
- Typo in method name: rttr::constructor::get_instanciated_type() [\#83](https://github.com/rttrorg/rttr/issues/83)
- Polymorphic assignment of raw pointer properties does not work  [\#56](https://github.com/rttrorg/rttr/issues/56)
- Comparison of raw pointer properties does not work [\#55](https://github.com/rttrorg/rttr/issues/55)
- fix wrong enum conversion in variant function `convert(bool* ok)` [\#53](https://github.com/rttrorg/rttr/issues/53)
- Fixed not correct handling of std::ref(int*) type in variant [\#37](https://github.com/rttrorg/rttr/issues/37)
- Custom installation directory via CMAKE_INSTALL_PREFIX [\#34](https://github.com/rttrorg/rttr/issues/34)
- Replaced default constructor binding from `as_object` to `as_std_shared_ptr` [\#14](https://github.com/rttrorg/rttr/issues/14)
- Fixed missing invoke of dtor when type is pointer type [\#14](https://github.com/rttrorg/rttr/issues/14)
- Do not search for boost, when not needed [\#93](https://github.com/rttrorg/rttr/issues/93)
- `type::is_array()` now returns only `true` for C-style arrays, 
before that it also `true` for specialization of array_mapper types [\#144](https://github.com/rttrorg/rttr/pull/144)

[![Generic badge](https://img.shields.io/badge/►-Removed-613F09.svg?colorA=613F09)](https://github.com/acki-m/rttr/blob/feature-adjust-readme/ChangeLog.md)

- `variant_array_view` class [\#144](https://github.com/rttrorg/rttr/pull/144)
- `property::is_array()` method [\#144](https://github.com/rttrorg/rttr/pull/144)

## [v0.9.5](https://github.com/rttrorg/rttr/releases/tag/v0.9.5) (2015-12-23)
[Full Changelog](https://github.com/rttrorg/rttr/compare/init_commit...v0.9.5)

Blog Entry: http://www.rttr.org/news/new-stable-release-0-9-5

- the macro RTTR_DECLARE_TYPE was removed, so it is not necessary anymore to declare 
  a type before using it (RTTR does still not require a compiler with rtti)
- completely adjusted the registration API to be more future proof
- added the possibility to register the access specifier of your class member
- added the possibility to register free functions for constructors
- the `variant` class has get comparison operators `==` and `<`
- the `variannt_array` class is replaced by `variant_array_view` class
- added the possibility to register default values to methods and constructors
- added the possibilities to register names of method parameters
- new policies for constructors were added
- it's now possible to invoke methods while the instance is in a wrapped type, like std::shared_ptr<T>
- a lot of refactoring internally, almost every class was internally refactored
- added several benchmarks to measure the performance impact from certain 
  API calls of the library
- adjusted documentation style to use cleaner and modern HTML5 style
- rewritten the tutorial
- switched from mercurial to git and uploaded the source code to github
- made own website: www.rttr.org
