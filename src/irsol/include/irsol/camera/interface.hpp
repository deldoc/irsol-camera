/**
 * @file irsol/camera/interface.hpp
 * @brief High-level wrapper around `NeoAPI` camera control for the irsol library.
 *
 * This header defines the `irsol::camera::Interface` class, a thread-safe abstraction
 * around NeoAPI camera control. It handles camera discovery, configuration, image acquisition,
 * and feature parameter access in a synchronized manner.
 *
 * See @ref loading_images for an example.
 */

#pragma once

#include "irsol/assert.hpp"
#include "irsol/types.hpp"
#include "irsol/utils.hpp"

#include <chrono>
#include <mutex>
#include <neoapi/neoapi.hpp>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace irsol {

/**
 * @namespace irsol::camera
 * @brief Provides a high-level interface to interact with camera devices using `NeoAPI`.
 *
 * The `irsol::camera` namespace contains components for accessing and configuring industrial
 * cameras in a thread-safe and convenient manner. It abstracts away the low-level `NeoAPI`
 * interface and offers simple, strongly-typed methods to control camera parameters and
 * acquire images.
 *
 * Key functionality includes:
 * - Connecting to and configuring the default camera device.
 * - Getting and setting camera parameters (exposure, resolution, etc.).
 * - Capturing image frames with timeout support.
 * - Ensuring thread-safe access to camera operations.
 *
 * This namespace is designed to be extendable and integrates closely with the rest of the
 * `irsol` framework.
 */
namespace camera {

/**
 * @class Interface
 * @brief High-level wrapper around the NeoAPI camera for synchronized access.
 *
 * The `Interface` class provides thread-safe control over camera acquisition and configuration
 * via NeoAPI. It abstracts common camera operations such as setting exposure, retrieving and
 * setting camera parameters, capturing images, and managing sensor state.
 */
class Interface
{
public:
  /// Alias for the image type returned by the NeoAPI.
  using image_t = NeoAPI::Image;

  /// Union of all supported types for camera parameters.
  using camera_param_t = std::variant<bool, int, int64_t, double, std::string, const char*>;

  /// Default exposure time (2 milliseconds) used to initialize the camera.
  static constexpr irsol::types::duration_t DEFAULT_EXPOSURE_TIME = std::chrono::milliseconds(2);

  /**
   * @brief Constructs the Interface by loading the default camera.
   *
   * Initializes the camera by calling @ref irsol::utils::loadDefaultCamera(), which automatically
   * finds and opens the default device based on serial number.
   *
   * @param cam Optional camera handle. Defaults to the result of @ref
   * irsol::utils::loadDefaultCamera().
   * @throws std::runtime_error if camera initialization fails.
   */
  Interface(NeoAPI::Cam cam = irsol::utils::loadDefaultCamera());

  /// Move constructor.
  Interface(Interface&& other);

  /// Move assignment operator.
  Interface& operator=(Interface&& other);

  /**
   * @brief Factory method to create a camera interface using full sensor resolution.
   * @return Interface instance initialized at full resolution.
   */
  static Interface FullResolution();

  /**
   * @brief Factory method to create a camera interface using half sensor resolution.
   *
   * Uses hardware binning to reduce resolution by averaging pixels.
   *
   * @return Interface instance initialized at half resolution.
   */
  static Interface HalfResolution();

  /**
   * @brief Get human-readable camera information.
   *
   * Collects fields from `NeoAPI::CamInfo` and formats them into a readable tabular string.
   *
   * @return Formatted string with camera model, serial, etc.
   */
  std::string cameraInfoAsString() const;

  /**
   * @brief Get current camera status.
   *
   * Reads camera state such as resolution, exposure, and other operational flags
   * and returns a descriptive tabular string.
   *
   * @return Formatted string describing current state of the camera.
   */
  std::string cameraStatusAsString() const;

  /**
   * @brief Access the underlying NeoAPI camera instance.
   * @return Reference to the underlying `NeoAPI::Cam` object.
   */
  NeoAPI::Cam& getNeoCam();

  /**
   * @brief Check if the camera is currently connected.
   *
   * Uses `NeoAPI::Cam::IsConnected()` internally.
   *
   * @return true if camera is connected; false otherwise.
   */
  bool isConnected() const
  {
    return m_cam.IsConnected();
  }

  /**
   * @brief Reset the sensor area to the full sensor dimensions.
   *
   * Useful if a previous program limited the sensor region and full dimensions need to be restored.
   */
  void resetSensorArea();

  /**
   * @brief Get the current exposure time from the camera.
   *
   * @return Current exposure duration.
   */
  irsol::types::duration_t getExposure() const;

  /**
   * @brief Set the exposure time of the camera.
   *
   * @param exposure New exposure duration.
   * @return The actual exposure set on the camera (may differ slightly).
   */
  irsol::types::duration_t setExposure(irsol::types::duration_t exposure);

  /**
   * @brief Retrieve a camera parameter of arbitrary type T.
   *
   * Uses the NeoAPI API to retrieve the value and cast to T. Supported types:
   * - std::string
   * - bool
   * - integral types (e.g., int, int64_t)
   * - floating-point types (e.g., double)
   *
   * Thread-safe: locks internal mutex.
   *
   * @tparam T Desired return type.
   * @param param Name of the camera parameter.
   * @return Value of type T or a default on read error.
   */
  template<
    typename T,
    std::enable_if_t<
      std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<std::string, T>,
      int> = 0>
  T getParam(const std::string& param) const;

  /**
   * @brief Retrieve a camera parameter and convert it to string.
   *
   * Thread-safe: locks internal mutex.
   *
   * @param param Name of the camera parameter.
   * @return Value as string or "Unknown" on error.
   */
  std::string getParam(const std::string& param) const;

  /**
   * @brief Set a camera parameter of arbitrary type T.
   *
   * Thread-safe. Supported types:
   * - std::string
   * - const char*
   * - bool
   * - integral types (converted to int64_t)
   * - floating-point types (converted to double)
   *
   * @tparam T Value type.
   * @param param Parameter name.
   * @param value Value to set.
   * @return The set value (after conversion/rounding, if any).
   * @see Interface::setMultiParam()
   */
  template<
    typename T,
    std::enable_if_t<
      std::is_integral_v<std::decay_t<T>> || std::is_floating_point_v<std::decay_t<T>> ||
        std::is_same_v<std::decay_t<T>, std::string>,
      int> = 0>
  T setParam(const std::string& param, T value);

  /**
   * @brief Specialization for setting const char* values as strings.
   *
   * @param param Parameter name.
   * @param value C-string value.
   * @return Set value as std::string.
   * @see Interface::setMultiParam()
   */
  template<typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, const char*>, int> = 0>
  std::string setParam(const std::string& param, T value);

  /**
   * @brief Set multiple parameters in one call.
   *
   * Iterates through a map of key-value pairs and sets each camera parameter
   * accordingly. Internally dispatches based on type.
   *
   * @param params Map of parameter names and values.
   * @see Interface::setParam()
   */
  void setMultiParam(const std::unordered_map<std::string, camera_param_t>& params);

  /**
   * @brief Trigger a camera feature (e.g., software trigger).
   *
   * @param param Name of the triggerable feature.
   */
  void trigger(const std::string& param);

  /**
   * @brief Capture a single image from the camera.
   *
   * Waits up to a specified timeout (or the cached exposure time if not provided as argument).
   * Thread-safe.
   *
   * @param timeout Optional duration to wait for a frame.
   * @return Captured image.
   * @throws std::runtime_error on timeout or camera error.
   */
  image_t captureImage(std::optional<irsol::types::duration_t> timeout = std::nullopt);

private:
  /// Mutex to protect access to camera parameters and image acquisition.
  mutable std::mutex m_camMutex;

  /// Internal camera instance from NeoAPI.
  NeoAPI::Cam m_cam;

  /// Cached exposure value used for implicit timeout behavior in @ref
  /// irsol::camera::Interface::captureImage().
  irsol::types::duration_t m_CachedExposureTime;

  /**
   * @brief Internal, non-thread-safe parameter setter used by `setParam`.
   *
   * @tparam T Parameter value type.
   * @param param Name of the parameter.
   * @param value Value to assign.
   */
  template<
    typename T,
    std::enable_if_t<
      std::is_integral_v<std::decay_t<T>> || std::is_floating_point_v<std::decay_t<T>> ||
        std::is_same_v<std::decay_t<T>, std::string> ||
        std::is_same_v<std::decay_t<T>, const char*>,
      int> = 0>
  void setParamNonThreadSafe(const std::string& param, T value);
};

}  // namespace camera
}  // namespace irsol

#include "irsol/camera/interface.tpp"
