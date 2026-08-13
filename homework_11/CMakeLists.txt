cmake_minimum_required(VERSION 3.20)
project(homework_11 CXX)

add_library(Hw11BallisticSolver src/solvers/AnalyticalSolver.cpp src/solvers/BallisticTable.cpp
                                src/solvers/TableSolver.cpp)
target_include_directories(Hw11BallisticSolver PUBLIC include)
target_compile_options(Hw11BallisticSolver PRIVATE -Wall -Wextra -pedantic)

add_library(
    Hw11DroneStates
    src/states/StoppedState.cpp src/states/AcceleratingState.cpp src/states/DeceleratingState.cpp
    src/states/TurningState.cpp src/states/MovingState.cpp)
target_include_directories(Hw11DroneStates PUBLIC include)
target_compile_options(Hw11DroneStates PRIVATE -Wall -Wextra -pedantic)

add_library(Hw11Io src/io/UartLink.cpp src/io/GpioOutput.cpp)
target_include_directories(Hw11Io PUBLIC include)
target_link_libraries(Hw11Io PUBLIC gpiod)
target_compile_options(Hw11Io PRIVATE -Wall -Wextra -pedantic)

add_library(Hw11TargetTracker src/TargetTracker.cpp)
target_include_directories(Hw11TargetTracker PUBLIC include)
target_compile_options(Hw11TargetTracker PRIVATE -Wall -Wextra -pedantic)

add_library(Hw11DroneController src/DroneController.cpp)
target_include_directories(Hw11DroneController PUBLIC include)
target_compile_options(Hw11DroneController PRIVATE -Wall -Wextra -pedantic)

add_library(Hw11MissionProcessor src/MissionProcessor.cpp)
target_include_directories(Hw11MissionProcessor PUBLIC include)
target_link_libraries(Hw11MissionProcessor PUBLIC Hw11BallisticSolver Hw11DroneStates)
target_compile_options(Hw11MissionProcessor PRIVATE -Wall -Wextra -pedantic)

add_library(Hw11Factory src/config/ComponentFactory.cpp)
target_include_directories(Hw11Factory PUBLIC include)
target_link_libraries(Hw11Factory PUBLIC Hw11BallisticSolver Hw11Io)
target_compile_options(Hw11Factory PRIVATE -Wall -Wextra -pedantic)

add_library(Hw11MissionRunner src/MissionRunner.cpp)
target_include_directories(Hw11MissionRunner PUBLIC include)
target_link_libraries(Hw11MissionRunner PUBLIC Hw11MissionProcessor Hw11DroneController
                                               Hw11TargetTracker Hw11Io)
target_compile_options(Hw11MissionRunner PRIVATE -Wall -Wextra -pedantic)

add_executable(Hw11MissionCheck src/main.cpp)
target_link_libraries(
    Hw11MissionCheck PRIVATE Hw11MissionProcessor Hw11Factory Hw11Io Hw11TargetTracker
                             Hw11DroneController Hw11MissionRunner)
target_compile_options(Hw11MissionCheck PRIVATE -Wall -Wextra -pedantic)

if(NOT CMAKE_CROSSCOMPILING)
    include(FetchContent)
    include(GoogleTest)
    FetchContent_Declare(
        googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
                       DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(googletest)

    add_executable(Hw11MissionTests tests/MissionTests.cpp)
    target_include_directories(Hw11MissionTests PRIVATE include)
    target_link_libraries(
        Hw11MissionTests
        PRIVATE Hw11BallisticSolver
                Hw11DroneStates
                Hw11MissionProcessor
                Hw11TargetTracker
                Hw11Io
                Hw11DroneController
                Hw11MissionRunner
                Hw11Factory
                GTest::gtest_main)
    gtest_discover_tests(Hw11MissionTests DISCOVERY_MODE PRE_TEST)
endif()
