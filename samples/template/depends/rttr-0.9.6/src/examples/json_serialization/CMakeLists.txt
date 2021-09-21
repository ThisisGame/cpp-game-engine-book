####################################################################################
#                                                                                  #
#  Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           #
#                                                                                  #
#  This file is part of the examples of RTTR (Run Time Type Reflection)            #
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

project(json_example LANGUAGES CXX)

message(STATUS "Scanning "  ${PROJECT_NAME} " module.")
message(STATUS "===========================")

generateLibraryVersionVariables(${RTTR_VERSION_MAJOR} ${RTTR_VERSION_MINOR} ${RTTR_VERSION_PATCH}
                                "RTTR Examples: json serialization" "Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>" "MIT License")

loadFolder("files" HPP_FILES SRC_FILES)

if (USE_PCH)
  activate_precompiled_headers("pch.h" SRC_FILES)
endif()

add_executable(json_example ${SRC_FILES} ${HPP_FILES})
target_link_libraries(json_example RTTR::Core)
target_include_directories(json_example PUBLIC ${RAPID_JSON_DIR})
set_target_properties(json_example 
                       PROPERTIES DEBUG_POSTFIX ${RTTR_DEBUG_POSTFIX}
                                  FOLDER "Examples"
                                  INSTALL_RPATH "${RTTR_EXECUTABLE_INSTALL_RPATH}"
                                  CXX_STANDARD ${MAX_CXX_STANDARD})

set_compiler_warnings(json_example)

message(STATUS "Scanning " ${PROJECT_NAME} " module finished!")
message(STATUS "")
