#include "Server/Acceptor.h"

#include <cassert>

#include "Server/BuildAnswer.h"

namespace server {

bool TAcceptor::Init(uint32_t port) {

  // манёвр с shared_from_this, для того что бы объект гарантированно не был уничтожен
  // до вызова Stop из другого потока
  auto self = shared_from_this();
  m_indxInStopHandler = common::TStopHandler::Add([self] { self->Stop(); });

  if (!m_nlConnector.Bind(&port, nullptr))
    return false;
  
  return true;
}

void TAcceptor::Loop() {
  while (!m_stopFlag) {
    std::string msg;
    uint32_t port = m_nlConnector.Receive(msg);
    m_nlConnector.SetPeerPort(port);
    if (msg.empty() || m_stopFlag)
      continue;
    std::string answ = BuildAnswer(msg);
    m_nlConnector.Send(answ);
  }
}

void TAcceptor::Stop() {
  common::TStopHandler::Clear(m_indxInStopHandler);
  MyLog("server: stop command");
  m_stopFlag = true;
  m_nlConnector.Stop();
}


} // namespace server