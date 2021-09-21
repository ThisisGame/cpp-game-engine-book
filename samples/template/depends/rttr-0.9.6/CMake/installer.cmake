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

# first step set up all necessary variables
if(WIN32 AND NOT UNIX)
    if (NOT CMAKE_VERSION VERSION_LESS 3.1.0)
        set(CPACK_GENERATOR 7Z)
    else()
        set(CPACK_GENERATOR ZIP)
    endif()
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR TGZ;TBZ2)
elseif(APPLE)
    set(CPACK_GENERATOR TGZ;TBZ2;PackageMaker)
endif()

set(CPACK_PACKAGE_NAME "${RTTR_PRODUCT_NAME}")
set(CPACK_PACKAGE_VENDOR "AMS")
set(CPACK_PACKAGE_VERSION_MAJOR "${RTTR_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${RTTR_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${RTTR_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${RTTR_VERSION}")
set(CPACK_RESOURCE_FILE_README "${README_FILE}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${README_FILE}")
set(CPACK_RESOURCE_FILE_LICENSE "${LICENSE_FILE}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}/RTTR")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RTTR is a C++ runtime reflection library")

# detect system
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(RTTR_OS_NAME "linux${RTTR_NATIVE_ARCH}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(RTTR_OS_NAME "win${RTTR_NATIVE_ARCH}")
else()
    message(WARNING "This system is not supported for packing")
    return()
endif()

getCompilerName(RTTR_COMPILER_NAME)

# we use the same naming scheme like Qt
set(CPACK_PACKAGE_FILE_NAME "rttr-${RTTR_VERSION_STR}-${RTTR_OS_NAME}-${RTTR_COMPILER_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "rttr-${RTTR_VERSION_STR}-src")

# now as last step we can include CPack
include(CPack)