cmake_minimum_required(VERSION 3.15)
project(code)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Release")

if(MSVC)
	add_compile_options("/W4")
	add_compile_options("/O2")
else()
	add_compile_options("-Wall" "-Wextra")
	add_compile_options("-O3")
endif()

find_package(OpenMP REQUIRED)

add_executable(main.bin main.cpp sequential.h lockBased.h lockFree.h)

target_link_libraries(main.bin PUBLIC OpenMP::OpenMP_CXX)

