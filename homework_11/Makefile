CXX_FILES = include/Types.h include/MissionProcessor.h include/Logging.h \
            include/interfaces/IBallisticSolver.h include/interfaces/IDroneState.h include/interfaces/ILinkHandler.h include/interfaces/IUartLink.h include/interfaces/IGpioOutput.h \
            include/io/UartLink.h include/io/GpioOutput.h \
            include/solvers/AnalyticalSolver.h include/solvers/BallisticTable.h include/solvers/TableSolver.h \
            include/config/ComponentFactory.h \
            include/states/StoppedState.h include/states/AcceleratingState.h include/states/DeceleratingState.h include/states/TurningState.h include/states/MovingState.h \
            src/MissionProcessor.cpp src/main.cpp \
            src/io/UartLink.cpp src/io/GpioOutput.cpp \
            src/solvers/AnalyticalSolver.cpp src/solvers/BallisticTable.cpp src/solvers/TableSolver.cpp \
            src/config/ComponentFactory.cpp \
            src/states/StoppedState.cpp src/states/AcceleratingState.cpp src/states/DeceleratingState.cpp src/states/TurningState.cpp src/states/MovingState.cpp \
            tests/MissionTests.cpp
CMAKE_FILE = CMakeLists.txt

.PHONY: format lint test quality

format:
	clang-format -i $(CXX_FILES)
	cmake-format -i $(CMAKE_FILE)

lint:
	run-clang-tidy -p ../build/debug -header-filter='homework_11/(src|include)/(?!drone_link\.h).*' 'homework_11/' 2>&1 | grep "error:" || true

test:
	cmake --build ../build/debug --target Hw11MissionTests
	ctest --test-dir ../build/debug --output-on-failure

quality: format lint test