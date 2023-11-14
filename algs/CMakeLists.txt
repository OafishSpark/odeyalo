cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

project(Odeyalo)
add_executable(Odeyalo main.cpp ${SRC_FILES})
target_include_directories(Odeyalo PUBLIC ./include)
