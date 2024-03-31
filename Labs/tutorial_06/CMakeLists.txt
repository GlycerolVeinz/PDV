cmake_minimum_required(VERSION 3.15)
project(sorting)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Release")
if(MSVC)
	add_compile_options("/W4")
	add_compile_options("/O2")
else()
	add_compile_options("-Wall" "-Wextra")
	add_compile_options("-O3")
endif()

# OpenMP 4.0 is required for min and max reductions
find_package(OpenMP 4.5 REQUIRED) 

add_executable(sorting sorting.cpp)
add_executable(prefixsum _prefixsum/prefixsum.cpp)

target_link_libraries(sorting PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(prefixsum PUBLIC OpenMP::OpenMP_CXX)