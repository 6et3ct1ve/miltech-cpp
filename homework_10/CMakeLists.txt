cmake_minimum_required(VERSION 3.20)
project(homework_10 CXX)

add_library(Hw10ConfigLoader src/config/FileConfigLoader.cpp)
target_include_directories(Hw10ConfigLoader PUBLIC include)
target_compile_options(Hw10ConfigLoader PRIVATE -Wall -Wextra -pedantic)

add_library(Hw10TargetProvider src/providers/ThreadSafeTargetProvider.cpp)
target_include_directories(Hw10TargetProvider PUBLIC include)
target_compile_options(Hw10TargetProvider PRIVATE -Wall -Wextra -pedantic)

add_library(Hw10BallisticSolver src/solvers/AnalyticalSolver.cpp src/solvers/BallisticTable.cpp
                                src/solvers/TableSolver.cpp)
target_include_directories(Hw10BallisticSolver PUBLIC include)
target_compile_options(Hw10BallisticSolver PRIVATE -Wall -Wextra -pedantic)

add_library(
    Hw10DroneStates
    src/states/StoppedState.cpp src/states/AcceleratingState.cpp src/states/DeceleratingState.cpp
    src/states/TurningState.cpp src/states/MovingState.cpp)
target_include_directories(Hw10DroneStates PUBLIC include)
target_compile_options(Hw10DroneStates PRIVATE -Wall -Wextra -pedantic)

add_library(Hw10DronePhysics src/DronePhysics.cpp)
target_include_directories(Hw10DronePhysics PUBLIC include)
target_compile_options(Hw10DronePhysics PRIVATE -Wall -Wextra -pedantic)

add_library(Hw10MissionProcessor src/MissionProcessor.cpp)
target_include_directories(Hw10MissionProcessor PUBLIC include)
target_link_libraries(
    Hw10MissionProcessor PUBLIC Hw10ConfigLoader Hw10TargetProvider Hw10BallisticSolver
                                Hw10DroneStates Hw10DronePhysics)
target_compile_options(Hw10MissionProcessor PRIVATE -Wall -Wextra -pedantic)

add_library(Hw10Factory src/config/ComponentFactory.cpp)
target_include_directories(Hw10Factory PUBLIC include)
target_link_libraries(Hw10Factory PUBLIC Hw10ConfigLoader Hw10TargetProvider Hw10BallisticSolver)
target_compile_options(Hw10Factory PRIVATE -Wall -Wextra -pedantic)

add_executable(Hw10MissionCheck src/main.cpp)
target_link_libraries(Hw10MissionCheck PRIVATE Hw10MissionProcessor Hw10Factory)
target_compile_options(Hw10MissionCheck PRIVATE -Wall -Wextra -pedantic)

if(NOT CMAKE_CROSSCOMPILING)
    include(FetchContent)
    include(GoogleTest)
    FetchContent_Declare(
        googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
                       DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(googletest)

    add_executable(Hw10MissionTests tests/MissionTests.cpp)
    target_include_directories(Hw10MissionTests PRIVATE include)
    target_link_libraries(
        Hw10MissionTests
        PRIVATE Hw10ConfigLoader
                Hw10TargetProvider
                Hw10BallisticSolver
                Hw10DroneStates
                Hw10MissionProcessor
                Hw10Factory
                Hw10DronePhysics
                GTest::gtest_main)
    gtest_discover_tests(Hw10MissionTests DISCOVERY_MODE PRE_TEST)
endif()
