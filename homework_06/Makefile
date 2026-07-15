CXX_FILES = include/ballistics.hpp src/ballistics.cpp src/main.cpp tests/ballistics_tests.cpp
CMAKE_FILE = CMakeLists.txt

.PHONY: format lint test quality

format:
	clang-format -i $(CXX_FILES)
	cmake-format -i $(CMAKE_FILE)

lint:
	run-clang-tidy -p ../build/debug 'homework_06/'

test:
	cmake --build ../build/debug --target ballistics_tests
	ctest --test-dir ../build/debug --output-on-failure

quality: format lint test