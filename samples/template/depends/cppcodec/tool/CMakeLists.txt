# For cppcodec itself, don't prefer system headers over development ones.
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

add_executable(base32enc base32enc.cpp)
add_executable(base32dec base32dec.cpp)

add_executable(base64enc base64enc.cpp)
add_executable(base64dec base64dec.cpp)

add_executable(hexenc hexenc.cpp)
add_executable(hexdec hexdec.cpp)
