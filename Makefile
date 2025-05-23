lint:
	@echo "Running clang-format..."
	@clang-format --style=file -i -- src/examples/**/*.cpp
	@clang-format --style=file -i -- src/irsol/lib/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/*.hpp
	@clang-format --style=file -i -- src/irsol/lib/**/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/**/*.hpp
	@clang-format --style=file -i -- src/irsol/include/**/**/*.tpp
	@clang-format --style=file -i -- src/tests/**/*.cpp
	@echo "Done running clang-format."
.PHONY: lint


build:
	@echo "Setting up build environment..."
	@if [ -n "$$DEBUG" ]; then \
		BUILD_DIR=src/build/debug; \
		CONFIG=Debug; \
	else \
		BUILD_DIR=src/build/release; \
		CONFIG=Release; \
	fi; \
	mkdir -p $$BUILD_DIR && cd $$BUILD_DIR && \
	cmake -DCMAKE_BUILD_TYPE=$$CONFIG -DIRSOL_BUILD_TESTS=ON ../.. && \
	make -j8
	@echo "Done building."
.PHONY: build

tests: build
	@echo "Running unit-tests"
	@if [ -n "$$DEBUG" ]; then \
		echo "Debug mode is enabled"; \
		./src/dist/debug/bin/unit_tests --log-level trace; \
	else \
		echo "Release mode is enabled"; \
		./src/dist/release/bin/unit_tests; \
	fi
.PHONY: tests

clean:
	@echo "Cleaning build files..."
	@cd src/build && make clean
	@echo "Done cleaning."
.PHONY: clean

deepclean: clean
	@echo "Removing build and dist directories..."
	@cd src && rm -rf build/ && rm -rf dist/
	@echo "Done deep-cleaning."
.PHONY: deepclean

all: lint build
	@echo "All tasks completed."
.PHONY: all