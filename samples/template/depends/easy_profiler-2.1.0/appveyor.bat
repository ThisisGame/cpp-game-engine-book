mkdir build_msvc
cd build_msvc
cmake -G "%GENERATOR%"  ../
cmake --build . --config Release

goto :EOF
