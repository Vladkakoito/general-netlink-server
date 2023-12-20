#include "Common/NlWrapper/NlWrapper.h"

#include <cassert>

#include "Common/Common.h"

namespace common {

namespace nl_wrapper {

bool TNlWrapper::Bind(uint32_t* localPort, uint32_t* peerPort) {
  if (!localPort && !peerPort) {
    MyErr(m_kLoggerPrefix + "set one or both ports");
    return false;
  }

  m_socket = nl_socket_alloc();
  if (!m_socket) {
    MyErr(m_kLoggerPrefix + "initialization error: socket allocation failed");
    return false;
  }

  // можно биндить только один сокет - это на совести использующего класс
  if (localPort)
    nl_socket_set_local_port(m_socket, *localPort);
  if (peerPort)
    nl_socket_set_peer_port(m_socket, *peerPort);

  int ret = nl_connect(m_socket, NETLINK_ROUTE);
  if (ret != 0) {
    MyErr(m_kLoggerPrefix +"initialization error: socket binding failed: " + nl_geterror(ret));
    return false;
  }
  MyLog(m_kLoggerPrefix + "initialized");
  return true;
}

bool TNlWrapper::Send(const std::string& message) {
  nl_msg* msg = nlmsg_alloc_size(sizeof(nlmsghdr) + message.size());
  if (!msg) {
    MyErr(m_kLoggerPrefix + "to send msg allocation failed");
    return false;
  }
  int ret =
    nl_send_simple(m_socket, NL_AUTO_PID, NL_AUTO_SEQ, 
                   const_cast<char*>(message.data()), message.size());
  nlmsg_free(msg);
  if (ret < 0) {
    MyErr(m_kLoggerPrefix + "error send message: " + nl_geterror(ret));
    return false;
  }
  return true;
}

uint32_t TNlWrapper::Receive(std::string& msg) {
  sockaddr_nl client;
  unsigned char* payload{ nullptr };
  int ret = nl_recv(m_socket, &client, &payload, nullptr);
  if (ret <= 0) {
    MyErr(m_kLoggerPrefix + "error reading message: " + nl_geterror(ret));
    return {};
  }
  size_t readed = static_cast<size_t>(ret);

#ifdef DEBUG
  assert(readed >= sizeof(nlmsghdr));
#endif // DEBUG

  msg = std::string(readed - sizeof(nlmsghdr), '\0');
  memcpy(msg.data(), payload + sizeof(nlmsghdr), msg.size());
  free(payload);

  // оставлю это для демонстрации в консоли
  MyLog(m_kLoggerPrefix + "readed " + msg + ". size " + std::to_string(msg.size()));

  // что бы можно было остановить всё это, есть возможность отправить стоп-команду
  if (m_kStopCommand == msg) {
    return {};
  }
  return client.nl_pid;
}

void TNlWrapper::Stop() {
  // сообщим, если не получается отправить стоп-команду
  const std::string kErrMsg = m_kLoggerPrefix + 
    "the listening cannot be interrupted. kill this PID, or " + 
    "send " + m_kStopCommand + " to the port from the server settings. ";

  // создаём вспомогательный сокет, и с него шлём на основной стоп команду
  nl_sock* suppSocket = nl_socket_alloc();
  if (!suppSocket) {
    MyErr(kErrMsg);
    return;
  }
  nl_socket_set_peer_port(suppSocket, nl_socket_get_local_port(m_socket));
  int ret = nl_connect(suppSocket, NETLINK_ROUTE);
  if (ret != 0) {
    MyErr(kErrMsg);
    nl_socket_free(suppSocket);
    return;
  }
  nl_send_simple(suppSocket, NL_AUTO_PID, NL_AUTO_SEQ,
                 const_cast<char*>(m_kStopCommand.data()), m_kStopCommand.size());
  nl_socket_free(suppSocket);
}

} // nl_wrapper

} // namespace common