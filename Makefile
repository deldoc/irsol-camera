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
	@cd examples/build/ && cmake .. && make -j 4
	@echo "Done building examples."