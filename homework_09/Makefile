CXX_FILES = include/Types.h include/MissionProcessor.h include/Logging.h \
            include/interfaces/ITargetProvider.h include/interfaces/IBallisticSolver.h include/interfaces/IConfigLoader.h \
            include/solvers/AnalyticalSolver.h include/providers/JsonTargetProvider.h \
            include/config/FileConfigLoader.h include/config/ComponentFactory.h \
            src/MissionProcessor.cpp src/main.cpp \
            src/solvers/AnalyticalSolver.cpp src/providers/JsonTargetProvider.cpp \
            src/config/FileConfigLoader.cpp src/config/ComponentFactory.cpp \
            tests/MissionTests.cpp
CMAKE_FILE = CMakeLists.txt

.PHONY: format lint test quality

format:
	clang-format -i $(CXX_FILES)
	cmake-format -i $(CMAKE_FILE)

lint:
	run-clang-tidy -p ../build/debug -header-filter='homework_08/(src|include)/(?!json\.hpp).*' 'homework_08/' 2>&1 | grep "error:" || true

test:
	cmake --build ../build/debug --target MissionTests
	ctest --test-dir ../build/debug --output-on-failure

quality: format lint test