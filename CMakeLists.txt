cmake_minimum_required(VERSION 3.21)
project(nes CXX)

include(cmake/base.cmake)
include(cmake/warnings.cmake)
include(cmake/debug.cmake)
include(cmake/libraries.cmake)

add_executable(${PROJECT_NAME})

target_compile_features(
	${PROJECT_NAME}
	PRIVATE
		cxx_std_17
)

target_sources(
	${PROJECT_NAME}
	PRIVATE
		src/main.cpp
		src/CPU.cpp
		src/Bus.cpp
		src/Rom.cpp
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set_debug_options(${PROJECT_NAME})
else()
	target_compile_options(${target} PRIVATE -g0 -O3)
endif()

set_default_warnings(${PROJECT_NAME})
link_default_libraries(${PROJECT_NAME})
