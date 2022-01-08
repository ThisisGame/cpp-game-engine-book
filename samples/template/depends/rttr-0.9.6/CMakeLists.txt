####################################################################################
#                                                                                  #
#  Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           #
#                                                                                  #
#  This file is part of RTTR (Run Time Type Reflection)                            #
#  License: MIT License                                                            #
#                                                                                  #
#  Permission is hereby granted, free of charge, to any person obtaining           #
#  a copy of this software and associated documentation files (the "Software"),    #
#  to deal in the Software without restriction, including without limitation       #
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,        #
#  and/or sell copies of the Software, and to permit persons to whom the           #
#  Software is furnished to do so, subject to the following conditions:            #
#                                                                                  #
#  The above copyright notice and this permission notice shall be included in      #
#  all copies or substantial portions of the Software.                             #
#                                                                                  #
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      #
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        #
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          #
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   #
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   #
#  SOFTWARE.                                                                       #
#                                                                                  #
####################################################################################

####################################################################################
# Welcome to the CMake build system for RTTR( Run Time Type Reflection).           #
# This is the main file where the general build environment is set-up and the      #
# the build configuration options are initialized.                                 #
####################################################################################

cmake_minimum_required (VERSION 3.0)

project ("rttr" LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# our little cmake helper functions
include(utility)

# set up option variable for cmake
option(BUILD_RTTR_DYNAMIC "Build the dynamic/shared version of RTTR library" ON)
option(BUILD_UNIT_TESTS "Build the unit tests of RTTR" ON)
option(BUILD_STATIC "Build RTTR as static library" OFF)
option(BUILD_WITH_STATIC_RUNTIME_LIBS "Link against the static runtime libraries" OFF)
option(BUILD_WITH_RTTI "Enable build with C++ runtime type information for compilation" ON)
option(BUILD_BENCHMARKS "Enable this to build the benchmarks" OFF)
option(BUILD_EXAMPLES "Enable this to build the examples" On)
option(BUILD_DOCUMENTATION "Enable this to build the documentation" ON)
option(BUILD_INSTALLER "Enable this to build the installer" ON)
option(BUILD_PACKAGE "Enable this to build the installer" ON)
option(USE_PCH "Use precompiled header files for compilation" ON)
option(CUSTOM_DOXYGEN_STYLE "Enable this option to use a custom doxygen style for HTML documentation; Otherwise the default will be used" ON)
option(BUILD_WEBSITE_DOCU "Enable this option to create the special docu for the website" OFF)

# one precompiled headers cannot be used for multiple ninja targets
# thats why we have to disable this option, when BUILD_STATIC or
# BUILD_WITH_STATIC_RUNTIME_LIBS is ON (every target will create the same PCH.pch file)
# to get it working, we need the feature to enable different source properties
# for different targets
if (USE_PCH)
  if (BUILD_STATIC OR BUILD_WITH_STATIC_RUNTIME_LIBS)
    set(USE_PCH FALSE)
  endif()
endif()

include(config)
include(3rd_party_libs)

if (BUILD_PACKAGE)
    include(installer)
endif()

# here we add our source code
add_subdirectory(src)
# and the documentation
if (BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()
