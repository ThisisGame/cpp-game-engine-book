Building & Installation   {#building_install_page}
========================

This short guide will help you to build and install RTTR.

Building
--------
RTTR can be built with any modern C++11 compiler (MSVC, MinGW, g++, clang). 
The build files are generated using <a target="_blank" href=http://www.cmake.org/>CMake</a> (2.8.12 or later).
Because the library itself does not depend on any other 3rd party libraries, you can perform a build with just the usage of CMake.

e.g. for Visual Studio:

    cmake -G "Visual Studio 12 2013" <Path to RTTR src code>
    
or gcc:

    cmake -DCMAKE_BUILD_TYPE=Release <Path to RTTR src code>

However, in order to build a complete installation, <a target="_blank" href="http://www.boost.org/">Boost</a> and <a target="_blank" href="http://www.doxygen.org/">Doxygen</a>
are as external dependency necessary.

- `Documentation` : <a target="_blank" href="http://www.doxygen.org/">Doxygen</a> is used for generating the documentation (the target is called: `doc`); the config file was generated from v. 2.8.10<br>
On windows and linux you may have to put the path to the doxygen binary in the system path environment variable (`PATH`),
or set it to the environment variable `DOXYGEN_EXECUTABLE` so that CMake can find it.
- `Benchmarks`: RTTR uses <a target="_blank" href="http://flamingdangerzone.com/nonius/">Nonius</a> (which is bundled with RTTR) for benchmarking and this library depends 
on <a target="_blank" href="http://flamingdangerzone.com/nonius/#main_content">Boost</a>.<br>
You may have to set the `BOOST_ROOT` environment variable to the Boost installation directory.


###Config CMake Build###
The build of RTTR can be configured at CMake configuration time with following variables:

- `BUILD_STATIC`: Build RTTR as static library; Default: `FALSE`
- `BUILD_WITH_STATIC_RUNTIME_LIBS`: Link against the static runtime libraries; Default: `FALSE`
- `BUILD_WITH_RTTI`: Enable the C++ language feature for runtime type information (RTTI); Default: `TRUE`
- `BUILD_BENCHMARKS`: Enable this to build the benchmarks; Default: `TRUE`
- `USE_PCH`: Use precompiled header files for compilation; Default: `TRUE`*
- `CUSTOM_DOXYGEN_STYLE`: Enable this option to use a custom doxygen style for HTML documentation; Otherwise the default will be used; Default: `TRUE`

\*Precompiled headers are only supported with MSVC based build

After running CMake, you can start the compilation, depending on the generator: MSVC: `nmake` or `jom`, Unix based: `make`, MinGW: `mingw32-make`.

Installation
-------------
In order to install RTTR, you have to build the target `install`. This will create a directory called `install` in the specified build directory of RTTR.
Inside this folder, you will find following folder structure:

- `bin`: The binaries (rttr_core library (win only); benchmark executables)
- `cmake`: cmake files for finding the library in client code
- `doc`: the html documentation
- `include`: all necessary header files
- `lib`: the library files

It is also possible to create a package of the installer. Therefore <a target="_blank" href="http://www.cmake.org/Wiki/CMake:Packaging_With_CPack">CPack</a> is used.
Execute following command in the build directory of RTTR:

    cpack
    
and in the root directory of the build dir will be an install package placed (e.g. `rttr-1.0.0-win64-vs2013.7z`)

Using Installation of RTTR
--------------------------
The last step is to use RTTR in your own application. During the install process also some CMake config files were created to find RTTR.
For this reason, you have to set up an environment variable called: `RTTR_DIR`, which should contains the path to the installation directory.

e.g for windows:

    set RTTR_DIR=c:/rttr-1.0.0-win64-vs2013

Then add following to your CMake script file:

    find_package(RTTR CONFIG REQUIRED Core)

and final step is to link against the library:

    target_link_libraries(MyApp RTTR::Core)
    
Following link targets are available, depending on the build config:

    target_link_libraries(MyApp RTTR::Core)         # rttr as dynamic library
    target_link_libraries(MyApp RTTR::Core_STL)     # rttr as dynamic library but with static runtime library
    target_link_libraries(MyApp RTTR::Core_Lib)     # rttr as static library
    target_link_libraries(MyApp RTTR::Core_Lib_STL) # rttr as static library and static runtime library

That's it, you can use RTTR in your own library now. Congrats!
