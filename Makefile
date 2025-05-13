.PHONY: lint
lint:
	@echo "Running clang-format..."
	@clang-format --style=file -i -- examples/**/*.cpp
	@echo "Done running clang-format."


.PHONY: examples
examples:
	@echo "Setting up examples..."
	@cd examples/ && mkdir -p build && cd build
	@echo "Building examples..."
	@if [ -n "$$DEBUG" ]; then \
		echo "Debug mode is enabled"; \
		cd examples/build && cmake -D CMAKE_BUILD_TYPE=Debug ..; \
	else \
		echo "Release mode is enabled"; \
		cd examples/build && cmake -D CMAKE_BUILD_TYPE=Release ..; \
	fi
	@cd examples/build && make -j 4
	@echo "Done building examples."