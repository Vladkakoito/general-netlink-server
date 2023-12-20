#include "Client/Controller.h"

#include <thread>

#include "Client/Worker.h"
#include "Third-party/SimpleJson/json.hpp"

namespace client {

bool TController::Init() {

  // shared_from_this - что бы не уничтожился объект, 
  // пока вызвана/может быть вызвана функция остановки
  auto self = shared_from_this();
  m_indxStopHandler = common::TStopHandler::Add([self] { self->Stop(); });

  // читаем сообщения из файла
  std::ifstream ifs(m_config.messagesFile);
  if (ifs.fail()) {
    MyErr("Client: File with messages(\"" + m_config.messagesFile + "\") opening failed");
    return false;
  }

  // директория для вывода - задумывалось много клиентов, и каждый с номером, 
  // но не доделал
  m_outPath = std::filesystem::path(m_config.answerDir);
  if (!m_outPath.empty() && !std::filesystem::exists(m_outPath)) {
    std::error_code ec;
    std::filesystem::create_directories(m_outPath, ec);
    if (!ec)
      std::filesystem::permissions(m_outPath, std::filesystem::perms::all, ec);
    if (ec) {
      MyErr("Client: Directory to answers creating error: " + ec.message());
      return false;
    }
  }

  // чтение файла в строку, потом парсинг json. 
  // лучше бы библиотека умела сразу с потока принимать данные
  std::ostringstream oss;
  oss << ifs.rdbuf();
  std::string rawJson(oss.str());
  json::JSON json = json::JSON::Load(rawJson);
  if (json.JSONType() != json::JSON::Class::Array) {
    MyErr("Client: incorrect json structure in messages file");
    return false;
  }

  // TODO(VF): здесь вообще, наверное, нужен кастомный парсинг - 
  // не нужно разбирать json глубже корневого массива
  for (const auto& msg : json.ArrayRange()) {
    m_messages.emplace_back(msg.dump());
  }

  MyLog("Client: initialized");
  return true;
}
 
// задумка была запустить в каждый поток по воркеру, пусть себе бомбит сообщениями по серверу
void TController::Start() {
  std::vector<std::thread> thrdsWorkers;
  for (size_t i = 0; i < m_config.workersCount; ++i) {
    thrdsWorkers.emplace_back(&TController::WorkerDriver, this, i);
  }
  for (std::thread& thrd : thrdsWorkers) {
    thrd.join();
  }
}

// функция управляет объектом воркера
void TController::WorkerDriver(size_t num) const {
  std::ofstream ofs(m_outPath / ("Worker" + std::to_string(num)));
  TConfigWorker config;
  std::srand(std::time(nullptr));
  // группируем сообщения по полученному рандомному числу
  config.countMessagesInGroup = 1; // TODO(VF): хотел сделать, но... не успел
  // пауза между сообщениями 0 - 2 сек с градацией 100мс
  config.pauseBetweenMessages = std::chrono::milliseconds((std::rand() % 20) * 100);

  // shared ptr опять же, т.к. в другому потоку даём возможность вызывать Stop для этого объекта
  auto worker = std::make_shared<TWorker>(config, num);
  size_t indx = common::TStopHandler::Add([worker] { worker->Stop(); });
  std::vector<std::string> answers = worker->Start(m_messages, m_config.serverPort);
  common::TStopHandler::Clear(indx);

  // вывод в файл запрос - ответ
  for (size_t i = 0; i < m_messages.size(); ++i) {
    ofs << "REQUEST:\n" << m_messages[i] << "\nANSWER:\n" << answers[i] <<
      "\n-------------------------------------------------------------------------------\n";
    if (m_stopFlag)
      break;
  }
  ofs.flush();
}

void TController::Stop() {
  MyLog("Client: stop command");
  common::TStopHandler::Clear(m_indxStopHandler);
  m_stopFlag = true;
}

} // namespace client