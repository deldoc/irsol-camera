#pragma once

#include "irsol/logging.hpp"
#include "irsol/macros.hpp"

#include <mutex>
#include <type_traits>

namespace irsol {
namespace camera {

template<
  typename T,
  std::enable_if_t<
    std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<std::string, T>,
    int>>
T
Interface::getParam(const std::string& param) const
{
  using U = std::decay_t<T>;
  IRSOL_LOG_DEBUG("Getting parameter '{}'", param);
  try {
    std::lock_guard<std::mutex> lock(m_camMutex);
    NeoAPI::NeoString           neoParam(param.c_str());
    auto                        feature = m_cam.GetFeature(neoParam);

    if constexpr(std::is_same_v<U, std::string>) {
      return std::string(feature.GetString());
    } else if constexpr(std::is_same_v<U, bool>) {
      return feature.GetBool();
    } else if constexpr(std::is_integral_v<U>) {
      return static_cast<T>(feature.GetInt());
    } else if constexpr(std::is_floating_point_v<U>) {
      return static_cast<T>(feature.GetDouble());
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Interface::getParam()");
    }
  } catch(const std::exception& e) {
    IRSOL_LOG_ERROR("Failed to get parameter '{}': {}", param, e.what());
    if constexpr(std::is_same_v<U, std::string>) {
      return "Unknown";
    } else {
      return T{};
    }
  }
}

template<
  typename T,
  std::enable_if_t<
    std::is_integral_v<std::decay_t<T>> || std::is_floating_point_v<std::decay_t<T>> ||
      std::is_same_v<std::decay_t<T>, std::string>,
    int> = 0>
T
Interface::setParam(const std::string& param, T value)
{
  IRSOL_LOG_DEBUG("Setting parameter '{}' to value '{}'", param, value);
  {
    std::lock_guard<std::mutex> lock(m_camMutex);
    setParamNonThreadSafe(param, value);
  }
  return getParam<std::decay_t<T>>(param);
}

template<typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, const char*>, int> = 0>
std::string
Interface::setParam(const std::string& param, T value)
{
  return setParam<std::string>(param, std::string(value));
}

template<
  typename T,
  std::enable_if_t<
    std::is_integral_v<std::decay_t<T>> || std::is_floating_point_v<std::decay_t<T>> ||
      std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, const char*>,
    int> = 0>
void
Interface::setParamNonThreadSafe(const std::string& param, T value)
{
  using U = std::decay_t<T>;
  try {
    NeoAPI::NeoString neoParam(param.c_str());
    auto              feature = m_cam.GetFeature(neoParam);

    // Make sure the feature is writable
    if(!feature.IsWritable()) {
      IRSOL_LOG_ERROR("Feature '{}' is not writable", param);
      throw std::runtime_error(std::string("Feature '") + param + "' is not writable");
    }

    if constexpr(std::is_same_v<U, std::string>)
      feature.SetString(NeoAPI::NeoString(value.c_str()));
    else if constexpr(std::is_same_v<U, const char*>) {
      feature.SetString(NeoAPI::NeoString(value));
    } else if constexpr(std::is_same_v<U, bool>) {
      feature.SetBool(value);
    } else if constexpr(std::is_integral_v<U>) {
      feature.SetInt(value);
    } else if constexpr(std::is_floating_point_v<U>) {
      feature.SetDouble(value);
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Interface::setParamNonThreadSafe()");
    }

  } catch(const std::exception& e) {
    IRSOL_LOG_ERROR("Failed to set parameter '{}': {}", param, e.what());
  }
}

}  // namespace camera
}  // namespace irsol
