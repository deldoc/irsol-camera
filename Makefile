.PHONY: lint
lint:
	@echo "Running clang-format..."
	@clang-format --style=file -i -- src/examples/**/*.cpp
	@clang-format --style=file -i -- src/irsol/lib/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/*.hpp
	@echo "Done running clang-format."


.PHONY: examples
examples:
	@echo "Setting up examples..."
	@cd src/examples/ && mkdir -p build && cd build
	@echo "Building examples..."
	@if [ -n "$$DEBUG" ]; then \
		echo "Debug mode is enabled"; \
		cd src/examples/build && cmake -D CMAKE_BUILD_TYPE=Debug ..; \
	else \
		echo "Release mode is enabled"; \
		cd src/examples/build && cmake -D CMAKE_BUILD_TYPE=Release ..; \
	fi
	@cd src/examples/build && make -j 4
	@echo "Done building examples."