#pragma once 

#include <filesystem>
#include <vector>

#include "Common/Common.h"

namespace client {

struct TConfigWorker {
  size_t countMessagesInGroup{ 1 }; // TODO(VF): не реализовано
  common::TDuration pauseBetweenMessages{ common::TDuration::zero() };
};

struct TConfig {
  size_t workersCount{ 1 }; // TODO(VF): не реализовано
  uint32_t serverPort{ 0 };
  std::string messagesFile = "Messages.json";
  std::string answerDir = "Answers";
};

} // namespace client