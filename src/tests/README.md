# Tests

This directory contains the test suite for the `irsol` library.

## Unit tests
### Structure

The [`unit`](./unit) folder structure closely mirrors the organization of the main `irsol` library [source code](../irsol/).  
This helps to keep tests well-organized and easy to navigate in relation to the corresponding library components.

### Testing Framework

All tests are implemented using [Catch2](https://github.com/catchorg/Catch2), a modern, header-only C++ testing framework.  
Catch2 provides expressive test cases, rich assertions, and powerful test organization capabilities, making it a great choice for our project.

### Running Tests

Build and run the tests according to [these](../../docs/development-environmnet-configuration/README.md#building-steps) instructions. The tests will automatically execute and report results using Catch2's reporting style.

---

Feel free to explore the test files to see example test cases and coverage of the various parts of the `irsol` library.
