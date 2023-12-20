#include "Client/Worker.h"

#include <algorithm>
#include <cassert>
#include <unistd.h>
#include <thread>

namespace client {

std::vector<std::string> TWorker::Start(const std::vector<std::string>& messages,
                                        uint32_t port) {
  if (!m_nlConnector.Bind(/*localPort=*/nullptr, /*peerPort=*/&port))
    return {};
  std::vector<std::string> result;
  for (const std::string& msg : messages) {
    if (m_stopFlag)
      break;
    // вставка элемента в любом случае - что бы номера в ответе соответстовали исходным
    result.push_back({});
    std::this_thread::sleep_for(m_config.pauseBetweenMessages);
    if (!m_nlConnector.Send(msg))
      continue;
    m_nlConnector.Receive(result.back());
  }
  return result;
}

} // namespace client