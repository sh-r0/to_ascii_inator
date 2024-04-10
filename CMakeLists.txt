cmake_minimum_required(VERSION 3.13.4)

set (CMAKE_CXX_STANDARD 20)

project(to_ascii_inator)

add_compile_definitions(CRT_SECURE_NO_WARNINGS)

include_directories(${MY_ROOT} src/stb)

#target_include_directories(${PROJECT_NAME} PRIVATE ${MY_ROOT} src/stb)

add_executable(${PROJECT_NAME} src/main.cpp)