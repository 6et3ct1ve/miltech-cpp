CXX_FILES = include/Types.h include/ThreadSafeQueue.h include/DronePhysics.h include/MissionProcessor.h include/Logging.h \
            include/interfaces/ITargetProvider.h include/interfaces/IBallisticSolver.h include/interfaces/IConfigLoader.h include/interfaces/IDroneState.h \
            include/solvers/AnalyticalSolver.h include/solvers/BallisticTable.h include/solvers/TableSolver.h include/providers/ThreadSafeTargetProvider.h \
            include/config/FileConfigLoader.h include/config/ComponentFactory.h \
            include/states/StoppedState.h include/states/AcceleratingState.h include/states/DeceleratingState.h include/states/TurningState.h include/states/MovingState.h \
            src/MissionProcessor.cpp src/main.cpp \
            src/solvers/AnalyticalSolver.cpp src/solvers/BallisticTable.cpp src/solvers/TableSolver.cpp src/providers/ThreadSafeTargetProvider.cpp \
            src/config/FileConfigLoader.cpp src/config/ComponentFactory.cpp \
            src/states/StoppedState.cpp src/states/AcceleratingState.cpp src/states/DeceleratingState.cpp src/states/TurningState.cpp src/states/MovingState.cpp \
            tests/MissionTests.cpp
CMAKE_FILE = CMakeLists.txt

.PHONY: format lint test quality

format:
	clang-format -i $(CXX_FILES)
	cmake-format -i $(CMAKE_FILE)

lint:
	run-clang-tidy -p ../build/debug -header-filter='homework_10/(src|include)/(?!json\.hpp).*' 'homework_10/' 2>&1 | grep "error:" || true

test:
	cmake --build ../build/debug --target Hw10MissionTests
	ctest --test-dir ../build/debug --output-on-failure

quality: format lint test