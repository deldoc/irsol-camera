# 02-interacting-with-camera-features {#interacting_with_camera_features}

This example serves as as a playground for understanding how to control the parameters of the _Baumer_ camera via the `irsol::camera::Interface` class.


## Camera Parameter Interface

The `irsol::camera::Interface` class provides a convenient abstraction over `NeoAPI`'s camera control system. It simplifies getting and setting camera parameters while ensuring thread safety and proper type conversion. This section explains the design and usage of its parameter management functions.

### `getParam`

`getParam` is a templated method used to retrieve the value of a camera feature by name. It is type-safe and automatically deduces the correct underlying `NeoAPI` getter based on the requested C++ type.
```cpp
template<typename T>
T Interface::getParam(const std::string& param) const;
```

Supported types:
  * `int`, `int64_t` – for integer-based parameters
  * `double`, `float` – for floating-point parameters
  * `std::string` – for string-valued parameters
  * `bool` – for boolean switches

Example usage:
```cpp
  int width = interface.getParam<int>("Width");
  std::string pixelFormat = interface.getParam<std::string>("PixelFormat");
```

If a parameter fetch fails, the function logs the error and returns a default-constructed value (e.g., 0 for integers, "Unknown" for strings).

### `setParam`

`setParam` is a templated, thread-safe setter that assigns a value to a camera feature, then reads back and returns the applied value to confirm the update.
```cpp
template<typename T>
T Interface::setParam(const std::string& param, T value);
```

This method ensures:
 * Type safety: Automatically chooses the correct `NeoAPI` setter.
 * Thread safety: Uses a mutex lock to protect access to the shared camera handle.
 * Error handling: Logs detailed errors and verifies write capability.

Example usage:
```cpp
interface.setParam("ExposureAuto", "Off");
interface.setParam("Width", 2048);
```

#### Note on `setParam` behavior
When using the `setParam` methods to configure camera parameters, it's important to understand that the value ultimately applied by the camera may differ slightly from the one provided. This is because some camera features accept only specific discrete values, or impose constraints such as value ranges or fixed precision steps. For example, setting an exposure time of `1037µs` may result in the camera applying `1040µs` if that's the closest supported value. To ensure the caller is aware of the actual value in effect, all `setParam` overloads return the final value that was accepted by the camera after attempting the update.

```cpp
// Attempt to set exposure time to 1037 microseconds
auto requestedExposure = std::chrono::microseconds(1037);
auto appliedExposure = cam.setExposure(requestedExposure);

IRSOL_LOG_INFO("Requested exposure: {}", irsol::utils::durationToString(requestedExposure));
IRSOL_LOG_INFO("Applied exposure: {}", irsol::utils::durationToString(appliedExposure));

// Output might be:
// Requested exposure: 1037 µs
// Applied exposure: 1040 µs
```


### `getExposure` and `setExposure`

Exposure is a time-based parameter, and this abstraction uses `irsol::types::duration_t` for safe and expressive exposure control.
```cpp
irsol::types::duration_t getExposure() const;
irsol::types::duration_t setExposure(irsol::types::duration_t exposure);
```
  * `getExposure()` reads the `"ExposureTime"` feature (in microseconds) and returns it as a `irsol::types::duration_t` type.
  * `setExposure()` sets the exposure time.
  
One should alway prefer to call these methods rather than relying on the raw `setParam` and `getParam` methods.

Example usage:

```cpp
auto exposure = interface.getExposure();
interface.setExposure(std::chrono::milliseconds(15));
```

This approach provides type safety and unit clarity when managing exposure values.

### `setMultiParam`

To optimize configuration and avoid repetitive locking and logging, `setMultiParam` allows batch updates to multiple parameters:
```cpp
void setMultiParam(const std::unordered_map<std::string, camera_param_t>& params);
```
 * `camera_param_t` is a variant supporting multiple value types (`int`, `double`, `std::string`, etc.).
 * This function acquires the mutex once, then updates all parameters in sequence.
 * It's useful during initialization, resolution setup, or mode switching.

Example:
```cpp
interface.setMultiParam({
  {"BinningVertical", 2},
  {"BinningVerticalMode", "Average"},
  {"ExposureAuto", "Off"},
  {"ExposureMode", "Timed"}
});
```

Using `setMultiParam` ensures consistency and performance in situations where several related parameters must be updated simultaneously.