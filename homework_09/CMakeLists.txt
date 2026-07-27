cmake_minimum_required(VERSION 3.20)
project(homework_09 CXX)

add_library(ConfigLoader src/config/FileConfigLoader.cpp)
target_include_directories(ConfigLoader PUBLIC include)
target_compile_options(ConfigLoader PRIVATE -Wall -Wextra -pedantic)

add_library(TargetProvider src/providers/JsonTargetProvider.cpp)
target_include_directories(TargetProvider PUBLIC include)
target_compile_options(TargetProvider PRIVATE -Wall -Wextra -pedantic)

add_library(BallisticSolver src/solvers/AnalyticalSolver.cpp)
target_include_directories(BallisticSolver PUBLIC include)
target_compile_options(BallisticSolver PRIVATE -Wall -Wextra -pedantic)

add_library(MissionProcessor src/MissionProcessor.cpp)
target_include_directories(MissionProcessor PUBLIC include)
target_link_libraries(MissionProcessor PUBLIC ConfigLoader TargetProvider BallisticSolver)
target_compile_options(MissionProcessor PRIVATE -Wall -Wextra -pedantic)

add_library(Factory src/config/ComponentFactory.cpp)
target_include_directories(Factory PUBLIC include)
target_link_libraries(Factory PUBLIC ConfigLoader TargetProvider BallisticSolver)
target_compile_options(Factory PRIVATE -Wall -Wextra -pedantic)

add_executable(MissionCheck src/main.cpp)
target_link_libraries(MissionCheck PRIVATE MissionProcessor Factory)
target_compile_options(MissionCheck PRIVATE -Wall -Wextra -pedantic)

if(NOT CMAKE_CROSSCOMPILING)
    include(FetchContent)
    include(GoogleTest)
    FetchContent_Declare(
        googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
                       DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(googletest)

    add_executable(MissionTests tests/MissionTests.cpp)
    target_include_directories(MissionTests PRIVATE include)
    target_link_libraries(MissionTests PRIVATE ConfigLoader TargetProvider BallisticSolver
                                               MissionProcessor Factory GTest::gtest_main)
    gtest_discover_tests(MissionTests DISCOVERY_MODE PRE_TEST)
endif()
