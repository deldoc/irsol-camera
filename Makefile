lint:
	@echo "Running clang-format..."
	@clang-format --style=file -i -- src/examples/**/*.cpp
	@clang-format --style=file -i -- src/irsol/lib/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/*.hpp
	@clang-format --style=file -i -- src/irsol/lib/**/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/**/*.hpp
	@echo "Done running clang-format."
.PHONY: lint


build:
	@echo "Setting up build environment..."
	@cd src/ && mkdir -p build && cd build
	@echo "Building code..."
	@if [ -n "$$DEBUG" ]; then \
		echo "Debug mode is enabled"; \
		cd src/build && cmake -D CMAKE_BUILD_TYPE=Debug ..; \
	else \
		echo "Release mode is enabled"; \
		cd src/build && cmake -D CMAKE_BUILD_TYPE=Release ..; \
	fi
	@cd src/build && make -j 8
	@echo "Done building."
.PHONY: build

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