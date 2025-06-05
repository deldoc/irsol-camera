# Tests {#tests}

This directory contains the test suite for the `irsol` library.

## Unit tests
### Structure

The `unit/` folder structure closely mirrors the organization of the main `irsol` library source code.  
This helps to keep tests well-organized and easy to navigate in relation to the corresponding library components.

### Testing Framework

All tests are implemented using [Catch2](https://github.com/catchorg/Catch2), a modern, header-only C++ testing framework.  
Catch2 provides expressive test cases, rich assertions, and powerful test organization capabilities, making it a great choice for our project.

### Running Tests

Build and run the tests according to @ref development_environment_configuration instructions. The tests will automatically execute and report results using Catch2's reporting style.

### Writing new Tests
Feel free to create new tests in existing `test_...cpp` files, or creating new test files. If creating new test files, make sure to add them into the corresponding `CMakeLists.txt` file for compilation.

The structure of a test file is:
```cpp
#include "irsol/irsol.hpp"  // or whatever headers you need

#include <catch2/catch_all.hpp>  // include the testing framework

TEST_CASE("MyTestCaseName", "[Tag1][Tag2]")
{
  int i = 1;
  CHECK(i == 1);
  CHECK_FALSE(i < 1);
}
```
Where:
 * `"MyTestCaseName"` is a string describing the test you're running. Ideally this should be short and representative
 * `"[Tag1][Tag2]"` is a string with formed by 0 or more `[tagname]` elements. This allows to run all testcases that are tagged with a specific tag easily via the command line.

More details are to be found on the [Catch2](https://github.com/catchorg/Catch2) documentation.
