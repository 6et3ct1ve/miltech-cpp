CXX_FILES = include/coord.hpp include/config_loader.hpp include/target_provider.hpp \
            include/ballistic_solver.hpp include/mission_processor.hpp include/factory.hpp \
            include/logging.hpp \
            src/config_loader.cpp src/target_provider.cpp src/ballistic_solver.cpp \
            src/mission_processor.cpp src/factory.cpp src/main.cpp \
            tests/mission_tests.cpp
CMAKE_FILE = CMakeLists.txt

.PHONY: format lint test quality

format:
	clang-format -i $(CXX_FILES)
	cmake-format -i $(CMAKE_FILE)

lint:
	run-clang-tidy -p ../build/debug -header-filter='homework_07/(src|include)/(?!json\.hpp).*' 'homework_07/' 2>&1 | grep "error:" || true
test:
	cmake --build ../build/debug --target mission_tests
	ctest --test-dir ../build/debug --output-on-failure

quality: format lint test