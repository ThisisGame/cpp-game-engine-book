set(CPP_FILES
    converter.cpp
    reader.cpp)

set(HEADER_FILES
    converter.h
    reader.h)

include_directories(../easy_profiler_core/)
include_directories(./include)

add_executable(profiler_converter ${HEADER_FILES} ${CPP_FILES} main.cpp)
target_link_libraries(profiler_converter easy_profiler)

install(
    TARGETS
    profiler_converter
    RUNTIME
    DESTINATION
    bin
)

set_property(TARGET profiler_converter PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
