cmake_minimum_required(VERSION 3.15)
project(mult)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Release")
if(MSVC)
	add_compile_options("/W4")
	add_compile_options("/O2")
else()
	add_compile_options("-Wall" "-Wextra")
	add_compile_options("-O3")
endif()

# OpenMp 4.0 required for user defined reductions
find_package(OpenMP 4.0 REQUIRED) 

add_executable(mult main.cpp  sparse.h multiply.cpp multiply.h)

target_link_libraries(mult PUBLIC OpenMP::OpenMP_CXX)