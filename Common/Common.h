#pragma once

#include <chrono>

#define DEBUG
#define NLMSG_CALCULATING 0x666

namespace common {

using TTimeStamp = std::chrono::steady_clock::time_point;
using TDuration = TTimeStamp::duration;

} // namespace common