#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "Common/NlWrapper/NlWrapper.h"
#include "Common/StopHandler.h"
#include "Server/Common.h"

namespace server {

// класс-сервер принимает сообщения, считает, отправляет ответы.
// по хорошему бы так и назвать, server, ну или receiver ) но уже не буду переименовывать
// наследуется от enable_shared_from_this для того что бы передать self в функцию остановки,
// и тогда будет гарантия неуничтожения объекта, пока жива та функция
class TAcceptor : public std::enable_shared_from_this<TAcceptor> {
public:
  static std::shared_ptr<TAcceptor> Make();

  // тут ничего особенного то и не делаем. m_nlConnector биндим, да функцию Stop назначаем
  bool Init(uint32_t port);

  // начало приёма сообщений
  void Start();

  // остановить приём сообщений, в т.ч. и отправить Stop nlConnector-у
  void Stop();

  // что бы была точка, где можно подождать основной поток сервиса
  void Join();
private:
  // приватный конструктор, что бы можно было создать класс только через Make,
  // иначе будет некорректная логика при использовании shared_from_this
  TAcceptor() = default;

  void Loop();

  // в этом потоке идёт основной процесс
  std::thread m_mainThread;

  // в Loop цикл наблюдает за этим флагом
  std::atomic<bool> m_stopFlag{ false };

  // индекс, что бы удалить себя из списка вызовов для остановки
  size_t m_indxInStopHandler{ 0 };

  // класс-обёртка над libnl
  common::nl_wrapper::TNlWrapper m_nlConnector{/*loggerPrefix=*/"Server", kStopCommand };
};

inline void TAcceptor::Start() {
  m_mainThread = std::thread(&TAcceptor::Loop, this);
  MyLog("server: started");
}

inline void TAcceptor::Join() {
  if (m_mainThread.joinable())
    m_mainThread.join();
}

// static
inline std::shared_ptr<TAcceptor> TAcceptor::Make() {
  return std::shared_ptr<TAcceptor>(new TAcceptor());
}

} // namespace server