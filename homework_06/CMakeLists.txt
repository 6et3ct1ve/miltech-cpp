cmake_minimum_required(VERSION 3.20)
project(homework_06 CXX)

add_library(ballistics src/ballistics.cpp)
target_include_directories(ballistics PUBLIC include)

add_executable(ballistics_check src/main.cpp)
target_link_libraries(ballistics_check PRIVATE ballistics)

target_compile_options(ballistics_check PRIVATE -Wall -Wextra -Werror -pedantic)
