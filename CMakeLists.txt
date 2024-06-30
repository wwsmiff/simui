cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER clang)
option(SIMUI_BUILD_EXAMPLES OFF)
project(simui)

find_package(SDL2)

add_library(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2_ttf)
target_sources(${PROJECT_NAME} PRIVATE
	src/context.c
	src/helper.c
	src/text.c
	src/widget.c
	src/window.c
)

if(${SIMUI_BUILD_EXAMPLES})
	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	project(stopwatch CXX)
	add_executable(stopwatch examples/stopwatch.cpp)
	target_include_directories(stopwatch PRIVATE ${CMAKE_SOURCE_DIR}/include)
	target_link_libraries(stopwatch PRIVATE simui SDL2 SDL2_ttf)
endif()
