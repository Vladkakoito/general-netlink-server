#include "Common/Actions/Actions.h"

#include <limits>

namespace common {

namespace actions {

namespace {

// набор функций, реализующих действия

double Plus(double* args, size_t argsCount) {
  double result = 0;
  for (size_t i = 0; i < argsCount; ++i) {
    result += args[i];
  }
  return result;
}

double Minus(double* args, size_t argsCount) {
  return argsCount != 2
          ? std::numeric_limits<double>::infinity()
          : args[0] - args[1];
}

double Multiplication(double* args, size_t argsCount) {
  double result = 1;
  for (size_t i = 0; i < argsCount; ++i) {
    result *= args[i];
  }
  return result;
}

double Divsion(double* args, size_t argsCount) {
  return (argsCount != 2 || args[1] < std::numeric_limits<double>::epsilon())
    ? std::numeric_limits<double>::infinity()
    : args[0] / args[1];
}

} // namespace

// что бы не искать действия по строковым ключам - делаем это один раз в конструкторе TAction
const std::unordered_map<std::string, TAction::EAction> TAction::s_kStringToActions = {
  { "plus", TAction::EAction::PLUS },
  { "minus", TAction::EAction::MINUS },
  { "multi", TAction::EAction::MULTIPLICATION },
  { "div", TAction::EAction::DIVISION }
};

// функции, реализующие действия
const std::function<double(double*, size_t)> TAction::s_kFunctions[] = {
  Plus,
  Minus,
  Multiplication,
  Divsion
};

std::optional<TAction> TAction::Make(const std::string& action) {
  auto it = s_kStringToActions.find(action);
  if (it == s_kStringToActions.end())
    return std::nullopt;
  return TAction(it->second);
}

} // namespace actions

} // namespace common