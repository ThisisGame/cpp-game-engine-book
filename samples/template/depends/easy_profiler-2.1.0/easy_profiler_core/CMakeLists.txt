message(STATUS "")
message(STATUS "EASY_PROFILER.Core version = ${EASY_PRODUCT_VERSION_STRING}")
message(STATUS "")


set(EASY_MAX_SIZE_VALUE 2816)


#####################################################################
# Checking c++11 thread_local support
if (NOT WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8")
            set(NO_CXX11_THREAD_LOCAL_SUPPORT TRUE)
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.3")
            set(NO_CXX11_THREAD_LOCAL_SUPPORT TRUE)
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
            set(NO_CXX11_THREAD_LOCAL_SUPPORT TRUE)
        endif ()
    endif ()

    # TODO: Check thread_local keyword support for other compilers for Unix

    if (NO_CXX11_THREAD_LOCAL_SUPPORT)
        message(WARNING "  Your compiler does not support C++11 thread_local feature.")
        message(WARNING "  Without C++11 thread_local feature you may face to possible memory leak or application crash if using implicit thread registration and using EASY_THREAD instead of EASY_THREAD_SCOPE.")
    endif ()
endif ()
#####################################################################



###########################################################
# EasyProfiler options:
set(EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT "Enable new threads registration when collecting context switch events")
set(EASY_DEFAULT_PORT                  28077  CACHE STRING "Default listening port")
set(EASY_OPTION_LISTEN                 OFF    CACHE BOOL   "Enable automatic startListen on startup")
set(EASY_OPTION_PROFILE_SELF           OFF    CACHE BOOL   "Enable self profiling (measure time for internal storage expand)")
set(EASY_OPTION_PROFILE_SELF_BLOCKS_ON OFF    CACHE BOOL   "Storage expand default status (profiler::ON or profiler::OFF)")
set(EASY_OPTION_TRUNCATE_RUNTIME_NAMES OFF    CACHE BOOL   "Enable truncation of block dynamic names (set at run-time, not compile-time). Reduces performance. Turn ON only if you want to use dynamic block names of length >${EASY_MAX_SIZE_VALUE} symbols at runtime. It is better to use EASY_VALUE instead of such long names.")
set(EASY_OPTION_CHECK_MAX_VALUE_SIZE   OFF    CACHE BOOL   "Enable checking EASY_VALUE maximum data size. Slightly reduces performance. Turn ON only if you want to pass big EASY_ARRAY, EASY_STRING, EASY_TEXT of length >${EASY_MAX_SIZE_VALUE} bytes. It is better to split such arrays into the smaller ones.")
set(EASY_OPTION_LOG                    OFF    CACHE BOOL   "Print errors to stderr")
set(EASY_OPTION_PRETTY_PRINT           OFF    CACHE BOOL   "Use pretty-printed function names with signature and argument types")
set(EASY_OPTION_PREDEFINED_COLORS      ON     CACHE BOOL   "Use predefined set of colors (see profiler_colors.h). If you want to use your own colors palette you can turn this option OFF")
set(BUILD_SHARED_LIBS                  ON     CACHE BOOL   "Build easy_profiler as shared library.")
if (WIN32)
    set(EASY_OPTION_IMPLICIT_THREAD_REGISTRATION ON CACHE BOOL ${EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT})
    set(EASY_OPTION_EVENT_TRACING                ON CACHE BOOL "Enable event tracing by default")
    set(EASY_OPTION_LOW_PRIORITY_EVENT_TRACING   ON CACHE BOOL "Set low priority for event tracing thread")
else ()
    if (NO_CXX11_THREAD_LOCAL_SUPPORT)
        set(EASY_OPTION_IMPLICIT_THREAD_REGISTRATION OFF CACHE BOOL ${EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT})
        set(EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS OFF CACHE BOOL "Enable easy_profiler to remove empty unguarded threads. This fixes potential memory leak on Unix systems, but may lead to an application crash! This is used when C++11 thread_local is unavailable.")
    else ()
        set(EASY_OPTION_IMPLICIT_THREAD_REGISTRATION ON CACHE BOOL ${EASY_OPTION_IMPLICIT_THREAD_REGISTER_TEXT})
    endif ()
endif ()
set(BUILD_WITH_CHRONO_STEADY_CLOCK OFF CACHE BOOL "Use std::chrono::steady_clock as a timer" )
set(BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK OFF CACHE BOOL "Use std::chrono::high_resolution_clock as a timer")
# End EasyProfiler options.
###########################################################



#####################################################################
# Print EasyProfiler options status:
message(STATUS "-------- EASY_PROFILER OPTIONS: --------")
if (BUILD_WITH_CHRONO_STEADY_CLOCK)
    message(STATUS "  Use std::chrono::steady_clock as a timer")
elseif (BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK)
    message(STATUS "  Use std::chrono::high_resolution_clock as a timer")
else ()
    if (WIN32)
        message(STATUS "  Use QueryPerformanceCounter as a timer")
    else ()
        message(STATUS "  Use rtdsc as a timer")
    endif ()
endif ()

message(STATUS "  Default listening port = ${EASY_DEFAULT_PORT}")
message(STATUS "  Auto-start listening = ${EASY_OPTION_LISTEN}")
message(STATUS "  Profile self = ${EASY_OPTION_PROFILE_SELF}")
message(STATUS "  Profile self blocks initial status = ${EASY_OPTION_PROFILE_SELF_BLOCKS_ON}")

if (EASY_OPTION_TRUNCATE_RUNTIME_NAMES)
    message(STATUS "  Truncate block dynamic names = ${EASY_OPTION_TRUNCATE_RUNTIME_NAMES} (reduces performance)")
else()
    message(STATUS "  Truncate block dynamic names = ${EASY_OPTION_TRUNCATE_RUNTIME_NAMES} (may cause crash if using dynamic block names of length >${EASY_MAX_SIZE_VALUE} symbols)")
endif()

if (EASY_OPTION_CHECK_MAX_VALUE_SIZE)
    message(STATUS "  Check maximum EASY_VALUE data size = ${EASY_OPTION_CHECK_MAX_VALUE_SIZE} (slightly reduces performance)")
else()
    message(STATUS "  Check maximum EASY_VALUE data size = ${EASY_OPTION_CHECK_MAX_VALUE_SIZE} (may cause crash if using EASY_VALUE arrays of total data size >${EASY_MAX_SIZE_VALUE} bytes)")
endif()

message(STATUS "  Implicit thread registration = ${EASY_OPTION_IMPLICIT_THREAD_REGISTRATION}")
if (WIN32)
    message(STATUS "  Event tracing = ${EASY_OPTION_EVENT_TRACING}")
    message(STATUS "  Event tracing has low priority = ${EASY_OPTION_LOW_PRIORITY_EVENT_TRACING}")
elseif (NO_CXX11_THREAD_LOCAL_SUPPORT)
    if (EASY_OPTION_IMPLICIT_THREAD_REGISTRATION)
        message(STATUS "    WARNING! Implicit thread registration for Unix systems can lead to memory leak")
        message(STATUS "             because there is no possibility to check if thread is alive and remove dead threads.")
    endif ()
    message(STATUS "  Removing empty unguarded threads = ${EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS}")
    if (EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS)
        message(STATUS "    WARNING! Removing empty unguarded threads may lead to an application crash!")
        message(STATUS "             But fixes potential memory leak on Unix systems.")
    else ()
        message(STATUS "    WARNING! There is a possibility of memory leak without removing empty unguarded threads.")
    endif ()
endif ()
message(STATUS "  Log messages = ${EASY_OPTION_LOG}")
message(STATUS "  Function names pretty-print = ${EASY_OPTION_PRETTY_PRINT}")
message(STATUS "  Use EasyProfiler colors palette = ${EASY_OPTION_PREDEFINED_COLORS}")
message(STATUS "  Shared library: ${BUILD_SHARED_LIBS}")
message(STATUS "------ END EASY_PROFILER OPTIONS -------")
message(STATUS "")
# End printing EasyProfiler options status.
#####################################################################



#################################################
# Add source files:
set(CPP_FILES
    base_block_descriptor.cpp
    block.cpp
    block_descriptor.cpp
    easy_socket.cpp
    event_trace_win.cpp
    nonscoped_block.cpp
    profile_manager.cpp
    profiler.cpp
    reader.cpp
    serialized_block.cpp
    thread_storage.cpp
    writer.cpp
)

set(H_FILES
    block_descriptor.h
    chunk_allocator.h
    current_time.h
    current_thread.h
    event_trace_win.h
    nonscoped_block.h
    profile_manager.h
    thread_storage.h
    spin_lock.h
    stack_buffer.h
)

set(EASY_INCLUDE_DIR "include/easy")

set(INCLUDE_FILES
    ${EASY_INCLUDE_DIR}/arbitrary_value.h
    ${EASY_INCLUDE_DIR}/easy_net.h
    ${EASY_INCLUDE_DIR}/easy_socket.h
    ${EASY_INCLUDE_DIR}/profiler.h
    ${EASY_INCLUDE_DIR}/reader.h
    ${EASY_INCLUDE_DIR}/utility.h
    ${EASY_INCLUDE_DIR}/serialized_block.h
    ${EASY_INCLUDE_DIR}/writer.h
    ${EASY_INCLUDE_DIR}/details/arbitrary_value_aux.h
    ${EASY_INCLUDE_DIR}/details/arbitrary_value_public_types.h
    ${EASY_INCLUDE_DIR}/details/easy_compiler_support.h
    ${EASY_INCLUDE_DIR}/details/profiler_aux.h
    ${EASY_INCLUDE_DIR}/details/profiler_colors.h
    ${EASY_INCLUDE_DIR}/details/profiler_in_use.h
    ${EASY_INCLUDE_DIR}/details/profiler_public_types.h
)

source_group(include FILES ${INCLUDE_FILES})

set(SOURCES
    ${CPP_FILES}
    ${H_FILES}
    ${INCLUDE_FILES}
)

add_library(easy_profiler ${SOURCES} resources.rc)
# End adding source files.
#################################################



target_include_directories(easy_profiler PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_compile_definitions(easy_profiler PRIVATE
    -D_BUILD_PROFILER=1
    #-DEASY_PROFILER_API_DISABLED # uncomment this to disable profiler api only (you will have to rebuild only easy_profiler)
)
if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(easy_profiler PUBLIC -DEASY_PROFILER_STATIC=1)
endif ()
target_compile_definitions(easy_profiler PUBLIC
    -DEASY_PROFILER_VERSION_MAJOR=${EASY_PROGRAM_VERSION_MAJOR}
    -DEASY_PROFILER_VERSION_MINOR=${EASY_PROGRAM_VERSION_MINOR}
    -DEASY_PROFILER_VERSION_PATCH=${EASY_PROGRAM_VERSION_PATCH}
    -DEASY_DEFAULT_PORT=${EASY_DEFAULT_PORT}
    -DBUILD_WITH_EASY_PROFILER=1
)



#####################################################################
# Add EasyProfiler options definitions:
easy_define_target_option(easy_profiler BUILD_WITH_CHRONO_STEADY_CLOCK EASY_CHRONO_STEADY_CLOCK)
easy_define_target_option(easy_profiler BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK EASY_CHRONO_HIGHRES_CLOCK)
easy_define_target_option(easy_profiler EASY_OPTION_LISTEN EASY_OPTION_START_LISTEN_ON_STARTUP)
easy_define_target_option(easy_profiler EASY_OPTION_PROFILE_SELF EASY_OPTION_MEASURE_STORAGE_EXPAND)
easy_define_target_option(easy_profiler EASY_OPTION_PROFILE_SELF_BLOCKS_ON EASY_OPTION_STORAGE_EXPAND_BLOCKS_ON)
easy_define_target_option(easy_profiler EASY_OPTION_TRUNCATE_RUNTIME_NAMES EASY_OPTION_TRUNCATE_LONG_RUNTIME_NAMES)
easy_define_target_option(easy_profiler EASY_OPTION_CHECK_MAX_VALUE_SIZE EASY_OPTION_CHECK_MAX_VALUE_DATA_SIZE)
easy_define_target_option(easy_profiler EASY_OPTION_IMPLICIT_THREAD_REGISTRATION EASY_OPTION_IMPLICIT_THREAD_REGISTRATION)
if (WIN32)
    easy_define_target_option(easy_profiler EASY_OPTION_EVENT_TRACING EASY_OPTION_EVENT_TRACING_ENABLED)
    easy_define_target_option(easy_profiler EASY_OPTION_LOW_PRIORITY_EVENT_TRACING EASY_OPTION_LOW_PRIORITY_EVENT_TRACING)
else ()
    easy_define_target_option(easy_profiler EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS EASY_OPTION_REMOVE_EMPTY_UNGUARDED_THREADS)
endif ()
easy_define_target_option(easy_profiler EASY_OPTION_LOG EASY_OPTION_LOG_ENABLED)
easy_define_target_option(easy_profiler EASY_OPTION_PRETTY_PRINT EASY_OPTION_PRETTY_PRINT_FUNCTIONS)
easy_define_target_option(easy_profiler EASY_OPTION_PREDEFINED_COLORS EASY_OPTION_BUILTIN_COLORS)
# End adding EasyProfiler options definitions.
#####################################################################



###############################################################################
# Add platform specific compile options:
if (UNIX)
    target_compile_options(easy_profiler PRIVATE -Wall -Wno-long-long -Wno-reorder -Wno-braced-scalar-init -pedantic)
    if (NOT ANDROID AND NOT QNX)
        target_link_libraries(easy_profiler pthread)
    endif()
    if (QNX)
        target_link_libraries(easy_profiler socket)
    endif()
elseif (WIN32)
    target_compile_definitions(easy_profiler PRIVATE -D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
    target_link_libraries(easy_profiler ws2_32 psapi)
endif ()

if (MINGW)
    target_compile_definitions(easy_profiler PRIVATE -DSTRSAFE_NO_DEPRECATE)
endif ()

if (MSVC)
    target_compile_options(easy_profiler PRIVATE /WX)
endif ()

# End adding platform specific compile options.
###############################################################################



#########################################################################################
# Installation:
set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")

include(CMakePackageConfigHelpers)
include(InstallRequiredSystemLibraries)

write_basic_package_version_file(
    "${version_config}"
    VERSION
    ${EASY_PRODUCT_VERSION_STRING}
    COMPATIBILITY
    SameMajorVersion
)

configure_package_config_file(
    "cmake/config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

install(
    DIRECTORY
    ${EASY_INCLUDE_DIR}
    DESTINATION
    include/
)

install(
    FILES
    LICENSE.MIT
    LICENSE.APACHE
    DESTINATION
    .
)

install(
    TARGETS
    easy_profiler
    EXPORT
    ${targets_export_name}
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    BUNDLE DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

install(
    EXPORT "${targets_export_name}"
    DESTINATION "${config_install_dir}"
)

target_compile_definitions(easy_profiler PUBLIC )
