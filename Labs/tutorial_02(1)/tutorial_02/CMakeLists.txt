cmake_minimum_required(VERSION 3.15)
project(B4B36PDV_02)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Release")

if(MSVC)
	add_compile_options("/W4")
	add_compile_options("/O2")
else()
	add_compile_options("-Wall" "-Wextra")
	add_compile_options("-O3")
endif()

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(benchmark.bin benchmark.cpp decryption.cpp PDVCrypt.cpp)
add_executable(condition_variable.bin condition_variable.cpp)
add_executable(decrypt.bin decrypt.cpp PDVCrypt.cpp)

target_link_libraries(condition_variable.bin PUBLIC Threads::Threads)
target_link_libraries(benchmark.bin PUBLIC OpenMP::OpenMP_CXX)
