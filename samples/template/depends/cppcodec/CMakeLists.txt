cmake_minimum_required(VERSION 2.8.5)
project(cppcodec CXX)
set(PROJECT_VERSION 0.1)

include(GNUInstallDirs)
include(CTest)

# These flags are for binaries built by this particular CMake project (test_cppcodec, base64enc, etc.).
# In your own project that uses cppcodec, you might want to specify a different standard or error level.

# Request C++11, or let the user specify the standard on via -D command line option.
if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  # MSVC will respect CMAKE_CXX_STANDARD for CMake >= 3.10 and MSVC >= 19.0.24215
  # (VS 2017 15.3). Older versions will use the compiler default, which should be
  # fine for anything except ancient MSVC versions.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

  # CMake versions before 3.1 do not understand CMAKE_CXX_STANDARD.
  # Remove this block once CMake >=3.1 has fixated in the ecosystem.
  if(${CMAKE_VERSION} VERSION_LESS 3.1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD}")
  endif()
endif()

set(PUBLIC_HEADERS
    # base32
    cppcodec/base32_crockford.hpp
    cppcodec/base32_default_crockford.hpp
    cppcodec/base32_default_hex.hpp
    cppcodec/base32_default_rfc4648.hpp
    cppcodec/base32_hex.hpp
    cppcodec/base32_rfc4648.hpp
    # base64
    cppcodec/base64_default_rfc4648.hpp
    cppcodec/base64_default_url.hpp
    cppcodec/base64_default_url_unpadded.hpp
    cppcodec/base64_rfc4648.hpp
    cppcodec/base64_url.hpp
    cppcodec/base64_url_unpadded.hpp
    # hex
    cppcodec/hex_default_lower.hpp
    cppcodec/hex_default_upper.hpp
    cppcodec/hex_lower.hpp
    cppcodec/hex_upper.hpp
    # other stuff
    cppcodec/parse_error.hpp
    cppcodec/data/access.hpp
    cppcodec/data/raw_result_buffer.hpp
    cppcodec/detail/base32.hpp
    cppcodec/detail/base64.hpp
    cppcodec/detail/codec.hpp
    cppcodec/detail/config.hpp
    cppcodec/detail/hex.hpp
    cppcodec/detail/stream_codec.hpp)

add_library(cppcodec OBJECT ${PUBLIC_HEADERS}) # unnecessary for building, but makes headers show up in IDEs
set_target_properties(cppcodec PROPERTIES LINKER_LANGUAGE CXX)
add_subdirectory(tool)
add_subdirectory(example)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

foreach(h ${PUBLIC_HEADERS})
    get_filename_component(FINAL_PATH ${h} PATH) # use DIRECTORY instead of PATH once requiring CMake 3.0
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FINAL_PATH} COMPONENT "headers")
endforeach()

if (NOT WIN32)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cppcodec.pc.in ${CMAKE_CURRENT_BINARY_DIR}/cppcodec-1.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cppcodec-1.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
endif()
