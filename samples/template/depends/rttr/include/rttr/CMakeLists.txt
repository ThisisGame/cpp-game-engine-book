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

project(rttr LANGUAGES CXX)

message(STATUS "Scanning "  ${PROJECT_NAME} " module.")
message(STATUS "===========================")

generateLibraryVersionVariables(${RTTR_VERSION_MAJOR} ${RTTR_VERSION_MINOR} ${RTTR_VERSION_PATCH}
                                "RTTR" "Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>" "MIT License")

loadFolder("rttr" HPP_FILES SRC_FILES BUILD_INSTALLER)

if (USE_PCH)
    activate_precompiled_headers("detail/base/pch.h" SRC_FILES)
endif()

if (${BUILD_RTTR_DYNAMIC})
    add_library(rttr_core SHARED ${UnityBuild} ${SRC_FILES} ${HPP_FILES})
    add_library(RTTR::Core ALIAS rttr_core)
    
    target_compile_definitions(rttr_core PRIVATE RTTR_DLL_EXPORTS)
    target_compile_definitions(rttr_core PUBLIC RTTR_DLL)

    

    set_target_properties(rttr_core PROPERTIES
                          VERSION ${RTTR_VERSION} SOVERSION ${RTTR_VERSION}
                          EXPORT_NAME Core
                          DEBUG_POSTFIX ${RTTR_DEBUG_POSTFIX}
                          CXX_STANDARD ${MAX_CXX_STANDARD}
                          CXX_VISIBILITY_PRESET hidden
                          VISIBILITY_INLINES_HIDDEN 1)
    
    target_link_libraries(rttr_core PRIVATE ${CMAKE_DL_LIBS})
                          
                          
    if(${CMAKE_VERSION} VERSION_GREATER "3.8.0") 
        target_compile_features(rttr_core PUBLIC cxx_std_11) # at least c++11 is needed to compile RTTR
    endif()
                         
    target_include_directories(rttr_core PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../>
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
                               $<INSTALL_INTERFACE:include>)

    set_compiler_warnings(rttr_core)
    
    if (BUILD_INSTALLER)
        install(TARGETS rttr_core EXPORT rttr_targets
                RUNTIME       DESTINATION ${RTTR_RUNTIME_INSTALL_DIR}
                LIBRARY       DESTINATION ${RTTR_LIBRARY_INSTALL_DIR}
                ARCHIVE       DESTINATION ${RTTR_ARCHIVE_INSTALL_DIR}
                FRAMEWORK     DESTINATION ${RTTR_FRAMEWORK_INSTALL_DIR})

        # install also pdb file
        install_pdb_files(rttr_core)
    endif()

endif()

if (BUILD_STATIC)
    add_library(rttr_core_lib STATIC ${UnityBuild} ${SRC_FILES} ${HPP_FILES})
    add_library(RTTR::Core_Lib ALIAS rttr_core_lib)

    

    set_target_properties(rttr_core_lib PROPERTIES
                          VERSION ${RTTR_VERSION} SOVERSION ${RTTR_VERSION}
                          EXPORT_NAME Core_Lib
                          CXX_STANDARD ${MAX_CXX_STANDARD}
                          DEBUG_POSTFIX ${RTTR_DEBUG_POSTFIX})
                          
    target_link_libraries(rttr_core_lib PRIVATE ${CMAKE_DL_LIBS})
    
    if (MSVC)
        set_target_properties(rttr_core_lib PROPERTIES OUTPUT_NAME librttr_core)
    elseif (CMAKE_COMPILER_IS_GNUCXX)
        set_target_properties(rttr_core_lib PROPERTIES OUTPUT_NAME rttr_core)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_target_properties(rttr_core_lib PROPERTIES OUTPUT_NAME rttr_core)
    else()
        message(SEND_ERROR "Do not know how to name the static library.")
    endif()
    
    target_include_directories(rttr_core_lib PUBLIC
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../>
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
                                $<INSTALL_INTERFACE:include>)
    
    if(${CMAKE_VERSION} VERSION_GREATER "3.8.0") 
        target_compile_features(rttr_core_lib PUBLIC cxx_std_11) # at least c++11 is needed to compile RTTR
    endif()
    
    set_compiler_warnings(rttr_core_lib)

    if (BUILD_INSTALLER)
        install(TARGETS rttr_core_lib EXPORT rttr_targets
                RUNTIME       DESTINATION ${RTTR_RUNTIME_INSTALL_DIR}
                LIBRARY       DESTINATION ${RTTR_LIBRARY_INSTALL_DIR}
                ARCHIVE       DESTINATION ${RTTR_ARCHIVE_INSTALL_DIR}
                FRAMEWORK     DESTINATION ${RTTR_FRAMEWORK_INSTALL_DIR})
    endif()
endif()

if (BUILD_WITH_STATIC_RUNTIME_LIBS)
    add_library(rttr_core_s SHARED ${UnityBuild} ${SRC_FILES} ${HPP_FILES})
    add_library(RTTR::Core_STL ALIAS rttr_core_s)
    
    target_compile_definitions(rttr_core_s PRIVATE RTTR_DLL_EXPORTS)
    target_compile_definitions(rttr_core_s PUBLIC RTTR_DLL)

    target_include_directories(rttr_core_s PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../>
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
                               $<INSTALL_INTERFACE:include>)

    target_link_libraries(rttr_core_s PRIVATE ${CMAKE_DL_LIBS})
                               
    set_target_properties(rttr_core_s PROPERTIES
                          VERSION ${RTTR_VERSION} SOVERSION ${RTTR_VERSION}
                          EXPORT_NAME Core_STL
                          CXX_STANDARD ${MAX_CXX_STANDARD}
                          DEBUG_POSTFIX ${RTTR_DEBUG_POSTFIX}
                          CXX_VISIBILITY_PRESET hidden
                          VISIBILITY_INLINES_HIDDEN 1)

    if(${CMAKE_VERSION} VERSION_GREATER "3.8.0") 
        target_compile_features(rttr_core_s PUBLIC cxx_std_11) # at least c++11 is needed to compile RTTR
    endif()
    
    set_compiler_warnings(rttr_core_s)

    if (MSVC)
        target_compile_options(rttr_core_s PUBLIC "/MT$<$<CONFIG:Debug>:d>")
    elseif(CMAKE_COMPILER_IS_GNUCXX)
        set_target_properties(rttr_core_s PROPERTIES LINK_FLAGS ${GNU_STATIC_LINKER_FLAGS})
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_target_properties(rttr_core_s PROPERTIES LINK_FLAGS ${CLANG_STATIC_LINKER_FLAGS})
    else()
        message(SEND_ERROR "Do not know how to statically link against the standard library with this compiler.")
    endif()

    if (BUILD_INSTALLER)
        install_pdb_files(rttr_core_s)
        install(TARGETS rttr_core_s EXPORT rttr_targets
                RUNTIME       DESTINATION ${RTTR_RUNTIME_INSTALL_DIR}
                LIBRARY       DESTINATION ${RTTR_LIBRARY_INSTALL_DIR}
                ARCHIVE       DESTINATION ${RTTR_ARCHIVE_INSTALL_DIR}
                FRAMEWORK     DESTINATION ${RTTR_FRAMEWORK_INSTALL_DIR})
    endif()
    
    if (BUILD_STATIC)
        add_library(rttr_core_lib_s STATIC ${UnityBuild} ${SRC_FILES} ${HPP_FILES})
        add_library(RTTR::Core_Lib_STL ALIAS rttr_core_lib_s)
        
        

        target_include_directories(rttr_core_lib_s PUBLIC
                                   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../>
                                   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
                                   $<INSTALL_INTERFACE:include>)

        set_target_properties(rttr_core_lib_s PROPERTIES
                              VERSION ${RTTR_VERSION} SOVERSION ${RTTR_VERSION}
                              EXPORT_NAME Core_Lib_STL
                              CXX_STANDARD ${MAX_CXX_STANDARD}
                              DEBUG_POSTFIX ${RTTR_DEBUG_POSTFIX})
                              
        target_link_libraries(rttr_core_lib_s PRIVATE ${CMAKE_DL_LIBS})
            
        if(${CMAKE_VERSION} VERSION_GREATER "3.8.0") 
            target_compile_features(rttr_core_lib_s PUBLIC cxx_std_11) # at least c++11 is needed to compile RTTR
        endif()
    
        set_compiler_warnings(rttr_core_lib_s)

        if (MSVC)
            target_compile_options(rttr_core_lib_s PUBLIC "/MT$<$<CONFIG:Debug>:d>")
            set_target_properties(rttr_core_lib PROPERTIES OUTPUT_NAME librttr_core_s)
        elseif(CMAKE_COMPILER_IS_GNUCXX)
            set_target_properties(rttr_core_lib_s PROPERTIES LINK_FLAGS ${GNU_STATIC_LINKER_FLAGS} OUTPUT_NAME rttr_core_s)
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set_target_properties(rttr_core_lib_s PROPERTIES LINK_FLAGS ${CLANG_STATIC_LINKER_FLAGS} OUTPUT_NAME rttr_core_s)
        else()
            message(SEND_ERROR "Do not know how to statically link against the standard library with this compiler.")
        endif()
        
        if (BUILD_INSTALLER)
            install(TARGETS rttr_core_lib_s EXPORT rttr_targets
                    RUNTIME       DESTINATION ${RTTR_RUNTIME_INSTALL_DIR}
                    LIBRARY       DESTINATION ${RTTR_LIBRARY_INSTALL_DIR}
                    ARCHIVE       DESTINATION ${RTTR_ARCHIVE_INSTALL_DIR}
                    FRAMEWORK     DESTINATION ${RTTR_FRAMEWORK_INSTALL_DIR})
        endif()

    endif()
endif()


if(BUILD_INSTALLER)
	install(EXPORT rttr_targets
		DESTINATION ${RTTR_CMAKE_CONFIG_INSTALL_DIR}
		NAMESPACE RTTR::
		FILE rttr-config.cmake)
endif()


message(STATUS "Scanning " ${PROJECT_NAME} " module finished!")
message(STATUS "")
