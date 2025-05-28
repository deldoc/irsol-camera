#Development Environment Configuration

This guide will lead you through the setup of a development environment that will allow you to edit, compile, and test new code.

## Repository Structure
This section presents the structure of the repository:
- [`docs`](../../docs/): Contains all documentation associated with this repository.
- [`src`](../../src/): Contains the source code for the project:
  - [`irsol`](../../src/irsol/): Core library for interacting with the cameras.
  - [`examples`](../../src/examples/): Example applications demonstrating how to use the `neoAPI` with the `irsol` library.
  - [`external`](../../src/external/): External dependencies, such as `neoAPI`.
    - [neoapi](../../src/external/neoapi/): Contains the library provided by the camera vendor for interacting with the cameras programmatically.
      - [headers](../../src/external/neoapi/include/): Contains the public headers for the `neoAPI` library.
      - [lib](../../src/external/neoapi/lib/): Contains the *pre-compiled* library for the `neoAPI` library.


### Building the source code
The source code is structured to be compiled via [`cmake`](https://cmake.org/). Ensure `cmake` is installed in your environment. For openSUSE, this can be done via:
```sh
$> sudo zypper refresh
$> sudo zypper install cmake
```

#### `cmake` Configuration
The [`CMakeLists.txt`](../../src/CMakeLists.txt) file at the root of the [`src`](../../src/) directory is the main configuration file for all the source code contained in this repository: it builds the `irsol` library, links against dependencies and also builds the examples.

Each specific example (e.g., [`getting_started`](../../src/examples/getting_started/)) folder contains its own `CMakeLists.txt` file, invoked by the root `CMakeLists.txt`. These files contain configuration and instructions for building the specific examples.

#### Building Steps
1. Navigate to the `src` folder:
   ```sh
   $> cd src/
   ```
2. Create a `build` directory and enter it:
   ```sh
   $> mkdir -p build/
   $> cd build/
   ```
3. Run `cmake`, pointing to the directory containing the root `CMakeLists.txt` file:
   ```sh
   $> cmake [-D CMAKE_BUILD_TYPE={Debug|Release} ] ..
   ```
4. Build the `irsol` library and the example executables:
   ```sh
   $> make [-j<N>]
   ```

Executables will be generated in the `src/build/<debug|release>/bin` directory.

#### Fast Build
A utility [`Makefile`](../../Makefile) is available to build all examples from the repository root:
```sh
$> make examples
```
or, for debug mode:
```sh
$> DEBUG=1 make examples
```

### Optional development dependencies
OpenCV is an optional dependency you can install into your development environment in order to compile and run some extra examples that allow interactive visualization of images.
In order to do so, make sure to:
1. Install OpenCV via `zypper`:
   ```sh
   $> sudo zypper install opencv-devel
   ```
2. Make sure the following command returns a valid path:
   ```sh
   $> sudo find / -name "OpenCVConfig.cmake"
   ```
   such as `/usr/share/OpenCV/OpenCVConfig.cmake`


## Linting
The [root Makefile](../../Makefile) contains a rule for linting the `C++` code in this repository using [clang-format](https://clang.llvm.org/docs/ClangFormat.html). To lint the source files:
```sh
$> make lint
```

**Note**: you might need to have to install `clang` in your development environment. For openSuse, this can be done via:
```sh
$> sudo zypper refresh
$> sudo zypper install llvm-clang
```