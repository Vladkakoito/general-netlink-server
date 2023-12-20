#pragma once 

#include <chrono>

#include "Common/Common.h"
#include "Common/SimpleLogger/SimpleLogger.h"

namespace server {

constexpr common::TDuration kTimeoutConnection = std::chrono::seconds{ 15 };
constexpr size_t kCountMaxConnections = 100;

constexpr const char* kStopCommand = "stop";

} // namespace server