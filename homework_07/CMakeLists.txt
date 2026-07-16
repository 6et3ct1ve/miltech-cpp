cmake_minimum_required(VERSION 3.20)
project(homework_07 CXX)

add_library(config_loader src/config_loader.cpp)
target_include_directories(config_loader PUBLIC include)
target_compile_options(config_loader PRIVATE -Wall -Wextra -pedantic)

add_library(target_provider src/target_provider.cpp)
target_include_directories(target_provider PUBLIC include)
target_compile_options(target_provider PRIVATE -Wall -Wextra -pedantic)

add_library(ballistic_solver src/ballistic_solver.cpp)
target_include_directories(ballistic_solver PUBLIC include)
target_compile_options(ballistic_solver PRIVATE -Wall -Wextra -pedantic)

add_library(mission_processor src/mission_processor.cpp)
target_include_directories(mission_processor PUBLIC include)
target_link_libraries(mission_processor PUBLIC config_loader target_provider ballistic_solver)
target_compile_options(mission_processor PRIVATE -Wall -Wextra -pedantic)

add_library(factory src/factory.cpp)
target_include_directories(factory PUBLIC include)
target_link_libraries(factory PUBLIC config_loader target_provider ballistic_solver)
target_compile_options(factory PRIVATE -Wall -Wextra -pedantic)

add_executable(mission_check src/main.cpp)
target_link_libraries(mission_check PRIVATE mission_processor factory)
target_compile_options(mission_check PRIVATE -Wall -Wextra -pedantic)
