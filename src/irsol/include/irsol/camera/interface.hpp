#pragma once

#include "irsol/assert.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"

#include <chrono>
#include <mutex>
#include <string>
#include <type_traits>

namespace irsol {

namespace camera {

/**
 * @class Interface
 * @brief High-level wrapper around the NeoAPI camera for synchronized access.
 *
 * Interface initializes the default camera device and provides thread-safe
 * methods to capture images and get/set camera features via templated helpers.
 */
class Interface
{
public:
  /**
   * @brief Constructs the Interface by loading the default camera.
   *
   * Internally calls utils::loadDefaultCamera(), which discovers available
   * devices and opens the one matching the default serial number.
   * Throws if the camera cannot be initialized.
   */
  Interface(NeoAPI::Cam cam = ::irsol::utils::loadDefaultCamera());

  Interface(Interface&& other);
  Interface& operator=(Interface&& other);

  /**
   * Factory method to create a camera interface with full resolution.
   */
  static Interface FullResolution();
  /**
   * Factory method to create a camera interface with half resolution.
   *
   * @note The resolution is halved in both dimension by performing camera-hardware binning
   * (averaged).
   */
  static Interface HalfResolution();

  /**
   * @brief Provides direct access to the underlying NeoAPI camera handle.
   *
   * @return A reference to the NeoAPI::Cam object.
   */
  NeoAPI::Cam& getNeoCam();

  /**
   * @brief Check if the camera is currently connected and valid.
   *
   * Uses NeoAPI::Cam::IsConnected(), allowing quick status polling.
   *
   * @return true if the camera is connected; false otherwise.
   */
  bool isConnected() const
  {
    return m_cam.IsConnected();
  }

  /**
   * @brief Retrieve a camera parameter of arbitrary type T.
   *
   * This templated getter fetches the camera feature via NeoAPI,
   * and returns it as type T. Supported types are:
   * - std::string or char *: returns a string value
   * - bool: boolean feature
   * - integral types: integer feature
   * - floating-point types: double feature
   *
   * In case of errors, returns a default-constructed T
   * or "Unknown" for string types.
   *
   * @tparam T Type of the parameter to retrieve.
   * @param param Name of the camera feature.
   * @return The feature value as type T, or fallback on error.
   */
  template<
    typename T,
    std::enable_if_t<
      std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<std::string, T>,
      int> = 0>
  T getParam(const std::string& param) const;

  /**
   * @brief Convenience overload to always return a std::string parameter.
   *
   * Logs, retrieves the feature, and converts to std::string. On failure,
   * returns "Unknown".
   *
   * @param param Name of the camera feature.
   * @return Feature value as std::string or "Unknown" on error.
   */
  std::string getParam(const std::string& param) const;

  /**
   * @brief Set a camera parameter of arbitrary type T.
   *
   * Thread-safe: takes a lock across set operations. Supported types:
   * - std::string or const char *
   * - bool
   * - integral types (cast to int64_t)
   * - floating-point types (cast to double)
   *
   * @tparam T Type of the value to set.
   * @param param Name of the camera feature.
   * @param value Value to set for the feature.
   */
  template<
    typename T,
    std::enable_if_t<
      std::is_integral_v<std::decay_t<T>> || std::is_floating_point_v<std::decay_t<T>> ||
        std::is_same_v<std::decay_t<T>, std::string> ||
        std::is_same_v<std::decay_t<T>, const char*>,
      int> = 0>
  void setParam(const std::string& param, T value);

  /**
   * @brief Capture a single image from the camera, blocking up to timeout.
   *
   * Thread-safe: locks m_imageMutex to serialize image grabs. Returns a
   * NeoAPI::Image containing the raw frame data. Throws on timeout or error.
   *
   * @param timeout Maximum duration to wait for a new image (default 400 ms).
   * @return NeoAPI::Image The captured image buffer and metadata.
   */
  NeoAPI::Image captureImage(std::chrono::milliseconds timeout = std::chrono::milliseconds(400));

private:
  /// Protects concurrent access to image capture operations.
  mutable std::mutex m_imageMutex;

  /// Protects concurrent setParam operations.
  mutable std::mutex m_paramMutex;

  /// Underlying NeoAPI camera handle.
  NeoAPI::Cam m_cam;
};
}  // namespace camera
}  // namespace irsol

#include "irsol/camera/interface.tpp"
