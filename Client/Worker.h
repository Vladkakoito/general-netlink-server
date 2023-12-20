#pragma once

#include <atomic>
#include <filesystem>
#include <string>
#include <vector>

#include "Common/Common.h"
#include "Common/NlWrapper/NlWrapper.h"
#include "Client/Common.h"
#include "Client/Config.h"

namespace client {

// класс пересылает коллекцию сообщений серверу
class TWorker {
public:
  TWorker(const TConfigWorker& config, size_t num)
    : m_config(config)
    , m_nlConnector("Worker_" + std::to_string(num), kStopCommand) {}

  // тут даём набор ответов на набор вопросов
  std::vector<std::string> Start(const std::vector<std::string>& rawMessages, uint32_t port);

  void Stop();
private:

  // настройки из .ini
  const TConfigWorker& m_config;

  // объект - посредник между программой и libnl
  common::nl_wrapper::TNlWrapper m_nlConnector;

  // возможно, стоит сделать ссылкой на флаг контроллера - тогда не надо каждый атомик дёргать в 
  // Stop. но думаю, небольшой оверхед, зато есть возможность останавливать каждый воркер отдельно
  std::atomic<bool> m_stopFlag{ false };
};

inline void TWorker::Stop() {
  m_stopFlag = true;
  m_nlConnector.Stop();
}

} // namespace client