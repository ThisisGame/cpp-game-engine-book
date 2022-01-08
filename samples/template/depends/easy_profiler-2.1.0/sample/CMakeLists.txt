set(CPP_FILES
    main.cpp
)

set(SOURCES
    ${CPP_FILES}
)

link_directories(${CMAKE_SOURCE_DIR}/../bin)

add_executable(profiler_sample ${SOURCES})
target_link_libraries(profiler_sample easy_profiler)

add_executable(profiler_sample_disabled_profiler ${SOURCES})
target_link_libraries(profiler_sample_disabled_profiler easy_profiler)
target_compile_definitions(profiler_sample_disabled_profiler PRIVATE DISABLE_EASY_PROFILER)
