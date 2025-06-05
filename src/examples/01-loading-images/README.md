# 01-loading-images {#loading_images}

@see examples/01-loading-images/main.cpp
@see examples/01-loading-images/main-opencv.cpp

This example serves as a minimal, practical introduction to the `irsol` codebase. It demonstrates how to initialize the system, configure logging, connect to a camera, and capture a series of images.

> **Goal:** Provide a clear, working entrypoint to help new developers and users understand how to integrate with core `irsol` components.


## Overview

@see examples/01-loading-images/main.cpp showcases the following core features of the `irsol` library:

- **Command-line argument parsing**  
  Configure the program at runtime (log level, number of frames).

- **Logging system initialization**  
  Logs are stored in the `logs/` directory and named based on the program.

- **Camera connection and capture**  
  Uses `irsol::camera::Interface` to acquire images and log capture metadata.

- **Performance metrics**  
  Outputs estimated FPS and image size info to give insight into runtime performance.


@see examples/01-loading-images/main-opencv.cpp implements the same example as @see examples/01-loading-images/main.cpp, but additionally displays the image to the screen. This example is only available on systems having an installed version of OpenCV.