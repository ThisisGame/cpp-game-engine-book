set(TARGET_BENCHMARK benchmark)

set(SRC_BENCHMARK
	testmain.cpp
	b1_callbacklist_invoking_vs_cpp.cpp
	b2_map_vs_unordered_map.cpp
	b3_b5_eventqueue.cpp
	b6_callbacklist_add_remove_callbacks.cpp
	b7_callbacklist_vs_function_list.cpp
)

add_executable(
	${TARGET_BENCHMARK}
	${SRC_BENCHMARK}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET_BENCHMARK} Threads::Threads)

