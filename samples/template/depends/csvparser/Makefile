# Makefile used for building/testing on Travis CI

# Force Travis to use updated compilers
ifeq ($(TRAVIS_COMPILER), gcc)
	CXX = g++-8
else ifeq ($(TRAVIS_COMPILER), clang)
	CXX = clang++
endif

ifeq ($(STD), )
	STD = c++11
endif

BUILD_DIR = build
SOURCE_DIR = include
SINGLE_INCLUDE_DIR = single_include
TEST_DIR = tests
CFLAGS = -pthread -std=$(STD)

TEST_OFLAGS =
ifeq ($(CXX), g++-8)
	TEST_OFLAGS = -Og
endif

TEST_FLAGS = -Itests/ $(CFLAGS) $(TEST_OFLAGS) -g --coverage -Wno-unknown-pragmas -Wall

# Main Library
SOURCES = $(wildcard include/internal/*.cpp)
OBJECTS = $(subst .cpp,.o,$(subst src/,$(BUILD_DIR)/,$(SOURCES)))

TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_SOURCES_NO_EXT = $(subst tests/,,$(subst .cpp,,$(TEST_SOURCES)))

all: csv_parser test_all clean distclean

################
# Main Library #
################
csv:
	$(CXX) -c -O3 $(CFLAGS) $(SOURCES)
	mkdir -p $(BUILD_DIR)
	mv *.o $(BUILD_DIR)
	
libcsv.a:
	make csv
	ar rvs libcsv.a $(wildcard build/*.o)
	
docs:
	doxygen Doxyfile
	
############
# Programs #
############
csv_stats:
	$(CXX) -o csv_stats -O3 $(CFLAGS) programs/csv_stats.cpp -I$(SINGLE_INCLUDE_DIR)
	
#########
# Tests #
#########	
csv_test:
	$(CXX) -o csv_test $(SOURCES) $(TEST_SOURCES) -I${SOURCE_DIR} $(TEST_FLAGS)
	
run_csv_test: csv_test
	mkdir -p tests/temp
	./csv_test
	
	# Test Clean-Up
	rm -rf $(TEST_DIR)/temp
	
# Run code coverage analysis
code_cov: csv_test
	mkdir -p test_results
	mv *.gcno *.gcda $(PWD)/test_results
	gcov-8 $(SOURCES) -o test_results --relative-only
	mv *.gcov test_results
	
# Generate report
code_cov_report:
	cd test_results
	lcov --capture --directory test_results --output-file coverage.info
	genhtml coverage.info --output-directory out

valgrind: csv_stats
	# Can't run valgrind against csv_test because it mangles the working directory
	# which causes csv_test to not be able to find test files
	valgrind --leak-check=full ./csv_stats $(TEST_DIR)/data/real_data/2016_Gaz_place_national.txt
	
.PHONY: all clean distclean
	
clean:
	rm -f build/*
	rm -f *.gc*
	rm -f libcsv.a
	rm -f csv_*
	
distclean: clean