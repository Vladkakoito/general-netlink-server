#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>


#include "Client/Worker.h"
#include "Common/SimpleLogger/SimpleLogger.h"
#include "Common/StopHandler.h"

namespace client {

// по задумке, класс держит пул потоков, которые бомбят по серверу
class TController : public std::enable_shared_from_this<TController> {
public:
  // подсовываем вместо конструктора, который позволит владеть объектом - 
  // умный указатель. что бы объект сам решал, когда ему умереть
  static std::shared_ptr<TController> Make(const TConfig& config);

  // тут создаётся директория вывода (если нету), читаются сообщения
  bool Init();

  // отправка всех сообщений в N потоках с разным таймингом
  void Start();

  // остановить всё это удовольствие
  void Stop();
private:
  // конструктор убран, что бы нечаянно не создать объект, который не владеет сам собой
  TController(const TConfig& config);

  // функция, одна на поток. создаёт воркера
  void WorkerDriver(size_t num) const;


  // настройки из .ini файла
  const TConfig& m_config;

  // задумывался, что бы передать каждому воркеру ссылку на этот флаг. 
  // но в итоге, сделал каждому воркеру свой
  std::atomic<bool> m_stopFlag{ false };

  // сообщениячитаются и разбираются при инициализации
  std::vector<std::string> m_messages;

  // директория для вывода ответов
  std::filesystem::path m_outPath;

  // индекс объекта в списке функций на остановку
  // когда уничтожается объект - стирает себя из этого списка
  size_t m_indxStopHandler{ 0 };
};

inline TController::TController(const TConfig& config)
  : m_config(config) {}

// static
inline std::shared_ptr<TController> TController::Make(const TConfig& config) {
  return std::shared_ptr<TController>(new TController(config));
}

} // client