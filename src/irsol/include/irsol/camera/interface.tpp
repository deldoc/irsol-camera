#pragma once

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"

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
    NeoAPI::NeoString neoParam(param.c_str());
    auto              feature = m_cam.GetFeature(neoParam);

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
      std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, const char*>,
    int> = 0>
void
Interface::setParam(const std::string& param, T value)
{
  std::lock_guard<std::mutex> lock(m_paramMutex);
  IRSOL_LOG_DEBUG("Setting parameter '{}' to value '{}'", param, value);
  try {
    if constexpr(std::is_same_v<std::decay_t<T>, std::string>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), NeoAPI::NeoString(value.c_str()));
    } else if constexpr(std::is_same_v<std::decay_t<T>, const char*>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), NeoAPI::NeoString(value));
    } else if constexpr(std::is_same_v<std::decay_t<T>, bool>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), value);
    } else if constexpr(std::is_integral_v<std::decay_t<T>>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), static_cast<int64_t>(value));
    } else if constexpr(std::is_floating_point_v<std::decay_t<T>>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), static_cast<double>(value));
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Interface::setParam()");
    }
  } catch(const std::exception& e) {
    IRSOL_LOG_ERROR("Failed to set parameter '{}': {}", param, e.what());
  }
}

}  // namespace camera
}  // namespace irsol
