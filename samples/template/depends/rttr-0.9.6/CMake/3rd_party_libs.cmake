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
# Search and install 3rd party libraries
#
####################################################################################

MESSAGE(STATUS ${LIBRARY_OUTPUT_DIRECTORY})
MESSAGE(STATUS "Finding 3rd party libs...")
MESSAGE(STATUS "===========================")

if (BUILD_BENCHMARKS)
    if (MSVC)
        # there is a the moment a problem with finding multiple versions of boost,
        # i.e. the static AND the static runtime version; that is not possible atm.
        # Because of that, the benchmarks cannot be build with the static runtime lib option enabled
        set(Boost_USE_STATIC_LIBS       ON)
        set(Boost_USE_STATIC_RUNTIME    OFF)
        set(BOOST_ALL_DYN_LINK          OFF)

        find_package(Boost COMPONENTS chrono system)
    else()
        find_package(Boost)
    endif()
    
    find_package(Threads REQUIRED)
endif()

set(RAPID_JSON_DIR ${RTTR_3RD_PARTY_DIR}/rapidjson-1.1.0)
set(NONIUS_DIR ${RTTR_3RD_PARTY_DIR}/nonius-1.1.2)

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${RTTR_3RD_PARTY_DIR}/catch-1.12.0)
add_library(Catch INTERFACE)
add_library(Catch2::Catch ALIAS Catch)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

MESSAGE(STATUS "Finished finding 3rd party libs!")
