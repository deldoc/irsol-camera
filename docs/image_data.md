# Image Data format {#note_on_image_data}

## Overview

The IRSOL server transmits image data to clients using a custom binary protocol designed for efficiency and compatibility with scientific image processing tools (e.g., OpenCV). This protocol is implemented in the server's @ref irsol::protocol::Serializer and is used whenever an @ref irsol::protocol::ImageBinaryData message is sent.

This document describes:

- The structure of the data sent over the socket for an image.
- The meaning and purpose of special bytes and headers.
- The byte order and format of the pixel data (including byte-swapping).
- An example of the actual data sent.
- Pseudocode for robust client-side parsing and decoding.


## Protocol Structure

When the server sends an image to the client, the data is structured as follows:

```
img=<SOH>u16[height,width] [attributes...]<STX><image_data><ETX>
```

Where:

- `img=`: ASCII header indicating the start of an image message.
- `<SOH>`: Special byte (0x01, Start of Header) marking the beginning of the metadata section.
- `u16[height,width]`: ASCII metadata describing the image type (16-bit unsigned), and its shape.
- `[attributes...]`: Optional ASCII key-value pairs for additional metadata (e.g., imageId, timestamp).
- `<STX>`: Special byte (0x02, Start of Text) marking the start of the binary image data.
- `<image_data>`: Raw image bytes (see below for format and byte order).
- `<ETX>`: Special byte (0x03, End of Text) marking the end of the image data.

Each message is terminated by a newline (`\n`) after the `<ETX>` byte.

### Special Bytes

- **SOH** (`0x01`): Marks the start of the header/metadata section.
- **STX** (`0x02`): Marks the start of the binary image data.
- **ETX** (`0x03`): Marks the end of the binary image data.

These bytes are defined in @ref irsol::protocol::Serializer::SpecialBytes.


## Example

Suppose the server sends a 2D grayscale image of size 10x16 (height x width), with 16 bits per pixel.

The message sent over the socket will look like (in bytes):

```
69 6d 67 3d 01 75 31 36 5b 31 30 2c 31 36 5d 20 69 6d 61 67 65 49 64 3d 31 20 74 69 6d 65 73 74 61 6d 70 3d 7b 32 30 32 34 2d 30 34 2d 32 35 54 31 32 3a 33 34 3a 35 36 2e 37 38 39 7d 02 ... <image bytes> ... 03 0a
```

Breaking this down:

- `69 6d 67 3d` = "img="
- `01` = SOH
- `75 31 36 5b 31 30 2c 31 36 5d` = "u16[10,16]"
- `20 69 6d 61 67 65 49 64 3d 31` = " imageId=1"
- `20 74 69 6d 65 73 74 61 6d 70 3d 7b ... 7d` = " timestamp={...}"
- `02` = STX
- `<image bytes>` = 10*16*2 = 320 bytes of image data (see below)
- `03` = ETX
- `0a` = newline


## Image Data Format and Byte Order

- Each pixel is represented by **2 bytes** (16 bits).
- The server extracts image data from the camera in native order, but **before sending, it swaps the two bytes of each pixel** (see @ref irsol::camera::PixelByteSwapper<true>() usage in @ref irsol::protocol::Serializer::serializeImageBinaryData).
- This means that for each pixel, if the original bytes are `[LSB, MSB]`, the server sends `[MSB, LSB]`.

**Why byte-swapping?**  
This is done to ensure compatibility with existing client-side tools that expect a specific byte order for 16-bit images.


## Client-Side Decoding

To correctly decode the image, the client must:

1. **Parse the ASCII header** to extract image shape and attributes.
2. **Wait for the STX byte** to know when the binary image data starts.
3. **Read the correct number of bytes** (`height * width * 2`).
4. **Optionally Swap the bytes for each pixel** to restore the original order, if your application interface needs the 16-bit pixel data in `[LSB, MSB]`.
5. **Convert the buffer to an image matrix** (e.g., OpenCV `cv::Mat`).

### Pseudocode for Parsing and Decoding

```pseudocode
function receive_image(socket):
    # 1. Read until 'img=' header is found
    header = read_until(socket, '=')
    assert header == 'img'

    # 2. Wait for SOH (0x01)
    while true:
        byte = read_byte(socket)
        if byte == 0x01:
            break

    # 3. Parse metadata (e.g., u16[height,width])
    meta = read_ascii_until(socket, '[')
    height = parse_int(read_ascii_until(socket, ','))
    width = parse_int(read_ascii_until(socket, ']'))

    # 4. Skip attributes until STX (0x02)
    while true:
        byte = read_byte(socket)
        if byte == 0x02:
            break

    # 5. Read image data
    num_bytes = height * width * 2  // 16bit-depth
    image_data = read_exact(socket, num_bytes)

    # 6. Wait for ETX (0x03)
    etx = read_byte(socket)
    assert etx == 0x03

    # 7. (Optional) Read newline
    read_byte(socket)  # Should be '\n'

    # 8. Optionally wap bytes for each pixel if needed in `[LSB, MSB]` form
    for i in 0 .. num_bytes/2 - 1:
        swap(image_data[2*i], image_data[2*i+1])

    # 9. Convert to image (e.g., OpenCV)
    image = cv::Mat(height, width, CV_16UC1, image_data)
    # Optionally, rescale from 12-bit to 16-bit if needed, so that the max value of 12-bit representation is mapped to the max value of 16-bit representation

    return image
```

### Notes

- The actual implementation should handle errors and partial reads robustly.
- The byte-swapping step is **essential** for correct image reconstruction, whenever the client-application **expects** the image byte order for 16-bit-depth pixels to be `[LSB, MSB]`.
- The server may include additional attributes (e.g., imageId, timestamp) in the header, which can be parsed if needed.


## Reference: Server Serialization Logic

See @ref irsol::protocol::Serializer, especially:

```cpp
// Copy image data to the end of the payload buffer
size_t dataOffset = payload.size();
payload.resize(payload.size() + msg.data.size());
std::memcpy(&payload[dataOffset], msg.data.data(), msg.data.size());

// Swap bytes in-place for 16-bit data (assume always 16-bit)
irsol::camera::PixelByteSwapper<true>()(
    payload.begin() + static_cast<std::ptrdiff_t>(dataOffset), payload.end());
```


## Reference: Client Parsing Example

See @ref viewer_client_gi.cpp for a real-world implementation.

