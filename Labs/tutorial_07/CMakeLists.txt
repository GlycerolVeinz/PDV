cmake_minimum_required(VERSION 3.15)
project(Vectorization)

# C++ 20 for std::numbers::pi
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE "Release")
if(MSVC)
	add_compile_options("/W4")
	add_compile_options("/O2")

	# report the autovectorized loops
	add_compile_options("/Qvec-report:2")

	# enable floating point arithmetic reordering necessary for some autovectorizations
	add_compile_options("/fp:fast")

	# increase the stack size to prevent the SO as a result of large static array allocation
	add_link_options("/STACK: 10000000")
	
else()
	add_compile_options("-Wall" "-Wextra")

	# we need to optimize only using O2 because with O3, autovectorization (-ftree-vectorize) is active by default
	add_compile_options("-O2")

	# report the autovectorized loops
	add_compile_options("-fopt-info-vec-all")

	# use the hardware specific optimization. Without this or -mavx, the vector instructions are disabled to support 
	# old CPUs.
	add_compile_options("-march=native")

	# autovectorization. toggle comment to see the change in runtime
	add_compile_options("-ftree-vectorize")
endif()

add_executable(autovec autovec.cpp)

add_executable(normdist normdist.cpp)

add_executable(condswap cond.cpp)

add_executable(lzcnt  lzcnt.cpp)

add_executable(popcnt  popcnt.cpp)