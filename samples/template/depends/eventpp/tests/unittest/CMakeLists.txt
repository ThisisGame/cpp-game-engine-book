set(TARGET_TEST unittest)

set(SRC_TEST
	testmain.cpp
	test_callbacklist_basic.cpp
	test_callbacklist_ctors.cpp
	test_callbacklist_multithread.cpp
	test_dispatcher_basic.cpp
	test_dispatcher_ctors.cpp
	test_dispatcher_multithread.cpp
	test_queue_basic.cpp
	test_queue_ctors.cpp
	test_queue_multithread.cpp
	test_queue_ordered_list.cpp
	test_hetercallbacklist_basic.cpp
	test_hetercallbacklist_ctors.cpp
	test_heterdispatcher_basic.cpp
	test_heterdispatcher_ctors.cpp
	test_heterdispatcher_multithread.cpp
	test_heterqueue_basic.cpp
	test_eventutil.cpp
	test_eventmaker.cpp
	test_conditionalremover.cpp
	test_counterremover.cpp
	test_scopedremover.cpp
	test_no_extra_copy_move.cpp
	test_argumentadapter.cpp
	test_conditionalfunctor.cpp
	test_anyid.cpp
)

add_executable(
	${TARGET_TEST}
	${SRC_TEST}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET_TEST} Threads::Threads)

set_target_properties(${TARGET_TEST} PROPERTIES CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX)
if(coverage)
	set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
endif()
endif()

add_test(NAME ${TARGET_TEST} COMMAND ${TARGET_TEST})
