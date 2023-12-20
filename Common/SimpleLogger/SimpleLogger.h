#pragma once

#include <iostream>
#include <mutex>

namespace common {

namespace logger {

// логгер для вывода под мьютексом. ну заодно еще можно в файл перенаправить
// не хотел еще какую-то бибиотеку подключать.
// а так то еще б добавить уровни логгирования хотя бы для дебага
class TLogger {
public:
  static void SetLogOut(std::ostream& osLog);
  static void SetErrOut(std::ostream& m_osErr);

  template <typename TType>
  static std::ostream& Log(const TType& value);

  template <typename TType>
  static std::ostream& Err(const TType& value);

private:
  // указатели по умолчанию на консоль, но можно установить и на файлы
  inline static std::ostream* m_osLog{ &std::cout };
  inline static std::ostream* m_osErr{ &std::cerr };

  // выводим под мьютексом
  inline static std::mutex m_mtx;
};

//static
template<typename TType>
std::ostream& TLogger::Log(const TType& value) {
  std::lock_guard g(m_mtx);
  *m_osLog << value << std::endl;
  return *m_osLog;
}

//static
template<typename TType>
std::ostream& TLogger::Err(const TType& value) {
  std::lock_guard g(m_mtx);
  *m_osErr << value << std::endl;
  return *m_osErr;
}

// static
inline void TLogger::SetLogOut(std::ostream& osLog) {
  m_osLog = &osLog;
}

// static
inline void TLogger::SetErrOut(std::ostream& osErr) {
  m_osErr = &osErr;
}

} // namespace logger

#define MyLog(text) common::logger::TLogger::Log(text)
#define MyErr(text) common::logger::TLogger::Err(text)

} // namespace common