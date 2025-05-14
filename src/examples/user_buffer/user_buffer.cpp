/** \example user_buffer.cpp
    This example describes the use of buffers allocated by the user or other
   frameworks. The given source code applies to handle one camera and image
   acquisition. Copyright (c) by Baumer Optronic GmbH. All rights reserved,
   please see the provided license for full details.
*/

#include "irsol/logging.hpp"
#include "neoapi/neoapi.hpp"
#include <iostream>
#include <utility>
#include <vector>

class TestBuffer : public NeoAPI::BufferBase {
public:
  explicit TestBuffer(size_t size) : size_(size), mem_(new uint8_t[size_]) {
    if (mem_ == nullptr) {
      throw std::runtime_error("Failed to allocate memory");
    }
    RegisterMemory(mem_, size_);
  }

  ~TestBuffer() {
    UnregisterMemory();
    delete[] mem_;
  }
  size_t GetSize() { return size_; }
  uint8_t *GetMemory() { return mem_; }

private:
  const size_t size_;
  uint8_t *mem_;
};

struct Coordinates {
  size_t x;
  size_t y;
};

struct Resolution {
  size_t width;
  size_t height;

  Coordinates FromIndex(size_t index) const { return {index % width, index / width}; }
  size_t ToIndex(size_t x, size_t y) const { return y * width + x; }
  size_t ToIndex(const Coordinates &coords) const { return ToIndex(coords.x, coords.y); }
};

int main() {
  int result = 0;
  try {
    NeoAPI::Cam camera = NeoAPI::Cam();
    camera.Connect();
    const Resolution resolution = Resolution{static_cast<size_t>(camera.f().Width.Get()),
                                             static_cast<size_t>(camera.f().Height.Get())};
    IRSOL_LOG_INFO("Resolution: {0:d} x {1:d}", resolution.height, resolution.width);
    size_t payloadsize = static_cast<size_t>(camera.f().PayloadSize.Get());
    IRSOL_LOG_INFO("Payload size: {0:d}", payloadsize);
    std::vector<TestBuffer *> buffers;
    while (buffers.size() < 5) {
      IRSOL_LOG_INFO("Adding user buffer {0:d}", buffers.size());
      TestBuffer *buffer = new TestBuffer(payloadsize);
      camera.AddUserBuffer(buffer);
      buffers.push_back(buffer);
    }
    camera.f().ExposureTime.Set(10000);

    camera.SetUserBufferMode();
    NeoAPI::Image image = camera.GetImage();

    IRSOL_LOG_INFO("Image captured");

    image.Save("user_buffer.bmp");
    auto *buffer = image.GetUserBuffer<TestBuffer *>();

    // Log the non-zero pixels from the buffer
    std::vector<size_t> non_zero_pixels;
    for (size_t i = 0; i < resolution.height; ++i) {
      for (size_t j = 0; j < resolution.width; ++j) {
        Coordinates coords{j, i};
        size_t index = resolution.ToIndex(coords);
        if (buffer->GetMemory()[index] != 0) {
          IRSOL_LOG_INFO("Pixel at ({0:d}, {1:d}) is non-zero: {2:d}", i, j,
                         buffer->GetMemory()[index]);
          non_zero_pixels.push_back(index);
        }
      }
    }

    // Modify the image data in the buffer
    for (size_t non_zero_pixel : non_zero_pixels) {

      Coordinates coord = resolution.FromIndex(non_zero_pixel);
      IRSOL_LOG_INFO("Modifying pixel at ({0:d}, {1:d})", coord.x, coord.y);

      for (int dy = -10; dy <= 10; ++dy) {
        for (int dx = -20; dx <= 20; ++dx) {
          int64_t new_x = coord.x + dx;
          int64_t new_y = coord.y + dy;
          if (new_x <= 0 || new_y <= 0) {
            continue; // Skip out-of-bounds coordinates
          }

          size_t new_x_t = static_cast<size_t>(new_x);
          size_t new_y_t = static_cast<size_t>(new_y);

          if (new_x_t >= resolution.width || new_y_t >= resolution.height) {
            continue; // Skip out-of-bounds coordinates
          }
          size_t new_index = resolution.ToIndex(new_x_t, new_y_t);
          buffer->GetMemory()[new_index] = 255; // Set the surrounding pixels to white
        }
      }
    }

    // Save the modified image
    image.Save("user_buffer_modified.bmp");
    IRSOL_LOG_INFO("Modified image saved");

    while (buffers.size()) {
      TestBuffer *buffer = buffers.back();
      camera.RevokeUserBuffer(buffer);
      buffers.pop_back();
      delete buffer;
    }
  } catch (NeoAPI::NeoException &exc) {
    IRSOL_LOG_FATAL("error: {0}", exc.GetDescription());
    result = 1;
  } catch (...) {
    IRSOL_LOG_FATAL("oops, error");
    result = 1;
  }

  return result;
}
