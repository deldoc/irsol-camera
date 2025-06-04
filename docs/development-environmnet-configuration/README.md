# Development Environment Configuration {#development_environment_configuration}

This guide will lead you through the setup of a development environment that will allow you to edit, compile, and test new code.

## Repository Structure
This section presents the structure of the repository:
- [`docs`](../../docs/): Contains all documentation associated with this repository.
- [`src`](../../src/): Contains the source code for the project:
  - [`examples`](../../src/examples/): Example applications demonstrating how to use the `neoAPI` with the `irsol` library.
  - [`external`](../../src/external/): External dependencies, such as `neoAPI`, `ppk_assert` and `tabulate`.
  - [`irsol`](../../src/irsol/): Core library for interacting with the cameras.
  - [`tests`](../../src/tests/): Tests implementation for the irsol library.


### Building the source code

The `irsol` project uses [`CMake`](https://cmake.org/) as its primary build system. CMake is a powerful, cross-platform build automation tool that generates native build scripts (e.g., Makefiles, Ninja, Visual Studio projects) from a declarative configuration file (CMakeLists.txt).

We use CMake because it enables:

 * _Platform independence_ – The same configuration can target Linux, macOS, or Windows.
 * _Modular dependency management_ – External libraries like spdlog, sockpp, or NeoAPI are cleanly integrated via FetchContent or find_package().
 * _Flexible configuration_ – Build types like Debug or Release can be selected easily.
 * _Reusable targets_ – Internal libraries like irsol::core and irsol::cv are defined once and reused across multiple tools or test suites.
 * _Toolchain integration_ – Compiler warnings, assertions, logging levels, and other build settings are driven by configuration flags.
 * _Out-of-source builds_ – Keeps build artifacts separate from the source directory.

Ensure `cmake` is installed in your environment. For openSUSE, this can be done via:
```sh
$> sudo zypper refresh
$> sudo zypper install cmake
```

#### Building Steps
A convenience [`Makefile`](../../Makefile) at the root of the repository implements rules for invoking `CMake` for you.

**Note**: for each of the following commands, you can build the same target in `Debug` mode by adding the `DEBUG=1` flag after the make command. By default, targets are build in `Release` mode.

* Building the `irsol` project, the examples and associated unit-tests
  ```sh
  $> make build
  ```
  or, for `Debug` build type
  ```sh
  $> make build DEBUG=1
  ```
  This will compile the `irsol` project, associated examples and unit-tests.

* Building only the `irsol` examples without the unit-tests
  ```sh
  $> make build/examples
  ```
  This will compile the core `irsol` project and associated examples.

* Building only the `irsol` project without the examples and without the unit-tests
  ```sh
  $> make build/core
  ```
  This will compile only the `irsol` core project, no examples and no tests.
  
* Building  unit-tests
  ```sh
  $> make build/tests
  ```
  This will compile all targets needed for building unit-tests.

* Building and running unit-tests
  ```sh
  $> make tests
  ```
  This will first build, then run unit-tests. Test results are shown on the console once tests are finished.

* Building documentation
  ```sh
  $> make docs
  ```
  This will build [Doxygen](https://www.doxygen.nl/index.html) documentation and generate HTML content in the `docs/generated/html` folder.
  Documentation is always generated automatically when code is pushed to the `main` branch of the repository, following the _Github action_ implemented as a workflow.

  A live version of the documentation is published here: https://deldoc.github.io/irsol-camera/

* Linting the code (autoformatting)
  ```sh
  $> make lint
  ```
  This will run [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) on the repository's codebase and format the files according to the <a href=".clang-format" target="_blank"><b>clang-format style guide</b></a>.

  **Note**: you might need to have to install `clang` in your development environment. For openSuse, this can be done via:
  ```sh
  $> sudo zypper refresh
  $> sudo zypper install llvm-clang
  ```

* Cleaning the build artifacts
  ```sh
  $> make clean
  ```
  This will remove all build artifacts for the current build configuration following CMake's best attempt at cleaning the build state.

* Deep-cleaning the build artifacts
  ```sh
  $> make deepclean
  ```
  This will remove all build artifacts for the current build configuration by deleting all artifacts in the build and distribution directories.


Some of the above commands will generate artifacts (executables, or libraries) that are generated within the `src/dist/<debug|release>/bin` and `src/dist/<debug|release>/lib` folders respectively.


### Optional development dependencies
[OpenCV](https://opencv.org/) is an optional dependency you can install into your development environment in order to compile and run some extra examples that allow interactive visualization of images. Without `OpenCV` available in your system, these examples won't be compiled.

Make sure to:
1. Install OpenCV via `zypper`:
   ```sh
   $> sudo zypper install opencv-devel
   ```
2. Make sure the following command returns a valid path:
   ```sh
   $> sudo find / -name "OpenCVConfig.cmake"
   ```
   such as `/usr/share/OpenCV/OpenCVConfig.cmake`
   
   If this is found, when building the examples of the `irsol` project, the ones requiring `OpenCV` will automatically be built as well.
