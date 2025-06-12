# IRSOL Baumer Camera Interface

## Origin

This codebase was developed during a 1-month Swiss civil service assignment ("Zivildienst"/"Servizio Civile") at IRSOL. In Switzerland, civil service is an alternative to military service where citizens contribute to public institutions. This work is therefore the result of a public service contribution rather than a private or professional commercial project.

## Documentation

Detailed API documentation is available:

- Online: [https://cdeldon.github.io/irsol-camera/](https://cdeldon.github.io/irsol-camera/)
- Local: Generate documentation with `make docs` and open `docs/generated/html/index.html`

A C++ implementation for interfacing with Baumer cameras, primarily designed for the Istituto Ricerche Solari Locarno (IRSOL) observatory applications.

![IRSOL Logo](docs/assets/irsol_logo.jpeg)

## Overview

This library provides a robust, high-performance interface for controlling Baumer cameras and processing image data. It implements a socket server that handles communication between client applications and the camera hardware, enabling remote operation and data acquisition.

## Key Features

- **Camera Control**: Complete API for controlling Baumer cameras
- **Socket Server**: Remote camera operation via network interface
- **Image Processing**: Tools for handling image data
- **Logging & Assertions**: Flexible logging and error handling system
- **Cross-platform**: Support for major operating systems

## Installation

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 19.14+)
- CMake 3.14 or higher
- Baumer SDK

### Build Instructions

1. Clone the repository:

```bash
git clone https://github.com/cdeldon/irsol-camera.git
cd irsol-camera
```

2. Build the project using the convenience [Makefile](Makefile):

```bash
make build
```

3. Optionally, run tests:

```bash
make test
```

## Usage Examples

The project comes with several examples demonstrating various features. Check the example in [`src/examples/`](src/examples/README.md) for details on how to use the `irsol-camera` library.
