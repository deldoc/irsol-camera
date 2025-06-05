# Variables for build configuration
BUILD_TYPE_RELEASE = Release
BUILD_TYPE_DEBUG   = Debug

BUILD_DIR_RELEASE  = src/build/release
BUILD_DIR_DEBUG    = src/build/debug

DIST_DIR_RELEASE   = src/dist/release
DIST_DIR_DEBUG     = src/dist/debug

DOCS_DIR           = docs/generated

ifeq ($(DEBUG),1)
	BUILD_TYPE = $(BUILD_TYPE_DEBUG)
	BUILD_DIR  = $(BUILD_DIR_DEBUG)
	DIST_DIR   = $(DIST_DIR_DEBUG)
else
	BUILD_TYPE = $(BUILD_TYPE_RELEASE)
	BUILD_DIR  = $(BUILD_DIR_RELEASE)
	DIST_DIR   = $(DIST_DIR_RELEASE)
endif

# Formatting rule
lint:
	@echo "Running clang-format..."
	@clang-format --style=file -i -- src/examples/**/*.cpp
	@clang-format --style=file -i -- src/irsol/lib/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/*.hpp
	@clang-format --style=file -i -- src/irsol/lib/**/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/**/*.hpp
	@clang-format --style=file -i -- src/irsol/include/**/**/*.tpp
	@clang-format --style=file -i -- src/irsol/lib/**/**/**/*.cpp
	@clang-format --style=file -i -- src/irsol/include/**/**/**/*.hpp
	@clang-format --style=file -i -- src/tests/unit/*.cpp
	@clang-format --style=file -i -- src/tests/unit/**/*.cpp
	@clang-format --style=file -i -- src/tests/unit/**/**/*.cpp
	@echo "Done running clang-format."
.PHONY: lint

# Build: core + examples + tests
build:
	@echo "Building full irsol project (core, examples, and tests)..."
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DIRSOL_BUILD_CORE=ON -DIRSOL_BUILD_TESTS=ON -DIRSOL_BUILD_EXAMPLES=ON ../../.. && \
	cmake --build . --parallel
	@echo "Done building all."
.PHONY: build/all

# Build: core + examples (no tests)
build/examples:
	@echo "Building irsol project and examples (core, examples)..."
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DIRSOL_BUILD_CORE=ON -DIRSOL_BUILD_TESTS=OFF -DIRSOL_BUILD_EXAMPLES=ON ../../.. && \
	cmake --build . --parallel
	@echo "Done building core and examples."
.PHONY: build/all

# Build only core project (no tests, no examples)
build/core:
	@echo "Building core irsol project..."
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DIRSOL_BUILD_CORE=ON -DIRSOL_BUILD_TESTS=OFF -DIRSOL_BUILD_EXAMPLES=OFF ../../.. && \
	cmake --build . --parallel
	@echo "Done building core."
.PHONY: build

# Build and run tests
build/tests:
	@echo "Building test suite..."
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DIRSOL_BUILD_CORE=ON -DIRSOL_BUILD_TESTS=ON ../../.. && \
	cmake --build . --target unit_tests --parallel
	@echo "Done building tests."
.PHONY: build/tests

tests: build/tests
	@echo "Running unit tests..."
	@$(DIST_DIR)/bin/unit_tests
	@echo "Done running tests."
.PHONY: tests

# Documentation generation
docs:
	@echo "Generating documentation..."
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DIRSOL_BUILD_CORE=OFF -DIRSOL_BUILD_DOCS=ON -DIRSOL_BUILD_TESTS=OFF -DIRSOL_BUILD_EXAMPLES=OFF -DIRSOL_BUILD_CORE=OFF ../../.. && \
	cmake --build .
	@echo "Running doxygen..."
	doxygen $(DOCS_DIR)/Doxyfile
	mkdir -p $(DOCS_DIR)/html/neoapi/
	cp -r src/external/neoapi/docs/* $(DOCS_DIR)/html/neoapi/
	@echo "Done building documentation."
.PHONY: docs

# Clean build artifacts
clean:
	@echo "Cleaning build files for $(BUILD_TYPE)..."
	@if [ -d "$(BUILD_DIR)" ]; then cd $(BUILD_DIR) && make clean; fi
	@echo "Done cleaning."
.PHONY: clean

# Remove all build and dist folders and docs folders
deepclean: clean
	@echo "Removing all build and dist directories for $(BUILD_TYPE)..."
	@rm -rf $(BUILD_DIR) $(DIST_DIR) $(DOCS_DIR)
	@echo "Done deep-cleaning."
.PHONY: deepclean

# Convenience rule: lint + full build + docs
all: lint build docs
	@echo "All tasks completed."
.PHONY: all
