#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace common {

namespace actions {

// класс реализует одно действие
class TAction {
public:
  // если некорректное строковое действие - вернется nullopt
  static std::optional<TAction> Make(const std::string& action);
  double Calculate(double* args, size_t argsCount) const;

private:
  enum EAction {
    PLUS = 0,
    MINUS,
    MULTIPLICATION,
    DIVISION
  };

  TAction(EAction action) : m_action(action) {}

  static const std::unordered_map<std::string, EAction> s_kStringToActions;
  static const std::function<double(double*, size_t)> s_kFunctions[];

  EAction m_action;
};

// вызов функции, реализующей действие
inline double TAction::Calculate(double* args, size_t argsCount) const {
  return s_kFunctions[m_action](args, argsCount);
}

} // namespace actions

} // namespace common