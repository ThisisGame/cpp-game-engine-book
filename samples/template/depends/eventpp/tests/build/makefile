CACHE_DIR = temp_cache
CMAKE = cmake ../.. $(CMAKE_FLAGS)

MK_DIR = @cmake -E make_directory
CH_DIR = @cmake -E chdir
ECHO = @cmake -E echo

MK_CACHE = $(MK_DIR) $(CACHE_DIR)
EXEC_BUILD = $(CH_DIR) $(CACHE_DIR)

PROJECT=eventpptest

PROJECT_PREFIX = project

SUPPORT_MAKES = linux mingw mingw_debug msys nmake vc17 vc15

none: needcmake
	$(ECHO) "Usage:"
	$(ECHO) "  make MakeType [CMAKE_FLAGS='flags passed to cmake']"
	$(ECHO) "or"
	$(ECHO) "  nmake MakeType"
	$(ECHO) "if MS VC is used"
	$(ECHO) 
	$(ECHO) "Available MakeType"
	$(ECHO) "  $(SUPPORT_MAKES)"
	$(ECHO) 
	$(ECHO) "  linux        Generate Linux/Unix makefile and then use GCC make to build."
	$(ECHO) "  mingw        Generate MinGW makefile and then use mingw32-make to build (release version)."
	$(ECHO) "  mingw_debug  Generate MinGW makefile and then use mingw32-make to build (debug version)."
	$(ECHO) "  mingw_coverage  Generate MinGW makefile and then use mingw32-make to build and generate code coverage report."
	$(ECHO) "  msys         Generate MSys makefile and then use make to build (release version)."
	$(ECHO) "  nmake        Generate Microsoft VC makefile and then use nmake to build."
	$(ECHO) "  vc19         Generate project files for Microsoft VC 2019. No auto build. You need to open the project in VC IDE then build."
	$(ECHO) "  vc17         Generate project files for Microsoft VC 2017. No auto build. You need to open the project in VC IDE then build."
	$(ECHO) "  vc15         Generate project files for Microsoft VC 2015. No auto build. You need to open the project in VC IDE then build."
	$(ECHO) "  auto         Auto detect the compiler and make environment and then use make to build. NOT recommend."

needcmake:
	$(ECHO)

auto: needcmake
	$(MK_CACHE)_auto
	$(CH_DIR) $(CACHE_DIR)_auto $(CMAKE)
	$(EXEC_BUILD)_auto make $(TARGET)

mingw: needcmake
	$(MK_CACHE)_mingw
	$(CH_DIR) $(CACHE_DIR)_mingw $(CMAKE) -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles"
	$(EXEC_BUILD)_mingw mingw32-make $(TARGET)

mingw_debug: needcmake
	$(MK_CACHE)_mingw_debug
	$(CH_DIR) $(CACHE_DIR)_mingw_debug $(CMAKE) -DCMAKE_BUILD_TYPE=Debug -G"MinGW Makefiles"
	$(EXEC_BUILD)_mingw_debug mingw32-make $(TARGET)

mingw_coverage: needcmake
	$(MK_CACHE)_mingw_coverage
	$(MK_DIR) ../coverage
	@cmake -E remove $(CACHE_DIR)_mingw_coverage/unittest/CMakeFiles/unittest.dir/*.gcov
	@cmake -E remove ../coverage/*
	$(CH_DIR) $(CACHE_DIR)_mingw_coverage $(CMAKE) -Dcoverage=True -G"MinGW Makefiles"
	$(EXEC_BUILD)_mingw_coverage mingw32-make $(TARGET)
	$(ECHO) "Running tests..."
	$(CH_DIR) $(CACHE_DIR)_mingw_coverage/unittest unittest
	$(CH_DIR) $(CACHE_DIR)_mingw_coverage/unittest/CMakeFiles/unittest.dir gcov -b ./*.cpp.gcno
	$(CH_DIR) ../coverage gcovr -r ../.. --html --html-details -o coverage.html

msys: needcmake
	$(MK_CACHE)_msys
	$(CH_DIR) $(CACHE_DIR)_msys $(CMAKE) -DCMAKE_BUILD_TYPE=Release -G"MSYS Makefiles"
	$(EXEC_BUILD)_msys make $(TARGET)

msys_debug: needcmake
	$(MK_CACHE)_msys_debug
	$(CH_DIR) $(CACHE_DIR)_msys_debug $(CMAKE) -DCMAKE_BUILD_TYPE=Debug -G"MSYS Makefiles"
	$(EXEC_BUILD)_msys_debug make $(TARGET)

nmake: needcmake
	$(ECHO) NOTE: *****************************
	$(ECHO) NOTE: If cmake raises errors, try run this in Visual Studio Command Prompt from the VS package.
	$(ECHO) NOTE: *****************************
	$(MK_CACHE)_nmake
	$(CH_DIR) $(CACHE_DIR)_nmake $(CMAKE) -G"NMake Makefiles"
	$(EXEC_BUILD)_nmake nmake $(TARGET)

linux: needcmake
	$(MK_CACHE)_linux
	$(CH_DIR) $(CACHE_DIR)_linux $(CMAKE) -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"
	$(EXEC_BUILD)_linux make $(TARGET)

linux_debug: needcmake
	$(MK_CACHE)_linux_debug
	$(CH_DIR) $(CACHE_DIR)_linux_debug $(CMAKE) -DCMAKE_BUILD_TYPE=Debug -G"Unix Makefiles"
	$(EXEC_BUILD)_linux_debug make $(TARGET)

vc19: needcmake
	$(MK_DIR) $(PROJECT_PREFIX)_vc19
	$(CH_DIR) $(PROJECT_PREFIX)_vc19 $(CMAKE) -G "Visual Studio 16 2019" -A x64
	$(ECHO) Please open the solution $(PROJECT).sln in $(PROJECT_PREFIX)_vc19 in VC IDE.

vc17: needcmake
	$(MK_DIR) $(PROJECT_PREFIX)_vc17
	$(CH_DIR) $(PROJECT_PREFIX)_vc17 $(CMAKE) -G"Visual Studio 15 2017 Win64"
	$(ECHO) Please open the solution $(PROJECT).sln in $(PROJECT_PREFIX)_vc17 in VC IDE.

vc15: needcmake
	$(MK_DIR) $(PROJECT_PREFIX)_vc15
	$(CH_DIR) $(PROJECT_PREFIX)_vc15 $(CMAKE) -G"Visual Studio 14 Win64"
	$(ECHO) Please open the solution $(PROJECT).sln in $(PROJECT_PREFIX)_vc15 in VC IDE.

.PHONY: clean

