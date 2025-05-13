/** \example getting_started.cpp
    This example describes the FIRST STEPS of handling Cam SDK.
    The given source code applies to handle one camera and image acquisition
    Copyright (c) by Baumer Optronic GmbH. All rights reserved, please see the provided license for full details.
*/

#include <iostream>
#include <thread>
#include <chrono>
#include "neoapi/neoapi.hpp"
#include "spdlog/spdlog.h"

void run_example() {

    NeoAPI::Cam camera = NeoAPI::Cam();
    camera.Connect();
    camera.f().ExposureTime.Set(10000);

    for (int i = 0; i < 10; i++) {
        NeoAPI::Image image = camera.GetImage();
        spdlog::debug("Writing image {0:d}", i);
        const NeoAPI::NeoString image_name = NeoAPI::NeoString(("getting_started_" + std::to_string(i) + ".bmp").c_str());
        image.Save(image_name);
        spdlog::info("Image {0:s} saved", image_name.c_str());
        // sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
        
}
int main() {
    int result = 0;
    try {
        run_example();
    }
    catch (NeoAPI::NeoException& exc) {
        spdlog::error("error:  {0:s}", exc.GetDescription());
        result = 1;
    }
    catch (...) {
        spdlog::error("error:  unknown exception");
        result = 1;
    }

    return result;
}
