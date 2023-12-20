#pragma once

#include <functional>
#include <vector>

namespace common {

// в класс передаются функции, которые нужно вызвать при остановке сервера
// в целом это всё, что бы обойти использование глобальных переменных для остановки
class TStopHandler {
public:
  static size_t Add(std::function<void()> func) {
    s_handlers.push_back(func);
    return s_handlers.size() - 1;
  }
  static void Call() {
    for (auto handler : s_handlers) {
      if (handler)
        handler();
    }
  }
  // нельзя вырезать - остальные индексы собьются. поэтому, просто меняем на пустышку
  static void Clear(size_t indx) {
    s_handlers[indx] = std::function<void()>{};
  }
private:
  inline static std::vector<std::function<void()>> s_handlers;
};

} // namespace common