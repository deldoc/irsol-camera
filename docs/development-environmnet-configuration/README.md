# Development Environment Configuration

This guide will lead you through the setup of a development environment that will allow you to edit/compile and test new code.

## Repository structure
This section presents the structure of the repository:
* [`docs`](../../docs/): a folder containing all documentation associated with this repository
* [`examples`](../../examples/): a folder containing some example applications showing how one might want to integrate the `neoAPI` into a custom `C++` executable.
* [`include`](../../include/): contains the *header* files for libraries that the examples are using. This folder includes the *header* files for the following libraries:
  * [`include/neoapi`](../../include/neoapi/): the header files of the `neoAPI` library, essential for building code for the *Baumer* cameras using the high-level API.
  * [`include/spdlog`](../../include/spdlog/): a performant `C++` logging [library](https://github.com/gabime/spdlog).
* [`lib`](../../lib/): contains *pre-compiled* dynamic libraries that the examples are linking towards. This folder contains the *pre-compiled* dynamic libraries for:
  * [`lib/neoapi`](../../lib/neoapi/): the *pre-compiled* dynamic libraries of the `neoAPI` library, essential for building code for the *Baumer* cameras using the high-level API.

## Code examples
The [`examples`](../../examples/) folder contains a set of example projects, with associated build configuration files that allow to explore the capabilities of `neoAPI`.

### Building the examples
The examples are all structured in a way that they can be compiled via [`cmake`](https://cmake.org/). In order to build the examples, you'll need to make sure to have `cmake` installed in your environment. For openSuse, this can be done via:
```
$> sudo zypper refresh
$> sudo zypper install cmake
```

#### `cmake`  configuration
The [`CMakeLists.txt`](../../examples/CMakeLists.txt) file at the root of the [`examples`](../../examples/) directory is the main configuration file for all the examples.

It contains instructions on where the compiler should find header files, where to find pre-compiled dynamic libraries, and where to find the actual example source code.

Each specific example (e.g. [`getting_started](../../examples/getting_started/)) folder contains a specific `CMakeLists.txt` file that is invoked by the root `CMakeLists.txt`. The example-specific `CMakeLists.txt` file contains configuration and instructions for how that specific example should be built.

#### Building the examples
The process for building `C++` executables following the instructions reported in the associated `CMakeLists.txt` files, is the following:

1. Navigate the to `examples`  folder:
   ```sh
   $> cd examples/
   ```
2. If not already created, create a `build` directory and enter it:
   ```sh
   $> mkdir -p build/
   $> cd build/
   ```
3. Run `cmake` making sure to point the directory containing the root `CMakeLists.txt` file (note, you can optionally specify the build type, as one of `Debug` or `Release`):
   ```sh
   $> cmake [-D CMAKE_BUILD_TYPE={Debug|Release} ]..
   ```
   This will generate compilation instructions for the (default) generator: GNU Make
4. Build the executables using the generated resources (alway from within the `build/`  directory):
   ```sh
   $> make [-j<N>]
   ```

These steps will compile and link the examples in the [`examples`](../../examples/) folder and generate executables in the `examples/build/<debug|release>/bin` target directory.

#### Building the example (fast)
A utility [`Makefile`](../../Makefile) with a rule to build all examples is available, and can be invoked from the root of the repository to compile all examples in the `examples`  folder:
```sh
$> make examples
```
or, if building in debug mode:
```
$> DEBUG=1 make examples
```

This will automatically run the steps defined above, and compile/link the examples for you in one command.

## Linting
The [root Makefile](../../Makefile) contains a rule for _linting_ the `C++` code in this repository. The linting process relies on [clang-format](https://clang.llvm.org/docs/ClangFormat.html): a tool that can be used to format a codebase following some [configurable rules](../../.clang-format).

In order to run linting on the repository source files, run the following command from the root of the repository:
```sh
$> make lint
```

**Note**: you might need to have to install `clang` in your development environment. For openSuse, this can be done via:
```sh
$> sudo zypper refresh
$> sudo zypper install llvm-clang
```