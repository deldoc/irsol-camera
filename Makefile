lint:
	@echo "Running clang-format..."
	@clang-format --style=file -i -- examples/**/*.cpp
	@echo "Done running clang-format."