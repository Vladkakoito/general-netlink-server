#pragma once

#include <string>

#include "Common/SimpleLogger/SimpleLogger.h"
#include <Third-party/libnl-3.0/include/netlink/msg.h>
#include <Third-party/libnl-3.0/include/netlink/netlink.h>
#include "Third-party/libnl-3.0/include/netlink/socket.h"

namespace common {

namespace nl_wrapper {

// класс-обёртка над libnl для элементарного последовательного чтения-записи
class TNlWrapper {
public:
  // в конструктор отдаём префикс для логгера и стоп команду, по факту и потом можно будет
  // пользовать объектом (после стоп-команды), но по крайней мере не будет висеть на операции чтения
  TNlWrapper(std::string&& loggerPrefix, const std::string& stopCommand) 
    : m_kLoggerPrefix(std::move(loggerPrefix) + ": ")
    , m_kStopCommand(stopCommand) {}

  ~TNlWrapper() {
    nl_socket_free(m_socket);
  }

  TNlWrapper(const TNlWrapper&) = delete;
  const TNlWrapper& operator=(const TNlWrapper&) = delete;

  // прерывает ожидание сообщения
  void Stop();

  // после получения сообщения, отдаётся порт, который можно использовать для отправки ответа
  uint32_t Receive(std::string& msg);
  void SetPeerPort(uint32_t port);

  // биндим сокет
  bool Bind(uint32_t* localPort, uint32_t* peerPort);

  // отправка сообщения
  bool Send(const std::string& message);

private:
  // вывод в лог через этот префикс. по хорошему, 
  // дописать логгер и передавать этот префикс туда, но я уже не буду
  const std::string m_kLoggerPrefix;

  // команда остановки
  const std::string m_kStopCommand;

  // основной сокет
  nl_sock* m_socket;
};

inline void TNlWrapper::SetPeerPort(uint32_t port) {
  nl_socket_set_peer_port(m_socket, port);
}

} // namespace nl_wrapper

} // namespace common