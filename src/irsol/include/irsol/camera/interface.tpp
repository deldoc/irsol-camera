#pragma once

#include "irsol/logging.hpp"

#include <type_traits>

namespace irsol {
namespace camera {

template <typename T> T Interface::getParam(const std::string &param) const {
  IRSOL_LOG_DEBUG("Getting parameter '{}'", param);
  try {
    NeoAPI::NeoString neoParam(param.c_str());
    auto feature = m_cam.GetFeature(neoParam);
    if constexpr (std::is_same_v<std::decay_t<T>, std::string> ||
                  std::is_same_v<std::decay_t<T>, char *>) {
      return feature.GetString();
    }
    if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
      return feature.GetBool();
    }
    if constexpr (std::is_integral_v<std::decay_t<T>>) {
      return feature.GetInt();
    }
    if constexpr (std::is_floating_point_v<std::decay_t<T>>) {
      return feature.GetDouble();
    } else {
      static_assert(sizeof(T) == 0, "Unsupported parameter type for getParam");
    }
  } catch (const std::exception &e) {
    IRSOL_LOG_ERROR("Failed to get parameter '{}': {}", param, e.what());
    if constexpr (std::is_same_v<std::decay_t<T>, std::string> ||
                  std::is_same_v<std::decay_t<T>, char *>) {
      return "Unknown";
    } else {
      return T{};
    }
  }
}

template <typename T> void Interface::setParam(const std::string &param, T value) {
  std::lock_guard<std::mutex> lock(m_paramMutex);
  IRSOL_LOG_DEBUG("Setting parameter '{}' to value '{}'", param, value);
  try {
    if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), NeoAPI::NeoString(value.c_str()));
    } else if constexpr (std::is_same_v<std::decay_t<T>, const char *>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), NeoAPI::NeoString(value));
    } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), value);
    } else if constexpr (std::is_integral_v<std::decay_t<T>>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), static_cast<int64_t>(value));
    } else if constexpr (std::is_floating_point_v<std::decay_t<T>>) {
      m_cam.SetFeature(NeoAPI::NeoString(param.c_str()), static_cast<double>(value));
    } else {
      static_assert(sizeof(T) == 0, "Unsupported parameter type for setParam");
    }
  } catch (const std::exception &e) {
    IRSOL_LOG_ERROR("Failed to set parameter '{}': {}", param, e.what());
  }
}
} // namespace camera
} // namespace irsol